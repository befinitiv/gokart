#include <avr/io.h>
#include <util/delay.h>


#include "hwdefs.h"
#include "hbridge.h"
#include "uart.h"
#include "joystick.h"



#define IDLE 0
#define DRIVE 1
#define BRAKE 2
 
#define DEBUG(...) printf(__VA_ARGS__);

#define BREAK_TIME 10
#define BREAK_POWER 200

#define MAX_REVERSE_POWER 100

int main (void)
{
	hbridge_init();
	uart_init();
	joystick_init();

 /* set pin 5 of PORTB for output*/
 DDRB |= _BV(DDB5);

	uint8_t cnt = 0;
	uint8_t state = IDLE;
	int16_t last_jp = 0;
 
 while(1) {
  PORTB |= _BV(PORTB5);
  _delay_ms(50);
  PORTB &= ~_BV(PORTB5);
  _delay_ms(50);

	int16_t jp = joystick_get();
	

	switch(state) {

		case IDLE:
			if(jp) {
				state = DRIVE;
				DEBUG("Driving...\n");
			}
			break;
		case DRIVE:
			printf("%d\t%d\n", jp, last_jp);
			if((jp > 0 && last_jp < 0) || (jp < 0 && last_jp > 0)) {
				DEBUG("Breaking...\n")
				cnt = 0;
				state = BRAKE;
			}
			else if(jp > 0) {
				DEBUG("driving forward %d\n", jp)
				hbridge_power(jp/2, 0);
				last_jp = jp;
			}
			else if(jp < 0) {
				if(jp < -MAX_REVERSE_POWER)
					jp = -MAX_REVERSE_POWER;
				DEBUG("driving reverse %d\n", jp)
				hbridge_power(-jp/2, 1);
				last_jp = jp;
			} else if(jp == 0) {
				DEBUG("driving without power\n");
				hbridge_idle();
			}
		break;

		case BRAKE:
			if(cnt < BREAK_TIME) {
				hbridge_brake(BREAK_POWER);
				cnt++;
			} else {
				if(jp == 0) {
					cnt = 0;
					state = IDLE;
					last_jp = 0;
					DEBUG("Idle...\n");
				}
			}
		break;

		default:
			break;

		}

 }
 
 return 0;
}

