/*
* tracker.c
*
* Created: 29.10.2012 12:47:26
* Author: ivovk
*/
#define F_CPU 8000000UL

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include "hd44780.h"

#define USART_BAUDRATE 115200//Bits per second
#define UBRR_VALUE ((F_CPU/(USART_BAUDRATE * 16UL)) - 1)//Asynchronous Normal Mode
#define RXBUFLENGTH_GSM 512//Length of buffer of UART0 RX. Buffer for data from gsm
#define PIN 0000
#define TRY 10

typedef struct{
	char timestamp[7];//hhmmss
	char lat[11];//DDMM.MMMMM
	char n_s[2];//N/w
	char longi[11];//DDMM.MMMMM
	char e_w[2];//E/W
	char gps_qual[2];//d - 0,1,2
	char sats[3];//dd
	char hdop[5];//dd.d
	char alt[7];//mmm.ss
	char speed[6];//kkk.m
	char date[7];//ddmmyy
}gps_data;

char quote[1] = {0x22};
char ctrlz[1] = {0x1A};
char NUMBER[12] = {'+','7','9','1','5','0','3','5','2','3','4','5'};
char GPGGA[6] = {'$','G','P','G','G','A'};
char GPRMC[6] = {'$','G','P','R','M','C'};
gps_data cur_g_data;//Global variable contains current gps data
char output[20][11];//Global variable contains last received nmea message (parsed)
char RX_buf[RXBUFLENGTH_GSM];//Buffer for data from gsm
volatile unsigned char RxBufPoint = 0;

uint8_t lcd_cur = 0;



//===================================
//Useful functions
//===================================

/**
This function parses a string into 20 tokens. 
Character that delimit the tokens in
the parsed string is ','
Maximum length of one block is 11 symbols
Returns 0 if succeed and -1 if output is overflowed
*/
uint8_t delim(char * input){
	/*Output array structure:
					[1 block] - block of chars before next ','
					[2 block] - block of chars before next ','
					.........
	*/
	uint8_t i = 0, j = 0, k = 0;
	while (input[i] != 0x0a){
		output[j][k] = input[i];
		k++;
		if( input[i] == ','){
			j++;
			k=0;
		}	
		i++;
		if(k >= 11)
			return -1;
		if(j >= 20)
			return -1;
	}
	return 0;
}

/**
Compare of two strings, but only N first symbols
returns 1 if these strings are similar and 0 if not
*/
bool compare_st(char* one, char *two, uint8_t N){
	int i;
	for(i = 0; i < N; i++){
		if (one[i] != two[i]){
			return false;
		}
	}
	return true;	
}

/**
This function copy N characters from source to dest
*/
void copystr(char* dest, char* source, uint8_t N){
	uint8_t i;
	for(i = 0; i < N; i++){
		dest[i] = source[i];
	}
	dest[N] = '\0';
}

void lcd_print_GPS(){
	lcd_clrscr();
	lcd_prints((uint8_t *)"Date - ");
	lcd_prints((uint8_t *)cur_g_data.date);
	lcd_prints((uint8_t *)"\n");
	lcd_prints((uint8_t *)"Time - ");
	lcd_prints((uint8_t *)cur_g_data.timestamp);
	lcd_return();
	_delay_ms(1000);
	
	lcd_clrscr();
	lcd_prints((uint8_t *)"Lat - ");
	lcd_prints((uint8_t *)cur_g_data.lat);
	lcd_prints((uint8_t *)"[");
	lcd_prints((uint8_t *)cur_g_data.n_s);
	lcd_prints((uint8_t *)"]");
	lcd_prints((uint8_t *)"\n");
	lcd_prints((uint8_t *)"Long- ");
	lcd_prints((uint8_t *)cur_g_data.longi);
	lcd_prints((uint8_t *)"[");
	lcd_prints((uint8_t *)cur_g_data.e_w);
	lcd_prints((uint8_t *)"]");
	lcd_return();
	_delay_ms(1000);
	
	lcd_clrscr();
	lcd_prints((uint8_t *)"Speed - ");
	lcd_prints((uint8_t *)cur_g_data.speed);
	lcd_prints((uint8_t *)"\n");
	lcd_prints((uint8_t *)"Sats - ");
	lcd_prints((uint8_t *)cur_g_data.sats);
	lcd_return();
	_delay_ms(1000);
}


