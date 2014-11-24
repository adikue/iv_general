#include "wifi.h"
#include "sd_processing.h"
#include "tm_stm32f4_fatfs.h"
#include "usart.h"
#include "hal.h"
#include <string.h>

/*
1. Read configuration script from SD. Script is a set of AT commands
2. Execute script via USART1
3. Log script execution to file on SD

Write async messages processor and data receiver
*/

static int cfg_file_indx = -1;
static int answered = 0;
static char response[256];

static char* split_wifi_cmd_asnwer(char* script_line, char* cmd, char* answ){
    size_t ln;
    char *cmb_buf;
    char* answ_buf;
    
    cmb_buf = strtok(script_line, ";");//Get wifi command
    if (cmb_buf == NULL)//If string does not contain delimiter
        return NULL;
    
    //strtok returns shifted pointers to original buffer
    //We need this tokens as separate c-strings
    //That's why we change '\0' in cmd line with the '\r' caracter
    ln = strlen(cmb_buf);
    cmb_buf[ln] = '\r';
    //To correct interpret c-string we should add \0 caracter;
    strncpy(cmd, cmb_buf, ln+1);//that's why copyind cmd+\r as new c-string
    cmd[ln+1] = '\0';// add \0
    //now we have cmd = cmb_buf + \r + \0
   
    answ_buf = strtok(NULL, "\r\n");//Get wifi command OK answer
    //We have ok answer contans without \n or \r
    ln = strlen(answ_buf);
    //strncpy(answ, "\r\n", 2);//Store \r\n
    //strncpy(answ + 2, answ_buf, ln + 1);//Store answer contains
    //answ[ln+3] = '\0';
    //now we have answ = \r + \n + answ_buf + \0
    
    strncpy(answ, answ_buf, ln);//Store answer contains
    answ[ln+1] = '\0';
    
    return answ;
}

int answer_anylyzer(char *data_byte){
    static int str_started;
    static int r_poiter;
    
    if ( r_poiter == 0 ){
        str_started = 1;//change flag of start new answer line     
    }
    
    if ( (*data_byte == '\r') || (*data_byte == '\n') ){
        str_started = 0;//change flag of start new answer line     
    }
    
    if ( (str_started) && (r_poiter < 255) ){
        response[r_poiter] = *data_byte;//store answer line
        r_poiter++;
    }else if (r_poiter == 255)
        r_poiter = 0;
    
    if (!(str_started) && (r_poiter > 0)){
        answered = 1;
        response[r_poiter] = '\0';
        r_poiter = 0;
        return 1;
    }
    
    return 0;
    
}
wf_answer_t execute_wifi_cmd(char* cmd, char* ok_answer, char* save_answer){
    uint32_t dcnt = 4000000;//Roughly 1s. Enough time for receive any answer. EXCEPT SCAN!!!
    size_t ok_answ_len = strlen(ok_answer);
    
    answered = 0;

    while( *cmd != 0 ){
        usart1_send(*cmd);
        cmd++;
    }
    while(!usart_cmd_is_executed()){}
        
    while(dcnt){
        dcnt--;
        if (answered){//If we have an answer line
            //Check it for OK aswer
            if (strncmp(response, ok_answer, ok_answ_len) == 0){
                answered = 0;
                if(save_answer != NULL)
                    strcpy(save_answer, response);
                return WF_OK;
            //Check it for ERROR answer
            }else if (strncmp(response, WF_ERR_ASWER, strlen(WF_ERR_ASWER)) == 0){
                answered = 0;
                return WF_ERROR;
            }
        }
        //Wait for next answer line
    }
    return WF_TIMEOUT;
}

void chg_wifi_led_state(wf_led_t led, int state){
    char WF_CHANGE_LED_CMD[] = WF_GPIO_CHG_STATE_CMD;
    
    switch (led){
        case WF_ON_LED:
            WF_CHANGE_LED_CMD[GPIO_CHG_STATE_CMD_NUMPOS] = WF_ON_LED_GPIONUM;
            break;
        case WF_UP_LED:
            WF_CHANGE_LED_CMD[GPIO_CHG_STATE_CMD_NUMPOS] = WF_UP_LED_GPIONUM;
            break;
        case WF_RUN_LED:
            WF_CHANGE_LED_CMD[GPIO_CHG_STATE_CMD_NUMPOS] = WF_RUN_LED_GPIONUM;
            break;
        default:
            return;     
    }
    
    if (state > 0)
        WF_CHANGE_LED_CMD[GPIO_CHG_STATE_CMD_STATPOS] = '1';
    else
        WF_CHANGE_LED_CMD[GPIO_CHG_STATE_CMD_STATPOS] = '0';
    
    execute_wifi_cmd(WF_CHANGE_LED_CMD, WF_OK_ASWER, NULL);
}


int ex_wifi_script(void){
    char buf_string[256];
    char command[256];
    char cmd_ok_answ[32];
    
    
    cfg_file_indx = open_file(WF_SCRIPT_FILENAME, FA_OPEN_ALWAYS | FA_READ);
    if (cfg_file_indx < 0)
        return cfg_file_indx;//returns -1 if failed
    
    seek_file(cfg_file_indx, 0);//Seek to the start of file
    
    while(read_line(cfg_file_indx, buf_string)){
        
        if (strncmp(buf_string, "*", 1) == 0)//If string starts with '*'
            continue;//Move to next string
        else{
            //Split stirng to param and value
            if (split_wifi_cmd_asnwer(buf_string, command, cmd_ok_answ) == NULL)
                continue;
            
            if (execute_wifi_cmd(command, cmd_ok_answ, NULL) != WF_OK){//if responded error
                return -2;
            }
        }
    }
    
    return 0;
}

int is_adc_connected(void){
    uint8_t a = 0, peer_cmdnum_pos = 11, peer_ansnum_pos = 3;
    char peer_num_buf[] = "12345";
    char get_peer_mac_cmd[] = "AT+S.PEERS=X,addr\r";
    char cmd_answer_pattern[] = "#  X.addr = ";
    char answer_storage[128];
    
    while(a < strlen(peer_num_buf)){
        //Form correct command for next peer number
        get_peer_mac_cmd[peer_cmdnum_pos] = peer_num_buf[a];
        cmd_answer_pattern[peer_ansnum_pos] = peer_num_buf[a];
        a++;
        
        if (execute_wifi_cmd(get_peer_mac_cmd, cmd_answer_pattern, answer_storage) != WF_OK)
            return -1;
        //If succes we have answer_storage = "# X.addr = xx:xx:xx:xx:xx:xx"
        if (strlen(answer_storage) < 28)//Check answer length
            continue;
        //Check MAC for desired module address
        if (strncmp(&answer_storage[12], ADC_MAC_ADDR, MAC_STRLEN) == 0)
            return 1;
        
        
    }
    
    return 0;
}
