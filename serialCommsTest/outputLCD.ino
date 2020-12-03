//LCD Methods
void refreshLcd(){
    if(CURR_MILLIS - PREV_LCD_MILLIS >= refreshInterval){
        printLcdOutput();
        PREV_LCD_MILLIS+=refreshInterval;
        lcd.blink();
    }
}

void printLcdOutput(){
    if(hasInput){
        setLineText(STR_IN);
        if(CURSOR_COL+STR_IN.length()-1 < LINE_CURSOR_MAXCOL){
           CURSOR_COL+=STR_IN.length()-1;
        }
        else {
            CURSOR_COL = 0;
            if(CURSOR_ROW < LINE_CURSOR_MAXROW){
                CURSOR_ROW++;
            }
            else{
                CURSOR_ROW = 0;
            }
        }
        lcd.setCursor(CURSOR_COL,CURSOR_ROW);
        hasInput = false;
    }
}

void resetCursorPosition(){
    CURSOR_COL = 0;
    CURSOR_ROW = 0;
    lcd.setCursor(CURSOR_COL,CURSOR_ROW);
}

void setLineText(String text){
    lcd.setCursor(CURSOR_COL, CURSOR_ROW);
    // int padChars = LINE_CURSOR_MAXCOL - (CURSOR_COL + text.length());
    // if(padChars > 0){
    //     CURSOR_COL+=text.length();
    // }
    lcd.print(text.substring(0,text.length()-1));
}
//End LCD Methods