#ifndef DSBSD_H
#define DSBSD_H
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_DS_SOCKETS_BSD
/*===========================================================================

        D A T A  S E R V I C E S  B S D  S O C K E T  A P I  F I L E

DESCRIPTION

 The Data Services mobile BSD socket API file. Contains basic API functions
 for socket manipulation.

EXTERNALIZED FUNCTIONS

  bind()
    Attaches a local address and port number to a socket.

  bsd_init()
    Opens BSD socket services to an application.

  bsd_reg_usr_notifier()
    Register user notification call back (Qos, Mcast etc.) with BSD
  
  bsd_shutdown()
    Ends BSD socket services to an application.

  close()
    Terminates data communication and frees the socket descriptor for reuse.

  connect()
    Initiates a TCP handshake with a remote endpoint address.  Cannot be
    used to specify a peer for sockets that use UDP.

  inet_addr()
    Takes a string containing a dotted-decimal IP address and returns the
    equivalent IP address in binary form.

  ioctl()
    Sets a socket's I/O behavior to either blocking or nonblocking.

  gethostbyname()
    Gets information on a host given the host name.

  getsockname()
    Gets information (local address) on a given socket.

  read()
    Reads data from a socket that uses TCP.

  recv()
    Receives data from a socket that uses TCP.  Flags are not supported.

  recvfrom()
    Gets the next message from a UDP socket and records the sender's
    address.

  select()
    Waits on a list of socket descriptors and unblocks when one of them
    becomes ready.

  send()
    Sends data on a socket that uses TCP.  Flags are not supported.

  sendto()
    Sends a message on a socket that uses UDP.

  socket()
    Creates an endpoint for data communication.

  write()
    Writes data to a socket that uses TCP.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/dsbsd/main/latest/inc/dsbsd.h#6 $
$DateTime: 2008/08/29 05:12:35 $
$Change: 733570 $

========================================================================== */

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
/* To fix the compilation errors in LTK 4.4 */
#ifndef FEATURE_LTK_QTV_44
#endif
#include "rex.h"
#include "dssbsdh.h"
#include "ps_iface_defs.h"

#if defined(FEATURE_QTV_QOS_SELECTION) && !defined(PLATFORM_LTK)
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTIION */

#if defined(PLATFORM_LTK)
#error code not present
#endif

/*---------------------------------------------------------------------------
  Macro to convert from the socket file descriptor to a unique index.
---------------------------------------------------------------------------*/
#define SOCKFD_TO_INDEX(fd) (((uint32)((fd) - SOCKFDBASE_MIN_VAL)) % DSS_MAX_SOCKS)

/*---------------------------------------------------------------------------
  #undef macros in Window's errno.h file that will be redefined.  This is to
  rid of the macro redefinition warnings when this file is included along
  with <errno.h> in the ZREX LTK.
---------------------------------------------------------------------------*/
#undef EINTR
#undef EBADF
#undef EFAULT
#undef EAGAIN
#undef EMFILE
#undef EPIPE
#undef EINVAL
#undef ENOMEM
#undef errno

/*---------------------------------------------------------------------------
                      Transport Error Condition Values
---------------------------------------------------------------------------*/
#define EEOF            DS_EEOF            /* End of file                  */
#define EINTR           DS_EINTR           /* Interruption during system
                                              call produced fault          */
#define EBADF           DS_EBADF           /* Invalid socket descriptor    */
#define EFAULT          DS_EFAULT          /* Invalid buffer or argument   */
#define EWOULDBLOCK     DS_EWOULDBLOCK     /* Operation would block        */
#define EAGAIN          EWOULDBLOCK
#define EAFNOSUPPORT    DS_EAFNOSUPPORT    /* Address family not supported */
#define EPROTOTYPE      DS_EPROTOTYPE      /* Wrong protocol for socket
                                              type                         */
#define ESOCKTNOSUPPORT DS_ESOCKNOSUPPORT  /* Socket parm not supported    */
#define EPROTONOSUPPORT DS_EPROTONOSUPPORT /* Protocol not supported       */
#define EMFILE          DS_EMFILE          /* No more sockets available for
                                              opening */
#define EOPNOTSUPP      DS_EOPNOTSUPP      /* Operation not supported      */
#define EADDRINUSE      DS_EADDRINUSE      /* Address already in use       */
#define EDESTADDRREQ    DS_EADDRREQ        /* Destination address required */
#define EINPROGRESS     DS_EINPROGRESS     /* Connect in progress          */
#define EISCONN         DS_EISCONN         /* Connection already
                                              established                  */
#define ENOTCONN        DS_ENOTCONN        /* socket not connected         */
#define ECONNREFUSED    DS_ECONNREFUSED    /* Connection attempt refused   */
#define ETIMEDOUT       DS_ETIMEDOUT       /* Connection attempt timed out */
#define ECONNRESET      DS_ECONNRESET      /* Connection reset             */
#define ECONNABORTED    DS_ECONNABORTED    /* Connection aborted           */
#define EPIPE           DS_EPIPE           /* Broken pipe                  */
#define ENETDOWN        DS_ENETDOWN        /* Network unavailable          */
#define EINVAL          DS_EINVAL          /* Invalid operation            */
#define ENOMEM          DS_ENOMEMORY       /* No memory available          */
#define ENOPROTOOPT     DS_ENOPROTOOPT     /* unknown socket option        */ 

