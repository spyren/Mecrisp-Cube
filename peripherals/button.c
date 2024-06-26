/**
 *  @brief
 *      Buffered calculator buttons
 *
 *      Columns are input ports with pull up resistors and interrupts,
 *      rows are open drain output ports.
 *
 *          C0       C1       C2       C3       C4
 *      +--------+--------+--------+--------+--------+
 *      | Pi     | 1/x    | x^2    | LOG    | LN     |
 *      | R/S    | y^x    | SQRT   | 10^x   | e^x    |  R0
 *      | A      | B      | C      | D      | E      |
 *      | R/S    | SL     | SR     | RL     | RR     |
 *      +--------+--------+--------+--------+--------+
 *      | STO    | RCL    | SIN    | COS    | TAN    |
 *      | CEIL   | FLOOR  | ASIN   | ACOS   | ATAN   |  R1
 *      | STO    | RCL    |        |        | F      |
 *      |        | SLn    | SRn    |        |        |
 *      +--------+--------+--------+--------+--------+
 *      | ENTER  | SWAP   | -      | E      | BS     |
 *      | DUP    | ROT    | ABS    | F>S    | CLx    |  R2
 *      | ENTER  | SWAP   | -      | E      | BS     |
 *      | DUP    | ROT    | ABS    | S>F    | CLx    |
 *      +--------+--------+--------+--------+--------+
 *      | CLOCK  | 7      | 8      | 9      | /      |
 *      | FLOAT  | HEX    | DEC    | OCT    | BIN    |  R3
 *      | CLOCK  | 7      | 8      | 9      | /      |
 *      | FLOAT  | HEX    | DEC    | OCT    | BIN    |
 *      +--------+--------+--------+--------+--------+
 *      | CALC   | 4      | 5      | 6      | *      |
 *      | TERM   | FIX    | ENG    | SCI    | PREC   |  R4
 *      | CALC   | 4      | 5      | 6      | *      |
 *      | TERM   | SIGN   | UNSGN  |        |        |
 *      +--------+--------+--------+--------+--------+
 *      | f      | 1      | 2      | 3      | -      |
 *      | f      | p      | n      | u      | m      |  R5
 *      | f      | 1      | 2      | 3      | -      |
 *      | f      | XOR    | AND    | NOT    | OR     |
 *      +--------+--------+--------+--------+--------+
 *      | C ON   | 0      | .      | FFCT   | +      |
 *      | OFF    | k      | M      | G      | T      |  R6
 *      | C ON   | 0      | .      | FCT    | +      |
 *      | OFF    | /      | MOD    | /MOD   |        |
 *      +--------+--------+--------+--------+--------+
 *
 *  @file
 *      button.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-05-27
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
#include "stm32_lpm_if.h"
#include "stm32_lpm.h"

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "button.h"
#include "myassert.h"
#include "bsp.h"
#include "usb_cdc.h"
#include "oled.h"
#include "tiny.h"

#if BUTTON == 1


// Buffer Length
// **************
#define BUTTON_BUFFER_LENGTH	(100)

#define BUTTON_CHAR_RECEIVED	0x01
#define KEYS	6
#define BUTTON_PRESSED		GPIO_PIN_RESET
#define BUTTON_RELEASED		GPIO_PIN_SET

#define DEBOUNCE_TIME		20							// 20 ms

#define BUTTON_COLUMN_COUNT	5
#define BUTTON_ROW_COUNT	7
#define BUTTON_COUNT		(BUTTON_COLUMN_COUNT * BUTTON_ROW_COUNT)

#define BUTTON_CLOCK		15
#define BUTTON_CALC			20
#define BUTTON_SHIFT		25
#define BUTTON_ON_OFF		30

#define BUTTON_FLOAT		15
#define BUTTON_HEX			16
#define BUTTON_DEC			17
#define BUTTON_OCT			18
#define BUTTON_BIN			19

#define BUTTON_FIX			21
#define BUTTON_ENG			22
#define BUTTON_SCI			23

#define BUTTON_VIEW			33

enum float_format {FIX, ENG, SCI} ;

// Private function prototypes
// ***************************
static void BUTTON_Thread(void *argument);
static void put_key_string(uint8_t c);

// Global Variables
// ****************

// Locale Variables
// ****************
static uint8_t startup = TRUE;

// Hardware resources
// ******************

// RTOS resources
// **************

// Definitions for BUTTON thread
static osThreadId_t BUTTON_ThreadId;
static const osThreadAttr_t BUTTON_ThreadAttr = {
		.name = "BUTTON",
		.priority = (osPriority_t) osPriorityLow,
		.stack_size = 512 * 2
};

osMutexId_t BUTTON_MutexID;
const osMutexAttr_t BUTTON_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

osSemaphoreId_t BUTTON_SemaphoreID;

// Definitions for ButtonQueue
static osMessageQueueId_t BUTTON_QueueId;
static const osMessageQueueAttr_t button_Queue_attributes = {
		.name = "BUTTON_Queue"
};


// Private Variables
// *****************
typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} PortPin_t;

static const PortPin_t PortPinColumn_a[BUTTON_COLUMN_COUNT] = {
		{ COL0_GPIO_Port,    COL0_Pin } ,
		{ COL1_GPIO_Port,    COL1_Pin } ,
		{ COL2_GPIO_Port,    COL2_Pin } ,
		{ COL3_GPIO_Port,    COL3_Pin } ,
		{ COL4_GPIO_Port,    COL4_Pin } ,
};

static const PortPin_t PortPinRow_a[BUTTON_ROW_COUNT] = {
		{ ROW0_GPIO_Port,    ROW0_Pin } ,
		{ ROW1_GPIO_Port,    ROW1_Pin } ,
		{ ROW2_GPIO_Port,    ROW2_Pin } ,
		{ ROW3_GPIO_Port,    ROW3_Pin } ,
		{ ROW4_GPIO_Port,    ROW4_Pin } ,
		{ ROW5_GPIO_Port,    ROW5_Pin } ,
		{ ROW6_GPIO_Port,    ROW6_Pin } ,
};

// float keyboard
static const char  *keyboard[BUTTON_COUNT] = {
		" pi\n", 			// r0, c0 PiBUTTON_FLOAT
		" 1e0 swap f/\n", 	// r0, c1 1/x
		" dup f*\n", 		// r0, c2 x^2
		" flog\n", 			// r0, c3 log
		" fln\n", 			// r0, c4 ln

		" sto\n", 			// r1, c0 STO
		" rcl\n", 			// r1, c1 RCL
		" fsin\n", 			// r1, c2 SIN
		" fcos\n", 			// r1, c3 COS
		" ftan\n", 			// r1, c4 TAN

		"\n", 				// r2, c0 ENTER
		" swap\n", 			// r2, c1 SWAP x<>y
		"-", 		        // r2, c2 -
		"e", 				// r2, c3 E
		"\b", 				// r2, c4 BS

		" clock\n", 		// r3, c0 CLOCK
		"7", 				// r3, c1 7
		"8", 				// r3, c2 8
		"9", 				// r3, c3 9
		" f/\n", 			// r3, c4 /

		" calc\n", 			// r4, c0 CALC
		"4", 				// r4, c1 4
		"5", 				// r4, c2 5
		"6", 				// r4, c3 6
		" f*\n", 			// r4, c4 *

		"", 				// r5, c0 f
		"1", 				// r5, c1 1
		"2", 				// r5, c2 2
		"3", 				// r5, c3 3
		" f-\n", 			// r5, c4 -

		" quit\n\n", 		// r6, c0 C
		"0", 				// r6, c1 0
		".", 				// r6, c2 .
		" ffct\n", 			// r6, c3 FFCT
		" f+\n", 			// r6, c4 +
};

// float shift keyboard
static const char* keyboard_f[BUTTON_COUNT] = {
		" r/s\n", 			// r0, c0 R/S
		" f**\n", 			// r0, c1 y^x
		" fsqrt\n", 		// r0, c2 SQRT
		" 10e f**\n", 		// r0, c3 10^x
		" fexp\n", 			// r0, c4 e^x

		" fceil\n", 		// r1, c0 CEIL
		" ffloor\n", 		// r1, c1 FLOOR
		" fasin\n", 		// r1, c2 ASIN
		" facos\n", 		// r1, c3 ACOS
		" fatan\n", 		// r1, c4 ATAN

		" dup\n", 			// r2, c0 DUP
		" rot\n", 			// r2, c1 ROT
		" fabs\n", 			// r2, c2 ABS
		" f>s\n", 			// r2, c3 F>S
		" drop\n", 		    // r2, c4 CLx

		" decimal float\n", // r3, c0 FLOAT
		" hex int\n",		// r3, c1 HEX
		" decimal int\n", 	// r3, c2 DEC
		" octal int\n", 	// r3, c3 OCT
		" binary int\n", 	// r3, c4 BIN

		" term\n", 			// r4, c0 TERM
		" fix\n",			// r4, c1 FIX
		" eng\n", 			// r4, c2 ENG
		" sci\n",			// r4, c3 SCI
		" set-precision\n", // r4, c4 PREC

		"", 				// r5, c0 f
		"p", 				// r5, c1 p
		"n", 				// r5, c2 n
		"u", 				// r5, c3 u
		"m", 				// r5, c4 m

		" onoff\n",		    // r6, c0 off
		"k", 				// r6, c1 k
		"M", 				// r6, c2 M
		"G", 				// r6, c3 G
		"T", 				// r6, c4 T
};

static const char* keyboard_int[BUTTON_COUNT] = {
		"A", 				// r0, c0 A
		"B", 				// r0, c1 B
		"C", 				// r0, c2 C
		"D", 				// r0, c3 D
		"E", 				// r0, c4 E

		" sto\n", 			// r1, c0 STO
		" rcl\n", 			// r1, c1 RCL
		"", 				// r1, c2
		"", 				// r1, c3
		"F", 				// r1, c4 F

		"\n", 				// r2, c0 ENTER
		" swap\n", 			// r2, c1 SWAP x<>y
		"-", 				// r2, c2 -
		"e", 				// r2, c3 E
		"\b", 				// r2, c4 BS

		" clock\n", 		// r3, c0 CLOCK
		"7", 				// r3, c1 7
		"8", 				// r3, c2 8
		"9", 				// r3, c3 9
		" /\n", 			// r3, c4 /

		" calc\n", 			// r4, c0 CALC
		"4", 				// r4, c1 4
		"5", 				// r4, c2 5
		"6", 				// r4, c3 6
		" *\n", 			// r4, c4 *

		"", 				// r5, c0 f
		"1", 				// r5, c1 1
		"2", 				// r5, c2 2
		"3", 				// r5, c3 3
		" -\n", 			// r5, c4 -

		"quit\n\n",			// r6, c0 C
		"0", 				// r6, c1 0
		".", 				// r6, c2 .
		" fct\n", 			// r6, c3 FCT
		" +\n", 			// r6, c4 +
};

static const char* keyboard_int_f[BUTTON_COUNT] = {
		" r/s\n",			// r0, c0 R/S
		" shl\n", 			// r0, c1 SL
		" shr\n", 			// r0, c2 SR
		" rol\n", 			// r0, c3 RL
		" ror\n", 			// r0, c4 RR

		"", 				// r1, c0
		" lshift\n", 		// r1, c1 SLn
		" rshift\n", 		// r1, c2 SRn
		"", 				// r1, c3
		"", 				// r1, c4

		" dup\n", 			// r2, c0 DUP
		" rot\n", 			// r2, c1 ROT
		" abs\n", 			// r2, c2 ABS
		" s>f\n", 			// r2, c3 S>F
		" drop\n", 		    // r2, c4 CLx

		" float\n", 		// r3, c0 FLOAT
		" hex int\n",		// r3, c1 HEX
		" decimal int\n", 	// r3, c2 DEC
		" octal int\n", 	// r3, c3 OCT
		" binary int\n", 	// r3, c4 BIN

		" term\n", 			// r4, c0 TERM
		" sgn\n", 			// r4, c1 SIGN
		" unsgn\n", 		// r4, c2 UNSGN
		"", 				// r4, c3
		"", 				// r4, c4

		"", 				// r5, c0 f
		" xor\n", 			// r5, c1 XOR
		" and\n", 			// r5, c2 AND
		" not\n", 			// r5, c3 NOT
		" or\n", 			// r5, c4 OR

		" onoff\n", 		// r6, c0 OFF
		" */\n", 			// r6, c1 */
		" mod\n", 			// r6, c2 MOD
		" /mod\n", 			// r6, c3 /MOD
		"", 				// r6, c4
};

