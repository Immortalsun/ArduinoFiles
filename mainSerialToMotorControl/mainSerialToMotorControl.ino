/*
This file serves as the main entry point for intiial tests to
send serial G-Code or other movement commands to the arduino ->
robot arm
*/

//IMPORTS
#include <Wire.h> 
#include <LiquidCrystal_PCF8574.h>
#include <AccelStepper.h>
#include <Servo.h>
#include <string.h>
#include <ctype.h>
using namespace std;
//END IMPORTS

//MOVEMENT CONSTANTS
const int MAX_SERVO_DEG = 180;
const int DRIVER_MODE = 1;
//END MOVEMENT CONSTANTS

//LCD VARIABLES
int CURSOR_ROW = 0;
int CURSOR_COL = 0;
const int MAX_VALUE_LENGTH = 5;
const int LINE_CURSOR_MAXROW = 4;
const int LINE_CURSOR_MAXCOL = 20;
//END LCD VARIABLES

//PIN DECLARATIONS
int MTR_A_STP = 22; //base
int MTR_A_DIR = 23;

int MTR_B_STP = 26; // shoulder motor 1
int MTR_B_DIR = 27;

int MTR_C_STP = 30; // shoulder motor 2
int MTR_C_DIR = 31;

int MTR_D_STP = 7; //wrist rotate
int MTR_D_DIR = 6;

int ELBOW_SERVO = 13;
//END PIN DECLARATIONS

//TIMING VARIABLES
unsigned long CURR_MILLIS = 0;
unsigned long PREV_MILLIS = 0;
unsigned long PREV_LCD_MILLIS = 0;
unsigned long PREV_MOTOR_MILLIS = 0;
unsigned long PREV_SERVO_MILLIS = 0;
int LCDRefreshInterval = 1000;//refreshes lcd every 1s
int scrollInterval = 10000;//10 second interval
const unsigned int MAX_INPUT = 50;
const long SERVO_INTERVAL = 60;//milliseconds
//END TIMING VARIABLES

//SERVO AND STEPPER OBJECTS
AccelStepper stprA(DRIVER_MODE, MTR_A_STP, MTR_A_DIR);//steppers B and C are twinned in the current design, and should be synced if possible
AccelStepper stprB(DRIVER_MODE, MTR_B_STP, MTR_B_DIR);
AccelStepper stprC(DRIVER_MODE, MTR_C_STP, MTR_C_DIR);
AccelStepper stprD(DRIVER_MODE, MTR_D_STP, MTR_D_DIR);
Servo elbowServo;
//END MOTOR AND STEPPER OBJECTS

//GLOBAL VARIABLES
LiquidCrystal_PCF8574 lcd(0x27);


int stepsPerRev = 6400;
int acceleration = 250; //steps per second, per second
int constantShoulderSpeed = 200; //steps per second (for NEMA 23 shoulder motors)
int constantBaseSpeed = 50; //steps per second (for nema 17 base motor)
int constantWristSpeed = 100;//steps per second (for wrist nema 17 motor)
int maxSpeed = 200; //steps per second

int elbowServoPos = 0;
int elbowServoTargetPos = 0;
int elbowServoInc = 1;

String currentCommand = "";
//END GLOBAL VARIABLES

//MAIN REQUIRED METHODS
void setup() 
{  
    Serial.begin(9600);
    elbowServo.attach(ELBOW_SERVO);
    lcd.begin(20, 4); // initialize the lcd
    lcd.setBacklight(255);
    resetCursorPosition();
    initialize(); //init Motors
}

void loop() 
{
    CURR_MILLIS = millis();
    listenForSerialInput();
    runMotors();
    runServos();
    refreshLcd();
}
//END MAIN REQUIRED METHODS

//Utility and Motor Control
void initialize()
{
    resetMaxSpeed();
    resetAllMotors();
    initializeTextArrays();
}

void initializeTextArrays(){
    setLineText("BSE: ", 0,0, false);
    setLineText("ELB: ",10,0, false);
    setLineText("SHD: ", 0,1, false);
    setLineText("RST: ",10,1, false);
    initCurrentStatus();
}

void resetAllMotors()
{
    resetZeroPosition(stprA);
    resetZeroPosition(stprB);
    resetZeroPosition(stprC);
    setServoZeroPosition(elbowServo);
    resetZeroPosition(stprD);
}

void resetMaxSpeed(){
    stprA.setMaxSpeed(maxSpeed);
    stprB.setMaxSpeed(maxSpeed);
    stprC.setMaxSpeed(maxSpeed);
    stprD.setMaxSpeed(maxSpeed);
}

