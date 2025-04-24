#include <stdio.h>
#include <math.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <geometry_msgs/msg/twist.h>

#include "pico/stdlib.h"
#include "tribune_scripts.h"
#include "stepper.h"
#include "servo.h"
#include "SCServo.h"

int open_angles[] = {175, 175, 175, 175};
int close_angles[] = {5, 5, 5, 5};

MoveController::MoveController(rcl_publisher_t *twist_publisher, SMS_STS *sms_sts)
{
    twist_publisher_ptr = twist_publisher;
    sms_sts_ptr = sms_sts;
}

void MoveController::print_banner()
{
    sleep_ms(1000);
    this->set_pitch(0, 100); // waveshare
    sleep_ms(500);
    this->set_visor(100, 100); // waveshare
    sleep_ms(1000);
    this->go_dist(0.1, 0.1, -1); // движение назад
}

void MoveController::build_tribune()
{
    this->go_dist(0.1, 0.1, 1);
    this->lift(5 * STEPPER_6_MM);
    
    int states[] = {1, 0, 0, 1};
    this->set_grippers(states);
    sleep_ms(500);
    this->lift(8 * STEPPER_6_MM);
    this->go_dist(0.1, 0.1, -1);
    sleep_ms(500);
    set_pusher(100, 100);
    sleep_ms(500);
    set_pusher(0, 100);
    sleep_ms(500);
    set_pusher(10, 100);
    sleep_ms(500);
    set_pusher(0, 100);
    this->lift(57 * STEPPER_6_MM);
    this->go_dist(0.1, 0.1, 1);
    this->lift(54 * STEPPER_6_MM);
    states[0] = 0;
    states[3] = 0;
    this->set_grippers(states);
    this->lift(45 * STEPPER_6_MM);
    this->set_visor(20, 100);
    sleep_ms(500);
    this->go_dist(0.1, 0.1, -1);
}

void MoveController::prepare()
{
    position = 0;
    this->set_pusher(40, 100);
    this->set_visor(0, 100);
    stepper_go_home();
    this->position = 0;
    this->set_pitch(95, 100);
    // this->lift(50 * STEPPER_6_MM);
}

void MoveController::collect()
{
    stepper_go_home();
    this->position = 0;

    this->set_visor(100, 100);
    this->set_pusher(0, 100);
    this->set_pitch(0, 100);
    int states0[] = {0, 0, 0, 0};
    this->set_grippers(states0);
    sleep_ms(500);
    sms_sts_ptr->EnableTorque(1, 0);
    this->go_dist(0.15, 0.075, 1);
    sleep_ms(500);
    this->set_visor(0, 100);
    sleep_ms(500);
    this->lift(5 * STEPPER_6_MM);
    int states[] = {1, 1, 1, 1};
    this->set_grippers(states);
    sleep_ms(500);
    this->lift(10 * STEPPER_6_MM);
    this->go_dist(0.15, 0.1, -1);
}

void MoveController::set_waveshare_servo(u8 servo_id, s16 position_percents, u16 speed_percents, s16 start_v, s16 end_v, u8 acc = 50)
{
    s16 servo_value = round((end_v - start_v) * (position_percents / 100.0) + start_v);
    u16 servo_speed = round((4095 * speed_percents) / 100.0);
    sms_sts_ptr->WritePosEx(servo_id, servo_value, servo_speed, acc);
}

/*!
    @brief Для управления держателем досок
    @param value положение, от 0 до 100
    @param speed скорость, от 0 до 100
*/
void MoveController::set_visor(s16 value, u16 speed)
{
    const s16 v1 = 1973; // 0 percents, опущено
    const s16 v2 = 1047; // 100 percents, поднято
    const u8 servo_id = 2;
    this->set_waveshare_servo(servo_id, value, speed, v1, v2);
}

/*!
    @brief Для управления толкателем
    @param value положение, от 0 до 100
    @param speed скорость, от 0 до 100
*/
void MoveController::set_pusher(s16 value, u16 speed)
{
    const s16 v1 = 2047; // 0 percents, задвинут
    const s16 v2 = 1081; // 100 percents, выдвинут
    const u8 servo_id = 3;
    this->set_waveshare_servo(servo_id, value, speed, v1, v2);
}

/*!
    @brief Для управления наклоном подъёмника
    @param value положение, от 0 до 100
    @param speed скорость, от 0 до 100
*/
void MoveController::set_pitch(s16 value, u16 speed)
{
    const s16 v1 = 1600; // 0 percents, опущено
    const s16 v2 = 2500; // 100 percents, поднято
    const u8 servo_id = 1;
    this->set_waveshare_servo(servo_id, value, speed, v1, v2);
}

/*!
    @brief Движение вперёд в течение time_ms в направлении direction
    @param time_ms время движения, мс
    @param direction направление, 1 - вперёд, -1 - назад
*/
void MoveController::go_dist(float dist, float vel_mps, int direction)
{
    rcl_ret_t ret;
    twist_msg.linear.x = vel_mps * direction;
    int time_ms = (dist / vel_mps) * 1000;
    twist_msg.linear.y = 0;
    twist_msg.angular.z = 0;
    ret = rcl_publish(twist_publisher_ptr, &twist_msg, NULL);
    sleep_ms(time_ms);
    twist_msg.linear.x = 0;
    twist_msg.linear.y = 0;
    twist_msg.angular.z = 0;
    ret = rcl_publish(twist_publisher_ptr, &twist_msg, NULL);
}

void MoveController::set_servo(int angles[4])
{
    for (int i = 0; i < 4; i++)
    {
        servo(i + 1, angles[i]);
    }
}

void MoveController::set_grippers(int states[4])
{
    for (int i = 0; i < 4; i++)
    {
        int angle = 0;
        if (states[i] == 1)
        {
            angle = close_angles[i];
        }
        else
        {
            angle = open_angles[i];
        }
        servo(i + 1, angle);
    }
}

/*!
    @brief Поднимает подъёмник шаговиками на уровень new_pos
    @param new_pos новый уровень, от 0
*/
void MoveController::lift(int new_pos)
{
    int delta_pos = new_pos - position;
    if (delta_pos > 0)
    {
        stepper(1, delta_pos);
    }
    if (delta_pos < 0)
    {
        stepper(0, -delta_pos);
    }
    position = new_pos;
}