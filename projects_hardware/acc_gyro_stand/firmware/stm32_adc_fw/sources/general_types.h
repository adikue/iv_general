#ifndef _GENERAL_TYPES_H
#define _GENERAL_TYPES_H

#include <stdint.h>

typedef struct{
    uint8_t flag_new;
    uint16_t ch_id;
    double data;
}adc_value_t;

typedef enum{
    NO_ERROR = 0,
    SD_ERROR = 1,
    ADC_ERROR = 2
}error_t;

typedef struct{
    uint8_t config0_reg;
    uint8_t config1_reg;
    uint8_t muxsch_reg;
    uint8_t muxdif_reg;
    uint8_t muxsg0_reg;
    uint8_t muxsg1_reg;
    uint8_t sysred_reg;
    uint8_t gpioc_reg;
    uint8_t gpiod_reg;
}adc_config_t;

typedef enum{
    TIMER_MS_SCALE = 0,
    TIMER_US_SCALE = 1
}timer_scale_t;

typedef struct{
    uint16_t        period;
    timer_scale_t   scale;
}timer_config_t;

typedef enum{
    INITIALIZING_MCU,
    RESET_ADC,
    CHECK_ADC_CONNECTION,
    MOUNT_SD_CARD,
    CHECK_SD_CARD,
    READ_CONFIGS,
    SET_CONFIGS,
    INITIALIZE_IS_OK,
    ADC_PROCESSING,
    UNMOUNT_SD_CARD
}mcu_state_t;

#endif
