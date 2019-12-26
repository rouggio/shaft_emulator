const float RPM = 150;
const float TEETH = 36;

const float PI2 = 2.0 * PI;
const float MICROS_PER_MINUTES = 60000000;
const float RAD_PER_MICROS = RPM*PI2/MICROS_PER_MINUTES;
const float TEETH2 = TEETH*2;
const float TEETH_RAD = PI2/TEETH2;
const float TEETH_SPEED_MICROS = TEETH_RAD / RAD_PER_MICROS;

// used pins
const int OUTPUT_PIN = 12;
const int LED_PIN = 13;

unsigned long previousTimeMicros;
float currentRad = 0;

void setup() {
  previousTimeMicros = micros();
}

void loop() {
  const unsigned long nowMicros = micros();
  const unsigned long deltaTmicro = nowMicros - previousTimeMicros;
  previousTimeMicros = nowMicros;

  currentRad += (TEETH_SPEED_MICROS * deltaTmicro);
  if(currentRad > PI2) {
    currentRad -= PI2;
  }
  const unsigned int tooth = (currentRad/PI2) * TEETH2;
  const auto pinHighLow = tooth == TEETH2 - 2 || tooth % 2 != 0 ? LOW : HIGH;
  digitalWrite(OUTPUT_PIN, pinHighLow);
  digitalWrite(LED_PIN, pinHighLow);
}
