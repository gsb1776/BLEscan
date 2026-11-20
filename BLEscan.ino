#include "BLEscan.h"

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  initializeSystem();
  //  testAll();
  //  testTurns();
  //  while(true);
}

void loop()
{
  static full_state fs = {
      {peek().x, peek().y, NONE}, // snakeHead
      randomFruit(),              // fruit
      500,                        // step
      1,                          // length
      millis(),                   // savedClock
      0,                          // avoidSteps
      s_INIT                      // state
  };

  // detect edge on button push
  static bool prevButton = false;
  bool curButton = false;
  if (digitalRead(2) == HIGH)
  {
    if (!prevButton)
    {
      curButton = true;
    }
    prevButton = true;
  }
  else
  {
    prevButton = false;
  }

  fs = updateSM(fs, getControl(), curButton, millis());
  // Serial.println(fs.state);

  static int loopCount = 5;
  loopCount++;
  if (loopCount == 6)
  { // toggle every 60ms
    toggleFruit();
    loopCount = 0;
  }
  delay(10);
}

/*
 * Takes in a struct representing the current SM state and state variables, as well as the three inputs
 * and returns the updated SM state and state variables
 */
full_state updateSM(full_state currState, orientation control, bool button, unsigned long clock)
{
  xyo head = currState.head;
  xy fruit = currState.fruit;
  unsigned long step = currState.step;
  byte snakeLength = currState.snakeLength;
  unsigned long savedClock = currState.savedClock;
  byte avoidSteps = currState.avoidSteps;
  fsm_state state = currState.state;

  full_state ret = currState; // FSM state and state variables to return. Default is to keep unchanged.
  switch (state)
  {
  case s_IDLE:
    // t 1-5
    break; // s_IDLE

  case s_SCAN_CALC:
    // t 2-2

    // t 2-3

    // t 2-4

    // t 2-5
    break; // s_SCAN_CALC

  case s_TRACK:
    // t 3-2

    // t 3-5
    break; // s_TRACK

  case s_SIG_LOST:
    // t 4-2

    // t 4-4

    // t 4-5

    break; // s_SIG_LOST

  case s_BROWSE:
    // t 5-2

    // t 5-5

    // t 5-6
    break; // s_BROWSE

  case s_UPDATE:
    // t 6-5

    // t 6-6
    break; // s_UPDATE

  default:
    Serial.println("Invalid state");
  }
  return ret;
}
