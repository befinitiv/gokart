#include <avr/io.h>

#include "hwdefs.h"

void pwm_init(void) {
	TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01); //fast pwm non inverted
	TCCR0B = (1 << CS01) | (1 << CS00); // /8 prescaler
	OCR0A = 0;
	OCR0B = 0;

}

//creates a constant frequency for the charge pump
void pump_init(void) {
	TCCR2A = (1 << COM2B1) | (1 << COM2B0) | (1 << WGM21) | (1 << WGM20); //ctc mode
	TCCR2B = (1 << CS20); // /8 prescaler
	OCR2B = 128;

}


void hbridge_en_h1(uint8_t val) {
	OCR0B = val;
	DDRD |= (1 << H1_PIN);
}

void hbridge_en_h2(uint8_t val) {
	OCR0A = val;
	DDRD |= (1 << H2_PIN);
}

void hbridge_en_l1(void) {
	PORTD &= ~(1 << L1_PIN);
}

void hbridge_en_l2(void) {
	PORTD &= ~(1 << L2_PIN);
}


void hbridge_dis_h1(void) {
	DDRD &= ~(1 << H1_PIN);
	OCR0B = 0;
}

void hbridge_dis_h2(void) {
	DDRD &= ~(1 << H2_PIN);
	OCR0A = 0;
}

void hbridge_dis_l1(void) {
	PORTD |= (1 << L1_PIN);
}

void hbridge_dis_l2(void) {
	PORTD |= (1 << L2_PIN);
}


void hbridge_en_pump(void) {
	DDRD |= (1 << PUMP_PIN);
}

void hbridge_dis_pump(void) {
	DDRD &= ~(1 << PUMP_PIN);
}

void hbridge_init(void) {
	DDRD |= (1 << L1_PIN) | (1 << L2_PIN);

	//first: disable everything
	hbridge_dis_h1();
	hbridge_dis_h2();
	hbridge_dis_l1();
	hbridge_dis_l2();
	hbridge_dis_pump();

	pump_init();
	pwm_init();

	hbridge_en_pump();
}

void hbridge_power(uint8_t val, uint8_t dir) {
	if(dir) {
		hbridge_dis_h1();
		hbridge_dis_l2();

		hbridge_en_l1();
		hbridge_en_h2(val);
	}
	else {
		hbridge_dis_h2();
		hbridge_dis_l1();

		hbridge_en_l2();
		hbridge_en_h1(val);
	}
}

void hbridge_brake(uint8_t val) {
	hbridge_dis_l1();
	hbridge_dis_l2();

	hbridge_en_h1(255);
	hbridge_en_h2(val);
}

void hbridge_idle(void) {
	hbridge_dis_h1();
	hbridge_dis_h2();
	hbridge_dis_l1();
	hbridge_dis_l2();
}




	

