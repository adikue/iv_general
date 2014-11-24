#include "hal.h"
#include "config.h" //User MCU config
#include "stm32f4xx.h" //CMSIS
#include "misc.h"   //Interrupts
#include "fifo_q.h"

static uint32_t SYSCLK_Frequency; //SYSCLK clock frequency expressed in Hz
static uint32_t HCLK_Frequency;   //HCLK clock frequency expressed in Hz
static uint32_t PCLK1_Frequency;  //PCLK1 clock frequency expressed in Hz
static uint32_t PCLK2_Frequency;  //PCLK2 clock frequency expressed in Hz
static uint8_t clock_initialized = 0; //For propper delay_us() operation

static double next_motor_val = 0;
uint32_t pwm_width = (PWM_PULSE_DEF - 1);//Default throttle = BREAK;
uint8_t pwm_udpated = 0;

MAKE_FIFO( USART_TX , uint8_t , USART1_BUFFER_SIZE );
MAKE_FIFO( TIME_TASKS , motortask_t , MOTORTASK_FIFOSIZE );
static unsigned int tx_restart = 1;               // NZ if TX restart is required

extern int answer_anylyzer(char *);
extern void motor_sensor_callback(uint8_t );
extern int set_motor_throttle(double );

/*---------MISC FUNCTIONS SECTION START---------*/

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

void delay_us(uint32_t us)
{
    uint32_t ticks;
    if (clock_initialized)
        ticks = (SYSCLK_Frequency/1000000) * us;
    else
        ticks = 8 * us;//if clock is not initialized then sysclk is assumed to be 8MHz
    
    while(ticks){ticks--;};
        
}

/*-------MISC FUNCTIONS SECTION START---------*/

/*----------INIT HAL SECTION START------------*/

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
        /* APB1 clock = AHB / 2. 24MHz */
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
        /* APB2 clock = AHB. 48MHz*/
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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|
                           RCC_APB1Periph_TIM5, 
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
    
    //DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM5_STOP;
}

static void gpio_init(void)
{   
    
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
    GPIOA->MODER |= GPIO_MODER_MODER9_1|//Enable AF mode
                    GPIO_MODER_MODER10_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0|//Enable PullUp
                    GPIO_PUPDR_PUPDR10_0;
    //Set AF
    SETAFR(GPIOA,9,7); //USART1_TX
    SETAFR(GPIOA,10,7);//USART1_RX
    //SETAFR(GPIOA,12,7);//USART1_CTS
    //SETAFR(GPIOA,12,7);//USART1_RTS
    
    /* MOTOR ROTARY SENSOR */
    //EXTI 13,14,15 for motor rotary sensor lines
    //Physycal PB13, PB14, PB15
    // Tell system that you will use PB13-15 lines for External interrupts
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PB | SYSCFG_EXTICR4_EXTI14_PB | SYSCFG_EXTICR4_EXTI15_PB;
    //Unmask interrupts
    EXTI->IMR |= EXTI_IMR_MR13|EXTI_IMR_MR14|EXTI_IMR_MR15;
    //Select both triggers - falling and rising edge
    EXTI->RTSR |= EXTI_RTSR_TR13|EXTI_RTSR_TR14|EXTI_RTSR_TR15;
    EXTI->FTSR |= EXTI_FTSR_TR13|EXTI_FTSR_TR14|EXTI_FTSR_TR15;
    
    NVIC_EnableIRQ(EXTI15_10_IRQn);
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
    
    //Setup TIM5 Capture compare.
    TIM5->CCR1 = RT_DATA_SAVE_PERIOD;
    TIM5->DIER |= TIM_DIER_CC1IE;
    
    TIM5->EGR |= TIM_EGR_UG;//Update all registers
    
    NVIC_EnableIRQ(TIM5_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);
}

