/*
 * flash.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

void FLASH_init(void);
int FLASH_programDouble(uint32_t Address, uint32_t word1, uint32_t word2);
int FLASH_erasePage(uint32_t Address);

#endif /* INC_FLASH_H_ */
