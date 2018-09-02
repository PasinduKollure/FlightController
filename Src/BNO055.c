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
#include "master_configuration.h"

#define TRANSACTION_TIMEOUT 10


/* Variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

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
	printf("Operating Mode set to CONFIG \r");
	HAL_Delay(500);

	data = 0x20; // resets system
	BNO_I2C_Write(BNO055_SYS_TRIGGER_ADDR,&data);
	printf("Set system to reset \r");
	HAL_Delay(500);

	printf("Checking Chip ID...  \r");
	while (BNO_I2C_Read(BNO055_CHIP_ID_ADDR) != 0xA0){
		HAL_Delay(30);
	}

	printf("Chip ID: %d \r",BNO_I2C_Read(BNO055_CHIP_ID_ADDR));
	HAL_Delay(500);

	printf("Chip ID Correct.. \r");

	data = 0x00; //set power mode to normal
	BNO_I2C_Write(BNO055_PWR_MODE_ADDR,&data);
	printf("Power mode set to Normal.. \r");
	HAL_Delay(500);

	data = 0x00; // Register page ID set to page 0
	BNO_I2C_Write(BNO055_PAGE_ID_ADDR,&data);
	printf("Register page set to Page 0.. \r");
	HAL_Delay(500);

	data = 0x00; // system no longer set to reset and internal CLK is selected
	BNO_I2C_Write(BNO055_SYS_TRIGGER_ADDR,&data);
	printf("System no longer set to reset.. \r");
	HAL_Delay(500);

	data = 0x0C; // set operation mode to NDOF
	BNO_I2C_Write(BNO055_OPR_MODE_ADDR,&data);
	printf("Operation mode set to NDOF.. \n\n\r");
	HAL_Delay(500);
}

void updateEulerAngles(EulerAngles* bno){
	bno->rawEulerX = ((BNO_I2C_Read(BNO055_EULER_P_MSB_ADDR) << 8) | BNO_I2C_Read(BNO055_EULER_P_LSB_ADDR));
	bno->rawEulerX = bno->rawEulerX / 16;
	bno->rawEulerY = ((BNO_I2C_Read(BNO055_EULER_R_MSB_ADDR) << 8) | BNO_I2C_Read(BNO055_EULER_R_LSB_ADDR));
	bno->rawEulerY = bno->rawEulerY / 16;
	bno->rawEulerZ = ((BNO_I2C_Read(BNO055_EULER_H_MSB_ADDR) << 8) | BNO_I2C_Read(BNO055_EULER_H_LSB_ADDR));
	bno->rawEulerZ = bno->rawEulerZ / 16;

	if(bno->rawEulerX > IMU_MAX_ANGLE) 
		bno->eulerX = IMU_MAX_ANGLE;
	else if(bno->rawEulerX < -1 * IMU_MAX_ANGLE) 
		bno->eulerX = -1 * IMU_MAX_ANGLE;
	else 
		bno->eulerX = bno->rawEulerX;
	
	if(bno->rawEulerY > IMU_MAX_ANGLE) 
		bno->eulerY = IMU_MAX_ANGLE;
	else if(bno->rawEulerY < -1 * IMU_MAX_ANGLE) 
		bno->eulerY = -1 * IMU_MAX_ANGLE;
	else 
		bno->eulerY = bno->rawEulerY;
}

void BnoPrintEuler(EulerAngles* imu){
	printf("X: %d\t Y: %d\t Z: %d\n", imu->eulerX, imu->eulerY, imu->rawEulerZ);
}
