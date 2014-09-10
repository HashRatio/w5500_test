#ifndef	_DNS_H_
#define	_DNS_H_

//#include "device.h"
//#include "config.h"

#define  DNS_RESPONSE_TIMEOUT 3 // 3 seconds
#define  DNS_RETRY            3//3 times
#define  DNS_RET_PROGRESS  0
#define  DNS_RET_FAIL      1
#define  DNS_RET_SUCCESS   2

#define  EXTERN_DNS_SERVERIP  ConfigMsg.dns
#define  EXTERN_DNS_SOCKBUF   BUFPUB
#define  EXTERN_DNS_ANYPORT   2000//tsvDEVCONFnew.any_port

#define	MAX_DNS_BUF_SIZE	256		/* maximum size of DNS buffer. */


#define  INITRTT     2000L	/* Initial smoothed response time */
#define  MAXCNAME    10	/* Maximum amount of cname recursion */
         
#define  TYPE_A		1	/* Host address */
#define  TYPE_NS		2	/* Name server */
#define  TYPE_MD		3	/* Mail destination (obsolete) */
#define  TYPE_MF		4	/* Mail forwarder (obsolete) */
#define  TYPE_CNAME	5	/* Canonical name */
#define  TYPE_SOA    6	/* Start of Authority */
#define  TYPE_MB		7	/* Mailbox name (experimental) */
#define  TYPE_MG		8	/* Mail group member (experimental) */
#define  TYPE_MR		9	/* Mail rename name (experimental) */
#define  TYPE_NULL	10	/* Null (experimental) */
#define  TYPE_WKS    11	/* Well-known sockets */
#define  TYPE_PTR    12	/* Pointer record */
#define  TYPE_HINFO	13	/* Host information */
#define  TYPE_MINFO	14	/* Mailbox information (experimental)*/
#define  TYPE_MX		15	/* Mail exchanger */
#define  TYPE_TXT    16	/* Text strings */
#define  TYPE_ANY    255/* Matches any type */
         
#define  CLASS_IN    1  /* The ARPA Internet */

/* Round trip timing parameters */
#define  AGAIN       8	/* Average RTT gain = 1/8 */
#define  LAGAIN      3	/* Log2(AGAIN) */
#define  DGAIN       4  /* Mean deviation gain = 1/4 */
#define  LDGAIN      2  /* log2(DGAIN) */
         
#define  IPPORT_DOMAIN	53

/* Header for all domain messages */
struct dhdr
{
	unsigned short  id;		/* Identification */
	unsigned char 	qr;		/* Query/Response */
#define	QUERY		0
#define	RESPONSE	1
	unsigned char 	opcode;
#define	IQUERY		1
	unsigned char 	aa;		/* Authoratative answer */
	unsigned char 	tc;		/* Truncation */
	unsigned char 	rd;		/* Recursion desired */
	unsigned char 	ra;		/* Recursion available */
	unsigned char 	rcode;		/* Response code */
#define	NO_ERROR	0
#define	FORMAT_ERROR	1
#define	SERVER_FAIL	2
#define	NAME_ERROR	3
#define	NOT_IMPL	4
#define	REFUSED		5
	unsigned short  qdcount;	/* Question count */
	unsigned short  ancount;	/* Answer count */
	unsigned short  nscount;	/* Authority (name server) count */
	unsigned short  arcount;	/* Additional record count */
};

//extern uint8 DEFAULT_DNS[4];
//extern unsigned char DNS_GET_IP[4];
extern unsigned char BUFPUB[1024];
extern unsigned char dns_query(uint8 * dns_ip, unsigned char s, unsigned char * name);
extern void init_dns_client(void);
extern int32 do_dns(uint8 * dns_ip, uint8 * domain,uint8 * ip);
#endif	/* _DNS_H_ */
