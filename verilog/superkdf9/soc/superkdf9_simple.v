//   ==================================================================
//   >>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<
//   ------------------------------------------------------------------
//   Copyright (c) 2006-2011 by Lattice Semiconductor Corporation
//   ------------------------------------------------------------------
//   ALL RIGHTS RESERVED
//
//   IMPORTANT: THIS FILE IS AUTO-GENERATED BY THE LATTICEMICO SYSTEM.
//
//   Permission:
//
//      Lattice Semiconductor grants permission to use this code
//      pursuant to the terms of the Lattice Semiconductor Corporation
//      Open Source License Agreement.
//
//   Disclaimer:
//
//      Lattice Semiconductor provides no warranty regarding the use or
//      functionality of this code. It is the user's responsibility to
//      verify the user’s design for consistency and functionality through
//      the use of formal verification methods.
//
//   --------------------------------------------------------------------
//
//                  Lattice Semiconductor Corporation
//                  5555 NE Moore Court
//                  Hillsboro, OR 97214
//                  U.S.A
//
//                  TEL: 1-800-Lattice (USA and Canada)
//                         503-286-8001 (other locations)
//
//                  web: http://www.latticesemi.com/
//                  email: techsupport@latticesemi.com
//
//   --------------------------------------------------------------------
//
//      Project:           superkdf9_simple
//      File:              superkdf9_simple.v
//      Date:              星期三, 10 四月 2013 18:45:44 CST
//      Version:           2.1
//      Targeted Family:   All
//
//   =======================================================================

`include "system_conf.v"
`include "mm_defines.vh"
`include "./arbiter2.v"
`include "../components/lm32_top/lm32_functions.v" // for clogb2_v1
`include "../components/lm32_top/lm32_include.v" // for {IROM,DRAM}_ADDR_WIDTH
`include "../components/lm32_top/lm32_include_all.v"
`include "../components/uart_core/uart_core.v"
`include "../components/spi/wb_spi.v"
`include "../components/gpio/gpio.v"
`include "../components/gpio/tpio.v"
`include "../components/sha/sha.v"
`include "../components/sha/dbl_sha.v"
`include "../components/sha/sha_core.v"

`include "../components/twi/twi_define.v"
`include "../components/twi/twi.v"
`include "../components/twi/shift.v"
`include "../components/twi/twi_core.v"

`ifdef UART_PRO_EN
`include "../components/uart_pro/async_receiver.v"
`include "../components/uart_pro/async_transmitter.v"
`include "../components/uart_pro/uart_pro_define.v"
`include "../components/uart_pro/uart_pro.v"
`include "../components/uart_pro/uart_txc.v"
`endif
//module superkdf9_simple (
module mm (
  ex_clk_i
, ex_clk_o

, INT

, uartSIN
, uartSOUT

, uart_debugSIN
, uart_debugSOUT

, w5500_spi_MISO_MASTER
, w5500_spi_MOSI_MASTER
, w5500_spi_SS_N_MASTER
, w5500_spi_SCLK_MASTER
, w5500_spi_RESET
, w5500_INT

//, LED
);

input	ex_clk_i;
output  ex_clk_o ;
wire clk_i , reset_n ;
clkgen clk (.clkin(ex_clk_i), .clkout(clk_i), .clk25m(ex_clk_o), .locked(reset_n));

wire [31:0] irom_q_rd, irom_q_wr;
wire [31:0] dram_q_rd, dram_q_wr /* unused */;
wire irom_clk_rd, irom_clk_wr;
wire irom_rst_rd, irom_rst_wr;
wire [31:0] irom_d_rd /* unused */, irom_d_wr;
wire [32-2-1:0] irom_addr_rd, irom_addr_wr;
//wire [32-2-1:0] irom_addr_rd, irom_addr_wr;
wire irom_en_rd, irom_en_wr;
wire irom_write_rd, irom_write_wr;
wire dram_clk_rd, dram_clk_wr;
wire dram_rst_rd, dram_rst_wr;
wire [31:0] dram_d_rd /* unused */, dram_d_wr;
wire [32-2-1:0] dram_addr_rd, dram_addr_wr;
//wire [32-2-1:0] dram_addr_rd, dram_addr_wr;
wire dram_en_rd, dram_en_wr;
wire dram_write_rd, dram_write_wr;

