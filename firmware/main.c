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
//#include <stdio.h>
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
#include "protocol.h"
#include "crc.h"
#include "miner.h"
#include "hexdump.c"
#include "utils.h"
#include "spi.h"
#include "w5500.h"
#include "socket.h"
#include "dns.h"
#include "jsmn.h"
#include "stratum.h"

#define MM_BUF_NUM  3
#define IDLE_TIME	60	/* Seconds */

#define CH 0

//uint8 buffer[];/*定义一个2KB的缓存*/
uint8 mac[6]={0x00,0x08,0xDC,0x01,0x02,0x03};/*定义Mac变量*/
uint8 ip[4]={192,168,2,200};/*定义Ip变量*/
uint8 sn[4]={255,255,255,0};/*定义Subnet变量*/
uint8 gw[4]={192,168,2,1};/*定义Gateway变量*/
uint8 dip[4]={192,168,2,116};
uint8 DEFAULT_DNS[4] = {192,168,2,1};
uint8 RIP[4];
uint8 DOMAIN[] = "stratum.f2pool.com";
//int8 a[40];

void W5500_Init(void)
{
	iinchip_hw_init();
	setSHAR(mac);/*配置Mac地址*/
	setSIPR(ip);/*配置Ip地址*/
	setSUBR(sn);/*配置子网掩码*/
	setGAR(gw);/*配置默认网关*/
}


int main(int argv,char * * argc)
{	
	
	irq_setmask(0);
	irq_enable(1);
	
	uart_init();
	uart1_init();
	
	debug32("Init.\n");
	
	uint8 txsize[8] = {2,2,2,2,2,2,2,2};/*给每个socket配置一个2KB的发送内存*/
	uint8 rxsize[8] = {2,2,2,2,2,2,2,2};/*给每个socket配置一个2KB的接收内存*/
	
	W5500_Init();
	
	setRTR(2000);/*设置溢出时间值*/
	setRCR(3);/*设置最大重新发送次数*/
	sysinit(txsize, rxsize);/*初始化8个socket*/
	
	curr_mm_work = g_mm_works;
	
	if(do_dns(DEFAULT_DNS,DOMAIN,RIP))
		debug32("parse ok.\n");
	else
		debug32("parse failed.\n");
	
    connect_poll(RIP,3333);
	send_subscribe();
	send_authorize();
	while(1)
		recv_stratum(&g_mm_works[0]);

	return 0;
}

