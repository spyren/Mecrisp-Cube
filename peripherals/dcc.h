/*
 * dcc.h
 *
 *  Created on: Apr 27, 2025
 *      Author: psi
 */

#ifndef DCC_H_
#define DCC_H_

#define DCC_MAX_PACKET_LENGTH 	20
#define DCC_MAX_LOCO_SLOTS		4

typedef struct DCC_LocoSlot_t {
	uint8_t 	state;
	uint16_t 	address;
	uint8_t		speed;
	uint8_t		direction;
	uint32_t	function;
} DCC_LocoSlot_t;

void DCC_init(void);
void DCC_start(void);
void DCC_stop(void);
void DCC_setState(int slot, int state);
int DCC_getState(int slot);
void DCC_setAddress(int slot, int address);
int DCC_getAddress(int slot);
void DCC_setSpeed(int slot, int speed);
int DCC_getSpeed(int slot);
void DCC_setDirection(int slot, int direction);
int DCC_getDirection(int slot);
void DCC_setFunction(int slot, int function);
void DCC_resetFunction(int slot, int function);
int DCC_getFunction(int slot);

void DCC_TIM16_PeriodElapsedIRQHandler();

#endif /* DCC_H_ */
