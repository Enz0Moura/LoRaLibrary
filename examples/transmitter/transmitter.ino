#include <LoRaLibrary.h>
#define MESSAGELEN 21
#define BEACONLEN 10
#define BUFFERLEN 2
#define ISACK 1

enum State {
    INITIALIZING,
    WAITING_FOR_MESSAGE,
    SENDING_BEACON,
    WAITING_FOR_BEACON,
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
        case SENDING_BEACON:
            sendBeaconMessage();
            break;
        case WAITING_FOR_BEACON:
            waitForBeacon();
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
            currentState = WAITING_FOR_BEACON;
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
        currentState = WAITING_FOR_MESSAGE;
    } else {
        start_time = millis();
        currentState = WAITING_FOR_BEACON;
    }
}

void waitForBeacon() {
	unsigned long start_time = millis();
    int beacon_response = -1;
    while (millis() - start_time < 3000) {
        beacon_response = receiveMessage(1);
        if (beacon_response == 1) {
            Serial.println("Beacon Received");
            currentState = WAITING_FOR_MESSAGE;
            return;
        }
    }

    if (beacon_response == -1) {
        start_time = millis();
        Serial.println("Sending Beacon");
        currentState = SENDING_BEACON;
    }
}

void sendAck() {
    uint8_t ackMessage[2] = {0xFF, 0xFF};
    sendMessage(ackMessage, 2);
}

void listenForRecord() {
	int response = receiveMessage(0);
	unsigned long start_time = millis();
	while(response == -1 && start_time - millis() < 10000){
		response = receiveMessage(0);
	}
	if (response == 1) {
		sendAck();
		currentState = WAITING_FOR_MESSAGE;
	}
}


void sendBeaconMessage() {
	unsigned long start_time = millis();
    while (Serial.available() < BEACONLEN) {
        if (millis() - start_time > 3000) {
            Serial.println("Timeout waiting for beacon, restarting...");
            currentState = WAITING_FOR_BEACON;
            return;
        }
    }

    if (Serial.available() == BEACONLEN) {
        uint8_t received_message[BEACONLEN];
        Serial.readBytes(received_message, BEACONLEN);

        int ack = sendBeacon(received_message, BEACONLEN);
		if (ack == 1) {
            unsigned long start_time = millis();
			while (millis - start_time < 10000){
				int received_message = receiveMessage(0);
				if (received_message == 1) {
						currentState = WAITING_FOR_MESSAGE;
						return;
					}
			}
            currentState = WAITING_FOR_MESSAGE;
        } else {
            Serial.println("No confirmation received");
            currentState = SENDING_BEACON;
        }

    }
}
