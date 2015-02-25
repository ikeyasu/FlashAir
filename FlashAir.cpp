// Copyright (c) 2015 IKEUCHI Yasuki

#include "FlashAir.h"

#ifndef MEMORY_SAVING
#include "CommandQueue.h"
#endif
#include <iSdio.h>
#include <utility/AbstructSd2Card.h>

#ifndef USING_MOCK
#include <utility/Sd2Card.h>
#else
#ifdef DEBUG_METHODS
#include <Serial.h>
#endif
#endif

#ifdef ENABLE_GET_STATUS
uint8_t gBuffer[512];
#else
uint8_t gBuffer[0x34];
#endif

#ifdef ENABLE_GET_STATUS
void copyIPAddress(uint8_t src[4], uint8_t dest[4]) {
  for (int i = 0; i < 4; i++) {
    dest[i] = src[i];
  }
}
#endif

#ifndef USING_MOCK
FlashAir::FlashAir(uint8_t chipSelectPin) {
  card_ = new Sd2Card();
  if (!card_->init(SPI_HALF_SPEED, chipSelectPin)) {
    abort();
  }
}
#else
FlashAir::FlashAir(uint8_t chipSelectPin, AbstructSd2Card* card) {
  card_ = card;
  if (!card_->init(SPI_HALF_SPEED, chipSelectPin)) {
    abort();
  }
}
#endif

uint32_t FlashAir::getNextSequenceId() {
  if (card_->readExtMemory(1, 1, 0x420, 0x34, gBuffer)) {
    if (gBuffer[0x20] == 0x01) {
      return get_u32(gBuffer + 0x24) + 1;
    }
  }
  return 0;
}

FlashAir::CommandResponse FlashAir::getCommandResponse(uint32_t sequenceId) {
  //memset(gBuffer, 0, 0x14);

  for (int i = 0; i < 8; i++) {
    if (!card_->readExtMemory(1, 1, 0x440 + (0x14 * i), 0x14, gBuffer)) {
      return FlashAir::FAILED;
    }

    if (sequenceId != get_u32(gBuffer + 4)) {
      continue;
    }
    uint8_t resp = get_u8(gBuffer + 8);
    switch (resp) {
    case 0x00:
      return FlashAir::INITIAL;
    case 0x01:
      return FlashAir::PROCESSING;
    case 0x02:
      return FlashAir::REJECTED;
    case 0x03:
      return FlashAir::SUCCEEDED;
    case 0x04:
      return FlashAir::TERMINATED;
    case 0x80:
      return FlashAir::GENERAL_ERROR;
    case 0x81:
      return FlashAir::ARGUMENT_ERROR;
    case 0x82:
      return FlashAir::NETWORK_ERROR;
    case 0x83:
      return FlashAir::FILE_SYSTEM_ERROR;
    case 0x84:
      return FlashAir::BUFFER_OVERFLOW_ERROR;
    default:
      return FlashAir::FAILED;
    }
  }
  return FlashAir::FAILED;
}

#ifdef DEBUG_METHODS
void FlashAir::debugCommandResponse() {
  memset(gBuffer, 0, 0x14);

  for (int i = 0; i < 8; i++) {
    if (!card_->readExtMemory(1, 1, 0x440 + (0x14 * i), 0x14, gBuffer)) {
      return;
    }

    Serial.print(F("cmd="));
    Serial.print(get_u8(gBuffer + 0), DEC);
    Serial.print(F("\t"));
    Serial.print(F("seq="));
    Serial.print(get_u32(gBuffer + 4), DEC);
    Serial.print(F("\t"));
    Serial.print(F("resp="));
    uint8_t resp = get_u8(gBuffer + 8);
    Serial.print(resp, HEX);
    Serial.print(F("\t"));
    switch (resp) {
    case 0x00:
      Serial.print(F("INITIAL"));
      break;
    case 0x01:
      Serial.print(F("PROCESSING"));
      break;
    case 0x02:
      Serial.print(F("REJECTED"));
      break;
    case 0x03:
      Serial.print(F("SUCCEEDED"));
      break;
    case 0x04:
      Serial.print(F("TERMINATED"));
      break;
    case 0x80:
      Serial.print(F("GENERAL_ERROR"));
      break;
    case 0x81:
      Serial.print(F("ARGUMENT_ERROR"));
      break;
    case 0x82:
      Serial.print(F("NETWORK_ERROR"));
      break;
    case 0x83:
      Serial.print(F("FILE_SYSTEM_ERROR"));
      break;
    case 0x84:
      Serial.print(F("BUFFER_OVERFLOW_ERROR"));
      break;
    default:
      Serial.print(F("FAILED"));
      break;
    }
    Serial.println();
  }
}

