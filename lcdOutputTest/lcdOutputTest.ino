#include <Wire.h> 
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

void setup() 
{  
    lcd.begin(20, 4); // initialize the lcd
    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
}

void loop() 
{
    lcd.clear();
    lcd.print("MTR Aa POS: 800");
    lcd.setCursor(0,1);
    lcd.print("MTR B POS: 1600");
    lcd.setCursor(0,2);
    lcd.cursor();
    lcd.blink();
    delay(8000);
}

void doOutputScan(){
    Serial.println("LCD...");
    int error = -1;
    // wait on Serial to be available on Leonardo
    while (!Serial)
        ;

    Serial.println("Dose: check for LCD");

    // See http://playground.arduino.cc/Main/I2cScanner how to test for a I2C device.
    Wire.begin();
    Wire.beginTransmission(0x27);
    error = Wire.endTransmission();

    if (error == 0) {
        Serial.println(": LCD found.");
        

    } else {
        Serial.println(": LCD not found.");
        Serial.print("Error: ");
        Serial.print(error);
    } // if
}