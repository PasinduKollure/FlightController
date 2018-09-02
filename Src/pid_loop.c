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

extern TIM_HandleTypeDef htim1;
extern TxCtrlData turnigy;
extern FaultCheck fault;
static EulerAngles bno;
static PID pid;
static int prevDelay;
MotorPWM motor;

const float PID_P = 2.8;
const float PID_I = 0.00;
const float PID_D = 26;

void startPWM(void) {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
}

void startMotor(void) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PULSE_STOP_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PULSE_STOP_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PULSE_STOP_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PULSE_STOP_PERIOD);
	HAL_Delay(1000);
}

void pid_loop(void){
	updateEulerAngles(&bno);
	convertSetPoints();
	pidCalculation(pid.pitchSetPoint, pid.rollSetPoint, pid.yawSetPoint);

	if((!fault.isHeaderValid) || fault.shutdown) {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PULSE_STOP_PERIOD);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PULSE_STOP_PERIOD);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PULSE_STOP_PERIOD);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PULSE_STOP_PERIOD);
	}
	else {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motor.oFrontLeft);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motor.oFrontRight);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, motor.oRearLeft);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, motor.oRearRight);
	}
}

static void pidCalculation(uint16_t pitchSetPoint, uint16_t rollSetPoint, uint16_t yawSetPoint) {
	// ROLL
	pid.rollSetPoint = rollSetPoint;
	pid.rollError = pid.rollSetPoint - bno.eulerX;
	pid.rollSum = PID_P * (pid.rollError) + 
				  PID_D * (pid.rollError - pid.prevRollError) + 
			      (pid.integratedRollError + PID_I * pid.rollError);
	pid.integratedRollError += PID_I * pid.rollError;

	// PITCH
	pid.pitchSetPoint = pitchSetPoint;
	pid.pitchError = pid.pitchSetPoint - bno.eulerY;
	pid.pitchSum = PID_P * (pid.pitchError) + 
				   PID_D * (pid.pitchError - pid.prevPitchError) + 
				   (pid.integratedPitchError + PID_I * pid.pitchError);
	pid.integratedPitchError += PID_I * pid.pitchError;
	
	// Previous Error recorded with a delay.
	// Difference was too low since sampling
	// frequency was too high.
	if(prevDelay == 3) {
		pid.prevRollError = pid.rollError;
		pid.prevPitchError = pid.pitchError;
		prevDelay = 0;
	}
	else {
		prevDelay++;
	}
	
	motor.oFrontLeft  = pid.throttle - pid.pitchSum + pid.rollSum + pid.yawSum;
	motor.oFrontRight = pid.throttle - pid.pitchSum - pid.rollSum - pid.yawSum;
	motor.oRearLeft   = pid.throttle + pid.pitchSum + pid.rollSum - pid.yawSum;
	motor.oRearRight  = pid.throttle + pid.pitchSum - pid.rollSum + pid.yawSum;

	motorLimiter(&motor);
}

static void convertSetPoints(void) {
	if(turnigy.ctrlData[HEADER_INDEX] == iBUS_HEADER){
		fault.isHeaderValid = 1;
		//setPoint (angle) = MAX_ANGLE*((ctrl - 1500) / 500)
		pid.pitchSetPoint = MAX_PITCH_ANGLE * ((turnigy.ctrlData[PITCH_INDEX] - REST_PERIOD) / MAX_PERIOD_DEVIATION);
		pid.rollSetPoint = MAX_ROLL_ANGLE * ((turnigy.ctrlData[ROLL_INDEX] - REST_PERIOD) / MAX_PERIOD_DEVIATION);
		pid.yawSetPoint = MAX_YAW_ANGLE * ((turnigy.ctrlData[YAW_INDEX] - REST_PERIOD) / MAX_PERIOD_DEVIATION);
		pid.throttle = turnigy.ctrlData[THROTTLE_INDEX];
		
		if( (pid.rollSetPoint > -1 * REST_ANGLE) && (pid.rollSetPoint < REST_ANGLE) )
			pid.rollSetPoint = 0;

		if( (pid.pitchSetPoint > -1 * REST_ANGLE) && (pid.pitchSetPoint < REST_ANGLE) )
			pid.pitchSetPoint = 0;

		if( (pid.yawSetPoint > -1 * REST_ANGLE) && (pid.yawSetPoint < REST_ANGLE) )
			pid.yawSetPoint = 0;

		if(pid.throttle > MAX_THROTTLE) 
			pid.throttle = MAX_THROTTLE;
		else if(pid.throttle < PULSE_MIN_PERIOD)
			pid.throttle = PULSE_MIN_PERIOD;
	}
	else {
		fault.isHeaderValid = 0;
		pid.pitchSetPoint = 0;
		pid.rollSetPoint = 0;
		pid.yawSetPoint = 0;
		pid.throttle = PULSE_LANDING_PERIOD;
	}
}

inline static void motorLimiter(MotorPWM *motor) {
	if(motor->oFrontLeft > PULSE_MAX_PERIOD)
		motor->oFrontLeft = PULSE_MAX_PERIOD;
	else if(motor->oFrontLeft < PULSE_MIN_PERIOD)
		motor->oFrontLeft = PULSE_MIN_PERIOD;
	
	if(motor->oFrontRight > PULSE_MAX_PERIOD)
		motor->oFrontRight = PULSE_MAX_PERIOD;
	else if(motor->oFrontRight < PULSE_MIN_PERIOD)
		motor->oFrontRight = PULSE_MIN_PERIOD;
	
	if(motor->oRearLeft > PULSE_MAX_PERIOD)
		motor->oRearLeft = PULSE_MAX_PERIOD;
	else if(motor->oRearLeft < PULSE_MIN_PERIOD)
		motor->oRearLeft = PULSE_MIN_PERIOD;
	
	if(motor->oRearRight > PULSE_MAX_PERIOD)
		motor->oRearRight = PULSE_MAX_PERIOD;
	else if(motor->oRearRight < PULSE_MIN_PERIOD)
		motor->oRearRight = PULSE_MIN_PERIOD;
}

void printPostPidPeriods(MotorPWM m) {
	printf("CH1: %d\tCH2: %d\tCH3: %d\tCH4: %d\t\n",
			(int)m.oFrontLeft,
			(int)m.oFrontRight,
			(int)m.oRearLeft,
			(int)m.oRearRight);
}
