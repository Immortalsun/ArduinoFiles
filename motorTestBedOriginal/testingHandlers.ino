void runTestsMain(){
    runServoTests();
    runStepperTests();
}

void runServoTests(){
    if(CURR_MILLIS - PREV_SERVO_MILLIS >= testServoInterval){
        if(servoTestRun == 0){
            MaxServoTestDeg = MAX_SERVO_DEG;
        }
        else if(servoTestRun == 1){
            MaxServoTestDeg = 90;
        }
        else if(servoTestRun == 2){
            MaxServoTestDeg = 60;
        }

        if(servoTestRun >= 0){
            headlessServoControlTestContinuous();
        }
        
        PREV_SERVO_MILLIS+=testServoInterval;
    }
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
    int positionB =  stprB.currentPosition() + stepCount;
    constantSpeedMotorToTargetPosition(stprB, positionB);
    //move shoulder motor C (INVERTED)
    int positionC =  stprC.currentPosition() - stepCount;
    constantSpeedMotorToTargetPosition(stprC, positionC);
}

void headlessControlTestBackward(int stepCount){
     //move shoulder motor B
    int positionB =  stprB.currentPosition() - stepCount;
    constantSpeedMotorToTargetPosition(stprB, positionB);
    //move shoulder motor C (INVERTED)
    int positionC =  stprC.currentPosition() + stepCount;
    constantSpeedMotorToTargetPosition(stprC, positionC);
}

void headlessServoControlTestContinuous(){
    if((elbowServoDeg+testServoDeg <= MaxServoTestDeg) && (elbowServoDeg+testServoDeg >= 0)){
        elbowServoDeg += testServoDeg;
    }
    else{
       testServoDeg = testServoDeg*(-1);
    }
    elbowServo.write(elbowServoDeg);
}

void headlessServoControlTestForward(int servoDeg){
    elbowServoDeg += servoDeg;
    elbowServo.write(elbowServoDeg);
}

void headlessServoControlTestBackward(int servoDeg){
    elbowServoDeg -= servoDeg;
    elbowServo.write(elbowServoDeg);
}

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