/*---------------------------------------------------------------------------
  The following error code is an exception in the sense that BSD does not
  have an error code called ENETWOULDBLOCK. This error code is currently
  defined to be same as DS_ENETCLOSEINPROGRESS. However, dsbsd.c code may
  map multiple DS error codes to ENETWOULDBLOCK to indicate that a network
  related operation is underway.
---------------------------------------------------------------------------*/
#define ENETWOULDBLOCK  DS_ENETCLOSEINPROGRESS

/*---------------------------------------------------------------------------
                           Network Subsystem Errors
---------------------------------------------------------------------------*/
#define ENONET          DS_ENETNONET       /* Network unavailable          */

/*---------------------------------------------------------------------------
                          DNS Error Condition Values
---------------------------------------------------------------------------*/
#define HOST_NOT_FOUND  DS_EHOSTNOTFOUND /* Host not found                 */
#define NO_RECOVERY     DS_ENORECOVERY   /* Irrecoverable error occurred   */
#define TRY_AGAIN       DS_ETRYAGAIN     /* Temporary error                */
#define NO_ADDRESS      DS_ENOADDRESS    /* No address for the domain name */

/*---------------------------------------------------------------------------
  Macros used with select() or used in macros that are used in conjunction
  with select().
  We cast fd to unsigned int so that SOCKFD_TO_INDEX() results in an unsigned,
  so we never have a negative array index (reported by Klockwork).
---------------------------------------------------------------------------*/
#define FD_MASK(fd) \
  ((unsigned char)1 << (SOCKFD_TO_INDEX(fd) % BITS_PER_MASK))

#define FD_CLR(fd, fdset) \
  ((fdset)->set[SOCKFD_TO_INDEX(fd) / BITS_PER_MASK] &= ~FD_MASK(fd))

#define FD_SET(fd, fdset) \
  ((fdset)->set[(uint32)SOCKFD_TO_INDEX(fd) / BITS_PER_MASK] |= FD_MASK(fd))

#define FD_ZERO(fdset)       memset(fdset, 0, sizeof(fd_set))

#define FD_ISSET(fd, fdset) \
  ((fdset)->set[(uint32)SOCKFD_TO_INDEX(fd) / BITS_PER_MASK] & FD_MASK(fd))

/*---------------------------------------------------------------------------
  Maximum number of callback functions. These callback functions are called 
  when specified signal is received.
---------------------------------------------------------------------------*/

#define BSD_MAX_NUM_OF_SIG_CB 2

/*---------------------------------------------------------------------------
                         Type/structure definitions
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
                           DNS-relate structure(s)
---------------------------------------------------------------------------*/
#define h_addr     h_addr_list[0]

/*---------------------------------------------------------------------------
  struct hostent should have the same members as dss_hostent.  It is needed
  so that the application can make such declarations:  struct hostent host;.
  Note the struct keyword.
---------------------------------------------------------------------------*/
struct hostent
{
  char  *h_name;                       /* Official host name               */
  char **h_aliases;                    /* NULL-terminated alias list       */
  int    h_addrtype;                   /* host address type                */
  int    h_length;                     /* length of address                */
  char **h_addr_list;                  /* NULL-terminated address list     */
};

/*---------------------------------------------------------------------------
  Supported Socket Option Names
---------------------------------------------------------------------------*/
#define IP_ADD_MEMBERSHIP  1           /* join multicast group             */
#define IP_DROP_MEMBERSHIP 2           /* leave multicast group            */
#define SO_LINGER          3           /* LINGER OPTION                    */
#define SO_RCVBUF          4           /* Set Receive Buffer Size          */
#define TCP_SACK           5           /* Set SACK option                  */
#define TCP_NODELAY        6           /* Disable Nagle's algorithm        */

/*---------------------------------------------------------------------------
  Supported Socket Option Levels
---------------------------------------------------------------------------*/
/* These are the standard values taken from virtually all BSD flavors.
 * They are based on RFC 790. Note that ps_in.h defines IPPROTO_IP as 4
 * which conflicts with the following definition. The problem is that 4 is 
 * supposed to be mapped to CMCC Gateway Monitoring Message according to 
 * the RFC, and the definition of IPROTO_IP is pretty much standardized to
 * 0. If a file includes this source file, we expect that it wants the 
 * BSD definitions instead of the QC definitions. By using the standard
 * values, we allow the user to pass in well-known values which are not 
 * based on the defintions below.
 */
#define IPPROTO_IP         0           /* IP layer                         */
#define SOL_SOCK           1           /* Socket                           */
#define IPPROTO_TCP        6           /* TCP layer                        */
#define IPPROTO_UDP       17           /* UDP layer                        */
#define IPPROTO_IPV6      41           /* IPV6 layer                       */