genvar i;
wire [31:0] zwire = 32'hZZZZZZZZ;
wire [31:0] zerowire = 32'h00000000;
wire [31:0] SHAREDBUS_ADR_I;
wire [31:0] SHAREDBUS_DAT_I;
wire [31:0] SHAREDBUS_DAT_O;
wire [3:0] SHAREDBUS_SEL_I;
wire   SHAREDBUS_WE_I;
wire   SHAREDBUS_ACK_O;
wire   SHAREDBUS_ERR_O;
wire   SHAREDBUS_RTY_O;
wire [2:0] SHAREDBUS_CTI_I;
wire [1:0] SHAREDBUS_BTE_I;
wire   SHAREDBUS_LOCK_I;
wire   SHAREDBUS_CYC_I;
wire   SHAREDBUS_STB_I;
wire SHAREDBUS_en;

wire [31:0] superkdf9I_ADR_O;
wire [31:0] superkdf9I_DAT_O;
wire [31:0] superkdf9I_DAT_I;
wire [3:0] superkdf9I_SEL_O;
wire   superkdf9I_WE_O;
wire   superkdf9I_ACK_I;
wire   superkdf9I_ERR_I;
wire   superkdf9I_RTY_I;
wire [2:0] superkdf9I_CTI_O;
wire [1:0] superkdf9I_BTE_O;
wire   superkdf9I_LOCK_O;
wire   superkdf9I_CYC_O;
wire   superkdf9I_STB_O;
wire [31:0] superkdf9D_ADR_O;
wire [31:0] superkdf9D_DAT_O;
wire [31:0] superkdf9D_DAT_I;
wire [3:0] superkdf9D_SEL_O;
wire   superkdf9D_WE_O;
wire   superkdf9D_ACK_I;
wire   superkdf9D_ERR_I;
wire   superkdf9D_RTY_I;
wire [2:0] superkdf9D_CTI_O;
wire [1:0] superkdf9D_BTE_O;
wire   superkdf9D_LOCK_O;
wire   superkdf9D_CYC_O;
wire   superkdf9D_STB_O;
wire [31:0] superkdf9DEBUG_DAT_O;
wire   superkdf9DEBUG_ACK_O;
wire   superkdf9DEBUG_ERR_O;
wire   superkdf9DEBUG_RTY_O;
wire superkdf9DEBUG_en;
wire [31:0] superkdf9interrupt_n;

wire [7:0] uartUART_DAT_O;
wire   uartUART_ACK_O;
wire   uartUART_ERR_O;
wire   uartUART_RTY_O;
wire uartUART_en;
wire uartINTR;

input  uartSIN;
inout  uartSOUT;

output  INT;

wire [7:0] uart_debugUART_DAT_O;
wire   uart_debugUART_ACK_O;
wire   uart_debugUART_ERR_O;
wire   uart_debugUART_RTY_O;
wire uart_debugUART_en;
wire uart_debugINTR;
input  uart_debugSIN;
output  uart_debugSOUT;

input w5500_INT;
//w5500 spi
wire [31:0] w5500_spi_SPI_DAT_O;
wire   w5500_spi_SPI_ACK_O;
wire   w5500_spi_SPI_ERR_O;
wire   w5500_spi_SPI_RTY_O;
wire w5500_spi_SPI_en;
wire w5500_spi_SPI_INT_O;
input  w5500_spi_MISO_MASTER;
output  w5500_spi_MOSI_MASTER;
output  w5500_spi_SS_N_MASTER;
output  w5500_spi_SCLK_MASTER;
output w5500_spi_RESET;

//sha core
wire [31:0] shaSHA_DAT_O;
wire   shaSHA_ACK_O;
wire   shaSHA_ERR_O;
wire   shaSHA_RTY_O;
wire   shaSHA_en;

//twi
wire [31:0] twiTWI_DAT_O;
wire        twiTWI_ACK_O;
wire        twiTWI_ERR_O;
wire        twiTWI_RTY_O;
wire        twiTWI_en;
wire        TWI_SCL_O ;
wire        TWI_SDA_OEN ;


// Enable the FT232 and HUB

