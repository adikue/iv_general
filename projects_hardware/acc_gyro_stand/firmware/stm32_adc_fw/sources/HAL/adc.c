#include "adc.h"
#include "spi.h"
#include "stm32f4xx.h"

static adc_config_t factory_config = {  ADC_CONFIG0_DEF_VAL,
                                        ADC_CONFIG1_DEF_VAL,
                                        ADC_MUXSCH_DEF_VAL,
                                        ADC_MUXDIF_DEF_VAL,
                                        ADC_MUXSG0_DEF_VAL,
                                        ADC_MUXSG1_DEF_VAL,
                                        ADC_SYSRED_DEF_VAL,
                                        ADC_GPIOC_DEF_VAL,
                                        ADC_GPIOD_DEF_VAL};

adc_config_t current_config = { ADC_CONFIG0_DEF_VAL,
                                ADC_CONFIG1_DEF_VAL,
                                ADC_MUXSCH_DEF_VAL,
                                ADC_MUXDIF_DEF_VAL,
                                ADC_MUXSG0_DEF_VAL,
                                ADC_MUXSG1_DEF_VAL,
                                ADC_SYSRED_DEF_VAL,
                                ADC_GPIOC_DEF_VAL,
                                ADC_GPIOD_DEF_VAL};

static uint8_t adc_get_id(void){
    return spi_read_reg((uint8_t)ADC_ID_REG);
}

void adc_config_struct_init(adc_config_t *conf_struct){
    *conf_struct = factory_config;
}

void adc_hard_reset(void){
    //nRESET. PB8
    GPIOB->ODR &= ~GPIO_ODR_ODR_8;//Set nRESET line to low
    delay_us(1);
    GPIOB->ODR |= GPIO_ODR_ODR_8;//Set nRESET line to high
    delay_us(300);
    current_config = factory_config;
}

void adc_soft_reset(void){
    spi_reset();
    delay_us(100);
}


int adc_write_config(adc_config_t config){    
    //Write configuration into ADC
    spi_write_reg(ADC_CONFIG0_REG, config.config0_reg);
    spi_write_reg(ADC_CONFIG1_REG, config.config1_reg);
    spi_write_reg(ADC_MUXSCH_REG, config.muxsch_reg);
    spi_write_reg(ADC_MUXDIF_REG, config.muxdif_reg);
    spi_write_reg(ADC_MUXSG0_REG, config.muxsg0_reg);
    spi_write_reg(ADC_MUXSG1_REG, config.muxsg1_reg);
    spi_write_reg(ADC_SYSRED_REG, config.sysred_reg);
    spi_write_reg(ADC_GPIOC_REG, config.gpioc_reg);
    spi_write_reg(ADC_GPIOD_REG, config.gpiod_reg);
    
    current_config = config;
    
    return adc_check_config();
}

 int adc_check_config(void){

    if (spi_read_reg(ADC_CONFIG0_REG) != current_config.config0_reg)
        return -1;
    
    if (spi_read_reg(ADC_CONFIG1_REG) != current_config.config1_reg)
        return -1;
    
    if (spi_read_reg(ADC_MUXSCH_REG) != current_config.muxsch_reg)
        return -1;

    if (spi_read_reg(ADC_MUXDIF_REG) != current_config.muxdif_reg)
        return -1;

    if (spi_read_reg(ADC_MUXSG0_REG) != current_config.muxsg0_reg)
        return -1;

    if (spi_read_reg(ADC_MUXSG1_REG) != current_config.muxsg1_reg)
        return -1;
    
    if (spi_read_reg(ADC_SYSRED_REG) != current_config.sysred_reg)
        return -1;
    
    if (spi_read_reg(ADC_GPIOC_REG) != current_config.gpioc_reg)
        return -1;
    
    if (spi_read_reg(ADC_GPIOD_REG) != current_config.gpiod_reg)
        return -1;

    return 0;
}

int adc_check_connect(void){
    if (adc_get_id() != 0x8B)
        return -1;
    
    return 0;
}

void adc_get_next_ch_val(adc_value_t* adc_value){
    uint8_t buffer[4];
    uint32_t adc_val = 0;
    
    spi_pulse_convert();//Send command for convert next channel
    spi_read_channel(buffer);//Collect channel value
    
    adc_value->flag_new = buffer[0] >> 7;
    adc_value->ch_id = buffer[0] & 0x1F;
    
    adc_val = buffer[1] << 16 |
                      buffer[2] << 8 |
                      buffer[3];
    
    adc_value->data = adc_val * ADC_VOLTAGE_COEFF;
    
    return;
}


