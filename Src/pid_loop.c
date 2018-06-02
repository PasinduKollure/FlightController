/*
 * pid_loop.c
 *
 *  Created on: May 21, 2018
 *      Author: Pasindu
 */

#include "stm32f4xx_hal.h"
#include "BNO055.h"
#include "pid_loop.h"
#include "master_configuration.h"

extern TIM_HandleTypeDef htim1;
BNO* angles;
PID pid;
Motor motor;

void startPWM(void){
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
}

void startMotor(){
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,PULSE_STOP_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,PULSE_STOP_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,PULSE_STOP_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,PULSE_STOP_PERIOD);
}

void anglesToTime(BNO* angles){
	
}

void pid_loop(void){
	//TO DO: receiveData()
	BnoUpdateEuler(angles);
	
	pid.spRoll  = 0;
	pid.cRoll   = angles->cappedEulerX;
	pid.eRoll   = pid.cRoll - pid.spRoll;
	pid.sumRoll = PID_P*pid.eRoll + PID_D*pid.eRoll;
	
	motor.oFrontLeft  = PULSE_IDLE_PERIOD + pid.sumRoll;
	motor.oFrontRight = PULSE_IDLE_PERIOD - pid.sumRoll;
	
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motor.oFrontLeft);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motor.oFrontRight);
}
