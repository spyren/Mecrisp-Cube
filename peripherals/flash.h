/*
 * flash.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

void FLASH_init(void);
int FLASH_erasePage(uint32_t Address);
int FLASH_program32B(uint32_t Address, uint32_t* buffer);
#endif /* INC_FLASH_H_ */
