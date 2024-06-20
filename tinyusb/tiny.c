/**
 *  @brief
 *      tinyusb for Mecrisp-Cube
 *
 *		The tud_task() is called in the usb_device_thread.
 *		The callbacks are also handled here.
 *  @file
 *      tiny.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-02-22
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


/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "app_common.h"
#include "cmsis_os.h"

#include "usb_cdc.h"

// Private function prototypes
// ***************************
static void usb_device_thread(void *argument);

// Global Variables
// ****************
const char TINY_Version[] =
	    "  * TinyUSB CDC, MSC v0.16.0 (C) 2023, hathach (tinyusb.org)\n";
int volatile TINY_tud_cdc_connected = TRUE;

// RTOS resources
// **************

// Definitions for USB Device thread
static osThreadId_t TINY_UsbDeviceId;
static const osThreadAttr_t usb_device_attributes = {
		.name = "USB_Device",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 128*8
};


void TINY_init(void) {
	board_init();
	tusb_init();

	// creation of USB Device thread
	TINY_UsbDeviceId = osThreadNew(usb_device_thread, NULL, &usb_device_attributes);
	ASSERT_fatal(TINY_UsbDeviceId != NULL, ASSERT_THREAD_CREATION, __get_PC());
}

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
static void usb_device_thread(void *param) {
  (void) param;

  // init device stack on configured roothub port
  // This should be called after scheduler/kernel is started.
  // Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  // RTOS forever loop
  while (1) {
    // put this thread to waiting state until there is new events
    tud_task();

    // following code only run if tud_task() process at least 1 event
//    tud_cdc_write_flush();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
}

// Invoked when a TX is complete and therefore space becomes available in TX buffer
void tud_cdc_tx_complete_cb(uint8_t itf) {
	(void) itf;
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
	(void) itf;
	(void) rts;

	if (dtr) {
		// Terminal connected
		osThreadFlagsSet(CDC_ThreadID, CDC_CONNECTED);
		TINY_tud_cdc_connected = TRUE;
	} else {
		// Terminal disconnected
		osThreadFlagsSet(CDC_ThreadID, CDC_DISCONNECTED);
		TINY_tud_cdc_connected = FALSE;
	}
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
	(void) itf;
	uint8_t buf;

	ASSERT_nonfatal(itf != 0x03, ASSERT_CDC_SIGINT, 0); // ^C character abort
	// this can block the calling thread/task
	osMessageQueuePut(CDC_RxQueueId, &itf, 0, osWaitForever);
	while (tud_cdc_available()) {
		buf = tud_cdc_read_char();
		ASSERT_nonfatal(buf != 0x03, ASSERT_CDC_SIGINT, 0); // ^C character abort
		osMessageQueuePut(CDC_RxQueueId, &buf, 0, osWaitForever);
	}

}


