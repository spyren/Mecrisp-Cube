/**
 *  @brief
 *  	On-board MEMS sensors.
 *
 *  @file
 *      mems.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-02-17
 *  @remark
 *      Language: C, STM32CubeIDE GCC
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This project Mecrsip-Cube is free software: you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation, either version 3 of
 *      the License, or (at your option) any later version.
 *
 *      Mecrsip-Cube is distributed in the hope that it will be useful, but
 *      WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *      General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.
 */

#ifndef INC_MEMS_H_
#define INC_MEMS_H_


// Temperature sensor TTS22H
// *************************

// 7 bit I2C addresses
#define MEMS_TTS22H_I2C_ADR			0x38

// Device Identification (Who am I)
#define MEMS_TTS22H_ID				0xA0U

// TTS22H Registers
#define MEMS_TTS22H_WHOAMI			1
#define MEMS_TTS22H_TEMP_H_LIMIT	2
#define MEMS_TTS22H_TEMP_L_LIMIT	3
#define MEMS_TTS22H_CTRL			4
#define MEMS_TTS22H_STATUS			5
#define MEMS_TTS22H_TEMP_L_OUT		6
#define MEMS_TTS22H_TEMP_H_OUT		7

#define MEMS_TTS22H_ONE_SHOT		0x01
#define MEMS_TTS22H_BUSY			0x01

// Accelerometer ISM330DHCX
// ************************

#define MEMS_ISM330DHCX_I2C_ADR		0x6b

// Device Identification (Who am I)
#define MEMS_ISM330DHCX_ID			0b01101011

// ISM330DHCX Registers
#define MEMS_ISM330DHCX_WHOAMI		0x0F


// Time‑of‑Flight sensor VL53L0CXV0DH
// **********************************

#define MEMS_VL53L0X_I2C_ADR		0x29

// Device Identification (Who am I)
#define MEMS_VL53L0X_ID				0xA0U

// VL53L0CXV0DH Registers
#define MEMS_VL53L0X_WHOAMI			1

extern osSemaphoreId_t MEMS_DRDY_SemaphoreID;

void MEMS_init(void);
int MEMS_TTS22H_getRegister(uint8_t reg);
void MEMS_TTS22H_setRegister(uint8_t reg, uint8_t data);
int MEMS_ISM330_getRegister(uint8_t reg);
void MEMS_ISM330_setRegister(uint8_t reg, uint8_t data);
int MEMS_getTemperature(void);

#endif /* INC_MEMS_H_ */
