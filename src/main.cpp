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

struct Light {
  char name[17];
  unsigned int on_code;
  unsigned int off_code;
  boolean action;
  boolean power;
};

Light light1 = {"Light1", LIGHT_1_ON, LIGHT_1_OFF, false, false};
Light light2 = {"Light2", LIGHT_2_ON, LIGHT_2_OFF, false, false};
Light light3 = {"Light3", LIGHT_3_ON, LIGHT_3_OFF, false, false};
Light light4 = {"Light4", LIGHT_4_ON, LIGHT_4_OFF, false, false};
Light light5 = {"Light5", LIGHT_5_ON, LIGHT_5_OFF, false, false};
Light light6 = {"Light6", LIGHT_6_ON, LIGHT_6_OFF, false, false};
Light light7 = {"AllBedroomLights", ALL_LIGHTS_BEDROOM_ON, ALL_LIGHTS_BEDROOM_OFF, false, false};

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

// Replaces placeholder with LED state value
String processorSetBlinds(const String& var){
  if(var == "vCloseSun"){
    String value = (String)loadCloseSunAngle();
    return value;
  }
  if(var == "vCloseNight"){
    String value = (String)loadCloseNightAngle();
    return value;
  }
  if(var == "vOpenLow"){
    String value = (String)loadOpenLowtAngle();
    return value;
  }
  if(var == "OpenMiddle"){
    String value = (String)loadOpenMiddleAngle();
    return value;
  }
  if(var == "vOpenHight"){
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
        if (pName == (String)lights[i].name){
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

  server.on("/eeprom/default/set", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Inic EEPROM was succesfull");
    eeprom_inic();
  });

  server.begin();
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