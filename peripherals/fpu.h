/*
 * fpu.h
 *
 *  Created on: 01.11.2022
 *      Author: psi
 */

#ifndef FPU_H_
#define FPU_H_

#define FPU_MAX_STR	40

void FPU_setFPSCR(uint32_t fpscr) ;
uint32_t FPU_getFPSCR(void);
float FPU_str2f(char *str, int len);
float FPU_sin(float arg);

#endif /* FPU_H_ */
