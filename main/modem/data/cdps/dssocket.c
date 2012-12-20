/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  A P I  F I L E
                   
DESCRIPTION

 The Data Services mobile socket API File. Contains basic API functions for
 socket manipulation.
 
EXTERNALIZED FUNCTIONS

  dss_socket()
    Creates a socket and returns a socket descriptor.

  dss_connect()
    Initiates an active open for TCP connection.  For datagram sockets,
    merely binds the remote address.

  dss_accept()
    Returns the descriptor for a new passively opened connection.

  dss_listen()
    Initiates passive open for TCP connections.

  dss_open_netlib()
    Opens the network library.

  dss_close_netlib()
    Closes the network library.

  dss_pppopen()
    Brings up the network subsystem.

  dss_bind()
    For all client sockets, attaches a local address and port value to the 
    socket.

  dss_close()
    Close a socket, freeing it for reuse.

  dss_pppclose()
    Closes the network subsystem.

  dss_netstatus()
    Reports the status of the network subsystem.

  dss_async_select()
    Specify which events to wait on for a particular socket.

  dss_async_deselect()
    Clears specific events from the interest mask for a particular socket.

  dss_getnextevent()
    Get the next socket descriptor and events that have occurred.

  dss_write()
    Sends specified number of bytes across the TCP transport.

  dss_writev()
    Provides gather write variant, that allows writing from non-contiguous 
    buffers.

  dss_read()
    Reads specified number of bytes from the TCP transport.

  dss_readv()
    Provides scatter read variant, that allows reading into non-contiguous 
    buffers.

  dss_sendto()
    Sends specified number of bytes across the UDP transport.

  dss_recvfrom()
    Reads specified number of bytes from the UDP transport.

  dss_setsockopt()    
    Set the options associated with a socket.     

  dss_getsockopt()    
    Return an option associated with a socket.
   
  dss_getsockname()
    Returns the current local address assigned to the specified socket.

  dss_getpeername()
    Returns the address of the peer connected to the specified socket.

  dss_shutdown
    Shuts down a connected socket in the specified direction.

Copyright (c) 2003-2011 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssocket.c_v   1.29   27 Feb 2003 12:00:40   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssocket.c#9 $ $DateTime: 2011/04/18 01:26:00 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/18/11    sn     Fixed compilation error.
03/30/11    ms     Added support to pick non-default profile for non-OMH apps.
05/28/10    ps     Added support to pick up the profile id with unspecified bit
                   set for Legacy/Non-OMH Applications.
12/19/08    sp     Setting the value of data_session_profile_id to -1 in
                   dss_init_net_policy_info().
06/24/08    pp     Fixed RVCT compiler warnings.
06/07/08    pp     OMH-1b Profile Arbitration development.
07/28/04    SV     Added support for background ppp close.
03/09/04    vp     Removed FEATURE_JAVA from NODELAY. KEEPALIVE still under
                   FEATURE_JAVA
01/06/04    aku    Set ps_iface_ptr in acb to NULL in dss_pppopen() if iface 
                   is in GOING_DOWN state.
11/18/03    sv     Fixed bug in udp close where it is passing a NULL dsm item
                   to dssocki_send_to_ps.
11/05/03    sv     Changed dss_close for UDP sockets to free UCB and SCB
                   in PS context.
10/22/03    sv     Added check in dss_connect to return EINVAL if the 
                   destination port is 0.
10/21/03    sv     Fixed bug in dss_shutdown to allow read shutdown when
                   the socket is in CLOSING state. 
09/15/03    sv     Removed event_buf initialization after freeing the dsm
                   item.
08/06/03    sv     Added an error check to verify that tcb is not null
                   in dss_getsockopt for NODELAY socket option.
08/04/03    sv     Added SO_KEEPALIVE AND TCP_NODELAY socket options.
07/07/03    ss     Made changes to support SDB ack callback socket option.
07/06/03    sv     Added DSS_SO_SNDBUF socket option.
06/16/03    aku    Added MSGs in dss_open_netlib() and dss_socket().
06/04/03    om     Fixed MSS setting to adjust for IPSec ESP overhead,
                   fixed routing in connect() for IPSec case.
05/29/03    om     Changed ppp_open() to bypass IPSec in the routing lookup.
05/27/03    om     Changed calls to sendmsg() and recvmsg() for new signature,
                   added sendto() and recvfrom() for DSM chains.
05/15/03    usb    Modified pppopen to always return EWOULDBLOCK if iface in 
                   ROUTEABLE state.
05/13/03    usb    Perform routing lookup in dss_pppopen even if a specific
                   iface is required.
05/05/03    jd     Removed dependency on dsiface.h
05/04/03    ss     Modified dss_close() to support SO_LINGER socket option
05/02/03    usb    Replaced Asserts with MSG_ERRORs in public APIs, Changed 
                   dss_close() to return error if a valid dss_errno is not 
                   passed in, removed unwanted header file inclusions.
05/01/03    usb    A larger than required addrlen for address buffer should 
                   not generate an error.
04/30/03    aku    Moved dss_get_iface_id() to dss_iface_ioctl.c
04/25/03    aku    Moved dss_read_dsm_chain() and dss_write_dsm_chain() to
                   dssinteral.c and added support for app_specified_iface_ptr
04/23/03    ss     Modified setsockopt() and getsockopt() to reflect the new
                   socket option handling
04/14/03    sv     Consolidate I/o calls to use a single fucntion. Support
                   connect, getpeername, shutdown for udp sockets. Initialized
                   socket family in socket control block.
04/10/03    ss     Modified dss_shutdown() to return SUCCESS for nominal case
                   Added call to dssocki_is_addr_in_use() in dss_bind().
03/19/03    aku    Point to next item in queue when clearing the registered
                   event callbacks in dss_close_netlib()
03/18/03    ss     Return SUCCESS from pppclose() if iface_ptr in the acb
                   is NULL
03/07/03    jd     Return EWOULDBLOCK from pppopen() if iface is routeable
02/26/03    aku    Return EWOULDBLOCK from dss_recvfrom() if network isn't 
                   available
02/25/03    atp    Changed implementation of enable_dorm_timer() to call
                   ioctl instead of directly calling 707 pkt function.
02/24/03    aku    Access ps_iface_ptr from scb instead of acb and fixed bug
                   in dss_recvfrom() wherein the function was returning 
                   EWOULDBLOCK instead of ENETDOWN when the phy iface was 
                   down.
02/21/03    usb    Fixed dss_force_dormancy() to make it backward compatible,
                   does not check for appid and only works for 1X iface now
02/19/03    om     Fixed address based routing in connect() and sendto().
02/13/03    ss     Added new functions: getsockname, getpeername and shutdown
02/12/03    om     Fixed early routing in Connect() and Sendto().
02/12/03    atp    Removed FEATURE_707 from force_dormancy(), as GoDormant
                   ioctl has been made common rather than 707 specific.
02/10/03    atp    Added check for NULL ptr iface in force_dormancy().
02/07/03    atp    Changed implementation of force_dormancy() to call ioctl
                   instead of directly calling phys_link_down().
01/24/03    usb    Renamed dss_get_iface_name_and_instance() to 
                   dss_get_iface_id() and updated the prototype to use same 
                   iface_id as passed in dss_iface_ioctl(), added error 
                   handling for ps_iface_bring_up_cmd().
12/24/02    aku    Added function dss_get_iface_name_and_instance() and 
                   support for cleaning up event callbacks stored in acb_type
                   when dss_closenetlib() is called.
12/22/02    aku    IFACE_UP and IFACE_DOWN are registered on a global basis 
                   at powerup. 
12/22/02    aku    Return DS_EWOULDBLOCK if network is not up in dss_accept().
11/25/02    ss     Added NULL pointer check in dss_readv() for iov.
11/25/02    usb    Setting dss_errno when route_get fails,  removed redundant
                   check for ps_iface == NULL in dss_pppclose. Registering 
                   for IFACE_UP and IFACE_DOWN in all states other than 
                   DISABLED, setting dss_errno for event cback reg failures
11/25/02    aku    Added check to see if event buf is already 
                   allocated before allocating in dss_pppopen(). 
                   Return error code ENETINPROGRESS if iface is in routable 
                   state in dss_pppopen() and dss_netstatus().
11/19/02    jd     Moved dss_is_mip_up_on_um to ds707_drs.c,
                   TASKLOCK/TASKFREE in pppopen
11/14/02    jd     Allow ppp_close for iface ROUTEABLE - for MIP
                   taskfree before returning in pppclose
                   Fixed bug in getnextevent when scb == NULL
                   Fixed bug in setsockopt when setting boolean value
                   dss_is_mip_up_on_um under PS_UNSUPPORTED (not used)
11/12/02    mvl    Added support for changing default window size.
11/11/02    om     Changed dss_connect() to not allow routing lookups.
11/01/02    ss     Removed the check to determine network status from 
                   dss_listen()
11/07/02    ss     Return more accurate errno for bad protocol in socket()
10/30/02    ss     Added check for the socket state in bind routine.
                   Added check in connect routine for the server address if 
                   the socket is already connected.
10/30/02    ss     Added a check in stream I/O routines to ensure that the
                   specified socket is a TCP socket
10/10/02    rc     Feature wrapped 707 includes under FEATURE_DATA_IS707.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
09/25/02    om     Added some pointer checks and early routing MSG.
09/24/02    om     Added using dssocki_gen_meta_info in sendto, added policy
                   info for UMTS PDP context for early routing.
09/12/02    aku    Reverting change to dssocki_common_write_check() function
                   signature as using pointer to struct degrades performance
09/11/02    aku    Changed dssocki_common_write_check() to take pointer to
                   struct instead of multiple individual arguments.
09/09/02    aku    Featurized force_dormancy functionality with 
                   FEATURE_DATA_IS707.
09/06/02    aku    Fixed bug in registering for iface events and added checks
                   for return values for ps_iface functions.
09/05/02    aku    Updated for ps_iface cback buf interface.
09/04/02    ss     Set proto based on type if proto passed to dss_socket is 0
08/30/02    om     Added early routing via route_get().
08/22/02    rwh    Reset the app_field upon entry to dss_write_dsm_chain().
08/13/02    mvl    updated ps_iface state macros for name change (MM feature)
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
08/01/02    rc     Removed !FEATURE_DATA_WCDMA_PS around call to 
                   dssdorm_reorigination().
07/31/02    ss     Added NULL pointer checks in read/write calls. Added check
                   for protocol parameter in dss_socket() call.
07/30/02    aku    Added support for dss_force_dormancy()
07/25/02    aku    Added support for closing app when one or more apps is 
                   still using iface.
07/24/02    aku    app_iface_state is now used only if FEATURE_DATA_MM is 
                   not defined. Also featurized dss_ppp_in_use() with
                   FEATURE_DATA_MM.                   
07/23/02    jd     Initialized meta_info_ptr to NULL in dss_sendto()
07/23/02    aku    Removed code used to determine if we are in a packet call
                   in dss_pppopen().
07/22/02    aku    Register for events after issuing command to ps_iface
                   and set acb_ptr->ps_iface_ptr to NULL only after iface 
                   comes down.
07/17/02    aku    Removed HACK psiface_enable_ind() in dss_pppopen()
07/15/02    aku    Removed dssdorm.h and include dsmip_io.h only if 
                   FEATURE_DS_MOBILE_IP is defined. 
07/14/02    aku    Added new versions of dss_pppopen(), dss_pppclose() and
                   dss_netstatus() for FEATURE_DATA_MM.
07/11/02    na     Added multimode support under FEATURE_DATA_MM.
05/02/02    mvl    Some file cleanup, and support for new sendto() flags.
04/17/02    rc     Wrapped code in !FEATURE_DATA and !FEATURE_DATA_WCDMA_PS 
                   as appropriate to build for all targets.