/*ioctl event status types*/
typedef enum 
{
  BSD_STATUS_ERROR                  = -3,
  BSD_QOS_STATUS_MIN                = -2,
  BSD_QOS_STATUS_WAITING            = -2,
  BSD_QOS_STATUS_ERROR              = -1,
  BSD_QOS_STATUS_AVAILABLE          = 0,
  BSD_QOS_STATUS_AVAILABLE_MODIFIED = 1,
  BSD_QOS_STATUS_UNAVAILABLE        = 2,
  BSD_QOS_STATUS_DEACTIVATED        = 3,
  BSD_QOS_STATUS_MODIFY_ACCEPTED    = 4,
  BSD_QOS_STATUS_MODIFY_REJECTED    = 5,
  BSD_QOS_STATUS_MAX                = 5,

  BSD_MCAST_STATUS_MIN              = 6,
  BSD_MCAST_STATUS_WAITING          = 6,
  BSD_MCAST_STATUS_ERROR            = 7,
  BSD_MCAST_STATUS_SUCCESS          = 8,
  BSD_MCAST_STATUS_DEREGISTERED     = 9,
  BSD_MCAST_STATUS_FAILED           = 10,
  BSD_MCAST_STATUS_MAX              = 10,
  BSD_QOS_SYSTEM_MIN                = 11,
  BSD_QOS_AWARE_SYSTEM              = 11,
  BSD_QOS_UNAWARE_SYSTEM            = 12,
  BSD_QOS_SYSTEM_MAX                = 12

} bsd_status_enum_type;

#ifndef PLATFORM_LTK
typedef enum
{
  BSD_IFACE_IOCTL_MIN                   = 0,
  BSD_IFACE_IOCTL_GET_BEARER_TECHNOLOGY,
  BSD_IFACE_IOCTL_GET_DATA_BEARER_RATE,
  BSD_IFACE_IOCTL_GET_ST_DATA_BEARER_RATE,
  BSD_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM 
} bsd_iface_ioctl_type;
#endif /* PLATFORM_LTK */

typedef struct
{
  ip_addr_enum_type type; 
  union
  {
    struct in_addr  v4;
    struct in6_addr v6;
  }addr;
} IpAddrType;

typedef struct
{
  uint8 type;
  union
  {
    struct sockaddr_in  v4;
    struct sockaddr_in6 v6;
  }sockaddr;
} SockAddrType;

#ifdef FEATURE_QTV_MCAST 
/*supported broadcast/multicast interfaces*/
typedef enum 
{
  BSD_IFACE_MIN                     = 1,
  BSD_IFACE_CDMA_BCAST              = 1,
  BSD_IFACE_DVBH                    = 2,
  BSD_IFACE_MBMS                    = 3,
#ifdef QTV_FEATURE_STA_IFACE
  BSD_IFACE_STA                     = 4,
#endif
  BSD_IFACE_LAST,
  BSD_IFACE_MAX                     = BSD_IFACE_LAST - 1
}bsd_iface_name_enum_type;

/*---------------------------------------------------------------------------
  Structure used for multicast add/drop membership socket option
---------------------------------------------------------------------------*/
struct ip_mreq
{
  IpAddrType imr_multiaddr;    /* IPv6 multicast addr */
  IpAddrType imr_interface;    /* interface to join on */
};

/*---------------------------------------------------------------------------
  Structure used for multicast add/drop membership through ioctl
---------------------------------------------------------------------------*/
struct ip_mcast_info
{
  IpAddrType im_multiaddr;             /* multicast group to join          */
  uint16 im_port ;        
};
#endif /* FEATURE_QTV_MCAST */

// Declaring a Duplicate of the struct dss_so_linger_type in dssocket.h
// which can be accessed in NetBase.cpp and passed then passed down to lower layers

typedef struct
{
  int l_onoff;                                     /* linger active or not */
  int l_linger;                   /* how many milli-secondss to linger for */
} so_linger_type;

/*---------------------------------------------------------------------------
  Structure used in select() call.  Specifies the maximum time to wait for
  socket descriptors of interest to become ready.  When the value of the
  structure members are 0, then select() is in poll mode.
---------------------------------------------------------------------------*/
struct timeval
{
  uint32 tv_sec;                                           /* seconds      */
  uint32 tv_usec;                                          /* microseconds */
};

typedef struct
{
  rex_sigs_type select;
  rex_sigs_type socket;
  rex_sigs_type net;
  rex_sigs_type iface_ioctl;
  rex_sigs_type dns;
  rex_sigs_type abort;
  rex_sigs_type tcp_close_timeout;
} default_sigs_type;

typedef int (signal_clbk_type)(rex_sigs_type, void *);

typedef struct
{
  rex_sigs_type     signal;          /* signal mask                        */
  signal_clbk_type *clbk_fnc_ptr;    /* Pointer to the BSD app's callback
                                        function for the associated signal */
  void             *arg;             /* Callback function's argument       */
  boolean           isActive;
} sig_clbk_type;

#ifdef FEATURE_QTV_NET_POLICY
#ifdef PLATFORM_LTK
#error code not present
#else
typedef dss_net_policy_info_type bsd_net_policy_info_type;
#endif /* PLATFORM_LTK */
#endif /* FEATURE_QTV_NET_POLICY */


typedef void (*bsdcb_fcn_type)(bsd_status_enum_type, 
                               void *cb_data, void* cb_info);

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

#ifdef PLATFORM_LTK
#error code not present
#else
typedef dss_iface_ioctl_data_bearer_rate bsd_data_bearer_rate;
typedef dss_iface_ioctl_bearer_tech_type bsd_bearer_tech_type;
#endif
 
