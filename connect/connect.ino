#include <Arduino.h>
#include "bluetooth.h"
#include <servo.h>

void setup() {
  Serial.begin(115200); // Start serial communication
  initializeBluetooth();
  servoInit();
}

void loop() {
  delay(2000);
}
