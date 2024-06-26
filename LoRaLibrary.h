#ifndef LORALIBRARY_H
#define LORALIBRARY_H

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

extern RH_RF95 rf95;

void setupLoRa();
void sendMessage(uint8_t* data, uint8_t length);
void receiveMessage();

#endif
