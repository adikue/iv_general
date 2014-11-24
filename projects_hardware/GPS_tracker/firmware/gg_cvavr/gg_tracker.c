/*
* tracker.c
*
* Created: 29.10.2012 12:47:26
* Author: ivovk
*/
#define F_CPU 8000000

#include <delay.h>
//#include <avr/interrupt.h>
#include <stdbool.h>
// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x1B ;PORTC
#endasm
#include <lcd.h>
#include <mega162.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define E  		 	   	  		      2 // pin number in selected port
#define RS 		  	   	  		      0 // pin number in selected port
#define D7 		 	   	  		      7 // pin number in selected port
#define D6 		 	   	  		      6 // pin number in selected port
#define D5 		 	   	  		      5 // pin number in selected port
#define D4 	                          4 //

char buffer[180];



//===================================
//Hardware initialization and reset
//===================================

/**
Procedure of hardware initialization of atmega controller
*/
void initialization(){
#asm ("cli")
// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization                                                                                               
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA = (1<<E)|(1<<RS)|(1<<D7)|(1<<D6)|(1<<D5)|(1<<D4);

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Port E initialization
// Func2=Out Func1=Out Func0=Out 
// State2=0 State1=0 State0=0 
PORTE=0x00;
DDRE=0x07;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// Timer/Counter 3 initialization
// Clock value: Timer3 Stopped
// Mode: Normal top=0xFFFF
// OC3A output: Discon.
// OC3B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR3A=0x00;
TCCR3B=0x00;
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// Interrupt on any change on pins PCINT0-7: Off
// Interrupt on any change on pins PCINT8-15: Off
MCUCR=0x00;
EMCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

ETIMSK=0x00;

// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud Rate: 115200
UCSR0A=0x00;
UCSR0B=0x18;
UCSR0C=0x86;
UBRR0H=0x00;
UBRR0L=0x03;

// USART1 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART1 Receiver: On
// USART1 Transmitter: On
// USART1 Mode: Asynchronous
// USART1 Baud Rate: 115200
UCSR1A=0x00;
UCSR1B=0x18;
UCSR1C=0x86;
UBRR1H=0x00;
UBRR1L=0x03;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;

// SPI initialization
// SPI disabled
SPCR=0x00;
// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTA Bit 0
// RD - PORTA Bit 1
// EN - PORTA Bit 2
// D4 - PORTA Bit 4
// D5 - PORTA Bit 5
// D6 - PORTA Bit 6
// D7 - PORTA Bit 7
// Characters/line: 16
lcd_init(16);  //*/
#asm ("sei")
}


#pragma used-

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)
// Get a character from the USART1 Receiver
#pragma used+

char getchar1(void)
{
char status,data;
while (1)
      {
      while (((status=UCSR1A) & RX_COMPLETE)==0);
      data=UDR1; 
//      lcd_clear();  
//      if (FRAMING_ERROR) lcd_putsf("FRAMING_ERROR") ;     
//      if (PARITY_ERROR) lcd_putsf("PARITY_ERROR") ;
//      if (DATA_OVERRUN) lcd_putsf("DATA_OVERRUN") ;
//      
//      delay_ms(400);
      if ((status )) //& (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
         return data;
      }
}
#pragma used-


// Write a character to the USART1 Transmitter
#pragma used+
void putchar1(char c)
{
while ((UCSR1A & DATA_REGISTER_EMPTY)==0);
UDR1=c;
}

/**
Send reset signal to GPS chip
*/
void reset_gps(){
	uint8_t UCSR1B_store = UCSR1B;//Save state of UCSR1B
	while( !( UCSR1A & (1<<UDRE1)) ){};//Wait for empty USART1 transmit buffer
	UCSR1B &= ~(0x98);//Disable USART1 transmitter and receiver and interruption

	DDRB = 0x9;// PB3 and PB0 set as out;
	PORTB = 0x1;//Set HIGH level on /reset pin (PB0)
	//TX1 pin is in Z state
	delay_ms(1);

	PORTB = 0;//Set LOW level on /reset pin (PB0) for 15ms
	delay_ms(15);

	PORTB = 0x1;//Set HIGH level on /reset pin (PB0)
	delay_ms(25);//Wait gps chip reset and set up to work mode

	UCSR1B = UCSR1B_store;//Restore state of UCSR1B
}

