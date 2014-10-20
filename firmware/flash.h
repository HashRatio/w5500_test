#include "types.h"

//#define WRITE_INSTRUCTION         0x0b
#define PAGE_PROGRAM              0x02
#define READ_INSTRUCTION          0x03
#define READ_STATUS_INSTRUCTION   0x05
#define WRITE_STATUS_INSTRUCTION  0x01 
#define WRITE_ENABLE_INSTRUCTION  0x06
#define WRITE_DISABLE_INSTRUCTION 0x04
#define W25X_ChipErase            0xc7
#define W25X_SectorErase          0x20
void flash_INCHIP_CSoff(void);
void flash_IINCHIP_CSon(void);
uint8  flash_IINCHIP_SpiSendData(uint8 dat);
uint8 SPI_Flash_ReadSR(void);
uint8 SPI_Flash_WriteSR(uint8 status);
void SPI_FLASH_Write_Enable(void);
void SPI_FLASH_Write_Disable(void);
uint16 SPI_Flash_ReadID(void);
uint16 fwiz_write_buf(uint32 addr,uint8* buf,uint16 len);
uint16 flash_wiz_read_buf(uint32 addr, uint8* buf,uint16 len);
void SPI_Flash_Erase_Chip(void);
void SPI_Flash_Wait_Busy(void);
void SPI_Flash_Erase_Sector(uint32 Dst_Addr);
struct config config_read();
void config_recover(void);
void config_write(struct config *configread);
struct config configbak_read();
void configbak_write(struct config *configread);





