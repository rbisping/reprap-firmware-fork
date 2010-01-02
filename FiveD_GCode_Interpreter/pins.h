#ifndef PINS_H
#define PINS_H

#if MOTHERBOARD == 0

#error The Arduino_168 cannot run the 5D GCode interpreter
* Arduino_168 pin assignment - left here as they might be useful
*
****************************************************************************************/

#define X_STEP_PIN (byte)2
#define X_DIR_PIN (byte)3
#define X_MIN_PIN (byte)4
#define X_MAX_PIN (byte)9

#define Y_STEP_PIN (byte)10
#define Y_DIR_PIN (byte)7
#define Y_MIN_PIN (byte)8
#define Y_MAX_PIN (byte)13

#define Z_STEP_PIN (byte)19
#define Z_DIR_PIN (byte)18
#define Z_MIN_PIN (byte)17
#define Z_MAX_PIN (byte)16

#define BASE_HEATER_PIN   (byte)-1
#define POWER_SUPPLY_PIN (byte)-1

//extruder pins
#define EXTRUDER_0_MOTOR_SPEED_PIN  (byte)11
#define EXTRUDER_0_MOTOR_DIR_PIN    (byte)12
#define EXTRUDER_0_HEATER_PIN       (byte)6
#define EXTRUDER_0_FAN_PIN          (byte)5
#define EXTRUDER_0_TEMPERATURE_PIN  (byte)0  // Analogue input
#define EXTRUDER_0_VALVE_DIR_PIN             (byte)16       //NB: Conflicts with Max Z!!!!
#define EXTRUDER_0_VALVE_ENABLE_PIN          (byte)15 
#define EXTRUDER_0_STEP_ENABLE_PIN  5 // 5 - NB conflicts with the fan; set -ve if no stepper


/****************************************************************************************
* Sanguino/RepRap Motherboard with direct-drive extruders
*
****************************************************************************************/
#elif MOTHERBOARD == 1

#ifndef __AVR_ATmega644P__
#error Oops!  Make sure you have 'Sanguino' selected from the 'Tools -> Boards' menu.
#endif

//#define USE_EXTRUDER_CONTROLLER false

//cartesian bot pins
#define X_STEP_PIN (byte)15
#define X_DIR_PIN (byte)18
#define X_MIN_PIN (byte)20
#define X_MAX_PIN (byte)21
#define X_ENABLE_PIN (byte)8

#define Y_STEP_PIN (byte)23
#define Y_DIR_PIN (byte)22
#define Y_MIN_PIN (byte)25
#define Y_MAX_PIN (byte)26
#define Y_ENABLE_PIN (byte)8

#define Z_STEP_PIN (byte)29
#define Z_DIR_PIN (byte)30
#define Z_MIN_PIN (byte)1
#define Z_MAX_PIN (byte)2
#define Z_ENABLE_PIN (byte)8

#define BASE_HEATER_PIN   (byte)-1
#define POWER_SUPPLY_PIN (byte)-1

//first extruder pins
#define EXTRUDER_0_MOTOR_SPEED_PIN   (byte)12
#define EXTRUDER_0_MOTOR_DIR_PIN     (byte)16
#define EXTRUDER_0_HEATER_PIN        (byte)14
#define EXTRUDER_0_FAN_PIN           (byte)3
#define EXTRUDER_0_TEMPERATURE_PIN  (byte)4    // Analogue input
#define EXTRUDER_0_VALVE_DIR_PIN     (byte)17
#define EXTRUDER_0_VALVE_ENABLE_PIN  (byte)13  // Valve needs to be redesigned not to need this
#define EXTRUDER_0_STEP_ENABLE_PIN  (signed int)3  // 3 - Conflicts with the fan; set -ve if no stepper

// second extruder pins, if used:
#ifdef EXTRUDER_TYPE_1
#define EXTRUDER_1_MOTOR_SPEED_PIN   (byte)4
#define EXTRUDER_1_MOTOR_DIR_PIN    (byte)0
#define EXTRUDER_1_HEATER_PIN        (byte)24
#define EXTRUDER_1_FAN_PIN           (byte)7
#define EXTRUDER_1_TEMPERATURE_PIN  (byte)3  // Analogue input
#define EXTRUDER_1_VALVE_DIR_PIN    (byte) 6
#define EXTRUDER_1_VALVE_ENABLE_PIN (byte)5   // Valve needs to be redesigned not to need this 
#define EXTRUDER_1_STEP_ENABLE_PIN  (signed int)-1  // 7 - Conflicts with the fan; set -ve if no stepper
#endif

