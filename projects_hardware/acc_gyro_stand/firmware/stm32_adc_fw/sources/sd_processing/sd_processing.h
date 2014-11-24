#ifndef SD_DATA_H
#define SD_DATA_H

#include "general_types.h"
#include "tm_stm32f4_fatfs.h"

typedef enum{
    FILE_OPENED = 0,
    DRIVE_MOUNTED = 1,
    UNINITIALIZED = 2
}file_ops_status_t;

int mount_sd_card(void);
int check_sd_card_size(void);
void unmount_sd_card(void);

int open_log_file(uint8_t f_indx);
void close_log_files(void);
int log_data(adc_value_t data, uint32_t timestamp);
int read_config_from_file(char* filename, adc_config_t* conf);
int read_timer_setting_from_file(char* filename, timer_config_t* conf);


#endif
