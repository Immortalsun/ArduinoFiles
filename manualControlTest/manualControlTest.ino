#include <Wire.h> 
#include <LiquidCrystal_PCF8574.h>
#include <AccelStepper.h>
#include <string.h>

//motors
int MTR_A_STP = 13; //base
int MTR_A_DIR = 12;

int MTR_B_STP = 11; // shoulder motor 1
int MTR_B_DIR = 10;

int MTR_C_STP = 9; // shoulder motor 2
int MTR_C_DIR = 8;

int MTR_D_STP = 7; //wrist rotate
int MTR_D_DIR = 6;


//control pins, each button will be a digital high-low, using internal pull-up resistors 
const int CTRL_A = 2;
int prevCtrlAState = 1;

const int CTRL_B = 3;
int prevCtrlBState = 1;

const int CTRL_C = 4;
int prevCtrlCState = 1;

int motorSelection = 0; //0 is base, 1 is shoulder motor-1, 2 is shoulder motor-2, 3 is wrist rotation



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
int constantSpeed = 150; //steps per second
int maxSpeed = 150;    //steps per second
//global settings
int stepsPerRev = 3200; //running in 1/16 step mode, 200 full steps per rev * 16
int stepsPerPress = 100; // 1/16th rev per press - 3200 full rev, 1600 half, 800 qtr, 400 eigth, 200 sixteenth, 100 thirty-secondth
//Stepper Notes Section

/*
For the base motor (A): gear ratio is 2:1, two motor revolutions per axle revolution

Current tests show one full 360 degree base rotation in two full revolutions of the motor or 3200 steps 
Therefore, moving in a 180 deg range requires from steps 0 to 1600

For the shoulder motors (B and C), direct mount to motor spindles means 1:1 ratio. 
Usable arc from O position (determined my limit switch) to approx 1600 (half rev)
*/

//End Notes
//Create accelStepper objects
AccelStepper stprA(DRIVER_MODE, MTR_A_STP, MTR_A_DIR);//steppers A and B are twinned in the current design, and should be synced if possible
AccelStepper stprB(DRIVER_MODE, MTR_B_STP, MTR_B_DIR);
AccelStepper stprC(DRIVER_MODE, MTR_C_STP, MTR_C_DIR);
AccelStepper stprD(DRIVER_MODE, MTR_D_STP, MTR_D_DIR);
LiquidCrystal_PCF8574 lcd(0x27);

//MAIN REQUIRED METHODS
void setup() 
{  
    Serial.begin(9600);
    pinMode(CTRL_A, INPUT_PULLUP);
    pinMode(CTRL_B, INPUT_PULLUP);
    pinMode(CTRL_C, INPUT_PULLUP);
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
    //resetAcceleration();
    resetAllMotors();
}

void resetAllMotors()
{
    resetZeroPosition(stprA);
    resetZeroPosition(stprB);
    resetZeroPosition(stprC);
    //resetZeroPosition(stprD);
}

// void resetAcceleration(){
//     stprA.setAcceleration(acceleration);
//     stprB.setAcceleration(acceleration);
//     stprC.setAcceleration(acceleration);
//     //stprD.setAcceleration(acceleration);
// }

void resetMaxSpeed(){
    stprA.setMaxSpeed(maxSpeed);
    stprB.setMaxSpeed(maxSpeed);
    stprC.setMaxSpeed(maxSpeed);
    //stprD.setMaxSpeed(maxSpeed);
}

void runMotors()
{
    stprA.runSpeedToPosition();
    stprB.runSpeedToPosition();
    stprC.runSpeedToPosition();
    //uncomment to switch to accel/decel
    // stprA.run();
    // stprB.run();
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
    printStepperPositionText(stprA,String("A"));
    printStepperPositionText(stprB,String("B"));
    printStepperPositionText(stprC,String("C"));
    printCurrentStatus();
}

void printStepperPositionText(AccelStepper &stepper, String label){
    if(!IsStepperRunning(stepper)){
        String stepperPos = String(stepper.currentPosition(), DEC);
        lcd.print("MOTOR "+label+" POS: "+stepperPos+"      ");
    }
   
    //update cursor to print the next statement no matter what
    CURSOR_ROW++;
    lcd.setCursor(CURSOR_COL,CURSOR_ROW);
}

