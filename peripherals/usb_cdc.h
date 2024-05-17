/*
 * usb_cdc.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_USB_CDC_H_
#define INC_USB_CDC_H_

#define CDC_CONNECTED		0x01
#define CDC_DISCONNECTED	0x02

extern osMessageQueueId_t CDC_RxQueueId;
extern osThreadId_t CDC_ThreadID;

void CDC_init(void);
int CDC_getc(void);
int CDC_RxReady(void);
int CDC_putc(int c);
int CDC_TxReady(void);
int CDC_putkey(const char c);

#endif /* INC_USB_CDC_H_ */
