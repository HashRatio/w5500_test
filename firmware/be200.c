#include "be200.h"
#include "miner.h"
#include "utils.h"
#include "stratum.h"
#include "io.h"
#include "minilibc.h"
#include "defines.h"
#include "pub_var.h"

static uint32 nonce2 = 0; 
static uint8 last_cmd = C_ASK;
static uint8 last_ans = A_NO;

void tube_send_work(struct work *w)
{
    w->data[44] = 0xff & (w->nonce2 >> 24);
    w->data[45] = 0xff & (w->nonce2 >> 16);
    w->data[46] = 0xff & (w->nonce2 >> 8);
    w->data[47] = 0xff & (w->nonce2);
    w->data[48] = w->mm_idx;
    uart_writecmd(C_JOB, 1);
    uart_nwrite((const char *)w->data, 49);
    return;
}

static int tube_get_result(int32 board, uint32 * ptr_ntime, uint32 * ptr_nonce, uint32 * ptr_nonce2)
{
    static int8 result[54];
    uint32 nonce_new, mm_idx;
    int32 nonce_check = NONCE_HW;
    int32 i;
    int8 diff_nonce[] = {0, -1, 1, -2, 2, -3, 3, 4, -4};

    for (i = 0; i < 54; i++)
    {
        result[i] = uart_read(1);
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

void tube_handler(uint8 bid)
{
    int32 to_read;
    int32 nonce_check;
    uint32 nonce_submit, nonce2_submit, ntime_submit;
    
    //debug32("LAST_CMD:%02x  LAST_ANS:%02x\n",last_cmd,last_ans);
    
    if(last_cmd == C_ASK && last_ans == A_NO)
    {
        uart_writecmd(C_ASK, 1);
        last_ans = 0;
    }
    else if(last_cmd == C_ASK && last_ans == 0)
    {
        if(uart_read_nonblock() == 0){ 
            return;
        }
        //debug32("hash board response.");
        last_ans = uart_read(1);
        //debug32("0x%02x\n",last_ans);
    }
    else if(last_cmd == C_ASK && last_ans == A_YES)//ready to read nonce
    {
        //debug32("ready to read nonce\n");
        to_read = uart_read_nonblock();
        //debug32("Data to read:%d\n",to_read);
        if(to_read < 54){
            return;
        }
        nonce_check = tube_get_result(bid, &ntime_submit, &nonce_submit, &nonce2_submit);
        if (nonce_check == NONCE_VALID)
        {
            send_submit(mm_work_ptr, nonce2_submit, ntime_submit, bswap_32(nonce_submit));
            calc_hashrate();
        }
        last_cmd = C_ASK;
        last_ans = A_NO;
        
    }
    else if(last_cmd == C_ASK && last_ans == A_WAL)//send work to hash board
    {
        if(g_new_stratum)
        {
            //debug32("send work to hash board\n");
            miner_gen_nonce2_work(mm_work_ptr, nonce2, &g_works[0]);
            nonce2++;
            //debug32("before send\n");
            tube_send_work(&g_works[0]);
            //debug32("after send\n");
            g_new_stratum = 1;
        }
        last_cmd = C_JOB;
        last_ans = A_NO;
    }
    else if(last_cmd == C_JOB && last_ans == A_NO)//wait hash board sync work
    {
        uart_read(1);
        last_cmd = C_ASK;
        last_ans = A_NO; 
    }
    else
    {
        debug32("LAST_CMD:%02x  LAST_ANS:%02x\n",last_cmd,last_ans);
        uart_writecmd(0,1);
        last_cmd = C_ASK;
        last_ans = A_NO; 
    }
}