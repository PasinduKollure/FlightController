/*
 * BNO055.c
 *
 *  Created on: May 16, 2018
 *      Author: pasindu
 */

#include "stm32f4xx_hal.h"
#include "BNO055.h"
#include "BNO055_definitions.h"
#include "stm32_peripherals.h"

#define TRANSACTION_TIMEOUT 10


/* Variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
static BNO bno;

uint8_t BNO_I2C_Read(uint8_t regAddr){
	uint8_t rxData = 0;
	// BNO055 definition needs RW bit enabled
	HAL_I2C_Mem_Read(&hi2c1,BNO055_I2C_ADDR1_R,regAddr,1,&rxData,1,TRANSACTION_TIMEOUT);
	return  rxData;
}

void BNO_I2C_Write(uint8_t regAddr, uint8_t* data){
	HAL_I2C_Mem_Write(&hi2c1,BNO055_I2C_ADDR1_W,regAddr,1,data,sizeof(*data),TRANSACTION_TIMEOUT);
}

void BNO_I2C_Configure(void){
	uint8_t data = 0x00; // sets Operating mode to CONFIG
	BNO_I2C_Write(BNO055_OPR_MODE_ADDR,&data);
	printf("Operating Mode set to CONFIG \n\r");
	HAL_Delay(500);

	data = 0x20; // resets system
	BNO_I2C_Write(BNO055_SYS_TRIGGER_ADDR,&data);
	printf("Set system to reset \n\r");
	HAL_Delay(500);

	printf("Checking Chip ID...  \n\r");
	while (BNO_I2C_Read(BNO055_CHIP_ID_ADDR) != 0xA0){
		HAL_Delay(30);
	}

	printf("Chip ID: %d \n\r",BNO_I2C_Read(BNO055_CHIP_ID_ADDR));
	HAL_Delay(500);

	printf("Chip ID Correct.. \n\r");

	data = 0x00; //set power mode to normal
	BNO_I2C_Write(BNO055_PWR_MODE_ADDR,&data);
	printf("Power mode set to Normal.. \n\r");
	HAL_Delay(500);

	data = 0x00; // Register page ID set to page 0
	BNO_I2C_Write(BNO055_PAGE_ID_ADDR,&data);
	printf("Register page set to Page 0.. \n\r");
	HAL_Delay(500);

	data = 0x00; // system no longer set to reset and internal CLK is selected
	BNO_I2C_Write(BNO055_SYS_TRIGGER_ADDR,&data);
	printf("System no longer set to reset.. \n\r");
	HAL_Delay(500);

	data = 0x0C; // set operation mode to NDOF
	BNO_I2C_Write(BNO055_OPR_MODE_ADDR,&data);
	printf("Operation mode set to NDOF.. \n\n\r");
	HAL_Delay(500);
}

void BnoUpdateEuler(void){
	bno.eulerX = (BNO_I2C_Read(BNO055_EULER_P_MSB_ADDR) << 8) | BNO_I2C_Read(BNO055_EULER_P_LSB_ADDR);
	bno.eulerY = (BNO_I2C_Read(BNO055_EULER_R_MSB_ADDR) << 8) | BNO_I2C_Read(BNO055_EULER_R_LSB_ADDR);
	bno.eulerZ = (BNO_I2C_Read(BNO055_EULER_H_MSB_ADDR) << 8) | BNO_I2C_Read(BNO055_EULER_H_LSB_ADDR);
}

void BnoPrintEuler(void){
	printf("X: %f\t Y: %f\t Z: %f\n",((double)bno.eulerX)/16.0f,((double)bno.eulerY)/16.0f,((double)bno.eulerZ)/16.0f);
}
