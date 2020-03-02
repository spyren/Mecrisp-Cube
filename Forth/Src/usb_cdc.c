/**
 *  @brief
 *      USB CDC terminal console
 *
 *      Line buffered serial communication. Using DMA and interrupts.
 *      CR is end of line for Rx.
 *      LF is end of line for Tx.
 *  @file
 *      usb_cdc.c
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

// System include files
// ********************
#include "cmsis_os.h"

// Application include files
// *************************
#include "usb_cdc.h"


// Private function prototypes
// ***************************
static void cdc_thread(void *argument);

// Global Variables
// ****************

// Definitions for CDC thread
osThreadId_t CDC_ThreadID;
const osThreadAttr_t cdc_thread_attributes = {
		.name = "CDC_Thread",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 128
};

// Private Variables
// *****************

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the CDC.
 *  @return
 *      None
 */
void CDC_init(void) {
	// creation of CDC_Thread
	CDC_ThreadID = osThreadNew(cdc_thread, NULL, &cdc_thread_attributes);

}


/**
 *  @brief
 *		Reads a char from the CDC Rx (serial in). Blocking until char is
 *      ready (the line is written).
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int CDC_getc(void) {
	return 0;
}


/**
 *  @brief
 *      Reads a line from the CDC Rx (serial in). Blocking until line is
 *      ready (newline character CR is read) or max length is reached.
 *  @param[out]
 *  	str This is the pointer to an array of chars where the C string is stored
 *  @param[in]
 *      length  max. string length
 *  @return
 *      Return 0 if successful, -1 on error
 */
int CDC_gets(char *str, int length) {
	return 0;
}


/**
 *  @brief
 *		Rx End Of Line.
 *  @return
 *		True if a line has been received (CR) or the buffer is full.
 */
int CDC_RxEOL(void) {
	return 0;
}


/**
 *  @brief
 *      Writes a char to the CDC Tx (serial out). Blocking until char can be
 *      written into the buffer. The buffer will be send after LF is written.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int CDC_putc(int c) {
	return 0;
}


/**
 *  @brief
 *      Writes a line (string) to the CDC Tx (serial out). Blocking until
 *      string can be written into the buffer.
 *  @param
 *      s  string to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int CDC_puts(const char *s) {
	return 0;
}


/**
 *  @brief
 *      Tx buffer ready for next char.
 *  @return
 *      False if the buffer is full.
 */
int CDC_TxReady(void) {
	return 0;
}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the USB-CDC thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void cdc_thread(void *argument) {
	// Infinite loop
	for(;;) {
		osDelay(1);
	}
}

