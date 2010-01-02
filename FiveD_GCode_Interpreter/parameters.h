#ifndef PARAMETERS_H
#define PARAMETERS_H

// Here are the moterboard codes; set MOTHERBOARD to the right one
// Arduino_168 : 0 ( no longer in use)
// Sanguino or RepRap Motherboard with direct drive extruders: 1
// RepRap Motherboard with RS485 extruders: 2
// Arduino_Mega: 3
// Arudino_328: 4  ( eg: Duemilanove w/ ATMega328P ) 
  
#define MOTHERBOARD 4



// there are a number of different hardware methods for getting directional movement, here we decide which one we want to use normally: 
#define STEP_DIR 0      // standard step & direction information, recommended
#define GRAY_CODE 1     // also called quadrature stepping, less popular but basically same quality as above
#define UNMANAGED_DC 2  //open-ended DC motor using timer.  very low res, use for Z axis only, and only if you must
#define ENCODER_MANAGED_DC 3// closed-loop DC motor who's position is maintained by an opto or magneto encoder generating edges on an external imterrupt line. MEGA only at this time



//NOW WE DEFINE WHICH "FEATURES" each motherboard type has:

#if MOTHERBOARD == 0
#ifdef __AVR_ATmega328P__
#error Oops!  'Arduino Duemilanove w/ ATMega328' is motherboard type 4, not 0. see parameters.h
#endif
#error Oops, 5D firmware doesn't run on the Arduino 168 any more. please use an older firmware.
#endif

// utilise the extra hardware on the SANGUINO for enabling/disabling the stepper drivers as required ( to save power, etc ) , and define extruder type/s you have 
#if MOTHERBOARD == 1
#define SANGUINO 1
#define USE_EXTRUDER_CONTROLLER false
#define USE_STEPPER_ENABLE 1
#define EXTRUDER_TYPE_0  STEP_DIR  // change to ENCODER_MANAGED_DC for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE STEP_DIR  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends

#endif 

// utilise the extra hardware on the "RepRap Motherboard with RS485" for enabling/disabling the stepper drivers as required ( to save power, etc ) , and define extruder type/s you have 
#if MOTHERBOARD == 2 
#define RM485 1
#define USE_EXTRUDER_CONTROLLER true
#define USE_STEPPER_ENABLE 1
#define EXTRUDER_TYPE_0  STEP_DIR  // change to ENCODER_MANAGED_DC for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE STEP_DIR  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends

#endif 


// utilise the extra hardware interrupts on the MEGA for Opto end-stops, and Encoder WHeel/s, and define the basic hardware profile you have
#if MOTHERBOARD == 3
#define MEGA 1
#define INTERRUPT_ENDSTOPS 1
#define INTERRUPT_ENCODERS 1
#define EXTRUDER_TYPE_0  ENCODER_MANAGED_DC  // change to STEP_DIR for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment only if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE GRAY_CODE  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends

#endif

// Set 1s where you have endstops; 0s where you don't
// this COULD be part of the motherboard features defined above, if we had any concensus on opto-types used.
#define ENDSTOPS_MIN_ENABLED 1
#define ENDSTOPS_MAX_ENABLED 0


//our command string length
#define COMMAND_SIZE 128

// The size of the movement buffer

#define BUFFER_SIZE 4

// Number of microseconds between timer interrupts when no movement
// is happening

#define DEFAULT_TICK (long)1000

// What delay() value to use when waiting for things to free up in milliseconds

#define WAITING_DELAY 1

#define INCHES_TO_MM 25.4

// define the parameters of our machine.
#define X_STEPS_PER_MM   3.58
#define X_STEPS_PER_INCH (X_STEPS_PER_MM*INCHES_TO_MM)
#define X_MOTOR_STEPS    400 //1.8 deg/step
#define INVERT_X_DIR 0

// buzzs belt-driven y-axis
#define Y_STEPS_PER_MM   3.94 //82? 
#define Y_STEPS_PER_INCH (Y_STEPS_PER_MM*INCHES_TO_MM)
#define Y_MOTOR_STEPS    400
#define INVERT_Y_DIR 1

#define Z_STEPS_PER_MM   20 //10 = way too small, 30 = a bit too big
#define Z_STEPS_PER_INCH (Z_STEPS_PER_MM*INCHES_TO_MM)
#define Z_MOTOR_STEPS    400
#define INVERT_Z_DIR 1

// For when we have a stepper-driven extruder
// E_STEPS_PER_MM is the number of steps needed to 
// extrude 1mm out of the nozzle.

// STEPPER:
//#define E_STEPS_PER_MM   0.706   // 5mm diameter drive - empirically adjusted.   
//for ENCODER DC motors, this is "encoder wheel edges per MM of extrudate"
#define E_STEPS_PER_MM   1  //fixed at 1 for convenience sake - see below ( the steps() function is called with 'true' this often)
#define E_STEPS_PER_INCH (E_STEPS_PER_MM*INCHES_TO_MM)
#define E_MOTOR_STEPS    400

// TODO implement these variables: For when we have a DC Driven Extruder:
// either:
#define E_MILLIS_PER_MM 300 // the number of miliseconds to produce 1mm of output when at full speed ( open loop extruder)
// or:
#define E_INTERRUPTS_PER_STEP 800 // depends on the number of slots in your encoder and the number of revolutions of the encoder it takes to make a MM of output. ( assuming E_STEPS_PER_WWMM = 1 ) 
//our maximum feedrates
#define FAST_XY_FEEDRATE 2000.0
#define FAST_Z_FEEDRATE  5.0

// Data for acceleration calculations
// Comment out the next line to turn accelerations off
#define ACCELERATION_ON
#define SLOW_XY_FEEDRATE 1000.0 // Speed from which to start accelerating
#define SLOW_Z_FEEDRATE 20

// Set to 1 if enable pins are inverting
// For RepRap stepper boards version 1.x the enable pins are *not* inverting.
// For RepRap stepper boards version 2.x and above the enable pins are inverting.
#define INVERT_ENABLE_PINS 0

#if INVERT_ENABLE_PINS == 1
#define ENABLE_ON LOW
#else
#define ENABLE_ON HIGH
#endif

// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops are *not* inverting.
#define ENDSTOPS_INVERTING 1

// Inline interrupt control functions

inline void enableTimerInterrupt() 
{
   TIMSK1 |= (1<<OCIE1A);
}
	
inline void disableTimerInterrupt() 
{
     TIMSK1 &= ~(1<<OCIE1A);
}
        
inline void setTimerCeiling(unsigned int c) 
{
    OCR1A = c;
}

inline void resetTimer()
{
  TCNT2 = 0;
}

#endif
