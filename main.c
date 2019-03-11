/*
 * 375Project.c
 *
 * Created: 12/1/2016 11:27:38 AM
 * Author : JP
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//flags set for LEDs

#define F_CPU 16000000UL
#define REDD_ON	PORTD |= (1<<PORTD0)
#define YELLOWD_ON	PORTD |= (1<<PORTD1)
#define GREEND_ON	PORTD |= (1<<PORTD2)
#define REDD_OFF	PORTD &= ~(1<<PORTD0)
#define YELLOWD_OFF	PORTD &= ~(1<<PORTD1)
#define GREEND_OFF	PORTD &= ~(1<<PORTD2)

#define REDC_ON	PORTC |= (1<<PORTC0)
#define YELLOWC_ON	PORTC |= (1<<PORTC1)
#define GREENC_ON	PORTC |= (1<<PORTC2)
#define REDC_OFF	PORTC &= ~(1<<PORTC0)
#define YELLOWC_OFF	PORTC &= ~(1<<PORTC1)
#define GREENC_OFF	PORTC &= ~(1<<PORTC2)

#define BUTTON_PRESSED !(PINB & (1<<PINB7))
#define BUTTON_PRESSED2 !(PIND & (1<<PIND7))

volatile unsigned int carC = 0;
volatile unsigned int carD = 0;
volatile unsigned int yellowC = 0;
volatile unsigned int greenC = 0;
volatile unsigned int yellowD = 0;
volatile unsigned int greenD = 1;
volatile unsigned int redC = 1;
volatile unsigned int redD = 0;
ISR(PCINT0_vect)//button press, car present
{
	if(BUTTON_PRESSED)
		carC =1;
}
ISR(PCINT2_vect)
{
	if(BUTTON_PRESSED2)
		carD =1;
}
ISR(TIMER1_COMPA_vect)
{	
		if(carD==1 &&carC ==0)
			{
				if(yellowC==1)
				{
					YELLOWC_OFF;
					REDC_ON;
					_delay_ms(20000);
					yellowC=0;
					redC=1;
					REDD_OFF;
					greenD=1;
					GREEND_ON;
					carD = 0;
					_delay_ms(20000);
				}
				if(greenC==1)
				{
					GREENC_OFF;
					YELLOWC_ON;
					greenC=0;
					yellowC=1;
				}
			}
		else if(carC==1 && carD ==0)
		{
			if(yellowD==1)
			{
				YELLOWD_OFF;
				REDD_ON;
				_delay_ms(20000);
				yellowD=0;
				redD=1;
				REDC_OFF;
				greenC=1;
				GREENC_ON;
				carC=0;
				_delay_ms(20000);
			}
			if(greenD==1)
			{
				GREEND_OFF;
				YELLOWD_ON;
				greenD=0;
				yellowD=1;
			}
		}
		else if(carC==1 && (greenC ==1 ||yellowC ==1))
			carC = 0; //car arrives at a green light and leaves
		else if(carD==1 && (greenD ==1 || yellowD ==1))
			carD = 0; //car arrives at a green light and leaves
		
}

int main(void)
{
	DDRD |= (1<<DDD0) | (1<<DDD1) | (1<<DDD2); //D0-RED, D1-YELLOW, D2-GREEN
	DDRC |= (1<<DDC0) | (1<<DDC1) | (1<<DDC2); //C0-RED, C1-YELLOW, C2-GREEN

	DDRB &= ~(1<<DDB7); //SIDE C CAR PRESENCE AS PUSH BUTTON
	DDRD &= ~(1<<DDD7); //SIDE D CAR PRESENCE AS PUSH BUTTON
	sei();
	PCICR |= (1<<PCIE0) | (1<<PCIE2);//enabled pcint 7 and pcint 23
	PCMSK0 |= (1<<PCINT7); //pinchange on port b7
	PCMSK2 |= (1<<PCINT23); //pinchange on port d7
	//TODO SET INTERRUPT SETTINGS FOR BUTTONPRESS 
    /* Replace with your application code */
	GREEND_ON; //main lane
	REDC_ON;
    while (1) 
    {
		if(carC ==1 || carD==1)
			{
				TCCR1B |= (1<<CS12) | (1<<WGM12) | (1<<CS10); //cs12- clk/256 prescaler, wgm12- sets timer in ctc-clear timer on compare mode
				TIMSK1 |= 1<<OCIE1A;//sets timer compare interrupt
				OCR1A = 40000;
			}


    }
}