#ifdef FEATURE_QTV_MCAST_IOCTL
#ifdef PLATFORM_LTK
#error code not present
#else
typedef dss_iface_ioctl_mcast_handle_type bsd_mcast_handle_type;
#endif
#endif

typedef struct
{
  rex_tcb_type   *owning_tcb_ptr;       /* task owning the control block   */
  rex_timer_type  select_timer;         /* timer used for select()         */
  rex_sigs_type   select_timer_sig;     /* timer signal mask               */
  rex_sigs_type   socket_sig;           /* socket event signal mask        */
  rex_sigs_type   network_sig;          /* network event signal mask       */
  rex_sigs_type   dns_sig;              /* DNS signal mask                 */
  rex_sigs_type   iface_ioctl_sig;      /* IFACE IOCTL signal              */
  rex_sigs_type   abort_sig;            /* abort signal                    */
  rex_sigs_type   tcp_close_timer_sig;  /* TCP close timeout signal        */
  rex_sigs_type   wait_mask;            /* mask of all interested signals  */
  rex_timer_type *watchdog_timer;       /* pointer to watchdog timer       */
  rex_sigs_type   watchdog_sig;         /* watchdog signal mask            */
  rex_timer_cnt_type watchdog_rpt_time; /* watchdog report interval        */ 
  unsigned long    watchdog_rpt_mask;

  /* array of callback functions to be called for specified signals */
  sig_clbk_type   sig_cb_array[BSD_MAX_NUM_OF_SIG_CB]; 
  int             num_sig_cb;           /* num of entries in sig_cb_array[]*/

  int             sock_ref_count;       /* num of sockets app has opened   */
  struct hostent *hostent_ptr;          /* address of hostent structure    */
  sint15          netlib_app_id;        /* ID of app using the BSD socket  */
  sint15          bsd_errno;            /* error code                      */

  /*-------------------------------------------------------------------------
    Variables used for minimizing calls to dss_pppopen()
  --------------------------------------------------------------------------*/
  int network_event_occurred;           /* has a network event occurred,    */
                                        /* indicated by the network cb      */

  int network_up;                       /* is the network up?               */

  bsdcb_fcn_type cb_fcn;

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif  

#ifdef FEATURE_QTV_MCAST_IOCTL
  /* Mcast callback function to the upper layer */
  boolean mcast_cb_flag;
  void *mcast_cb_data;
#endif  /* FEATURE_QTV_MCAST */

} bsdcb_type;

/*---------------------------------------------------------------------------
                                Other Macros
---------------------------------------------------------------------------*/
#define errno           (((bsdcb_type *)(rex_self())->bsdcb_ptr)->bsd_errno)
#define h_errno         errno
#define FIONBIO         100           /* request to change I/O behavior    */
#define INADDR_NONE     0xFFFFFFFFUL

/*===========================================================================

                         PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef __cplusplus
   extern "C"
   {
#endif
/*===========================================================================

FUNCTION SOCKET()

DESCRIPTION
  Creates an endpoint for data communication.  socket() checks the socket
  reference count to determine the number of sockets the application has
  opened.

DEPENDENCIES
  The function bsd_init() must have been called without an intervening call
  to bsd_shutdown().

PARAMETERS
  family       address family (only AF_INET is supported)
  type         type of service (SOCK_STREAM for TCP, SOCK_DGRAM for UDP)
  protocol     protocol number to use for the given family and type.  The
               caller may specify 0 for the protocol number to request the
               default for the given family and type or explicitly specify
               IPPROTO_TCP and IPPROTO_UDP as appropriate.

RETURN VALUE
  A socket descriptor or -1 to indicate that an error has occurred.  errno
  contains the code specifying the cause of the error.

  Errno Values
  ------------
  EAFNOSUPPORT      address family not supported
  EFAULT            invalid BSD control block
  EPROTOTYPE        invalid protocol for the socket type
  ESOCKTNOSUPPORT   invalid or unsupported socket parameter specified
  EMFILE            too many descriptors open.  A socket is already open
                    or has not closed completely.

SIDE EFFECTS
  None.

===========================================================================*/
int socket
(
  int family,                             /* Address family - AF_INET only */
  int type,                                                 /* socket type */
  int protocol                                          /* protocol number */
);

/*===========================================================================

FUNCTION BIND()

DESCRIPTION
  Attaches a local address and port value to a client socket.  If the call is
  not explicitly issued, the socket will implicitly bind in calls to connect()
  or sendto().  Note that this function does not support binding a local IP
  address, but rather ONLY a local port number.  The local IP address is
  assigned automatically by the sockets library.

DEPENDENCIES
  None.

PARAMETERS
  sockfd         socket descriptor
  *localaddr     address of the structure specifying the IP address and
                 port number
  addrlen        size of the above address structure in bytes

RETURN VALUE
  0 if successful.  -1 if an error occurred.  An error code indicating the
  cause of the error is written to errno.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  EOPNOTSUPP       operation not supported
  EADDRINUSE       the local address is already in use
  EINVAL           the socket is already attached to a local name
  EFAULT           invalid address parameter has been specified

SIDE EFFECTS
  None.

===========================================================================*/
int bind
(
  int sockfd,                                         /* socket descriptor */
  struct sockaddr *localaddr,                             /* local address */
  int addrlen                                         /* length of address */
);

