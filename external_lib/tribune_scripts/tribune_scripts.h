#ifndef _TRIBUNE_SCRIPTS_H
#define _TRIBUNE_SCRIPTS_H

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <geometry_msgs/msg/twist.h>
#include "SCServo.h"
#include "INST.h"

#define STEPPER_6_MM 1000

class MoveController {
public:
    MoveController(rcl_publisher_t *twist_publisher, SMS_STS *sms_sts);

    void prepare();
    void collect();
    void build_tribune();
    void print_banner();

    void set_servo(int angles[4]);
    void set_grippers(int states[4]);
    void lift(int position);
    void go_dist(float dist, float vel_mps, int direction);
    void set_pitch(s16 value, u16 speed);
    void set_pusher(s16 value, u16 speed);
    void set_visor(s16 value, u16 speed);
    void set_waveshare_servo(u8 servo_id, s16 position_percents, u16 speed_percents, s16 start_v, s16 end_v, u8 acc);
    void set_position(int position);
private:
    SMS_STS *sms_sts_ptr;
    rcl_publisher_t *twist_publisher_ptr;
    geometry_msgs__msg__Twist twist_msg;
    int position = 0;
};

#endif