/*
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
*/

/* Includes ----------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32_peripherals.h"
#include "BNO055.h"
#include "BNO055_definitions.h"
#include "pid_loop.h"
#include "tx_controls.h"

extern UART_HandleTypeDef huart3;
extern TxCtrlData turnigy;
                                
int main(void)
{
	HAL_Init();
	SystemClock_Config();

	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART3_UART_Init();
	HAL_UART_Receive_DMA(&huart3, (uint8_t*)turnigy.rawData, BYTE_COUNT);
	MX_TIM1_Init();
	MX_I2C1_Init();
	MX_USART2_UART_Init();
	
	
	BNO_I2C_Configure();
	startPWM();
	startMotor();

	while (1){
		pid_loop();
	}
}
