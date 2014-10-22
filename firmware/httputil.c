#include "httputil.h"

#define DEBUG_HTTP

extern char tx_buf[MAX_TX_URI_SIZE];
extern char rx_buf[MAX_RX_URI_SIZE];

const int8 RESPONSE_HEADER[] __attribute__((section(".text")))= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:";
const int8 page_index[] __attribute__((section(".text"))) = "<html><head><title>UniHash Miner Controller</title></head><body bgcolor=##6BC235 text=#ffffff><table cellpadding=3 cellspacing=0 width=100%><tbody><tr bgcolor=#5050ff><td align=center><h2><b>UniHash Miner Controller</b></h2></td></tr></tbody></table>\
<center><h3><a href=\"/TestStatus/\">Test Status</a>&nbsp&nbsp&nbsp&nbsp&nbsp<a href=\"/Statistics/\">Statistics</a>&nbsp&nbsp&nbsp&nbsp&nbsp<a href=\"/Settings/\">Settings</a></center></h3><hr width=\"80%\" align=\"center\" size=\"5\" color=\"black\"></hr>\
</body></html>\r\n";

const int8 page_setting[] __attribute__((section(".text"))) = "\
<html><head><title>BE200 Jet Stratum Miner</title>\
</head>\
\
<body bgcolor=#9999FF text=#ffffff>\
<table cellpadding=3 cellspacing=0 width=100%>\
<tbody><tr bgcolor=#5050ff><td align=center><h2><b>UniHash Miner Controller</b></h2></td></tr></tbody>\
</table>\
<center><h3><a href=\"/TestStatus/\">Test Status</a>\
<a href=\"/Statistics/\">Statistics</a>\
<a href=\"/Settings/\">Settings</a></center>\
</h3><hr width=\"80%\" align=\"center\" size=\"5\" color=\"black\"></hr><center>\
<h2>Settings</h2>\
<form action=Upload_Data method=post name=upload><table align=center border=0 cellspacing=0><tr>\
<td align=right>LAN</td><td align=left><input name=JMIP value='' size=42 type=text>IP<br></td></tr>\
<tr><td align=right>LAN</td><td align=left><input name=JMSK value='' size=42 type=text>Mask<br></td></tr>\
<tr><td align=right>LAN</td><td align=left><input name=JGTW value='' size=42 type=text>Gateway<br></td></tr>\
<tr><td align=right>LAN</td><td align=left><input name=WPRT value='' size=42 type=text>WEB Port<br></td></tr>\
<tr><td align=right>LAN</td><td align=left><input name=JSPT value='' size=42 type=text>System Port<br></td></tr>\
<tr><td align=right>LAN</td><td align=left><input name=PDNS value='' size=42 type=text>Primary DNS<br></td></tr>\
<tr><td align=right>LAN</td><td align=left><input name=SDNS value='' size=42 type=text>Secondary DNS<br></td></tr>\
<tr><td align=right>Stratum</td><td align=left><input name=MURL value='' size=42 type=text>Pool URL<br></td></tr>\
<tr><td align=right>Stratum</td><td align=left><input name=MPRT value='' size=42 type=text>Pool Port<br></td></tr>\
<tr><td align=right>Stratum</td><td align=left><input name=USEF value='' size=42 type=text>User name<br></td></tr>\
<tr><td align=right>Stratum</td><td align=left><input name=PASO value='' size=42 type=text>Password<br></td></tr>\
<tr><td align=right>ASIC</td><td align=left><input name=GCLK value='' size=42 type=text>Clock[MHz]<br></td></tr>\
<tr><td align=center colspan=2><br><input name=update value=Update/Restart type=submit><br><br>\
</form></table></html>";

//const int8 page_index[] __attribute__((section(".text"))) = "<html><head><title>BE200 Jet Stratum Miner</title><script>var xmlhttp;if (window.XMLHttpRequest) {  xmlhttp=new XMLHttpRequest();  }else { xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\"); }xmlhttp.onreadystatechange=function() {if (xmlhttp.readyState==4 && xmlhttp.status==200) {document.open(); document.write(xmlhttp.responseText); document.close(); } }</script></head><body bgcolor=#9999FF text=#ffffff><table cellpadding=3 cellspacing=0 width=100%><tbody><tr bgcolor=#5050ff><td align=center><h2><b>BE200 Jet Stratum Miner V 5.47</b></h2></td></tr></tbody></table><center><h3><a href=\"/TestStatus/\">Test Status</a>&nbsp&nbsp&nbsp&nbsp&nbsp<a href=\"/Statistics/\">Statistics</a>&nbsp&nbsp&nbsp&nbsp&nbsp<a href=\"/Settings/\">Settings</a></center></h3><hr width=\"80%\" align=\"center\" size=\"5\" color=\"black\"></hr><center><h2>Statistics</h2><b><font face=\"courier new\" size=2 color=white><br><br><input type=\"button\" value=\"ReSession\" onclick=\"xmlhttp.open('GET','Re_Ses',false);xmlhttp.send();\">&nbsp&nbsp<input type=\"button\" value=\"Force LP\" onclick=\"xmlhttp.open('GET','Frs_LP',false);xmlhttp.send();\">&nbsp&nbsp<input type=\"button\" value=\"Clock Up\" onclick=\"xmlhttp.open('GET','Clk_Up',false);xmlhttp.send();\">&nbsp&nbsp&nbsp&nbspClock:240MHz&nbsp&nbsp&nbsp&nbsp<input type=\"button\" value=\"Clock Down\" onclick=\"xmlhttp.open('GET','Clk_Dn',false);xmlhttp.send();\">&nbsp&nbsp<input type=\"button\" value=\"Wake Up\" onclick=\"xmlhttp.open('GET','Wake_Up',false);xmlhttp.send();\">&nbsp&nbsp<input type=\"button\" value=\"ReClock\" onclick=\"xmlhttp.open('GET','Clk_Rc',false);xmlhttp.send();\"></font><font face=\"courier new\" size=2 color=black><h3>Accepted:0.0000e+00, Rejected:0.0000e+00, Hardware errors:0.00%, PwrDn:0<br>Real performance:0.00GHs, Expected performance:0.00GHs, Utility:0.00<br>Miner:0.00%, Network diff:0.0000e+00,  Worker diff:0.00, Real_LP:0<br>Running time: 0d:00h:04m:24s, Session time: 0d:00h:04m:07s, RS:0, RC:0<br>Sha/m:0, Subm/m:0 Jobs/m:0, Nonce/m:0, Cmd/s:0, Ans/s:0</h3><br><br>*** (C) JET for Block Erupter ***";
uint8 boundary[64];
uint8 tmp_buf[1460]={0xff,};

