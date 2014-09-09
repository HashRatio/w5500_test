/*
 * Author: Xiangfu Liu <xiangfu@openmobilefree.net>
 * Bitcoin:	1CanaaniJzgps8EV6Sfmpb7T8RutpaeyFn
 *
 * This is free and unencumbered software released into the public domain.
 * For details see the UNLICENSE file at the root of the source tree.
 */

#ifndef _SYSTEM_CONFIG_H_
#define _SYSTEM_CONFIG_H_

// for test build
//#define AVA2_DEFAULT_MODULES 3
//#define AVA2_DEFAULT_MINERS  20
//#define AVA2_P_TEST_RET   26

//#define BE200_ASIC_FREQUENCY 280
//#define ASIC_COUNT	7

#define ENABLE_LED 1

#define CPU_FREQUENCY		(100 * 1000 * 1000) /* 50Mhz */
#define UART_BAUD_RATE          (115200)

/* Interrupt
 * Define at last few lines of verilog/superkdf9/soc/superkdf9_simple.v
 */
#define IRQ_UART		(0x00000008) /* 3 */
#define IRQ_UARTDEBUG	(0x00000010) /* 4 */

#define IRQ_TIMER0		(0x00000020) /* 5 */
#define IRQ_TIMER1		(0x00000040) /* 6 */

/* Registers */
#define W5500_SPI_BASE	(0x80000000)
#define SPI0_BASE		(0x80000000)
#define UART0_BASE		(0x80000100)
#define UART1_BASE		(0x80000300)
#define SHA256_BASE		(0x80000400)
#define ALINK_BASE		(0x80000500)
#define TWIPWM_BASE		(0x80000600)
#define SHIFTER_BASE	(0x80000614)
#define TIMER_BASE		(0x80000620)
#define SPI1_BASE		(0x80000700)
#define SPI2_BASE		(0x80000800)
#define SPI3_BASE		(0x80000900)
#define SPI4_BASE		(0x80000A00)
#define SPI5_BASE		(0x80000B00)
#define SPI6_BASE		(0x80000C00)
#define SPI7_BASE		(0x80000D00)
#define SPI8_BASE		(0x80000E00)
#define SPI9_BASE		(0x80000F00)


/* SPI */
#define LM32_SPI_STAT_ROE	(1 << 3)
#define LM32_SPI_STAT_TOE	(1 << 4)
#define LM32_SPI_STAT_TMT	(1 << 5)
#define LM32_SPI_STAT_TRDY	(1 << 6)
#define LM32_SPI_STAT_RRDY	(1 << 7)
#define LM32_SPI_STAT_ERR	(1 << 8)

#define LM32_SPI_CTRL_IROE	(1 << 0)
#define LM32_SPI_CTRL_ITOE	(1 << 1)
#define LM32_SPI_CTRL_ITRDY	(1 << 3)
#define LM32_SPI_CTRL_IRRDY	(1 << 4)
#define LM32_SPI_CTRL_IE	(1 << 5)
#define LM32_SPI_CTRL_SSO	(1 << 7)

/*struct lm32_spi {
	volatile unsigned int rx;
	volatile unsigned int tx;
	volatile unsigned int status;
	volatile unsigned int control;
	volatile unsigned int ssmask;
};*/

struct lm32_spi {
	volatile unsigned int rx;
	volatile unsigned int tx;
	volatile unsigned int status;
	volatile unsigned int control;
	volatile unsigned int ssmask;
};

/* UART */
#define LM32_UART_IER_RBRI	(1 << 0)
#define LM32_UART_IER_THRI	(1 << 1)

#define LM32_UART_STAT_RX_EVT	(0x4)
#define LM32_UART_STAT_TX_EVT	(0x2)

/* Line status register */
#define LM32_UART_LSR_DR	(1 << 0)
#define LM32_UART_LSR_THRR	(1 << 5)
#define LM32_UART_LSR_TEMT	(1 << 6)

