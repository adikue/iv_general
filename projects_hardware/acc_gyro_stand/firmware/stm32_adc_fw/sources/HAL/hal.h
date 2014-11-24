#ifndef HAL_H
#define HAL_H
#include "general_types.h"

extern uint8_t rs_started;
extern uint8_t spi_started;

/*-------------------------------------------------------------*/
/* Initialization section */

void init(void);
static void clock_init(void);//should not be used from outside. Use init()
static void gpio_init(void);//should not be used from outside. Use init()

/* END Initialization section */
/*-------------------------------------------------------------*/
/* Control section */
void led_on(uint32_t us);
void led_off(void);

//ADC control functions
extern void adc_hard_reset(void);
extern void adc_soft_reset(void);
extern int adc_check_connect(void);
extern void adc_get_next_ch_val(adc_value_t* adc_value);
extern int adc_write_config(adc_config_t config);

//SPI control functions
extern void spi_init(void);


//Timer control functions
void init_timer(timer_config_t conf);
void timer_config_struct_init(timer_config_t* config_struct);
void start_tmier(void);
void stop_timer(void);
uint32_t get_timestamp(void);

/* END Control section */

//MISC
void delay_us(uint32_t us);
void error_loop(error_t err);

#endif /*HAL_H*/
