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

  gFlashAir = new FlashAir(CHIP_SELECT_PIN);
  gFlashAir->connect("kumotori",  "ikeuchiyasuki");
}

uint8_t buffer[512];

void loop() {
  delay(2000);
  Serial.print(F("isDone :"));
  Serial.println(gFlashAir->isAllCommandDone());
  if (gFlashAir->isAllCommandDone()) {
    Status* status = gFlashAir->getStatus();
    Serial.print("ipAddress: ");
    printIPAddress(status->wifi.ipAddress);
    Serial.print("ssid: ");
    Serial.println(status->wifi.ssid);
    Serial.print("conencted: ");
    Serial.println(status->wifi.connected);
  }
  //gFlashAir->debugCommandResponse();
  gFlashAir->resume();
  memset(buffer, 0, 0x200);
}

