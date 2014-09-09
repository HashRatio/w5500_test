/*
 * (c)COPYRIGHT
 * ALL RIGHT RESERVED
 *
 * FileName : w5500.c
  * -----------------------------------------------------------------
 */
//#include <stdio.h>
//#include <string.h>
#include "minilibc.h"
#include "defines.h"
#include "spi.h"		
#include "w5500.h"
#include "socket.h"

static uint8 I_STATUS[MAX_SOCK_NUM];
static uint16 SMASK[MAX_SOCK_NUM]; /**< Variable for Tx buffer MASK in each channel */
static uint16 RMASK[MAX_SOCK_NUM]; /**< Variable for Rx buffer MASK in each channel */
static uint16 SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static uint16 RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */

uint8 windowfull_retry_cnt[MAX_SOCK_NUM];

uint8 incr_windowfull_retry_cnt(uint8 s)
{
  return windowfull_retry_cnt[s]++;
}

void init_windowfull_retry_cnt(uint8 s)
{
  windowfull_retry_cnt[s] = 0;
}

uint16 pre_sent_ptr, sent_ptr;

uint8 getISR(uint8 s)
{
	return I_STATUS[s];
}
void putISR(uint8 s, uint8 val)
{
   I_STATUS[s] = val;
}
uint16 getIINCHIP_RxMAX(uint8 s)
{
   return RSIZE[s];
}
uint16 getIINCHIP_TxMAX(uint8 s)
{
   return SSIZE[s];
}
uint16 getIINCHIP_RxMASK(uint8 s)
{
   return RMASK[s];
}
uint16 getIINCHIP_TxMASK(uint8 s)
{
   return SMASK[s];
}


void IINCHIP_CSoff(void)
{
	WIZ_CS(LOW);
}
void IINCHIP_CSon(void)
{
	WIZ_CS(HIGH);
}
uint8  IINCHIP_SpiSendData(uint8 dat)
{
	return(SPI1_SendByte(dat));
}


 /**
@brief	This function writes the data into W5200 registers.
*/

uint8 IINCHIP_WRITE(uint16 addr,uint8 data)
{
    IINCHIP_ISR_DISABLE();                      // Interrupt Service Routine Disable

	//SPI MODE I/F
	IINCHIP_CSoff();                            // CS=0, SPI start

	IINCHIP_SpiSendData((addr & 0x0700) >> 8);  // Address byte 1
	IINCHIP_SpiSendData(addr & 0x00FF);         // Address byte 2
	IINCHIP_SpiSendData(((addr&0xF800) >> 8)|FDM1|RWB_WRITE);        // Data write command and Write data length 1
	IINCHIP_SpiSendData(data);                  // Data write (write 1byte data)
	
	IINCHIP_CSon();                             // CS=1,  SPI end

    IINCHIP_ISR_ENABLE();                       // Interrupt Service Routine Enable
	return 1;
}
/**
@brief	This function reads the value from W5200 registers.
*/
uint8 IINCHIP_READ(uint16 addr)
{
	uint8 data;
        
	IINCHIP_ISR_DISABLE();                       // Interrupt Service Routine Disable
	
	IINCHIP_CSoff();                             // CS=0, SPI start
	
    IINCHIP_SpiSendData((addr & 0x0700) >> 8);   // Address byte 1
	IINCHIP_SpiSendData(addr & 0x00FF);          // Address byte 2
	IINCHIP_SpiSendData(((addr & 0xF800) >> 8)|FDM1|RWB_READ);                    // Data read command and Read data length 1
	data = IINCHIP_SpiSendData(0x00);                   // Read data 
	
    IINCHIP_CSon();                              // CS=1,  SPI end
	
    IINCHIP_ISR_ENABLE();                        // Interrupt Service Routine Enable
	return data;
}

