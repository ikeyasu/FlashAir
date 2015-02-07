// Copyright (c) 2015 IKEUCHI Yasuki

#include "FlashAir.h"

#include <iSdio.h>
#include <utility/Sd2CardExt.h>
Sd2CardExt gCard;
uint8_t gBuffer[512];

FlashAir::FlashAir(uint8_t chipSelectPin) {
  if (!gCard.init(SPI_HALF_SPEED, chipSelectPin)) {
    abort();
  }
}

void copyIPAddress(uint8_t src[4], uint8_t dest[4]) {
  for (int i = 0; i < 4; i++) {
    dest[i] = src[i];
  }
}

Status* FlashAir::getStatus() {
  memset(gBuffer, 0, 0x200);
  if (!gCard.readExtMemory(1, 1, 0x400, 0x200, gBuffer)) {
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



