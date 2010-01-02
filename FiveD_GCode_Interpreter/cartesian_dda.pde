#include <stdio.h>
#include "parameters.h"
#include "pins.h"
#include "extruder.h"
#include "vectors.h"
#include "cartesian_dda.h"
#include <stdio.h>


// Initialise X, Y and Z.  The extruder is initialized
// separately.

cartesian_dda::cartesian_dda()
{
        live = false;
        nullmove = false;
        
  // Default is going forward
  
        x_direction = true;
        y_direction = true;
        z_direction = true;
        e_direction = true;
        f_direction = true;
        
  // Default to the origin and not going anywhere
  
	target_position.x = 0.0;
	target_position.y = 0.0;
	target_position.z = 0.0;
	target_position.e = 0.0;
        target_position.f = SLOW_XY_FEEDRATE;

  // Set up the pin directions
  
	pinMode(X_STEP_PIN, OUTPUT);
	pinMode(X_DIR_PIN, OUTPUT);

	pinMode(Y_STEP_PIN, OUTPUT);
	pinMode(Y_DIR_PIN, OUTPUT);

	pinMode(Z_STEP_PIN, OUTPUT);
	pinMode(Z_DIR_PIN, OUTPUT);

#ifdef USE_STEPPER_ENABLE
	pinMode(X_ENABLE_PIN, OUTPUT);
	pinMode(Y_ENABLE_PIN, OUTPUT);
	pinMode(Z_ENABLE_PIN, OUTPUT);
#endif

  //turn the motors off at the start.

	disable_steppers();

#if ENDSTOPS_MIN_ENABLED == 1
	pinMode(X_MIN_PIN, INPUT);
	pinMode(Y_MIN_PIN, INPUT);
	pinMode(Z_MIN_PIN, INPUT);
#endif

#if ENDSTOPS_MAX_ENABLED == 1
	pinMode(X_MAX_PIN, INPUT);
	pinMode(Y_MAX_PIN, INPUT);
	pinMode(Z_MAX_PIN, INPUT);
#endif
	
        // Default units are mm
        
        set_units(true);
}

// Switch between mm and inches

void cartesian_dda::set_units(bool using_mm)
{
    if(using_mm)
    {
      units.x = X_STEPS_PER_MM;
      units.y = Y_STEPS_PER_MM;
      units.z = Z_STEPS_PER_MM;
      units.e = E_STEPS_PER_MM;
      units.f = 1.0;
    } else
    {
      units.x = X_STEPS_PER_INCH;
      units.y = Y_STEPS_PER_INCH;
      units.z = Z_STEPS_PER_INCH;
      units.e = E_STEPS_PER_INCH;
      units.f = 1.0;
    }
}


void cartesian_dda::set_target(const FloatPoint& p)
{
        target_position = p;
        nullmove = false;
        
	//figure our deltas.

        delta_position = fabsv(target_position - where_i_am);
        
        // The feedrate values refer to distance in (X, Y, Z) space, so ignore e and f
        // values unless they're the only thing there.

        FloatPoint squares = delta_position*delta_position;
        distance = squares.x + squares.y + squares.z;
        // If we are 0, only thing changing is e
        if(distance < SMALL_DISTANCE2)
          distance = squares.e;
        // If we are still 0, only thing changing is f
        if(distance < SMALL_DISTANCE2)
          distance = squares.f;
        distance = sqrt(distance);          
                                                                                   			
	//set our steps current, target, and delta

        current_steps = to_steps(units, where_i_am);
	target_steps = to_steps(units, target_position);
	delta_steps = absv(target_steps - current_steps);

	// find the dominant axis.
        // NB we ignore the f values here, as it takes no time to take a step in time :-)
        //   hint: 'total_steps' variable here represents the number of interpolated points we are going to 
        //   calculate between current and target, must be bigger than the largest number of steps in any of the axises. ( ignore F here, we tweak that below)
        total_steps = max(delta_steps.x, delta_steps.y);
        total_steps = max(total_steps, delta_steps.z);
        total_steps = max(total_steps, delta_steps.e);
  
        // If we're not going anywhere, flag the fact
        
        if(total_steps == 0)
        {
          nullmove = true;
          where_i_am = p;
          return;
        }    

#ifndef ACCELERATION_ON
        current_steps.f = target_steps.f;
#endif

        delta_steps.f = abs(target_steps.f - current_steps.f);
        
        // Rescale the feedrate so it doesn't take lots of steps to do. Effectively,  we are applying a multiplication factor to the 'total_steps'
        // to scale the feedrate down into it.  we just do this because  otherwise the 'total_steps' calculated from above ( if we included 'f' )
        // would be HUGE, and  we'd have to do LOTS of interpolations for little value, and no change in anything except in F.
        
        t_scale = 1;
        if(delta_steps.f > total_steps)
        {
            t_scale = delta_steps.f/total_steps;
            if(t_scale >= 3)
            {
              target_steps.f = target_steps.f/t_scale;
              current_steps.f = current_steps.f/t_scale;
              delta_steps.f = abs(target_steps.f - current_steps.f);
              if(delta_steps.f > total_steps)
                total_steps =  delta_steps.f;
            } else
            {
              t_scale = 1;
              total_steps =  delta_steps.f;
            }
        } 
        	
	//what is our direction?

	x_direction = (target_position.x >= where_i_am.x);
	y_direction = (target_position.y >= where_i_am.y);
	z_direction = (target_position.z >= where_i_am.z);
	e_direction = (target_position.e >= where_i_am.e);
	f_direction = (target_position.f >= where_i_am.f);

	dda_counter.x = -total_steps/2;     // strating at a negative number 1/2 the total possible steps, means that for each dda_step...
	dda_counter.y = dda_counter.x;
	dda_counter.z = dda_counter.x;
        dda_counter.e = dda_counter.x;
        dda_counter.f = dda_counter.x;
  
        where_i_am = p;

        //sprintf(debugstring, "%d %d %d", (int)current_steps.e, (int)target_steps.e, (int)delta_steps.e);
        
        return;        
}



