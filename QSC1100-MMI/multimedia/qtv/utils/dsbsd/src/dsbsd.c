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

  bsd_shutdown()
    Ends BSD socket services to an application.

  close()
    Terminates data communication and frees the socket descriptor for reuse.

  connect()
    Initiates a TCP handshake with a remote endpoint address.  Cannot be
    used to specify a peer for sockets that use UDP.

  gethostbyname()
    Gets information on a host given the host name.

  getsockname()
    Gets information (local address) on a given socket.

  inet_addr()
    Takes a string containing a dotted-decimal IP address and returns the
    equivalent IP address in binary form.

  ioctl()
    Sets a socket's I/O behavior to either blocking or nonblocking.

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

$Header: //source/qcom/qct/multimedia/qtv/utils/dsbsd/main/latest/src/dsbsd.c#16 $
$DateTime: 2008/12/08 12:45:14 $
$Change: 800682 $

========================================================================== */

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "AEEstd.h"
#include "target.h"
#include "rex.h"
#include "ran.h"
#ifdef FEATURE_DS_SOCKETS_BSD
#include "assert.h"
#include "dsbsd.h"
#include "dssdns.h"
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "dog.h"
#include <ctype.h>
#include <stdlib.h>

#include "qtv_msg.h"

#define BSD_DORMANCY_WORKAROUND 

#ifdef FEATURE_QTV_DYNAMIC_TCP_CLOSE
#define BSD_TCP_LINGER_TIMEOUT 250
#else
#define BSD_TCP_LINGER_TIMEOUT 0
#endif

#define MAX_NUM_OF_CB_INFO 36

#if defined(DSS_VERSION) && (DSS_VERSION >= 50)
  #define BSD_BACKGROUND_PPPCLOSE 
  #define BSD_DORMANCY_TIMEOUT 200
#else
  #if defined(FEATURE_QTV_NET_POLICY) && !defined(PLATFORM_LTK)
  #error "FEATURE_QTV_NET_POLICY cannot be turned on: incompatible DS API" 
  #endif

  #undef  BSD_BACKGROUND_PPPCLOSE 
  #define BSD_DORMANCY_TIMEOUT 200
#endif /* if DSS_VERSION >= 50 */

#if defined(DSS_VERSION) && (DSS_VERSION >= 900)
  /* DSS version 9xx supports silent close that we use to implement 
   * lingering the way we like -- after a timer expires, TCP is reset
   * immediately.
   */
  #define SILENT_CLOSE_SUPPORTED
#endif   

#ifdef FEATURE_REAL_TCP_SACK
#define TCP_SACK_ENABLED
#endif /* FEATURE_REAL_TCP_SACK */

#ifdef PLATFORM_LTK
#error code not present
#endif /* PLATFORM_LTK */

/*---------------------------------------------------------------------------
   Consider a situation where CM (call manager) brings down a call due to no
   signal causing DS (data services) to go dormant.  At this time, the app
   does a dss_sendto() operation.  DS tries to go out of dormancy, initiates
   a call setup, and flows control the app.  Since there is no signal, the
   attempt fails and DS goes dormant again.  While going dormant, DS flows
   control the app which triggers a DS_WRITE_EVENT for the app.  In the mean
   time dss_sendto() returns DS_ERROR with error code DS_EWOULDBLOCK.

   Now consider a situation where the no signal condition persists and the
   app is waiting on the DS_WRITE_EVENT to make another write attempt.  The
   triggering of the DS_WRITE_EVENT as described above unblocks the app and
   causes it to write again.  However, the condition has not changed and
   dss_sendto() again returns DS_ERROR with error code DS_EWOULDBLOCK.  A
   situation exists where the meaning of DS_WRITE_EVENT is unreliable - that
   is the socket is writable yet when a write attempt to the socket generates
   a DS_EWOULDBLOCK.  DS should return another code other than DS_EWOULDBLOCK
   if the DS_WRITE_EVENT is set.  But this is not the case.  How long this
   cycle lasts is up to DS which may not be acceptable to the app.

   The workaround is to put a cap on the blocking time of sendto(), send(),
   write().  For now, only sendto() needs to have the workaround for Qtv.
   When the blocking time has been reached, sendto() returns -1 with error
   code ENETDOWN.  The question arises whether the cap on the blocking time
   should be app dependent or decided by the BSD layer for all apps.  For
   now, the BSD layer decides the maximum wait time for all apps.
---------------------------------------------------------------------------*/

/*===========================================================================

                             FORWARD DECLARATIONS

===========================================================================*/
static bsdcb_type *bsdi_alloc_bsdcb(void);
static int bsdi_wait(rex_sigs_type *, int, boolean);
static void bsdi_socket_cb_fcn(void *);
static void bsdi_network_cb_fcn(void *);
static void bsdi_dns_cb_fcn(dss_dns_session_mgr_handle_type   session_handle,
                             dss_dns_query_handle_type         query_handle,
                             dss_dns_api_type_enum_type        api_type,
                             uint16                            num_records,
                             void                            * user_data_ptr,
                             int16                             dss_errno);
static dss_hostent  * bsdi_dns_construct_hostent_name_results(
                             int16                           addr_family,
                             char                          * query_data_ptr,
                             dss_dns_addrinfo              * results_buf,
                             uint16                          num_records,
                             int16                         * dss_errno);
static void bsdi_freehostent(struct dss_hostent *phostent);

static int bsdi_pppopen(sint15);
static int bsdi_pppclose(sint15);
static int bsdi_map_errno(sint15);
static int bsdi_map_bsd_dss_sockopt_level ( int level );
static int bsdi_map_bsd_dss_sockopt_name ( int optname );

#ifndef PLATFORM_LTK
static dss_iface_ioctl_type bsdi_map_bsd_dss_ioctl_type (bsd_iface_ioctl_type ioctl_type);

#if defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL)
static void bsdi_iface_ioctl_cb_fcn
(
  dss_iface_ioctl_event_enum_type event,
  dss_iface_ioctl_event_info_union_type event_info,
  void* user_data,
  sint15 app_id,
  dss_iface_id_type iface_id
);

static int bsdi_iface_ioctl_sig_handler(rex_sigs_type sigs, void* user_data);

typedef struct
{
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif
#ifdef FEATURE_QTV_MCAST_IOCTL
  bsd_mcast_handle_type mcast_handle;
#endif
}event_info_type;

static bsd_status_enum_type bsdi_get_event_info(event_info_type *event_info);
#endif /* defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL) */

#ifdef FEATURE_QTV_MCAST
static int bsdi_map_bsd_dss_optval_ip_mreq 
( 
  void* optval, 
  int   optlen,
  IpAddrType* dss_mcast_info
);

static dss_iface_name_enum_type bsdi_map_bsd_dss_bcast_iface_name
(
  bsd_iface_name_enum_type bcast_iface_name
);
#endif /* FEATURE_QTV_MCAST */
#endif /* !PLATFORM_LTK */

/*===========================================================================

                    DEFINITIONS AND VARIABLE DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  General macro to check for pointers that are not supposed to be NULL.  If
  ptr is NULL, ASSERT(0), set errno to errnum, and return a -1.
---------------------------------------------------------------------------*/
#define NULL_CHECK(ptr, errnum) \
  if (ptr == NULL) \
  { \
    errno = errnum; \
    return -1; \
  }

bsdcb_type bsdcb_array[DSS_MAX_APPS] = {{0}};

/*---------------------------------------------------------------------------
  Array of socket descriptors, their I/O characteristic (blocking or non-
  blocking), and their owners.  It is used for mapping a bit position in a
  fd_set type variable to the corresponding socket descriptor as well as for
  looking up a socket's I/O behavior.
---------------------------------------------------------------------------*/
static struct bsd_scb_s
{
  int sockfd;
  int nonblocking_io;
  rex_tcb_type *owning_tcb_ptr;
  /* Timeout in milliseconds for TCP to gracefully close before sending a 
   * reset. Should be 0 for UDP.
   */ 
  int tcp_close_timeout;                                        
} bsd_scb_array[DSS_MAX_SOCKS] = {{0, 0, NULL, 0}};

#ifndef PLATFORM_LTK
#if defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL)
typedef struct
{  
  /* Must be the first element of this structure for qget/qput to work! */
  q_link_type link; 
  boolean InUse;
  dss_iface_ioctl_event_enum_type event;
  dss_iface_ioctl_event_info_union_type event_info;
  void* user_data;
  sint15 app_id;
  dss_iface_id_type iface_id;   
} bsdi_iface_ioctl_cb_info_type;

static void SetInUseFlag(bsdi_iface_ioctl_cb_info_type *);
static void ResetInUseFlag(bsdi_iface_ioctl_cb_info_type *);

q_type bsdi_iface_ioctl_cb_info_q;
#endif /* defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL) */
#endif /* !PLATFORM_LTK */

/*drop rate variable*/
static uint8 udp_drop_rate;
static uint8 udp_packet_drop_index;

/*support functions for random udp packet drop mechanism*/
static boolean udp_drop_packet(void);
static void generate_unique_random_range(int8 *random_store);


/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

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
  EFAULT            invalid BSD control block or dss_setsockopt() error 
  EPROTOTYPE        invalid protocol for the socket type
  ESOCKTNOSUPPORT   invalid or unsupported socket parameter specified
  EMFILE            too many descriptors open.  A socket is already open
                    or has not closed completely.

SIDE EFFECTS
  None.

===========================================================================*/
int socket
(
  int family,
  int type,
  int protocol
)
{
  bsdcb_type   *bsdcb_ptr;            /* BSD control block pointer         */
  sint15        dss_errno;            /* Error code that is to be ignored  */
  sint15        sockfd;               /* Socket file descriptor            */
#ifdef FEATURE_DATA_MM
  so_linger_type so_linger = {1, BSD_TCP_LINGER_TIMEOUT};                                      
                                      /* Socket linger option on close     */
#else
  so_linger_type so_linger = {0, 0};
                                      /* Socket linger option on close     */
#endif /* FEATURE_DATA_MM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  MSG_LOW("App %d to create socket of type %d, protocol %d",
          bsdcb_ptr->netlib_app_id, type, protocol);

  sockfd = dss_socket(bsdcb_ptr->netlib_app_id,
                      (byte)family,
                      (byte)type,
                      (byte)protocol,
                      &errno);

  /*-------------------------------------------------------------------------
    Check to see if the socket was successfully created.
  -------------------------------------------------------------------------*/
  if (sockfd == DSS_ERROR)
  {
    MSG_ERROR("socket() failed with error %d", errno, 0, 0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    If the socket is a TCP socket, set the SO_LINGER option to off so that
    close() returns immediately with the connection gracefully closed in the
    background.  This is the BSD default but not the DS Sock default.  The
    background graceful close is not yet implemented (2/27/2003).  At this
    time, the SO_LINGER is disabled to reduce the 2MSL timeout from 2 seconds
    to 200 ms.
  -------------------------------------------------------------------------*/ 
  if (type == SOCK_STREAM)
  {    
    if (0 != setsockopt(sockfd, SOL_SOCK, SO_LINGER,
                        (void *) &so_linger, sizeof(so_linger)))
    {
      MSG_FATAL("setsockopt() failed with error %d", errno, 0, 0);
      errno = EFAULT;
      (void)close(sockfd);
      return -1;
    }
  }
  else
  {
    // UDP sockets don't need any timer on close.
    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].tcp_close_timeout = 0;
  }

  bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].sockfd = sockfd;
  bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].owning_tcb_ptr = rex_self();
  bsdcb_ptr->sock_ref_count++;
  MSG_MED("Application %d created socket %d",
          bsdcb_ptr->netlib_app_id, sockfd, 0);

  /*-------------------------------------------------------------------------
    Try to establish PPP so that app won't have to block too long later.
  -------------------------------------------------------------------------*/
  (void)dss_pppopen(bsdcb_ptr->netlib_app_id, &dss_errno);

  return sockfd;

} /* socket() */

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
  *localaddr      address of the structure specifying the IP address and
                  port number
   addrlen        size of the above address structure in bytes

RETURN VALUE
   0 if successful.
  -1 if an error occurred.  An error code indicating the cause of the error
     is written to errno.

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
  int sockfd,
  struct sockaddr *localaddr,
  int addrlen
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_QTV_IPV6 
#error code not present
#else
  MSG_LOW("Bind socket %d to address %x port %d",
          sockfd,
          ((struct sockaddr_in *)localaddr)->sin_addr.s_addr,
          ((struct sockaddr_in *)localaddr)->sin_port);
#endif
  if (dss_bind((sint15)sockfd,
               localaddr,
               (uint16)addrlen,
               &errno) == DSS_ERROR)
  {
    return -1;
  }

  return 0;

} /* bind() */

/*===========================================================================

FUNCTION CONNECT()

DESCRIPTION
  Initiates a TCP handshake with a remote endpoint address.  The underlying
  implementation does not support connected UDP sockets.

DEPENDENCIES
  None.

PARAMETERS
   sockfd      socket descriptor
  *addr        pointer to the structure that specifies the IP address and
               port number of the server
   addrlen     size of the server address structure in bytes

RETURN VALUE
   0 if TCP is established.
  -1 if an error occurred.  errno contains a code specifying the cause of
     the error.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  ECONNREFUSED     connection attempt refused
  ETIMEDOUT        connection attempt timed out
  EFAULT           addrlen parameter or BSD control block is invalid
  EISCONN          the socket descriptor specified is already connected
  ENETDOWN         network subsystem not availble.
  ENOTCONN         network subsystem went down during the connect attempt
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
)
{
  sint15 dss_errno;             /* Holds error code that is to be ignored  */
  sint15 ret_val = 0;           /* Return value                            */
  rex_sigs_type sigs;           /* Input/Output signal mask to bsdi_wait() */
  bsdcb_type *bsdcb_ptr;        /* BSD control block pointer               */
  boolean  loop_flag = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  MSG_LOW("Application %d to connect on socket %d",
          bsdcb_ptr->netlib_app_id, sockfd, 0);

  if (bsdi_pppopen(bsdcb_ptr->netlib_app_id) == -1)
  {
    /*-----------------------------------------------------------------------
      Note that getting here does not necessarily mean that there is 
      something wrong with the network. For example, PPP could be in the 
      middle of being torn down from previous session and will come up if 
      the application tries again after a while. It is left up to the 
      application to decide what to do in such cases depending on the errno.
    -----------------------------------------------------------------------*/
    return -1;
  }

#ifdef FEATURE_QTV_IPV6 
#error code not present
#else
  MSG_MED("Connect socket %d to address %x port %d",
          sockfd,
          ((struct sockaddr_in *)addr)->sin_addr.s_addr,
          ((struct sockaddr_in *)addr)->sin_port);
#endif
  do
  {
    (void)rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig | bsdcb_ptr->socket_sig);
    ret_val = dss_connect((sint15)sockfd, addr, (uint16)addrlen, &errno);

    if ((ret_val == DSS_SUCCESS) ||
        ((ret_val == DSS_ERROR) && (errno == DS_EISCONN)))
    {
      MSG_MED("Socket %d connected", sockfd, 0, 0);
      ret_val = 0;
      loop_flag = FALSE;
    }
    else if ((ret_val == DSS_ERROR) &&
             ((errno == DS_EINPROGRESS) || (errno == DS_EWOULDBLOCK)))
    {
      /*---------------------------------------------------------------------
        From the point of view of the caller, EWOULDBLOCK is same as
        EINPROGRESS. In both these cases, the caller needs to block or
        call connect() again.
      ---------------------------------------------------------------------*/
      errno = DS_EWOULDBLOCK;

      /*---------------------------------------------------------------------
        For a blocking socket, wait for TCP to get established or a network
        down event.  Note:  For a nonblocking socket, errno must remain
        EWOULDBLOCK.  Therefore, do not pass &errno to dss_async_select()
        where errno's value gets clobbered.
      ---------------------------------------------------------------------*/
      if (dss_async_select((sint15)sockfd,
                           DS_WRITE_EVENT,
                           &dss_errno) == DSS_ERROR)
      {
        errno = dss_errno;
        return -1;
      }

      sigs = 0;
      if (bsdi_wait(&sigs,
                    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].nonblocking_io, 
                    FALSE) == -1)
      {
        /*-------------------------------------------------------------------
          Two possibilities why bsdi_wait() returned an error.  The socket is
          is nonblocking.  Or, the socket is blocking and during the wait, a
          a signal occurred that caused an application callback function to
          get called.  The callback function returned an error.
        -------------------------------------------------------------------*/
        ret_val = -1;
        loop_flag = FALSE;
      }
    }
    else
    {
      MSG_ERROR("Connect on sock %d failed with error %d", sockfd, errno, 0);
      ret_val = -1;
      loop_flag = FALSE;
    }
 
  } while (loop_flag);

  (void)dss_async_deselect((sint15)sockfd, DS_WRITE_EVENT, &dss_errno);

  return ret_val;

} /* connect() */

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
   0 if successful.
  -1 if an error occurred.  errno describes the error encountered.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specified
  EFAULT           internal DS Sock error or invalid BSD control block
  EINTR            BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
