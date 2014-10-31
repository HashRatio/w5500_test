#include "driver-tube.h"
#include "miner.h"
#include "utils.h"
#include "stratum.h"
#include "io.h"
#include "minilibc.h"
#include "defines.h"
#include "pub_var.h"

static uint32 board_share[32];
static uint32 board_hashrate[32];
static uint8 freq;

static void tube_send_cmd(uint8 bid, uint8 cmd)
{
    uart_writecmd(cmd|(bid&0x1f),1);
}


static void tube_send_work(uint8 bid, struct work *w)
{
    w->data[44] = 0xff & (w->nonce2 >> 24);
    w->data[45] = 0xff & (w->nonce2 >> 16);
    w->data[46] = 0xff & (w->nonce2 >> 8);
    w->data[47] = 0xff & (w->nonce2);
    w->data[48] = w->mm_idx;
    tube_send_cmd(bid, C_JOB);
    uart_nwrite((const char *)w->data, 49);
    uart_read();
    return;
}


static int tube_get_result(uint32 * ptr_ntime, uint32 * ptr_nonce, uint32 * ptr_nonce2)
{
    static int8 result[54];
    uint32 nonce_new, mm_idx;
    int32 nonce_check = NONCE_HW;
    int32 i;
    int8 diff_nonce[] = {0, -1, 1, -2, 2, -3, 3, 4, -4};

    for (i = 0; i < 54; i++)
    {
        result[i] = uart_read();
    }

    *ptr_ntime = (((uint32_t)result[36] << 24)  |
                  ((uint32_t)result[37] << 16)  |
                  ((uint32_t)result[38] << 8)  |
                  ((uint32_t)result[39])) ;

    *ptr_nonce2 = (((uint32_t)result[44] << 24)  |
                   ((uint32_t)result[45] << 16)  |
                   ((uint32_t)result[46] << 8)  |
                   ((uint32_t)result[47])) ;

    *ptr_nonce = (((uint32_t)result[51] << 24)  |
                  ((uint32_t)result[50] << 16)  |
                  ((uint32_t)result[49] << 8)  |
                  ((uint32_t)result[48])) + 1;

    mm_idx = result[52];

    for (i = 0; i < sizeof(diff_nonce) / sizeof(diff_nonce[0]); i++)
    {
        nonce_new = *ptr_nonce + diff_nonce[i];
        nonce_check = test_nonce(mm_work_ptr, result, *ptr_nonce2, nonce_new);
        if (nonce_check != NONCE_HW)
        {
            *ptr_nonce = nonce_new;
            break;
        }
    }
    return nonce_check;
}


static uint8 tube_ask_status(uint8 bid)
{
    tube_send_cmd(bid, C_ASK);
    return uart_read();
}

static int32 tube_calc_hashrate(uint8 bid)
{
    board_share[bid] += (4295 * g_diff) / 1000;
    if (g_curr_ntime != g_last_ntime)
        board_hashrate[bid] = board_share[bid] / (g_curr_ntime - g_last_ntime);
    debug32("Board%02d Hashrate:%d Difficulty:%d\n", bid, board_hashrate[bid], g_diff);
    return 0;
}

uint32 tube_get_hashrate(uint8 bid)
{
    return board_hashrate[bid];
}

uint32 tube_total_hashrate()
{
    int16 bid;
    uint32 total_hashrate = 0;
    for(bid = 0; bid < sizeof(last_ans); bid++)
    {
        total_hashrate += board_hashrate[bid];
    }
    return total_hashrate;
}

int8 tube_board_count()
{
    int8 bid;
    int8 board_cnt = 0;
    for(bid = 0; bid < sizeof(last_ans); bid++)
    {
        if(last_ans[(int32)bid] != 0xFF)
            board_cnt++;
    }
    return board_cnt;
}

