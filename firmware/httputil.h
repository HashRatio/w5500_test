#ifndef __HTTPUTIL_H__
#define __HTTPUTIL_H__

#include "minilibc.h"
#include "w5500.h"
#include "utils.h"
#include "defines.h"
#include "httpd.h"

#include "socket.h"
#include "sockutil.h"

void proc_http(SOCKET s, uint8 * buf);
void do_http(void);
void cgi_ipconfig(st_http_request *http_request);

//void trimp(uint8* src, uint8* dst, uint16 len);
uint16 make_msg_response(uint8* buf, int8* msg);

void make_cgi_response(uint16 a, int8* b, int8* c);
void make_pwd_response(int8 isRight, uint16 delay, int8* cgi_response_content, int8 isTimeout);

#endif