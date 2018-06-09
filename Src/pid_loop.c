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

#define DEBUGx

extern TIM_HandleTypeDef htim1;
static BNO bno;
PID pid;
Motor motor;

const float PID_P = 1.65;
const float PID_I = 0;
const float PID_D = 24;

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
	HAL_Delay(1000);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,1100);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,1100);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,1100);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,1100);
}

void pid_loop(void){
	//TO DO: receiveData()
	BnoUpdateEuler(&bno);
	
	pid.spRoll  = 6;
	pid.cRoll   = bno.cappedEulerX;
	pid.eRoll   = pid.cRoll - pid.spRoll;
	pid.inteRoll += PID_I*pid.eRoll;
	
	//printf("%d\n",(int)(pid.eRoll-pid.peRoll));
	pid.sumRoll = PID_P*(pid.eRoll) + PID_D*(pid.eRoll-pid.peRoll) + (pid.inteRoll);
	
	if(pid.prevDelay == 3){
		pid.peRoll  = pid.eRoll;
		pid.prevDelay = 0;
	}else{
		pid.prevDelay++;
	}
	
	motor.oFrontLeft  = PULSE_IDLE_PERIOD + pid.sumRoll;
	motor.oFrontRight = PULSE_IDLE_PERIOD - pid.sumRoll;
	
	if(motor.oFrontLeft > PULSE_MAX_PERIOD) 
		motor.oFrontLeft  = PULSE_MAX_PERIOD;
	else if(motor.oFrontLeft < PULSE_MIN_PERIOD)
		motor.oFrontLeft  = PULSE_MIN_PERIOD;
	
	if(motor.oFrontRight > PULSE_MAX_PERIOD)
		motor.oFrontRight = PULSE_MAX_PERIOD;
	else if(motor.oFrontRight < PULSE_MIN_PERIOD)
		motor.oFrontRight = PULSE_MIN_PERIOD;
			
	#ifdef DEBUG
		//printf("Left Motor: %d\t Right Motor: %d \n",(int)motor.oFrontRight,(int)motor.oFrontLeft);
		//printf("P: %d\t D: %d \n\n",(int)(PID_P*(pid.eRoll)),(int)(PID_D*(pid.eRoll-pid.peRoll)));
		printf("%d \n",(int)(pid.eRoll-pid.peRoll));
	#else
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motor.oFrontLeft);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,motor.oFrontRight);
	#endif
}
