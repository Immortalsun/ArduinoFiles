int motorPinA = 11;
int motorPinB = 10;
int potentiometerPin = 0;
int prevForecast = -1;
double alphaFactor = .1;

void setup() {
  Serial.begin(57600);
  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);


  digitalWrite(motorPinA, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int potValue = analogRead(A0);
  if(prevForecast == -1){
      prevForecast = potValue;
  }
  int smoothPotValue = smoothInput(prevForecast, potValue);
  int pwmOut = map(smoothPotValue,0,1023,40,230);

  printResult(potValue,smoothPotValue,pwmOut);
  analogWrite(motorPinB, pwmOut);
  
}

int smoothInput(int &forecast, int observedVal){
    double output = forecast + alphaFactor*(observedVal - forecast);
    forecast = int(output);
    return forecast;
}

void printResult(int rawPotVal, int smoothPotVal, int pwmOut){
  Serial.print("POT IN: ");
  Serial.println(rawPotVal);

  Serial.print("POT SMOOTH: ");
  Serial.println(smoothPotVal);
  
  Serial.print("PWM OUT: ");
  Serial.println(pwmOut);
}