int close
(
  int sockfd
)
{
  int ret_val;                  /* Return value                            */
  sint15 dss_errno;             /* Holds error code that is to be ignored  */
  rex_sigs_type sigs;           /* Input/Output signal mask to bsdi_wait() */
#ifdef SILENT_CLOSE_SUPPORTED
  int tcp_close_timeout;        /* Millis to wait before reseting          */
  rex_timer_type tcp_close_timer; /* Timer for graceful TCP close          */
#endif /* SILENT_CLOSE_SUPPORTED */
  bsdcb_type *bsdcb_ptr;        /* BSD control block pointer               */
  sint15 dss_sockfd = sockfd;   /* sint15 version of socket descriptor     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  MSG_LOW("Application %d closing socket %d",
          bsdcb_ptr->netlib_app_id, sockfd, 0);

  if (dss_async_deselect((sint15)sockfd,
                         DS_WRITE_EVENT | DS_READ_EVENT,
                         &errno) == DSS_ERROR)
  {
    return -1;
  }

  /* Initialize a timer for TCP closing. */
#ifdef SILENT_CLOSE_SUPPORTED
  tcp_close_timeout = bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].tcp_close_timeout;

  if (tcp_close_timeout > 0)
  {
     rex_def_timer(&tcp_close_timer, bsdcb_ptr->owning_tcb_ptr,
                   bsdcb_ptr->tcp_close_timer_sig);
   
     (void)rex_set_timer(&tcp_close_timer, (rex_timer_cnt_type) tcp_close_timeout);
  }
#endif /* SILENT_CLOSE_SUPPORTED */

  ret_val = dss_close((sint15)sockfd, &errno);

  if (ret_val == DSS_SUCCESS)
  {
    MSG_MED("Socket %d closed", sockfd, 0, 0);
    ret_val = 0;
  }
  else if (errno == DS_EWOULDBLOCK)
  {
    /*-----------------------------------------------------------------------
      TCP close in progress.
    -----------------------------------------------------------------------*/
    while (1)
    {
      (void)rex_clr_sigs(rex_self(),
                   bsdcb_ptr->socket_sig | bsdcb_ptr->network_sig);
      if (dss_async_select((sint15)sockfd,
                           DS_CLOSE_EVENT,
                           &errno) == DSS_ERROR)
      {
        ret_val = -1;
        goto CloseCleanup;
      }

#ifdef SILENT_CLOSE_SUPPORTED
      sigs = bsdcb_ptr->tcp_close_timer_sig;
#else
      sigs = 0;
#endif /* SILENT_CLOSE_SUPPORTED */

      if (bsdi_wait(&sigs, 0, TRUE) == -1 && errno != EINTR)
      {
        /*-------------------------------------------------------------------
          During the wait, a signal occurred that caused an application
          callback function to get called.  The function returned an error.
        -------------------------------------------------------------------*/
        (void)dss_async_deselect((sint15)sockfd, DS_CLOSE_EVENT, &dss_errno);

        ret_val = -1;
        goto CloseCleanup;
      }

#ifdef SILENT_CLOSE_SUPPORTED
      /*---------------------------------------------------------------------
        See if we timed out. If so, set the silent close option. The next close
        should finish closing.
      ---------------------------------------------------------------------*/
      if ((tcp_close_timeout > 0) &&
          ((sigs & bsdcb_ptr->tcp_close_timer_sig) != 0))
      {
        int on = 1;
        uint32 size = sizeof(int);

        if (dss_setsockopt(sockfd, DSS_SOCK, DSS_SO_SILENT_CLOSE, &on, &size,
                           &dss_errno) == DSS_ERROR)
        {
            MSG_ERROR("Can't set silent close option on socket %d!", sockfd, 0, 0);
        }

        (void)rex_clr_sigs(rex_self(), bsdcb_ptr->socket_sig | bsdcb_ptr->tcp_close_timer_sig);
        break;        
      }
#endif /* SILENT_CLOSE_SUPPORTED */
        
      /*---------------------------------------------------------------------
        Check for a socket close event.
      ---------------------------------------------------------------------*/
      if ((ret_val = dss_getnextevent(bsdcb_ptr->netlib_app_id,
                                      &dss_sockfd,
                                      &dss_errno)) == DSS_ERROR)
      {
        MSG_ERROR("dss_getnextevent() failed! error %d", dss_errno, 0, 0);
        errno = bsdi_map_errno(dss_errno);
        ret_val = -1;
        goto CloseCleanup;
      }
      
      if (ret_val & DS_CLOSE_EVENT)
      {
        (void)rex_clr_sigs(rex_self(), bsdcb_ptr->socket_sig);
        break;
      }
    } /* while (1) */

    /*-----------------------------------------------------------------------
      Close again and it should succeed.
    -----------------------------------------------------------------------*/
    if (dss_close((sint15)sockfd, &errno) == DSS_SUCCESS)
    {
      MSG_MED("Socket %d closed", sockfd, 0, 0);
      ret_val = 0;
    }
    else
    {
      MSG_ERROR("Close error %d on socket %d", errno, sockfd, 0);
      ret_val = -1;
    }  
  }
  else
  {
    /*-----------------------------------------------------------------------
      Serious error.  Should always be able to close a socket.
    -----------------------------------------------------------------------*/
    MSG_ERROR("Close error %d on socket %d", errno, sockfd, 0);
    ret_val = -1;
  }

  if (ret_val == 0)
  {
    /*-----------------------------------------------------------------------
      Socket closed without error.
    -----------------------------------------------------------------------*/
    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].sockfd = 0;
    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].owning_tcb_ptr = NULL;
    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].nonblocking_io = 0;
    if (--bsdcb_ptr->sock_ref_count == 0)
    {
      /*---------------------------------------------------------------------
        Just closed last socket for this application.  Close PPP just in case
        this is the only application.  The underlying DS Sock layer keeps PPP
        up if there are other applications that need it.
      ---------------------------------------------------------------------*/
      (void)bsdi_pppclose(bsdcb_ptr->netlib_app_id);
    }
  }

CloseCleanup:

#ifdef SILENT_CLOSE_SUPPORTED
  if (tcp_close_timeout > 0)
  {
    (void)rex_clr_timer(&tcp_close_timer);
  }
#endif /* SILENT_CLOSE_SUPPORTED */

  return ret_val;

} /* close() */

/*===========================================================================

FUNCTION WRITE()

DESCRIPTION
  Writes data to a socket descriptor for transfer to a remote machine over
  TCP.

DEPENDENCIES
  None.

PARAMETERS
   sockfd     socket descriptor
  *buf        address of the buffer containing the data
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
  EFAULT           DS Sock internal error or invalid BSD control block
  EINTR            BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
int write
(
  int   sockfd,
  const void *buf,
  unsigned int len
)
{
  sint15 dss_errno;                  /* Holds error code that is ignored   */
  sint15 num_bytes;                  /* Bytes written per dss_write() call */
  unsigned int bytes_written = 0;    /* Sum of bytes written               */
  int ret_val = len;                 /* Return value                       */
  rex_sigs_type sigs;                /* In/Out signal mask to bsdi_wait()  */
  bsdcb_type *bsdcb_ptr;             /* BSD control block pointer          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  if (bsdi_pppopen(bsdcb_ptr->netlib_app_id) == -1)
  {
    /*-----------------------------------------------------------------------
      No PPP.  bsdi_pppopen() sets errno to the cause of the error.
    -----------------------------------------------------------------------*/
    return -1;
  }

  MSG_LOW("App %d writes %d bytes to socket %d",
          bsdcb_ptr->netlib_app_id, len, sockfd);

  do
  {
    num_bytes = dss_write((sint15)sockfd,
                          (byte *)buf + bytes_written,
                          (uint16)(len - bytes_written),
                          &errno);

    if (num_bytes >= 0)
    {
      MSG_MED("%d bytes written to socket %d", num_bytes, sockfd, 0);
      bytes_written += num_bytes;
      ret_val = bytes_written; 
    }
    else if (errno == DS_EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        For a blocking socket, wait for signals that indicate the socket is
        write-ready, TCP closed or network went down.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs(rex_self(),
                   bsdcb_ptr->socket_sig | bsdcb_ptr->network_sig);

      /*---------------------------------------------------------------------
        Note:  For a nonblocking socket, errno must remain EWOULDBLOCK.
        Therefore, do not pass &errno to dss_async_select() where errno's
        value gets clobbered.
      ---------------------------------------------------------------------*/
      if (dss_async_select((sint15)sockfd,
                           DS_WRITE_EVENT | DS_CLOSE_EVENT,
                           &dss_errno) == DSS_ERROR)
      {
        errno = dss_errno;
        return -1;
      }

      sigs = 0;
      if (bsdi_wait(&sigs,
                    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].nonblocking_io,
                    FALSE) == -1)
      {
        /*-------------------------------------------------------------------
          Two possibilities why bsdi_wait() returned an error.  The socket is
          is nonblocking.  Or, the socket is blocking and during the wait, a
          a signal occurred that caused an application callback function to
          get called.  The callback function returned an error.  If the
          buffer was partially written, return the number of bytes written.
          Otherwise, return -1.
        -------------------------------------------------------------------*/
        ret_val = ((int)bytes_written > 0) ? (int)bytes_written : -1;
        break;
      }

      /*---------------------------------------------------------------------
        Got either a network event OR a close or write socket event.  Let
        dss_write() figure out which of these happened.
      ---------------------------------------------------------------------*/
    }
    else
    {
      MSG_ERROR("Write error %d on socket %d", errno, sockfd, 0);
      ret_val = -1;
      break;
    }
  } while (bytes_written != len);

  (void)dss_async_deselect((sint15)sockfd,
                     DS_WRITE_EVENT | DS_CLOSE_EVENT,
                     &dss_errno);

  return ret_val;

} /* write() */

/*===========================================================================

FUNCTION SEND()

DESCRIPTION
  Sends data to a remote machine via a socket that uses TCP.

DEPENDENCIES
  None.

PARAMETERS
   sockfd     socket descriptor
  *buf        address of the buffer containing the data
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
  EFAULT           DS Sock internal error or invalid BSD control block
  EOPNOTSUPP       unsupported flags parameter is specified
  EINTR            BSD application's callback function encountered an error

  Note that an error may have occurred after a partial send.  In such a cases
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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (flags != 0)
  {
    errno = EOPNOTSUPP;
    return -1;
  }
  return write(sockfd, buf, len);

} /* send() */

/*===========================================================================

FUNCTION SENDTO()

DESCRIPTION
  Sends a message to a remote machine via a socket that uses UDP.

DEPENDENCIES
  None.

PARAMETERS
   sockfd     socket descriptor
  *buf        address of the buffer containing the data
   len        number of bytes in the buffer
   flags      0 (not supported)
  *toaddr     pointer to the destination address structure
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
  EFAULT           invalid address length, DS Sock internal error or invalid
                   BSD control block
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
)
{
  sint15 dss_errno;                 /* Holds error code that is ignored    */
  sint15 num_bytes;                 /* Bytes written per dss_sendto() call */
  int bytes_written = 0;            /* Sum of bytes written                */
  int ret_val = len;                /* Return value                        */
  rex_sigs_type sigs;               /* In/Out signal mask to bsdi_wait()   */
  bsdcb_type *bsdcb_ptr;            /* BSD control block pointer           */
#ifdef BSD_DORMANCY_WORKAROUND
  int wait_timer_set = 0;           /* Timer state (set or not set)        */ 
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  if (bsdi_pppopen(bsdcb_ptr->netlib_app_id) == -1)
  {
    /*-----------------------------------------------------------------------
      No PPP.  bsdi_pppopen() sets errno to the cause of the error.
    -----------------------------------------------------------------------*/
    return -1;
  }

  MSG_LOW("App %d sendtos %d bytes to socket %d",
          bsdcb_ptr->netlib_app_id, len, sockfd);

  do
  {
    num_bytes = dss_sendto((sint15)sockfd,
                           (byte *)buf + bytes_written,
                           (uint16)(len - bytes_written),
                           0,
                           toaddr,
                           (uint16)tolen,
                           &errno);
    if (num_bytes >= 0)
    {
#ifndef PLATFORM_LTK
      MSG_MED("%d bytes written to socket %d", num_bytes, sockfd, 0);
#endif
      bytes_written += num_bytes;
      ret_val = bytes_written;
    }
    else if (errno == DS_EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        For a blocking socket, wait for signals that indicate socket is
        write-ready or network went down.
      ---------------------------------------------------------------------*/

#ifdef BSD_DORMANCY_WORKAROUND
      /*---------------------------------------------------------------------
        Use the select timer to put a cap on how long sendto() will block.
      ---------------------------------------------------------------------*/
      if (wait_timer_set == 0)
      {
        (void)rex_clr_sigs(rex_self(), bsdcb_ptr->select_timer_sig);
        (void)rex_set_timer(&bsdcb_ptr->select_timer, BSD_DORMANCY_TIMEOUT);
        wait_timer_set = 1;
      }
#endif

      (void)rex_clr_sigs(rex_self(),
                   bsdcb_ptr->socket_sig | bsdcb_ptr->network_sig);

      /*---------------------------------------------------------------------
        Note:  For a nonblocking socket, errno must remain EWOULDBLOCK.
        Therefore, do not pass &errno to dss_async_select() where errno's
        value gets clobbered.
      ---------------------------------------------------------------------*/
      if (dss_async_select((sint15)sockfd,
                           DS_WRITE_EVENT,
                           &dss_errno) == DSS_ERROR)
      {
        errno = dss_errno;
#ifdef BSD_DORMANCY_WORKAROUND
        (void)rex_clr_timer(&bsdcb_ptr->select_timer);
        (void)rex_clr_sigs(rex_self(), bsdcb_ptr->select_timer_sig);
#endif
        return -1;
      }

      sigs = 0;
      if (bsdi_wait(&sigs,
                    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].nonblocking_io, 
                    FALSE) == -1)
      {
        /*-------------------------------------------------------------------
          Two possibilities why bsdi_wait() returned an error.  The socket is
          is nonblocking.  Or, the socket is blocking and during the wait, a
          a signal occurred that caused an application callback function to
          get called.  The callback function returned an error.  If the
          buffer was partially written, return the number of bytes written.
          Otherwise, return -1.
        -------------------------------------------------------------------*/
        ret_val = (bytes_written > 0) ? bytes_written : -1;
        break;
      }

      /*---------------------------------------------------------------------
        Either a network event or a write socket event occurred.  The BSD
        layer need not check which of these events unblocked the wait.  Loop
        to call dss_sendto() and let dss_sendto() figure out whether it is
        OK to write or if the network went down.
      ---------------------------------------------------------------------*/

#ifdef BSD_DORMANCY_WORKAROUND
      /*---------------------------------------------------------------------
        The select timer signal may also have occurred.  If so, then sendto()
        has blocked too long and it is time to return -1 with error code
        EWOULDBLOCK to the app , signaling app about the dormancy 
      ---------------------------------------------------------------------*/
      if (sigs & bsdcb_ptr->select_timer_sig)
      {
        errno = EWOULDBLOCK;
        ret_val = -1;
        MSG_ERROR("Sendto error %d on socket %d", errno, sockfd, 0);
        break;
      }
#endif
    }
    else
    {
      MSG_ERROR("Sendto error %d on socket %d", errno, sockfd, 0);
      ret_val = -1;
      break;
    }
  } while (bytes_written != len);

  (void)dss_async_deselect((sint15)sockfd, DS_WRITE_EVENT, &dss_errno);

#ifdef BSD_DORMANCY_WORKAROUND
  if (wait_timer_set)
  {
    (void)rex_clr_timer(&bsdcb_ptr->select_timer);
    (void)rex_clr_sigs(rex_self(), bsdcb_ptr->select_timer_sig);
  }
#endif

  return ret_val;

} /* sendto() */

/*===========================================================================

FUNCTION READ()

DESCRIPTION
  Reads data from a socket that uses TCP.  

DEPENDENCIES
  None.

PARAMETERS
   sockfd     socket descriptor
  *buf        address of the buffer to put the data
   len        size of the buffer in bytes 

RETURN VALUE
  The number of bytes read or -1 on failure.  When read() fails, errno is set
  to one of the following values.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  ENOTCONN         socket not connected
  ECONNRESET       TCP connection reset by server
  ECONNABORTED     TCP connection aborted due to timeout or other failure
  ENETDOWN         network subsystem went down
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           DS Sock internal error or invalid BSD control block
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
  unsigned int len
)
{
  sint15 dss_errno;                   /* Holds error code that is ignored  */
  int    ret_val;                     /* Return value/bytes read           */
  rex_sigs_type sigs;                 /* In/Out signal mask to bsdi_wait() */
  bsdcb_type *bsdcb_ptr;              /* BSD control block pointer         */
  boolean  loop_flag = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  MSG_LOW("Application %d reads from socket %d",
          bsdcb_ptr->netlib_app_id, sockfd, 0);

  if (len == 0)
  {
    errno = 0;
    return 0;
  }

  if (bsdi_pppopen(bsdcb_ptr->netlib_app_id) == -1)
  {
    /*-----------------------------------------------------------------------
      No PPP.  bsdi_pppopen() sets errno to the cause of the error.
    -----------------------------------------------------------------------*/
    return -1;
  }

  do
  {
    ret_val = dss_read((sint15)sockfd, buf, (uint16)len, &errno);

    if (ret_val >= 0)
    {
      MSG_MED("Read %d bytes from socket %d", ret_val, sockfd, 0);
      loop_flag = FALSE;
    }
    else if (errno == DS_EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        Set up to wait for a network event or a close/read socket event.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs(rex_self(),
                   bsdcb_ptr->socket_sig | bsdcb_ptr->network_sig);

      /*---------------------------------------------------------------------
        Note:  For a nonblocking socket, errno must remain EWOULDBLOCK.
        Therefore, do not pass &errno to dss_async_select_deselect() where
        errno's value gets clobbered.
      ---------------------------------------------------------------------*/
      if (dss_async_select_deselect(sockfd,
                                    DS_READ_EVENT | DS_CLOSE_EVENT,
                                    DS_WRITE_EVENT,
                                    &dss_errno) == DSS_ERROR)
      {
        errno = dss_errno;
        return -1;
      }

      sigs = 0;
      if (bsdi_wait(&sigs,
                    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].nonblocking_io,
                    FALSE ) == -1)
      {
        /*-------------------------------------------------------------------
          Two possibilities why bsdi_wait() returned an error.  The socket is
          is nonblocking.  Or, the socket is blocking and during the wait, a
          a signal occurred that caused an application callback function to
          get called.  The callback function returned an error.
        -------------------------------------------------------------------*/
        ret_val = -1;
        loop_flag = FALSE;
      }

      /*---------------------------------------------------------------------
        Either a network event or a close/read socket event occurred.  The
        BSD layer need not check which of these events unblocked the wait.
        Loop to call dss_read() again and let dss_read() figure out whether
        there is data to read or if the network went down.
      ---------------------------------------------------------------------*/
    }
    else
    {
      MSG_ERROR("Read error %d on socket %d", errno, sockfd, 0);
      loop_flag = FALSE;
    }

  } while (loop_flag);

  (void)dss_async_deselect((sint15)sockfd,
                     DS_READ_EVENT | DS_CLOSE_EVENT,
                     &dss_errno);
  return ret_val;

} /* read() */

