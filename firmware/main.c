/*
 * Author: Minux
 * Author: Xiangfu Liu <xiangfu@openmobilefree.net>
 * Bitcoin:	1CanaaniJzgps8EV6Sfmpb7T8RutpaeyFn
 *
 * This is free and unencumbered software released into the public domain.
 * For details see the UNLICENSE file at the root of the source tree.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "minilibc.h"
#include "system_config.h"
#include "defines.h"
#include "io.h"
#include "intr.h"
#include "uart.h"
#include "miner.h"
#include "sha256.h"
#include "twipwm.h"
#include "shifter.h"
#include "timer.h"
#include "spi.h"
#include "protocol.h"
#include "crc.h"
#include "be200.h"
#include "miner.h"
#include "hexdump.c"
#include "utils.h"
#include "w5500.h"
#include "socket.h"
#include "dns.h"
#include "jsmn.h"
#include "stratum.h"
#include "pub_var.h"


#define MM_BUF_NUM  3
#define IDLE_TIME	60	/* Seconds */

//int8 buffer[BUFFER_SIZE];/*定义一个2KB的缓存*/
uint8 mac[6]={0x00,0x08,0xDC,0x01,0x02,0x03};/*定义Mac变量*/
uint8 ip[4]={192,168,2,200};/*定义Ip变量*/
uint8 sn[4]={255,255,255,0};/*定义Subnet变量*/
uint8 gw[4]={192,168,2,1};/*定义Gateway变量*/
uint8 dip[4]={192,168,2,116};
uint8 DEFAULT_DNS[4] = {192,168,2,1};
uint8 RIP[4];
uint8 DOMAIN[] = "stratum.f2pool.com";



//static uint8_t g_pkg[HRTO_P_COUNT];
//static uint8_t g_act[HRTO_P_COUNT];
 int8 g_new_stratum = 0;
static int g_local_work  = 0;
static int g_hw_work     = 0;
static int g_total_nonce = 0;
//static int g_asic_freq   = BE200_DEFAULT_FREQ;
//static int g_cur_mm_idx  = 0;
//static int g_temp_high   = 60;
//static int g_temp_normal = 50;
static int g_working = 0;
//static struct mm_work g_mm_works[MM_BUF_NUM];

//static uint32_t g_nonce2_offset = 0;
//static uint32_t g_nonce2_range  = 0xffffffff;

#define BE200_RET_RINGBUFFER_SIZE_RX 64
#define BE200_RET_RINGBUFFER_MASK_RX (BE200_RET_RINGBUFFER_SIZE_RX-1)

static volatile unsigned int ret_produce = 0;
static volatile unsigned int ret_consume = 0;

static struct chip_status miner_status[CHIP_NUMBER];
static struct be200_result be200_result_buff[BE200_RET_RINGBUFFER_SIZE_RX];
static volatile unsigned int be200_ret_produce = 0;
static volatile unsigned int be200_ret_consume = 0;

//int8 buffer[BUFFER_SIZE];/*定义一个2KB的缓存*

uint8 merkle_branch[] = {
0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x12,0x34,0x56,0x78,0x90,0xAB,0xCD,0xEF,0x12,0x34,0x56,0x78,0x90,0xAB,0xCD,0xEF,
0x12,0x34,0x56,0x78,0x90,0xAB,0xCD,0xEF,0x12,0x34,0x56,0x78,0x90,0xAB,0xCD,0xEF,
};
int a = 0x11223344;
//uint8 b[0x2000] = {0x55};

void W5500_Init(void)
{
        iinchip_hw_init();
        setSHAR(mac);/*配置Mac地址*/
        setSIPR(ip);/*配置Ip地址*/
        setSUBR(sn);/*配置子网掩码*/
        setGAR(gw);/*配置默认网关*/
}

