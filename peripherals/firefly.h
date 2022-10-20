/*
 * firefly.h
 *
 *  Created on: 14.10.2022
 *      Author: psi
 */

#ifndef FIREFLY_H_
#define FIREFLY_H_

#define firefly_width 56
#define firefly_height 64
static char firefly_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01,
  0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0x81, 0x03, 0x00, 0x00, 0x00, 0x38, 0x70, 0x00, 0x07, 0x00, 0x00, 0x00,
  0xFC, 0x38, 0x00, 0x0E, 0x00, 0x00, 0x00, 0xFC, 0x1B, 0x00, 0x0C, 0x00,
  0x00, 0x00, 0xFC, 0x1F, 0x00, 0x0C, 0x00, 0x00, 0x00, 0xFE, 0x3F, 0x00,
  0x18, 0x00, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFE,
  0x7F, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x18, 0x00, 0x00,
  0x00, 0xFE, 0xFF, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x01, 0x18,
  0x00, 0x00, 0x00, 0xFE, 0xFF, 0x01, 0x18, 0x00, 0x00, 0x00, 0xFE, 0xFF,
  0x01, 0x18, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x01, 0x18, 0x00, 0x00, 0x00,
  0xFC, 0xFF, 0x01, 0x1C, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x03, 0x0C, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0x03, 0x0E, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x03,
  0xC6, 0x0F, 0x00, 0x00, 0xF0, 0xFF, 0x03, 0xFE, 0x1F, 0x00, 0x00, 0xF0,
  0xFF, 0x01, 0xFF, 0x31, 0x00, 0x00, 0xE0, 0xFF, 0x81, 0xFF, 0x63, 0x00,
  0x00, 0xE0, 0xFF, 0xC1, 0xFF, 0x63, 0x00, 0x00, 0xC0, 0xFF, 0xE1, 0xFF,
  0x67, 0x00, 0x00, 0xC0, 0xFF, 0xF1, 0xFF, 0x67, 0x00, 0x00, 0x80, 0xFF,
  0xF9, 0xFF, 0x6F, 0x00, 0x00, 0x00, 0xFF, 0xFD, 0xFF, 0x3F, 0x00, 0x00,
  0x00, 0xFE, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x0F,
  0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0xFE, 0xFF,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0xFC, 0x00,
  0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x03, 0x7E, 0x00,
  0x00, 0xFE, 0xFF, 0x3F, 0x00, 0xF0, 0x01, 0x00, 0x00, 0xFF, 0x1F, 0x00,
  0xC0, 0x03, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0x07, 0x00, 0x00, 0xFE,
  0xFF, 0x01, 0x00, 0x0F, 0x00, 0x00, 0xF3, 0xFF, 0x03, 0x00, 0x1C, 0x00,
  0x00, 0x81, 0xFF, 0x0F, 0x00, 0x18, 0x00, 0x80, 0x81, 0xFF, 0x1F, 0x00,
  0x30, 0x00, 0xC0, 0x80, 0xFF, 0x3F, 0x00, 0x30, 0x00, 0x40, 0x00, 0xFF,
  0x7F, 0x00, 0x30, 0x00, 0x60, 0x00, 0xFF, 0xFF, 0x00, 0x30, 0x00, 0x20,
  0x00, 0xFF, 0xFF, 0x01, 0x30, 0x00, 0x30, 0x00, 0xFE, 0xFF, 0x01, 0x30,
  0x00, 0x10, 0x00, 0xFE, 0xFF, 0x03, 0x30, 0x00, 0x18, 0x00, 0xFE, 0xFF,
  0x03, 0x30, 0x00, 0x18, 0x00, 0xFC, 0xFF, 0x03, 0x38, 0x00, 0x08, 0x00,
  0xFC, 0xFF, 0x03, 0x1C, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x07, 0x0F, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0x8F, 0x07, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF,
  0x01, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x73, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x80, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, };


#endif /* FIREFLY_H_ */