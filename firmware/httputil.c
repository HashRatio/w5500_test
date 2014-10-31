#include "httputil.h"

#define DEBUG_HTTP

char tx_buf[MAX_TX_URI_SIZE];
char rx_buf[MAX_RX_URI_SIZE];

const int8 URL_TESTSTATUS[] SECTION_TEXT = "/TestStatus/";
const int8 URL_STATISTICS[] SECTION_TEXT = "/Statistics/";
const int8 URL_NETWORK[] SECTION_TEXT = "/Network/";
const int8 URL_MINING[] SECTION_TEXT = "/Mining/";

extern void send_page_teststatus(SOCKET s);
extern void send_page_statistics(SOCKET s);
extern void send_page_network(SOCKET s);
extern void send_page_mining(SOCKET s);

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
        memset(tx_buf, 0, MAX_TX_URI_SIZE);
        if (strcmp(name, "/") == 0 || (strcmp(name, URL_STATISTICS) == 0))
        {
            send_page_statistics(s);
        }
        else if (strcmp(name, URL_NETWORK) == 0)
        {
            send_page_network(s);
        }
        else if (strcmp(name, URL_TESTSTATUS) == 0)
        {
            send_page_teststatus(s);
        }
        else if (strcmp(name, URL_MINING) == 0)
        {
            send_page_mining(s);
        }
        break;
    default :
        break;
    }
}


