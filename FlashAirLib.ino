#include "FlashAir.h"

#define SERIAL_DEBUG
//#define SERIAL_DEBUG_VERBOSE 
#define CHIP_SELECT_PIN 4 /*3*/
#define LED 13 /*4*/

FlashAir* gFlashAir;
boolean ledOn = LOW;
uint32_t gSeq;

enum {
  STEP_START = 0,
  STEP_CONNECT = 1,
  STEP_REQUEST = 2,
  STEP_RESPONSE = 3,
  STEP_DISPLAY = 4,
  STEP_FIN = 5
} gStep;

#ifdef SERIAL_DEBUG
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
#ifdef SERIAL_DEBUG
  // Initialize UART for message print.
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
#endif
  pinMode(LED, OUTPUT);

  gFlashAir = new FlashAir(CHIP_SELECT_PIN);
  //gFlashAir->disconnect(gFlashAir->getNextSequenceId());

  gStep = STEP_CONNECT;
}

void loop() {
  const char* response;
  uint32_t len, i;
  switch (gStep) {
    case STEP_CONNECT:
      gSeq = gFlashAir->getNextSequenceId();
      //gFlashAir->connect(gSeq, "kumotori",  "ikeuchiyasuki");
      gFlashAir->connect(gSeq, "urayasu",  "socialap");
      gStep = STEP_REQUEST;
      break;
    case STEP_REQUEST:
      if (!gFlashAir->isCommandDone(gSeq)) {
        break;
      }
      gSeq = gFlashAir->getNextSequenceId();
      gFlashAir->requestHTTP(gSeq, true, "s3.amazonaws.com",  "/ikeyasu-pub/test.json");
      gStep = STEP_RESPONSE;
      break;
    case STEP_RESPONSE:
      if (!gFlashAir->isCommandDone(gSeq)) {
        break;
      }
      gStep = STEP_DISPLAY;
      response = gFlashAir->getHTTPResponse(&len);
      break;
    case STEP_DISPLAY:
      gStep = STEP_FIN;
    default:
      break;
  }
  if (gFlashAir->isCommandDone(gSeq)) {
    digitalWrite(LED, HIGH); 
  } else {
    digitalWrite(LED, ledOn);
    ledOn = (ledOn == HIGH) ? LOW : HIGH;
  }
#ifdef SERIAL_DEBUG
#ifdef SERIAL_DEBUG_VERBOSE
  Serial.print(F("gStep="));
  Serial.print(gStep);
  Serial.print(F(" gSeq="));
  Serial.print(gSeq);
  Serial.print(F(" isDone="));
  Serial.println(gFlashAir->isAllCommandDone());
  Status* status = gFlashAir->getStatus();
  Serial.print("ipAddress=");
  printIPAddress(status->wifi.ipAddress);
  Serial.print(" ssid=");
  Serial.println(status->wifi.ssid);
  Serial.print("conencted=");
  Serial.print(status->wifi.connected);
  if (gStep == STEP_DISPLAY) {
    Serial.print(F(" len="));
    Serial.print(len);
    Serial.println(F(""));
    for (i = 0; i < len; i++) {
      Serial.print(response[i]);
    }
  } else {
    Serial.println(F(""));
  }
  gFlashAir->debugCommandResponse();
#else

  switch (gStep) {
    case STEP_DISPLAY:
      Serial.println(F(""));
      Serial.print(F("len="));
      Serial.println(len);
      for (i = 0; i < len; i++) {
        Serial.print(response[i]);
      }
      break;
    default:
      Serial.print(gStep);
      Serial.print(':');
      Serial.print(gSeq);
      Serial.print('.');
      break;
  }
#endif
#endif
  delay(500);
  gFlashAir->resume();

}

