//Main Control Methods
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
//END Control Methods