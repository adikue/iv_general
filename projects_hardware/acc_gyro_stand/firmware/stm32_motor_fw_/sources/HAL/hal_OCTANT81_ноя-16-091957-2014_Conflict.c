#include "hal.h"
#include "config.h" //User MCU config
#include "stm32f4xx.h" //CMSIS
#include "misc.h"   //Interrupts
#include "fifo_q.h"

uint32_t SYSCLK_Frequency; //SYSCLK clock frequency expressed in Hz
uint32_t HCLK_Frequency;   //HCLK clock frequency expressed in Hz
uint32_t PCLK1_Frequency;  //PCLK1 clock frequency expressed in Hz
uint32_t PCLK2_Frequency;  //PCLK2 clock frequency expressed in Hz
uint8_t clock_initialized = 0; //For propper delay_us() operation

uint32_t timestamp = 0;
uint32_t pwm_width = (PWM_PULSE_DEF - 1);//Default throttle = BREAK;
uint8_t pwm_udpated = 0;

MAKE_FIFO( USART_TX , uint8_t , USART1_BUFFER_SIZE );
MAKE_FIFO( USART_RX , uint8_t , USART1_BUFFER_SIZE );

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
            led_on(SD_STATUS_LED, 100000);
            delay_us(100000);
        }
    }
    if(err == STM_ERROR){
        //Blink 1 time
        while(1){
            led_on(STM_STATUS_LED, 100000);
            delay_us(100000);
        }
    }
}

void delay_us( uint32_t us)
{
    uint32_t ticks;
    if (clock_initialized)
        ticks = (SYSCLK_Frequency/1000000) * us;
    else
        ticks = 8 * us;//if clock is not initialized then sysclk is assumed to be 8MHz
    
    while(ticks){ticks--;};
        
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
        error_loop(STM_ERROR);
    }

    //Enable peripheral clocking
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|
                           RCC_AHB1Periph_GPIOB|
                           RCC_AHB1Periph_GPIOC,
                           ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, 
                           ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG|
                           RCC_APB2Periph_SDIO|
                           RCC_APB2Periph_USART1, 
                           ENABLE);

    //Store selected frequencies
    RCC_GetClocksFreq(&rcc_clocks);
    SYSCLK_Frequency = rcc_clocks.SYSCLK_Frequency; 
    HCLK_Frequency = rcc_clocks.HCLK_Frequency;   
    PCLK1_Frequency = rcc_clocks.PCLK1_Frequency;  
    PCLK2_Frequency = rcc_clocks.PCLK2_Frequency;
    clock_initialized  = 1;
}

static void gpio_init(void)
{   
    GPIO_InitTypeDef port;
    
    /* stm_ok led pin. PB0 */
    GPIOB->MODER |= GPIO_MODER_MODER0_0;//Set as output
    GPIOB->ODR &= ~GPIO_ODR_ODR_0;//Set off
    /* stm_status led pin. PC1 */
    GPIOC->MODER |= GPIO_MODER_MODER1_0;//Set as output
    GPIOC->ODR &= ~GPIO_ODR_ODR_1;//Set off
    /* sd_status led pin. PC5 */
    GPIOC->MODER |= GPIO_MODER_MODER5_0;//Set as output
    GPIOC->ODR &= ~GPIO_ODR_ODR_5;//Set off
                    
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
    
    /* MOTOR PWM */
    SETAFR(GPIOB,3,1);//PWM pin - PB3. T2C2
    
    /* USART1. Wi-Fi */
    //PA9.PA10.PA11.PA12
    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_AF;
    port.GPIO_Pin = GPIO_Pin_9;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);
 
    port.GPIO_Mode = GPIO_Mode_IN;
    port.GPIO_Pin = GPIO_Pin_10;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);
    SETAFR(GPIOA,9,7); //USART1_TX
    SETAFR(GPIOA,10,7);//USART1_RX
    //SETAFR(GPIOA,12,7);//USART1_CTS
    //SETAFR(GPIOA,12,7);//USART1_RTS
}

static void timer_init(void){
    
    //TIM2 - PWR timer
    TIM2->PSC = PCLK2_Frequency/PWM_TIMER_CLK - 1;//Set timer clock
    TIM2->ARR = PWM_PERIOD - 1;//Set timer period to 20ms
    
    //Set PWM mode 1,CCR2 preload enable
    TIM2->CCMR1 |= TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_1|TIM_CCMR1_OC2PE;
    TIM2->CR1 |= TIM_CR1_ARPE;//Timer ARR register is buffered
    TIM2->CCER |= TIM_CCER_CC2E;//Enable C2 output
    TIM2->CCR2 |= (uint32_t)(PWM_PULSE_DEF - 1);//Set pulse width 1.5ms
  
    TIM2->EGR |= TIM_EGR_UG;//Update all registers
    
    TIM2->CR1 |= TIM_CR1_CEN;//Start PWM
    
    //TIM5 - timestamp counter
    TIM5->PSC = PCLK2_Frequency/1000000 - 1;//Set timer clock to 1MHz
    TIM5->ARR = 0xFFFFFFFF;//Set timer overload to MAX
    TIM5->EGR |= TIM_EGR_UG;//Update all registers
    
    
    
    NVIC_EnableIRQ(TIM2_IRQn);
}

