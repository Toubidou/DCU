#ifndef _DEV_MOTOR_H_
#define _DEV_MOTOR_H_

#include "tim.h"
#include "main.h"

enum
{
    MOTOR_A = 1,
    MOTOR_B = 2,
};
enum
{
    MOTOR_STOP = 0,
    MOTOR_FORWARD = 1,
    MOTOR_BACKWARD = 2,
};

void dev_motor_init(void);

#endif // _DEV_MOTOR_H_