#ifndef _DRIVER_TUBE_H
#define _DRIVER_TUBE_H

#include <stdint.h>
#include "spi.h"
#include "defines.h"
#include "uart.h"
#include "miner.h"


#define CHIP_NUMBER 1

#define C_RES   (0 << 5)    // resets all the mega88s on all boards, returns silence
#define C_LPO   (1 << 5)    // LongPoll - stop the jobs, clear the FIFO pojnters, returns silence, the BoardID contains future/10 - 1 value, eg v=2 -> fu=10*(2+1) = 30 seconds
#define C_GCK   (2 << 5)    // global clock for all boards, on the BoardID place & 0x0f
#define C_DIF   (3 << 5)    // the BoardID replacedby last LSB 2 bits the difficulty

#define C_JOB   (4 << 5)    // followed by WL[44] + exnc2[4] + MJOB_IDx[1] in 8N1, returns 0x58= confirmation that the Job has been gotten, good for sync also
#define C_ASK   (5 << 5)    // see below
#define C_TRS   (6 << 5)    // returns 32 bytes status of the core test + 32 bytes clocks + 1 byte = g_dif + (InFuture/10)-1)[1]  ... total 66 bytes
#define C_CLK   (7 << 5)    // resets mega88 on the selected board, returns silence

#define A_WAL   0x56    // ready to take a new master job :)
#define A_NO    0xa6    // nothing, means the chips are working/busy
#define A_YES   0x5A    // there is a nonce in the FIFO
#define A_STR   0x6c    // send a string or data followed by termi

#define R_RES 0
#define R_LPO 0
#define R_GCK 0
#define R_DIFF 0
#define R_JOB 0
#define R_ASK 1
#define R_CLK 0
void tube_init();
void tube_reboot_all();
void tube_handler();
void tube_discover();
void tube_reset_all();
void tube_freq_all(uint8 freq);
void tube_diff_all(uint8 diff);
#endif
