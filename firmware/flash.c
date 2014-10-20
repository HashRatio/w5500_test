#include "minilibc.h"
#include "defines.h"
#include "spi.h"
#include "flash.h"
#include "stratum.h"
#include "uart.h"

void flash_IINCHIP_CSoff(void)
{
        flash_WIZ_CS(LOW);
}

void flash_IINCHIP_CSon(void)
{
        flash_WIZ_CS(HIGH);
}

uint8  flash_IINCHIP_SpiSendData(uint8 dat)
{
   uint8 rbyte;
        rbyte=flash_SPI1_SendByte(dat);  
        debug32("flash=%0x\n",rbyte);
        return(rbyte);
}

/*
@brief  This function writes the data into W5200 registers.


uint8 flash_INCHIP_WRITE(uint16 addr,uint8 data)
{
    //IINCHIP_ISR_DISABLE();                      // Interrupt Service Routine Disable

        //SPI MODE I/F
        flash_INCHIP_CSoff();                            // CS=0, SPI start

        flash_IINCHIP_SpiSendData(WRITE_INSTRUCTION); 
        flash_IINCHIP_SpiSendData((addr & 0x0700) >> 8);  // Address byte 1
        flash_IINCHIP_SpiSendData(addr & 0x00FF);         // Address byte 2
        flash_IINCHIP_SpiSendData(((addr&0xF800) >> 8)|FDM1|RWB_WRITE);        // Data write command and Write data length 1
        flash_IINCHIP_SpiSendData(data);                  // Data write (write 1byte data)

        flash_IINCHIP_CSon();                             // CS=1,  SPI end

//        flash_IINCHIP_ISR_ENABLE();                       // Interrupt Service Routine Enable
        return 1;
}
*/
//@brief  This function reads the value from W5200 registers.
/*
uint8 flash_IINCHIP_READ(uint16 addr)
{
        uint8 data;

//        flash_IINCHIP_ISR_DISABLE();                       // Interrupt Service Routine Disable

        flash_IINCHIP_CSoff();                             // CS=0, SPI start

        flash_IINCHIP_SpiSendData((addr & 0x0700) >> 8);   // Address byte 1
        flash_IINCHIP_SpiSendData(addr & 0x00FF);          // Address byte 2
        flash_IINCHIP_SpiSendData(((addr & 0xF800) >> 8)|FDM1|RWB_READ);                    // Data read command and Read data length 1
        data = flash_IINCHIP_SpiSendData(0x00);                   // Read data 

        flash_IINCHIP_CSon();                              // CS=1,  SPI end

//        flash_IINCHIP_ISR_ENABLE();                        // Interrupt Service Routine Enable
        return data;
}
*/
uint8 SPI_Flash_ReadSR(void)   
{        uint8 byte=0;   
         flash_IINCHIP_CSoff();                            //使能器件   
         flash_IINCHIP_SpiSendData(0x05);
         byte=flash_IINCHIP_SpiSendData(0xff);         
         //发送读取状态寄存器命令    
         debug32("status=%0x\n",byte); 
         flash_IINCHIP_CSon();                              // CS=1,  SPI end                      
         return byte;   
} 

uint8 SPI_Flash_WriteSR(uint8 status)
{
        uint8 byte=0;
         flash_IINCHIP_CSoff();                            //使能器件   
         flash_IINCHIP_SpiSendData(WRITE_STATUS_INSTRUCTION);
         flash_IINCHIP_SpiSendData(status);//SEL = 1
         //发送读取状态寄存器命令    
         //读取一个字节  
         flash_IINCHIP_CSon();                              // CS=1,  SPI end
                            //取消片选     
         return byte;
}

void SPI_FLASH_Write_Enable(void)   
{
         flash_IINCHIP_CSoff();                            //使能器件   
         flash_IINCHIP_SpiSendData(0x06);
         //发送读取状态寄存器命令    
         //读取一个字节  
         flash_IINCHIP_CSon(); 
                        //取消片选            
} 

void SPI_FLASH_Write_Disable(void)
{
         flash_IINCHIP_CSoff();                            //使能器件   
         flash_IINCHIP_SpiSendData(WRITE_DISABLE_INSTRUCTION);
         //发送读取状态寄存器命令    
         //读取一个字节  
         flash_IINCHIP_CSon();
                        //取消片选            
}

uint16 SPI_Flash_ReadID(void)
{
     uint16 Temp = 0;   
        flash_IINCHIP_CSoff();                             // CS=0, SPI start
        flash_IINCHIP_SpiSendData(0x90);          // Address byte 1
        flash_IINCHIP_SpiSendData(0x00);          // Address byte 2
        flash_IINCHIP_SpiSendData(0x00);          // Data read command and Read data length 1
        flash_IINCHIP_SpiSendData(0x00);          // Address byte 2
        Temp|=flash_IINCHIP_SpiSendData(0xff)<<8; // Data read command and Read data length 1
        Temp|=flash_IINCHIP_SpiSendData(0xff);    // Read data 
        flash_IINCHIP_CSon(); 
        return Temp; 
}       

