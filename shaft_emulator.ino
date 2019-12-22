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

// number of loops to wait for each rpm rate change
const unsigned int CHANGE_RATE = 20;

// rpm increment step
const unsigned int ACCELERATION_STEP = 2;

// rpm min and max
const float RPM_MIN = 500.0;
const float RPM_MAX = 1000.0;

// used pins
const int OUTPUT_PIN = 12;
const int LED_PIN = 13;

// instant rpm
volatile int rpm = RPM_MIN;

// whether in deceleration phase
volatile bool falling = false;

// current tooth transitioning
int currentTooth = -1;

int stepLoop = 0;

void setup() {
  
  Serial.begin(9600);

  pinMode(OUTPUT_PIN, OUTPUT);

  Serial.print("Degrees per tooth: ");
  Serial.println(DEGREES_PER_TOOTH);

}

void loop() {

  int position = rotateGearForward();
  long transitTime = computeTransitTime();

  digitalWrite(LED_PIN, position == 0 ? HIGH : LOW);
  
  if (position < TEETH_PRESENT) {  
    
    // emulate transit of a present tooth: produce a tooth followed by a gap
    
    digitalWrite(OUTPUT_PIN, HIGH);
    accurateDelay(transitTime);
    digitalWrite(OUTPUT_PIN, LOW);
    accurateDelay(transitTime);

  } else {
    
    // emulate transit of a missing tooth: produce twice the gap
    
    digitalWrite(OUTPUT_PIN, LOW);
    accurateDelay(transitTime * 2);

  }

  changeRpm();
}

/**
 * moves the gear one tooth forward or cycles back at the end of the turn
 */
int rotateGearForward() {
  if (currentTooth == TEETH_TOTAL - 1) {
    return currentTooth = 0;
  } else {
    return ++currentTooth;  
  }
}

/**
 * calculates a tooth or gap transit duration at the current rpm in microseconds
 */
long computeTransitTime() {

  float rps = rpm / 60.0;
  float revTime = 1000.0 * 1000.0 / rps;
  float timeDeg = revTime / 360.0;
  float transTime = timeDeg * DEGREES_PER_TOOTH;
  return transTime;

}

/**
 * halts execution for the given microseconds
 */
void accurateDelay(long intervalMicroseconds) {
  unsigned long start = micros();
  unsigned long milliseconds = intervalMicroseconds / 1000;
  if (milliseconds > 0) {
    delay(milliseconds);
  }
  unsigned long elapsed = micros() - start;
  if (elapsed < intervalMicroseconds) {
    delayMicroseconds(intervalMicroseconds - elapsed);
  }
}

/**
 * steps up or down the current rpm
 */
void changeRpm() {

  if (++stepLoop == CHANGE_RATE) {
    stepLoop = 0;
    if (falling) {
      
      if (rpm > RPM_MIN) rpm -= ACCELERATION_STEP;
      else falling = false;    
    
    } else {
    
      if (rpm < RPM_MAX) rpm += ACCELERATION_STEP;
      else falling = true;
      
    }
  }
  
}
