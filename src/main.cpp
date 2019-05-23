#include <Arduino.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <RCSwitch.h>
#include <EEPROM.h>
#include <configuration.h>

#include <eeprom_functions.h>
#include <servo_functions.h>

#define BUTTON_LEFT_UP_PIN 1
#define BUTTON_LEFT_DOWN_PIN 2
#define BUTTON_RIGHT_UP_PIN 3
#define BUTTON_RIGHT_DOWN_PIN 4
#define BUTTON_DEFAULT_PIN 5

struct Light {
  char name[20];
  char fauxmo_name[20];
  unsigned int on_code;
  unsigned int off_code;
  boolean action;
  boolean power;
};

Light light1 = {"Light1", "light one", LIGHT_1_ON, LIGHT_1_OFF, false, false};
Light light2 = {"Light2", "light two", LIGHT_2_ON, LIGHT_2_OFF, false, false};
Light light3 = {"Light3", "light three", LIGHT_3_ON, LIGHT_3_OFF, false, false};
Light light4 = {"Light4", "light four", LIGHT_4_ON, LIGHT_4_OFF, false, false};
Light light5 = {"Light5", "light five", LIGHT_5_ON, LIGHT_5_OFF, false, false};
Light light6 = {"Light6", "light six", LIGHT_6_ON, LIGHT_6_OFF, false, false};
Light light7 = {"AllBedroomLights", "bedroom", ALL_LIGHTS_BEDROOM_ON, ALL_LIGHTS_BEDROOM_OFF, false, false};

Light lights[] = {light1, light2, light3, light4, light5, light6, light7};
int sizeArrayLights = sizeof(lights)/sizeof(Light);

AsyncWebServer server(80);
RCSwitch radio433 = RCSwitch();

#define delayTimeBetweenSendRC 200

void startWiFi()
{
  Serial.println("Start connecting to wifi network");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to Wifi was successful");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void checkWiFiConnect()
{
  if (WiFi.status() != WL_CONNECTED) {
    startWiFi();
  }
}

void startRadio433()
{
  radio433.enableTransmit(radio433Pin);
  radio433.setPulseLength(321);
  radio433.setProtocol(1);
  radio433.setRepeatTransmit(15);
}

void startEEPROM()
{
  EEPROM.begin(20);
}

void startSPIFFS()
{
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}

void handleDeviceActions() {
  //LIGHTS
  for (int i=0; i<sizeArrayLights; i++){
    if (lights[i].action) {
      if (lights[i].power) {
        radio433.send(lights[i].on_code, 24);
        delay(delayTimeBetweenSendRC);
        Serial.println("Radio send code:"+(String)lights[i].on_code+" on device "+lights[i].name);
      } else {
        radio433.send(lights[i].off_code, 24);
        delay(delayTimeBetweenSendRC);
        Serial.println("Radio send code:"+(String)lights[i].off_code+" on device "+lights[i].name);
      }
      lights[i].action = false;
      break;
    }
  }
  //SERVO POWER OFF
  if (isRunLeftServo) {
    if (millis() >= timeEndLeftServo) {
      stopLeftServo();
    }
  }
  if (isRunRightServo) {
    if (millis() >= timeEndRightServo) {
      stopRightServo();
    }
  }
  if (isRunLeftServo == false && isRunRightServo == false && isOpenTransistor == true) {
    closeTransistor();
  }
}

//BLIND CONTROLER
void blindControler(String blind, byte value, String action) {
  if (action == "m") {
    if (blind == "left") {
      moveLeftBlind(value, true);
    }
    if (blind == "right") {
      moveRightBlind(value, true);
    }
    if (blind == "all") {
      moveAllBlinds(value, true);
    }
  }
  if (action == "s_cs") {
    saveCloseSunAngle(value);
  }
  if (action == "s_cn") {
    saveCloseNightAngle(value);
  }
  if (action == "s_ol") {
    saveOpenLowAngle(value);
  }
  if (action == "s_om") {
    saveOpenMiddleAngle(value);
  }
  if (action == "s_oh") {
    saveOpenHightAngle(value);
  }
}

// Replaces placeholder with LED state value
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
    request->send(SPIFFS, "/index.html", String());
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
        if (pValue == "CloseNight") {
          moveRightBlind(loadCloseNightAngle(), true);
        }
        if (pValue == "OpenLow") {
          moveRightBlind(loadOpenLowtAngle(), true);
        }
        if (pValue == "OpenMiddle") {
          moveRightBlind(loadOpenMiddleAngle(), true);
        }
        if (pValue == "OpenHight") {
          moveRightBlind(loadOpenHightAngle(), true);
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

void handleButtons() {
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

/* ------------------------------------------------------------------ */
void setup() {
  Serial.begin(115200);
  startEEPROM();
  startSPIFFS();
  startWiFi();
  startRadio433();
  startWebServer();
}

void loop() {
  checkWiFiConnect();
  handleDeviceActions();
}
