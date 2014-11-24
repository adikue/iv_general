#include "wifi.h"
#include "sd_processing.h"
#include "tm_stm32f4_fatfs.h"
#include "usart.h"
#include <string.h>

/*
1. Read configuration script from SD. Script is a set of AT commands
2. Execute script via USART1
3. Log script execution to file on SD

Write async messages processor and data receiver
*/

static int cfg_file_indx = -1;

void execute_wifi_cmd(char* cmd){
    //flush_rx_buff();
    while( *cmd != 0 ){
        usart1_send(*cmd);
        cmd++;
    }
}

//returns 0 if 'OK'
//returns 1 if error
int wait_for_response(void){
    char buf_resp[256];
    int r_poiter = 0;
    uint8_t buf_char = 0;
    int str_started = 0;
    
    //every command response from wifi ended with '\r\nOK\r\n' or '\r\nERROR:....\r\n'
    while( (strncmp(buf_resp, "\r\nOK", 4) != 0) || (strncmp(buf_resp, "\r\nERROR:", 8) != 0) ){
        if (usart1_recv(&buf_char) != 0){
            //we capture bytes in strings
            if (buf_char == '\r'){//if we catch \r character
                    str_started ^= 1;//change flag of start new string        
            }
            if ( (str_started) && (r_poiter < 256) ){
                buf_resp[r_poiter] = buf_char;
                r_poiter++;
            }else
                r_poiter = 0;
        }else
            break;
    }
    
    return (strncmp(buf_resp, "\r\nOK", 4) != 0 ? 1 : 0);
}

int ex_wifi_script(void){
    char buf_string[256];
    
    cfg_file_indx = open_file(WF_SCRIPT_FILENAME, FA_OPEN_ALWAYS | FA_READ);
    if (cfg_file_indx < 0)
        return cfg_file_indx;//returns -1 if failed
    
    seek_file(cfg_file_indx, 0);//Seek to the start of file
    
    while(read_line(cfg_file_indx, buf_string)){
        
        if (strncmp(buf_string, "*", 1) == 0)//If string starts with '*'
            continue;//Move to next string
        else{
            execute_wifi_cmd(buf_string);
            if (wait_for_response() != 0){//if responded error
                return -2;
            }
        }
    }
    
    return 0;
}
