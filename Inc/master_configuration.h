/*
 * master_configuration.h
 *
 *  Created on: June 1, 2018
 *      Author: pasindu
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef master_configuration_H_
#define master_configuration_H_
#endif /* master_configuration_H_ */
#define DEBUG_TEST 0

typedef enum {
	PULSE_BUFFER_TIME       = 100,
	PULSE_MAX_PID_TIME      = 250,
	PULSE_STOP_PERIOD       = 1000,
	PULSE_IDLE_PERIOD       = PULSE_STOP_PERIOD + 
							  PULSE_BUFFER_TIME + 
							  PULSE_MAX_PID_TIME,
	PULSE_MAX_PERIOD        = 1700,
	PULSE_MIN_PERIOD        = 1100
}esc_pulse;

typedef enum {
	IMU_MAX_ANGLE           = 45
}imu_conditions;
