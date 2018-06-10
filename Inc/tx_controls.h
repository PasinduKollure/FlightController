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
#define WORD_COUNT (BYTE_COUNT/2)
#define TURNIGY_HEADER 16416
#define BYTE2WORD(MSB,LSB) ((MSB<<8) | LSB)

typedef struct TurnigyData {
	uint8_t rawData[BYTE_COUNT];
	uint16_t ctrlData[WORD_COUNT];
} TxCtrlData;
