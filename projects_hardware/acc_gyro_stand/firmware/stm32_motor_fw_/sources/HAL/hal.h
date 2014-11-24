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
void led_on(stm_led_t led, uint32_t us);
void led_off(stm_led_t led);

void TIM2_update_PWM(void);

void start_timer5(void);
void stop_timer5(void);
void reset_timer5(void);
uint32_t get_timer5_cnt(void);

int add_timer_task(motortask_t new_task);

int add_to_usart_tx(uint8_t data);
int usart_tx_isempty(void);

/* END Control section */

//MISC
void delay_us(uint32_t us);
void error_loop(error_t err);

#endif /*HAL_H*/
