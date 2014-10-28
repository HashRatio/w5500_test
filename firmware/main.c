/*
 * Author: Minux
 * Author: Xiangfu Liu <xiangfu@openmobilefree.net>
 * Bitcoin: 1CanaaniJzgps8EV6Sfmpb7T8RutpaeyFn
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
#define IDLE_TIME   60  /* Seconds */
char result[54];
//int8 buffer[BUFFER_SIZE];/*定义一个2KB的缓存*/
uint8 mac[6] = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x03}; /*定义Mac变量*/
uint8 ip[4] = {192, 168, 2, 200}; /*定义Ip变量*/
uint8 sn[4] = {255, 255, 255, 0}; /*定义Subnet变量*/
uint8 gw[4] = {192, 168, 2, 1}; /*定义Gateway变量*/
uint8 dip[4] = {192, 168, 2, 116};
uint8 DEFAULT_DNS[4] = {192, 168, 2, 1};
uint8 RIP[4] ;
//uint8 DOMAIN[] = "uk1.ghash.io";
uint8 DOMAIN[] = "stratum.f2pool.com";
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

uint32_t be200_send_work(struct work *w)
{
    uint8_t last = 0;
    w->data[44] = 0xff & (w->nonce2 >> 24);
    w->data[45] = 0xff & (w->nonce2 >> 16);
    w->data[46] = 0xff & (w->nonce2 >> 8);
    w->data[47] = 0xff & (w->nonce2);
    w->data[48] = w->mm_idx;
    uart_writecmd(C_JOB, 1);
    uart_nwrite((const char *)w->data, 49);
    last = uart_read();
    debug32("Read in be200_send_work:0x%02x\n",last);
    return 1;
}

// static int get_result(uint32 * ptr_ntime, uint32 * ptr_nonce, uint32 * ptr_nonce2)
// {
    // uint32_t nonce_new, mm_idx;
    // int32_t nonce_check = NONCE_HW;
    // int i;
    // int8_t diff_nonce[] = {0, -1, 1, -2, 2, -3, 3, 4, -4};
    
    // int32 data_to_read;
    
    // data_to_read = uart_read_nonblock();
    
    // while(data_to_read < 54)
    // {
        // data_to_read = uart_read_nonblock();
        
    // }
    // for (i = 0; i < 54; i++)
    // {
        
        // result[i] = uart_read();
    // }

    // *ptr_ntime = (((uint32_t)result[36] << 24)  |
                  // ((uint32_t)result[37] << 16)  |
                  // ((uint32_t)result[38] << 8)  |
                  // ((uint32_t)result[39])) ;

    // *ptr_nonce2 = (((uint32_t)result[44] << 24)  |
                   // ((uint32_t)result[45] << 16)  |
                   // ((uint32_t)result[46] << 8)  |
                   // ((uint32_t)result[47])) ;

    // *ptr_nonce = (((uint32_t)result[51] << 24)  |
                  // ((uint32_t)result[50] << 16)  |
                  // ((uint32_t)result[49] << 8)  |
                  // ((uint32_t)result[48])) + 1;

    // mm_idx = result[52];

    // for (i = 0; i < sizeof(diff_nonce) / sizeof(diff_nonce[0]); i++)
    // {
        // nonce_new = *ptr_nonce + diff_nonce[i];
        // nonce_check = test_nonce(mm_work_ptr, result, *ptr_nonce2, nonce_new);       //打印test 是这一段函数
        // if (nonce_check != NONCE_HW)
        // {
            // *ptr_nonce = nonce_new;
            // break;
        // }
    // }
    // return nonce_check;
// }

int main(int argv, char * * argc)
{
    // uint32_t nonce2 = 0;
    // int32 nonce_check;
    // uint32 nonce_submit, nonce2_submit, ntime_submit;
    uint8 txsize[8] = {2, 2, 2, 2, 2, 2, 2, 2}; /*给每个socket配置一个2KB的发送内存*/
    uint8 rxsize[8] = {2, 2, 2, 2, 2, 2, 2, 2}; /*给每个socket配置一个2KB的接收内存*/
    irq_setmask(0);
    irq_enable(1);
    uart_init();
    uart1_init();
    debug32("Init done.\n");
    W5500_Init();
    setRTR(2000);//设置溢出时间值
    setRCR(3);//设置最大重新发送次数
    sysinit(txsize, rxsize);//初始化8个socket
    mm_work_ptr = &g_mm_works[0];
    memcpy(mm_work_ptr->target, g_diff256_target, 32);

    if (do_dns(DEFAULT_DNS, DOMAIN, RIP))
        debug32("parse ok.\n");
    else
        debug32("parse failed.\n");
    connect_poll(RIP, 3333);
    send_subscribe();
    send_authorize();
    g_working = 1;
    
    uart_writecmd(C_RES, 1);
    uart_writecmd(C_LPO, 1);
    uart_writecmd(C_DIF | 0x00, 1);
    uart_writecmd(C_GCK | (uint8)29,1);
    while (1)  // if(flag[idx]==0)
    {
        //debug32("do_http()\n");
        do_http();
        //debug32("recv_stratum(&g_mm_works[0])\n");
        recv_stratum(&g_mm_works[0]);
        //debug32("tube_handler(0)\n");
        tube_handler(0);
    } // while(1)
    return 0;
}
