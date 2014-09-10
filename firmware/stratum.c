#include <stdlib.h>

#include "minilibc.h"
#include "defines.h"
#include "types.h"
#include "w5500.h"
#include "socket.h"
#include "stratum.h"
#include "jsmn.h"
#include "miner.h"
#include "utils.h"
#include "pub_var.h"

static jsmn_parser jp;
static jsmntok_t jt[TOKEN_BUFFER];
static jsmnerr_t je;
static int32 pkg_id = 0;

static int32 json_begin = 0;
static int32 json_end = 0;
//static int32 json_ptr = 0;
static int32 notify_cnt = 0;
int32 flag_diff;
int32 flag_notify;

int32 current_diff;
int32 authorize_id;
int32 subscribe_id;
int32 submit_id;

int8 buffer[2048];

struct pool_task{
	
};

int8 nonce1_str[9];
uint32 nonce1_bin;

int32 connect_poll(uint8 * addr, uint16 port)
{
	uint8 ret;
    uint16 anyport = 20000;
	while(1){
		switch(getSn_SR(SOCK_STRATUM))/*获取socket0的状态*/
		{
			case SOCK_INIT:/*socket初始化完成*/
				//debug32("SOCK_INIT\n");
				connect(SOCK_STRATUM, addr ,port);/*在TCP模式下向服务器发送连接请求*/
				delay(500);
				break;
			case SOCK_ESTABLISHED:/*socket连接建立*/
				//debug32("SOCK_ESTABLISHED\n");
				debug32("Successfully connect pool:%d.%d.%d.%d:%d\n",addr[0],addr[1],addr[2],addr[3],port);
				return 1;
			case SOCK_CLOSE_WAIT:/*socket等待关闭状态*/
				//debug32("SOCK_CLOSE_WAIT\n");
			    break;
			case SOCK_CLOSED:/*socket关闭*/
			    ret = socket(SOCK_STRATUM,Sn_MR_TCP,anyport++,Sn_MR_ND);/*打开socket的一个端口*/
			    //debug32("SOCKCLOSED:0x%02x\n",ret);
				break;
		 }
	}
}

int32 send_subscribe()
{
	pkg_id++;
	memset(buffer,0,BUFFER_SIZE);
	m_sprintf(buffer,"{\"id\": %d, \"method\": \"mining.subscribe\", \"params\": [\"hashratio miner\"]}\n", pkg_id);
	//debug32("%s",buffer);
	//debug32("0x%08x\n",buffer);
	if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
    {
        setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
    }
	//hexdump((const uint8 *)buffer,2048);
	send(SOCK_STRATUM,(uint8*)buffer,strlen(buffer),0);
	subscribe_id = pkg_id;
	//debug32("0x%08x\n",buffer);
	//hexdump((const uint8 *)buffer,2048);
	return 0;
}

int32 send_authorize()
{
	pkg_id++;
	memset(buffer,0,BUFFER_SIZE);
	m_sprintf(buffer,"{\"params\": [\"whb.miner1\", \"password\"], \"id\": %d, \"method\": \"mining.authorize\"}\n", pkg_id);
	//debug32("%s",buffer);
	if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
    {
        setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
    }
	//debug32("send start.\n");
	//hexdump((const uint8 *)buffer,2048);
	send(SOCK_STRATUM,(uint8*)buffer,strlen(buffer),0);
	//debug32("send ok.\n");
	authorize_id = pkg_id;
	//hexdump((const uint8 *)buffer,2048);
	return 0;
}

int32 send_submit()
{
	submit_id = pkg_id;
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
		//hexdump((const uint8 *)buffer,2048);
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
	int32 len;
	int32 i;
	
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
	memset(g_mm_works[0].job_id,0,20);
	memcpy(mm_work_ptr->job_id,json+jt[idx].start,jt[idx].end-jt[idx].start);
	debug32("job_id:%s\n",mm_work_ptr->job_id);
	idx++;
	
	/*prehash*/
	idx++;
	
	/*coinb1*/
	len = (jt[idx].end - jt[idx].start)/2;
	i = len;
	idx++;
	
	
	/*coinb2*/
	idx++;
	
	/*merkel_branch*/
	idx+=jt[idx].size+1;
	
	/*version*/
	memcpy(mm_work_ptr->header,json+jt[idx].start,jt[idx].end-jt[idx].start);
	debug32("version:%s\n",mm_work_ptr->header);
	idx++;
	
	/*nbits*/
	memcpy(mm_work_ptr->target,json+jt[idx].start,jt[idx].end-jt[idx].start);
	debug32("nbits:%s\n",mm_work_ptr->target);
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
	// int idx = 0;
	// jsmn_init(&jp);
	// je = jsmn_parse(&jp,json,strlen(json),jt, TOKEN_BUFFER);
	return 0;
}

int32 parse_stratum(const int8 * json)
{
	int32 idx = 0;
	int32 i;
	int32 recv_pkg_id = 0;
	jsmn_init(&jp);
	je = jsmn_parse(&jp,json,strlen(json),jt, TOKEN_BUFFER);
	
/*  	while(1){
		if(idx >= je)
			break;
		//memset(buffer,0,BUFFER_SIZE);
		memcpy(buffer,json+jt[idx].start,jt[idx].end-jt[idx].start);
		//buffer[jt[idx].end] = '\0';
		debug32("idx:%d err:%d start:%d end:%d size:%d type:%d\n",idx,je,jt[idx].start,jt[idx].end,jt[idx].size,jt[idx].type);
		idx++;
	}  */
	
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
		if((strncmp(json+jt[idx].start,"id",2) == 0)&&(strncmp(json+jt[idx+1].start,"n",1) != 0)){
			for(i=jt[idx+1].start;i<jt[idx+1].end;i++){
				recv_pkg_id = recv_pkg_id*10 + (json[i]-0x30);
			}
			break;
		}
		idx++;
	}
	
	idx=0;
	while(1){
		if(idx >= je)
			return -1;
		if(strncmp(json+jt[idx].start,"result",6) == 0){
			if(recv_pkg_id == authorize_id){
				debug32("authorize\n");
			}
			else if(recv_pkg_id == subscribe_id){
				memcpy(nonce1_str,json+jt[idx+9].start,8);
				nonce1_bin = ATOI32(nonce1_str,16);
				debug32("subscribe:nonce1:%s %x\n",nonce1_str,nonce1_bin);
				break;
			}
			else if(recv_pkg_id == submit_id){
				
			}
			break;
		}
		idx++;
	}
	return 0;
}

