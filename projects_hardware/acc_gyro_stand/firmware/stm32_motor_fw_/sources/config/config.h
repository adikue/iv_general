#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

#define SECOND_DELAY                1000000
#define MILSECOND_DELAY             1000

//#define HSE_VALUE                 16000000
#define PLL_M				        8
#define PLL_N				        192
#define PLL_P				        8
#define PLL_Q				        8
        
#define PWM_TIMER_CLK               1000000//
#define PWM_PULSE_MAX               0.002 * PWM_TIMER_CLK //2ms
#define PWM_PULSE_DEF               0.0015 * PWM_TIMER_CLK//1.5ms
#define PWM_PULSE_MIN               0.001 * PWM_TIMER_CLK//1ms
#define PWM_PERIOD                  0.02 * PWM_TIMER_CLK//20ms

#define RT_DATA_SAVE_PERIOD         10000//10000us = 10ms
        
#define USART1_BRR                  115200
#define USART1_BUFFER_SIZE          256
        
#define CONFIG_FILENAME             "motor_module_config.txt"
#define CONFIG_FILENAME_WR          "motor_module_config_wr.txt"
#define WF_SCRIPT_FILENAME          "wifi_config.txt"
#define MOTOR_SCRIPT_FILENAME       "motor_run_script.txt"
    
#define PARAMS_NUM                  2
#define NUM_FILES                   16
    
 //Enable Card detect pin   
 #define FATFS_USE_DETECT_PIN	    1
 
 //CD pin
 #define FATFS_USE_DETECT_PIN_RCC	RCC_AHB1Periph_GPIOC			
 #define FATFS_USE_DETECT_PIN_PORT	GPIOC
 #define FATFS_USE_DETECT_PIN_PIN	GPIO_Pin_13
 
 #define MOTORTASK_FIFOSIZE         256


#endif
