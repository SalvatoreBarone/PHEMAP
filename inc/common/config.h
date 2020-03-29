#ifndef CH_CONF
#define CH_CONF

#include "phemap_message.h"

/*
    This macro permit to chose for which environment 
    we are building the library. Only one must be decommented.
 */
#define LINUX_ENV
// #define FRTOS_ENV

#define CH_TH_WAIT_PERIOD   50
#define DEVICE_NUM          3
#define MSG_SIZE            sizeof(PHEMAP_Message_t)
#define SENTINEL            4
#define DEV_FANIN           1
#define DEV_FANOUT          0

#endif /* CH_CONF*/