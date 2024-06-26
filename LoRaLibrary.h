#ifndef LORALIBRARY_H
#define LORALIBRARY_H

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

extern RH_RF95 rf95;

struct Message {
  unsigned int message_type: 1;
  unsigned int id: 15;
  unsigned long latitude: 24;
  unsigned long longitude: 24;
  unsigned int group_flag: 1;
  unsigned int record_time: 16;
  unsigned int max_records: 11;
  unsigned int hop_count: 4;
  unsigned int channel: 2;
  unsigned int location_time: 16;
  unsigned int help_flag: 2;
  unsigned int battery: 4;
};

void setupLoRa();
void sendMessage(uint8_t* data, uint8_t length);
void receiveMessage();

#endif
