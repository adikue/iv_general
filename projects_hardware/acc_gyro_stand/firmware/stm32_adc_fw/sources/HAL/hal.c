#include "hal.h"
#include "config.h"
#include "stm32f4xx.h"
#include "misc.h"

uint32_t SYSCLK_Frequency; //SYSCLK clock frequency expressed in Hz
uint32_t HCLK_Frequency;   //HCLK clock frequency expressed in Hz
uint32_t PCLK1_Frequency;  //PCLK1 clock frequency expressed in Hz
uint32_t PCLK2_Frequency;  //PCLK2 clock frequency expressed in Hz
uint8_t clock_initialized = 0; //For propper delay_us() operation

uint32_t timestamp = 0;

static void SETAFR(GPIO_TypeDef *port, uint8_t pin, uint8_t afr_n)
{
    uint32_t AFR = 0;
    
    if (pin >= 8){//Use GPIOx_AFRH
        AFR = port->AFR[1];//Get current GPIOx_AFRH
        AFR &= ~(0x0000000F << (4*(pin-8))); //Set ZERO current AFR settings
        AFR |= ((uint32_t)(afr_n&0xF)) << (4*(pin-8));//Set afr_n in ARH word
        port->AFR[1] = AFR;//Write afr settings
    } else {
        AFR = port->AFR[0];//Get current GPIOx_AFRL
        AFR &= ~(0x0000000F << (4*(pin-8))); //Set ZERO current AFR settings
        AFR |= ((uint32_t)(afr_n&0xF)) << (4*pin);//Set afr_n in ARH word
        port->AFR[0] = AFR;//Write afr settings
    }
}

void error_loop(error_t err){
    if(err == SD_ERROR){
        //Blink 1 time
        while(1){
            led_on(100000);
            
            delay_us(100000);
        }
    }
    if(err == ADC_ERROR){
        //Blink 2 times
        while(1){
            led_on(15000);
            delay_us(15000);
            led_on(15000);
            
            delay_us(100000);
        }
    }
}

void delay_us(__IO uint32_t us)
{
    uint32_t ticks;
    if (clock_initialized)
        ticks = (SYSCLK_Frequency/1000000) * us;
    else
        ticks = 8 * us;//if clock is not initialized then sysclk is assumed to be 8MHz
    
    while(ticks){ticks--;};
        
}

void init()
{
    __disable_irq();

    clock_init();
    gpio_init();
    spi_init();

    __enable_irq();
}


