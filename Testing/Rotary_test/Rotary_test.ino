#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Accelerometer TODO
// SDA = A1
// SCL = A2

LiquidCrystal_I2C lcd(0x27, 20, 4);  

#define ROT_A      D3     // encoder A (moved from D3, which is now the red LED)
#define ROT_B      A3    // encoder B
#define BUTTON     D2     // color button (wired to GND, uses INPUT_PULLUP)
#define LED_RED    D4
#define LED_GREEN  D5
#define LED_BLUE   D6

const int MAX_VALUE       = 10;   // list goes 0..10, then wraps around
const int COUNTS_PER_STEP = 4;    // raw encoder pulses per click

volatile int           rotaryCounter  = 0;
volatile bool          rotaryChanged  = false;
volatile bool          buttonPending  = false; // if button has been dealt with yet
volatile unsigned long buttonEdgeTime = 0;
uint8_t encoderState = 0;

int  selected   = 0;       // number shown at the top of the list
bool buttonDown = false;   // true while the button is held
int  colorMode  = 0;       // 0 = red, 1 = green, 2 = blue

void setup()
{
  Serial.begin(9600);

  pinMode(ROT_A, INPUT_PULLUP);
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  showColor();   // start on red

  encoderState = (digitalRead(ROT_A) << 1) | digitalRead(ROT_B);

  attachInterrupt(digitalPinToInterrupt(ROT_A),  rotaryISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_B),  rotaryISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, CHANGE);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  drawNumbers();
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
  int8_t  move     = encoderTable[(encoderState << 2) | newState];

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

void loop()
{
  // if encoder changes, scroll list
  noInterrupts();
  bool changed = rotaryChanged;
  int  counts  = rotaryCounter;
  rotaryChanged = false;
  interrupts();

  if (changed) {
    int range = MAX_VALUE + 1;
    int steps = counts / COUNTS_PER_STEP;
    selected  = ((steps % range) + range) % range;   // wrap in both directions
    drawNumbers();
  }

  // if button pressed, change color
  noInterrupts();
  bool pending           = buttonPending;
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
      showColor();
      Serial.print("Color mode: ");
      Serial.println(colorMode);
    }
    else if (!pressedNow) {
      buttonDown = false;
    }
  }
}

void showColor()
{
  analogWrite(LED_RED,   colorMode == 0 ? 255 : 0);
  analogWrite(LED_GREEN, colorMode == 1 ? 255 : 0);
  analogWrite(LED_BLUE,  colorMode == 2 ? 255 : 0);
}

void drawNumbers()
{
  // Row 0 is the selected number (marked with '>'). Rows 1-3 show the next numbers.
  for (int row = 0; row < 4; row++) {
    int  n      = (selected + row) % (MAX_VALUE + 1);
    char marker = (row == 0) ? '>' : ' ';

    char text[21];
    char line[21];
    snprintf(text, sizeof(text), "%c Number %3d", marker, n);
    snprintf(line, sizeof(line), "%-20s", text);   // pad to 20 columns

    lcd.setCursor(0, row);
    lcd.print(line);
  }
}