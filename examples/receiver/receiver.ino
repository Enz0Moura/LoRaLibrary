#include <LoRaLibrary.h>
#define BEACONLEN 10

void setup() {
  setupLoRa();
  Serial.println("Setup completed");
}

void loop() {
   if (Serial.available() == BEACONLEN) { // Esperar até que todos os bytes da mensagem sejam recebidos, incluindo cabeçalho
    uint8_t received_message[MESSAGELEN];
    Serial.readBytes(received_message, MESSAGELEN);
    Serial.println("ACK");

	Serial.print("Received message: ");
    for (uint8_t i = 0; i < MESSAGELEN; i++) {
      Serial.print(received_message[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    int response = sendBeacon(received_message, MESSAGELEN);

if (response == 1){
		Serial.println("ACK");
	}

	}

  receiveMessage();
  delay(250);
}
