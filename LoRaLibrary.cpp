#include "LoRaLibrary.h"

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setupLoRa() {
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);
    Serial.begin(9600);

    while (!Serial);
    delay(100);

    Serial.println("Initializing LoRa...");

    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    if (!rf95.init()) {
        Serial.println("LoRa radio init failed");
        while (1);
    } else {
        Serial.println("LoRa radio init successful");
    }

    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed");
        while (1);
    } else {
        Serial.println("setFrequency successful");
    }

    rf95.setTxPower(14, false);
    rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128); //using sf11 config
    Serial.println("LoRa setup completed");
}

void sendMessage(uint8_t *data, uint8_t length) {
    Serial.print("Attempting to send message of length: ");
    Serial.println(length);
    Serial.print("Message content: ");
    for (uint8_t i = 0; i < length; i++) {
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    if (data[0] == 0xFF && data[1] == 0xFF) {
        Serial.println("Header verified");

        // Enviar a mensagem via LoRa, incluindo o cabeçalho
        rf95.send(data, length);
        rf95.waitPacketSent();
        Serial.println("Message sent successfully.");

        Serial.print("Sent message via LoRa: ");
        for (uint8_t i = 0; i < length; i++) {
            Serial.print(data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    } else {
        Serial.println("Incorrect Header");
    }
}

int sendBeacon(uint8_t *data, uint8_t length) {
    unsigned long start_time = millis();
    int response = -1;

    while (millis() - start_time < 5000) {
        sendMessage(data, length);
        response = receiveMessage(1);
        if (response != -1) {
            Serial.println("Beacon Received");
            break;
        }

    }
    if (response != -1){
        Serial.println("No beacon available");
    }
    return response;
};

int receiveMessage(bool is_ack) {
    if (rf95.available()) {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len)) {
            for (uint8_t i = 0; i < len; i++) {
                Serial.print(buf[i], HEX);
                Serial.print(" ");
            }
            Serial.println();

            if (buf[0] == 0xFF && buf[1] == 0xFF) {
                Serial.println("Header verified");

                // Transmite os dados recebidos de volta para o Python para desserialização
                if (is_ack == 0) {
                    Serial.write(buf, len);
                }
                return 1;
            }
            Serial.println("Incorrect Header");
            return -1;
        }
        Serial.println("Reception failed");
        return -1;
    }
    return -1;

};