//===================================
//Hardware initialization and reset
//===================================

/**
Procedure of hardware initialization of atmega controller
*/
void initialization(){
	//Initialization of port B (GPS)
	DDRB = 0x9;// PB3 and PB0 set as out;
	PORTB = 0x1;//Set HIGH level on /reset pin (PB0)
	//TX1 and RX1 pins are Z inputs
	
	//Initialization USART1 (GPS)
	// Set baud rate
	UBRR1H = (uint8_t)(UBRR_VALUE>>8);
	UBRR1L = (uint8_t)(UBRR_VALUE);
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR1C |= (1<<URSEL1)|(1<<UCSZ11)|(1<<UCSZ10);
	//enable transmission and reception but not interruption
	UCSR1B |= (1<<RXEN1)|(1<<TXEN1);
	
	//Initialization of port D and E (GSM)
	DDRD = 0x16;// PD1 and PD2 and PD4 are out (TX, RTS, DTR)
	DDRE = 0x3;//PE0 and PE1 are out (Reset_gsm, IGN)
	PORTD = 0;//No HIGH levels on PORTD
	PORTE = 0;//No HIGH levels on PORTE
	//TX0 and RX0 pins are Z inputs
	
	//Initialization USART0 (GSM)
	// Set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)(UBRR_VALUE);
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<URSEL0)|(1<<UCSZ01)|(1<<UCSZ00);
	//enable transmission and reception and receive interrupt
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);//|(1<<RXCIE0);

	//PORTD |= (1<<PD4);//DTR = '1'. It says to GSM chip that atmega ready
	
	//Initialization of lcd PORTA
	DDRA = (1<<E)|(1<<RS)|(1<<D7)|(1<<D6)|(1<<D5)|(1<<D4);
	PORTA = 0x0;
	
	lcd_init();
}

/**
Send reset signal to GPS chip
*/
void reset_gps(){
	uint8_t UCSR1B_store = UCSR1B;//Save state of UCSR1B
	while( !( UCSR1A & (1<<UDRE1)) ){};//Wait for empty USART1 transmit buffer
	UCSR1B &= ~((1<<TXEN1)|(1<<RXEN1));//Disable USART1 transmitter and receiver {and interruption}

	DDRB = 0x9;// PB3 and PB0 set as out;
	PORTB = 0x1;//Set HIGH level on /reset pin (PB0)
	//TX1 pin is in Z state
	_delay_ms(1);

	PORTB = 0;//Set LOW level on /reset pin (PB0) for 15ms
	_delay_ms(15);

	PORTB = 0x1;//Set HIGH level on /reset pin (PB0)
	_delay_ms(25);//Wait gps chip reset and set up to work mode

	UCSR1B = UCSR1B_store;//Restore state of UCSR1B
}

/**
Send emergency reset signal to GSM chip
*/
void e_reset_gsm(){
	uint8_t UCSR0B_store = UCSR0B;//Save state of UCSR0B
	while( !( UCSR0A & (1<<UDRE0)) ){};//Wait for empty USART1 transmit buffer
	UCSR0B &= ~((1<<TXEN0)|(1<<RXEN0));//Disable USART0 transmitter and receiver
	
	DDRD = 0x16;// PD1 and PD2 and PD3 are out (TX, RTS, DTR)
	DDRE = 0x3;//PE0 and PE1 are out (Reset_gsm, IGN)
	_delay_ms(1);
	
	PORTE |= 0x2; //Set '1' on Reset_gsm pin
	_delay_ms(15);
	
	PORTE &= ~0x2;//Set Reset_gsm pin back to '0'
	_delay_ms(500);//Wait 0.5s for GSM module set up
		
	UCSR0B = UCSR0B_store;//Restore state of UCSR0B
}

