/*
 * pid_loop.h
 *
 *  Created on: May 21, 2018
 *      Author: pasindu
 */

/* Define to prevent recursive inclusion -----------------------------*/
#ifndef pid_loop_H_
#define pid_loop_H_
#endif /* pid_loop_H_ */

/* Includes ----------------------------------------------------------*/
#include "main.h"

/* Struct ------------------------------------------------------------*/

typedef struct receiverData {
	//rx: raw receiver data
	float rxRoll;
	float rxPitch;
	float rxYaw;
	
	//rxp: receiver data to period
	float rxpRoll;
	float rxpPitch;
	float rxpYaw;
} RxD;

typedef struct PID {
	//  sp: setpoint
	//Unit: rx
	float spRoll;
	float spPitch;
	float spYaw;
	
	//   c: current; 
	//Unit: degrees
	float cRoll;
	float cPitch;
	float cYaw;
	
	//   e: error (sp-c)
	//Unit: degrees
	float eRoll;
	float ePitch;
	float eYaw;
	
	//  pe: previous error
	//Unit: degrees
	float peRoll;
	float pePitch;
	float peYaw;
	
	float inteRoll;
	float intePitch;
	float inteYaw;
	
	//Unit: degrees
	float sumRoll;
	float sumPitch;
	float sumYaw;
	
	//Unit: time
	float tRoll;
	float tPitch;
	float tYaw;
	
}PID;

typedef struct Motor {
	//Final output periods
	float oFrontLeft;
	float oFrontRight;
	float oRearLeft;
	float oRearRight;
}Motor;

void startPWM(void);
void startMotor(void);
void pid_loop(void);
