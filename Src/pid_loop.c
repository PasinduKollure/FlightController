/*
 * pid_loop.c
 *
 *  Created on: May 21, 2018
 *      Author: Pasindu
 */

#include "stm32f4xx_hal.h"
#include "BNO055.h"
#include "pid_loop.h"
#include "tx_controls.h"
#include "master_configuration.h"

#define ROLLx

extern TIM_HandleTypeDef htim1;
extern TxCtrlData turnigy;
static BNO bno;
PID pid;
Motor motor;

const float PID_P = 1.65f;
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
	BnoUpdateEuler(&bno);
	
	/* ROLL */
	#ifdef ROLL
		pid.spRoll  = 0;
		pid.cRoll   = bno.cappedEulerX;
		pid.eRoll   = pid.cRoll - pid.spRoll;
		pid.inteRoll = pid.accRoll + PID_I*pid.eRoll;
		
		pid.sumRoll = PID_P*(pid.eRoll) + PID_D*(pid.eRoll-pid.peRoll) + (pid.inteRoll);
		pid.accRoll = pid.inteRoll;
	#else 
		/* PITCH */
		pid.spPitch  = 0;
		pid.cPitch   = bno.cappedEulerY;
		pid.ePitch =  pid.cPitch - pid.spPitch;
		pid.intePitch = pid.accPitch + PID_I*pid.ePitch;
		
		pid.sumPitch = PID_P*(pid.ePitch) + PID_D*(pid.ePitch-pid.pePitch) + (pid.intePitch);
		pid.accPitch = pid.intePitch;
	#endif
	
	
	if(pid.prevDelay == 3){
		pid.peRoll   = pid.eRoll;
		pid.pePitch  = pid.ePitch;
		pid.prevDelay = 0;
	}else{
		pid.prevDelay++;
	}
	
	#ifdef ROLL
		/*ROLL NOTE: Change motor.MOTOR# */
		motor.oFrontLeft  = PULSE_IDLE_PERIOD + pid.sumRoll;
		motor.oFrontRight = PULSE_IDLE_PERIOD - pid.sumRoll;
	#else
		/*PITCH, NOTE: Change motor.MOTOR# */
		motor.oFrontLeft  = PULSE_IDLE_PERIOD + pid.sumPitch;
		motor.oFrontRight = PULSE_IDLE_PERIOD - pid.sumPitch;
	#endif
	
	#ifdef ROLL
		/*ROLL, NOTE: Change motor.MOTOR# */
		if(motor.oFrontLeft > PULSE_MAX_PERIOD) 
			motor.oFrontLeft  = PULSE_MAX_PERIOD;
		else if(motor.oFrontLeft < PULSE_MIN_PERIOD)
			motor.oFrontLeft  = PULSE_MIN_PERIOD;
		
		if(motor.oFrontRight > PULSE_MAX_PERIOD)
			motor.oFrontRight = PULSE_MAX_PERIOD;
		else if(motor.oFrontRight < PULSE_MIN_PERIOD)
			motor.oFrontRight = PULSE_MIN_PERIOD;
	#else
		//PITCH, NOTE: Change motor.MOTOR#
		if(motor.oFrontLeft > PULSE_MAX_PERIOD) 
			motor.oFrontLeft  = PULSE_MAX_PERIOD;
		else if(motor.oFrontLeft < PULSE_MIN_PERIOD)
			motor.oFrontLeft  = PULSE_MIN_PERIOD;
		
		if(motor.oFrontRight > PULSE_MAX_PERIOD)
			motor.oFrontRight = PULSE_MAX_PERIOD;
		else if(motor.oFrontRight < PULSE_MIN_PERIOD)
			motor.oFrontRight = PULSE_MIN_PERIOD;
	#endif
			
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motor.oFrontLeft);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,motor.oFrontRight);

}