void cartesian_dda::dda_step()
{  
  if(!live)
   return;

  do
  {
		x_can_step = can_step(X_MIN_PIN, X_MAX_PIN, current_steps.x, target_steps.x, x_direction);
		y_can_step = can_step(Y_MIN_PIN, Y_MAX_PIN, current_steps.y, target_steps.y, y_direction);
		z_can_step = can_step(Z_MIN_PIN, Z_MAX_PIN, current_steps.z, target_steps.z, z_direction);
                e_can_step = can_step(-1, -1, current_steps.e, target_steps.e, e_direction);
                f_can_step = can_step(-1, -1, current_steps.f, target_steps.f, f_direction);
                
                real_move = false;
                
		if (x_can_step)
		{
            // ... adding the desired number of steps to the dda_counter.x means that we cause the dda_counter.x to go above zero sooner, and more often, 
            //  , if the delta_steps.x is a bigger number, so .....
			dda_counter.x += delta_steps.x; 
			
            // ... the more often the dda_counter.x overflows into a positive value, the more often this (x) axis will do a step, and start-over.
			if (dda_counter.x > 0)
			{
				do_x_step();
                                real_move = true;
				dda_counter.x -= total_steps;  // start counting again for this axis. from the starting point ( very near -totalsteps/2 )
				
				if (x_direction)
					current_steps.x++;  // remember the ACTUAL stepper steps, so we know where we are positioned. ( not the DDA "total_steps" ) 
				else
					current_steps.x--;
			}
		}

		if (y_can_step)
		{
			dda_counter.y += delta_steps.y;
			
			if (dda_counter.y > 0)
			{
				do_y_step();
                                real_move = true;
				dda_counter.y -= total_steps;

				if (y_direction)
					current_steps.y++;
				else
					current_steps.y--;
			}
		}
		
		if (z_can_step)
		{
			dda_counter.z += delta_steps.z;
			
			if (dda_counter.z > 0)
			{
				do_z_step();
                                real_move = true;
				dda_counter.z -= total_steps;
				
				if (z_direction)
					current_steps.z++;
				else
					current_steps.z--;
			}
		}

		if (e_can_step)
		{
			dda_counter.e += delta_steps.e;
			
			if (dda_counter.e > 0)
			{
				do_e_step(true); // causes actual step request
                                real_move = true;
				dda_counter.e -= total_steps;
				
				if (e_direction)
					current_steps.e++;
				else
					current_steps.e--;
			}
            do_e_step(false); // only for monitoring if extruder is caught-up on DC encoder extruders
		}
		
		if (f_can_step)
		{
			dda_counter.f += delta_steps.f;
			
			if (dda_counter.f > 0)
			{
				dda_counter.f -= total_steps;
				if (f_direction)
					current_steps.f++;
				else
					current_steps.f--;
			}
		}

				
      // wait for next step.
      // Use milli- or micro-seconds, as appropriate
      // If the only thing that changed was f keep looping
  
                if(real_move)
                {
                  //if(t_scale > 1)
                    timestep = t_scale*current_steps.f;
                  //else
                    //timestep = current_steps.f;
                  timestep = calculate_feedrate_delay((float) timestep);
                  setTimer(timestep);
                }
  } while (!real_move && f_can_step);

  live = (x_can_step || y_can_step || z_can_step  || e_can_step || f_can_step);


// Wrap up at the end of a line

  if(!live)
  {
      disable_steppers();
      setTimer(DEFAULT_TICK);
  }    
  
}

