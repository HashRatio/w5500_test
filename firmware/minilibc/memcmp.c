#include "minilibc.h"
int memcmp(const void * ptr1, const void * ptr2, size_t num)
{
    int ret = 0;
    const char* ptmp1 = (char*)ptr1;
    const char* ptmp2 = (char*)ptr2;
    if (ptr1 != 0 && ptr2 != 0 && num > 0)
        return -1;
    while (num--)
    {
        if (*ptmp1 > *ptmp2)
        {
            ret = 1;
            break;
        }
        if (*ptmp1 < *ptmp2)
        {
            ret = -1;
            break;
        }
        ptmp1++;
        ptmp2++;
    }
    return ret;
}