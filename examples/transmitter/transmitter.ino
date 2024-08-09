#include <LoRaLibrary.h>
#define MESSAGELEN 21
#define BEACONLEN 10
#define BUFFERLEN 2
#define ISACK 1

enum State {
    IDLE,
    WAITING_FOR_MESSAGE,
    WAITING_FOR_RECORD
};

State currentState = IDLE;

void setup() {
    Serial.begin(9600);
    setupLoRa();
    currentState = IDLE;
}

void loop() {
    // Verifica se há um comando na serial
    if (Serial.available()) {
        char command = Serial.read();
        handleCommand(command);
    }

    // Executa a rotina correspondente ao estado atual
    switch (currentState) {
        case WAITING_FOR_MESSAGE:
            waitForMessage();
            break;
        case WAITING_FOR_RECORD:
            listenForRecord();
            break;
        case IDLE:
        default:
            break;
    }
}

void handleCommand(char command) {
    if (command == 'M') {
        currentState = WAITING_FOR_MESSAGE;
        Serial.println("Estado redefinido para WAITING_FOR_MESSAGE.");
        return;
    } else if (command == 'B') {
        currentState = WAITING_FOR_RECORD;
        Serial.println("Estado alterado para WAITING_FOR_RECORD.");
        return;
    } else {
        Serial.println("Comando desconhecido.");
        return;
    }
}

void waitForMessage() {
    unsigned long start_time = millis();
    Serial.println("READY");

    while (Serial.available() < MESSAGELEN) {
        Serial.println("READY");
        if (millis() - start_time > 10000) {
            Serial.println("Timeout waiting for message, retornando ao estado IDLE...");
            currentState = IDLE;
            return;
        }
    }

    if (Serial.available() >= MESSAGELEN) {
        uint8_t received_message[MESSAGELEN];
        Serial.readBytes(received_message, MESSAGELEN);

        for (uint8_t i = 0; i < MESSAGELEN; i++) {
            Serial.print(received_message[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        sendMessage(received_message, MESSAGELEN);
        currentState = IDLE;
    }
}

void listenForRecord() {
    int response = receiveMessage(0);
    unsigned long start_time = millis();
    while (response == -1 && millis() - start_time < 10000) {
        response = receiveMessage(0);
    }
    if (response == 1) {
        currentState = IDLE;
    }
}
