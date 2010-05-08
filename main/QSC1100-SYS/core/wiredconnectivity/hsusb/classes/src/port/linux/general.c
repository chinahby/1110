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
void dump_stack(void);

void os_printf(const char *fmt, ...)
{
    va_list args;
    char buf[1024]; 
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = 0;
    printk("%s", buf);
}

void os_panic(const char *fmt, ...)
{
    va_list args;
    char buf[1024];
    dump_stack();
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf)-1] = 0;
    os_printf("panic - %s\n", buf);
    panic(buf);
}

#ifdef JDEBUG
void jmicrotime(os_time_t *tvp)
{
    do_gettimeofday((struct timeval *)tvp);
}

void os_dprintf(const char *fmt, ...)
{
    va_list args;
    char buf[1024]; /* TODO: maybe optimize this. printk uses 1024 bytes buf */
    
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = 0;

    os_printf("  ");

#ifdef DEBUG_WITH_TIMERS
    os_printf("%3ld ms:  %s", dbg_get_time_diff(), buf);
#else
    os_printf("%s", buf);
#endif
}
#endif