static void usart1_init(void){
    double usartdiv;
    uint16_t mantissa;
    uint16_t fraction;
    
    usartdiv = PCLK2_Frequency / (16.0 * USART1_BRR);
    mantissa = (uint16_t)(usartdiv) & 0xFFF;
    fraction = (uint16_t)(16.0 * (usartdiv - (double)mantissa) + 0.5) & 0xF;
    
    USART1->BRR = (uint32_t)((mantissa << 4) | fraction);
    
    //USART1->CR3 |= USART_CR3_CTSE|USART_CR3_RTSE;
    
    USART1->CR1 |= USART_CR1_RXNEIE;
    USART1->CR1 |= USART_CR1_TE|USART_CR1_RE|USART_CR1_UE;

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

/*----------INIT HAL SECTION START------------*/

/*-------MOTOR SENSOR HAL SECTION START-------*/

void EXTI15_10_IRQHandler(void){
    uint32_t IRR = EXTI->PR;
    uint8_t ABC_hall = 0;
    
    //If interrupt was triggered by PB13,PB14,P15 line
    if ((IRR & EXTI_PR_PR13) || (IRR & EXTI_PR_PR14) || (IRR & EXTI_PR_PR15)){
        //Clear pending bit
        EXTI->PR |= EXTI_PR_PR13|EXTI_PR_PR14|EXTI_PR_PR15;//Write 1 clears bit in register
        
        //Process Hall sensors data
        //Form byte of ABC data
        ABC_hall = (uint8_t)((GPIOB->IDR >> 13) & 0x7);
        motor_sensor_callback(ABC_hall);
    }
}


/*--------MOTOR SENSOR HAL SECTION END--------*/


/*---------TIMERS HAL SECTION START-----------*/

void start_timer5(void){
    TIM5->CR1 |= TIM_CR1_CEN;//Start TIM5
}

void stop_timer5(void){
    TIM5->CR1 &= ~TIM_CR1_CEN;//Stop TIM5
}

void reset_timer5(void){
    stop_timer5();
    
    TIM5->ARR = 0xFFFFFFFF;
    TIM5->CNT = 0;
    
    TIM5->CCR1 = RT_DATA_SAVE_PERIOD;
    
    TIM5->CCR2 = 0;
    
    TIM5->DIER &= ~TIM_DIER_CC2IE;
    TIME_TASKS_flush();
    
    TIM5->EGR |= TIM_EGR_UG;//Update all registers
    
}

static void setup_timer_task(motortask_t task){
    if (TIM5->CNT < task.stamp + 10){
        TIM5->CCR2 = task.stamp;
        next_motor_val = task.throttle;
        
        TIM5->DIER |= TIM_DIER_CC2IE;
    }
}

int add_timer_task(motortask_t new_task){
    //If Timer channel is not busy
    if( !(TIM5->DIER & TIM_DIER_CC2IE) ){
        setup_timer_task(new_task);   
        return 0;
    }else{
        if (!TIME_TASKS_isfull()){
            TIME_TASKS_push(new_task);
            return 0;
        }
    }
    return -1;
}

uint32_t get_timer5_cnt(void){
    return TIM5->CNT;
}

void TIM2_update_PWM(void){
    TIM2->DIER |= TIM_DIER_UIE;//Update interrupt event
}

void TIM2_IRQHandler(void){
    if (TIM2->SR & TIM_SR_UIF){//If counter update happened
        TIM2->SR &= ~TIM_SR_UIF;//Clear interrupt flag
        TIM2->CCR2 = pwm_width;//Apply new pulse width
        pwm_udpated = 1;
        TIM2->DIER &= ~TIM_DIER_UIE;//Disable interrupt
    }
}

void TIM5_IRQHandler(void){
    if (TIM5->SR & TIM_SR_CC1IF){//If log period elapsed
        TIM5->SR &= ~TIM_SR_CC1IF;//Clear interrupt flag
        //TODO: dump all sensetive data to SD
        
        TIM5->CCR1 = TIM5->CNT + RT_DATA_SAVE_PERIOD;
    }
    
    if (TIM5->SR & TIM_SR_CC2IF){
        TIM5->SR &= ~TIM_SR_CC2IF;//Clear interrupt flag
        
        //Execute task
        set_motor_throttle(next_motor_val);
        
        if(!TIME_TASKS_isempty()){
            setup_timer_task(TIME_TASKS_pop());
        }else{
            TIM5->DIER &= ~TIM_DIER_CC2IE;
        }
    }
}

/*----------TIMERS HAL SECTION END------------*/

/*----------LEDS HAL SECTION START------------*/

void led_on(stm_led_t led, uint32_t us){
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


void led_off(stm_led_t led){
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

void change_sd_led(void){
    static uint32_t div;
    if (div == 10){
        GPIOC->ODR ^= GPIO_ODR_ODR_5;
        div = 0;
    }
    div++;
}

/*---------LEDS HAL SECTION END-----------*/

/*--------USART HAL SECTION START---------*/

void USART1_IRQHandler (void) {
    volatile unsigned int IIR = USART1->SR;
    uint8_t buf;
    
    if (IIR & USART_FLAG_RXNE){// read interrupt
        USART1->SR &= ~USART_FLAG_RXNE;// clear interrupt

        buf = USART1->DR;
        //USART_RX_push(buf);
        answer_anylyzer((char*)&buf);
    }

    if (IIR & USART_FLAG_TXE) {
        USART1->SR &= ~USART_FLAG_TXE;// clear interrupt

        if (!USART_TX_isempty()){
            buf = USART_TX_pop();
            USART1->DR = buf;
            tx_restart = 0;
        }else {
            tx_restart = 1;
            USART1->CR1 &= ~USART_FLAG_TXE;// disable TX interrupt if nothing to send
        }
    }
}

int add_to_usart_tx(uint8_t data){
    if (!USART_TX_isfull()){
        USART_TX_push(data);
        if (tx_restart) {// If transmit interrupt is disabled, enable it
            tx_restart = 0;
            USART1->CR1 |= USART_FLAG_TXE;// enable TX interrupt
        }
        return 0;
    }
    
    return -1;
}

int usart_tx_isempty(void){
    return USART_TX_isempty();
}

/*---------USART HAL SECTION END----------*/