uint8_t button_state[BUTTON_COUNT];

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the BUTTON.			strcpy(str, keyboard_f[c]);
 *
 *  @return
 *      None
 */
void BUTTON_init(void) {
	int i;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
	for (i=0; i<BUTTON_COLUMN_COUNT; i++) {
	    GPIO_InitStruct.Pin = PortPinColumn_a[i].pin;
	    HAL_GPIO_Init(PortPinColumn_a[i].port, &GPIO_InitStruct);
	}

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
	for (i=0; i<BUTTON_ROW_COUNT; i++) {
		HAL_GPIO_WritePin(PortPinRow_a[i].port, PortPinRow_a[i].pin, GPIO_PIN_SET);
	    GPIO_InitStruct.Pin = PortPinRow_a[i].pin;
	    HAL_GPIO_Init(PortPinRow_a[i].port, &GPIO_InitStruct);
	}

	BUTTON_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (BUTTON_SemaphoreID == NULL) {
		Error_Handler();
	}

	BUTTON_MutexID = osMutexNew(&BUTTON_MutexAttr);
	if (BUTTON_SemaphoreID == NULL) {
		Error_Handler();
	}

	// Create the queue(s)
	// creation of  buttonQueue
	BUTTON_QueueId = osMessageQueueNew(BUTTON_BUFFER_LENGTH, sizeof(uint8_t),
			&button_Queue_attributes);
	ASSERT_fatal(BUTTON_QueueId != NULL, ASSERT_QUEUE_CREATION, __get_PC());

	// creation of BUTTON_Thread
	BUTTON_ThreadId = osThreadNew(BUTTON_Thread, NULL, &BUTTON_ThreadAttr);
	ASSERT_fatal(BUTTON_ThreadId != NULL, ASSERT_THREAD_CREATION, __get_PC());

	HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}