boolean FlashAir::isCommandSucceeded(uint32_t sequenceId) {
  return getCommandResponse(sequenceId) == FlashAir::SUCCEEDED;
}
#endif

boolean FlashAir::isCommandDone(uint32_t sequenceId) {
  return getCommandResponse(sequenceId) != FlashAir::INITIAL &&
    getCommandResponse(sequenceId) != FlashAir::PROCESSING;
}

boolean FlashAir::disconnect(uint32_t sequenceId) {
  //memset(gBuffer, 0, 512);
  uint8_t* p = gBuffer;
  p = put_command_header(p, 1, 0);
  p = put_command_info_header(p, 0x07, sequenceId, 0);
  put_command_header(gBuffer, 1, (p - gBuffer));
  return card_->writeExtDataPort(1, 1, 0x000, gBuffer);
}

#ifndef MEMORY_SAVING
boolean FlashAir::isAllCommandDone() {
  return countCommandQueue() == 0;
}

boolean FlashAir::isLastCommandSucceededToDispatch() {
  return isLastCommandSucceededToDispatch_;
}

void CallbackEachCommands(void* object, uint32_t sequenceId) {
  FlashAir* self = (FlashAir*)object;
  if (self->isCommandDone(sequenceId)) {
    removeCommandFromQueue(sequenceId);
  }
}

void FlashAir::resume() {
  if (!isAllCommandDone()) {
    eachCommandQueue(CallbackEachCommands, this);
  }
}
#endif

boolean FlashAir::connect(uint32_t sequenceId, const char* ssid, const char* networkKey) {
  //memset(gBuffer, 0, 512);
  uint8_t* p = gBuffer;
  p = put_command_header(p, 1, 0);
  p = put_command_info_header(p, 0x02, sequenceId, 2);
  p = put_str_arg(p, ssid);
  p = put_str_arg(p, networkKey);
  put_command_header(gBuffer, 1, (p - gBuffer));
  return card_->writeExtDataPort(1, 1, 0x000, gBuffer) ? true : false;
}

#ifndef MEMORY_SAVING
uint32_t FlashAir::connect(const char* ssid, const char* networkKey) {
  uint32_t seq = getNextSequenceId();
  if (!appendCommandToQueue(seq)) return 0;
  isLastCommandSucceededToDispatch_ = connect(seq, ssid, networkKey);
  return seq;
}

uint32_t FlashAir::disconnect() {
  uint32_t seq = getNextSequenceId();
  if (appendCommandToQueue(seq)) return 0;
  isLastCommandSucceededToDispatch_ = disconnect(seq);
  return seq;
}
#endif

