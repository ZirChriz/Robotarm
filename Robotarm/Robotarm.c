/*
 * Robotarm.c
 *
 * Created: 4/2/2017 17:20:35
 * 
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "IncFile.h"

volatile char pressed_R = 0, pressed_L = 0;

void delay(int scale);
void ftos(char *string, float input);
double map_float(int x, double in_min, double in_max, double out_min, double out_max);
long map(long x, long in_min, long in_max, long out_min, long out_max);

//Enable push button PIN 9 | Pull up
void init_intr(){
	
//Venstre knap
	PORTD |=(1<<PD0);
	EICRA |=(1<<ISC01);
	EIMSK |=(1<<INT0);
//Højre knap
	PORTD |=(1<<PD1);
	EICRA |=(1<<ISC11);
	EIMSK |=(1<<INT1);
}



int main(void)
{
    init_phase_correct_alt();
	init_ADC();
	init_uart(103);
	init_timer();
	EN_intr_RX();
	init_intr();
	sei();
	
	int grab = 45, Vert = 50,Horiz = 50 , rotate = 45, Rotate_IN = 538, Vertical_IN=509, Horizontal_IN=503; //Initialiseret til startværdien for robotten
	int middle; int diff; int wait_time = 0;
	char Voltage[6], *ptr, limit[5], motor=0, power=0;
	float volt=0;

    while (1) 
    {
		//Skaber et delay til robotarmen så den kan operere ved forskællige hastigheder 
		//Wait_time styrer hastigheden 
		if(wait==0){
			
  /////////////////////////////////
 //         ADC Update          //
/////////////////////////////////
			
			//Når en ADC sample er klar bliver de analoge input på skift opdateret
			if(flag_ADC==1){
		
				flag_ADC = 0;
		
				if(ADMUX==0b01000000){
					Rotate_IN = ADC_hold;//Analog0 | Højre x værdi
					ADMUX = 0x40;
					ADMUX |=(1<<MUX1);
				}
				else if(ADMUX==0b01000010){
					Horizontal_IN = ADC_hold;//Analog2 | Venstre x værdi
					ADMUX = 0x40;
					ADMUX |= (1<<MUX0)|(1<<MUX1);
				}
				else if(ADMUX==0b01000011){
					Vertical_IN = ADC_hold;//Analog3 | Venstre y værdi
					ADMUX = 0x40;
				}
			}
			
  /////////////////////////////////
 //    Robotarm styringskode    //
/////////////////////////////////

			//Trykknapper til grabben
			if(pressed_L == 1 || pressed_R == 1){
			
				grab += pressed_R-pressed_L;	
				// Boundaries
				if(grab < 24){
					grab = 24;
				} 
				else if(grab > 54){
					grab = 54;
				}
				Change_DuCy(Grab, grab);
				wait_time += 1;
			
				if((PIND&0b00000010) == 0b00000010){
					pressed_R = 0;
				}
				if((PIND&0b00000001) == 0b00000001){
					pressed_L = 0;
				}
			}
		
		
			// højre x | Rotate
			middle = 535;
			diff = Rotate_IN-middle;
			if (abs(diff)>10){
			
				rotate -= diff/abs(diff);
				// Boundaries
				if(rotate < 18){
					rotate = 18;
				} 
				else if(rotate > 75){
					rotate = 75;
				}
				Change_DuCy(Rotate, rotate);
				wait_time += 4-abs(diff)/150;
			}
		
		
			// venstre y | Vertical
			middle = 509;
			diff = Vertical_IN-middle;
			if (abs(diff)>10){
			
				Vert -= diff/abs(diff);
				// Boundaries
			
			
				Change_DuCy(Vertical, Vert);
				wait_time += 5-abs(diff)/150;
			}
		
			// venstre x | Horizontal
			middle = 503;
			diff = Horizontal_IN-middle;
			if (abs(diff)>10){
			
				Horiz -= diff/abs(diff);
				// Boundaries
			
			
				Change_DuCy(Horizontal, Horiz);
				wait_time += 5-abs(diff)/150;
			}
		
				
			delay(wait_time);
			wait_time = 0;
			}
		
		
  /////////////////////////////////
 //  SEND AND RECIVE FROM UART  //
/////////////////////////////////		

/////Sender
		
		if(flag==1){
			flag = 0;
			ptr = buff;
			memset(limit,0,strlen(limit));
			strcpy(limit, ptr);
			memset(buff,0,strlen(buff));
			motor = limit[0];
			power = (((limit[2]-48)*100)+((limit[3]-48)*10)+limit[4]-48);
		}
		
		switch(motor){
			case 'R':
				rotate = map(power,0,100,18,75);
				Change_DuCy(Rotate, rotate);
				motor = 0;
				break;
			case 'V':
				//Move code
				break;
			case 'G':
				grab = map(power,0,100,24,54);
				Change_DuCy(Grab, grab);
				motor = 0;
				break;
			case 'H':
				//Move code
				break;
		}
		
/////Læser
		
		//Konverterer kontrollerns værdi til volt og gemmer den som en float 
		volt = map_float(Rotate_IN,0,1023,0,5); 
	
		//Konverterer en float til en string
		memset(Voltage,0,strlen(Voltage));
		ftos(Voltage, volt);
		
		//Sender Stringen til uarten 
		if(update_uart == 1){
			put_string("Rotate pin = ");
			put_string(Voltage);
			put_char('V');
			put_char('\r');
			
			update_uart = 0;
		}
	}
}

// Interrupt for trykknapperne
ISR(INT0_vect){
	
	pressed_L = 1;
}
ISR(INT1_vect){
	
	pressed_R = 1;
}

void delay(int scale){
	
	Time_delay = scale;
	wait = 1;	
}

//konverterer en float til en string	
void ftos(char *string, float input){
	
	uint8_t size = 2; //Antal decimaler
	unsigned char pos = 0;
	char *current = 0; 
	uint8_t len = 0;  
	int value = 0;  
	
	//Fjerner alle decimaltallene og konvertere det til en string
	value = (int)input;
	itoa(value,string,10); 
	 
	//Finder længden af heltallet (som string) og indsætter et komma bagefter
	len = strlen(string); 
	pos = len; 
	string[pos++] = ','; 
	
	//Tager hvert enkelt decimaltal og sætter det efter kommaet
	while(pos < (size + len + 1)){
		input -= (float)value; 
		input *= 10;  
		value = (int)input;
		itoa(value, current,10); 
		string[pos++] = *current; 
	}
}

//Baseret på arduinos funktion map
double map_float(int x, double in_min, double in_max, double out_min, double out_max)
{
	return ((double)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Arduinos map funktion
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}