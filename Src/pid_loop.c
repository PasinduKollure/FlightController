/*
 * pid_loop.c
 *
 *  Created on: May 21, 2018
 *      Author: Pasindu
 */

#include "stm32f4xx_hal.h"
#include "BNO055.h"
#include "pid_loop.h"

extern TIM_HandleTypeDef htim1;

void startPWM(void){
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
}

void pid_loop(void){
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,500);
	HAL_Delay(1000);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,250);
	HAL_Delay(1000);
}