static void usart1_init(void){
    double usartdiv;
    uint16_t mantissa;
    uint16_t fraction;
    USART_InitTypeDef usart;
    
    USART_StructInit(&usart);
    
    usartdiv = PCLK2_Frequency / (16.0 * USART1_BRR);
    mantissa = (uint16_t)(usartdiv) & 0xFFF;
    fraction = (uint16_t)(16.0 * (usartdiv - (double)mantissa) + 0.5) & 0xF;
    
    USART1->BRR = (uint32_t)((mantissa << 4) | fraction);
    
    //USART1->CR3 |= USART_CR3_CTSE|USART_CR3_RTSE;
    
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_RXNEIE|USART_CR1_TXEIE|USART_CR1_TCIE;
    USART1->CR1 |= USART_CR1_UE;
    
    usart.USART_BaudRate = USART1_BRR;
    USART_Init(USART1, &usart);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    
    USART_Cmd(USART1, ENABLE);
    
    NVIC_EnableIRQ(USART1_IRQn);
}

void init()
{
    __disable_irq();

    clock_init();
    gpio_init();
    timer_init();
    usart1_init();

    __enable_irq();
}


void led_on(led_t led, uint32_t us){
    __IO uint32_t *led_odr;
    uint32_t pin_odr;
    
    switch (led){
        case STM_OK_LED:
            /* stm_ok led pin. PB0 */
            led_odr = &GPIOB->ODR;
            pin_odr = GPIO_ODR_ODR_0;
            break;
        case STM_STATUS_LED:
            /* stm_status led pin. PC1 */
            led_odr = &GPIOC->ODR;
            pin_odr = GPIO_ODR_ODR_1;
            break;
        case SD_STATUS_LED:
            /* sd_status led pin. PC5 */
            led_odr = &GPIOC->ODR;
            pin_odr = GPIO_ODR_ODR_5;
            break;
        default:
            return;     
    }
    
    *led_odr |= pin_odr;// Light ON
    if (us > 0){
        delay_us(us);//Delay
        *led_odr &= ~pin_odr;//Light OFF
    }
}


void led_off(led_t led){
    __IO uint32_t *led_odr;
    uint32_t pin_odr;
    
    switch (led){
        case STM_OK_LED:
            /* stm_ok led pin. PB0 */
            led_odr = &GPIOB->ODR;
            pin_odr = GPIO_ODR_ODR_0;
            break;
        case STM_STATUS_LED:
            /* stm_status led pin. PC1 */
            led_odr = &GPIOC->ODR;
            pin_odr = GPIO_ODR_ODR_1;
            break;
        case SD_STATUS_LED:
            /* sd_status led pin. PC5 */
            led_odr = &GPIOC->ODR;
            pin_odr = GPIO_ODR_ODR_5;
            break;
        default:
            return;
    }

    *led_odr &= ~pin_odr;
}

void TIM2_update_PWM(void){
    TIM2->DIER |= TIM_DIER_UIE;//Enable interrupt
}

void start_timer5(void){
    TIM5->CR1 |= TIM_CR1_CEN;//Start TIM5
}

void stop_timer5(void){
    TIM5->CR1 &= ~TIM_CR1_CEN;//Start TIM5
}

void reset_timer5(void){
    stop_timer5();
    TIM5->ARR = 0xFFFFFFFF;
    TIM5->CNT = 0;
    TIM5->EGR |= TIM_EGR_UG;//Update all registers
}


void TIM2_IRQHandler(void){
    if (TIM2->SR & TIM_SR_UIF){//If counter update happened
        TIM2->SR = TIM_SR_UIF;//Clear interrupt flag
        TIM2->CCR2 = pwm_width;//Apply new pulse width
        pwm_udpated = 1;
        TIM2->DIER &= ~TIM_DIER_UIE;//Disable interrupt
    }
}

void USART1_IRQHandler(void){
    uint16_t _SR = USART1->SR;
    uint8_t buf = 0;
    
    if (_SR & USART_SR_TXE){
        if (!USART_TX_isempty()){
            buf = (uint16_t)USART_TX_pop();
            USART1->DR = buf;
        }else if (_SR & USART_SR_TC){
            USART1->CR1 &= ~USART_CR1_TXEIE;
            USART1->CR1 &= ~USART_CR1_TCIE;
        }
    }
    if (_SR & USART_SR_RXNE){
        if(!USART_RX_isfull()){
            buf = USART1->DR;
            USART_RX_push(buf);
        }else{
            //If rx buffer is full. Then 
            USART_RX_pop();//Throw away one byte from buffer
            USART_RX_push((uint8_t)USART1->DR);//Store received byte
        }
    } 
}

int add_to_usart_tx(uint8_t data){
    if (!USART_TX_isfull()){
        USART_TX_push(data);
        USART1->CR1 |= USART_CR1_TXEIE;
        USART1->CR1 |= USART_CR1_TCIE;
        return 0;
    }
    
    return -1;
}

int get_from_usart_rx(uint8_t* data){
    if (!USART_RX_isempty()){
        *data = USART_RX_pop();
        return 0;
    }
    
    return -1;    
}

void flush_usart_rx(void){
    USART_RX_flush();
}

