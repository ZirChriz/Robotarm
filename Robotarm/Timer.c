/*
 * Timer.c
 *
 * Created: 4/15/2017 16:02:19
 *  Author: Christian
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

int Time_delay=70;
volatile char wait = 1, update_uart=0;

void init_timer(){
//timer 2
	TCCR2B |=(1<<WGM22);  //CTC mode
	TCCR2B |=(1<<CS22)|(1<<CS21)|(1<<CS20); //prescalling til 1024
	OCR2A=124;		//delay på 0,008s
	
	TIMSK2|=(1<<OCIE2A);
	
//timer 0
	TCCR0A |= (1<<WGM02);//CTC mode
	TCCR0B |= (1<<CS02)|(1<<CS00);//prescaler 1024
	TIMSK0 |=(1<<TOIE0); //Overflow interrupt enable
	

}
//Bestemmer hvor lang tid delayet for motoren er 
ISR(TIMER2_COMPA_vect){
	static int i=0;
	if(i>=Time_delay){
	wait = 0;
	i=0;
	}
	i++;
}

//overflower ved 16ms
//Benyttes til at opdatere uarten samt styre adc konverteringen 
ISR(TIMER0_OVF_vect){
	//Skal være der for at der sker et interrupt
	static int i=0;
	if(i>=8){
		update_uart=1;
		i=0;
	}
	i++;
}