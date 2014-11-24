#ifndef MOTOR_H
#define MOTOR_H

#include "general_types.h"

//Position 1 = 0b101
#define MOTOR_POS1          0x5 
//Position 2 = 0b100
#define MOTOR_POS2          0x4 
//Position 3 = 0b110
#define MOTOR_POS3          0x6 
//Position 4 = 0b010
#define MOTOR_POS4          0x2 
//Position 5 = 0b011
#define MOTOR_POS5          0x3
//Position 6 = 0b001
#define MOTOR_POS6          0x1

#define MOTOR_SECTOR_LEN    1.0/6.0

#define MOTOR_SPEED_ACCUR   0.05
#define MOTOR_SPEED_STEP    0.000001
#define MOTOR_THROTTLE_MAX     1

#define MOTOR_RPM_AV_NUM    100


int motor_calibrate(void);

void motor_sensor_callback(uint8_t ABC_hall_data);

int ex_motor_script(void);

int set_motor_throttle(double throttle);
uint16_t get_motor_speed_rpm(void);

void full_stop(void);

#endif
