#include "usart.h"
#include "hal.h"

int usart1_send(uint8_t data){
    return add_to_usart_tx(data);
}

int usart1_recv(uint8_t* data){
    return get_from_usart_rx(data);
}

void flush_rx_buff(void){
    flush_usart_rx();
}
