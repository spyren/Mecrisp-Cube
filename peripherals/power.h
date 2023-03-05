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

void POWER_startup(void);
void POWER_init(void);
void POWER_halt(void);
void POWER_setSwitch(int sw);
int POWER_getSwitch(void);
void POWER_switchEvent(int sw);

#endif /* INC_POWER_H_ */