void printCurrentStatus(){
    switch (motorSelection)
    {
        case 0:
            lcd.print("CURRENT MOTOR: A    ");
            break;
        case 1:
            lcd.print("CURRENT MOTORS: B+C ");
            break;
        case 2:
            lcd.print("CURRENT MOTOR: D    ");
            break;
        default:
           lcd.print("CURRENT MOTOR: A    ");
            break;
    }
 }
//End LCD Methods

//Test and Control Method Section

void testControlInputs(){

    int ctrlAReading = digitalRead(CTRL_A);//forward (increase step)
    int ctrlBReading = digitalRead(CTRL_B);//backward (decrease step)
    int ctrlCReading = digitalRead(CTRL_C);//motor selector

    // increase step of selected motor
    if(ctrlAReading != prevCtrlAState){
       if(ctrlAReading == LOW){
            moveConstantSelectedMotor(stepsPerPress);
        }
        prevCtrlAState = ctrlAReading;
    }
    
    if(ctrlBReading != prevCtrlBState){
        if(ctrlBReading == LOW){
            moveConstantSelectedMotor(-stepsPerPress);
        }
        prevCtrlBState = ctrlBReading;
    }

    if(ctrlCReading != prevCtrlCState){
        if(ctrlCReading == LOW){
            selectMotor();
        }
        prevCtrlCState = ctrlCReading;
    }
}

void selectMotor(){
    if(motorSelection == 3){
        motorSelection = 0;
    }
    else {
         motorSelection++;
    }
}

void moveConstantSelectedMotor(long stepsPerPress){
    long positionA = stprA.currentPosition() + stepsPerPress;
    long positionB = stprB.currentPosition() + stepsPerPress;
    long positionC = stprC.currentPosition() + stepsPerPress;
    long positionD = stprD.currentPosition() + stepsPerPress;
  
    switch (motorSelection)
    {
        case 0:
            constantSpeedMotorToTargetPosition(stprA, positionA);
            return;
        case 1:
            constantSpeedMotorToTargetPosition(stprB, positionB);
            constantSpeedMotorToTargetPosition(stprC, positionC);
            return;
        case 2:
            constantSpeedMotorToTargetPosition(stprD, positionD);
            return;
        default:
            constantSpeedMotorToTargetPosition(stprB, positionB);
            constantSpeedMotorToTargetPosition(stprC, positionC);
            return;
    }
}

// void moveAccelerateSelectedMotor(long stepsPerPress){
//     long positionA = stprA.currentPosition() + stepsPerPress;
//     long positionB = stprB.currentPosition() + stepsPerPress;
//     long positionC = stprC.currentPosition() + stepsPerPress;
//     long positionD = stprD.currentPosition() + stepsPerPress;
  
//     switch (motorSelection)
//     {
//         case 0:
//             accelerateMotorToTargetPosition(stprA, positionA);
//             return;
//         case 1:
//             accelerateMotorToTargetPosition(stprB, positionB);
//             accelerateMotorToTargetPosition(stprC, positionC);
//             return;
//         case 2:
//             accelerateMotorToTargetPosition(stprD, positionD);
//             return;
//         default:
//             accelerateMotorToTargetPosition(stprB, positionB);
//             accelerateMotorToTargetPosition(stprC, positionC);
//             return;
//     }
// }

void resetZeroPosition(AccelStepper &stepper)
{
    stepper.setCurrentPosition(0);
}

// void accelerateMotorToTargetPosition(AccelStepper &stepper, long pos)
// {
//     if (!stepper.isRunning() && stepper.currentPosition() != pos)
//     {
//         stepper.moveTo(pos);
//     }
// }

void constantSpeedMotorToTargetPosition(AccelStepper &stepper, long pos)
{
    if (!IsStepperRunning(stepper) && stepper.currentPosition() != pos)
    {
        stepper.moveTo(pos);
        stepper.setSpeed(constantSpeed);
    }
}

bool IsStepperRunning(AccelStepper &stepper){
    return stepper.distanceToGo() != 0;
}
