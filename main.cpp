#include <stdio.h>
#include <math.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/u_int8.h>
#include <std_msgs/msg/int16.h>
#include <geometry_msgs/msg/twist.h>
#include <rmw_microros/rmw_microros.h>

#include "pico/stdlib.h"
#include "pico_uart_transports.h"
#include "servo.h"
#include "stepper.h"
#include "tribune_scripts.h"
#include "tft.h"

#define LED_PIN 25
#define WAVESHARE_TX 8
#define WAVESHARE_RX 9
#define START_PIN 21

#define PUBLISH_DELAY_MS 33

const char screen_topic[] = "/screen";
const char request_topic[] = "/elevator/request";
const char answer_topic[] = "/elevator/answer";
const char node_name[] = "grip_pico_node";

HardwareSerial hardware_serial(uart1, 1000000, WAVESHARE_TX, WAVESHARE_RX);
SMS_STS sms_sts;

rcl_publisher_t elevator_answer_publisher;
std_msgs__msg__UInt8 elevator_answer_msg;

rcl_subscription_t elevator_request_subscriber;
std_msgs__msg__UInt8 elevator_request_msg;

rcl_subscription_t screen_subscriber;
std_msgs__msg__Int16  screen_msg;

rcl_publisher_t twist_publisher;

rcl_publisher_t start_publisher;
std_msgs__msg__UInt8 start_msg;


MoveController move_controller(&twist_publisher, &sms_sts);


void elevator_request_subscriber_callback(const void *msgin) {
    uint8_t command = ((const std_msgs__msg__UInt8 *)msgin)->data;
    if (command == 40) {
        // все сервы в нулевое положение
        move_controller.set_pusher(0, 100);
        move_controller.set_visor(0, 100);
        move_controller.set_pitch(0, 100);
    }
    if (command == 41) {
        // выдвинуть толкатель на максимум
        move_controller.set_pusher(100, 100);
    }
    else if (command == 42) {
        // держалка для досок
        move_controller.set_visor(100, 100);
    }
    else if (command == 43) {
        int angles_53[4] = { 1, 0, 0, 1 };
        // быть крайне аккуратным с этим, возможно не до конца откалибрована
        move_controller.set_pitch(100, 100);
    }
    else if (command == 51) {
        // общий тест захватов
        int angles_51[4] = { 175, 175, 175, 175 };
        move_controller.set_servo(angles_51);
    }
    else if (command == 52) {
        // общий тест захватов
        int angles_51[4] = { 5, 5, 5, 5 };
        move_controller.set_servo(angles_51);
    }
    else if (command == 53) {
        // тест серв на захват и соответсвие баковым и внутрим
        int angles_53[4] = { 1, 0, 0, 1 };
        move_controller.set_grippers(angles_53);
    }
    else if (command == 60) {
        // шаговики в начальное положение
        move_controller.lift(0);
    }
    else if (command == 61) {
        // шаговики в положение STEPPER_6_MM
        move_controller.lift(STEPPER_6_MM);
    }
    else if (command == 62) {
        // максимальый подъём шаговиков
        move_controller.lift(34 * STEPPER_6_MM);
    }
    else if (command == 0) {
        // начальное положение
        move_controller.prepare();
    }
    else if (command == 1) {
        // захват банок
        move_controller.collect();
    }
    else if (command == 2) {
        // построить двухэтажную трибуну
        move_controller.build_tribune();
    }
    else if (command == 3) {
        // вывесить баннер
        move_controller.print_banner();
    }

    elevator_answer_msg.data = command;
    // sms_sts.WritePosEx(3, 1500, 1000, 50);

    rcl_ret_t ret = rcl_publish(&elevator_answer_publisher, &elevator_answer_msg, NULL);
}

void screen_subscriber_callback(const void *msgin) {
    int16_t command = ((const std_msgs__msg__Int16 *)msgin)->data;
    Print_Number(command);
}

uint8_t start() {
    if (gpio_get(START_PIN)) {
        return 1;
    }
    return 0;
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
    rcl_ret_t ret;
    start_msg.data = start();
    ret = rcl_publish(&start_publisher, &start_msg, NULL);
}

int main()
{
    stdio_init_all();
    stepper_init();
    servo_init();
    Setup();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Инициализируем пин старта
    gpio_init(START_PIN);
    gpio_set_dir(START_PIN, GPIO_IN);

    rmw_uros_set_custom_transport(
        true,
        NULL,
        pico_serial_transport_open,
        pico_serial_transport_close,
        pico_serial_transport_write,
        pico_serial_transport_read
    );

    sms_sts.pSerial = &hardware_serial;
    display_print_text("Start!");
    sleep_ms(1000);

    rcl_timer_t timer;
    rcl_node_t node;
    rcl_allocator_t allocator;
    rclc_support_t support;
    rclc_executor_t executor;

    allocator = rcl_get_default_allocator();

    // Wait for agent successful ping for 2 minutes.
    const int timeout_ms = 1000;
    const uint8_t attempts = 120;

    rcl_ret_t ret = rmw_uros_ping_agent(timeout_ms, attempts);

    if (ret != RCL_RET_OK) {
        // Unreachable agent, exiting program.
        return ret;
    }
    sleep_ms(3000);
    rclc_support_init(&support, 0, NULL, &allocator);

    rclc_node_init_default(&node, node_name, "", &support);

    rclc_subscription_init_default(
        &screen_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int16),
        screen_topic);
    rclc_subscription_init_default(
        &elevator_request_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
        request_topic);
    rclc_publisher_init_default(
        &elevator_answer_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
        answer_topic);
    rclc_publisher_init_default(
        &twist_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel");
    rclc_publisher_init_default(
        &start_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
        "timer");
    rclc_timer_init_default2(
        &timer,
        &support,
        RCL_MS_TO_NS(PUBLISH_DELAY_MS),
        timer_callback,
        true);

    rclc_executor_init(
        &executor,
        &support.context,
        4,
        &allocator);

    rclc_executor_add_subscription(
        &executor,
        &elevator_request_subscriber,
        &elevator_request_msg,
        &elevator_request_subscriber_callback,
        ON_NEW_DATA);

    rclc_executor_add_subscription(
        &executor,
        &screen_subscriber,
        &screen_msg,
        &screen_subscriber_callback,
        ON_NEW_DATA);

    rclc_executor_add_timer(
        &executor,
        &timer);

    gpio_put(LED_PIN, 1);
    display_print_text("loop..");

    while (true) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }
    return 0;
}
