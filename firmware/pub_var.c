#include "pub_var.h"

int8 buffer[BUFFER_SIZE];
int8 g_new_stratum;
uint8 g_current_target[32];

uint8 g_worker_name[50] = "hyperwang.unihash";
uint8 g_worker_pwd[10] = "xxx";

uint32 g_diff;

uint32 g_last_ntime;
uint32 g_curr_ntime;
uint32 g_share;
uint32 g_hashrate;
uint8 g_hashrate_reset_flag = 1;

uint8 g_diff1_target[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00};
uint8 g_diff256_target[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00};