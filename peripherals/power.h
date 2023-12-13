/**
 *  @brief
 *      Power management e.g. low power, shutdown and the like.
 *
 *  @file
 *      power.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2023-03-02
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



#ifndef INC_POWER_H_
#define INC_POWER_H_

#define POWER_SWITCH1 0x01
#define POWER_SWITCH2 0x02
#define POWER_SHUTDOWN 0x1000

#define CHARGER_I2C_ADR		0x6b
#define GAUGE_I2C_ADR		0x55

#define GAUGE_CMD_CONTROL				0x00
#define GAUGE_CMD_AT_RATE 				0x02
#define GAUGE_CMD_AT_RATE_TIME_TO_EMPTY	0x04
#define GAUGE_CMD_TEMPERATURE 			0x06
#define GAUGE_CMD_VOLTAGE 				0x08
#define GAUGE_CMD_BATTERY_STATUS	 	0x0A
#define GAUGE_CMD_CURRENT 				0x0C
#define GAUGE_CMD_REMAINING_CAPACITY	0x10
#define GAUGE_CMD_FULL_CHARGE_CAPACITY 	0x12
#define GAUGE_CMD_AVERAGE_CURRENT 		0x14
#define GAUGE_CMD_TIME_TO_EMPTY 		0x16
#define GAUGE_CMD_TIME_TO_FULL 			0x18
#define GAUGE_CMD_STANDBY_CURRENT 		0x1A
#define GAUGE_CMD_STANDBY_TO_EMPTY	 	0x1C
#define GAUGE_CMD_MAX_LOAD_CURRENT 		0x1E
#define GAUGE_CMD_MAX_LOADTIME_TO_EMPTY 0x20
#define GAUGE_CMD_RAW_COULOMB_COUNT 	0x22
#define GAUGE_CMD_AVERAGE_POWER	 		0x24
#define GAUGE_CMD_INTERNAL_TEMPERATURE 	0x28
#define GAUGE_CMD_CYCLE_COUNT			0x2A
#define GAUGE_CMD_STATE_OF_CHARGE 		0x2C
#define GAUGE_CMD_STATE_OF_HEALTH 		0x2E
#define GAUGE_CMD_CHARGE_VOLTAGE	 	0x30
#define GAUGE_CMD_CHARGE_CURRENT	 	0x32
#define GAUGE_CMD_BTP_DISCHARGE_SET 	0x34
#define GAUGE_CMD_BTP_CHARGE_SET	 	0x36
#define GAUGE_CMD_OPERATION_STATUS 		0x3A
#define GAUGE_CMD_DESIGN_CAPACITY 		0x3C
#define GAUGE_CMD_SELECT_SUBCLASS 		0x3E
#define GAUGE_CMD_MAC_DATA				0x40
#define GAUGE_CMD_MAC_DATA_SUM 			0x60
#define GAUGE_CMD_MAC_DATA_LEN 			0x61
#define GAUGE_CMD_ANALOG_COUNT 			0x79
#define GAUGE_CMD_RAW_CURRENT 			0x7A
#define GAUGE_CMD_RAW_VOLTAGE 			0x7C
#define GAUGE_CMD_RAW_INT_TEMP 			0x7E

#define GAUGE_CTRL_CONTROL_STATUS 		0x0000
#define GAUGE_CTRL_DEVICE_NUMBER 		0x0001
#define GAUGE_CTRL_FW_VERSION 			0x0002
#define GAUGE_CTRL_BOARD_OFFSET 		0x0009
#define GAUGE_CTRL_CC_OFFSET 			0x000A
#define GAUGE_CTRL_CC_OFFSET_SAVE 		0x000B
#define GAUGE_CTRL_OCV_CMD 				0x000C
#define GAUGE_CTRL_BAT_INSERT 			0x000D
#define GAUGE_CTRL_BAT_REMOVE 			0x000E
#define GAUGE_CTRL_SET_SNOOZE 			0x0013
#define GAUGE_CTRL_CLEAR_SNOOZE 		0x0014
#define GAUGE_CTRL_SET_PROFILE_1 		0x0015
#define GAUGE_CTRL_SET_PROFILE_2 		0x0016
#define GAUGE_CTRL_SET_PROFILE_3 		0x0017
#define GAUGE_CTRL_SET_PROFILE_4 		0x0018
#define GAUGE_CTRL_SET_PROFILE_5 		0x0019
#define GAUGE_CTRL_SET_PROFILE_6 		0x001A
#define GAUGE_CTRL_CAL_TOGGLE 			0x002D
#define GAUGE_CTRL_SEALED 				0x0030
#define GAUGE_CTRL_RESET 				0x0041
#define GAUGE_CTRL_EXIT_CAL 			0x0080
#define GAUGE_CTRL_ENTER_CAL 			0x0081
#define GAUGE_CTRL_ENTER_CFG_UPDATE 	0x0090
#define GAUGE_CTRL_EXIT_CFG_UPDATE_REINIT 0x0091
#define GAUGE_CTRL_EXIT_CFG_UPDATE 		0x0092
#define GAUGE_CTRL_RETURN_TO_ROM 		0x0F00

extern uint8_t GAUGE_UpdateBatState;


void POWER_startup(void);
void POWER_init(void);
void POWER_halt(void);
void POWER_setSwitch(int sw);
int POWER_getSwitch(void);
void POWER_switchEvent(int sw);
void POWER_setPeripheral(int status);

int GAUGE_getVoltage(void);
int GAUGE_getCurrent(void);
int GAUGE_getCharge(void);
int GAUGE_getRegister(uint8_t reg);
void GAUGE_setRegister(uint8_t reg, int16_t data);
int CHARGER_getRegister(uint8_t reg);
void CHARGER_setRegister(uint8_t reg, uint8_t data);

#endif /* INC_POWER_H_ */
