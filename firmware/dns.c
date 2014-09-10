//#include <stdio.h>
#include "minilibc.h"
//#include <string.h>
//#include <stdlib.h>
#include "utils.h"
#include "w5500.h"
#include "socket.h"
#include "dns.h"
#include "sockutil.h"
#include "defines.h"

uint8 BUFPUB[1024];

static uint8 DNS_GET_IP[4];
uint16 MSG_ID = 0;
//uint8 DEFAULT_DNS[4] = {192,168,2,1};

static uint8 dns_retry_cnt=0;
static uint8 dns_ok=0;
	
/*
********************************************************************************
*              MAKE DNS QUERY MESSAGE
*
* Description : This function makes DNS query message.
* Arguments   : op   - Recursion desired
*               name - is a pointer to the domain name.
*               buf  - is a pointer to the buffer for DNS message.
*               len  - is the MAX. size of buffer.
* Returns     : the pointer to the DNS message.
* Note        :
********************************************************************************
*/
int dns_makequery(uint16 op, uint8 * name, uint8 * buf, uint16 len)
{
  uint8  *cp;
  uint8   *cp1;
  //	int8   sname[MAX_DNS_BUF_SIZE];
  uint8  *dname;
  uint16 p;
  uint16 dlen;
  
  cp = buf;
  
  MSG_ID++;
  *(uint16*)&cp[0] = htons(MSG_ID);
  p = (op << 11) | 0x0100;			/* Recursion desired */
  //p = (op << 11) & 0xFEFF;			/* No Recursion desired */
  *(uint16*)&cp[2] = (p);
  *(uint16*)&cp[4] = htons(0x0100);
  *(uint16*)&cp[6] = htons(0);
  *(uint16*)&cp[8] = htons(0);
  *(uint16*)&cp[10]= htons(0);
  
  cp += sizeof(uint16)*6;
  //	strcpy(sname, name);
  dname = name;
  dlen = strlen((char*)dname);
  for (;;)
  {
    /* Look for next dot */
    cp1 = (unsigned char*)strchr((char*)dname, '.');
    
    if (cp1) len = cp1 - dname;	/* More to come */
    else len = dlen;			/* Last component */
    
    *cp++ = len;				/* Write length of component */
    if (len == 0) break;
    
    /* Copy component up to (but not including) dot */
    strncpy((char *)cp, (char*)dname, len);
    cp += len;
    if (!cp1)
    {
      *cp++ = 0;			/* Last one; write null and finish */
	  break;
    }
    dname += len+1;
    dlen -= len+1;
  }
  *(uint8*)&cp[0] = 0x00;//htons(0x55);				/* type */
  *(uint8*)&cp[1] = 0x01;//htons(0x55);				/* type */
  *(uint8*)&cp[2] = 0x00;//htons(0xAA);				/* class */
  *(uint8*)&cp[3] = 0x01;//htons(0xAA);				/* class */
  cp += sizeof(uint16)*2;
  return ((int)((uint32)(cp) - (uint32)(buf)));
}

/*
********************************************************************************
*              CONVERT A DOMAIN NAME TO THE HUMAN-READABLE FORM
*
* Description : This function convnerts a compressed domain name to the human-readable form
* Arguments   : msg        - is a pointer to the reply message
*               compressed - is a pointer to the domain name in reply message.
*               buf        - is a pointer to the buffer for the human-readable form name.
*               len        - is the MAX. size of buffer.
* Returns     : the length of compressed message
* Note        :
********************************************************************************
*/
int parse_name(uint8 * msg, uint8 * compressed, /*char * buf,*/ uint16 len)
{
  uint16 slen;		/* Length of current segment */
  uint8  * cp;
  int16  clen = 0;		/* Total length of compressed name */
  int16  indirect = 0;	/* Set if indirection encountered */
  int16  nseg = 0;		/* Total number of segments in name */
  int8   name[MAX_DNS_BUF_SIZE];
  int8   *buf;
  
  buf = name;
  
  cp = compressed;
  for (;;)
  {
    slen = *cp++;	/* Length of this segment */
    
    if (!indirect) clen++;
    
    if ((slen & 0xc0) == 0xc0)
    {
      if (!indirect)
        clen++;
      indirect = 1;
      /* Follow indirection */
      cp = &msg[((slen & 0x3f)<<8) + *cp];
      slen = *cp++;
    }
    
    if (slen == 0)	/* zero length == all done */
      break;
    
    len -= slen + 1;
    
    if (len <= 0) return -1;
    
    if (!indirect) clen += slen;
    
    while (slen-- != 0) *buf++ = (int8)*cp++;
    *buf++ = '.';
    nseg++;
  }
  
  if (nseg == 0)
  {
    /* Root name; represent as single dot */
    *buf++ = '.';
    len--;
  }
  
  *buf++ = '\0';
  len--;
  
  return clen;	/* Length of compressed message */
}



