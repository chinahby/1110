#ifndef __MP4LIB_BASEDEF_INCLUDED__
#define __MP4LIB_BASEDEF_INCLUDED__

// the  mpeg4 demux mux version are always in this file 
#ifndef __MP4LIB_VERSION__
#define __MP4LIB_VERSION__

#define MPEG4LIB_PACKAGE 			"mpeg4_demux_mux"
#define MPEG4LIB_VERSION 			"2.2.00"
#define MPEG4LIB_MAJOR_VERSION 	0x2
#define MPEG4LIB_MINOR_VERSION 	0x2
#define MPEG4LIB_CVS_VERSION 		0x00
#define MPEG4LIB_HEX_VERSION 		((MPEG4LIB_MAJOR_VERSION << 16) | (MPEG4LIB_MINOR_VERSION << 8) | MPEG4IP_CVS_VERSION)

#endif /* __MPEG4IP_VERSION__ */

// the DATA TYPE define are always in this file 
#ifndef __MP4LIB_DATATYPE__
#define __MP4LIB_DATATYPE__

#ifdef _WIN32	/**/

#define _WIN32_WINNT 			0x0400
#define _WINSOCKAPI_
#define _INTEGRAL_MAX_BITS 	64

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

typedef __int64 			int64;
typedef long 				int32;
typedef short 			int16;
typedef char 				int8;
typedef unsigned __int64 	uint64;
typedef unsigned int 		uint32;
typedef unsigned short 	uint16;
typedef unsigned char 		uint8;
typedef unsigned char 		uchar;

#define UINT64_TO_DOUBLE(a) 	((double)((int64)(a)))

#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

typedef long long 			int64;
typedef int 				int32;
typedef short 			int16;
typedef char 				int8;
typedef unsigned long long 	uint64;
typedef unsigned int 		uint32;
typedef unsigned short 	uint16;
typedef unsigned char 		uint8;
typedef unsigned char 		uchar;

#endif /* define other  */

#endif /* __MP4LIB_DATATYPE__ */



/*****************************************************************************
 *             Generic type includes used in the whole package               					  *
 *****************************************************************************/
#define B2N_16(x) \
 x = ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))

#define B2N_32(x) \
 x = ((((x) & 0xff000000) >> 24) | \
          (((x) & 0x00ff0000) >>  8) | \
          (((x) & 0x0000ff00) <<  8) | \
          (((x) & 0x000000ff) << 24))

#define B2N_64(x) \
 x = ((((x) & 0xff00000000000000) >> 56) | \
         (((x) & 0x00ff000000000000) >> 40) | \
         (((x) & 0x0000ff0000000000) >> 24) | \
         (((x) & 0x000000ff00000000) >>  8) | \
         (((x) & 0x00000000ff000000) <<  8) | \
         (((x) & 0x0000000000ff0000) << 24) | \
         (((x) & 0x000000000000ff00) << 40) | \
         (((x) & 0x00000000000000ff) << 56))

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef ROUND
#define ROUND(f) (int)(floor((f) + 0.5))
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef INT16_MAX
#define INT16_MAX (32767)
#endif

#ifndef INT16_MIN 
#define INT16_MIN (-32767-1)
#endif 

#ifndef UINT32_MAX
#define UINT32_MAX  (4294967295U)
#endif

#endif /* __MP4LIB_IP_INCLUDED__ */

