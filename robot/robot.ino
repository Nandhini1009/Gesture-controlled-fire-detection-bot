/*
  Robot (ESP32)
  - Receives movement commands from the gesture controller using ESP-NOW
  - Drives the motors through the L298N motor driver
  - Checks 3 infrared flame sensors. If fire is seen, the robot stops,
    the buzzer turns on, and movement commands are ignored until the fire is gone.
  - If no command arrives for a short time (controller off or out of range),
    the robot stops by itself.

  Note: this uses the ESP32 Arduino core 3.x ESP-NOW callback format.
*/

#include <esp_now.h>
#include <WiFi.h>

// Motor pins (L298N)
#define MOTOR_A1 33
#define MOTOR_A2 25
#define MOTOR_B1 26
#define MOTOR_B2 27

// Flame sensor pins (these modules give LOW when fire is detected)
#define FIRE_SENSOR_1 32
#define FIRE_SENSOR_2 35
#define FIRE_SENSOR_3 34

// Buzzer pin - CHANGE THIS to the pin you actually wired the buzzer to
#define BUZZER_PIN 14

// Stop the robot if no command is received for this long (milliseconds)
#define COMMAND_TIMEOUT_MS 500

typedef struct struct_message {
  char action[15];
} struct_message;

struct_message receivedData;

volatile bool fireDetected = false;
volatile unsigned long lastCommandTime = 0;

// ---------- Motor Functions ----------
void forward() {
  digitalWrite(MOTOR_A1, HIGH);  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, HIGH);  digitalWrite(MOTOR_B2, LOW);
}

void backward() {
  digitalWrite(MOTOR_A1, LOW);   digitalWrite(MOTOR_A2, HIGH);
  digitalWrite(MOTOR_B1, LOW);   digitalWrite(MOTOR_B2, HIGH);
}

void left() {
  digitalWrite(MOTOR_A1, LOW);   digitalWrite(MOTOR_A2, HIGH);  // left motor reverse
  digitalWrite(MOTOR_B1, HIGH);  digitalWrite(MOTOR_B2, LOW);   // right motor forward
}

void right() {
  digitalWrite(MOTOR_A1, HIGH);  digitalWrite(MOTOR_A2, LOW);   // left motor forward
  digitalWrite(MOTOR_B1, LOW);   digitalWrite(MOTOR_B2, HIGH);  // right motor reverse
}

void stopMotors() {
  digitalWrite(MOTOR_A1, LOW);
  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, LOW);
  digitalWrite(MOTOR_B2, LOW);
}

// ---------- Fire Check ----------
void checkFire() {
  bool f1 = digitalRead(FIRE_SENSOR_1) == LOW;
  bool f2 = digitalRead(FIRE_SENSOR_2) == LOW;
  bool f3 = digitalRead(FIRE_SENSOR_3) == LOW;

  static bool wasFire = false;
  fireDetected = (f1 || f2 || f3);

  if (fireDetected) {
    stopMotors();
    digitalWrite(BUZZER_PIN, HIGH);

    if (!wasFire) {
      Serial.print("FIRE DETECTED! Sensors: ");
      if (f1) Serial.print("1 ");
      if (f2) Serial.print("2 ");
      if (f3) Serial.print("3 ");
      Serial.println();
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    if (wasFire) Serial.println("Fire cleared");
  }

  wasFire = fireDetected;
}

// ---------- ESP-NOW Receive ----------
void onDataReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (len != sizeof(receivedData)) return;

  memcpy(&receivedData, incomingData, sizeof(receivedData));
  receivedData.action[sizeof(receivedData.action) - 1] = '\0';
  lastCommandTime = millis();

  // Never move while fire is detected
  if (fireDetected) {
    stopMotors();
    return;
  }

  if (strcmp(receivedData.action, "FORWARD") == 0) forward();
  else if (strcmp(receivedData.action, "BACKWARD") == 0) backward();
  else if (strcmp(receivedData.action, "LEFT") == 0) left();
  else if (strcmp(receivedData.action, "RIGHT") == 0) right();
  else stopMotors();
}

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(MOTOR_B1, OUTPUT);
  pinMode(MOTOR_B2, OUTPUT);
  stopMotors();

  pinMode(FIRE_SENSOR_1, INPUT);
  pinMode(FIRE_SENSOR_2, INPUT);
  pinMode(FIRE_SENSOR_3, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_recv_cb(onDataReceive);

  Serial.println("Receiver Ready");
}

void loop() {
  checkFire();

  // Safety: stop if the controller has gone quiet
  if (millis() - lastCommandTime > COMMAND_TIMEOUT_MS) {
    stopMotors();
  }

  delay(50);
}
