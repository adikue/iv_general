#ifndef SD_DATA_H
#define SD_DATA_H

#include "general_types.h"

typedef enum{
    FILE_OPENED = 0,
    DRIVE_MOUNTED = 1,
    UNINITIALIZED = 2
}file_ops_status_t;

int mount_sd_card(void);
int check_sd_card_size(void);

void unmount_sd_card(void);

//Tries to open file with name @filename
//Opened with RW access and append new data at the end of file
//If success returns opened file index
int open_file(char * filename, uint8_t mode);
int seek_file(uint8_t findex, uint32_t pos);
void close_file(uint8_t findex);
char* read_line(uint8_t findex, char* strbuf);
int write_line(uint8_t findex, char* strbuf);
void rem_file(char* filename);


#endif