/**
Send ignition signal to GSM chip
Block execution for 8 sec!
*/
void gsm_on(){
	PORTE |= 0x1;//Set PE0 to '1' for 10ms
	_delay_ms(10);
	PORTE &= ~0x1;//Set PE0 back to '0'
	//_delay_ms(8000);
}


//===================================
//USART processing
//===================================

/**
Receive 1 byte from USART1(GPS)
*/
uint8_t USART1_Receive(){
	// Wait for byte to be received
	while( ( UCSR1A & (1<<RXC1)) == 0){};
	// Return received data
	return UDR1;
}

/**
Send 1 byte via USART0(GSM)
*/
void USART0_Send(uint8_t u8Data){
	//wait for empty transmit buffer and GSM is ready for send
	while( !( UCSR0A & (1<<UDRE0)) && (PIND & 0x8) ){};
	// Put data into buffer, sends the data
	UDR0 = u8Data;
}

/**
Send AT command to GSM module
*/
void Send_AT_cmd(char *cmd, bool delay)
{
	while(*cmd){
		USART0_Send( (uint8_t)*cmd );
		cmd++;
	}
	if (delay)
		_delay_ms(50);	
}

void ClearRxBuf(){
	uint8_t i;
	for (i = 0; i < RXBUFLENGTH_GSM; RX_buf[i++] = 0);
	RxBufPoint = 0;
	PORTD |= 0x4;//Set 1 to inform GSM that buffer is ready and atmega ready for data
}

//===================================
//Interruptions precessing
//===================================

ISR(USART0_RXC_vect){
	char InData = UDR0;//Read data from USART0 anyway
	if (InData > 30){//We don't care about spec symbols
		RX_buf[RxBufPoint] = InData;//Store information
		RxBufPoint++;
		if (RxBufPoint > 230)
			PORTD &= ~0x4;//Set 0 to inform GSM that buffer is near to overflow.
	}
}

ISR(__vector_default){
	asm("nop");
}


//===================================
//GPS data processing
//===================================

/**
 This function gets nmea messages from USART1. 
There are two meanful messages:
GPGGA(5_ - position (with 5 signs after point), satellites, high(in meters) and hdop.
GPRMC - position (4 signs), speed and date.
It updates global variable cur_g_data according to these messages if there any.
*/
uint8_t get_gps_data(){
	//TODO:
	//If GNSS starts to be '1' for 2 secs and '0' for 2 secs this function should be invoked
	//char _buffer[90];//String buffer for read message	
	//uint8_t count = 0;//Counter of bytes in message
	
	uint8_t _buf = USART1_Receive();
	lcd_putc(_buf);
	lcd_cur++;
	lcd_goto(1, lcd_cur);
	
	return -4;
		
	//TODO: Check last byte value in real(now 0x0a - new line symbol <\n>) also check last byte number(now 90) 
	/*
	while ( (_buffer[count-1] != '\n')||(count < 90) ){//Receive nmea message as a string
		_buffer[count] = USART1_Receive();
		count++;
	}
	
	if (count < 4)//To few symbols in the name of message
		return -1;//Wrong message was read
		
	if (delim(_buffer) != 0)//Parse the string in _buffer into blocks delimited by ','
		return -2; //Overflow of output buffer;
	//Now we have updated output variable
	
	//Define if the message is GPGGA
	if(compare_st(output[0], GPGGA, 6)){//Message seems to be GPGGA
		*//*$GPGGA,<Timestamp>,<Lat>,<N/S>,<Long>,<E/W>,<GPSQual>,<Sats>,<HDOP>,<Alt>,
		<AltVal>,<GEOSep>,<GEOVal>,<DGPSAge>,<DGPSRef>,<checksum><cr><lf>*/
	/*	copystr(cur_g_data.timestamp, output[1],6);
		copystr(cur_g_data.lat, output[2],10);
		copystr(cur_g_data.n_s, output[3],1);
		copystr(cur_g_data.longi, output[4],10);
		copystr(cur_g_data.e_w, output[5],1);
		copystr(cur_g_data.gps_qual, output[6],1);
		copystr(cur_g_data.sats, output[7],2);
		copystr(cur_g_data.hdop, output[8],4);
		copystr(cur_g_data.alt, output[9],6);
		return 0;				
	}
	//Define if the message is GPRMC
	if(compare_st(output[0], GPRMC, 6)){//Message seems to be GPRMC
		*//*$GPRMC,<Timestamp>,<Status>,<Lat>,<N/S>,<Long>,<E/W>,<Speed>,<Trackgood>,
		<Date>,<MagVar>,<MagVarDir> <checksum><cr><lf>*/
	/*	copystr(cur_g_data.timestamp, output[1],6);
		copystr(cur_g_data.speed, output[7],5);
		copystr(cur_g_data.date, output[9],6);
		return 0;
	}
	return -3;//No necessary message was read
	*/
}


