/*
 * clock.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#define RTC_MAGIC_COOKIE	(0xa5a5a5a5)

// Type Definitions
// ****************

/** Structure holding RTC backup registers preserved during reset (except POR/cold start) */
typedef struct {
	/** magic cookie if the RTC is already running                             (RTC_BKP0R)*/
	uint32_t        rtc;
	/** magic cookie if the watchdog has bitten                                (RTC_BKP1R)*/
	uint32_t        watchdog;
	/** where (address) the watchdog has bitten                                (RTC_BKP2R)*/
	uint32_t        watchdog_adr;
	/** watchdog bites                                                         (RTC_BKP3R)*/
	uint32_t        watchdog_bites;
	/** magic cookie if an assert occurred                                     (RTC_BKP4R)*/
	uint32_t        assert;
	/** assert count                                                           (RTC_BKP5R)*/
	uint32_t        assert_cnt;
	/** assert ID                                                              (RTC_BKP6R)*/
	uint32_t        assert_id;
	/** assert parameter e.g. address where the assert occurred                (RTC_BKP7R)*/
	uint32_t        assert_param;
} RTC_Backup_t;


// Constants
// *********

/** The preserved values are located in the RTC backup registers */
#define RTC_Backup         (*(RTC_Backup_t *)(&RTC->BKP0R))

void RTC_init(void);
uint32_t RTC_getTime(void);
int RTC_setTime(uint32_t timestamp);
uint64_t RTC_typeTime(uint64_t forth_stack);

#endif /* INC_RTC_H_ */
