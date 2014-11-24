#include "sd.h"
#include <stdint.h>

//static uint32_t SDType;
static uint32_t RCA;

void SD_Initialization(void) {
    /* Hardware initialization stage*/
    
    SDIO_InitTypeDef SDIO_InitStructure;
    SDIO_CmdInitTypeDef SD_Cmd_tmp;
    uint32_t response;
    uint32_t ACMD41_TimeOut=0xFF;
   
    /* SD card hard initialization
    * HW_Flow Disabled
    * Rising Clock Edge
    * Disable CLK ByPass
    * Bus Width=0
    * Power save Disable */
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_4b;
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    SDIO_InitStructure.SDIO_ClockDiv = (uint32_t)0x76;
    
    SDIO_Init(&SDIO_InitStructure);
    SDIO_ClockCmd (ENABLE);
    //Power Up SD interface
    SDIO_SetPowerState(SDIO_PowerState_ON);\
    /*!< Enable SDIO Clock */
    
    
    /* Software initialization stage*/
    
    SDIO_CmdStructInit(&SD_Cmd_tmp);
  
    //CMD0: GO_IDLE_STATE (No Response)
    SD_Cmd_tmp.SDIO_CmdIndex = CMD0;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_No;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_No;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp);
      
    //CMD8: SEND_IF_COND  //Response to CMD8 is R7. But I will ignore that response
    SD_Cmd_tmp.SDIO_CmdIndex = CMD8;
    SD_Cmd_tmp.SDIO_Argument = 0x000001AA;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_Short;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_Pend;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp); //Non v2.0 compliant sd's will cause panic here due to the timeout
    response = SDIO_GetResponse(SDIO_RESP1); 
    response = (response & 0xFF00) >> 16;//AA is the check pattern. If response does not match with it, execution will be blocked in panic
  
  
    while (1) {
        /* Send ACMD41 */
          
        //CMD55
        SD_Cmd_tmp.SDIO_CmdIndex = CMD55;
        SD_Cmd_tmp.SDIO_Argument = 0;
        SD_Cmd_tmp.SDIO_Response = SDIO_Response_Short;
        SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_Pend;
        SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
        SD_Command(&SD_Cmd_tmp); //Note that argument should be RCA. But at this point RCA of SD is 0. (It will be changed after cmd3)
        response = SDIO_GetResponse(SDIO_RESP1);

        //ACMD41 (Response is R3 which does not contain any CRC)
        SD_Cmd_tmp.SDIO_CmdIndex = ACMD41;
        SD_Cmd_tmp.SDIO_Argument = (uint32_t) 0x80100000 | (uint32_t) 0x40000000;//Second part indicates that host supports SDHC.
        SD_Cmd_tmp.SDIO_Response = SDIO_Response_Short;
        SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_Pend;
        SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;

        SD_Command(&SD_Cmd_tmp);
        response = SDIO_GetResponse(SDIO_RESP1);

        //Check the ready status in the response (R3) 
        if ((response >> 31) == 1) {  //When card is busy this bit will be 0
            //Card is now initialized. Check to see if SD is SC or HC
            //SDType=(response & 0x40000000) >> 30;  //1=HC, 0=SC
            break;
        } else {
            ACMD41_TimeOut--;
            if (!ACMD41_TimeOut) {
                //TODO: Initialization error handler
            }      
        }
    }
  
    //Now we are in the Ready State. Ask for CID using CMD2
    //Response is R2. RESP1234 are filled with CID. I will ignore them
    SD_Cmd_tmp.SDIO_CmdIndex = CMD2;
    SD_Cmd_tmp.SDIO_Argument = 0;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_Long;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_No;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp);
    response = SDIO_GetResponse(SDIO_RESP1);

    //Now the card is in the identification mode. Request for the RCA number with cmd3
    SD_Cmd_tmp.SDIO_CmdIndex = CMD3;
    SD_Cmd_tmp.SDIO_Argument = 0;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_Short;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_No;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp);
    response = SDIO_GetResponse(SDIO_RESP1);

    RCA = response >> 16;//The first 16 bit is equal to status

    //Now the card is in stand-by mode. From this point on I can change frequency as I wish (max24MHz)
    
    //Use cmd9 to read the card specific information
    //Response is R2 with CSI. I will ignore the response
    SD_Cmd_tmp.SDIO_CmdIndex = CMD9;
    SD_Cmd_tmp.SDIO_Argument = RCA << 16;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_Long;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_No;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp);
    //CSI register. 128 bit
    //response = SDIO_GetResponse(SDIO_RESP1);
    //response = SDIO_GetResponse(SDIO_RESP2);
    //response = SDIO_GetResponse(SDIO_RESP3);
    //response = SDIO_GetResponse(SDIO_RESP4);


    //Put the Card in the tranfer mode using cmd7. (I will change the clock spped later together with bus width)
    //Bus width can only be changed in transfer mode
    SD_Cmd_tmp.SDIO_CmdIndex = CMD7;
    SD_Cmd_tmp.SDIO_Argument = RCA << 16;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_Short;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_No;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp);
    response = SDIO_GetResponse(SDIO_RESP1);

    //Change the bus-width with cmd6
    //CMD55
    SD_Cmd_tmp.SDIO_CmdIndex = CMD55;
    SD_Cmd_tmp.SDIO_Argument = RCA << 16;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_Short;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_No;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp);
    response = SDIO_GetResponse(SDIO_RESP1);
    //ACMD6
    SD_Cmd_tmp.SDIO_CmdIndex = ACMD6;
    SD_Cmd_tmp.SDIO_Argument = 0x02;
    SD_Cmd_tmp.SDIO_Response = SDIO_Response_Short;
    SD_Cmd_tmp.SDIO_Wait = SDIO_Wait_No;
    SD_Cmd_tmp.SDIO_CPSM = SDIO_CPSM_Enable;
    SD_Command(&SD_Cmd_tmp);
    response = SDIO_GetResponse(SDIO_RESP1);

    //Configure SDIO->CLKCr for new clock
    //As the clock divider=0 => New clock=48/(Div+2)=48/2=24MHz
    SDIO_InitStructure.SDIO_ClockDiv = (uint32_t)0x76;
    SDIO_Init(&SDIO_InitStructure);

    //Now we can start issuing read/write commands
}


static uint32_t SD_Command(SDIO_CmdInitTypeDef* _Cmd){
    uint8_t retries = 3;
    
    do{
        SDIO_SendCommand(_Cmd);//Send a command
        retries--;

        //Block till we get a response  
        if (_Cmd->SDIO_Response == SDIO_Response_No) {
            //We should wait for CMDSENT
            while (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT|SDIO_FLAG_CMDSENT) != SET) {};
        }else{//SDIO_Response_Short or SDIO_Response_Long
            //We should wait for CMDREND or CCRCFAIL
            while (SDIO_GetFlagStatus(SDIO_STA_CTIMEOUT|SDIO_FLAG_CMDREND|SDIO_FLAG_CCRCFAIL) != SET ) {};
        }
    }while (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT|SDIO_FLAG_CCRCFAIL) == SET && retries);
    
    if (retries <= 0){
        //TODO: write command timeout handler
    }
    if ((SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) == SET) && (_Cmd->SDIO_CmdIndex != ACMD41)){
        //TODO: write command CRC error handler
    }
    
    return SDIO->STA;
}


