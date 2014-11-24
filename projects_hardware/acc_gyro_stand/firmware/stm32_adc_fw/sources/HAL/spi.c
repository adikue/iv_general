#include "spi.h"
#include "stm32f4xx.h"
#include "config.h"

uint8_t spi_state = spi_uninitilized;

static void set_spi_cs(void){
    GPIOB->ODR &= ~GPIO_ODR_ODR_12;//Set /CS low
}

static void clear_spi_cs(void){
    GPIOB->ODR |= GPIO_ODR_ODR_12;//Set /CS high
}

static uint16_t spi_send(uint16_t data){
    set_spi_cs();
    SPI2->DR = data;
    while( !(SPI2->SR & SPI_SR_RXNE) );
    while( !(SPI2->SR & SPI_SR_TXE) );
    while( SPI2->SR & SPI_SR_BSY );
    clear_spi_cs();
        
    return SPI2->DR;
}

void spi_test(void){
    volatile uint16_t tmp0 = 0;
    
    tmp0 = spi_send(0x4000);
    
    tmp0 = spi_send(0x4100);

    tmp0 = spi_send(0x4200);

    tmp0 = spi_send(0x4300);

    tmp0 = spi_send(0x4400);

    tmp0 = spi_send(0x4500);

    tmp0 = spi_send(0x4600);

    tmp0 = spi_send(0x4700);

    tmp0 = spi_send(0x4800);

    tmp0 = spi_send(0x4900);

    
    
    //spi_write_reg(0x00, 0x0A);
}

/**
  * @brief  Initializes the SPIx peripheral.

  * @retval None
*/
void spi_init(void)
{   
    SPI_InitTypeDef SPI_InitStructure;
   
    /*
    * SPI Master
    * MSB first
    * SPI SCLK frquency 6MHz (APB1 / 4)
    * CPOL = 0, CPHA = 1
    * Frame size = 1 byte
    * Software nSS pin  control
    */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(SPI2, &SPI_InitStructure);
    clear_spi_cs();    

    spi_state |= spi_initialized;
    
    SPI_Cmd(SPI2, ENABLE);//Enable SPI
}

/**
    * @brief  Read register value from ADC
    
    * @param  addr: Address of register to read
    * @param  stor_val: Pointer where to save register value
    
    * @retval error code
*/
uint8_t spi_read_reg(uint8_t addr){
    uint16_t cmd = REGISTER_READ_COMMAND;
    
    if (spi_state != spi_initialized)
        return 0;
    
    cmd |= (uint16_t)addr << 8;

    return  (uint8_t)spi_send(cmd);

}

/**
    * @brief  Write value of register to ADC
 
    * @param  addr: Address of register to write
    * @param  value: Value of register to write
    
    * @retval Status of operation.
*/
int spi_write_reg(uint8_t addr, uint8_t value){
    uint16_t cmd = REGISTER_WRITE_COMMAND;
    
    if (spi_state != spi_initialized)
        return -1;
    
    cmd |= (uint16_t)addr << 8;
    cmd |= (uint16_t)value;
    
    spi_send(cmd);
    
    return 0;
}

/**
    * @brief  Issue 'Read direct' command to ADC
 
    * @param  buf: 4 bytes (32bits) width buffer  for recieved value
    
    * @retval None
*/
void spi_read_direct(uint8_t* buf){
    //Read direct is not available in Fixed Channel Mode
    #if defined (FIXED_CHANNEL_MODE)
        return;
    #endif
    
    uint16_t cmd = CHANNEL_READ_DIRECT;
    
    SPI2->DR = cmd;//We send 32 zeroes
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){}
    buf[0] = SPI_I2S_ReceiveData(SPI2);
        
    SPI2->DR = cmd;//We send 32 zeroes
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){}
    buf[1] = (uint8_t)SPI_I2S_ReceiveData(SPI2);
        
    SPI2->DR = cmd;//We send 32 zeroes
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){}
    buf[2] = (uint8_t)SPI_I2S_ReceiveData(SPI2);    
     
    SPI2->DR = cmd;//We send 32 zeroes
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){}
    buf[3] = (uint8_t)SPI_I2S_ReceiveData(SPI2);
        
    return;
}

/**
    * @brief  Read value from ADC preselected channel
 
    * @param  addr: 5 bytes (40bits) width buffer  for recieved value
    
    * @retval None
*/
void spi_read_channel(uint8_t* buf){
    uint16_t cmd = CHANNEL_READ_COMMAND;
    uint16_t tmp = 0;
    
    set_spi_cs();
    
    SPI2->DR = cmd;
    while( !(SPI2->SR & SPI_SR_TXE) );
    SPI2->DR = cmd;
    
    while( !(SPI2->SR & SPI_SR_RXNE) );
    buf[0] = (uint8_t)SPI2->DR;
    
    while( !(SPI2->SR & SPI_SR_TXE) );
    SPI2->DR = cmd;
    
    while( !(SPI2->SR & SPI_SR_RXNE) );
    tmp = SPI2->DR;
    buf[1] = (uint8_t)(tmp >> 8);//MSB
    buf[2] = (uint8_t)(tmp);//MSB-1
    
    while( !(SPI2->SR & SPI_SR_TXE) );
    SPI2->DR = cmd;
    
    while( !(SPI2->SR & SPI_SR_RXNE) );
    tmp = SPI2->DR;
    buf[3] = (uint8_t)(tmp >> 8);//LSB   
    
    while( !(SPI2->SR & SPI_SR_TXE) );
    while( (SPI2->SR & SPI_SR_BSY) );

    clear_spi_cs();
        
    return;
}

void spi_pulse_convert(void){
    spi_send(PULSE_CONVERT_COMMAND);
    return;
}

void spi_reset(void){      
    spi_send(RESET_COMMAND);
    return;
}