reg [2:0] counter;
wire sys_reset = !counter[2];
always @(posedge clk_i or negedge reset_n)
if (reset_n == 1'b0)
counter <= #1 3'b000;
else
if (counter[2] == 1'b0)
counter <= #1 counter + 1'b1;

wire one_zero = 1'b0;
wire[1:0] two_zero = 2'b00;
wire[2:0] three_zero = 3'b000;
wire[3:0] four_zero = 4'b0000;
wire[31:0] thirtytwo_zero = 32'b0000_0000_0000_0000_0000_0000_0000_0000;

arbiter2
#(
.MAX_DAT_WIDTH ( 32 )
,.WBS_DAT_WIDTH ( 32 )
,.WBM0_DAT_WIDTH ( 32 )
,.WBM1_DAT_WIDTH ( 32 )
)
arbiter (
.WBM0_ADR_O(superkdf9I_ADR_O),
.WBM0_DAT_O(superkdf9I_DAT_O[31:0]),
.WBM0_DAT_I(superkdf9I_DAT_I),
.WBM0_SEL_O(superkdf9I_SEL_O[3:0]),
.WBM0_WE_O(superkdf9I_WE_O),
.WBM0_ACK_I(superkdf9I_ACK_I),
.WBM0_ERR_I(superkdf9I_ERR_I),
.WBM0_RTY_I(superkdf9I_RTY_I),
.WBM0_CTI_O(superkdf9I_CTI_O),
.WBM0_BTE_O(superkdf9I_BTE_O),
.WBM0_LOCK_O(superkdf9I_LOCK_O),
.WBM0_CYC_O(superkdf9I_CYC_O),
.WBM0_STB_O(superkdf9I_STB_O),
.WBM1_ADR_O(superkdf9D_ADR_O),
.WBM1_DAT_O(superkdf9D_DAT_O[31:0]),
.WBM1_DAT_I(superkdf9D_DAT_I),
.WBM1_SEL_O(superkdf9D_SEL_O[3:0]),
.WBM1_WE_O(superkdf9D_WE_O),
.WBM1_ACK_I(superkdf9D_ACK_I),
.WBM1_ERR_I(superkdf9D_ERR_I),
.WBM1_RTY_I(superkdf9D_RTY_I),
.WBM1_CTI_O(superkdf9D_CTI_O),
.WBM1_BTE_O(superkdf9D_BTE_O),
.WBM1_LOCK_O(superkdf9D_LOCK_O),
.WBM1_CYC_O(superkdf9D_CYC_O),
.WBM1_STB_O(superkdf9D_STB_O),
.WBS_ADR_I(SHAREDBUS_ADR_I[31:0]),
.WBS_DAT_I(SHAREDBUS_DAT_I[31:0]),
.WBS_DAT_O(SHAREDBUS_DAT_O[31:0]),
.WBS_SEL_I(SHAREDBUS_SEL_I[3:0]),
.WBS_WE_I(SHAREDBUS_WE_I),
.WBS_ACK_O(SHAREDBUS_ACK_O),
.WBS_ERR_O(SHAREDBUS_ERR_O),
.WBS_RTY_O(SHAREDBUS_RTY_O),
.WBS_CTI_I(SHAREDBUS_CTI_I),
.WBS_BTE_I(SHAREDBUS_BTE_I),
.WBS_LOCK_I(SHAREDBUS_LOCK_I),
.WBS_CYC_I(SHAREDBUS_CYC_I),
.WBS_STB_I(SHAREDBUS_STB_I),
.clk (clk_i),
.reset (sys_reset));

assign SHAREDBUS_DAT_O =
uartUART_en ? {4{uartUART_DAT_O[7:0]}} :
uart_debugUART_en ? {4{uart_debugUART_DAT_O[7:0]}} :
shaSHA_en ? shaSHA_DAT_O :
w5500_spi_SPI_en ? w5500_spi_SPI_DAT_O :
twiTWI_en ? twiTWI_DAT_O :
0;

assign SHAREDBUS_ERR_O = SHAREDBUS_CYC_I & !(
(!uartUART_ERR_O & uartUART_en) |
(!uart_debugUART_ERR_O & uart_debugUART_en) |
(!shaSHA_ERR_O & shaSHA_en ) |
(!w5500_spi_SPI_ERR_O & w5500_spi_SPI_en) |
(!twiTWI_ERR_O & twiTWI_en ) |
0);

assign SHAREDBUS_ACK_O =
uartUART_en ? uartUART_ACK_O :
uart_debugUART_en ? uart_debugUART_ACK_O :
shaSHA_en ? shaSHA_ACK_O :
w5500_spi_SPI_en ? w5500_spi_SPI_ACK_O :
twiTWI_en ? twiTWI_ACK_O :
0;

assign SHAREDBUS_RTY_O =
uartUART_en ? uartUART_RTY_O :
uart_debugUART_en ? uart_debugUART_RTY_O :
shaSHA_en ? shaSHA_RTY_O :
w5500_spi_SPI_en ? w5500_spi_SPI_RTY_O :
twiTWI_en ? twiTWI_RTY_O :
0;

wire [31:0] superkdf9DEBUG_DAT_I;
assign superkdf9DEBUG_DAT_I = 0;
wire [3:0] superkdf9DEBUG_SEL_I;
assign superkdf9DEBUG_SEL_I = 0;
assign superkdf9DEBUG_en = 0;
lm32_top
 superkdf9(
.I_ADR_O(superkdf9I_ADR_O),
.I_DAT_O(superkdf9I_DAT_O),
.I_DAT_I(superkdf9I_DAT_I),
.I_SEL_O(superkdf9I_SEL_O),
.I_WE_O(superkdf9I_WE_O),
.I_ACK_I(superkdf9I_ACK_I),
.I_ERR_I(superkdf9I_ERR_I),
.I_RTY_I(superkdf9I_RTY_I),
.I_CTI_O(superkdf9I_CTI_O),
.I_BTE_O(superkdf9I_BTE_O),
.I_LOCK_O(superkdf9I_LOCK_O),
.I_CYC_O(superkdf9I_CYC_O),
.I_STB_O(superkdf9I_STB_O),
.D_ADR_O(superkdf9D_ADR_O),
.D_DAT_O(superkdf9D_DAT_O),
.D_DAT_I(superkdf9D_DAT_I),
.D_SEL_O(superkdf9D_SEL_O),
.D_WE_O(superkdf9D_WE_O),
.D_ACK_I(superkdf9D_ACK_I),
.D_ERR_I(superkdf9D_ERR_I),
.D_RTY_I(superkdf9D_RTY_I),
.D_CTI_O(superkdf9D_CTI_O),
.D_BTE_O(superkdf9D_BTE_O),
.D_LOCK_O(superkdf9D_LOCK_O),
.D_CYC_O(superkdf9D_CYC_O),
.D_STB_O(superkdf9D_STB_O),
.DEBUG_ADR_I(SHAREDBUS_ADR_I[31:0]),
.DEBUG_DAT_I(superkdf9DEBUG_DAT_I[31:0]),
.DEBUG_DAT_O(superkdf9DEBUG_DAT_O[31:0]),
.DEBUG_SEL_I(superkdf9DEBUG_SEL_I[3:0]),
.DEBUG_WE_I(SHAREDBUS_WE_I),
.DEBUG_ACK_O(superkdf9DEBUG_ACK_O),
.DEBUG_ERR_O(superkdf9DEBUG_ERR_O),
.DEBUG_RTY_O(superkdf9DEBUG_RTY_O),
.DEBUG_CTI_I(SHAREDBUS_CTI_I),
.DEBUG_BTE_I(SHAREDBUS_BTE_I),
.DEBUG_LOCK_I(SHAREDBUS_LOCK_I),
.DEBUG_CYC_I(SHAREDBUS_CYC_I & superkdf9DEBUG_en),
.DEBUG_STB_I(SHAREDBUS_STB_I & superkdf9DEBUG_en),
.interrupt_n(superkdf9interrupt_n),
.clk_i (clk_i), .rst_i (sys_reset),
// the exposed IROM
.irom_clk_rd(irom_clk_rd),
.irom_clk_wr(irom_clk_wr),
.irom_rst_rd(irom_rst_rd),
.irom_rst_wr(irom_rst_wr),
.irom_d_rd(irom_d_rd) /* unused */,
.irom_d_wr(irom_d_wr),
.irom_q_rd(irom_q_rd),
.irom_q_wr(irom_q_wr),
.irom_addr_rd(irom_addr_rd[`IROM_ADDR_WIDTH-1:0]),
.irom_addr_wr(irom_addr_wr[`IROM_ADDR_WIDTH-1:0]),
.irom_en_rd(irom_en_rd),
.irom_en_wr(irom_en_wr),
.irom_write_rd(irom_write_rd),
.irom_write_wr(irom_write_wr),
// the exposed DRAM
.dram_clk_rd(dram_clk_rd),
.dram_clk_wr(dram_clk_wr),
.dram_rst_rd(dram_rst_rd),
.dram_rst_wr(dram_rst_wr),
.dram_d_rd(dram_d_rd) /* unused */,
.dram_d_wr(dram_d_wr),
.dram_q_rd(dram_q_rd),
.dram_q_wr(dram_q_wr) /* unused */,
.dram_addr_rd(dram_addr_rd[`DRAM_ADDR_WIDTH-1:0]),
.dram_addr_wr(dram_addr_wr[`DRAM_ADDR_WIDTH-1:0]),
.dram_en_rd(dram_en_rd),
.dram_en_wr(dram_en_wr),
.dram_write_rd(dram_write_rd),
.dram_write_wr(dram_write_wr)
);
// VIO/ILA and ICON {{{
wire [35:0] icon_ctrl_0, icon_ctrl_1;
wire [255:0] trig0 = {
	8'ha                 ,//[15:8]
	irom_addr_rd[29:0]   ,
	irom_q_rd[31:0]      ,
	superkdf9interrupt_n[7:0]
} ;
icon icon_test(.CONTROL0(icon_ctrl_0));
ila ila_test(.CONTROL(icon_ctrl_0), .CLK(clk_i), .TRIG0(trig0)
);
//vio vio_test(.CONTROL(icon_ctrl_1), .ASYNC_OUT(intr_i));
// }}}

// IROM {{{
bram #(
	.size(1+`CFG_IROM_LIMIT - `CFG_IROM_BASE_ADDRESS),
	.name("irom")
) irom (
	.ClockA(irom_clk_rd),
	.ClockB(irom_clk_wr),
	.ResetA(irom_rst_rd),
	.ResetB(irom_rst_wr),
	.AddressA(irom_addr_rd),
	.AddressB(irom_addr_wr),
	.DataInA(irom_d_rd), /* unused */
	.DataInB(irom_d_wr),
	.DataOutA(irom_q_rd),
	.DataOutB(irom_q_wr), /* unused */
	.ClockEnA(irom_en_rd),
	.ClockEnB(irom_en_wr),
	.WriteA(irom_write_rd),
	.WriteB(irom_write_wr)
);
// }}}
// DRAM {{{
bram #(
	.size(1+`CFG_DRAM_LIMIT - `CFG_DRAM_BASE_ADDRESS),
	.name("irom")
) dram (
	.ClockA(dram_clk_rd),
	.ClockB(dram_clk_wr),
	.ResetA(dram_rst_rd),
	.ResetB(dram_rst_wr),
	.AddressA(dram_addr_rd),
	.AddressB(dram_addr_wr),
	.DataInA(dram_d_rd), /* unused */
	.DataInB(dram_d_wr),
	.DataOutA(dram_q_rd),
	.DataOutB(dram_q_wr),
	.ClockEnA(dram_en_rd),
	.ClockEnB(dram_en_wr),
	.WriteA(dram_write_rd),
	.WriteB(dram_write_wr)
);
// }}}


