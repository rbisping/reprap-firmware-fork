
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

private:

//these our the default values for the extruder.
    byte e_speed;
    int target_celsius;
    int max_celsius;
    byte heater_low;
    byte heater_high;
    byte heater_current;
    int extrude_step_count;
    bool can_step;  // bool, defaults to 1, if 0, we disallow any extruder movement.

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


#define WAIT_T 'W'        // wait_for_temperature();
#define VALVE 'V'         // valve_set(bool open, int dTime);
#define DIRECTION 'D'     // set_direction(bool direction);
#define COOL 'C'          // set_cooler(byte e_speed);
#define SET_T 'T'         // set_target_temperature(int temp);
#define GET_T 't'         // get_temperature();
#define STEP 'S'          // step();
#define ENABLE 'E'        // enableStep();
#define DISABLE 'e'       // disableStep();
#define PREAD 'R'         // read the pot voltage
#define SPWM 'M'          // Set the motor PWM
#define PING 'P'          // Just acknowledge

class extruder
{

public:
   extruder(byte a);
   void wait_for_temperature();
   void valve_set(bool open, int dTime);
   void set_direction(bool direction);
   void set_cooler(byte e_speed);
   void set_target_temperature(int temp);
   int get_target_temperature();
   void manage();
   void step();

   void enableStep();
   void disableStep();
   int potVoltage();
   void setPWM(int p);
   bool ping();

private:

   char my_name;
   int target_celcius;
   int count;
   int oldT, newT;
   char commandBuffer[RS485_BUF_LEN];
   char* reply;
   bool stp;

   void buildCommand(char c);   
   void buildCommand(char c, char v);
   void buildNumberCommand(char c, int v);
   void temperatureError();  
};

inline extruder::extruder(char name)
{
  my_name = name;
  pinMode(E_STEP_PIN, OUTPUT);
  pinMode(E_DIR_PIN, OUTPUT);
  digitalWrite(E_STEP_PIN, 0);
  digitalWrite(E_DIR_PIN, 0);
  setTemperature(0);
  stp = false;
}

inline void extruder::buildCommand(char c)
{
  commandBuffer[0] = c;
  commandBuffer[1] = 0;  
}

inline void extruder::buildCommand(char c, char v)
{
  commandBuffer[0] = c;
  commandBuffer[1] = v;
  commandBuffer[2] = 0;  
}

inline void extruder::buildNumberCommand(char c, int v)
{
  commandBuffer[0] = c;
  itoa(v, &commandBuffer[1], 10);
}




inline  void extruder::valve_set(bool open, int dTime)
{
   if(open)
     buildCommand(VALVE, '1');
   else
     buildCommand(VALVE, '0');
   talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer);
   
   delay(dTime);
}

inline  void extruder::set_cooler(byte e_speed)
{   
   buildNumberCommand(COOL, e_speed);
   talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer);
}

inline  void extruder::set_target_temperature(int temp)
{
   target_celcius = temp;
   buildNumberCommand(SET_T, temp);
   talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer); 
}

inline  int extruder::get_target_temperature()
{
   buildCommand(GET_T);
   char* reply = talker.sendPacketAndGetReply(my_name, commandBuffer);
   return(atoi(reply));
}

inline  void extruder::manage()
{

}

inline void extruder::set_direction(bool direction)
{
//   if(direction)
//     buildCommand(DIRECTION, '1');
//   else
//     buildCommand(DIRECTION, '0');
//   talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer);
   if(direction)
     digitalWrite(E_DIR_PIN, 1);
   else
     digitalWrite(E_DIR_PIN, 0);
}


inline  void extruder::step()
{
   //buildCommand(STEP);
   //talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer); 
   stp = !stp;
   if(stp)
     digitalWrite(E_STEP_PIN, 1);
   else
     digitalWrite(E_STEP_PIN, 0);
}

inline  void extruder::enableStep()
{
  // Not needed - stepping the motor enables it automatically

}

inline  void extruder::disableStep()
{
  // N.B. Disabling the extrude stepper causes the backpressure to
  // turn the motor the wrong way.  Usually leave it on.
#if DISABLE_E  
  buildCommand(DISABLE);
  talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer);
#endif
}

inline int extruder::potVoltage()
{
   buildCommand(PREAD);
   char* reply = talker.sendPacketAndGetReply(my_name, commandBuffer);
   return(atoi(reply));  
}

inline void extruder::setPWM(int p)
{
   buildNumberCommand(SPWM, p);
   talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer);   
}

inline bool extruder::ping()
{
  buildCommand(PING);
  return talker.sendPacketAndCheckAcknowledgement(my_name, commandBuffer);  
}

#endif

extern extruder* ex[ ];
extern byte extruder_in_use;


#endif
