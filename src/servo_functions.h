#include <Arduino.h>
#include <Servo.h>

#define maxTimeRunServo 1500 //ms
#define MIN_POSITION 5
#define MAX_POSITION 175

#define LEFT_SERVO_PIN 5
#define RIGHT_SERVO_PIN 20

boolean isRunLeftServo = false;
boolean isRunRightServo = false;

long timeEndLeftServo = 0;
long timeEndRightServo = 0;

Servo leftServo;
Servo rightServo;

boolean isOpenTransistor = false;

/*----------------------FUNCTIONS FOR OPEN POWER TO SERVO ----------------*/
void openTransistor()
{
  //digitalWrite(TRANSISTOR_PIN, HIGH);
  isOpenTransistor = true;
}

void closeTransistor()
{
  //digitalWrite(TRANSISTOR_PIN, LOW);
  isOpenTransistor = false;
}

/*-----------------------FUNCTIONS FOR ON/OFF SERVO MOTORS--------------------*/
void startLeftServo(int angle)
{
  openTransistor();
  timeEndLeftServo = millis() + maxTimeRunServo;
  isRunLeftServo = true;
  leftServo.attach(LEFT_SERVO_PIN);
  leftServo.write(angle);
  Serial.println("Left servo started with angle " + (String)angle);
}

void startRightServo(int angle)
{
  openTransistor();
  timeEndRightServo = millis() + maxTimeRunServo;
  isRunRightServo = true;
  rightServo.attach(RIGHT_SERVO_PIN);
  rightServo.write(angle);
  Serial.println("Right servo started with angle " + (String)angle);
}

void stopLeftServo()
{
  leftServo.detach();
  isRunLeftServo = false;
  timeEndLeftServo = 0;
  Serial.println("Left servo was stoped");
}

void stopRightServo()
{
  rightServo.detach();
  isRunRightServo = false;
  timeEndRightServo = 0;
  Serial.println("Right servo was stoped");
}
/*--------------------------HELPER FUNCTIONS-----------------------*/
byte mirrorAngle(byte angle)
{
  if (angle >= MIN_POSITION) {
    byte newAngle = MAX_POSITION - (angle - MIN_POSITION);
    return newAngle;
  }
}

byte validationAngle(byte angle)
{
  if (angle > MAX_POSITION) {
    return (byte)MAX_POSITION;
  }else if (angle < MIN_POSITION) {
    return (byte)MIN_POSITION;
  }else {
    return (byte)angle;
  }
}

byte convertPercentOnPoint(byte angle)
{
  byte point =  map(angle, MIN_POSITION, MAX_POSITION, 0, 100);
  return point;
}

byte convertPointToPercent(byte point)
{
  byte percent = map(point, 0, 100, MIN_POSITION, MAX_POSITION);
  return percent;
}

/*-----------------------CONTROL BLINDS FUNCTIONS--------------------*/
void moveLeftBlind(int angle, boolean save)
{
  byte newAngle = mirrorAngle(validationAngle(angle)); // lave servo sa toci v opacnom smere
  startLeftServo(newAngle);
  if (save == true) {
    saveLeftBlindAngle(angle);
  }
}

void moveRightBlind(int angle, boolean save)
{
  byte newAngle = validationAngle(angle);
  startRightServo(newAngle);
  if (save == true) {
    saveRightBlindAngle(newAngle);
  }
}

void moveAllBlinds(int angle, boolean save)
{
  if (save == true) {
    moveLeftBlind(angle, true);
    moveRightBlind(angle, true);
  }else {
    moveLeftBlind(angle, false);
    moveRightBlind(angle, false);
  }
}
/*-------------------------HANDLER FUNCTION FOR POWER OFF SERVO --------------*/
