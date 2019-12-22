/**
 * This program will produce hall sensor signals emulating the behaviour of a gear installed on a shaft, as found on thermic engines.
 * The transit of a tooth in front of the hall sensor is represented by a high value on the OUTPUT_PIN, a low is the absence of a tooth.
 * 
 * The gear teeth / gaps layout is specified by the values of TEETH_PRESENT and TEETH_MISSING, assumption is that the teeth are contiguous, followed by the missing teeth.
 * 
 * As an example, these parameters:
 * 
 *  TEETH_PRESENT = 5
 *  TEETH_MISSING = 2 
 *  
 * will produce this layout: 
 * 
 * [ H L H L H L H L H L L L L L ]
 * [ -1- -2- -3- -4- -5- --- --- ]
 * 
 * The program can produce a fluctuating rpm by specifying RPM_MIN, RPM_MAX, CHANGE_RATE and ACCELERATION_STEP accordingly
 * 
 * Author: dario.x.busco@gmail.com
 */
#include <TimerOne.h>

// number of teeth on wheel
const unsigned int TEETH_PRESENT = 35;

// number of missing teeth on wheel, contiguous
const unsigned int TEETH_MISSING = 1;

// total number of teeth and gaps
const unsigned int TEETH_TOTAL = TEETH_PRESENT + TEETH_MISSING;

// angular degrees per tooth position
const float DEGREES_PER_TOOTH = 360 / (TEETH_TOTAL * 2);

// number of revolutions for each rpm rate change
const unsigned int CHANGE_RATE = 5;

// rpm increment step
const unsigned int ACCELERATION_STEP = 2;

// rpm min and max
const float RPM_MIN = 60.0;
const float RPM_MAX = 60.0;

// constants used in calculations
const unsigned long ONE_MINUTE_IN_MICROSECONDS = 60000000;

const float TRANSITION_CONSTANT = ONE_MINUTE_IN_MICROSECONDS * DEGREES_PER_TOOTH / 360.0;

// used pins
const int OUTPUT_PIN = 12;
const int LED_PIN = 13;

// instant rpm
volatile int rpm = RPM_MIN;

// whether in deceleration phase
volatile bool falling = false;

// current tooth transitioning
int currentTooth = -1;

// loop counter, used to alter the rpm periodically
int currentLoopIndex = 0;

unsigned long transitTime;

void setup() {
  
  Serial.begin(9600);

  pinMode(OUTPUT_PIN, OUTPUT);

  Serial.print("Degrees per tooth: ");
  Serial.println(DEGREES_PER_TOOTH);

  // initial transit time based on starting rpm
  transitTime = computeTransitTime();
}

void loop() {

  unsigned long start = micros();

  // allow rpm to change if needed
  if (changeRpm()) {
    transitTime = computeTransitTime();
  }

  unsigned long precalculationElapsed = micros() - start;

  unsigned long expectedRevolutionDuration = transitTime * 2 * TEETH_TOTAL;

  for (int position = 0; position < TEETH_TOTAL - 1; position++) {

    // flash led to give visual indication of loop
    digitalWrite(LED_PIN, position == 0 ? HIGH : LOW);

    // emulate transit of a present tooth: produce a tooth followed by a gap
    digitalWrite(OUTPUT_PIN, HIGH);
    accurateDelay(transitTime);
    digitalWrite(OUTPUT_PIN, LOW);
    accurateDelay(transitTime);
    
  }

  // we are past all teeth, now emulate the transit of the missing teeth: produce twice the gap
  digitalWrite(OUTPUT_PIN, LOW);

  // before delaying for the low gap, calculate any error and compensate
  unsigned long elapsedBeforeEndGap = micros() - start;
  accurateDelay(expectedRevolutionDuration - elapsedBeforeEndGap);

/*
  unsigned long overallElapsed = micros() - start;
  Serial.print(" precalc: ");
  Serial.print(precalculationElapsed);
  Serial.print(" expected: ");
  Serial.print(expectedRevolutionDuration);
  Serial.print(" final: ");
  Serial.print(overallElapsed);
  Serial.print("\n");
*/

}

/**
 * calculates a tooth or gap transit duration at the current rpm in microseconds
 */
float computeTransitTime() {

  // duration of the transit of a single tooth (present or missing or gap in between) in microseconds
  return TRANSITION_CONSTANT / rpm;

}

/**
 * halts execution for the given microseconds
 */
void accurateDelay(long intervalMicroseconds) {

  unsigned long desiredEnd = micros() + intervalMicroseconds;

  // keep retarding at the smallest microseconds resolution
  while (micros() < desiredEnd) {
    delayMicroseconds(4);
  }
  
}

/**
 * steps up or down the current rpm if needed
 * returns true if rpm actually changed
 */
boolean changeRpm() {

  if (++currentLoopIndex == CHANGE_RATE) {
    
    // this iteration produces a change, either in rpm or direction
    currentLoopIndex = 0;

    if (falling) {
      
      if (rpm > RPM_MIN) {
        rpm -= ACCELERATION_STEP;
        return true;
      } else {
        falling = false;    
        return false;
      }
    
    } else {
    
      if (rpm < RPM_MAX) {
        rpm += ACCELERATION_STEP;
        return true;
      } else {
        falling = true;
        return false;
      }
      
    }
    
  } else {

    // this iteration did not produce a change of rpm
    return false;
    
  }
  
}
