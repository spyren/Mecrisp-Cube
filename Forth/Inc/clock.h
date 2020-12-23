/*
 * clock.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

void RTC_init(void);
uint32_t RTC_getTime(void);
int RTC_setTime(uint32_t timestamp);
uint64_t RTC_typeTime(uint64_t forth_stack);

#endif /* INC_RTC_H_ */
