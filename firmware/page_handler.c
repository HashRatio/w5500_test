#include "httputil.h"
#include "pub_var.h"
#include "driver-tube.h"

#define TESETSTATUS_ITEM_LENGTH 60

const int8 RESPONSE_HEADER[] SECTION_TEXT = "HTTP/1.1 200 OK\r\nConnection:Keep-Alive\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n";
const int8 PAGE_HEADER[] SECTION_TEXT = "\
<html><head><title>UniHash Miner Controller</title>\
</head>\
\
<body bgcolor=#A0BF7C text=#ffffff>\
<table cellpadding=3 cellspacing=0 width=100%>\
<tbody><tr bgcolor=#65934A><td align=center><h2><b>UniHash Miner Controller</b></h2></td></tr></tbody>\
</table>\
<center><h3><a href=\"/TestStatus/\">Test Status</a>&nbsp&nbsp&nbsp&nbsp\
<a href=\"/Statistics/\">Statistics</a>&nbsp&nbsp&nbsp&nbsp\
<a href=\"/Network/\">Network</a>&nbsp&nbsp&nbsp&nbsp\
<a href=\"/Mining/\">Mining</a></center>\
</h3><hr width=\"80%\" align=\"center\" size=\"5\" color=\"black\"></hr><center>";


const int8 TESETSTATUS_TEMPLATE1[] SECTION_TEXT = "<br>Board %02d: |%c%c%c|%c%c%c|%c%c%c|%c%c%c|%c%c%c|%c%c%c|%c%c%c|%c%c%c| Version:%1d.%02d";
const int8 PAGE_TESTSTATUS_HEADER[] SECTION_TEXT = "\
<h2>TestStatus</h2>\
<font face=\"courier new\" size=2 color=white>";
const int8 PAGE_TESTSTATUS_TAIL[] SECTION_TEXT = "<br><br>\
</body></html>";

void send_http_header(SOCKET s,int32 len)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    m_sprintf(tx_buf, RESPONSE_HEADER, len);
    send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
}

void send_page_header(SOCKET s)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    memcpy(tx_buf, PAGE_HEADER, strlen(PAGE_HEADER));
    send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
}

void send_page_teststatus_header(SOCKET s)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    memcpy(tx_buf, PAGE_TESTSTATUS_HEADER, strlen(PAGE_TESTSTATUS_HEADER));
    send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
}

void send_page_teststatus_tail(SOCKET s)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    memcpy(tx_buf, PAGE_TESTSTATUS_TAIL, strlen(PAGE_TESTSTATUS_TAIL));
    send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
}


void send_page_teststatus_item(SOCKET s,uint8 bid)
{
    uint32 i;
    int8 st[24];
    int8 version1,version2;
    
    for(i = 0;i < 24; i++)
    {
        switch((board_status[bid] >> (i<<1)) & (uint64)0x00000000000003)
        {
        case STATUS_ERR:
            st[i] = '-';
            break;
        case STATUS_PWR:
            st[i] = '.';
            break;
        case STATUS_OK:
            st[i] = 'O';
            break;
        default:
            st[i] = '?';
        }
    }
    version1 = board_version[bid]/100;
    version2 = board_version[bid]%100;
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    m_sprintf(tx_buf,TESETSTATUS_TEMPLATE1,bid+1,
        st[0],st[1],st[2],st[3],
        st[4],st[5],st[6],st[7],
        st[8],st[9],st[10],st[11],
        st[12],st[13],st[14],st[15],
        st[16],st[17],st[18],st[19],
        st[20],st[21],st[22],st[23],
        version1,version2);
    send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
}

void send_page_teststatus(SOCKET s)
{
    int8 board_cnt = 0;
    int8 bid;
    int32 total_length;
    
    board_cnt = tube_board_count();
    total_length =  strlen(PAGE_HEADER)
                     + strlen(PAGE_TESTSTATUS_HEADER)
                     + TESETSTATUS_ITEM_LENGTH * board_cnt
                     + strlen(PAGE_TESTSTATUS_TAIL);
    send_http_header(s, total_length);
    send_page_header(s);
    send_page_teststatus_header(s);
    for(bid = 0; bid < sizeof(last_ans); bid++)
    {
        if(last_ans[(int32)bid] != 0xFF)
            send_page_teststatus_item(s,bid);
    }
    send_page_teststatus_tail(s);
}

const int8 PAGE_STATISTICS_HEADER[] SECTION_TEXT = "\
<h2>Statistics</h2>\
<center><h3>Hashrate:%4d GH/s&nbsp&nbsp&nbsp&nbsp\
Clock:%3d MHz&nbsp&nbsp&nbsp&nbsp\
Chips:%3d </center>\
</h3><center><font face=\"courier new\" size=2 color=white>";
const int8 STATISTICS_TEMPLATE1[] SECTION_TEXT = "<br>Board %02d:  Hashrate:%03dGH/s  ,Working:%02d  ,Error:%02d  ,PwrDown:%02d";
const int8 PAGE_STATISTICS_TAIL[] SECTION_TEXT = "\
</body></html>";

