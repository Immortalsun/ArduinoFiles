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
    printServoPositionText(15,0);
    printStepperPositionText(stprB,13,1);
    printStepperPositionText(stprC,13,2);
    printCurrentStatus();
}

void setLineText(String text, int column, int row){
    CURSOR_ROW = row;
    CURSOR_COL = column;
    lcd.setCursor(CURSOR_COL, CURSOR_ROW);
    int padChars = LINE_CURSOR_MAXCOL - (column + text.length());
    if(padChars > 0){
        int startPadIdx = (column+text.length());
        for(int j=startPadIdx; j<LINE_CURSOR_MAXCOL; j++){
            text+=' ';
        }
    }
    lcd.print(text);
}

void printStepperPositionText(AccelStepper &stepper,int column, int row){
    if(!IsStepperRunning(stepper)){
        String stepperPos = String(stepper.currentPosition(), DEC);
        setLineText(stepperPos, column, row);
    }
}

void printServoPositionText(int column, int row){
    String servoPos = String(elbowServoDeg, DEC);
    setLineText(servoPos, column, row);
}

void initCurrentStatus(){
    switch(testLCDMode){
        case 0:
            setLineText("CURRENT MOTOR: ",0,3);
            break;
        case 1:
            setLineText("T: ",0,3);
            break;
    }
}

void printCurrentStatus(){
    switch(testLCDMode){
        case 0:
            printCurrentMotorStatus();
            break;
        case 1:
            printCurrentTime();
            break;
    }
}

void printCurrentTime(){
    String timeString = "";
    //todo: convert MILLIS into an elapsed min:sec timer
    unsigned long currentMilliValue = CURR_MILLIS;
    //60000 milliseconds in a minute
    long min = currentMilliValue / 60000;
    currentMilliValue -= (60000 * min);
    //1000 milliseconds in a second
    long sec = currentMilliValue / 1000;
    currentMilliValue -= (1000 * sec);
    if(min < 10){
        timeString+="0";
        timeString+= String(min, DEC);
    }
    else{
       timeString += String(min, DEC);
    }
    timeString+= ":";

    if(sec < 10){
        timeString+= "0";
        timeString+= String(sec,DEC);
    }
    else if(sec >= 10 && sec <= 59){
        timeString+= String(sec, DEC);
    }
    else {
        timeString+="00";
    }
    setLineText(timeString,2,3);
}


void printCurrentMotorStatus(){
     switch (motorSelection)
    {
        case 0:
            setLineText("A",15,3);
            break;
        case 1:
            setLineText("B+C",15,3);
            break;
        case 2:
            setLineText("D", 15,3);
            break;
        default:
            setLineText("A",15,3);
            break;
    }
 }
//End LCD Methods