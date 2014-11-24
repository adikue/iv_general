#include "hal.h"
#include "general_types.h"
#include "sd_processing.h"

#include "defines.h"
#include "tm_stm32f4_delay.h"
//#include "tm_stm32f4_fatfs.h"

uint8_t tries = 10;
uint16_t num = 100;
adc_config_t adc_conf;
timer_config_t tim_conf;

mcu_state_t program_state = INITIALIZING_MCU;

int main(){
    adc_value_t val;
    error_t reset_cause = NO_ERROR;

    
    while (1){//Switch between programm states in loop
        switch (program_state){
            
            case (INITIALIZING_MCU):
                init();
                TM_DELAY_Init();
            
                //If we reset MCU 10 times, smth goes wrong
                tries--;
                if (tries == 0)
                    error_loop(reset_cause);
                
                program_state = RESET_ADC;
                break;
            
            case (RESET_ADC):
                adc_soft_reset();
                adc_hard_reset();
            
                //If we reset MCU 10 times, smth goes wrong
                tries--;
                if (tries == 0)
                    error_loop(reset_cause);
                
                program_state = CHECK_ADC_CONNECTION;
                break;
            
            case (CHECK_ADC_CONNECTION):
                if( adc_check_connect() != 0 ){
                    reset_cause = ADC_ERROR;
                    program_state = RESET_ADC;
                }else
                    program_state = MOUNT_SD_CARD;
                break;
                
            case (MOUNT_SD_CARD):
                if (mount_sd_card() != 0 ){
                    reset_cause = SD_ERROR;
                    program_state = INITIALIZING_MCU;
                }else
                    program_state = CHECK_SD_CARD;
                break;
                
            case (CHECK_SD_CARD):
                if (check_sd_card_size() != 0 ){
                    reset_cause = SD_ERROR;
                    program_state = INITIALIZING_MCU;
                }else
                    program_state = READ_CONFIGS;
                break;
                
            case (READ_CONFIGS):
                if (read_timer_setting_from_file(TIMER_CONFIG_FILENAME, &tim_conf) != 0){
                    program_state = MOUNT_SD_CARD;
                    break;
                }
                if (read_config_from_file(ADC_CONFIG_FILENAME, &adc_conf) != 0){
                    program_state = MOUNT_SD_CARD;
                    break;
                }
                
                program_state = SET_CONFIGS;
                break;
            
            case (SET_CONFIGS):
                init_timer(tim_conf);
                
                if (adc_write_config(adc_conf) != 0){
                    reset_cause = ADC_ERROR;
                    program_state = RESET_ADC;
                }else
                    program_state = INITIALIZE_IS_OK;
                break;
                
            case (INITIALIZE_IS_OK):
                led_on(0);
                //At this point ADC ready for start
                program_state = ADC_PROCESSING;
                break;
            
            case (ADC_PROCESSING):
                for(num = 0; num < 100; num++){
                    adc_get_next_ch_val(&val);
                    if (open_log_file(val.ch_id) == 0)
                        log_data(val, get_timestamp());
                    delay_us(100);
                }
                close_log_files();
                program_state = UNMOUNT_SD_CARD;
                break;
                
           case (UNMOUNT_SD_CARD):
                unmount_sd_card(); 
           
                while(1){
                    led_on(15000);
                    delay_us(15000);
                    led_on(15000);
                    delay_us(15000);
                    led_on(15000);
                    
                    delay_us(100000);
                }       
                break;
           
           default:
               program_state = INITIALIZING_MCU;
               break;  
        }
    }   
     
       
    
}
