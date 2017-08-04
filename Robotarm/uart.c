/*
 * CFile1.c
 *
 * Created: 3/1/2017 21:48:47
 * 
 */ 
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>

volatile char flag= 0;
char buff[100]; 


//initialiserer uart0 | hastigheden bestemt af baud
void init_uart(int baud){
	UBRR0H =(unsigned char)baud>>8;
	UBRR0L =(unsigned char)baud;
	UCSR0A =(1<<U2X0);  
	UCSR0B|=(1<<RXEN0)|(1<<TXEN0);  
	UCSR0C|=(1<<UCSZ00)|(1<<UCSZ01); 

}

//Sender en 8 bit char
void put_char(char data){
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0=data;
}

//Modtager en 8 bit char
char get_char(){
	while(!(UCSR0A&(1<<RXC0)));
	return UDR0; 
}

//Sender en string
void put_string(char array[]){

	int i = 0, size;
	size = strlen(array);
	
	do{
		put_char(array[i]);
		i++;
		
	}while(i <= size-1);
}

//Modtager en string
void EN_intr_RX(){
	
	UCSR0B |=(1<<RXCIE0);
}

void DIS_intr_RX(){
	
	UCSR0B &=~(1<<RXCIE0);
}


// modtager string/char via interrupt
ISR(USART0_RX_vect){
	static int i = 0;
	
	buff[i] = UDR0;
	i++;
	
	if(UDR0 == '\r'){
		flag = 1;
		i = 0;
	}
}