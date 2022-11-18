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

void BSP_setLED1(int state);
int BSP_getLED1(void);
void BSP_setLED2(int state);
int BSP_getLED2(void);
void BSP_setLED3(int state);
int BSP_getLED3(void);
int BSP_getSwitch1(void);
int BSP_getSwitch2(void);
int BSP_getSwitch3(void);
void BSP_setDigitalPort(int state);
int BSP_getDigitalPort(void);
void BSP_setDigitalPin(int pin_number, int state);
int BSP_getDigitalPin(int pin_number);
int BSP_getAnalogPin(int pin_number);
int BSP_getVref(void);
int BSP_getVbat(void);
int BSP_getCpuTemperature(void);
void BSP_setPwmPin(int pin_number, int value);
void BSP_setPwmPrescale(uint16_t value);
void BSP_TIM2_PeriodElapsedCallback(void);
void BSP_setPrescaleICOC(uint32_t prescale);
void BSP_setPeriodICOC(uint32_t period);
void BSP_setCounterICOC(uint32_t count);
uint32_t BSP_getCounterICOC(void);
void BSP_startPeriodICOC(void);
void BSP_stopPeriodICOC(void);
void BSP_setModeOC(int pin_number, uint32_t mode);
void BSP_startOC(int pin_number, uint32_t pulse);
void BSP_stopOC(int pin_number);
void BSP_startIC(uint32_t mode);
void BSP_stopIC(void);
void BSP_waitPeriod(void);
uint32_t BSP_waitIC(uint32_t timeout);
void BSP_waitOC(int pin_number);

void BSP_neopixelDataTx(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t GRBx);
void BSP_neopixelBufferTx(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t *buffer, uint32_t len);
void BSP_setNeoPixel(uint32_t rgb);
int BSP_getNeoPixel(void);
void BSP_setNeoPixels(uint32_t *buffer, uint32_t len);


#endif /* INC_BSP_H_ */
