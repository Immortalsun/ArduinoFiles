#include <LiquidCrystal_PCF8574.h>

//display variables
int CURSOR_ROW = 0;
const int LINE_CURSOR_MAXROW = 4;
int CURSOR_COL = 0;
const int LINE_CURSOR_MAXCOL = 20;

int VIEWPORT_OFFSET = 0;
unsigned long CURR_MILLIS = 0;
unsigned long PREV_LCD_MILLIS = 0;

String STR_IN;
bool hasInput = false;

//display modes
int displayMode = 0; //0 - default displays positions for A, B, and C, 4th line configurable via testLCDMode
int testLCDMode = 1; //0 - default displays current motor selection, 1 - display current MILLIS as seconds value, max
//refresh intervals
int refreshInterval = 1000;//refreshes lcd every 1s
//END VARIABLE SETTINGS

//create lcd screen
LiquidCrystal_PCF8574 lcd(0x27);
//MAIN REQUIRED METHODS
void setup() 
{  
    Serial.begin(9600);
    lcd.begin(20, 4); // initialize the lcd
    lcd.setBacklight(255);
    resetCursorPosition();
    initialize(); //init 
}

void loop() 
{
    CURR_MILLIS = millis();
    checkBuffer();
    refreshLcd();
}
//END MAIN REQUIRED METHODS

void initialize(){
    while(!Serial){
        //wait until serial is ready
    }
}

void checkBuffer(){
    if(Serial.available()){
        STR_IN = Serial.readStringUntil('\n');
        hasInput = true;
    }
}

