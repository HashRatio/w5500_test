#ifndef _STRATUM_H_
#define _STRATUM_H_
#include "types.h"

#define TOKEN_BUFFER 64
extern int32 parse_stratum(const int8 * json);
extern int32 connect_poll(uint8 * addr, uint16 port);
extern int32 recv_stratum();
extern int32 send_subscribe();
extern int32 send_authorize();
#endif