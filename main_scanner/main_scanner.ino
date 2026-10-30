/*
* Prints the result of the button press to the serial monitor.
*/

int BUTTON_PIN = 6;
int BUTTON_PIN_2 = 7;
int LED_PIN_0 = A5;
int LED_PIN_1 = A4;
int LED_PIN_2 = A3;// most significant 

int counter;// goes from 0 to 7

bool down_1;
bool down_2;

void setup() {
   Serial.begin(9600);
   while (!Serial); // Wait for Serial to initialize
   pinMode(BUTTON_PIN, INPUT);
   pinMode(BUTTON_PIN_2, INPUT);
   pinMode(LED_PIN_0, OUTPUT);
   pinMode(LED_PIN_1, OUTPUT);
   pinMode(LED_PIN_2, OUTPUT);
   counter =0;

   bool down_1 = false;
   bool down_2 = false;

}

void loop() {
  // analogWrite(LED_PIN_0, digitalRead())
  // Serial.println(digitalRead(BUTTON_PIN));
  // delay(100);

  // Serial.println(digitalRead(BUTTON_PIN_2));
  // Serial.println(digitalRead(BUTTON_PIN));
  // Serial.println(counter);
  if (digitalRead(BUTTON_PIN)){
    if(!down_1){
      if(counter != 7){
        counter +=1;
      } 
      down_1 = true;
    }
  }
  else {
    down_1 = false; 
  }
  if (digitalRead(BUTTON_PIN_2)){
    //Serial.println(5);
    if(!down_2){
      if(counter!=0){
        //Serial.println("yes");
        counter = counter-1;
      }
      down_2 = true;
    }
  }
  else {
    down_2 = false; 
  }

  // led logic
  if(counter & 1){
    //Serial.println("yes");
    digitalWrite(LED_PIN_0, HIGH);
  }
  else {
    digitalWrite(LED_PIN_0, LOW);
  }

  if(counter & 2){
    //Serial.println("no");
    digitalWrite(LED_PIN_1, HIGH);
  }
  else {
    digitalWrite(LED_PIN_1, LOW);
  }
  
  if(counter & 4){
    // Serial.println("come");
    digitalWrite(LED_PIN_2, HIGH);
  }
  else {
    digitalWrite(LED_PIN_2, LOW); 
  }
}