#ifndef _DSSOCKET_H
#define _DSSOCKET_H

#ifdef FEATURE_DS_SOCKETS
/*===========================================================================

   D A T A   S E R V I C E S   S O C K E T   A P I   H E A D E R   F I L E
                   
DESCRIPTION

 The Data Services Socket Header File. Contains shared variables and enums, 
 as well as declarations for functions.

Copyright (c) 1998-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssocket.h_v   1.13   27 Feb 2003 12:01:52   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssocket.h#5 $ $DateTime: 2009/02/02 05:33:24 $ $Author: spathuru $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/02/09    sp     Reducing the MAX number of TCP/UDP sockets if
                   FEATURE_LOW_MEMORY_USAGE is defined.
06/07/08    pp     OMH 1b Profile Arbitration development.
06/14/06    gr/sk  Merged featurization changes to support Low Memory usages
07/02/04    vp     Added DSS_WRITE_MAX_BYTES constant declaration.
05/17/04    sv     Added DSS_READ_MAX_BYTES constant declaration.
05/17/04    vp     Under FEATURE_IS2000_REL_A, DSS_MAX_SNDBUF and 
                   DSS_MAX_RCVBUF defined to be 36 KB.
04/22/04    sv     Increased the maximum number of applications, TCP sockets
                   and UDP sockets.
12/01/03    aku    Increased default SNDBUF size from 5K to 10K
11/05/03    sv     Added default SNDBUF size declaration.
08/04/03    sv     Added SO_KEEPALIVE and TCP_NODELAY socket options.
07/29/03    ss     Moved declarations used externally and internally both to 
                   dssocket_defs.h. Changed signature of sdb ack cb function. 
07/08/03    ss     Made changes to support SDB ack callback socket option.
07/07/03    atp    Renamed SDB flags as part of SDB enhancements.
07/06/03    sv     Added DSS_SO_SNDBUF socket option to socket option enum 
                   declaration.
05/27/03    om     Added ESP protocol ID.
05/12/03    ifk    Added IPv6 address and protocol family macros, struct
                   in6_addr.
05/05/03    jd     Removed dependency on dsiface.h
05/04/03    ss     Added DSS_MAX_SYS_APP to support SO_LINGER socket option 
04/30/03    aku    Removed unused struct dss_iface_name_and_instance_type.
                   Moved dss_get_iface_id() to dss_iface_ioctl.h
04/23/03    ss     Added SO_LINGER related declarations
04/10/03    ss     Moved dss_iface_status_type typedef to dssnet.h
02/21/03    usb    Modified deccription for dss_force_dormancy().
02/13/03    ss     Added new functions: getsockname, getpeername and shutdown
01/24/03    usb    Renamed dss_get_iface_name_and_instance() to 
                   dss_get_iface_id() and updated the prototype to use same 
                   iface_id as passed in dss_iface_ioctl()
12/23/02    aku    Added dss_get_iface_name_and_instance().
12/04/02    mvl    Fixed typo
11/26/02    usb    Increased DSS_MAX_APPS value.
11/19/02    mvl    removed JCDMA from window size socket option.
11/12/02    mvl    Added support for changing default window size.
11/11/02    aku    Include header file dss_iface_ioctl.h
11/08/02    ak     Do not include dssdns.h
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/27/02    ss     Moved the socket option related declarations from 
                   dssinternal.h to this file
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/23/02    aku    Added featurization for #define for dss_get_iface_state
07/22/02    aku    Changed #define for dss_get_iface_status to 
                   dssnet_get_iface_status from dsiface_get_status
07/15/02    aku    Include dsmip.h only if FEATURE_DS_MOBILE_IP is defined.
07/10/02    jd     featurized inclusion of dsmip.h under FEATURE_DS_MOBILE_IP
                   - otherwise, define MIP_MAX_SESSIONS=0 (for max # sockets)
07/01/02    ak     Now include dserrno.h.
05/02/02    mvl    Added definition of some flags for sendto() call.
06/06/02    jd     increased DSS_MAX_APPS to 4 so tcp test can open netlib
04/08/02    ss     Added code DS_EMSGTRUNC to indicate trucation of datagram
02/22/02    pjb    Added accept and listen
01/30/02    atp    Added API for fast dormancy (disabling dormant timer).
01/25/02    ss     Moved the DNS related declarations to dssdns.h. Included
                   dssdns.h in this file.
11/29/01    mvl    redefinedd dss_get_iface_status() as a macro - so now it
                   calls dsiface_get_status().  Moved all of the definitions
                   for that call to dsiface_def.h.  General clean up.
11/11/01    aku    Added new enum type for ppp_close_reason
09/16/01    ss     Made macro DSS_MAX_UDP_SOCKS dependent on the number of
                   MIP sessions and max number of application sockets
08/23/01    aku    Renamed dss_getifacestatus as dss_get_iface_status
08/14/01    aku    Added wrapper function dss_getifacestatus and function
                   dss_force_dormancy
07/26/01    aku    Added new interface to get iface status 
                   dss_getifacestatus
07/20/01    mt     Add DNS-related error codes and structure, add
                   dss_gethostbyname() declaration.
07/16/01    mt     Correct value of SOCKFDBASE_MAX_VAL, fix up comment lines.
05/23/01    mt     Added DS_EINTR.
03/29/01    js     Moved function prototypes of dss_getsockopt() and
                   dss_setsockopt() to dssinternal.h
03/29/01    js     Moved definitions related to socket options to 
                   dssinternal.h
03/27/01    mt     Changed buffer argument types in dss_write(), dss_read(),
                   dss_sendto(), dss_recvfrom().
03/27/01    mvl    Changed all instances of errno to dss_errno, incorporated
                   the definitiaions of base and max sockfd values, also
                   added macro to convert from sockfd to scb index.  Added
                   DS_ENOMEM errno value.
03/27/01    js     Introduced DSS_MAX_ICMP_SOCKS, and hence incresed the
                   value of DSS_MAX_SOCKS.
03/27/01    js     Added support for ICMP sockets.
03/27/01    js     Changed the return type from sint15 from int for
                   dss_getsockopt() and dss_setsockopt().
02/14/01    js     Added socket options support. 
02/14/01    js     Added the definition for DS_EMSGSIZE. 
10/30/00    snn    Renamed MAX_SOCKS as DSS_MAX_SOCKS and MAX_APPS as 
                   DSS_MAX_APPS.
10/10/00    snn    Added MAX_SOCKS, DSS_MAX_TCP_SOCKS and DSS_MAX_UDP_SOCKS 
                   defines.
09/08/00    ttl    Added DNS lookup messages.
09/05/00    snn    Added DNS support
04/21/99    hcg    Removed unused error definitions.
04/16/99    hcg    Updated incorrect comment for dss_socket().
04/01/99    hcg    Replaced functions with macros to perform byte-ordering 
                   for host/network conversions.  Removed the external 
                   function declarations for the removed functions.  
                   Incorporated code review changes/comments.
10/05/98  na/hcg   Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "psglobal.h"

#include "dsiface.h"
#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip.h"
#else
#define MIP_MAX_SESSIONS 0
#endif /* FEATURE_DS_MOBILE_IP */

