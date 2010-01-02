
// Class for controlling each extruder
//
// Adrian Bowyer 14 May 2009

#ifndef EXTRUDER_H
#define EXTRUDER_H

#define EXTRUDER_FORWARD true
#define EXTRUDER_REVERSE false

// optionally enable extruder two based on setting in parameters.h
#ifdef EXTRUDER_TYPE_0 
#define EXTRUDER_COUNT 1
#endif
#ifdef EXTRUDER_TYPE_1 
#define EXTRUDER_COUNT 2
#endif

void manage_all_extruders();

void new_extruder(byte e);

/*
*********************************************************************************************
* Sanguino/Mega/RepRap Motherboard v 1.0 ( ie directly connected extruder, no RS485 ) 
*********************************************************************************************
*/

#if USE_EXTRUDER_CONTROLLER == false


class extruder
{
private:

//these our the default values for the extruder.
    byte e_speed;
    int target_celsius;
    int max_celsius;
    byte heater_low;
    byte heater_high;
    byte heater_current;
    int extrude_step_count;
    bool can_step;  // bool, defualts to 1, if 0, we disallow any extruder movement.

// These are used for temperature control    
    byte count ;
    int oldT, newT;
    
//this is for doing encoder based extruder control
//    int rpm;
//    long e_delay;
//    int error;
//    int last_extruder_error;
//    int error_delta;
    bool e_direction;
    bool valve_open;

    int encoder_target; // for encoder-based DC motor control only 

// The pins we control
    byte motor_dir_pin, motor_speed_pin, heater_pin, fan_pin, temp_pin, valve_dir_pin, valve_en_pin;
    signed int step_en_pin;
    
     byte wait_till_hot();
     //byte wait_till_cool();
     void temperature_error(); 
     int sample_temperature();
     
public:

   extruder(byte md_pin, byte ms_pin, byte h_pin, byte f_pin, byte t_pin, byte vd_pin, byte ve_pin, signed int se_pin);
   void wait_for_temperature();
   void valve_set(bool open, int dTime);

   void set_direction(bool direction);
   //void set_speed(float es);
   void set_cooler(byte e_speed);
   void set_target_temperature(int temp);
   int get_target_temperature();
   int get_temperature();
   void manage();

   void hold();
   void resume();
 
// Interrupt setup and handling functions for stepper-driven extruders
   
   //void interrupt();
   void step(bool actual);

   void enableStep();
   void disableStep();
   
};

// enable/disable the hardware from stepping, if it supports it, otherwise 
// use a soft-stop to prevent any stepping even if requested
inline void extruder::enableStep()
{
  if(step_en_pin < 0) {
    resume();  //resume from soft-stop
  } else {
    digitalWrite(step_en_pin, ENABLE_ON); 
  }
}

inline void extruder::disableStep()
{
  if(step_en_pin < 0){
    hold(); //soft stop
  } else {
    digitalWrite(step_en_pin, !ENABLE_ON); //optional hard stop
  }
}

// pretend to disable the stepper by stopping all motion/steps
inline void extruder::hold(){
  //espeed is the user_requested speed from set_speed() function, we DON'T want to change that here or the user-supplied value will be lost.
  can_step = false;
}
inline void extruder::resume(){
  can_step = true;
  //set_speed(e_speed);    //espeed is the previously user_requested speed
}

/* moved to .pde as it's non-trivial now
inline void extruder::step()
{
   digitalWrite(motor_speed_pin, HIGH);
   delayMicroseconds(5);
   digitalWrite(motor_speed_pin, LOW);
}
*/

inline void extruder::temperature_error()
{
      Serial.print("E: ");
      Serial.println(get_temperature());  
}

//warmup if we're too cold; cool down if we're too hot
inline void extruder::wait_for_temperature()
{
/*
  if(wait_till_cool())
   {
      temperature_error();
      return;
   }
*/
   if(wait_till_hot())
     temperature_error();
}

inline void extruder::set_direction(bool dir)
{
	e_direction = dir;
        hold(); // pause extruder, first, so we don't blow the driver chips with back-EMF! 
        delay( 100); // todo trinm this to be as small as possible! 
	digitalWrite(motor_dir_pin, e_direction);
        resume(); 
}

inline void extruder::set_cooler(byte sp)
{
  if(step_en_pin >= 0) // Step enable conflicts with the fan
    return;
  analogWrite(fan_pin, sp);
}

/**********************************************************************************************

* RepRap Motherboard v 1.x (x >= 1)
* Extruder is now on RS485

*/

#else

class extruder
{
private:

  byte address;
 
public:
   extruder(byte a);
   void wait_for_temperature();
   void valve_set(bool open, int dTime);
   void set_direction(bool direction);
   void set_cooler(byte e_speed);
   void set_temperature(int temp);
   int get_temperature();
   void manage();
   void step();

   void enableStep();
   void disableStep();
   
};

inline extruder::extruder(byte a)
{
  address = a;
}

inline  void extruder::wait_for_temperature()
{
  
}

inline  void extruder::valve_set(bool open, int dTime)
{
   if(open)
     talker.sendPacket(address, "V1");
   else
     talker.sendPacket(address, "V0");
   delay(dTime);
}

inline void extruder::set_direction(bool direction)
{
  
}

inline  void extruder::set_cooler(byte e_speed)
{
  
}

inline  void extruder::set_temperature(int temp)
{
  
}

inline  int extruder::get_temperature()
{
  return 1;  
}

inline  void extruder::manage()
{
  
}

inline  void extruder::step()
{
  
}

inline  void extruder::enableStep()
{
  
}

inline  void extruder::disableStep()
{
  
}

#endif

extern extruder* ex[];
extern byte extruder_in_use;

#endif
