/*
 * This class controls the movement of the RepRap machine.
 * It implements a DDA in four dimensions, so the length of extruded 
 * filament is treated as a variable, just like X, Y, and Z.
 *
 * Adrian Bowyer 9 May 2009
 */

#ifndef CARTESIAN_DDA_H
#define CARTESIAN_DDA_H

// Main class for moving the RepRap machine about

class cartesian_dda
{
private:

  //extruder* ext;               // The extruder I'm currently using - keep this up to date...

  FloatPoint units;            // Factors for converting either mm or inches to steps

  // FloatPoint where_i_am; -- is defined as a singularity in the main application, as we only want to be in a single actual position at a time, ever. this isnt really a time-and-space-machine :-)
  FloatPoint target_position;  // Where it's going
  FloatPoint delta_position;   // The difference between the two
  float distance;              // How long the path is
  
  LongPoint current_steps;     // Similar information as above in steps rather than units -- where we are now
  LongPoint target_steps;      // -- we we are targeting
  LongPoint delta_steps;       // -- difference of the above two.
  LongPoint dda_counter;       // DDA error-accumulation variables
  long t_scale;                // When doing lots of t steps, scale them so the DDA doesn't spend for ever on them
  
  byte x_direction;            // Am I going in the + or - direction?
  byte y_direction;
  byte z_direction;
  byte e_direction;
  byte f_direction;

  bool x_can_step;             // Am I not at an endstop?  Have I not reached the target? etc.
  bool y_can_step;
  bool z_can_step;
  bool e_can_step;
  bool f_can_step;

// Variables for acceleration calculations

  long total_steps;            // The number of calculations to make betwene the source and the destination.  
                                // holds no direct relation to current_steps, target_steps, or delta_steps other than htat they provide  convenient numbers 
                                //to determine a useful value for this, as it must be as large or larger than the maximum number of steps on the domininant axis.
  
  long timestep;               // microseconds
  bool nullmove;               // this move is zero length
  bool real_move;              // Flag to know if we've changed something physical
  volatile bool live;                   // Flag for when we're plotting a line

// Internal functions that need not concern the user

  // Take a single step

  void do_x_step();               
  void do_y_step();
  void do_z_step();
  void do_e_step(bool actual); //extruder supports DC/encoder, with virtual/actual steps
  
  // Can this axis step?
  
  bool can_step(byte min_pin, byte max_pin, long current, long target, byte dir);
  
  // Read a limit switch
  
  bool read_switch(byte pin);
  
  // Work out the number of microseconds between steps
  
  long calculate_feedrate_delay(const float& feedrate);
  
  // Switch the steppers on and off
  
  void enable_steppers();
  void disable_steppers();
  
  // Custom short delay function (microseconds)
  
  //void delayMicrosecondsInterruptible(unsigned int us);
  
  
public:

  cartesian_dda();
  
  // Set where I'm going
  
  void set_target(const FloatPoint& p);
  
  // Start the DDA
  
  void dda_start();
  
  // Do one step of the DDA
  
  void dda_step();
  
  // Are we running at the moment?
  
  bool active();
  
  // True for mm; false for inches
  
  void set_units(bool using_mm);
  
  // Record the selection of a new extruder
  
  //void set_extruder(extruder* ex);
};

// Short functions inline to save memory; particularly useful in the Arduino


//inline void cartesian_dda::set_extruder(extruder* ex)
//{
//  ext = ex;
//}

inline bool cartesian_dda::active()
{
  return live;
}
/* 
inline void cartesian_dda::do_x_step()
{
	digitalWrite(X_STEP_PIN, HIGH);
	delayMicroseconds(5);
	digitalWrite(X_STEP_PIN, LOW);
}

inline void cartesian_dda::do_y_step()
{
	digitalWrite(Y_STEP_PIN, HIGH);
	delayMicroseconds(5);
	digitalWrite(Y_STEP_PIN, LOW);
}

inline void cartesian_dda::do_z_step()
{
	digitalWrite(Z_STEP_PIN, HIGH);
	delayMicroseconds(5);
	digitalWrite(Z_STEP_PIN, LOW);
}

inline void cartesian_dda::do_e_step(bool actual)
{
        ex[extruder_in_use]->step(actual);
}
*/

inline long cartesian_dda::calculate_feedrate_delay(const float& feedrate)
{  
        
	// Calculate delay between steps in microseconds.  Here it is in English:
        // (feedrate is in mm/minute, distance is in mm)
	// 60000000.0*distance/feedrate  = move duration in microseconds
	// move duration/total_steps = time between steps for master axis.

	return round( (distance*60000000.0) / (feedrate*(float)total_steps) );	
}

inline bool cartesian_dda::read_switch(byte pin)
{
/* 
#ifdef INTERRUPT_ENDSTOPS
        #if ENDSTOPS_INVERTING == 1
                if ( pin == X_MIN_PIN ) return !optoAstate;
                if ( pin == Y_MIN_PIN ) return !optoBstate;
                if ( pin == Z_MIN_PIN ) return !digitalRead(Z_MIN_PIN); //oops, z is not on an interrupt 
        #else
                if ( pin == X_MIN_PIN ) return optoAstate;
                if ( pin == Y_MIN_PIN ) return optoBstate;
                if ( pin == Z_MIN_PIN ) return digitalRead(Z_MIN_PIN);
        #endif
#else 
	//dual read as crude debounce

	#if ENDSTOPS_INVERTING == 1
		return !digitalRead(pin) && !digitalRead(pin);
	#else
		return digitalRead(pin) && digitalRead(pin);
	#endif
#endif
*/
}

/*NOTE: EMC type 2 stepper driver is what we will achieve here :
  Type 2:  Quadrature (aka Gray/Grey? code)
    State   Phase A   Phase B
      0        1        0
      1        1        1
      2        0        1
      3        0        0
      0        1        0
  Here's the simplest algorithm for translating binary to Gray code. This algorithm can convert an arbitrary binary number to Gray code in finite time. Wonderful! He
  grayCode = binary ^ (binary >> 1)
*/
#if STEP_TYPE == GRAY_CODE

int x_quadrature_state = 0; // allowable values are: 0,1,2,3
int y_quadrature_state = 0; // allowable values are: 0,1,2,3
int z_quadrature_state = 0;
int e_quadrature_state = 0;

// we also use x_direction and y_direction to decide the direction we roll through each quadrature
#endif

#endif