/**
@brief	This function writes into W5200 memory(Buffer)
*/ 
uint16 wiz_write_buf(uint16 addr,uint8* buf,uint16 len)
{
	uint16 idx = 0;
	
	IINCHIP_ISR_DISABLE();
     
	//SPI MODE I/F
	IINCHIP_CSoff();                                        // CS=0, SPI start 
	
    IINCHIP_SpiSendData((addr & 0x0700) >> 8);        // Address byte 1
	IINCHIP_SpiSendData((addr) & 0x00FF);               // Address byte 2
	IINCHIP_SpiSendData(((addr & 0xF800) >> 8)|RWB_WRITE|VDM);    // Data write command and Write data length 1
	for(idx = 0; idx < len; idx++)                          // Write data in loop
	{	
		IINCHIP_SpiSendData(buf[idx]);
	}
	
    IINCHIP_CSon();                                         // CS=1, SPI end 
        
    IINCHIP_ISR_ENABLE();                                   // Interrupt Service Routine Enable        
	return len;
}


/**
@brief	This function reads into W5200 memory(Buffer)
*/ 
uint16 wiz_read_buf(uint16 addr, uint8* buf,uint16 len)
{
	uint16 idx = 0;
        
	IINCHIP_ISR_DISABLE();                                  // Interrupt Service Routine Disable
        
	IINCHIP_CSoff();                                        // CS=0, SPI start 
        
	IINCHIP_SpiSendData((addr & 0x0700) >> 8);        // Address byte 1
	IINCHIP_SpiSendData( addr & 0x00FF);               // Address byte 2
	IINCHIP_SpiSendData(((addr & 0xF800)>>8)|RWB_READ|VDM);    // Data read command

	for(idx = 0; idx < len; idx++)                          // Read data in loop
	{
	 	buf[idx] = IINCHIP_SpiSendData(0x00);
	}
        
	IINCHIP_CSon();                                         // CS=0, SPI end 	   	
        
	IINCHIP_ISR_ENABLE();                                   // Interrupt Service Routine Enable
	return len;
}

void iinchip_hw_init(void)
{
	WIZ_HW_RESET();
	while(!(IINCHIP_READ(PHYCFGR)&PHYCFGR_LINK));
}

/**
@brief	This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode
*/ 
void iinchip_init(void)
{	
	setMR( MR_RST );
#ifdef __DEF_IINCHIP_DBG__	
	printf("MR value is %02x \r\n",IINCHIP_READ(MR));
#endif	
}


/**
@brief	This function set the transmit & receive buffer size as per the channels is used
Note for TMSR and RMSR bits are as follows\n
bit 1-0 : memory size of channel #0 \n
bit 3-2 : memory size of channel #1 \n
bit 5-4 : memory size of channel #2 \n
bit 7-6 : memory size of channel #3 \n
bit 9-8 : memory size of channel #4 \n
bit 11-10 : memory size of channel #5 \n
bit 12-12 : memory size of channel #6 \n
bit 15-14 : memory size of channel #7 \n
Maximum memory size for Tx, Rx in the W5200 is 16K Bytes,\n
In the range of 16KBytes, the memory size could be allocated dynamically by each channel.\n
Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
and to data transmission and receiption from non-allocated channel may cause some problems.\n
If the 16KBytes memory is already  assigned to centain channel, \n
other 3 channels couldn't be used, for there's no available memory.\n
If two 4KBytes memory are assigned to two each channels, \n
other 2 channels couldn't be used, for there's no available memory.\n
*/ 
void sysinit( uint8 * tx_size, uint8 * rx_size	)
{
	int16 i;
	int16 ssum,rsum;

#ifdef __DEF_IINCHIP_DBG__
	printf("sysinit()\r\n"); 
#endif

	ssum = 0;
	rsum = 0;

  for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
	{
    IINCHIP_WRITE((Sn_TXMEM_SIZE(i)),tx_size[i]);
    IINCHIP_WRITE((Sn_RXMEM_SIZE(i)),rx_size[i]);

#ifdef __DEF_IINCHIP_DBG__
         printf("Sn_TXMEM_SIZE = %d\r\n",IINCHIP_READ(Sn_TXMEM_SIZE(i)));
         printf("Sn_RXMEM_SIZE = %d\r\n",IINCHIP_READ(Sn_RXMEM_SIZE(i)));
#endif

		SSIZE[i] = (int16)(0);
		RSIZE[i] = (int16)(0);

		if (ssum <= 16384)
		{
         switch( tx_size[i] )
			{
			case 1:
				SSIZE[i] = (int16)(1024);
				SMASK[i] = (uint16)(0x03FF);
				break;
			case 2:
				SSIZE[i] = (int16)(2048);
				SMASK[i] = (uint16)(0x07FF);
				break;
			case 4:
				SSIZE[i] = (int16)(4096);
				SMASK[i] = (uint16)(0x0FFF);
				break;
			case 8:
				SSIZE[i] = (int16)(8192);
				SMASK[i] = (uint16)(0x1FFF);
				break;
			case 16:
				SSIZE[i] = (int16)(16384);
				SMASK[i] = (uint16)(0x3FFF);
			break;
			}
		}

		if (rsum <= 16384)
		{
         switch( rx_size[i] )
			{
			case 1:
				RSIZE[i] = (int16)(1024);
				RMASK[i] = (uint16)(0x03FF);
				break;
			case 2:
				RSIZE[i] = (int16)(2048);
				RMASK[i] = (uint16)(0x07FF);
				break;
			case 4:
				RSIZE[i] = (int16)(4096);
				RMASK[i] = (uint16)(0x0FFF);
				break;
			case 8:
				RSIZE[i] = (int16)(8192);
				RMASK[i] = (uint16)(0x1FFF);
				break;
			case 16:
				RSIZE[i] = (int16)(16384);
				RMASK[i] = (uint16)(0x3FFF);
				break;
			}
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];

#ifdef __DEF_IINCHIP_DBG__
		printf("ch = %d\r\n",i);
		printf("SSIZE = %d\r\n",SSIZE[i]);
		printf("RSIZE = %d\r\n",RSIZE[i]);		
#endif
	}
}