int32 send_page_statistics_header(SOCKET s,uint8 send_flag)
{
    uint16 freq;
    uint8 bid;
    uint16 chip_cnt = 0;
    
    freq = (tube_get_freq() + 1)/2*20; 
    for(bid = 0; bid < 32; bid++)
    {
        chip_cnt += tube_chip_count(bid,STATUS_OK);
    }
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    m_sprintf(tx_buf, PAGE_STATISTICS_HEADER,tube_total_hashrate(),freq,chip_cnt);
    if(send_flag)
        send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
    return strlen(tx_buf);
}

int32 send_page_statistics_item(SOCKET s,uint8 bid,uint8 send_flag)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    m_sprintf(tx_buf, STATISTICS_TEMPLATE1,bid,tube_get_hashrate(bid),
                tube_chip_count(bid,STATUS_OK),
                tube_chip_count(bid,STATUS_ERR),
                tube_chip_count(bid,STATUS_PWR));
    if(send_flag)
        send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
    return strlen(tx_buf);
}

void send_page_statistics_tail(SOCKET s)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    memcpy(tx_buf, PAGE_STATISTICS_TAIL, strlen(PAGE_STATISTICS_TAIL));
    send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
}

void send_page_statistics(SOCKET s)
{
    int8 board_cnt = 0;
    int8 bid;
    int32 total_length;
    board_cnt = tube_board_count();
    
    total_length =  strlen(PAGE_HEADER)
                     + send_page_statistics_header(s,0)
                     + send_page_statistics_item(s,0,0) * board_cnt
                     + strlen(PAGE_STATISTICS_TAIL);
    send_http_header(s, total_length);
    send_page_header(s);
    send_page_statistics_header(s,1);
    for(bid = 0; bid < board_cnt; bid++)
    {
        send_page_statistics_item(s,bid,1);
    }
    send_page_statistics_tail(s);
}

const int8 PAGE_NETWORK[] SECTION_TEXT = "\
<h2>Network</h2>\
<form action=Upload method=post name=upload><table align=center border=0 cellspacing=0><tr>\
<td align=right>IP Addr:</td><td align=left><input name=JMIP value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Subnet :</td><td align=left><input name=JMSK value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Gateway:</td><td align=left><input name=JGTW value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Pri DNS:</td><td align=left><input name=PDNS value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Sec DNS:</td><td align=left><input name=SDNS value='%s' size=42 type=text></td></tr>\
<tr><td align=center colspan=2><br><input name=update value=Update/Restart type=submit><br><br>\
</form></table></html>";

int32 send_page_network_header(SOCKET s, uint8 send_flag)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    m_sprintf(tx_buf, PAGE_NETWORK,
                "nothing filled",
                "nothing filled",
                "nothing filled",
                "nothing filled",
                "nothing filled");
    if(send_flag)
        send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
    return strlen(tx_buf);
}

void send_page_network(SOCKET s)
{
    int32 total_length;
    
    total_length =  strlen(PAGE_HEADER)
                     + send_page_network_header(s,0);
    send_http_header(s, total_length);
    send_page_header(s);
    send_page_network_header(s,1);
}

const int8 PAGE_MINING[] SECTION_TEXT = "\
<h2>Mining</h2>\
<form action=Upload method=post name=upload><table align=center border=0 cellspacing=0><tr>\
<td align=right>Pool 1:</td><td align=left><input name=JMIP value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Miner 1:</td><td align=left><input name=JMSK value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Passwd 1:</td><td align=left><input name=JGTW value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Pool 2:</td><td align=left><input name=PDNS value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Miner 2:</td><td align=left><input name=SDNS value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Passwd 2:</td><td align=left><input name=SDNS value='%s' size=42 type=text></td></tr>\
<tr><td align=right>Clock[Mhz]:</td><td align=left><input name=SDNS value='%d' size=42 type=text></td></tr>\
<tr><td align=center colspan=2><br><input name=update value=Update type=submit><br><br>\
</form></table></html>";

int32 send_page_mining_header(SOCKET s, uint8 send_flag)
{
    memset(tx_buf, 0, MAX_TX_URI_SIZE);
    m_sprintf(tx_buf, PAGE_MINING,
                "nothing filled",
                "nothing filled",
                "nothing filled",
                "nothing filled",
                "nothing filled",
                "nothing filled",
                0);
    if(send_flag)
        send(s, (uint8 *)tx_buf, strlen((int8 const*)tx_buf), 0);
    return strlen(tx_buf);
}

void send_page_mining(SOCKET s)
{
    int32 total_length;
    
    total_length =  strlen(PAGE_HEADER)
                     + send_page_mining_header(s,0);
    send_http_header(s, total_length);
    send_page_header(s);
    send_page_mining_header(s,1);
}