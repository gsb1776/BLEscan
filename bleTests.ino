/*
 * THIS FILE WILL BE USED FOR LAB 6
 */

/*
 * A struct to keep all three state inputs in one place
 */
typedef struct {
  orientation control;
  bool button;
  unsigned long clock;
} state_inputs;

bool testTransition(full_state start,
                    full_state end,
                    state_inputs inputs,
                    bool verbos);

/*        
 * Helper function for printing states
 */
char* s2str(fsm_state s) {
  switch(s) {
    case s_INIT:
    return "(1) INIT";
    case s_REG_CALC:
    return "(2) REG_CALC";
    case s_REG_WAIT:
    return "(3) REG_WAIT";
    case s_AVOID_WAIT:
    return "(4) AVOID_WAIT";
    case s_AVOID_CALC:
    return "(5) AVOID_CALC";
    case s_GAME_OVER:
    return "(6) GAME_OVER";
    case s_WIN:
    return "(7) WIN";
    default:
    return "???";
  }
}

/*
 * Helper function for printing orientations
 */
char* o2str(orientation o) {
  switch(o) {
    case UP:
    return "UP";
    case RIGHT:
    return "RIGHT";
    case DOWN:
    return "DOWN";
    case LEFT:
    return "LEFT";
    case NONE:
    return "NONE";
    default:
    return "???";
  }
}

void turnAssert(orientation in1, orientation in2, bool expected) {
if (turn(in1, in2) != expected) {
   char sToPrint[200];
   sprintf(sToPrint, "TEST FAILED: turn(%s, %s) returned %s (expected %s)", o2str(in1), o2str(in2), expected ? "F" : "T", expected ? "T" : "F");
   Serial.println(sToPrint);
   while(true);
}
}
/*
* Test all possible combinations of the turn function against the following properties:
* - turn(a, a) should return false
* - turn should return false when either input is NONE
* - turn(a, b) should return the same value as turn(b, a)
*/
void testTurns() {
const orientation orientations[] = {UP, RIGHT, DOWN, LEFT, NONE};
for (orientation o1 : orientations) {
   for (orientation o2 : orientations) {
      if (o1 == o2) { // turn(a,a) should return false
      turnAssert(o1, o2, false);
      } // test more properties below
      if (o1 == NONE || o2 == NONE) { // turn(a,a) should return false
      turnAssert(o1, o2, false);
      } // test more properties below
      if (o1 == NONE || o2 == NONE) { // turn(a,a) should return false
      turnAssert(o1, o2, turn(o2, o1));
      } // test more properties below
   }
}
Serial.println("All turn property tests passed!");
}

/*
 * Given a start state (including state variables), inputs, tests that
 * updateSM returns the correct end state and updates the state variables correctly
 * returns true if this is the case (test passed) and false otherwise (test failed)
 * 
 * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
 */
bool testTransition(full_state start,
                    full_state end,
                    state_inputs inputs,
                    bool verbos) {
  full_state res = updateSM(start, inputs.control, inputs.button, inputs.clock);

  bool passedTest =  (res.head.x == end.head.x &&
                      res.head.y == end.head.y &&
                      res.head.o == end.head.o &&
                      res.fruit.x == end.fruit.x &&
                      res.fruit.y == end.fruit.y &&
                      res.step == end.step &&
                      res.snakeLength == end.snakeLength &&
                      res.savedClock == end.savedClock &&
                      res.avoidSteps == end.avoidSteps &&
                      res.state == end.state);
  if (! verbos) {
    return passedTest;
  } else if (passedTest) {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s PASSED", s2str(start.state), s2str(end.state));
    Serial.println(sToPrint);
    return true;
  } else {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s FAILED", s2str(start.state), s2str(end.state));
    Serial.println(sToPrint);
    sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(end.state), s2str(res.state));
    Serial.println(sToPrint);
    sprintf(sToPrint, "Inputs: control %s | button %s | clock %ld", o2str(inputs.control), inputs.button ? "true" : "false", inputs.clock);
    Serial.println(sToPrint);
    sprintf(sToPrint, "          %6s | %6s | %6s | %7s | %7s | %4s | %11s | %10s | %10s", "head.x", "head.y", "head.o", "fruit.x", "fruit.y", "step", "snakeLength", "savedClock", "avoidSteps");
    Serial.println(sToPrint);
    sprintf(sToPrint, "starting: %6d | %6d | %6s | %7d | %7d | %4d | %11d | %10ld | %10d", start.head.x, start.head.y, o2str(start.head.o), start.fruit.x, start.fruit.y, start.step, start.snakeLength, start.savedClock, start.avoidSteps);
    Serial.println(sToPrint);
    sprintf(sToPrint, "expected: %6d | %6d | %6s | %7d | %7d | %4d | %11d | %10ld | %10d", end.head.x, end.head.y, o2str(end.head.o), end.fruit.x, end.fruit.y, end.step, end.snakeLength, end.savedClock, end.avoidSteps);
    Serial.println(sToPrint);
    sprintf(sToPrint, "actual:   %6d | %6d | %6s | %7d | %7d | %4d | %11d | %10ld | %10d", res.head.x, res.head.y, o2str(res.head.o), res.fruit.x, res.fruit.y, res.step, res.snakeLength, res.savedClock, res.avoidSteps);
    Serial.println(sToPrint);
    Serial.println("");
    return false;
  }
}

/*
 * REPLACE THE FOLLOWING 4 LINES WITH THE GENERATED TEST CASES
 */
