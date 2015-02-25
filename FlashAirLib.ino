#include "FlashAir.h"
#include <iSdio.h>
#include <utility/Sd2CardExt.h>

#define SERIAL_DEBUG 0

#define CHIP_SELECT_PIN 4 //3
#define LED 13 //4

FlashAir* gFlashAir;
boolean ledOn = LOW;
uint32_t gSeq;

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
  pinMode(LED, OUTPUT);

  gFlashAir = new FlashAir(CHIP_SELECT_PIN);
  gSeq = gFlashAir->connect(gFlashAir->getNextSequenceId(), "kumotori",  "ikeuchiyasuki");
}

void loop() {
  if (gFlashAir->isCommandDone(gSeq)) {
    digitalWrite(LED, HIGH); 
  } else {
    digitalWrite(LED, ledOn);
    ledOn = (ledOn == HIGH) ? LOW : HIGH;
  }
  delay(500);
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
  //gFlashAir->resume();
}

