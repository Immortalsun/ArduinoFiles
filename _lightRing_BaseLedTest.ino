
int ledPin = 9;
int ledPinGroup2 = 3;
int delayTime = 15;

int btnPin = 8;
int clickCount = 0;
int prevBtnState = HIGH;

int ledBrightness = 0;
int ledFade = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPinGroup2, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
}

void loop() {
  checkButtonState();
  applyButtonState();
  
  delay(delayTime);
}


void checkButtonState(){
  int buttonState = digitalRead(btnPin);
  int prevClick = clickCount;
  if(buttonState == HIGH && prevBtnState == LOW){
      clickCount++;
   }
   if(clickCount >= 4){
     clickCount = 0;
   }

   if(prevClick != clickCount 
    || clickCount == 0){
     getLedScheme();
   }
   prevBtnState = buttonState;
}

void applyButtonState(){
  applyToLed(ledPin, ledFade, ledBrightness);
}

void turnAllOff(){
      ledBrightness = 0;
      ledFade = 0;
}

void applyToLed(int ledPin, int &fadeFactor, int &brightness){
      updateLedBrightness(fadeFactor, brightness);
      fadeLED(ledPin, brightness);
      fadeLED(ledPinGroup2, brightness);
      updateFadeFactor(fadeFactor, brightness);
}

void getLedScheme(){
    switch(clickCount){
    case 0:
      turnAllOff();
      break;
    case 1:
      ledFade = 5;
      delayTime = 5;
      break;
    case 2:
      delayTime = 20;
      break;
    case 3:
      delayTime = 60;
      break;
  }
}


void updateFadeFactor(int &fade, int &brightness){
   if (brightness <= 0 || brightness >= 255) {
    fade = -fade;
  }
}

void updateLedBrightness(int &fade, int &brightness){
  brightness += fade;
}

void fadeLED(int ledPin, int brightness){
  analogWrite(ledPin, brightness);
}
