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

extern TIM_HandleTypeDef htim1;

int main(void)
{
	HAL_Init();

	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART2_UART_Init();
	MX_TIM1_Init();
	BNO_I2C_Configure();
	startPWM();
		
	while (1){
		BnoUpdateEuler();
		BnoPrintEuler();
		pid_loop();
	}
}

