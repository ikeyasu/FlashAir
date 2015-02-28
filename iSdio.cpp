/* Arduino Sdio Library
 *  Copyright (C) 2014 by Munehiro Doi, Fixstars Corporation
 *  All rights reserved.
 *  Released under the BSD 2-Clause license.
 *  http://flashair-developers.com/documents/license.html
 */

#include "iSdio.h"

//------------------------------------------------------------------------------
/** specific version for string. it adds padding bytes after text data. */
template <>
uint8_t* put_T(uint8_t* p, const char* value) {
  while (*value != 0) {
    *p++ = *((uint8_t*)value++);
  }
  return p;
}

template <>
uint8_t* put_T_arg(uint8_t* p, const char* value) {
  uint8_t* orig = p;
  p += sizeof(uint32_t);            // skip length area.
  p = put_T(p, value);              // data
  uint32_t len = p - orig - sizeof(uint32_t);
  put_T(orig, len);                 // write length.
  for (uint32_t i = 0; i < ((4 - (len & 3)) & 3); ++i) { // padding
    *p++ = 0;
  }
  return p;
}

uint8_t* put_command_header(uint8_t* p, uint8_t num_commands,
                            uint32_t command_bytes) {
  p = put_u8(p, 0x01);           // Write Data ID
  p = put_u8(p, num_commands);   // Number of commands.
  p = put_u16(p, 0);             // reserved.
  p = put_u32(p, command_bytes); // size of command write data.
  p = put_u32(p, 0);             // reserved.
  return p;
}

uint8_t* put_command_info_header(uint8_t* p, uint16_t command_id,
                                 uint32_t sequence_id, uint16_t num_args) {
  p = put_u16(p, 0);             // reserved.
  p = put_u16(p, command_id);    // iSDIO command id.
  p = put_u32(p, sequence_id); // iSDIO command sequence id.
  p = put_u16(p, num_args);      // Number of Arguments.
  p = put_u16(p, 0);             // Reserved.
  return p;
}