uint16 fwiz_write_buf(uint32 addr,uint8* buf,uint16 len)
{
        uint16 idx = 0;
     //   IINCHIP_ISR_DISABLE();
        SPI_FLASH_Write_Enable();
        SPI_Flash_ReadSR();
//        SPI_Flash_WriteSR(0x02);
//        SPI_Flash_ReadSR();
        //SPI MODE I/F
        flash_IINCHIP_CSoff();                                  // CS=0, SPI start 
        flash_IINCHIP_SpiSendData(PAGE_PROGRAM);
        flash_IINCHIP_SpiSendData((uint8)addr >> 16);           // Address byte 1
        flash_IINCHIP_SpiSendData((uint8)addr >> 8 );           // Address byte 2
        flash_IINCHIP_SpiSendData((uint8)addr  );               // Address byte 2
        for(idx = 0; idx < len; idx++)                          // Write data in loop
        {
             flash_IINCHIP_SpiSendData(buf[idx]);
        }
        flash_IINCHIP_CSon();                                         // CS=1, SPI end 
        SPI_Flash_Wait_Busy(); 
//    IINCHIP_ISR_ENABLE();                                   // Interrupt Service Routine Enable        
        return len;

}

uint16 flash_wiz_read_buf(uint32 addr, uint8* buf,uint16 len)
{
             uint16 idx = 0;

     //   IINCHIP_ISR_DISABLE();

        //SPI MODE I/F
        flash_IINCHIP_CSoff();                                        // CS=0, SPI start 
        flash_IINCHIP_SpiSendData(READ_INSTRUCTION);
        flash_IINCHIP_SpiSendData((uint8)addr >> 16);                  // Address byte 1
        flash_IINCHIP_SpiSendData((uint8)addr >> 8 );           // Address byte 2
        flash_IINCHIP_SpiSendData((uint8)addr  );               // Address byte 2
        for(idx = 0; idx < len; idx++)                          // Write data in loop
        {
             buf[idx]=flash_IINCHIP_SpiSendData(0x00);
        }
        flash_IINCHIP_CSon();                                         // CS=1, SPI end 

//    IINCHIP_ISR_ENABLE();                                   // Interrupt Service Routine Enable        
        return len;
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!  
void SPI_Flash_Erase_Chip(void)   
{                                   
    SPI_FLASH_Write_Enable();                  //SET WEL 
    SPI_Flash_Wait_Busy();   
    flash_IINCHIP_CSoff();                             //使能器件   
    flash_IINCHIP_SpiSendData(W25X_ChipErase);        //发送片擦除命令  
    flash_IINCHIP_CSon();                          //取消片选            
    SPI_Flash_Wait_Busy();          //等待芯片擦除结束
}   

void config_recover(void)
{
     struct config configread;
     configread=configbak_read();
     config_write(&configread); 
}

struct config config_read()
{   struct config *configread; 
    uint8 buf[sizeof(struct config)];
    flash_wiz_read_buf(0x00000000, buf,sizeof(struct config)); 
    configread= (struct config *)buf;
    return *configread;
}

struct config configbak_read()
{   struct config *configread;
    uint8 buf[sizeof(struct config)];
    flash_wiz_read_buf(0x00001000, buf,sizeof(struct config));  
    configread= (struct config *)buf;
    return *configread;
}

void config_write(struct config *configread)
{   uint8 *buf;
    buf=(uint8 *)configread;
    fwiz_write_buf(0x00000000,buf,sizeof(struct config)); 
}

void configbak_write(struct config *configread)
{   uint8 *buf;
    buf=(uint8 *)configread;
    fwiz_write_buf(0x00001000,buf,sizeof(struct config));
}

void SPI_Flash_Wait_Busy(void)   
{         
 while((SPI_Flash_ReadSR()&0x01)==0x01);
debug32("NO BUSY\n");   // 等待BUSY位清空
}  

void SPI_Flash_Erase_Sector(uint32 Dst_Addr)   
{  
 //监视falsh擦除情况,测试用   
//  printf("fe:%x\r\n",Dst_Addr);   
  Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  //SET WEL   
    SPI_Flash_Wait_Busy();   
    flash_IINCHIP_CSoff();                     //使能器件   
    flash_IINCHIP_SpiSendData(W25X_SectorErase);      //发送扇区擦除指令 
    flash_IINCHIP_SpiSendData((uint8)((Dst_Addr)>>16));  //发送24bit地址    
    flash_IINCHIP_SpiSendData((uint8)((Dst_Addr)>>8));   
    flash_IINCHIP_SpiSendData((uint8)Dst_Addr);  
    flash_IINCHIP_CSon();                      //取消片选            
    SPI_Flash_Wait_Busy();          //等待擦除完成
}  