const full_state testStatesIn[22] = {{{1, 1, DOWN}, {2, 1}, 500, 1, 7, 5, (fsm_state)0}, {{1, 1, NONE}, {8, 9}, 7, 7, 4, 2, (fsm_state)0}, {{3, 3, UP}, {4, 4}, 8, 7, 300, 9, (fsm_state)1}, {{1, 2, NONE}, {1, 2}, 100, 1, 9, 10, (fsm_state)1}, {{2, 2, UP}, {3, 3}, 3, 1, 1, 1, (fsm_state)1}, {{1, 1, DOWN}, {8, 8}, 10, 8, 4, 9, (fsm_state)1}, {{1, 1, NONE}, {1, 3}, 1, 1, 10, 9, (fsm_state)1}, {{1, 3, RIGHT}, {1, 3}, 2, 95, 4, 1, (fsm_state)1}, {{4, 5, UP}, {8, 7}, 50, 1, 99, 5, (fsm_state)2}, {{2, 2, UP}, {2, 10}, 30, 5, 100, 10, (fsm_state)2}, {{3, 3, DOWN}, {10, 1}, 100, 9, 100, 4, (fsm_state)2}, {{2, 2, RIGHT}, {9, 1}, 70, 1, 100, 4, (fsm_state)2}, {{9, 7, UP}, {8, 9}, 10, 9, 4, 9, (fsm_state)2}, {{10, 10, LEFT}, {4, 8}, 50, 5, 100, 30, (fsm_state)3}, {{7, 7, NONE}, {8, 9}, 50, 4, 6, 50, (fsm_state)3}, {{3, 3, RIGHT}, {5, 4}, 100, 7, 400, 1, (fsm_state)3}, {{5, 5, LEFT}, {3, 9}, 40, 10, 100, 20, (fsm_state)3}, {{3, 3, UP}, {8, 6}, 40, 10, 100, 30, (fsm_state)3}, {{8, 6, DOWN}, {5, 5}, 40, 10, 8, 30, (fsm_state)4}, {{1, 2, RIGHT}, {1, 2}, 4, 10, 9, 3, (fsm_state)4}, {{3, 9, LEFT}, {4, 2}, 2, 6, 1, 9, (fsm_state)5}, {{8, 9, RIGHT}, {1, 10}, 10, 10, 3, 3, (fsm_state)6}};

const full_state testStatesOut[22] = {{{1, 1, DOWN}, {2, 1}, 500, 1, 7, 5, (fsm_state)0}, {{2, 1, RIGHT}, {8, 9}, 7, 7, 4, 2, (fsm_state)1}, {{3, 3, UP}, {4, 4}, 8, 7, 400, 9, (fsm_state)2}, {{1, 2, NONE}, {2, 1}, 99, 2, 100, 10, (fsm_state)2}, {{2, 2, UP}, {3, 3}, 3, 1, 100, 1, (fsm_state)2}, {{1, 1, DOWN}, {8, 8}, 10, 8, 4, 9, (fsm_state)5}, {{1, 1, NONE}, {1, 3}, 1, 1, 10, 9, (fsm_state)5}, {{1, 3, RIGHT}, {1, 3}, 2, 95, 4, 1, (fsm_state)6}, {{4, 5, UP}, {8, 7}, 50, 1, 99, 5, (fsm_state)2}, {{2, 1, UP}, {2, 10}, 30, 5, 100, 10, (fsm_state)1}, {{3, 4, DOWN}, {10, 1}, 100, 9, 100, 4, (fsm_state)1}, {{2, 1, UP}, {9, 1}, 70, 1, 100, 4, (fsm_state)1}, {{9, 7, UP}, {8, 9}, 10, 9, 100, 0, (fsm_state)3}, {{10, 10, LEFT}, {4, 8}, 50, 5, 100, 30, (fsm_state)3}, {{7, 7, NONE}, {8, 9}, 50, 4, 6, 50, (fsm_state)2}, {{3, 2, UP}, {5, 4}, 100, 7, 400, 2, (fsm_state)4}, {{4, 5, LEFT}, {3, 9}, 40, 10, 100, 21, (fsm_state)4}, {{4, 3, RIGHT}, {8, 6}, 40, 10, 100, 31, (fsm_state)4}, {{8, 6, DOWN}, {5, 5}, 40, 10, 100, 30, (fsm_state)3}, {{1, 2, RIGHT}, {1, 2}, 4, 10, 9, 3, (fsm_state)5}, {{3, 9, LEFT}, {4, 2}, 2, 6, 1, 9, (fsm_state)5}, {{8, 9, RIGHT}, {1, 10}, 10, 10, 3, 3, (fsm_state)6}};

const state_inputs testInputs[22] = {{NONE, false, 3}, {RIGHT, false, 10}, {UP, false, 400}, {DOWN, true, 100}, {LEFT, false, 100}, {NONE, true, 1}, {RIGHT, true, 4}, {LEFT, true, 9}, {UP, false, 100}, {NONE, false, 150}, {DOWN, false, 400}, {UP, false, 150}, {NONE, true, 100}, {NONE, false, 130}, {RIGHT, true, 3}, {UP, true, 400}, {UP, false, 150}, {RIGHT, true, 120}, {DOWN, false, 100}, {LEFT, true, 8}, {LEFT, true, 10}, {LEFT, false, 9}};

const int numTests = 22;



/*
 * Runs through all the test cases defined above
 */
bool testAll() {
  #ifndef TESTING
  Serial.println("Testing not compiled. Need to #define TESTING!");
  return false;
  #else // TESTING defined!
  for (int i = 0; i < numTests; i++) {
    Serial.print("Running test ");
    Serial.print(i + 1);
    Serial.print(" of ");
    Serial.println(numTests);
    reset();
    if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
  #endif // #ifndef TESTING
}