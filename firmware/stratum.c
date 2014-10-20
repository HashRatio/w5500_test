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
static int32 notify_cnt = 0;
int32 flag_diff;
int32 flag_notify;

int32 authorize_id;
int32 subscribe_id;
int32 submit_id;

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
	m_sprintf(buffer,"{\"id\": %d, \"method\": \"mining.subscribe\", \"params\": [\"hashratio\"]}\n", pkg_id);
	if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
    {
        setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
    }
	send(SOCK_STRATUM,(uint8*)buffer,strlen(buffer),0);
	subscribe_id = pkg_id;
	return 0;
}

int32 send_authorize()
{
	pkg_id++;
	memset(buffer,0,BUFFER_SIZE);
	m_sprintf(buffer,"{\"params\": [\"%s\", \"%s\"], \"id\": %d, \"method\": \"mining.authorize\"}\n", g_worker_name,g_worker_pwd,pkg_id);
	debug32("%s",buffer);
	if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
    {
        setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
    }
	send(SOCK_STRATUM,(uint8*)buffer,strlen(buffer),0);
	authorize_id = pkg_id;
	return 0;
}

int32 send_submit(struct mm_work *mw, uint32 nonce2, uint32 ntime, uint32 nonce)
{
	pkg_id++;
	memset(buffer,0,BUFFER_SIZE);
	m_sprintf(buffer,"{\"params\": [\"%s\", \"%s\",\"%08x\", \"%08x\", \"%08x\"], \"id\": %d, \"method\": \"mining.submit\"}\n", g_worker_name,mw->job_id,nonce2,ntime,nonce,pkg_id);
	debug32("%s",buffer);
	if(getSn_IR(SOCK_STRATUM) & Sn_IR_CON)
    {
        setSn_IR(SOCK_STRATUM, Sn_IR_CON);/*Sn_IR的第0位置1*/
    }
	submit_id = pkg_id;
	return send(SOCK_STRATUM,(uint8*)buffer,strlen(buffer),0);
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
        recv(SOCK_STRATUM,(uint8*)buffer+json_end,len);/*W5200接收来自Sever的数据*/
		len+=json_end;
		json_begin = json_end = 0;
		while(1){
			if(buffer[json_end] == '\n'){
				buffer[json_end] = '\0';
				json_len = json_end - json_begin;
				debug32("ready to parse:%s\n",buffer+json_begin);
				parse_stratum(buffer+json_begin);
				json_begin = json_end + 1;
			}
			if(json_end == len - 1){
				if(buffer[json_end] == '\n' || buffer[json_end] == '\0' ){
					//debug32("!!!!!good end.\n");
					memset(buffer,0,BUFFER_SIZE);
					json_begin = 0;
					json_end = 0;
				}
				else{
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

static void hex2bin(uint8 * bin,uint8 * hex,int32 bin_len){
	int i;
	char hex_str[3] = "\0\0\0";
	for(i=0;i<bin_len;i++){
		memcpy(hex_str,hex+i*2,2);
		bin[i] = (uint8)ATOI(hex_str,16);
	}
}

static int32 find_tag_idx(const int8* json, const int8* tag, uint8 tag_len)
{
	int32 idx = 0;
	while(42){
		if(idx >= je)
			return -1;
		if(strncmp(json+jt[idx].start,tag,tag_len) == 0){
			idx++;
			idx++;
			return idx;
		}
		idx++;
	}
}

int32 parse_nofify(const int8 * json)
{
	int32 idx = 0;
	int32 prehash_idx;
	int32 len;
	int32 offset = 0;
	int32 i;

	notify_cnt++;
	
	idx = find_tag_idx(json,"params",6);

	/*job_id*/
	memset(mm_work_ptr->job_id,0,20);
	memcpy(mm_work_ptr->job_id,json+jt[idx].start,jt[idx].end-jt[idx].start);
	//debug32("job_id:%s\n",mm_work_ptr->job_id);
	idx++;
	
	/*locate prehash*/
	prehash_idx = idx;
	idx++;
	
	/*coinb1*/
	len = (jt[idx].end - jt[idx].start)/2;
	hex2bin(mm_work_ptr->coinbase+offset,(uint8*)json+jt[idx].start,len);
	offset += len;
	idx++;
	
	/*extra_nonce1*/
	memcpy(mm_work_ptr->coinbase+offset,(uint8 *)&nonce1_bin,4);
	//swap_int32(mm_work_ptr->coinbase+offset);
	offset += 4;
	
	/*extra_nonce2*/
	memset(mm_work_ptr->coinbase+offset,0,4);
	//swap_int32(mm_work_ptr->coinbase+offset);
	mm_work_ptr->nonce2_offset = offset;
	offset += 4;
	
	/*coinb2*/
	len = (jt[idx].end - jt[idx].start)/2;
	hex2bin(mm_work_ptr->coinbase+offset,(uint8*)json+jt[idx].start,len);
	offset += len;
	idx++;

	/*coinbase length*/
	mm_work_ptr->coinbase_len = offset;
	
	/*merkel number*/
	mm_work_ptr->nmerkles = jt[idx].size;
	idx++;
	
	/*merkel_branch*/
	for(i=0;i < mm_work_ptr->nmerkles;i++){
		len = (jt[idx].end - jt[idx].start)/2;
		hex2bin((uint8*)mm_work_ptr->merkles[i],(uint8*)json+jt[idx].start,len);
		idx++;
	}
	
	/*version #LITTLE ENDIAN# */
	hex2bin(mm_work_ptr->header,(uint8*)json+jt[idx].start,4);
	//swap_int32(mm_work_ptr->header);
	offset = 4;
	idx++;
	
	/*copy prehash*/
	len = (jt[prehash_idx].end - jt[prehash_idx].start)/2;
	hex2bin(mm_work_ptr->header+offset,(uint8*)json+jt[prehash_idx].start,len);
	offset += len;
	
	/*merkel root offset*/
	mm_work_ptr->merkle_offset = offset;
	offset += 32;
	
	/*ntime #BIG ENDIAN#*/
	idx++;
	len = (jt[idx].end - jt[idx].start)/2;
	hex2bin(mm_work_ptr->header+offset,(uint8*)json+jt[idx].start,len);
	if(g_hashrate_reset_flag){
		memcpy((uint8*)&g_last_ntime,mm_work_ptr->header+offset,4);
		g_hashrate_reset_flag = 0;
	}
	memcpy((uint8*)&g_curr_ntime,mm_work_ptr->header+offset,4);
	offset += len;
	
	/*nbits #BIT ENDIAN#*/
	idx--;
	len = (jt[idx].end - jt[idx].start)/2;
	hex2bin(mm_work_ptr->header+offset,(uint8*)json+jt[idx].start,len);
	offset += len;
	//idx++;
	
	/*nonce*/
	offset += 4;
	
	/*padding*/
	mm_work_ptr->header[offset+3] = 0x80;
	mm_work_ptr->header[124] = 0x80;
	mm_work_ptr->header[125] = 0x02;

	//hexdump((uint8*)mm_work_ptr->header,128);
	
	return 0;
}

static void shift_32bytes(uint8 * target)
{
	int32 i;
	for(i=0;i<32;i++){
		target[i] = target[i+1]&0x01?(0x80|(target[i] >> 1)):(target[i] >> 1);
	}
}

/* Can only deal with diff = pow(2,n) */
static void calc_target(uint8 * target,uint32 diff)
{
	int32 i=0;
	int32 loop=0;
	memcpy(target,g_diff1_target,32);
	while(diff){
		if( (diff & 0x00000001) == 0x00000001)
			for(i=0;i<loop;i++){
				shift_32bytes(target);
			}
		diff = diff>>1;
		loop++;
	}
}

int32 parse_diff(const int8 * json)
{
	int32 idx = find_tag_idx(json,"params",6);
	*(char*)(json+jt[idx].end)=0;
	g_diff = ATOI((char *)json+jt[idx].start,10);
	//g_diff = 16;
	calc_target(mm_work_ptr->target,g_diff);
	debug32("Set diff to %d\n",g_diff);
	return 0;
}

int32 parse_result(const int8 * json)
{
	return 0;
}

int32 parse_stratum(const int8 * json)
{
	int32 idx = 0;
	int32 i;
	int32 recv_pkg_id = 0;
	jsmn_init(&jp);
	je = jsmn_parse(&jp,json,strlen(json),jt, TOKEN_BUFFER);
	
	idx=0;
	while(1){	
		if(idx >= je)
			return -1;
		if(strncmp(json+jt[idx].start,"method",6) == 0){
			if(strncmp(json+jt[idx+1].start,"mining.notify",13) == 0){
				g_new_stratum = 1;
				return parse_nofify(json);
			}
			if(strncmp(json+jt[idx+1].start,"mining.set_difficulty",21) == 0){
				return parse_diff(json);
			}
		}
		if((strncmp(json+jt[idx].start,"id",2) == 0)&&(strncmp(json+jt[idx+1].start,"n",1) != 0)){
			for(i=jt[idx+1].start;i<jt[idx+1].end;i++){
				recv_pkg_id = recv_pkg_id*10 + (json[i]-0x30);
			}
			break;
		}
		idx++;
	}
	
	idx=find_tag_idx(json,"result",6);
	idx -= 2;
	if(recv_pkg_id == authorize_id){
		;
	}
	else if(recv_pkg_id == subscribe_id){
		memcpy(nonce1_str,json+jt[idx+9].start,8);
		nonce1_bin = ATOI32(nonce1_str,16);
		;
	}
	else if(recv_pkg_id == submit_id){
		;
	}
	return 0;
}