/**
 *  @brief
 *      Reset (flush) the BUTTON queue
 *  @return
 *      None
 */
void BUTTON_reset(void) {
	osMessageQueueReset(BUTTON_QueueId);
}


/**
 *  @brief
 *		Reads a char from the BUTTON
 *
 *		Blocking until char is ready.
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int BUTTON_getc(void) {
	uint8_t c;
	if (osMessageQueueGet(BUTTON_QueueId, &c, NULL, osWaitForever) == osOK) {
		return c;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**
 *  @brief
 *		There is a character in the queue (key pressed).
 *  @return
 *		TRUE if a character has been received.
 */

int BUTTON_Ready(void) {
	if (osMessageQueueGetCount(BUTTON_QueueId) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/**
 *  @brief
 *      Writes a char direct into the key queue.
 *
 *      Handy for testing and redirecting.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int BUTTON_putkey(const char c) {
	osStatus_t status;

	if (c == '\r') {
		// eat CR
		return 0;
	}
	status = osMessageQueuePut(BUTTON_QueueId, &c, 0, osWaitForever);
	if (status == osOK) {
		return 0;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**	uint8_t startup = TRUE;
 *
 *  @brief
 *      Go into stop mode till on-button is pressed
 *
 *  @return
 *      None
 */
void BUTTON_OnOff(void) {
	int row;

	OLED_off();

	osMutexAcquire(BUTTON_MutexID, 100);
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

	// deactivate all rows except the last one
	for (row=0; row<(BUTTON_ROW_COUNT-1); row++) {
		HAL_GPIO_WritePin(PortPinRow_a[row].port, PortPinRow_a[row].pin, GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(PortPinRow_a[BUTTON_ROW_COUNT-1].port,
			          PortPinRow_a[BUTTON_ROW_COUNT-1].pin, GPIO_PIN_RESET);

	// wait till off-button is released
	while (HAL_GPIO_ReadPin(PortPinColumn_a[0].port,  PortPinColumn_a[0].pin) == BUTTON_PRESSED) {
		osDelay(10);
	}
	osDelay(10);

	// wait till on-button is pressed
	while (HAL_GPIO_ReadPin(PortPinColumn_a[0].port,  PortPinColumn_a[0].pin) != BUTTON_PRESSED) {
		osDelay(10);
	}

	// wait till off-button is released
	while (HAL_GPIO_ReadPin(PortPinColumn_a[0].port,  PortPinColumn_a[0].pin) == BUTTON_PRESSED) {
		osDelay(10);
	}
	osDelay(10);

	OLED_on();

	// activate all rows
	for (row=0; row<BUTTON_ROW_COUNT; row++) {
		HAL_GPIO_WritePin(PortPinRow_a[row].port, PortPinRow_a[row].pin, GPIO_PIN_RESET);
	}
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	osMutexRelease(BUTTON_MutexID);

}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the BUTTON thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	NoneBUTTON_FLOAT
  */
static void BUTTON_Thread(void *argument) {
	int button;
	int column, row;
	uint8_t new_state;

	for (button=0; button < BUTTON_COUNT; button++) {
		button_state[button] = BUTTON_RELEASED;
	}

	// Infinite loop
	for(;;) {
		// activate all rows
		osMutexAcquire(BUTTON_MutexID, osWaitForever);
		for (row=0; row<BUTTON_ROW_COUNT; row++) {
			HAL_GPIO_WritePin(PortPinRow_a[row].port, PortPinRow_a[row].pin, GPIO_PIN_RESET);
		}
		osMutexRelease(BUTTON_MutexID);

		// wait for button event
		osSemaphoreAcquire(BUTTON_SemaphoreID, osWaitForever);
		osDelay(10);	// wait for debouncing

		osMutexAcquire(BUTTON_MutexID, osWaitForever);

		// deactivate all rows
		for (row=0; row<BUTTON_ROW_COUNT; row++) {
			HAL_GPIO_WritePin(PortPinRow_a[row].port, PortPinRow_a[row].pin, GPIO_PIN_SET);
		}

		button = 0;
		for (row=0; row < BUTTON_ROW_COUNT; row++) {
			// set a row
			HAL_GPIO_WritePin(PortPinRow_a[row].port, PortPinRow_a[row].pin, GPIO_PIN_RESET);
			osDelay(1);
			for (column=0; column < BUTTON_COLUMN_COUNT; column++) {
				// read column
				new_state = HAL_GPIO_ReadPin(PortPinColumn_a[column].port,  PortPinColumn_a[column].pin);
				if (new_state != button_state[button]) {
					// button state has been changed
					if (new_state == BUTTON_PRESSED) {
						put_key_string(button);
					}
					button_state[button] = new_state;
				}
				button++;
			}

			HAL_GPIO_WritePin(PortPinRow_a[row].port, PortPinRow_a[row].pin, GPIO_PIN_SET);
		}

		osMutexRelease(BUTTON_MutexID);

	}
}


/**
 *  @brief
 *      Writes calculator key string into the key queue.
 *  @param[in]
 *      c  char to write (0 to 34)
 *  @return
 *      Return EOF on error, 0 on success.
 */
static void put_key_string(uint8_t c) {
	char str[20];
	int i;

	static int shift = FALSE;
	static int mode_float = TRUE;

	if (startup) {
		switch (c) {
		case BUTTON_ON_OFF:
			// fallthru
		case BUTTON_CLOCK:
			// fallthru
			break;
		case BUTTON_CALC:
			startup = FALSE;
			break;
		default:
			// ignore
			return;
		}
	}

	if (c == BUTTON_SHIFT) {
		shift = !shift;
	}

	if (shift) {
		switch (c) {
		case BUTTON_FLOAT:
			mode_float = TRUE;
			break;
		case BUTTON_HEX:
			// fall through
		case BUTTON_DEC:
			// fall through
		case BUTTON_OCT:
			// fall through
		case BUTTON_BIN:
			mode_float = FALSE;
			break;
		}
		if (mode_float) {
			strcpy(str, keyboard_f[c]);
		} else {
			strcpy(str, keyboard_int_f[c]);
		}
		if (c != BUTTON_SHIFT) {
			shift = FALSE;
		}
	} else {
		// not shifted
		if (mode_float) {
			strcpy(str, keyboard[c]);
		} else {
			// integer
			strcpy(str, keyboard_int[c]);
		}
	}
	if (strlen(str) == 1) {
		// a single letter
		osMessageQueuePut(CDC_RxQueueId, &str[0], 0, osWaitForever);
		if (str[0] == 8) {
			// backspace
			OLED_putc(8);
			OLED_putc(' ');
			OLED_putc(8);
		} else if (str[0] == '\n' || str[0] == '\r') {
			; // eat CR LF
		} else {
			OLED_putc(str[0]);
		}
	} else {
		for (i=0; i<strlen(str); i++) {
			osMessageQueuePut(CDC_RxQueueId, &str[i], 0, osWaitForever);
		}
	}
}

#endif