/*===========================================================================

FUNCTION CONNECT()

DESCRIPTION
  Initiates a TCP handshake with a remote endpoint address.  The underlying
  implementation does not support connected UDP sockets.

DEPENDENCIES
  None.

PARAMETERS
  sockfd      socket descriptor
  *addr       pointer to the structure that specifies the IP address and
              port number of the server
  addrlen     size of the server address structure in bytes

RETURN VALUE
  0 if TCP is established.  -1 if an error occurred.  errno contains a code
  specifying the cause of the error.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  ECONNREFUSED     connection attempt refused
  ETIMEDOUT        connection attempt timed out
  EFAULT           addrlen parameter, BSD control block is invalid or
                   attempt to block on a socket the thread does not own
  EISCONN          the socket descriptor specified is already connected
  ENOTDOWN         network subsystem not available.
  ENETCONN         network subsystem went down during the connect attempt
  ENONET           attempt to bring up PPP failed - network subsystem down
  EOPNOTSUPP       invalid server address specified
  EADDRREQ         destination address is required
  EINTR            BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
int connect
(
  int sockfd,
  struct sockaddr *addr,
  int addrlen
);

/*===========================================================================

FUNCTION CLOSE()

DESCRIPTION
  Frees the socket descriptor for reuse.  For a socket that uses TCP, a
  handshake to terminate the TCP session is initiated.  If the application is
  closing its last socket, close() attempts to close PPP.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     socket descriptor

RETURN VALUE
  0 if successful.  -1 if an error occurred.  errno describes the error
  encountered.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specified
  EFAULT           internal DS Sock error, invalid BSD control block or
                   attempt to block on a socket the thread does not own
  EINTR            BSD application's callback function encountered an error

SIDE EFFECTS

===========================================================================*/
int close
(
  int sockfd
);

/*===========================================================================

FUNCTION WRITE()

DESCRIPTION
  Writes data to a socket descriptor for transfer to a remote machine over
  TCP.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     socket descriptor
  *buf       address of the buffer containing the data
  len        number of bytes in the buffer

RETURN VALUE
  The number of bytes written or -1 on failure.  When write() fails, errno
  indicates the cause of error.  Note that an error may happen after a
  partial write.  For instance, the underlying DS Sock layer may initially
  accept only part of the data because the system is low on buffers.  write()
  blocks until the DS Sock layer accepts remaining data.  During the wait, an
  application's callback function gets invoked and returns an error.  In such
  cases, the number of bytes written thus far is returned not -1.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specified
  ENOTCONN         socket not connected
  ECONNRESET       TCP connection reset by server
  ECONNABORTED     TCP connection aborted due to timeout or other failure
  ENETDOWN         network subsystem went down during write attempt
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           DS Sock internal error, invalid BSD control block or
                   attempt to block on a socket the thread does not own
  EINTR            BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
int write
(
  int   sockfd,
  const void *buf,
  unsigned int len                   /* number of bytes to write, can be 0 */
);

/*===========================================================================

FUNCTION SEND()

DESCRIPTION
  Sends data to a remote machine via a socket that uses TCP.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     socket descriptor
  *buf       address of the buffer containing the data
  len        number of bytes in the buffer
  flags      0 (not supported)

RETURN VALUE
  The number of bytes sent or -1 on failure.  When send() fails, errno is set
  to one of the following values.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specified
  ENOTCONN         socket not connected
  ECONNRESET       TCP connection reset by server
  ECONNABORTED     TCP connection aborted due to timeout or other failure
  ENETDOWN         network subsystem went down during write attempt
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           DS Sock internal error, invalid BSD control block or
                   attempt to block on a socket the thread does not own
  EOPNOTSUPP       unsupported flags parameter is specified
  EINTR            BSD application's callback function encountered an error

  Note that an error may have occurred after a partial send.  In such a case,
  the number of bytes sent thus far is returned not -1.

SIDE EFFECTS
  None.

===========================================================================*/
int send
(
  int   sockfd,
  const void *buf,
  int   len,
  int   flags
);

/*===========================================================================

FUNCTION SENDTO()

DESCRIPTION
  Sends a message to a remote machine via a socket that uses UDP.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     socket descriptor
  *buf       address of the buffer containing the data
  len        number of bytes in the buffer
  flags      0 (not supported)
  *toaddr    pointer to the destination address structure
  tolen      length of the destination address in bytes

RETURN VALUE
  The number of bytes sent or -1 on failure.  When sendto() fails, errno is
  set to one of the following values.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  EAFNOSUPPORT     address family not supported
  EADDRREQ         destination address required
  ENETDOWN         network went down during sendto() operation
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           invalid address length, DS Sock internal error, invalid
                   BSD control block or attempt to block on a socket the
                   thread does not own
  EOPNOTSUPP       option not supported
  EINTR            BSD application's callback function encountered an error

  Note that the underlying layer may accept only some of the bytes before an
  error occurs.  In such a case, the number of bytes accepted thus far is
  returned not -1.

SIDE EFFECTS
  None.

===========================================================================*/
int sendto
(
  int   sockfd,
  const void *buf,
  int   len,
  int   flags,
  struct sockaddr *toaddr,
  int   tolen
);

