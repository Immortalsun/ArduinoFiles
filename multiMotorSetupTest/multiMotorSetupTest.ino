//This file is mainly for documenting and testing 5 stepper motors (NEMA 17s)
//Arduino is connected to Five A4988 Controllers through digital pins

//include AccelStepperLibrary for simpler motor interface
#include <AccelStepper.h>

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

int DRIVER_MODE = 1;

int acceleration = 750; //steps per second, per second
int maxSpeed = 1000;    //steps per second
//global settings
int stepsPerRev = 1600; //running in 1/8 step mode, 200 full steps per rev * 8

//Create accelStepper objects
AccelStepper stprA(DRIVER_MODE, MTR_A_STP, MTR_A_DIR);
AccelStepper stprB(DRIVER_MODE, MTR_B_STP, MTR_B_DIR);
AccelStepper stprC(DRIVER_MODE, MTR_C_STP, MTR_C_DIR);
AccelStepper stprD(DRIVER_MODE, MTR_D_STP, MTR_D_DIR);

void setup()
{
    initialize();
}

void loop()
{
    //TEST SECTION
    accelerateMotorToTargetPositionTest(stprA, 1600);
    //constantSpeedMotorToTargetPositionTest(stprA,1600,500);
    accelerateMotorToTargetPositionTest(stprB, 1200);
    // accelerateMotorToTargetPositionTest(stprC, 800);
    // accelerateMotorToTargetPositionTest(stprD, 400);
    //END TESTS
    runMotors();
}

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
    delayMicroseconds(100);
}

//Test and Utility Method Section
void resetZeroPosition(AccelStepper &stepper)
{
    stepper.setCurrentPosition(0);
}

void accelerateMotorToTargetPositionTest(AccelStepper &stepper, long pos)
{
    if (stepper.currentPosition() != pos && !stepper.isRunning())
    {
        stepper.moveTo(pos);
    }
}

void constantSpeedMotorToTargetPositionTest(AccelStepper &stepper, long pos, int spd)
{
    if (stepper.currentPosition() != pos && !stepper.isRunning())
    {
        stepper.moveTo(pos);
    }
    stepper.setSpeed(spd);
}
//End Test and Utility Section

//Serial Output Functions
// void printTestOutput(){
//     Serial.print("STEPPER A POS: " + stprA.currentPosition());
// }
//End Serial Output Functions