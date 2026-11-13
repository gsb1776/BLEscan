#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  //The address could be either 0x27 or 0x3C.
// https://makerhardware.net/knowledge-base/projects/interfacing-the-2004-lcd-screen-with-an-arduino-uno/
 
 
void setup() {
  lcd.init();                 //Initialise the LCD
  lcd.backlight();            //Activate backlight     
  lcd.home();  
}
void loop() {
  lcd.setCursor(0,0);
  lcd.print("2004 LCD");
  lcd.setCursor(0,1);
  lcd.print("POWERED BY");
  lcd.setCursor(0,2);
  lcd.print("JULIE WANG");
  lcd.setCursor(0,3);
  lcd.print("CS1600");
  lcd.display();
}