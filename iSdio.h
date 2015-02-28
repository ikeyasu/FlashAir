/* Arduino Sdio Library
 *  Copyright (C) 2014 by Munehiro Doi, Fixstars Corporation
 *  All rights reserved.
 *  Released under the BSD 2-Clause license.
 *  http://flashair-developers.com/documents/license.html
 */
#ifndef iSdio_h
#define iSdio_h

#include <stdint.h>

//------------------------------------------------------------------------------
// utilities to make a data block.
template <typename T>
uint8_t* put_T(uint8_t* p, T value) {
  *((T*)p) = value;
  p += sizeof(T);
  return p;
}
/** specific version for string. it set length, data, and paddings. */
template <>
uint8_t* put_T(uint8_t* p, const char* value);

// utilities for those who want to see value types by function names.
static inline
uint8_t* put_u8(uint8_t* p, uint8_t value) {
  return put_T(p, value);
}
static inline
uint8_t* put_u16(uint8_t* p, uint16_t value) {
  return put_T(p, value);
}
static inline
uint8_t* put_u32(uint8_t* p, uint32_t value) {
  return put_T(p, value);
}
static inline
uint8_t* put_str(uint8_t* p, const char* value) {
  return put_T(p, value);
}

static inline
uint8_t get_u8(uint8_t* p) {
  return *((uint8_t*)p);
}
static inline
uint16_t get_u16(uint8_t* p) {
  return *((uint16_t*)p);
}
static inline
uint32_t get_u32(uint8_t* p) {
  return *((uint32_t*)p);
}

template <typename T>
uint8_t* put_T_arg(uint8_t* p, T value) {
  p = put_T(p, (uint32_t)sizeof(T)); // length
  p = put_T(p, value);                      // data
  for (uint8_t i = 0; i < ((4 - sizeof(T)) & 0x3); ++i) { // padding
    *p++ = 0;
  }
  return p;
}
template <>
uint8_t* put_T_arg(uint8_t* p, const char* value);

static inline
uint8_t* put_u8_arg(uint8_t* p, uint8_t value) {
  return put_T_arg(p, value);
}
static inline
uint8_t* put_u16_arg(uint8_t* p, uint16_t value) {
  return put_T_arg(p, value);
}
static inline
uint8_t* put_u32_arg(uint8_t* p, uint32_t value) {
  return put_T_arg(p, value);
}
static inline
uint8_t* put_str_arg(uint8_t* p, const char* value) {
  return put_T_arg(p, value);
}
uint8_t* put_command_header(uint8_t* p, uint8_t num_commands,
                            uint32_t command_bytes);
uint8_t* put_command_info_header(uint8_t* p, uint16_t command_id,
                                 uint32_t sequence_id, uint16_t num_args);

//------------------------------------------------------------------------------
/**
 * Header data for CMD48/49 param
 */
typedef struct {
  uint8_t  id;
  uint8_t  number;
  uint16_t _reserved1;
  uint32_t size;
  uint32_t _reserved2;
} isdio_command_header_t;

/**
 * Information data for CMD48/49 param
 */
typedef struct {
  uint16_t _reserved0;
  uint16_t command;
  uint32_t sequence_id;
  uint16_t num_args;
  uint16_t _reserved1;
} isdio_command_info_header_t;

/**
 * iSDIO Response Status Register at 0x440
 */
typedef struct {
  uint8_t  registration;
  uint8_t  _reserved0;
  uint16_t command_id;
  uint32_t sequence_id;
  uint8_t  status;
  uint8_t  _reserved1[3];
  uint32_t error_status;
  uint32_t data_size;
} isdio_response_status_t;

/**
 * Status Register at 0x400
 */
typedef struct {
  uint8_t                 CWU       : 1; // Command Write Update
  uint8_t                 CWA       : 1; // Command Write Abort
  uint8_t                 _reserved0: 6;
  uint8_t                 _reserved1[31];
  uint16_t                CRU       : 1; // Command Response Update
  uint16_t                ESU       : 1; // Error Status Update
  uint16_t                MCU       : 1; // Media Change Update
  uint16_t                ASU       : 1; // Application Status Update
  uint16_t                _reserved2: 12;
  uint16_t                CRU_ENA   : 1; // CRU Enabled
  uint16_t                ESU_ENA   : 1; // ESU Enabled
  uint16_t                MCU_ENA   : 1; // MCU Enabled
  uint16_t                ASU_ENA   : 1; // ASU Enabled
  uint16_t                _reserved3: 12;
  uint16_t                CRE       : 1; // Command Response Error
  uint16_t                CWE       : 1; // Command Write Error
  uint16_t                RRE       : 1; // Response Receive Error
  uint16_t                APE       : 1; // Application Specific Error
  uint16_t                _reserved4: 12;
  uint16_t                MEX       : 1; // Memory Existence
  uint16_t                FAT       : 1; // FAT System
  uint16_t                _reserved5: 14;
  uint8_t                 _reserved6[24];
  isdio_response_status_t response[8];
  uint8_t                 _reserved7[32];
  uint8_t                 application_status[256];
} isdio_status_t;


typedef struct {
  uint8_t    ULR : 1;
  uint8_t    DLU : 1;
  uint8_t    CBR : 1;
  uint8_t    CDR : 1;
  uint8_t    _reserved0 : 4;
  uint8_t    ILU : 1;
  uint8_t    FLU : 1;
  uint8_t    _reserved1 : 6;
  uint8_t    RPO : 1;
  uint8_t    RPD : 1;
  uint8_t    RPC : 1;
  uint8_t    CPI : 1;
  uint8_t    DPI : 1;
  uint8_t    CIL : 1;
  uint8_t    _reserved2 : 2;
  uint8_t    _reserved3;
  uint8_t    application;
  uint8_t    _reserved4;
  uint16_t   Scan         : 1;
  uint16_t   WPS          : 2;
  uint16_t   Group        : 1;
  uint16_t   AP_STA       : 1;
  uint16_t   Infra_Direct : 2;
  uint16_t   Connected    : 1;
  uint16_t   _reserved5    : 8;
  char       SSID[32];
  uint8_t    encryption_mode;
  uint8_t    signal_strength;
  uint8_t    channel;
  uint8_t    _reserved6[5];
  uint8_t    MACAddress[6];
  uint8_t    _reserved7[10];
  uint8_t    ID[16];
  uint8_t    IPAddress[4];
  uint8_t    SubnetMask[4];
  uint8_t    DefaultGateway[4];
  uint8_t    PreferredDNSServer[4];
  uint8_t    AlternateDNSServer[4];
  uint8_t    ProxyServer: 1;
  uint8_t    _reserved8: 7;
} isdio_wlan_status_t;

/**
 * iSDIO Vendor Status Register at 0x5C0
 */
typedef struct {
  uint32_t vendorId;
  uint32_t vendorFunction;
  uint32_t HttpBodyTransferSize;
  uint32_t HttpBodyTotalSize;
  uint32_t HttpBuffSize;
  uint32_t LHFStatus: 8;
  uint32_t PhotoshareStatus: 8;
  uint32_t FileDownloadStatus: 8;
  uint32_t rsv0: 8;
  uint32_t FtpTransferSize;
  uint32_t rsv1;
  uint32_t rsv2[4];
  uint8_t VendorFwVersion[16];
} isdio_vender_status_t;

#endif  // iSdio_h
