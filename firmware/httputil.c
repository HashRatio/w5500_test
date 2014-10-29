#include "httputil.h"

#define DEBUG_HTTP
#define SECTION_TEXT __attribute__((section(".text")))
extern char tx_buf[MAX_TX_URI_SIZE];
extern char rx_buf[MAX_RX_URI_SIZE];

const int8 RESPONSE_HEADER[] SECTION_TEXT = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:";
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

const int8 PAGE_TESTSTATUS[] SECTION_TEXT = "\
<h2>TestStatus</h2>\
</body></html>";

const int8 PAGE_STATISTICS[] SECTION_TEXT = "\
<h2>Statistics</h2>\
</body></html>";

const int8 PAGE_NETWORK[] SECTION_TEXT = "\
<h2>Network</h2>\
<form action=Upload method=post name=upload><table align=center border=0 cellspacing=0><tr>\
<td align=right>IP Addr:</td><td align=left><input name=JMIP value='' size=42 type=text></td></tr>\
<tr><td align=right>Subnet :</td><td align=left><input name=JMSK value='' size=42 type=text></td></tr>\
<tr><td align=right>Gateway:</td><td align=left><input name=JGTW value='' size=42 type=text></td></tr>\
<tr><td align=right>Pri DNS:</td><td align=left><input name=PDNS value='' size=42 type=text></td></tr>\
<tr><td align=right>Sec DNS:</td><td align=left><input name=SDNS value='' size=42 type=text></td></tr>\
<tr><td align=center colspan=2><br><input name=update value=Update/Restart type=submit><br><br>\
</form></table></html>";

const int8 PAGE_MINING[] SECTION_TEXT = "\
<h2>Mining</h2>\
<form action=Upload method=post name=upload><table align=center border=0 cellspacing=0><tr>\
<td align=right>Pool 1:</td><td align=left><input name=JMIP value='' size=42 type=text></td></tr>\
<tr><td align=right>Miner 1:</td><td align=left><input name=JMSK value='' size=42 type=text></td></tr>\
<tr><td align=right>Passwd 1:</td><td align=left><input name=JGTW value='' size=42 type=text></td></tr>\
<tr><td align=right>Pool 2:</td><td align=left><input name=PDNS value='' size=42 type=text></td></tr>\
<tr><td align=right>Miner 2:</td><td align=left><input name=SDNS value='' size=42 type=text></td></tr>\
<tr><td align=right>Passwd 2:</td><td align=left><input name=SDNS value='' size=42 type=text></td></tr>\
<tr><td align=right>Clock[Mhz]:</td><td align=left><input name=SDNS value='' size=42 type=text></td></tr>\
<tr><td align=center colspan=2><br><input name=update value=Update type=submit><br><br>\
</form></table></html>";

const int8 URL_TESTSTATUS[] SECTION_TEXT = "/TestStatus/";
const int8 URL_STATISTICS[] SECTION_TEXT = "/Statistics/";
const int8 URL_NETWORK[] SECTION_TEXT = "/Network/";
const int8 URL_MINING[] SECTION_TEXT = "/Mining/";


uint8 boundary[64];

