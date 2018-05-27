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
typedef struct pid {
	int16_t prop;
	int16_t inte;
	int16_t diff;
	
	
}PID;

void startPWM(void);
void startMotor(void);
void pid_loop(void);
