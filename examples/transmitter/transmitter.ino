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
  Serial.println("READY");

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
  else { // Esperar até que todos os bytes da mensagem sejam recebidos, incluindo cabeçalho
	unsigned long start_time = millis();
	int beacon_response;
	while (millis() - start_time < 3000) {
    	beacon_response = receiveMessage(1);
		if (beacon_response == 1){
			Serial.println("Beacon Received");
			break;
		};
	};

	if (beacon_response == -1){
	start_time = millis();
	Serial.println("Sending Beacon");
    if (Serial.available() == BEACONLEN){
	    uint8_t received_message[BEACONLEN];
	    Serial.readBytes(received_message, BEACONLEN);

	    int ack = sendBeacon(received_message, BEACONLEN);

	    if (ack == 1){
			start_time = millis();
			Serial.println("ACK");
			while (millis - start_time < 5000){
				int received_message = receiveMessage(0);
				if (received_message == 1) {
						break;
					}
			}

	    	delay(250);
		  }
		else {
    		Serial.println("No confirmation received");
		}
	}
	}
	}
	//TODO: Lógica de recebimento de mensagem está zuada para caramba

  delay(1000);
}