// added

/**
@brief	This function sets up gateway IP address.
*/ 
void setGAR(
	uint8 * addr	/**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
	)
{
	IINCHIP_WRITE((GAR + 0),addr[0]);
	IINCHIP_WRITE((GAR + 1),addr[1]);
	IINCHIP_WRITE((GAR + 2),addr[2]);
	IINCHIP_WRITE((GAR + 3),addr[3]);
}

/*
void getGWIP(uint8 * addr)
{
	addr[0] = IINCHIP_READ((GAR0 + 0));
	addr[1] = IINCHIP_READ((GAR0 + 1));
	addr[2] = IINCHIP_READ((GAR0 + 2));
	addr[3] = IINCHIP_READ((GAR0 + 3));
}
*/

/**
@brief	It sets up SubnetMask address
*/ 
void setSUBR(
	uint8 * addr	/**< a pointer to a 4 -byte array responsible to set the SubnetMask address */
	)
{
	IINCHIP_WRITE((SUBR + 0),addr[0]);
	IINCHIP_WRITE((SUBR + 1),addr[1]);
	IINCHIP_WRITE((SUBR + 2),addr[2]);
	IINCHIP_WRITE((SUBR + 3),addr[3]);
}


/**
@brief	This function sets up MAC address.
*/ 
void setSHAR(
	uint8 * addr	/**< a pointer to a 6 -byte array responsible to set the MAC address. */
	)
{
	IINCHIP_WRITE((SHAR + 0),addr[0]);
	IINCHIP_WRITE((SHAR + 1),addr[1]);
	IINCHIP_WRITE((SHAR + 2),addr[2]);
	IINCHIP_WRITE((SHAR + 3),addr[3]);
	IINCHIP_WRITE((SHAR + 4),addr[4]);
	IINCHIP_WRITE((SHAR + 5),addr[5]);
}

/**
@brief	This function sets up Source IP address.
*/
void setSIPR(
	uint8 * addr	/**< a pointer to a 4 -byte array responsible to set the Source IP address. */
	)
{
	IINCHIP_WRITE((SIPR + 0),addr[0]);
	IINCHIP_WRITE((SIPR + 1),addr[1]);
	IINCHIP_WRITE((SIPR + 2),addr[2]);
	IINCHIP_WRITE((SIPR + 3),addr[3]);
}

/**
@brief	This function sets up Source IP address.
*/
void getGAR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(GAR);
	addr[1] = IINCHIP_READ(GAR+1);
	addr[2] = IINCHIP_READ(GAR+2);
	addr[3] = IINCHIP_READ(GAR+3);
}
void getSUBR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SUBR);
	addr[1] = IINCHIP_READ(SUBR+1);
	addr[2] = IINCHIP_READ(SUBR+2);
	addr[3] = IINCHIP_READ(SUBR+3);
}
void getSHAR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SHAR);
	addr[1] = IINCHIP_READ(SHAR+1);
	addr[2] = IINCHIP_READ(SHAR+2);
	addr[3] = IINCHIP_READ(SHAR+3);
	addr[4] = IINCHIP_READ(SHAR+4);
	addr[5] = IINCHIP_READ(SHAR+5);
}
void getSIPR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SIPR);
	addr[1] = IINCHIP_READ(SIPR+1);
	addr[2] = IINCHIP_READ(SIPR+2);
	addr[3] = IINCHIP_READ(SIPR+3);
}

