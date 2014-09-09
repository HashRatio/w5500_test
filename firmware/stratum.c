#include <stdlib.h>

#include "minilibc.h"
#include "defines.h"
#include "types.h"
#include "w5500.h"
#include "socket.h"
#include "stratum.h"
#include "jsmn.h"
#include "miner.h"

#define BUFFER_SIZE 2048
static jsmn_parser jp;
static jsmntok_t jt[TOKEN_BUFFER];
static jsmnerr_t je;
static int8 buffer[BUFFER_SIZE];
static int32 pkg_id = 0;

static int32 json_begin = 0;
static int32 json_end = 0;
//static int32 json_ptr = 0;
static int32 notify_cnt = 0;
int32 flag_diff;
int32 flag_notify;

int32 current_diff;
struct pool_task{
	
};

mm_work g_mm_works[1];

int32 connect_poll(uint8 * addr, uint16 port)
{
    uint16 anyport = 30000;
	while(1){
		switch(getSn_SR(SOCK_STRATUM))/*获取socket0的状态*/
		{
			case SOCK_INIT:/*socket初始化完成*/
				debug32("SOCK_INIT\n");
				connect(SOCK_STRATUM, addr ,port);/*在TCP模式下向服务器发送连接请求*/
				break;
			case SOCK_ESTABLISHED:/*socket连接建立*/
				debug32("SOCK_ESTABLISHED\n");
				debug32("Successfully connect pool:%d.%d.%d.%d:%d\n",addr[0],addr[1],addr[2],addr[3],port);
				return 1;
			case SOCK_CLOSE_WAIT:/*socket等待关闭状态*/
				debug32("SOCK_CLOSE_WAIT\n");
			    break;
			case SOCK_CLOSED:/*socket关闭*/
				debug32("SOCK_CLOSED\n");
			    socket(SOCK_STRATUM,Sn_MR_TCP,anyport++,Sn_MR_ND);/*打开socket的一个端口*/
			    break;
		 }
	}
}

int32 send_subscribe()
{
	pkg_id++;
	memset(buffer,0,BUFFER_SIZE);
	m_sprintf(buffer,"{\"id\": %d, \"method\": \"mining.subscribe\", \"params\": [\"hashratio miner\"]}\n", pkg_id);
	debug32("%s",buffer);
	if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
    {
        setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
    }
	send(SOCK_STRATUM,(uint8*)buffer,strlen(buffer),0);
	return 0;
}

int32 send_authorize()
{
	pkg_id++;
	memset(buffer,0,BUFFER_SIZE);
	m_sprintf(buffer,"{\"params\": [\"whb.miner1\", \"password\"], \"id\": %d, \"method\": \"mining.authorize\"}\n", pkg_id);
	debug32("%s",buffer);
	if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
    {
        setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
    }
	send(SOCK_STRATUM,(uint8*)buffer,strlen(buffer),0);
	return 0;
}

int32 send_submit()
{
	return 0;
}

int32 recv_stratum()
{
    int32 len;
	int32 json_len;
	//int32 end = 0;
	// switch(getSn_SR(SOCK_STRATUM))/*获取socket0的状态*/
    // {
		// case SOCK_INIT:/*socket初始化完成*/
			// debug32("SOCK_INIT\n");
			// break;
		// case SOCK_ESTABLISHED:/*socket连接建立*/
			// debug32("SOCK_ESTABLISHED\n");
			// break;
		// case SOCK_CLOSE_WAIT:/*socket等待关闭状态*/
			// debug32("SOCK_CLOSE_WAIT\n");
			// break;
		// case SOCK_CLOSED:/*socket关闭*/
			// debug32("SOCK_CLOSED\n");
	// }
	
    len=getSn_RX_RSR(SOCK_STRATUM);/*len为已接收数据的大小*/
	
    if(len>0){
		//debug32("\n******************recv_stratum***json_end:%d******************\n",json_end);	
        recv(SOCK_STRATUM,(uint8*)buffer+json_end,len);/*W5200接收来自Sever的数据*/
		len+=json_end;
		json_begin = json_end = 0;
		while(1){
			//if(buffer[json_ptr] != '{')
			//	break;
			if(buffer[json_end] == '\n'){
				buffer[json_end] = '\0';
				//debug32("end: %d\n",json_end);
				json_len = json_end - json_begin;
				//debug32("ready to parse:%s\n",buffer+json_begin);
				parse_stratum(buffer+json_begin);
				json_begin = json_end + 1;
			}
			if(json_end == len - 1){
				if(buffer[json_end] == '\n' || buffer[json_end] == '\0' ){
					debug32("!!!!!good end.\n");
					memset(buffer,0,BUFFER_SIZE);
					json_begin = 0;
					json_end = 0;
				}
				else{
					//debug32("buffer:%s\n",buffer);
					//debug32("json_end:%d end:%c 0x%x\n",json_end,buffer[json_end],buffer[json_end]);
					debug32("!!!!!bad end.\n");
					memcpy(buffer,buffer+json_begin,len-json_begin);
					json_end = len-json_begin;
				}
				break;
			}
			json_end++;
		}
		return 1;
	}
	return 0;
}

