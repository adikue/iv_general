#include "config.h"
#include "sd_processing.h"
#include "tm_stm32f4_fatfs.h"
#include <string.h>
#include <stdlib.h>
#include "main_config_processor.h"

static int cfg_file_indx = -2;
static cfg_parameter_t config_params[PARAMS_NUM] = { {"START_MOTOR_STATE", "STOP_AT_START\n", DISCRETE, {"STOP_AT_START\n", "RUN_AT_START\n", ""} },
                                                     {"MOTOR_RUN",         "SCRIPT_RUN\n",    DISCRETE, {"SCRIPT_RUN\n", "WF_CONTROL\n",      ""} },
};

int find_in_params(char* name){
    int i = 0;
    
    for (i = 0; i< PARAMS_NUM; i++){
    if (strcmp(name, config_params[i].pname) == 0)
        return i;
    }
    
    return -1;
}

int check_param_val(int par_idx, char* pvalue){
    int i = 0;
    long parsed_digital = 0;
    long min_v = 0;
    long max_v = 0;
    
    if (par_idx < 0)
        return -1;
    
    if (config_params[par_idx].bound_type == DISCRETE){
        while(strncmp(config_params[par_idx].restrictions[i], "", 1) != 0){//Cycle through all restrictions
            
            if (strcmp(pvalue, config_params[par_idx].restrictions[i]) == 0)//Compare value with allowed values
                return 1;
            i++;
        }
    }
        
    if (config_params[par_idx].bound_type == VAR){
        parsed_digital = strtol(pvalue, NULL, 2);//Convert string to long value
        min_v = strtol(config_params[par_idx].restrictions[0], NULL, 2);//Convert min bound
        max_v = strtol(config_params[par_idx].restrictions[1], NULL, 2);//Convert max bound
        if ((parsed_digital <= max_v) && (parsed_digital >= min_v))
            return 1;
    }
    
    return 0;
        
}

int read_main_cofig(void){
    char buf_string[256];
    int  param_idx;
    char *param_name, *param_value; 
    
    cfg_file_indx = open_file(CONFIG_FILENAME, FA_OPEN_ALWAYS | FA_READ);
    if (cfg_file_indx < 0)
        return cfg_file_indx;
    
    seek_file(cfg_file_indx, 0);//Seek to the start of file
    
    while(read_line(cfg_file_indx, buf_string)){
        
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
            param_idx = find_in_params(param_name);
            if (param_idx){
                if(check_param_val(param_idx, param_value))
                    config_params[param_idx].value = param_value;
            }
	
        }

    }
    return 0;
    
}

int write_main_cofig(void){
    int tmp, i;
    
    tmp = open_file(CONFIG_FILENAME_WR, FA_CREATE_ALWAYS | FA_WRITE);
    if (tmp < 0)
        return tmp;
    
    for (i = 0; i < PARAMS_NUM; i++){
        if ( (write_line(tmp, config_params[i].pname) ||
              write_line(tmp, " ") ||
              write_line(tmp, config_params[i].value)) != 0){
                  
            close_file(tmp);
            rem_file(CONFIG_FILENAME_WR);
            return -1;
                  
        }
        
    }
    
    return 0;
    
    
}

