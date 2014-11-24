#ifndef DEFINES_H
#define DEFINES_H

//Put your global defines for all libraries here used in your project


//Activate SDIO 4-bit mode
#define FATFS_SDIO_4BIT         1

#define ADC_DIF0_FILENAME       "adc_ch_DIF0.txt"
#define ADC_DIF1_FILENAME       "adc_ch_DIF1.txt"
#define ADC_DIF2_FILENAME       "adc_ch_DIF2.txt"
#define ADC_DIF3_FILENAME       "adc_ch_DIF3.txt"
#define ADC_DIF4_FILENAME       "adc_ch_DIF4.txt"
#define ADC_DIF5_FILENAME       "adc_ch_DIF5.txt"
#define ADC_DIF6_FILENAME       "adc_ch_DIF6.txt"
#define ADC_DIF7_FILENAME       "adc_ch_DIF7.txt"

#define ADC_AIN0_FILENAME       "adc_ch_AIN0.txt"
#define ADC_AIN1_FILENAME       "adc_ch_AIN1.txt"
#define ADC_AIN2_FILENAME       "adc_ch_AIN2.txt"
#define ADC_AIN3_FILENAME       "adc_ch_AIN3.txt"
#define ADC_AIN4_FILENAME       "adc_ch_AIN4.txt"
#define ADC_AIN5_FILENAME       "adc_ch_AIN5.txt"
#define ADC_AIN6_FILENAME       "adc_ch_AIN6.txt"
#define ADC_AIN7_FILENAME       "adc_ch_AIN7.txt"
#define ADC_AIN8_FILENAME       "adc_ch_AIN8.txt"
#define ADC_AIN9_FILENAME       "adc_ch_AIN9.txt"
#define ADC_AIN10_FILENAME      "adc_ch_AIN10.txt"
#define ADC_AIN11_FILENAME      "adc_ch_AIN11.txt"
#define ADC_AIN12_FILENAME      "adc_ch_AIN12.txt"
#define ADC_AIN13_FILENAME      "adc_ch_AIN13.txt"
#define ADC_AIN14_FILENAME      "adc_ch_AIN14.txt"
#define ADC_AIN15_FILENAME      "adc_ch_AIN15.txt"

#define ADC_OFFSET_FILENAME     "adc_ch_OFFST.txt"
#define ADC_VCC_FILENAME        "adc_ch_VCC__.txt"
#define ADC_TEMP_FILENAME       "adc_ch_TEMP_.txt"
#define ADC_GAIN_FILENAME       "adc_ch_GAIN_.txt"
#define ADC_REF_FILENAME        "adc_ch_REF__.txt"

#define DATA_FILENAMES {ADC_DIF0_FILENAME,\
                        ADC_DIF1_FILENAME,\
                        ADC_DIF2_FILENAME,\
                        ADC_DIF3_FILENAME,\
                        ADC_DIF4_FILENAME,\
                        ADC_DIF5_FILENAME,\
                        ADC_DIF6_FILENAME,\
                        ADC_DIF7_FILENAME,\
                        ADC_AIN0_FILENAME,\
                        ADC_AIN1_FILENAME,\
                        ADC_AIN2_FILENAME,\
                        ADC_AIN3_FILENAME,\
                        ADC_AIN4_FILENAME,\
                        ADC_AIN5_FILENAME,\
                        ADC_AIN6_FILENAME,\
                        ADC_AIN7_FILENAME,\
                        ADC_AIN8_FILENAME,\
                		ADC_AIN9_FILENAME,\
                		ADC_AIN10_FILENAME,\
                		ADC_AIN11_FILENAME,\
                		ADC_AIN12_FILENAME,\
                		ADC_AIN13_FILENAME,\
                		ADC_AIN14_FILENAME,\
                		ADC_AIN15_FILENAME,\
                		ADC_OFFSET_FILENAME,\
                		ADC_VCC_FILENAME,\
                		ADC_TEMP_FILENAME,\
                		ADC_GAIN_FILENAME,\
                		ADC_REF_FILENAME}

#define ADC_CONFIG_FILENAME     "adc_config.txt"

#define TIMER_CONFIG_FILENAME   "timer_config.txt"

#endif
