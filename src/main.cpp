#include <Arduino.h>

#define LED_PIN 8

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32-C3!");
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
