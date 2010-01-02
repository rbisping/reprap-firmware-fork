
#include "parameters.h"
#include "pins.h"
#include "ThermistorTable.h"
#include "extruder.h" 

// Keep all extruders up to temperature etc.

void manage_all_extruders()
{
    for(byte i = 0; i < EXTRUDER_COUNT; i++)
       ex[i]->manage();
}

// Select a new extruder

void new_extruder(byte e)
{
  if(e < 0)
    e = 0;
  if(e >= EXTRUDER_COUNT)
    e = EXTRUDER_COUNT - 1;
  
  if(e != extruder_in_use)
  {  
    extruder_in_use = e;
    //setExtruder();
  }
}

/***************************************************************************************************************************

If we have a new motherboard (V 1.x, x >= 1), the extruder is entirely controlled via the RS485, and all  the functions to do
it are simple inlines in extruder.h

Otherwise, we have to do the work ourselves...
*/

#if USE_EXTRUDER_CONTROLLER == false
extruder::extruder(byte md_pin, byte ms_pin, byte h_pin, byte f_pin, byte t_pin, byte vd_pin, byte ve_pin, signed int se_pin)
{
         motor_dir_pin = md_pin;
         motor_speed_pin = ms_pin;
         heater_pin = h_pin;
         fan_pin = f_pin;
         temp_pin = t_pin;
         valve_dir_pin = vd_pin;
         valve_en_pin = ve_pin;
         step_en_pin = se_pin;
         
	//setup our pins
	pinMode(motor_dir_pin, OUTPUT);
	pinMode(motor_speed_pin, OUTPUT);
	pinMode(heater_pin, OUTPUT);

	pinMode(temp_pin, INPUT);
	pinMode(valve_dir_pin, OUTPUT); 
        pinMode(valve_en_pin, OUTPUT);

	//initialize values
	digitalWrite(motor_dir_pin, EXTRUDER_FORWARD);
	
	analogWrite(heater_pin, 0);
	analogWrite(motor_speed_pin, 0);
	digitalWrite(valve_dir_pin, false);
	digitalWrite(valve_en_pin, 0);

// The step enable pin and the fan pin are the same...
// We can have one, or the other, but not both

        if(step_en_pin >= 0)
        {
          pinMode(step_en_pin, OUTPUT);
	  disableStep();
        } else
        {
	  pinMode(fan_pin, OUTPUT);
          analogWrite(fan_pin, 0);
        }

        //these our the default values for the extruder.
        e_speed = 0;
        //target_celsius = 17;
        target_celsius = 240;  //default to HOT!
        max_celsius = 0;
        heater_low = 64;
        heater_high = 255;
        heater_current = 0;
        valve_open = false;
        can_step = true; 
        
//this is for doing encoder based extruder control
//        rpm = 0;
//        e_delay = 0;
//        error = 0;
//        last_extruder_error = 0;
//        error_delta = 0;
        e_direction = EXTRUDER_FORWARD;
        
        encoder_target = 0; // for DVC driven extruder/s        
        
        //set default temp
        set_target_temperature(target_celsius);
}


byte extruder::wait_till_hot()
{  
  count = 0;
  oldT = get_temperature();
  while (get_temperature() < target_celsius - HALF_DEAD_ZONE)
  {
	manage_all_extruders();
        count++;
        if(count > 20)
        {
            newT = get_temperature();
            if(newT > oldT)
               oldT = newT;
            else
                return 1;
            count = 0;
        }
	delay(1000);
  }
  return 0;
}

/*
byte extruder::wait_till_cool()
{  
  count = 0;
  oldT = get_temperature();
  while (get_temperature() > target_celsius + HALF_DEAD_ZONE)
  {
	manage_all_extruders();
        count++;
        if(count > 20)
        {
            newT = get_temperature();
            if(newT < oldT)
               oldT = newT;
            else
                return 1;
            count = 0;
        }
	delay(1000);
  }
  return 0;
}
*/



void extruder::valve_set(bool open, int dTime)
{
        wait_for_temperature();
	valve_open = open;
	digitalWrite(valve_dir_pin, open);
        digitalWrite(valve_en_pin, 1);
        delay(dTime);
        digitalWrite(valve_en_pin, 0);
}


void extruder::set_target_temperature(int temp)
{
	target_celsius = temp;
	max_celsius = (temp*11)/10;

        // If we've turned the heat off, we might as well disable the extrude stepper
       // if(target_celsius < 1)
        //  disableStep(); 
}

int extruder::get_target_temperature()
{
        return target_celsius;
}

/**
*  Samples the temperature and converts it to degrees celsius.
*  Returns degrees celsius.
*/
int extruder::get_temperature()
{
#ifdef USE_THERMISTOR
	int raw = sample_temperature();

	int celsius = 0;
	byte i;

	for (i=1; i<NUMTEMPS; i++)
	{
		if (temptable[i][0] > raw)
		{
			celsius  = temptable[i-1][1] + 
				(raw - temptable[i-1][0]) * 
				(temptable[i][1] - temptable[i-1][1]) /
				(temptable[i][0] - temptable[i-1][0]);

			break;
		}
	}

        // Overflow: Set to last value in the table
        if (i == NUMTEMPS) celsius = temptable[i-1][1];

        // Clamp
        if (celsius > 511) celsius = 511;
        else if (celsius < 0) celsius = 0; 
  
	return celsius;
#else
  return ( 5.0 * sample_temperature() * 100.0) / 1024.0;
#endif
}