/****************************************************************************************
* RepRap Motherboard with RS485 extruders
*
****************************************************************************************/

#elif MOTHERBOARD == 2

#ifndef __AVR_ATmega644P__
#error Oops!  Make sure you have 'Sanguino' selected from the 'Tools -> Boards' menu.
#endif

//#define USE_EXTRUDER_CONTROLLER true

//x axis pins
#define X_STEP_PIN      15
#define X_DIR_PIN       18
#define X_MIN_PIN       20
#define X_MAX_PIN       21
#define X_ENABLE_PIN    19

//y axis pins
#define Y_STEP_PIN      23
#define Y_DIR_PIN       22
#define Y_MIN_PIN       25
#define Y_MAX_PIN       26
#define Y_ENABLE_PIN    24

//z axis pins
#define Z_STEP_PIN      27
#define Z_DIR_PIN       28
#define Z_MIN_PIN       30
#define Z_MAX_PIN       31
#define Z_ENABLE_PIN    29

#define BASE_HEATER_PIN   (byte)-1
#define POWER_SUPPLY_PIN (byte)-1

//our pin for debugging.
#define DEBUG_PIN        0

//our SD card pins
#define SD_CARD_WRITE    2
#define SD_CARD_DETECT   3
#define SD_CARD_SELECT   4

//our RS485 pins
#define TX_ENABLE_PIN	12
#define RX_ENABLE_PIN	13

//pin for controlling the PSU.
#define PS_ON_PIN       14

/****************************************************************************************
* Arduino Mega pin assignment
*
****************************************************************************************/

#elif MOTHERBOARD == 3

/*

BUZZS wiring:
0= (RX)
1= (TX)
2=DC_DIR_B - extruder motor
3=DC_PWM_B - extruder motor
4=DC_DIR_A - z-axis motor
5=DC_PWM_A - z-axis motor
6= X1
7= X2
8= -unused- min,max
9=PWM_3  - mini CPU/heater fan 
10=PWM_2 - unused ( broken - avail PWM connection )
11=PWM_1 - heater element
12= Y1 
13= Y2
14=ANALOG0= temperature sensor
15=ANALOG1= unused
16=ANALOG2= 
17=ANALOG3=
18=ANALOG4=
19=ANALOG5=
...
50=opto endstop Y
51=opto endstop X
52= MOUSE CLOCK
53= MOUSE DATA

*/


#define USE_EXTRUDER_CONTROLLER false

//cartesian bot pins
#define X_STEP_PIN (byte)6
#define X_DIR_PIN (byte)7
#define X_MIN_PIN (byte)21
#define X_MAX_PIN (byte)21   // must support interrupt if MEGA, and be setup
#define X_MAX_PIN (byte)8 //NC
#define X_ENABLE_PIN (byte)24

#define Y_STEP_PIN (byte)12
#define Y_DIR_PIN (byte)13
#define Y_MIN_PIN (byte)20  // must support interrupt, if MEGA, and be setup
#define Y_MAX_PIN (byte)8 //NC
#define Y_ENABLE_PIN (byte)27

#define Z_STEP_PIN (byte)5
#define Z_DIR_PIN (byte)4
#define Z_MIN_PIN (byte)22 
#define Z_MAX_PIN (byte)8 //NC
#define Z_ENABLE_PIN (byte)8 //NC

#define BASE_HEATER_PIN   (byte)-1
#define POWER_SUPPLY_PIN (byte)-1


//extruder pins
#define EXTRUDER_0_MOTOR_SPEED_PIN   (byte)3
#define EXTRUDER_0_MOTOR_DIR_PIN     (byte)2
#define EXTRUDER_0_HEATER_PIN        (byte)11
#define EXTRUDER_0_FAN_PIN           (byte)9
#define EXTRUDER_0_TEMPERATURE_PIN  (byte)0    // Analogue input
#define EXTRUDER_0_VALVE_DIR_PIN     (byte)8 //NC
#define EXTRUDER_0_VALVE_ENABLE_PIN  (byte)8 //NC  // Valve needs to be redesigned not to need this
#define EXTRUDER_0_STEP_ENABLE_PIN  (signed int)-1  //Conflicts with the fan; set -ve if no stepper
// altername pin names, more applicable when used with stepper driven extruder, but only for single extruder setups:
#define E_STEP_PIN EXTRUDER_0_MOTOR_SPEED_PIN
#define E_DIR_PIN EXTRUDER_0_MOTOR_DIR_PIN

