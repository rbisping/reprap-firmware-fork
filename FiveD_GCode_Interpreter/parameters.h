#ifndef PARAMETERS_H
#define PARAMETERS_H

// Here are the moterboard codes; set MOTHERBOARD to the right one
// (Arduino: 0 - no longer in use)
// Sanguino or RepRap Motherboard with direct drive extruders: 1
// RepRap Motherboard with RS485 extruders: 2
// Arduino Mega: 3
// Arudino Duemilanove w/ ATMega328P: 4

#define MOTHERBOARD 4



// there are a number of different hardware methods for getting directional movement, here we decide which one we want to use normally: 
#define STEP_DIR 0      // standard step & direction information, recommended
#define GRAY_CODE 1     // also called quadrature stepping, less popular but basically same quality as above
#define UNMANAGED_DC 2  //open-ended DC motor using timer.  very low res, use for Z axis only, and only if you must
#define ENCODER_MANAGED_DC 3// closed-loop DC motor who's position is maintained by an opto or magneto encoder generating edges on an external imterrupt line. MEGA only at this time



//NOW WE DEFINE WHICH "FEATURES" each motherboard type has:

// utilise the extra hardware interrupts on the MEGA for Opto end-stops, and Encoder WHeel/s, and define the basic hardware profile you have
#ifdef MOTHERBOARD == 3  //MEGA
#define INTERRUPT_ENDSTOPS 1
#define INTERRUPT_ENCODERS 1
#define EXTRUDER_TYPE_0  ENCODER_MANAGED_DC  // change to STEP_DIR for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment only if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE GRAY_CODE  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends

#endif

// utilise the extra hardware on the SANGUINO for enabling/disabling the stepper drivers as required ( to save power, etc ) , and define extruder type/s you have 
#ifdef MOTHERBOARD == 1 // SANGUINO
#define USE_STEPPER_ENABLE 1
#define EXTRUDER_TYPE_0  STEP_DIR  // change to ENCODER_MANAGED_DC for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE STEP_DIR  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends

#endif 

// utilise the extra hardware on the SANGUINO for enabling/disabling the stepper drivers as required ( to save power, etc ) , and define extruder type/s you have 
#ifdef MOTHERBOARD == 2 // RepRap Motherboard with RS485
#define USE_STEPPER_ENABLE 1
#define EXTRUDER_TYPE_0  STEP_DIR  // change to ENCODER_MANAGED_DC for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE STEP_DIR  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends

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
#define X_STEPS_PER_MM   9.8482399079
#define X_STEPS_PER_INCH (X_STEPS_PER_MM*INCHES_TO_MM)
#define X_MOTOR_STEPS    400
#define INVERT_X_DIR 0

#define Y_STEPS_PER_MM   9.8482399079
#define Y_STEPS_PER_INCH (Y_STEPS_PER_MM*INCHES_TO_MM)
#define Y_MOTOR_STEPS    400
#define INVERT_Y_DIR 1

#define Z_STEPS_PER_MM   160 // 200 steps per revoloution / 1.25 mm thread pitch
#define Z_STEPS_PER_INCH (Z_STEPS_PER_MM*INCHES_TO_MM)
#define Z_MOTOR_STEPS    200
#define INVERT_Z_DIR 0

// For when we have a stepper-driven extruder
// E_STEPS_PER_MM is the number of steps needed to 
// extrude 1mm out of the nozzle.

#define E_STEPS_PER_MM   0.7525056 // drive diameter = 4.7 mm, material in:material out = 36:1, 400 steps per revolution
#define E_STEPS_PER_INCH (E_STEPS_PER_MM*INCHES_TO_MM)
#define E_MOTOR_STEPS    400

//our maximum feedrates
#define FAST_XY_FEEDRATE 4000.0
#define FAST_Z_FEEDRATE  60.0

// Data for acceleration calculations
// Comment out the next line to turn accelerations off
//#define ACCELERATION_ON
#define SLOW_XY_FEEDRATE 1000.0 // Speed from which to start accelerating
#define SLOW_Z_FEEDRATE 20

// Set to 1 if enable pins are inverting
// For RepRap stepper boards version 1.x the enable pins are *not* inverting.
// For RepRap stepper boards version 2.x and above the enable pins are inverting.
#define INVERT_ENABLE_PINS 1

#if INVERT_ENABLE_PINS == 1
#define ENABLE_ON LOW
#else
#define ENABLE_ON HIGH
#endif

// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops are *not* inverting.
#define ENDSTOPS_INVERTING 0

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
