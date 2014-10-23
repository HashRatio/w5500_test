#include "minilibc.h"
char *strncpy(char *dst, const char *src, size_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                /* NUL pad the remaining n-1 bytes */
                while (--n)
                {
                    *d++ = 0;
                }
                break;
            }
        }
        while (--n);
    }
    return dst;
}