/*===========================================================================

FUNCTION RECV()

DESCRIPTION
  Receives data from a TCP socket

DEPENDENCIES
  None.

PARAMETERS
   sockfd     socket descriptor
  *buf        address of the buffer to put the data
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
  EFAULT           DS Sock internal error or invalid BSD control block
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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (flags != 0)
  {
    errno = EOPNOTSUPP;
    return -1;
  }
  return read(sockfd, buf, len);

} /* recv() */

/*===========================================================================

FUNCTION RECVFROM()

DESCRIPTION
  Attempts to read a message from a UDP socket.  It blocks until a message
  is available or an error occurred.

DEPENDENCIES
  None.

PARAMETERS
   sockfd        socket descriptor
  *buf           address of the buffer to put the message
   len           size of the buffer in bytes
   flags         0 (not supported)
  *fromaddr      pointer to the sender's address
  *fromlen       length of the sender's address in bytes

RETURN VALUE
  The number of bytes read or -1 on failure.  When recvfrom() fails, errno
  is set to one of the following.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  EAFNOSUPPORT     address family not supported
  ENETDOWN         network subsystem went down
  ENONET           attempt to bring up PPP failed - network subsystem down
  EFAULT           DS Sock internal error, invalid BSD control block
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
)
{
  sint15 dss_errno;                   /* Holds error code that is ignored  */
  int    ret_val;                     /* Return value                      */
  rex_sigs_type sigs;                 /* In/Out signal mask to bsdi_wait() */
  bsdcb_type *bsdcb_ptr;              /* BSD control block pointer         */
  uint16 dss_fromlen = 0;             /* uint16 version of fromlen         */ 
  boolean  loop_flag = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  MSG_LOW("Application %d recvfroms from socket %d",
          bsdcb_ptr->netlib_app_id, sockfd, 0);

  if (len == 0)
  {
    errno = 0;
    return 0;
  }

  if (bsdi_pppopen(bsdcb_ptr->netlib_app_id) == -1)
  {
    /*-----------------------------------------------------------------------
      No PPP.  bsdi_pppopen() sets errno to the cause of the error.
    -----------------------------------------------------------------------*/
    return -1;
  }

  if (fromlen != NULL)
  {
    dss_fromlen = *fromlen;
  }

  do
  {
    ret_val = dss_recvfrom((sint15)sockfd,
                           buf,
                           (uint16)len,
                           (uint16)flags,
                           fromaddr,
                           &dss_fromlen,
                           &errno);

    if (fromlen != NULL)
    {
      *fromlen = dss_fromlen;
    }

    if (ret_val > 0)
    {
#ifndef PLATFORM_LTK
      MSG_MED("Read %d bytes from socket %d", ret_val, sockfd, 0);
#endif
      /*check whether we need to drop any packets*/
      if (udp_drop_rate > 0)
      {
        if (udp_drop_packet())
        {
          /*drop this packet*/
          ret_val=0;
        }	
      }
      loop_flag = FALSE;
    }
    else if (errno == DS_EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        Set up to wait for a network event or a read socket event.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs(rex_self(),
                   bsdcb_ptr->socket_sig | bsdcb_ptr->network_sig);

      /*---------------------------------------------------------------------
        Note:  For a nonblocking socket, errno must remain EWOULDBLOCK.
        Therefore, do not pass &errno to dss_async_select_deselect() where
        errno's value gets clobbered.
      ---------------------------------------------------------------------*/
      if (dss_async_select_deselect(sockfd,
                                    DS_READ_EVENT,
                                    DS_WRITE_EVENT,
                                    &dss_errno) == DSS_ERROR)
      {
        errno = dss_errno;
        return -1;
      }

      sigs = 0;
      if (bsdi_wait(&sigs,
                    bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].nonblocking_io,
                    FALSE) == -1)
      {
        /*-------------------------------------------------------------------
          Two possibilities why bsdi_wait() returned an error.  The socket is
          is nonblocking.  Or, the socket is blocking and during the wait, a
          a signal occurred that caused an application callback function to
          get called.  The callback function returned an error.
        -------------------------------------------------------------------*/
        ret_val = -1;
        loop_flag = FALSE;
      }

      /*---------------------------------------------------------------------
        Either a network event or a read socket event occurred.  The BSD
        layer need not check which of these events unblocked the wait.  Loop
        to call dss_recvfrom() again and let dss_recvfrom() figure out
        whether there is data to read or if the network went down.
      ---------------------------------------------------------------------*/
    }
    else
    {
      MSG_ERROR("Recvfrom error %d on socket %d", errno, sockfd, 0);
      /*---------------------------------------------------------------------
        In case dss_recvfrom() returns 0, set ret_val to -1.
      ---------------------------------------------------------------------*/
      ret_val = -1;
      loop_flag = FALSE;
    }

  } while (loop_flag);

  (void)dss_async_deselect((sint15)sockfd, DS_READ_EVENT, &dss_errno);
  return ret_val;

} /* recvfrom() */
/*===================================================================================
FUNCTION UDP_DROP_PACKET()

DESCRIPTION
  Generate the unique number range between 1 to 100 and compare with the configured drop rate .if 
  the number is less then the drop rate then drop the packet.

DEPENDENCIES
  None

PARAMETERS
   None

RETURN VALUE
  boolean value

SIDE EFFECTS
  None
==============================================================================*/
boolean udp_drop_packet(void)
{
  static int8 random_store[100]; 
  boolean status = FALSE; 

  if (udp_packet_drop_index == 0)
  {
    generate_unique_random_range(random_store);
  }

  if (random_store[udp_packet_drop_index] <= (int)udp_drop_rate )
  {
    //drop the udp packet
    MSG_MED("Udp DropRate = %d; Dropping a packet at index: %d\n", 
             udp_drop_rate,udp_packet_drop_index, 0);
    status = TRUE;
  }

  udp_packet_drop_index++; 
  udp_packet_drop_index %= 100;
  return status;
}

/*===========================================================================

FUNCTION SELECT()

DESCRIPTION
  Waits on a list of socket descriptors and unblocks when one of them become
  ready.  A maximum timeout value can also be specified.

DEPENDENCIES
  None.

PARAMETERS
   n              any value (not supported)
  *readfds        pointer to a set of socket descriptors to be monitored for
                  input
  *writefds       pointer to a set of socket descriptors to be monitored for
                  output
  *exceptfds      NULL (not supported)
  *timeout        address of a structure specifying maximum time to wait.  If
                  NULL, the wait is infinite.  If the contents is 0, select()
                  merely polls the socket descriptors.

RETURN VALUE
  Number of ready socket descriptors or -1 if an error occurred.  When
  select() encounters an error, errno is set to one of the following.

  Errno Values
  ------------
  EOPNOTSUPP       option not supported
  EINTR            BSD application's callback function encountered an error
  EFAULT           invalid BSD control block
  EBADF            internal DS Sock error

SIDE EFFECTS
  None.

===========================================================================*/
int select
(
  int     n,
  fd_set *readfds,
  fd_set *writefds,
  fd_set *exceptfds,
  struct timeval *timeout
)
{
  sint15 dss_errno;                  /* Holds error code that is ignored   */
  int    num_ready = 0;              /* Number of socket descriptors ready */
  rex_sigs_type sigs;                /* In/Out signal mask to bsdi_wait()  */
  rex_timer_cnt_type milli_secs = 0; /* select() timeout value             */
  bsdcb_type *bsdcb_ptr;             /* BSD control block pointer          */
  fd_set validfds;                   /* Bitmap of sockets owned by task    */
  rex_tcb_type *tcb_ptr = rex_self();/* Pointer to task's control block    */
  int i;                             /* Loop index                         */
  boolean  loop_flag = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)tcb_ptr->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  MSG_LOW("Application %d selects", bsdcb_ptr->netlib_app_id, 0, 0);

  if (exceptfds != NULL)
  {
    errno = EOPNOTSUPP;
    return -1;
  }

  (void)rex_clr_sigs(tcb_ptr, bsdcb_ptr->network_sig |
                        bsdcb_ptr->socket_sig |
                        bsdcb_ptr->select_timer_sig);

  /*-------------------------------------------------------------------------
    Set up the underlying DS Sock code to notify the BSD to DS layer when
    sockets of interest have data and/or can accept data.
  -------------------------------------------------------------------------*/
  FD_ZERO(&validfds);
  for (i = 0; i < FD_SET_SIZE; i++)
  {
    if (bsd_scb_array[i].owning_tcb_ptr == tcb_ptr)
    {
      /*---------------------------------------------------------------------
        The calling task owns (created) the socket that maps to bit i.
        Indicate this by setting bit i in validfds.  The validfds bitmap is
        used to determine whether READ/WRITE events can be deselected on a
        socket when its corresponding bit in readfds/writefds is 0.
      ---------------------------------------------------------------------*/
      _SET(i, &validfds);
    }
  }
  if (dss_multiple_async_select_deselect(readfds,
                                         writefds,
                                         &validfds,
                                         &errno) == -1)
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Start the select() timer if the application specified a nonzero maximum
    time for select() to block until a socket descriptor becomes ready.
  -------------------------------------------------------------------------*/
  if (timeout != NULL)
  {
    milli_secs = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;

    if (milli_secs != 0)
    {
      (void)rex_set_timer(&bsdcb_ptr->select_timer, milli_secs);
    }
  }

  /*-------------------------------------------------------------------------
    Check whether select() should just poll, block infinitely or block for
    milli_secs milliseconds for any of the specified descriptors to become
    ready.
  -------------------------------------------------------------------------*/
  if (timeout == NULL || milli_secs != 0)
  {
    wait_loop:
    do
    {
      /*---------------------------------------------------------------------
        Petting the watchdog and calling the app's callback functions are
        handled within the function bsdi_wait().
      ---------------------------------------------------------------------*/
      sigs = 0;
      
      if (bsdi_wait(&sigs, 0, FALSE) == -1)
      {
        /*-------------------------------------------------------------------
          bsdi_wait() unblocked due to an application callback function
          returning with an error or a BSD registered signal without a
          corresponding callback function occurred.
        -------------------------------------------------------------------*/
         num_ready = -1;
         loop_flag = FALSE;
         break;
      }

      if (sigs & bsdcb_ptr->network_sig)
      {
        (void)dss_netstatus(bsdcb_ptr->netlib_app_id, &dss_errno);
        if (dss_errno != DS_ENETISCONN)
        {
          errno = ENETDOWN;
          bsdcb_ptr->network_up = 0;
          num_ready = -1;
          loop_flag = FALSE;
          break;
        }
      }

      if (sigs & bsdcb_ptr->socket_sig)
      {
        (void)rex_clr_sigs(tcb_ptr, bsdcb_ptr->socket_sig);
        loop_flag = FALSE;
        break;
      }

      if (sigs & bsdcb_ptr->select_timer_sig)
      {
        /*-------------------------------------------------------------------
          Select timer expired before a socket signal occurred.  This implies
          0 sockets of interest are ready.  Clear the select timer signal and
          return num_ready (0).  As a shortcut, the socket bit arrays are not
          reset.
        -------------------------------------------------------------------*/
        (void)rex_clr_sigs(tcb_ptr, bsdcb_ptr->select_timer_sig);
        return 0;
      }

    } while(loop_flag);

    /*-----------------------------------------------------------------------
      Exit the select wait NOT due to the select timer expiring.  If
      num_ready is set to -1, it means that an error has occurred.
    -----------------------------------------------------------------------*/
    if (num_ready == -1)
    {
      /*---------------------------------------------------------------------
        Clear the select timer and its signal if the timer was started.
      ---------------------------------------------------------------------*/
      if (milli_secs != 0)
      {
        (void)rex_clr_timer(&bsdcb_ptr->select_timer);
        (void)rex_clr_sigs(tcb_ptr, bsdcb_ptr->select_timer_sig);
      } 
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Execution arrives here because a socket became ready or the timeout is 0
    (poll).  Now check if any of the socket descriptors the application wants
    monitored for read and/or write readiness are indeed ready.
  -------------------------------------------------------------------------*/
  num_ready = dss_get_sockets_events(bsdcb_ptr->netlib_app_id,
                                     readfds,
                                     writefds,
                                     &dss_errno);

  if (num_ready == 0 && (milli_secs != 0 || timeout == NULL))
  {
    /*-----------------------------------------------------------------------
      This is the situation where a DS_WRITE_EVENT occurred to signal the
      failed progress of a TCP connection.  bsdi_wait() unblocks due to the
      DS_WRITE_EVENT from DS Sock, but dss_get_socket_events() reports 0
      descriptor ready because a TCP socket is not writeable if it is not
      connected.  Since this is not a poll, reblock as if the DS_WRITE_EVENT
      did not happen.
    -----------------------------------------------------------------------*/
    goto wait_loop;
  }

  if (num_ready == -1)
  {
    errno = bsdi_map_errno(dss_errno);
  }

  /*-------------------------------------------------------------------------
    Clear the select timer and its signal if the timer was started.
  -------------------------------------------------------------------------*/
  if (milli_secs != 0)
  {
    (void)rex_clr_timer(&bsdcb_ptr->select_timer);
    (void)rex_clr_sigs(tcb_ptr, bsdcb_ptr->select_timer_sig);
  }

  return num_ready;

} /* select() */

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
unsigned long inet_addr(const char *addr)
{
  struct in_addr tmp_addr;

  if (dss_inet_aton(addr, &tmp_addr) == DSS_ERROR)
   {
      return INADDR_NONE;
   }
  return (tmp_addr.s_addr);
}

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
)
{
  int ret_val = 0;                                         /* Return value */
  int i;                                                   /* Loop index   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Find the position of the sock descriptor in the bsd_scb_array[].
  -------------------------------------------------------------------------*/
  for (i = 0; i < DSS_MAX_SOCKS; i++)
  {
    if (bsd_scb_array[i].sockfd == sockfd)
    {
      break;
    }
  }

  if (i != DSS_MAX_SOCKS)
  {  
    switch (request)
    {
      case FIONBIO:
        /*-------------------------------------------------------------------
          Set the socket for nonblocking I/O.
        -------------------------------------------------------------------*/
        if (*arg_ptr != 0)
        {
          bsd_scb_array[i].nonblocking_io = 1;
        }
        else
        {
          bsd_scb_array[i].nonblocking_io = 0;
        }
        break;

      default:
        if (((bsdcb_type *)(rex_self())->bsdcb_ptr) != NULL)
        {
          errno = EINVAL;
        }
        ret_val = -1;
    }
  }
  else
  {
    if (((bsdcb_type *)(rex_self())->bsdcb_ptr) != NULL)
    {
      errno = EBADF;
    }
    ret_val = -1;
  }
  return ret_val;

} /* ioctl() */


#ifdef FEATURE_QTV_IPV6
#error code not present
#endif

/*===========================================================================

FUNCTION GETHOSTBYNAME2()

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
#ifdef FEATURE_QTV_IPV6
#error code not present
#else
struct hostent *gethostbyname
(
  const char *name
)
#endif
{
  bsdcb_type *bsdcb_ptr;              /* BSD control block pointer         */
  rex_sigs_type sigs;           /* Input/Output signal mask to bsdi_wait() */ 
#ifndef FEATURE_QTV_IPV6
  int addr_family = AF_INET;
