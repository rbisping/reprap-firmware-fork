#ifndef PARAMETERS_H
#define PARAMETERS_H

// Here are the moterboard codes; set MOTHERBOARD to the right one
// Arduino_168 : 0 ( no longer in use)
// Sanguino or RepRap Motherboard with direct drive extruders: 1
// RepRap Motherboard with RS485 extruders: 2
// Arduino_Mega: 3
// Arudino_328: 4  ( eg: Duemilanove w/ ATMega328P ) 
  
#define MOTHERBOARD 3

//want to be able to parse 3D code without barfing? 
#define OLD3DCODE 1


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
#define DARWIN_DEFAULTS 1
#define USE_EXTRUDER_CONTROLLER false // ie no RS485 subsystem
#define USE_STEPPER_ENABLE 1
#define EXTRUDER_TYPE_0  STEP_DIR  // change to ENCODER_MANAGED_DC for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE STEP_DIR  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends
// For RepRap stepper boards version 1.x the enable pins are *not* inverting.
#define INVERT_ENABLE_PINS 0
// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops with H21LOI sensors are not inverting; ones with H21LOB are
#define ENDSTOPS_INVERTING 0
#endif 

#endif


// utilise the extra hardware on the "RepRap Motherboard with RS485" for enabling/disabling the stepper drivers as required ( to save power, etc ) , and define extruder type/s you have \
#if MOTHERBOARD == 2
#define MENDEL_DEFAULTS 1 //the common case is that you are a motherboard of type 2 are used for mendel
//or define BATHPROTO_DEFAULTS 1 
#define USE_EXTRUDER_CONTROLLER true // ie enable full RS485 subsystem
#define USE_STEPPER_ENABLE 1
#define EXTRUDER_TYPE_0  STEP_DIR  // change to ENCODER_MANAGED_DC for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE STEP_DIR  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends
// For RepRap stepper boards version 2.x and above the enable pins are inverting.
#define INVERT_ENABLE_PINS 0
// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops with H21LOI sensors are not inverting; ones with H21LOB are
#define ENDSTOPS_INVERTING 0
//#endif

// utilise the extra hardware interrupts on the MEGA for Opto end-stops, and Encoder WHeel/s, and define the basic hardware profile you have
#if MOTHERBOARD == 3
#define MEGA 1
#define CUSTOM_DEFAULTS 1
#define INTERRUPT_ENDSTOPS 1
#define INTERRUPT_ENCODERS 1
#define USE_EXTRUDER_CONTROLLER false  // ie no RS485 subsystem
#define EXTRUDER_TYPE_0  ENCODER_MANAGED_DC  // change to STEP_DIR for the other type of extruder
//#define EXTRUDER_TYPE_1  STEP_DIR  // uncomment only if two extruders are in use
// now pick one of the above for the 3 x XYZ axes based on Motherboard type
#define STEP_TYPE GRAY_CODE  //use same stepping type on all axes, including extruder.  to override on a per-axis basis, redefine in cartesian_dda::do_x_step and friends
// for buzzs board, they are inverting
#define INVERT_ENABLE_PINS 1
// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops with H21LOI sensors are not inverting; ones with H21LOB are
#define ENDSTOPS_INVERTING 1
#endif

// Set 1s where you have endstops; 0s where you don't
// this COULD be part of the motherboard features defined above, if we had any concensus on opto-types used.
// Both Darwin and Mendel have MIN endstops, but not MAX ones.

#define ENDSTOPS_MIN_ENABLED 1
#define ENDSTOPS_MAX_ENABLED 0

// The width of Henry VIII's thumb (or something).
#define INCHES_TO_MM 25.4 // *RO

/* 
 * next we decide which set of hardware-coordinate-axis-system 
 * parameters we want to use.
 * This is a bit of guesswork based on MOTHERBOARD type.
 *  types are MENDEL,DARWIN,MAKERBOT,CUSTOM(for repstraps etc),
 *  BATHPROTO,
 */

//*************************************************************************

#ifdef MENDEL_DEFAULTS

// define the XYZ parameters of Mendel

#define X_STEPS_PER_MM   10.047
#define X_STEPS_PER_INCH (X_STEPS_PER_MM*INCHES_TO_MM) // *RO
#define INVERT_X_DIR 0

#define Y_STEPS_PER_MM   10.047
#define Y_STEPS_PER_INCH (Y_STEPS_PER_MM*INCHES_TO_MM) // *RO
#define INVERT_Y_DIR 0

#define Z_STEPS_PER_MM   833.398
#define Z_STEPS_PER_INCH (Z_STEPS_PER_MM*INCHES_TO_MM) // *RO
#define INVERT_Z_DIR 0

// For when we have a stepper-driven extruder
// E_STEPS_PER_MM is the number of steps needed to 
// extrude 1mm out of the nozzle.
#define E_STEPS_PER_MM   0.706   // NEMA 17 extruder 5mm diameter drive - empirically adjusted
//#define E_STEPS_PER_MM   2.2       // NEMA 14 geared extruder 8mm diameter drive

#define FAST_XY_FEEDRATE 3000.0
#define FAST_Z_FEEDRATE  5.0
#endif
//*************************************************************************



#ifdef DARWIN_DEFAULTS
// This is for Darwin.

#define X_STEPS_PER_MM   7.99735
#define X_STEPS_PER_INCH (X_STEPS_PER_MM*INCHES_TO_MM) // *RO
#define INVERT_X_DIR 0

