#ifndef TIMER_H
#define TIMER_H

#include "general_types.h"

int set_pwm_width_ms(double new_width);

void start_timestamp_cnt(void);
void stop_timestamp_cnt(void);
void reset_timestamp_cnt(void);
uint32_t get_timestamp(void);

int add_timestamp_motortask(uint32_t tstamp, double next_throttle);

#endif
