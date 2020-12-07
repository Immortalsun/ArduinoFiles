//COMMAND DECLARATIONS
const char BASE_CMD = 'B';
const char ELBW_CMD = 'E';
const char SHLDR_CMD = 'S';
const char WRIST_CMD = 'W';
const char INCR_CMD = '+';
const char DECR_CMD = '-';
const char CMD_DELIM = '|';
//END COMMAND DECLARATIONS

void listenForSerialInput(){
    if(Serial.available() > 0){
        processIncomingByte(Serial.read());
    }
}

void processIncomingByte (const byte inByte)
{
    //static variables are treated as globals
    //they are not reinitialized on each call of function
    static char input_line [MAX_INPUT];
    static unsigned int input_pos = 0;

    switch (inByte)
    {
        case CMD_DELIM:   // end of text delimiter
        case '\n': //or end of input
            input_line [input_pos] = CMD_DELIM;  // terminating byte
            processSerialData(input_line, input_pos+1);
            clearBuffer(input_line, input_pos+1);

            // reset buffer for next time
            input_pos = 0;  
            break;

        case '\r':   // discard carriage return
            break;

        default:
            if (input_pos < (MAX_INPUT - 1)){
                input_line [input_pos++] = inByte;
            }
            break;
    }
}

void clearBuffer(char *dataArray, int size){
    for(int i = 0; i<size; i++){
        dataArray[i]=' ';
    }
}

void processSerialData(char *dataArray, int size){
    char currentChar;
    char axis;
    int signMultiplier = 1;
    int position = 0;
    char positionBytes[] = {' ',' ',' '};
    int pBytesIdx = 0;
    bool isAbsolute = true;
    currentCommand = "";
    for(int i=0; i<size; i++){
        currentChar = dataArray[i];
        if(isAxisCharacter(currentChar)){
            currentCommand = String(currentChar);
            axis = currentChar;
        }
        else if(isIncrementOrDecrement(currentChar)){
            //we want increase or decrese of current position
            isAbsolute = false;
            currentCommand += currentChar;
            if(currentChar == DECR_CMD){
                signMultiplier = -1;
            }
        }
        else if(isDigit(currentChar) && pBytesIdx < 3){
            positionBytes[pBytesIdx] = currentChar;
            pBytesIdx++;
            currentCommand += currentChar;
        }

        if(currentChar == CMD_DELIM){
            position = atoi(positionBytes);
            position *= signMultiplier;
            break;
        }
    }
    Serial.println(String(axis)+" "+ String(position));
    processCommand(axis, position, isAbsolute);    
}

bool isAxisCharacter(char input){
    if(isalpha(input)){
        return input == BASE_CMD || input == ELBW_CMD ||
        input == SHLDR_CMD || input == WRIST_CMD;
    }
    return false;
}

bool isIncrementOrDecrement(char input){
    return input == INCR_CMD || input == DECR_CMD;
}

void processCommand(char axis, int movement, bool isAbsolute){
    switch(axis){
        case BASE_CMD:
            break;
        case ELBW_CMD:
            elbowServoTargetPos = calculateServoTargetPosition(movement, elbowServoPos, isAbsolute);
            break;
        case SHLDR_CMD:
            calculateAndRunStepper(stprB, movement, isAbsolute);
            calculateAndRunStepper(stprC, (movement*-1), isAbsolute);
            break;
        case WRIST_CMD:
            break;
        default:
            return;
    }
}


