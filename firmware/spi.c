#include "system_config.h"
#include "spi.h"
#include "utils.h"
#include "io.h"
#include "uart.h"
#include "defines.h"

static struct lm32_spi * w5500_spi = (struct lm32_spi *)W5500_SPI_BASE;
static struct lm32_timer *tim = (struct lm32_timer *)TIMER_BASE;

static struct lm32_spi * flash_spi = (struct lm32_spi *)SPI1_BASE;

void WIZ_HW_RESET(void)
{
	WIZ_CS(1);
	writel(tim->gpio&0xFFFFFFFE,&tim->gpio);
	delay(200);
	writel(tim->gpio|0x00000001,&tim->gpio);
	delay(200);
}

void WIZ_CS(uint8 val)
{
	if (val == LOW) {
   		writel(tim->gpio&0xFFFFFFFD,&tim->gpio);
	}else if (val == HIGH){
   		writel(tim->gpio|0x00000002,&tim->gpio);
	}
}

void flash_WIZ_CS(uint8 val)
{
        if (val == LOW) {
                writel(tim->gpio&0xFFFFFFFB,&tim->gpio);
        }else if (val == HIGH){
                writel(tim->gpio|0x00000004,&tim->gpio);
        }
}

uint8 SPI1_SendByte(uint8 byte)
{
	unsigned char rByte=0;
	writeb(1,&w5500_spi->ssmask);
	while (!(w5500_spi->status & (LM32_SPI_STAT_TRDY)));
	writeb(byte,&w5500_spi->tx);
	while (!(w5500_spi->status & (LM32_SPI_STAT_RRDY)));
	rByte = w5500_spi->rx;
	while (!(w5500_spi->status & (LM32_SPI_STAT_TMT)));
	return rByte;
}

uint8 flash_SPI1_SendByte(uint8 byte)
{
        unsigned char rByte=0;
        writeb(1,&flash_spi->ssmask);
        while (!(flash_spi->status & (LM32_SPI_STAT_TRDY)));
        writeb(byte,&flash_spi->tx);
        while (!(flash_spi->status & (LM32_SPI_STAT_RRDY)));
        rByte = flash_spi->rx;
        while (!(flash_spi->status & (LM32_SPI_STAT_TMT)));
       // debug32("flash");
        return rByte;
}

