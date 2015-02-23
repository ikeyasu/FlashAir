#include "FlashAir.h"
#include <iSdio.h>
#include <utility/Sd2CardExt.h>

#define SERIAL_DEBUG 0

int CHIP_SELECT_PIN = 4;

FlashAir* gFlashAir;

#if SERIAL_DEBUG
void printIPAddress(uint8_t ip[4]) {
  Serial.print(ip[0], DEC);
  Serial.print('.');
  Serial.print(ip[1], DEC);
  Serial.print('.');
  Serial.print(ip[2], DEC);
  Serial.print('.');
  Serial.print(ip[3], DEC);
}
#endif

void setup() {
#if SERIAL_DEBUG
  // Initialize UART for message print.
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
#endif

  gFlashAir = new FlashAir(CHIP_SELECT_PIN);
  gFlashAir->connect("kumotori",  "ikeuchiyasuki");
}

uint8_t buffer[512];

void loop() {
  delay(2000);
#if SERIAL_DEBUG
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
#endif
  //gFlashAir->debugCommandResponse();
  gFlashAir->resume();
  memset(buffer, 0, 0x200);
}

