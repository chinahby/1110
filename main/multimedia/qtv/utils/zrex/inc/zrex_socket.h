/* =======================================================================
                              zrex_socket.h
DESCRIPTION
  Substitutions for Windows system calls and runtime library routines.  
  For use with the ZREX versions of the core libraries.
  
EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/inc/zrex_socket.h#8 $
$DateTime: 2009/01/29 06:14:06 $
$Change: 829425 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef _ZREX_SOCKET_H_
#define _ZREX_SOCKET_H_


/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "zrex_os.h"
#ifndef FEATURE_WINCE
#include "dsbsd.h"
#else
#error code not present
#endif



/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif


/*
//////////////////////////////////////////////////////////////////////////////////
//
// Windows Socket defs.
//
//////////////////////////////////////////////////////////////////////////////////
*/

/*
** Redefine windows stuff to ZREX.
*/

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
#ifndef FEATURE_WINCE
typedef int SOCKET;
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#ifndef FEATURE_DS_SOCKETS_BSD

#ifdef AF_INET
#undef AF_INET
	#define AF_INET			2
#endif

#ifdef SOCK_STREAM
#undef SOCK_STREAM
   #define SOCK_STREAM		1
#endif

#ifdef SOCK_DGRAM
#undef SOCK_DGRAM
   #define SOCK_DGRAM		2
#endif

#ifdef SOCK_RAW
#undef SOCK_RAW
   #define SOCK_RAW			3
#endif

#endif

#ifndef INADDR_NONE
#define INADDR_NONE 0xFFFFFFFFUL
#endif

#define INVALID_SOCKET  (SOCKET)(~0)
#ifndef FEATURE_WINCE
#define SOCKET_ERROR            (-1)
#define WSAEWOULDBLOCK			EWOULDBLOCK
#define WSAEMSGSIZE				DS_EMSGSIZE
#endif

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
// Functions that do not have a zrex counterpart.
#define shutdown(s, how)

/*
** redefine windows calls to zrex calls.
*/
#define WSAStartup(a, b) zrex_bsd_init()
#define WSACleanup() zrex_bsd_shutdown(rex_self())
#define WSAGetLastError() errno
#define WSASetLastError(x) errno = x
#define ioctlsocket(a, b, c) ioctl(a, b, (char *) c)
#define closesocket(s) zrex_close(s)

//tbd.  these won't compile with current target include files.
// it may eventually be OK once sockets are implemented for
//the target.
#ifdef T_WINNT
#error code not present
#endif //T_WINNT

// -RH temporary defines!
#define RTP_PORT  64000
#define RTCP_PORT 64001

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
struct hostent *gethostbyname(const char * name);



#ifdef __cplusplus
}
#endif

#endif //_ZREX_SOCKET_H_

