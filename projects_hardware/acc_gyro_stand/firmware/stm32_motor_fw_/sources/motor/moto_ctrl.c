#include "motor.h"
#include "timer.h"
#include "hal.h"
#include "sd_processing.h"
#include "tm_stm32f4_fatfs.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static uint16_t cur_motor_speed_RPM = 0;
//static uint16_t desired_motor_speed_RPM = 0;

static uint16_t motor_speed_RPM_max = 0;
static uint16_t motor_speed_RPM_min = 0;

static double cur_throttle = 0;
static double throttle_max = 0;
static double throttle_min = 0;

static uint8_t calibrated = 0;

static int run_file_indx = -1;
static int rotary_log_file_indx = -1;

/*
Motor rotary sensor has 6 states. Placed equaly per 30 degrees
So if motor transition was from position 5 to position 1 
this thansition takes 2 sectors or 60 degrees.
That's why one round contain 12 states - 1 2 3 4 5 6 1 2 3 4 5 6
*/

void motor_sensor_callback(uint8_t ABC_hall_data){
    static uint32_t prev_occurence_time;
    uint32_t curr_occurence_time = get_timestamp();
    uint32_t diff_time;
//    double step = 0;
    
    if (ABC_hall_data == MOTOR_POS1){//Check if we met same state        
        diff_time = curr_occurence_time - prev_occurence_time;     
        prev_occurence_time = curr_occurence_time;
        cur_motor_speed_RPM = (uint16_t)(1000000/(2 * diff_time));
        
//        if ( calibrated ){
//            if (desired_motor_speed_RPM > cur_motor_speed_RPM)
//                step = +1;
//            else
//                step = -1;
//            
//            step = pow(desired_motor_speed_RPM - cur_motor_speed_RPM, 3)/(0.2 * cur_motor_speed_RPM);
//            set_motor_throttle(cur_throttle + step  * MOTOR_SPEED_STEP);        
//        }
    }
}

//Speed = -1.0 ... 1.0
int set_motor_throttle(double throttle){
    if ((throttle > MOTOR_THROTTLE_MAX) || (throttle < -MOTOR_THROTTLE_MAX)){
        return -1;
    }
    if( calibrated ){
        if (throttle < throttle_min)
            cur_motor_speed_RPM = 0;
        
        if (throttle > throttle_max)
            throttle = throttle_max;
    }
        
    cur_throttle = throttle;
    set_pwm_width_ms(1.5 + throttle * 0.5);
    return get_motor_speed_rpm();
}

//int set_motor_speed_rpm(uint16_t rpm_val){
//    
//    if (!calibrated)
//        return -1;
//   
//    start_timestamp_cnt();
//    
//    desired_motor_speed_RPM = rpm_val;
//    if (cur_motor_speed_RPM == 0 && desired_motor_speed_RPM != 0)
//        set_motor_throttle(throttle_min);
//    
//    while(1){}
//    
//    return get_motor_speed_rpm();
//}

uint16_t get_motor_speed_rpm(void){
    int i = MOTOR_RPM_AV_NUM;
    uint32_t motor_rpm_1ms_sum = 0;

    //Get accumulated 1ms value
    while (i){
        motor_rpm_1ms_sum += cur_motor_speed_RPM;
        delay_us(10);
        i--;
    }
    
    return motor_rpm_1ms_sum/MOTOR_RPM_AV_NUM;
}

int motor_calibrate(void){
    double throttle = 0;
    uint16_t rpm = 0;
    
    full_stop();
    reset_timestamp_cnt();
    start_timestamp_cnt();
    
    //Increase speed   
    while (throttle < MOTOR_THROTTLE_MAX){
        set_motor_throttle(throttle);
        throttle += 100 * MOTOR_SPEED_STEP;
        rpm = get_motor_speed_rpm();
        
        //If motor started rotating
        if (rpm > 0 && !motor_speed_RPM_min){
            motor_speed_RPM_min = rpm;
            throttle_min = cur_throttle;
        }
        
        if (rpm > motor_speed_RPM_max){
            motor_speed_RPM_max = rpm;
            throttle_max = cur_throttle;
        }
    }    
    
    if (motor_speed_RPM_min){    
        motor_speed_RPM_min *= 1.1;
        motor_speed_RPM_max *= 0.9;
        calibrated = 1;
    }
    
    full_stop();
    reset_timestamp_cnt();
    
    return calibrated - 1;
}

void full_stop(){
    set_motor_throttle(0);
    delay_us(SECOND_DELAY);
    cur_motor_speed_RPM = 0;
    
}

int ex_motor_script(void){
    char buf_string[256];
    char *time_string, *throttle_string;
    uint32_t r_timestamp;
    double r_throttle_val;
    
    reset_timestamp_cnt();
    
    run_file_indx = open_file(MOTOR_SCRIPT_FILENAME, FA_OPEN_ALWAYS | FA_READ);
    if (run_file_indx < 0)
        return run_file_indx;//returns -1 if failed
    
    seek_file(run_file_indx, 0);//Seek to the start of file
    
    while(read_line(run_file_indx, buf_string)){
        
        if (strncmp(buf_string, "*", 1) == 0)//If string starts with '*'
            continue;//Move to next string
        else{
            /* Get text values */
            //Split stirng to timestamp and motor_speed
            time_string = strtok(buf_string, ";");//Get timestamp
            if (time_string == NULL)//If string does not contain delimiter
                continue;//Move to next string
            throttle_string = strtok(NULL, "\n");//Get motor speed value
            
            /* Convert text to numbers*/
            r_timestamp = strtoul(time_string, NULL, 10);
            r_throttle_val = strtod(throttle_string, NULL);
            
            start_timestamp_cnt();            
            /* Use values */

            while (add_timestamp_motortask(r_timestamp, r_throttle_val)) {}
        }
    }
    
    return 0;
}

void dump_motor_data(){
}
