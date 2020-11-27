void runTestsMain(){
     if(CURR_MILLIS - PREV_TEST_MILLIS >= testInterval){
        if(testRun == 0){
            headlessControlTestForward(200);
            testRun++;
        }
        else if(testRun == 1){
            headlessControlTestBackward(200);
            testRun--;
        }
        PREV_TEST_MILLIS+=testInterval;
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
