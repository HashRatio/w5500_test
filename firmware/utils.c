#include "utils.h"
#include "system_config.h"
#include "minilibc.h"
#include "defines.h"

void delay(unsigned int ms)
{
	unsigned int i;

	while (ms && ms--) {
		for (i = 0; i < CPU_FREQUENCY / 1000 / 5; i++)
			__asm__ __volatile__("nop");
	}
}

void delay_us(unsigned int us)
{
	unsigned int i;
	
	while (us && us--) {
		for (i = 0; i < CPU_FREQUENCY / 1000 / 1000 / 5; i++)
			__asm__ __volatile__("nop");
	}
}

/**
@brief	CONVERT STRING INTO INTEGER
@return	a integer number
*/
uint16 ATOI(
	char* str,	/**< is a pointer to convert */
	uint16 base	/**< is a base value (must be in the range 2 - 16) */
	)
{
  unsigned int num = 0;
  while (*str !=0)
          num = num * base + C2D(*str++);
  return num;
}

uint32 ATOI32(
	char* str,	/**< is a pointer to convert */
	uint16 base	/**< is a base value (must be in the range 2 - 16) */
	)
{
  uint32 num = 0;
  while (*str !=0)
          num = num * base + C2D(*str++);
  return num;
}

void itoa(uint16 n,uint8 str[5], uint8 len)
{
  
  uint8 i=len-1;

  memset(str,0x20,len);
  do{
  str[i--]=n%10+'0';
  
 }while((n/=10)>0);

 return;
}
/**
@brief	CONVERT STRING INTO HEX OR DECIMAL
@return	success - 1, fail - 0
*/
int ValidATOI(
	char* str, 	/**< is a pointer to string to be converted */
	int base, 	/**< is a base value (must be in the range 2 - 16) */
	int* ret		/**<  is a integer pointer to return */
	)
{
  int c;
  char* tstr = str;
  if(str == 0 || *str == '\0') return 0;
  while(*tstr != '\0')
  {
    c = C2D(*tstr);
    if( c >= 0 && c < base) tstr++;
    else    return 0;
  }
  
  *ret = ATOI(str,base);
  return 1;
}

/**
@brief	replace the specified character in a string with new character
*/ 
void replacetochar(
	char * str, 		/**< pointer to be replaced */
	char oldchar, 	/**< old character */
	char newchar	/**< new character */
	)
{
  int x;
  for (x = 0; str[x]; x++) 
    if (str[x] == oldchar) str[x] = newchar;	
}
/**
@brief	CONVERT CHAR INTO HEX
@return	HEX
  
This function converts HEX(0-F) to a character
*/
char C2D(
	uint8 c	/**< is a character('0'-'F') to convert to HEX */
	)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';

	return (char)c;
}

uint16 swaps(uint16 i)
{
  uint16 ret=0;
  ret = (i & 0xFF) << 8;
  ret |= ((i >> 8)& 0xFF);
  return ret;	
}

uint32 swapl(uint32 l)
{
  uint32 ret=0;
  ret = (l & 0xFF) << 24;
  ret |= ((l >> 8) & 0xFF) << 16;
  ret |= ((l >> 16) & 0xFF) << 8;
  ret |= ((l >> 24) & 0xFF);
  return ret;
}