#endif
  dss_dns_session_mgr_handle_type  session_handle = DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  bsd_net_policy_info_type net_policy;
  dss_dns_addrinfo hints;
  dss_dns_query_handle_type query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
  int16 retval = DSS_SUCCESS;

  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  if (bsdcb_ptr == NULL)
  {
    h_errno = NO_RECOVERY;
    return NULL;
  }

  if (bsdcb_ptr->hostent_ptr != NULL)
  {
    bsdi_freehostent((struct dss_hostent *)bsdcb_ptr->hostent_ptr);
    bsdcb_ptr->hostent_ptr = NULL;
  }

  if (bsdi_pppopen(bsdcb_ptr->netlib_app_id) == -1)
  {
    /*-----------------------------------------------------------------------
      No PPP.
    -----------------------------------------------------------------------*/
    if (errno == DS_ENETCLOSEINPROGRESS )
    {
      h_errno = TRY_AGAIN;
    }
    return NULL;
  }

  MSG_LOW("App %d calls gethostbyname()", bsdcb_ptr->netlib_app_id, 0, 0);

  // Below is DNS query using new DNS subsystem APIs
  // 1. Create DNS ssession
  session_handle = dss_dns_create_session(bsdi_dns_cb_fcn, 
                                          (void *)&bsdcb_ptr->netlib_app_id,
                                               &h_errno);
  if (DSS_DNS_SESSION_MGR_INVALID_HANDLE == session_handle)
  {
    MSG_ERROR("dss_dns_create_session failure %d", h_errno, 0, 0);
  }
  else
  {
    // 2. Set proper config params to DNS
    // get net policy
    if (bsd_get_net_policy(&net_policy) < 0)
    {
      MSG_ERROR("bsd_get_net_policy failure", 0, 0, 0);
    }
    else
    {
      // set dns config params - DSS_DNS_CONFIG_PARAMS_NET_POLICY
      retval = dss_dns_set_config_params(session_handle,
                                         DSS_DNS_CONFIG_PARAMS_NET_POLICY,
                                         (void *)&net_policy,
                                         sizeof(bsd_net_policy_info_type),
                                         &h_errno );
      if (DSS_ERROR == retval)
      {
        MSG_ERROR("dss_dns_set_config_params failure %d", h_errno, 0, 0);
      }
    }

    // 3. Resolve address information
    hints.ai_family = addr_family;
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    hints.ai_flags = DSS_DNS_AI_FLAGS_PASSIVE;
    query_handle = dss_dns_get_addrinfo(session_handle,
                                        name,
                                        NULL,
                                        &hints,
                                        &h_errno );
    if (DSS_DNS_QUERY_INVALID_HANDLE == query_handle)
    {
      MSG_ERROR("dss_dns_get_addrinfo failure %d", h_errno, 0, 0);
    }
    else
    {
      // 4. Wait dns call back to read address information
      if (bsdcb_ptr->hostent_ptr == NULL)
      {
        if (h_errno == DS_EWOULDBLOCK)
        {
          while (1)
          {
            (void)rex_clr_sigs(rex_self(), bsdcb_ptr->dns_sig |
                         bsdcb_ptr->socket_sig | bsdcb_ptr->network_sig);
   
            sigs = bsdcb_ptr->dns_sig;
            if (bsdi_wait(&sigs, 0, FALSE) == -1)
            {
              /*-----------------------------------------------------------------
                During the wait, a signal occurred that caused an application
                callback function to get called.  The function returned an error.
                A network signal is also possible.
              -----------------------------------------------------------------*/ 
              (void)rex_clr_sigs(rex_self(),
                           bsdcb_ptr->dns_sig | bsdcb_ptr->network_sig);
              h_errno = NO_RECOVERY;
              break;
            }
            else if (sigs & bsdcb_ptr->dns_sig)
            {
              (void)rex_clr_sigs(rex_self(),
                           bsdcb_ptr->dns_sig | bsdcb_ptr->network_sig);
              break;
            }
          }
         } // h_errno == DS_EWOULDBLOCK
         else
         {
           MSG_ERROR("bsdcb_ptr->hostent_ptr has valid information already", 0, 0, 0);
         }
       } // hostent_ptr == NULL
    } // query_handle != DSS_DNS_QUERY_INVALID_HANDLE

    // 5. Destroy DNS session
    retval = dss_dns_delete_session(session_handle,
                                    &h_errno );
    if (DSS_ERROR == retval)
    {
      MSG_ERROR("dss_dns_delete_session failure %d", session_handle, 0, 0);
  }
  }// DSS_DNS_SESSION_INVALID_HANDLE != session_handle

  bsdcb_ptr->bsd_errno = h_errno;
  return bsdcb_ptr->hostent_ptr;

} /* gethostbyname() */

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
)
{
  uint16 addrlen_copy = (uint16) *addrlen;     /* Short version of addrlen */
  sint15 dss_errno;                            /* Holds error code         */
  int ret_val;                                 /* Return value             */

  ret_val = (int) dss_getsockname((sint15) sockfd,
                                  addr,
                                  &addrlen_copy,
                                  &dss_errno
                                 );

  /*-------------------------------------------------------------------------
    Cast and return the addrlen value
  -------------------------------------------------------------------------*/
  *addrlen = (int) addrlen_copy;

  /*-------------------------------------------------------------------------
    Set the error code.  This is applicable only if the caller is the one who
    created the socket.
  -------------------------------------------------------------------------*/  
  if (rex_self()->bsdcb_ptr != NULL)
  {
    errno = dss_errno;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION GETPEERNAME()

DESCRIPTION
  Gets peer information (remote address) on a given socket.

DEPENDENCIES
  None.

PARAMETER
   sockfd         socket descriptor
  *addr           address of the socket
  *addrlen        address length (value-result parameter)

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
int getpeername
(
  int sockfd,
  struct sockaddr *addr,
  int *addrlen
)
{
  uint16 addrlen_copy = (uint16) *addrlen;     /* Short version of addrlen */
  sint15 dss_errno;                            /* Holds error code         */
  int ret_val;                                 /* Return value             */

  ret_val = (int) dss_getpeername((sint15) sockfd,
                                  addr,
                                  &addrlen_copy,
                                  &dss_errno
                                 );

  /*-------------------------------------------------------------------------
    Cast and return the addrlen value
  -------------------------------------------------------------------------*/
  *addrlen = (int) addrlen_copy;

  /*-------------------------------------------------------------------------
    Set the error code.  This is applicable only if the caller is the one who
    created the socket.
  -------------------------------------------------------------------------*/  
  if (rex_self()->bsdcb_ptr != NULL)
  {
    errno = dss_errno;
  }

  return ret_val;
}

int bsd_get_last_errno(){ return errno; }

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
  default_sigs_type* default_sigs,
  rex_sigs_type   watchdog_sig,
  rex_timer_type *watchdog_timer,
  rex_timer_cnt_type watchdog_rpt_time, 
  unsigned long   watchdog_rpt_mask
)
{
  bsdcb_type *bsdcb_ptr;                      /* BSD control block pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rex_self()->bsdcb_ptr != NULL)
  {
    MSG_ERROR("BSD socket services already opened", 0, 0, 0);
    return -1;
  }

  bsdcb_ptr = bsdi_alloc_bsdcb();
  if (bsdcb_ptr == NULL)
  {
    MSG_ERROR("Internal BSDCB array is full", 0, 0, 0);
    return -1;
  }

  bsdcb_ptr->select_timer_sig = default_sigs->select;
  bsdcb_ptr->socket_sig = default_sigs->socket;
  bsdcb_ptr->network_sig = default_sigs->net;
  bsdcb_ptr->dns_sig = default_sigs->dns;
  bsdcb_ptr->iface_ioctl_sig = default_sigs->iface_ioctl;
  bsdcb_ptr->abort_sig = default_sigs->abort;
  bsdcb_ptr->tcp_close_timer_sig = default_sigs->tcp_close_timeout;

  /*-------------------------------------------------------------------------
    Open the underlying DS Sock library.
  -------------------------------------------------------------------------*/
  if ((bsdcb_ptr->netlib_app_id = dss_open_netlib(bsdi_network_cb_fcn,
                                                  bsdi_socket_cb_fcn,
                                                  &errno)) == DSS_ERROR)
  {
    MSG_ERROR("dss_open_netlib() failed, error %d", errno, 0, 0);
    errno = EMFILE;
    /*-----------------------------------------------------------------------
      Free the BSD control block.
    -----------------------------------------------------------------------*/
    INTLOCK()
    bsdcb_ptr->owning_tcb_ptr->bsdcb_ptr = NULL;
    bsdcb_ptr->owning_tcb_ptr = NULL;
    INTFREE()
    return -1;
  }

  /*-------------------------------------------------------------------------
   Fill in other members of the BSD control block.
  -------------------------------------------------------------------------*/
  rex_def_timer(&bsdcb_ptr->select_timer,
                bsdcb_ptr->owning_tcb_ptr,
                bsdcb_ptr->select_timer_sig);

  /*-------------------------------------------------------------------------
    Set wait_mask to the OR of the signal mask of all the events bsdi_wait()
    should automatically unblock on or process.  This includes the socket,
    network, select timer, watchdog timer signals as well as the signals
    associated with each callback function.
  -------------------------------------------------------------------------*/
  bsdcb_ptr->wait_mask = bsdcb_ptr->select_timer_sig |
                         bsdcb_ptr->socket_sig |
                         bsdcb_ptr->network_sig |
                         bsdcb_ptr->dns_sig |
                         bsdcb_ptr->abort_sig;

  bsdcb_ptr->watchdog_sig = watchdog_sig;
  bsdcb_ptr->watchdog_timer = watchdog_timer;
  bsdcb_ptr->watchdog_rpt_mask = watchdog_rpt_mask;
  bsdcb_ptr->watchdog_rpt_time = watchdog_rpt_time;
  bsdcb_ptr->wait_mask |= watchdog_sig;

  bsdcb_ptr->hostent_ptr = NULL;
  bsdcb_ptr->sock_ref_count = 0;
  bsdcb_ptr->network_up = 0;
  bsdcb_ptr->network_event_occurred = 0;

  bsdcb_ptr->num_sig_cb = 0;

#ifndef PLATFORM_LTK
#if (defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL))
  (void)q_init(&bsdi_iface_ioctl_cb_info_q);
#endif /* FEATURE_QTV_QOS_SELECTION */
#endif /* !PLATFORM_LTK */

  return 0;

} /* bsd_init() */

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
void bsd_reg_usr_notifier(bsdcb_fcn_type cb_fcn, void *qos_cb_data)
{
  bsdcb_type* bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  if (bsdcb_ptr != NULL)
  {
    bsdcb_ptr->cb_fcn = cb_fcn;
  }
#ifdef FEATURE_QTV_QOS_SELECTION_EVDO
  /* Set the callback data for the QoS System notification handler */
  bsdcb_ptr->qos_cb_data = qos_cb_data;

  /* Register for QoS aware and unaware system notification */
  bsd_reg_qos_system_notification();
#endif /* FEATURE_QTV_QOS_SELECTION_EVDO */
}

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
)
{
  bsdcb_type *bsdcb_ptr;                      /* BSD control block pointer */
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  /* Check if a handler is already registered for the specified signal */
  for(i = 0; i < bsdcb_ptr->num_sig_cb; i++)
  {
      if( (bsdcb_ptr->sig_cb_array[i].signal == signal) &&
          (bsdcb_ptr->sig_cb_array[i].clbk_fnc_ptr == clbk_fnc_ptr))
      {
          MSG_HIGH("bsd_reg_cb(): Callback already active,slot %d", i, 0, 0);

          /* Mark the slot in Use just in case and store the function argument */
          bsdcb_ptr->sig_cb_array[i].arg = clbk_fnc_arg;
          bsdcb_ptr->sig_cb_array[i].isActive = TRUE;

          /* Just add the signal mask to the wait mask */
          bsdcb_ptr->wait_mask |= signal;

          return 0;
      }
  }

  if (bsdcb_ptr->num_sig_cb == BSD_MAX_NUM_OF_SIG_CB)
  {
    MSG_ERROR("bsd_reg_cb(): max number of cb's already registered", 0, 0, 0);
    return -1;
  }

  bsdcb_ptr->sig_cb_array[bsdcb_ptr->num_sig_cb].signal = signal;
  bsdcb_ptr->sig_cb_array[bsdcb_ptr->num_sig_cb].clbk_fnc_ptr = clbk_fnc_ptr;
  bsdcb_ptr->sig_cb_array[bsdcb_ptr->num_sig_cb].arg = clbk_fnc_arg;
  bsdcb_ptr->sig_cb_array[bsdcb_ptr->num_sig_cb].isActive = TRUE;

  bsdcb_ptr->wait_mask |= signal;

  bsdcb_ptr->num_sig_cb++;

  return 0;

} /* bsd_reg_cb() */

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
)
{
  bsdcb_type *bsdcb_ptr;            /* BSD control block structure pointer */
#if defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL) && !defined(PLATFORM_LTK)
  bsdi_iface_ioctl_cb_info_type* info_elem;
#endif /* FEATURE_QTV_QOS_SELECTION */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ptr == NULL)
  {
    MSG_ERROR("bsd_shutdown(): NULL task pointer", 0, 0, 0);
    errno = EINVAL;
    return -1;
  }

  bsdcb_ptr = (bsdcb_type *)((rex_tcb_type *)ptr)->bsdcb_ptr;

  if (NULL == bsdcb_ptr)
  {
    MSG_ERROR("bsd_shutdown(): NULL bsdcb_ptr", 0, 0, 0);
    return -1;
  }
  if (bsdcb_ptr->sock_ref_count > 0)
  {
    MSG_ERROR("Some sockets are not closed", 0, 0, 0);
    errno = EFAULT;
    return -1;
  }

  if (bsdcb_ptr->hostent_ptr != NULL)
  {
    bsdi_freehostent((struct dss_hostent *)bsdcb_ptr->hostent_ptr);
    bsdcb_ptr->hostent_ptr = NULL;
  }

  if (bsdi_pppclose(bsdcb_ptr->netlib_app_id) == -1)
  {
    return -1;
  }

  if (dss_close_netlib(bsdcb_ptr->netlib_app_id, &errno) == DSS_ERROR)
  {
    MSG_ERROR("dss_close_netlib() failed, error %d", errno, 0, 0);        
    errno = EFAULT;
    return -1;
  }

  (void)rex_clr_timer(&bsdcb_ptr->select_timer);
  bsdcb_ptr->netlib_app_id = 0;
  INTLOCK()
  bsdcb_ptr->owning_tcb_ptr->bsdcb_ptr = NULL;
  bsdcb_ptr->owning_tcb_ptr = NULL;
  INTFREE()

#if defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL) && !defined(PLATFORM_LTK)
  while ((info_elem = (bsdi_iface_ioctl_cb_info_type *) 
         q_get(&bsdi_iface_ioctl_cb_info_q)) != NULL)
  {
    ResetInUseFlag(info_elem);
  }
#endif /* FEATURE_QTV_QOS_SELECTION */

  return 0;

} /* bsd_shutdown() */

/*===========================================================================

FUNCTION SETSOCKOPT()

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
   sockfd         socket descriptor

RETURN VALUE
   0 if successful.
  -1 if an error occurred.  An error code indicating the cause of the error
     is written to errno.

  Errno Values
  ------------
  EBADF            invalid socket descriptor is specfied
  EOPNOTSUPP       operation not supported
  EINVAL           the socket is already attached to a local name
  EFAULT           invalid address parameter has been specified

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  int dss_level;
  int dss_optname;
  dss_so_linger_type dss_so_linger;
  int time;
  int resetOnClose = 0;

  if ( (dss_level = bsdi_map_bsd_dss_sockopt_level(level)) < 0)
  {
    MSG_HIGH("setsockopt(): BSD level %d: not supported", level, 0, 0);
    errno = EOPNOTSUPP;
    return -1;
  }
  
  if ( (dss_optname = bsdi_map_bsd_dss_sockopt_name(optname)) < 0)
  {
    MSG_HIGH("setsockopt(): BSD option %d: not supported", optname, 0, 0);
    errno = EOPNOTSUPP;
    return -1;
  }
  
  switch (optname)
  {

#if defined(FEATURE_QTV_MCAST) && !defined(PLATFORM_LTK)
    IpAddrType dss_mcast_info;

    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
      if (bsdi_map_bsd_dss_optval_ip_mreq(optval, 
                                          optlen, 
                                          &dss_mcast_info) < 0)
      {
        MSG_HIGH("setsockopt(): Could not map value for option %d", 
                 optname, 0, 0);
        errno = EFAULT;
        return -1;
      }
      if (dss_mcast_info.type == IPV6_ADDR)
      {
        optval = (void *) &dss_mcast_info.addr.v6;
        optlen = sizeof(dss_mcast_info.addr.v6);
      }
      else if (dss_mcast_info.type == IPV4_ADDR)
      {
        optval = (void *) &dss_mcast_info.addr.v4;
        optlen = sizeof(dss_mcast_info.addr.v4);
      }
      else
      {
        errno = EFAULT;
        return -1;
      }
      break;

#endif /* FEATURE_QTV_MCAST */

     case SO_LINGER:
       if (SOL_SOCK != level)
       {
          MSG_HIGH("Linger option must be set on SOCK level", level, 0, 0);
          errno = EFAULT;
          return -1;
       }

       if (sizeof(so_linger_type) != optlen)
       {
          MSG_HIGH("Linger option value has incorrect length", 
                   optlen, 0, 0);
          errno = EFAULT;
          return -1;
       }

       /* Keep track of the the linger value so we can set a timer when we
        * close the socket.
        */
       if (((so_linger_type*)optval)->l_onoff == 0)
       {
           time = 0;
       } 
       else
       {
           if (((so_linger_type*)optval)->l_linger == 0)
           {              
              time = 0;
              resetOnClose = 1;       
           } 
           else 
           {
              time = ((so_linger_type*)optval)->l_linger;
           }
       }

       if (time < 0)
       {
           errno = EFAULT;
           return -1;
       }

       bsd_scb_array[SOCKFD_TO_INDEX(sockfd)].tcp_close_timeout = time;
       
#ifdef SILENT_CLOSE_SUPPORTED
       if (resetOnClose)
       { 
           dss_so_linger.l_onoff = 1;
           dss_so_linger.l_linger = 0;
       }
       else 
       {          
           /* Disable DSS linger feature, so we can implement our own. */
           dss_so_linger.l_onoff = 0;
           dss_so_linger.l_linger = 0;
       }
#else
       dss_so_linger.l_onoff = ((so_linger_type*)optval)->l_onoff;
       dss_so_linger.l_linger = time;
