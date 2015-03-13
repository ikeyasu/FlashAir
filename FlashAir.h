// Copyright (c) 2015 IKEUCHI Yasuki

#ifndef FlashAir_h
#define FlashAir_h

#include "config.h"

#include <Arduino.h>
#include "Sd2Card.h"

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
    Sd2Card* card_;
    boolean isLastCommandSucceededToDispatch_;
  public:
    enum CommandResponse {
      INITIAL,
      PROCESSING,
      REJECTED,
      SUCCEEDED,
      TERMINATED,
      GENERAL_ERROR,
      ARGUMENT_ERROR,
      NETWORK_ERROR,
      FILE_SYSTEM_ERROR,
      BUFFER_OVERFLOW_ERROR,
      FAILED
    };

    FlashAir(uint8_t chipSelectPin);
    uint32_t getNextSequenceId();
    CommandResponse getCommandResponse(uint32_t sequenceID);
    boolean isCommandSucceeded(uint32_t sequenceID);
    boolean isCommandDone(uint32_t sequenceID);

    Status* getStatus();
    boolean disconnect(uint32_t sequenceID);
    boolean connect(uint32_t sequenceId, const char* ssid, const char* networkKey);
    boolean isConnected();
    boolean requestHTTP(uint32_t sequenceID, boolean is_https, const char* host, const char* path);
    //uint32_t requestHTTP(const char* host, const char* path);
    const char* getHTTPResponse(uint32_t* out_length);
    uint32_t disconnect();
    uint32_t connect( const char* ssid, const char* networkKey);
    boolean isLastCommandSucceededToDispatch();
    boolean isAllCommandDone();
    void resume();

#ifdef DEBUG_METHODS
    void debugCommandResponse();
    Sd2Card* getSd2Card() {
      return card_;
    };
#endif
};

#endif


