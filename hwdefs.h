


#define PUMP_PIN PD3 //Pin 3 on Arduino. J6.4 on Hbridge. This one puts out a square wave for the cascade
#define H1_PIN PD5 //Pin 5 on Arduino. J6.5 on Hbridge. Active High. This one switches the H1 side of the H bridge on
#define H2_PIN PD6 //Pin 6 on Arduino. J6.6 on Hbridge. Active High. This one switches the H2 side of the H bridge on
#define L1_PIN PD2 //Pin 2 on Arduino. J6.8 on Hbridge. Active Low. This one switches the L1 side of the H bridge on
#define L2_PIN PD4 //Pin 4 on Arduino. J6.7 on Hbridge Active Low. This one switches the L2 side of the H bridge on

#define TUNE_OUT_PIN PB3 //Pin 11 on Arduino. Output pin constantly at 0V.This is because there was no more GND connection available
#define TUNE_IN_PIN PB2  //Pin 10 on Arduino. Input pin which is sampled at power up. If it is low, the motor can go full speed. Otherwise the speed is limited
