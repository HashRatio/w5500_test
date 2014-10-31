#ifndef _STRATUM_H_
#define _STRATUM_H_
#include "types.h"

#define TOKEN_BUFFER 64

#define P_MINING_SUBSCRIBE 0x01
#define P_MINING_AUTHORIZE 0x02
#define P_MINING_SUBMIT 0x03

struct config
{
    uint8 ip[4];        /* The miner ID */
    uint8 mask[4];      /* Same with work task_id */
    uint8 gateway[4];
    uint8 pdns[4];
    uint8 sdns[4];
    uint8 poolip1[32];     /* The miner ID */
    uint8 username1[20];
    uint8 password1[16];
    uint8 poolip2[32];     /* The miner ID */
    uint8 username2[20];
    uint8 password2[16];
    uint8 sysclk[2];
};

extern int32 authorize_id;
extern int32 subscribe_id;
extern int32 submit_id;

extern int32 parse_stratum(const int8 * json);
extern int32 connect_poll(uint8 * addr, uint16 port);
extern int32 recv_stratum();
extern int32 send_subscribe();
extern int32 send_authorize();
extern int32 send_submit();
#endif