/*===========================================================================

FUNCTION READ()

DESCRIPTION
  Reads data from a socket that uses TCP.  

DEPENDENCIES
  None.

PARAMETERS
  sockfd     socket descriptor
  *buf       address of the buffer to put the data
  len        size of the buffer in bytes

RETURN VALUE
  The number of bytes read or -1 on failure.  When read() fails, errno is set
  to one of the following.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  ENOTCONN         socket not connected
  ECONNRESET       TCP connection reset by server
  ECONNABORTED     TCP connection aborted due to timeout or other failure
  ENETDOWN         network subsystem went down
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           DS Sock internal error, invalid BSD control block or
                   attempt to block on a socket the thread does not own
  EINTR            BSD application's callback function encountered an error

  Typically, errno values are meaningful only when an error occurs
  (function returns -1).  However, there is an exception.  When read()
  returns 0 as the number of bytes read and the caller specified a
  nonzero length, it means that the socket has closed.  In this case,
  errno is set to EEOF.

SIDE EFFECTS
  None.

===========================================================================*/
int read
(
  int   sockfd,
  void *buf,
  unsigned int len                    /* number of bytes to read, can be 0 */
);

/*===========================================================================

FUNCTION RECV()

DESCRIPTION
  Receives data from a TCP socket

DEPENDENCIES
  None.

PARAMETERS
  sockfd     socket descriptor
  *buf       address of the buffer to put the data
  len        size of the buffer in bytes
  flags      0 (not supported)

RETURN VALUE
  The number of bytes read or -1 on failure.  When recv() fails, errno
  is set to one of the following.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  ENOTCONN         socket not connected
  ECONNRESET       TCP connection reset by server
  ECONNABORTED     TCP connection aborted due to timeout or other failure
  ENETDOWN         network subsystem went down
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           DS Sock internal error, invalid BSD control block or
                   attempt to block on a socket the thread does not own
  EOPNOTSUPP       unsupported flags parameter is specified
  EINTR            BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
int recv
(
  int   sockfd,
  void *buf,
  int   len,
  int   flags
);

/*===========================================================================

FUNCTION RECVFROM()

DESCRIPTION
  Attempts to read a message from a UDP socket.  It blocks until a message
  is available or an error occurred.

DEPENDENCIES
  None.

PARAMETERS
  sockfd        socket descriptor
  *buf          address of the buffer to put the message
  len           size of the buffer in bytes
  flags         0 (not supported)
  *fromaddr     pointer to the sender's address
  *fromlen      length of the sender's address in bytes

RETURN VALUE
  The number of bytes read or -1 on failure.  When recvfrom() fails, errno
  is set to one of the following.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  EAFNOSUPPORT     address family not supported
  ENETDOWN         network subsystem went down
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           DS Sock internal error, invalid BSD control block or
                   attempt to block on a socket the thread does not own
  EOPNOTSUPP       option not supported
  EINTR            BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
int recvfrom
(
  int   sockfd,
  void *buf,
  int   len,
  int   flags,
  struct sockaddr *fromaddr,
  int  *fromlen
);

/*===========================================================================

FUNCTION SELECT()

DESCRIPTION
  Waits on a list of socket descriptors and unblocks when one of them become
  ready.  A maximum timeout value can also be specified.

DEPENDENCIES
  None.

PARAMETERS
  n              any value (not supported)
  *readfds       pointer to a set of socket descriptors to be monitored for
                 input
  *writefds      pointer to a set of socket descriptors to be monitored for
                 output
  *exceptfds     NULL (not supported)
  *timeout       address of a structure specifying maximum time to wait.  If
                 NULL, the wait is infinite.  If the contents is 0, select()
                 merely polls the socket descriptors.

RETURN VALUE
  Number of ready socket descriptors or -1 if an error occurred.  When
  select() encounters an error, errno is set to one of the following.

  Errno Values
  ------------
  EOPNOTSUPP       option not supported
  EINTR            BSD application's callback function encountered an error
  EFAULT           invalid BSD control block or the socket descriptor set
                   has a socket the thread does not own
  EBADF            internal DS Sock error

SIDE EFFECTS
  None.

===========================================================================*/
int select
(
  int     n,                                                    /* ignored */
  fd_set *readfds,
  fd_set *writefds,
  fd_set *exceptfds,
  struct timeval *timeout
);


/*===========================================================================

FUNCTION INET_ADDR()

DESCRIPTION
  Takes a string containing a dotted-decimal IP address and returns the
  equivalent IP address in binary form.  

DEPENDENCIES
  None.

PARAMETERS
  *addr       string containing a dotted-decimal IP address

RETURN VALUE
   IP address in network order if successful.
   INADDR_NONE if an error occurred.

SIDE EFFECTS
  None.

===========================================================================*/
unsigned long inet_addr(const char *addr);

#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif


/*===========================================================================

FUNCTION IOCTL()

DESCRIPTION
  Sets a socket's I/O behavior.  When the request is FIONBIO, ioctl() sets
  or clears the nonblocking I/O flag depending on the contents of arg_ptr.
  If *arg_ptr is nonzero, the socket is set for nonblocking I/O.  If *arg_ptr
  is zero, the socket returns to its default I/O behavior which is blocking.

DEPENDENCIES
  None.

PARAMETERS
   sockfd     socket descriptor
   request    request for ioctl() to perform
  *arg_ptr    address of the request's argument

RETURN VALUE
   0 if successful.
  -1 if an error occurred.  errno describes the error encountered.

  Errno Values
  ------------
  EBADF            invalid socket descriptor
  EINVAL           invalid request or arg_ptr

SIDE EFFECTS
  None.

===========================================================================*/
int ioctl
(
  int sockfd,
  unsigned long request,
  char *arg_ptr
);