void do_http(void)
{
  uint8 ch=SOCK_HTTP;
  uint16 len;

  //st_http_request *http_request;
  memset(rx_buf,0x00,MAX_RX_URI_SIZE);
  //http_request = (st_http_request*)rx_buf;		// struct of http request  
  /* http service start */
  switch(getSn_SR(ch))
  {
    case SOCK_INIT:
      listen(ch);
      break;
    case SOCK_LISTEN:
      break;
    case SOCK_ESTABLISHED:
    //case SOCK_CLOSE_WAIT:
      if(getSn_IR(ch) & Sn_IR_CON)
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
    int32 offset = 0;
    int8 tail[10];
    
    memcpy(tx_buf,RESPONSE_HEADER,strlen(RESPONSE_HEADER));
    offset = strlen(RESPONSE_HEADER);
    
    m_sprintf(tail,"%d\r\n\r\n",strlen(content));
    memcpy(tx_buf+offset,tail,strlen(tail));
    offset += strlen(tail);
    
    memcpy(tx_buf+offset,content,strlen(content));
    
}
void proc_http(SOCKET s, uint8 * buf)
{
    int8* name; //get method request file name
    st_http_request http_request;
    memset(tx_buf,0x00,MAX_TX_URI_SIZE);
    parse_http_request(&http_request, buf);    // After analyze request, convert into http_request
    switch (http_request.METHOD)	
    {
    case METHOD_ERR :
        debug32("e");
        if(strlen((int8 const*)boundary)>0)
        {
            debug32("Error=%s\r\n",http_request.URI);
        }
        else
        {
            memcpy(rx_buf, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
            send(s, (uint8 *)rx_buf, strlen((int8 const*)rx_buf),0);
        }
        break;
    case METHOD_HEAD:
    case METHOD_GET:
        name = http_request.URI;
        if(strcmp(name,"/index.htm")==0 || strcmp(name,"/")==0 || (strcmp(name,"/index.html")==0) || (strcmp(name,"/Statistics/")==0))
        {
            memset(tx_buf,0,MAX_TX_URI_SIZE);
            make_response(page_index);
            send(s, (uint8 *)tx_buf, strlen((char const*)tx_buf),0);
        }else if(strcmp(name,"/Settings/")==0){
            memset(tx_buf,0,MAX_TX_URI_SIZE);
            make_response(page_setting);
            debug32("%d\n",strlen(page_setting));
            send(s, (uint8 *)tx_buf, strlen((char const*)tx_buf),0);
        }else if(strcmp(name,"/favicon.ico")==0){
            break;
        }
        break;     
    default :
      break;
  }
}

void init_http_server(void)
{
  uint8 DEFAULT_MAC[6] = {0x00, 0x08, 0xdc, 0x11, 0x12, 0x13};
  uint8 DEFAULT_LIP[4] = {192, 168, 10, 150};
  uint8 DEFAULT_SN[4] = {255, 255, 255, 0};
  uint8 DEFAULT_GW[4] = {192, 168, 10, 1}; 
  uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
  uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
  // memcpy(ConfigMsg.mac,DEFAULT_MAC,6);
  // memcpy(ConfigMsg.lip,DEFAULT_LIP,6);
  // memcpy(ConfigMsg.sub,DEFAULT_SN,6);
  // memcpy(ConfigMsg.gw,DEFAULT_GW,6);    
  setSHAR(DEFAULT_MAC);
  setSUBR(DEFAULT_SN);
  setGAR(DEFAULT_GW);
  setSIPR(DEFAULT_LIP);
  sysinit(txsize, rxsize); 
  debug32("ip=%d.%d.%d.%d\r\n",DEFAULT_LIP[0],DEFAULT_LIP[1],DEFAULT_LIP[2],DEFAULT_LIP[3]);
}



