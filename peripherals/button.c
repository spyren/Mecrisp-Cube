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
 *      |        | y^x    | SQRT   | 10^x   | e^x    |  R0
 *      | A      | B      | C      | D      | E      |
 *      | 0x20   | 0x21 ! | 0x22 " | 0x23 # | 0x24 $ |
 *      +--------+--------+--------+--------+--------+
 *      | STOx   | RCLx   | SIN    | COS    | TAN    |
 *      | CEIL   | FLOOR  | ASIN   | ACOS   | ATAN   |  R1
 *      |        |        | SL     | SR     | F      |
 *      | 0x25 % | 0x26 & | 0x27 ' | 0x28 ( | 0x29 ) |
 *      +--------+--------+--------+--------+--------+
 *      | ENTER  | SWAP   | NEG    | EXP    | BS     |
 *      | DROP   | ROT    | ABS    | F>S    | CLx    |  R2
 *      |        |        |        | S>F    |        |
 *      | 0x2A * | 0x2B + | 0x2C , | 0x2D _ | 0x2E . |
 *      +--------+--------+--------+--------+--------+
 *      | CLOCK  | 7      | 8      | 9      | /      |
 *      |        | FLOAT  | DEC    | HEX    | BIN    |  R3
 *      |        |        |        |        |        |
 *      | 0x2F / | 0x30 0 | 0x31 1 | 0x32 2 | 0x33 3 |
 *      +--------+--------+--------+--------+--------+
 *      | CALC   | 4      | 5      | 6      | *      |
 *      |        | FIX    | ENG    | SCI    | PREC   |  R4
 *      |        |        |        |        |        |
 *      | 0x34 4 | 0x35 5 | 0x36 6 | 0x37 7 | 0x38 8 |
 *      +--------+--------+--------+--------+--------+
 *      | f      | 1      | 2      | 3      | -      |
 *      |        | p      | n      | u      | m      |  R5
 *      |        | XOR    | AND    | NOT    | OR     |
 *      | 0x39 9 | 0x3A : | 0x3B ; | 0x3C < | 0x3D = |
 *      +--------+--------+--------+--------+--------+
 *      | ON/OFF | 0      | .      |        | +      |
 *      |        | k      | M      | G      | T      |  R6
 *      |        | 1'S    | 2'S    | UNSGN  |        |
 *      | 0x3E > | 0x3F ? | 0x40 @ | 0x41 A | 0x42 B |
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

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "button.h"
#include "myassert.h"
#include "bsp.h"
#include "usb_cdc.h"
#include "oled.h"

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

#define BUTTON_FLOAT		16
#define BUTTON_DEC			17
#define BUTTON_HEX			18
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
		" pi\n", 			// 0, r0, c0 Pi
		" 1e0 swap f/\n", 	// 1, r0, c1 1/x
		" dup f*\n", 		// 2, r0, c2 x^2
		" flog\n", 			// 3, r0, c3 log
		" fln\n", 			// 4, r0, c4 ln

		"", 				// 5, r1, c0 STO
		"", 				// 6, r1, c1 RCL
		" fsin\n", 			// 7, r1, c2 SIN
		" fcos\n", 			// r1, c3 COS
		" ftan\n", 			// r1, c4 TAN

		"\n", 				// r2, c0 ENTER
		" swap\n", 			// r2, c1 SWAP x<>y
		" fnegate\n", 		// r2, c2 NEG
		"e", 				// r2, c3 EXP
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

		"", 				// r6, c0 on/off
		"0", 				// r6, c1 0
		".", 				// r6, c2 .
		" dup fm.\n", 		// r6, c3 FCT
		" f+\n", 			// r6, c4 +
};

