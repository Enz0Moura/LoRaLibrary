#include <LoRaLibrary.h>

void setup() {
  setupLoRa();
  Serial.println("Setup completed");
}

void loop() {
  receiveMessage();
  delay(250);
}
