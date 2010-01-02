

#ifndef HARDWARE_INTERRUPT_H
#define HARDWARE_INTERRUPT_H

//opto/s
#ifdef INTERRUPT_ENDSTOPS
volatile int  optoAstate = 0;
volatile int  optoBstate = 0;
void init_optos();
void doOptoA();
void doOptoB();
#endif

//encoder0
volatile long  encoder0Pos = 0;
void init_encoders();
void doEncoder0();

#endif