#ifdef ENABLE_GET_STATUS
Status* FlashAir::getStatus() {
  memset(gBuffer, 0, 0x200);
  if (!card_->readExtMemory(1, 1, 0x400, 0x200, gBuffer)) {
    return false;
  }
  // Show values in the application status area.

  if ((gBuffer[0x104]) == 0x00) status_.wifi.application = Status::WiFi::NO_APPLICATION;
  if ((gBuffer[0x104]) == 0x01) status_.wifi.application = Status::WiFi::P2P_SENDER_APPLICATION;
  if ((gBuffer[0x104]) == 0x02) status_.wifi.application = Status::WiFi::P2P_RECEIVER_APPLICATION;
  if ((gBuffer[0x104]) == 0x03) status_.wifi.application = Status::WiFi::DLNA_M_DMU_MODE;
  if ((gBuffer[0x104]) == 0x04) status_.wifi.application = Status::WiFi::DLNA_PU_MODE;
  if ((gBuffer[0x104]) == 0x05) status_.wifi.application = Status::WiFi::DLNA_M_DMS_MODE;
  if ((gBuffer[0x104]) == 0x06) status_.wifi.application = Status::WiFi::PTP_PULL_MODE;
  if ((gBuffer[0x104]) == 0x07) status_.wifi.application = Status::WiFi::DPS_MODE;
  if ((gBuffer[0x104]) == 0x08) status_.wifi.application = Status::WiFi::PTP_PASSTHROUGHT_MODE;

  status_.wifi.scanning = ((gBuffer[0x106] & 0x01) == 0x01);

  if ((gBuffer[0x106] & 0x06) == 0x00) status_.wifi.wps =  Status::WiFi::NO_WPS;
  if ((gBuffer[0x106] & 0x06) == 0x02) status_.wifi.wps =  Status::WiFi::WPS_WITH_PIN;
  if ((gBuffer[0x106] & 0x06) == 0x04) status_.wifi.wps =  Status::WiFi::WPS_WITH_PBC;

  if ((gBuffer[0x106] & 0x08) == 0x00) status_.wifi.groupOfWiFiDirect =  Status::WiFi::CLIENT;
  if ((gBuffer[0x106] & 0x08) == 0x08) status_.wifi.groupOfWiFiDirect =  Status::WiFi::OWNER;

  if ((gBuffer[0x106] & 0x10) == 0x00) status_.wifi.infrastructureMode =  Status::WiFi::STA;
  if ((gBuffer[0x106] & 0x10) == 0x10) status_.wifi.infrastructureMode =  Status::WiFi::AP;

  if ((gBuffer[0x106] & 0x60) == 0x00) status_.wifi.connectionScheme = Status::WiFi::INITIAL;
  if ((gBuffer[0x106] & 0x60) == 0x20) status_.wifi.connectionScheme = Status::WiFi::INFRASTRUCTURE;
  if ((gBuffer[0x106] & 0x60) == 0x40) status_.wifi.connectionScheme = Status::WiFi::WIFI_DIRECT;

  status_.wifi.connected = ((gBuffer[0x106] & 0x80) == 0x80);

  for (int i = 0; i < 32; ++i) {
    status_.wifi.ssid[i] = (char)gBuffer[0x108 + i];
  }
  status_.wifi.ssid[32] = 0;

  switch (gBuffer[0x128]) {
  case 0 :
    status_.wifi.encryptionMode = Status::WiFi::OPEN_NO_ENCRYPTION;
    break;
  case 1 :
    status_.wifi.encryptionMode = Status::WiFi::OPEN_AND_WEP;
    break;
  case 2 :
    status_.wifi.encryptionMode = Status::WiFi::SHARED_EY_AND_WEP;
    break;
  case 3 :
    status_.wifi.encryptionMode = Status::WiFi::WPA_PSK_AND_TKIP;
    break;
  case 4 :
    status_.wifi.encryptionMode = Status::WiFi::WPA_PSK_AND_AES;
    break;
  case 5 :
    status_.wifi.encryptionMode = Status::WiFi::WPA2_PSK_AND_TKIP;
    break;
  case 6 :
    status_.wifi.encryptionMode = Status::WiFi::WPA2_PSK_AND_AES;
    break;
  default:
    status_.wifi.encryptionMode = Status::WiFi::UNKNOWN_ENCRYPTION;
  }

  status_.wifi.signalStrength = gBuffer[0x129];
  status_.wifi.channel = gBuffer[0x12A];

  for (int i = 0; i < 6; ++i) {
    status_.wifi.macAddress[i] = gBuffer[0x130 + i];
  }
  for (int i = 0; i < 16 && gBuffer[0x140 + i] != 0; ++i) {
    status_.wifi.idForP2P[i] = (char)gBuffer[0x140 + i];
  }
  copyIPAddress(gBuffer + 0x150, status_.wifi.ipAddress);
  copyIPAddress(gBuffer + 0x154, status_.wifi.subnetMask);
  copyIPAddress(gBuffer + 0x158, status_.wifi.gateway);
  copyIPAddress(gBuffer + 0x15C, status_.wifi.dnsServerPreferred);
  copyIPAddress(gBuffer + 0x160, status_.wifi.dnsServerAlternate);
  status_.wifi.proxyEnabled = (gBuffer[0x164] & 0x01) == 0x01;
  status_.wifi.httpProgress = gBuffer[0x174] & 0xEF;
  status_.wifi.httpProcessing = (gBuffer[0x174] & 0x80) == 0x80;
  status_.wifi.powerSaveMode = (gBuffer[0x175] & 0x01) == 0x01;
  status_.wifi.fileSystemModified = (gBuffer[0x176] & 0x01) == 0x00;
  return &status_;
}
#endif


