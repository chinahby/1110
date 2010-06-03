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
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _JTYPES_H_
#define _JTYPES_H_

#include <jconfig.h>

/* Generic flags */
#define BUFFER_IS_DMA_ABLE      0x1

#ifndef CONFIG_T_TYPE_SUPPORT
typedef unsigned long           juint32_t;
typedef signed long             jint32_t;
typedef unsigned short          juint16_t;
typedef signed short            jint16_t;
typedef unsigned char           juint8_t;
/* HSU TempFix START */
/* This fix will be included in the next distribution of the stack.
** It is needed because in ARM the default of char is unsigned.
** It solves an issue in HID where negative logical_min values are not reported
** correctly.
*/
typedef signed char             jint8_t;
/* HSU TempFix END */
#endif

#ifndef CONFIG_UINT_T_TYPE_SUPPORT
typedef unsigned int            juint_t;
#endif
typedef signed int              jint_t;
typedef signed int              jresult_t;
typedef int                     jbool_t;

#ifndef NULL
#define NULL    ((void *) 0)
#endif

#ifndef MIN
# define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
# define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#ifndef ABS
# define ABS(a)  ((a) < 0 ? (-(a)) : (a))
#endif

#ifndef TRUE
# define TRUE    1
#endif

#ifndef FALSE
# define FALSE   0
#endif

/*
 * General byte order swapping functions.
 */
juint16_t __bswap16(juint16_t _x);
juint32_t __bswap32(juint32_t _x);

#define CPU_LITTLE_ENDIAN       1234 /* LSB first: i386, vax */
#define CPU_BIG_ENDIAN          4321 /* MSB first: 68000, ibm, net */
#define CPU_PDP_ENDIAN          3412 /* LSB first in word, MSW first in long */

#ifndef CONFIG_BYTE_ORDER
#error "CONFIG_BYTE_ORDER for this configuration is not specified !"
#endif 

#define bswap16(x)      __bswap16(x)
#define bswap32(x)      __bswap32(x)

/*
 *  Unaligned buffer access macros.
 *  
 *  The following macros perform ** BOTH ** unaligned access and
 *  endianity conversion where necessary.
 *  For unaligned access WITHOUT affecting endianity, use the
 *  ua_get() and ua_set() macros below.
 *  
 *  the 'le' and 'be' notation indicates the endianity of
 *  the BUFFER being READ from ('xxxtoh') or
 *  the BUFFER being WRITTEN to ('htoxxx').
 *  The 'h' (i.e. host order) is implied and is automatically
 *  adjusted for by the macros.
 *
 *  e.g, reading a uint16 from a little endian packed buffer
 *  use the ua_le16toh() macro. for writing a uint32 to a big
 *  endian packed buffer, use the ua_htobe32() macro.
 */

#ifdef CONFIG_ALIGN_CRITICAL

/*
 * Little endian unaligned buffers:
 * 
 * ua_leXXtoh - READ a value from an unaligned buffer. CONVERT to host order.
 * ua_htoleXX - CONVERT a value to little endian. WRITE to an unaligned buffer.
 *
 * NOTE: the endianity conversion to and from host order is automatic, i.e
 * CPU ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
 *
 */
#define ua_le16toh(ptr)   \
    (((juint8_t *)(ptr))[0] | (((juint8_t *)(ptr))[1] << 8))

#define ua_htole16(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)(v), \
    ((juint8_t *)(ptr))[1] = (juint8_t)((v) >> 8))

#define ua_le32toh(ptr) \
    ((juint32_t)(((juint8_t *)(ptr))[0]) | \
    ((juint32_t)(((juint8_t *)(ptr))[1]) << 8) | \
    ((juint32_t)(((juint8_t *)(ptr))[2]) << 16) | \
    ((juint32_t)(((juint8_t *)(ptr))[3]) << 24))

#define ua_htole32(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)(v), \
    ((juint8_t *)(ptr))[1] = (juint8_t)((v) >> 8), \
    ((juint8_t *)(ptr))[2] = (juint8_t)((v) >> 16), \
    ((juint8_t *)(ptr))[3] = (juint8_t)((v) >> 24))

/*
 * Big endian unaligned buffers:
 * 
 * ua_beXXtoh - READ a value from an unaligned buffer. CONVERT to host order.
 * ua_htobeXX - CONVERT a value to big endian. WRITE to an unaligned buffer.
 * 
 * NOTE: the endianity conversion to and from host order is automatic, i.e
 * CPU ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
 * 
 */