int32 parse_nofify(const int8 * json)
{
	int32 idx = 0;
	notify_cnt++;
	debug32("parse_notify:%d\n",notify_cnt);
/* 	uint8 job_id[4];

	size_t coinbase_len;
	uint8 coinbase[HRTO_P_COINBASE_SIZE];

	uint32 nonce2;
	int nonce2_offset;
	int nonce2_size;

	int merkle_offset;
	int nmerkles;
	uint8 merkles[HRTO_P_MERKLES_COUNT][32];

	uint8 header[128];

	uint32 diff;
	uint32 pool_no;

	uint8	target[32]; */
	while(1){
		if(idx >= je)
			break;
		if(strncmp(json+jt[idx].start,"params",6) == 0){
			debug32("params:%d\n",idx);
			idx++;
			debug32("%s\n",json+jt[idx].start);
			idx++;
			break;
		}
		idx++;
	}
	/*job_id*/
	memcpy(g_mm_works[0].job_id,json+jt[idx].start,jt[idx].end-jt[idx].start);
	debug32("job_id:%s\n",g_mm_works[0].job_id);
	idx++;
	
	/*prehash*/
	idx++;
	
	/*coinb1*/
	idx++;
	
	/*coinb2*/
	idx++;
	
	/*merkel_branch*/
	idx+=jt[idx].size+1;
	
	/*version*/
	memcpy(g_mm_works[0].header,json+jt[idx].start,jt[idx].end-jt[idx].start);
	debug32("version:%s\n",g_mm_works[0].header);
	idx++;
	
	/*nbits*/
	memcpy(g_mm_works[0].target,json+jt[idx].start,jt[idx].end-jt[idx].start);
	debug32("nbits:%s\n",g_mm_works[0].target);
	idx++;
	
	
	
	return 0;
}

int32 parse_diff(const int8 * json)
{
	debug32("diff\n");
	return 0;
}

int32 parse_result(const int8 * json)
{
	debug32("result\n");
	return 0;
}

int32 parse_stratum(const int8 * json)
{
	int idx = 0;

	jsmn_init(&jp);
	je = jsmn_parse(&jp,json,strlen(json),jt, TOKEN_BUFFER);
	
	while(1){
		if(idx >= je)
			break;
		//memset(buffer,0,BUFFER_SIZE);
		//memcpy(buffer,json+jt[idx].start,jt[idx].end-jt[idx].start);
		//buffer[jt[idx].end= = '\0';
		//debug32("idx:%d err:%d start:%d end:%d size:%d type:%d\n",idx,je,jt[idx].start,jt[idx].end,jt[idx].size,jt[idx].type);
		idx++;
	}
	
	idx=0;
	while(1){	
		if(idx >= je)
			return -1;
		if(strncmp(json+jt[idx].start,"method",6) == 0){
			if(strncmp(json+jt[idx+1].start,"mining.notify",13) == 0)
				return parse_nofify(json);
			if(strncmp(json+jt[idx+1].start,"mining.diff",11) == 0)
				return parse_diff(json);
		}	
		if(strncmp(json+jt[idx].start,"result",6) == 0)
			return parse_result(json);
		idx++;
	}
	
	return 0;
}

