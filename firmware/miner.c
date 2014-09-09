/*
 * Author: Xiangfu Liu <xiangfu@openmobilefree.net>
 * Bitcoin:	1CanaaniJzgps8EV6Sfmpb7T8RutpaeyFn
 *
 * This is free and unencumbered software released into the public domain.
 * For details see the UNLICENSE file at the root of the source tree.
 */

#include <stdbool.h>
//#include <stdint.h>
#include<stdio.h>
#include "types.h"
#include "minilibc.h"
#include "system_config.h"
#include "defines.h"
#include "io.h"
#include "uart.h"
#include "miner.h"
#include "sha256.h"
#include "twipwm.h"

static inline void flip32(void *dest_p, const void *src_p)
{
	uint32_t *dest = dest_p;
	const uint32_t *src = src_p;
	int i;

	for (i = 0; i < 8; i++)
		dest[i] = bswap_32(src[i]);
}

static inline void flip64(void *dest_p, const uint8_t *src_p)
{
	uint32_t *dest = dest_p;
	int i;

	for (i = 0; i < 16; i++) {
		dest[i] = src_p[i * 4 + 0] | src_p[i * 4 + 1] << 8 |
			src_p[i * 4 + 2] << 16 | src_p[i * 4 + 3] << 24;
	}
}

static inline void flip80(void *dest_p, const void *src_p)
{
	uint32_t *dest = dest_p;
	const uint32_t *src = src_p;
	int i;

	for (i = 0; i < 20; i++)
		dest[i] = bswap_32(src[i]);
}

static void calc_midstate(struct mm_work *mw, struct work *work)
{
	unsigned char data[64];
	uint32_t *data32 = (uint32_t *)data;

	flip64(data32, mw->header);

	sha256_init();
	sha256_update(data, 64);
	sha256_final(work->data);

	memcpy(data, work->data, 32);
	flip32(work->data, data);
	
	memcpy(work->data + 32, mw->header + 64, 12);
}

void miner_gen_nonce2_work(struct mm_work *mw, uint32 nonce2, struct work *work)
{
	uint8 merkle_root[32], merkle_sha[64];
	uint32 *data32, *swap32, tmp32;
	int i;

	tmp32 = bswap_32(nonce2);
	memcpy(mw->coinbase + mw->nonce2_offset, (uint8_t *)(&tmp32), sizeof(uint32_t));
	work->nonce2 = nonce2;

	dsha256(mw->coinbase, mw->coinbase_len, merkle_root);
	memcpy(merkle_sha, merkle_root, 32);
	for (i = 0; i < mw->nmerkles; i++) {
		memcpy(merkle_sha + 32, mw->merkles[i], 32);
		dsha256(merkle_sha, 64, merkle_root);
		memcpy(merkle_sha, merkle_root, 32);
	}
	data32 = (uint32 *)merkle_sha;
	swap32 = (uint32 *)merkle_root;
	flip32(swap32, data32);

	memcpy(mw->header + mw->merkle_offset, merkle_root, 32);
	memcpy(work->header, mw->header, 128);
	
	calc_midstate(mw, work);
}

int fulltest(const unsigned char *hash, const unsigned char *target)
{
	uint32_t *hash32 = (uint32_t *)hash;
	uint32_t *target32 = (uint32_t *)target;
	int rc = 1;
	int i;

	for (i = 28 / 4; i >= 0; i--) {
		uint32_t h32tmp = bswap_32(hash32[i]);
		uint32_t t32tmp = bswap_32(target32[i]);

		if (h32tmp > t32tmp) {
			rc = NONCE_VALID;
			break;
		}
		if (h32tmp < t32tmp) {
			rc = NONCE_DIFF;
			break;
		}
	}

	return rc;
}

int32 test_nonce(struct mm_work *mw, uint32 nonce2, uint32 nonce)
{
	/* Generate the work base on nonce2 */
	struct work work;
	debug32("Test: %08x %08x\n", nonce2, nonce);
	miner_gen_nonce2_work(mw, nonce2, &work);

	/* Write the nonce to block header */
	uint32_t *work_nonce = (uint32_t *)(work.header + 64 + 12);
	*work_nonce = bswap_32(nonce);

	/* Regen hash */
	uint32_t *data32 = (uint32_t *)(work.header);
	unsigned char swap[80];
	uint32_t *swap32 = (uint32_t *)swap;
	unsigned char hash1[32];
	uint32_t *hash_32 = (uint32_t *)(hash1 + 28);

	flip80(swap32, data32);
	dsha256(swap, 80, hash1);

	if (*hash_32 != 0)
		return NONCE_HW;
	
	/* Compare hash with target */
	return fulltest(hash1, mw->target);
}