#include "dserrno.h"
#include "dss_iface_ioctl.h"
#include "dssocket_defs.h"
#include "dss_netpolicy.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*
 * To reflect any future changes in the DSS APIs, the DSS_VERSION has to be 
 * incremented sequentially and is to be in the range [20, 29].
 */
#define DSS_VERSION 20

/*---------------------------------------------------------------------------
                          Address/Protocol Family
---------------------------------------------------------------------------*/
#define PF_INET         0                  /* Protocol Family - Internet   */
#define PF_INET6        2                  /* Protocol Family - Internet v6*/

#define AF_INET         PF_INET            /* Address Family - Internet    */
#define AF_INET6        PF_INET6           /* Address Family - Internet v6 */

#ifdef INADDR_ANY                  /* safeguard against previously defined */
#undef INADDR_ANY
#endif

#define INADDR_ANY      0x0L                  /* Local IP wildcard address */
/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define DSS_SUCCESS         0                      /* successful operation */
#define DSS_ERROR          -1                    /* unsuccessful operation */

/*---------------------------------------------------------------------------
      IP protocol numbers - use in dss_socket() to identify protocols.
---------------------------------------------------------------------------*/
#define IPPROTO_ICMP   1                                   /* ICMP protocol*/
#define IPPROTO_TCP    6                                   /* TCP Protocol */
#define IPPROTO_UDP   17                                   /* UDP Protocol */
#define IPPROTO_ESP   50                                   /* ESP Protocol */

/*---------------------------------------------------------------------------
                             Socket types
---------------------------------------------------------------------------*/
#define SOCK_STREAM     0                        /* TCP - streaming socket */
#define SOCK_DGRAM      1                         /* UDP - datagram socket */
#define SOCK_ICMP       2                       /* ICMP   TBC              */

/*---------------------------------------------------------------------------
                      Asynchronous Socket Events
---------------------------------------------------------------------------*/
#define DS_WRITE_EVENT  0x01         /* associated with a writeable socket */
#define DS_READ_EVENT   0x02          /* associated with a readable socket */
#define DS_CLOSE_EVENT  0x04         /* associated with a closeable socket */
#define DS_ACCEPT_EVENT 0x08        /* associated with an accetable socket */

/*---------------------------------------------------------------------------
                     Specify the number of applications
---------------------------------------------------------------------------*/
#define DSS_MAX_SYS_APPS     1 /* max no of sys apps, 1 for lingering sock */

