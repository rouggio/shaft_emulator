#include <TimerOne.h>

// number of teeth on wheel
const unsigned int TEETH_PRESENT = 35;

// number of missing teeth on wheel, contiguous
const unsigned int TEETH_MISSING = 1;

// total number of teeth and gaps
const unsigned int TEETH_TOTAL = TEETH_PRESENT + TEETH_MISSING;

// angular degrees per tooth position
const float DEGREES_PER_TOOTH = 360 / (TEETH_TOTAL * 2);

// microseconds between each rpm step
const unsigned long ACCELERATION_RATE = 1000L * 50L;

// rpm increment step
const unsigned int ACCELERATION_STEP = 1;

// rpm min and max
const float RPM_MIN = 200.0;
const float RPM_MAX = 500.0;

// used pins
const int OUTPUT_PIN = 12;
const int LED_PIN = 13;

// instant rpm
volatile int rpm = RPM_MIN;

// whether in deceleration phase
volatile bool falling = false;

// current tooth transitioning
int currentTooth = -1;

void setup() {
  Serial.begin(9600);

  Timer1.initialize(ACCELERATION_RATE); // time in us
  Timer1.attachInterrupt(changeRpm);

  pinMode(OUTPUT_PIN, OUTPUT);
}

void loop() {

  int position = rotateGearForward();
  int transitionTimeMs = computeTransitionTimeMs();

  digitalWrite(LED_PIN, position == 0 ? HIGH : LOW);
  
  if (position < TEETH_PRESENT) {  
    
    // emulate transit of a present tooth: produce a tooth followed by a gap
    
    digitalWrite(OUTPUT_PIN, HIGH);
    delay(transitionTimeMs);
    digitalWrite(OUTPUT_PIN, LOW);
    delay(transitionTimeMs);

  } else {
    
    // emulate transit of a missing tooth: produce twice the gap
    
    digitalWrite(OUTPUT_PIN, LOW);
    delay(transitionTimeMs * 2);

  }

}

/**
 * moves the gear one tooth forward or cycles back at the end of the turn
 */
int rotateGearForward() {
  if (currentTooth == TEETH_TOTAL-1) {
    return currentTooth = 0;
  } else {
    return ++currentTooth;  
  }
}

/**
 * calculates how long a tooth or a gap transit lasts in time at the current rpm
 */
float computeTransitionTimeMs() {

  float rps = rpm / 60.0;
  float revTimeMs = 1000.0 / rps;
  float timeDegMs = revTimeMs / 360.0;
  float transTimeMs = timeDegMs * DEGREES_PER_TOOTH;
  return transTimeMs;

}

/**
 * steps up or down the current rpm
 */
void changeRpm() {

  if (falling) {
    
    if (rpm > RPM_MIN) rpm -= ACCELERATION_STEP;
    else falling = false;    
  
  } else {
  
    if (rpm < RPM_MAX) rpm += ACCELERATION_STEP;
    else falling = true;
    
  }
  
}
