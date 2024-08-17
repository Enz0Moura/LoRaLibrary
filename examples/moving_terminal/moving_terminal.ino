#include <LoRaLibrary.h>
#define MESSAGELEN 21
#define BEACONLEN 10
#define BUFFERLEN 2
#define ISACK 1

enum State {
    IDLE,
    INITIALIZING,
    WAITING_FOR_MESSAGE,
	WAITING_FOR_RECORD,
    SENDING_BEACON,
    WAITING_ACK,
    WAITING_BEACON
};

State currentState = INITIALIZING;

void setup() {
    setupLoRa();
    currentState = WAITING_FOR_MESSAGE;
    Serial.println("Sistema iniciado. Aguardando comandos.");
}

void loop() {
   if (Serial.available()) {
        char command = Serial.read();
        handleCommand(command);
    }

}

void handleCommand(char command) {
    if (command == 'M') {
        currentState = WAITING_FOR_MESSAGE;
        Serial.println("Estado redefinido para WAITING_FOR_MESSAGE.");
        waitForMessage();
    } else if (command == 'R') {
        currentState = WAITING_FOR_RECORD;
        Serial.println("Estado alterado para WAITING_FOR_RECORD.");
        listenForRecord();
    } else if (command == 'B'){
        currentState = SENDING_BEACON;
        Serial.println("Estado alterado para SENDING_BEACON.");
        sendBeacon();
    } else if (command == 'L'){
        currentState = WAITING_BEACON;
        listenForBeacon();
    }
     else {
        Serial.println("Comando desconhecido.");
		Serial.println(command);
    }
}

void sendBeacon(){
    Serial.println("READY");
    unsigned long start_time = millis();
    while (Serial.available() < BEACONLEN) {
        Serial.println("READY");
        if (millis() - start_time > 10000) {
            Serial.println("Timeout waiting for beacon, restarting...");
            currentState = WAITING_FOR_RECORD;
            return;
        }
    }

    if (Serial.available() == BEACONLEN) {
        uint8_t buffer[MESSAGELEN];
        Serial.readBytes(buffer, BEACONLEN);

        int headerIndex = -1;
        for (int i = 0; i < BEACONLEN; i++) {
            if (buffer[i] == 0xFF && buffer[i + 1] == 0xFF) {
                headerIndex = i;
                break;
            }
        }

        if (headerIndex != -1) {
            uint8_t received_message[BEACONLEN];
            memcpy(received_message, buffer + headerIndex, BEACONLEN);

            // Imprimir a mensagem recebida
            for (uint8_t i = 0; i < BEACONLEN; i++) {
                Serial.print(received_message[i], HEX);
                Serial.print(" ");
            }
            Serial.println();

            sendMessage(received_message, BEACONLEN);
            currentState = WAITING_ACK;
            start_time = millis();
            while (receiveMessage(ISACK) == -1){
                sendMessage(received_message, BEACONLEN);
                if (millis() - start_time > 10000) {
                    Serial.println("Timeout waiting for beacon, restarting...");
                    currentState = IDLE;
                    return;
                }
            }
            Serial.println("ACK Received.");
        } else {
            Serial.println("Header not found or insufficient data after header");
            Serial.println("Timeout waiting for beacon, restarting...");
        }
    }

}

void waitForMessage() {
	Serial.println("READY");
	unsigned long start_time = millis();
    while (Serial.available() < MESSAGELEN) {
        Serial.println("READY");
        if (millis() - start_time > 10000) {
            Serial.println("Timeout waiting for message, restarting...");
            currentState = WAITING_FOR_RECORD;
            return;
        }
    }

    if (Serial.available() == MESSAGELEN) {
        uint8_t buffer[MESSAGELEN];
        Serial.readBytes(buffer, MESSAGELEN);

		int headerIndex = -1;
        for (int i = 0; i < MESSAGELEN; i++) {
            if (buffer[i] == 0xFF && buffer[i + 1] == 0xFF) {
                headerIndex = i;
                break;
            }
        }

        if (headerIndex != -1) {
            uint8_t received_message[MESSAGELEN];
            memcpy(received_message, buffer + headerIndex, MESSAGELEN);

            // Imprimir a mensagem recebida
            for (uint8_t i = 0; i < MESSAGELEN; i++) {
                Serial.print(received_message[i], HEX);
                Serial.print(" ");
            }
            Serial.println();

            sendMessage(received_message, MESSAGELEN);
            currentState = IDLE;
        } else {
            Serial.println("Header not found or insufficient data after header");
        }
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
	if (response == -1){
	    Serial.println("Timeout waiting for record, restarting...");
	}
}

void listenForBeacon() {
    int response = receiveMessage(0);
    unsigned long start_time = millis();
    while(response == -1 && start_time - millis() < 10000){
        response = receiveMessage(0);
    }
    if (response == 1) {
        currentState = WAITING_FOR_MESSAGE;
        Serial.println("Beacon Received");
        byte ack[BUFFERLEN] = {0xFF, 0xFF};
        sendMessage(ack, BUFFERLEN);
    }
    else{
        Serial.println("Beacon listening timeout");
      }
}