#define DSS_MAX_APPS         (25 + DSS_MAX_SYS_APPS)      /* max no of apps */


/*---------------------------------------------------------------------------
  Constants to be defined for Multiple Sockets
    SOMAXCONN is the maximum number of unopened or half opened tcp sockets
        we will allow at any one time. 
    MAX_TCP_SOCKS is the maximum number of tcp sockets total.  Each listening 
        socket will use at least 2 of these, and will need backlog+1 
        available for the listen call to succeed. 
---------------------------------------------------------------------------*/

#define DSS_SOMAXCONN        (3)       /* Maximum value for listen backlog */


#define DSS_MAX_TCP_SOCKS    (20)            /* Maximum TCP sockets allowed */


/* Maximum number  of UDP sockets available to applications */

#define DSS_MAX_UDP_APP_SOCKS 10


/*-------------------------------------------------------------------------
  Maximum number of sockets available system wide -- application sockets 
  plus one for each MIP session 
--------------------------------------------------------------------------*/

#define DSS_MAX_UDP_SOCKS    (DSS_MAX_UDP_APP_SOCKS + MIP_MAX_SESSIONS) 

/* Maximum ICMP sockets allowed */
#define DSS_MAX_ICMP_SOCKS   (2)

#define DSS_MAX_SOCKS       (DSS_MAX_TCP_SOCKS + DSS_MAX_UDP_SOCKS + \
                             DSS_MAX_ICMP_SOCKS)

/*---------------------------------------------------------------------------
  Ranges of sockfd and sockfdbase:
  NOTE THAT THE  SOCKFDBASE_MIN_VAL SHOULD NOT EQUAL ZERO.
  Note these should be within range of sockfdbase.
---------------------------------------------------------------------------*/
#define SOCKFDBASE_MIN_VAL  DSS_MAX_SOCKS
#define SOCKFDBASE_MAX_VAL  ((1<<15) - 1 - DSS_MAX_SOCKS)


/*---------------------------------------------------------------------------
  Maximum bytes you can read using a single read call.
---------------------------------------------------------------------------*/
#define DSS_READ_MAX_BYTES ( (1 << 15) - 1) 
#define DSS_WRITE_MAX_BYTES ( (1 << 15) - 1) 

/*---------------------------------------------------------------------------
  Range values for various socket options
---------------------------------------------------------------------------*/
#define DSS_MIN_RCVBUF (536)
#ifndef FEATURE_IS2000_REL_A
  #define DSS_MAX_RCVBUF (18 * 1024)
#else
  #define DSS_MAX_RCVBUF (36 * 1024)
#endif /* FEATURE_IS2000_REL_A */

#define DSS_MIN_SNDBUF (536)
#define DSS_DEF_SNDBUF (10 * 1024)
#ifndef FEATURE_IS2000_REL_A
  #define DSS_MAX_SNDBUF (18 * 1024)
#else
  #define DSS_MAX_SNDBUF (36 * 1024)
#endif /* FEATURE_IS2000_REL_A */

/*---------------------------------------------------------------------------
  Macro to convert from the socket file descriptor to a unique index.
---------------------------------------------------------------------------*/
#define SOCKFD_2_INDEX(fd) (((fd) - SOCKFDBASE_MIN_VAL) % DSS_MAX_SOCKS)

/*===========================================================================
  Define flags passed to dss_sendto() calls here. Any time a new flag is to 
  be added, define it here and OR it with the existing flags in
  MSG_VALID_BITS macro definition in dssocki.c so that the reserved bit
  mask can be properly built.
  
  Note: Refer to the SDB related flags defined in dssocket_defs.h when 
        adding a new flag to avoid assigning the same value.
===========================================================================*/

/*===========================================================================
                          Socket Address Structures
===========================================================================*/
/*---------------------------------------------------------------------------
  The socket address structures follow the BSD convention, including data
  types, etc.  These are the BSD generic address structures needed to
  support Internet-family addressing.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                     BSD Generic Socket Address structure
---------------------------------------------------------------------------*/
struct sockaddr                        /* generic socket address structure */
{
  uint16 sa_family;                               /* socket address family */
  unsigned char   sa_data[14];                             /* address data */
};

/*---------------------------------------------------------------------------
               Internet-family specific host internet address
---------------------------------------------------------------------------*/
struct in_addr                       /* structure for "historical" reasons */
{
  uint32 s_addr;                                         /* socket address */
};

/* IPv6 address structure */
struct in6_addr
{
  uint32 s_addr[4];                                       /* socket address */
};

/*---------------------------------------------------------------------------
                  Internet-family Socket Address Structures
---------------------------------------------------------------------------*/
struct sockaddr_in                        /* Internet style socket address */
{
  uint16 sin_family;                             /* internet socket family */
  uint16 sin_port;                                 /* internet socket port */
  struct in_addr sin_addr;                      /* internet socket address */
  char sin_zero[8];              /* zero'ed out data for this address type */
};

