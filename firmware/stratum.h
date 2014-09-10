#ifndef _STRATUM_H_
#define _STRATUM_H_
#include "types.h"

#define TOKEN_BUFFER 64

#define P_MINING_SUBSCRIBE 0x01
#define P_MINING_AUTHORIZE 0x02
#define P_MINING_SUBMIT 0x03

extern int32 authorize_id;
extern int32 subscribe_id;
extern int32 submit_id;

extern int32 parse_stratum(const int8 * json);
extern int32 connect_poll(uint8 * addr, uint16 port);
extern int32 recv_stratum();
extern int32 send_subscribe();
extern int32 send_authorize();
#endif