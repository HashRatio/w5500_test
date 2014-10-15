#ifndef _PUB_VAR_H_
#define _PUB_VAR_H_
#include "types.h"

#define BUFFER_SIZE 2048

extern int8 buffer[BUFFER_SIZE];
extern int8 g_new_stratum;

extern uint8 g_current_target[32];

extern uint8 g_worker_name[50];
extern uint8 g_worker_pwd[10];
extern uint8 g_diff1_target[32];
#endif