#endif /* SILENT_CLOSE_SUPPORTED */
       optval = (void*) &dss_so_linger;
       optlen = sizeof(dss_so_linger);
       break;

#ifdef TCP_SACK_ENABLED
    case TCP_SACK:
       if (IPPROTO_TCP != level)
       {
         MSG_ERROR("SACK option must be set on IPPROTO_TCP level %d", level, 0, 0);
         errno = EFAULT;
         return -1;
       }

       if (sizeof(int) != optlen)
       {
          MSG_ERROR("SACK option value has incorrect length", 
                   optlen, 0, 0);
          errno = EFAULT;
          return -1;
       }

       if (NULL == optval)
       {
         MSG_HIGH("SACK option set with NULL", level, 0, 0);
         errno = EFAULT;
         return -1;
       }

       MSG_HIGH("SACK option set on IPPROTO_TCP level %d", level, 0, 0);
       break;
#endif /* TCP_SACK_ENABLED */

    case TCP_NODELAY:
      if (IPPROTO_TCP != level)
      {
        MSG_ERROR("TCP_NODELAY option must be set on IPPROTO_TCP level %d", level, 0, 0);
        errno = EFAULT;
        return -1;
      }

      if (sizeof(int) != optlen)
      {
         MSG_ERROR("TCP_NODELAY option value has incorrect length", 
                   optlen, 0, 0);
         errno = EFAULT;
         return -1;
      }
      break;

    default:
      break;
  }

  if ( dss_setsockopt( (sint15)sockfd,
                       dss_level,
                       dss_optname,
                       optval,
                       &optlen,
                       &errno ) == DSS_ERROR )
  {
    return -1;
  }

  return 0;

} /* setsockopt() */

#ifdef FEATURE_QTV_MCAST
/*===========================================================================

FUNCTION OPEN_BCAST_IFACE()

DESCRIPTION
  Selects and opens the broadcast interface

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
open_bcast_iface(bsd_iface_name_enum_type bcast_iface_name,
                 boolean mcastAddrIPv6Type)
{
#ifndef PLATFORM_LTK
  bsdcb_type   *bsdcb_ptr;            /* BSD control block pointer         */
  dss_net_policy_info_type net_policy; 
  int ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  dss_init_net_policy_info (&net_policy);
  net_policy.iface.info.name = 
     bsdi_map_bsd_dss_bcast_iface_name(bcast_iface_name);

  /* If this is IPv6 Address type, set address family to AF_INET6. In case of IPv4, even if mapped
     address family should be set to AF_INET which is the default and hence needn't be set explicitly
  */
  if( mcastAddrIPv6Type)
  {
     net_policy.family = AF_INET6;
  }

  ret = dss_set_app_net_policy( bsdcb_ptr->netlib_app_id, 
                                &net_policy, 
                                &errno);

  if (ret != DSS_SUCCESS) 
  {
    MSG_ERROR("dss_set_app_net_policy() failed, error %d", errno, 0, 0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    call dss_pppopen. since we selected bcast iface above, no PPP will be
    brought up in this case. instead, the DMSS will do the set-up necessary
    to receive the multicast packets. Note that in case of bcast iface, we 
    don't expect pppopen() to return EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if (dss_pppopen(bsdcb_ptr->netlib_app_id, &errno) == DSS_ERROR)
  {
    MSG_ERROR("dss_pppopen() failed for mcast, error = %d", errno, 0, 0);
    return -1;
  }
#endif /* PLATFORM_LTK */
  return 0;
}

/*===========================================================================

FUNCTION CLOSE_BCAST_IFACE()

DESCRIPTION
  Performs the operations associated with closing the broadcast interface.
  Currently, all we need to do is to reset the network policy.
  

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
close_bcast_iface(void)
{
  /*-------------------------------------------------------------------------
    Currently, we need not do anything more than simply resetting the 
    network policy.
  -------------------------------------------------------------------------*/
  return bsd_set_net_policy(NULL);
}

