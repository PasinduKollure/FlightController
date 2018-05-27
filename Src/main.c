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
//a
/* Variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;
extern ADC_HandleTypeDef hadc1;

int main(void)
{
	HAL_Init();

	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART2_UART_Init();
	MX_ADC1_Init();
	MX_TIM1_Init();
	BNO_I2C_Configure();
	
	while (1){
		BnoUpdateEuler();
		BnoPrintEuler();
		pid_loop();
		HAL_Delay(150);
	}
}

