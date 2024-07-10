#include <LoRaLibrary.h>
#define MESSAGELEN 21

void setup() {
  setupLoRa();
  Serial.println("READY");
}

void loop() {
  if (Serial.available() >= MESSAGELEN) { // Esperar até que todos os bytes da mensagem sejam recebidos, incluindo cabeçalho
    uint8_t received_message[MESSAGELEN];
    Serial.readBytes(received_message, MESSAGELEN);
    Serial.println("ACK");

    // Verificar o cabeçalho e a mensagem recebida
    Serial.print("Received message: ");
    for (uint8_t i = 0; i < MESSAGELEN; i++) {
      Serial.print(received_message[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    sendMessage(received_message, MESSAGELEN);
  }

  delay(1000);
}
