/*
 * ADC.c
 *
 * Created: 4/3/2017 12:03:20
 *  Author: Christian
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>


volatile char flag_ADC=0;
int ADC_hold = 0;

void init_ADC(){
	
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);    //Prescaler at til 128 | clock 125Khz
	ADMUX |= (1<<REFS0);				//reference
	ADMUX &=~ (1<<REFS1);               //+5v voltage reference
	
	DIDR0 = (1<<ADC0D) | (1<<ADC1D); // digital input disable
	ADCSRA |= (1<<ADEN);                //Enable ADC
	
	ADCSRA |= (1<<ADIE);	//enable intrerrupt
	
	
	
	ADCSRB |= (1<<ADTS2); //Kigger på timer 0 overflow 
	ADCSRA |= (1<<ADATE);  // Auto trigger mode
	
	ADCSRA |= (1<<ADSC); //Starts a new conversion
	ADMUX = 0x40;
	
//NB benytter timer 0 til timer overflow
					
}


ISR(ADC_vect){
	
	flag_ADC = 1;
	ADC_hold = ADCW;//Læser alle 10 bit
}
