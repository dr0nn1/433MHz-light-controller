enum states
{
  S_IDLE,
  S_ON,
  S_OFF,
};  
uint8_t currentState = S_IDLE;
uint8_t lastState = S_OFF;
uint8_t LED = 4; //used for testing.
boolean timedOut = false;
unsigned long nextTimeout = 0; //Time for next timeout, in milliseconds
unsigned long turnOffTime = 300000; //turn off the light if it is on for more then 5 min

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT); //sensor 1
  pinMode(A1, INPUT); //sensor 2
  //pinMode(LED, OUTPUT); tested with LED.
  // Transmitter is connected to Arduino Pin #12
  mySwitch.enableTransmit(12);
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(6);
}

void loop() {

  switch (currentState) {
    case S_IDLE:
      controller();
      break;
    case S_ON:
      Serial.println("S_ON");
      //turnLedOn();
      lightOn();
      startTimer(turnOffTime);
      changeStateTo(S_IDLE);
      break;
    case S_OFF:
      Serial.println("S_OFF");
      //turnLedOff();
      lightOff();
      changeStateTo(S_IDLE);
      break;
  }
}

void controller() {
  //delay(100);
  int valueRight = analogRead(A0);
  int valueLeft = analogRead(A1);

  for (int i=0;i<9;i++){
    delay(10);
    valueRight += analogRead(A0);
    valueLeft += analogRead(A1);
  }
  valueRight = int(valueRight / 10);
  valueLeft = int(valueLeft / 10);

//      Serial.print("Høyre: ");
//      Serial.print(valueRight);
//      Serial.print("          Venstre: ");
//      Serial.println(valueLeft);

  if ((valueRight < 350 || valueLeft < 300) && lastState != S_ON) {
    changeStateTo(S_ON);
    lastState = S_ON;
    timedOut = false;
  }

  else if (valueRight > 350 && valueLeft > 400 && lastState != S_OFF) {
    changeStateTo(S_OFF);
    lastState = S_OFF;
  }
  else if (timerHasExpired() && lastState != S_OFF && timedOut != true) {
    changeStateTo(S_OFF);
    timedOut = true;
  }

}

void lightOn() {
  mySwitch.send("00011111000011010000000101010000");
}

void lightOff() {
  mySwitch.send("00011111000011010000000101010001");
}

boolean timerHasExpired()
{
  boolean hasExpired = false;
  if (millis() > nextTimeout)
  {
    hasExpired = true;
  }
  else
  {
    hasExpired = false;
  }
  return hasExpired;
}

void startTimer(unsigned long duration)
{
  nextTimeout = millis() + duration;
}

void changeStateTo(uint8_t newState)
{
  currentState = newState;
}

//Testing fuctions
void turnLedOn()
{
  digitalWrite(LED, HIGH);
}

void turnLedOff()
{
  digitalWrite(LED, LOW);
}
