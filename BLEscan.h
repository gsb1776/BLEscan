#define TESTING
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

typedef enum
{
    s_IDLE = 1,
    s_SCAN = 2,
    s_TRACK = 3,
    s_SIGNAL_LOST = 4,
    s_BROWSE = 5,
    s_UPDATE_MODE = 6,
} fsm_state;

typedef struct
{
    uint8_t target;
    BLEAdvertisedDevice deviceList;
    float smoothedRssi;
    byte savedClock;
    unsigned long mode;
} full_state;

/*
 * Utilities
 */
void updateDeviceList();
void updateTarget();
void scanForTarget();
void uppdateRssi();
void updateBuzzer();
void updateDisplay();
void nextMode();
void updateLed();
void getMoveReading();