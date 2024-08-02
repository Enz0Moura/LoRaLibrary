#include <LoRaLibrary.h>
#define MESSAGELEN 21
#define BEACONLEN 10
#define BUFFERLEN 2
#define ISACK 1
void setup() {
  setupLoRa();
  Serial.println("READY");
}

void loop() {
  int response = -1;
  while (response == -1){
    response = receiveMessage(ISACK);
  }
  if (Serial.available() == MESSAGELEN) { // Esperar até que todos os bytes da mensagem sejam recebidos, incluindo cabeçalho
    uint8_t received_message[MESSAGELEN];
    Serial.readBytes(received_message, MESSAGELEN);
    Serial.println("ACK");

    // Verificar o cabeçalho e a mensagem recebida
    for (uint8_t i = 0; i < MESSAGELEN; i++) {
      Serial.print(received_message[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    sendMessage(received_message, MESSAGELEN);
  }
  else if (Serial.available() == BEACONLEN) { // Esperar até que todos os bytes da mensagem sejam recebidos, incluindo cabeçalho
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
    receiveMessage(0);
    delay(250);



  delay(1000);
}
