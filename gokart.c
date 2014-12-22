#include <avr/interrupt.h>  
#include <avr/io.h>


/*

#include "uart.h"



void uart_puts (const char *s)
{
    do
    {
        uart_putc (*s);
    }
    while (*s++);
}



unsigned short getAnalogVal(unsigned char port)
{
	unsigned short retval;
	unsigned char vl, vh;


	ADMUX = port;

	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);

	vl = ADCL;
	vh = ADCH;

	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);

	vl = ADCL;
	vh = ADCH; //measure twice for stable value

	

	retval = ((vh & 3) << 8) | vl;

	return retval;
}

*/

#include <avr/io.h>
#include <util/delay.h>
 
enum {
 BLINK_DELAY_MS = 1000,
};
 
int main (void)
{
 /* set pin 5 of PORTB for output*/
 DDRB |= _BV(DDB5);
 
 while(1) {
  /* set pin 5 high to turn led on */
  PORTB |= _BV(PORTB5);
  _delay_ms(BLINK_DELAY_MS);
 
  /* set pin 5 low to turn led off */
  PORTB &= ~_BV(PORTB5);
  _delay_ms(BLINK_DELAY_MS);
 }
 
 return 0;
}