/*===========================================================================

FUNCTION SETSOCKOPT()

DESCRIPTION
  Manipulates the options associated with a socket. Options may exist at
  different protocol levels. 

DEPENDENCIES
  None.

PARAMETERS
   sockfd     socket descriptor
   level      the level at which the option resides (e.g., IP level)
   optname    the name of the option
   optval     the option specific value (passed as opaque data)
   optlen     the length of the optval 

RETURN VALUE
   0 if successful.
  -1 if an error occurred.  errno describes the error encountered.

  Errno Values
  ------------
  EBADF            invalid socket descriptor
  EINVAL           invalid request or arg_ptr
  ENOPROTOOPT      the option is unknown at the level indicated
  EFAULT           the address pointed to by optval is invalid

SIDE EFFECTS
  None.

===========================================================================*/
int setsockopt
(
  int sockfd,
  int level,
  int optname,
  void* optval,
  uint32 optlen
);

/*===========================================================================

FUNCTION GETHOSTBYNAME()

DESCRIPTION
  Gets information about a host given the host's name.

DEPENDENCIES
  The application must allocate the memory for storing the host information.

PARAMETERS
  *name            character string that contains the host name.

RETURN VALUE
  Pointer to a hostent structure if successful.
  NULL on error with h_errno containing the error code.

  H_errno Values
  --------------
  HOST_NOT_FOUND       the specified host is unknown
  TRY_AGAIN            temporary error has occurred
  NO_RECOVERY          an irrecoverable error has occurred

SIDE EFFECTS
  None.

===========================================================================*/
struct hostent *gethostbyname
(
  const char *name
);
#ifdef FEATURE_QTV_IPV6
#error code not present
#endif

/*===========================================================================

FUNCTION GETSOCKNAME()

DESCRIPTION
  Gets information (local address) on a given socket.

DEPENDENCIES
  None.

PARAMETER
   sockfd         socket descriptor
  *addr           address of the socket
  *addrlen        address length

RETURN VALUE
   0 if successful.
  -1 if an error occurred.  errno describes the error encountered.

  Errno Values
  ------------
  EBADF                invalid socket descriptor
  EFAULT               invalid addr memory location
                          
SIDE EFFECTS
  None.

===========================================================================*/
int getsockname
(
  int sockfd,
  struct sockaddr *addr,                  
  int *addrlen
);

int getpeername
(
  int sockfd,
  struct sockaddr *addr,                  
  int *addrlen
);

int bsd_get_last_errno(void);

/*===========================================================================

FUNCTION BSD_INIT()

DESCRIPTION
  Opens BSD socket services to an application.  Also calls dss_open_netlib()
  to open the underlying DS Sock network library.

DEPENDENCIES
  None.

PARAMETERS
   default_sigs          structure specifying default signals
   watchdog_sig          signal mask for expiration of watchdog timer
  *watchdog_timer        address of watchdog timer
   watchdog_rpt_time     watchdog report time interval
   watchdog_rpt_mask     report mask that identifies this task to the
                         watchdog task
RETURN VALUE
   0 if successful.
  -1 if an error occurred.

SIDE EFFECTS
  None.

===========================================================================*/
int bsd_init
(
  default_sigs_type* defaults_sigs,
  rex_sigs_type   watchdog_sig,
  rex_timer_type *watchdog_timer,
  rex_timer_cnt_type watchdog_rpt_time,
  unsigned long   watchdog_rpt_mask
);

/*===========================================================================
FUNCTION bsd_reg_usr_notifier()

DESCRIPTION
  Registers a user notifier to be called back when certain events 
  (Qos, Mcast etc.) occur

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void bsd_reg_usr_notifier(bsdcb_fcn_type cb_fcn, void *qos_cb_data);

/*===========================================================================

FUNCTION BSD_SHUTDOWN()

DESCRIPTION
  Frees the resources associated with an application's BSD socket.

DEPENDENCIES
  Must match an earlier bsd_init() call.

PARAMETERS
  *ptr             pointer to a REX task control block 

RETURN VALUE
   0 if no error.
  -1 on error with errno containing the cause of the error.

  Errno Values
  ------------
  EINVAL           invalid parameter value
  EFAULT           not all sockets are closed, call do dss_close_netlib()
                   failed.

SIDE EFFECTS
  None.

===========================================================================*/
int bsd_shutdown
(
  void *ptr
);

/*===========================================================================

FUNCTION BSD_REG_CB()

DESCRIPTION
  Registers a user specified callback function to be called for a given
  signal.

DEPENDENCIES
  None.

PARAMETERS
  signal: Signal for which the callback function is being installed
  clbk_fnc_ptr: Pointer to the callback function
  clbk_fnc_arg: Argument to be passed to the callback function

RETURN VALUE
   0 if successful.
  -1 if an error occurred.

SIDE EFFECTS
  None.

===========================================================================*/
int bsd_reg_cb
(
  rex_sigs_type     signal,          /* signal mask                        */
  signal_clbk_type *clbk_fnc_ptr,    /* Pointer to the BSD app's callback  */
                                     /* function for the associated signal */
  void             *clbk_fnc_arg     /* Callback function's argument       */
);

