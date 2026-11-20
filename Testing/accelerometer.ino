#include <Wire.h>
#include <math.h>

#define MPU_ADDR 0x68

// Calibration
const int CALIBRATION_SAMPLES = 1000;

float gyroBiasX = 0;
float gyroBiasY = 0;
float gyroBiasZ = 0;

// Movement sensitivity
const float ACC_THRESHOLD = 0.08;  // g
const float GYRO_THRESHOLD = 2.0;  // degrees/second

void setup() {
  Serial.begin(9600);
  delay(1000);

  Wire.begin();

  Serial.println("\n=== MPU6050 Movement Detector ===");

  // Wake up sensor
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

  calibrateGyroscope();

  Serial.println("\nCalibration complete.");
  Serial.println("Movement detection started.\n");
}

void calibrateGyroscope() {
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

  Serial.println("\nGyroscope offsets:");

  Serial.print("X: ");
  Serial.println(gyroBiasX, 3);

  Serial.print("Y: ");
  Serial.println(gyroBiasY, 3);

  Serial.print("Z: ");
  Serial.println(gyroBiasZ, 3);
}

void loop() {

  // Read accelerometer and gyroscope
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  if (Wire.requestFrom(MPU_ADDR, 14, true) != 14) {
    Serial.println("Sensor read error!");
    delay(50);
    return;
  }

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

  Serial.print(" | STATUS: ");

  if (moving) {
    Serial.println("MOVING");
  } else {
    Serial.println("STATIONARY");
  }

  delay(50);
}