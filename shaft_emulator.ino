#include <TimerOne.h>

const unsigned int TEETH_PRESENT = 11;
const unsigned int TEETH_MISSING = 1;
const unsigned int TEETH_TOTAL = TEETH_PRESENT + TEETH_MISSING;

const unsigned long ACCELERATION_RATE = 1000L * 100L;  // microseconds between each rpm step
const unsigned int ACCELERATION_STEP = 1;              // rpm increment step

const float RPM_MIN = 15.0;
const float RPM_MAX = 15.0;

const int OUTPUT_PIN = 13;

volatile int rpm = RPM_MIN;
volatile bool falling = false;

int currentTooth = -1;

void setup() {
  Serial.begin(9600);

  Timer1.initialize(ACCELERATION_RATE); // time in us
  Timer1.attachInterrupt(changeRPMs);

  pinMode(OUTPUT_PIN, OUTPUT);
}

void loop() {

  int position = rotateGearForward();
  int transitionTimeMs = computeTransitionTimeMs();
  
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
 * calculates how long a tooth or a gap transit lasts in time at the current rpms
 */
float computeTransitionTimeMs() {
  float rps = rpm / 60.0;
  float revTimeMs = 1000.0 / rps;
  float timeDegMs = revTimeMs / 360.0;
  float toothDeg = 360 / (TEETH_TOTAL * 2);
  float transTimeMs = timeDegMs * toothDeg;
/*
  Serial.print("rpm: ");
  Serial.println(rpm);
  Serial.print("rps: ");
  Serial.println(rps);
  Serial.print("revTimeMs: ");
  Serial.println(revTimeMs);
  Serial.print("timeDegMs: ");
  Serial.println(timeDegMs);
  Serial.print("toothDeg: ");
  Serial.println(toothDeg);
  Serial.print("transitionMs: ");
  Serial.println(transTimeMs);
*/
  return transTimeMs;
}

/**
 * steps up or down the current rpms
 */
void changeRPMs() {
  if (falling) {
    if (rpm > RPM_MIN) rpm -= ACCELERATION_STEP;
    else falling = false;    
  } else {
    if (rpm < RPM_MAX) rpm += ACCELERATION_STEP;
    else falling = true;
  }
}