static void clock_init(){
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    RCC_ClocksTypeDef rcc_clocks;
	
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif
  /* Reset the RCC clock configuration to the default reset state ------------*/
  //Set HSION bit
  RCC->CR |= (uint32_t)0x00000001;
  //Reset CFGR register
  RCC->CFGR = 0x00000000;
  //Reset HSEON, CSSON and PLLON bits
  RCC->CR &= (uint32_t)0xFEF6FFFF;
  //Reset PLLCFGR register
  RCC->PLLCFGR = 0x24003010;
  //Reset HSEBYP bit
  RCC->CR &= (uint32_t)0xFFFBFFFF;
  //Disable all interrupts
  RCC->CIR = 0x00000000;
  
  //Enable HSE
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do{
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  }while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET){
    HSEStatus = (uint32_t)0x01;
  }else{
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01){
    /* Select regulator voltage output Scale 1 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

	/* Configure the main PLL
    * HSE oscillator clock selected as PLL clock entry
    * 8 - PLLM: specifies 2MHz PLL VCO input clock
	* 192 - PLLN: specifies 384MHz VCO output clock
	* 8 - PLLP: specifies the 384/8 = 48MHz system clock
	* 8 - PLLQ: specifies the 384/8 = 48MHz OTG FS, SDIO and RNG clocks
	* Constants defined in 'config.h' */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
		
	/* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0){}
		
	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
    
	/* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;
	//PLL selected as system clock source (SYSCLK). 48MHz
		
	/* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL){}
		
	/* AHB clock = SYSCLK. 48MHz */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    /* APB1 clock = HCLK / 2. 24MHz */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    /* APB2 clock = HCLK. 48MHz*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;		
  }else{ 
      /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
      //TODO: harware fault handler
  }
  
    //Enable peripheral clocking
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|
                           RCC_AHB1Periph_GPIOC|
                           RCC_AHB1Periph_GPIOD,
                           ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG|RCC_APB2Periph_SDIO, ENABLE);
	
	//Store selected frequencies
	RCC_GetClocksFreq(&rcc_clocks);
	SYSCLK_Frequency = rcc_clocks.SYSCLK_Frequency; 
	HCLK_Frequency = rcc_clocks.HCLK_Frequency;   
	PCLK1_Frequency = rcc_clocks.PCLK1_Frequency;  
	PCLK2_Frequency = rcc_clocks.PCLK2_Frequency;
	clock_initialized  = 1;
}

void timer_config_struct_init(timer_config_t* config_struct){
    config_struct->scale = TIMER_US_SCALE;
    config_struct->period = 1;
}

void init_timer(timer_config_t conf){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
    //APB1 clock = 24MHz
    if (conf.scale == TIMER_MS_SCALE)
        TIM6->PSC = 12000 - 1; // Timer ticks 12000 time per second
    if (conf.scale == TIMER_US_SCALE)
        TIM6->PSC = 12 - 1; // Timer ticks 12000000 time per second
    TIM6->ARR = 11 * conf.period; // We want interrupt every period
    TIM6->DIER |= TIM_DIER_UIE; //Enables timer interrupt
    TIM6->EGR |= TIM_EGR_UG;
    NVIC_EnableIRQ(TIM6_DAC_IRQn); //Enable TIM6_DAC_IRQn processing
}

void start_tmier(void)
{
    TIM6->CR1 |= TIM_CR1_CEN; // Enable counter
}

void stop_timer(void)
{
    TIM6->CR1 &= ~TIM_CR1_CEN; // Disable counter
}

uint32_t get_timestamp(void){
    return timestamp;
}


static void gpio_init(void)
{   
    /* LED0 pin. PB0 */
    GPIOB->MODER |= GPIO_MODER_MODER0_0;//Set as output
    GPIOB->ODR &= ~GPIO_ODR_ODR_0;//Set off
    
    /* ADC Control */
    //nDRDY. PB6
    GPIOB->MODER &= ~(GPIO_MODER_MODER6_0|GPIO_MODER_MODER6_1);//Set as input
    //GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0;
    EXTI->IMR |= EXTI_IMR_MR6;//Unmask interrupt line 
    EXTI->FTSR |= EXTI_FTSR_TR6;//Interrupt on line 6 set for falling edge
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR2_EXTI6_PB;//Unmask and enable interrupt on PC2 for line 2
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    
    //START. PB7
    GPIOB->MODER |= GPIO_MODER_MODER7_0;//Set as output
    //nRESET. PB8
    GPIOB->MODER |= GPIO_MODER_MODER8_0;//Set as output
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR8_0;//Set pullup
    //nPWDN. PB9
    GPIOB->MODER |= GPIO_MODER_MODER9_0;//Set as output
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR9_0;//Set pullup
    //CLKIO. PB10
    //GPIOB->MODER &= ~(GPIO_MODER_MODER10_0|GPIO_MODER_MODER10_1);//Set as input of ADC clk
    //CLKSEL. PB11
    //GPIOB->MODER |= GPIO_MODER_MODER11_0;//Set as output
    //GPIOB->ODR &= ~GPIO_ODR_ODR_11;//Set onboard crystal as clock source for ADC
    //GPIOB->ODR |= GPIO_ODR_ODR_8|GPIO_ODR_ODR_9;//Set high for lines //nRESET and //nPWDN
    
    /* SPI 2 */
    //SETAFR(GPIOB,12,5);//NSS
    GPIOB->MODER |= GPIO_MODER_MODER12_0;//Set as output
    SETAFR(GPIOB,13,5);//SCK
    SETAFR(GPIOB,14,5);//MISO-DOUT
    SETAFR(GPIOB,15,5);//MOSI-DIN
    GPIOB->MODER |= GPIO_MODER_MODER13_1|GPIO_MODER_MODER14_1|GPIO_MODER_MODER15_1;//Set mode to AF
                    
    /* SDIO */
    SETAFR(GPIOC,8,12);//SDIO_0
    SETAFR(GPIOC,9,12);//SDIO_1
    SETAFR(GPIOC,10,12);//SDIO_2
    SETAFR(GPIOC,11,12);//SDIO_3
    SETAFR(GPIOC,12,12);//SDIO_CLK
    SETAFR(GPIOD,2,12);//SDIO_CMD
    GPIOC->MODER |= GPIO_MODER_MODER8_1|GPIO_MODER_MODER9_1
                    |GPIO_MODER_MODER10_1|GPIO_MODER_MODER11_1
                    |GPIO_MODER_MODER12_1;//Set mode to AF
    GPIOD->MODER |= GPIO_MODER_MODER2_1;//Set mode to AF
}

void led_on(uint32_t us){
    GPIOB->ODR |= GPIO_ODR_ODR_0;
    if (us > 0){
        delay_us(us);
        led_off();
    }
}

void led_off(){
    GPIOB->ODR &= ~GPIO_ODR_ODR_0;
}

////Interupt handler for /DRDY line.
//void EXTI9_5_IRQHandler(void)
//{
//    uint8_t buf[5];
//    EXTI->PR |= EXTI_PR_PR6;//Reset interrupt penging flag
//    
//    //spi_read_channel(buf);
//}

//Timer interrupt
void TIM6_DAC_IRQHandler(void)
{
  TIM6->SR &= ~TIM_SR_UIF; //Clear flag UIF
  timestamp++;
}