#ifdef FEATURE_QTV_MCAST
# if defined(FEATURE_QTV_MCAST_IOCTL) && !defined(PLATFORM_LTK)
/*===========================================================================
FUNCTION BSD_ENABLE_MCAST()

DESCRIPTION
  broadcast connection through ioctl instead of setsockopt

DEPENDENCIES
  None.

PARAMETER
  None.

RETURN VALUE
  >= 0: in case of success
  < 0: otherwise

SIDE EFFECTS
  None.
===========================================================================*/
int
bsd_enable_mcast
(
  bsd_mcast_handle_type* mcast_handle,
  struct ip_mcast_info* mcast_info,
  boolean mcast_cb_flag,
  void* mcast_cb_data,
  void* mcast_param_ptr
);

/*===========================================================================
FUNCTION BSD_DISABLE_MCAST()

DESCRIPTION
  dropping broadcast connection through ioctl instead of setsockopt  

DEPENDENCIES
  None.

PARAMETER
  mcast_handle

RETURN VALUE
  >= 0: in case of success
  < 0: otherwise

SIDE EFFECTS
  None.
===========================================================================*/
int
bsd_disable_mcast
(
  bsd_mcast_handle_type mcast_handle
);

/*===========================================================================
FUNCTION print_mcast_event()
   print the mcast DSS event
DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETER
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
print_mcast_event
(
 dss_iface_ioctl_mcast_info_code_enum_type info_code
);

#endif /* defined(FEATURE_QTV_MCAST_IOCTL) && !defined(PLATFORM_LTK) */
/*===========================================================================

FUNCTION OPEN_BCAST_IFACE()

DESCRIPTION
  Selects and opens a broadcast interface

DEPENDENCIES
  None.

PARAMETER
   bcast_iface_name: broadcast interface name

RETURN VALUE
  >= 0: in case of success
  < 0: otherwise

SIDE EFFECTS
  None.

===========================================================================*/
int
open_bcast_iface(bsd_iface_name_enum_type bcast_iface_name,
                 boolean mcastAddrIPv6Type);

/*===========================================================================

FUNCTION CLOSE_BCAST_IFACE()

DESCRIPTION
  Restores the application's network policy to DEFAULT so that any default
  interface is brought up when PPP is brought up the next time around.

DEPENDENCIES
  None.

PARAMETER
  None.

RETURN VALUE
  >= 0: in case of success
  < 0: otherwise

SIDE EFFECTS
  None.

===========================================================================*/
int
close_bcast_iface(void);
#endif /* FEATURE_QTV_MCAST */

#ifdef FEATURE_QTV_NET_POLICY
/*===========================================================================
FUNCTION BSD_INIT_NET_POLICY_INFO

DESCRIPTION
  Initializes the supplied policy info to default values

DEPENDENCIES
  None.

PARAMETER
  bsd_net_policy: the network policy to be initialized to default values

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
bsd_init_net_policy_info
(
  bsd_net_policy_info_type* bsd_net_policy
);

/*===========================================================================

FUNCTION BSD_SET_NET_POLICY

DESCRIPTION
  Sets the network policy for the BSD application as specified by the 
  parameter.

DEPENDENCIES
  None.

PARAMETER
  bsd_net_policy: the network policy specification

RETURN VALUE
  >= 0: in case of success
  < 0: otherwise

SIDE EFFECTS
  None.

===========================================================================*/
int
bsd_set_net_policy
(
  bsd_net_policy_info_type* bsd_net_policy
);

/*===========================================================================

FUNCTION BSD_GET_NET_POLICY

DESCRIPTION
  Gets the network policy for the BSD application as specified by the 
  parameter.

DEPENDENCIES
  None.

PARAMETER
  bsd_net_policy: the network policy specification

RETURN VALUE
  >= 0: in case of success
  < 0: otherwise

SIDE EFFECTS
  None.

===========================================================================*/
int
bsd_get_net_policy
(
  bsd_net_policy_info_type* bsd_net_policy
);
#endif /* FEATURE_QTV_NET_POLICY */

#if defined(FEATURE_QTV_QOS_SELECTION) && !defined(PLATFORM_LTK) 
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

/*===========================================================================
FUNCTION BSDI_PACKET_DROP_INIT()

DESCRIPTION
  Inititize the packet drop rate 

DEPENDENCIES
  None

PARAMETER
  udp_droprate: The configured UDP packet drop rate
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bsd_packet_drop_init
(
  uint8 udp_droprate 
);

#ifndef PLATFORM_LTK
int 
bsd_dss_iface_ioctl
(
   void* arg_ptr,
   bsd_iface_ioctl_type  ioctl_type
);
int 
bsd_dss_iface_ioctl_ex
(
   void* arg_ptr,
   bsd_iface_ioctl_type  ioctl_type ,
   void *data_ptr
);
#endif /* PLATFORM_LTK */

#ifdef __cplusplus
   }
#endif

#endif /* FEATURE_DS_SOCKETS_BSD */
#endif /* DSBSD_H */

