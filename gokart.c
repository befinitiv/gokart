#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


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



typedef struct {
	uint8_t max_power;
	uint8_t ramp_start_power;
	uint8_t ramp_dp;
	} drive_parameter;

typedef struct {
	drive_parameter fwd;
	drive_parameter rev;
	} parameter;



parameter param[] = {

//ultra slow mode
{
.fwd = {
	.max_power = 70,
	.ramp_start_power = 30,
	.ramp_dp = 5
	},
.rev = {
	.max_power = 60,
	.ramp_start_power = 30,
	.ramp_dp = 5
	}
},
//higher forward speed
{
.fwd = {
	.max_power = 140,
	.ramp_start_power = 30,
	.ramp_dp = 10
	},
.rev = {
	.max_power = 60,
	.ramp_start_power = 30,
	.ramp_dp = 5
	}
},
//fully unlocked
{
.fwd = {
	.max_power = 255,
	.ramp_start_power = 60,
	.ramp_dp = 30
	},
.rev = {
	.max_power = 120,
	.ramp_start_power = 30,
	.ramp_dp = 10
	}
}
};

uint8_t check_mode(void) {
	DDRB &= ~(1 << TUNE_IN_PIN);
	DDRB |= (1 << TUNE_OUT_PIN);
	PORTB |= (1 << TUNE_IN_PIN);
	PORTB &= ~(1 << TUNE_OUT_PIN);
	_delay_ms(100);
	if((PINB & (1 << TUNE_IN_PIN)) != 0)
		return 2;

	if(joystick_get() < -100) {
		//wait until release
		while(joystick_get() != 0);
		return 1;
	}

	return 0;
}


#define BREAK_TIME 10
#define BREAK_POWER_FWD 200
#define BREAK_POWER_REV 200

#define ROLLOUT_TIME 20

int main (void)
{
	hbridge_init();
	uart_init();
	joystick_init();

 /* set pin 5 of PORTB for output*/
 DDRB |= _BV(DDB5);

	uint8_t tune_mode = check_mode();

	

	//for(;;)printf("%d\n", (PINB & (1 << TUNE_IN_PIN)));
	uint8_t cnt = 0;
	uint8_t state = IDLE;
	int16_t last_jp = 0;
	uint16_t last_power = 0;
	parameter *p = &param[tune_mode];
 
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
				DEBUG_PRINT("Breaking %d %d...\n", jp, last_jp)
				cnt = 0;
				last_power = 0;
				state = BRAKE;
			}
			else if(jp != 0) {
				uint8_t forward = jp > 0 ? 1 : 0;
				drive_parameter *dp = forward ? &p->fwd : &p->rev;

				//normalize joystick position to max power
				uint32_t norm_power = abs(jp) * dp->max_power / 255;
				
				//if the wanted power is higher than the ramp start and the last power was lower, we ramp up 
				if(norm_power > dp->ramp_start_power && norm_power > last_power) {
					DEBUG_PRINT("ramping from %d to %d\n", last_power, last_power + dp->ramp_dp);
					last_power = last_power + dp->ramp_dp;
					if(last_power > 255)
						last_power = 255;
				}
				else { //else we just forward the wanted power to te motor
					DEBUG_PRINT("direct power to %ld\n", norm_power);
					last_power = norm_power;
				}

				DEBUG_PRINT("tune: %d\tforward: %d\tjp: %d\tnorm_power: %ld\tlast_pwr: %d\n",tune_mode,forward,jp,norm_power,last_power);
				hbridge_power(last_power, !forward);

				last_jp = jp;
				cnt = 0;
			} else if(jp == 0) {
				if(cnt < ROLLOUT_TIME) {
					//just let it roll
					DEBUG_PRINT("driving without power\n");
					hbridge_idle();
					last_power = 0;
					cnt++;
				}
				else
				{
					last_jp = 0;
					last_power = 0;
					state = IDLE;
					DEBUG_PRINT("going to idle\n");
				}
			}
		break;

		case BRAKE:
			//we stay at least BREAK_TIME in the BRAKE state
			if(cnt < BREAK_TIME) {
				if(last_jp < 0) {
					DEBUG_PRINT("braking reverse\n");
					hbridge_brake(BREAK_POWER_REV);
				}
				else
				{
					DEBUG_PRINT("braking forward\n");
					hbridge_brake(BREAK_POWER_FWD);
				}
				
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

