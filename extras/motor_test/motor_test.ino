/*
  OPTIONAL: Simple motor wiring test (ESP8266 NodeMCU).
  Runs Forward, Backward, Left, Right in a loop so you can check the motor directions.
*/

#define IN1 D6   // Left motor
#define IN2 D7
#define IN3 D1   // Right motor
#define IN4 D2

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {

  // FORWARD
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(2000);
  stopMotors();
  delay(1000);

  // BACKWARD
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(2000);
  stopMotors();
  delay(1000);

  // LEFT TURN (left motor backward, right motor forward)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(1500);
  stopMotors();
  delay(1000);

  // RIGHT TURN (left motor forward, right motor backward)
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(1500);
  stopMotors();
  delay(2000);
}