// float shift keyboard
static const char* keyboard_f[BUTTON_COUNT] = {
		"", 				// r0, c0
		" f**\n", 			// r0, c1 y^x7
		" fsqrt\n", 		// r0, c2 SQRT
		" 10**>f\n", 		// r0, c3 10^x
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

		" reset\n", 		// r3, c0 EXIT
		" float\n",			// r3, c1 FLOAT
		" decimal int\n", 	// r3, c2 DEC
		" hex int\n", 		// r3, c3 HEX
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

		"", 				// r6, c0 on/off ^C
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

		"", 				// r1, c0
		"", 				// r1, c1
		" shl\n", 			// r1, c2 SL
		" shr\n", 			// r1, c3 SR
		"F", 				// r1, c4 F

		"\n", 				// r2, c0 ENTER
		" swap\n", 			// r2, c1 SWAP x<>y
		" negate\n", 		// r2, c2 NEG
		"", 				// r2, c3 EXP
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

		"", 				// r6, c0 on/off
		"0", 				// r6, c1 0
		".", 				// r6, c2 .
		" dup fm.\n", 		// r6, c3 FCT
		" +\n", 			// r6, c4 +
};

static const char* keyboard_int_f[BUTTON_COUNT] = {
		"A", 				// r0, c0 A
		"B", 				// r0, c1 B
		"C", 				// r0, c2 C
		"D", 				// r0, c3 D
		"E", 				// r0, c4 E

		"", 				// r1, c0
		"", 				// r1, c1
		" shl\n", 			// r1, c2 SL
		" shr\n", 			// r1, c3 SR
		"F", 				// r1, c4 F

		" dup\n", 			// r2, c0 DUP
		" rot\n", 			// r2, c1 ROT
		" abs\n", 			// r2, c2 ABS
		" s>f\n", 			// r2, c3 S>F
		" drop\n", 		    // r2, c4 CLx

		" reset\n", 		// r3, c0 EXIT
		" float\n", 		// r3, c1 FLOAT
		" decimal int\n", 	// r3, c2 DEC
		" hex int\n", 		// r3, c3 HEX
		" binary int\n", 	// r3, c4 BIN

		"", 				// r4, c0
		"", 				// r4, c1
		"", 				// r4, c2
		"", 				// r4, c3
		"", 				// r4, c4

		"", 				// r5, c0 f
		" xor\n", 			// r5, c1 XOR
		" and\n", 			// r5, c2 AND
		" not\n", 			// r5, c3 NOT
		" or\n", 			// r5, c4 OR

		"", // r6, c0
		"", // r6, c1
		"", // r6, c2
		"", // r6, c3
		"", // r6, c4
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


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the BUTTON thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
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
		for (row=0; row<BUTTON_ROW_COUNT; row++) {
			HAL_GPIO_WritePin(PortPinRow_a[row].port, PortPinRow_a[row].pin, GPIO_PIN_RESET);
		}

		// wait for button event
		osSemaphoreAcquire(BUTTON_SemaphoreID, osWaitForever);
		osDelay(10);	// wait for debouncing

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
	static enum float_format float_f = ENG;

	switch (c) {
	case BUTTON_SHIFT:
		shift = !shift;
		break;
	case BUTTON_ON_OFF:
		break;
	case BUTTON_CLOCK:
		break;
	case BUTTON_CALC:
		break;
	}

	if (shift) {
		switch (c) {
		case BUTTON_FLOAT:
			mode_float = TRUE;
			break;
		case BUTTON_DEC:
			// fall through
		case BUTTON_HEX:
			// fall through
		case BUTTON_BIN:
			mode_float = FALSE;
			break;
		case BUTTON_FIX:
			float_f = FIX;
			break;
		case BUTTON_ENG:
			float_f = ENG;
			break;
		case BUTTON_SCI:
			float_f = SCI;
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
		if (mode_float) {
			if (c == BUTTON_VIEW) {
				switch (float_f) {
				case FIX:
					strcpy(str, " dup f.\n");
					break;
				case ENG:
					strcpy(str, " dup fm.\n");
					break;
				case SCI:
					strcpy(str, " dup fs.\n");
					break;
				}
			} else {
				strcpy(str, keyboard[c]);
			}
		} else {
			// integer
			if (c == BUTTON_VIEW) {
				strcpy(str, " dup .\n");
			} else {
				strcpy(str, keyboard_int[c]);
			}
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
		} else {
			OLED_putc(str[0]);
		}
	} else {
		for (i=0; i<strlen(str); i++) {
			//		osMessageQueuePut(BUTTON_QueueId, &str[i], 0, osWaitForever);
			osMessageQueuePut(CDC_RxQueueId, &str[i], 0, osWaitForever);
		}
	}
}

#endif

