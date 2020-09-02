/*
 * block.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_BLOCK_H_
#define INC_BLOCK_H_

#define BLOCK_BUFFER_COUNT			10
#define BLOCK_BUFFER_SIZE			1024

typedef struct {
	uint8_t Data[BLOCK_BUFFER_SIZE];
	int BlockNumber;  // -1 = Buffer unassigned
	uint8_t Current;
	uint8_t Updated;
} block_buffer_t;

extern block_buffer_t BLOCK_Buffers[];

void    BLOCK_init(void);
void    BLOCK_emptyBuffers(void);
void    BLOCK_update(void);
uint8_t *BLOCK_get(int block_number);
uint8_t *BLOCK_assign(int block_number);
void    BLOCK_saveBuffers(void);
void    BLOCK_flushBuffers(void);

#endif /* INC_BLOCK_H_ */
