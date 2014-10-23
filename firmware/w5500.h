/*
@file		w5500.h
*/
#ifndef	_W5500_H_
#define	_W5500_H_

#define WINDOWFULL_FLAG_ON 1
#define WINDOWFULL_FLAG_OFF 0 
#define WINDOWFULL_MAX_RETRY_NUM 3
#define WINDOWFULL_WAIT_TIME 1000

#include "types.h" 

#define SOCK_DNS		1	// UDP
#define SOCK_STRATUM	0
#define SOCK_HTTP       2

#define CH_SIZE		                0x2000
#define COMMON_BASE 				0x0000
#define SOCKET_REG_OFFSET 			0x0800
#define SOCKET_TX_OFFSET 			0x1000
#define SOCKET_RX_OFFSET 			0x1800

#define Sn_MR(ch)		        (SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0000)
#define Sn_CR(ch)				(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0001)
#define Sn_IR(ch)				(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0002)
#define Sn_SR(ch)				(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0003)
#define Sn_PORT0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0004)
#define Sn_DHAR0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0006)
#define Sn_DIPR0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x000C)
#define Sn_DPORT0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0010)
#define Sn_MSSR0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0012)
#define Sn_PROTO(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0014)
#define Sn_TOS(ch)				(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0015)
#define Sn_TTL(ch)				(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0016)
#define Sn_RXMEM_SIZE(ch)	    (SOCKET_REG_OFFSET + ch * CH_SIZE + 0x001E)
#define Sn_TXMEM_SIZE(ch)	    (SOCKET_REG_OFFSET + ch * CH_SIZE + 0x001F)
#define Sn_TX_FSR0(ch)		    (SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0020)
#define Sn_TX_RD0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0022)
#define Sn_TX_WR0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0024)
#define Sn_RX_RSR0(ch)		    (SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0026)
#define Sn_RX_RD0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x0028)
#define Sn_RX_WR0(ch)			(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x002A)
#define Sn_IMR(ch)				(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x002C)
#define Sn_FRAG(ch)				(SOCKET_REG_OFFSET +S ch * CH_SIZE + 0x002D)
#define Sn_KEEP_TIMER(ch)		(SOCKET_REG_OFFSET + ch * CH_SIZE + 0x002F)

#define getIINCHIP_RxBASE(ch)	(SOCKET_RX_OFFSET + ch * CH_SIZE)
#define getIINCHIP_TxBASE(ch)	(SOCKET_TX_OFFSET + ch * CH_SIZE)

/* MODE register values */
//#define MR_RST			0x80 /**< reset */
//#define MR_WOL			0x20 /**< Wake on Lan */
//#define MR_PB			0x10 /**< ping block */
//#define MR_PPPOE		0x08 /**< enable pppoe */
//#define MR_LB  		        0x04 /**< little or big endian selector in indirect mode */
//#define MR_AI			0x02 /**< auto-increment in indirect mode */
//#define MR_IND			0x01 /**< enable indirect mode */

/***********PORT FROM EXTERN***************/

/***************** Common Register *****************/
#define MR			0x0000
#define MR_RST		0x80	// reset
#define MR_WOL		0x20	// Wake on Lan
#define MR_PB		0x10	// ping block
#define MR_PPPOE	0x08	// enable pppoe
#define MR_FARP		0x02	// Force ARP

#define GAR			0x0001 //Gateway IP Register address
#define SUBR		0x0005 //Subnet Register address
#define SHAR		0x0009 //Source MAC Register address
#define SIPR		0x000f //Source IP Register address

#define INTLEVEL	0x0013

#define IR			0x0015 //Interrupt Register address
#define IR_CONFLICT	0x80
#define IR_UNREACH	0x40
#define IR_PPPOE	0x20
#define IR_MP		0x10

#define IMR		0x0016 //Interrupt Mask Register address
#define IM_IR7		0x80
#define IM_IR6		0x40
#define IM_IR5		0x20
#define IM_IR4		0x10