//===================================
//GSM chip processing
//===================================

/**
Set all settings to necessary in GSM chip
*/
uint8_t set_settings_gsm(){
	uint8_t i = 0;
	ClearRxBuf();

	sei();//Enable interruptions. Need for correct working of UART0(GSM) receiver
	
	ClearRxBuf();
	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY) ){
		ClearRxBuf();
		Send_AT_cmd("AT+IPR=115200\n\r", 1);//Setup fixed UART speed in gsm
		i++;
	}		

	ClearRxBuf();
	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 2) ){
		ClearRxBuf();
		Send_AT_cmd("ATE0\n\r", 1);//Disable echo mode
		i++;
	}

	ClearRxBuf();
	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 3) ){
		ClearRxBuf();
		Send_AT_cmd("AT+CLCK=", 0);//Disable PIN code blocking. <AT+CLCK="SC",0,"PIN">
		Send_AT_cmd(quote, 0); 
		Send_AT_cmd("SC", 0);
		Send_AT_cmd(quote, 0);
		Send_AT_cmd(",0,", 0);
		Send_AT_cmd(quote, 0);
		Send_AT_cmd(PIN, 0);
		Send_AT_cmd(quote, 0);
		Send_AT_cmd("\n\r", 1);
		i++;
	}

	ClearRxBuf();
	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 4) ){
		ClearRxBuf();
		Send_AT_cmd("AT\\Q3\n\r", 1);//Enable hardware data flow control
		i++;
	}
	
	ClearRxBuf();
	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 5) ){
		ClearRxBuf();
		Send_AT_cmd("AT+CMGF=1\n\r", 1);//Set text mode for sms sending
		i++;
	}

	
	//ClearRxBuf();
	//while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 5) ){
	//	ClearRxBuf();
	//	Send_AT_cmd("AT^SSET=1\n\r", 1);//Enable URC <^SSIM READY>
	//	i++;
	//}

	if(i == TRY * 5)
		return -1;//All settings wasn't set successfully
	else
		return 0;//All settings was set successfully
}

uint8_t Send_SMS(char *text){
	ClearRxBuf();
	
	Send_AT_cmd("AT+CMGS=", 0);
	Send_AT_cmd(NUMBER, 0);
	Send_AT_cmd("\n\r", 1);
	Send_AT_cmd(text,0);
	Send_AT_cmd(ctrlz,1);//[Ctrl+Z]
		
	if ( compare_st(RX_buf, "OK", 2) == false )
		return -1;
		
	return 0;	
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

int main(void){	
		
	initialization();
	
	//reset_gps();
	
	//gsm_on();
	
	//set_settings_gsm();
	lcd_prints((unsigned char*)"Hello World");
	
	_delay_ms(2000);
	
	lcd_clrscr();
	lcd_return();
	lcd_cur = 0;
	
	while(1){
		
	while( ( UCSR1A & (1<<RXC1)) == 0){};
	// Return received data
	unsigned char tmp = UDR1;
	_delay_ms(100);
	lcd_putc(tmp);
		//Now we have cur_g_data filled.
		//lcd_print_GPS();//Print received gps data on lcd display
		
		//TODO:
		//Wait for GNSS(PB4) starts to be '1' for 2 secs and '0' for 2 secs

		//get_gps_data() == 0 //Get current gps data
		//If this succeed 

		//Transmit cur_gps_data to gsm chip for processing
	}
}