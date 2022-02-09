
#if !defined(_FLAGS_H__)
#define _FLAGS_H__

#include <inttypes.h>

#define DISCOVERED_FLAG (uint8_t) 1
#define VISITED (uint8_t) 3
#define PRODUCTIVE_FLAG (uint8_t) 4
#define FINAL_STATE_FLAG (uint8_t) 8
#define START_STATE_FLAG (uint8_t) 16
#define ACTIVE_FLAG (uint8_t) 32
#define DESTINATION_FLAG (uint8_t) 64

#endif
