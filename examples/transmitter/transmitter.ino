#include <LoRaLibrary.h>

void setup() {
  setupLoRa();
  Serial.println("READY");
}

void loop() {
  if (Serial.available() >= 17) { // Esperar até que todos os bytes da mensagem sejam recebidos, incluindo cabeçalho
    uint8_t received_message[17];
    Serial.readBytes(received_message, 17);
    Serial.println("ACK");

    // Verificar o cabeçalho e a mensagem recebida
    Serial.print("Received message: ");
    for (uint8_t i = 0; i < 17; i++) {
      Serial.print(received_message[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    sendMessage(received_message, 17);
  }

  delay(1000);
}