void setMR(uint8 val)
{
	IINCHIP_WRITE(MR,val);
}

/**
@brief	This function gets Interrupt register in common register.
 */
uint8 getIR( void )
{
   return IINCHIP_READ(IR);
}


/**
 Retransmittion 
 **/
 
/**
@brief	This function sets up Retransmission time.

If there is no response from the peer or delay in response then retransmission 
will be there as per RTR (Retry Time-value Register)setting
*/
void setRTR(uint16 timeout)
{
	IINCHIP_WRITE(RTR,(uint8)((timeout & 0xff00) >> 8));
	IINCHIP_WRITE((RTR + 1),(uint8)(timeout & 0x00ff));
}

/**
@brief	This function set the number of Retransmission.

If there is no response from the peer or delay in response then recorded time 
as per RTR & RCR register seeting then time out will occur.
*/
void setRCR(uint8 retry)
{
	IINCHIP_WRITE(RCR,retry);
}




/**
@brief	This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)

If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
set in IR register.
*/
void setIMR(uint8 mask)
{
	IINCHIP_WRITE(IMR,mask); // must be setted 0x10.
}

/**
@brief	This sets the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*/
void setSn_MSS(SOCKET s, uint16 Sn_MSSR0)
{
	IINCHIP_WRITE(Sn_MSSR0(s),(uint8)((Sn_MSSR0 & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_MSSR0(s) + 1),(uint8)(Sn_MSSR0 & 0x00ff));
}

void setSn_TTL(SOCKET s, uint8 ttl)
{
   IINCHIP_WRITE(Sn_TTL(s), ttl);
}


/**
@brief	These below function is used to setup the Protocol Field of IP Header when
		executing the IP Layer RAW mode.
*/
void setSn_PROTO(SOCKET s, uint8 proto)
{
	IINCHIP_WRITE(Sn_PROTO(s),proto);
}


/**
@brief	get socket interrupt status

These below functions are used to read the Interrupt & Soket Status register
*/
uint8 getSn_IR(SOCKET s)
{
   return IINCHIP_READ(Sn_IR(s));
}


/**
@brief	 get socket status
*/
uint8 getSn_SR(SOCKET s)
{
   return IINCHIP_READ(Sn_SR(s));
}


/**
@brief	get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16 getSn_TX_FSR(SOCKET s)
{
	uint16 val=0,val1=0;
	do
	{
		val1 = IINCHIP_READ(Sn_TX_FSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
      if (val1 != 0)
		{
   			val = IINCHIP_READ(Sn_TX_FSR0(s));
   			val = (val << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
		}
	} while (val != val1);
   return val;
}


/**
@brief	 get socket RX recv buf size

This gives size of received data in receive buffer. 
*/
uint16 getSn_RX_RSR(SOCKET s)
{
	uint16 val=0,val1=0;
	do
	{
		val1 = IINCHIP_READ(Sn_RX_RSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
      if(val1 != 0)
		{
   			val = IINCHIP_READ(Sn_RX_RSR0(s));
   			val = (val << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
		}
	} while (val != val1);
   return val;
}


/**
@brief	 This function is being called by send() and sendto() function also. 

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
*/
void send_data_processing(SOCKET s, uint8 *data, uint16 len)
{
	
	uint16 ptr;
	ptr = IINCHIP_READ(Sn_TX_WR0(s));
	ptr = (ptr << 8) + IINCHIP_READ(Sn_TX_WR0(s) + 1);
	write_data(s, data, (uint8 *)(uint32)(ptr), len);
	ptr += len;

	IINCHIP_WRITE(Sn_TX_WR0(s),(uint8)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_TX_WR0(s) + 1),(uint8)(ptr & 0x00ff));
	
}


/**
@brief	This function is being called by recv() also.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
*/
void recv_data_processing(SOCKET s, uint8 *data, uint16 len)
{
	uint16 ptr;
	ptr = IINCHIP_READ(Sn_RX_RD0(s));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(s) + 1);
	
#ifdef __DEF_IINCHIP_DBG__
	printf(" ISR_RX: rd_ptr : %.4x\r\n", ptr);
#endif

	read_data(s, (uint8 *)(uint32)ptr, data, len); // read data
	ptr += len;
	IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8)(ptr & 0x00ff));
}


/**
@brief	for copy the data form application buffer to Transmite buffer of the chip.

This function is being used for copy the data form application buffer to Transmite
buffer of the chip. It calculate the actual physical address where one has to write
the data in transmite buffer. Here also take care of the condition while it exceed
the Tx memory uper-bound of socket.
*/
void write_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len)
{
	uint16 size;
	uint16 dst_mask;
	uint8 * dst_ptr;

	dst_mask = (uint32)dst & getIINCHIP_TxMASK(s);
	dst_ptr = (uint8 *)(getIINCHIP_TxBASE(s) + dst_mask);
	
	if (dst_mask + len > getIINCHIP_TxMAX(s)) 
	{
		size = getIINCHIP_TxMAX(s) - dst_mask;
		wiz_write_buf((uint32)dst_ptr, (uint8*)src, size);
		src += size;
		size = len - size;
		dst_ptr = (uint8 *)(uint32)(getIINCHIP_TxBASE(s));
		wiz_write_buf((uint32)dst_ptr, (uint8*)src, size);
	} 
	else
	{
		wiz_write_buf((uint32)dst_ptr, (uint8*)src, len);
	}
}


/**
@brief	This function is being used for copy the data form Receive buffer of the chip to application buffer.

It calculate the actual physical address where one has to read
the data from Receive buffer. Here also take care of the condition while it exceed
the Rx memory uper-bound of socket.
*/
void read_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len)
{
	uint16 size;
	uint16 src_mask;
	uint8 * src_ptr;

	src_mask = (uint32)src & getIINCHIP_RxMASK(s);
	src_ptr = (uint8 *)(getIINCHIP_RxBASE(s) + src_mask);
	debug32("read_data\n");
	debug32("src_mask:0x%08x src_ptr:0x%08x len:%d src_mask+len:%d max:%d\n",src_mask,(uint32)src_ptr,len,src_mask+len,getIINCHIP_RxMAX(0));
	if( (src_mask + len) > getIINCHIP_RxMAX(s) ) 
	{
		size = getIINCHIP_RxMAX(s) - src_mask;
		wiz_read_buf((uint32)src_ptr, (uint8*)dst,size);
		dst += size;
		size = len - size;
		src_ptr = (uint8 *)(uint32)(getIINCHIP_RxBASE(s));
		debug32("(in if)src_mask:0x%08x src_ptr:0x%08x\n",src_mask,(uint32)src_ptr);
		wiz_read_buf((uint32)src_ptr, (uint8*) dst,size);
	} 
	else
	{
		debug32("(in else)src_mask:0x%08x src_ptr:0x%08x\n",src_mask,(uint32)src_ptr);
		wiz_read_buf((uint32)src_ptr, (uint8*) dst,len);
	}
}

void setSn_IR(uint8 s, uint8 val)
{
    IINCHIP_WRITE(Sn_IR(s), val);
}
