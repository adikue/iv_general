#ifndef ADC_H
#define ADC_H

#include "general_types.h"

/* ADC registers */
#define ADC_CONFIG0_REG     0x00
#define ADC_CONFIG1_REG     0x01
#define ADC_MUXSCH_REG      0x02
#define ADC_MUXDIF_REG      0x03
#define ADC_MUXSG0_REG      0x04
#define ADC_MUXSG1_REG      0x05
#define ADC_SYSRED_REG      0x06
#define ADC_GPIOC_REG       0x07
#define ADC_GPIOD_REG       0x08
#define ADC_ID_REG          0x09

/* ADC registers default values */
#define ADC_CONFIG0_DEF_VAL	0x0A
#define ADC_CONFIG1_DEF_VAL	0x83
#define ADC_MUXSCH_DEF_VAL	0x00
#define ADC_MUXDIF_DEF_VAL	0x00
#define ADC_MUXSG0_DEF_VAL	0xFF
#define ADC_MUXSG1_DEF_VAL	0xFF
#define ADC_SYSRED_DEF_VAL	0x00
#define ADC_GPIOC_DEF_VAL	0xFF
#define ADC_GPIOD_DEF_VAL	0x00

/* ADC registers bits */
#define ADC_CONFIG0_SPIRST 	(1<<6)
#define ADC_CONFIG0_MUXMOD	(1<<5)
#define ADC_CONFIG0_BYPAS	(1<<4)
#define ADC_CONFIG0_CLKENB	(1<<3)
#define ADC_CONFIG0_CHOP	(1<<2)
#define ADC_CONFIG0_STAT	(1<<1)

#define ADC_CONFIG1_IDLMOD	(1<<7)
#define ADC_CONFIG1_DLY2	(1<<6)
#define ADC_CONFIG1_DLY1	(1<<5)
#define ADC_CONFIG1_DLY0	(1<<4)
#define ADC_CONFIG1_SBCS1	(1<<3)
#define ADC_CONFIG1_SBCS0	(1<<2)
#define ADC_CONFIG1_DRATE1	(1<<1)
#define ADC_CONFIG1_DRATE0	(1<<0)

#define ADC_MUXSCH_AINP3	(1<<7)
#define ADC_MUXSCH_AINP2	(1<<6)
#define ADC_MUXSCH_AINP1	(1<<5)
#define ADC_MUXSCH_AINP0	(1<<4)
#define ADC_MUXSCH_AINN3	(1<<3)
#define ADC_MUXSCH_AINN2	(1<<2)
#define ADC_MUXSCH_AINN1	(1<<1)
#define ADC_MUXSCH_AINN0	(1<<0)

#define ADC_MUXDIF_DIFF7	(1<<7)
#define ADC_MUXDIF_DIFF6	(1<<6)
#define ADC_MUXDIF_DIFF5	(1<<5)
#define ADC_MUXDIF_DIFF4	(1<<4)
#define ADC_MUXDIF_DIFF3	(1<<3)
#define ADC_MUXDIF_DIFF2	(1<<2)
#define ADC_MUXDIF_DIFF1	(1<<1)
#define ADC_MUXDIF_DIFF0	(1<<0)

#define ADC_MUXSG0_AIN7		(1<<7)
#define ADC_MUXSG0_AIN6		(1<<6)
#define ADC_MUXSG0_AIN5		(1<<5)
#define ADC_MUXSG0_AIN4		(1<<4)
#define ADC_MUXSG0_AIN3		(1<<3)
#define ADC_MUXSG0_AIN2		(1<<2)
#define ADC_MUXSG0_AIN1		(1<<1)
#define ADC_MUXSG0_AIN0		(1<<0)

#define ADC_MUXSG1_AIN15	(1<<7)
#define ADC_MUXSG1_AIN14	(1<<6)
#define ADC_MUXSG1_AIN13	(1<<5)
#define ADC_MUXSG1_AIN12	(1<<4)
#define ADC_MUXSG1_AIN11	(1<<3)
#define ADC_MUXSG1_AIN10	(1<<2)
#define ADC_MUXSG1_AIN9		(1<<1)
#define ADC_MUXSG1_AIN8		(1<<0)

#define ADC_SYSRED_REF		(1<<5)
#define ADC_SYSRED_TEMP		(1<<4)
#define ADC_SYSRED_GAIN		(1<<3)
#define ADC_SYSRED_VCC		(1<<2)
#define ADC_SYSRED_OFFSET	(1<<0)

#define ADC_GPIOC_CIO7		(1<<7)
#define ADC_GPIOC_CIO6		(1<<6)
#define ADC_GPIOC_CIO5		(1<<5)
#define ADC_GPIOC_CIO4		(1<<4)
#define ADC_GPIOC_CIO3		(1<<3)
#define ADC_GPIOC_CIO2		(1<<2)
#define ADC_GPIOC_CIO1		(1<<1)
#define ADC_GPIOC_CIO0		(1<<0)

#define ADC_GPIOD_DIO7		(1<<7)
#define ADC_GPIOD_DIO6		(1<<6)
#define ADC_GPIOD_DIO5		(1<<5)
#define ADC_GPIOD_DIO4		(1<<4)
#define ADC_GPIOD_DIO3		(1<<3)
#define ADC_GPIOD_DIO2		(1<<2)
#define ADC_GPIOD_DIO1		(1<<1)
#define ADC_GPIOD_DIO0		(1<<0)

#define ADC_ID_ID7			(1<<7)
#define ADC_ID_ID6			(1<<6)
#define ADC_ID_ID5			(1<<5)
#define ADC_ID_ID4			(1<<4)
#define ADC_ID_ID3			(1<<3)
#define ADC_ID_ID2			(1<<2)
#define ADC_ID_ID1			(1<<1)
#define ADC_ID_ID0			(1<<0)

//ADC run configuration
//ADC mode selector - enable one of AUTO_SCAN mode or FIXED_CHANNEL mode
//#define FIXED_CHANNEL_MODE
#define AUTO_SCAN_MODE

#if defined (FIXED_CHANNEL_MODE)
    #define ADC_MUXMOD_VAL ADC_CONFIG0_MUXMOD
#elif defined (AUTO_SCAN_MODE)
    #define ADC_MUXMOD_VAL 0
#endif

#if defined (FIXED_CHANNEL_MODE) && defined (AUTO_SCAN_MODE)
    #error "ADC can operate only in one mode - Fixed channel or Auto Scan"
#endif

//Fixed channel settings
#define FIXED_CHANNEL_P_IN 0
#define FIXED_CHANNEL_N_IN 15

//AutoScan settings
//Select channels for scanning
//Now enabled only AIN0 channel
#define AUTO_SCAN_DIFF_CH  0
#define AUTO_SCAN_SING_CH0 ADC_MUXSG0_AIN0
#define AUTO_SCAN_SING_CH1 0
#define AUTO_SCAN_SYS_CH   0

//ADC voltage coefficient
#define ADC_VOLTAGE_COEFF (5.0/7864320.0)


/* ADC control functions */
int adc_check_connect(void);

void adc_soft_reset(void);

void adc_hard_reset(void);

void adc_config_struct_init(adc_config_t* conf_struct);

int adc_write_config(adc_config_t config);

int adc_check_config(void);

void adc_get_next_ch_val(adc_value_t* adc_value);


#endif
