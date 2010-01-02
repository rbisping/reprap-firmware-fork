#include "WProgram.h"
#include "parameters.h"
#include "pins.h"
#include "extruder.h"
#include "vectors.h"
#include "cartesian_dda.h"

// only do this on the mega, which has lots of spare hardware interrupts: 
#ifdef INTERRUPT_ENDSTOPS 
//-------------------------------------------------------------------------------------------------
// Interrupt-based End-Stops for the major 2 directions.  elsewhere we then just check if these variables are changed magically 
volatile int  optoAstate = 0;      //the optoA variable.
volatile int  optoBstate = 0;      //the optoB variable.
// we will use these interrupts and pins -  interrupt 2 (pin 21), interrupt 3 (pin 20)
//#define OPTOA 21  //Xaxis
//#define OPTOB 20  //Yaxis

void init_optos() {
  attachInterrupt(2, doOptoA, CHANGE);  
  attachInterrupt(3, doOptoB, CHANGE);
  pinMode (OPTOA,INPUT);  
  //digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor, seems unneeded in my case
  pinMode (OPTOB,INPUT);
  //digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor, seems unneeded in my case
}
// save the opto states into something easy to review, but only when an external trigger told us to! 
void doOptoA(){
    optoAstate = digitalRead(OPTOA);
}
void doOptoB(){
  optoBstate = digitalRead(OPTOB);
}

#endif
//-------------------------------------------------------------------------------------------------

#ifdef INTERRUPT_ENCODERS

// Interrupt-based Encoder-wheel for an/the extruder: 
volatile long  encoder0Pos = 0;      //the encoder position variable.
// we will use these interrupts and pins -  interrupt 4 (pin 19), interrupt 5 (pin 18)
//#define ENCODER0 19
//#define ENCODER1 18  //  if you have another encoder? ( or you want to read the full quadrature signal) you may want to use this!  

void init_encoders() {
  attachInterrupt(4, doEncoder0, CHANGE);   // whenever pin 19 changes, the function below  will be called! 
 // attachInterrupt(5, doEncoder1, CHANGE);
 
  //pinMode (ENCODER1,INPUT);
  //digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor
  pinMode (ENCODER0,INPUT);
  //digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor
}

//really simple interrupt, so we can process them at a super high speed! ( eg I use an encoder wheel with >600 edges, totally unneeded, but I had one) 
void doEncoder0(){
 encoder0Pos++;
}
//void doEncoder1(){
// encoder1Pos++;
//}

#endif
//-------------------------------------------------------------------------------------------------

