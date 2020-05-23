#include <Wire.h> 
#include <LiquidCrystal_PCF8574.h>
#include <AccelStepper.h>
#include <string.h>

//motors
int MTR_A_STP = 13;
int MTR_A_DIR = 12;

int MTR_B_STP = 11;
int MTR_B_DIR = 10;

int MTR_C_STP = 9;
int MTR_C_DIR = 8;

int MTR_D_STP = 7;
int MTR_D_DIR = 6;

int MTR_E_STP = 5;
int MTR_E_DIR = 4;

//control pins, each button wil lbe a digital high-low 
const int CTRL_A = 2;
int prevCtrlAState = 0;
const int CTRL_B = 3;
int prevCtrlBState = 0;



int DRIVER_MODE = 1;

//display variables
int CURSOR_ROW = 0;
int LINE_CURSOR_MAXROW = 4;
int CURSOR_COL = 0;
int LINE_CURSOR_MAXCOL = 20;

int VIEWPORT_OFFSET = 0;
unsigned long CURR_MILLIS = 0;
unsigned long PREV_MILLIS = 0;
unsigned long PREV_LCD_MILLIS = 0;

int refreshInterval = 1500;//refreshes lcd every 1.5s
int scrollInterval = 10000;//10 second interval
int acceleration = 250; //steps per second, per second
int maxSpeed = 750;    //steps per second
//global settings
int stepsPerRev = 1600; //running in 1/8 step mode, 200 full steps per rev * 8
int stepsPerPressA = 200; // 1/16th rev per press - 3200 full rev, 1600 half, 800 qtr, 400 eigth, 200 sixteenth
int stepsPerPressB = 50;
//Stepper Notes Section

/*
For the base motor: gear ratio is 2:1, two motor revolutions per axle revolution

Current tests show one full 360 degree base rotation in two full revolutions of the motor or 3200 steps 
Therefore, moving in a 180 deg range requires from steps 0 to 1600

*/

//End Notes
//Create accelStepper objects
AccelStepper stprA(DRIVER_MODE, MTR_A_STP, MTR_A_DIR);
AccelStepper stprB(DRIVER_MODE, MTR_B_STP, MTR_B_DIR);
AccelStepper stprC(DRIVER_MODE, MTR_C_STP, MTR_C_DIR);
AccelStepper stprD(DRIVER_MODE, MTR_D_STP, MTR_D_DIR);
AccelStepper stprE(DRIVER_MODE, MTR_E_STP, MTR_E_DIR);
LiquidCrystal_PCF8574 lcd(0x27);

//MAIN REQUIRED METHODS
void setup() 
{  
    Serial.begin(9600);
    pinMode(CTRL_A, INPUT);
    pinMode(CTRL_B, INPUT);
    lcd.begin(20, 4); // initialize the lcd
    lcd.setBacklight(255);
    resetCursorPosition();
    initialize(); //init Motors
}

void loop() 
{
    CURR_MILLIS = millis();
    testControlInputs();
    runMotors();
    refreshLcd();
}
//END MAIN REQUIRED METHODS

//Utility and Motor Control
void initialize()
{
    resetMaxSpeed();
    resetAcceleration();
    resetAllMotors();
}

void resetAllMotors()
{
    resetZeroPosition(stprA);
    resetZeroPosition(stprB);
    resetZeroPosition(stprC);
    resetZeroPosition(stprD);
}

void resetAcceleration(){
    stprA.setAcceleration(acceleration);
    stprB.setAcceleration(acceleration);
    // stprC.setAcceleration(acceleration);
    // stprD.setAcceleration(acceleration);
}

void resetMaxSpeed(){
    stprA.setMaxSpeed(maxSpeed);
    stprB.setMaxSpeed(maxSpeed);
    // stprC.setMaxSpeed(maxSpeed);
    // stprD.setMaxSpeed(maxSpeed);
}

void runMotors()
{
    stprA.run();
    stprB.run();
    // stprC.run();
    // stprD.run();
}
//end Utility and Motor Control


//LCD Methods
void refreshLcd(){
    if(CURR_MILLIS - PREV_LCD_MILLIS >= refreshInterval){
        printLcdOutput();
        PREV_LCD_MILLIS+=refreshInterval;
    }
}

void resetCursorPosition(){
    CURSOR_COL = 0;
    CURSOR_ROW = 0;
    lcd.setCursor(CURSOR_COL,CURSOR_ROW);
}

void printLcdOutput(){
    resetCursorPosition();
    // if(CURR_MILLIS - PREV_MILLIS >= scrollInterval){
    //     PREV_MILLIS+=scrollInterval;
    //     switch (VIEWPORT_OFFSET)
    //     {
    //         case 0:
    //             VIEWPORT_OFFSET = 1;
    //             break;
    //         default:
    //             VIEWPORT_OFFSET = 0;
    //             break;
    //     }
    // }

    printStepperPositionText(stprA,String("A"));
    printStepperPositionText(stprB,String("B"));
    // printStepperPositionText(stprC,String("C"));
    // printStepperPositionText(stprD,String("D"));
    // if(VIEWPORT_OFFSET == 1){
    //     printStepperPositionText(stprE,String("E"));
    // }
}

void printStepperPositionText(AccelStepper &stepper, String label){
    if(!stepper.isRunning()){
        String stepperPos = String(stepper.currentPosition(), DEC);
        lcd.print("MOTOR "+label+" POS: "+stepperPos+"   ");
        CURSOR_ROW++;
        lcd.setCursor(CURSOR_COL,CURSOR_ROW);
    }
}
//End LCD Methods

//Test and Control Method Section

void testControlInputs(){

    int ctrlAReading = digitalRead(CTRL_A);
    int ctrlBReading = digitalRead(CTRL_B);

    //test button A (base motor)
    if(ctrlAReading != prevCtrlAState){
        prevCtrlAState = ctrlAReading;
        if(ctrlAReading == HIGH){
            //move motor 1/16 adjust
            long targetAPos = stprA.currentPosition() + stepsPerPressA;
            accelerateMotorToTargetPosition(stprA, targetAPos);
        }
    }
    //test button B (shoulder motor)
    if(ctrlBReading != prevCtrlBState){
        prevCtrlBState = ctrlBReading;
        if(ctrlBReading == HIGH){
            long targetBPos = stprB.currentPosition() + stepsPerPressB;
            accelerateMotorToTargetPosition(stprB, targetBPos);
        }
    }
}




void resetZeroPosition(AccelStepper &stepper)
{
    stepper.setCurrentPosition(0);
}

void accelerateMotorToTargetPosition(AccelStepper &stepper, long pos)
{
    if (!stepper.isRunning() && stepper.currentPosition() != pos)
    {
        stepper.moveTo(pos);
    }
}

void constantSpeedMotorToTargetPosition(AccelStepper &stepper, long pos, int spd)
{
    if (!stepper.isRunning() && stepper.currentPosition() != pos)
    {
        stepper.moveTo(pos);
    }
    stepper.setSpeed(spd);
}
