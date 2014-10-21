#ifndef MINILIBC_H_
#define MINILIBC_H_

typedef unsigned int size_t;

void *memcpy(void *restrict dst, const void *restrict src, int n);
void *memset(void *dst, int c, size_t n);
int strncmp(register const char *s1, register const char *s2, register size_t n);

char *m_sprintf(char *dest, const char *format, ...); /* Return dest not length of the string */
int memcmp ( const void * ptr1, const void * ptr2, size_t num );
size_t strlen(const char *str);
char *strchr(const char *p, int ch);
char *strncpy(char *dst, const char *src, size_t n);
char *strcpy(char *dest, const char *src);
char * strcat(char *s1, const char *s2);
char * strstr(const char *s, const char *find);
char * strtok(char *s, const char *delim);
int strcmp(const char *s1, const char *s2);
#endif
