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

#include "driver-tube.h"

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
uint8 DOMAIN[] = "uk1.ghash.io";
//uint8 DOMAIN[] = "stratum.f2pool.com";
//uint8 DOMAIN[] = "182.92.180.216";

int8 g_new_stratum = 0;
static int g_working = 0;

#define BE200_RET_RINGBUFFER_SIZE_RX 64
#define BE200_RET_RINGBUFFER_MASK_RX (BE200_RET_RINGBUFFER_SIZE_RX-1)

static volatile unsigned int ret_produce = 0;
static volatile unsigned int ret_consume = 0;

static volatile unsigned int be200_ret_produce = 0;
static volatile unsigned int be200_ret_consume = 0;


void w5500_init(void)
{
    uint8 txsize[8] = {2, 2, 2, 2, 2, 2, 2, 2}; /*给每个socket配置一个2KB的发送内存*/
    uint8 rxsize[8] = {2, 2, 2, 2, 2, 2, 2, 2}; /*给每个socket配置一个2KB的接收内存*/
    
    iinchip_hw_init();
    setSHAR(mac);/*配置Mac地址*/
    setSIPR(ip);/*配置Ip地址*/
    setSUBR(sn);/*配置子网掩码*/
    setGAR(gw);/*配置默认网关*/
    
    setRTR(2000);//设置溢出时间值
    setRCR(3);//设置最大重新发送次数
    sysinit(txsize, rxsize);//初始化8个socket
}

void lm32_init()
{
    irq_setmask(0);
    irq_enable(1);
    uart_init();
    uart1_init();
    debug32("Init done.\n"); 
}

int main(int argv, char * * argc)
{   
    lm32_init();
    w5500_init();
   
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

    tube_init();
    int loop = 0;
    uint64 i = 0;
    debug32("len:%d\n",sizeof(i));
    while (1)
    {
        loop++;
        if (loop%500 == 0)
        {
            debug32(".");
        }
        if (loop%1000 == 0)
        {
            tube_status();
            loop = 0;
        }
        recv_stratum(&g_mm_works[0]);
        tube_handler(do_http);
    } // while(1)
    return 0;
}
