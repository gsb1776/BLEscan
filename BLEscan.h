#define TESTING
// #include <BLEDevice.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>

extern volatile int rotaryCounter;
extern volatile bool rotaryChanged;
extern volatile bool buttonPending;
extern volatile unsigned long buttonEdgeTime;
extern uint8_t encoderState;

extern int selected;
extern bool buttonDown;
extern int colorMode;

// typedef enum
// {
//     s_IDLE = 1,
//     s_SCAN = 2,
//     s_TRACK = 3,
//     s_SIGNAL_LOST = 4,
//     s_BROWSE = 5,
//     s_UPDATE_MODE = 6,
// } fsm_state;

// typedef struct
// {
//     uint8_t target;
//     // BLEAdvertisedDevice deviceList;
//     float smoothedRssi;
//     byte savedClock;
//     unsigned long mode;
// } full_state;

/*
 * Utilities
 */
// Set up pins for LED, encoder, button, turn on screen and start I2C
void initPins();
// Handle rotary interrupt
void rotaryISR();
// Handle button interrupt
void buttonISR();
// Read button/encoder and handle turning/local var updates
void getReadings();
// Push to LED diode using prewritten colors
void updateLed();
// Initial gyroscope calibration to be called on init
void calibrateGyroscope();
// Push to display with text chars
void updateDisplay(const char* text);
// Get a move reading as a T for moving or F for still
// Uses I2C
bool getMoveReading();
// Update our list based on scan results
void updateDeviceList();
// Update our current target based on rotary rotation or button toggle
void updateTarget();
// Scan for target on either BLE or wifi
void scanForTarget();
// Smooth found RSSI vals
void uppdateRssi();
// Hit piezo buzzer with frequency for RSSI dist
void updateBuzzer();
// Flip the mode
void nextMode();