#ifndef __WIFI_H__
#define __WIFI_H__

void execute_wifi_cmd(char* cmd);

//returns 0 if 'OK'
//returns 1 if error
int wait_for_response(void);

int ex_wifi_script(void);

#endif