/*---------------------------------------------------------------------------
                       Non-Contiguous Buffer Structure
---------------------------------------------------------------------------*/
struct iovec
{
  byte *iov_base;                            /* starting address of buffer */
  uint16 iov_len;                           /* size of the buffer in bytes */
};

/*---------------------------------------------------------------------------
                       Socket Options Data Types
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Definition for various options that affect the behaviors of a socket. 
  Only the options whose names are defined here are supported. The 
  following definition is also used as index to the table of all supported 
  options.

  +----------------------+-----------+--------------+-----------+-----------+
  |  Name                |   type    |  default val |  min val  |  max val  |
  +----------------------+-----------+--------------+-----------+-----------+
  |  DSS_IP_TTL          |    int    |     255      |     0     |    255    |
  |  DSS_SO_SYS_SOCK     |   bool    |    False     |    N/A    |    N/A    |
  |  DSS_SO_SILENT_CLOSE |   bool    |    False     |    N/A    |    N/A    |
  |  DSS_SO_RCVBUF **    |    int    |     2144     |   2144    |   17520   |
  |  DSS_SO_LINGER       |  dss_so_  |    {0,0}     |   {0,0}   |  {1, N/A} |
  |                      |linger_type|              |           |           |
  |  DSS_SO_SNDBUF       |    int    |     2144     |    536    |   18432   |
  +----------------------+-----------+--------------+-----------+-----------+

  ** NOTE: the use of this option REQUIRES that more memory be added to the
           DSM item pool.  For each socket that expands its window the
           appropriate memory needs to be added.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_SOCKOPT_MIN     = -1,        /* lower bound                          */
  DSS_IP_TTL          =  0,        /* time-to-live                         */
  DSS_SO_SYS_SOCK     =  1,        /* bool: is this a system socket?       */
  DSS_SO_SILENT_CLOSE =  2,        /* bool: close() call causes conn reset */
  DSS_SO_RCVBUF       =  3,        /* set the receive window size          */
  DSS_SO_LINGER       =  4,        /* linger on close                      */
  DSS_SO_SNDBUF       =  5,        /* set/get the sndbuf queue size        */
  DSS_SO_SDB_ACK_CB   =  6,        /* call a cb upon recv'ing SDB data ack */
  DSS_SO_KEEPALIVE    =  7,        /* Periodically test if the connection  */ 
                                   /* is still alive                       */
  DSS_TCP_NODELAY     =  8,        /* Disable Nagle's algorithm            */
  DSS_SOCKOPT_MAX                  /* determine upper bound and array size */
} dss_sockopt_names_type;

/*---------------------------------------------------------------------------
  Socket option level specifies the code in the system to interpret the 
  option: the general socket code or some protocol-specific code.  If
  an option is unknown at a given level, an error will be generated. 
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IPPROTO_IP = 1,               /* IP protocol level                   */
  DSS_SOL_SOCKET = 2,               /* socket level                        */
  DSS_SOCK = DSS_SOL_SOCKET,        /* another alias for socket level      */
  DSS_IPPROTO_TCP = 3               /* TCP protoocl level                  */
} dss_sockopt_levels_type;

/*---------------------------------------------------------------------------
  The data type used for SO_LINGER socket option. Note that l_linger is in
  ms, not in seconds as on UNIX systems
---------------------------------------------------------------------------*/
typedef struct
{
  int l_onoff;                                     /* linger active or not */
  int l_linger;                    /* how many milli-seconds to linger for */
} dss_so_linger_type;

/*---------------------------------------------------------------------------
  The pointer to function data type used for registering callback for the
  SO_SDB_ACK_CB socket option.
---------------------------------------------------------------------------*/
typedef 
void (*dss_so_sdb_ack_cb_fcn) 
(
  sint15 sockfd,                                     /* socket descriptor  */  
  dss_sdb_ack_status_info_type* sdb_ack_info,    /* info about the sdb ack */
  void*  user_data    /* supplied at the time of setting the socket option */
);

/*---------------------------------------------------------------------------
  The data type used for SO_SDB_ACK_CB socket option. The caller supplies 
  user_data when setting the socket option and it is passed to the 
  sdb_ack_cb_fcn() whenever it is invoked.
---------------------------------------------------------------------------*/
typedef struct
{
  dss_so_sdb_ack_cb_fcn sdb_ack_cb;
  void*                 user_data;
} dss_so_sdb_ack_cb_type;

/*---------------------------------------------------------------------------
  The values for the 'how' argument of the shutdown() call. Specifies what
  action to perform:
---------------------------------------------------------------------------*/
#define  DSS_SHUT_RD   0                      /* disallow further receives */
#define  DSS_SHUT_WR   1                         /* disallow further sends */
#define  DSS_SHUT_RDWR 2     /* disallow further receives as well as sends */


