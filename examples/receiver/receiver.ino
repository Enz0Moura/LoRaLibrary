#include <LoRaLibrary.h>
#define BEACONLEN 10
#define BUFFERLEN 2
#define ISACK 0

void setup() {
  setupLoRa();
  Serial.println("Setup completed");
}

void loop() {
   Serial.println("READY");
   if (Serial.available() == BEACONLEN) { // Esperar até que todos os bytes da mensagem sejam recebidos, incluindo cabeçalho
    uint8_t received_message[BEACONLEN];
    Serial.readBytes(received_message, BEACONLEN);

	Serial.print("Received message: ");
    for (uint8_t i = 0; i < BEACONLEN; i++) {
      Serial.print(received_message[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    int response = sendBeacon(received_message, BEACONLEN);

    if (response == 1){
		Serial.println("ACK");
	  }
    Serial.println("No confirmation received");

	}
  else if (Serial.available() == BUFFERLEN){
    receiveMessage(ISACK);
    delay(250);
  }


}
