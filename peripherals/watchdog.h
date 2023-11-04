/**
 *  @brief
 *      Window watchdog.
 *
 *		There is a thread only for kicking (regularly feed) the watchdog.
 *		This thread has the second lowest priority (just higher than the
 *		idle thread), if any task does not want to give up control, the
 *		watchdog thread does not get any more CPU time and cannot feed
 *		the watchdog and the watchdog will bite after the timeout.
 *  @file
 *      watchdog.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2021-12-27
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



#ifndef INC_WATCHDOG_H_
#define INC_WATCHDOG_H_

void WATCHDOG_init(void);
void WATCHDOG_activate(void);
int WATCHDOG_bitten(void);
int WATCHDOG_bites(void);

#endif /* INC_WATCHDOG_H_ */
