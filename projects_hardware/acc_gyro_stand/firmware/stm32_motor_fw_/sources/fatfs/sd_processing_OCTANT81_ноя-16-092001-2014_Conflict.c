#include "tm_stm32f4_fatfs.h"
#include "sd_processing.h"
#include "config.h"
//#include <stdio.h>
//#include <stdlib.h>

static FATFS FatFs;
static file_ops_status_t FS_STAT = UNINITIALIZED;
static FIL file_ptrs[NUM_FILES];
static uint8_t f_toopen = 0;
    
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

//Tries to open file with name @filename
//Opened with RW access and append new data at the end of file
//If success returns opened file index
int open_file(char * filename, uint8_t mode){
    if (FS_STAT > DRIVE_MOUNTED)
        return -1;
    
    if (f_toopen > NUM_FILES)
        return -2;
    
    //Try to open file
    if (f_open(&(file_ptrs[f_toopen]), filename, mode) != FR_OK)
        return -1;
    
    return f_toopen;
}

int seek_file(uint8_t findex, uint32_t pos){
    if (FS_STAT > DRIVE_MOUNTED)
        return -1;
    
    return f_lseek(&(file_ptrs[f_toopen]), pos);
}

char* read_line(uint8_t findex, char* strbuf){
    return f_gets(strbuf, 256, &file_ptrs[findex]);
}

int write_line(uint8_t findex, char* strbuf){
    return f_puts(strbuf, &(file_ptrs[findex]));
}

void close_file(uint8_t findex){
     f_close(&(file_ptrs[findex]));
}

void rem_file(char* filename){
    f_unlink(filename);
}
