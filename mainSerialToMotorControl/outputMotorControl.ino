//Main Control Methods
void resetZeroPosition(AccelStepper &stepper)
{
    stepper.setCurrentPosition(0);
}

void setServoZeroPosition(Servo &servo){
    servo.write(0);
}

void servoRunToTargetPosition(Servo &servo, int pos){
    if(pos >= 0 && pos <= MAX_SERVO_DEG){
        servo.write(pos);
    }
}

void constantSpeedMotorToTargetPosition(AccelStepper &stepper, long pos)
{
    if (!IsStepperRunning(stepper) && stepper.currentPosition() != pos)
    {
        stepper.moveTo(pos);
        stepper.setSpeed(constantSpeed);
    }
}

int getMotorTravelTimeInMillis(int stepsTravel){
    return ((constantSpeed/stepsTravel)*1000)+500;
}

bool IsStepperRunning(AccelStepper &stepper){
    return stepper.distanceToGo() != 0;
}

void calculateAndRunStepper(AccelStepper &stepper, int position, bool isAbsolute){
    long targetPos = 0;
    if(isAbsolute){
        targetPos += position;
    }
    else{
        targetPos = stepper.currentPosition() + position;
    }
    constantSpeedMotorToTargetPosition(stepper, targetPos);
}

int calculateServoTargetPosition(int inputPosition, int currentPosition, bool isAbsolute){
    int targetPosition = currentPosition;
    if(isAbsolute){
        targetPosition = inputPosition;
    }
    else{
        targetPosition = (currentPosition + inputPosition);
    }
    return targetPosition;
}
//END Control Methods