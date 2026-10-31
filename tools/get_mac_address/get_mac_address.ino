/*
  Prints the ESP32's MAC address in the Serial Monitor (115200 baud).
  Upload this to the ROBOT's ESP32, then copy the address into
  receiverAddress[] in controller/controller.ino.
*/

#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Initializing Wi-Fi to get MAC address...");

  WiFi.mode(WIFI_STA);
  delay(100);

  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {}
