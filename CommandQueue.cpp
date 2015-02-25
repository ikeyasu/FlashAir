#include <stdint.h>
#include "CommandQueue.h"
#ifdef __cplusplus
extern "C" {
#endif

#if COMMAND_QUEUE_SIZE > 8
#error COMMAND_QUEUE_SIZE must be equal to or less than 8
#endif

uint32_t gQueue[1];
uint8_t gFlags = 0;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

uint8_t getFreeCommandQueue() {
  uint8_t i;
  for (i = 0; i < COMMAND_QUEUE_SIZE; i++) {
    if (((gFlags >> i) & 0x1) == 0) {
      return i;
    }
  }
  // Could not find free queue
  return COMMAND_QUEUE_SIZE;
}

uint8_t appendCommandToQueue(uint32_t sequenceId) {
  uint8_t i = getFreeCommandQueue();
  if (i >= COMMAND_QUEUE_SIZE) return FALSE;

  gQueue[i] = sequenceId;
  gFlags |= (1 << i);
  return TRUE;
}

uint8_t removeCommandFromQueue(uint8_t sequenceId) {
  uint8_t i;
  for (i = 0; i < COMMAND_QUEUE_SIZE; i++) {
    if (((gFlags >> i) & 0x1) == 1 &&
        gQueue[i] == sequenceId) {
      gFlags ^= (1 << i);
      return TRUE;
    }
  }
  // Could not find
  return FALSE;
}

uint8_t countCommandQueue() {
  uint8_t bits = gFlags;
  bits = (bits & 0x55) + (bits >> 1 & 0x55);
  bits = (bits & 0x33) + (bits >> 2 & 0x33);
  return (bits & 0x0f) + (bits >> 4 & 0x0f);
}

void clearCommandQueue() {
  gFlags = 0;
}

void eachCommandQueue(CallbackEach eachCallback, void* object) {
  uint8_t i;
  if (eachCallback == 0) return;
  for (i = 0; i < COMMAND_QUEUE_SIZE; i++) {
    if (((gFlags >> i) & 0x1) == 1) {
      (eachCallback)(object, gQueue[i]);
    }
  }
}

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif
