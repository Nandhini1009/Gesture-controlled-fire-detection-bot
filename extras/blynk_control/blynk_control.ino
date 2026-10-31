/*
  OPTIONAL: Control the robot from the Blynk phone app instead of gestures.
  Board: ESP8266 (NodeMCU)

  Blynk buttons (set each to "push" mode):
    V0 = Forward, V1 = Backward, V2 = Left, V3 = Right

  Fill in your own details below. Do NOT upload real passwords or tokens to GitHub.
*/

/*********** BLYNK DEFINES (DO NOT MOVE THESE) ***********/
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "bot"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"

/*******************************************************/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

/*********** WIFI CREDENTIALS ***********/
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

/*********** MOTOR PINS ***********/
#define IN1 D6   // Left motor
#define IN2 D7
#define IN3 D1   // Right motor
#define IN4 D2

/*********** MOTOR FUNCTIONS ***********/
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

/*********** BLYNK BUTTON CONTROLS ***********/
// Forward
BLYNK_WRITE(V0) {
  if (param.asInt()) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    stopMotors();
  }
}

// Backward
BLYNK_WRITE(V1) {
  if (param.asInt()) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    stopMotors();
  }
}

// Left
BLYNK_WRITE(V2) {
  if (param.asInt()) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    stopMotors();
  }
}

// Right
BLYNK_WRITE(V3) {
  if (param.asInt()) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    stopMotors();
  }
}

/*********** SETUP ***********/
void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

/*********** LOOP ***********/
void loop() {
  Blynk.run();
}
