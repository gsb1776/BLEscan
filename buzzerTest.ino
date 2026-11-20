#include "BLEscan.h"
#include <Wire.h>

const unsigned int FREQ_BASE   = 200;   // Hz at selected = 0
const unsigned int FREQ_STEP   = 40;    // Hz per encoder click (0..100 -> 200..4200 Hz)
const unsigned long MOVE_POLL_MS = 50;  // how often to read the MPU6050
const unsigned long SETTLE_MS    = 500; // must be still this long before resuming

// void setup()
// {
//   Serial.begin(9600);
//   while (!Serial)
//     ;

//   loop();
//   //initializeSystem();
//   //  testAll();
//   //  testTurns();
//   //  while(true);
// }

void buzzerTask() {
  static bool initialized = false;
  static bool paused = false;
  static bool needRedraw = true;
  static unsigned int freq = FREQ_BASE;
  static int lastSelected = -1;
  static int lastColorMode = -1;
  static unsigned long lastMoveRead = 0, lastMotion = 0;

  unsigned long now = millis();

  getReadings();   // encoder + button

  if (selected != lastSelected) {
    lastSelected = selected;
    freq = FREQ_BASE + (unsigned int)selected * FREQ_STEP;
    if (!paused) updateBuzzer(freq);
    needRedraw = true;
  }

  if (colorMode != lastColorMode) {
    lastColorMode = colorMode;
    updateLed();
  }

  if (now - lastMoveRead >= MOVE_POLL_MS) {
    lastMoveRead = now;
    if (getMoveReading()) {
      lastMotion = now;
      if (!paused) {
        paused = true;
        updateBuzzer(0);
        needRedraw = true;
      }
    } else if (paused && now - lastMotion >= SETTLE_MS) {
      paused = false;
      updateBuzzer(freq);
      needRedraw = true;
    }
  }

  if (needRedraw) {
    needRedraw = false;
    char buf[48];
    if (paused) snprintf(buf, sizeof(buf), "PAUSED");
    else        snprintf(buf, sizeof(buf), "Buzzer freq:\n%u Hz", freq);
    updateDisplay(buf);
  }

  if (!initialized) {
    initialized = true;
    updateBuzzer(freq);
  }
}

// void setup() {
//   Serial.begin(9600);
//   delay(1000);                 // lets the Serial Monitor catch up

//   Wire.begin();
//   Wire.setClock(100000);       // 100 kHz is the safest speed for mixed devices

//   initPins();                  // pins, interrupts, then lcd.init()
//   calibrateGyroscope();        // keep the board still for ~3 seconds

//   Serial.println("Setup done.");
// }

void loop() {
  buzzerTask();
}
