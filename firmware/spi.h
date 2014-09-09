#ifndef __SPI2_H
#define __SPI2_H

#include "types.h"

#define LOW 0
#define HIGH 1
void WIZ_HW_RESET(void);
void WIZ_CS(uint8 val);
uint8 SPI1_SendByte(uint8 byte);

#endif