#ifdef FEATURE_QTV_MCAST_IOCTL

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
)
{
  bsd_status_enum_type retVal = BSD_MCAST_STATUS_SUCCESS;

#ifndef PLATFORM_LTK
  bsdcb_type   *bsdcb_ptr;            /* BSD control block pointer*/
  dss_iface_id_type primary_iface_id;
  dss_iface_ioctl_mcast_join_type mcast_req_info;
  int status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (mcast_handle == NULL)
  {
    MSG_ERROR("mcast_handle is NULL", 0, 0, 0);
    errno = EFAULT;
    return BSD_MCAST_STATUS_ERROR;
  }

  // clear out handle first. So if there is any error happened during
  // following steps, bsd_disable_mcast() will be able to handle properly
  *mcast_handle = NULL;

  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  if (bsdcb_ptr == NULL)
  {
    MSG_ERROR("Error: bsdcb_ptr == NULL", 0, 0, 0);
    errno = EFAULT;
    return BSD_MCAST_STATUS_ERROR;
  }

  bsdcb_ptr->mcast_cb_flag  = mcast_cb_flag;
  bsdcb_ptr->mcast_cb_data = mcast_cb_data;

  primary_iface_id = dss_get_iface_id(bsdcb_ptr->netlib_app_id); 

  if (primary_iface_id == DSS_IFACE_INVALID_ID)
  {
    MSG_ERROR("Got bad iface id, appid = %d", bsdcb_ptr->netlib_app_id, 0, 0);
    return BSD_MCAST_STATUS_ERROR;
  }

  /*-----------------------------------------------------------------------
    Install a signal handler to handle a iface_ioctl_event 
    asynchronously.
  -----------------------------------------------------------------------*/
  status = bsd_reg_cb(bsdcb_ptr->iface_ioctl_sig, 
                      bsdi_iface_ioctl_sig_handler, 
                      NULL);

  if ( status < 0)
  {
    MSG_ERROR("bsd_reg_cb() failed for iface_ioctl_sig", 0, 0, 0);
    // don't need to abort, as the following ioctl() could succeed immediately
  }

  mcast_req_info.app_id        = bsdcb_ptr->netlib_app_id;
  mcast_req_info.event_cb      = bsdi_iface_ioctl_cb_fcn;
  mcast_req_info.user_data_ptr = NULL;

  mcast_req_info.ip_addr.type    = mcast_info->im_multiaddr.type;
  if (mcast_info->im_multiaddr.type == IPV6_ADDR)
  {
    // copy ipv6 address: data block size of "sizeof(struct in6_addr)"
    memcpy(mcast_req_info.ip_addr.addr.v6, 
           &mcast_info->im_multiaddr.addr.v6.s6_addr64, 
           sizeof(mcast_info->im_multiaddr.addr.v6.s6_addr64));
  }
  else if (mcast_info->im_multiaddr.type == IPV4_ADDR)
  {
    mcast_req_info.ip_addr.addr.v4 = mcast_info->im_multiaddr.addr.v4.s_addr;
  }
  else
  {
    return BSD_MCAST_STATUS_ERROR;
  }

  mcast_req_info.port = mcast_info->im_port;
  mcast_req_info.mcast_param_ptr = mcast_param_ptr;

  (void)rex_clr_sigs(rex_self(), bsdcb_ptr->iface_ioctl_sig);

  status = dss_iface_ioctl( primary_iface_id, 
                            DSS_IFACE_IOCTL_MCAST_JOIN, 
                            (void *) &mcast_req_info, 
                            &errno);

  *mcast_handle = mcast_req_info.handle;

  retVal = BSD_MCAST_STATUS_WAITING;

  if (status == DSS_SUCCESS)
  {
    MSG_HIGH("dss_iface_ioctl() succeeded immediately for JOIN request", 0, 0, 0);      
    // wait for response from Register
    //if (bsdcb_ptr->mcast_cb_flag && bsdcb_ptr->cb_fcn)
    //{
    //   bsdcb_ptr->cb_fcn(BSD_MCAST_STATUS_SUCCESS,
    //                     bsdcb_ptr->mcast_cb_data,
    //                     *mcast_handle);
    //}
    //retVal = BSD_MCAST_STATUS_SUCCESS;
  } 
  else if (errno != DS_EWOULDBLOCK) 
  {
    MSG_ERROR("dss_iface_ioctl() failed, error=%d", errno, 0, 0);
    retVal = BSD_MCAST_STATUS_ERROR;
  }

#endif /* PLATFORM_LTK */

  return retVal;
}

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
)
{
#ifndef PLATFORM_LTK
  bsdcb_type   *bsdcb_ptr;            /* BSD control block pointer*/
  dss_iface_id_type primary_iface_id;
  dss_iface_ioctl_mcast_leave_type mcast_leavereq_info;
  int status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  if (bsdcb_ptr == NULL)
  {
    MSG_ERROR("Error: bsdcb_ptr == NULL", 0, 0, 0);
    errno = EFAULT;
    return BSD_MCAST_STATUS_ERROR;
  }
  
  primary_iface_id = dss_get_iface_id(bsdcb_ptr->netlib_app_id); 

  if (primary_iface_id == DSS_IFACE_INVALID_ID)
  {
    MSG_ERROR("Got bad iface id, appid = %d", bsdcb_ptr->netlib_app_id, 0, 0);
    return -1;
  }

  if (mcast_handle == NULL)
  {
    MSG_ERROR("mcast_handle is NULL. Nothing to release.", 0, 0, 0);
    return 0;
  }

  mcast_leavereq_info.app_id = bsdcb_ptr->netlib_app_id;
  mcast_leavereq_info.handle = mcast_handle;
    
  status = dss_iface_ioctl( primary_iface_id, 
                            DSS_IFACE_IOCTL_MCAST_LEAVE, 
                            (void *) &mcast_leavereq_info, 
                            &errno);

  if (status == DSS_SUCCESS )
  {
    MSG_HIGH("dss_iface_ioctl() succeeded for MCAST LEAVE", 0, 0, 0);
  }
  else if (errno == DS_EWOULDBLOCK) 
  {
    MSG_HIGH("dss_iface_ioctl() returned EWOULDBLOCK, ignoring", 0, 0, 0);
  }
  else 
  {
    MSG_ERROR("dss_iface_ioctl() failed, error=%d", errno, 0, 0);
    /* Call the application installed callback here */
    if (bsdcb_ptr->mcast_cb_flag && bsdcb_ptr->cb_fcn)
    {
       bsdcb_ptr->cb_fcn(BSD_MCAST_STATUS_ERROR,
                         bsdcb_ptr->mcast_cb_data,
                         (void*)mcast_handle);
    }
    // For now, we don't care about result of diable_mcast.
    // So no need to maintain mcast_cb_data
    /* Reset the callback info */
    bsdcb_ptr->mcast_cb_flag = FALSE;
    bsdcb_ptr->mcast_cb_data = NULL;
    return BSD_MCAST_STATUS_ERROR;
  }

#endif /* PLATFORM_LTK */

  return BSD_MCAST_STATUS_SUCCESS;
}

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
#ifndef PLATFORM_LTK
void
print_mcast_event
(
 dss_iface_ioctl_mcast_info_code_enum_type info_code
)
{
    switch(info_code)
    {
       case DSS_IFACE_IOCTL_MCAST_IC_NOT_SPECIFIED:
       MSG_HIGH("Qtv No-op: DSS_IFACE_IOCTL_MCAST_IC_NOT_SPECIFIED:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_CANCELLED:
       MSG_HIGH("Qtv No-op: DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_CANCELLED:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR:
       MSG_HIGH("Qtv Temp Failure: DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_REQUESTED:
       MSG_HIGH("Qtv Temp Failure: DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_REQUESTED:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_TIMEOUT:
       MSG_HIGH("Qtv Temp Failure: DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_TIMEOUT:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_LOST:
       MSG_HIGH("Qtv Temp Failure: DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_LOST:", 0, 0, 0);
       break;

       //case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE:
       //MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE:", 0, 0, 0);
       //break;

       case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE:
       MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED:
       MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG:
       MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_UNAVAILABLE:
       MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_UNAVAILABLE:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_NO_MAPPING:
       MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_NO_MAPPING:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP:
       MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_FLOWS_REACHED:
       MSG_HIGH("Qtv Perm Failure: DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_FLOWS_REACHED:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_FLO_IP_OR_PORT_NOT_SUPPORTED:
       MSG_HIGH("Qtv Media Flo: DSS_IFACE_IOCTL_MCAST_FLO_IP_OR_PORT_NOT_SUPPORTED:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_FLO_NO_AUTHORIZATION:
       MSG_HIGH("Qtv Media Flo: DSS_IFACE_IOCTL_MCAST_FLO_NO_AUTHORIZATION:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_FLO_NO_SYSTEM_COVERAGE:
       MSG_HIGH("Qtv Media Flo: DSS_IFACE_IOCTL_MCAST_FLO_NO_SYSTEM_COVERAGE:", 0, 0, 0);
       break;

       case DSS_IFACE_IOCTL_MCAST_FLO_MAX_FLOW_REACHED:
       MSG_HIGH("Qtv Media Flo: DSS_IFACE_IOCTL_MCAST_FLO_MAX_FLOW_REACHED:", 0, 0, 0);
       break;
    }
}
#endif /* PLATFORM_LTK */
#endif /* FEATURE_QTV_MCAST_IOCTL */

#endif /* FEATURE_QTV_MCAST */

#ifdef FEATURE_QTV_NET_POLICY
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
)
{
#ifndef PLATFORM_LTK
  bsdcb_type   *bsdcb_ptr;            /* BSD control block pointer         */
  dss_net_policy_info_type* dss_net_policy = 
    (dss_net_policy_info_type *) bsd_net_policy;
  int ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  ret = dss_get_app_net_policy( bsdcb_ptr->netlib_app_id, 
                                dss_net_policy, 
                                &errno);
  
  if (ret != DSS_SUCCESS) 
  {
    MSG_ERROR("dss_get_app_net_policy() failed, error %d", errno, 0, 0);
    return -1;
  }

#endif /* PLATFORM_LTK */
  return 0;
  
}

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
  Sets errno to EISCONN if the network is already up, or EINVAL if the argument
  is invalid. Otherwise errno is something else.
 

===========================================================================*/
int
bsd_set_net_policy
(
  bsd_net_policy_info_type* bsd_net_policy
)
{
#ifndef PLATFORM_LTK
  bsdcb_type   *bsdcb_ptr;            /* BSD control block pointer         */
  dss_net_policy_info_type *dss_net_policy = 
    (dss_net_policy_info_type *) bsd_net_policy;
  int ret = DSS_ERROR;
  sint15 dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  (void)dss_netstatus(bsdcb_ptr->netlib_app_id, &dss_errno);
  if (dss_errno == DS_ENETISCONN)
  {
    ret = DSS_ERROR;
    errno = (int) EISCONN;
  }
  else
  {
    ret = dss_set_app_net_policy( bsdcb_ptr->netlib_app_id, 
                                  dss_net_policy, 
                                  &errno);
  }

  if (ret != DSS_SUCCESS) 
  {
    MSG_ERROR("dss_set_app_net_policy() failed, error %d", dss_errno, 0, 0);
    return -1;
  }

#endif /* PLATFORM_LTK */
  return 0;
  
}

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
)
{
#ifndef PLATFORM_LTK
  dss_net_policy_info_type* dss_net_policy = 
    (dss_net_policy_info_type *) bsd_net_policy;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dss_init_net_policy_info(dss_net_policy);

#endif /* PLATFORM_LTK */
  return;
  
}
#endif /* FEATURE_QTV_NET_POLICY */

#if defined(FEATURE_QTV_QOS_SELECTION) && !defined(PLATFORM_LTK)
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION BSDI_WAIT()

DESCRIPTION
  Depending on the value of 'nonblocking' parameter, bsdi_wait() either
  returns immediately without waiting or blocks until the requested signals,
  socket signal, network signal or select timer signal occur.

  During the wait, the function periodically reports to the watch dog timer
  and monitors those signals that were registered during the call to
  bsd_init().  When one of the registered signals occur, the signal's
  callback function is called and its return value is checked.  A return
  value of -1 indicates that the callback function encountered an error.
  In such a case, bsdi_wait() unblocks.  Otherwise, bsdi_wait() continues
  to wait.  If the registered signal that occurred does not have a callback
  function (callback function pointer is NULL), bsdi_wait() just unblocks.

DEPENDENCIES
  None.

PARAMETERS
  *sigs_ptr       pointer to a signal mask indicating the events to wait for
  nonblocking     determines whether or not bsdi_wait() blocks
  ignoreAbortSignal If true then ignore the abort signal, i.e., don't wait
                    on the abort signal. We need to ignore the abort signal
                    when closing down a TCP socket or PPP so that we quit
                    only after cleaning up completely. 

RETURN VALUE
  current REX signal state if successful.
  -1 if an error occurred.  Below are the possible error codes.

  Errno Values
  ------------
  EINTR            application's call back function encountered an error or
                   a user-defined signal with no call back function occurred 

  In addition the contents of the sigs_ptr is set to the current REX signal
  state.

SIDE EFFECTS
  None.

===========================================================================*/
static int bsdi_wait
(
  rex_sigs_type *sigs_ptr,
  int nonblocking,
  boolean ignoreAbortSignal
)
{
  rex_sigs_type rex_signal_state;    /* State of signals returned by REX   */
  bsdcb_type *bsdcb_ptr;             /* Thread's BSD control block pointer */
  int i;                             /* Loop index                         */
  int ret_val = 0;                   /* Return value                       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;

  if (nonblocking)
  {
    /*-----------------------------------------------------------------------
      Don't wait on signals because bsdi_wait() is called from a socket
      function call on a nonblocking socket.  Just get the current signals
      state for this task and return -1.
    -----------------------------------------------------------------------*/
     *sigs_ptr = rex_get_sigs(rex_self());
     return -1;
  }

  /*-------------------------------------------------------------------------
    bsdi_wait() automatically unblocks on socket, network, or select timer
    signal whether or not they are specified by the caller.  So OR in these
    signals into the signal mask.
  -------------------------------------------------------------------------*/
  *sigs_ptr |= bsdcb_ptr->wait_mask;

  if (ignoreAbortSignal)
  {
    *sigs_ptr &= ~(bsdcb_ptr->abort_sig);
  }

  do
  {
    rex_signal_state = rex_wait(*sigs_ptr);

    if (rex_signal_state & bsdcb_ptr->abort_sig)
    {
      errno = EINTR;
      ret_val = -1;
      break;
    }

    /*-----------------------------------------------------------------------
       For tasks that are not subject to the watchdog timer,
       bsdcb_ptr->watchdog_sig is 0 so that reporting to the dog task and
       setting the watchdog timer below are skipped.
    -----------------------------------------------------------------------*/  
    if (rex_signal_state & bsdcb_ptr->watchdog_sig)
    {
      /*---------------------------------------------------------------------
        Time to pet the watchdog.
      ---------------------------------------------------------------------*/
      dog_report(bsdcb_ptr->watchdog_rpt_mask);
      (void)rex_set_timer(bsdcb_ptr->watchdog_timer,
                    bsdcb_ptr->watchdog_rpt_time);
      /*---------------------------------------------------------------------
        rex_set_timer() clears the timer signal.
      ---------------------------------------------------------------------*/
    }

    for (i = 0; i < bsdcb_ptr->num_sig_cb; i++)
    {
      if (rex_signal_state & bsdcb_ptr->sig_cb_array[i].signal)
      {
        if (bsdcb_ptr->sig_cb_array[i].clbk_fnc_ptr == NULL ||
            bsdcb_ptr->sig_cb_array[i].clbk_fnc_ptr(
              bsdcb_ptr->sig_cb_array[i].signal,
              bsdcb_ptr->sig_cb_array[i].arg) == -1)
        {
          errno = EINTR;
          ret_val = -1;
          break;
        }
      }
    }
  } while (!(*sigs_ptr & rex_signal_state) && errno != EINTR);

  /*-------------------------------------------------------------------------
    A socket, network, select timer or caller specified signal has occurred.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Kick watchdog only if the task is subject to reporting to the dog task.
  -------------------------------------------------------------------------*/ 
  if (bsdcb_ptr->watchdog_timer != NULL)
  {
    dog_report(bsdcb_ptr->watchdog_rpt_mask);
    (void)rex_set_timer(bsdcb_ptr->watchdog_timer, bsdcb_ptr->watchdog_rpt_time);
  }

  *sigs_ptr = rex_signal_state;
  return ret_val;

} /* bsdi_wait() */

/*===========================================================================

FUNCTION BSDI_PPPOPEN()

DESCRIPTION
  Attempts to open PPP if necessary. 

DEPENDENCIES
  None.

PARAMETERS
  app_id     application id

RETURN VALUE
   0 if PPP opened.
  -1 if an error occurred.  errno is set to one of the following values.

  Errno Values
  ------------
  EFAULT            internal DS Sock error
  ENONET            network subsystem unavailable
  EINTR             BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
static int bsdi_pppopen
(
  sint15  app_id
)
{
  rex_sigs_type sigs;                 /* In/Out signal mask to bsdi_wait() */
  bsdcb_type *bsdcb_ptr;              /* BSD control block pointer         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;

  /*-------------------------------------------------------------------------
    Only call dss_pppopen() if a network event has occurred or PPP is known
    to be down.
  -------------------------------------------------------------------------*/
  if (!bsdcb_ptr->network_event_occurred && bsdcb_ptr->network_up)
  {
    /*-----------------------------------------------------------------------
      PPP is up and there is no net event to indicate that PPP's status may
      have changed.
    -----------------------------------------------------------------------*/
    return 0;
  }
  else
  {
    bsdcb_ptr->network_event_occurred = 0;
    (void)dss_netstatus(app_id, &errno);
    {
      if (errno == DS_ENETISCONN)
      {
        bsdcb_ptr->network_up = 1;
        errno = DSS_SUCCESS;
        return 0;
      }
    }
  }

  /*------------------------------------------------------------------------- 
    The network is not up.  Bringing up the network usually requires blocking
    on the network event.  This is an issue for threads that entered this
    function using another thread's app_id.  This can happen if an
    application calls a socket system call on a socket it did not create.
    Actually, if the thread has a valid app_id, it can call dss_pppopen()
    with its own app_id and block.  However, this is may not be the case
    since threads that never create a socket do not have an app_id because
    they never call dss_open_netlib().  The least confusing solution is to
    compare the passed in app_id with the thread's app_id.  If they are
    different then just call dss_pppopen() with the passed in app_id and
    return a -1 with error code ENONET.
  -------------------------------------------------------------------------*/

  if (app_id != ((bsdcb_type *)rex_self()->bsdcb_ptr)->netlib_app_id)
  {
     (void)dss_pppopen(app_id, &errno);
     errno = ENONET;
     return -1;
  }
     
  while (1)
  {
    (void)rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);
    if (dss_pppopen(app_id, &errno) == DSS_SUCCESS)
    {
      /*---------------------------------------------------------------------
        PPP is already established.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);
      return 0;
    }
    else if (errno == EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        The attempt to establish PPP is made and PPP is in the process of
        being opened.  Now break out of this loop to monitor the net status.
      ---------------------------------------------------------------------*/
      MSG_MED("bsdi_pppopen(): Waiting for PPP to open", 0, 0, 0);
      break;
    } /* EWOULDBLOCK */
    else if (errno == DS_ENETCLOSEINPROGRESS)
    {
      /*---------------------------------------------------------------------
        The previous PPP session is still in the middle of closing. We just
        set the errno and leave it up to the caller to decide what to do 
        in this case. 
      ---------------------------------------------------------------------*/
      MSG_MED("bsdi_pppopen(): dss_pppopen() returned NETCLOSEINPROGRESS",
              0, 
              0, 
              0 );

      return -1;
    } /* DS_ENETCLOSEINPROGRESS */
    else
    {
      MSG_ERROR("bsdi_pppopen(): Error %d opening PPP", errno, 0, 0);
      errno = EFAULT;
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Monitor net status to determine whether the PPP open attempt succeeds or
    fails.
  -------------------------------------------------------------------------*/
  while (1)
  {
    sigs = 0;
    if (bsdi_wait(&sigs, 0, FALSE) == -1)
    {
      (void)rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);
      return -1;
    }
    if ((sigs & bsdcb_ptr->network_sig) == 0)
    {
      /*---------------------------------------------------------------------
        bsdi_wait() did not unblock due to a network signal.
        Must have been due to socket or select_timer.
        Assume we can just ignore these signals.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs(rex_self(),
                   bsdcb_ptr->socket_sig | bsdcb_ptr->select_timer_sig);
      continue;
    }
    (void)rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);

    if (dss_netstatus(app_id, &errno) == DSS_ERROR)
    {
      switch (errno)
      {
        case DS_ENETISCONN:
          MSG_MED("bsdi_pppopen(): PPP opened", 0, 0, 0);
          errno = DSS_SUCCESS;
          bsdcb_ptr->network_up = 1;
          return 0;

        case DS_ENETINPROGRESS:
          continue;

        case DS_ENETCLOSEINPROGRESS:
          /*-----------------------------------------------------------------
            PPP open attempt was attempted but is now failing and will fail.
            Set errno and fall thru to the no net case.
          -----------------------------------------------------------------*/
          errno = ENONET;
          MSG_ERROR("bsdi_pppopen(): PPP did not open, error %d",
                   errno, 0, 0);
          bsdcb_ptr->network_up = 0;
          return -1;
        case DS_ENETNONET:
          MSG_ERROR("bsdi_pppopen(): PPP did not open, error %d",
                   errno, 0, 0);
          bsdcb_ptr->network_up = 0;
          return -1;

        case DS_EBADAPP:
        default:
          errno = EFAULT;
          bsdcb_ptr->network_up = 0;
          return -1;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        dss_netstatus() should always return DSS_ERROR.
      ---------------------------------------------------------------------*/
      errno = EFAULT;
      bsdcb_ptr->network_up = 0;
      return -1;
    }
  }
} /* bsdi_pppopen() */

/*===========================================================================

FUNCTION BSDI_PPPCLOSE()

DESCRIPTION
  Attempts to close PPP.  The underlying DS Net layer does not close PPP if
  another application requires PPP.  However, the DS Net layer still returns
  DSS_SUCCESS (PPP close attempt successful) to this application.

DEPENDENCIES
  None.

PARAMETERS
  app_id     application id

RETURN VALUE
   0 if PPP closed.
  -1 if an error occurred.  errno is set to one of the following values.

  Errno Values
  ------------
  EFAULT            internal DS Sock error 
  EINTR             BSD application's callback function encountered an error

SIDE EFFECTS
  None.

===========================================================================*/
static int bsdi_pppclose
(
  sint15  app_id
)
{
  bsdcb_type *bsdcb_ptr;              /* BSD control block pointer         */
#ifndef BSD_BACKGROUND_PPPCLOSE 
  rex_sigs_type sigs;                 /* In/Out signal mask to bsdi_wait() */
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  (void)rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);
  if (dss_pppclose(app_id, &errno) == DSS_SUCCESS)
  {
    bsdcb_ptr->network_up = 0;
    /*-----------------------------------------------------------------------
      PPP is already closed or some other application needs PPP.
    -----------------------------------------------------------------------*/
    (void)rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);
  }
  else if (errno == EWOULDBLOCK || errno == DS_ENETCLOSEINPROGRESS)
  {
    /*-----------------------------------------------------------------------
      if BSD_BACKGROUND_PPPCLOSE is defined then all we need to do here is to
      reset network_up flag to indicate that the network is not up, actual
      PPP close will happen in the background.
    -----------------------------------------------------------------------*/
#ifdef BSD_BACKGROUND_PPPCLOSE 
    bsdcb_ptr->network_up = 0;
#else
    /*-----------------------------------------------------------------------
      PPP is in the process of being closed.
    -----------------------------------------------------------------------*/
    MSG_MED("bsdi_pppclose(): Wait for PPP to close", 0, 0, 0);

    /*-----------------------------------------------------------------------
      Monitor net status to determine whether PPP closes for this
      application.  Note that if there are other applications that require 
      PPP, PPP will stay up but the lower layer should still indicate that
      PPP is down as far as this application is concerned.
    -----------------------------------------------------------------------*/ 
    while (1)
    {
      sigs = 0;

      /*---------------------------------------------------------------------
        Note that we don't want bsdi_wait() to examine the net abort signal
        otherwise we might return before PPP has been closed completely and
        won't be able to close the network library.
      ---------------------------------------------------------------------*/ 

      if (bsdi_wait(&sigs, 0, TRUE) == -1)
      {
        rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);
        return -1;
      }

      if ((sigs & bsdcb_ptr->network_sig) == 0)
      {
        /*-------------------------------------------------------------------
          bsdi_wait() did not unblock due to a network signal.
          Must have been due to socket or select_timer.
          Assume we can just ignore these signals.
        -------------------------------------------------------------------*/
        rex_clr_sigs(rex_self(),
                     bsdcb_ptr->socket_sig | bsdcb_ptr->select_timer_sig);
        continue;
      }
      rex_clr_sigs(rex_self(), bsdcb_ptr->network_sig);

      if (dss_netstatus(app_id, &errno) == DSS_ERROR)
      {
        switch (errno)
        {
          case DS_ENETNONET:
            MSG_MED("bsdi_pppclose(): PPP closed", 0, 0, 0);
            bsdcb_ptr->network_up = 0;
            errno = 0;
            return 0;

          case DS_ENETCLOSEINPROGRESS:
            continue;

          case DS_ENETISCONN:
            bsdcb_ptr->network_up = 1;
          case DS_ENETINPROGRESS:
            MSG_ERROR("bsdi_pppclose(): PPP did not close, error %d",
                     errno, 0, 0);
            errno = EFAULT;
            return -1;

          case DS_EBADAPP:
          default:
            errno = EFAULT;
            return -1;
        }
      }
      else
      {
        /*---------------------------------------------------------------------
          dss_netstatus() should always return DSS_ERROR.
        ---------------------------------------------------------------------*/
        errno = EFAULT;
        return -1;
      }
    } /* while (1) */
#endif /* BSD_BACKGROUND_PPPCLOSE */
  }
  else
  {
    errno = EFAULT;
    MSG_ERROR("bsdi_pppclose(): Error %d closing PPP", errno, 0, 0);
    return -1;
  }

  return 0;
} /* bsdi_pppclose() */

/*===========================================================================

FUNCTION BSDI_ALLOC_BSDCB()

DESCRIPTION
  Allocates a BSD control block in the BSD control block array.  Calls
  dss_open_netlib() to make the underlying DS Sock layer accessible.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Pointer to a BSD control block if successful, NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static bsdcb_type *bsdi_alloc_bsdcb
(
  void
)
{
  int index;                                            /* Loop index      */
  unsigned long int_sav;                                /* Interrupt state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Loop through array to find available BSDCB entry.  Protect access to the
    array.
  -------------------------------------------------------------------------*/
  for (index = 0; index < DSS_MAX_APPS; index++)
  {
    INTLOCK_SAV(int_sav);
    if (bsdcb_array[index].owning_tcb_ptr == NULL)
    {
      bsdcb_array[index].owning_tcb_ptr = rex_self();
      INTFREE_SAV(int_sav);
      bsdcb_array[index].owning_tcb_ptr->bsdcb_ptr = bsdcb_array + index;
      break;
    }
    else
    {
      INTFREE_SAV(int_sav);
    }
  }

  if (index == DSS_MAX_APPS)
  {
    /*-----------------------------------------------------------------------
      No more BSD control block is available.
    -----------------------------------------------------------------------*/
    return NULL;
  }
  return &bsdcb_array[index];

} /* bsdi_alloc_bsdcb() */

/*===========================================================================

FUNCTION BSDI_APPID2BSDCB()

DESCRIPTION
  Maps an application ID to its corresponding BSD control block in the BSDCB
  array.

DEPENDENCIES
  None.

PARAMETERS
  app_id     application id

RETURN VALUE
  Pointer to BSD control block if successful, NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static bsdcb_type *bsdi_appid2bsdcb
(
  sint15 app_id
)
{
  int index;                                  /* Loop index                */
  bsdcb_type *bsdcb_ptr = NULL;               /* BSD control block pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Match the app_id to find the application's BSD control block.
  -------------------------------------------------------------------------*/
  for (index = 0; index < DSS_MAX_APPS; index++)
  {
    if (bsdcb_array[index].netlib_app_id == app_id)
    {
      bsdcb_ptr = bsdcb_array + index;
      break;
    }
  }
  return bsdcb_ptr;

} /* bsdi_appid2bsdcb() */

/*===========================================================================

FUNCTION BSDI_MAP_ERRNO()

DESCRIPTION
  Translate a DSS-specific error value to an appropriate BSD errno value.

DEPENDENCIES
  None.

PARAMETERS
  dss_errno     error value from DS Sock layer

RETURN VALUE
  Pointer to BSD control block if successful, NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static int bsdi_map_errno
(
  sint15 dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(dss_errno)
  {
    case DS_EBADAPP:
      dss_errno = EFAULT;
      break;

    default: ;
  }
  return dss_errno;

} /* bsdi_map_errno() */

/*===========================================================================

FUNCTION BSDI_SOCKET_CB_FCN()

DESCRIPTION
  An application's callback function for a socket event.  Sets the socket
  signal for the application.

DEPENDENCIES
  None.

PARAMETER
  *ptr     pointer to an application id

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bsdi_socket_cb_fcn
(
  void *ptr
)
{
  bsdcb_type *bsdcb_ptr;                      /* BSD control block pointer */
  sint15 app_id = *(sint15 *)ptr;             /* Application ID            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = bsdi_appid2bsdcb(app_id);
  INTLOCK()
  if (bsdcb_ptr != NULL && bsdcb_ptr->owning_tcb_ptr != NULL)
  {
    (void)rex_set_sigs(bsdcb_ptr->owning_tcb_ptr, bsdcb_ptr->socket_sig);
  }
  INTFREE()
  
} /* bsdi_socket_cb_fcn() */

/*===========================================================================

FUNCTION BSDI_NETWORK_CB_FCN()

DESCRIPTION
  An application's callback function for a network event.  Sets the network
  signal for the application.

DEPENDENCIES
  None.

PARAMETERS
  *ptr     pointer to an application id

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bsdi_network_cb_fcn
(
  void *ptr
)
{
  bsdcb_type *bsdcb_ptr;                      /* BSD control block pointer */
  sint15 app_id = *(sint15 *)ptr;             /* Application ID            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = bsdi_appid2bsdcb(app_id);

  INTLOCK()
  if (bsdcb_ptr != NULL && bsdcb_ptr->owning_tcb_ptr != NULL)
  {
    bsdcb_ptr->network_event_occurred = 1;
    (void)rex_set_sigs(bsdcb_ptr->owning_tcb_ptr, bsdcb_ptr->network_sig);
  }
  INTFREE()
} /* bsdi_network_cb_fcn() */

