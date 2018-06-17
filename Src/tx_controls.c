/*
 * tx_decoder.c
 *
 *  Created on: June 9, 2018
 *      Author: Pasindu
 */

#include "stm32f4xx_hal.h"
#include "tx_controls.h"
#include "master_configuration.h"

TxCtrlData turnigy;
FaultCheck fault;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	/*
	ctrlData[0]: Header
	ctrlData[1]: Yaw
	ctrlData[2]: Pitch
	ctrlData[3]: Throttle
	ctrlData[4]: Roll
	*/
	
	UNUSED(huart);
	
	for(int i=0; i<WORD_COUNT; i++)
		turnigy.ctrlData[i] = turnigy.rawData[i*2+1] << 8 | turnigy.rawData[i*2];
		
	if(turnigy.ctrlData[3] < PULSE_MIN_PERIOD){
		fault.shutdown = 1;
	}else{
		fault.shutdown = 0;
	}
}