void do_http(void)
{
    uint8 ch = SOCK_HTTP;
    uint16 len;

    //st_http_request *http_request;
    memset(rx_buf, 0x00, MAX_RX_URI_SIZE);
    //http_request = (st_http_request*)rx_buf;        // struct of http request
    /* http service start */
    switch (getSn_SR(ch))
    {
    case SOCK_INIT:
        listen(ch);
        break;
    case SOCK_LISTEN:
        break;
    case SOCK_ESTABLISHED:
        //case SOCK_CLOSE_WAIT:
        if (getSn_IR(ch) & Sn_IR_CON)
        {
            setSn_IR(ch, Sn_IR_CON);
        }
        if ((len = getSn_RX_RSR(ch)) > 0)
        {
            len = recv(ch, (uint8*)rx_buf, len);
            *(rx_buf + len) = 0;
            proc_http(ch, (uint8*)rx_buf); // request is processed
            disconnect(ch);
        }
        break;
    case SOCK_CLOSE_WAIT:
        if ((len = getSn_RX_RSR(ch)) > 0)
        {
            //debug32("close wait: %d\r\n",len);
            len = recv(ch, (uint8*)rx_buf, len);
            *(rx_buf + len) = 0;
            proc_http(ch, (uint8*)rx_buf); // request is processed
        }
        disconnect(ch);
        break;
    case SOCK_CLOSED:
        socket(ch, Sn_MR_TCP, 80, 0x00);    /* reinitialize the socket */
        break;
    default:
        break;
    }// end of switch
}
static void make_response(const int8 * content)
{
    int16 offset = 0;
    int16 content_len = 0;
    int8 tail[10];

    content_len = strlen(PAGE_HEADER) + strlen(content);

    memcpy(tx_buf, RESPONSE_HEADER, strlen(RESPONSE_HEADER));
    offset = strlen(RESPONSE_HEADER);

    m_sprintf(tail, "%d\r\n\r\n", content_len);

    memcpy(tx_buf + offset, tail, strlen(tail));
    offset += strlen(tail);

    debug32("PAGE_HEADER:%d\n", strlen(PAGE_HEADER));
    memcpy(tx_buf + offset, PAGE_HEADER, strlen(PAGE_HEADER));
    offset += strlen(PAGE_HEADER);

    memcpy(tx_buf + offset, content, strlen(content));

    debug32("%s\n", tx_buf);
}
void proc_http(SOCKET s, uint8 * buf)
{
    int8* name; //get method request file name
    st_http_request http_request;
    memset(tx_buf, 0x00, MAX_TX_URI_SIZE);
    parse_http_request(&http_request, buf);    // After analyze request, convert into http_request
    switch (http_request.METHOD)
    {
    case METHOD_ERR :
        debug32("e");
        if (strlen((int8 const*)boundary) > 0)
        {
            debug32("Error=%s\r\n", http_request.URI);
        }
        else
        {
            memcpy(rx_buf, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
            send(s, (uint8 *)rx_buf, strlen((int8 const*)rx_buf), 0);
        }
        break;
    case METHOD_HEAD:
    case METHOD_GET:
        name = http_request.URI;
        if (strcmp(name, "/index.htm") == 0 || strcmp(name, "/") == 0 || (strcmp(name, "/index.html") == 0) || (strcmp(name, URL_STATISTICS) == 0))
        {
            memset(tx_buf, 0, MAX_TX_URI_SIZE);
            make_response(PAGE_STATISTICS);
            debug32("%d\n", strlen(tx_buf));
            send(s, (uint8 *)tx_buf, strlen((char const*)tx_buf), 0);
        }
        else if (strcmp(name, URL_NETWORK) == 0)
        {
            memset(tx_buf, 0, MAX_TX_URI_SIZE);
            make_response(PAGE_NETWORK);
            debug32("%d\n", strlen(tx_buf));
            send(s, (uint8 *)tx_buf, strlen((char const*)tx_buf), 0);
        }
        else if (strcmp(name, URL_TESTSTATUS) == 0)
        {
            memset(tx_buf, 0, MAX_TX_URI_SIZE);
            make_response(PAGE_TESTSTATUS);
            debug32("%d\n", strlen(tx_buf));
            send(s, (uint8 *)tx_buf, strlen((char const*)tx_buf), 0);
        }
        else if (strcmp(name, URL_MINING) == 0)
        {
            memset(tx_buf, 0, MAX_TX_URI_SIZE);
            make_response(PAGE_MINING);
            debug32("%d\n", strlen(tx_buf));
            send(s, (uint8 *)tx_buf, strlen((char const*)tx_buf), 0);
        }
        break;
    default :
        break;
    }
}