#define ua_be16toh(ptr)     \
    ((((juint8_t *)(ptr))[0] << 8) | ((juint8_t *)(ptr))[1])

#define ua_htobe16(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)((v) >> 8), \
    ((juint8_t *)(ptr))[1] = (juint8_t)(v))

#define ua_be32toh(ptr) \
    (((juint32_t)(((juint8_t *)(ptr))[0]) << 24) | \
    ((juint32_t)(((juint8_t *)(ptr))[1]) << 16) | \
    ((juint32_t)(((juint8_t *)(ptr))[2]) << 8) | \
    ((juint32_t)(((juint8_t *)(ptr))[3])))

#define ua_htobe32(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)((v) >> 24), \
    ((juint8_t *)(ptr))[1] = (juint8_t)((v) >> 16), \
    ((juint8_t *)(ptr))[2] = (juint8_t)((v) >> 8), \
    ((juint8_t *)(ptr))[3] = (juint8_t)(v))

#else

/* "Do nothing" macros */
#define ua_be16toh(ptr)   (*(juint16_t *)(ptr))
#define ua_htobe16(ptr,v) (*(juint16_t *)(ptr) = (v))
#define ua_be32toh(ptr)   (*(juint32_t *)(ptr))
#define ua_htobe32(ptr,v) (*(juint32_t *)(ptr) = (v))
#define ua_le16toh(ptr)   (*(juint16_t *)(ptr))
#define ua_htole16(ptr,v) (*(juint16_t *)(ptr) = (v))
#define ua_le32toh(ptr)   (*(juint32_t *)(ptr))
#define ua_htole32(ptr,v) (*(juint32_t *)(ptr) = (v))

#endif

/*
 * Host to big endian, host to little endian, big endian to host, and little
 * endian to host byte order functions as detailed in byteorder(9).
 */
#ifndef _SYS_BSDTYPES_H_
#if CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN

#define htobe16(x)      bswap16((x))
#define htobe32(x)      bswap32((x))
#define htole16(x)      ((juint16_t)(x))
#define htole32(x)      ((juint32_t)(x))

#define be16toh(x)      bswap16((x))
#define be32toh(x)      bswap32((x))
#define be64toh(x)      bswap64((x))
#define le16toh(x)      ((juint16_t)(x))
#define le32toh(x)      ((juint32_t)(x))

/* casting from 1 byte pointer in an alignment critical system */
#ifdef CONFIG_ALIGN_CRITICAL

/*
 * The following macros READ from and WRITE to unaligned buffers
 * WITHOUT affecting endianity
 *
 * ua_getXX - read a value from an unaligned buffer (endianity unaffected)
 * ua_setXX - write a value to an unaligned buffer (endianity unaffected)
 *
 * NOTE: Both CPU endianity and BUFFER endianity are IRRELEVANT.
 */
#define ua_get16(ptr)   ua_le16toh(ptr)
#define ua_get32(ptr)   ua_le32toh(ptr)
#define ua_set16(ptr,v) ua_htole16(ptr,v)
#define ua_set32(ptr,v) ua_htole32(ptr,v)

#endif

#elif CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN 

#define htobe16(x)      ((juint16_t)(x))
#define htobe32(x)      ((juint32_t)(x))
#define htole16(x)      ((juint16_t)((((x) >> 8) & 0xff) | ((x) & 0xff) << 8))
#define htole32(x)      bswap32((x))
#define htole64(x)      bswap64((x))

#define be16toh(x)      ((juint16_t)(x))
#define be32toh(x)      ((juint32_t)(x))
#define le16toh(x)      bswap16((x))
#define le32toh(x)      bswap32((x))
#define le64toh(x)      bswap64((x))

/* casting from 1 byte pointer in an alignment critical system */
#ifdef CONFIG_ALIGN_CRITICAL

/*
 * The following macros READ from and WRITE to unaligned buffers
 * WITHOUT affecting endianity
 *
 * ua_getXX - read a value from an unaligned buffer (endianity unaffected)
 * ua_setXX - write a value to an unaligned buffer (endianity unaffected)
 *
 * NOTE: Both CPU endianity and BUFFER endianity are IRRELEVANT.
 */
