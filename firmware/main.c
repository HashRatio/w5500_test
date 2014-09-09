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
uint8 buffer[2048];/*定义一个2KB的缓存*/
uint8 mac[6]={0x00,0x08,0xDC,0x01,0x02,0x03};/*定义Mac变量*/
uint8 ip[4]={192,168,2,200};/*定义Ip变量*/
uint8 sn[4]={255,255,255,0};/*定义Subnet变量*/
uint8 gw[4]={192,168,2,1};/*定义Gateway变量*/
uint8 dip[4]={192,168,2,116};
//uint8 DEFAULT_DNS[4] = {192,168,2,1};
uint8 RIP[4];
uint8 DOMAIN[] = "stratum.f2pool.com";
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
	
	// while(1)
   // {
      // switch(getSn_SR(SOCK_STRATUM))/*获取socket0的状态*/
      // {
         // case SOCK_INIT:/*socket初始化完成*/
           // connect(SOCK_STRATUM, RIP ,3333);/*在TCP模式下向服务器发送连接请求*/
           // debug32("SOCK_INIT\n");
           // break;
         // case SOCK_ESTABLISHED:/*socket连接建立*/
           // if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
           // {
              // setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
            //}
			// memcpy(buffer,"{\"id\": 1, \"method\": \"mining.subscribe\", \"params\": []}\n",100);
			// send(SOCK_STRATUM,buffer,60,(bool)0);/*W5200向Server发送数据*/
           // len=getSn_RX_RSR(SOCK_STRATUM);/*len为已接收数据的大小*/
           // if(len>0)
           // {
              // recv(SOCK_STRATUM,buffer,len);/*W5200接收来自Sever的数据*/
			  // debug32("%s",buffer);
            // }
			
           // break;
         // case SOCK_CLOSE_WAIT:/*socket等待关闭状态*/
           // debug32("SOCK_CLOSE_WAIT\n");
		   // break;
         // case SOCK_CLOSED:/*socket关闭*/
		   // debug32("SOCK_CLOSED\n");
           // socket(SOCK_STRATUM,Sn_MR_TCP,anyport++,Sn_MR_ND);/*打开socket0的一个端口*/
           // break;
     // }
   // }
   
    connect_poll(RIP,3333);
	send_subscribe();
	send_authorize();
	while(1)
		recv_stratum();
	//while(!recv_stratum());
	
	// while(1)
		// recv_stratum();
	
	char *js = "{\"id\": 1, \"method\": \"mining.notify\", \"params\": [\"name\":\"hyper\"],\"params\": [\"name\":null]}";
	// char o[128];
	// jsmn_parser jp;
	// jsmntok_t jt[32];
	// jsmnerr_t je;
	
	// debug32("strlen:%d\n",strlen(js));
	// jsmn_init(&jp);
	// int IDX = 0;
	// je = jsmn_parse(&jp,js,strlen(js),jt, 32);
	// while(1){	
		// if(IDX > je) break;
		// memset(o,0,128);
		// memcpy(o,js+jt[IDX].start,jt[IDX].end-jt[IDX].start);
		// debug32("err:%d start:%d end:%d size:%d type:%d value:%s\n ",je,jt[IDX].start,jt[IDX].end,jt[IDX].size,jt[IDX].type,o);
		// IDX++;
	// }
	parse_stratum(js);
	debug32("done");
	return 0;
}