/* Line control register */
#define LM32_UART_LCR_WLS0	(1 << 0)
#define LM32_UART_LCR_WLS1	(1 << 1)
#define LM32_UART_LCR_8BIT	(LM32_UART_LCR_WLS1 | LM32_UART_LCR_WLS0)

/* Modem control register */
#define LM32_UART_MCR_DTR	(1 << 0)
#define LM32_UART_MCR_RTS	(1 << 1)

struct lm32_uart {
	volatile unsigned char rxtx;
	volatile unsigned char ier;
	volatile unsigned char iir;
	volatile unsigned char lcr;
	volatile unsigned char mcr;
	volatile unsigned char lsr;
	volatile unsigned char msr;
	volatile unsigned char pad0;
	volatile unsigned char divl;
	volatile unsigned char divh;
};

/* SHA256
 * Please read http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
 * Example here: http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA256.pdf
 * 1. Write the CMD_INIT
 * 2. Wait the CMD_DONE when write every 512bits data
 *    You may need pad the data to 512bits
 * 3. Wait teh CMD_DONE. read the result out
 */
#define LM32_SHA256_CMD_INIT	(1 << 0)
#define LM32_SHA256_CMD_DONE	(1 << 1)
#define LM32_SHA256_CMD_RST	(1 << 2)
#define LM32_SHA256_CMD_DBL	(1 << 3)

struct lm32_sha256 {
	volatile unsigned int cmd;
	volatile unsigned int din;
	volatile unsigned int hash;
	volatile unsigned int hi;
	volatile unsigned int pre; /* Please read the A3255 datasheet */
};


/* ALINK */
#define LM32_ALINK_STATE_TXFULL	(1 << 0)
#define LM32_ALINK_STATE_FLUSH	(1 << 1)
#define LM32_ALINK_STATE_TXCOUNT	(0x00007FF0)
#define LM32_ALINK_STATE_RXEMPTY	(1 << 16)
#define LM32_ALINK_STATE_RXCOUNT	(0x3FF00000)
#define LM32_ALINK_STATE_TEST	(1 << 31)

//struct lm32_alink {
//	volatile unsigned int tx;
//	volatile unsigned int state;	/* Read only */
//	volatile unsigned int en;
//	volatile unsigned int busy;	/* Read only */
//	volatile unsigned int rx;
//};

/* TWI PWM */
#define LM32_TWIPWM_CR_ENABLE	(1 << 0)
#define LM32_TWIPWM_CR_TSTART	(1 << 1)
#define LM32_TWIPWM_CR_TDONE	(1 << 2)
#define LM32_TWIPWM_CR_CMD_START	(0 << 4)
#define LM32_TWIPWM_CR_CMD_WD		(1 << 4)
#define LM32_TWIPWM_CR_CMD_RDACK	(2 << 4)
#define LM32_TWIPWM_CR_CMD_STOP		(3 << 4)
#define LM32_TWIPWM_CR_CMD_RCNOACKT	(4 << 4)

#define LM32_TWIPWM_WDG_ENABLE	(1 << 0);
/* [26:1] Feed Dog: 0x3~0x3ffffff;[WR] */

#define LM32_TWI_REG_TEMP	0x48

struct lm32_twipwm {
	volatile unsigned int cr; /* TWI ctrl register */
	volatile unsigned int wd; /* TWI write byte */
	volatile unsigned int rd; /* TWI read byte */
	volatile unsigned int pwm; /* PWM Counter register */
	volatile unsigned int wdg; /* Watch dog ctrl */
	volatile unsigned int pad; /* Watch dog ctrl */
	volatile unsigned int fan0; /* Watch dog ctrl */
	volatile unsigned int fan1; /* Watch dog ctrl */
};

struct lm32_shifter {
	volatile unsigned int reg; /* Shifter register for power chip */
};

struct lm32_timer {
	volatile unsigned int reg; /* Timer register */
	volatile unsigned int gpio; /* GPIO register */
};

#endif /* _SYSTEM_CONFIG_H_ */
