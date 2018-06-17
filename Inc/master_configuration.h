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
	PULSE_BUFFER_TIME       = 0,
	PULSE_STOP_PERIOD       = 1000,
	PULSE_BASE_TIME         = PULSE_STOP_PERIOD + 
							  PULSE_BUFFER_TIME,
	PULSE_MAX_PERIOD        = 2000,
	PULSE_MIN_PERIOD        = 1100,
	PULSE_LANDING_PERIOD    = 1300
}esc_pulse;

typedef enum {
	IMU_MAX_ANGLE           = 45
}imu_conditions;

typedef struct FaultCheck{
	uint8_t isHeaderValid;
	uint8_t shutdown;
}FaultCheck;
