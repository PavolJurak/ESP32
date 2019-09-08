#ifndef USER_DEF
#define USER_DEF

#include "wifi_config.h"

// SERVO CONFIGURATION
#define MIN_POSITION 5
#define MAX_POSITION 175

#define LEFT_SERVO_PIN 16
#define RIGHT_SERVO_PIN 17

#define MAX_TIME_RUN_SERVO 1500 //ms

//BUTTONS PINS
#define BUTTON_LEFT_UP_PIN 1
#define BUTTON_LEFT_DOWN_PIN 2
#define BUTTON_RIGHT_UP_PIN 3
#define BUTTON_RIGHT_DOWN_PIN 4
#define BUTTON_DEFAULT_PIN 5

//TRANSISTOR configuration
#define TRANSISTOR_PIN 21

// Radio codes for controling lights
#define LIGHT_1_ON 5393
#define LIGHT_1_OFF 5293
#define LIGHT_2_ON 5193
#define LIGHT_2_OFF 5093
#define LIGHT_3_ON 4993
#define LIGHT_3_OFF 4893
#define LIGHT_4_ON 4793
#define LIGHT_4_OFF 4693
#define LIGHT_5_ON 4593
#define LIGHT_5_OFF 4493
#define LIGHT_6_ON 4193
#define LIGHT_6_OFF 4093
#define ALL_LIGHTS_BEDROOM_ON 5593
#define ALL_LIGHTS_BEDROOM_OFF 5493

// Radio pin on ESP32
#define radio433Pin 7
#define DELAY_BETWEEN_SEND_RC 200
#endif
