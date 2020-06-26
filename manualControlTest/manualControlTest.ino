#include <Wire.h> 
#include <LiquidCrystal_PCF8574.h>
#include <AccelStepper.h>
#include <Servo.h>
#include <string.h>

//motors
int MTR_A_STP = 22; //base
int MTR_A_DIR = 23;

int MTR_B_STP = 26; // shoulder motor 1
int MTR_B_DIR = 27;

int MTR_C_STP = 30; // shoulder motor 2
int MTR_C_DIR = 31;

int MTR_D_STP = 7; //wrist rotate
int MTR_D_DIR = 6;

//servos
int ELBOW_SERVO = 13;
int elbowServoDeg = 0;
int prevElbowServoDeg = 0;
const int elbowServoDegPerPress = 30;
const int MAX_SERVO_DEG = 270;

//control pins, each button will be a digital high-low, using internal pull-up resistors 
const int CTRL_A =7;
int prevCtrlAState = 1;

const int CTRL_B = 6;
int prevCtrlBState = 1;

const int CTRL_C = 5;
int prevCtrlCState = 1;

const int CTRL_D = 4;
int prevCtrlDState = 1;

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
int constantSpeed = 100; //steps per second
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
Usable arc from 500 position (determined by weight tests) to approx 800 (60 deg of rotation)
*/

//End Notes
//Create accelStepper objects
AccelStepper stprA(DRIVER_MODE, MTR_A_STP, MTR_A_DIR);//steppers B and C are twinned in the current design, and should be synced if possible
AccelStepper stprB(DRIVER_MODE, MTR_B_STP, MTR_B_DIR);
AccelStepper stprC(DRIVER_MODE, MTR_C_STP, MTR_C_DIR);
AccelStepper stprD(DRIVER_MODE, MTR_D_STP, MTR_D_DIR);
//create servo objects
Servo elbowServo;
LiquidCrystal_PCF8574 lcd(0x27);

//MAIN REQUIRED METHODS
void setup() 
{  
    Serial.begin(9600);
    pinMode(CTRL_A, INPUT_PULLUP);
    pinMode(CTRL_B, INPUT_PULLUP);
    pinMode(CTRL_C, INPUT_PULLUP);
    pinMode(CTRL_D, INPUT_PULLUP);
    elbowServo.attach(ELBOW_SERVO);
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
    runServos();
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

void runServos(){
    if(prevElbowServoDeg != elbowServoDeg){
         elbowServo.write(elbowServoDeg);
    }
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
    int ctrlDReading = digitalRead(CTRL_D);//prompt servo movement

    // increase step of selected motor
    if(ctrlAReading != prevCtrlAState){
       if(ctrlAReading == LOW){
            moveConstantSelectedMotor(stepsPerPress);
        }
        prevCtrlAState = ctrlAReading;
    }
    //decrease step of selected motor
    if(ctrlBReading != prevCtrlBState){
        if(ctrlBReading == LOW){
            moveConstantSelectedMotor(-stepsPerPress);
        }
        prevCtrlBState = ctrlBReading;
    }

    //change stepper motor selection
    if(ctrlCReading != prevCtrlCState){
        if(ctrlCReading == LOW){
            selectMotor();
        }
        prevCtrlCState = ctrlCReading;
    }

    //variable fourth control, currently servo motor control
    if(ctrlDReading != prevCtrlDState){
        if(ctrlDReading == LOW){
            updateServoPosition();
        }
        prevCtrlDState = ctrlDReading;
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

void updateServoPosition(){
    prevElbowServoDeg = elbowServoDeg;
    
    if(elbowServoDeg < MAX_SERVO_DEG){
        elbowServoDeg += elbowServoDegPerPress;
    }
    else{
        elbowServoDeg = 0;
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

void resetZeroPosition(AccelStepper &stepper)
{
    stepper.setCurrentPosition(0);
}

void constantSpeedMotorToTargetPosition(AccelStepper &stepper, long pos)
{
    if (!IsStepperRunning(stepper) && stepper.currentPosition() != pos)
    {
        stepper.moveTo(pos);
        stepper.setSpeed(constantSpeed);
    }
}

// void accelerateMotorToTargetPosition(AccelStepper &stepper, long pos)
// {
//     if (!stepper.isRunning() && stepper.currentPosition() != pos)
//     {
//         stepper.moveTo(pos);
//     }
// }


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


bool IsStepperRunning(AccelStepper &stepper){
    return stepper.distanceToGo() != 0;
}