/*===========================================================================

                          PUBLIC MACRO DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION DSS_SOCKET()

DESCRIPTION
 Create a socket and related data structures, and return a socket descriptor.

 The mapping to actual protocols is as follows:
                
 ADDRESS FAMILY         Stream          Datagram

 AF_INET                TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

DEPENDENCIES
  The function dss_open_netlib() must be called to open the network library
  and put the DS/PS managers into sockets mode.

RETURN VALUE
  On successful creation of a socket, this function returns socket file 
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.
  
  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  specified protocol not supported
  DS_EMFILE           too many descriptors open.  A socket is already open or 
                      has not closed compeletely.

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_socket
(
  sint15 app_id,                                         /* application ID */
  byte   family,                          /* Address family - AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_CONNECT()

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  The implementation does not support connected
  UDP sockets and will return an error.  The function must receive
  (as a parameter) a valid socket descriptor, implying a previous successful
  call to dss_socket().

DEPENDENCIES
  Network subsystem must be established and available.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specfied
  DS_ECONNREFUSED     connection attempt refused
  DS_ETIMEDOUT        connection attempt timed out
  DS_EFAULT           addrlen parameter is invalid
  DS_EIPADDRCHANGED   IP address changed due to PPP resync
  DS_EINPROGRESS      connection establishment in progress
  DS_EISCONN          a socket descriptor is specified that is already 
                      connected
  DS_ENETDOWN         network subsystem unavailable
  DS_EOPNOTSUPP       invalid server address specified
  DS_EADDRREQ         destination address is required.
  DS_NOMEM            not enough memory to establish connection

SIDE EFFECTS
  For TCP, initiates active open for connection.

===========================================================================*/
extern sint15 dss_connect
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct sockaddr *servaddr,                        /* destination address */
  uint16 addrlen,                                    /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_LISTEN()

DESCRIPTION

  For TCP, this starts a passive open for connections.  Upon a
  sucessful connection, the socket callback function is invoked
  asserting DS_ACCEPT_EVENT as TRUE.  The application should respond
  with a call to dss_accept(). If a connection is recieved and there
  are no free queue slots the new connection is rejected
  (ECONNREFUSED).  The backlog queue is for ALL unaccepted sockets
  (half-open, or completely established).

  A listening UDP doesn't make sense, and as such isn't supported.
  DS_EOPNOTSUPP is returned.

  The sockfd parameter is a created (dss_socket) and bound (dss_bind)
  socket that will become the new listening socket.  The backlog
  parameter indiates the maximum length for the queue of pending
  sockets.  If backlog is larger than the maximum, it will be 
  reduced to the maximum (see DSS_SOMAXCONN).

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind) .

RETURN VALUE

  Returns DSS_SUCCESS on success.  If the backlog was truncated
  DS_EFAULT will be set in errno, but the call will still be
  successful.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block (PJ: I don't think this CAN happen)
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not capable of listening (UDP)
  DS_EFAULT           backlog parameter is invalid
  DS_ENETDOWN         network subsystem unavailable
  DS_NOMEM            not enough memory to establish backlog connections. 
  DS_EINVAL           Socket already open, closed, unbound or not one 
                      you can listen on. 

SIDE EFFECTS 
  For TCP, initiates passive open for new connections.

===========================================================================*/
extern sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_ACCEPT()

DESCRIPTION

  The accept function is used on listening sockets to respond when
  DS_ACCEPT_EVENT is asserted.  The first backlog queued connection is
  removed from the queue, and bound to a new connected socket (as if
  you called dss_socket).  The newly created socket is in the
  connected state.  The listening socket is unaffect the queue size is
  maintained (ie. there is not need to call listen again.)

  The argument sockfd is the file descriptor of the listening socket

  The argument remote addr is a pointer to a struct sockaddr.  This
  structure is populated with the address information for the remote
  end of the new connection. addrlen should initially contain the
  length of the struct sockaddr passed in.  The length of the real
  address is placed in this location when the struct is populated.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind).  The socket should be listening (implying a
  previously successful call to dss_listen).

RETURN VALUE
  Returns the socket descriptor of the new socket on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block 
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not of type SOCK_STREAM 
  DS_EINVAL           Socket is not listening. 
  DS_EFAULT           The addr parameter is bogus.
  DS_ENETDOWN         network subsystem unavailable
  DS_NOMEM            not enough memory to establish backlog connections. 

SIDE EFFECTS 

  The head backlog item from the queue of the listening socket is
  removed from that queue.

===========================================================================*/
extern sint15 dss_accept 
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct sockaddr *remoteaddr,                       /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_OPEN_NETLIB()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress.  The callback are called with a pointer
  to a sint15 containing the application ID for the callback.
  NOTE: the memory for the application ID is ephemeral and likely will not be
    available after the callback returns - if it is desired to use this
    information outside the scope of the callback it should be COPIED, a
    pointer MUST not be used.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in 
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.
            
