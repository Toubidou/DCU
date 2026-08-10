#include "dev_motor.h"

/// @brief 
/// @param dir  0: stop, 1: forward, 2: backward 
/// @param motor MOTOR_A or MOTOR_B
void dev_motor_dir_set(uint8_t dir, uint8_t motor)
{
    GPIO_TypeDef *GPIOx_IN1, *GPIOx_IN2;
    uint16_t GPIO_Pin_IN1;
    uint16_t GPIO_Pin_IN2;
    if (motor == MOTOR_A)
    {
        GPIOx_IN1 = MOTORA_IN1_GPIO_Port;
        GPIOx_IN2 = MOTORA_IN2_GPIO_Port;
        GPIO_Pin_IN1 = MOTORA_IN1_Pin;
        GPIO_Pin_IN2 = MOTORA_IN2_Pin;
    }
    else if (motor == MOTOR_B)
    {
        GPIOx_IN1 = MOTORB_IN1_GPIO_Port;
        GPIOx_IN2 = MOTORB_IN2_GPIO_Port;
        GPIO_Pin_IN1 = MOTORB_IN1_Pin;
        GPIO_Pin_IN2 = MOTORB_IN2_Pin;
    }
    
    if (dir == 0)
    {
        HAL_GPIO_WritePin(GPIOx_IN1, GPIO_Pin_IN1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOx_IN2, GPIO_Pin_IN2, GPIO_PIN_SET);
    }
    else if (dir == 1)
    {
        HAL_GPIO_WritePin(GPIOx_IN1, GPIO_Pin_IN1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOx_IN2, GPIO_Pin_IN2, GPIO_PIN_RESET);
    }
    else if (dir == 2)
    {
        HAL_GPIO_WritePin(GPIOx_IN1, GPIO_Pin_IN1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOx_IN2, GPIO_Pin_IN2, GPIO_PIN_SET);
    }
}

void dev_motor_init(void)
{
    dev_motor_dir_set(MOTOR_FORWARD, MOTOR_A);
    dev_motor_dir_set(MOTOR_FORWARD, MOTOR_B);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 8000);
    // __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 8000);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
}