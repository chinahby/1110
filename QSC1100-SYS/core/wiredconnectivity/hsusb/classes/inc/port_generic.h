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
#ifndef _PORT_GENERIC_H_
#define _PORT_GENERIC_H_

#include "jtypes.h"

void os_printf(const char *fmt, ...);
void os_panic(const char *fmt, ...);
void os_dprintf(const char *fmt, ...);
#ifdef JDEBUG
jresult_t os_print_stack(void);
#endif

jint_t os_memcmp(const void *s1, const void *s2, juint32_t n);
juint32_t os_strlcpy(char *dst, const char *src, juint32_t n);
jint32_t os_snprintf(char *buf, juint32_t size, const char *fmt, ...);
jint_t jos_strcmp(const char *s1, const char *s2);
jint_t jos_strncmp(const char *s1, const char *s2, const juint32_t n);
void *jos_memset(void *b, jint_t c, juint32_t len);
void *jos_memcpy(void *out, const void *in, juint32_t n);
juint32_t jos_strlen(const char *str);

jresult_t os_port_init(void);
void os_port_uninit(void);

#endif