/*===========================================================================

FUNCTION BSDI_DNS_CB_FCN()

DESCRIPTION
  An application's callback function for a domain name server event.  Sets
  the DNS signal for the application.

DEPENDENCIES
  None.

PARAMETER
  session_handle      dns session handle
  query_handle        address query handle
  api_type            dns api type
  num_records         number of address records
  user_data_ptr       application passed pointer
  dss_errno           error code

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bsdi_dns_cb_fcn(dss_dns_session_mgr_handle_type   session_handle,
                            dss_dns_query_handle_type         query_handle,
                            dss_dns_api_type_enum_type        api_type,
                            uint16                            num_records,
                            void                            * user_data_ptr,
                            int16                             dss_errno)
{
  bsdcb_type *bsdcb_ptr;                      /* BSD control block pointer */
  struct dss_hostent* hostent_ptr = NULL;
  dss_dns_addrinfo rec_buf;
  int16 retval = DSS_SUCCESS;
  int16 cb_errno = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (api_type == DSS_DNS_API_ADDRINFO && num_records > 0)
  {
    // Read address info. For now, just read one address ...
    retval = dss_dns_read_addrinfo(session_handle, 
                                   query_handle,
                                   &rec_buf,
                                   1,
                                   &cb_errno);
    if (DSS_ERROR == retval)
    {
      MSG_ERROR("dss_dns_read_addrinfo returned error %d", cb_errno, 0, 0);
    }
    else
    {
      // Process rec_buf, convert it to hostent structure
      if (AF_INET == rec_buf.ai_family || AF_INET6 == rec_buf.ai_family)
      {
        // Note: To minimize the change impact, use helper function to convert address 
        // information to an dss_hostent structure. A more optimized way is
        // to use rec_buf.sockaddr_storage directly, which would requires corresponding
        // changes in other parts of dsbsd and dsbsd's clients.
        // 
        // Use "dummy.hostname" since we don't care about hostname here. 
        // Refer to dss_dns_hostent.c for the details of this parameter
        hostent_ptr = bsdi_dns_construct_hostent_name_results(rec_buf.ai_family,
                                                              "dummy.hostname",
                                                              &rec_buf,
                                                              1,
                                                              &cb_errno);
        if (hostent_ptr == NULL)
        {
          MSG_ERROR("dns record can't be converted to hostent", 0, 0, 0);
          cb_errno = DS_EFAULT;
        }
      } // rec_buf.ai_family
      else
      {
        MSG_ERROR("dns record is not either IPv4 or IPv6", 0, 0, 0);
        cb_errno = DS_EFAULT;
      }
    } //reval == DSS_SUCCESS
  }// api_type == DSS_DNS_API_ADDRINFO

  bsdcb_ptr = bsdi_appid2bsdcb(*((sint15 *)(user_data_ptr)));
  INTLOCK()
  if (bsdcb_ptr != NULL && bsdcb_ptr->owning_tcb_ptr != NULL)
  {
    if (hostent_ptr != NULL)
    {
      bsdcb_ptr->hostent_ptr = (struct hostent *)hostent_ptr;
    }
    bsdcb_ptr->bsd_errno = dss_errno;
    if (cb_errno != DSS_SUCCESS)
    {
      bsdcb_ptr->bsd_errno = cb_errno;
    }
    (void)rex_set_sigs(bsdcb_ptr->owning_tcb_ptr, bsdcb_ptr->dns_sig);
  }
  else if (hostent_ptr != NULL)
  {
    bsdi_freehostent((struct dss_hostent *)hostent_ptr);
  }
  INTFREE()
} /* bsdi_dns_cb_fcn() */

/*===========================================================================
FUNCTION BSDI_DNS_CONSTRUCT_HOSTENT_NAME_RESULTS()

DESCRIPTION
  Constructs a hostent structure from the given generic RR Q

PARAMETERS
  ques_type   - Type of query initially made.
  addr_family - Address family for the query.
  rr_q        - A queue of generic resource records.

RETURN VALUE
  A hostent struct containing the results.
  NULL if rr_q is NULL or memory cannot be allocated for hostent or
  any field of hostent.

DEPENDENCIES
  This is an exact copy of dss_dnsi_construct_hostent_name_results() from
  dss_dns_hostent.c, which is internal to DS service so we have to redefine
  it here for our usage.

SIDE EFFECTS
  None.
===========================================================================*/
static dss_hostent  * bsdi_dns_construct_hostent_name_results
(
  int16                           addr_family,
  char                          * query_data_ptr,
  dss_dns_addrinfo              * results_buf,
  uint16                          num_records,
  int16                         * dss_errno
)
{
  struct dss_hostent            * hostent_ptr = NULL;
  struct dss_dns_addrinfo       * ai_ptr;
  uint16                          num_aliases = 0;
  uint16                          num_address = 0;
  struct in_addr                * temp_inaddr_ptr = NULL;
  struct in6_addr               * temp_in6addr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == dss_errno) ||
      (NULL == results_buf) ||
      (NULL == query_data_ptr) ||
      ((AF_INET != addr_family) && (AF_INET6 != addr_family)) ||
      (num_records == 0))
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Loop through the addrinfo records to find number of elements in
    the alias and address array
  -------------------------------------------------------------------------*/
  for( ai_ptr = results_buf; ai_ptr < results_buf + num_records; ai_ptr++ )
  {
    /*-------------------------------------------------------------------
      dss_hostent can't mix v4 and v6 addresses.  Skip address type not
      queried for.
    -------------------------------------------------------------------*/
    if( addr_family == ai_ptr->ai_family )
    {
      num_address++;
    }
    if (NULL != ai_ptr->ai_canonname && 0 != *(ai_ptr->ai_canonname) )
    {
      num_aliases++;
    }
  }

  /*-------------------------------------------------------------------------
    Allocate memory for the hostent, aliases and address lists.  In case of
    failure return with DSS_ERROR and the error code set to DS_ENOMEM.
    Memset aliases and address array to zero
  -------------------------------------------------------------------------*/
  if( NULL != (hostent_ptr = (struct dss_hostent*)malloc( sizeof(struct dss_hostent) )) )
  {
    hostent_ptr->h_aliases = (char**)malloc((num_aliases + 1) * sizeof(char *));

    hostent_ptr->h_addr_list = (char**)malloc((num_address + 1) * sizeof(char *));
  }

  if( NULL == hostent_ptr ||
      NULL == hostent_ptr->h_aliases ||
      NULL == hostent_ptr->h_addr_list )
  {
    bsdi_freehostent( hostent_ptr );
    *dss_errno = DS_ENOMEM;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Initialize aliases and addr_list arrays to zero.  Set h_name to NULL,
    h_addrtype and h_length to values.
  -------------------------------------------------------------------------*/
  memset( hostent_ptr->h_aliases, 0, sizeof(char*) * (num_aliases + 1) );
  memset( hostent_ptr->h_addr_list, 0, sizeof(char*) * (num_address + 1) );
  num_aliases              = 0;
  num_address              = 0;
  hostent_ptr->h_name      = NULL;
  hostent_ptr->h_addrtype  = addr_family;
  hostent_ptr->h_length    = ( AF_INET == addr_family ) ?
                              sizeof(struct in_addr)    :
                              sizeof(struct in6_addr);

  /*-------------------------------------------------------------------------
    Traverse the queue and fill up the hostent.
  -------------------------------------------------------------------------*/
  for( ai_ptr = results_buf; ai_ptr < results_buf + num_records; ai_ptr++ )
  {
    if( AF_INET == addr_family )
    {
      hostent_ptr->h_addr_list[num_address] = 
        (char*)malloc( sizeof(struct in_addr) );
      if( NULL == hostent_ptr->h_addr_list[num_address] )
      {
        bsdi_freehostent( hostent_ptr );
        *dss_errno = DS_ENOMEM;
        return NULL;
      }

      temp_inaddr_ptr =
        &(((struct sockaddr_in *)&(ai_ptr->ai_sockaddr))->sin_addr);

      memcpy( hostent_ptr->h_addr_list[num_address],
              temp_inaddr_ptr,
              sizeof(struct in_addr) );
      num_address++;
    }

    else if( AF_INET6 == addr_family )
    {
      hostent_ptr->h_addr_list[num_address] = 
        (char*)malloc( sizeof(struct in6_addr) );
      if( NULL == hostent_ptr->h_addr_list[num_address] )
      {
        bsdi_freehostent( hostent_ptr );
        *dss_errno = DS_ENOMEM;
        return NULL;
      }

      temp_in6addr_ptr =
        &(((struct sockaddr_in6 *)&(ai_ptr->ai_sockaddr))->sin6_addr);

      memcpy( hostent_ptr->h_addr_list[num_address],
              temp_in6addr_ptr,
              sizeof(struct in6_addr) );
      num_address++;
    }
    else
    {
      break;
    } /* if-else addr_family */

    if( 0 != *(ai_ptr->ai_canonname) )
    {
      if( NULL == hostent_ptr->h_name )
      {
        hostent_ptr->h_name = 
          (char*)malloc(std_strlen(ai_ptr->ai_canonname) + 1);
        if( NULL == hostent_ptr->h_name )
        {
          bsdi_freehostent( hostent_ptr );
          *dss_errno = DS_ENOMEM;
          return NULL;
        }

        std_strlcpy( hostent_ptr->h_name,
                     ai_ptr->ai_canonname,
                     DSS_DNS_MAX_DOMAIN_NAME_LEN );
      }
      else
      {
        hostent_ptr->h_aliases[num_aliases] =
          (char*)malloc(std_strlen(ai_ptr->ai_canonname) + 1);
        if( NULL == hostent_ptr->h_addr_list[num_aliases] )
        {
          bsdi_freehostent( hostent_ptr );
          *dss_errno = DS_ENOMEM;
          return NULL;
        }

        std_strlcpy( hostent_ptr->h_aliases[num_aliases],
                     ai_ptr->ai_canonname,
                     DSS_DNS_MAX_DOMAIN_NAME_LEN );
        num_aliases++;
      }
    } /* if-else 0 != *(ai_ptr->ai_canonname) */

  } /* for */

  /*-------------------------------------------------------------------------
    If h_name is still NULL, copy the query data into h_name
  -------------------------------------------------------------------------*/
  if( NULL == hostent_ptr->h_name )
  {
    hostent_ptr->h_name =
      (char*)malloc(std_strlen(query_data_ptr) + 1);
    if( NULL == hostent_ptr->h_name )
    {
      bsdi_freehostent( hostent_ptr );
      *dss_errno = DS_ENOMEM;
      return NULL;
    }

    std_strlcpy( hostent_ptr->h_name,
                 query_data_ptr,
                 DSS_DNS_MAX_DOMAIN_NAME_LEN );
  }

  return hostent_ptr;
} /* bsdi_dns_construct_hostent_name_results() */

/*===========================================================================
FUNCTION bsdi_freehostent()

DESCRIPTION
  Frees the hostent structure and associated memory

PARAMETERS
  hostent_ptr   - dss_hostent to be freed.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void bsdi_freehostent(struct dss_hostent *phostent)
{
  int index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == phostent)
  {
    MSG_ERROR("Null argument passed", 0, 0, 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Free the aliases list
  -------------------------------------------------------------------------*/
  if( NULL != phostent->h_aliases )
  {
    index = 0;
    while( NULL != phostent->h_aliases[index] )
    {
      free( phostent->h_aliases[index++] );
    } /* while */
    free( phostent->h_aliases );
  }

  /*-------------------------------------------------------------------------
    Free the address list
  -------------------------------------------------------------------------*/
  if( NULL != phostent->h_addr_list )
  {
    index = 0;
    while( NULL != phostent->h_addr_list[index] )
    {
      free( phostent->h_addr_list[index++] );
    } /* while */
    free( phostent->h_addr_list );
  }

  if( NULL != phostent->h_name)
  {
    free( phostent->h_name );
  }

  /*-------------------------------------------------------------------------
    Free the hostent
  -------------------------------------------------------------------------*/
  free( phostent );

  return;
}// bsdi_freehostent()

#ifndef PLATFORM_LTK
#if defined(FEATURE_QTV_QOS_SELECTION) || defined(FEATURE_QTV_MCAST_IOCTL)
/*===========================================================================

FUNCTION SetInUseFlag()

DESCRIPTION
  This function Sets in InUse flag of the Call Back Information Structure 

DEPENDENCIES
  None.

PARAMETER
  Pointer to the Structure

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void SetInUseFlag
(bsdi_iface_ioctl_cb_info_type *info_elem)
{
  info_elem->InUse=TRUE;
}
/*===========================================================================

FUNCTION ResetInUseFlag()

DESCRIPTION
  This function Resets in InUse flag of the Call Back Information Structure 

DEPENDENCIES
  None.

PARAMETER
  Pointer to the Structure

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ResetInUseFlag
(bsdi_iface_ioctl_cb_info_type *info_elem)
{
  info_elem->InUse=FALSE;
}

/*===========================================================================

FUNCTION BSDI_IFACE_IOCTL_CB_FCN()

DESCRIPTION
  This callback function is called in the PS context to handle an iface ioctl 
  event. All this function does is to enqueue all the information in the 
  ioctl info callback info queue and sets a signal to the task owning the 
  appid.

DEPENDENCIES
  None.

PARAMETER
  event: The ioctl event for which this callback is being invoked
  event_info: Event specific information
  user_data: User data given at the time of installing this callback
  app_id: Application id
  iface_id: Interface id for which the event has happened

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bsdi_iface_ioctl_cb_fcn
(
  dss_iface_ioctl_event_enum_type event,
  dss_iface_ioctl_event_info_union_type event_info,
  void* user_data,
  sint15 app_id,
  dss_iface_id_type iface_id
)
{
  bsdcb_type *bsdcb_ptr;                      /* BSD control block pointer */
  static bsdi_iface_ioctl_cb_info_type info_elem[MAX_NUM_OF_CB_INFO];
  static boolean info_elem_init = FALSE;

  int cb_info_num=MAX_NUM_OF_CB_INFO;
  int i;
  if (!info_elem_init)
  {
    memset(info_elem, 0x00, 
         MAX_NUM_OF_CB_INFO * sizeof(bsdi_iface_ioctl_cb_info_type));
    info_elem_init = TRUE;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("bsdi_iface_ioctl_cb_fcn: iface_id=0x%x, event=%d", 
           iface_id, 
           event,
           0);

  bsdcb_ptr = bsdi_appid2bsdcb(app_id);

  if (bsdcb_ptr == NULL)
  {
    MSG_ERROR("error: bsdcb_ptr == NULL ", errno, 0, 0);
    errno = EFAULT;
    return;
  }

  //as qtv still using deprecated bcmcs api, we filter out new 
  //duplicate events
  if (event_info.mcast_info.info_code
       >= DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_DEPRECATED_INFO_CODE)
  {
    MSG_ERROR("dsbsd:rejecting a duplicate event", 0, 0, 0);
    return;
  }

  for (i=0;i < MAX_NUM_OF_CB_INFO;i++)
  {
    if (info_elem[i].InUse!=TRUE)
    {
      cb_info_num=i;
      break;
    }
  
  }
   
  if (cb_info_num>=MAX_NUM_OF_CB_INFO)
  {
    MSG_ERROR("Failed to allocate info_elem for ioctl_cb_info", 0, 0, 0);
  }

  else
  {
    MSG_HIGH("bsdi_iface_ioctl_cb_fcn: Index %d", cb_info_num, 0, 0);
    (void)q_link(&(info_elem[cb_info_num]), &(info_elem[cb_info_num].link));
    info_elem[cb_info_num].event = event;
    info_elem[cb_info_num].event_info = event_info;
    info_elem[cb_info_num].user_data = user_data;
    info_elem[cb_info_num].app_id = app_id;
    info_elem[cb_info_num].iface_id = iface_id;
    SetInUseFlag(&info_elem[cb_info_num]);
    q_put(&bsdi_iface_ioctl_cb_info_q, &(info_elem[cb_info_num].link));
  }

  INTLOCK()
  if (bsdcb_ptr->owning_tcb_ptr != NULL)
  {
    (void)rex_set_sigs(bsdcb_ptr->owning_tcb_ptr, bsdcb_ptr->iface_ioctl_sig);
  }
  INTFREE()
  return;
} /* bsdi_iface_ioctl_cb_fcn() */

/*===========================================================================

FUNCTION BSDI_IFACE_IOCTL_SIG_HANDLER()

DESCRIPTION
  This signal handler handles the iface ioctl signal set by the iface 
  ioctl callback function. Currently, this function simply gets the 
  information about the QoS event. In future, this function may be enhanced
  to take an action (calling a user installed callback) depending on what 
  event has happened. 

DEPENDENCIES
  None.

PARAMETER
  sigs: The signal mask
  user_data: The user data specified at the time of installing this handler

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int
bsdi_iface_ioctl_sig_handler
(
  rex_sigs_type sigs,
  void* user_data
)
{
  bsdcb_type *bsdcb_ptr;                      /* BSD control block pointer */
  bsd_status_enum_type retVal;
  event_info_type event_info;
  int count = 0;
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  NULL_CHECK(bsdcb_ptr, EFAULT)

  (void)rex_clr_sigs(rex_self(), bsdcb_ptr->iface_ioctl_sig);

  MSG_HIGH("bsdi_iface_ioctl_sig_handler() called", 0, 0, 0);

  // Multiple events may have already been queued. If so continue to process them.
  // One way to do it is setting the signal at the end of this handler, such that 
  //    this handler will be invoked again to process next item. The drawback of 
  //    doing this is potential long delay of processing time due to current 
  //    implementation of ioctl_sig / ioctl_sig_handler (refer to bsdi_wait())
  // The other alternative is processing the events in batch mode. It will save
  //    lots of processing time. But need to watch out the stickness of call back 
  //    routine and prevent deal loop. Theoretically, QoS event may trigger dead
  //    loop scenario(DEACTIVATED->AVAILABLE(upon reactivation)->DEACTIVATED->...).
  //    But in reality, this can't happen. Still we are using MAX_CONSECUTIVE_PROCESS_CNT
  //    to throttle for the sake of safety   
  // In case this needs to be further improved, one solution is to use link instead 
  //    of queue, such that only MCAST events will be process in batch mode. Or maintain
  //    different queue for QoS and MCAST.