#define ua_get16(ptr)   ua_be16toh(ptr)
#define ua_get32(ptr)   ua_be32toh(ptr)
#define ua_set16(ptr,v) ua_htobe16(ptr,v)
#define ua_set32(ptr,v) ua_htobe32(ptr,v)

#endif

#else
#error "Invalid CONFIG_BYTE_ORDER specified !"
#endif

#endif 

#ifndef CONFIG_ALIGN_CRITICAL

#define ua_get16(ptr)     (*(juint16_t *)(ptr))
#define ua_set16(ptr,v)   (*(juint16_t *)(ptr) = (v))
#define ua_get32(ptr)     (*(juint32_t *)(ptr))
#define ua_set32(ptr,v)   (*(juint32_t *)(ptr) = (v))

/* Backward compatibility macros for host stack (TODO: REMOVE THEM) */
#define UGETW(ptr)        (*(juint16_t *)(ptr))
#define USETW(ptr,v)      (*(juint16_t *)(ptr) = (v))
#define UGETDW(ptr)       (*(juint32_t *)(ptr))
#define USETDW(ptr,v)     (*(juint32_t *)(ptr) = (v))

#else

/* Backward compatibility macros for host stack (TODO: REMOVE THEM) */
#define UGETW(ptr)        ua_le16toh(ptr)
#define USETW(ptr,v)      ua_htole16(ptr,v)
#define UGETDW(ptr)       ua_le32toh(ptr)
#define USETDW(ptr,v)     ua_htole32(ptr,v)

#endif

/* Convert value to (void *) */
#define htobe16_t(ptr, x) \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >>  0 & 0xff);

#define htobe32_t(ptr, x) \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >> 24 & 0xff); \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >> 16 & 0xff); \
        ((juint8_t *)(ptr))[2] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[3] = (juint8_t)((x) >>  0 & 0xff);

#define htole16_t(ptr, x) \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >>  0 & 0xff);

#define htole32_t(ptr, x) \
        ((juint8_t *)(ptr))[3] = (juint8_t)((x) >> 24 & 0xff); \
        ((juint8_t *)(ptr))[2] = (juint8_t)((x) >> 16 & 0xff); \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >>  0 & 0xff);

/* Convert (void *) to value */
#define be16toh_t(ptr) be16toh(ua_get16(ptr))
#define be32toh_t(ptr) be32toh(ua_get32(ptr))

#define le16toh_t(ptr) le16toh(*(juint16_t *)(ptr))
#define le32toh_t(ptr) le32toh(*(juint32_t *)(ptr))

/* Add "ms" milli-seconds to the time structure */
#define TIME_ADD_MS(time, ms) \
        do { \
        (time)->tv_usec += ms * 1000; \
        (time)->tv_sec  += ((time)->tv_usec / (1000 * 1000)); \
        (time)->tv_usec %= (1000 * 1000); \
        } while (0);

/* Check if time "a" is above time "b" */
#define TIME_OVER(a, b) \
        (((a).tv_sec > (b).tv_sec) || \
        (((a).tv_sec == (b).tv_sec) && \
        ((a).tv_usec >= (b).tv_usec)) ? \
        1 : 0)

/* Check if time "a" is above time "b" with millisecond resolution */
#define TIME_OVER_MS(a, b) \
        ((TIME_DIFF(a, b) > 0) ? 0 : 1) 

/* Return time difference between "start" and "end" in milli-seconds */
#define TIME_DIFF(start, end) \
        (((end).tv_sec - (start).tv_sec) * 1000 + \
        ((end).tv_usec - (start).tv_usec) / 1000)

#define TIME_GET_MS(time) \
        (((time).tv_sec) * 1000 + ((time).tv_usec) / 1000)
   
/* greater than */
#define jtime_gt(t1, t2) \
    ((t1)->tv_sec > (t2)->tv_sec || \
        ((t1)->tv_sec == (t2)->tv_sec && (t1)->tv_usec > (t2)->tv_usec))

/* less than */
#define jtime_lt(t1, t2) \
    ((t1)->tv_sec < (t2)->tv_sec || \
        ((t1)->tv_sec == (t2)->tv_sec && (t1)->tv_usec < (t2)->tv_usec))

#define jtime_egt(t1, t2) (!jtime_lt(t1, t2))
#define jtime_elt(t1, t2) (!jtime_gt(t1, t2))

#endif

