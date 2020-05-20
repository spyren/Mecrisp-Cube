/**
 *  @brief
 *      UART terminal console
 *
 *      Line buffered serial communication.
 *      CR is end of line for Rx.
 *      LF is end of line for Tx.
 *  @file
 *      uart.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-02-19
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



#ifndef INC_UART_H_
#define INC_UART_H_

extern osMutexId_t UART_MutexID;

void UART_init(void);
int UART_getc(void);
int UART_gets(char *str, int length);
int UART_RxReady(void);
int UART_putc(int c);
int UART_puts(const char *s);
int UART_TxReady(void);
void UART_setBaudrate(const int baudrate);
void UART_setWordLength(const int wordlength);
void UART_setParityBit(const int paritybit);
void UART_setStopBits(const int stopbits);

#endif /* INC_UART_H_ */
