/*
 * PWM.c
 *
 * Created: 4/2/2017 17:26:14
 *  Author: Christian
 */ 
//uint8_t
#include <avr/io.h>
enum Timer{Horizontal,Vertical,Rotate,Grab};

void init_phase_correct_alt()
{
	//Timer 1
	DDRB|=(1<<PB6);   //pin 12
	TCCR1A|=(1<<COM1B1);	//Clear OC1B on Compare Match when up-counting. Set OC1A on Compare Match when down-counting
	TCCR1B =(1<<CS12)|(1<<WGM13);   //prescalling by 256
	ICR1= 625;  //top value then OC1A pin can be used as output  // 256 bit top value
	OCR1B = 50; // Går til start tilstand
	//TCCR1B =(1<<CS00); // Stops the timer
	
	//Timer 3
	DDRE|=(1<<PE3);   //pin 5
	TCCR3A|=(1<<COM3A1);	//Clear OC3A
	TCCR3B =(1<<CS12)|(1<<WGM13);   //prescalling by 256
	ICR3= 625;  //top value
	OCR3A = 50; // Går til start tilstand
	
	//Timer 4	
	DDRH|=(1<<PH5);   //pin 8
	TCCR4A|=(1<<COM4C1);	//Clear OC4C
	TCCR4B =(1<<CS42)|(1<<WGM43);   //prescalling by 256
	ICR4= 625;  //top value
	OCR4C = 45; // Går til start tilstand
	
	//Timer 5
	DDRL|=(1<<PL3);   //pin 46
	TCCR5A|=(1<<COM5A1);	//Clear OC5C
	TCCR5B =(1<<CS52)|(1<<WGM53);   //prescalling by 256
	ICR5= 625;  //top value
	OCR5A = 45; // Går til start tilstand
}
//typedef 


void Change_DuCy(enum Timer motor, int dutyCycle){	 
	
	switch (motor){
		case Vertical:
			OCR1B = dutyCycle;
			break;
		case Horizontal:
			OCR3A = dutyCycle;
			break;
		case Rotate:
			OCR4C = dutyCycle;
			break;
		case Grab:
			OCR5A = dutyCycle;
			break;
	}
}