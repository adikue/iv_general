#include "sd_processing.h"
#include "adc.h"
#include "hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static FATFS FatFs;
static FIL adc_config_file;
static FIL timer_config_file;
static file_ops_status_t FS_STAT = UNINITIALIZED;
static uint8_t data_files_opened[29] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static FIL data_files[29];
    
char data_filenames[29][17] = DATA_FILENAMES;
    
int mount_sd_card(void){
    //Mount drive
    if (f_mount(&FatFs, "", 1) != FR_OK)
        return -1;
    
    return 0;
}

int check_sd_card_size(void){
    //Free and total space
    uint32_t total, free;
    
    //Get dirve size and free space
    if(TM_FATFS_DriveSize(&total, &free) != FR_OK)
        return -1;
    
    FS_STAT = DRIVE_MOUNTED;
    return 0;
}

void unmount_sd_card(){
    //Unmount drive
    f_mount(0, "", 1);
    FS_STAT = UNINITIALIZED;
}

int open_log_file(uint8_t f_indx){
    if (FS_STAT > DRIVE_MOUNTED)
        return -1;
    
    if (f_indx > 28)
        return -2;
    
    //Try to open adc data logfile
    if (data_files_opened[f_indx] == 0){
        if (f_open(&(data_files[f_indx]), data_filenames[f_indx], FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
            return -1;
        f_lseek(&(data_files[f_indx]), f_size(&(data_files[f_indx])));//Move pointer at the end to append data
        data_files_opened[f_indx] = 1;
    }
    
    return 0;
}

void close_log_files(void){
    uint8_t i;
    
    for (i = 0; i < 29; i++){
        if (data_files_opened[i])
            f_close(&(data_files[i]));
    }
}

int log_data(adc_value_t data, uint32_t timestamp){
    char str_tmp[256];
    
    snprintf(str_tmp, 256, "%d ; %d ; %f\n", data.ch_id, timestamp, data.data);
    f_puts(str_tmp, &(data_files[data.ch_id]));  

    return 0;
}

int read_config_from_file(char* filename, adc_config_t* conf){
    char buf_string[256];
    char *param_name, *param_value; 
    long parsed_d_buf = 0;
    
    if (FS_STAT > DRIVE_MOUNTED)
        return -1;
    
    //Try to open adc config file
    if (f_open(&adc_config_file, filename, FA_OPEN_ALWAYS | FA_READ) != FR_OK)
        return -1;
    //If ok continue
    
    adc_config_struct_init(conf);
    
    while(f_gets(buf_string, 256, &adc_config_file)){
        if (strncmp(buf_string, "*", 1) == 0)//If string starts with '*'
            continue;//Move to next string
        else{
            //Split stirng to param and value
            param_name = strtok(buf_string, " ");
            if (param_name == NULL)//If string does not contain space delimiter
                continue;
            param_value = strtok(NULL, " ");
            
            //Parsing and filling parameters
            //All invalid strings are skipped
            if (strcmp(param_name, "SPI_RESET_TIMER") == 0){
                if (strcmp(param_value, "256\n") == 0)
                    conf->config0_reg |= ADC_CONFIG0_SPIRST;
                if (strcmp(param_value, "4096\n") == 0)
                    conf->config0_reg &= ~ADC_CONFIG0_SPIRST;
                continue;
            }
            if (strcmp(buf_string, "SCAN_MODE") == 0){
                if (strcmp(param_value, "FIXED\n") == 0)
                    conf->config0_reg |= ADC_CONFIG0_MUXMOD;
                if (strcmp(param_value, "SCAN\n") == 0)
                    conf->config0_reg &= ~ADC_CONFIG0_MUXMOD;
                continue;
            }
                
            if (strcmp(buf_string, "MULTIPLEXER") == 0){
                if (strcmp(param_value, "EXT\n") == 0)
                    conf->config0_reg |= ADC_CONFIG0_BYPAS;
                if (strcmp(param_value, "INT\n") == 0)
                    conf->config0_reg &= ~ADC_CONFIG0_BYPAS;
                continue;
            }
                
            if (strcmp(buf_string, "CLK_OUT") == 0){
                if (strcmp(param_value, "ENABLE\n") == 0)
                    conf->config0_reg |= ADC_CONFIG0_CLKENB;
                if (strcmp(param_value, "DISABLE\n") == 0)
                    conf->config0_reg &= ~ADC_CONFIG0_CLKENB;
                continue;
            }
                
            if (strcmp(buf_string, "CHOPPING") == 0){
                if (strcmp(param_value, "ENABLE\n") == 0)
                    conf->config0_reg |= ADC_CONFIG0_CHOP;
                if (strcmp(param_value, "DISABLE\n") == 0)
                    conf->config0_reg &= ~ADC_CONFIG0_CHOP;
                continue;
            }
                
            if (strcmp(buf_string, "STATUS_BYTE") == 0){
                if (strcmp(param_value, "ENABLE\n") == 0)
                    conf->config0_reg |= ADC_CONFIG0_STAT;
                if (strcmp(param_value, "DISABLE\n") == 0)
                    conf->config0_reg &= ~ADC_CONFIG0_STAT;
                continue;
            }
                
            if (strcmp(buf_string, "IDLE_MODE") == 0){
                if (strcmp(param_value, "SLEEP\n") == 0)
                    conf->config1_reg |= ADC_CONFIG1_IDLMOD;
                if (strcmp(param_value, "STANBY\n") == 0)
                    conf->config1_reg &= ~ADC_CONFIG1_IDLMOD;
                continue;
            }
                
                
            if (strcmp(buf_string, "SWITCH_DELAY") == 0){
                parsed_d_buf = strtol(param_value, NULL, 2);
                if (parsed_d_buf > 7)
                    continue;
                //Zero DLY bits and write new value
                conf->config1_reg &= ~(ADC_CONFIG1_DLY2|ADC_CONFIG1_DLY1|ADC_CONFIG1_DLY0);
                conf->config1_reg |= ((uint8_t)parsed_d_buf) << 4;
                continue;
            }
                
            if (strcmp(buf_string, "SENSOR_BIAS") == 0){
                if (strcmp(param_value, "OFF\n") == 0)
                    conf->config1_reg &= ~(ADC_CONFIG1_SBCS1|ADC_CONFIG1_SBCS0);
                if (strcmp(param_value, "15\n") == 0){
                    conf->config1_reg &= ~(ADC_CONFIG1_SBCS1|ADC_CONFIG1_SBCS0);
                    conf->config1_reg |= ADC_CONFIG1_SBCS0;
                }
                if (strcmp(param_value, "24\n") == 0)
                    conf->config1_reg |= ADC_CONFIG1_SBCS1|ADC_CONFIG1_SBCS0;
                continue;
            }
                
            if (strcmp(buf_string, "DATA_RATE") == 0){
                parsed_d_buf = strtol(param_value, NULL, 2);
                if (parsed_d_buf > 3)
                    continue;
                //Zero DLY bits and write new value
                conf->config1_reg &= ~(ADC_CONFIG1_DRATE1|ADC_CONFIG1_DRATE0);
                conf->config1_reg |= ((uint8_t)parsed_d_buf);
                continue;
            }                
                
            if (strcmp(buf_string, "FIXED_CHANNEL_N_IN") == 0){
                parsed_d_buf = strtol(param_value, NULL, 10);
                if (parsed_d_buf > 15)
                    continue;
                conf->muxsch_reg = (uint8_t)parsed_d_buf;
                continue;            
            }
                
            if (strcmp(buf_string, "FIXED_CHANNEL_P_IN") == 0){
                parsed_d_buf = strtol(param_value, NULL, 10);
                if (parsed_d_buf > 15)
                    continue;
                conf->muxsch_reg = ((uint8_t)parsed_d_buf) << 4;
                continue;
            }
                
            if (strcmp(buf_string, "SCAN_DIFF_CHANNELS") == 0){
                parsed_d_buf = strtol(param_value, NULL, 16);
                if (parsed_d_buf > 15)
                    continue;
                conf->muxdif_reg = (uint8_t)parsed_d_buf;
                continue;
            }
                
            if (strcmp(buf_string, "SCAN_SE_CHANNELS") == 0){
                parsed_d_buf = strtol(param_value, NULL, 16);
                if (parsed_d_buf > 255)
                    continue;
                conf->muxsg1_reg = (uint8_t)(parsed_d_buf >> 8);
                conf->muxsg0_reg = (uint8_t)parsed_d_buf;
                continue;
            }
                
            if (strcmp(buf_string, "SCAN_SYSTEM") == 0){
                parsed_d_buf = strtol(param_value, NULL, 16);
                if (parsed_d_buf > 0x3D)
                    continue;
                conf->sysred_reg = (uint8_t)parsed_d_buf;
            }
	
        }
        
    }
    
    return 0;
}

int read_timer_setting_from_file(char* filename, timer_config_t* conf){
    char buf_string[256];
    char *param_name, *param_value; 
    long parsed_d_buf = 0;
    
    if (FS_STAT > DRIVE_MOUNTED)
        return -1;
    
    //Try to open adc config file
    if (f_open(&timer_config_file, filename, FA_OPEN_ALWAYS | FA_READ) != FR_OK)
        return -1;
    //If ok continue
    
    timer_config_struct_init(conf);
    
    while(f_gets(buf_string, 256, &timer_config_file)){
        if (strncmp(buf_string, "*", 1) == 0)//If string starts with '*'
            continue;//Move to next string
        else{
            //Split stirng to param and value
            param_name = strtok(buf_string, " ");
            if (param_name == NULL)//If string does not contain space delimiter
                continue;
            param_value = strtok(NULL, " ");
            
            //Parsing and filling parameters
            //All invalid strings are skipped
            if (strcmp(param_name, "SCALE") == 0){
                if (strcmp(param_value, "US\n") == 0)
                    conf->scale = TIMER_US_SCALE;
                if (strcmp(param_value, "MS\n") == 0)
                    conf->scale = TIMER_MS_SCALE;
                continue;
            }
            if (strcmp(buf_string, "PERIOD") == 0){
                parsed_d_buf = strtol(param_value, NULL, 10);
                if (parsed_d_buf > 65535)
                    continue;
                conf->period = (uint16_t)parsed_d_buf;
            }
        }
        
    }
    
    return 0;
}
