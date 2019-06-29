int ledPin = 9;
int fade = 5;
int brightness = 0;
int delayTime = 15;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(ledPin, brightness);
}
