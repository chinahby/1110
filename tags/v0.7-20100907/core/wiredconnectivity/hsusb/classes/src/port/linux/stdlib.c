/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include "includes.h"
#include <port.h>

jint_t os_memcmp(const void *s1, const void *s2, juint32_t n)
{
    return memcmp(s1, s2, n);
}

void* jos_memcpy(void *out, const void *in, juint32_t n)
{
    return (void*)memcpy(out, in, n);
}

void *jos_memset(void *b, int c, juint32_t len)
{
    return (void*)memset(b, c, len);
}

juint32_t jos_strlen(const char *str)
{
    return strlen(str);
}

jint_t jos_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

jint_t jos_strncmp(const char *s1, const char *s2, const juint32_t n)
{
    return strncmp(s1, s2, n);
}

juint32_t os_strlcpy(char *dst, const char *src, juint32_t n)
{
    char *d = dst;
    const char *s = src;

    if (n)
    {
        /* Copy as many bytes as will fit */
        for (; n; n--)
        {
            if ((*d++ = *s++) == 0)
                break;
        }

        /* Not enough room in dst, add NULL */
        if (n == 0)
            *(d-1) = '\0';
    }
    return strlen(src);
}

jint32_t os_snprintf(char *buf, juint32_t size, const char *fmt, ...)
{
    va_list args;
    jint32_t rc;
    
    va_start(args, fmt);
    rc = vsnprintf(buf, size, fmt, args);
    va_end(args);
    
    return rc;
}

