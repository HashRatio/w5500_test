#include "minilibc.h"

char *
strtok(char *s, const char *delim)
{
    char *spanp;
    int c, sc;
    char *tok;
    static char *last;


    if (s == 0 && (s = last) == 0)
        return (0);

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;)
    {
        if (c == sc)
            goto cont;
    }

    if (c == 0)         /* no non-delimiter characters */
    {
        last = 0;
        return (0);
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;)
    {
        c = *s++;
        spanp = (char *)delim;
        do
        {
            if ((sc = *spanp++) == c)
            {
                if (c == 0)
                    s = 0;
                else
                    s[-1] = 0;
                last = s;
                return (tok);
            }
        }
        while (sc != 0);
    }
    /* NOTREACHED */
}