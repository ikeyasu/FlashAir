// Copyright (c) 2015 IKEUCHI Yasuki

#ifndef FlashAir_h
#define FlashAir_h

#include <Arduino.h>

class Status {
  public:
    class WiFi {
      public:
        enum {
          NO_APPLICATION,
          P2P_SENDER_APPLICATION,
          P2P_RECEIVER_APPLICATION,
          DLNA_M_DMU_MODE,
          DLNA_PU_MODE,
          DLNA_M_DMS_MODE,
          PTP_PULL_MODE,
          DPS_MODE,
          PTP_PASSTHROUGHT_MODE
        } application;
        boolean scanning;
        enum WPS {
          NO_WPS,
          WPS_WITH_PIN,
          WPS_WITH_PBC
        } wps;
        enum {
          OWNER,
          CLIENT
        } groupOfWiFiDirect;
        enum {
          STA,
          AP
        } infrastructureMode;
        enum {
          INITIAL,
          INFRASTRUCTURE,
          WIFI_DIRECT
        } connectionScheme;
        boolean connected;
        char ssid[33];
        enum {
          OPEN_NO_ENCRYPTION,
          OPEN_AND_WEP,
          SHARED_EY_AND_WEP,
          WPA_PSK_AND_WEP,
          WPA_PSK_AND_TKIP,
          WPA_PSK_AND_AES,
          WPA2_PSK_AND_TKIP,
          WPA2_PSK_AND_AES,
          UNKNOWN_ENCRYPTION
        }
        encryptionMode;
        uint8_t signalStrength;
        uint8_t channel;
        byte macAddress[6];
        char idForP2P[17];
        uint8_t ipAddress[4];
        uint8_t subnetMask[4];
        uint8_t gateway[4];
        uint8_t dnsServerPreferred[4];
        uint8_t dnsServerAlternate[4];
        boolean proxyEnabled;
        uint8_t httpProgress;
        boolean httpProcessing;
        boolean powerSaveMode;
        boolean fileSystemModified;
        /* TODO
         DLNA Status
         P2P Status
         PTP Status
         Date
         Tiime
         */
    };
    WiFi wifi;
};

class FlashAir {
  private:
    Status status_;
  public:
    FlashAir(uint8_t chipSelectPin);
    Status* getStatus();
};

#endif


