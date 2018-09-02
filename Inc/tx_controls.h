/*
 * tx_controls.h
 *
 *  Created on: June 9, 2018
 *      Author: pasindu
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef tx_controls_H_
#define tx_controls_H_
#endif /* tx_decoder_H_ */

#define BYTE_COUNT 32
#define WORD_COUNT (BYTE_COUNT / 2)
#define BYTE_TO_WORD(MSB,LSB) ((MSB << 8) | LSB)
#define iBUS_HEADER 16416

#define HEADER_INDEX 0
#define YAW_INDEX 1
#define PITCH_INDEX 2
#define THROTTLE_INDEX 3
#define ROLL_INDEX 4

typedef struct TurnigyData {
	uint8_t rawData[BYTE_COUNT];
	uint16_t ctrlData[WORD_COUNT];
} TxCtrlData;

void printControlData(void);
