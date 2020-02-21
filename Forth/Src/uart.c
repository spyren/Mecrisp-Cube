/**
 *  @brief
 *      UART terminal console
 *
 *      Line buffered serial communication. Using DMA and interrupts.
 *      CR is end of line for Rx.
 *      LF is end of line for Tx.
 *  @file
 *      uart.c
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

/**
 *  @brief
 *      Initializes the UART.
 *  @return
 *      None
 */
void UART_init(void) {
}

/**
 *  @brief
 *      Reads a char from the UART Rx (serial in). Blocking until char is
 *      ready (the line is written).
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int UART_getc(void) {

}


/**
 *  @brief
 *      Reads a line from the UART Rx (serial in). Blocking until line is
 *      ready (CR).
 *  @return
 *      Return the character string.
 */
char *UART_gets(char *s) {
	;
}


/**
 *  @brief
 *    Rx End Of Line.
 *  @return
 *    True if a line has been received (CR) or the buffer is full.
 */
int UART_RxEOL(void) {

}


/**
 *  @brief
 *      Writes a char to the UART Tx (serial out). Blocking until char can be
 *      written into the buffer. The buffer will be send after LF is written.
 *  @param
 *      c  char to write
 *  @return
 *      Return EOF on error.
 */
int UART_putc(int c) {
}


/**
 *  @brief
 *      Writes a line (string) to the UART Tx (serial out). Blocking until
 *      string can be written into the buffer.
 *  @param
 *      s  string to write
 *  @return
 *      Return EOF on error.
 */
int UART_puts(const char *s) {
}


/**
 *  @brief
 *      Tx buffer ready for next char.
 *  @return
 *      False if the buffer is full.
 */
int UART_TxReady(void) {

}

