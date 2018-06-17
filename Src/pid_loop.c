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

void startMotor(void){
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
	pidGetCtrlData();
		
	if((!fault.isHeaderValid) || fault.shutdown){
		//turnigy.mainCtrlData[3] = PULSE_LANDING_PERIOD;
		//pidCalculation(0,0,0);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,PULSE_STOP_PERIOD);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,PULSE_STOP_PERIOD);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,PULSE_STOP_PERIOD);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,PULSE_STOP_PERIOD);
		
	}else{
		pidCalculation(pid.spPitch,pid.spRoll,pid.spYaw);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motor.oFrontLeft);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,motor.oRearLeft);
	}
}

static void pidCalculation(uint16_t pitchSetPoint, uint16_t rollSetPoint, uint16_t yawSetPoint){
	/* ROLL */
	pid.spRoll   = rollSetPoint;
	pid.cRoll    = bno.eulerX;
	pid.eRoll    = pid.spRoll - pid.cRoll;
	pid.inteRoll = pid.accRoll + PID_I*pid.eRoll;
	pid.sumRoll  = PID_P*(pid.eRoll) + PID_D*(pid.eRoll-pid.peRoll) + (pid.inteRoll);
	pid.accRoll  = pid.inteRoll;
	
	/* PITCH */
	pid.spPitch   = pitchSetPoint;
	pid.cPitch    = bno.eulerY;
	pid.ePitch    = pid.spPitch - pid.cPitch;
	pid.intePitch = pid.accPitch + PID_I*pid.ePitch;
	pid.sumPitch  = PID_P*(pid.ePitch) + PID_D*(pid.ePitch-pid.pePitch) + (pid.intePitch);
	pid.accPitch  = pid.intePitch;
	
	/* Previous Error */
	if(pid.prevDelay == 3){
		pid.peRoll    = pid.eRoll;
		pid.pePitch   = pid.ePitch;
		pid.prevDelay = 0;
	}else{
		pid.prevDelay++;
	}
	
	motor.oFrontLeft  = PULSE_BASE_TIME + pid.throttle - pid.sumPitch + pid.sumRoll + pid.sumYaw;
	motor.oFrontRight = PULSE_BASE_TIME + pid.throttle - pid.sumPitch - pid.sumRoll - pid.sumYaw;
	motor.oRearLeft   = PULSE_BASE_TIME + pid.throttle + pid.sumPitch + pid.sumRoll - pid.sumYaw;
	motor.oRearRight  = PULSE_BASE_TIME + pid.throttle + pid.sumPitch - pid.sumRoll + pid.sumYaw;

	motorLimiter(&motor);
}

static void pidGetCtrlData(void){
	if(turnigy.ctrlData[0] == iBUS_HEADER){
		fault.isHeaderValid = 1;
		pid.spPitch  = ((30)*(turnigy.ctrlData[2]-1500))/500;
		pid.spRoll   = ((30)*(turnigy.ctrlData[4]-1500))/500;
		pid.spYaw    = ((30)*(turnigy.ctrlData[1]-1500))/500;
		pid.throttle = (turnigy.ctrlData[3]-1000);
		if(pid.throttle > 700) pid.throttle = 700;
	}else{
		fault.isHeaderValid = 0;
		pid.spPitch  = 0;
		pid.spRoll   = 0;
		pid.spYaw    = 0;
		pid.throttle = PULSE_LANDING_PERIOD;
	}
}

static void shutdownProcedure(void){
	//place setpoint for roll, pitch, yaw to 0 so drone is stationary
	//then reduce PWM to 1300 to allow drone to land gracefully
	
}

inline static void motorLimiter(Motor *motor){
	if(motor->oFrontLeft > PULSE_MAX_PERIOD)       motor->oFrontLeft  = PULSE_MAX_PERIOD;
	else if(motor->oFrontLeft < PULSE_MIN_PERIOD)  motor->oFrontLeft  = PULSE_MIN_PERIOD;
	
	if(motor->oFrontRight > PULSE_MAX_PERIOD)      motor->oFrontRight = PULSE_MAX_PERIOD;
	else if(motor->oFrontRight < PULSE_MIN_PERIOD) motor->oFrontRight = PULSE_MIN_PERIOD;
	
	if(motor->oRearLeft > PULSE_MAX_PERIOD)		   motor->oRearLeft  = PULSE_MAX_PERIOD;
	else if(motor->oRearLeft < PULSE_MIN_PERIOD)   motor->oRearLeft = PULSE_MIN_PERIOD;
	
	if(motor->oRearRight > PULSE_MAX_PERIOD)	   motor->oRearRight  = PULSE_MAX_PERIOD;
	else if(motor->oRearRight < PULSE_MIN_PERIOD)  motor->oRearRight = PULSE_MIN_PERIOD;
}
