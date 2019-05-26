
void handleButtonTouch()
{
  if (digitalRead(BUTTON_LEFT_UP_PIN) || digitalRead(BUTTON_LEFT_DOWN_PIN)
      || digitalRead(BUTTON_RIGHT_UP_PIN) || digitalRead(BUTTON_RIGHT_DOWN_PIN)) {
        boolean isChangeLeftPosition = false;
        boolean isChangeRightPosition = false;
        byte minPosition = loadCloseSunAngle();
        byte maxPosition = loadCloseNightAngle();
        byte actualLeftPosition = loadLeftBlindAngle();
        byte actualRightPosition = loadRightBlindAngle();

        while (digitalRead(BUTTON_LEFT_UP_PIN) || digitalRead(BUTTON_LEFT_DOWN_PIN)
              || digitalRead(BUTTON_RIGHT_UP_PIN) || digitalRead(BUTTON_RIGHT_DOWN_PIN)) {

              if (digitalRead(BUTTON_LEFT_UP_PIN)) {
                actualLeftPosition++;
                isChangeLeftPosition = true;
                moveLeftBlind(actualLeftPosition, false);
                delay(8);
              }
              if (digitalRead(BUTTON_LEFT_DOWN_PIN)) {
                actualLeftPosition--;
                isChangeLeftPosition = true;
                moveLeftBlind(actualLeftPosition, false);
                delay(8);
              }
              if (digitalRead(BUTTON_RIGHT_UP_PIN)) {
                actualRightPosition++;
                isChangeRightPosition = true;
                moveRightBlind(actualRightPosition, false);
                delay(8);
              }
              if (digitalRead(BUTTON_RIGHT_DOWN_PIN)) {
                actualRightPosition--;
                isChangeRightPosition = true;
                moveRightBlind(actualRightPosition, false);
                delay(8);
              }

            }
            if (isChangeLeftPosition) {
              saveLeftBlindAngle(actualLeftPosition);
            }
            if (isChangeRightPosition) {
              saveRightBlindAngle(actualRightPosition);
            }
      }
}
