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
	uint8_t 	active;
	uint16_t 	address;
	uint8_t		speed;
	uint8_t		direction;
	uint32_t	function;
} DCC_LocoSlot_t;

void DCC_init(void);
void DCC_TIM16_PeriodElapsedIRQHandler();

#endif /* DCC_H_ */