04/08/02    sjy    Fixed byte ordering for address comparison and when 
                   assigning port number in dss_bind().  Fixed loop to use
                   dssocki_indextoscb() instead of dssocki_sockfdtoscb() when
                   iterating through socket control blocks. Fixed problem 
                   that allowed multiple binds to the same port.  Added port
                   autoassign feature when binding to port 0.
03/20/02    ss     Changed the return value to DSS_ERROR instead from 
                   DSS_SUCCESS for write and sendto calls in case of EOF.
03/13/02    aku    Added support for new iface_state APP_IFACE_OPEN_PENDING
03/05/02    pjb    Finished Accept Listen Code Review Items 
02/22/02    pjb    Added accept and listen
01/30/02    atp    Added API for fast dormancy (disabling dormant timer).
01/25/02    ss     Backed out the change to allow netlib to be closed even if
                   the network has not been closed.
01/02/02    aku    dss_ppp_in_use() now returns uint instead of a boolean.
                   Added check for dssocki_non_sys_sockets_flow_controlled() 
                   in dss_connect().
12/18/01    rsl    Updated tracer field.
12/14/01    ss     Allow netlib to be closed even if the network exists for
                   all except last application
12/07/01    ss     Moved determination of dormancy and call to abort() from 
                   dss_close to PS
11/29/01    mvl    removed dss_get_iface_status() now an iface call. clean up 
11/26/01    js     Added definition for dssi_force_dorm(). Modify
                   dss_force_dormancy() to work with dsiface_ioctl().
11/12/01    aku    Changed dss_pppopen(), dss_pppclose() and dss_netstatus
                   behavior
11/09/01    ss     Modified dss_close() to queue up close command for
                   stream sockets to PS.
10/15/01    aku    In dss_netstatus return DS_ENETINPROGRESS if ppp_active is
                   set to TRUE and iface status is CLOSED.
10/11/01    mt     Pass in dss_errno to dsstcp_connect_setup()
10/05/01    aku    Added check to see if ppp_active flag is set before
                   posting ppp_open_cmd_ev to dssnet. This prevents multiple
                   PPP open commands being enqueued in the PS cmd queue.
10/04/01    aku    Added check in dss_pppopen to see if mobile is in
                   laptop call before originating sockets call
08/27/01    aku    Removing net_event_active flag from acb_type struct. No
                   longer used.
08/23/01    aku    Renamed dss_getifacestatus as dss_get_iface_status
08/17/01    aku    Protecting access to ppp_active using INTLOCK()
08/14/01    aku    Added wrapper function dss_getifacestatus and function
                   dss_force_dormancy
07/30/01    aku    moved dss_getifacestatus to dssnet.c
07/27/01    na     Added dss_is_mip_up_on_um().
07/24/01    sjy    Added dss_get_iface_status() interface function to copy
                   interface status from control block into user buffer
07/23/01    pjb    Moved PS command kinds from DSM to PS 
07/12/01    jd     NULL parameter can be passed to dss_close() if the caller
                   does not want the l_errno value returned.
06/26/01    na     removed references to dssocki_num_active_apps()
04/16/01    js     Removed the function call to dssnet_enter_socket_mode() 
                   in dss_open_netlib(). This is already done when calling
                   dssnet_open_network(). 
04/06/01    js     Modified dss_setsockopt() so that the value attempted to
                   be assigned to a socket option is checked against its
                   predefined range. 
04/06/01    js     Modified dss_write(), dss_sendto() and 
                   dss_write_dsm_chain() to use dssocki_common_write_check().
03/29/01    js     Added inclusion for dssinternal.h. 
03/27/01    mt     Changed buffer argument types in dss_write(), dss_read(),
                   dss_sendto(), dss_recvfrom().  Add casts.
03/27/01    mvl    changed all instances of errno to dss_errno, also some
                   general code cleanup.  Fixed bug in dss_connect() where if
                   out of memory connect would never succeed.  Connect will
                   now return DS_ENOMEM if there is no memory available.
                   Fixed dss_getnextevent() so that only sockets with
                   matching app ids.
03/27/01    js     Added checking for socket type in dss_write_dsm_chain() to
                   ensure the function is only used by TCP sockets.
03/27/01    js     Added support to ICMP sockets in dss_sendto() and
                   dss_recvfrom().
03/27/01    js     Modified calls to dssocki_send_to_ps() to include the
                   extra parameter holding the destination information.
02/14/01    js     Modified errno ptr validation in all funtions. 
02/14/01    js     Added socket options support. 
02/14/01    js     Changed dss_sendto() so that if a msg is too large to be
                   sent at once, an errno is returned. When the available 
                   memory is not sufficient, free itemptr and issue 
                   DSS_EWOULDBLOCK.
02/11/01    snn    Moved the clearing of notified_cb_flag = FALSE in 
                   getnextevent(), outside the if loop. This ensures that
                   we will not miss any notifications in future.
02/08/01    yll    Merged in dss_write_dsm_chain() and dss_read_dsm_chain().
11/15/00    snn    Renamed MAX_SOCKS as DSS_MAX_SOCKS;
                   Modified the value allocated to sockfd
                   changed scb_type to scb_s
11/14/00    mvl    Changed ds_socket() so that it determines the protocol
                   value based on the type of the socket.
09/04/00    snn    Made changes to support multiple applications.
08/03/00  mvl/js   Protocol specific bind is invoked within dss_bind().
11/09/99  rc/hcg   Added byte ordering conversion in dss_bind().
09/22/99    rc     Incorporated code review changes and comments.
08/11/99    ak/rc  Code for sockets dormancy, and the related flow control
                   and re-origination requirements.  Also now check netstate
                   before doing commands such as write or close.  If no
                   network and doing a close, we instead abort.  Fixed bug in
                   readv, where the return value from the read was not 
                   checked properly.
04/25/99    hcg    More comment cleanup.
04/16/99    hcg    Updated incorrect comment for dss_socket().
04/10/99    hcg    Added additional error checking for I/O routines.  Also,
                   for I/O routines, ensured that the buffer copying actually
                   takes place (i.e. there are free buffers), to guard against
                   passing NULL item_ptr to PS task.
04/02/99    hcg    Removed byte ordering routines which have now been converted
                   to macros and are found in dssocket.h.  Incorporated
                   code review changes/comments.
10/06/98  na/hcg   Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DS_SOCKETS
#include "msg.h"

#include "err.h"
#include "dssocket.h"
#include "dssocki.h"
#include "dsm.h"
#include "ps_svc.h"
#include "ps_udp.h"
#include "dstask.h"
#include "ps_acl.h"
#include "ps_route.h"
#include "assert.h"
#include "dssinternal.h"
#include "ps_meta_info.h"
#include "dss_iface_ioctl.h"
#include "ps_iface_ioctl.h"
#include "pstimer.h"

#ifdef FEATURE_SEC_IPSEC
#include "secips.h"
#include "secipsiface.h"
#endif /* FEATURE_SEC_IPSEC */

#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
#include "ds707_data_session_profile.h"
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */

#define DSS_NETPOLICY_COOKIE (0x12343210UL)

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSS_SOCKET()

DESCRIPTION
 Create a socket and related data structures,and return a socket descriptor.

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
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  invalid or unsupported protocol
  DS_EMFILE           too many descriptors open.  A socket is already open
                      or has not closed compeletely.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_socket
