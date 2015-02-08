#include "FlashAir.h"
#include <iSdio.h>
#include <utility/Sd2CardExt.h>

int CHIP_SELECT_PIN = 4;

FlashAir* gFlashAir;

void printIPAddress(uint8_t ip[4]) {
  Serial.print(ip[0], DEC);
  Serial.print('.');
  Serial.print(ip[1], DEC);
  Serial.print('.');
  Serial.print(ip[2], DEC);
  Serial.print('.');
  Serial.print(ip[3], DEC);
}

void setup() {
  // Initialize UART for message print.
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Initialize SD card.
  Serial.print(F("\nInitializing SD card..."));  
  gFlashAir = new FlashAir(CHIP_SELECT_PIN);
  //Status* status = gFlashAir->getStatus();
  //printIPAddress(status->wifi.ipAddress);
  Serial.print(F("nextSequenceID: "));
  Serial.println(gFlashAir->getNextSequenceId(), DEC);
  Serial.print(F("nextSequenceID: "));
  Serial.println(gFlashAir->getNextSequenceId(), DEC);
  gFlashAir->debugCommandResponse();
  gFlashAir->connect(gFlashAir->getNextSequenceId(), "kumotori",  "ikeuchiyasuki");
  //gFlashAir->debugCommandResponse();
}

void loop() {
  delay(2000);
  gFlashAir->debugCommandResponse();
}

