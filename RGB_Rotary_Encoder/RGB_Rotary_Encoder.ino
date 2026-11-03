/*
  RGB_Rotary_Encoder
  Updated for Arduino Uno R4 WiFi
  Original: SparkFun Electronics
  Modified: Ho Yun Bobby Chan
  Uno R4 WiFi adaptation: PinChangeInt removed

  Rotary encoder:
    A  -> D3
    B  -> A3
    C  -> GND

  RGB LED (common anode):
    Red    -> D10
    Green  -> A1
    Blue   -> D5
    Common -> 5V

  Button:
    Switch -> D4
    Use external pulldown resistor (1K-10K)
    Button reads HIGH when pressed.
*/

#include <Arduino.h>

// LilyPad MP3 pin definitions
#define ROT_LEDG A1
#define ROT_B A3
#define ROT_A 3
#define ROT_SW 4
#define ROT_LEDB 5
#define ROT_LEDR 10

// RGB LED colors (common anode: 0 = ON, 1 = OFF)
#define OFF    B111
#define RED    B110
#define GREEN  B101
#define YELLOW B100
#define BLUE   B011
#define PURPLE B010
#define CYAN   B001
#define WHITE  B000

// Interrupt variables
volatile int rotary_counter = 0;
volatile bool rotary_change = false;

// Button interrupt event tracking
volatile bool button_edge_pending = false;
volatile unsigned long button_edge_time = 0;

// Button state variables
bool button_pressed = false;
bool button_released = false;
unsigned long button_downtime = 0;

bool button_state = false;
bool button_down = false;
bool hold_reported = false;

unsigned long button_down_start = 0;
unsigned long button_down_time = 0;

// LED color
static unsigned char x = 0;


// --------------------------------------------------
// SETUP
// --------------------------------------------------

void setup()
{
  // Rotary encoder inputs
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(ROT_A, INPUT_PULLUP);

  // Button input: external pulldown resistor required
  pinMode(ROT_SW, INPUT);

  // RGB LED outputs
  pinMode(ROT_LEDB, OUTPUT);
  pinMode(ROT_LEDG, OUTPUT);
  pinMode(ROT_LEDR, OUTPUT);

  setLED(WHITE);

  Serial.begin(9600);

  Serial.println("Begin RGB Rotary Encoder Testing - Uno R4 WiFi");

  // Attach rotary encoder interrupt
  attachInterrupt(
    digitalPinToInterrupt(ROT_A),
    rotaryIRQ,
    CHANGE
  );

  attachInterrupt(
    digitalPinToInterrupt(ROT_B),
    rotaryIRQ,
    CHANGE
  );

  // Attach button interrupt
  attachInterrupt(
    digitalPinToInterrupt(ROT_SW),
    buttonIRQ,
    CHANGE
  );
}


// --------------------------------------------------
// BUTTON INTERRUPT
// --------------------------------------------------

void buttonIRQ()
{
  // Record the time of the latest button edge.
  // Debouncing is handled in loop(), not in the ISR.

  button_edge_time = millis();
  button_edge_pending = true;
}


// --------------------------------------------------
// ROTARY ENCODER INTERRUPT
// --------------------------------------------------

void rotaryIRQ()
{
  static uint8_t lastState = 0;

  uint8_t A = digitalRead(ROT_A);
  uint8_t B = digitalRead(ROT_B);

  uint8_t currentState = (A << 1) | B;

  // Initialize the previous state
  if (lastState == 0) {
    lastState = currentState;
    return;
  }

  // Quadrature transition lookup table
  const int8_t encoderTable[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  uint8_t index = (lastState << 2) | currentState;

  int8_t movement = encoderTable[index];

  if (movement != 0)
  {
    rotary_counter += movement;
    rotary_change = true;
  }

  lastState = currentState;
}


// --------------------------------------------------
// MAIN LOOP
// --------------------------------------------------

void loop()
{
  // ----------------------------------------------
  // Check rotary encoder
  // ----------------------------------------------

  bool changed;
  int position;

  noInterrupts();

  changed = rotary_change;
  position = rotary_counter;
  rotary_change = false;

  interrupts();

  if (changed)
  {
    Serial.print("rotary: ");
    Serial.println(position);

    // Blink for visual feedback
    setLED(OFF);
    delay(10);
    setLED(x);
  }


  // ----------------------------------------------
  // Check button interrupt and debounce
  // ----------------------------------------------

  bool pending;
  unsigned long edgeTime;

  noInterrupts();

  pending = button_edge_pending;
  edgeTime = button_edge_time;
  button_edge_pending = false;

  interrupts();

  // Wait until the button signal has been stable
  // for at least 10 milliseconds.

  if (pending && (millis() - edgeTime >= 10))
  {
    bool reading = digitalRead(ROT_SW);

    // Button pressed
    if (reading == HIGH && !button_state)
    {
      button_state = true;
      button_pressed = true;
    }

    // Button released
    else if (reading == LOW && button_state)
    {
      button_state = false;
      button_released = true;

      button_downtime = millis() - button_down_start;
    }
  }


  // ----------------------------------------------
  // Handle button press
  // ----------------------------------------------

  if (button_pressed)
  {
    Serial.println("button press");

    button_pressed = false;

    button_down = true;
    hold_reported = false;

    button_down_start = millis();
  }


  // ----------------------------------------------
  // Handle button release
  // ----------------------------------------------

  if (button_released)
  {
    Serial.print("button release, downtime: ");
    Serial.println(button_downtime);

    x++;
    setLED(x);

    button_released = false;
    button_down = false;

    // Blink red LED when button is released

    for (int i = 0; i < 4; i++)
    {
      digitalWrite(ROT_LEDR, LOW);
      digitalWrite(ROT_LEDG, HIGH);
      digitalWrite(ROT_LEDB, HIGH);

      delay(50);

      digitalWrite(ROT_LEDR, HIGH);
      digitalWrite(ROT_LEDG, HIGH);
      digitalWrite(ROT_LEDB, HIGH);

      delay(50);
    }

    setLED(x);
  }


  // ----------------------------------------------
  // Check button hold duration
  // ----------------------------------------------

  if (button_down)
  {
    button_down_time = millis() - button_down_start;

    if (button_down_time > 1000 && !hold_reported)
    {
      Serial.println("button held down for one second");
      hold_reported = true;
    }

    // Display red while button is held

    digitalWrite(ROT_LEDR, LOW);
    digitalWrite(ROT_LEDG, HIGH);
    digitalWrite(ROT_LEDB, HIGH);
  }
}


// --------------------------------------------------
// RGB LED CONTROL
// --------------------------------------------------

void setLED(unsigned char color)
{
  digitalWrite(ROT_LEDR, color & B001);
  digitalWrite(ROT_LEDG, color & B010);
  digitalWrite(ROT_LEDB, color & B100);
}