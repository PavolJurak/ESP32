#include <Arduino.h>
#include <RCSwitch.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <Servo.h>
#include "std_def.h"

AsyncWebServer server(80);
RCSwitch radio433 = RCSwitch();

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

void startRadio433()
{
  radio433.enableTransmit(radio433Pin);
  radio433.setPulseLength(321);
  radio433.setProtocol(1);
  radio433.setRepeatTransmit(15);
}

#include "eeprom_my_library.h"
#include "wifi_my_library.h"
#include "servo_my_library.h"
#include "button_my_library.h"
#include "server_my_library.h"


void handleDeviceActions() {
  //LIGHTS
  for (int i=0; i<sizeArrayLights; i++){
    if (lights[i].action) {
      if (lights[i].power) {
        radio433.send(lights[i].on_code, 24);
        delay(DELAY_BETWEEN_SEND_RC);
        Serial.println("Radio send code:"+(String)lights[i].on_code+" on device "+lights[i].name);
      } else {
        radio433.send(lights[i].off_code, 24);
        delay(DELAY_BETWEEN_SEND_RC);
        Serial.println("Radio send code:"+(String)lights[i].off_code+" on device "+lights[i].name);
      }
      lights[i].action = false;
      break;
    }
  }
  hanleServoPowerOff();
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