(
  sint15 app_id,
  byte   family,                          /* Address family - AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr; /* ptr to socket control block allocated in       */
                     /* this fcn used to set fields in SCB                 */
  sint15   result;   /* tmp return value from called fcns                  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  MSG_MED( "Creating socket; app_id=%d prot=%d type=%d",
                                app_id, protocol, type);

  /*-------------------------------------------------------------------------
    Only support Internet address family
   ------------------------------------------------------------------------*/
  if (family != AF_INET)
  {
    *dss_errno = DS_EAFNOSUPPORT;
    MSG_HIGH("Incorrect Address family in dss_socket() call",0 ,0, 0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Ensure that we support this socket type - TCP or UDP only
  -------------------------------------------------------------------------*/
  if ( (type != SOCK_STREAM) && (type != SOCK_DGRAM) )
  {
    *dss_errno = DS_ESOCKNOSUPPORT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Allocate a socket control block and fill in
  -------------------------------------------------------------------------*/
  if ( (scb_ptr = dssocki_allocscb()) == NULL)
  {
    *dss_errno = DS_EMFILE;
    MSG_HIGH("Socket not allocated in dss_socket() call",0 ,0, 0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Invalid application ID
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->acb_ptr = dssocki_appidtoacb(app_id)) == NULL )
  {
    dssocki_freescb(scb_ptr);
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  scb_ptr->type             = type;  
  scb_ptr->family           = family;
  /*-------------------------------------------------------------------------
    Set the protocol field. Return an error if the type and protocol do
    not match.
  -------------------------------------------------------------------------*/
  if (protocol == 0)
  {
    scb_ptr->protocol = (type == SOCK_STREAM) ? IPPROTO_TCP:IPPROTO_UDP;
  }
  else if (type == SOCK_STREAM && protocol == IPPROTO_TCP)
  {
    scb_ptr->protocol = IPPROTO_TCP;
  }
  else if (type == SOCK_DGRAM && protocol == IPPROTO_UDP)
  {
    scb_ptr->protocol = IPPROTO_UDP;
  }
  else
  {
    if ( protocol != IPPROTO_TCP && protocol != IPPROTO_UDP )
    {
    *dss_errno = DS_EPROTONOSUPPORT;
    }
    else
    {
      *dss_errno = DS_EPROTOTYPE;
    }
    dssocki_freescb(scb_ptr);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Set up protocol specific function pointer table
  -------------------------------------------------------------------------*/
  result = dssocki_fcnptr_setup(scb_ptr, dss_errno);
  if (result == DSS_ERROR)
  {
    dssocki_freescb(scb_ptr);
    return (DSS_ERROR);
  } 

  /*-------------------------------------------------------------------------
    Set socket_state to NULL. Note, this must be done before the protocol
    specific function which changes the state.
  -------------------------------------------------------------------------*/
  dssocki_set_sock_state(scb_ptr, DSSOCKI_NULL);

  /*-------------------------------------------------------------------------
    Call protocol specific socket function
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->socket != NULL)
  {
    result = scb_ptr->fcn_ptr_table->socket(scb_ptr, dss_errno);
    if (result == DSS_ERROR)
    {
      dssocki_freescb(scb_ptr);
      return (DSS_ERROR);
    } 
  }

  /*-------------------------------------------------------------------------
    Error was encountered - free the socket control block and return
  -------------------------------------------------------------------------*/
  if (*dss_errno != DSS_SUCCESS)
  {
    dssocki_freescb(scb_ptr);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Success! - return valid socket descriptor 
   ------------------------------------------------------------------------*/
  MSG_MED("Creating socket; app_id=%d sockfd=%d ",
                                app_id, scb_ptr->sockfd, 0);
  return (scb_ptr->sockfd);

} /* dss_socket() */

/*===========================================================================

FUNCTION DSS_CONNECT()

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  For udp it fills in the peers ipaddr in the
  socket control block and binds the socket to an interface.  
  The function must receive (as a parameter) a valid socket descriptor, 
  implying a previous successful call to dss_socket().

DEPENDENCIES
  Network subsystem must be established and available.

RETURN VALUE
  Return DSS_ERROR and places the error condition value in *dss_errno.

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
sint15 dss_connect
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct sockaddr *servaddr,                        /* destination address */
  uint16 addrlen,                                    /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)
{
   struct scb_s *scb_ptr;                   /* Ptr to socket control block */
   sint15 result;                                          /* return value */
   struct sockaddr_in *sa = NULL;             /* tmp ptr to server address */
   acl_ipfilter_info_type ip_info;      /* Storage for IP info for routing */
   boolean udp_disconnect = FALSE; /* whether it is a udp disconnect call  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }
  /*-------------------------------------------------------------------------
    If there is no connect setup function defined then connect on this socket
    is un supported.
  -------------------------------------------------------------------------*/
  if(!scb_ptr->fcn_ptr_table->connect_setup)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Check for errors in address parameter
  -------------------------------------------------------------------------*/
  if (servaddr == NULL)
  {
    udp_disconnect = TRUE;
    *dss_errno = DS_EADDRREQ;
  }
  else
  {
    /*---------------------------------------------------------------------
      Invalid protocol family
    ---------------------------------------------------------------------*/
    if(servaddr->sa_family != AF_INET)
    {
      *dss_errno = DS_EAFNOSUPPORT;
      return DSS_ERROR;
    }
    /*-----------------------------------------------------------------------
      Invalid address length
    -----------------------------------------------------------------------*/
    if (addrlen < sizeof(struct sockaddr_in))
    {
      *dss_errno = DS_EFAULT;
      return (DSS_ERROR);  
    }

    /*-----------------------------------------------------------------------
      Invalid server address
    -----------------------------------------------------------------------*/
    sa = (struct sockaddr_in *) servaddr;
    if (sa->sin_addr.s_addr == 0)
    {
      udp_disconnect = TRUE;
      *dss_errno = DS_EOPNOTSUPP;
    }

    /*-----------------------------------------------------------------------
      Invalid server port number
    -----------------------------------------------------------------------*/
    if (sa->sin_port == 0)
    {
      udp_disconnect = TRUE;
      *dss_errno = DS_EINVAL;
    }
  }

  if(udp_disconnect == TRUE)
  {
    if( scb_ptr->protocol == IPPROTO_UDP)
    {
      /*---------------------------------------------------------------------
        Call protocl specific function if it is a udp socket else return 
        DSS_ERROR.
      ---------------------------------------------------------------------*/
      scb_ptr->servname.address = 0;
      scb_ptr->servname.port    = 0;
      return scb_ptr->fcn_ptr_table->connect_setup(scb_ptr, dss_errno);
    }
    else
    {
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_CONNECT_ROUTINE, dss_errno);
  if ( (result == DSS_ERROR) && (*dss_errno != DS_EWOULDBLOCK) )
  {

    /*-----------------------------------------------------------------------
      We return EINVAL if the socket is already connected but to a different
      endpoint than passed in as servaddr. 
    -----------------------------------------------------------------------*/
    if ( *dss_errno == DS_EISCONN && 
	 ( scb_ptr->servname.address != dss_ntohl(sa->sin_addr.s_addr) ||
	   scb_ptr->servname.port != dss_ntohs(sa->sin_port) ) )
    {
      *dss_errno = DS_EINVAL;
    }
    
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Put servaddr in peername in socket control block.  Convert back to 
    host byte ordering, as the lower layer protocols change back to 
    network byte ordering.
  -------------------------------------------------------------------------*/
  scb_ptr->servname.address = dss_ntohl(sa->sin_addr.s_addr);
  scb_ptr->servname.port = dss_ntohs(sa->sin_port);
  
  /*-------------------------------------------------------------------------
    Do early routing and add the result to the SCB's routing cache.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->routing_cache == NULL) ||
       (scb_ptr->address_cache.addr.v4 != dss_ntohl(sa->sin_addr.s_addr)))
  {
       
    /* Set the address specific routing information */
    memset( &ip_info, 0, sizeof( ip_info ) );
    ip_info.ip_vsn = IP_V4;
    ip_info.ip_hdr.v4.protocol = scb_ptr->protocol;
    ip_info.ip_hdr.v4.dest = scb_ptr->servname.address;
    ip_info.ptcl_info.tcp.dst_port = scb_ptr->servname.port;
    
    /* Policy information is coming through socket options if       */
    /* the ACB's routing cache is empty. For now there are none ... */
    result = route_get( &ip_info, 
                        NULL, 
                        FALSE, 
                        NULL, 
                        &(scb_ptr->routing_cache) );
    if ( result == -1 )
    {
      MSG_HIGH( "Connect: Rt lookup failed !", 0, 0, 0 );
      scb_ptr->address_cache.addr.v4 = 0x00000000;
      *dss_errno = DS_ENOROUTE;
      return result;
    }
    else
    {
#ifdef FEATURE_SEC_IPSEC
      if ( scb_ptr->routing_cache == &secips_iface )
      {
        MSG_LOW( "Connect: got IPSec pkt", 0, 0, 0 );
        ip_info.is_secure = TRUE;

        /* Do a secondary routing lookup to get the physical iface */
        result = route_get( &ip_info, 
                            NULL, 
                            FALSE, 
                            NULL, 
                            &(scb_ptr->routing_cache) );
        if ( result == -1 )
        {
          MSG_HIGH( "Connect: Rt lookup failed !", 0, 0, 0 );
          *dss_errno = DS_ENOROUTE;
          scb_ptr->routing_cache = NULL;
          scb_ptr->address_cache.addr.v4 = 0x00000000;
          return result;
        }        
        else
        {
          MSG_LOW( "Connect: Rt lookup -> 0x%x",
                   scb_ptr->routing_cache, 0, 0 );
          scb_ptr->address_cache.addr.v4 = scb_ptr->servname.address;
          scb_ptr->ipsec_hdr_size = SECIPS_ESP_MAX_HDR_SIZE;

          /* Adjust TCP's MSS for the IPSec ESP overhead */
          if ( scb_ptr->protocol_ctl_blk.tcb )
          {
            scb_ptr->protocol_ctl_blk.tcb->mss -= scb_ptr->ipsec_hdr_size;
          }
        }
      }
      else
#endif /* FEATURE_SEC_IPSEC */
      {
        MSG_HIGH( "Connect: Rt lookup -> 0x%x", 
                  scb_ptr->routing_cache, 0, 0 );
      }
    }
  }
    
  /*-------------------------------------------------------------------------
    Check to see if interface is ready to connect. If not, return appropriate 
    error to application.
  -------------------------------------------------------------------------*/
  result = dssocki_iface_check( scb_ptr, TRUE, TRUE, dss_errno);
  if( result == DSS_ERROR)
  {
    return (DSS_ERROR);
  }
  /*-------------------------------------------------------------------------
    Setup protocol specific connection requirements
  -------------------------------------------------------------------------*/
  result = scb_ptr->fcn_ptr_table->connect_setup(scb_ptr, dss_errno);

  return (result);

}  /* dss_connect() */

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
  reduced to the maximum (see DSS_SOMAXCONN). This is the BSD behavior.

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
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress. 
  DS_NOMEM            not enough memory to establish backlog connections. 
  DS_EINVAL           Socket already open, closed, unbound or not one 
                      you can listen on. 

SIDE EFFECTS 
  For TCP, initiates passive open for new connections.
===========================================================================*/
sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
)
{
   struct scb_s *scb_ptr;                   /* Ptr to socket control block */
   sint15 result;                                          /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Only support TCP - no listening UDP sockets
  -------------------------------------------------------------------------*/
  if (scb_ptr->protocol != IPPROTO_TCP)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check to see that socket has already been bound to a local address.  If
    not, return error.
  -------------------------------------------------------------------------*/
  if (scb_ptr->localname.port == 0)
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check for errors in backlog parameter - It must be greater than 0
  -------------------------------------------------------------------------*/
  if (backlog <= 0) 
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_LISTEN_ROUTINE, dss_errno);

  if (result == DSS_ERROR)
  {
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Silently truncate queue to SOMAXCONN
  -------------------------------------------------------------------------*/
  if (backlog > DSS_SOMAXCONN)
  {
    *dss_errno = DS_EFAULT;
    backlog = DSS_SOMAXCONN;
  }

  /*-------------------------------------------------------------------------
    Setup the protocol specific listen
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->listen_setup != NULL)
  {
    *dss_errno = DS_ENOMEM;
    return scb_ptr->fcn_ptr_table->listen_setup(scb_ptr,
                        backlog,
                        dss_errno);
  }
  else
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (result);
  }
}  /* dss_listen() */

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
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress. 
  DS_NOMEM            not enough memory to establish backlog connections. 

SIDE EFFECTS 

  The head backlog item from the queue of the listening socket is
  removed from that queue.
===========================================================================*/
sint15 dss_accept 
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct sockaddr *remoteaddr,                       /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)

{
   struct scb_s *scb_ptr;                   /* Ptr to socket control block */
   sint15 result;                                          /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if ((scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Only support TCP - can't accept on non-TCP sockets.
  -------------------------------------------------------------------------*/
  if (scb_ptr->protocol != IPPROTO_TCP)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Verify remote addr and addr len are legit. 
  -------------------------------------------------------------------------*/
  if ((remoteaddr == NULL) || 
      (addrlen == NULL) || 
      ((*addrlen) < sizeof(struct sockaddr_in)))
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_ACCEPT_ROUTINE, dss_errno);

  if ( (result == DSS_ERROR) && (*dss_errno != DS_EWOULDBLOCK) )
  {
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Do the protocol specific accept 
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->accept != NULL)
  {
    *dss_errno = DS_ENOMEM;
    return scb_ptr->fcn_ptr_table->accept(scb_ptr,  
                      remoteaddr, 
                      addrlen, 
                      dss_errno);
  }
  else
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (result);
  }

}  /* dss_accept() */


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
sint15 dss_open_netlib
(
  void   (*net_callback_fcn)(void *),         /* network callback function */
  void   (*socket_callback_fcn)(void *),       /* socket callback function */
  sint15 *dss_errno                               /* error condition value */
)
{
  acb_type *acb_ptr;                   /* ptr to application control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;
                              
  /*-------------------------------------------------------------------------
    Assign application ID
  -------------------------------------------------------------------------*/
  if ( (acb_ptr = dssocki_allocacb()) == NULL)
  {
    *dss_errno = DS_EMAPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Set callback functions
  -------------------------------------------------------------------------*/
  acb_ptr->net_callback_fcn    = net_callback_fcn;
  acb_ptr->socket_callback_fcn = socket_callback_fcn;

  MSG_MED("Allocating app_id %d",acb_ptr->app_id,0,0);

  return( acb_ptr->app_id);
} /* dss_open_netlib() */

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
sint15 dss_close_netlib
(
  sint15 app_id,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
  acb_type *acb_ptr;                   /* ptr to application control block */
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  int      i;                                       /* local index counter */
  dssocki_event_buf_type *event_buf_ptr=NULL;       /* event buf pointer   */
  q_type                 *working_q_ptr=NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  MSG_HIGH("App %d closing Network Library",app_id,0,0);

  /*-------------------------------------------------------------------------
    Invalid application ID
  -------------------------------------------------------------------------*/
  if ( (acb_ptr=dssocki_appidtoacb(app_id)) == NULL)
  {
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Ensure that the network subsystem has been closed. 
  -------------------------------------------------------------------------*/
  dss_pppclose(app_id, dss_errno);

  /*-------------------------------------------------------------------------
    Ensure that all sockets are closed for the application being closed.
  -------------------------------------------------------------------------*/
  for (i=0; i<DSS_MAX_SOCKS; i++)
  {
    if ( (scb_ptr=dssocki_indextoscb(i)) != NULL)
    {
      if ( scb_ptr->acb_ptr->app_id == acb_ptr->app_id )
      {
        ERR("Can't Exit - Socket %d still exists", scb_ptr->sockfd, 0, 0);
        *dss_errno = DS_SOCKEXIST;
        return (DSS_ERROR);
      }
    }
  }

  /*-------------------------------------------------------------------------
    Dequeue all the event callbacks and free the associated dsm items.
  -------------------------------------------------------------------------*/
  for(i=0; i< DSS_IFACE_IOCTL_EVENT_MAX; i++)
  {
    working_q_ptr = (acb_ptr->event_q_array) + i;
    while((event_buf_ptr = q_check(working_q_ptr)) != NULL)
    {
      /*---------------------------------------------------------------------
        Remove this callback from the event queue.
      ---------------------------------------------------------------------*/
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete(working_q_ptr, &(event_buf_ptr->link));
#else
      q_delete(&(event_buf_ptr->link));
#endif

      /*---------------------------------------------------------------------
        Free the associated DSM item.
      ---------------------------------------------------------------------*/
      dsm_free_buffer(event_buf_ptr->dsm_ptr);
      MSG_MED("Ev%d callback deregistered for app %d",i,app_id,0);
    }
  }

  /*-------------------------------------------------------------------------
    Free application control block
  -------------------------------------------------------------------------*/
  dssocki_freeacb(acb_ptr);

  return (DSS_SUCCESS);

} /* dss_close_netlib() */

/*===========================================================================

FUNCTION DSS_PPPOPEN()

DESCRIPTION
  Starts up the network subsystem (CDMA data service and PPP) over the Um
  interface for all sockets.

DEPENDENCIES
  Note that a valid application ID must be specified as a parameter,
  obtained by a successful return of dss_open_netlib().

RETURN VALUE
  Return DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_EWOULDBLOCK           the operation would block
  DS_EOPNOTSUPP            Phone in laptop call
  DS_ENETNONET             App specified interface is disabled or in 
                           routable state.

SIDE EFFECTS
  Initiates call origination and PPP negotiation.

===========================================================================*/
#define BRANCH_TASKFREE() {TASKFREE()
sint15 dss_pppopen
(
  sint15 app_id,                                         /* application id */
  sint15 *dss_errno                               /* error condition value */
)
{
  acb_type *acb_ptr;                          /* application control block */
  acl_ipfilter_info_type ip_info;
  acl_policy_info_type   pol_info;
  ps_iface_state_enum_type   iface_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  MSG_MED(" dss_pppopen() called by App %d", app_id, 0, 0);

  /*-------------------------------------------------------------------------
    Get acb_ptr and check for invalid application ID
  -------------------------------------------------------------------------*/
  if ( (acb_ptr=dssocki_appidtoacb(app_id)) == NULL)
  {
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  TASKLOCK();
  if(acb_ptr->ps_iface_ptr == NULL)
  {
    /*-----------------------------------------------------------------------
      Do early routing to determine the interface to use. 
      If the app wants a specific interface, use that interface. 
      Else use any default.      
    -----------------------------------------------------------------------*/
    
    /* Set the policy based routing info */
    memset(&pol_info, 0, sizeof(pol_info));
    pol_info.pdp_ctx_cfg_ptr = NULL;
    if(acb_ptr->app_specified_iface_ptr != NULL)
    {
      MSG_HIGH("Using app spec iface name=0x%x,inst=%d",
               acb_ptr->app_specified_iface_ptr->name,
               acb_ptr->app_specified_iface_ptr->instance,
               0);
      pol_info.if_id = acb_ptr->app_specified_iface_ptr->name;
    }
    else
    {
      pol_info.if_id = ANY_DEFAULT_GROUP;
    }
    pol_info.orig_from_sock = TRUE;

#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
    /* Get the data session profile id from ACB net policy */
    pol_info.data_session_profile_id =
      acb_ptr->acl_netpolicy_info.data_session_profile_id;
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */

    /* Set the IP based routing info */
    memset(&ip_info, 0, sizeof(ip_info));
    ip_info.ip_vsn = IP_V4;
    ip_info.ip_hdr.v4.dest = ROUTE_DEFAULT_DST;
    ip_info.is_secure = TRUE;

    if( route_get(&ip_info,
                  &pol_info,
                  FALSE,
                  NULL,
                  &(acb_ptr->ps_iface_ptr)) == -1 )
    {
      *dss_errno = DS_ENETNONET;
      BRANCH_TASKFREE();
      return (DSS_ERROR);
    }

    /*-----------------------------------------------------------------------
      If ps_iface_ptr is still not set, return DS_ENETNONET.
    -----------------------------------------------------------------------*/
    if(acb_ptr->ps_iface_ptr == NULL)
    {
      *dss_errno = DS_ENETNONET;
      BRANCH_TASKFREE();
      return (DSS_ERROR);
    }
  }

  MSG_HIGH( "ACB iface for ID %d set to 0x%x",
            acb_ptr->app_id, acb_ptr->ps_iface_ptr, 0 );

  iface_state = ps_iface_state(acb_ptr->ps_iface_ptr);
  switch(iface_state)
  {
    case IFACE_DOWN:
      /*---------------------------------------------------------------------
        The iface is down. Send command to PS to bring up iface. 
        CHECK FOR RETURN CODE FROM BRING_UP_CMD
      ---------------------------------------------------------------------*/
      if((ps_iface_bring_up_cmd(acb_ptr->ps_iface_ptr, dss_errno, NULL) < 0)
         && 
         (*dss_errno != DS_EWOULDBLOCK))
      {
        acb_ptr->ps_iface_ptr = NULL;
      }
      break;

    case IFACE_COMING_UP:
      *dss_errno = DS_EWOULDBLOCK;
      break;

    case IFACE_UP:
      /*---------------------------------------------------------------------
        dss_errno is DSS_SUCCESS
      ---------------------------------------------------------------------*/
      break;

    case IFACE_ROUTEABLE:
      *dss_errno = DS_EWOULDBLOCK;
      break;

    case IFACE_GOING_DOWN:
#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
      MSG_HIGH("Setting Control Flag DSSOCKI_NET_UP_EVENT_WAIT for APP ID %d",
                acb_ptr->app_id, 0, 0);
      DSSOCKI_SET_APP_CTRL_FLAG(acb_ptr, DSSOCKI_NET_UP_EVENT_WAIT);
      *dss_errno = DS_EWOULDBLOCK;
#else
      MSG_HIGH("Attempting to open iface in going_down state",0,0,0);
      acb_ptr->ps_iface_ptr = NULL;
      *dss_errno = DS_ENETCLOSEINPROGRESS;
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */
      break;

    case IFACE_DISABLED:
      MSG_HIGH("Attempting to open disabled iface",0,0,0);
      acb_ptr->ps_iface_ptr = NULL;
      *dss_errno = DS_ENETNONET;
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid iface state value.
      ---------------------------------------------------------------------*/
      ERR("invalid iface(0x%x) state %d",
          acb_ptr->ps_iface_ptr,
          ps_iface_state(acb_ptr->ps_iface_ptr),
          0);
      *dss_errno = DS_EOPNOTSUPP;
      ASSERT(0);
      BRANCH_TASKFREE();
      return (DSS_ERROR);
  } /* end switch()  */

  TASKFREE();

  if (iface_state == IFACE_UP)
  {
    dssocki_net_event_notify(acb_ptr);
    return (DSS_SUCCESS);
  }
  else
  {
    return (DSS_ERROR);
  }

} /* dss_pppopen() */

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
  DS_EAFNOSUPPORT     address family not supported
  DS_EOPNOTSUPP       operation not supported
  DS_EADDRINUSE       the local address is already in use.
  DS_EINVAL           the socket is already attached to a local name
  DS_EFAULT           invalid address and/or invalid address length

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_bind
(
  sint15 sockfd,                                      /* socket descriptor */
  struct sockaddr *localaddr,                             /* local address */
  uint16 addrlen,                                     /* length of address */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;      /*               ptr to socket control block */
  struct sockaddr_in *la;     /*                  tmp ptr to local address */
  sint15 result;       
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  MSG_MED(" DSS_BIND called", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_BIND_ROUTINE, dss_errno);

  if (result == DSS_ERROR)
  {
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check to see if socket has already been bound to a local address.  If
    so, return error.
  -------------------------------------------------------------------------*/
  if (scb_ptr->localname.address != INADDR_ANY)
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  if (scb_ptr->localname.port != 0)
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Invalid local address specified
  -------------------------------------------------------------------------*/
  if (localaddr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Local ptr of local address structure.
  -------------------------------------------------------------------------*/
  la = (struct sockaddr_in *) ( (unsigned char *) localaddr);

  /*-------------------------------------------------------------------------
    Incorrect length or family for socket address structure parameter
  -------------------------------------------------------------------------*/
  if (la->sin_family != AF_INET)
  {
    *dss_errno = DS_EAFNOSUPPORT;
    return (DSS_ERROR);
  }

  if (addrlen < sizeof(struct sockaddr_in))
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);  
  }

  /*-------------------------------------------------------------------------
    Don't support local IP address binding.
  -------------------------------------------------------------------------*/
  if (la->sin_addr.s_addr != INADDR_ANY)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check to see that no other sockets are using this IP address/port 
    combination for the particular protocol.  We do not support local 
    address re-use.
  -------------------------------------------------------------------------*/
  if ( dssocki_is_addr_in_use(scb_ptr->protocol, la) )
  {
    *dss_errno = DS_EADDRINUSE;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Fill in the address structure. Convert back to host byte ordering, as 
    the lower layer protocols change back to network byte ordering.
  -------------------------------------------------------------------------*/
  scb_ptr->localname.address = dss_ntohl(la->sin_addr.s_addr);  

  /*-------------------------------------------------------------------------
    Autoassign port if not set. 
  -------------------------------------------------------------------------*/
  if (dss_ntohs(la->sin_port) == 0) 
  {
    la->sin_port = dssocki_getephem_port(scb_ptr->protocol);
    scb_ptr->localname.port = dss_ntohs(la->sin_port);
  }
  else
  {
    scb_ptr->localname.port = dss_ntohs(la->sin_port);
  }

  /*-----------------------------------------------------------------------
    Call protocol specific function to complete the bind.
  -----------------------------------------------------------------------*/
  scb_ptr->fcn_ptr_table->bind(scb_ptr, la);

  /*-------------------------------------------------------------------------
    Return result code.
  -------------------------------------------------------------------------*/
  return (DSS_SUCCESS);

} /* dss_bind() */

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
sint15 dss_close
(
  sint15 sockfd,                                      /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  sint15 result = DSS_SUCCESS;              /* error checking return value */
  dsm_item_type *item_ptr;
  dssocki_state_enum_type socket_state;
  ps_timer_error_type timer_ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED(" DSS_CLOSE called by Sock%d ", sockfd, 0, 0);

  /*-------------------------------------------------------------------------
    If caller does not care about the errno (specifies NULL address), then
    store errno result in local variable.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }  
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Obtain the socket state
  -------------------------------------------------------------------------*/
  socket_state = scb_ptr->socket_state;

  /*-------------------------------------------------------------------------
    If the socket state is WAIT_FOR_CLOSE or CLOSED, it means that the
    application is calling dss_close() the second time. In this case, all
    we need to do is to free up the scb since the protocol control block
    would have been freed as part of the protocol specific close operation. 
  -------------------------------------------------------------------------*/
  
  if ( (socket_state == DSSOCKI_WAIT_FOR_CLOSE) ||
       (socket_state == DSSOCKI_CLOSED) )
  {
    if ( scb_ptr != NULL)
    {
      dssocki_freescb( scb_ptr );
    }
    return (DSS_SUCCESS);
  }
 
  /*-------------------------------------------------------------------------
    Modify the state flags to indicate that this socket can neither send
    nor receive anything now. Needs to be done in the application context.
  -------------------------------------------------------------------------*/
  DSSOCKI_SOCKET_CANTSENDMORE( scb_ptr );
  DSSOCKI_SOCKET_CANTRECVMORE( scb_ptr );

  /*-------------------------------------------------------------------------
    If it is a stream socket, send the close command to PS. This ensures 
    that the close happens in the context of the PS task.
  -------------------------------------------------------------------------*/
  if ( scb_ptr->type == SOCK_STREAM )
  {
    /*-----------------------------------------------------------------------
      If the socket is lingering then return SUCCESS right away. The socket 
      will be closed in the background.
    -----------------------------------------------------------------------*/
    if ( scb_ptr->state_flags & DSSOCKI_LINGER )
    {
      scb_ptr->acb_ptr = scb_ptr->linger_acb_ptr;
      return DSS_SUCCESS;
    }

    item_ptr = dsm_new_buffer( DSM_DS_POOL_SIZE(sizeof(scb_ptr)) );
    if ( item_ptr == NULL )
    {
      ERR("Running out of buffers", 0, 0, 0);
      *dss_errno = DS_ENOMEM;
      return (DSS_ERROR);
    }
#ifdef FEATURE_DSM_MEM_CHK
    item_ptr->tracer = DSM_SCKT_TO_PS;
#endif /*FEATURE_DSM_MEM_CHK */

    /*-----------------------------------------------------------------------
      If the SO_LINGER socket option is set and the timeout is non-zero then 
      start the linger timer. The case of zero timeout is handled by the 
      TCP code.
    -----------------------------------------------------------------------*/
    if ( IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_LINGER) &&
         scb_ptr->so_linger != 0 )
    {
      timer_ret_val = ps_timer_start( scb_ptr->linger_timer, 
                                      scb_ptr->so_linger
                                    );
      ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
    }

    dssocki_send_to_ps( scb_ptr, item_ptr, NULL, DSM_PS_SOCKET_CLOSE );

    *dss_errno = DS_EWOULDBLOCK;
    result = DSS_ERROR;
  }
  /*-----------------------------------------------------------------------
    If it is a datagram socket, call the protocol specific close operation.
  -----------------------------------------------------------------------*/
  else if ( ( scb_ptr->type == SOCK_DGRAM ) && 
            ( scb_ptr->fcn_ptr_table != NULL ) &&
            ( scb_ptr->fcn_ptr_table->close != NULL ) )
  {

    item_ptr = dsm_new_buffer( DSM_DS_POOL_SIZE(sizeof(scb_ptr)) );
    if ( item_ptr == NULL )
    {
      ERR("Running out of buffers", 0, 0, 0);
      *dss_errno = DS_ENOMEM;
      return (DSS_ERROR);
    }
#ifdef FEATURE_DSM_MEM_CHK
    item_ptr->tracer = DSM_SCKT_TO_PS;
#endif /*FEATURE_DSM_MEM_CHK */

    dssocki_send_to_ps( scb_ptr, item_ptr, NULL, DSM_PS_SOCKET_CLOSE );
    result = DSS_SUCCESS;
  }
  /*-----------------------------------------------------------------------
    Unknown/unsupported socket type.
  -----------------------------------------------------------------------*/
  else
  {
    ERR("Unknown socket type: %d", scb_ptr->type, 0, 0);
    *dss_errno = DS_EBADF;
    result = DSS_ERROR;
    ASSERT(0);
  }

  /*-------------------------------------------------------------------------
    Return result code.
  -------------------------------------------------------------------------*/
  return (result);

}  /* dss_close() */

/*===========================================================================

FUNCTION DSS_PPPCLOSE()

DESCRIPTION
  Initiates termination to bring down PPP and the traffic channel.  Upon
  successful close of the network subsystem, invokes the network callback
  function.
  It is suggested that, before calling this function, dss_netstatus should
  be called to check the status of the connection.

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
#define PS_BRANCH_TASKFREE() {TASKFREE()
sint15 dss_pppclose
(
  sint15 app_id,                                         /* application id */
  sint15 *dss_errno                               /* error condition value */
)
{
  acb_type *acb_ptr;                          /* application control block */
  boolean  result = FALSE;                    /* Return value              */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("dss_ppplcose() called by app %d",app_id,0,0);

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }

  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid application ID
  -------------------------------------------------------------------------*/
  if ( (acb_ptr=dssocki_appidtoacb(app_id)) == NULL)
  {
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  if(acb_ptr->ps_iface_ptr == NULL)
  {
    MSG_MED("iface_ptr NULL, return SUCCESS",0,0,0);
    return(DSS_SUCCESS);
  }

#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
  /*-------------------------------------------------------------------------
    Clear the DSSOCKI_NET_UP_EVENT_WAIT flag for this App.
  -------------------------------------------------------------------------*/
  DSSOCKI_RESET_APP_CTRL_FLAG(acb_ptr, DSSOCKI_NET_UP_EVENT_WAIT);
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */
  
  /*-------------------------------------------------------------------------
    If network is closed, bail right away.
  -------------------------------------------------------------------------*/
  switch ( ps_iface_state(acb_ptr->ps_iface_ptr))
  {

    case IFACE_DOWN:
    case IFACE_DISABLED:
     /*----------------------------------------------------------------------
        Notify app that ppp is already closed.
      ---------------------------------------------------------------------*/
      result = TRUE;
      break;

    case IFACE_ROUTEABLE:
    case IFACE_UP:
    case IFACE_COMING_UP:

      /*---------------------------------------------------------------------
        Check if some other application is using this interface.
      ---------------------------------------------------------------------*/
      TASKLOCK();
      if(dssocki_is_iface_in_use(acb_ptr) == FALSE)
      {
        /*-------------------------------------------------------------------
          Bring down phy iface if no other app is using this interface.
        -------------------------------------------------------------------*/
        ps_iface_tear_down_cmd(acb_ptr->ps_iface_ptr, dss_errno, NULL);
      }
      else
      {
        MSG_MED("Iface 0x%x in use",acb_ptr->ps_iface_ptr,0,0);
        acb_ptr->ps_iface_ptr = NULL;
        MSG_MED("Calling network cback for app %d", acb_ptr->app_id,0,0);
        dssocki_net_event_notify(acb_ptr);
        dssocki_socket_event_occurred(NULL,acb_ptr);
        PS_BRANCH_TASKFREE();
        return DSS_SUCCESS;
      }
      TASKFREE();
      *dss_errno = DS_EWOULDBLOCK;
      result = FALSE;
      break;

    case IFACE_GOING_DOWN:

      /*---------------------------------------------------------------------
        PPP_CLOSE has already been sent to dssnet. Notify app that close is
        in progress.
      ---------------------------------------------------------------------*/
      *dss_errno = DS_ENETCLOSEINPROGRESS;
      result = FALSE;
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid iface state.
      ---------------------------------------------------------------------*/
      ASSERT(0);
  } /* switch() */

  if(result == TRUE)
  {
    return (DSS_SUCCESS);
  }
  else
  {
    return (DSS_ERROR);
  }

} /* dss_pppclose() */


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
sint15 dss_netstatus
(
  sint15 app_id,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
  acb_type *acb_ptr;         /* ptr to application control block structure */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }

  // PJB This is wrong. DSS_SUCCESS is a ret val not an errno.
  *dss_errno = DSS_SUCCESS;
   
  MSG_HIGH(" dss_netstatus() called by App %d", app_id, 0, 0);

  /*-------------------------------------------------------------------------
    Invalid application ID
  -------------------------------------------------------------------------*/
  if ((acb_ptr = dssocki_appidtoacb(app_id)) == NULL)
  {
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  if(acb_ptr->ps_iface_ptr == NULL)
  {
    MSG_MED("iface_ptr NULL, return ENETNONET",0,0,0);
    *dss_errno = DS_ENETNONET;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Depending on the current state of the network subsystem, return the 
    appropriate error code.
  -------------------------------------------------------------------------*/
  switch ( ps_iface_state(acb_ptr->ps_iface_ptr))
  {
 
    case IFACE_DOWN:
#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
      /*---------------------------------------------------------------------
        Iface is not available
      ---------------------------------------------------------------------*/  
      if(DSSOCKI_IS_APP_CTRL_FLAG_SET(acb_ptr, DSSOCKI_NET_UP_EVENT_WAIT))
      {
        /* App is going to come up, return DS_ENETINPROGRESS */
        MSG_HIGH("App %d is being brought up, netstatus is DS_ENETINPROGRESS",
                  app_id, 0, 0);
        *dss_errno = DS_ENETINPROGRESS;
      }
      else
      {
        /* return DS_ENETNONET */
        MSG_HIGH("netstatus is DS_ENETNONET to App %d", app_id, 0, 0);
        *dss_errno = DS_ENETNONET;
      }
      break;
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */

    case IFACE_DISABLED:
      /*---------------------------------------------------------------------
        Iface is not available, return DS_ENETNONET.
      ---------------------------------------------------------------------*/  
      MSG_HIGH("netstatus is DS_ENETNONET to App %d", app_id, 0, 0);
      *dss_errno = DS_ENETNONET;
      break;
 
    case IFACE_UP:
      /*---------------------------------------------------------------------
        Iface is up, return DS_ENETISCONN.
      ---------------------------------------------------------------------*/  
      MSG_HIGH("netstatus is DS_ENETISCONN to App %d", app_id, 0, 0);
      *dss_errno = DS_ENETISCONN;
      break;
 
    case IFACE_COMING_UP:
    case IFACE_ROUTEABLE:
      /*---------------------------------------------------------------------
        Iface is opening, return DS_ENETINPROGRESS
      ---------------------------------------------------------------------*/  
      MSG_HIGH("netstatus is DS_ENETINPROGRESS to App %d", app_id, 0, 0);
      *dss_errno = DS_ENETINPROGRESS;
      break;
  
    case IFACE_GOING_DOWN:
#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
      /*---------------------------------------------------------------------
        Iface is closing
      ---------------------------------------------------------------------*/  
      if(DSSOCKI_IS_APP_CTRL_FLAG_SET(acb_ptr, DSSOCKI_NET_UP_EVENT_WAIT))
      {
        /* App is going to come up, return DS_ENETINPROGRESS */
        MSG_HIGH("App %d is being brought up, netstatus is DS_ENETINPROGRESS",
              app_id, 0, 0);
        *dss_errno = DS_ENETINPROGRESS;
      }
#else
      /* return DS_ENETCLOSEINPROGRESS */
      MSG_HIGH("netstatus is DS_ENETCLOSEINPROGRESS to App %d", app_id, 0, 0);
      *dss_errno = DS_ENETCLOSEINPROGRESS;
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */
      break;
        
    default:
      /*---------------------------------------------------------------------
        Sanity check.
      ---------------------------------------------------------------------*/  
     /*lint -e506, -e774 */
     ASSERT(0);
     /*lint +e506, +e774 */
       
  } /* switch() */

  return (DSS_ERROR);

} /* dss_netstatus() */

/*===========================================================================
FUNCTION DSS_ASYNC_SELECT()

DESCRIPTION
  Enables the events to be notified about through the asynchronous 
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function. This function also performs a real-time
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
sint31 dss_async_select
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 interest_mask,                        /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  MSG_MED(" In dss_async_select()", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  INTLOCK();
  /*-------------------------------------------------------------------------
    Reset the notified callback flag, so that the callback can be called
    again.
  -------------------------------------------------------------------------*/
  scb_ptr->acb_ptr->notified_cb_flag = FALSE;

  /*-------------------------------------------------------------------------
    Update requested event mask for the socket. Add it to existing requested
    mask.
  -------------------------------------------------------------------------*/
  scb_ptr->requested_event_mask |= interest_mask;

  INTFREE();

  /*-------------------------------------------------------------------------
    Do a realtime check to see if any of the events are true. If there are 
    any active events, call the callbacks.
  -------------------------------------------------------------------------*/
  if ( dssocki_socket_event_check( scb_ptr, scb_ptr->requested_event_mask))
  {
    dssocki_notify_app( scb_ptr->acb_ptr);
  }

  return (DSS_SUCCESS);

} /* dss_async_select() */

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
sint15 dss_async_deselect
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 clr_interest_mask,                  /* bitmask of events to clear */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Update requested event mask for the socket. Clear out any of the bits
    specified.
  -------------------------------------------------------------------------*/
  INTLOCK();
  /*lint -e502 expected unsigned type for bit operation */
  scb_ptr->requested_event_mask &= ~clr_interest_mask;
  /*lint +e502 expected unsigned type for bit operation */
  INTFREE();

  return (DSS_SUCCESS);
} /* dss_async_deselect() */

/*===========================================================================
FUNCTION DSS_GETNEXTEVENT()

DESCRIPTION
  This function performs a real-time check to determine if any of the events 
  of interest specified in the socket control block's event mask have 
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to 
  dss_async_select().  The application may pass in a pointer to a single 
  socket descriptor to determine if any events have occurred for that socket

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
  Returns an event mask of the events that were asserted for sockets whose
  app ID matches that passed in.  A value of 0 indicates that no events have
  occurred.

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
sint31 dss_getnextevent
( 
  sint15 app_id,                                         /* application ID */
  sint15 *sockfd_ptr,                                 /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
  sint31 event_mask=0;                              /* returned event mask */
  int index;                           /* index to go through socket array */
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
  acb_type *acb_ptr;         /* ptr to application control block structure */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a sockfd_ptr variable is specified
  -------------------------------------------------------------------------*/
  if( sockfd_ptr == NULL )
  {
    MSG_HIGH("sockfd_ptr is NULL", 0, 0, 0);
    *dss_errno = DS_EBADF;
    return(DSS_ERROR);
  }
 
  MSG_MED(" dss_getnextevent() called ", 0, 0, 0); 

  /*-------------------------------------------------------------------------
    Check for invalid application ID
  -------------------------------------------------------------------------*/
  if ( (acb_ptr = dssocki_appidtoacb(app_id)) == NULL)
  {
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  *dss_errno = DSS_SUCCESS;
  /*-------------------------------------------------------------------------
    Clear the notified flag. 
  -------------------------------------------------------------------------*/
  INTLOCK();
  acb_ptr->notified_cb_flag = FALSE;
  INTFREE();

  /*-------------------------------------------------------------------------
    If the passed value of sockfd_ptr is not NULL, look for events for only 
    that socket.
  -------------------------------------------------------------------------*/
  if ( *sockfd_ptr != NULL)  // JD -need to verify fix
  {
    /*-----------------------------------------------------------------------
      Convert socket descriptor to SCB
    -----------------------------------------------------------------------*/
    if ( (scb_ptr=dssocki_sockfdtoscb(*sockfd_ptr)) == NULL)
    {
      *dss_errno = DS_EBADF;
      return (DSS_ERROR);
    }

    /*-----------------------------------------------------------------------
      Make sure that the sockfd belongs to this app.
    -----------------------------------------------------------------------*/
    if(acb_ptr != scb_ptr->acb_ptr)
    {
      *dss_errno = DS_EBADF;
      return (DSS_ERROR);
    }

    /*-----------------------------------------------------------------------
      Get the event mask for the socket and if the requested events for that
      socket are enabled, return the event mask, and remove the events 
      from the requested_event_mask.
     ----------------------------------------------------------------------*/
    event_mask = dssocki_socket_event_check( scb_ptr, 
                                             scb_ptr->requested_event_mask);

    if ( event_mask & scb_ptr->requested_event_mask)
    {
      INTLOCK();

      /*lint -e502 expected unsigned type for bit operation */
      scb_ptr->requested_event_mask &= ~event_mask;
      /*lint +e502 expected unsigned type for bit operation */

      INTFREE();
      return(event_mask);
    }
  } /* if *sockfd_ptr != NULL */
  else
  {
    /*-----------------------------------------------------------------------
      The value specified was NULL, indicating we want to return the next
      available socket. Go through the sockets in the scb array and for 
      the given app, check if there any events that have occured.  If any 
      event has occurred for a socket, put the socket descriptor in the
      sockfd_ptr, and return the event mask.
    -----------------------------------------------------------------------*/
    for (index=0; index<DSS_MAX_SOCKS; index++)
    {
      INTLOCK();
      scb_ptr = &scb_array[index];
      INTFREE();  

      /*---------------------------------------------------------------------
        Make sure that the socket being checked matches the app id supplied.
        Get the event mask for the socket and if the requested events for 
        the socket are enabled, return the event mask, and remove the events 
        from the requested_event_mask.
      ---------------------------------------------------------------------*/
      if(acb_ptr == scb_ptr->acb_ptr)
      {
        event_mask = dssocki_socket_event_check(scb_ptr, 
                                                scb_ptr->requested_event_mask);

        if ( event_mask & scb_ptr->requested_event_mask)
        {
          INTLOCK();
          /*lint -e502 expected unsigned type for bit operation */
          scb_ptr->requested_event_mask &= ~event_mask;
          /*lint +e502 expected unsigned type for bit operation */
          INTFREE();
          *sockfd_ptr = scb_ptr->sockfd;
          return (event_mask);
        }
      }
    } /* for */

  } /* if *sockfd_ptr == NULL */

  /*-------------------------------------------------------------------------
    No events have occurred, so just return 0.
  -------------------------------------------------------------------------*/
  return(DSS_SUCCESS);

} /* dss_getnextevent() */

/*===========================================================================
FUNCTION DSS_WRITE()

DESCRIPTION
  Writes upto nbytes of data from buffer over the transport specified
  by the socket descriptor.

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
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           can't write to a listen socket 

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_write
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,               /* user buffer from which to copy data */
  uint16 nbytes,                /* number of bytes to be written to socket */
  sint15 *dss_errno                               /* error condition value */
)
{

  struct dssocki_msghdr msg;  /* Header containing data and dest addr      */
  struct iovec          iov;  /* encapsulated data array                   */
  int                   flags = 0 ; /* local flags field for dss_sendmsg   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  iov.iov_base = (byte *)buffer;
  iov.iov_len  = nbytes;
  
  msg.msg_name       = NULL;
  msg.msg_namelen    = 0;
  msg.msg_iov        = & iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_sendmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_write() */

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
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Cant write to a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_writev
(
  sint15 sockfd,                                      /* socket descriptor */
  struct iovec iov[],     /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
)
{

  struct dssocki_msghdr msg;  /* Msg header containing data and destaddr   */
  int                   flags = 0; /* flags for dss_sendmsg call           */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  msg.msg_name         = NULL;
  msg.msg_namelen      = 0;
  msg.msg_iov          = iov;
  msg.msg_iovlen       = iovcount;
  msg.msg_control      = NULL;
  msg.msg_controllen   = 0;
  msg.msg_flags        = 0;

  return dssocki_sendmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_writev() */

/*===========================================================================
FUNCTION DSS_READ()

DESCRIPTION
  Reads upto nbytes of data into buffer from the transport specified
  by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of 
      bytes specified. A return of 0 indicates that an End-of-File condition 
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
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read a listen socket. 

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_read
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,                     /* user buffer to which to copy data */
  uint16 nbytes,                 /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dssocki_msghdr msg; /* msg header containing data and dest addr   */
  int                   flags = 0; /* flags for dss_recvmsg                */
  struct iovec          iov;  /* encapsulated data array                   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  iov.iov_base = (byte *) buffer;
  iov.iov_len  = nbytes;

  msg.msg_name       = NULL;
  msg.msg_namelen    = 0;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_recvmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_read() */

/*===========================================================================
FUNCTION DSS_READV()

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which 
  allows the application to read into non-contiguous buffers. 

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of 
      bytes specified. A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_readv
(
  sint15 sockfd,                                      /* socket descriptor */
  struct iovec iov[],           /* array of data buffers to copy data into */
  sint15 iovcount,               /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{

  struct dssocki_msghdr msg; /* msg header containing data and dest addr   */
  int                   flags = 0; /* flags for dss_recvmsg                */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg.msg_name       = NULL;
  msg.msg_namelen    = 0;
  msg.msg_iov        = iov;
  msg.msg_iovlen     = iovcount;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_recvmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_readv() */

/*===========================================================================
FUNCTION DSS_SENDTO()

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the transport specified by the
  socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required 
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once  

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendto
(
  sint15 sockfd,                /* socket descriptor                       */
  const void *buffer,           /* user buffer from which to copy the data */
  uint16 nbytes,                /* number of bytes to be written           */
  uint32 flags,                 /* used for SDB (if enabled)               */
  struct sockaddr *toaddr,      /* destination address                     */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno             /* error condition value                   */
)
{
  struct dssocki_msghdr msg;   /* Header containing data and dest addr     */
  struct iovec          iov;   /* encapsulated data array                  */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  iov.iov_base = (byte *) buffer;
  iov.iov_len  = nbytes;
  
  msg.msg_name       = toaddr;
  msg.msg_namelen    = addrlen;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_sendmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_sendto() */

/*===========================================================================
FUNCTION DSS_RECVFROM()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the transport specified by the
  socket descriptor.  Fills in address structure with values from who sent 
  the data.

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
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvfrom
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,               /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct sockaddr *fromaddr,                        /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dssocki_msghdr msg; /* Header containing data and dest address    */
  struct iovec          iov; /* Data encapsulation array                   */
  int                   bytes_read; /* Bytes read                          */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  iov.iov_base = (byte *) buffer;
  iov.iov_len = nbytes;

  msg.msg_name       = fromaddr;
  msg.msg_namelen    = (addrlen != NULL) ? *addrlen : 0;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  bytes_read = dssocki_recvmsg(sockfd, &msg, NULL, flags, dss_errno);  

  if(addrlen != NULL)
  {
    *addrlen = msg.msg_namelen;
  }

  return bytes_read;
} /* dss_recvfrom() */

/*===========================================================================
FUNCTION DSS_SENDTO_DSM_CHAIN()

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

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
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.
===========================================================================*/
sint15 dss_sendto_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  uint32 flags,                 /* used for SDB (if enabled)               */
  struct sockaddr *toaddr,      /* destination address                     */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dssocki_msghdr msg;   /* Header containing data and dest addr     */
  struct iovec          iov;   /* encapsulated data array                  */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  iov.iov_base = NULL;
  iov.iov_len  = 0;
  
  msg.msg_name       = toaddr;
  msg.msg_namelen    = addrlen;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_sendmsg(sockfd, &msg, item_ptr, flags, dss_errno);

} /* dss_sendto_dsm_chain() */

/*===========================================================================
FUNCTION DSS_RECVFROM_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File 
      condition has occurred.

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
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvfrom_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  uint32 flags,                                                  /* unused */
  struct sockaddr *fromaddr,                        /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dssocki_msghdr msg; /* Header containing data and dest address    */
  struct iovec          iov; /* Data encapsulation array                   */
  int                   bytes_read; /* Bytes read                          */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  iov.iov_base = NULL;
  iov.iov_len = 0;

  msg.msg_name       = fromaddr;
  msg.msg_namelen    = (addrlen != NULL) ? *addrlen : 0;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  bytes_read = dssocki_recvmsg(sockfd, &msg, item_ptr, flags, dss_errno);  

  if(addrlen != NULL)
  {
    *addrlen = msg.msg_namelen;
  }

  return bytes_read;

} /* dss_recvfrom_dsm_chain() */

/*===========================================================================
FUNCTION DSS_SETSOCKOPT()

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
  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_setsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
  struct scb_s *scb_ptr;                 /* Ptr to socket control block    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if(dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Assert all arguments are valid - Prevents dereferencing of NULL ptrs. 
  -------------------------------------------------------------------------*/
  if((optval == NULL) || (optlen == NULL) )
  {
    MSG_HIGH("Invalid optlen=0x%x or optval=0x%x", optlen, optval, 0);
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Initialize dss_errno. 
  -------------------------------------------------------------------------*/
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if((scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check if the optname is valid to prevent illegal indexings into the
    socket option table. 
  -------------------------------------------------------------------------*/
  if((optname <= DSS_SOCKOPT_MIN) || (optname >= DSS_SOCKOPT_MAX))
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check level/option correspondence. Return DS_ENOPROTOOPT if the 
    requested option is unknown at the level specified, or if the level
    if unknown. 
  -------------------------------------------------------------------------*/  
  if(level != dss_sock_opts_default[optname].level)
  {
    *dss_errno = DS_ENOPROTOOPT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Validate the value
  -------------------------------------------------------------------------*/
  switch(dss_sock_opts_default[optname].optvaltype)
  {
    case DSSI_SO_BOOL:
      /*---------------------------------------------------------------------
        Size checking
      ---------------------------------------------------------------------*/
      if (*optlen < dss_sock_opts_default[optname].optlen)
      {
        *dss_errno = DS_EINVAL;
        return DSS_ERROR;
      }
      /*---------------------------------------------------------------------
        We need not do any range checking for boolean types. Any non-zero
        value is treated as TRUE and zero as FALSE 
      ---------------------------------------------------------------------*/
      break;

    case DSSI_SO_INT:
      /*---------------------------------------------------------------------
         Size checking
      ---------------------------------------------------------------------*/
      if (*optlen < dss_sock_opts_default[optname].optlen)
      {
        *dss_errno = DS_EINVAL;
        return DSS_ERROR;
      }
      /*---------------------------------------------------------------------
         Range checking. 
      ---------------------------------------------------------------------*/
      if(((*(int*)optval) < dss_sock_opts_default[optname].minval) || 
         ((*(int*)optval) > dss_sock_opts_default[optname].maxval ))
      {
        *dss_errno = DS_EINVAL;
        return (DSS_ERROR);
      }      
      break;

#ifdef FEATURE_DATA_IS2000_SDB
    case DSSI_SO_SDB_ACK_CB_TYPE:
#endif
    case DSSI_SO_LINGER:
      /*---------------------------------------------------------------------
         Size checking
      ---------------------------------------------------------------------*/
      if (*optlen != dss_sock_opts_default[optname].optlen)
      {
        *dss_errno = DS_EINVAL;
        return DSS_ERROR;
      }

      /*---------------------------------------------------------------------
        No range checking needed
      ---------------------------------------------------------------------*/
      break;

    default:
      /*---------------------------------------------------------------------
        Option value type not supported. 
      ---------------------------------------------------------------------*/
      *dss_errno = DS_ENOPROTOOPT;
      return (DSS_ERROR);
  } /* switch(optval type) */

  switch (optname)
  {
    case DSS_SO_SYS_SOCK:
    case DSS_SO_SILENT_CLOSE:
      if (*((int *)optval))
      {
        SOCKOPT_SET(scb_ptr->options_mask, optname);
      }
      else
      {
        SOCKOPT_RESET(scb_ptr->options_mask, optname);
      }
      break;

    case DSS_SO_RCVBUF:
      scb_ptr->rcvbuf = *((int *) optval);
      break;

    case DSS_SO_SNDBUF:
      scb_ptr->sndbuf = *((int *) optval);
      break;

    case DSS_IP_TTL:
      scb_ptr->ip_ttl = *((int *) optval);
      break;

    case DSS_SO_LINGER:
      if ( ((dss_so_linger_type *)optval)->l_onoff )
      {
        SOCKOPT_SET(scb_ptr->options_mask, optname);
      }
      else
      {
        SOCKOPT_RESET(scb_ptr->options_mask, optname);
      }
      scb_ptr->so_linger = ((dss_so_linger_type *)optval)->l_linger;
      break;

#ifdef FEATURE_DATA_IS2000_SDB
    case DSS_SO_SDB_ACK_CB:
      if ( ((dss_so_sdb_ack_cb_type *)optval)->sdb_ack_cb != NULL )
      {
        SOCKOPT_SET(scb_ptr->options_mask, optname);
        scb_ptr->sdb_ack_cb_fcn = 
          ((dss_so_sdb_ack_cb_type *)optval)->sdb_ack_cb;
        scb_ptr->sdb_ack_user_data = 
          ((dss_so_sdb_ack_cb_type *)optval)->user_data;
      }
      else
      {
        SOCKOPT_RESET(scb_ptr->options_mask, optname);
        scb_ptr->sdb_ack_cb_fcn    = NULL;
        scb_ptr->sdb_ack_user_data = NULL;
      }
      break;
#endif /* FEATURE_DATA_IS2000_SDB */

#ifdef FEATURE_JAVA
#error code not present
#endif /* FEATURE_JAVA */
 
    case DSS_TCP_NODELAY:
      if(scb_ptr->protocol != IPPROTO_TCP || 
	 scb_ptr->protocol_ctl_blk.tcb == NULL)
      {
        *dss_errno = DS_EOPNOTSUPP;
        return (DSS_ERROR);
      }
      if( *((int *) optval))
      {
        scb_ptr->protocol_ctl_blk.tcb->nodelay = TRUE;
      }
      else
      {
        scb_ptr->protocol_ctl_blk.tcb->nodelay = FALSE;
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Socket option not supported. 
      ---------------------------------------------------------------------*/
      *dss_errno = DS_ENOPROTOOPT;
      return (DSS_ERROR);
  }

  return (DSS_SUCCESS);

} /* dss_setsockopt() */



/*===========================================================================
FUNCTION DSS_GETSOCKOPT()

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
  the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value  
  DS_EFAULT               Invalid buffer or argument  

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if(dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Assert all arguments are valid - Prevents dereferencing of NULL ptrs. 
  -------------------------------------------------------------------------*/
  if((optval == NULL) || (optlen == NULL))
  {
    MSG_HIGH("Invalid optlen=0x%x or optval=0x%x", optlen, optval, 0);
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Initialize dss_errno. 
  -------------------------------------------------------------------------*/
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if((scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check if the optname is valid to prevent illegal indexings into the
    socket option table. 
  -------------------------------------------------------------------------*/
  if((optname <= DSS_SOCKOPT_MIN) || (optname >= DSS_SOCKOPT_MAX))
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }
  
  /*-------------------------------------------------------------------------
    Check level/option correspondence. Return DS_ENOPROTOOPT if the 
    requested option is unknown at the level specified, or if the level
    if unknown. 
  -------------------------------------------------------------------------*/  
  if(level != dss_sock_opts_default[optname].level)
  {
    *dss_errno = DS_ENOPROTOOPT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Returning the queried option.  optlen is a value-result 
    parameter, initially containing the size of the buffer pointed to 
    by optval, and modified on return to indicate the  actual  size  of
    the value returned. 
  -------------------------------------------------------------------------*/
  switch(dss_sock_opts_default[optname].optvaltype)
  {
    case DSSI_SO_BOOL:
    case DSSI_SO_INT:
      /*---------------------------------------------------------------------
         Integer or boolean type socket option value. 
      ---------------------------------------------------------------------*/
      if ( *optlen < dss_sock_opts_default[optname].optlen )
      {
        *dss_errno = DS_EINVAL;
        return (DSS_ERROR);
      }
      else
      {
        *optlen = dss_sock_opts_default[optname].optlen;
      }
      break;

#ifdef FEATURE_DATA_IS2000_SDB
    case DSSI_SO_SDB_ACK_CB_TYPE:
#endif
    case DSSI_SO_LINGER:
      /*---------------------------------------------------------------------
        The specified optlen should be same as the optlen stored in the 
        opts_default.
      ---------------------------------------------------------------------*/
      if ( *optlen != dss_sock_opts_default[optname].optlen )
      {
        *dss_errno = DS_EINVAL;
        return (DSS_ERROR);
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Socket option value type not supported. 
      ---------------------------------------------------------------------*/
      *dss_errno = DS_ENOPROTOOPT;
      return (DSS_ERROR);
  } /* switch(optval type) */


  switch (optname)
  {
    case DSS_SO_SYS_SOCK:
    case DSS_SO_SILENT_CLOSE:
      *((int *) optval) = IS_SOCKOPT_SET(scb_ptr->options_mask, optname);
      break;

    case DSS_IP_TTL:
      *((int*)optval) = scb_ptr->ip_ttl; 
      break;
   
    case DSS_SO_RCVBUF:
      *((int*)optval) = scb_ptr->rcvbuf; 
      break;

    case DSS_SO_SNDBUF:
      *((int *)optval) = scb_ptr->sndbuf;
      break;

    case DSS_SO_LINGER:
      ((dss_so_linger_type *)optval)->l_onoff = 
          IS_SOCKOPT_SET(scb_ptr->options_mask, optname);
      ((dss_so_linger_type *)optval)->l_linger = scb_ptr->so_linger;
      break;

#ifdef FEATURE_DATA_IS2000_SDB
    case DSS_SO_SDB_ACK_CB:
      ((dss_so_sdb_ack_cb_type *)optval)->sdb_ack_cb = 
        scb_ptr->sdb_ack_cb_fcn; 

      ((dss_so_sdb_ack_cb_type *)optval)->user_data =
        scb_ptr->sdb_ack_user_data;
      break;
#endif /* FEATURE_DATA_IS2000_SDB */

#ifdef FEATURE_JAVA
#error code not present
#endif /* FEATURE_JAVA */
 
    case DSS_TCP_NODELAY:
      if( scb_ptr->protocol != IPPROTO_TCP || 
	  scb_ptr->protocol_ctl_blk.tcb == NULL )
      {
        *dss_errno = DS_EOPNOTSUPP;
	return (DSS_ERROR);
      }
      *((int *) optval) = scb_ptr->protocol_ctl_blk.tcb->nodelay;
      break;

    default:
      /*---------------------------------------------------------------------
        Socket option not supported. 
      ---------------------------------------------------------------------*/
      *dss_errno = DS_ENOPROTOOPT;
      return (DSS_ERROR);
  }

  return (DSS_SUCCESS);

} /* dss_getsock_opt ()*/


/*===========================================================================
FUNCTION DSS_FORCE_DORMANCY()

DESCRIPTION
  NOTE - THIS FUNCTION IS NOW DEPRECATED. 
  Applications should use the new ioctl interface - dss_iface_ioctl() -
  to force dormancy. Refer to dss_iface_ioctl.h more details.
  
  This function is used to force dormancy (ie release traffic channel without
  bringing down the call) on an IS707 (1X) packet data interface only.

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
)
{
#ifdef FEATURE_DATA_IS707
  ps_iface_type *iface_ptr;
  sint15        ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Force dorm called by App %d", app_id, 0, 0);

  /*-------------------------------------------------------------------------
    Get the CDMA iface pointer instance 0 and send the ioctl to go dormant
  -------------------------------------------------------------------------*/
  iface_ptr = ps_iface_get_handle(DSS_IFACE_IOCTL_CDMA_SN, 0);
  if(iface_ptr == NULL)
  {
    ERR("Force dorm failed, NULL 1X iface", 0, 0, 0);
    return;
  }

  if(ps_iface_ioctl(iface_ptr,
                    DSS_IFACE_IOCTL_GO_DORMANT,
                    NULL,
                    &ps_errno) < 0)
  {
    ERR("Force dorm failed, err=%d", ps_errno, 0, 0);
  }
#endif /* FEATURE_DATA_IS707 */

} /* dss_force_dormancy() */


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
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dss_enable_dorm_timer
(
  sint15 app_id,
  boolean flag
)
{
#ifdef FEATURE_DATA_IS707
  ps_iface_type *iface_ptr;
  sint15        ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("enable_dorm_timer() called by %d", app_id, 0, 0);

  /*-------------------------------------------------------------------------
    Get the CDMA iface pointer instance 0 and send the ioctl to go dormant
  -------------------------------------------------------------------------*/
  iface_ptr = ps_iface_get_handle(DSS_IFACE_IOCTL_CDMA_SN, 0);
  if(iface_ptr == NULL)
  {
    ERR("enable_dorm_timer() failed, NULL 1X iface", 0, 0, 0);
    return;
  }

  if(ps_iface_ioctl(iface_ptr,
                    DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN,
                    &flag,
                    &ps_errno) < 0)
  {
    ERR("enable_dorm_timer() failed, err=%d", ps_errno, 0, 0);
  }
#endif /* FEATURE_DATA_IS707 */

} /* dss_enable_dorm_timer() */

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
sint15 dss_getsockname
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */                  
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
)
{
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  struct sockaddr_in retaddr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("dss_getsockname called, sockfd=%d", sockfd, 0, 0);
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    MSG_HIGH("Bad sockfd=%d", sockfd, 0, 0);
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Invalid local address specified
  -------------------------------------------------------------------------*/
  if ( addr == NULL )
  {
    MSG_HIGH("Bad memory address: addr is NULL", 0, 0, 0);
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Invalid address length specified
  -------------------------------------------------------------------------*/
  if ( addrlen == NULL )
  {
    MSG_HIGH("Bad memory address: addrlen is NULL", 0, 0, 0);
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Fill up the local address from the scb. Note that the scb stores the 
    port, address in host byte order and it needs to be converted to network
    byte order.
  -------------------------------------------------------------------------*/
  retaddr.sin_family = scb_ptr->family;
  retaddr.sin_port  = dss_htons(scb_ptr->localname.port);
  retaddr.sin_addr.s_addr = dss_htonl(scb_ptr->localname.address);
  ((sint31 *) retaddr.sin_zero)[0] = 0;
  ((sint31 *) retaddr.sin_zero)[1] = 0;

  /*-------------------------------------------------------------------------
    If the address is larger than the supplied buffer then it is silently
    truncated. The addrlen is set to indicate the size prior to truncation, 
    if any.
  -------------------------------------------------------------------------*/
  memcpy(addr, &retaddr, MIN(*addrlen, sizeof(retaddr)));
  *addrlen = sizeof(retaddr);

  return DSS_SUCCESS;
} /* dss_getsockname() */

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
sint15 dss_getpeername
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */                  
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
)
{
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  struct sockaddr_in retaddr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("dss_getpeername called, sockfd=%d", sockfd, 0, 0);
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    MSG_HIGH("Bad sockfd=%d", sockfd, 0, 0);
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Invalid local address specified
  -------------------------------------------------------------------------*/
  if ( addr == NULL )
  {
    MSG_HIGH("Bad memory address: addr is NULL", 0, 0, 0);
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Invalid address length specified
  -------------------------------------------------------------------------*/
  if ( addrlen == NULL )
  {
    MSG_HIGH("Bad memory address: addrlen is NULL", 0, 0, 0);
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Check if it is a connected socket, i.e., it is a stream socket AND it 
    is in OPEN state
  -------------------------------------------------------------------------*/
  if ( scb_ptr->socket_state != DSSOCKI_OPEN  ||
       scb_ptr->servname.address == 0         ||
       scb_ptr->servname.port    == 0)
  {
    MSG_HIGH("Socket is not connected, sockfd=%d", sockfd, 0, 0);
    *dss_errno = DS_ENOTCONN;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Fill up the peer address from the scb. Note that the scb stores the 
    port, address in host byte order and it needs to be converted to network
    byte order.
  -------------------------------------------------------------------------*/
  retaddr.sin_family = scb_ptr->family;
  retaddr.sin_port  = dss_htons(scb_ptr->servname.port);
  retaddr.sin_addr.s_addr = dss_htonl(scb_ptr->servname.address);
  ((sint31 *) retaddr.sin_zero)[0] = 0;
  ((sint31 *) retaddr.sin_zero)[1] = 0;

  /*-------------------------------------------------------------------------
    If the address is larger than the supplied buffer then it is silently
    truncated. The addrlen is set to indicate the size prior to truncation, 
    if any.
  -------------------------------------------------------------------------*/

  memcpy(addr, &retaddr, MIN(*addrlen, sizeof(retaddr)));
  *addrlen = sizeof(retaddr);

  return DSS_SUCCESS;
} /* dss_getpeername() */

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
sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
)
{
  struct scb_s  *scb_ptr;                   /* ptr to socket control block */
  dsm_item_type *item_ptr = NULL;        /* data services memory pool item */
  sint15        result = 0;                 /* error checking return value */   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("dss_shutdown called, sockfd=%d", sockfd, 0, 0);

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified 
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    MSG_HIGH("Bad file descriptor, sockfd=%d", sockfd, 0, 0);
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Validate 'how' parameter
  -------------------------------------------------------------------------*/
  if ( how != DSS_SHUT_RD && how != DSS_SHUT_WR && how != DSS_SHUT_RDWR )
  {
    MSG_HIGH("Invalid operation, how=%d", how, 0, 0);
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  result = dssocki_get_err_val(scb_ptr, DSSOCKI_SHUTDOWN_ROUTINE, dss_errno);

  if (result == DSS_ERROR && *dss_errno != DS_ESHUTDOWN)
  { 
    return (result);
  }

  if(*dss_errno == DS_ESHUTDOWN && 
     (how == DSS_SHUT_WR || how == DSS_SHUT_RDWR))
  {
    *dss_errno = DS_ENOTCONN;
    return (result);
  }

  if ((item_ptr = dsm_new_buffer( DSM_DS_POOL_SIZE(sizeof(how)))) == NULL)
  {
    ERR("Running out of buffers", 0, 0, 0);
    *dss_errno = DS_ENOMEM;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Mark the scb to disallow specified operation. Note that this needs to
    be done in the application context.
  -------------------------------------------------------------------------*/
  if ( how == DSS_SHUT_RD || how == DSS_SHUT_RDWR )
  { 
    DSSOCKI_SOCKET_CANTRECVMORE( scb_ptr );
  }

  if ( how == DSS_SHUT_WR || how == DSS_SHUT_RDWR )
  { 
    DSSOCKI_SOCKET_CANTSENDMORE( scb_ptr );
  }

  /*-------------------------------------------------------------------------
    Push the value of 'how' parameter onto the DSM item and queue a 
    shutdown command to the PS so that the protocol specific shutdown
    operation executes in the PS context.
  -------------------------------------------------------------------------*/
  if ( dsm_pushdown( &item_ptr, 
                     &how, 
                     sizeof(how),
                     DSM_DS_SMALL_ITEM_POOL ) < sizeof(how) )
  {
    dsm_free_packet(&item_ptr);
    ERR("Running out of buffers", 0, 0, 0);
    *dss_errno = DS_ENOMEM;
    return (DSS_ERROR);
  }
  dssocki_send_to_ps( scb_ptr, item_ptr, NULL, DSM_PS_SOCKET_SHUTDOWN );

  return (DSS_SUCCESS);
} /* dss_shutdown */

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
  - Except the field data_session_profile_id, other fields doesn't have 
    much signifcance in this release.
===========================================================================*/
void dss_init_net_policy_info
(
  dss_net_policy_info_type * policy_info_ptr       /* policy info structure */
)
{
  if (policy_info_ptr != NULL)
  {
    policy_info_ptr->iface.kind = DSS_IFACE_NAME;
    policy_info_ptr->iface.info.name = DSS_IFACE_ANY_DEFAULT;
    policy_info_ptr->policy_flag = DSS_IFACE_POLICY_ANY;
    policy_info_ptr->ipsec_disabled = FALSE;
    policy_info_ptr->family = AF_INET;
    policy_info_ptr->umts.pdp_profile_num = 0;
    policy_info_ptr->cdma.data_session_profile_id = 0; /* default */
    policy_info_ptr->dss_netpolicy_private.cookie = DSS_NETPOLICY_COOKIE;
  }
  else
  {
    MSG_ERROR("policy_info_ptr is NULL", 0, 0, 0);
  }
} /* dss_init_net_policy_info() */


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
  DS_EINVAL               Operation is not allowed in current state.

SIDE EFFECTS
  Sets the application netpolicy to the user specified value.
===========================================================================*/
sint15 dss_set_app_net_policy
(
  sint15 dss_nethandle,                           /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                              /* error number */
)
{
  acb_type *acb_ptr;         /* ptr to application control block structure */
  dss_net_policy_info_type  default_policy;      /* default network policy */
#ifdef FEATURE_DS_MULTIPLE_PROFILES
  int8 profile_id = DATA_SESSION_PROFILE_ID_INVALID;
#endif /* FEATURE_DS_MULTIPLE_PROFILES */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("dss_set_app_net_policy() called %d", dss_nethandle, 0, 0);

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return (DSS_ERROR);
  }

  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Check for invalid application ID
  -------------------------------------------------------------------------*/
  if ((acb_ptr = dssocki_appidtoacb(dss_nethandle)) == NULL)
  {
    MSG_ERROR("Invalid dss_nethandle: %d", dss_nethandle, 0, 0);
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    If the application is bound to an interface, don't allow application
    to change the network policy.
  -------------------------------------------------------------------------*/
  if (acb_ptr->ps_iface_ptr != NULL)
  {
    MSG_ERROR("Application %d is already bound to the interface 0x%x",
               dss_nethandle, acb_ptr->ps_iface_ptr, 0);
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    If policy_info is NULL then set the policy to default policy
  -------------------------------------------------------------------------*/
  if (policy_info_ptr == NULL)
  {
    MSG_MED("policy_info_ptr is NULL, using default policy", 0, 0, 0);
    policy_info_ptr = &(default_policy);
    dss_init_net_policy_info(policy_info_ptr);
  }

  /*-------------------------------------------------------------------------
    Verify that netpolicy structure is initialized.
  -------------------------------------------------------------------------*/
  if (policy_info_ptr->dss_netpolicy_private.cookie != DSS_NETPOLICY_COOKIE)
  {
    MSG_ERROR("Net policy structure incorrectly initialized", 0 ,0 ,0);
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  /* Generate ACL policy info in ACB from the incoming policy info */
  memset(&(acb_ptr->acl_netpolicy_info), 0, sizeof(acl_policy_info_type));

#ifdef FEATURE_DS_MULTIPLE_PROFILES
  if(policy_info_ptr->cdma.data_session_profile_id == 0)
  {
    profile_id = ds707_data_session_get_profile_id_for_special_nai();
    if(profile_id != DATA_SESSION_PROFILE_ID_INVALID)
    {
      acb_ptr->acl_netpolicy_info.data_session_profile_id = profile_id;
    }
    else
    {
      acb_ptr->acl_netpolicy_info.data_session_profile_id = 
                             policy_info_ptr->cdma.data_session_profile_id;
    }
  }
  else
  {
#endif /* FEATURE_DS_MULTIPLE_PROFILES */
    acb_ptr->acl_netpolicy_info.data_session_profile_id =
                            policy_info_ptr->cdma.data_session_profile_id;
#ifdef FEATURE_DS_MULTIPLE_PROFILES
  }
#endif /* FEATURE_DS_MULTIPLE_PROFILES */

  return (DSS_SUCCESS);
} /* dss_set_app_net_policy() */

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
)
{

#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
  /* Call DS API to get the profile id */
  return (int32)(ds_get_app_profile_index(app_type));
#else
  return 0; /* default profile */
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */

} /* dss_get_app_profile_id() */

#endif  /* FEATURE_DS_SOCKETS */
