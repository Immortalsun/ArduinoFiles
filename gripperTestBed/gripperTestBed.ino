//include AccelStepperLibrary for simpler motor interface
#include <AccelStepper.h>
#include <string.h>
#include <Wire.h> 

int GRPR_STP = 9;
int GRPR_DIR = 8;
int DRIVER_MODE = 1;
int acceleration = 200; //steps per second, per second
int constantSpeed = 1000; //steps per second
int maxSpeed = 1000; //steps per second+

int testSteps = 3200; //steps per test run
int testRun = 0;
bool useAcceleration = false;
int testMotorInterval = ((testSteps/constantSpeed)*1000)+500; //calculated from (constantSpeed/testSteps)*1000(milliseconds)+(500)//.5-sec buffer
unsigned long PREV_MOTOR_MILLIS = 0;
unsigned long CURR_MILLIS = 0;
unsigned long PREV_MILLIS = 0;
//test intervals
int gripperCyclingMax = 400;
int gripperCurrentPos = 0;
//global settings
int stepsPerRev = 6400; //running in 1/32 step mode, 200 full steps per rev * 32
int stepsPerPress = 200; // 1/32nd rev per press - 6400 full rev, 3200 half, 1600 qtr, 800 eigth, 400 sixteenth, 200 thirty-secondth

AccelStepper grprMtr(DRIVER_MODE, GRPR_STP, GRPR_DIR);

void setup()
{
    initialize(); //init Motors
}

void loop()
{
    CURR_MILLIS = millis();
    runMotors();
    runStepperTests();
}

//Utility and Motor Control
void initialize()
{
    resetMaxSpeed();
    resetAllMotors();
}

void resetAllMotors()
{
    resetZeroPosition(grprMtr);
}

void resetMaxSpeed(){
    grprMtr.setMaxSpeed(maxSpeed);
    if(useAcceleration){
         grprMtr.setAcceleration(acceleration);
    }
    else{
        grprMtr.setSpeed(constantSpeed);
    }
}

void runMotors()
{
    if(useAcceleration){
        grprMtr.run();
    }
    else{
        grprMtr.runSpeedToPosition();
    }
}

void moveConstantSelectedMotor(long stepsPerPress){
    long positionA = grprMtr.currentPosition() + stepsPerPress;
    constantSpeedMotorToTargetPosition(grprMtr, positionA);
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

void accelerateMotorToTargetPosition(AccelStepper &stepper, long pos)
{
    if (stepper.currentPosition() != pos && !stepper.isRunning())
    {
        stepper.moveTo(pos);
    }
}

bool IsStepperRunning(AccelStepper &stepper){
    return stepper.distanceToGo() != 0;
}


void runStepperTests(){
    if(CURR_MILLIS - PREV_MOTOR_MILLIS >= testMotorInterval){
        if(testRun == 0){
            headlessControlTestForward(testSteps);
            testRun++;
        }
        else if(testRun == 1){
            headlessControlTestBackward(testSteps);
            testRun--;
        }
        PREV_MOTOR_MILLIS+=testMotorInterval;
    }
}

void headlessControlTestForward(int stepCount){
    //move shoulder motor B
    int gripperPos =  grprMtr.currentPosition() + stepCount;
    if(useAcceleration){
         accelerateMotorToTargetPosition(grprMtr, gripperPos);
    }
    else{
        constantSpeedMotorToTargetPosition(grprMtr, gripperPos);
    }
}

void headlessControlTestBackward(int stepCount){
     //move shoulder motor B
    int gripperPos =  grprMtr.currentPosition() - stepCount;
     if(useAcceleration){
         accelerateMotorToTargetPosition(grprMtr, gripperPos);
    }
    else{
        constantSpeedMotorToTargetPosition(grprMtr, gripperPos);
    }
}
