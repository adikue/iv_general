#ifndef __WIFI_H__
#define __WIFI_H__

#define ADC_MAC_ADDR                "00:80:E1:FF:EC:71"
#define MAC_STRLEN                  17

#define WF_GPIO_CHG_STATE_CMD       "AT+S.GPIOW=1X,1\r"
#define GPIO_CHG_STATE_CMD_NUMPOS   12
#define GPIO_CHG_STATE_CMD_STATPOS  14
#define WF_ON_LED_GPIONUM           '4'
#define WF_UP_LED_GPIONUM           '3'
#define WF_RUN_LED_GPIONUM          '0'

#define WF_OK_ASWER                 "OK"
#define WF_ERR_ASWER                "\r\nERROR"

typedef enum{
    ADDBEFORE = 11,
    ADDAFTER = 12
}trailing_t;

typedef enum{
    WF_OK = 16,
    WF_ERROR,
    WF_TIMEOUT
}wf_answer_t;

typedef enum{
    WF_ON_LED = 56,
    WF_UP_LED,
    WF_RUN_LED
}wf_led_t;

void chg_wifi_led_state(wf_led_t led, int state);

int answer_anylyzer(char *data_byte);

wf_answer_t execute_wifi_cmd(char* cmd, char* ok_answer, char* save_answer);

int ex_wifi_script(void);

int is_adc_connected(void);

#endif
