/*
 * dotstar.h
 *
 *  Created on: 04.12.2022
 *      Author: psi
 */

#ifndef DOTSTAR_H_
#define DOTSTAR_H_

void DOTSTAR_init(void);
void DOTSTAR_setPixels(uint32_t *buffer, uint32_t len);
void DOTSTAR_setPixel(uint32_t irgb);

#endif /* DOTSTAR_H_ */
