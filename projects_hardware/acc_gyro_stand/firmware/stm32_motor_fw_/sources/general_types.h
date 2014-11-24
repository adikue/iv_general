#ifndef _GENERAL_TYPES_H
#define _GENERAL_TYPES_H

#include <stdint.h>

typedef enum{
    STM_OK_LED = 10,
    STM_STATUS_LED,
    SD_STATUS_LED
}stm_led_t;

typedef enum{
    NO_ERROR = 0,
    SD_ERROR = 1,
    STM_ERROR = 2,
    WIFI_ERROR = 3
}error_t;

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
    MOUNT_SD_CARD,
    CHECK_SD_CARD,
    READ_MODULE_CONFIG,
    WIFI_UP,
    WIFI_D_EXCHAGE,
    UNMOUNT_SD_CARD
}mcu_state_t;

typedef enum{
    VAR = 123,
    DISCRETE = 234
}param_bound_t;

typedef struct{
    char* pname;
    char* value;
    param_bound_t bound_type;
    char *restrictions[32];    
}cfg_parameter_t;

typedef struct{
    uint32_t stamp;
    double throttle;
}motortask_t;

#endif
