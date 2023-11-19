/**
 *  @brief
 *  	RGB and White LED driver based on the controller LP5562.
 *
 *  	8-Bit Current Setting (From 0 mA to 25.5 mA With 100 μA Steps)
 *  	8-Bit PWM Control
 *  	I2C Interface, address 0x30.
 *  	400kHz I2C-compatible interface
 *  @file
 *      rgbw.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2023-11-19
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

#ifndef INC_RGBW_H_
#define INC_RGBW_H_

#define RGBW_I2C_ADR		0x30


void RGBW_init(void);
void RGBW_setRGB(uint32_t rgb);
uint32_t RGBW_getRGB(void);
void RGBW_setW(uint8_t w);
uint8_t RGBW_getW(void);


#endif /* INC_RGBW_H_ */
