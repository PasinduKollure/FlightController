/*
 * master_configuration.h
 *
 *  Created on: June 1, 2018
 *      Author: pasindu
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef master_configuration_H_
#define master_configuration_H_
#endif /* master_configuration_H_ */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

typedef enum {
	PID_P                   = 5,
	PID_I                   = 0,
	PID_D                   = 0
}pid_constants;

typedef enum {
	PULSE_BUFFER_TIME       = 100,
	PULSE_MAX_PID_TIME      = 100,
	PULSE_STOP_PERIOD       = 1000,
	PULSE_MAX_PERIOD        = 1500,
	PULSE_IDLE_PERIOD       = PULSE_STOP_PERIOD + 
							  PULSE_BUFFER_TIME + 
							  PULSE_MAX_PID_TIME
}esc_pulse;

typedef enum {
	IMU_MAX_ANGLE           = 45
}imu_conditions;
