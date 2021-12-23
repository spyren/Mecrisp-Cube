/*
 * terminal.h
 *
 *  Created on: 30.08.2020
 *      Author: psi
 */

#ifndef INC_TERMINAL_H_
#define INC_TERMINAL_H_

// Data Stack Pointer SPS is in R1 and Top Of Stack TOS is in R0 (first parameter).

uint64_t TERMINAL_emit(uint64_t forth_stack, char c);
uint64_t TERMINAL_key(uint64_t forth_stack, char *c);
uint64_t TERMINAL_qemit(uint64_t forth_stack, char *c);
uint64_t TERMINAL_qkey(uint64_t forth_stack, char *c);

void TERMINAL_redirect(void);
void TERMINAL_unredirect(void);


#endif /* INC_TERMINAL_H_ */