//static uint8_t itp_data[48];
//static uint8_t itp_result[80];	
static void freq_write(uint8_t idx, uint8_t multi) {
	be200_set_pll(idx, multi);
	delay_us(500);
	return;
}
/*
static void encode_pkg(uint8_t *p, int type, uint8_t *buf, unsigned int len, int idx, int cnt)
{
	uint32_t tmp;
	uint16_t crc;
	uint8_t *data;

<<<<<<< HEAD
	memset(p, 0, HRTO_P_COUNT);

	p[0] = HRTO_H1;
	p[1] = HRTO_H2;

	p[2] = type;
	p[3] = idx;
	p[4] = cnt;

	data = p + 5;

	switch(type) {
	case HRTO_P_ACKDETECT:
	case HRTO_P_NONCE:
	case HRTO_P_TEST_RET:
	case HRTO_P_GET_FREQ:
		memcpy(data, buf, len);
		break;
	case HRTO_P_STATUS:
		tmp = read_temp() << 16 | read_temp();
		memcpy(data + 0, &tmp, 4);

		tmp = read_fan0() << 16 | read_fan1();
		memcpy(data + 4, &tmp, 4);

		memcpy(data + 8,  &g_local_work, 4);
		memcpy(data + 12, &g_hw_work,    4);
		break;
	default:
		break;
	}

	crc = crc16(data, HRTO_P_DATA_LEN);
	p[HRTO_P_COUNT - 2] = crc & 0x00ff;
	p[HRTO_P_COUNT - 1] = (crc & 0xff00) >> 8;
}
*/
/*
void send_pkg(int type, uint8_t *buf, unsigned int len, uint8_t idx, uint8_t cnt)
{
	encode_pkg(g_act, type, buf, len, idx, cnt);
	uart_nwrite((char *)g_act, HRTO_P_COUNT);
}
*/
/*
static void be200_polling()
{
	uint8_t buf[HRTO_P_DATA_LEN];
	struct be200_result *data;
	uint32_t tmp;
	struct mm_work *mw;

	if (be200_ret_consume == be200_ret_produce) {
		send_pkg(HRTO_P_STATUS, NULL, 0, 1, 1);

		g_local_work = 0;
		g_hw_work = 0;
		return;
	}
	
	data = &be200_result_buff[be200_ret_consume];
	be200_ret_consume = (be200_ret_consume + 1) & BE200_RET_RINGBUFFER_MASK_RX;
=======
int main(int argv,char * * argc)
{	
	irq_setmask(0);
	irq_enable(1);
	
	uart_init();
	uart1_init();
	
	uart_write(0x55);
	
	debug32("0x%08x\n",(int)&a);
>>>>>>> feature/generate_midstate
	
	mw = &g_mm_works[data->mm_idx];
	
	memset(buf, 0, sizeof(buf));
	tmp = data->idx;
	memcpy(buf +  0, (uint8_t *)&tmp, 4);
	tmp = mw->pool_no;
	memcpy(buf +  4, (uint8_t *)&tmp, 4);
	tmp = data->nonce2;
	memcpy(buf +  8, (uint8_t *)&tmp, 4);
	tmp = data->nonce;
	memcpy(buf + 12, (uint8_t *)&tmp, 4);
	// job_id
	memcpy(buf + 16, mw->job_id, 4);
	
	debug32("polling, miner: %02x, pool_no: %02x, mm_idx: %02x, nonce2: %08x, nonce: %08x\n",
			data->idx, mw->pool_no, data->mm_idx, data->nonce2, data->nonce);

	send_pkg(HRTO_P_NONCE, buf, HRTO_P_DATA_LEN, 1, 1);
	return;
}
*/
//static int decode_pkg(uint8_t *p, struct mm_work *mw)
/*
static int decode_pkg(uint8_t *p)
{
	unsigned int expected_crc;
	unsigned int actual_crc;
	int idx, cnt, i;
	uint8_t chip_idx;
	uint8_t pkg_idx;
	uint32_t tmp;
	uint8_t ready;
	uint8_t nonce_mask;
	uint32_t actual_nonce,expected_nonce;
	int32_t diff_nonce;
	struct mm_work *mw;
	
	int mm_write_idx = (g_cur_mm_idx + 1) % MM_BUF_NUM;  // ring buf index
	mw = &g_mm_works[mm_write_idx];  // point to next mm_work

	uint8_t *data = p + 5;
	idx = p[3];
	cnt = p[4];
	expected_crc = (p[HRTO_P_COUNT - 1] & 0xff) |
		((p[HRTO_P_COUNT - 2] & 0xff) << 8);
	actual_crc = crc16(data, HRTO_P_DATA_LEN);
	if(expected_crc != actual_crc) {
		debug32("PKG: CRC failed (W %08x, R %08x)\n",
			expected_crc, actual_crc);
		return 1;
	}

	switch (p[2]) {
	case HRTO_P_DETECT:
		g_new_stratum = 0;
		break;
	case HRTO_P_STATIC:
		memcpy(&mw->coinbase_len, data, 4);
		memcpy(&mw->nonce2_offset, data + 4, 4);
		memcpy(&mw->nonce2_size, data + 8, 4);
		memcpy(&mw->merkle_offset, data + 12, 4);
		memcpy(&mw->nmerkles, data + 16, 4);
		memcpy(&mw->diff, data + 20, 4);
		memcpy(&mw->pool_no, data + 24, 4);
		debug32("HRTO_P_STATIC: (%d):  coinbase_len: %d, nonce2_offset: %d, nonce2_size: %d, "
				"merkle_offset: %d, nmerkles: %d, diff: %d, pool_no: %d\n",
			g_new_stratum,
			mw->coinbase_len,
			mw->nonce2_offset,
			mw->nonce2_size,
			mw->merkle_offset,
			mw->nmerkles,
			mw->diff,
			mw->pool_no);
		break;
	case HRTO_P_JOB_ID:
		memcpy(mw->job_id, data, 4);
		debug32("HRTO_P_JOB_ID: job_id: %s\n", mw->job_id);
		break;
	case HRTO_P_COINBASE:
		if (idx == 1)
			memset(mw->coinbase, 0, sizeof(mw->coinbase));
		memcpy(mw->coinbase + (idx - 1) * HRTO_P_DATA_LEN, data, HRTO_P_DATA_LEN);
		debug32("HRTO_P_COINBASE: idx: %d, cnt: %d\n", idx, cnt);
		break;
	case HRTO_P_MERKLES:
		memcpy(mw->merkles[idx - 1], data, HRTO_P_DATA_LEN);
		debug32("HRTO_P_MERKLES: idx: %d, cnt: %d\n", idx, cnt);
		break;
	case HRTO_P_HEADER:
		memcpy(mw->header + (idx - 1) * HRTO_P_DATA_LEN, data, HRTO_P_DATA_LEN);
		debug32("HRTO_P_HEADER: idx: %d, cnt: %d\n", idx, cnt);
		break;
	case HRTO_P_POLLING:
		be200_polling();
		break;
	case HRTO_P_REQUIRE:
		break;
	case HRTO_P_SET:
		timer_set(0, IDLE_TIME);
		memcpy(&tmp, data, 4);
		adjust_fan(tmp);
		
		// set asic freq if need
		memcpy(&tmp, data + 4, 4);
		if (tmp > 100 && tmp <= 750 && tmp != g_asic_freq) {
			for (i = 0; i < HRTO_DEFAULT_MINERS; i++) {
				freq_write(i, tmp/10 - 1);  // (X + 1) / 2
			}
			g_asic_freq = tmp;
			debug32("set freq: %d, multi: %d", tmp, tmp/10 - 1);
		}
		
		memcpy(&g_nonce2_offset, data + 8, 4);
		memcpy(&g_nonce2_range, data + 12, 4);

		mw->nonce2 = g_nonce2_offset + g_nonce2_range;

		// reset results ptr
		g_cur_mm_idx = (g_cur_mm_idx + 1) % MM_BUF_NUM;
		g_new_stratum = 1;
		
		debug32("HRTO_P_SET: idx: %d, cnt: %d\n", idx, cnt);
		break;
	case HRTO_P_TARGET:
		memcpy(mw->target, data, HRTO_P_DATA_LEN);
		break;
	case HRTO_P_TEST:
		break;
	case HRTO_P_ITP_TASK:
		debug32("HROT_P_ITP_TASK\n");
		pkg_idx = idx;
		//chip_idx = data[0];
		if(pkg_idx == 1){
			memcpy(itp_data,data+1,24);
			break;
		}
		if(pkg_idx == 2){
			memcpy(itp_data+24,data+1,24);
			uint8_t i;
			for(i=0;i<48;i++){
				debug32("%02x ",itp_data[i]);
			}
			debug32("\n");
			for (i = 0; i < 80; i++) {
				//be200_reset(i);
				freq_write(i, (BE200_DEFAULT_FREQ/10) - 1);  // (X + 1) / 2
			}
			delay(10);
			for(chip_idx = 0;chip_idx < 80;chip_idx++){
				be200_cmd_rd(chip_idx, BE200_REG_CLEAR);  // clear nonce_mask register
				be200_input_task(chip_idx,itp_data);
				be200_start(chip_idx);
			}
			break;
		}
		break;
	case HRTO_P_ITP_RESULT:
		debug32("HROT_P_ITP_RESULT\n");
		memcpy((uint8_t *)&expected_nonce,itp_data+44,4);
		for(chip_idx = 0;chip_idx < 80;chip_idx++){
			ready = be200_get_done(chip_idx, &nonce_mask);
			debug32("chip%02d ready=%d ",chip_idx,ready);
			if (ready == 0){
				itp_result[chip_idx] = 'N';
				debug32("result=%c\n",itp_result[chip_idx]);
				continue;
			}
		
			be200_get_result(chip_idx, nonce_mask, &actual_nonce);
			be200_cmd_rd(chip_idx, BE200_REG_CLEAR);  // clear nonce_mask register
			
			
			//debug32("%d\n",actual_nonce==expected_nonce);
			diff_nonce = (int32_t)actual_nonce - (int32_t)expected_nonce;
			if(diff_nonce >= -4 && diff_nonce <= 4)
				itp_result[chip_idx] = 'R';
			else
				itp_result[chip_idx] = 'E';
			debug32("result=%c\n",itp_result[chip_idx]);
			debug32("a_nonce:%08x e_nonce:%08x\n",actual_nonce,expected_nonce);
			actual_nonce = 0;
		}
		for(chip_idx = 0;chip_idx < 80;chip_idx++){
			uart_write(itp_result[chip_idx]);
		}
		break;
	case HRTO_P_ITP_SINGLE_TASK:
		debug32("HROT_P_ITP_SINGLE_TASK\n");
		pkg_idx = idx;
		//chip_idx = data[0];
		if(pkg_idx == 1){
			memcpy(itp_data,data+1,24);
			break;
		}
		if(pkg_idx == 2){
			memcpy(itp_data+24,data+1,24);
			uint8_t i;
			for(i=0;i<48;i++){
				debug32("%02x ",itp_data[i]);
			}
			debug32("\n");
			for (i = 0; i < 16; i++) {
				//be200_reset(i);
				freq_write(i, (BE200_DEFAULT_FREQ/10) - 1);  // (X + 1) / 2
			}
			delay(10);
			for(chip_idx = 0;chip_idx < 16;chip_idx++){
				be200_cmd_rd(chip_idx, BE200_REG_CLEAR);  // clear nonce_mask register
				be200_input_task(chip_idx,itp_data);
				be200_start(chip_idx);
			}
			break;
		}
		break;
	case HRTO_P_ITP_SINGLE_RESULT:
		debug32("HROT_P_ITP_SINGLE_RESULT\n");
		memcpy((uint8_t *)&expected_nonce,itp_data+44,4);
		for(chip_idx = 0;chip_idx < 16;chip_idx++){
			ready = be200_get_done(chip_idx, &nonce_mask);
			debug32("chip%02d ready=%d ",chip_idx,ready);
			if (ready == 0){
				itp_result[chip_idx] = 'N';
				debug32("result=%c\n",itp_result[chip_idx]);
				continue;
			}
		
			be200_get_result(chip_idx, nonce_mask, &actual_nonce);
			be200_cmd_rd(chip_idx, BE200_REG_CLEAR);  // clear nonce_mask register
			
			
			//debug32("%d\n",actual_nonce==expected_nonce);
			diff_nonce = (int32_t)actual_nonce - (int32_t)expected_nonce;
			if(diff_nonce >= -4 && diff_nonce <= 4)
				itp_result[chip_idx] = 'R';
			else
				itp_result[chip_idx] = 'E';
			debug32("result=%c\n",itp_result[chip_idx]);
			debug32("a_nonce:%08x e_nonce:%08x\n",actual_nonce,expected_nonce);
			actual_nonce = 0;
		}
		for(chip_idx = 0;chip_idx < 16;chip_idx++){
			uart_write(itp_result[chip_idx]);
		}
		break;
	case HRTO_P_ITP_SYNC:
		debug32("HRTO_P_ITP_SYNC\n");
		uart_puts("HRTO");
		break;
	case HRTO_P_ITP_ENTER:
		debug32("HRTO_P_ITP_ENTER\n");
		adjust_fan(1000);
		break;
	case HRTO_P_ITP_EXIT:
		debug32("HRTO_P_ITP_EXIT\n");
		adjust_fan(200);
		break;
	default:
		break;
	}

	return 0;
}
*/