#define MAX_CONSECUTIVE_PROCESS_CNT 6
  while (q_cnt(&bsdi_iface_ioctl_cb_info_q)>0 && 
         (count++) < MAX_CONSECUTIVE_PROCESS_CNT)
  {
#ifdef FEATURE_QTV_MCAST_IOCTL
     event_info.mcast_handle = 0;
#endif
     retVal = bsdi_get_event_info(&event_info);
     MSG_HIGH("bsdi_get_event_info() returned %d Count %d", retVal, count, 0);

     /* Call the application installed callback here */
#ifdef FEATURE_QTV_MCAST_IOCTL
     if (retVal >= BSD_MCAST_STATUS_MIN && retVal <= BSD_MCAST_STATUS_MAX)
     {
       if (bsdcb_ptr->mcast_cb_flag && bsdcb_ptr->cb_fcn)
       {
         bsdcb_ptr->cb_fcn(retVal, bsdcb_ptr->mcast_cb_data, 
                           (void*)event_info.mcast_handle);
       }
     }
#endif
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif
  }//q_cnt()

  // We clear the signal when we enter the function and process  
  //a maximum of  MAX_CONSECUTIVE_PROCESS_CNT events before exiting
  //from the above while loop. If there are more events in the queue
  //that need to processed,set the signal so that this task processes
  //them the next time it calls rex_wait within bsdi_wait(..)
  if(q_cnt(&bsdi_iface_ioctl_cb_info_q)>0)
  {
     rex_set_sigs(rex_self(), bsdcb_ptr->iface_ioctl_sig);
  }
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif

  return 0;
}

/*===========================================================================
FUNCTION BSDI_GET_EVENT_INFO

DESCRIPTION
  Dequeues the event information from the ioctl event info queue and 
  provides feed back information to the caller (QOS: granted qos spec)

DEPENDENCIES
  Should be called after getting a ioctl event.

PARAMETER
  event_info_type* event_info
  {
    *granted_qos_spec: Returns the granted qos spec to the caller.
    mcast_handle:     Returns the corresponding mcast handler.
  }

RETURN VALUE
bsd_status_enum type:
  BSD_QOS_STATUS_AVAILABLE: The requested QoS is available.
  BSD_QOS_STATUS_AVAILABLE_MODIFIED: The requested QoS is not available but
    the specified minimum QoS is availble.
  BSD_QOS_STATUS_UNAVAILABLE: The specified minimum QoS is not available.
  BSD_QOS_STATUS_ERROR: An error was encountered while requesting/obtaining
    QoS.
  BSD_QOS_STATUS_DEACTIVATED: The requested QoS has been revoked.
  ...

SIDE EFFECTS
  None.

===========================================================================*/
static bsd_status_enum_type
bsdi_get_event_info
(
  event_info_type *event_info
)
{
  bsd_status_enum_type retVal = BSD_STATUS_ERROR;
  bsdi_iface_ioctl_cb_info_type* info_elem;
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  info_elem = (bsdi_iface_ioctl_cb_info_type *) 
                q_get(&bsdi_iface_ioctl_cb_info_q);

  if (info_elem == NULL)
  {
    MSG_ERROR("Error in getting the q element", 0, 0, 0);
    return BSD_STATUS_ERROR;
  }

  switch (info_elem->event)
  {
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

#ifdef FEATURE_QTV_MCAST_IOCTL
    case DSS_IFACE_IOCTL_MCAST_REGISTER_SUCCESS_EV:
      MSG_HIGH("Requested multicast registration successful", 0, 0, 0);
      retVal = BSD_MCAST_STATUS_SUCCESS;
      if (event_info != NULL) 
      {
        event_info->mcast_handle = info_elem->event_info.mcast_info.handle;
      }
      break;

    case DSS_IFACE_IOCTL_MCAST_REGISTER_FAILURE_EV:
      MSG_HIGH("Mcast ioctl request failed, info code = %d", 
               info_elem->event_info.mcast_info.info_code, 
               0, 
               0);
      if((info_elem->event_info.mcast_info.info_code >= 300)
          && (info_elem->event_info.mcast_info.info_code < 400))
      {
          //Not a Permanent Failure
          //Do nothing
          retVal = BSD_MCAST_STATUS_WAITING;
      }
      else
      {
          //Permanent Failure
          retVal = BSD_MCAST_STATUS_FAILED;
      }
      print_mcast_event(info_elem->event_info.mcast_info.info_code);
      if (event_info != NULL) 
      {
        event_info->mcast_handle = info_elem->event_info.mcast_info.handle;
      }
      break;

    case DSS_IFACE_IOCTL_MCAST_DEREGISTERED_EV:
      MSG_HIGH("multicast deregistred, info code = %d", 
               info_elem->event_info.mcast_info.info_code, 
               0, 
               0);
      retVal = BSD_MCAST_STATUS_DEREGISTERED;
      if (event_info != NULL) 
      {
        event_info->mcast_handle = info_elem->event_info.mcast_info.handle;
      }
      break;
#endif
    default:
      MSG_ERROR("Unknown event received, %d",  info_elem->event, 0, 0);
      ResetInUseFlag(info_elem);
      return BSD_STATUS_ERROR;
  }
  
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif

  ResetInUseFlag(info_elem);

  return retVal;
}
#endif /* FEATURE_QTV_QOS_SELECTION || FEATURE_QTV_MCAST_IOCTL */
#endif /* !PLATFORM_LTK */

/*===========================================================================

FUNCTION BSDI_MAP_BSD_DSS_SOCKOPT_LEVEL()

DESCRIPTION
  Maps a BSD socket option level to DSS sockets API socket option level

DEPENDENCIES
  None.

PARAMETER
  level      BSD socket option level       

RETURN VALUE
  DSS Sockets API socket option level

SIDE EFFECTS
  None.

===========================================================================*/
static int 
bsdi_map_bsd_dss_sockopt_level
(
  int level
)
{
  int dss_level = -1;

  switch (level)
  {
    case IPPROTO_IP:
      dss_level = DSS_IPPROTO_IP;
      break;

    case SOL_SOCK:
      dss_level = DSS_SOCK;
      break;

    case IPPROTO_TCP:
      dss_level = DSS_IPPROTO_TCP;
      break;

    default:
      break;
  }

  return dss_level;
}

/*===========================================================================

FUNCTION BSDI_MAP_BSD_DSS_SOCKOPT_NAME()

DESCRIPTION
  Maps a BSD socket option name to DSS sockets API socket option name

DEPENDENCIES
  None.

PARAMETER
  optname      BSD socket option name       

RETURN VALUE
  DSS Sockets API socket option name: in case of success
  -1: otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static int 
bsdi_map_bsd_dss_sockopt_name
(
  int optname
)
{
  int dss_name = -1;

  switch (optname)
  {

#if defined(FEATURE_QTV_MCAST) && !defined(PLATFORM_LTK)
    case IP_ADD_MEMBERSHIP:
      dss_name = DSS_BCMCS_JOIN;
      break;

    case IP_DROP_MEMBERSHIP:
      dss_name = DSS_BCMCS_LEAVE;
      break;
#endif /* FEATURE_QTV_MCAST */

    case SO_LINGER:
      dss_name = DSS_SO_LINGER;
      break;

    case SO_RCVBUF:
      dss_name = DSS_SO_RCVBUF;
      break;

#ifdef TCP_SACK_ENABLED
    case TCP_SACK:
      dss_name = DSS_TCP_SACK;
      break;
#endif /* TCP_SACK_ENABLED */

    case TCP_NODELAY:
      dss_name = DSS_TCP_NODELAY;
      break;

    default:
      break;
  }

  return dss_name;
}

#if defined(FEATURE_QTV_MCAST) && !defined(PLATFORM_LTK)
/*===========================================================================

FUNCTION BSDI_MAP_BSD_DSS_OPTVAL_IP_MREQ()

DESCRIPTION
  Maps a BSD socket option name to DSS sockets API socket option name

DEPENDENCIES
  None.

PARAMETER
  optval   BSD socket option value
  optlen   Length of optval
  in_addr* Pointer to the dss specific socket option value to be returned

RETURN VALUE
  >= 0: in case of success
  < 0: otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static int
bsdi_map_bsd_dss_optval_ip_mreq
(
  void* optval,
  int   optlen,
  IpAddrType* dss_mcast_info 
)
{
  struct ip_mreq* mcast_info = (struct ip_mreq *) optval;

  if ( (optlen != sizeof(struct ip_mreq)) || (dss_mcast_info == NULL) )
  {
    return -1;
  }

  *dss_mcast_info = mcast_info->imr_multiaddr;
  return 0;
}

/*===========================================================================

FUNCTION BSDI_MAP_BSD_DSS_BCAST_IFACE_NAME()

DESCRIPTION
  Maps a BSD bcast interface name to DSS sockets API bcast interface name

DEPENDENCIES
  None.

PARAMETER
  bcast_iface_name      bcast interface name       

RETURN VALUE
  DSS Sockets API bcast interface name: in case of success
  -1: otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static dss_iface_name_enum_type
bsdi_map_bsd_dss_bcast_iface_name
(
  bsd_iface_name_enum_type bcast_iface_name
)
{
  dss_iface_name_enum_type dss_name = DSS_IFACE_CDMA_BCAST;

  switch (bcast_iface_name)
  {
    case BSD_IFACE_CDMA_BCAST:
      dss_name = DSS_IFACE_CDMA_BCAST;
      break;

#if defined(DSS_VERSION) && (DSS_VERSION >= 1207)
    case BSD_IFACE_DVBH:
      dss_name = DSS_IFACE_DVBH;
      break;
#endif

#ifdef FEATURE_MBMS
#if defined(DSS_VERSION) && (DSS_VERSION >= 1207)
    case BSD_IFACE_MBMS:
      dss_name = DSS_IFACE_MBMS;
      break;
#endif
#endif /* FEATURE_MBMS */

#ifdef QTV_FEATURE_STA_IFACE
    case BSD_IFACE_STA:
        dss_name = DSS_IFACE_STA;
        break;
#endif
    default:
      dss_name = (dss_iface_name_enum_type) bcast_iface_name;
      break;
  }

  return dss_name;
}

#endif /* FEATURE_QTV_MCAST */
/*===========================================================================

FUNCTION GENERATE_UNIQUE_RANDOM_RANGE()

DESCRIPTION
  generated 100 unique random numbers

DEPENDENCIES
  None.

PARAMETER
  none

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void generate_unique_random_range(int8 *random_store)
{ 
  qword   q_uptime;
  int rand_max_val, fi, ri, t ;
  unsigned long seed = 0;
  rand_max_val = 100 ;

  /*seed the random genrator
    make sure see not exceed 2^31-1 range*/
  clk_uptime_ms(q_uptime);
  seed = qw_lo(q_uptime);
  seed &= 0x7fff;
  ran_seed(seed);

  /*make the range unique*/
  for ( fi = 0 ; fi < rand_max_val ; fi++ ) 
  {
    random_store[fi] = fi + 1 ; 
  }

  for ( fi = 0 ; fi < rand_max_val - 1 ; fi++ )
  { 
    ri = ran_dist(ran_next(), 0, rand_max_val-fi);
    t = random_store[fi] ; random_store[fi] = random_store[fi+ri] ;
    random_store[fi+ri] = t ; 
  }
}
/*===========================================================================

FUNCTION BSDI_PACKET_DROP_INIT()

DESCRIPTION
  Inititize the packet drop rate and packet drop index

DEPENDENCIES
  None

PARAMETER
  udp_drop_rate: The configured UDP packet drop rate
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void 
bsd_packet_drop_init
(
  uint8 udp_droprate 
)
{
  udp_drop_rate = udp_droprate;
  udp_packet_drop_index = 0 ;
  MSG_HIGH("UDP Droprate=%d, Memory Location=%p",
                                    udp_drop_rate, &udp_drop_rate, 0);
  
}

#ifndef PLATFORM_LTK
/*===========================================================================
FUNCTION bsdi_map_bsd_dss_ioctl_type

DESCRIPTION
  Maps BSD IOCTL Type to DSS IOCTL type

DEPENDENCIES
  None.

PARAMETER
  BSD IOCTL Type
  
RETURN VALUE
  DSS IOCTL Type
  
SIDE EFFECTS
  None.

===========================================================================*/
static dss_iface_ioctl_type bsdi_map_bsd_dss_ioctl_type
(bsd_iface_ioctl_type ioctl_type)
{
   dss_iface_ioctl_type dss_ioctl_type = DSS_IFACE_IOCTL_MIN;
   switch (ioctl_type)
   {
#if !defined (PLATFORM_LTK) && (defined(DSS_VERSION) && (DSS_VERSION >= 1100))
      case BSD_IFACE_IOCTL_GET_BEARER_TECHNOLOGY:
         dss_ioctl_type = DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY;
         break;

      case BSD_IFACE_IOCTL_GET_DATA_BEARER_RATE:
         dss_ioctl_type = DSS_IFACE_IOCTL_GET_DATA_BEARER_RATE;
         break;
      case BSD_IFACE_IOCTL_GET_ST_DATA_BEARER_RATE:
         dss_ioctl_type = DSS_IFACE_IOCTL_GET_DATA_BEARER_RATE;
         break; 
#endif
      case BSD_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM:
        dss_ioctl_type = DSS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM;
        break;
      default:
         break;

   }
   return dss_ioctl_type;
}

/*===========================================================================
FUNCTION BSD_DSS_IOCTL_IFACE

DESCRIPTION
  Requests for the current system mode in which the UE is operating. 

DEPENDENCIES
  None.

PARAMETER
  bsd_bearer_tech_type: bearer structure which needs to be populated 
  by data services
  errno: Return Error Type

RETURN VALUE
  BSD_QOS_STATUS_WAITING: The QoS request is in process.
  BSD_QOS_STATUS_ERROR: An error was encountered while requesting/obtaining
    QoS.

SIDE EFFECTS
  None.

===========================================================================*/
int 
bsd_dss_iface_ioctl
(
  void* arg_ptr,
  bsd_iface_ioctl_type  ioctl_type
)
{
   sint15 error;
   int retVal = -1;
   bsdcb_type   *bsdcb_ptr;
   dss_iface_id_type primary_iface_id;
   dss_iface_ioctl_type dss_ioctl_type;

   bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
   if ((bsdcb_ptr == NULL) || (arg_ptr == NULL))
   {
      MSG_ERROR("Error: bsdcb_ptr == NULL", 0, 0, 0);
      errno = EFAULT;
      return retVal;
   }
   dss_ioctl_type = bsdi_map_bsd_dss_ioctl_type(ioctl_type);
   if (dss_ioctl_type > 0)
   {
  
       primary_iface_id = dss_get_iface_id(bsdcb_ptr->netlib_app_id);
       retVal = dss_iface_ioctl(primary_iface_id, 
                dss_ioctl_type, arg_ptr, &error);
       errno = error;
   }
   return retVal;
}
/*===========================================================================
FUNCTION BSD_DSS_IOCTL_IFACE_EX

DESCRIPTION
  Requests for the current system mode in which the UE is operating. 

DEPENDENCIES
  None.

PARAMETER
  bsd_bearer_tech_type: bearer structure which needs to be populated 
  by data services
  errno: Return Error Type

RETURN VALUE
  BSD_QOS_STATUS_WAITING: The QoS request is in process.
  BSD_QOS_STATUS_ERROR: An error was encountered while requesting/obtaining
    QoS.

SIDE EFFECTS
  None.

===========================================================================*/

int 
bsd_dss_iface_ioctl_ex
(
  void* arg_ptr,
  bsd_iface_ioctl_type  ioctl_type,
  void* data_ptr
)
{
  sint15 error;
  int retVal = -1;
  bsdcb_type   *bsdcb_ptr;
  dss_iface_id_type iface_id;
  dss_iface_ioctl_type dss_ioctl_type;

  bsdcb_ptr = (bsdcb_type *)rex_self()->bsdcb_ptr;
  if ((bsdcb_ptr == NULL) || (arg_ptr == NULL))
  {
    MSG_ERROR("Error: bsdcb_ptr == NULL", 0, 0, 0);
    errno = EFAULT;
    return retVal;
  }
  dss_ioctl_type = bsdi_map_bsd_dss_ioctl_type(ioctl_type);
  if (dss_ioctl_type > 0)
  {
    // Here ioctl_type(=BSD_GET_ST_RAB_DCH_RATE) is used just to 
    // identify whether it's primary PDP context or seconadary PDP context.
    // but the dss_ioctl_type will be DSS_IFACE_IOCTL_GET_DATA_BEARER_RATE .
  
    if(ioctl_type == BSD_IFACE_IOCTL_GET_ST_DATA_BEARER_RATE)
    {
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
      {
        MSG_ERROR("Error: Invalid data_ptr/ioctl", 0, 0, 0);
        errno = EFAULT;
        return -1;
      }
    }
    else
    {
      iface_id = dss_get_iface_id(bsdcb_ptr->netlib_app_id);

    }
    retVal = dss_iface_ioctl(iface_id,dss_ioctl_type, arg_ptr, &error);
    errno = error;
   }
   return retVal;
}
#endif /* PLATFORM_LTK */

#endif /* FEATURE_DS_SOCKETS_BSD */
