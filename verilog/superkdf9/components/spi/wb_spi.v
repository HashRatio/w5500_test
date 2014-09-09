//   ==================================================================
//   >>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<
//   ------------------------------------------------------------------
//   Copyright (c) 2006-2011 by Lattice Semiconductor Corporation
//   ALL RIGHTS RESERVED 
//   ------------------------------------------------------------------
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
//      verify the user�s design for consistency and functionality through
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
//                         FILE DETAILS
// Project          : SPI Controller
// File             : wb_spi.v
// Title            : Hardware debug registers and associated logic.
// Dependencies     : None
// Version          : 6.1.17
//                  : Initial Release
// Version          : 7.0SP2, 3.0
//                  : No Change
// Version          : 3.1
//                  : Rewrite the whole code for correct functionality
// =============================================================================
`ifndef WB_SPI_FILE
`define WB_SPI_FILE
`include "system_conf.v"
module spi #(
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
   SPI_ADR_I,    //32bits
   SPI_DAT_I,    //32bits
   SPI_WE_I,
   SPI_CYC_I,
   SPI_STB_I,
   SPI_SEL_I,
   SPI_CTI_I,
   SPI_BTE_I,
   SPI_LOCK_I,
   SPI_DAT_O,    //32bits
   SPI_ACK_O,
   SPI_INT_O,
   SPI_ERR_O,
   SPI_RTY_O,
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
   
   //slave port
   input [31:0]        SPI_ADR_I;    
   input [31:0]        SPI_DAT_I;    
   input               SPI_WE_I;
   input               SPI_STB_I;
   input               SPI_CYC_I;
   input [3:0]         SPI_SEL_I;
   input [2:0]         SPI_CTI_I;
   input [1:0]         SPI_BTE_I;
   input               SPI_LOCK_I;
   output [31:0]       SPI_DAT_O;    
   output              SPI_ACK_O;
   output              SPI_INT_O;
   output              SPI_ERR_O;
   output              SPI_RTY_O;

   //spi interface
   input               MISO_MASTER;
   output              MOSI_MASTER;
   output [SLAVE_NUMBER-1:0] SS_N_MASTER;
   output              SCLK_MASTER;
   output              MISO_SLAVE;
   input               MOSI_SLAVE;
   input               SS_N_SLAVE;
   input               SCLK_SLAVE;
   //system clock and reset
   input               CLK_I;
   input               RST_I;

   parameter           UDLY          = 1;
   parameter           ST_IDLE       = 3'b000;
   parameter           ST_LOAD       = 3'b001;
   parameter           ST_WAIT       = 3'b010;
   parameter           ST_TRANS      = 3'b011;
   parameter           ST_TURNAROUND = 3'b100;
   parameter           ST_INTERVAL   = 3'b101;
   
   //register access
   wire                    SPI_INT_O;
   reg                     SPI_ACK_O;
   reg  [31:0]             SPI_DAT_O;
   reg [SLAVE_NUMBER-1:0]  SS_N_MASTER;
   reg                     SCLK_MASTER;
   reg                     MOSI_MASTER;
   reg                     MISO_SLAVE;
   
   reg                     dw00_cs;
   reg                     dw04_cs;
   reg                     dw08_cs;
   reg                     dw0c_cs;
   reg                     dw10_cs;
   reg                     reg_wr;
   reg                     reg_rd;
   reg                     read_wait_done;
   reg [31:0]              latch_s_data;
   reg [DATA_LENGTH-1:0]   reg_rxdata;
   reg [DATA_LENGTH-1:0]   reg_txdata;
   reg [DATA_LENGTH-1:0]   rx_shift_data;
   reg [DATA_LENGTH-1:0]   tx_shift_data;
   reg                     rx_latch_flag;
   
   wire [10:0]             reg_control;  
   reg                     reg_iroe;
   reg                     reg_itoe;
   reg                     reg_itrdy;
   reg                     reg_irrdy;
   reg                     reg_ie;
   reg                     reg_sso;
   
   wire [8:0]              reg_status;
   reg                     reg_toe;
   reg                     reg_roe;
   reg                     reg_trdy;
   reg                     reg_rrdy;
   reg                     reg_tmt;
   wire                    reg_e; 
     
   assign SPI_ERR_O = 1'b0;
   assign SPI_RTY_O = 1'b0;

   always @(posedge CLK_I or posedge RST_I)
     if(RST_I) begin   
        dw00_cs  <= #UDLY 1'b0;
        dw04_cs  <= #UDLY 1'b0;
        dw08_cs  <= #UDLY 1'b0;
        dw0c_cs  <= #UDLY 1'b0;
        dw10_cs  <= #UDLY 1'b0;
     end else begin    
        dw00_cs  <= #UDLY (SPI_ADR_I[7:0] == 8'h00);
        dw04_cs  <= #UDLY (SPI_ADR_I[7:0] == 8'h04);
        dw08_cs  <= #UDLY (SPI_ADR_I[7:0] == 8'h08);
        dw0c_cs  <= #UDLY (SPI_ADR_I[7:0] == 8'h0C);
        dw10_cs  <= #UDLY (SPI_ADR_I[7:0] == 8'h10);
     end

   always @(posedge CLK_I or posedge RST_I)
     if(RST_I) begin
        reg_wr       <= #UDLY 1'b0;
        reg_rd       <= #UDLY 1'b0;
     end else begin
        reg_wr       <= #UDLY  SPI_WE_I && SPI_STB_I && SPI_CYC_I;
        reg_rd       <= #UDLY !SPI_WE_I && SPI_STB_I && SPI_CYC_I;
     end
     
   always @(posedge CLK_I or posedge RST_I)
     if(RST_I)
       latch_s_data  <= #UDLY 32'h0;
     else
       latch_s_data  <= #UDLY SPI_DAT_I;

//Receive Data Register(0x00)
   always @(posedge CLK_I or posedge RST_I)
     if(RST_I)
       reg_rxdata            <= #UDLY 'h0;
     else if (rx_latch_flag)
       reg_rxdata            <= #UDLY rx_shift_data;
       
//Transmit Data Register(0x04)
   always @(posedge CLK_I or posedge RST_I)
     if(RST_I)
       reg_txdata               <= #UDLY 'h0;
     else if (reg_wr && dw04_cs && reg_trdy)
       reg_txdata               <= #UDLY latch_s_data;

//Status Register(0x08)
   assign      reg_e       = reg_toe | reg_roe;
   assign      reg_status  = {reg_e,reg_rrdy,reg_trdy,reg_tmt,reg_toe,reg_roe,3'b000};
   
//Control Register(0x0C)
   always @(posedge CLK_I or posedge RST_I)
    if (RST_I) begin
      reg_iroe    <= #UDLY 1'b0;
      reg_itoe    <= #UDLY 1'b0;
      reg_itrdy   <= #UDLY 1'b0;
      reg_irrdy   <= #UDLY 1'b0;
      reg_ie      <= #UDLY 1'b0;
      reg_sso     <= #UDLY 1'b0; end 
    else if(reg_wr && dw0c_cs) begin
      reg_iroe    <= #UDLY latch_s_data[3];
      reg_itoe    <= #UDLY latch_s_data[4];
      reg_itrdy   <= #UDLY latch_s_data[6];
      reg_irrdy   <= #UDLY latch_s_data[7];
      reg_ie      <= #UDLY latch_s_data[8];
      reg_sso     <= #UDLY latch_s_data[10];  end
     
   assign  SPI_INT_O = (reg_ie & (reg_iroe & reg_roe | reg_itoe & reg_toe)) | 
                       (reg_itrdy & reg_trdy) |
                       (reg_irrdy & reg_rrdy);
   
   //For Write, ACK is asserted right after STB is sampled
   //For Read, ACK is asserted one clock later after STB is sampled for better timing
   always @(posedge CLK_I or posedge RST_I) begin
    if (RST_I)
      SPI_ACK_O <= 1'b0;
    else if (SPI_ACK_O)
      SPI_ACK_O <= 1'b0;
    else if (SPI_STB_I && SPI_CYC_I && (SPI_WE_I || read_wait_done)) 
      SPI_ACK_O <= 1'b1;
    end

   always @(posedge CLK_I or posedge RST_I) begin
    if (RST_I)
      read_wait_done <= 1'b0;
    else if (SPI_ACK_O)
      read_wait_done <= 1'b0;
    else if (SPI_STB_I && SPI_CYC_I && ~SPI_WE_I)
      read_wait_done <= 1'b1;
    end

generate
if (MASTER == 1) begin
   //Master Only registers
   reg [SLAVE_NUMBER-1:0]  reg_ssmask;
   reg [CLKCNT_WIDTH-1:0]  clock_cnt;
   reg  [5:0]              data_cnt;
   reg                     pending_data;
   reg [2:0]               c_status;
   reg [2:0]               n_status;
   
   assign  reg_control     = {reg_sso,1'b0,reg_ie,reg_irrdy,reg_itrdy,1'b0,reg_itoe,reg_iroe,3'h0};
   
   always @(posedge CLK_I or posedge RST_I)
     if(RST_I)
       pending_data               <= #UDLY 1'b0;
     else if (reg_wr && dw04_cs)
       pending_data               <= #UDLY 1'b1;
     else if (c_status == ST_LOAD)
       pending_data               <= #UDLY 1'b0;     

   always @(posedge CLK_I or posedge RST_I)
     if (RST_I)
       SPI_DAT_O     <= #UDLY 32'h0;
     else if (reg_rd)
       SPI_DAT_O     <= #UDLY  dw00_cs ? reg_rxdata : 
                               dw04_cs ? reg_txdata : 
                               //dw08_cs ? 9'b101010101 :
							   dw08_cs ? reg_status:							   
                               dw0c_cs ? reg_control: 
                               dw10_cs ? reg_ssmask :
                                         32'h0;

   always @(posedge CLK_I or posedge RST_I)
     if (RST_I)
       reg_ssmask                      <= #UDLY 'h0;
     else if (reg_wr && dw10_cs)
       reg_ssmask                      <= #UDLY latch_s_data;

   always @(posedge CLK_I or posedge RST_I) begin
     if (RST_I)
        SS_N_MASTER <= {SLAVE_NUMBER{1'b1}};
     else if (reg_sso)
        SS_N_MASTER <= (~reg_ssmask);
     else if ((c_status == ST_TRANS) || (c_status == ST_WAIT) || (c_status == ST_TURNAROUND))
        SS_N_MASTER <= (~reg_ssmask);
     else 
        SS_N_MASTER <= {SLAVE_NUMBER{1'b1}};
     end
     
   // Generate SCLK 
   always @(posedge CLK_I or posedge RST_I)
     if (RST_I) 
        SCLK_MASTER    <= #UDLY CLOCK_POLARITY;
     else if ((c_status == ST_TRANS)  && (clock_cnt == CLOCK_SEL))
        SCLK_MASTER    <= #UDLY ~SCLK_MASTER;
     
   always @(posedge CLK_I or posedge RST_I)
     if (RST_I)
        rx_shift_data       <= #UDLY 'h0;
     else if ((clock_cnt == CLOCK_SEL) && (CLOCK_PHASE == SCLK_MASTER) && (c_status == ST_TRANS))  begin
        if (SHIFT_DIRECTION)
           rx_shift_data    <= #UDLY {MISO_MASTER,rx_shift_data[DATA_LENGTH-1:1]};
        else
           rx_shift_data    <= #UDLY {rx_shift_data,MISO_MASTER};
     end

   // control when the data is received over 
   always @(posedge CLK_I or posedge RST_I) begin
      if (RST_I)
         rx_latch_flag <= 1'b0;
      else if ((c_status == ST_TRANS) && (n_status !== ST_TRANS))
         rx_latch_flag <= 1'b1;
      else if (rx_latch_flag)
         rx_latch_flag <= 1'b0;
      end

   always @(posedge CLK_I or posedge RST_I)
      if (RST_I || (c_status == ST_IDLE))
         clock_cnt <= 0;
      else if (clock_cnt == CLOCK_SEL)
         clock_cnt <= 0;
      else
         clock_cnt <= clock_cnt + 1;
         
   always @(posedge CLK_I or posedge RST_I) begin
      if (RST_I)
         c_status <= ST_IDLE;
      else
         c_status <= n_status;
      end
     
   wire [5:0] ACTUAL_MAX = (CLOCK_POLARITY == CLOCK_PHASE) ? 
                          DATA_LENGTH - 1 :
                          DATA_LENGTH;
   always @(*)
       begin
       case(c_status)
       ST_IDLE:   if (pending_data)
                       n_status <= ST_LOAD;
                  else 
                       n_status <= ST_IDLE;
       ST_LOAD:  begin
                     if (DELAY_TIME == 0)
                         n_status <= ST_TRANS;
                     else
                         n_status <= ST_WAIT;
                  end
       ST_WAIT:   begin
                     if ((clock_cnt == CLOCK_SEL) && (data_cnt == DELAY_TIME - 1))
                         n_status <= ST_TRANS;
                     else 
                         n_status <= ST_WAIT;
                  end
       ST_TRANS:  begin
                     if ((clock_cnt == CLOCK_SEL) &&
                         (data_cnt == ACTUAL_MAX) &&
                         (SCLK_MASTER != CLOCK_POLARITY))                      
                         n_status <= ST_TURNAROUND;
                     else
                         n_status <= ST_TRANS;
                  end
       ST_TURNAROUND:  begin
                        if (clock_cnt == CLOCK_SEL)
                          if (INTERVAL_LENGTH)
                             n_status <= ST_INTERVAL;
                          else 
                             n_status <= ST_IDLE;
                       end
       ST_INTERVAL:  begin
                       if ((clock_cnt == CLOCK_SEL) && (data_cnt == INTERVAL_LENGTH))
                          n_status <= ST_IDLE;
                       else
                          n_status <= ST_INTERVAL;
                       end
       default:     n_status <= ST_IDLE;
       endcase
       end

   always @(posedge CLK_I or posedge RST_I)  begin
      if (RST_I)
         data_cnt <= 0;
      else if ((c_status == ST_WAIT)   && (clock_cnt == CLOCK_SEL) &&  (data_cnt == DELAY_TIME - 1))
         data_cnt <= 0;
      else if ((c_status == ST_TRANS)  && (clock_cnt == CLOCK_SEL) && (data_cnt == ACTUAL_MAX) && (CLOCK_POLARITY != SCLK_MASTER))
         data_cnt <= 0;
      else if ((c_status == ST_INTERVAL) && (clock_cnt == CLOCK_SEL) && (data_cnt == INTERVAL_LENGTH))
         data_cnt <= 0;
      else if (((c_status == ST_WAIT)   && (clock_cnt == CLOCK_SEL)) || 
               ((c_status == ST_TRANS)  && (clock_cnt == CLOCK_SEL)  && (CLOCK_PHASE != SCLK_MASTER))||
               ((c_status == ST_INTERVAL) && (clock_cnt == CLOCK_SEL)))
         data_cnt <= data_cnt + 1;
      end

   reg wait_one_tick_done;
   always @(posedge CLK_I or posedge RST_I) begin
      if (RST_I) 
          wait_one_tick_done <= 1'b0;
      else if (CLOCK_PHASE == CLOCK_POLARITY)
          wait_one_tick_done <= 1'b1;
      else if ((c_status == ST_TRANS) && (clock_cnt == CLOCK_SEL) && (data_cnt == 1))
          wait_one_tick_done <= 1'b1;
      else if (data_cnt == 0)
          wait_one_tick_done <= 1'b0;
      end      
 
    always @(posedge CLK_I or posedge RST_I) begin
      if (RST_I) begin
         MOSI_MASTER <= 0;
         tx_shift_data <= 0;  end
      else if (((c_status == ST_LOAD) && (n_status == ST_TRANS)) || 
               ((c_status == ST_WAIT) && (n_status == ST_TRANS))) begin
             MOSI_MASTER   <= SHIFT_DIRECTION ? reg_txdata[0] : 
                                                reg_txdata[DATA_LENGTH-1];
             tx_shift_data <= SHIFT_DIRECTION ? {1'b0, reg_txdata[DATA_LENGTH-1:1]} :
                                                {reg_txdata, 1'b0}; 
             end
      else if ((c_status == ST_TRANS) && (clock_cnt == CLOCK_SEL) && (CLOCK_PHASE ^ SCLK_MASTER) )  
            if (wait_one_tick_done) begin
              MOSI_MASTER   <= SHIFT_DIRECTION ? tx_shift_data[0] :
                                                 tx_shift_data[DATA_LENGTH-1];
              tx_shift_data <= SHIFT_DIRECTION ? {1'b0, tx_shift_data[DATA_LENGTH-1:1]} : 
                                                 {tx_shift_data, 1'b0}; 
            end
      end
   
   always @(posedge CLK_I or posedge RST_I)
      if (RST_I)
         reg_trdy <= 1;
      else if ((c_status != ST_TRANS) && (n_status == ST_TRANS))
         reg_trdy <= 1;
      else if (reg_wr && dw04_cs && SPI_ACK_O)
         reg_trdy <= 0;
   
   always @(posedge CLK_I or posedge RST_I)
      if (RST_I)
         reg_toe <= 0;
      else if (!reg_trdy && reg_wr && dw04_cs && SPI_ACK_O)
         reg_toe <= 1'b1;
      else if (reg_wr && dw08_cs && SPI_ACK_O)
         reg_toe <= 1'b0;
   
   always @(posedge CLK_I or posedge RST_I)
      if (RST_I)  begin
         reg_rrdy <= 1'b0;
         reg_roe  <= 1'b0;  end
      else if ((n_status == ST_TURNAROUND) && (clock_cnt == CLOCK_SEL)) begin
         if (reg_rrdy)
            reg_roe <= 1'b1;
         else  begin
            reg_rrdy <= 1'b1;
            end
         end
      else if (reg_rd && dw00_cs && SPI_ACK_O) begin
         reg_rrdy <= 1'b0;
         reg_roe  <= 1'b0; end
   
   always @(posedge CLK_I or posedge RST_I)
      if (RST_I)
         reg_tmt <= 1'b1;
      else if ((c_status != ST_IDLE) || pending_data)
         reg_tmt <= 1'b0;
      else 
         reg_tmt <= 1'b1;

  end
  endgenerate
  //End of Master Section
  
  //Slave Mode     
generate 

if (MASTER == 0) begin
   //Slave Only registers/wires
   reg                     tx_done;
   reg                     rx_done;
   reg                     rx_done_flip1;
   reg                     rx_done_flip2;
   reg  [5:0]              rx_data_cnt;                     
   reg  [5:0]              tx_data_cnt;                     

   assign  reg_control     = {1'b0,   1'b0,reg_ie,reg_irrdy,reg_itrdy,1'b0,reg_itoe,reg_iroe,3'h0};
   
   always @(posedge CLK_I or posedge RST_I)
     if (RST_I)
       SPI_DAT_O     <= #UDLY 32'h0;
     else if (reg_rd)
       SPI_DAT_O     <= #UDLY  dw00_cs ? reg_rxdata : 
                               dw04_cs ? reg_txdata : 
                               dw08_cs ? reg_status : 
                               dw0c_cs ? reg_control: 
                                         32'h0;

   //For Rx data, sample at posedge when CLOCK_PHASE is 0
   if (CLOCK_PHASE == 0) begin
     always @(posedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
         rx_shift_data     <= #UDLY 'h0;
       else if (!SS_N_SLAVE) 
           if (SHIFT_DIRECTION)
              rx_shift_data   <= #UDLY {MOSI_SLAVE,rx_shift_data[DATA_LENGTH-1:1]};
           else
              rx_shift_data   <= #UDLY {rx_shift_data,MOSI_SLAVE};

     always @(posedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          rx_data_cnt     <= #UDLY 'h0; 
       else if (rx_data_cnt == DATA_LENGTH - 1) 
          rx_data_cnt     <= #UDLY 'h0;
       else if (!SS_N_SLAVE) 
          rx_data_cnt     <= #UDLY rx_data_cnt + 1;

     always @(posedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          rx_done         <= #UDLY 1'b0; 
       else if (rx_data_cnt == DATA_LENGTH - 1) 
          rx_done         <= #UDLY 1'b1; 
       else
          rx_done         <= #UDLY 1'b0; 

     end
   else begin
    //For Rx data, sample at negedge when CLOCK_PHASE is 1
    always @(negedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
         rx_shift_data     <= #UDLY 'h0;
       else if (!SS_N_SLAVE) 
           if (SHIFT_DIRECTION)
              rx_shift_data   <= #UDLY {MOSI_SLAVE,rx_shift_data[DATA_LENGTH-1:1]};
           else
              rx_shift_data   <= #UDLY {rx_shift_data,MOSI_SLAVE};

     always @(negedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          rx_data_cnt     <= #UDLY 'h0; 
       else if (rx_data_cnt == DATA_LENGTH - 1) 
          rx_data_cnt     <= #UDLY 'h0;
       else if (!SS_N_SLAVE) 
          rx_data_cnt     <= #UDLY rx_data_cnt + 1;

     always @(negedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          rx_done         <= #UDLY 1'b0; 
       else if (rx_data_cnt == DATA_LENGTH - 1) 
          rx_done         <= #UDLY 1'b1; 
       else
          rx_done         <= #UDLY 1'b0; 
     end
   
   always @(posedge CLK_I or posedge RST_I)
     if (RST_I) begin
       rx_done_flip1                <= #UDLY 1'b0;
       rx_done_flip2                <= #UDLY 1'b0;end
     else begin
       rx_done_flip1                <= #UDLY rx_done;
       rx_done_flip2                <= #UDLY rx_done_flip1;end

   always @(posedge CLK_I or posedge RST_I)
     if (RST_I)
       rx_latch_flag                <= #UDLY 1'b0;
     else if (rx_done_flip1 && !rx_done_flip2)
       rx_latch_flag                <= #UDLY 1'b1;
     else
       rx_latch_flag                <= #UDLY 1'b0;

   always @(posedge CLK_I or posedge RST_I)
     if (RST_I) 
       reg_rrdy                     <= #UDLY 1'b0;
     else if (rx_done_flip1 && !rx_done_flip2)
       reg_rrdy                     <= #UDLY 1'b1;
     else if (reg_rd && dw00_cs && SPI_ACK_O)
       reg_rrdy                     <= #UDLY 1'b0;

   always @(posedge CLK_I or posedge RST_I)
     if (RST_I) 
       reg_roe                      <= #UDLY 1'b0;  
     else if (rx_done_flip1 && !rx_done_flip2 && reg_rrdy) 
       reg_roe                      <= #UDLY 1'b1;   
     else if (reg_wr && dw08_cs && SPI_ACK_O)
       reg_roe                      <= #UDLY 1'b0;

   //For Tx data, update at negedge when CLOCK_PHASE is 0

   if (CLOCK_PHASE == 0) begin
     if (CLOCK_POLARITY == 1) begin
       always @(negedge SCLK_SLAVE or posedge RST_I) 
         if (RST_I) 
           MISO_SLAVE   <= #UDLY 1'b0;
         else
           MISO_SLAVE   <= #UDLY SHIFT_DIRECTION ? reg_txdata[tx_data_cnt] :
                                                   reg_txdata[DATA_LENGTH-tx_data_cnt-1];
       end
     else begin   
       always @(*) 
         MISO_SLAVE   <= #UDLY SHIFT_DIRECTION ? reg_txdata[tx_data_cnt] :
                                                 reg_txdata[DATA_LENGTH-tx_data_cnt-1];
       end
     
     always @(negedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          tx_data_cnt     <= #UDLY 'h0; 
       else if (tx_data_cnt == DATA_LENGTH - 1)
          tx_data_cnt     <= #UDLY 'h0;
       else if (!SS_N_SLAVE)
          tx_data_cnt     <= #UDLY tx_data_cnt + 1;
          
     always @(negedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          tx_done     <= #UDLY 1'b0; 
       else if (tx_data_cnt == DATA_LENGTH - 1)
          tx_done     <= #UDLY 1'b1;
       else 
          tx_done     <= #UDLY 1'b0;

   //For Tx data, update at posedge when CLOCK_PHASE is 1
   end else begin
     if (CLOCK_POLARITY == 1) begin
       always @(*) 
         MISO_SLAVE   <= #UDLY SHIFT_DIRECTION ? reg_txdata[tx_data_cnt] :
                                                 reg_txdata[DATA_LENGTH-tx_data_cnt-1];
       end
     else begin   
       always @(posedge SCLK_SLAVE or posedge RST_I) 
         if (RST_I) 
           MISO_SLAVE   <= #UDLY 1'b0;
         else
           MISO_SLAVE   <= #UDLY SHIFT_DIRECTION ? reg_txdata[tx_data_cnt] :
                                                   reg_txdata[DATA_LENGTH-tx_data_cnt-1];
       end

     always @(posedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          tx_data_cnt     <= #UDLY 'h0; 
       else if (tx_data_cnt == DATA_LENGTH - 1)
          tx_data_cnt     <= #UDLY 'h0;
       else if (!SS_N_SLAVE)
          tx_data_cnt     <= #UDLY tx_data_cnt + 1;
     
     always @(posedge SCLK_SLAVE or posedge RST_I)
       if (RST_I) 
          tx_done     <= #UDLY 1'b0; 
       else if (tx_data_cnt == DATA_LENGTH - 1)
          tx_done     <= #UDLY 1'b1;
       else 
          tx_done     <= #UDLY 1'b0;
     end
   
   always @(posedge CLK_I or posedge RST_I)
     if (RST_I) 
        reg_trdy     <= #UDLY 1'b1;
     else if (reg_wr && dw04_cs && SPI_ACK_O)
        reg_trdy     <= #UDLY 1'b0;
     else if (tx_done)
        reg_trdy     <= #UDLY 1'b1;

   always @(posedge CLK_I or posedge RST_I)
     if (RST_I) 
        reg_tmt     <= #UDLY 1'b1;
     else if (reg_wr && dw04_cs && SPI_ACK_O)
        reg_tmt     <= #UDLY 1'b0;
     else if (tx_done)
        reg_tmt     <= #UDLY 1'b1;

   always @(posedge CLK_I or posedge RST_I)
     if(RST_I)
       reg_toe      <= #UDLY 1'b0;
     else if(!reg_trdy && reg_wr && dw04_cs && SPI_ACK_O)
       reg_toe      <= #UDLY 1'b1;
     else if(reg_wr && dw08_cs && SPI_ACK_O)
       reg_toe      <= #UDLY 1'b0;
     
end
endgenerate

endmodule
`endif
