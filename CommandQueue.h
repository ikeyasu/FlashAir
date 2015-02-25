// Copyright (c) 2015 IKEUCHI Yasuki

#ifndef CommandQueue_h
#define CommandQueue_h

#ifdef __cplusplus
extern "C" {
#endif

#define COMMAND_QUEUE_SIZE 1
uint8_t getFreeCommandQueue();
uint8_t appendCommandToQueue(uint32_t sequenceId);
uint8_t removeCommandFromQueue(uint8_t sequenceId);
uint8_t countCommandQueue();
void clearCommandQueue();

typedef void (*CallbackEach)(void* object, uint32_t sequenceId);
void eachCommandQueue(CallbackEach, void* object);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif
