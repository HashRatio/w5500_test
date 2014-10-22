/*
 * Author: Xiangfu Liu <xiangfu@openmobilefree.net>
 * Bitcoin:	1CanaaniJzgps8EV6Sfmpb7T8RutpaeyFn
 *
 * This is free and unencumbered software released into the public domain.
 * For details see the UNLICENSE file at the root of the source tree.
 */

#include <stdint.h>

#include "minilibc.h"
#include "system_config.h"
#include "defines.h"
#include "intr.h"
#include "io.h"

#define UART_RINGBUFFER_SIZE_RX 128
#define UART_RINGBUFFER_MASK_RX (UART_RINGBUFFER_SIZE_RX-1)

static char rx_buf[4][UART_RINGBUFFER_SIZE_RX];
static volatile unsigned int rx_produce[4];
static volatile unsigned int rx_consume[4];

static struct lm32_uart *uart[4] = {(struct lm32_uart *)UART0_BASE, (struct lm32_uart *)UART2_BASE
, (struct lm32_uart *)UART3_BASE,(struct lm32_uart *)UART4_BASE};

static void uart_write32(unsigned int data)
{
	int i;
	unsigned char * p = (unsigned char *)&data;
	for(i=0;i<4;i++){		
		uart_write(1,p[i]);
	}
}

void uart_test(void)
{
	/*unsigned char sta;
	sta = readb(&uart->lsr);
	uart1_write(sta);
	sta = readb(&uart->rxtx);
	uart1_write(sta);*/
	unsigned int mask;
    __asm__ __volatile__("rcsr %0, IM" : "=r" (mask));
	uart_write32(mask);
	
}

void uart_isr(void)
   {    int i;
        for(i=0;i<4;i++) 
	while (readb(&((uart[i])->lsr)) & LM32_UART_LSR_DR) {
           	rx_buf[i][rx_produce[i]] = readb(&uart[i]->rxtx);
              //  uart1_write(rx_buf[rx_produce]);
		rx_produce[i] = (rx_produce[i] + 1) & UART_RINGBUFFER_MASK_RX;
	}
	irq_ack(IRQ_UART); 
	irq_ack(IRQ_UART2);
	irq_ack(IRQ_UART3);
	irq_ack(IRQ_UART4);
}
/*
void uart2_isr(void)
   {
        while (readb(&uart2->lsr) & LM32_UART_LSR_DR) {
                rx_buf2[rx_produce2] = readb(&uart2->rxtx);
              //  uart1_write(rx_buf[rx_produce]);
                rx_produce2 = (rx_produce2 + 1) & UART_RINGBUFFER_MASK_RX;
        }
        irq_ack(IRQ_UART2);
}

void uart3_isr(void)
   {
        while (readb(&uart3->lsr) & LM32_UART_LSR_DR) {
                rx_buf3[rx_produce3] = readb(&uart3->rxtx);
              //  uart1_write(rx_buf[rx_produce]);
                rx_produce3 = (rx_produce3 + 1) & UART_RINGBUFFER_MASK_RX;
        }
        irq_ack(IRQ_UART3);
}

void uart4_isr(void)
   {
        while (readb(&uart4->lsr) & LM32_UART_LSR_DR) {
                rx_buf4[rx_produce4] = readb(&uart4->rxtx);
              //  uart1_write(rx_buf[rx_produce]);
                rx_produce4 = (rx_produce4 + 1) & UART_RINGBUFFER_MASK_RX;
        }
        irq_ack(IRQ_UART4);
}
*/
/* Do not use in interrupt handlers! */
char uart_read(int board)
{
       char c=0;
	while (rx_consume[board] == rx_produce[board]);
	c = rx_buf[board][rx_consume[board]];
	rx_consume[board] = (rx_consume[board] + 1) & UART_RINGBUFFER_MASK_RX;	

/* 
       if(board==2) {
	while (rx_consume2 == rx_produce2);
	c = rx_buf2[rx_consume2];
	rx_consume2 = (rx_consume2 + 1) & UART_RINGBUFFER_MASK_RX;	
        }

       if(board==3) {
	while (rx_consume3 == rx_produce3);
	c = rx_buf3[rx_consume3];
	rx_consume3 = (rx_consume3 + 1) & UART_RINGBUFFER_MASK_RX;	
        }

       if(board==4) {
	while (rx_consume4 == rx_produce4);
	c = rx_buf4[rx_consume4];
	rx_consume4 = (rx_consume4 + 1) & UART_RINGBUFFER_MASK_RX;	
        }
*/
	return c;
}

int uart_read_nonblock(int board)
{
	return (rx_consume[board] != rx_produce[board]);
}

void uart_write(int board, char c)
{
	unsigned int oldmask;
        
        oldmask = irq_getmask();
        irq_setmask(0);

        while (!(readb(&uart[board]->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
        writeb(0xef&LM32_UART_LCR_8BIT, &uart[board]->lcr); 
        writeb(c, &uart[board]->rxtx); 
/*
if(board==2){
        while (!(readb(&uart2->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
        writeb(0xef&LM32_UART_LCR_8BIT, &uart2->lcr); 
        writeb(c, &uart2->rxtx);
    }

if(board==3){
        while (!(readb(&uart3->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
        writeb(0xef&LM32_UART_LCR_8BIT, &uart3->lcr); 
        writeb(c, &uart3->rxtx);
    }

if(board==4){
        while (!(readb(&uart4->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
        writeb(0xef&LM32_UART_LCR_8BIT, &uart4->lcr); 
        writeb(c, &uart4->rxtx);
    }
*/
        irq_setmask(oldmask);
}

void uart_writecmd(int board ,char c)
{
	unsigned int oldmask;
	oldmask = irq_getmask();
	irq_setmask(0);

	while (!(readb(&uart[board]->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
	writeb(LM32_UART_LCR_8BIT, &uart[board]->lcr);
	writeb(c, &uart[board]->rxtx);
/*
if(board==2){
	while (!(readb(&uart2->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
        writeb(LM32_UART_LCR_8BIT, &uart2->lcr);
	writeb(c, &uart2->rxtx);
}

if(board==3){
	while (!(readb(&uart3->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
        writeb(LM32_UART_LCR_8BIT, &uart3->lcr);
	writeb(c, &uart3->rxtx);
}

if(board==4){
	while (!(readb(&uart4->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)));
        writeb(LM32_UART_LCR_8BIT, &uart4->lcr);
	writeb(c, &uart4->rxtx);
}
*/
	irq_setmask(oldmask);
}

void uart_init(void)
{
	uint32_t mask;
	uint8_t value;
        int i;
        for(i=0;i<4;i++)
{
	rx_produce[i] = 0;
	rx_consume[i] = 0;
}
	irq_ack(IRQ_UART);
	irq_ack(IRQ_UART2);
	irq_ack(IRQ_UART3);
	irq_ack(IRQ_UART4);

	/* enable UART interrupts */
	writeb(LM32_UART_IER_RBRI, &uart[0]->ier); 
	writeb(LM32_UART_IER_RBRI, &uart[1]->ier);
	writeb(LM32_UART_IER_RBRI, &uart[2]->ier);
	writeb(LM32_UART_IER_RBRI, &uart[3]->ier);
	mask = irq_getmask();
	mask |= IRQ_UART|IRQ_UART2|IRQ_UART3|IRQ_UART4;
	irq_setmask(mask);

        for(i=0;i<4;i++)
    {	/* Line control 8 bit, 1 stop, no parity */
	writeb(LM32_UART_LCR_8BIT, &uart[i]->lcr);

	/* Modem control, DTR = 1, RTS = 1 */
	writeb(LM32_UART_MCR_DTR | LM32_UART_MCR_RTS, &uart[i]->mcr);

	/* Set baud rate */
	value = (CPU_FREQUENCY / UART_BAUD_RATE) & 0xff;
	writeb(value, &uart[i]->divl);
	value = (CPU_FREQUENCY / UART_BAUD_RATE) >> 8;
	writeb(value, &uart[i]->divh);
      }
}

void uart_puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			uart_write(1,'\r');
		uart_write(1,*s++);
	}
}

void uart_nwrite(const char *s, unsigned int l)
{
	while (l--)
		uart_write(1,*s++);
}


#ifdef DEBUG
#define UART1_RINGBUFFER_SIZE_RX 512
#define UART1_RINGBUFFER_MASK_RX (UART1_RINGBUFFER_SIZE_RX-1)

//UART1 rx buffer is limited up to 16 bytes, DO NOT send data more than that at one time.
static char rx_dbg_buf[UART1_RINGBUFFER_SIZE_RX];
static volatile unsigned int rx_dbg_produce;
static volatile unsigned int rx_dbg_consume;

static struct lm32_uart *uart1 = (struct lm32_uart *)UART1_BASE;

void uart1_init(void)
{	
	uint8_t value;
	uint32_t mask;
		
	rx_dbg_produce = 0;
	rx_dbg_consume = 0;

	irq_ack(IRQ_UARTDEBUG);

	/* enable UART interrupts */
	writeb(LM32_UART_IER_RBRI, &uart1->ier);
	mask = irq_getmask();
	mask |= IRQ_UARTDEBUG;
	irq_setmask(mask);
	
	/* Line control 8 bit, 1 stop, no parity */
	writeb(LM32_UART_LCR_8BIT, &uart1->lcr);

	/* Modem control, DTR = 1, RTS = 1 */
	writeb(LM32_UART_MCR_DTR | LM32_UART_MCR_RTS, &uart1->mcr);

	/* Set baud rate */
	value = (CPU_FREQUENCY / UART_BAUD_RATE) & 0xff;
	writeb(value, &uart1->divl);
	value = (CPU_FREQUENCY / UART_BAUD_RATE) >> 8;
	writeb(value, &uart1->divh);
}

void uart1_writecmd(char c)
{
	unsigned int oldmask;

	oldmask = irq_getmask();
	irq_setmask(0);

	if (c == '\n')
		uart1_write('\r');

	while (!(readb(&uart1->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)))
		;
         writeb(LM32_UART_LCR_8BIT, &uart1->lcr);
        writeb(c, &uart1->rxtx);
        
        irq_setmask(oldmask);
}

void uart1_write(char c)
{
	unsigned int oldmask;
      int i=0;

	oldmask = irq_getmask();
	irq_setmask(0);

	if (c == '\n')
		uart1_write('\r');
//      writeb((0xef&LM32_UART_LCR_8BIT), &uart1->lcr);
    //   writeb(0xef&LM32_UART_LCR_8BIT, &uart1->rxtx);
     //  writeb(uart1->lcr, &uart1->rxtx);
	while (!(readb(&uart1->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)))
		;
        writeb(0xef&LM32_UART_LCR_8BIT, &uart1->lcr);
       while(i<1000)
       {i++;}
       i=0;
       // writeb(uart1->lcr, &uart1->rxtx);
	writeb(c, &uart1->rxtx);
      //	writeb(LM32_UART_LCR_8BIT, &uart1->lcr);
	irq_setmask(oldmask);
}

void uart1_writeb(unsigned char b)
{
	unsigned int oldmask;

	oldmask = irq_getmask();
	irq_setmask(0);

	while (!(readb(&uart1->lsr) & (LM32_UART_LSR_THRR | LM32_UART_LSR_TEMT)))
		;
	writeb(b, &uart1->rxtx);

	irq_setmask(oldmask);

}

void uart1_writew(unsigned short w)
{
	unsigned char i;
	unsigned char * b = (unsigned char *)&w;
	for(i=0;i<2;i++){
		uart1_writeb(b[i]);
	}
}

void uart1_writel(unsigned int l)
{
	unsigned char i;
	unsigned char * b = (unsigned char *)&l;
	for(i=0;i<4;i++){
		uart1_writeb(b[i]);
	}
}

void uart1_isr(void)
{
	while (readb(&uart1->lsr) & LM32_UART_LSR_DR) {
		rx_dbg_buf[rx_dbg_produce] = readb(&uart1->rxtx);
		rx_dbg_produce = (rx_dbg_produce + 1) & UART1_RINGBUFFER_MASK_RX;
	}
	irq_ack(IRQ_UARTDEBUG);
}

/* Do not use in interrupt handlers! */
char uart1_read(void)
{
	char c;
	while (rx_dbg_consume == rx_dbg_produce);
	c = rx_dbg_buf[rx_dbg_consume];
	rx_dbg_consume = (rx_dbg_consume + 1) & UART1_RINGBUFFER_MASK_RX;
	return c;
}

int uart1_read_nonblock(void)
{
	return (rx_dbg_consume != rx_dbg_produce);
}


void uart1_puts(const char *s)
{
	while (*s)
		uart1_write(*s++);
}
#endif /* DEBUG */
