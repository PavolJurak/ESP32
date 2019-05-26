void startEEPROM()
{
  EEPROM.begin(20);
}

/*Pozicie v pamati
0 -> uhol otocenia lavej lamely zaluzie
1 -> uhol otocenia pravej lamely zaluzie
2 -> uhol CLOSE_SUN
3 -> uhol CLOSE_NIGHT
4 -> uhol OPEN_LOW
5 -> uhol OPEN_MIDDLE
6 -> uhol OPEN_HIGHT
*/

byte loadLeftBlindAngle()
{
  byte angle = EEPROM.read(0);
  return angle;
}

byte loadRightBlindAngle()
{
  byte angle = EEPROM.read(1);
  return angle;
}

byte loadCloseSunAngle()
{
  byte angle = EEPROM.read(2);
  return angle;
}

byte loadCloseNightAngle()
{
  byte angle = EEPROM.read(3);
  return angle;
}

byte loadOpenLowtAngle()
{
  byte angle = EEPROM.read(4);
  return angle;
}

byte loadOpenMiddleAngle()
{
  byte angle = EEPROM.read(5);
  return angle;
}

byte loadOpenHightAngle()
{
  byte angle = EEPROM.read(6);
  return angle;
}

void saveLeftBlindAngle(byte angle)
{
  EEPROM.write(0, angle);
  EEPROM.commit();
  Serial.println("Save to EEPROM LeftBlindAngle=" + (String)angle);
}

void saveRightBlindAngle(byte angle)
{
  EEPROM.write(1, angle);
  EEPROM.commit();
  Serial.println("Save to EEPROM RightBlindAngle=" + (String)angle);
}

void saveCloseSunAngle(byte angle)
{
  EEPROM.write(2, angle);
  EEPROM.commit();
  Serial.println("Save to EEPROM CLOSE_SUN=" + (String)angle);
}

void saveCloseNightAngle(byte angle)
{
  EEPROM.write(3, angle);
  EEPROM.commit();
  Serial.println("Save to EEPROM CLOSE_NIGHT=" + (String)angle);
}

void saveOpenLowAngle(byte angle)
{
  EEPROM.write(4, angle);
  EEPROM.commit();
  Serial.println("Save to EEPROM OPEN_LOW=" + (String)angle);
}

void saveOpenMiddleAngle(byte angle)
{
  EEPROM.write(5, angle);
  EEPROM.commit();
  Serial.println("Save to EEPROM OPEN_MIDDLE=" + (String)angle);
}

void saveOpenHightAngle(byte angle)
{
  EEPROM.write(6, angle);
  EEPROM.commit();
  Serial.println("Save to EEPROM OPEN_HIGHT=" + (String)angle);
}

String getJsonEepromValues()
{
  String json = "{";
  json += "\"CloseSun\":\"" + String(loadCloseSunAngle()) + "\",";
  json += "\"CloseNight\":\"" + String(loadCloseNightAngle()) + "\",";
  json += "\"OpenLow\":\"" + String(loadOpenLowtAngle()) + "\",";
  json += "\"OpenMiddle\":\"" + String(loadOpenMiddleAngle()) + "\",";
  json += "\"OpenHight\":\"" + String(loadOpenHightAngle()) + "\",";
  json += "\"LeftBlindPosition\":\"" + String(loadLeftBlindAngle()) + "\",";
  json += "\"RightBlindPosition\":\"" + String(loadRightBlindAngle()) + "\"";
  json += "}";
  return json;
}

void eeprom_inic()
{
  for (int i = 0; i < 20; i++) {
    EEPROM.write(i,0);
  }
  EEPROM.commit();
  saveCloseSunAngle(5);
  saveCloseNightAngle(175);
  saveOpenLowAngle(15);
  saveOpenMiddleAngle(90);
  saveOpenHightAngle(135);
}
