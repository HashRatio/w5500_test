/*
 * Author: Xiangfu Liu <xiangfu@openmobilefree.net>
 * Bitcoin: 1CanaaniJzgps8EV6Sfmpb7T8RutpaeyFn
 *
 * This is free and unencumbered software released into the public domain.
 * For details see the UNLICENSE file at the root of the source tree.
 */

#ifndef __MINER_H__
#define __MINER_H__

#include <stdbool.h>
#include "types.h"
//#include <stdint.h>

#include "protocol.h"

#define NONCE_HW    0
#define NONCE_VALID 1
#define NONCE_DIFF  2

typedef struct work
{
    uint32 nonce2;
    uint32 mm_idx;

    uint8 data[52];     /* midstate[32] + data[12] */
    uint8 header[128];  /* Block header */
} work;

typedef struct mm_work
{
    uint8 job_id[20];

    uint32 coinbase_len;
    uint8 coinbase[HRTO_P_COINBASE_SIZE];

    uint32 nonce2;
    int nonce2_offset;
    int nonce2_size; /* only 4 is support atm. */

    int merkle_offset;
    int nmerkles;
    uint8 merkles[HRTO_P_MERKLES_COUNT][32];

    uint8 header[128];

    uint32 diff;
    uint32 pool_no;

    uint8   target[32];
} mm_work;

extern mm_work * mm_work_ptr;
extern mm_work g_mm_works[];
extern work g_works[];

extern uint32 nonce1_bin;
extern int8 nonce1_str[9];

void miner_init_work(struct mm_work *mw, struct work *work);
void miner_gen_nonce2_work(struct mm_work *mw, uint32 nonce2, struct work *work);
//int test_nonce(struct mm_work *mw, struct result *ret);
int32 test_nonce(struct mm_work *mw, char *result, uint32 nonce2, uint32 nonce);

int32 test_nonce(struct mm_work *mw, char * result, uint32 nonce2, uint32 nonce);
void reset_hashrate();
void set_asic_freq(uint32 value);
uint32 get_asic_freq();

#endif /* __MINER_H__ */
