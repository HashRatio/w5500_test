/**
 @file		httpd.c
 @brief 		functions associated http processing
 */

#include "minilibc.h"

#include "types.h"
#include "w5500.h"
#include "socket.h"
#include "utils.h"
#include "defines.h"

#define HTTPD_INCLUDE
#include "httpd.h"
#undef HTTPD_INCLUDE


extern char  homepage_default[];
char tx_buf[MAX_TX_URI_SIZE];
char rx_buf[MAX_RX_URI_SIZE];
extern uint8 BUFPUB[1024];
/**
 @brief	convert escape characters(%XX) to ascii charater 
 */ 
void unescape_http_url(
	char * url	/**< pointer to be conveted ( escape characters )*/
	)
{
  int x, y;
  
  for (x = 0, y = 0; url[y]; ++x, ++y) {
    if ((url[x] = url[y]) == '%') {
      url[x] = C2D(url[y+1])*0x10+C2D(url[y+2]);
      y+=2;
    }
  }
  url[x] = '\0';
}


/**
 @brief	make reponse header such as html, gif, jpeg,etc.
 */ 
void make_http_response_head(
	unsigned char * buf, 	/**< pointer to response header to be made */
	char type, 	/**< response type */
	uint32 len	/**< size of response header */
	)
{
  char * head = NULL;
  char tmp[10];   
  //20120504
  memset(buf,0x00,MAX_TX_URI_SIZE); 
  //debug32("make http response\r\n");
  //
  /*  file type*/
  if 	(type == PTYPE_HTML) head = RES_HTMLHEAD_OK;
  else if (type == PTYPE_GIF)	head = RES_GIFHEAD_OK;
  else if (type == PTYPE_TEXT)	head = RES_TEXTHEAD_OK;
  else if (type == PTYPE_JPEG)	head = RES_JPEGHEAD_OK;
  else if (type == PTYPE_FLASH)	head = RES_FLASHHEAD_OK;
  else if (type == PTYPE_MPEG)	head = RES_MPEGHEAD_OK;
  else if (type == PTYPE_PDF)	head = RES_PDFHEAD_OK;

  m_sprintf(tmp,"%ld", len);	
  strcpy((char*)buf, head);
  strcat((char*)buf, tmp);
  strcat((char*)buf, "\r\n\r\n");
  debug32("%s\r\n", buf);
}


/**
 @brief	find MIME type of a file
 */ 
void find_http_uri_type(
	uint8 * type, 	/**< type to be returned */
	char * buf		/**< file name */
	) 
{
  /* Decide type according to extention*/
  if 	(strstr(buf, ".pl"))				*type = PTYPE_PL;
  else if (strstr(buf, ".html") || strstr(buf,".htm"))	*type = PTYPE_HTML;
  else if (strstr(buf, ".gif"))				*type = PTYPE_GIF;
  else if (strstr(buf, ".text") || strstr(buf,".txt"))	*type = PTYPE_TEXT;
  else if (strstr(buf, ".jpeg") || strstr(buf,".jpg"))	*type = PTYPE_JPEG;
  else if (strstr(buf, ".swf")) 				*type = PTYPE_FLASH;
  else if (strstr(buf, ".mpeg") || strstr(buf,".mpg"))	*type = PTYPE_MPEG;
  else if (strstr(buf, ".pdf")) 				*type = PTYPE_PDF;
  else if (strstr(buf, ".cgi") || strstr(buf,".CGI"))	*type = PTYPE_CGI;
  else if (strstr(buf, ".js") || strstr(buf,".JS"))	*type = PTYPE_TEXT;	
  else if (strstr(buf, ".xml") || strstr(buf,".XML"))	*type = PTYPE_HTML;
  else 							*type = PTYPE_ERR;
}


/**
 @brief	parse http request from a peer
 */ 
void parse_http_request(
	st_http_request * request, 	/**< request to be returned */
	uint8 * buf				/**< pointer to be parsed */
	)
{
  char * nexttok;
  nexttok = strtok((char*)buf," ");

  if(!strcmp(nexttok, "GET") || !strcmp(nexttok,"get"))
  {
    request->METHOD = METHOD_GET;
    nexttok = strtok(NULL," ");
   			
  }
  else if (!strcmp(nexttok, "HEAD") || !strcmp(nexttok,"head"))	
  {
    request->METHOD = METHOD_HEAD;
    nexttok = strtok(NULL," ");
  		
  }
  else if (!strcmp(nexttok, "POST") || !strcmp(nexttok,"post"))
  {
    nexttok = strtok(NULL,"\0");//20120316
    //nexttok = strtok(NULL," ");
    request->METHOD = METHOD_POST;
 		
  }
  else
  {
    request->METHOD = METHOD_ERR;
  }	
  
  if(!nexttok)
  {
    request->METHOD = METHOD_ERR;
  			
    return;
  }
  //strcpy(request->URI,nexttok);
  request->URI = nexttok;
                
	
}


/**
 @brief	get next parameter value in the request
 */ 
unsigned char* get_http_param_value(char* uri, char* param_name)
{
  uint8 * name=0; 
  uint8 *ret=BUFPUB;
  uint16 content_len=0;
  int8 tmp_buf[10]={0x00,};
  if(!uri || !param_name) return 0;
  /***************/
  mid(uri,"Content-Length: ","\r\n",tmp_buf);
  content_len=ATOI(tmp_buf,10);
  //debug32("content len=%d\r\n",content_len);
  uri = (int8*)strstr(uri,"\r\n\r\n");
  uri+=4;
  //debug32("uri=%s\r\n",uri);
  uri[content_len]=0;
  /***************/
  if((name = (uint8*)strstr(uri,param_name)))
  {
    name += strlen(param_name) + 1; 
    uint8* pos2=(uint8*)strstr((char*)name,"&");
    if(!pos2) 
    {
      pos2=name+strlen((char*)name);
    }
    uint16 len=0;
    len = pos2-name;
 
    if(len)
    {
      ret[len]=0;
      strncpy((char*)ret,(char*)name,len);
      unescape_http_url((char *)ret);
      replacetochar((char *)ret,'+',' ');
      //ret[len]=0;
      //ret[strlen((int8*)ret)]=0;
      //debug32("len=%d\r\n",len);
    }
    else
      ret[0]=0;
  }
  else
    return 0;
  return ret;		
}