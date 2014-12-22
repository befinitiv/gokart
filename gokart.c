#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "hwdefs.h"
#include "hbridge.h"
#include "uart.h"
#include "joystick.h"




#define DEBUG
#ifdef DEBUG
	#define DEBUG_PRINT(...) printf(__VA_ARGS__);
#else
	#define DEBUG_PRINT(...) ;
#endif



#define IDLE 0
#define DRIVE 1
#define BRAKE 2

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
 	//blink led and wait 100ms
  PORTB |= _BV(PORTB5);
  _delay_ms(50);
  PORTB &= ~_BV(PORTB5);
  _delay_ms(50);

	//get the position of the joystick
	int16_t jp = joystick_get();
	

	switch(state) {

		case IDLE:
			if(jp) {
				state = DRIVE;
				DEBUG_PRINT("Driving...\n");
			}
			break;
		case DRIVE:
			//has the joystick moved from one side to the other? then go to the brake state
			if((jp > 0 && last_jp < 0) || (jp < 0 && last_jp > 0)) {
				DEBUG_PRINT("Breaking...\n")
				cnt = 0;
				state = BRAKE;
			}
			else if(jp > 0) {
				DEBUG_PRINT("driving forward %d\n", jp)
				hbridge_power(jp/2, 0); //we half the power to stay safe
				last_jp = jp;
			}
			else if(jp < 0) {
				DEBUG_PRINT("driving reverse %d\n", jp)
				hbridge_power(-jp/4, 1); //a quarter of the power to stay safe when reversing
				last_jp = jp;
			} else if(jp == 0) {
				//just let it roll
				DEBUG_PRINT("driving without power\n");
				hbridge_idle();
			}
		break;

		case BRAKE:
			//we stay at least BREAK_TIME in the BRAKE state
			if(cnt < BREAK_TIME) {
				hbridge_brake(BREAK_POWER);
				cnt++;
			} else {
				if(jp == 0) {
					cnt = 0;
					state = IDLE;
					last_jp = 0;
					DEBUG_PRINT("Idle...\n");
				}
			}
		break;

		default:
			break;

		}

 }
 
 return 0;
}

