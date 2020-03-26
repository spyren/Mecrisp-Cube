/**
 *  @brief
 *      Board Support Package.
 *
 *      LEDs and switches.
 *  @file
 *      bsp.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-26
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



#ifndef INC_BSP_H_
#define INC_BSP_H_

void BSP_init(void);

void BSP_setLED1(unsigned state);
unsigned BSP_getLED1(void);
void BSP_setLED2(unsigned state);
unsigned BSP_getLED2(void);
void BSP_setLED3(unsigned state);
unsigned BSP_getLED3(void);
unsigned BSP_getSwitch1(void);
unsigned BSP_getSwitch2(void);
unsigned BSP_getSwitch3(void);

#endif /* INC_BSP_H_ */
