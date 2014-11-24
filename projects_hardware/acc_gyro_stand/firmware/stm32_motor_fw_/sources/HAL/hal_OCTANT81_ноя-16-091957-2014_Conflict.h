#ifndef HAL_H
#define HAL_H
#include "general_types.h"
/*-------------------------------------------------------------*/
/* Initialization section */

void init(void);

/* END Initialization section */
/*-------------------------------------------------------------*/
/* Control section */
//LED control functions
void led_on(led_t led, uint32_t us);
void led_off(led_t led);

void TIM2_update_PWM(void);

void start_timer5(void);
void stop_timer5(void);
void reset_timer5(void);

int add_to_usart_tx(uint8_t data);
int get_from_usart_rx(uint8_t* data);
void flush_usart_rx(void);


/* END Control section */

//MISC
void delay_us(uint32_t us);
void error_loop(error_t err);

#endif /*HAL_H*/
