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
#include "flash.h"
#include "httpd.h"
#include "httputil.h"


#define MM_BUF_NUM  3
#define IDLE_TIME	60	/* Seconds */
char result[54];
//int8 buffer[BUFFER_SIZE];/*定义一个2KB的缓存*/
uint8 mac[6]={0x00,0x08,0xDC,0x01,0x02,0x03};/*定义Mac变量*/
uint8 ip[4]={192,168,2,200};/*定义Ip变量*/
uint8 sn[4]={255,255,255,0};/*定义Subnet变量*/
uint8 gw[4]={192,168,2,1};/*定义Gateway变量*/
uint8 dip[4]={192,168,2,116};
uint8 DEFAULT_DNS[4] = {192,168,2,1};
uint8 RIP[4] ;
uint8 DOMAIN[] = "us1.ghash.io";
//uint8 DOMAIN[] = "182.92.180.216";

int8 g_new_stratum = 0;
static int g_working = 0;

#define BE200_RET_RINGBUFFER_SIZE_RX 64
#define BE200_RET_RINGBUFFER_MASK_RX (BE200_RET_RINGBUFFER_SIZE_RX-1)

static volatile unsigned int ret_produce = 0;
static volatile unsigned int ret_consume = 0;

static volatile unsigned int be200_ret_produce = 0;
static volatile unsigned int be200_ret_consume = 0;

//int buffer_test[1024] __attribute__((section(".bufdata")));

//int8 buffer[BUFFER_SIZE];/*定义一个2KB的缓存*

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
        uart_writecmd(idx,C_JOB);
     
//	uart1_write(0xBB);
        for(i=0;i<49;i++)
   { //   uart1_writecmd(i);       
		
                //while(!uart_read_nonblock());
                last = uart_read(idx);
                
                uart_write(idx,w->data[i]);
   }
//      uart1_write(0xBB);
          //   while(!uart_read_nonblock());
                last = uart_read(idx);
//                uart1_write(last);
           debug32("\nw->data\n");
            hexdump(w->data,49);
               
//      c = be200_cmd_rd(idx, i); 
	//	miner_status[idx].nonce2  = w->nonce2;
	//	miner_status[idx].mm_idx  = w->mm_idx;
		return 1;
	}

static int get_result(int board,uint32 * ptr_ntime,uint32 * ptr_nonce,uint32 * ptr_nonce2)
{	
	uint32_t nonce_new,mm_idx;
	int32_t nonce_check = NONCE_HW;
	int i;
	int8_t diff_nonce[] = {0, -1, 1, -2, 2, -3, 3, 4, -4};

	for(i=0;i<54;i++)
	{	
		result[i] = uart_read(board);
	}	
	
	*ptr_ntime = ( ( (uint32_t)result[36] << 24)  |
		 ( (uint32_t)result[37] << 16)  |
		 ( (uint32_t)result[38] << 8 )  |
		 ( (uint32_t)result[39]      )   ) ;
	
	*ptr_nonce2 = ( ( (uint32_t)result[44] << 24)  |
		 ( (uint32_t)result[45] << 16)  |
		 ( (uint32_t)result[46] << 8 )  |
		 ( (uint32_t)result[47]      )   ) ;
	
	*ptr_nonce = (  ( (uint32_t)result[51] << 24)  |
		 ( (uint32_t)result[50] << 16)  |
		 ( (uint32_t)result[49] << 8 )  |
		 ( (uint32_t)result[48]      )   ) + 1;
	
	mm_idx = result[52];
	
	for (i = 0; i < sizeof(diff_nonce)/sizeof(diff_nonce[0]); i++) {
		nonce_new = *ptr_nonce + diff_nonce[i];
		//debug32("\ntest_nonce  i=%0x \n",i);		
		nonce_check = test_nonce(mm_work_ptr,result,*ptr_nonce2,nonce_new);          //打印test 是这一段函数     
		if (nonce_check != NONCE_HW ) {
			*ptr_nonce = nonce_new;
			break;
		}
	}
	return nonce_check;
}

int main(int argv,char * * argc)
{
    uint16_t idx=1,last=0;
    uint32_t nonce2=0;
	int32 nonce_check;
	uint32 nonce_submit,nonce2_submit,ntime_submit;
    uint8 txsize[8] = {2,2,2,2,2,2,2,2};/*给每个socket配置一个2KB的发送内存*/
    uint8 rxsize[8] = {2,2,2,2,2,2,2,2};/*给每个socket配置一个2KB的接收内存*/		
	irq_setmask(0);
	irq_enable(1);		
	uart_init();
	uart1_init();   

    W5500_Init();
	debug32("Init done.\n");
    setRTR(2000);//设置溢出时间值
    setRCR(3);//设置最大重新发送次数
    sysinit(txsize, rxsize);//初始化8个socket
    mm_work_ptr = &g_mm_works[0];
	memcpy(mm_work_ptr->target,g_diff256_target,32);
      
    if(do_dns(DEFAULT_DNS,DOMAIN,RIP))
        debug32("parse ok.\n");
    else
        debug32("parse failed.\n");
    connect_poll(RIP,3333);
    send_subscribe();
    debug32("send subscribe.\n");
    send_authorize();
    debug32("send authorize.\n");
    g_working = 1; 
 
	uart_writecmd(idx,C_LPO);
	uart_writecmd(idx,C_DIF|0x00);

	while (1) {// if(flag[idx]==0)
        do_http();
        recv_stratum(&g_mm_works[0]);     
        continue;
		wdg_feed_sec(60);
        uart_writecmd(idx,C_ASK);
        while(!uart_read_nonblock());
        last = uart_read(idx);
		if(last == A_YES){
			nonce_check = get_result(idx,&ntime_submit,&nonce_submit,&nonce2_submit); 
			if(nonce_check == NONCE_VALID){
				send_submit(mm_work_ptr, nonce2_submit, ntime_submit, bswap_32(nonce_submit));
				calc_hashrate();
			}
			else if(nonce_check == NONCE_DIFF)
				;//debug32("\nLESS DIFF NONCE\n");
			continue;
		}
        else if(last== A_NO){ 
            continue;
        }
        else if(last== A_WAL)
        {
          if(g_new_stratum){
            miner_gen_nonce2_work(mm_work_ptr, nonce2, &g_works[0]);
            nonce2++;
			be200_send_work(idx,&g_works[0]);
            g_new_stratum = 1; 
            }     
        } 
        else
        {
            uart_writecmd(idx,0x00);
            delay(100);
        }
	} // while(1) 	
	return 0;
}
