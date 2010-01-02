

#ifndef HARDWARE_INTERRUPT_H
#define HARDWARE_INTERRUPT_H

//opto/s
volatile int  optoAstate = 0;
volatile int  optoBstate = 0;
void init_optos();
void doOptoA();
void doOptoB();

//encoder0
volatile long  encoder0Pos = 0;
void init_encoders();
void doEncoder0();

#endif
