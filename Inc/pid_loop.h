/*
 * pid_loop.h
 *
 *  Created on: May 21, 2018
 *      Author: pasindu
 */

#ifndef pid_loop_H_
#define pid_loop_H_
#endif /* pid_loop_H_ */

#include "main.h"


typedef struct PID {
	//Unit: degrees
	int16_t throttle;
   	int16_t rollSetPoint;
	int16_t pitchSetPoint;
	int16_t yawSetPoint;
	
	//Unit: degrees
	float rollError;
	float pitchError;
	float yawError;
	
	//Unit: degrees
	float prevRollError;
	float prevPitchError;
	float prevYawError;
	
	//Unit: degrees
	float integratedRollError;
	float integratedPitchError;
	float integratedYawError;
	
	//Unit: degrees
	float rollSum;
	float pitchSum;
	float yawSum;
} PID;

typedef struct Motor {
	float oFrontLeft;
	float oFrontRight;
	float oRearLeft;
	float oRearRight;
} MotorPWM;

void startPWM(void);
void startMotor(void);
void pid_loop(void);
static void pidCalculation(uint16_t, uint16_t, uint16_t);
static void convertSetPoints(void);
inline static void motorLimiter(MotorPWM*);
void printPostPidPeriods(MotorPWM);
