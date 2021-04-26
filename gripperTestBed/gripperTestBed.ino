//include AccelStepperLibrary for simpler motor interface
#include <AccelStepper.h>
#include <string.h>
#include <Wire.h> 

int GRPR_STP = 9;
int GRPR_DIR = 8;
int DRIVER_MODE = 1;
int acceleration = 400; //steps per second squared
int constantSpeed = 800; //steps per second
int maxSpeed = 800; //steps per second

int testStepsPerRun = 3200; //steps per test run 
int testRun = 0;
bool useAcceleration = false;
int testMotorInterval = 5000; //default value is 5 seconds. This interval is calculated based on use of acceleration vs. constant speed, and number of test steps per run
unsigned long PREV_MOTOR_MILLIS = 0;
unsigned long CURR_MILLIS = 0;
unsigned long PREV_MILLIS = 0;
//test intervals
int gripperCyclingMax = 400;
int gripperCurrentPos = 0;
//global settings
int testIntervalBuffer = 500;//milliseconds of buffer time between each test run
int stepsPerRev = 1600; //running in 1/8 step mode, 1600 full steps per rev

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
    setUpTestInterval();
    resetMaxSpeed();
    resetAllMotors();
}

void setUpTestInterval()
{
    if(useAcceleration){
        testMotorInterval = (calculateSecondsToPositionWithAcceleration(testStepsPerRun)*1000)+testIntervalBuffer; //convert to milliseconds and add buffer
    }
    else{
        testMotorInterval = (calculateSecondsToPositionForConstantSpeed(testStepsPerRun)*1000)+testIntervalBuffer;
    }
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

///<summary>
///@brief 
/// Estimates the time (in seconds) for a stepper motor to travel a distance (complete a supplied number of steps).
/// Formula takes into account both acceleration and deceleration (and assumes they are symmetric, 
/// thus deceleration is denoted as -acceleration)
///@param distanceInSteps 
///@return int 
///</summary>
int calculateSecondsToPositionWithAcceleration(int distanceInSteps){
    return (.5*((maxSpeed/acceleration)+(maxSpeed/-acceleration))+(distanceInSteps/maxSpeed));
}

///<summary>
///@brief 
/// Estimates the time (in seconds) for a stepper motor to travel a distance (complete a supplied number of steps)
/// while moving at a constant speed.
///@param distanceInSteps 
///@return int 
///</summary>
int calculateSecondsToPositionForConstantSpeed(int distanceInSteps){
    return (distanceInSteps/constantSpeed);
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
            headlessControlTestForward(testStepsPerRun, grprMtr);
            testRun++;
        }
        else if(testRun == 1){
            headlessControlTestBackward(testStepsPerRun, grprMtr);
            testRun--;
        }
        PREV_MOTOR_MILLIS+=testMotorInterval;
    }
}

void headlessControlTestForward(int stepCount, AccelStepper &stepper){
    int gripperPos =  stepper.currentPosition() + stepCount;
    if(useAcceleration){
         accelerateMotorToTargetPosition(stepper, gripperPos);
    }
    else{
        constantSpeedMotorToTargetPosition(stepper, gripperPos);
    }
}

void headlessControlTestBackward(int stepCount, AccelStepper &stepper){
    int gripperPos =  stepper.currentPosition() - stepCount;
     if(useAcceleration){
         accelerateMotorToTargetPosition(stepper, gripperPos);
    }
    else{
        constantSpeedMotorToTargetPosition(stepper, gripperPos);
    }
}
