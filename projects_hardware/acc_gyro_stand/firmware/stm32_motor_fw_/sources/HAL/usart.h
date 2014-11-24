#ifndef __USART_H__
#define __USART_H__

#include "general_types.h"

int usart1_send(uint8_t data);

int usart_cmd_is_executed(void);

#endif
