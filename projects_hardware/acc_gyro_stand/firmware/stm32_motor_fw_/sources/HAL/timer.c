#include "timer.h"
#include "config.h"
#include "hal.h"

extern uint32_t pwm_width;
extern uint8_t pwm_udpated;
extern uint32_t pwm_peiod;

int set_pwm_width_ms(double new_width){
//    if ((pwm_width > PWM_PULSE_MAX) || (pwm_width < PWM_PULSE_MIN))
//        return -1;
    
    pwm_width = (uint32_t)( new_width/1000.0 * PWM_TIMER_CLK - 1 );
    pwm_udpated = 0;
    TIM2_update_PWM();
    //New pwm pulse width will be apllied after timer counter update event via interrupt handler
    return 0;
}

void start_timestamp_cnt(void){
    start_timer5();
    //TODO: Send syncronizing packet
}

void stop_timestamp_cnt(void){
    stop_timer5();
}

void reset_timestamp_cnt(void){
    reset_timer5();
}

uint32_t get_timestamp(void){
    return get_timer5_cnt();
}

int add_timestamp_motortask(uint32_t tstamp, double next_throttle){
    motortask_t task;
    
    task.stamp = tstamp;
    task.throttle = next_throttle;
    
    return add_timer_task(task);
}