#define SIR		0x0017 //Socket Interrupt Register address
#define S7_INT		0x80
#define S6_INT		0x40
#define S5_INT		0x20
#define S4_INT		0x10
#define S3_INT		0x08
#define S2_INT		0x04
#define S1_INT		0x02
#define S0_INT		0x01

#define SIMR		0x0018 //Socket Interrupt Mask address
#define S7_IMR		0x80
#define S6_IMR		0x40
#define S5_IMR		0x20
#define S4_IMR		0x10
#define S3_IMR		0x08
#define S2_IMR		0x04
#define S1_IMR		0x02
#define S0_IMR		0x01

#define RTR		0x0019 //Retry Time Register address
#define RCR		0x001b //Retry Count Register address

#define PTIMER		0x001c //PPP Timer Register address
#define PMAGIC		0x001d //PPP Magic Register address
#define PHA			0x001e
#define PSID		0x0024
#define PMRU		0x0026

#define UIPR		0x0028
#define UPORT		0x002c

#define PHYCFGR		0x002e
#define PHYCFGR_RST_PHY		0x80
#define PHYCFGR_OPMD		0x40
#define PHYCFGR_OPMDC		0x38
#define PHYCFGR_DPX			0x04
#define PHYCFGR_SPD			0x02
#define PHYCFGR_LINK		0x01

#define VERR		0x0039

/********************* Socket Register *******************/
//#define Sn_MR		0x0000
#define MULTI_MFEN	0x80
#define BCASTB		0x40
#define	ND_MC_MMB	0x20
#define UCASTB_MIP6B	0x10
#define MR_CLOSE	0x00
#define MR_TCP		0x01
#define MR_UDP		0x02
#define MR_MACRAW	0x04

//#define Sn_CR		0x0001
#define OPEN		0x01
#define LISTEN		0x02
#define CONNECT		0x04
#define DISCON		0x08
#define CLOSE		0x10
#define SEND		0x20
#define SEND_MAC	0x21
#define SEND_KEEP	0x22
#define RECV		0x40

//#define Sn_IR		0x0002
#define IR_SEND_OK	0x10
#define IR_TIMEOUT	0x08
#define IR_RECV		0x04
#define IR_DISCON	0x02
#define IR_CON		0x01

//#define Sn_SR		0x0003
#define SOCK_CLOSED		0x00
#define SOCK_INIT		0x13
#define SOCK_LISTEN		0x14
#define SOCK_ESTABLISHED	0x17
#define SOCK_CLOSE_WAIT		0x1c
#define SOCK_UDP		0x22
#define SOCK_MACRAW		0x02

#define SOCK_SYNSEND	0x15
#define SOCK_SYNRECV	0x16
#define SOCK_FIN_WAI	0x18
#define SOCK_CLOSING	0x1a
#define SOCK_TIME_WAIT	0x1b
#define SOCK_LAST_ACK	0x1d

//#define Sn_PORT		0x0004
//#define Sn_DHAR	   	0x0006
//#define Sn_DIPR		0x000c
//#define Sn_DPORTR	0x0010

//#define Sn_MSSR		0x0012
//#define Sn_TOS		0x0015
//#define Sn_TTL		0x0016

//#define Sn_RXBUF_SIZE	0x001e
//#define Sn_TXBUF_SIZE	0x001f
//#define Sn_TX_FSR	0x0020
//#define Sn_TX_RD	0x0022
//#define Sn_TX_WR	0x0024
//#define Sn_RX_RSR	0x0026
//#define Sn_RX_RD	0x0028
//#define Sn_RX_WR	0x002a

//#define Sn_IMR		0x002c
#define IMR_SENDOK	0x10
#define IMR_TIMEOUT	0x08
#define IMR_RECV	0x04
#define IMR_DISCON	0x02
#define IMR_CON		0x01

