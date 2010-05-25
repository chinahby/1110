/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded 
 *
 * This software is copyrighted by and is the sole property of 
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,  
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/


#ifndef __UTIL_H
#define __UTIL_H

#include "port.h"

#if F_LONGFILENAME
#include "util_lfn.h"
#else
#include "util_sfn.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*#define INTERNAL_MEMFN*/
#ifdef INTERNAL_MEMFN
#define _memcpy(d,s,l) _f_memcpy(d,s,l)
#define _memset(d,c,l) _f_memset(d,c,l)
#define _memcmp(s1,s2,l) _f_memcmp(s1,s2,l)
#else
#include <string.h>
#define _memcpy(d,s,l) memcpy(d,s,l)
#define _memset(d,c,l) memset(d,c,l)
#define _memcmp(s1,s2,l) memcmp(s1,s2,l)
#endif


unsigned short _f_getword(void *);
unsigned long _f_getlong(void *);
char _f_toupper(char); 
void _f_memset(void *,unsigned char,int);
void _f_memcpy(void *, void *, int);
int _f_memcmp(void *, void *, int);


#if F_WRITING
void _f_setword(void *,unsigned short);
void _f_setlong(void *,unsigned long);
#endif

#if (F_GETLABEL || F_SETLABEL || F_FORMATTING || F_FAT32)
unsigned char *_getchar(unsigned char *,int ,unsigned char *);
#endif

#if (F_FORMATTING & F_WRITING)
unsigned char *_setchar(unsigned char *,int,unsigned char *);
unsigned char *_setcharzero(int,unsigned char *);
unsigned char *_setchar(unsigned char *,int,unsigned char *);
unsigned char *_setword(unsigned short,unsigned char *);
unsigned char *_setlong(unsigned long,unsigned char *);
#endif

#ifdef __cplusplus
}
#endif

#endif

