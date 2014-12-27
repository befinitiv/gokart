#include <math.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>


uint8_t joystick_pos;

ISR(ADC_vect)
{
 
	joystick_pos = ADCH;
	ADCSRA |= 1<<ADSC;		// Start Conversion
}

void joystick_init(void) {


	ADMUX = (1 << REFS0) | (1 << ADLAR); //select AVCC reference voltage

	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

	sei();

	ADCSRA |= (1 << ADSC);
}

#define ZERO_REGION 40


int16_t joystick_get(void) {
	int16_t val = (int16_t)127 - (int16_t)joystick_pos;
	val = val * 2;

	if(abs(val) < ZERO_REGION)
		return 0;

	int8_t sign = val > 0 ? 1 : -1;

	//normalize to 0-255
	int32_t norm_val  = ((int32_t)val - sign*ZERO_REGION) * 255 / (255-ZERO_REGION); 
	
	return norm_val;
} 
