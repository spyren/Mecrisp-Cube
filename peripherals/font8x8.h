/*
 * font8x8.h
 *
 *  Created on: 02.01.2021
 *      Author: psi
 */

#ifndef INC_FONT8X8_H_
#define INC_FONT8X8_H_

int FONT8X8_getColumn(int ch, int column);
void FONT8X8_transposeGlyph(int ch, unsigned char* pattern);

#endif /* INC_FONT8X8_H_ */