/**
Send emergency reset signal to GSM chip
*/
void e_reset_gsm(){
	uint8_t UCSR0B_store = UCSR0B;//Save state of UCSR0B
	while( !( UCSR0A & (1<<UDRE0)) ){};//Wait for empty USART1 transmit buffer
	UCSR0B &= ~(0x18);//Disable USART1 transmitter and receiver
	
	DDRD = 0x16;// PD1 and PD2 and PD3 are out (TX, RTS, DTR)
	DDRE = 0x3;//PE0 and PE1 are out (Reset_gsm, IGN)
	delay_ms(1);
	
	PORTE |= 0x2; //Set '1' on Reset_gsm pin
	delay_ms(15);
	
	PORTE &= ~0x2;//Set Reset_gsm pin back to '0'
	delay_ms(500);//Wait 0.5s for GSM module set up
		
	UCSR0B = UCSR0B_store;//Restore state of UCSR0B
}

/**
Send ignition signal to GSM chip
Block execution for 8 sec!
*/
void gsm_on(){
	PORTE |= 0x1;//Set PE0 to '1' for 10ms
	delay_ms(100);
	PORTE &= ~0x1;//Set PE0 back to '0'
	delay_ms(80);
}

//===================================
//GSM chip processing
//===================================

/**
Set all settings to necessary in GSM chip
*/
uint8_t set_settings_gsm(){
//	uint8_t i = 0;
//	ClearRxBuf();
//
//	sei();//Enable interruptions. Need for correct working of UART0(GSM) receiver
//	
//	ClearRxBuf();
//	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY) ){
//		ClearRxBuf();
//		Send_AT_cmd("AT+IPR=115200\n\r", 1);//Setup fixed UART speed in gsm
//		i++;
//	}		
//
//	ClearRxBuf();
//	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 2) ){
//		ClearRxBuf();
//		Send_AT_cmd("ATE0\n\r", 1);//Disable echo mode
//		i++;
//	}
//
//	ClearRxBuf();
//	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 3) ){
//		ClearRxBuf();
//		Send_AT_cmd("AT+CLCK=", 0);//Disable PIN code blocking. <AT+CLCK="SC",0,"PIN">
//		Send_AT_cmd(quote, 0); 
//		Send_AT_cmd("SC", 0);
//		Send_AT_cmd(quote, 0);
//		Send_AT_cmd(",0,", 0);
//		Send_AT_cmd(quote, 0);
//		Send_AT_cmd(PIN, 0);
//		Send_AT_cmd(quote, 0);
//		Send_AT_cmd("\n\r", 1);
//		i++;
//	}
//
//	ClearRxBuf();
//	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 4) ){
//		ClearRxBuf();
//		Send_AT_cmd("AT\\Q3\n\r", 1);//Enable hardware data flow control
//		i++;
//	}
//	
//	ClearRxBuf();
//	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 5) ){
//		ClearRxBuf();
//		Send_AT_cmd("AT+CMGF=1\n\r", 1);//Set text mode for sms sending
//		i++;
//	}
//
//	
//	//ClearRxBuf();
//	//while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 5) ){
//	//	ClearRxBuf();
//	//	Send_AT_cmd("AT^SSET=1\n\r", 1);//Enable URC <^SSIM READY>
//	//	i++;
//	//}
//
//	if(i == TRY * 5)
//		return -1;//All settings wasn't set successfully
//	else
		return 0;//All settings was set successfully
}

bool Gsm_Ready(){
	if(PIND & 0x28)//CTS(clear to send) DSR(always '1' by default)
		return true;
	else
		return false;
}


//===================================
//MAIN function
//===================================

void main(void){	
	    int i, j;
        char set[21] = "$PSTMSETPAR,1102,8*\n";
        char save[14] = "$PSTMSAVEPAR\n";  


	#asm ("cli")	
	initialization();
	
	reset_gps();  
	
	//gsm_on();
	
	//set_settings_gsm();

    lcd_clear();
	lcd_putsf("Hello_World!!\n123456789.");
	delay_ms(2000);            
    lcd_clear();  
//	reset_gps();
    //get_gps_data();
//    i=0;    
//    for (i = 0; i < 21; i++) {
//        putchar1(set[i]);
//        delay_ms(1);
//    }   
//    for (i = 0; save[i] != '\n'; i++) {
//        putchar1(save[i]);
//        delay_ms(1);
//    }   
	while(1){ 
        for (i = 0; i < 32; i++) 
        {
            buffer[i] = getchar1();
            lcd_putchar(buffer[i]);
            
        }
        delay_ms(1000);
        lcd_clear();
        
	}
}