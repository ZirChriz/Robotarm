/*
 * IncFile.h
 *
 * Created: 4/2/2017 17:25:24
 *  Author: Christian
 */ 


#ifndef INCFILE_H_
#define INCFILE_H_

//ADC
	extern void init_ADC();
	//int analog0, analog1, analog2, analog3;
	volatile char flag_ADC; //går høj når en ny sample er klar
	int ADC_hold; //gemmer den seneste ADC sample 

//PWM
	enum Timer{Horizontal,Vertical,Rotate,Grab};
	extern void init_phase_correct_alt();
	extern void Change_DuCy(enum Timer motor, int dutyCycle);	

//UART
	extern void init_uart(int baud);  //initialiser uart 3
	extern void put_char(char data); //send en 8 bit char
	extern char get_char(void);     //modtag en 8 bit char

	extern void put_string(char array[]);//sender en string til

	extern void EN_intr_RX(); //enable interrupt receive
	extern void DIS_intr_RX();// disable interrupt receive

	volatile char flag;//recive interrupt flag
	extern char buff[];//gemmer den indkomne besked
	
//Timer
	void init_timer();
	int Time_delay; //Gemmer Robotarmens tids delay for bevægelse
	volatile char wait, update_uart; //Uartens opdaterings delay

#endif /* INCFILE_H_ */