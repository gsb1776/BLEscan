#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include "BLEscan.h"

// LCD definition
LiquidCrystal_I2C lcd(0x27, 20, 4);  

// Define input pins
#define ROT_A D3     // encoder A (moved from D3, which is now the red LED)
#define ROT_B A3    // encoder B
#define BUTTON D2     // color button (wired to GND, uses INPUT_PULLUP)
#define LED_RED D4
#define LED_GREEN D5
#define LED_BLUE D6
#define PIEZO D7

// NEEDS MODIFICATION later to match the specs of scanner
const int MAX_VALUE = 100;   // list goes 0..10, then wraps around
const int COUNTS_PER_STEP = 4;    // raw encoder pulses per click

volatile int rotaryCounter = 0;
volatile bool rotaryChanged = false;
volatile bool buttonPending = false; // if button has been dealt with yet
volatile unsigned long buttonEdgeTime = 0;
uint8_t encoderState = 0;

int  selected = 0;       // number shown at the top of the list
bool buttonDown = false;   // true while the button is held
int  colorMode = 0;       // 0 = red, 1 = green, 2 = blue

// Accelerometer definitions
#define MPU_ADDR 0x68
const int CALIBRATION_SAMPLES = 1000;
float gyroBiasX = 0;
float gyroBiasY = 0;
float gyroBiasZ = 0;

// Movement sensitivity
const float ACC_THRESHOLD = 0.08;  // g
const float GYRO_THRESHOLD = 2.0;  // degrees/second

bool mpuAlive(const char* label) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);
  uint8_t err = Wire.endTransmission(false);
  uint8_t got = Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)1);
  uint8_t id = got ? Wire.read() : 0;
  Serial.print(label);
  Serial.print(": err=");  Serial.print(err);
  Serial.print(" got=");   Serial.print(got);
  Serial.print(" id=0x");  Serial.println(id, HEX);
  return err == 0 && got == 1;
}

void initPins() {
  pinMode(ROT_A, INPUT_PULLUP);
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  // Define piezo speaker
  pinMode(PIEZO, OUTPUT);

  updateLed(); // start on red

  encoderState = (digitalRead(ROT_A) << 1) | digitalRead(ROT_B);

  attachInterrupt(digitalPinToInterrupt(ROT_A), rotaryISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_B), rotaryISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, CHANGE);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void rotaryISR()
{
  static const int8_t encoderTable[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  uint8_t newState = (digitalRead(ROT_A) << 1) | digitalRead(ROT_B);
  int8_t move = encoderTable[(encoderState << 2) | newState];

  if (move != 0) {
    rotaryCounter += move;
    rotaryChanged = true;
  }

  encoderState = newState;
}

void buttonISR()
{
  buttonEdgeTime = millis();
  buttonPending  = true;
}

// Update variables for readings of button/encoder seen in ISR
void getReadings()
{
  noInterrupts();
  bool changed = rotaryChanged;
  int  counts  = rotaryCounter;
  rotaryChanged = false;
  interrupts();

  // Update local encoder value
  if (changed) {
    int range = MAX_VALUE + 1;
    int steps = counts / COUNTS_PER_STEP;
    selected  = ((steps % range) + range) % range;   // wrap in both directions
  }

  // if button pressed, change color
  // No interrupts due to changes messing i t up
  noInterrupts();
  bool pending = buttonPending;
  unsigned long edgeTime = buttonEdgeTime;
  interrupts();

  // if the button has changed + 10ms have passed since most recent change
  if (pending && (millis() - edgeTime >= 10)) {
    noInterrupts();
    buttonPending = false;
    interrupts();

    bool pressedNow = (digitalRead(BUTTON) == LOW); 

    if (pressedNow && !buttonDown) {
      buttonDown = true;
      colorMode = (colorMode + 1) % 3;
      updateLed();
      Serial.print("Color mode: ");
      Serial.println(colorMode);
    }
    else if (!pressedNow) {
      buttonDown = false;
    }
  }
}

void updateLed()
{
  analogWrite(LED_RED,   colorMode == 0 ? 255 : 0);
  analogWrite(LED_GREEN, colorMode == 1 ? 255 : 0);
  analogWrite(LED_BLUE,  colorMode == 2 ? 255 : 0);
}

// Write to LCD
void updateDisplay(const char* text)
{
  const char* ptr = text;

  for (int row = 0; row < 4; row++) {
    char line[21];
    int col = 0;

    // Copy characters for this row until '\n', end of string, or 20 columns
    while (*ptr != '\0' && *ptr != '\n' && col < 20) {
      line[col++] = *ptr++;
    }

    // Pad remaining columns with spaces to erase leftover characters
    while (col < 20) {
      line[col++] = ' ';
    }
    line[20] = '\0'; // Null-terminate the string

    // Print the 20-character row to the LCD
    lcd.setCursor(0, row);
    lcd.print(line);

    // If the line was longer than 20 chars, skip past the rest until '\n'
    while (*ptr != '\0' && *ptr != '\n') {
      ptr++;
    }

    // Advance past the '\n' for the next row
    if (*ptr == '\n') {
      ptr++;
    }
  }
}

// Initial calibration of our gyroscope biases
void calibrateGyroscope() {
  // Initial gyroscope setup and start it

  // Wake up sensor, beginning transmission and checking that it works
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(100);

  // Check connection
  Wire.beginTransmission(MPU_ADDR);
  if (Wire.endTransmission() != 0) {
    Serial.println("ERROR: MPU6050 not detected!");
    while (1);
  }

  Serial.println("MPU6050 connected.");

  long sumX = 0;
  long sumY = 0;
  long sumZ = 0;

  Serial.println("Keep sensor completely stationary...");
  Serial.println("Calibrating gyroscope.");

  int validSamples = 0;

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);

    if (Wire.requestFrom(MPU_ADDR, 6, true) == 6) {

      int16_t rawX = (int16_t)(Wire.read() << 8 | Wire.read());
      int16_t rawY = (int16_t)(Wire.read() << 8 | Wire.read());
      int16_t rawZ = (int16_t)(Wire.read() << 8 | Wire.read());

      sumX += rawX;
      sumY += rawY;
      sumZ += rawZ;

      validSamples++;
    }

    delay(3);
  }

  if (validSamples == 0) {
    Serial.println("Calibration failed!");
    while (1);
  }

  gyroBiasX = sumX / (float)validSamples / 131.0;
  gyroBiasY = sumY / (float)validSamples / 131.0;
  gyroBiasZ = sumZ / (float)validSamples / 131.0;
}