/*
* This function gives us an averaged sample of the analog temperature pin.
*/
int extruder::sample_temperature()
{
	int raw = 0;
	
	//read in a certain number of samples
	for (byte i=0; i<TEMPERATURE_SAMPLES; i++)
		raw += analogRead(temp_pin);
		
	//average the samples
	raw = raw/TEMPERATURE_SAMPLES;

	//send it back.
	return raw;
}

/*!
  Manages extruder functions to keep temps, speeds etc
  at the set levels.  Should be called only by manage_all_extruders(),
  which should be called in all non-trivial loops.
  o If temp is too low, don't start the motor
  o Adjust the heater power to keep the temperature at the target
 */
void extruder::manage()
{
	//make sure we know what our temp is.
	int current_celsius = get_temperature();
        byte newheat = 0;
  
        //put the heater into high mode if we're not at our target.
        if (current_celsius < target_celsius)
                newheat = heater_high;
        //put the heater on low if we're at our target.
        else if (current_celsius < max_celsius)
                newheat = heater_low;
        
        // Only update heat if it changed
        if (heater_current != newheat) {
                heater_current = newheat;
                analogWrite(heater_pin, heater_current);
        }
}


void extruder::step(bool actual)
{
// 'actual' determines if we are doing a extruder step requireing new movement, or a calculation requiring no further movement
int step_type = ENCODER_MANAGED_DC;  // manual override to force a specific step type for my extruder.  comment out to use same stepping as XYZ as set in parameters.h

if (can_step == false) return;  //don't move if we are on hold()

if ( step_type == STEP_DIR ){
   if ( actual == false ) return ; 
   digitalWrite(motor_speed_pin, HIGH);
   delayMicroseconds(5);
   digitalWrite(motor_speed_pin, LOW); 
}
if ( step_type == GRAY_CODE ){
   if ( actual == false ) return ; 
#if INVERT_E_DIR == 1
          if ( e_direction ) { e_quadrature_state++; } else { e_quadrature_state--; }
#else
           if ( e_direction ) { e_quadrature_state--; } else { e_quadrature_state++; }
#endif
          if ( e_quadrature_state > 3 ) { e_quadrature_state = 0; }  
          if ( e_quadrature_state < 0 ) { e_quadrature_state = 3; } 
          int gray_code = e_quadrature_state ^ ( e_quadrature_state >> 1 ) ; //two significant bits in gray_code are the two pin states we want.
          int E1 = gray_code & 1; //lower order bit
          int E2 = gray_code & 2;  //higher order bit
         // write the quadrature/grey code to the two pins commonly referrred to as STEP and DIRECTION ( despite them not actually being that in this case) 
          digitalWrite(motor_speed_pin, E1);  //equivalent to E_STEP_PIN 
          digitalWrite(motor_dir_pin,  E2);   //equivalent to E_DIR_PIN 
          delayMicroseconds(5);
}
if ( step_type == ENCODER_MANAGED_DC ){
//  set the number of encoder pulses it should take to move one virtual "step": 
//if you have a low-res encoder you might just set to match the numberof edges on your encoder wheel, so one "step" = 1 RPM, 
//or even better calculate how many encoder edges it takes to extrude a mm of material! 

// this is actually a game of cat-n-mouse between the DDA which calculates the desired absolute "position" and the DC motor extruder which counts how many ticks have occured in the encoder.
// the motor is turned on if it's encoder is behind the absolute position count, and turned off if it's caught-up or ahead of the count.

// each time we are asked to 'step' we are really asked to change the encoder target! 
 if ( actual == true ) { 
  encoder_target += E_INTERRUPTS_PER_STEP; 
 }
//every time the user-requested speed changes, we reset the global counters for elapsed time, and elapsed mouse ticks.
   float target_speed = 0; 
   
    if ( encoder0Pos < encoder_target ) {
      target_speed = 100;
   } else if ( encoder0Pos > encoder_target  ) {
     target_speed = 0;
   }  else { // current_speed = desired_speed
     target_speed = 100;
   }

   // if ( target_speed != encoder_speed ) {
        analogWrite(motor_speed_pin, target_speed);
   // }

}
if ( step_type == UNMANAGED_DC ){
// TODO implement properly by timing how many ms we have run for, or somehting? 
   digitalWrite(motor_speed_pin, HIGH);
   delayMicroseconds(5);  // in the mean time, adjust this value to something way bigger to make this work! 
   digitalWrite(motor_speed_pin, LOW); ;
}
}

/* 
#if 0
void extruder::set_speed(float sp)
{
  // DC motor?
    if(step_en_pin < 0)
    {
      e_speed = (byte)sp;
      if(e_speed > 0)
          wait_for_temperature();
      analogWrite(motor_speed_pin, e_speed);
      return;
    }
      
    // No - stepper
  disableTimerInterrupt();
  
  if(sp <= 1.0e-4)
  {
    disableStep();
    e_speed = 0; // Just use this as a flag
    return;
  } else
  {
    wait_for_temperature();
    enableStep();
    e_speed = 1;
  }
    
  extrude_step_count = 0;
  
  float milliseconds_per_step = 60000.0/(E_STEPS_PER_MM*sp);
  long thousand_ticks_per_step = 4*(long)(milliseconds_per_step);
  setupTimerInterrupt();
  setTimer(thousand_ticks_per_step);
  enableTimerInterrupt();
}


void extruder::interrupt()
{
    if(!e_speed)
      return;
    extrude_step_count++;
    if(extrude_step_count > 1000)
    {
      step();
      extrude_step_count = 0;
    }
}
#endif
#endif