#ifdef EXTRUDER_TYPE_1
/* 
#define EXTRUDER_1_MOTOR_SPEED_PIN   (byte)-1
#define EXTRUDER_1_MOTOR_DIR_PIN    (byte)-1
#define EXTRUDER_1_HEATER_PIN        (byte)-1
#define EXTRUDER_1_FAN_PIN           (byte)-1
#define EXTRUDER_1_TEMPERATURE_PIN  (byte)-1  // Analogue input
#define EXTRUDER_1_VALVE_DIR_PIN    (byte)-1
#define EXTRUDER_1_VALVE_ENABLE_PIN (byte)-1   // Valve needs to be redesigned not to need this 
#define EXTRUDER_1_STEP_ENABLE_PIN  (signed int)-1  // 7 - Conflicts with the fan; set -ve if no stepper
*/
#endif


// additional optional pins for the mega
//CAUTION: don't change these pin numbers without good cause, as the hardware required THIS pin ( hardware interrupts)!
#define OPTOA 21  //Xaxis, interrupt 2 
#define OPTOB 20  //Yaxis, interrupt 3
#define ENCODER0 19 //first encoder wheel, interrupt 4
//#define ENCODER1 18 // next encoder wheel?, interrupt 5

/****************************************************************************************
* Duemilanove w/ ATMega328P pin assignment
*
****************************************************************************************/

#elif MOTHERBOARD == 4

#ifndef __AVR_ATmega328P__
#error Oops!  Make sure you have 'Arduino Duemilanove w/ ATMega328' selected from the 'Tools -> Boards' menu.
#endif

#define USE_EXTRUDER_CONTROLLER false

#define X_STEP_PIN (byte)19
#define X_DIR_PIN (byte)18
#define X_MIN_PIN (byte)17
#define X_MAX_PIN (byte)-1
#define X_ENABLE_PIN (byte)-1

#define Y_STEP_PIN (byte)10
#define Y_DIR_PIN (byte)7
#define Y_MIN_PIN (byte)8
#define Y_MAX_PIN (byte)-1
#define Y_ENABLE_PIN (byte)-1

#define Z_STEP_PIN (byte)13
#define Z_DIR_PIN (byte)3
#define Z_MIN_PIN (byte)4
#define Z_MAX_PIN (byte)-1
#define Z_ENABLE_PIN (byte)-1

#define BASE_HEATER_PIN   (byte)1
#define POWER_SUPPLY_PIN (byte)16

//extruder pins
#define EXTRUDER_0_MOTOR_SPEED_PIN   (byte)11
#define EXTRUDER_0_MOTOR_DIR_PIN     (byte)12
#define EXTRUDER_0_HEATER_PIN        (byte)6
#define EXTRUDER_0_FAN_PIN           (byte)5
#define EXTRUDER_0_TEMPERATURE_PIN  (byte)0   // Analogue input
#define EXTRUDER_0_VALVE_DIR_PIN     (byte)-1
#define EXTRUDER_0_VALVE_ENABLE_PIN  (byte)-1  // Valve needs to be redesigned not to need this
#define EXTRUDER_0_STEP_ENABLE_PIN  (byte)2  // 3 - Conflicts with the fan; set -ve if no stepper

#define EXTRUDER_1_MOTOR_SPEED_PIN   (byte)-1
#define EXTRUDER_1_MOTOR_DIR_PIN    (byte)-1
#define EXTRUDER_1_HEATER_PIN        (byte)-1
#define EXTRUDER_1_FAN_PIN           (byte)-1
#define EXTRUDER_1_TEMPERATURE_PIN  (byte)-1  // Analogue input
#define EXTRUDER_1_VALVE_DIR_PIN    (byte)-1
#define EXTRUDER_1_VALVE_ENABLE_PIN (byte)-1   // Valve needs to be redesigned not to need this 
#define EXTRUDER_1_STEP_ENABLE_PIN  (byte)-1  // 7 - Conflicts with the fan; set -ve if no stepper


#else

#error Unknown MOTHERBOARD value in parameters.h

#endif

#endif
