/**
 *  @brief
 *      Some basic definitions.
 *
 *  @file
 *      app_common.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-04
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

#ifndef INC_APP_COMMON_H_
#define INC_APP_COMMON_H_

// Basic definitions
// *****************

// same as in Forth
#undef FALSE
#define FALSE                   0

#undef TRUE
#define TRUE                    (!0)


// Critical Section definition
// ***************************

#define BACKUP_PRIMASK()    uint32_t primask_bit= __get_PRIMASK()
#define DISABLE_IRQ()       __disable_irq()
#define RESTORE_PRIMASK()   __set_PRIMASK(primask_bit)

#endif /* INC_APP_COMMON_H_ */