//#define Sn_FRAG		0x002d
//#define Sn_KPALVTR	0x002f

/*******************************************************************/
/************************ SPI Control Byte *************************/
/*******************************************************************/
/* Operation mode bits */
#define VDM		0x00
#define FDM1	0x01
#define	FDM2	0x02
#define FDM4	0x03

/* Read_Write control bit */
#define RWB_READ	0x00
#define RWB_WRITE	0x04

/* Block select bits */
#define COMMON_R	0x00

/* Socket 0 */
#define S0_REG		0x08
#define S0_TX_BUF	0x10
#define S0_RX_BUF	0x18

/* Socket 1 */
#define S1_REG		0x28
#define S1_TX_BUF	0x30
#define S1_RX_BUF	0x38

/* Socket 2 */
#define S2_REG		0x48
#define S2_TX_BUF	0x50
#define S2_RX_BUF	0x58

/* Socket 3 */
#define S3_REG		0x68
#define S3_TX_BUF	0x70
#define S3_RX_BUF	0x78

/* Socket 4 */
#define S4_REG		0x88
#define S4_TX_BUF	0x90
#define S4_RX_BUF	0x98

/* Socket 5 */
#define S5_REG		0xa8
#define S5_TX_BUF	0xb0
#define S5_RX_BUF	0xb8

/* Socket 6 */
#define S6_REG		0xc8
#define S6_TX_BUF	0xd0
#define S6_RX_BUF	0xd8

/* Socket 7 */
#define S7_REG		0xe8
#define S7_TX_BUF	0xf0
#define S7_RX_BUF	0xf8

#define TRUE	0xff
#define FALSE	0x00

/**************************/

/* IR register values */
// #define IR_CONFLICT	        0x80 /**< check ip confict */
// #define IR_UNREACH	        0x40 /**< get the destination unreachable message in UDP sending */
// #define IR_PPPoE		0x20 /**< get the PPPoE close message */
// #define IR_MAGIC		0x10 /**< get the magic packet interrupt */
// #define IR_SOCK(ch)	        (0x01 << ch) /**< check socket interrupt */

/* Sn_MR values */
#define Sn_MR_CLOSE		0x00		/**< unused socket */
#define Sn_MR_TCP		0x01		/**< TCP */
#define Sn_MR_UDP		0x02		/**< UDP */
#define Sn_MR_IPRAW	        0x03		/**< IP LAYER RAW SOCK */
#define Sn_MR_MACRAW	        0x04		/**< MAC LAYER RAW SOCK */
#define Sn_MR_PPPOE		0x05		/**< PPPoE */
#define Sn_MR_ND		0x20		/**< No Delayed Ack(TCP) flag */
#define Sn_MR_MULTI		0x80		/**< support multicating */

/* Sn_CR values */
#define Sn_CR_OPEN		0x01		/**< initialize or open socket */
#define Sn_CR_LISTEN		0x02		/**< wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT	        0x04		/**< send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON		0x08		/**< send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE		0x10		/**< close socket */
#define Sn_CR_SEND		0x20		/**< update txbuf pointer, send data */
#define Sn_CR_SEND_MAC	        0x21		/**< send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP     	0x22		/**<  send keep alive message */
#define Sn_CR_RECV		0x40		/**< update rxbuf pointer, recv data */

#define Sn_IR_SEND_OK			0x10		/**< complete sending */
#define Sn_IR_TIMEOUT			0x08		/**< assert timeout */
#define Sn_IR_RECV			0x04		/**< receiving data */
#define Sn_IR_DISCON			0x02		/**< closed socket */
#define Sn_IR_CON			0x01		/**< established connection */

