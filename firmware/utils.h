#ifndef _UTILS_H
#define _UTILS_H

#include "types.h"

void delay(unsigned int ms);
void delay_us(unsigned int us);
extern uint16 ATOI(char* str,uint16 base);
uint32 ATOI32(char* str,uint16 base);
void itoa(uint16 n,uint8 str[5], uint8 len);
int ValidATOI(char* str,int base,int* ret);
void replacetochar(char * str,char oldchar,char newchar);
char C2D(uint8 c);
uint16 swaps(uint16 i);
uint32 swapl(uint32 l);
//void shift_32bytes(uint8 * target);
#endif
