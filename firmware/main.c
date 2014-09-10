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
//uint8 DEFAULT_DNS[4] = {192,168,2,1};
uint8 RIP[4];
uint8 DOMAIN[] = "stratum.f2pool.com";
int8 a[BUFFER_SIZE];

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
	//int32 len;
	//uint16 anyport=30000;
	
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
	
	uint8 dns_retry_cnt=0;
	uint8 dns_ok=0;
	curr_mm_work = g_mm_works;
	while(1)
	{ 
		if( (dns_ok==1) ||  (dns_retry_cnt > DNS_RETRY))
		{
			return 0;
		}

		else if(memcmp(DEFAULT_DNS,"\x00\x00\x00\x00",4))
		{
			switch(dns_query(SOCK_DNS,DOMAIN))
			{
			  case DNS_RET_SUCCESS:
				dns_ok=1;
				memcpy(RIP,DNS_GET_IP,4);
				dns_retry_cnt=0;
				debug32("Get [%s]'s IP address [%d.%d.%d.%d] from %d.%d.%d.%d\r\n",DOMAIN,RIP[0],RIP[1],RIP[2],RIP[3],DEFAULT_DNS[0],DEFAULT_DNS[1],DEFAULT_DNS[2],DEFAULT_DNS[3]);
				break;
			  case DNS_RET_FAIL:
				dns_ok=0;
				dns_retry_cnt++;
				debug32("Fail! Please check your network configuration or DNS server.\r\n");
				break;
			  default:
				break;
			}
		}
		else
			debug32("Invaild DNS server.\n");
		if(RIP[0]!=0 && RIP[1]!=0 && RIP[2]!=0 && RIP[3]!=0)
			break;
	}
   
    // connect_poll(RIP,3333);
	// send_subscribe();
	// send_authorize();
	// while(1)
		// recv_stratum(&g_mm_works[0]);

	return 0;
}

