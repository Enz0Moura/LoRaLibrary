#include <LoRaLibrary.h>
#define MESSAGELEN 21
#define BEACONLEN 10
#define BUFFERLEN 2
#define ISACK 1

enum State {
    INITIALIZING,
    WAITING_FOR_MESSAGE,
	WAITING_FOR_RECORD,
};

State currentState = INITIALIZING;

void setup() {
    setupLoRa();
    currentState = WAITING_FOR_MESSAGE;
    Serial.println("READY");
}

void loop() {
	Serial.println("READY");
    switch (currentState) {
        case WAITING_FOR_MESSAGE:
            waitForMessage();
            break;
		case WAITING_FOR_RECORD:
			listenForRecord();
			break;
        default:
            currentState = WAITING_FOR_MESSAGE;
            break;
    }
}

void waitForMessage() {
    Serial.println("READY");

	unsigned long start_time = millis();
    while (Serial.available() < MESSAGELEN) {
        if (millis() - start_time > 3000) {
            Serial.println("Timeout waiting for message, restarting...");
            currentState = WAITING_FOR_RECORD;
            return;
        }
    }

    if (Serial.available() == MESSAGELEN) {
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
        currentState = WAITING_FOR_RECORD;
    }
}


void listenForRecord() {
	int response = receiveMessage(0);
	unsigned long start_time = millis();
	while(response == -1 && start_time - millis() < 10000){
		response = receiveMessage(0);
	}
	if (response == 1) {
		currentState = WAITING_FOR_MESSAGE;
	}
}


