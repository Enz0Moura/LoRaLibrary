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

    int response = sendBeacon(received_message, BEACONLEN);

    if (response == 1){
		Serial.println("ACK");
	  }
	else if (response == -1){
    Serial.println("No confirmation received");
	}
	}
  else if (Serial.available() == BUFFERLEN){
    Serial.println("ACK");
    receiveMessage(ISACK);
    delay(250);
  }


}
