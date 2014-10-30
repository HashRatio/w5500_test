#ifndef _PUB_VAR_H_
#define _PUB_VAR_H_
#include "types.h"

#define BUFFER_SIZE 2048

extern int8 buffer[BUFFER_SIZE];
extern int8 g_new_stratum;

extern uint8 g_current_target[32];
extern uint32 g_diff;
extern uint32 g_last_ntime;
extern uint32 g_curr_ntime;
extern uint8 g_hashrate_reset_flag;
extern uint32 g_share;
extern uint32 g_hashrate;

extern uint8 g_worker_name[50];
extern uint8 g_worker_pwd[10];
extern const uint8 g_diff1_target[32];
extern const uint8 g_diff256_target[32];

extern uint64 board_status[32];
extern uint8 board_version[32];
#endif