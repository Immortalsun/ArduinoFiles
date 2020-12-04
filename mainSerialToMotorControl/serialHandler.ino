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
        case '\n':   // end of text
            input_line [input_pos] = 0;  // terminating null byte
            processSerialData(input_line);

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

void processSerialData(char *dataArray){
    
}