wire [7:0] uartUART_DAT_I;
assign uartUART_DAT_I = ((
	SHAREDBUS_ADR_I[1:0] == 2'b00) ? SHAREDBUS_DAT_I[31:24] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b01) ? SHAREDBUS_DAT_I[23:16] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b10) ? SHAREDBUS_DAT_I[15:8] : SHAREDBUS_DAT_I[7:0])));
wire uartUART_SEL_I;
assign uartUART_SEL_I = ((
	SHAREDBUS_ADR_I[1:0] == 2'b00) ? SHAREDBUS_SEL_I[3] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b01) ? SHAREDBUS_SEL_I[2] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b10) ? SHAREDBUS_SEL_I[1] : SHAREDBUS_SEL_I[0])));
assign uartUART_en = (SHAREDBUS_ADR_I[31:4] == 28'b1000000000000000000000010000);//Device address 0x80000100
wire uartSOUT_w ;
assign uartSOUT = uartSOUT_w ? 1'bz : 1'b0;

`ifndef UART_PRO_EN
uart_core
#(
.UART_WB_DAT_WIDTH(8),
.UART_WB_ADR_WIDTH(4),
.CLK_IN_MHZ(`MM_CLK_IN_MHZ),
.BAUD_RATE(115200),
.STDOUT_SIM(0),
.STDOUT_SIMFAST(0),
.LCR_DATA_BITS(8),
.LCR_STOP_BITS(1),
.LCR_PARITY_ENABLE(0),
.LCR_PARITY_ODD(0),
.LCR_PARITY_STICK(0),
.LCR_SET_BREAK(0),
.FIFO(1))
 uart(
.UART_ADR_I(SHAREDBUS_ADR_I[3:0]),
.UART_DAT_I(uartUART_DAT_I[7:0]),
.UART_DAT_O(uartUART_DAT_O[7:0]),
.UART_SEL_I(uartUART_SEL_I),
.UART_WE_I(SHAREDBUS_WE_I),
.UART_ACK_O(uartUART_ACK_O),
.UART_ERR_O(uartUART_ERR_O),
.UART_RTY_O(uartUART_RTY_O),
.UART_CTI_I(SHAREDBUS_CTI_I),
.UART_BTE_I(SHAREDBUS_BTE_I),
.UART_LOCK_I(SHAREDBUS_LOCK_I),
.UART_CYC_I(SHAREDBUS_CYC_I & uartUART_en),
.UART_STB_I(SHAREDBUS_STB_I & uartUART_en),
.SIN(uartSIN),
.SOUT(uartSOUT_w),
.RXRDY_N(uartRXRDY_N),
.TXRDY_N(uartTXRDY_N),
.INTR(uartINTR),
.CLK(clk_i), .RESET(sys_reset));
`else
uart_pro U_uart_pro(
	// system clock and reset
/*input         */.CLK_I      (clk_i                        ) ,
/*input         */.RST_I      (sys_reset                    ) ,

// wishbone interface signals
/*input         */.PRO_CYC_I  (SHAREDBUS_CYC_I & uartUART_en) ,//NC
/*input         */.PRO_STB_I  (SHAREDBUS_STB_I & uartUART_en) ,
/*input         */.PRO_WE_I   (SHAREDBUS_WE_I               ) ,
/*input         */.PRO_LOCK_I (SHAREDBUS_LOCK_I             ) ,//NC
/*input  [2:0]  */.PRO_CTI_I  (SHAREDBUS_CTI_I              ) ,//NC
/*input  [1:0]  */.PRO_BTE_I  (SHAREDBUS_BTE_I              ) ,//NC
/*input  [5:0]  */.PRO_ADR_I  (SHAREDBUS_ADR_I[3:0]         ) ,
/*input  [31:0] */.PRO_DAT_I  (uartUART_DAT_I[7:0]          ) ,
/*input  [3:0]  */.PRO_SEL_I  (uartUART_SEL_I               ) ,
/*output reg    */.PRO_ACK_O  (uartUART_ACK_O               ) ,
/*output        */.PRO_ERR_O  (uartUART_ERR_O               ) ,//const 0
/*output        */.PRO_RTY_O  (uartUART_RTY_O               ) ,//const 0
/*output [31:0] */.PRO_DAT_O  (uartUART_DAT_O[7:0]          ) ,

//Uart Pro interface
/*input         */.PRO_RX     (uartSIN                      ) ,
/*output        */.PRO_TX     (uartSOUT_w                   ) ,
/*output        */.PRO_INT    (uartINTR                     )
);
`endif

//assign gpioGPIO_en = (SHAREDBUS_ADR_I[31:4] == 28'b1000000000000000000000100000);//Device address 0x80000200

wire [7:0] uart_debugUART_DAT_I;
assign uart_debugUART_DAT_I = ((
	SHAREDBUS_ADR_I[1:0] == 2'b00) ? SHAREDBUS_DAT_I[31:24] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b01) ? SHAREDBUS_DAT_I[23:16] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b10) ? SHAREDBUS_DAT_I[15:8] : SHAREDBUS_DAT_I[7:0])));
wire uart_debugUART_SEL_I;
assign uart_debugUART_SEL_I = ((
	SHAREDBUS_ADR_I[1:0] == 2'b00) ? SHAREDBUS_SEL_I[3] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b01) ? SHAREDBUS_SEL_I[2] : ((
	SHAREDBUS_ADR_I[1:0] == 2'b10) ? SHAREDBUS_SEL_I[1] : SHAREDBUS_SEL_I[0])));
assign uart_debugUART_en = (SHAREDBUS_ADR_I[31:4] == 28'b1000000000000000000000110000);//Device address 0x80000300
wire uart_debugSOUT_w ;
assign uart_debugSOUT     = uart_debugSOUT_w ? 1'bz : 1'b0;

uart_core
#(
.UART_WB_DAT_WIDTH(8),
.UART_WB_ADR_WIDTH(4),
.CLK_IN_MHZ(`MM_CLK_IN_MHZ),
.BAUD_RATE(115200),
.STDOUT_SIM(0),
.STDOUT_SIMFAST(0),
.LCR_DATA_BITS(8),
.LCR_STOP_BITS(1),
.LCR_PARITY_ENABLE(0),
.LCR_PARITY_ODD(0),
.LCR_PARITY_STICK(0),
.LCR_SET_BREAK(0),
.FIFO(1))
 uart_debug(
.UART_ADR_I(SHAREDBUS_ADR_I[3:0]),
.UART_DAT_I(uart_debugUART_DAT_I[7:0]),
.UART_DAT_O(uart_debugUART_DAT_O[7:0]),
.UART_SEL_I(uart_debugUART_SEL_I),
.UART_WE_I(SHAREDBUS_WE_I),
.UART_ACK_O(uart_debugUART_ACK_O),
.UART_ERR_O(uart_debugUART_ERR_O),
.UART_RTY_O(uart_debugUART_RTY_O),
.UART_CTI_I(SHAREDBUS_CTI_I),
.UART_BTE_I(SHAREDBUS_BTE_I),
.UART_LOCK_I(SHAREDBUS_LOCK_I),
.UART_CYC_I(SHAREDBUS_CYC_I & uart_debugUART_en),
.UART_STB_I(SHAREDBUS_STB_I & uart_debugUART_en),
.SIN(uart_debugSIN),
.SOUT(uart_debugSOUT_w),
.INTR(uart_debugINTR),
.CLK(clk_i), .RESET(sys_reset));

wire [31:0] w5500_spi_SPI_DAT_I;
wire w5500_spi_fake_cs;
assign w5500_spi_SPI_DAT_I = SHAREDBUS_DAT_I[31:0];
wire [3:0] w5500_spi_SPI_SEL_I;
assign w5500_spi_SPI_SEL_I = SHAREDBUS_SEL_I;
assign w5500_spi_SPI_en = (SHAREDBUS_ADR_I[31:8] == 24'b100000000000000000000000);//Device address 0x80000000
spi
#(
.MASTER(1),
.SLAVE_NUMBER(32'h1),
.CLOCK_SEL(15),
.CLKCNT_WIDTH(4),
.INTERVAL_LENGTH(2),
.DATA_LENGTH(8),
.SHIFT_DIRECTION(0),
.CLOCK_PHASE(0),
.CLOCK_POLARITY(0))
 w5500_spi(
.SPI_ADR_I(SHAREDBUS_ADR_I[31:0]),
.SPI_DAT_I(w5500_spi_SPI_DAT_I[31:0]),
.SPI_DAT_O(w5500_spi_SPI_DAT_O[31:0]),
.SPI_SEL_I(w5500_spi_SPI_SEL_I[3:0]),
.SPI_WE_I(SHAREDBUS_WE_I),
.SPI_ACK_O(w5500_spi_SPI_ACK_O),
.SPI_ERR_O(w5500_spi_SPI_ERR_O),
.SPI_RTY_O(w5500_spi_SPI_RTY_O),
.SPI_CTI_I(SHAREDBUS_CTI_I),
.SPI_BTE_I(SHAREDBUS_BTE_I),
.SPI_LOCK_I(SHAREDBUS_LOCK_I),
.SPI_CYC_I(SHAREDBUS_CYC_I & w5500_spi_SPI_en),
.SPI_STB_I(SHAREDBUS_STB_I & w5500_spi_SPI_en),
.MISO_MASTER(w5500_spi_MISO_MASTER),
.MOSI_MASTER(w5500_spi_MOSI_MASTER),
.SS_N_MASTER(w5500_spi_fake_cs),
.SCLK_MASTER(w5500_spi_SCLK_MASTER),
.SPI_INT_O(w5500_spi_SPI_INT_O),
.CLK_I(clk_i), .RST_I(sys_reset));


assign shaSHA_en = (SHAREDBUS_ADR_I[31:5] == 28'b100000000000000000000100000);//Device address 0x80000400
sha sha256(
// system clock and reset
/*input        */ .CLK_I     (clk_i),
/*input        */ .RST_I     (sys_reset),

// wishbone interface signals
/*input        */ .SHA_CYC_I (SHAREDBUS_CYC_I & shaSHA_en ) ,//NC
/*input        */ .SHA_STB_I (SHAREDBUS_STB_I & shaSHA_en ) ,
/*input        */ .SHA_WE_I  (SHAREDBUS_WE_I              ) ,
/*input        */ .SHA_LOCK_I(SHAREDBUS_LOCK_I            ) ,//NC
/*input [2:0]  */ .SHA_CTI_I (SHAREDBUS_CTI_I             ) ,//NC
/*input [1:0]  */ .SHA_BTE_I (SHAREDBUS_BTE_I             ) ,//NC
/*input [4:0]  */ .SHA_ADR_I (SHAREDBUS_ADR_I[4:0]        ) ,
/*input [31:0] */ .SHA_DAT_I (SHAREDBUS_DAT_I[31:0]       ) ,
/*input [3:0]  */ .SHA_SEL_I (SHAREDBUS_SEL_I             ) ,
/*output reg   */ .SHA_ACK_O (shaSHA_ACK_O                ) ,
/*output       */ .SHA_ERR_O (shaSHA_ERR_O                ) ,//const 0
/*output       */ .SHA_RTY_O (shaSHA_RTY_O                ) ,//const 0
/*output [31:0]*/ .SHA_DAT_O (shaSHA_DAT_O                )
);

assign twiTWI_en = (SHAREDBUS_ADR_I[31:6] == 26'b10000000000000000000011000);//Device address 0x80000600
assign TWI_SCL = TWI_SCL_O == 1'b0 ? 1'b0 : 1'bz ;
assign TWI_SDA = TWI_SDA_OEN == 1'b0 ? 1'b0 : 1'bz ;
wire PWM_w;
wire [7:0] gpioGPIO_IN_w;
wire [23:0] gpioGPIO_OUT_w;
assign w5500_spi_RESET = gpioGPIO_OUT_w[0];
assign w5500_spi_SS_N_MASTER = gpioGPIO_OUT_w[1];

twi u_twi(
// system clock and reset
/*input         */ .CLK_I       (clk_i                       ) ,
/*input         */ .RST_I       (sys_reset                   ) ,

// wishbone interface signals
/*input         */ .TWI_CYC_I   (SHAREDBUS_CYC_I & twiTWI_en ) ,//NC
/*input         */ .TWI_STB_I   (SHAREDBUS_STB_I & twiTWI_en ) ,
/*input         */ .TWI_WE_I    (SHAREDBUS_WE_I              ) ,
/*input         */ .TWI_LOCK_I  (SHAREDBUS_LOCK_I            ) ,//NC
/*input  [2:0]  */ .TWI_CTI_I   (SHAREDBUS_CTI_I             ) ,//NC
/*input  [1:0]  */ .TWI_BTE_I   (SHAREDBUS_BTE_I             ) ,//NC
/*input  [5:0]  */ .TWI_ADR_I   (SHAREDBUS_ADR_I[5:0]        ) ,
/*input  [31:0] */ .TWI_DAT_I   (SHAREDBUS_DAT_I[31:0]       ) ,
/*input  [3:0]  */ .TWI_SEL_I   (SHAREDBUS_SEL_I             ) ,
/*output reg    */ .TWI_ACK_O   (twiTWI_ACK_O                ) ,
/*output        */ .TWI_ERR_O   (twiTWI_ERR_O                ) ,//const 0
/*output        */ .TWI_RTY_O   (twiTWI_RTY_O                ) ,//const 0
/*output [31:0] */ .TWI_DAT_O   (twiTWI_DAT_O                ) ,

/*output        */ .TWI_SCL_O   (TWI_SCL_O                   ) ,
/*input         */ .TWI_SDA_I   (TWI_SDA                     ) ,
/*output        */ .TWI_SDA_OEN (TWI_SDA_OEN                 ) ,
/*output        */ .PWM	        (PWM_w                       ) ,
/*output        */ .WATCH_DOG   (WATCH_DOG                   ) ,

/*output        */ .SFT_SHCP    (SFT_SHCP                    ) ,
/*output        */ .SFT_DS      (SFT_DS                      ) ,
/*output        */ .SFT_STCP    (SFT_STCP                    ) ,
/*output        */ .SFT_MR_N    (SFT_MR_N                    ) ,
/*output        */ .SFT_OE_N    (SFT_OE_N                    ) ,

/*input         */ .FAN_IN0     (FAN_IN0                     ) ,
/*input         */ .FAN_IN1     (FAN_IN1                     ) ,
/*output        */ .TIME0_INT   (TIME0_INT                   ) ,
/*output        */ .TIME1_INT   (TIME1_INT                   ) ,
/*output [23:0] */ .GPIO_OUT    (gpioGPIO_OUT_w               ) ,
/*input  [7:0]  */ .GPIO_IN     (gpioGPIO_IN_w               )
) ;

assign superkdf9interrupt_n[3] = !uartINTR ;
assign superkdf9interrupt_n[1] = 1 ;
assign superkdf9interrupt_n[0] = 1 ;
assign superkdf9interrupt_n[4] = !uart_debugINTR ;
assign superkdf9interrupt_n[2] = 1;
assign superkdf9interrupt_n[5] = 1;
assign superkdf9interrupt_n[6] = 1;
assign superkdf9interrupt_n[7] = 1;
assign superkdf9interrupt_n[8] = 1;
assign superkdf9interrupt_n[9] = 1;
assign superkdf9interrupt_n[10] = 1;
assign superkdf9interrupt_n[11] = 1;
assign superkdf9interrupt_n[12] = 1;
assign superkdf9interrupt_n[13] = 1;
assign superkdf9interrupt_n[14] = 1;
assign superkdf9interrupt_n[15] = 1;
assign superkdf9interrupt_n[16] = 1;
assign superkdf9interrupt_n[17] = 1;
assign superkdf9interrupt_n[18] = 1;
assign superkdf9interrupt_n[19] = 1;
assign superkdf9interrupt_n[20] = 1;
assign superkdf9interrupt_n[21] = 1;
assign superkdf9interrupt_n[22] = 1;
assign superkdf9interrupt_n[23] = 1;
assign superkdf9interrupt_n[24] = 1;
assign superkdf9interrupt_n[25] = 1;
assign superkdf9interrupt_n[26] = 1;
assign superkdf9interrupt_n[27] = 1;
assign superkdf9interrupt_n[28] = 1;
assign superkdf9interrupt_n[29] = 1;
assign superkdf9interrupt_n[30] = 1;
assign superkdf9interrupt_n[31] = 1;
//assign INT = ~(&superkdf9interrupt_n) ;
assign INT = 1;
endmodule
