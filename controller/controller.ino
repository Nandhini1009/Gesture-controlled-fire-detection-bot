/*
  Gesture Controller (ESP32 + MPU6050)
  Reads hand tilt and sends a movement command to the robot using ESP-NOW.

  Tilt forward  -> FORWARD
  Tilt backward -> BACKWARD
  Tilt left     -> LEFT
  Tilt right    -> RIGHT
  Flat hand     -> STOP
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// MAC address of the ROBOT's ESP32.
// Upload tools/get_mac_address to the robot's ESP32 to find it, then paste it here.
uint8_t receiverAddress[] = {0xCC, 0xDB, 0xA7, 0x96, 0xCD, 0x98};

// MPU6050 I2C pins
#define SDA_PIN 32
#define SCL_PIN 33

// How far you must tilt (in degrees) before the robot moves
#define TILT_THRESHOLD 10.0

// How often a command is sent (milliseconds)
#define SEND_INTERVAL_MS 100

typedef struct struct_message {
  char action[15];
} struct_message;

struct_message sendData;

float smoothPitch = 0;
float smoothRoll = 0;

bool initMPU() {
  mpu.initialize();
  return mpu.testConnection();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  while (!initMPU()) {
    Serial.println("Retrying MPU...");
    delay(1000);
  }

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add robot as peer");
    return;
  }

  Serial.println("Controller Ready");
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Tilt angles from the accelerometer
  float pitch = atan2(ax, sqrt((float)ay * ay + (float)az * az)) * 180 / PI;
  float roll  = atan2(ay, sqrt((float)ax * ax + (float)az * az)) * 180 / PI;

  // Smoothing filter so the robot does not jerk
  smoothPitch = 0.8 * smoothPitch + 0.2 * pitch;
  smoothRoll  = 0.8 * smoothRoll  + 0.2 * roll;

  // 4-direction control
  if (smoothPitch > TILT_THRESHOLD)
    strcpy(sendData.action, "BACKWARD");
  else if (smoothPitch < -TILT_THRESHOLD)
    strcpy(sendData.action, "FORWARD");
  else if (smoothRoll > TILT_THRESHOLD)
    strcpy(sendData.action, "RIGHT");
  else if (smoothRoll < -TILT_THRESHOLD)
    strcpy(sendData.action, "LEFT");
  else
    strcpy(sendData.action, "STOP");

  esp_now_send(receiverAddress, (uint8_t *)&sendData, sizeof(sendData));

  Serial.print("Pitch: ");  Serial.print(smoothPitch);
  Serial.print("  Roll: "); Serial.print(smoothRoll);
  Serial.print("  Action: "); Serial.println(sendData.action);

  delay(SEND_INTERVAL_MS);
}
