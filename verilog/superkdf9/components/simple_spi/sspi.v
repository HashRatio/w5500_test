`ifndef WB_SPI_FILE
`define WB_SPI_FILE
`include "system_conf.v"
module sspi #(
   parameter  SHIFT_DIRECTION = 0,
   parameter  CLOCK_PHASE     = 0,
   parameter  CLOCK_POLARITY  = 0,
   parameter  CLOCK_SEL       = 7,
   parameter  MASTER          = 1,
   parameter  SLAVE_NUMBER    = 1,
   parameter  DATA_LENGTH     = 32,
   parameter  DELAY_TIME      = 2,
   parameter  CLKCNT_WIDTH    = 5,
   parameter  INTERVAL_LENGTH = 2)
   (
   //slave port
   SSPI_ADR_I,    //32bits
   SSPI_DAT_I,    //32bits
   SSPI_WE_I,
   SSPI_CYC_I,
   SSPI_STB_I,
   SSPI_SEL_I,
   SSPI_CTI_I,
   SSPI_BTE_I,
   SSPI_LOCK_I,
   SSPI_DAT_O,    //32bits
   SSPI_ACK_O,
   SSPI_INT_O,
   SSPI_ERR_O,
   SSPI_RTY_O,
   //spi interface
   MISO_MASTER,
   MOSI_MASTER,
   SS_N_MASTER,
   SCLK_MASTER,
   MISO_SLAVE,
   MOSI_SLAVE,
   SS_N_SLAVE,
   SCLK_SLAVE,
   //system clock and reset
   CLK_I,
   RST_I
   );
   
endmodule
`endif