// Run the DDA

void cartesian_dda::dda_start()
{    
  // Set up the DDA
  //sprintf(debugstring, "%d %d", x_direction, nullmove);
  
  if(nullmove)
    return;

//set our direction pins as well
   
  byte d = 1;
  	
#if INVERT_X_DIR == 1
	if(x_direction)
            d = 0;
#else
	if(!x_direction)
            d = 0;	
#endif
        digitalWrite(X_DIR_PIN, d);
        
        d = 1;
    
#if INVERT_Y_DIR == 1
	if(y_direction)
            d = 0;
#else
	if(!y_direction)
            d = 0;	
#endif
        digitalWrite(Y_DIR_PIN, d);
        
        d = 1;
    
#if INVERT_Z_DIR == 1
	if(z_direction)
            d = 0;
#else
	if(!z_direction)
            d = 0;	
#endif
        digitalWrite(Z_DIR_PIN, d);


       //if(e_direction)
         ex[extruder_in_use]->set_direction(e_direction);
       //else
         //ex[extruder_in_use]->setDirection(false);
  
    //turn on steppers to start moving =)
    
	enable_steppers();
        
       // extcount = 0;

        setTimer(DEFAULT_TICK);
	live = true;
}


bool cartesian_dda::can_step(int min_pin, int max_pin, long current, long target, bool dir)
{

  //stop us if we're on target

	if (target == current){
		return false;
        } 

#if ENDSTOPS_MIN_ENABLED == 1

  //stop us if we're home and still going lower
  
	if(min_pin >= 0 && !dir)
        {
          if (read_switch(min_pin) )
		return false;
        }
#endif

#if ENDSTOPS_MAX_ENABLED == 1

  //stop us if we're at max and still going higher
  
	if(max_pin >= 0 && dir)
        {
 	    if (read_switch(max_pin))
 		return false;
        }
#endif

  // All OK - we can step
  
	return true;
}

void cartesian_dda::do_x_step()
{
int step_type = STEP_TYPE;  // override here for a specific axis, if you like! 
if (step_type == STEP_DIR){
	digitalWrite(X_STEP_PIN, HIGH);
	delayMicrosecondsInterruptible(5);
	digitalWrite(X_STEP_PIN, LOW);
}
if (step_type == GRAY_CODE){
#if INVERT_X_DIR == 1
          if ( x_direction ) { x_quadrature_state--; } else { x_quadrature_state++; }
#else 
          if ( x_direction ) { x_quadrature_state++; } else { x_quadrature_state--; }
#endif
          if ( x_quadrature_state > 3 ) { x_quadrature_state = 0; }
          if ( x_quadrature_state < 0 ) { x_quadrature_state = 3; } 
          int gray_code = x_quadrature_state ^ ( x_quadrature_state >> 1 ) ; //two significant bits in gray_code are the two pin states we want.
          int X1 = gray_code & 1; //lower order bit
          int X2 = gray_code & 2;  //higher order bit
          // write the quadrature/grey code to the two pins commonly referrred to as STEP and DIRECTION ( despite them not actually being that in this case) 
          digitalWrite(X_STEP_PIN, X1);
          digitalWrite(X_DIR_PIN,  X2);
          delayMicrosecondsInterruptible(5);
}
}

void cartesian_dda::do_y_step()
{
  int step_type = STEP_TYPE;  // override here for a specific axis, if you like! 
if (step_type == STEP_DIR){
	digitalWrite(Y_STEP_PIN, HIGH);
	delayMicrosecondsInterruptible(5);
	digitalWrite(Y_STEP_PIN, LOW);
}
if (step_type == GRAY_CODE){
#if INVERT_Y_DIR == 1
          if ( y_direction ) { y_quadrature_state++; } else { y_quadrature_state--; }
#else
           if ( y_direction ) { y_quadrature_state--; } else { y_quadrature_state++; }
#endif
          if ( y_quadrature_state > 3 ) { y_quadrature_state = 0; }  
          if ( y_quadrature_state < 0 ) { y_quadrature_state = 3; } 
          int gray_code = y_quadrature_state ^ ( y_quadrature_state >> 1 ) ; //two significant bits in gray_code are the two pin states we want.
          int Y1 = gray_code & 1; //lower order bit
          int Y2 = gray_code & 2;  //higher order bit
         // write the quadrature/grey code to the two pins commonly referrred to as STEP and DIRECTION ( despite them not actually being that in this case) 
          digitalWrite(Y_STEP_PIN, Y1);
          digitalWrite(Y_DIR_PIN,  Y2);
          delayMicrosecondsInterruptible(5);
}
}

