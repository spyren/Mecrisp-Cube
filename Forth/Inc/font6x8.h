/*
 * font6x8.h
 *
 *  Created on: 02.01.2021
 *      Author: psi
 */

#ifndef INC_FONT6X8_H_
#define INC_FONT6X8_H_

int FONT6X8_getColumn(int ch, int column);
void FONT6X8_transposeGlyph(int ch, unsigned char* pattern);

#endif /* INC_FONT6X8_H_ */