void runMotors()
{
    stprA.runSpeedToPosition();
    stprB.runSpeedToPosition();
    stprC.runSpeedToPosition();
    stprD.runSpeedToPosition();
}

void runServos(){
     if(CURR_MILLIS - PREV_SERVO_MILLIS >= SERVO_INTERVAL){
         //elbow servo
         if(elbowServoPos != elbowServoTargetPos){
             if(elbowServoTargetPos > elbowServoPos){
                 elbowServoPos += elbowServoInc;
             }
             else{
                 elbowServoPos -= elbowServoInc;
             }
             servoRunToTargetPosition(elbowServo,elbowServoPos);
         }
         //end eblow servo handling
         PREV_SERVO_MILLIS += SERVO_INTERVAL;
     }
}
//end Utility and Motor Control

//LCD OUTPUT
void refreshLcd(){
    if(CURR_MILLIS - PREV_LCD_MILLIS >= LCDRefreshInterval){
        printLcdOutput();
        PREV_LCD_MILLIS+=LCDRefreshInterval;
    }
}

void resetCursorPosition(){
    CURSOR_COL = 0;
    CURSOR_ROW = 0;
    lcd.setCursor(CURSOR_COL,CURSOR_ROW);
}

void printLcdOutput(){
    resetCursorPosition();
    printStepperPositionText(stprA,5,0);
    printServoPositionText(15,0);
    printStepperPositionText(stprB,5,1);
    printStepperPositionText(stprD,15,1);
    printCurrentStatus();
}

void setLineText(String text, int column, int row, bool usePadding){
    CURSOR_ROW = row;
    CURSOR_COL = column;
    lcd.setCursor(CURSOR_COL, CURSOR_ROW);
    int padChars = LINE_CURSOR_MAXCOL - (column + text.length());
     if(usePadding && padChars > 0){
        int startPadIdx = (column+text.length());
        for(int j=startPadIdx; j<LINE_CURSOR_MAXCOL; j++){
            text+=' ';
        }
    }
    lcd.print(text);
}

void setStepperValueLineText(String text, int column, int row){
    CURSOR_ROW = row;
    CURSOR_COL = column;
    lcd.setCursor(CURSOR_COL, CURSOR_ROW);
    int padChars = MAX_VALUE_LENGTH - text.length();
     if(padChars > 0){
        int startPadIdx = (column+text.length());
        for(int j=startPadIdx; j<(startPadIdx+padChars); j++){
            text+=' ';
        }
    }
    lcd.print(text);
}

void printStepperPositionText(AccelStepper &stepper,int column, int row){
    if(!IsStepperRunning(stepper)){
        String stepperPos = String(stepper.currentPosition(), DEC);
        setStepperValueLineText(stepperPos, column, row);
    }
}

void printServoPositionText(int column, int row){
    String servoPos = String(elbowServoPos, DEC);
    setLineText(servoPos, column, row, true);
}

void initCurrentStatus(){
    setLineText("COM: ",0,2, false);
}

void printCurrentStatus(){ 
    printCurrentCommand();
    printCurrentTime();
}

void printCurrentTime(){
    String timeString = "";
    //todo: convert MILLIS into an elapsed min:sec timer
    unsigned long currentMilliValue = CURR_MILLIS;
    //3600000 milliseconds in a hour
    long hour = currentMilliValue / 3600000;
    currentMilliValue -= (3600000 * hour);
    //60000 milliseconds in a minute
    long min = currentMilliValue / 60000;
    currentMilliValue -= (60000 * min);
    //1000 milliseconds in a second
    long sec = currentMilliValue / 1000;
    currentMilliValue -= (1000 * sec);
    if(hour < 10){
        timeString+="0";
        timeString+=String(hour,DEC);
    }
    else{
        timeString+=String(hour, DEC);
    }
    timeString+= ":";
    if(min < 10){
        timeString+="0";
        timeString+= String(min, DEC);
    }
    else{
       timeString += String(min, DEC);
    }
    timeString+= ":";

    if(sec < 10){
        timeString+= "0";
        timeString+= String(sec,DEC);
    }
    else if(sec >= 10 && sec <= 59){
        timeString+= String(sec, DEC);
    }
    else {
        timeString+="00";
    }
    setLineText(timeString,0,3, false);
}

void printCurrentCommand(){
    setLineText(currentCommand,5,2, true);
}
//END LCD OUTPUT