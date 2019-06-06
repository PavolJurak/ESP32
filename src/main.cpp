#include <Arduino.h>
#include <RCSwitch.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Servo.h>
#include "fauxmoESP.h"
#include "std_def.h"

AsyncWebServer server(8088);
fauxmoESP fauxmo;
RCSwitch radio433 = RCSwitch();

struct Blind {
  char name[20];
};

Blind blind1 = {"Left blind"};
Blind blind2 = {"Right blind"};
Blind blind3 = {"Blinds"};

Blind blinds[] = {blind1, blind2, blind3};
int sizeArrayBlinds = sizeof(blinds)/sizeof(Blind);

struct Light {
  char name[20];
  unsigned int on_code;
  unsigned int off_code;
  boolean action;
  boolean power;
};

Light light1 = {"Light one", LIGHT_1_ON, LIGHT_1_OFF, false, false};
Light light2 = {"Light two", LIGHT_2_ON, LIGHT_2_OFF, false, false};
Light light3 = {"Light three", LIGHT_3_ON, LIGHT_3_OFF, false, false};
Light light4 = {"Light four", LIGHT_4_ON, LIGHT_4_OFF, false, false};
Light light5 = {"Light five", LIGHT_5_ON, LIGHT_5_OFF, false, false};
Light light6 = {"Light six", LIGHT_6_ON, LIGHT_6_OFF, false, false};
Light light7 = {"Bedroom", ALL_LIGHTS_BEDROOM_ON, ALL_LIGHTS_BEDROOM_OFF, false, false};

Light lights[] = {light1, light2, light3, light4, light5, light6, light7};
int sizeArrayLights = sizeof(lights)/sizeof(Light);

void startRadio433()
{
  radio433.enableTransmit(radio433Pin);
  radio433.setPulseLength(321);
  radio433.setProtocol(1);
  radio433.setRepeatTransmit(15);
}

void startMDNS()
{
  if (!MDNS.begin("esp32")) {
        Serial.println("Error setting up MDNS responder!");
        while(1){
            delay(1000);
        }
    }
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

void startFauxmo()
{
  //add lights devices to fauxmo
  for (int i=0; i<sizeArrayLights; i++) {
    fauxmo.addDevice(lights[i].name);
  }
  //add blinds devices to fauxmo
  for (int i=0; i<sizeArrayBlinds; i++) {
    fauxmo.addDevice(blinds[i].name);
  }

  fauxmo.setPort(80);
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {

        // Callback when a command from Alexa is received.
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.

        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        //handle lights
        for (int i=0; i<sizeArrayLights; i++) {
          if (strcmp(device_name, lights[i].name) == 0) {
            if (state) {
              radio433.send(lights[i].on_code,24);
              fauxmo.setState(device_id, true, 0);
            } else {
              radio433.send(lights[i].off_code,24);
              fauxmo.setState(device_id, false, 0);
            }
          }
        }

        //LEFT BLIND
        if (device_name == blind1.name) {
          if (state) {
            moveLeftBlind(loadOpenMiddleAngle(), true);
          } else {
            moveLeftBlind(loadCloseSunAngle(), true);
          }
        }

        //RIGHT BLIND
        if (device_name == blind2.name) {
          if (state) {
            moveRightBlind(loadOpenMiddleAngle(), true);
          } else {
            moveRightBlind(loadCloseSunAngle(), true);
          }
        }

        //ALL BLINDS
        if (device_name == blind3.name) {
          if (state) {
            moveAllBlinds(loadOpenMiddleAngle(), true);
          } else {
            moveAllBlinds(loadCloseSunAngle(), true);
          }
        }

        //fauxmo.setState(device_id, true, 255);
        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

    });

}

/* ------------------------------------------------------------------ */
void setup() {
  Serial.begin(115200);
  startEEPROM();
  startSPIFFS();
  startWiFi();
  startRadio433();
  startWebServer();
  startFauxmo();
  startMDNS();
}

void loop() {
  checkWiFiConnect();
  handleDeviceActions();
  fauxmo.handle();
}