/*
********************************************************************************
*              PARSE QUESTION SECTION
*
* Description : This function parses the qeustion record of the reply message.
* Arguments   : msg - is a pointer to the reply message
*               cp  - is a pointer to the qeustion record.
* Returns     : a pointer the to next record.
* Note        :
********************************************************************************
*/
uint8 * dns_question(uint8 * msg, uint8 * cp)
{
  int16 len;
  //	int8  xdata name[MAX_DNS_BUF_SIZE];
  
  len = parse_name(msg, cp, /*name,*/ MAX_DNS_BUF_SIZE);
  
  if (len == -1) return 0;
  
  cp += len;
  cp += 2;		/* type */
  cp += 2;		/* class */
  
  return cp;
}


/*
********************************************************************************
*              PARSE ANSER SECTION
*
* Description : This function parses the answer record of the reply message.
* Arguments   : msg - is a pointer to the reply message
*               cp  - is a pointer to the answer record.
* Returns     : a pointer the to next record.
* Note        :
********************************************************************************
*/
uint8 * dns_answer(uint8 * msg, uint8 * cp)
{
  int16 len, type;
  //	int8  xdata name[MAX_DNS_BUF_SIZE];
  len = parse_name(msg, cp, /*name,*/ MAX_DNS_BUF_SIZE);
  if (len == -1) return 0;
  cp += len;
  
  //type = ntohs(*((uint16*)&cp[0]));
  type = ntohs(*((uint16*)&cp[1]));
  type = (cp[0]<<8) + cp[1];
  cp += 2;		/* type */
  cp += 2;		/* class */
  cp += 4;		/* ttl */
  cp += 2;		/* len */
  
  switch (type)
  {
    case TYPE_A:
      DNS_GET_IP[0] = *cp++;
      DNS_GET_IP[1] = *cp++;
      DNS_GET_IP[2] = *cp++;
      DNS_GET_IP[3] = *cp++;
      break;
    case TYPE_CNAME:
    case TYPE_MB:
    case TYPE_MG:
    case TYPE_MR:
    case TYPE_NS:
    case TYPE_PTR:
      len = parse_name(msg, cp, /*name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1){
		return 0;
	}
      
      cp += len;
      break;
    case TYPE_HINFO:
      len = *cp++;
      cp += len;
      
      len = *cp++;
      cp += len;
      break;
    case TYPE_MX:
      cp += 2;
      /* Get domain name of exchanger */
      len = parse_name(msg, cp,/* name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1) return 0;
      
      cp += len;
      break;
    case TYPE_SOA:
      /* Get domain name of name server */
      len = parse_name(msg, cp,/* name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1) return 0;
      
      cp += len;
      
      /* Get domain name of responsible person */
      len = parse_name(msg, cp,/* name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1) return 0;
      
      cp += len;
      
      cp += 4;
      cp += 4;
      cp += 4;
      cp += 4;
      cp += 4;
      break;
    case TYPE_TXT:
      /* Just stash */
      break;
    default:
    /* Ignore */
		break;
  }
  
  return cp;
}

/*
********************************************************************************
*              PARSE THE DNS REPLY
*
* Description : This function parses the reply message from DNS server.
* Arguments   : dhdr - is a pointer to the header for DNS message
*               buf  - is a pointer to the reply message.
*               len  - is the size of reply message.
* Returns     : None
* Note        :
********************************************************************************
*/
uint8 parseMSG(struct dhdr * pdhdr, uint8 * pbuf)
{
  uint16 tmp;
  uint16 i;
  uint8 * msg;
  uint8 * cp;
  
  msg = pbuf;
  memset(pdhdr, 0, sizeof(pdhdr));
  
  //pdhdr->id = ntohs(*((uint16*)&msg[0]));
  pdhdr->id = (*((uint16*)&msg[0]));
  
  //tmp = ntohs(*((uint16*)&msg[2]));
  tmp = (*((uint16*)&msg[2]));
  if (tmp & 0x8000) pdhdr->qr = 1;
  
  pdhdr->opcode = (tmp >> 11) & 0xf;
  
  if (tmp & 0x0400) pdhdr->aa = 1;
  if (tmp & 0x0200) pdhdr->tc = 1;
  if (tmp & 0x0100) pdhdr->rd = 1;
  if (tmp & 0x0080) pdhdr->ra = 1;
  
  pdhdr->rcode = tmp & 0xf;
  
  //pdhdr->qdcount = ntohs(*((uint16*)&msg[4]));
  pdhdr->qdcount = (*((uint16*)&msg[4]));
  
  //pdhdr->ancount = ntohs(*((uint16*)&msg[6]));
  pdhdr->ancount = (*((uint16*)&msg[6]));
  
  //pdhdr->nscount = ntohs(*((uint16*)&msg[8]));
  pdhdr->nscount = (*((uint16*)&msg[8]));
  
  //pdhdr->arcount = ntohs(*((uint16*)&msg[10]));
  pdhdr->arcount = (*((uint16*)&msg[10]));
  
  /* Now parse the variable length sections */
  cp = &msg[12];
  
  /* Question section */
  for (i = 0; i < pdhdr->qdcount; i++)
  {
    cp = dns_question(msg, cp);
  }
  
  /* Answer section */
  for (i = 0; i < pdhdr->ancount; i++)
  {
    cp = dns_answer(msg, cp);
  }
  
  /* Name server (authority) section */
  for (i = 0; i < pdhdr->nscount; i++)
  {
    ;
  }
  
  /* Additional section */
  for (i = 0; i < pdhdr->arcount; i++)
  {
    ;
  }
  
  if(pdhdr->rcode == 0) return 1;		// No error
  else return 0;
}



/*
********************************************************************************
*              MAKE DNS QUERY AND PARSE THE REPLY
*
* Description : This function makes DNS query message and parses the reply from DNS server.
* Arguments   : name - is a pointer to the domain name.
* Returns     : if succeeds : 1, fails : -1
* Note        :
********************************************************************************
*/
uint8 dns_query(uint8 * dns_ip, uint8 s, uint8 * name)
{
  static uint32 dns_wait_time = 0;
  struct dhdr dhp;
  uint8 ip[4];
  uint16 len, port;
  switch(getSn_SR(s))
  {
    case SOCK_UDP:
      if ((len = getSn_RX_RSR(s)) > 0)
      {
        if (len > MAX_DNS_BUF_SIZE) len = MAX_DNS_BUF_SIZE;
        len = recvfrom(s, BUFPUB, len, ip, &port);
        if(parseMSG(&dhp, BUFPUB))
        {
          close(s);
          return DNS_RET_SUCCESS;
         }
        else 
          dns_wait_time = DNS_RESPONSE_TIMEOUT;
      }
      else
      {
        delay(1000);
        dns_wait_time++;
      }
      if(dns_wait_time >= DNS_RESPONSE_TIMEOUT) 
      {
        close(s);
        return DNS_RET_FAIL;
      }
      break;
    case SOCK_CLOSED:
		dns_wait_time = 0;
		socket(s, Sn_MR_UDP, 3000, 0);
		len = dns_makequery(0, name, BUFPUB, MAX_DNS_BUF_SIZE);
		uint8 str[32];
		memcpy(str,BUFPUB,31);
		sendto(s, BUFPUB, len, dns_ip, IPPORT_DOMAIN);
		break;         
  }
  return DNS_RET_PROGRESS;
}

void init_dns_client(void)
{
  // uint8 DEFAULT_MAC[6] = {0x00, 0x08, 0xdc, 0x11, 0x12, 0x13};
  // uint8 DEFAULT_LIP[4] = {192, 168, 2, 200};
  // uint8 DEFAULT_SN[4] = {255, 255, 255, 0};
  // uint8 DEFAULT_GW[4] = {192, 168, 2, 1};
  //
  uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
  uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
  //uint8 domain = "www.baidu.com";
  // memcpy(ConfigMsg.mac,DEFAULT_MAC,6);
  // memcpy(ConfigMsg.lip,DEFAULT_LIP,6);
  // memcpy(ConfigMsg.sub,DEFAULT_SN,6);
  // memcpy(ConfigMsg.gw,DEFAULT_GW,6);
  // memcpy(ConfigMsg.dns, DEFAULT_DNS, 4);
  // setSHAR(DEFAULT_MAC);
  // setSUBR(DEFAULT_SN);
  // setGAR(DEFAULT_GW);
  // setSIPR(DEFAULT_LIP); 
  sysinit(txsize, rxsize);
  
  //sprintf((char*)ConfigMsg.domain,"%s","www.baidu.com"); 
}

int32 do_dns(uint8 * dns_ip,uint8 * domain,uint8 * ip)
{
	while(1)
	{ 		
		if(dns_ok==1)
			return 1;
		
		if(dns_retry_cnt > DNS_RETRY)
			return 0;

		if(memcmp(dns_ip,"\x00\x00\x00\x00",4))
		{
			switch(dns_query(dns_ip, SOCK_DNS,domain))
			{
			  case DNS_RET_SUCCESS:
				dns_ok=1;
				memcpy(ip,DNS_GET_IP,4);
				dns_retry_cnt=0;
				break;
			  case DNS_RET_FAIL:
				dns_ok=0;
				dns_retry_cnt++;
				break;
			  default:
				break;
			}
		}
		else
			return 0;
	}
	
	return 1;
}