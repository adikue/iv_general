#ifndef __USART_H__
#define __USART_H__

#include "general_types.h"

int usart1_send(uint8_t data);

int usart1_recv(uint8_t* data);

void flush_rx_buff(void);

#endif