uint32_t be200_send_work(uint8_t idx,struct work *w)
{

        uint8_t i;
        uint8_t last=0;
       

//	be200_cmd_rd(idx, BE200_REG_CLEAR);  // clear nonce_mask register
	       w->data[44]=0xff&(w->nonce2>>24);
	       w->data[45]=0xff&(w->nonce2>>16);
	       w->data[46]=0xff&(w->nonce2>>8);
	       w->data[47]=0xff&(w->nonce2);
	       w->data[48]=w->mm_idx;	
        uart_writecmd(C_JOB|idx);
     
//	uart1_write(0xBB);
        for(i=0;i<49;i++)
   { //   uart1_writecmd(i);       
		
                while(!uart_read_nonblock());
                last = uart_read();
                
                uart_write(w->data[i]);
   }
//      uart1_write(0xBB);
             while(!uart_read_nonblock());
                last = uart_read();
//                uart1_write(last);
           debug32("\nw->data\n");
            hexdump(w->data,49);
               
//      c = be200_cmd_rd(idx, i); 
	//	miner_status[idx].nonce2  = w->nonce2;
	//	miner_status[idx].mm_idx  = w->mm_idx;
		return 1;
	}

	uint32_t be200_read_result()
	{
		uint8_t idx;
		uint8_t ready;
		uint8_t nonce_mask;
		uint32_t nonce = 0, nonce_new;
		int32_t nonce_check = NONCE_HW;
		struct be200_result *data;
		
		int i, found = 0;
		int8_t diff_nonce[] = {0, -1, 1, -2, 2, -3, 3, 4, -4};

		for (idx = 0; idx < CHIP_NUMBER; idx++) {
			ready = be200_get_done(idx, &nonce_mask);
			if (ready == 0)
				continue;
		
			be200_get_result(idx, nonce_mask, &nonce);
			debug32("chip chenquan idx: %d, nonce: %08x, mm_idx: %02x\n", idx, nonce, miner_status[idx].mm_idx);
		
			
			/* check the validation of the nonce*/
			for (i = 0; i < sizeof(diff_nonce)/sizeof(diff_nonce[0]); i++) {
				nonce_new = nonce + diff_nonce[i];
	//			nonce_check = test_nonce(&g_mm_works[miner_status[idx].mm_idx],result,miner_status[idx].nonce2, nonce_new);
				if (nonce_check == NONCE_DIFF) {
					nonce = nonce_new;
					found = 1;
					break;
				}
			}
			
			g_local_work++;
			
			if (likely(nonce_check == NONCE_DIFF)) {
				g_total_nonce++;
			
				/* put the valid nonce into be200 ring buffer */
				data = &be200_result_buff[be200_ret_produce];
				be200_ret_produce = (be200_ret_produce + 1) & BE200_RET_RINGBUFFER_MASK_RX;

				data->idx = idx;
				data->nonce2 = miner_status[idx].nonce2;
				data->mm_idx = miner_status[idx].mm_idx;
				data->nonce  = nonce;
				
				debug32("be200_read_result, g_local_work: %d, miner: %d, "
						"mm_idx: %02x, nonce2: %08x, nonce: %08x, total:%d\n",
						g_local_work, data->idx, data->mm_idx,
						data->nonce2, data->nonce, g_total_nonce);
			}
			else if (unlikely(!found) /* NONCE_HW */) {
				g_hw_work++;
				debug32("========= invalid chenquan  nonce =========\n");
			}
		} /* /for */
		
		return 0;
	}

	static int get_result(int board)
	{
		static char result[54];
	//        int idx;
		
//	       struct lm32_uart *uart1 = (struct lm32_uart *)UART0_BASE;

		uint32_t nonce, nonce_new,nonce2,mm_idx;
		int32_t nonce_check = NONCE_HW;

		int i;
	        int found = 0;
		int8_t diff_nonce[] = {0, -1, 1, -2, 2, -3, 3, 4, -4};

		      for(i=0;i<54;i++)
		     {	
	                  while(!uart_read_nonblock());
                          result[i] = uart_read();
                        //  uart1_write(result[i]);
		       }                                   
                                   debug32("\nresult\n");
                                   hexdump((const uint8 *)result,54);
 		    		        
				  nonce2 = ( ( (uint32_t)result[44] << 24)  |
					     ( (uint32_t)result[45] << 16)  |
					     ( (uint32_t)result[46] << 8 )  |
					     ( (uint32_t)result[47]      )   ) ;
				  mm_idx= result[52];
				  nonce = (  ( (uint32_t)result[51] << 24)  |
					     ( (uint32_t)result[50] << 16)  |
					     ( (uint32_t)result[49] << 8 )  |
					     ( (uint32_t)result[48]      )   ) + 1;
			        
				for (i = 0; i < sizeof(diff_nonce)/sizeof(diff_nonce[0]); i++) {
				nonce_new = nonce + diff_nonce[i];
                                debug32("\ntest_nonce  before i=%0x \n",i);
                                
				nonce_check = test_nonce(&g_mm_works[0],result,nonce2, nonce_new);          //打印test 是这一段函数     
		                  debug32("\ntest_nonce  after\n");

                       		if (nonce_check == NONCE_DIFF ) {
					nonce = nonce_new;
					found = 1;
					break;
				}
			}
                        if(nonce_check == NONCE_DIFF)
		         debug32("\nnonce ok\n");
                        else if(nonce_check != NONCE_DIFF)
                         debug32("\ninvalid nonce \n");

	
		   //回送nonce给矿池  
	  return found;

	}


	/*
<<<<<<< HEAD
	static int get_pkg()
	{
		static char pre_last, last;
		static int start = 0, count = 2;

		while (1) {
			if (!uart_read_nonblock() && !start)
				break;

			pre_last = last;
			last = uart_read();

			if (start)
				g_pkg[count++] = last;

			if (count == HRTO_P_COUNT) {
				pre_last = last = 0;

				start = 0;
				count = 2;
			
			if (pre_last == HRTO_H1 && last == HRTO_H2 && !start) 
				g_pkg[0] = pre_last;
				g_pkg[1] = last;
				start = 1;
				count = 2;
			}
		}

		return 0;
	}
	*/
	void set_default_freq() {
		int idx;
		for (idx = 0; idx < 80; idx++) {
			freq_write(idx, (BE200_DEFAULT_FREQ/10) - 1);  // (X + 1) / 2
		}
	}

	void set_all_chips_idle() {
		int i;
	       // uint8_t c=0xe0;
	       // uint8_t d=0x40;
		for (i = 0; i < HRTO_DEFAULT_MINERS; i++) {
			uart_writecmd(0xe0|i/24);  
			uart_write(0x40|i%24);
			
			uart_writecmd(0xe0|i%24);
			uart_write(0x20|i%24);
			uart_write(0x11);               

	//		be200_reset(i);
	//		freq_write(i, (BE200_DEFAULT_FREQ/10) - 1);  // (X + 1) / 2
/*
=======
	memcpy(mm_work_ptr->coinbase,coinbase,117);
	mm_work_ptr->coinbase_len = 117;
	
	mm_work_ptr->nonce2_offset = 62;
	
	memcpy(mm_work_ptr->merkles,merkle_branch,160);
	mm_work_ptr->nmerkles = 5;
	mm_work_ptr->merkle_offset = 36;
	miner_gen_nonce2_work(mm_work_ptr, 0xaaaaaaaa, &g_works[0]);
	
	if(do_dns(DEFAULT_DNS,DOMAIN,RIP))
		debug32("parse ok.\n");
	else
		debug32("parse failed.\n");
    connect_poll(RIP,3333);
	send_subscribe();
	send_authorize();
	while(1){
		recv_stratum(&g_mm_works[0]);
		if(g_new_stratum){
			miner_gen_nonce2_work(mm_work_ptr, 0, &g_works[0]);
			g_new_stratum = 0;
>>>>>>> feature/generate_midstate
*/
		}
		delay(10);
	}

	int main(int argv,char * * argc)
	{
//		struct work work;
		uint16_t idx=0,last=0;
        uint32_t nonce2=0;
        uint8 txsize[8] = {2,2,2,2,2,2,2,2};/*给每个socket配置一个2KB的发送内存*/
        uint8 rxsize[8] = {2,2,2,2,2,2,2,2};/*给每个socket配置一个2KB的接收内存*/		
	irq_setmask(0);
	irq_enable(1);		
	uart_init();
	uart1_init();   	

        W5500_Init();
        setRTR(2000);//设置溢出时间值
        setRCR(3);//设置最大重新发送次数
        sysinit(txsize, rxsize);//初始化8个socket
        mm_work_ptr = &g_mm_works[0];
        if(do_dns(DEFAULT_DNS,DOMAIN,RIP))
                debug32("parse ok.\n");
        else
                debug32("parse failed.\n");
		connect_poll(RIP,3333);
        send_subscribe();
        send_authorize();
		g_working = 1; 
 
               debug32("\n55 66 77\n");   	
//       for(i=0;i<6;i++)
//       be200_send_work(idx,&g_works[0]);

	while (1) {// if(flag[idx]==0)
                 {               
		wdg_feed_sec(60);
                uart_writecmd(C_ASK|idx);
                while(!uart_read_nonblock());
                last = uart_read();
                if(last== A_YES)
                debug32("\ncmd:  %0x\n",last); 
                  if(last == A_YES) {                       
                           debug32("\njiaoyan result  %0x\n",get_result(idx)); 
                         //  uart1_write(get_result(idx));
                            continue;}
                         else if(last== A_NO)
                            { continue;}
                            else if(last== A_WAL)
                             { 
                                recv_stratum(&g_mm_works[0]);
                              if(g_new_stratum){
                                miner_gen_nonce2_work(mm_work_ptr, nonce2, &g_works[0]);
                                nonce2++; 
                             //   uart1_write(nonce2&0xff);
	 /*		mw = &g_mm_works[g_cur_mm_idx];
			mw->nonce2++;
			miner_gen_nonce2_work(mw, mw->nonce2, &work);
			work.mm_idx = g_cur_mm_idx;*/
                      //  wdg_feed_sec(60);
			be200_send_work(idx,&g_works[0]); 
                              }     } 
                 else {uart_writecmd(0x00);delay(100);}
                }  //end if  
                   //     idx++;
                   //     if(idx==4) idx=0;	
		//be200_read_result();
	} // while(1) 	
	return 0;
}
/*
int main1(int argv,char * * argc)

{
	uint8_t c;
//        struct lm32_uart *uart2 = (struct lm32_uart *)UART0_BASE;
	irq_setmask(0);
	irq_enable(1);
	
	uart_init();
	uart1_init();
        c = 0xa0;
       while(1)
     {
//	debug32("Init.\n");

      //  uart1_write(c);
       delay(1);
        uart1_writecmd(c);
     //  delay(1000);
      //  uart_write(c);
          delay(1);
        uart_writecmd(c);
      debug32("\n");
       delay(10000);

     //   uart_write(c);
//	if(readb(&uart2->lsr) & LM32_UART_LSR_DR) 
//	uart1_write(readb(&uart2->rxtx));
    
     }
//	uart1_writecmd(c);
        be200_reset(2);
      //  be200_cmd_wr(2,45,0x80|27);	
      //  be200_cmd_wr(2,45,27);


//      be200_mine(2);
//

	return 0;
}
*/