bool getMoveReading() {
  // Read accelerometer and gyroscope
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  uint8_t err = Wire.endTransmission(false);

  uint8_t got = Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)14, (uint8_t)true);
  if (err != 0 || got != 14) {
    Serial.print("I2C err=");
    Serial.print(err);
    Serial.print(" got=");
    Serial.println(got);
    return false;
  }

  // if (Wire.requestFrom(MPU_ADDR, 14, true) != 14) {
  //   Serial.println("Sensor read error!");
  //   delay(50);
  //   return false;
  // }

  int16_t rawAX = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t rawAY = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t rawAZ = (int16_t)(Wire.read() << 8 | Wire.read());

  // Skip temperature
  Wire.read();
  Wire.read();

  int16_t rawGX = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t rawGY = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t rawGZ = (int16_t)(Wire.read() << 8 | Wire.read());

  // Convert acceleration to g
  float ax = rawAX / 16384.0;
  float ay = rawAY / 16384.0;
  float az = rawAZ / 16384.0;

  // Convert gyro to degrees/second
  float gx = rawGX / 131.0 - gyroBiasX;
  float gy = rawGY / 131.0 - gyroBiasY;
  float gz = rawGZ / 131.0 - gyroBiasZ;

  // Calculate acceleration magnitude
  float accMagnitude = sqrt(
    ax * ax + ay * ay + az * az
  );

  // Estimate acceleration excluding gravity
  float linearAcceleration = fabs(accMagnitude - 1.0);

  // Detect acceleration movement
  bool accelerationMovement =
    linearAcceleration > ACC_THRESHOLD;

  // Detect rotational movement
  bool rotationMovement =
    fabs(gx) > GYRO_THRESHOLD ||
    fabs(gy) > GYRO_THRESHOLD ||
    fabs(gz) > GYRO_THRESHOLD;

  // Overall movement detection
  bool moving = accelerationMovement || rotationMovement;

  // Output
  Serial.print("ACC magnitude: ");
  Serial.print(accMagnitude, 3);

  Serial.print(" | Linear ACC: ");
  Serial.print(linearAcceleration, 3);

  Serial.print(" g | GYRO: ");

  Serial.print(gx, 2);
  Serial.print(", ");
  Serial.print(gy, 2);
  Serial.print(", ");
  Serial.print(gz, 2);
  Serial.println();

  return moving;
}

// Update our list based on scan results
void updateDeviceList();

// Update our current target based on rotary rotation or button toggle
void updateTarget();

// Scan for target on either BLE or wifi
void scanForTarget();

// Smooth found RSSI vals
void uppdateRssi();

// Hit piezo buzzer with frequency for RSSI dist
void updateBuzzer(unsigned int frequency) {
  if (frequency > 0) {
    tone(PIEZO, frequency);   // no duration given, so it plays until changed or stopped
  } else {
    noTone(PIEZO);
  }
}

// Flip the mode
void nextMode();