/* Sn_SR values */
#define SOCK_CLOSED			0x00		/**< closed */
#define SOCK_INIT 			0x13		/**< init state */
#define SOCK_LISTEN			0x14		/**< listen state */
#define SOCK_SYNSENT	   		0x15		/**< connection state */
#define SOCK_SYNRECV		   	0x16		/**< connection state */
#define SOCK_ESTABLISHED		0x17		/**< success to connect */
#define SOCK_FIN_WAIT			0x18		/**< closing state */
//#define SOCK_CLOSING		   	0x1A		/**< closing state */
//#define SOCK_TIME_WAIT			0x1B		/**< closing state */
//#define SOCK_CLOSE_WAIT			0x1C		/**< closing state */
//#define SOCK_LAST_ACK			0x1D		/**< closing state */
#define SOCK_UDP			0x22		/**< udp socket */
#define SOCK_IPRAW			0x32		/**< ip raw mode socket */
//#define SOCK_MACRAW			0x42		/**< mac raw mode socket */
#define SOCK_PPPOE			0x5F		/**< pppoe socket */

/* IP PROTOCOL */
#define IPPROTO_IP              0           /**< Dummy for IP */
#define IPPROTO_ICMP            1           /**< Control message protocol */
#define IPPROTO_IGMP            2           /**< Internet group management protocol */
#define IPPROTO_GGP             3           /**< Gateway^2 (deprecated) */
#define IPPROTO_TCP             6           /**< TCP */
#define IPPROTO_PUP             12          /**< PUP */
#define IPPROTO_UDP             17          /**< UDP */
#define IPPROTO_IDP             22          /**< XNS idp */
#define IPPROTO_ND              77          /**< UNOFFICIAL net disk protocol */
#define IPPROTO_RAW             255         /**< Raw IP packet */

/*********************************************************
* iinchip access function
*********************************************************/
uint8 IINCHIP_READ(uint16 addr); 
uint8 IINCHIP_WRITE(uint16 addr,uint8 data);
void iinchip_hw_init(void);
void iinchip_init(void); // reset iinchip
void sysinit(uint8 * tx_size, uint8 * rx_size); // setting tx/rx buf size
uint8 getISR(uint8 s);
void putISR(uint8 s, uint8 val);
uint16 getIINCHIP_RxMAX(uint8 s);
uint16 getIINCHIP_TxMAX(uint8 s);
uint16 getIINCHIP_RxMASK(uint8 s);
uint16 getIINCHIP_TxMASK(uint8 s);

void setMR(uint8 val);
void setRTR(uint16 timeout); // set retry duration for data transmission, connection, closing ...
void setRCR(uint8 retry); // set retry count (above the value, assert timeout interrupt)
void setIMR(uint8 mask); // set interrupt mask. 
uint8 getIR( void );
void setSn_MSS(SOCKET s, uint16 Sn_MSSR0); // set maximum segment size
void setSn_PROTO(SOCKET s, uint8 proto); // set IP Protocol value using IP-Raw mode
uint8 getSn_IR(SOCKET s); // get socket interrupt status
uint8 getSn_SR(SOCKET s); // get socket status
uint16 getSn_TX_FSR(SOCKET s); // get socket TX free buf size
uint16 getSn_RX_RSR(SOCKET s); // get socket RX recv buf size
uint8 getSn_SR(SOCKET s);
void setSn_TTL(SOCKET s, uint8 ttl);
void send_data_processing(SOCKET s, uint8 *wizdata, uint16 len);
void recv_data_processing(SOCKET s, uint8 *wizdata, uint16 len);
void write_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len);
void read_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len);

void setGAR(uint8 * addr); // set gateway address
void setSUBR(uint8 * addr); // set subnet mask address
void setSHAR(uint8 * addr); // set local MAC address
void setSIPR(uint8 * addr); // set local IP address
void getGAR(uint8 * addr);
void getSUBR(uint8 * addr);
void getSHAR(uint8 * addr);
void getSIPR(uint8 * addr);
void setSn_IR(uint8 s, uint8 val);

uint8 incr_windowfull_retry_cnt(uint8 s);
void init_windowfull_retry_cnt(uint8 s);

#endif