void cartesian_dda::do_z_step()
{
  int step_type = UNMANAGED_DC; // manual override for a specific axis, in this case, my Z is DC driven, comment out to use global setting in parameters.h
  //int step_type = STEP_TYPE;  // override here for a specific axis, if you like! 
if (step_type == STEP_DIR){
	digitalWrite(Z_STEP_PIN, HIGH);
	delayMicrosecondsInterruptible(5);
	digitalWrite(Z_STEP_PIN, LOW);
}
if ( step_type == GRAY_CODE ){
#if INVERT_Z_DIR == 1
          if ( z_direction ) { z_quadrature_state++; } else { z_quadrature_state--; }
#else
           if ( z_direction ) { z_quadrature_state--; } else { z_quadrature_state++; }
#endif
          if ( z_quadrature_state > 3 ) { z_quadrature_state = 0; }  
          if ( z_quadrature_state < 0 ) { z_quadrature_state = 3; } 
          int gray_code = z_quadrature_state ^ ( z_quadrature_state >> 1 ) ; //two significant bits in gray_code are the two pin states we want.
          int Z1 = gray_code & 1; //lower order bit
          int Z2 = gray_code & 2;  //higher order bit
         // write the quadrature/grey code to the two pins commonly referrred to as STEP and DIRECTION ( despite them not actually being that in this case) 
          digitalWrite(Z_STEP_PIN, Z1);
          digitalWrite(Z_DIR_PIN,  Z2);
          delayMicrosecondsInterruptible(5);
}
if ( step_type == UNMANAGED_DC ){
          analogWrite(Z_STEP_PIN, 255); // this is really a PWM speed pin at MAX
// TODO use delayMicrosecondsInterruptible(20000)?   
        delay(20); // how long does the motor need to run to go one virtual step?
           analogWrite(Z_STEP_PIN, 0); // this is really a PWM speed pin at OFF
}
}

/*  TODO finish writing this as a greneric gray_code logic loop to replace the above 3x indentical implementations 
void gray_step(step_axis)  {
 gray_code.x

direction = 
#if INVERT_Z_DIR == 1
          if ( z_direction ) { z_quadrature_state++; } else { z_quadrature_state--; }
#else
           if ( z_direction ) { z_quadrature_state--; } else { z_quadrature_state++; }
#endif
          if ( z_quadrature_state > 3 ) { z_quadrature_state = 0; }  
          if ( z_quadrature_state < 0 ) { z_quadrature_state = 3; } 
          int gray_code = z_quadrature_state ^ ( z_quadrature_state >> 1 ) ; //two significant bits in gray_code are the two pin states we want.
          int Z1 = gray_code & 1; //lower order bit
          int Z2 = gray_code & 2;  //higher order bit
         // write the quadrature/grey code to the two pins commonly referrred to as STEP and DIRECTION ( despite them not actually being that in this case) 
          digitalWrite(Z_STEP_PIN, Z1);
          digitalWrite(Z_DIR_PIN,  Z2);
          delayMicrosecondsInterruptible(5);
}
}
*/

void cartesian_dda::do_e_step(bool actual)
{
  //the extruder code is kept in per-extruder settings, so different extruders may 
  // use different stepping/control in the future. see extruder.h::step() for similar code to the above
        ex[extruder_in_use]->step(actual);
}

void cartesian_dda::enable_steppers()
{
#ifdef USE_STEPPER_ENABLE
 if(delta_steps.x)
    digitalWrite(X_ENABLE_PIN, ENABLE_ON);
  if(delta_steps.y)    
    digitalWrite(Y_ENABLE_PIN, ENABLE_ON);
  if(delta_steps.z)
    digitalWrite(Z_ENABLE_PIN, ENABLE_ON);
  if(delta_steps.e)
    ex[extruder_in_use]->enableStep();
#endif  
}



void cartesian_dda::disable_steppers()
{
#ifdef USE_STEPPER_ENABLE
	//disable our steppers
#if DISABLE_X
	digitalWrite(X_ENABLE_PIN, !ENABLE_ON);
#endif
#if DISABLE_Y
	digitalWrite(Y_ENABLE_PIN, !ENABLE_ON);
#endif
#if DISABLE_Z
        digitalWrite(Z_ENABLE_PIN, !ENABLE_ON);
#endif

        // Disabling the extrude stepper causes the backpressure to
        // turn the motor the wrong way.  Leave it on.
        
        //ex[extruder_in_use]->->disableStep();       
#endif
}


