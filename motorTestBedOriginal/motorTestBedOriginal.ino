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
const int elbowServoDegPerPress = 5;
const int MAX_SERVO_DEG = 180;

//control pins, each button will be a digital high-low, using internal pull-up resistors 
const int CTRL_A =7;
int prevCtrlAState = 1;

const int CTRL_B = 6;
int prevCtrlBState = 1;

const int CTRL_C = 5;
int prevCtrlCState = 1;

const int CTRL_D = 4;
int prevCtrlDState = 1;

int motorSelection = 0; //0 is base, 1 is shoulder motor-1 and 2, 2 is wrist rotation


int DRIVER_MODE = 1;

//display variables
int CURSOR_ROW = 0;
const int LINE_CURSOR_MAXROW = 4;
int CURSOR_COL = 0;
const int LINE_CURSOR_MAXCOL = 20;

int VIEWPORT_OFFSET = 0;
unsigned long CURR_MILLIS = 0;
unsigned long PREV_MILLIS = 0;
unsigned long PREV_LCD_MILLIS = 0;
unsigned long PREV_TEST_MILLIS = 0;

//VARIABLE SETTINGS FOR TESTING 
int testInterval = 3000;//wait two seconds between motor tests
int testRun = 0; //which test to run,  currently 1 is shoulder -forward-(same direction of wrist facing) and 2 is shoulder -backward-(opposite direction of wrist facing)
//display modes
int displayMode = 0; //0 - default displays positions for A, B, and C, 4th line configurable via testLCDMode
int testLCDMode = 1; //0 - default displays current motor selection, 1 - display current MILLIS as seconds value, max
//refresh intervals
int refreshInterval = 1000;//refreshes lcd every 1s
int scrollInterval = 10000;//10 second interval
//motor variables
int acceleration = 250; //steps per second, per second
int constantSpeed = 200; //steps per second
int maxSpeed = 200;    //steps per second
//END VARIABLE SETTINGS

//global settings
int stepsPerRev = 6400; //running in 1/32 step mode, 200 full steps per rev * 32
int stepsPerPress = 200; // 1/32nd rev per press - 6400 full rev, 3200 half, 1600 qtr, 800 eigth, 400 sixteenth, 200 thirty-secondth
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
//create lcd screen
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
    //testControlInputs(); - uncomment for manual control
    //runServos(); - uncomment for servo control
    runMotors();
    refreshLcd();
    runTestsMain();
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
    switch(displayMode){
        case 0:
            setLineText("MOTOR A POS: ",0,0);
            setLineText("MOTOR B POS: ", 0,1);
            setLineText("MOTOR C POS: ", 0,2);
            initCurrentStatus();
            break;
        case 1:
            break;
        default:
            break;
    }
}

void resetAllMotors()
{
    resetZeroPosition(stprA);
    resetZeroPosition(stprB);
    resetZeroPosition(stprC);
    setServoZeroPosition(elbowServo);
    //resetZeroPosition(stprD);
}

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
}

void runServos(){
    if(prevElbowServoDeg != elbowServoDeg){
         elbowServo.write(elbowServoDeg);
    }
}
//end Utility and Motor Control


//Test and Control Method Section
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
    if(elbowServoDeg+elbowServoDegPerPress <= MAX_SERVO_DEG){
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

void setServoZeroPosition(Servo &servo){
    servo.write(0);
}

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
