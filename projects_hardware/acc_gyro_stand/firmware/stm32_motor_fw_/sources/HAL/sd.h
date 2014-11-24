#ifndef SD_H
#define SD_H

#include "stm32f4xx.h"

#define DATATIMEOUT   (0xFFFFFF)  //I simply made this up. A method for computing a realistic values from CSD is described in the specs.

//SDIO Commands  Index 
#define CMD0          ((uint8_t)0)
#define CMD8          ((uint8_t)8)
#define CMD55         ((uint8_t)55)
#define ACMD41        ((uint8_t)41)
#define CMD2          ((uint8_t)2)
#define CMD3          ((uint8_t)3)
#define CMD9          ((uint8_t)9)
#define CMD7          ((uint8_t)7)
#define ACMD6         ((uint8_t)6)
#define CMD24         ((uint8_t)24)
#define CMD25         ((uint8_t)25)
#define CMD12         ((uint8_t)12)
#define CMD13         ((uint8_t)13)
#define CMD17         ((uint8_t)17)
#define CMD18         ((uint8_t)18)

//Auxilary defines
#define NORESP        (0x00)
#define SHRESP        (0x40)
#define LNRESP        (0xC0)
#define R3RESP        (0xF40)  //Note this is totaly out of standard. However, becouse of the masking in SD_Command it will be processed as SHRESP
//R3 does not contain a valid CRC. Therefore, CCRCFAIL is set and CMDREND is never set for R3.
//To properly process R3, exit the loop CCRCFAIL condition and don't check CMDREND

#define RESP_R1       (0x01)
#define RESP_R1b      (0x02)
#define RESP_R2       (0x03)
#define RESP_R3       (0x04)
#define RESP_R6       (0x05)
#define RESP_R7       (0x06)

#define UM2SD         (0x00)  //Transfer Direction
#define SD2UM         (0x02)

void SD_Initialization(void);
static uint32_t SD_Command(SDIO_CmdInitTypeDef* _Cmd);


#endif
