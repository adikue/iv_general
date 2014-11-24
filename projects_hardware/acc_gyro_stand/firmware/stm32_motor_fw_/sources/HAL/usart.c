#include "usart.h"
#include "hal.h"

int usart1_send(uint8_t data){
    return add_to_usart_tx(data);
}

int usart_cmd_is_executed(void){
    return usart_tx_isempty();
}