void tube_discover()
{
    int bid;
    for(bid = 0;bid < sizeof(last_ans);bid++)
    {
        last_ans[bid] = A_NO;
        tube_send_cmd(bid,C_ASK);
        delay(1);
        if(!uart_read_nonblock())
            last_ans[bid] = 0xFF;
        else
            last_ans[bid] = uart_read();
        debug32("%02x ",last_ans[bid]);
    }
    debug32("\n");
}


void tube_init()
{
    freq = 29;
    tube_reset_all();
    tube_diff_all(0x00);
    tube_freq_all(freq);
    delay(5000);
    tube_discover();
}

void tube_status_single(int32 bid)
{
    uint8 i,temp;
    tube_send_cmd(bid, C_TRS);
    board_status[bid] = 0;
    //uint32 upper,lower;
    for(i = 0; i < 67; i++)
    { 
        temp = uart_read();
        if(i >=0 && i< 24)
        {
            if(temp & 0xF0)        //Error
            {
                board_status[bid] |= STATUS_ERR << (i<<1); 
            }
            else if(temp & 0x0F)    //Pwr
            {
                board_status[bid] |= STATUS_PWR << (i<<1); 
            }
            else
            {
                board_status[bid] |= STATUS_OK << (i<<1); 
            }
        }
        if(i == 66)
            board_version[bid] = temp;
    }
    // upper =  board_status[bid] >> 32;
    // lower =  board_status[bid];
    // debug32("bid:%d status:%08x%08x version:%02x\n",bid,upper,lower,board_version[bid]);
}

void tube_status()
{
    int32 bid;
    for(bid = 0; bid < sizeof(last_ans); bid++)
    {
        if(last_ans[bid] == 0xFF)
            continue;
        else
        {
            tube_status_single(bid);
        }
    }
}


void tube_handler_single(uint8 bid)
{
    static int32 nonce2;
    int32 nonce_check;
    uint32 nonce_submit, nonce2_submit, ntime_submit;
    
    last_ans[bid] = tube_ask_status(bid);
    
    if(last_ans[bid] == A_NO)
    {
        return;
    }
    else if(last_ans[bid] == A_YES)
    {        
        nonce_check = tube_get_result(&ntime_submit, &nonce_submit, &nonce2_submit);
        if (nonce_check == NONCE_VALID)
        {
            //debug32("before send\n");
            send_submit(mm_work_ptr, nonce2_submit, ntime_submit, bswap_32(nonce_submit));
            //debug32("after send\n");
            tube_calc_hashrate(bid);
        }
    }
    else if(last_ans[bid] == A_WAL)
    {
        if (g_new_stratum)
        {
            //debug32("send work to hash board\n");
            miner_gen_nonce2_work(mm_work_ptr, nonce2, &g_works[0]);
            nonce2++;
            tube_send_work(bid, &g_works[0]);
            g_new_stratum = 1;
        }
    }
    else
    {
        debug32("Protocol error!\n");
    }
}


void tube_handler(void(* func)(void))
{
    int bid;
    for(bid = 0; bid < sizeof(last_ans); bid++)
    {
        if(last_ans[bid] != 0xFF)
        {
            tube_handler_single(bid);
            func();
        }
    }
}


void tube_reset_all()
{
    uart_writecmd(C_RES, 1);
}

uint8 tube_get_freq()
{
    return freq;
} 

uint16 tube_chip_count(uint8 bid,uint64 status)
{
    uint16 cnt = 0;
    uint8 i;
    for(i = 0;i < 24; i++)
    {   
        if( ((board_status[bid] >> (i<<1)) & (uint64)STATUS_OK) == (uint64)status )
            cnt++;
    }
    return cnt;
}

void tube_freq_all(uint8 freq)
{
    uart_writecmd(C_GCK | ((uint8)freq & 0x1f), 1);
    uart_writecmd(C_LPO, 1);
}


void tube_diff_all(uint8 diff)
{
    uart_writecmd(C_DIF | (diff & 0x0f), 1);
}