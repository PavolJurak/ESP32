
void startSPIFFS()
{
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}


String processorSetBlinds(const String& var)
{
    if (var == "vLeftBlindPosition") {
      String value = (String)loadLeftBlindAngle();
      return value;
    }
    if (var == "vRightBlindPosition") {
      String value = (String)loadRightBlindAngle();
      return value;
    }
    if(var == "vCloseSun") {
      String value = (String)loadCloseSunAngle();
      return value;
    }
    if(var == "vCloseNight") {
      String value = (String)loadCloseNightAngle();
      return value;
    }
    if(var == "vOpenLow") {
      String value = (String)loadOpenLowtAngle();
      return value;
    }
    if(var == "vOpenMiddle") {
      String value = (String)loadOpenMiddleAngle();
      return value;
    }
    if(var == "vOpenHight") {
      String value = (String)loadOpenHightAngle();
      return value;
    }
    return String();
}

void startWebServer()
{

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processorSetBlinds);
  });

  server.on("/custom.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/custom.css", "text/css");
  });

  server.on("/myscripts.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/myscripts.js", "application/javascript");
  });

  server.on("/setlwallswitch", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/setWallSwitch.html", String());
  });

  server.on("/setblinds", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/setBlinds.html", String(), false, processorSetBlinds);
  });

  server.on("/control/light", HTTP_GET, [](AsyncWebServerRequest *request){
    for (int i=0; i<request->params(); i++) {
      AsyncWebParameter* p = request->getParam(i);
      String pName = p->name();
      String pValue = p->value();
      for(int i=0; i<sizeArrayLights; i++) {
        if (pName == (String)lights[i].name) {
          if (pValue == "ON") {
            lights[i].action = true;
            lights[i].power = true;
            String messsage = "Light name:" + (String)lights[i].name + " ON";
            Serial.println(messsage);
          }
          if (pValue == "OFF") {
            lights[i].action = true;
            lights[i].power = false;
            String messsage = "Light name:" + (String)lights[i].name + " OFF";
            Serial.println(messsage);
          }
        }
      }
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/control/blind", HTTP_GET, [](AsyncWebServerRequest *request){
    for (int i=0; i<request->params(); i++) {
      AsyncWebParameter* p = request->getParam(i);
      String pName = p->name();
      String pValue = p->value();

      if (pName == "LeftBlind") {
        if (pValue == "CloseSun") {
          moveLeftBlind(loadCloseSunAngle(), true);
        }
        if (pValue == "CloseNight") {
          moveLeftBlind(loadCloseNightAngle(), true);
        }
        if (pValue == "OpenLow") {
          moveLeftBlind(loadOpenLowtAngle(), true);
        }
        if (pValue == "OpenMiddle") {
          moveLeftBlind(loadOpenMiddleAngle(), true);
        }
        if (pValue == "OpenHight") {
          moveLeftBlind(loadOpenHightAngle(), true);
        }
      }
      if (pName == "RightBlind") {
        if (pValue == "CloseSun") {
          moveRightBlind(loadCloseSunAngle(), true);
        }
        else if (pValue == "CloseNight") {
          moveRightBlind(loadCloseNightAngle(), true);
        }
        else if (pValue == "OpenLow") {
          moveRightBlind(loadOpenLowtAngle(), true);
        }
        else if (pValue == "OpenMiddle") {
          moveRightBlind(loadOpenMiddleAngle(), true);
        }
        else if (pValue == "OpenHight") {
          moveRightBlind(loadOpenHightAngle(), true);
        }
        else if (pValue.toInt() >= MIN_POSITION && pValue.toInt() <= MAX_POSITION) {
          int value = pValue.toInt();
          moveRightBlind(value, true);
        }
      }
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/control/calibration", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("blind") && request->hasParam("value") && request->hasParam("action")) {
      AsyncWebParameter* pBlind = request->getParam("blind");
      AsyncWebParameter* pValue = request->getParam("value");
      AsyncWebParameter* pAction = request->getParam("action");

      blindControler(pBlind->value(), byte(pValue->value().toInt()), pAction->value());
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/control/radio", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("code")) {
      AsyncWebParameter* p = request->getParam("code");
      String radio_code = p->value();
      if (radio_code.length() <= 5) {
        radio433.send(radio_code.toInt(), 24);
      };
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/eeprom/values", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getJsonEepromValues());
  });

  server.on("/eeprom/default/set", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Inic EEPROM was succesfull");
    eeprom_inic();
  });

  server.begin();
}
