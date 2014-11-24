#ifndef SPI_H
#define SPI_H

#include "hal.h"
#include <stdint.h>

#define WAIT_FOR_SPI_BUSY(SPI, us) \
{   while(us){ \
        if (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_BSY) != SET){ break; } \
        delay_us(1); \
    } \
}

#define SPI_SEND(SPI, data) \
{   \
    SPI_I2S_SendData(SPI, data);\
    WAIT_FOR_SPI_BUSY(SPI, 1);\
}

/* ADC SPI commands */
#define CHANNEL_READ_DIRECT     0x0000
#define CHANNEL_READ_COMMAND    0x3000
#define REGISTER_READ_COMMAND   0x4000
#define REGISTER_WRITE_COMMAND  0x6000
#define PULSE_CONVERT_COMMAND   0x8000
#define RESET_COMMAND           0xC000

//SPI states
#define spi_uninitilized        0x0
#define spi_initialized         0x1

void spi_test(void);

/**
    * @brief  Initializes the SPI peripheral.
    
    * @retval None
*/
void spi_init(void);

/**
    * @brief  Read register value from ADC
    
    * @param  addr: Address of register to read
    * @param  stor_val: Pointer where to save register value
    
    * @retval error code
*/
uint8_t spi_read_reg(uint8_t addr);

/**
    * @brief  Write value of register to ADC
 
    * @param  addr: Address of register to write
    * @param  value: Value of register to write
    
    * @retval Status of operation.
*/
int spi_write_reg(uint8_t addr, uint8_t value);


void spi_read_direct(uint8_t* buf);


void spi_read_channel(uint8_t* buf);

void spi_pulse_convert(void);

void spi_reset(void);


#endif
