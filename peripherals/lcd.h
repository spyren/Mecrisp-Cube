/**
 *  @brief
 *  	OLED driver based on the controller SSD1306.
 *
 *  	Resolution 128x64, monochrome.
 *  	SPI Interface.
 *  @file
 *      lcd.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2023-11-25
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

#ifndef INC_LCD_H_
#define INC_LCD_H_

#define LCD_LANDSCAPE		0
#define	LCD_PORTRAIT		1

// configure your LCD type
#define	LCD_ORIENTATION	LCD_LANDSCAPE

#define LCD_X_RESOLUTION	128
#define LCD_Y_RESOLUTION	64

#define LCD_LINES			(LCD_Y_RESOLUTION / 8)

typedef enum {LCD_FONT6X8, LCD_FONT8X8, LCD_FONT8X16, LCD_FONT12X16} LCD_FontT;

void LCD_init(void);
void LCD_sendCommand(const uint8_t *command);
void LCD_clear(void);
void LCD_update(void);
void LCD_setPos(uint8_t x, uint8_t y);
uint8_t LCD_getPosX();
uint8_t LCD_getPosY();
void LCD_writeColumn(uint8_t column);
int LCD_readColumn(void);
int LCD_putc(int c);
int LCD_puts(const char *s);
void LCD_setFont(LCD_FontT font);
int LCD_readStatus(void);
void LCD_putXBM(char* image, int width, int height);

#endif /* INC_LCD_H_ */