SIDE EFFECTS
  Puts data services manager into "socket" mode.

===========================================================================*/
extern sint15 dss_open_netlib
(
  void   (*net_callback_fcn)(void *),         /* network callback function */
  void   (*socket_callback_fcn)(void *),       /* socket callback function */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_CLOSE_NETLIB()

DESCRIPTION

  Closes the network library for the application.  All sockets must have
  been closed for the application, prior to closing.  If this is the last
  remaining application, the network subsytem (PPP/traffic channel) must 
  have been brought down, prior to closing the network library.  If this 
  is the last active application using the network library, this function 
  takes the data services manager out of "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP        invalid application ID
  DS_ESOCKEXIST     there are existing sockets
  DS_ENETEXIST      the network subsystem exists

SIDE EFFECTS
  Puts data services manager into "autodetect" mode.

===========================================================================*/
extern sint15 dss_close_netlib
(
  sint15 app_id,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_PPPOPEN()

DESCRIPTION
  Starts up the network subsystem (CDMA data service and PPP) over the Um 
  interface for all sockets.  
  
DEPENDENCIES
  dss_pppopen() cannot be called by the application if the network is in the 
  process of closing. The network layer cannot queue the open request until 
  the close is completely finished.  Therefore, the application should wait 
  for the net_callback_fn() to be called (after dss_pppclose() has 
  completed), before making a call to dss_pppopen().  Note that a valid
  application ID must be specified as a parameter, obtained by a successful
  return of dss_open_netlib().

RETURN VALUE
  If the network subsytem is already established, return DSS_SUCCESS.

  Return DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_EWOULDBLOCK           the operation would block
  DS_ENETCLOSEINPROGRESS   network close in progress. The application
                           should only call dss_pppopen() after the 
                           close/abort has completed.

SIDE EFFECTS
  Initiates call origination and PPP negotiation.

===========================================================================*/
extern sint15 dss_pppopen
(
  sint15 app_id,                                         /* application id */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_BIND()

DESCRIPTION
  For all client sockets, attaches a local address and port value to the 
  socket.  If the call is not explicitly issued, the socket will implicitly
  bind in calls to dss_connect() or dss_sendto().  Note that this function
  does not support binding a local IP address, but rather ONLY a local port
  number.  The local IP address is assigned automatically by the sockets 
  library.  The function must receive (as a parameter) a valid socket 
  descriptor, implying a previous successful call to dss_socket().

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       operation not supported
  DS_EADDRINUSE       the local address is already in use.
  DS_EINVAL           the socket is already attached to a local name
  DS_EFAULT           invalid address parameter has been specified

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_bind
(
  sint15 sockfd,                                      /* socket descriptor */
  struct sockaddr *localaddr,                             /* local address */
  uint16 addrlen,                                     /* length of address */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_CLOSE()

DESCRIPTION
  Non-blocking close of a socket.  Performs all necessary clean-up of data 
  structures and frees the socket for re-use.  For TCP initiates the active 
  close for connection termination.  Once TCP has closed, the DS_CLOSE_EVENT 
  will become TRUE, and the application can call dss_close() again to free 
  the socket for re-use.  UDP sockets also need to call this to 
  clean-up the socket and free it for re-use.
                         
DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block - TCP close in progress
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Initiates active close for TCP connections.

===========================================================================*/
extern sint15 dss_close
(
  sint15 sockfd,                                      /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_PPPCLOSE()

DESCRIPTION
  Initiates termination to bring down PPP and the traffic channel.  Upon
  successful close of the network subsystem, invokes the network callback
  function.
                         
DEPENDENCIES
  None.

RETURN VALUE
  If the network subsytem is already closed, return DSS_SUCCESS.

  Returns DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_EWOULDBLOCK           operation would block
  DS_ENETCLOSEINPROGRESS   network close in progress. A call to 
                           dss_pppclose() has already been issued.

SIDE EFFECTS
  Initiates termination of PPP.  Brings down PPP and traffic channel.

===========================================================================*/
extern sint15 dss_pppclose
(
  sint15 app_id,                                         /* application id */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_NETSTATUS()

DESCRIPTION
  Provides status of network subsystem.  Called in response to DS_ENETDOWN
  errors.  Note that origination status is based on the last attempted 
  origination.
                         
DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_ENETNONET             network subsystem unavailable for some unknown 
                           reason
  DS_ENETISCONN            network subsystem is connected and available
  DS_ENETINPROGRESS        network subsystem establishment currently in 
                           progress
  DS_ENETCLOSEINPROGRESS   network subsystem close in progress. 

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_netstatus
(
  sint15 app_id,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_ASYNC_SELECT()

DESCRIPTION
  Enables the events to be notified about through the asynchronous 
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function.  This function also performs a real-time
  check to determine if any of the events have already occurred, and if so
  invokes the application callback.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Sets the relevant event mask in the socket control block.  Will also 
  notify the application via the callback function.

===========================================================================*/
extern sint31 dss_async_select
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 interest_mask,                        /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_ASYNC_DESELECT()

DESCRIPTION
  Clears events of interest in the socket control block interest mask.  The
  application specifies a bitmask of events that it wishes to clear; events 
  for which it will no longer receive notification.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  

SIDE EFFECTS
  Clears specified events from the relevant event mask.

===========================================================================*/
extern sint15 dss_async_deselect
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 clr_interest_mask,                  /* bitmask of events to clear */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_GETNEXTEVENT()

DESCRIPTION
  This function performs a real-time check to determine if any of the events 
  of interest specified in the socket control block's event mask have 
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to 
  dss_async_select().  The application may pass in a pointer to a single 
  socket descriptor to determine if any events have occurred for that socket.

  Alternatively, the application may set this pointer's value to NULL (0) 
  (note, not to be confused with a NULL pointer, but rather a pointer whose 
  value is 0) in which case the function will return values for the next 
  available socket.  The next available socket's descriptor will be placed 
  in the socket descriptor pointer, and the function will return.  If no 
  sockets are available (no events have occurred across all sockets for
  that application) the pointer value will remain NULL (originally value 
  passed in), and the function will return 0, indicating that no events
  have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  Returns an event mask of the events that were asserted.  A value of 0
  indicates that no events have occurred.

  On passing a pointer whose value is NULL into the function for 
  the socket descriptor (not to be confused with a NULL pointer), places 
  the next available socket descriptor in *sockfd_ptr and returns the
  event mask for that socket. If no sockets are available (no events have 
  occurred across all sockets for that application) the pointer value 
  will remain NULL (originally value passed in), and the function will 
  return 0, indicating that no events have occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP           invalid app descriptor is specfied
  DS_EBADF             invalid socket descriptor is specfied

SIDE EFFECTS
  Clears the bits in the socket control block event mask, corresponding to 
  the events that have occurred.
  
===========================================================================*/
extern sint31 dss_getnextevent
( 
  sint15 app_id,                                         /* application ID */
  sint15 *sockfd_ptr,                                 /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_WRITE()

DESCRIPTION
  Sends specified number of bytes in the buffer over the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of 
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_write
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,               /* user buffer from which to copy data */
  uint16 nbytes,                /* number of bytes to be written to socket */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_WRITEV()

DESCRIPTION
  Provides the gather write variant of the dss_write() call, which 
  allows the application to write from non-contiguous buffers.    Sends 
  specified number of bytes in the buffer over the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of 
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_writev
(
  sint15 sockfd,                                      /* socket descriptor */
  struct iovec iov[],     /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_READ()

DESCRIPTION
  Reads specified number of bytes into buffer from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of 
      bytes specified.  A return of 0 indicates that an End-of-File condition 
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_read
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,                     /* user buffer to which to copy data */
  uint16 nbytes,                 /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_READV()

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which 
  allows the application to read into non-contiguous buffers.    Reads
  specified number of bytes into the buffer from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of 
      bytes specified.  A return of 0 indicates that an End-of-File condition 
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_readv
(
  sint15 sockfd,                                      /* socket descriptor */
  struct iovec iov[],           /* array of data buffers to copy data into */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_SENDTO()

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the UDP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required 
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EOPNOSUPPORT     option not supported

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_sendto
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,           /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct sockaddr *toaddr,                          /* destination address */
  uint16 addrlen,                                        /* address length */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_RECVFROM()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EOPNOSUPPORT     option not supported

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_recvfrom
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,               /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct sockaddr *fromaddr,                        /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================
FUNCTION DSS_GET_IFACE_STATUS()

DESCRIPTION Retrieve iface_status 
  
===========================================================================*/
#define dss_get_iface_status dssnet_get_iface_status

/*===========================================================================
FUNCTION DSS_FORCE_DORMANCY()

DESCRIPTION
  NOTE - THIS FUNCTION IS NOW DEPRECATED. 
  Applications should use the new ioctl interface - dss_iface_ioctl() -
  to force dormancy. Refer to dss_iface_ioctl.h more details.
  
  This function is used to force dormancy (ie release traffic channel without
  bringing down the call) on an IS707 (1X) packet data call only.

DEPENDENCIES
  None. 

PARAMETERS  
  app_id           application ID

RETURN VALUE
  None.
  
SIDE EFFECTS
  None
===========================================================================*/
void dss_force_dormancy
(
  sint15 app_id
);

/*===========================================================================
FUNCTION DSS_ENABLE_DORM_TIMER()

DESCRIPTION
  NOTE - THIS FUNCTION IS NOW DEPRECATED. 
  Applications should use the new ioctl interface - dss_iface_ioctl() - 
  to enable/disable dormancy timer. Refer to dss_iface_ioctl.h more details.

  Applications can call this function to enable or disable
  dormancy timer for an IS707 (1X) packet data call only.

DEPENDENCIES
  If multiple applications call this function, principle used is last
  caller wins.
  If this function is invoked to disable timer, the disabling of timer
  remains in effect until next data call (or if the function is again
  invoked in between to enable timer).

PARAMETERS
  app_id           application ID
  flag             true/false - enable/disable dorm timer

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void dss_enable_dorm_timer
(
  sint15 app_id,
  boolean flag
);

/*===========================================================================

FUNCTION DSS_SETSOCKOPT

DESCRIPTION
  Set the options associated with a socket. This fuction expects the 
  following parameters:
  
DEPENDENCIES
  None. 
  
PARAMETERS  
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level. 
  int optname,      -     Option name. 
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.  
  sint15 *errno     -     Error condition value.     


RETURN VALUE
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_setsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                       /* error condition value          */
);


/*===========================================================================
FUNCTION DSS_GETSOCKOPT

DESCRIPTION
  Return an option associated with a socket. This fuction expects the 
  following parameters:
  
DEPENDENCIES
  None. 

PARAMETERS  
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level. 
  int optname,      -     Option name. 
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.  
  sint15 *errno     -     Error condition value.     

RETURN VALUE
  optlen is a value-result parameter, initially containing the size of 
  the buffer pointed to by optval, and modified on return to indicate the  
  actual  size  of the value returned. On error, return DSS_ERROR and places 
  the error condition value in *errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value  
  DS_EFAULT               Invalid buffer or argument  

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */                  
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
);
      
/*===========================================================================
FUNCTION DSS_GETSOCKNAME

DESCRIPTION
  Returns the current local address assigned to the specified socket.
  
DEPENDENCIES
  None. 

PARAMETERS  
  sockfd    -  socket file descriptor
  addr      -  local address currently associated with the socket 
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it 
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the socket 
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to 
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory 
                          location

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getsockname
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
);

/*===========================================================================
FUNCTION DSS_GETPEERNAME

DESCRIPTION
  Returns the address of the peer connected to the specified socket.
  
DEPENDENCIES
  None. 

PARAMETERS  
  sockfd    -  socket file descriptor
  addr      -  address of the peer connected with the socket 
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it 
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the peer 
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to 
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory 
                          location
  DS_ENOTCONN             the socket is not connected

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getpeername
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
);

/*===========================================================================
FUNCTION DSS_SHUTDOWN

DESCRIPTION
  Shuts down the connection of the specified socket depending on the
  'how' parameter as follows:
   
  DSS_SHUT_RD:   Disallow subsequent calls to recv function
  DSS_SHUT_WR:   Disallow subsequent calls to send function
  DSS_SHUT_RDWR: Disallow subseuqnet calls to both recv and send functions

DEPENDENCIES
  None. 

PARAMETERS  
  sockfd    -  socket file descriptor
  how       -  action to be performed: shutdown read-half, write-half or
               both
  dss_errno -  error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns 
  DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOTCONN             the socket is not connected
  DS_EINVAL               invalid operation (e.g., how parameter is invalid)
  DS_ENOMEM               insufficient memory available to complete the 
                          operation

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
);

/*===========================================================================
FUNCTION DSS_INIT_NET_POLICY_INFO

DESCRIPTION
  Populates the policy info structure with default values.
DEPENDENCIES
  None.
PARAMETERS
  POLICY_INFO_PTR  pointer to policy info data structure.
RETURN VALUE
  None
SIDE EFFECTS
  Initializes the fields in the policy info data structure.

===========================================================================*/
void dss_init_net_policy_info
(
  dss_net_policy_info_type * policy_info_ptr       /* policy info structure */
);

/*===========================================================================
FUNCTION DSS_SET_APP_NET_POLICY

DESCRIPTION
   Sets the application netpolicy to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  DSS_NETHANDLE    application id
  POLICY_INFO_PTR  pointer to policy info data structure.
  DSS_ERRNO        error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.
  Errno Values
  ------------
  DS_EBADAPP              Invalid application ID is specfied
  DS_EFAULT               Invalid policy_info_ptr is specified.

SIDE EFFECTS
  Sets the application netpolicy to the user specified value.

===========================================================================*/
sint15 dss_set_app_net_policy
(
  sint15 dss_nethandle,                           /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                              /* error number */
);

/*===========================================================================
FUNCTION DSS_GET_APP_PROFILE_ID

DESCRIPTION
   Gets the application profile id.

DEPENDENCIES
  None.

PARAMETERS
  app_type            application type

RETURN VALUE
  Returns profile_id.

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_get_app_profile_id
(
  uint32 app_type
);

#endif  /* FEATURE_DS_SOCKETS */

#endif  /* _DSSOCKET_H */