#define Y_STEPS_PER_MM   7.99735
#define Y_STEPS_PER_INCH (Y_STEPS_PER_MM*INCHES_TO_MM) // *RO
#define INVERT_Y_DIR 0

#define Z_STEPS_PER_MM   320
#define Z_STEPS_PER_INCH (Z_STEPS_PER_MM*INCHES_TO_MM) // *RO
#define INVERT_Z_DIR 0

// darwin doesn't have a stepper-driven extruder normally.
#define E_STEPS_PER_MM   0

#define FAST_XY_FEEDRATE 2000.0
#define FAST_Z_FEEDRATE  5.0

#endif
//*************************************************************************

#ifdef MAKERBOT_DEFAULTS
 //TODO determine reasonable default attribs for MAKERBOT here

// For when we have a stepper-driven extruder
// E_STEPS_PER_MM is the number of steps needed to 
// extrude 1mm out of the nozzle.
#define E_STEPS_PER_MM   0.706   // NEMA 17 extruder 5mm diameter drive - empirically adjusted
//#define E_STEPS_PER_MM   2.2       // NEMA 14 geared extruder 8mm diameter drive

#endif 
//*************************************************************************

#ifdef CUSTOM_DEFAULTS

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

//for ENCODER DC motors, this is "encoder wheel edges per MM of extrudate"
#define E_STEPS_PER_MM   1  //fixed at 1 for convenience sake - see below ( the steps() function is called with 'true' this often)
#define E_MOTOR_STEPS    400
// TODO implement these variables: For when we have a DC Driven Extruder:
// either:
#define E_MILLIS_PER_MM 300 // the number of miliseconds to produce 1mm of output when at full speed ( open loop extruder)
// or:
#define E_INTERRUPTS_PER_STEP 800 // depends on the number of slots in your encoder and the number of revolutions of the encoder it takes to make a MM of output. ( assuming E_STEPS_PER_WWMM = 1 ) 

//our maximum feedrates
#define FAST_XY_FEEDRATE 2000.0
#define FAST_Z_FEEDRATE  5.0

#endif


//*************************************************************************

#if BATHPROTO_DEFAULTS
// parameters for the Bath U. mendel prototype
#define X_STEPS_PER_MM   13.333333
#define X_STEPS_PER_INCH (X_STEPS_PER_MM*INCHES_TO_MM)
#define INVERT_X_DIR 0

#define Y_STEPS_PER_MM   13.333333
#define Y_STEPS_PER_INCH (Y_STEPS_PER_MM*INCHES_TO_MM)
#define INVERT_Y_DIR 0

#define Z_STEPS_PER_MM   938.64
#define Z_STEPS_PER_INCH (Z_STEPS_PER_MM*INCHES_TO_MM)
#define INVERT_Z_DIR 0

#endif
//*************************************************************************

//TODO add your own defaults here if you are using a repeatable platform
// eg, some mc-wire defaults, or similar might be nice.

//*************************************************************************


#define E_STEPS_PER_INCH (E_STEPS_PER_MM*INCHES_TO_MM) // *RO



// Data for acceleration calculations
// Comment out the next line to turn accelerations off
#define ACCELERATION_ON
#define SLOW_XY_FEEDRATE 1000.0 // Speed from which to start accelerating
#define SLOW_Z_FEEDRATE 20


#if INVERT_ENABLE_PINS == 1
#define ENABLE_ON LOW
#else
#define ENABLE_ON HIGH
#endif


// Set these to 1 to disable an axis when it's not being used,
// and for the extruder.  Usually only Z is disabled when not in
// use.  You will probably find that disabling the others (i.e.
// powering down the steppers that drive them) when the ends of
// movements are reached causes poor-quality builds.  (Inertia
// causes overshoot if the motors are not left powered up.)

#define DISABLE_X 0
#define DISABLE_Y 0
#define DISABLE_Z 1
#define DISABLE_E 0

// The number of 5-second intervals to wait at the target temperature for things to stabilise.
// Too short, and the extruder will jam as only part of it will be hot enough.
// Too long and the melt will extend too far up the insulating tube.
// Default value: 10

#define WAIT_AT_TEMPERATURE 10

//our command string length
#define COMMAND_SIZE 128 // *RO

// The size of the movement buffer
#define BUFFER_SIZE 4 // *RO

// Number of microseconds between timer interrupts when no movement
// is happening
#define DEFAULT_TICK (long)1000 // *RO

// What delay() value to use when waiting for things to free up in milliseconds
#define WAITING_DELAY 1 // *RO

#define HOST_BAUD 19200 // *RO


#define SMALL_DISTANCE 0.01 // *RO

// Useful to have its square

#define SMALL_DISTANCE2 (SMALL_DISTANCE*SMALL_DISTANCE) // *RO

// only for systems with RS485 is this reqd:
#if USE_EXTRUDER_CONTROLLER == true
#define MY_NAME 'H'           // Byte representing the name of this device
#define E0_NAME '0'           // Byte representing the name of extruder 0
#define E1_NAME '1'           // Byte representing the name of extruder 1
#endif

// used by intercom.pde even when we aren't using RS485 at all, so define it in all master/s.
#define RS485_MASTER  1       // *RO 

//******************************************************************************

// You probably only want to edit things below this line if you really really
// know what you are doing...

extern char debugstring[];

void delayMicrosecondsInterruptible(unsigned int us);

#ifndef INTPARAMETERS_H
#define INTPARAMETERS_H
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

//*************************************************************************
