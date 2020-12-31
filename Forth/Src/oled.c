/**
 *  @brief
 *  @file
 *      oled.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-12-30
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
/**
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * This is part of the Tinusaur/SSD1306xLED project.
 *
 * Copyright (c) 2016 Neven Boyanov, Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Please, as a favor, retain the link http://tinusaur.org to The Tinusaur Project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// System include files
// ********************
#include "cmsis_os.h"
#include <stdio.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "oled.h"
#include "iic.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************

// Hardware resources
// ******************

// RTOS resources
// **************



// Private Variables
// *****************

static const uint8_t font6x8 [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sp
  0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, // !
  0x00, 0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, // #
  0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
  0x00, 0x62, 0x64, 0x08, 0x13, 0x23, // %
  0x00, 0x36, 0x49, 0x55, 0x22, 0x50, // &
  0x00, 0x00, 0x05, 0x03, 0x00, 0x00, // '
  0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, // (
  0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, // )
  0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, // *
  0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x00, 0x00, 0xA0, 0x60, 0x00, // ,
  0x00, 0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x00, 0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x00, 0x42, 0x61, 0x51, 0x49, 0x46, // 2
  0x00, 0x21, 0x41, 0x45, 0x4B, 0x31, // 3
  0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x00, 0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
  0x00, 0x01, 0x71, 0x09, 0x05, 0x03, // 7
  0x00, 0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x00, 0x56, 0x36, 0x00, 0x00, // ;
  0x00, 0x08, 0x14, 0x22, 0x41, 0x00, // <
  0x00, 0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x00, 0x00, 0x41, 0x22, 0x14, 0x08, // >
  0x00, 0x02, 0x01, 0x51, 0x09, 0x06, // ?
  0x00, 0x32, 0x49, 0x59, 0x51, 0x3E, // @
  0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C, // A
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, // D
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A, // G
  0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
  0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x00, 0x46, 0x49, 0x49, 0x49, 0x31, // S
  0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, // W
  0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x00, 0x07, 0x08, 0x70, 0x08, 0x07, // Y
  0x00, 0x61, 0x51, 0x49, 0x45, 0x43, // Z
  0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, // [
  0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55, // 55
  0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, // ]
  0x00, 0x04, 0x02, 0x01, 0x02, 0x04, // ^
  0x00, 0x40, 0x40, 0x40, 0x40, 0x40, // _
  0x00, 0x00, 0x01, 0x02, 0x04, 0x00, // '
  0x00, 0x20, 0x54, 0x54, 0x54, 0x78, // a
  0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, // b
  0x00, 0x38, 0x44, 0x44, 0x44, 0x20, // c
  0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, // d
  0x00, 0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x00, 0x08, 0x7E, 0x09, 0x01, 0x02, // f
  0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, // g
  0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x00, 0x40, 0x80, 0x84, 0x7D, 0x00, // j
  0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, // k
  0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, // m
  0x00, 0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0x00, 0x38, 0x44, 0x44, 0x44, 0x38, // o
  0x00, 0xFC, 0x24, 0x24, 0x24, 0x18, // p
  0x00, 0x18, 0x24, 0x24, 0x18, 0xFC, // q
  0x00, 0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x00, 0x48, 0x54, 0x54, 0x54, 0x20, // s
  0x00, 0x04, 0x3F, 0x44, 0x40, 0x20, // t
  0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x00, 0x44, 0x28, 0x10, 0x28, 0x44, // x
  0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C, // y
  0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x14, 0x14, 0x14, 0x14, 0x14, 0x14, // horiz lines
};

/* Standard ASCII 8x16 font */
static const uint8_t font8x16 []  = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00, // ! 1
  0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // " 2
  0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00, // # 3
  0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00, // $ 4
  0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00, // % 5
  0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10, // & 6
  0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ' 7
  0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00, // ( 8
  0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00, // ) 9
  0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00, // * 10
  0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00, // + 11
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00, // , 12
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01, // - 13
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00, // . 14
  0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00, // / 15
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00, // 0 16
  0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00, // 1 17
  0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00, // 2 18
  0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00, // 3 19
  0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00, // 4 20
  0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00, // 5 21
  0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00, // 6 22
  0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00, // 7 23
  0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00, // 8 24
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00, // 9 25
  0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00, // : 26
  0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00, // ; 27
  0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00, // < 28
  0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00, // = 29
  0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00, // > 30
  0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00, // ? 31
  0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00, // @ 32
  0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20, // A 33
  0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00, // B 34
  0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00, // C 35
  0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00, // D 36
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00, // E 37
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00, // F 38
  0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00, // G 39
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20, // H 40
  0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00, // I 41
  0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00, // J 42
  0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00, // K 43
  0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00, // L 44
  0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00, // M 45
  0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00, // N 46
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00, // O 47
  0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00, // P 48
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00, // Q 49
  0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20, // R 50
  0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00, // S 51
  0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00, // T 52
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00, // U 53
  0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00, // V 54
  0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00, // W 55
  0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20, // X 56
  0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00, // Y 57
  0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00, // Z 58
  0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00, // [ 59
  0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00, // \ 60
  0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00, // ] 61
  0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ^ 62
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80, // _ 63
  0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ` 64
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20, // a 65
  0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00, // b 66
  0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00, // c 67
  0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20, // d 68
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00, // e 69
  0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00, // f 70
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00, // g 71
  0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20, // h 72
  0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00, // i 73
  0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00, // j 74
  0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00, // k 75
  0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00, // l 76
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F, // m 77
  0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20, // n 78
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00, // o 79
  0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00, // p 80
  0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80, // q 81
  0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00, // r 82
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00, // s 83
  0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00, // t 84
  0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20, // u 85
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00, // v 86
  0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00, // w 87
  0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00, // x 88
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00, // y 89
  0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00, // z 90
  0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40, // { 91
  0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00, // | 92
  0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00, // } 93
  0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ~ 94
};

static const uint8_t display_off[] =	{ 1, 0xAE };			// Display OFF (sleep mode)
static const uint8_t adr_mode_horiz[] =	{ 2, 0x20, 0b00 };		// Set Memory Addressing Mode
							// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
							// 10=Page Addressing Mode (RESET); 11=Invalid
static const uint8_t page_adr[] = 		{ 3, 0x22, 0, 127 };	// Set Page Start Address for Page Addressing Mode, 0-7
static const uint8_t com_scan_rev[] = 	{ 1, 0xC8 };			// Set COM Output Scan Direction
static const uint8_t lower_col_adr [] =	{ 1, 0x00 };			// ---set low column address
static const uint8_t higher_col_adr[] =	{ 1, 0x10 };			// ---set high column address
static const uint8_t start_line_adr[] =	{ 1, 0x40 };			// --set start line address
static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x3F };		// Set contrast control register
static const uint8_t seg_remap[] = 		{ 1, 0xA1 };			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
static const uint8_t display_normal[] =	{ 1, 0xA6 };			// Set display mode. A6=Normal;
// static const uint8_t display_inverse[] ={ 0xA7 };			// A7=Inverse

static const uint8_t mplx_ratio[] =     { 2, 0xA8, 0x3F };		// Set multiplex ratio(1 to 64)
static const uint8_t ram_to_display[] = { 1, 0xA4 };			// Output RAM to Display
						// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
static const uint8_t display_offset[] = { 2, 0xD3, 0x00 };		// Set display offset. 00 = no offset
static const uint8_t clk_div_ratio[] =  { 2, 0xD5, 0x80 };		// --set display clock divide ratio/oscillator frequency
static const uint8_t div_ratio[] =      { 1, 0xF0 };			// --set divide ratio
static const uint8_t pre_charge[] =     { 2, 0xD9, 0x22 };		// Set pre-charge period
static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x12 };		// Set com pins hardware configuration
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x20};		// --set vcomh 0x20,0.77xVcc
static const uint8_t dcdc_en[] =		{ 2, 0x8D, 0x14 };		// Set DC-DC enable
static const uint8_t display_on[] =		{ 1, 0xAF };			// Display ON in normal mode

static const uint8_t *ssd1306_init_sequence[] = {	// Initialization Sequence
		display_off,
		adr_mode_horiz,
		page_adr,
		com_scan_rev,
		lower_col_adr,
		higher_col_adr,
		start_line_adr,
		set_contrast,
		seg_remap,
		display_normal,
		mplx_ratio,
		ram_to_display,
		display_offset,
		clk_div_ratio,
		div_ratio,
		pre_charge,
		com_pin_hw,
		set_vcomh,
		dcdc_en,
		display_on
};


// Public Functions
// ****************
#define  membersof(x) (sizeof(x) / sizeof(x[0]))

void OLED_init(void) {
	uint8_t i;

	for (i = 0; i < membersof(ssd1306_init_sequence); i++) {
//		OLED_sendCommand(ssd1306_init_sequence[i], sizeof(*ssd1306_init_sequence[i]));
		OLED_sendCommand(ssd1306_init_sequence[i]);
	}
}

void OLED_sendCommand(const uint8_t *command) {
	uint8_t buf[5];

	buf[0] = 0x00; // write command
	memcpy(&buf[1], &command[1], command[0]);
	IIC_setDevice(60);
	IIC_putMessage(buf, command[0]+1);
}


void OLED_setPos(uint8_t x, uint8_t y) {
	uint8_t buf[4];

	buf[0] = 0x00; // write command
	buf[1] = 0xb0 + y;
	buf[2] = ((x & 0xf0) >> 4) | 0x10; // | 0x10
	buf[3] = x & 0x0f; // | 0x01
	IIC_setDevice(60);
	IIC_putMessage(buf, 4);
}


void OLED_sendChar(char ch) {
	uint8_t buf[7];
	uint8_t i;

	buf[0] = 0x40;  // write data
	uint8_t c = ch - 32;
	for (i = 0; i < 6; i++) {
		buf[i+1] = font6x8[c * 6 + i];
	}
	IIC_setDevice(60);
	IIC_putMessage(buf, 7);
}


void OLED_sendCharLarge(char ch) {
	uint8_t buf[17];
	uint8_t i;

	buf[0] = 0x40;  // write data
	uint8_t c = ch - 32;
	for (i = 0; i < 16; i++) {
		buf[i+1] = font8x16[c * 16 + i];
	}
//	ssd1306_setpos(x, y + 1);
//	for (i = 0; i < 8; i++) {
//		buf[i+9] = font8x16[c * 16 + i+8];
//	}
	IIC_setDevice(60);
	IIC_putMessage(buf, 17);
}


//void OLED_drawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
//{
//	uint16_t j = 0;
//	uint8_t y;
//	if (y1 % 8 == 0) y = y1 / 8;
//	else y = y1 / 8 + 1;
//	for (y = y0; y < y1; y++)
//	{
//		ssd1306_setpos(x0,y);
//		ssd1306_send_data_start();
//		for (uint8_t x = x0; x < x1; x++)
//		{
//			ssd1306_send_byte(pgm_read_byte(&bitmap[j++]));
//		}
//		ssd1306_send_data_stop();
//	}
//}

