/**
 *  @brief
 *      Digital Command Control Driver
 *
 *		Using TIM16, resolution 1 us, interrupt every 58 us.
 *		max. 2*116 us for one 0-bit -> min. 4 kBit/s
 *		129 bit in 30 ms, about min. 12 bytes, average 18 bytes
 *		Max. 4 active locomotive slots
 *  @file
 *      dcc.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2025-04-27
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

// System include files
// ********************
#include "cmsis_os.h"
#include <stdio.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"

#include "dcc.h"

DCC_LocoSlot_t DCC_LocoSlot[DCC_MAX_LOCO_SLOTS];

static uint8_t  packet[DCC_MAX_PACKET_LENGTH];
static uint32_t	byte_count;
static uint32_t bit_count;
static uint32_t packet_length;

