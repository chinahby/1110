/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I N T E R N A L   F I L E
                   
DESCRIPTION

 The Data Services sockets internal file. This contains all the functions
 used by the network library internally.
 
EXTERNALIZED FUNCTIONS

  dssocki_allocscb()
    Allocates socket control block structure from SCB array.

  dssocki_init_scb()
    Initializes the given scb

  dssocki_freescb()
    Frees socket control block structure from SCB array and makes
    available for re-use.

  dssocki_sockfdtoscb()
    Given a socket descriptor, returns the corresponding socket control
    block.

  dssocki_allocacb()
    Allocates application control block structure from ACB array.

  dssocki_freeacb()
    Frees application control block structure from ACB array and makes
    available for re-use.

  dssocki_appidtoacb()
    Given an application ID, returns the corresponding application control
    block.

  dssocki_getacbcount()
    Gets the current number of allocated application control blocks in the 
    system.

  dssocki_getephem_port()
    Generates an ephemeral port number.
  
  dssocki_fcnptr_setup()
    Sets up the protocol specific function pointer table.
      
  dssocki_send_to_ps()
    Sends messages to the PS manager task using command queues.

  dssocki_buf_to_dsm()
    Copies the data buffer to the data services memory pool.

  dssocki_set_sock_state()
    Sets the current state of the socket.

  dssocki_net_event_notify()
    Calls application network callback function to notify the application 
    that a network event has occurred.

  dssocki_socket_event_check()
    Performs real-time check to determine if any of the socket events of 
    interest have occurred.  If so, notifies the application.

  dssocki_notify_app()
    Calls application callback function to notify application that a socket
    event has occurred.

  dssocki_socket_event_occurred()
    Notifies all interested sockets that a socket event has occurred.
  
  dssocki_sock_set_state_closed()
    Sets socket states to CLOSED when the network subsystem has gone down.

  dssocki_set_closed_state_reason()
    Sets the reason for going into the CLOSED state in the SCB.

  dssocki_get_err_val()
    Given the socket state and type of function call,returns the appropriate
    error condition value.
    
  dssocki_sendmsg()
    This function is the common output function for all the sockets ouptut
    functions.

  dssocki_recvmsg()
    This function is the common input function for all the socket input
    functions.

Copyright (c) 1998-2011 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssocki.c_v   1.25   28 Feb 2003 18:29:08   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssocki.c#6 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
06/24/08    pp     Fixed RVCT compiler warnings.
06/07/08    pp     OMH 1b Profile Arbitration development.
17/09/07    scb    Added the Socket configuration control block.
08/09/06    az     Fixed compiler warnings (Checkin for Anuj)
07/01/04    vp     Fixed dssocki_sendmsg to allow  maximum of 32k bytes in 
                   a single write call.
05/14/04    aku    Added support to call IOCTL event callback immediately if
                   event is true at the time of registration.
11/07/03    sv     Enabled read event when the socket is in wait_for_close
                   or listne state.
11/05/03    sv     Added check to make sure we don't enqueue more than
                   sndbuf size of data in tcp sndq.
10/22/03    sv     Modified dssocki_get_errval to return ESHUTDOWN for shutdown
                   routine when the socket is in CLOSING state
09/15/03    sv     Fixed dsm reference account increment bug in 
                   dssocki_reg_ev.
09/05/03    ss     TASKLOCK()'ed the code in process_iface_down_ev_cback
08/13/03    ss     Modified code to address tcp session getting closed before
                   second dss_close() is called for a lingering socket
08/04/03    sv     Added SO_KEEPALIVE and TCP_NODELAY socket options.
07/18/03    ss     Added sockfd in the command to PS to validate that scb_ptr
                   points to the correct socket.
07/08/03    ss     Made changes to support SDB ack callback socket option.
07/07/03    atp    Renamed SDB flags as part of SDB enhancements.
07/06/03    sv     Added SNDBUF socket option to the options default table. 
                   Modified dssocki_copy_buf_to_dsm to take an additional 
                   argument that specifies the maximum number of bytes that 
                   are to be copied.  
07/03/03    ss     Fixed the parenthesis problem in sendmsg()
06/16/03    aku    Fixed wraparound issue with dss_appidbase and 
                   dss_sockfdbase.
06/06/03    om     Fixed setting of ipsec_required flag in meta info.
06/04/03    om     Fixed MTU setting to adjust for IPSec ESP overhead,
                   added dssocki_set_ipsec_routing_caches().
05/28/03    ss     Changed dssocki_iface_check() so that the application gets 
                   EWOULDBLOCK instead of ENETDOWN when iface is routeable
05/27/03    om     Added dssudp_read_dsm_chain() to IO function table,
                   changed routing lookup in sendmsg() for IPSec iface,
                   updated sendmsg() and recv_msg() to take payload in DSM
                   item chains, added dssocki_flush_ipsec_routing_caches().
05/09/03    ss     Modified dssocki_socket_event_check() to check for flow
                   control, memory level, etc. in INIT state as well 
05/08/03    ss     Added range checking in dssocki_sockfdtoscb()
05/05/03    jd     Removed dependency on dsiface_def.h
05/04/03    ss     Modified allocscb(), freescb() and set_sock_state(), added 
                   linger_timeout_cb() and process_lingering_close() to 
                   support SO_LINGER socket option
05/01/03    usb    A larger than required addrlen for address buffer should 
                   not generate an error.
04/25/03    aku    Moved dssocki_set_non_sys_socket_flow() to dssinternal.c 
                   and added support for app_specified_iface_ptr.
04/23/03    aku    Set routing cache in scbs to NULL once interface goes down
04/23/03    ss     Separated the socket option validation information from 
                   the option values. Only the values are stored in the scb.
04/14/03    sv     Added dssocki_send_msg and dssocki_recv_msg functions. 
                   Added udp_conetsetup and udp_shutdown to function pointer
                   table. Modified dss_get_err_val to support shutdown for
                   connected udp sockets.
04/10/03    ss     Added dssocki_is_addr_in_use() and modified 
                   dssocki_getephem_port() to call it.
03/20/03    aku    Convert IP address in IP address change callback to 
                   network byte order.
03/19/03    ss     Modified copy_buf_to_dsm() to address the edge condition
                   when size of the first dsm item equals the first iov len
02/28/03    usb    In dssocki_socket_event_check() added iface_ptr valid 
                   check before checking for non sys socket flow control
02/24/02    aku    Access ps_iface_ptr from scb instead of acb. 
02/13/03    ss     Modified code to support socket shutdown
02/12/03    om     Preserve SCB's routing cache in metainfo.
02/12/03    aku    Added support for flow control of non system sockets on a 
                   per interface basis. Removed unused 
                   dssocki_non_sys_sockets_flow_controlled().
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h in
                   dsssocki_reg_ip_filter() and dssocki_dereg_ip_filter().
02/07/03   usb     Added support for adding/deleting IP filters to iface
01/17/03   usb     Misc fixes - In dssocki_iface_ev_cback() phys link state 
                   is being compared against state rather than link_state, 
                   dssocki_invoke_app_ev_cbacks() is called only if an ACB 
                   is currently being used, removed event callback 
                   IOCTL support to specify "All interfaces" 
12/24/02    aku    Added support for reg/dereg event callbacks using IOCTL.
12/23/02    aku    Added support for flow controlling non-system sockets in 
                   dssocki_event_check() and dssocki_common_write_check().
12/22/02    aku    Removed use of global dssocki_iface_cb_type array and 
                   Added checks for IFACE_UP and IFACE_DOWN event callbacks 
                   to ensure that the app network callbacks are called only 
                   if there is a iface state change.
11/26/02    usb    Increased number of iface_cb buffers to accomodate 
                   multiple events that each app can get
11/25/02    aku    Set the ps_iface_ptr in the acb to NULL upon receiving the
                   IFACE_DOWN_EV.
11/19/02    aku    Notify apps only after cbacks have been deregistered with
                   ps_iface in dssocki_process_iface_in_use(). Fixed 
                   MOBILE IP featurization in dssocki_iface_in_use().
11/19/02    mvl    removed JCDMA from window size socket option.
11/19/02   jd/usb  dssocki_mip_iface_in_use checks if iface is !down, 
                   doesn't do early routing (done in pppopen)
11/14/02    jd     dssocki_is_iface_in_use fixed for MIP to ignore MIP app 
                   Added dssocki_mip_iface_in_use to see if MIP iface is
                   available
11/12/02    mvl    Added support for changing default window size.
11/08/02    sv     Added protocol check for bind_route in get_err_val. When
                   the socket is in OPEN state it return DSS_SUCCESS if it
                   is a udp socket.
10/30/02    ss     Added code for bind routine in get_err_val().  Changed
                   copy_buf_to_dsm() to allocate large DSM items if small
                   ones are not available.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
10/08/02    om     Fixed preservation of app-field across send_q.
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
10/02/02    mvl    fixed meta info memory leak.
09/24/02    om     Added dssocki_generate_meta_info().
09/10/02    aku    Removed global Ip_addr if FEATURE_DATA_MM is defined.
09/12/02    aku    Reverting change to dssocki_common_write_check() function
                   signature as using pointer to struct degrades performance
09/11/02    aku    Changed dssocki_common_write_check() to take pointer to
                   struct instead of multiple individual arguments.
09/05/02    aku    Fixed bug in registering for ps_iface events and added
                   checks for return values from ps_iface functions.
09/05/02    aku    Updated for ps_iface cback buf interface.
08/28/02    ss     Corrected msg level from high to low for some debug msgs
08/16/02    ss     Added a new socket option for silent close 
08/13/02    mvl    updated ps_iface state macros for name change (MM feature)
08/10/02    na     Fixed bug in dssocki_iface_check under FEATURE_DATA_MM
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/25/02    aku    Deregister for iface events after DOWN event is recd.
07/24/02    aku    app_iface_state is now used only if FEATURE_DATA_MM is
                   not defined. 
07/23/02    aku    Added support for dormant reorignation.
07/22/02    aku    Set acb_ptr->ps_iface_ptr to NULL after iface comes down.
07/18/02    ss     Modified dssocki_appidtoacb() for range checking on app_id
07/18/02    aku    Featurized changes under FEATURE_DATA_MM.
                   Set Ip_addr when iface comes up and notify sockets when
                   iface goes down.
07/17/02    aku    Replaced rex_set_sig() with PS_SET_SIGNAL()
07/16/02    aku    dssifcb_init() moved to psi_init()
07/15/02    aku    Removed refences to dssdorm and fixed some compiler 
                   warnings.
07/14/02    aku    Added dssocki_handle_iface_ev_cback() and
                   dssocki_iface_in_use() for FEATURE_DATA_MM.
07/11/02    na     Added multimode support under FEATURE_DATA_MM.
05/09/02    mvl    Added boolean to common_write_check() which indicates
                   whether to re-orig if dormant.
03/05/02    pjb    Finished Accept Listen Code Review Items 
02/22/02    pjb    Added accept and listen
02/11/02    ss     Modified dssocki_copy_buf_to_dsm() routine to allocate
                   memory from small item pool when running low on large 
                   items. Made changes to flow control the application when
                   there is enough data queued for transmission.
01/15/01    aku    Added prototype for
                   dssocki_non_sys_sockets_flow_controlled()
12/18/01    rsl    Updated tracer field.
11/12/01    aku    Changed boolean ppp_active to app_iface_state (enum 
                   ppp_close_reason type). Removed redundant
                   dssocki_is_socket_app_active()
                   dssocki_socket_event_occurred() takes an additonal
                   acb_ptr arg
11/09/01    aku    Added function dssocki_is_socket_app_active()
09/25/01    ss     Modified dssocki_socket_event_check() to use the memory
                   buffers when free count is more than few (instead of more
                   than many) to make it consistent with DSM.
09/16/01    ss     Modified dssocki_set_non_sys_socket_flow() to notify the
                     applications when the flow control is enabled
08/27/01    aku    Removing net_event_active flag from acb_type struct. No
                   longer used.
08/25/01    na     CR18110: In dssocki_common_write_check() made sure the 
                   appropriate check for dsm item level is done.
08/23/01    aku    Renamed dss_getifacestatus as dss_get_iface_status
08/17/01    aku    Protecting access to ppp_active using INTLOCK()
07/23/01    pjb	   Return type of dsm_pushdown changed. Removed the calls 
                   to unneeded zone bit functions. Consolodated the free 
                   count API.
06/26/01    na     Removed dssocki_num_active_apps() because sockets layer
            mvl    now sends EXIT to dsmgr whenever it closes PPP.
                   Added the dssocki_set_non_sys_socket_flow() function and
                   the flow control variable
06/26/01    sjy    Added initial socket options values for DSS_SO_SYS_SOCK
                   and deleted entry for DSS_SO_ZERO_IP_OK.
05/18/01    mvl    Cleaned up code - removed zero address socket option code
                   as that is no longer the method for sockets flow control
                   for Mobile IP.
04/06/01   js      Modified dssocki_socket_event_check() to support socket
                   option DSS_SO_IP_ADDR_OK. When attempting to write when
                   the option is off and Ip_addr is indeed 0, the event will
                   not be enabled(which will result in DS_EWOULDBLOCK  
                   returning to the application requesting the write).
04/06/01    js     Added function dssocki_common_write_check(), which 
                   contains generic procedure in a socket write operation. 
04/06/01    js     Added default entry for socket option DSS_SO_IP_ADDR_OK 
03/27/01    mvl    Changed the all instances of errno to dss_errno.  General
                   cleanup (77, indentation, page breaks).  App id is now
                   passed into both the socket and network callback funtions.
03/27/01    js     Added dss_fcn_ptr_table entry for ICMP socket.
03/27/01    js     Modified dssocki_send_to_ps() to use the extra parameter
                   containing the desination information. (The destination
                   info in scb may be stale).
03/14/01    snn    Modified dssocki_appidtoacb function to check the appid
                   correctly for the upper bound.
02/14/01    js     Added default values for socket options. 
02/14/01    js     Modified dssocki_allocscb() to support socket options. 
02/08/01    yll    Added dsstcp_read_dsm_chain in dssock_fcn_ptr_table.
11/10/00    snn    modified dword to uint32; scb_type to scb_s;
                   Added new variables dss_sockfdbase and dss_appidbase;
                   Created a new function dssocki_indextoscb();
10/20/00    snn    Modified the way in which index is given when scb or acb
                   is allocated.
10/10/00    jd     Merged PPG r1.15 changes from PPG - adjustments to account
                   DSM small item data offset                   
09/03/00    snn    Made changes to support multiple applications.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. So, if pool size is to be passed in as 
                   parameter, a new macro DSM_DS_POOL_SIZE is used to 
                   return pool id based on pool size. Added Macros that 
                   check if there are enough items or if we are running out
                   of items.                   
08/03/00  mvl/js   Added table entry to support protocol specific bind().
06/07/00    rc     Removed ovld of link ptr in dsm items.
02/02/00    ak     Merged branch and tip to support dsm changes for rlp3.
12/23/99    na     Changed q_puts to use the ovld of link ptr in dsm items.
12/08/99    ak     In dssocki_appidtoacb, added range check on input app_id.
11/01/99    rc     Removed some lint errors.
09/22/99    rc     Incorporated code review changes and comments.
08/18/99    ak     Made fixes so compiles when FEATURE_DS turned off.
08/11/99    ak     In dssocki_copy_buf_to_dsm, now selectively copy to
                   either small or large items, as opposed to always to
                   large items.  Also added abort function, for closing
                   socket when no network.  In event_check, loosened
                   restrictions for write event, so that dormancy works
                   correctly.
07/15/99    ak     Check for socket and net callback function ptr being 
                   not equal to NULL before actually calling the callback.
04/21/99    hcg    Added logic to dssocki_allocscb() and dssocki_allocacb() 
                   to remove some Lint errors.  Also, updated
                   parameter to dssocki_send_to_ps(), to conform to new kind
                   field sizing in dsm_item_type.
04/16/99    hcg    Fixed buffer size comparison in dssocki_socket_event_check(),
                   which needed modification as a result of the changes in 
                   dsm.c.  The write event will now be asserted properly when
                   enough buffer space is available.
04/05/99    hcg    Incorporated code review changes and comments.  Changed
                   location of INTLOCK()/INTFREE() macros in allocation 
                   routines.  Modified access loops to provide more efficient
                   access.  Removed dssocki_convert_ntoh() as its functionality
                   is now provided through macros.
10/06/98  na/hcg   Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DS_SOCKETS
#include "task.h"
#include "rex.h"
#include <string.h>

#include "err.h"
#include "netuser.h"
#include "dssocket.h"
#include "dssocki.h"
#include "dsstcp.h"
#include "dssudp.h"
#include "dsm.h"
#include "assert.h"
#include "msg.h"
#include "dssicmp.h"
#include "dssifcbacks.h"
#include "dss_ps.h"
#include "ps_svc.h"
#include "ps_tcp.h"
#include "ps_iface.h"
#include "ps_iface_ipfltr.h"
#include "pstimer.h"
#include "ps_route.h"
#include "ran.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_io.h"
#include "ps_acl.h"

#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_SEC_IPSEC
#include "secips.h"
#include "secipsiface.h"

#endif /* FEATURE_SEC_IPSEC */


/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
                        DEFINES & CONSTANTS
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Threshold size to determine if a large or small item should be used in the 
  dssocki_copy_buf_to_dsm.
---------------------------------------------------------------------------*/
#define DSSOCKI_SMALL_ITEM_THRESHOLD   (3 * (DSM_DS_SMALL_ITEM_SIZ)) 

/*---------------------------------------------------------------------------
  SDB related valid bits for the sendto() flags.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS2000_SDB
  #define MSG_SDB_VALID_BITS (MSG_EXPEDITE | MSG_FAST_EXPEDITE)
#else
  #define MSG_SDB_VALID_BITS (0x0)
#endif

/*---------------------------------------------------------------------------
  The reserved bits for the sendto() flags. Whenever a new flag is to be
  added, it should be defined in dssocket.h and ORed with MSG_SDB_VALID_BITS
  below so that the reserved bits mask is properly built. 
---------------------------------------------------------------------------*/
#define MSG_VALID_BITS    (MSG_SDB_VALID_BITS)
#define MSG_RESERVED_BITS (~MSG_VALID_BITS)
/*---------------------------------------------------------------------------
                        Variable Definitions
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------- 
  Socket control block array 
---------------------------------------------------------------------------*/
struct scb_s scb_array[DSS_MAX_SOCKS];
/*---------------------------------------------------------------------------
  Socket configuration control block
---------------------------------------------------------------------------*/
struct dss_socket_config_s sock_config_cb;
/*--------------------------------------------------------------------------- 
Application control block array 
---------------------------------------------------------------------------*/
acb_type dssocki_acb_array[DSS_MAX_APPS]; 

/*---------------------------------------------------------------------------
  Global counter of number of active applications                       
---------------------------------------------------------------------------*/
LOCAL sint15   dssocki_acb_count = 0; 

/*---------------------------------------------------------------------------
  Socket function pointer table.  The functions correspond to the 
  protocol specific function calls.  The format of this table is as 
  follows:

  type,
  socket,     connect setup,           connect,
  listen_setup, listen,
  accept,
  write,      read,                    read_dsm_chain,
  shutdown,
  close,      abort,                   bind,
  is_writeable
  
  with the final entry in the table being DSS_ERROR.
  In UDP, abort behaves the same as close.
---------------------------------------------------------------------------*/
struct dss_fcn_ptr_table dssock_fcn_ptr_table[] = 
{

  /*-------------------------------------------------------------------------
    TCP functions - found in dsstcp.c
  -------------------------------------------------------------------------*/
  SOCK_STREAM,
  dsstcp_socket,  dsstcp_connect_setup,  dsstcp_connect,
  dsstcp_listen_setup,   dsstcp_listen,   
  dsstcp_accept,   
  dsstcp_write,   dsstcp_read,           dsstcp_read_dsm_chain,
  dsstcp_shutdown,
  dsstcp_close,   dsstcp_abort,          dsstcp_bind,
  dsstcp_is_writeable,

  /*-------------------------------------------------------------------------
    UDP functions - found in dssudp.c
  -------------------------------------------------------------------------*/
  SOCK_DGRAM,
  dssudp_socket,  dssudp_connect_setup,  NULL,
  NULL,           NULL,
  NULL, 
  dssudp_write,   dssudp_read,           dssudp_read_dsm_chain,
  dssudp_shutdown,
  dssudp_close,   dssudp_close,          dssudp_bind,
  NULL,

  /*-------------------------------------------------------------------------
    ICMP functions - found in dssicmp.c
  -------------------------------------------------------------------------*/
  SOCK_ICMP,
  NULL,           NULL,                  NULL,
  NULL,           NULL,
  NULL,  
  dssicmp_write,  dssicmp_read,          NULL,
  NULL,
  dssicmp_close,  NULL,                  NULL,
  NULL,

  /*-------------------------------------------------------------------------
    End of table
  -------------------------------------------------------------------------*/
  DSS_ERROR,
  NULL,           NULL,                  NULL,
  NULL,           NULL,
  NULL,
  NULL,           NULL,                  NULL,
  NULL,
  NULL,           NULL,                  NULL,
  NULL
};

/*---------------------------------------------------------------------------
  Socket File describtor Base.
  This variable is used to allocate the socket file descriptor for each 
  socket.
  
  sockfd = dss_sockfdbase + index; This ensures that each sockfd can now
  have a value SOCKFDBASE_MIN_VAL to 2^15. So, dss_sockfdbase can have a 
  value from SOCKFDBASE_MIN_VAL to 2^15 Minus maximum value of index. 
  Hence, this variable should be as same type as sockfd (i.e. sint15). 
  
  dss_sockfdbase is made as static because the earlier value should be
  remembered to give a new sockfd value.
  
  Initialize it to SOCKFDBASE_MIN_VAL, to start with. This will be the 
  sockfd allotted to the first opened socket. Note that we choose 
  SOCKFDBASE_MIN_VAL, arbitrarily and no special reason is attributed 
  for that. 
  
  The values that this variable will have belong to the following set:
  {  
     SOCKFDBASE_MIN_VAL + DSS_MAX_SOCKS, 
     SOCKFDBASE_MIN_VAL + 2*DSS_MAX_SOCKS, 
     SOCKFDBASE_MIN_VAL + 3*DSS_MAX_SOCKS...
     SOCKFDBASE_MIN_VAL + N * DSS_MAX_SOCKS
  } 
  where N is given by : ____________
  It should be a multiple of DSS_MAX_SOCKS
  
  Assuming we have SOCKFDBASE_MIN_VAL as 100 and DSS_MAX_SOCKS as 3, 
  here is the set of the values that each index can get:
  
     Index         Possible sockfd's
   ---------   --------------------------
  |    0    |   100, 103, 106,..
   --------- -----------------------------
  |    1    |   101, 104, 107
   --------- -----------------------------
  |    2    |   102, 105, 108 ...
   --------- -----------------------------
   
Relation between sockfdbase and sockfd:
   sockfd = sockfdbase + index
   sockfdbase = SOCKFDBASE_MIN_VAL + (n * MAX_SOCKS)
   
Ranges of sockfd and sockfdbase:   
   Minimum value of sockfd:      SOCKFDBASE_MIN_VAL
   Maximum value of sockfd:      SOCKFD_BASE_MAX_VAL
   Minimum value of sockfd_base: SOCKFDBASE_MIN_VAL
   Maximum value of sockfd_base: SOCKFD_BASE_MAX_VAL
   

---------------------------------------------------------------------------*/
static sint15 dss_sockfdbase = SOCKFDBASE_MIN_VAL;

/*---------------------------------------------------------------------------
  Application Identifier Base.
  This is calculated same way as sockfdbase.

---------------------------------------------------------------------------*/
static sint15 dss_appidbase = APPIDBASE_MIN_VAL;


/*---------------------------------------------------------------------------
  Initial Socket Options Values. 
  The following initial values will be copied to the socket option 
  table in the socket control block. The copying takes place upon
  the allocation of a scb. 
  
  When expanding the repertoire of supported socket options, all the 
  related definitions in dssocki.h as well as the table before have 
  to be updated. The table below follows this format:
  
    opt  |  opt value type |  opt   | optval |  init     |    range
   name  |    indicator    | level  |  size  | opt value |  min - max
 --------+-----------------+--------+--------+-----------+-------------
  
  PLEASE NOTE THAT ANY NEW ENTRIES SHOULD BE ORDERED ACCORDING TO THE 
  DEFINITION OF "optname" given by dss_sockopt_names_type.

  Note: The initial and min-max value of the SO_LINGER option below 
        corresponds only to the first field of the structure, i.e., l_onoff.
---------------------------------------------------------------------------*/
const dssocki_sockopt_type dss_sock_opts_default[DSS_SOCKOPT_MAX] =
{
  { DSS_IP_TTL, DSSI_SO_INT, DSS_IPPROTO_IP, sizeof(int), 255, 0, 255 },

  { DSS_SO_SYS_SOCK, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1 },

  { DSS_SO_SILENT_CLOSE, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1 },

  { DSS_SO_RCVBUF, DSSI_SO_INT, DSS_SOCK, sizeof(int), DEF_WND, 
    DSS_MIN_RCVBUF, DSS_MAX_RCVBUF },

  { DSS_SO_LINGER, DSSI_SO_LINGER, DSS_SOCK, sizeof(dss_so_linger_type), 0, 
    0, 1 },

  { DSS_SO_SNDBUF, DSSI_SO_INT, DSS_SOCK, sizeof(int), DSS_DEF_SNDBUF,
    DSS_MIN_SNDBUF, DSS_MAX_SNDBUF },
  
  { DSS_SO_SDB_ACK_CB, DSSI_SO_SDB_ACK_CB_TYPE, DSS_SOCK, 
    sizeof(dss_so_sdb_ack_cb_type), 0, 0, 0 },

  {DSS_SO_KEEPALIVE, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1},

  {DSS_TCP_NODELAY, DSSI_SO_BOOL, DSS_IPPROTO_TCP, sizeof(int), 0, 0, 1},
};

/*---------------------------------------------------------------------------
  Initial value for the l_linger field, i.e., the duration in ms to 
  linger for, of the dss_linger_type for SO_LINGER option.
---------------------------------------------------------------------------*/
static sint31 so_linger_initval = 0;

#ifdef FEATURE_SEC_IPSEC
/* For now we need to know not to run IKE msgs via the IPSEC iface */
extern sint15 secipsdss_ike_sockfd;
#endif /* FEATURE_SEC_IPSEC */


/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSOCKI_LINGER_TIMEOUT_CB()

DESCRIPTION
  The callback function associated with the pstimer for the linger timeout.
  This callback is called when the linger timeout on a socket expires.
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
dssocki_linger_timeout_cb
(
  void* in_scb_ptr
)
{
  struct scb_s* scb_ptr = (struct scb_s *) in_scb_ptr; /* ptr to the scb   */
  acb_type* linger_acb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate an acb and make its iface_ptr same as the iface being used by
    scb_ptr->acb_ptr so that a subsequent pppclose() by application does
    not bring down the physical interface. We need to do this so that TCP
    close can happen in the background. 
  -------------------------------------------------------------------------*/
  if ( (linger_acb_ptr = dssocki_allocacb()) == NULL )
  {
    MSG_ERROR("Failed to alloc acb", 0, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Set the state flag indicating that the socket is lingering
  -------------------------------------------------------------------------*/
  scb_ptr->state_flags |= DSSOCKI_LINGER;

  linger_acb_ptr->ps_iface_ptr = scb_ptr->acb_ptr->ps_iface_ptr;
  linger_acb_ptr->app_specified_iface_ptr = 
    scb_ptr->acb_ptr->app_specified_iface_ptr;
  scb_ptr->linger_acb_ptr = linger_acb_ptr;

  /*-------------------------------------------------------------------------
    Call socket_event_occurred() which will assert the CLOSE_EVENT 
  -------------------------------------------------------------------------*/
  dssocki_socket_event_occurred(scb_ptr, NULL);

  return; 
}

/*===========================================================================
FUNCTION      DSSOCKI_SDB_STATUS_HANDLER()

DESCRIPTION   This function is called by the ps_iface when it receives a
              notification from CM indicating SDB ack. This functions calls
              the socket specific callback if the socket option is set.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void
dssocki_sdb_status_handler
(
  sint15 sockfd,
  void*  cm_info_ptr
)
{
  struct scb_s      * scb_ptr;       /* Ptr to socket control block        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    MSG_ERROR("scb_ptr is NULL", 0, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the socket specific callback if the socket option is set
  -------------------------------------------------------------------------*/
  if ( IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_SDB_ACK_CB) )
  {
    /*-----------------------------------------------------------------------
      Call the sdb ack callback if the callback function pointer is not NULL
    -----------------------------------------------------------------------*/
    if (scb_ptr->sdb_ack_cb_fcn != NULL)
    {
      scb_ptr->sdb_ack_cb_fcn( sockfd, 
                               (dss_sdb_ack_status_info_type *) cm_info_ptr, 
                               scb_ptr->sdb_ack_user_data ); 
    }
  }

  return;
}

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DSSOCKI_ALLOCSCB()

DESCRIPTION
  Allocates socket control block from SCB array.  Calculates socket
  descriptor and sets it in the SCB.   
 
DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct scb_s *dssocki_allocscb
(
  void
)
{
  static uint16 index = 0;                               /* index of array */
  uint16 loop;                                               /* loop index */
  struct scb_s *scb_ptr = NULL;   /* ptr to socket control block structure */
  uint32 int_sav;        /* local variable used for saving interrupt state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Loop through array to find available SCB - break when available SCB
    is found.  Protect access to global scb_array through 
    INTLOCK_SAV()/INTFREE_SAV().
        The variable, "index" is the scb array index. It is made as static 
    because , whenever a socket is opened the index given is one higher than
    the previous one. This is a circular array. This method of index 
    allocation allows us to allocate different indices every time a new
    socket is opened and closed.
  -------------------------------------------------------------------------*/
  for (loop=0; loop < DSS_MAX_SOCKS; loop++)
  {
    
    INTLOCK_SAV(int_sav);
    index = (index+1) % DSS_MAX_SOCKS; 
    if (scb_array[index].sockfd == SOCKAVAIL)
    {
      scb_ptr = &scb_array[index];
      /*---------------------------------------------------------------------
        Assign socket descriptor value in the valid range between 
        SOCKFDBASE_MIN_VAL to SOCKFDBASE_MAX_VAL.  This is the socket 
        descriptor (handle) specified in all socket function calls.
      ---------------------------------------------------------------------*/
      scb_ptr->sockfd = (sint15) (dss_sockfdbase + index);
      INTFREE_SAV(int_sav);      
      break;
    }
    else
    {
      INTFREE_SAV(int_sav);
    }
  }

  /*-------------------------------------------------------------------------
    No more sockets are available.  Unlock interrupts and return.
  -------------------------------------------------------------------------*/
  if (loop == DSS_MAX_SOCKS)
  {
    return (NULL);
  }

  /*-------------------------------------------------------------------------
    Initialize the scb
  -------------------------------------------------------------------------*/
  if (scb_ptr != NULL)
  {
    if (dssocki_init_scb( scb_ptr ) == FALSE)
    {
      scb_ptr->sockfd = SOCKAVAIL;
      scb_ptr = NULL;
    }
  }

  return (scb_ptr);
} /* dssocki_allocscb() */

/*===========================================================================
FUNCTION DSSOCKI_INIT_SCB()

DESCRIPTION
  Initializes the given scb
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If the initialization was successful
  FALSE: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean
dssocki_init_scb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
)
{
  int i;
  int timer_ret_val;  /* Return value from timer fn. calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  scb_ptr->fcn_ptr_table        = NULL;
  scb_ptr->type                 = 0;
  scb_ptr->protocol             = 0;
  
  scb_ptr->localname.address    = INADDR_ANY;
  scb_ptr->localname.port       = 0;
  scb_ptr->servname.address     = 0;
  scb_ptr->servname.port        = 0;
  
  scb_ptr->requested_event_mask = 0;
  scb_ptr->data_available       = FALSE;
  scb_ptr->closed_state_reason  = DS_ENOTCONN;

  /*-------------------------------------------------------------------------
    zero out queue lengths
  -------------------------------------------------------------------------*/
  scb_ptr->so_head              = NULL;
  scb_ptr->so_estab             = NULL;
  scb_ptr->so_partial           = NULL;
  scb_ptr->so_qlimit            = 0;

  /*-------------------------------------------------------------------------
    Set socket state to INIT and reset the state_flags
  -------------------------------------------------------------------------*/
  scb_ptr->socket_state         = DSSOCKI_INIT;
  scb_ptr->state_flags          = 0x0;

  /*-------------------------------------------------------------------------
    Set socket options to default values. Certain fields should never 
    be changed once initilized, these fields include option name, option 
    level and option value data type. Only option value and the size of the 
    option value are subject to assignments. 
  -------------------------------------------------------------------------*/
  scb_ptr->options_mask = 0x0;

  for (i=DSS_SOCKOPT_MIN+1; i < DSS_SOCKOPT_MAX; i++)
  {
    if (dss_sock_opts_default[i].initval)
    {
      SOCKOPT_SET(scb_ptr->options_mask, i);
    }
  }

  scb_ptr->ip_ttl    = dss_sock_opts_default[DSS_IP_TTL].initval;
  scb_ptr->rcvbuf    = dss_sock_opts_default[DSS_SO_RCVBUF].initval;
  scb_ptr->sndbuf    = dss_sock_opts_default[DSS_SO_SNDBUF].initval;
  scb_ptr->so_linger = so_linger_initval;

  /*-------------------------------------------------------------------------
    Allocate the linger timer
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_alloc( dssocki_linger_timeout_cb, scb_ptr );
  if( timer_ret_val == -1 ) /*PS_TIMER_FAILURE*/ 
  {
    MSG_ERROR("Failed to allocate linger timer, sockfd=%d", 
               scb_ptr->sockfd, 0, 0);
    return( FALSE );
  }
  else
  {
    scb_ptr->linger_timer = timer_ret_val;
  }

  memset (scb_ptr->fi_map, 0, sizeof(scb_ptr->fi_map));

  return( TRUE );
}

/*===========================================================================
FUNCTION DSSOCKI_FREESCB()

DESCRIPTION
  Frees a socket control block in the SCB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_freescb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
)
{
  uint16 index;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(scb_ptr != NULL);

  /*-------------------------------------------------------------------------
    Cleanup any filters installed in the ifaces for this socket
  -------------------------------------------------------------------------*/
  for(index=0; index<MAX_SYSTEM_IFACES; index++)
  {
    if(scb_ptr->fi_map[index] != NULL)
    {
      ps_iface_ipfltr_delete(scb_ptr->fi_map[index], scb_ptr->sockfd);
    }
  }

  /*-------------------------------------------------------------------------
    Free the linger timer
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_free( scb_ptr->linger_timer);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS );

  /*-------------------------------------------------------------------------
    Free the socket control block and set everything to NULL - then
    make it available for re-use.
  -------------------------------------------------------------------------*/
  memset( scb_ptr, 0, sizeof(struct scb_s) );

  scb_ptr->sockfd = SOCKAVAIL;

  /*-------------------------------------------------------------------------
    Update the value of the dss_sockfdbase. 
    If >= to Maximum, 
       re-initialize it.    
  -------------------------------------------------------------------------*/
  dss_sockfdbase += DSS_MAX_SOCKS;
  if(dss_sockfdbase  >= SOCKFDBASE_MAX_VAL)
  {
    MSG_MED("Changing sockfdbase from %d to %d",
             dss_sockfdbase, SOCKFDBASE_MIN_VAL,0);
    dss_sockfdbase = SOCKFDBASE_MIN_VAL;
  }                                          
} /* dssocki_freescb() */

/*===========================================================================
FUNCTION DSSOCKI_SOCKFDTOSCB()

DESCRIPTION
  Given a socket descriptor, returns a socket control block from SCB array.  

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.
===========================================================================*/
struct scb_s *dssocki_sockfdtoscb
(
  int sockfd                                          /* socket descriptor */
)
{
  struct scb_s *scb_ptr;          /* ptr to socket control block structure */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Find the matching SCB by indexing into the socket array.
    Protect access to global scb_array through INTLOCK()/INTFREE().
    
    We need to find the index, we use following equations:
         sockfd = sockfdbase + index. 
         This can also be written as,
         sockfd = ( SOCKFDBASE_MIN_VAL + (n * MAX_SOCKS)) + index.
              where n is number of times sockets have been closed.
         
         index = (sockfd - SOCKFDBASE_MIN_VAL) mod MAX_SOCKS

  -------------------------------------------------------------------------*/

  if ( sockfd < SOCKFDBASE_MIN_VAL || 
       sockfd >= SOCKFDBASE_MAX_VAL + DSS_MAX_SOCKS )
  {
    return NULL;
  }

  INTLOCK();
  if (scb_array[((sockfd - SOCKFDBASE_MIN_VAL) % DSS_MAX_SOCKS)].sockfd ==
      sockfd)   
  {
    /*-----------------------------------------------------------------------
      Get pointer to SCB item and return it.
      from sockfd, we can obtain the index of the array. since sockfd equals
      (N * DSS_MAX_SOCKS + Index)  
    -----------------------------------------------------------------------*/
    scb_ptr = &scb_array[(sockfd - SOCKFDBASE_MIN_VAL) % DSS_MAX_SOCKS]; 
  }
  else
  {
    /*-----------------------------------------------------------------------
      The socket descriptor was not found, so return NULL.  This means that
      an invalid socket descriptor was specified to the function call.
    -----------------------------------------------------------------------*/
    scb_ptr = NULL;
  }
  INTFREE();

  return (scb_ptr);

} /* dssocki_sockfdtoscb() */

/*===========================================================================
FUNCTION DSSOCKI_INDEXTOSCB()

DESCRIPTION
  Convert the index of scb_array to the address.
  
  This function will take the index of the scb_array as the input. If the 
  sockfd is valid for this index in the scb_array, i.e. Not SOCKAVAIL, then 
  we return the address of that element of array. 
  If the sockfd equals SOCKAVAIL, then we return a NULL.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.
===========================================================================*/
struct scb_s *dssocki_indextoscb
(
  int i                                               /* socket descriptor */
)
{
  struct scb_s *scb_ptr;          /* ptr to socket control block structure */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    See if the socket is available.
  -------------------------------------------------------------------------*/

  INTLOCK();
  if (scb_array[i].sockfd != SOCKAVAIL)    
  {
    /*-----------------------------------------------------------------------
      Get pointer to SCB item and return it.
    -----------------------------------------------------------------------*/
    scb_ptr = &scb_array[i];  
  }
  else
  {
    /*-----------------------------------------------------------------------
      The socket descriptor was not found, so return NULL.  This means that
      an invalid socket descriptor was specified to the function call.
    -----------------------------------------------------------------------*/
    scb_ptr = NULL;
  }
  INTFREE();

  return (scb_ptr);

} /* dssocki_indextoscb() */


/*===========================================================================
FUNCTION DSSOCKI_ALLOCACB()

DESCRIPTION
  Allocates application control block from ACB array.  Assigns application
  ID and sets the network and socket callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
acb_type *dssocki_allocacb
(
  void
)
{
  static int index=0;                                /* Index of acb array */  
  int loop;                                                  /* loop index */
  acb_type *acb_ptr = NULL;  /* ptr to application control block structure */
  uint32 int_sav;        /* local variable used for saving interrupt state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Loop through array to find available ACB and break when an available
    ACB is found.  Protect access to global dssocki_acb_array through 
    INTLOCK_SAV()/INTFREE_SAV().
        The variable, "index" is the acb array index. It is made as static 
    because , whenever an app is opened the index given is one higher
    than the previous one. This is a circular array. This method of index 
    allocation allows us to allocate different indices every time a new app 
    is opened and closed.
  -------------------------------------------------------------------------*/
  for (loop=0; loop < DSS_MAX_APPS; loop++)
  {
    INTLOCK_SAV(int_sav);
    
    /*-----------------------------------------------------------------------
      Note that the first time index takes a value of 1 instead of zero. It's
      still okay.
    -----------------------------------------------------------------------*/
    index = (index+1) % DSS_MAX_APPS; 
    if (dssocki_acb_array[index].app_id == APPAVAIL)
    {
      acb_ptr = &dssocki_acb_array[index]; 

      /*---------------------------------------------------------------------
        Assign application ID in the valid range between SOCKFDBASE_MAX_VAL
        and SOCKFDBASE_INIT_VAL.
        This is the application descriptor (handle) specified in many of the 
        network library function calls.
      ---------------------------------------------------------------------*/
      acb_ptr->app_id = (sint15) (dss_appidbase + index ); 
      
      /*---------------------------------------------------------------------
        Increment global application allocation counter.
      ---------------------------------------------------------------------*/
      dssocki_acb_count++; 
      INTFREE_SAV(int_sav);
      break;
    }
    else
    {
      INTFREE_SAV(int_sav);
    }
  }

  /*-------------------------------------------------------------------------
    No more application control blocks are available, so return NULL,
    which will indicate failure to allocate a new ACB.
  -------------------------------------------------------------------------*/
  if (loop == DSS_MAX_APPS)
  {
    return (NULL);
  }

  if (acb_ptr != NULL)
  {
    acb_ptr->net_callback_fcn             = NULL;
    acb_ptr->socket_callback_fcn          = NULL;
    acb_ptr->notified_cb_flag             = FALSE;
    acb_ptr->ps_iface_ptr                 = NULL;
    acb_ptr->app_specified_iface_ptr      = NULL;
    /*---------------------------------------------------------------------
      Initialize the event queue structure.
    ---------------------------------------------------------------------*/
    for(index = 0; index < DSS_IFACE_IOCTL_EVENT_MAX; index++)
    {
      q_init((acb_ptr->event_q_array) + index);
    }
  }
  return (acb_ptr);

} /* dssocki_allocacb() */

/*===========================================================================
FUNCTION DSSOCKI_FREEACB()

DESCRIPTION
  Frees an application control block in the ACB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_freeacb
(
  acb_type *acb_ptr          /* ptr to application control block structure */
)
{

  ASSERT(acb_ptr != NULL);

  /*-------------------------------------------------------------------------
    Clear all fields and make available for re-use
  -------------------------------------------------------------------------*/
  acb_ptr->net_callback_fcn              = NULL;
  acb_ptr->socket_callback_fcn           = NULL;
  acb_ptr->notified_cb_flag              = FALSE;
  acb_ptr->ps_iface_ptr                  = NULL;
  acb_ptr->app_specified_iface_ptr       = NULL;

  /*-------------------------------------------------------------------------
    Decrement global application counter and make the app_id available for
    reuse - protect access through use of
    INTLOCK()/INTFREE().
  -------------------------------------------------------------------------*/
  INTLOCK();
  acb_ptr->app_id = APPAVAIL;
  dssocki_acb_count--;

  /*-------------------------------------------------------------------------
    Update the value of the dss_appidbase. If it is greater than the Maximum 
    value it can hold, bring it back to initial value.
  -------------------------------------------------------------------------*/
  dss_appidbase += DSS_MAX_APPS;
  if(dss_appidbase >= APPIDBASE_MAX_VAL)
  {
    MSG_MED("Changing appidbase from %d to %d",
             dss_appidbase,APPIDBASE_MIN_VAL,0);
    dss_appidbase = APPIDBASE_MIN_VAL;
  }

  INTFREE();

} /* dssocki_freeacb() */

/*===========================================================================
FUNCTION DSSOCKI_APPIDTOACB()

DESCRIPTION
  Given an application ID, returns an application control block from ACB 
  array.  

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.

SIDE EFFECTS
  None.
===========================================================================*/
acb_type *dssocki_appidtoacb
(
  sint15 app_id                                          /* application ID */
)
{
  acb_type *acb_ptr;         /* ptr to application control block structure */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Do range checking on input param.
  -------------------------------------------------------------------------*/
  if ( app_id >= (APPIDBASE_MAX_VAL + DSS_MAX_APPS) ||
       app_id < APPIDBASE_MIN_VAL )
  {
    return NULL;
  }
  
  /*-------------------------------------------------------------------------
    Find the matching ACB by indexing into the socket array.
    Protect access to global dssocki_acb_array through INTLOCK()/INTFREE().
    
    app_id = appidbase + index. We need to find the index.
    This can also be written as,
    app_id = ( APPIDBASE_MIN_VAL + n * MAX_APPS) + index.
    So, index = (app_id - APPIDBASE_MIN_VAL) mod MAX_APPS
  -------------------------------------------------------------------------*/
  INTLOCK();
  if (dssocki_acb_array[(app_id-APPIDBASE_MIN_VAL) % DSS_MAX_APPS].app_id ==
      app_id)
  {
    /*-----------------------------------------------------------------------
      get pointer to ACB item and return it.
     ----------------------------------------------------------------------*/
    acb_ptr = &dssocki_acb_array[(app_id-APPIDBASE_MIN_VAL) % DSS_MAX_APPS];
  }
  else
  {
    /*-----------------------------------------------------------------------
      The application ID could not be found, indicating an invalid 
      application ID was specified.  Return NULL, indicating an error.
    -----------------------------------------------------------------------*/
    acb_ptr = NULL;
  }
  INTFREE();

  return (acb_ptr);

} /* dssocki_appidtoacb() */

/*===========================================================================
FUNCTION DSSOCKI_GETACBCOUNT()

DESCRIPTION
    Gets the current number of allocated application control blocks in the 
    system.

DEPENDENCIES
  None.

RETURN VALUE
  Returns number of allocated applications in ACB array.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_getacbcount
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Return the global application count.  
  -------------------------------------------------------------------------*/
  return (dssocki_acb_count);

} /* dssocki_getacbcount() */

/*===========================================================================
FUNCTION DSSOCKI_IS_ADDR_IN_USE()

DESCRIPTION
  Determines if the specified address, in network byte order, is in use for 
  the given protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - The address is in use
  FALSE - The address is not in use

SIDE EFFECTS
  None.
===========================================================================*/
boolean
dssocki_is_addr_in_use 
( 
  byte proto, 
  struct sockaddr_in* laddr 
)
{
  struct scb_s *tmp_scb_ptr;  /* tmp socket control block used for compare */
  int      i;                                          /* local loop index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<DSS_MAX_SOCKS; i++)
  {
    if ( (tmp_scb_ptr=dssocki_indextoscb(i)) != NULL)
    {
      /*---------------------------------------------------------------------
        If sockets are using a different protocol, this is not a problem.
        Just continue.
      ---------------------------------------------------------------------*/
      if (proto != tmp_scb_ptr->protocol)
        continue;

      /*---------------------------------------------------------------------
        If socket has same local address as specified parameter, then we 
        need to check against the local port number.  Note that INADDR_ANY
        will result in the same generated local IP address.
      ---------------------------------------------------------------------*/
      if ( tmp_scb_ptr->localname.address == ntohl(laddr->sin_addr.s_addr) ||
           laddr->sin_addr.s_addr == INADDR_ANY )
      {
        /*-------------------------------------------------------------------
          Ports are unequal, so just continue.
        -------------------------------------------------------------------*/
        if (tmp_scb_ptr->localname.port != ntohs(laddr->sin_port))
        {
          continue;
        }

        /*-------------------------------------------------------------------
          Port and address are equal, so the address may already be in use.
          If the port number is not a wildcard (0), return an error.
        -------------------------------------------------------------------*/
        if (tmp_scb_ptr->localname.port != 0)
        {
          return TRUE;
        }

      } /* if address equal */

    } /* tmp_scb_ptr != NULL */

  } /* for DSS_MAX_SOCKS */

  return FALSE;
}

/*===========================================================================
FUNCTION DSSOCKI_GETEPHEM_PORT()

DESCRIPTION
  Returns an ephemeral port, for the specified protocol, between PORTBASE and 
  PORTBASE+PORTOFFSET after checking that it is not in use by another socket.
  Currently, the port number returned is unique across TCP/UDP and across
  interfaces.

DEPENDENCIES
  None.

RETURN VALUE
  Port number: in network byte order, not in use by another socket
  0: In case there is no ephemeral port available

SIDE EFFECTS
  None.
===========================================================================*/
uint16 dssocki_getephem_port
(
  byte proto      /* protocol for which the caller wants an ephemeral port */
)
{
  struct sockaddr_in tmp_addr;               /* tmp variable to store addr */
  int iter = 0;                               /* number of loop iterations */
  uint16   rand_port;                         /*random port*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the address structure. s_addr is initialized to 0x0 so that
    we return a port number not in use on any interface.
  -------------------------------------------------------------------------*/
  tmp_addr.sin_family = AF_INET;
  tmp_addr.sin_addr.s_addr = dss_htonl(0x0);

  /*-------------------------------------------------------------------------
    Set local port between PORTBASE and PORTBASE+PORTOFFSET, by incrementing 
    port ctr. This is the range of ephemeral port numbers generated when the 
    application has not bound to any particular local port (preferred for
    client applications).  Protect static value through INTLOCK()/INTFREE().
  -------------------------------------------------------------------------*/
  for (iter=0; iter<=PORTOFFSET; iter++)
  {
    rand_port = (uint16)ran_dist( ran_next(), PORTBASE, PORTBASE + PORTOFFSET);
    tmp_addr.sin_port = dss_htons(rand_port);

    if (dssocki_is_addr_in_use(proto, &tmp_addr) == FALSE)
    {
      break;
    }
  }

  if ( iter > PORTOFFSET )
  {
    tmp_addr.sin_port = dss_htons(0x0);
  }

  return (tmp_addr.sin_port);

} /* dssocki_genephem_port() */

/*===========================================================================
FUNCTION DSSOCKI_FCNPTR_SETUP()

DESCRIPTION
  Sets up the protocol specific function pointer table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Function pointer table is assinged.
===========================================================================*/
sint15 dssocki_fcnptr_setup
(
  struct scb_s *scb_ptr,                 /* socket control block structure */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dss_fcn_ptr_table *sp;             /* function pointer table item */

  /*-------------------------------------------------------------------------
    Look for entry in protocol table - break when socket protocol type
    matches the protocol type of the function pointer table.
  -------------------------------------------------------------------------*/
  for(sp=dssock_fcn_ptr_table; sp->type!= DSS_ERROR; sp++)
  {
    if(scb_ptr->type == sp->type)
    {
      break;
    }
  }

  /*-------------------------------------------------------------------------
    End of table, with no entries found - return error.
  -------------------------------------------------------------------------*/
  if(sp->type == DSS_ERROR)
  {
    *dss_errno = DS_ESOCKNOSUPPORT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Entry was found, so assign table in socket control block structure
  -------------------------------------------------------------------------*/
  scb_ptr->fcn_ptr_table = sp;
 
  return (DSS_SUCCESS);
} /* dssocki_fcnptr_setup() */

/*===========================================================================
FUNCTION DSSOCKI_SEND_TO_PS()

DESCRIPTION
  Sends the application's dsm_item data to the PS task, which will send it 
  out using the protocol for the given socket.
  
  This function is called from the context of the application task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_send_to_ps
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type* item_ptr,                    /* Ptr to payload to be sent */
  struct sockaddr_in* ta_ptr,      /* Ptr to structure containing dest info*/
  byte kind                                 /* Type of item to be enqueued */
)
{
  struct sockaddr_in tmp_ta;
  uint32             app_field;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    To send the data to the PS task, we enqueue the pointer to the head of
    the dsm packet chain (item pointer) into the ps_socket_send_q and signal 
    the PS task that there is data available it's the RX queue.  We also 
    need to have access to the socket control block, in the context of the 
    PS task, so we need to send that as well.  We do this, by simply pushing 
    the SCB pointer into the dsm memory item pool as well (using the item 
    pointer).
  -------------------------------------------------------------------------*/
  if( ta_ptr != NULL )
  {
    /*-----------------------------------------------------------------------
      convert ntoh address & port.
    -----------------------------------------------------------------------*/
    tmp_ta.sin_addr.s_addr = dss_ntohl(ta_ptr->sin_addr.s_addr);
    tmp_ta.sin_port = dss_ntohs(ta_ptr->sin_port);
  }
  else
  {
    /*-----------------------------------------------------------------------
      If no destination information is passed in, use 0 for address and port
    -----------------------------------------------------------------------*/
    tmp_ta.sin_addr.s_addr = 0;
    tmp_ta.sin_port = 0;
  }

  /*-----------------------------------------------------------------------
     save app-field for later
  -----------------------------------------------------------------------*/
  app_field = item_ptr->app_field;

  /*-----------------------------------------------------------------------
     pushdown destination IP address/portno
  -----------------------------------------------------------------------*/
  if (dsm_pushdown( &item_ptr,
                    &tmp_ta,
                    sizeof( struct sockaddr_in ),
                    DSM_DS_SMALL_ITEM_POOL) < sizeof( struct sockaddr_in ))
  {
    ERR("Running out of items",0,0,0);
    dsm_free_packet(&item_ptr);
    return;
  }

  /*-----------------------------------------------------------------------
     pushdown address of socket control block
  -----------------------------------------------------------------------*/
  if (dsm_pushdown( &item_ptr, 
		    &scb_ptr, 
		    sizeof( scb_ptr), 
		    DSM_DS_SMALL_ITEM_POOL) < sizeof ( scb_ptr))
  {
    ERR("Running out of items",0,0,0);
    dsm_free_packet(&item_ptr);
    return;
  }

  /*-----------------------------------------------------------------------
     pushdown the socket descriptor
  -----------------------------------------------------------------------*/
  if (dsm_pushdown( &item_ptr, 
		    &(scb_ptr->sockfd), 
		    sizeof(scb_ptr->sockfd), 
		    DSM_DS_SMALL_ITEM_POOL ) < sizeof(scb_ptr->sockfd))
  {
    ERR("Running out of items",0,0,0);
    dsm_free_packet(&item_ptr);
    return;
  }

  item_ptr->app_field = app_field;
  item_ptr->kind = kind;
#ifdef FEATURE_DSM_MEM_CHK
  item_ptr->tracer = DSM_SCKT_TO_PS;
#endif //FEATURE_DSM_MEM_CHK
  q_put( &ps_socket_send_q, &item_ptr->link);
  PS_SET_SIGNAL( PS_SOCKET_RX_Q_SIGNAL);

} /* dssocki_send_to_ps() */


/*===========================================================================
FUNCTION DSSOCKI_COPY_BUF_TO_DSM()

DESCRIPTION
  Copies application data buffer vector to data services memory pool. The
  copying is done intelligently. If the number of bytes to copy is less than 
  the DSSOCKI_SMALL_ITEM_THRESHOLD, the bytes are copied into a small dsm
  item otherwise they are copied into a large dsm item.
  Copies the data from each array item, and concatenates the data into a
  packet chain.  For efficiency, each dsm memory item is completely filled,
  thus a new dsm item for each array item is not allocated.

DEPENDENCIES
  None.

RETURN VALUE
  Total byte count.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_copy_buf_to_dsm
(
  struct iovec *iov,      /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 max_bytes,                     /* maximum number of bytes to copy */
  dsm_item_type **buf_ptr_ptr          /* pointer to dsm item to copy into */
)
{
  dsm_item_type     *item_ptr = NULL;       /* pointer to current dsm item */
  dsm_item_type     *last_ptr = NULL;          /* pointer to last dsm item */
  byte              *data_ptr;                   /* pointer to data buffer */
  byte              *tmp_data_ptr = NULL;      /* temporary scratch buffer */
  int               i;                                       /* loop index */

  sint15            buf_leftover = 0;    /* bytes left over in single item */

  uint16            data_len;   /* length to be copied in current dsm item */
  sint15            length;               /* specified length to be copied */
  sint15            remaining_length = 0; /* remaining bytes to be copied  */
  sint15            byte_count= 0;            /* total returned byte count */
  dsm_mempool_id_enum_type pool_id;  /* the pool to allocate DSM item from */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Ensure that there is at least one iovec array item to copy
  -------------------------------------------------------------------------*/
  ASSERT( iovcount > 0);

  remaining_length   = (sint15) max_bytes;

  /*-------------------------------------------------------------------------
    Loop across all array items, copying the data from each array item, and 
    concatenate the data into a packet chain.  For efficiency, each dsm 
    memory item is completely filled, thus a new dsm item for each array 
    item is not allocated.
  -------------------------------------------------------------------------*/
  for (i=0; i<iovcount  && (remaining_length > 0) ; i++)
  {
    length       = MIN(iov[i].iov_len, remaining_length);
    data_ptr     = iov[i].iov_base;
    
    /*-----------------------------------------------------------------------
      For efficiency determine if there are still free bytes left over in 
      the buffer before allocating a new one.
    -----------------------------------------------------------------------*/
    if (buf_leftover > 0)
    {
      data_len     = (uint16) MIN(length, buf_leftover);
    }
    else
    {
      /*---------------------------------------------------------------------
        No free bytes are left, so allocate a new dsm item. 

        First, determine an appropriate DSM item pool. If the remaining 
        length is more than a threshold then choose large item pool. Small
        item pool, otherwise.
      ---------------------------------------------------------------------*/
      if ( remaining_length > DSSOCKI_SMALL_ITEM_THRESHOLD )
      {
        pool_id = DSM_DS_LARGE_ITEM_POOL;
      }
      else
      {
        pool_id = DSM_DS_SMALL_ITEM_POOL;
      }

      /*---------------------------------------------------------------------
        If the preferred pool does not have sufficient free items left,
        try to use the other pool. If both of them are running out of
        free items, just return the current number of bytes already copied.
      ---------------------------------------------------------------------*/

      if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
      {
        pool_id = DSM_DS_OTHER_POOL_ID(pool_id);

        if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
        {
          ERR(" Running out of buffers", 0 , 0, 0);
          return (byte_count);
        }
      }

      /*---------------------------------------------------------------------
        Copy the minimum of the current buffer length and the size of the 
        retrieved data item.
      ---------------------------------------------------------------------*/

      data_len = (uint16) MIN(length, DSM_POOL_ITEM_SIZE(pool_id));
      MSG_LOW("Using pool %d: %d bytes", (int) pool_id, data_len, 0);
      
      if (last_ptr != NULL)
      {
        /*-------------------------------------------------------------------
          There are already existing items in the packet chain, so create 
          and insert the dsm item.  Note, the call to dsm_new_buffer() will
          never return NULL, if the resource checks (above) work correctly.
        -------------------------------------------------------------------*/
        last_ptr = item_ptr;
        
        if ( (item_ptr = dsm_new_buffer(pool_id)) == NULL )
        {
          ERR(" Running out of buffers", 0 , 0, 0);
          return (byte_count);
        }
        
        /*-------------------------------------------------------------------
          Insert pointer into packet chain 
        -------------------------------------------------------------------*/
        if (last_ptr != NULL)
        {
          last_ptr->pkt_ptr = item_ptr;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          This is the initial item for the packet chain.  Create a new item,
          and then save the head of the packet chain, which will be the 
          return value of this function.
        -------------------------------------------------------------------*/
        if ( (item_ptr = dsm_new_buffer(pool_id)) == NULL )
        {
          ERR("Running out of buffers", 0 , 0, 0);
          return (byte_count);
        }

        last_ptr = item_ptr;
        /*-------------------------------------------------------------------
          Save head of packet chain
        -------------------------------------------------------------------*/
        *buf_ptr_ptr = item_ptr;
      }
    } /* buf_leftover > 0 */
    
    /*-----------------------------------------------------------------------
      Copy data - possibly split over more than one dsm item
    -----------------------------------------------------------------------*/
    while (data_len != 0)
    {
      /*---------------------------------------------------------------------
        Offset into data array to be copied.  Need to assign temp variable,
        as item_ptr->data always needs to be pointing to beginning of dsm
        item.  Ensure (for sanity checking) that item_ptr can never be
        NULL.
      ---------------------------------------------------------------------*/

      ASSERT(item_ptr != NULL);

      tmp_data_ptr        = item_ptr->data_ptr; 
      tmp_data_ptr       += item_ptr->used;
      
      memcpy(tmp_data_ptr, data_ptr, (uint16)data_len);
      item_ptr->used     += (uint16)data_len;
      data_ptr            = (byte *) data_ptr + data_len;
      byte_count         += (sint15) data_len;
      
      /*---------------------------------------------------------------------
        Now calculate the length for the next part of data buffer.  ASSERT
        that more than the size of the dsm item has not been used.
      ---------------------------------------------------------------------*/
      ASSERT( item_ptr->used <= item_ptr->size );
      buf_leftover  = (sint15) ((item_ptr->size - item_ptr->used));
      
      length           -= (sint15) data_len;
      remaining_length -= (sint15) data_len;

      ASSERT(remaining_length >= 0);
      
      /*---------------------------------------------------------------------
        More data to copy
      ---------------------------------------------------------------------*/
      if (buf_leftover == 0 && length > 0)
      {
	/*-------------------------------------------------------------------
          First, determine an appropriate DSM item pool. If the remaining 
          length is more than a threshold then choose large item pool. Small
          item pool, otherwise.
        -------------------------------------------------------------------*/
            
        if ( remaining_length > DSSOCKI_SMALL_ITEM_THRESHOLD )
        {
          pool_id = DSM_DS_LARGE_ITEM_POOL;
        }
        else
        {
          pool_id = DSM_DS_SMALL_ITEM_POOL;
        }

        /*-------------------------------------------------------------------
          If the preferred pool does not have sufficient free items left,
          we try to use the other pool. If both of them are running out of
          free items, just return the current number of bytes already copied.
        -------------------------------------------------------------------*/

        if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
        {
          pool_id = DSM_DS_OTHER_POOL_ID(pool_id);

          if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
          {
            ERR(" Running out of buffers", 0 , 0, 0);
            return (byte_count);
          }
        }

        data_len = (uint16) MIN(length, DSM_POOL_ITEM_SIZE(pool_id));
        MSG_LOW("Using pool %d: %d bytes", (int) pool_id, data_len, 0);

        last_ptr = item_ptr;
        if ((item_ptr = dsm_new_buffer(pool_id)) == NULL)
        {
          ERR("Running out of buffers", 0 , 0, 0);
          return (byte_count);
        }
        /*-------------------------------------------------------------------
          Insert pointer into packet chain 
        -------------------------------------------------------------------*/
        if (last_ptr != NULL)
        {
          last_ptr->pkt_ptr = item_ptr;
        }
      }
      else
      {
        data_len = 0;
      }
      
      /*---------------------------------------------------------------------
        Sanity check - This should never happen.
      ---------------------------------------------------------------------*/
      if ( length < 0)
      {
        ERR_FATAL("Sanity check", 0, 0, 0);
      }
        
    } /* while */
    
  } /* for number of array items */

  return (byte_count);          
  
} /* dssocki_copy_buf_to_dsm() */


/*===========================================================================
FUNCTION DSSOCKI_GENERATE_META_INFO()

DESCRIPTION
  Generates PS meta info packet for TX flags and socket opetions
  set and attaches it to the packet.

DEPENDENCIES
  Packet head pointer has to include all headers above IP.

RETURN VALUE
  DSS_SUCCESS or DSS_ERROR (dss_errno contains cause).

SIDE EFFECTS
  None.
===========================================================================*/
int dssocki_generate_meta_info
(
  dsm_item_type*  pkt_head_ptr,
  scb_type*       scb_ptr,
  uint32          tx_flags,
  sint15*         dss_errno
)
{
  ps_meta_info_type*  meta_info_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( pkt_head_ptr != NULL );
  ASSERT( scb_ptr != NULL );
  ASSERT( dss_errno != NULL );

  PS_META_INFO_GET( meta_info_ptr );

  /*-----------------------------------------------------------------------
    If the get failed return enomem
  -----------------------------------------------------------------------*/
  if ( meta_info_ptr == NULL )
  {
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  /*-----------------------------------------------------------------------
    Set the socket options in the meta info
  -----------------------------------------------------------------------*/
  meta_info_ptr->sock_opts = scb_ptr->options_mask;
  meta_info_ptr->sock_ip_ttl = scb_ptr->ip_ttl;

  /*-----------------------------------------------------------------------
    Set the transmit flags and socket file descriptor
  -----------------------------------------------------------------------*/
  meta_info_ptr->tx_flags = tx_flags;
  meta_info_ptr->sockfd  = scb_ptr->sockfd;
  memset( &(meta_info_ptr->policy_info), 
          0, 
          sizeof( meta_info_ptr->policy_info ) );
  meta_info_ptr->policy_info.orig_from_sock = TRUE;

  /*-----------------------------------------------------------------------
    Set the routing cache information in the meta info
  -----------------------------------------------------------------------*/
  meta_info_ptr->routing_cache = (void*) scb_ptr->routing_cache;

  /*-----------------------------------------------------------------------
    Set the routing cache information in the meta info
  -----------------------------------------------------------------------*/
  meta_info_ptr->ipsec_required = (scb_ptr->ipsec_hdr_size > 0);

  /*-----------------------------------------------------------------------
    Book-keeping of the meta info item
  -----------------------------------------------------------------------*/
  pkt_head_ptr->app_field = (uint32) meta_info_ptr;

  return DSS_SUCCESS;

} /* dssocki_generate_meta_info() */


/*===========================================================================
FUNCTION DSSOCKI_SET_SOCK_STATE()

DESCRIPTION
  This function sets the socket state. It can be NULL, INIT, OPENING, OPEN, 
  CLOSING, WAIT_FOR_CLOSE, or CLOSED.
  
  NULL: Is a pseudo-state, that is set prior to initial socket
           creation.  A NULL socket cannot be accessed by any API
           function.

  INIT: Is specific to TCP, and is set when a TCP socket is first
           created, through a call to socket().  Note this is an
           invalid state for UDP sockets.

  LISTEN: This socket it listening.  This is specific to TCP, as such
           is an invalid state for UDP.  accept() may be called on
           this socket.
  
  OPENING: the socket is not writable and read() and write() would
           return DS_EWOULDBLOCK.
           A connected socket is in the OPENING state after dss_connect() 
           has been called and before the socket is completly connected.
           A connectionless socket is never in this state.
  
  OPEN:    means the socket is writable if there is enough memory available.
           A connected socket is in the OPEN state when TCP is in the 
           ESTABLISHED state.
           A connectionless socket is in OPEN state in the dss_socket() 
           call.
  
  CLOSED:  means the socket is writable and readable  and read() and write() 
           would return DS_ECONNABORTED, DS_ECONNRESET, DS_IPADDRCHANGED or
           DS_ENETDOWN. A connected socket is in the CLOSED state after it 
           lost its connection to the server or the network connectivity was
           lost any time after the dss_connect() was called. 
           A connectionless socket is in this state, only when the network
           subsystem has experienced a failure.
  
  CLOSING: means that a TCP socket has issued a close(), but the TCP close
           is still in progress.  The initial call to close() for TCP 
           sockets would return DS_EWOULDBLOCK.  This corresponds to the 
           active close, and the socket is waiting for TCP to fully 
           negotiate the close.
           This state is invalid for connectionless sockets.

  WAIT_FOR_CLOSE:  means that TCP has fully closed, or the TCP close timer
                   has expired.  In this state, the application may call
                   close() again to clean up the socket and free it for
                   re-use.
                   This state is invalid for connectionless sockets.

  This should be called whenever the state changes and is the only function
  which modifies the socket_state member of the struct scb_s.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_set_sock_state
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dssocki_state_enum_type new_state                        /* Socket state */
)
{
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Change socket state to the new state.  Protect access to since the
    SCB socket state can be modified through another task.
  -------------------------------------------------------------------------*/
  MSG_MED("Sockfd %d oldstate=%d newstate=%d",
          scb_ptr->sockfd,
          scb_ptr->socket_state,
          new_state);
  INTLOCK();
  scb_ptr->socket_state = new_state;
  switch ( new_state )
  {
    case DSSOCKI_NULL:
    case DSSOCKI_INIT:
    case DSSOCKI_LISTEN:
    case DSSOCKI_OPENING:
    case DSSOCKI_OPEN:
    case DSSOCKI_CLOSING:
      /*---------------------------------------------------------------------
        Nothing special needs to be done
      ---------------------------------------------------------------------*/
      break;

    case DSSOCKI_WAIT_FOR_CLOSE:
    case DSSOCKI_CLOSED:
      /*---------------------------------------------------------------------
        If the so_linger option is set, cancel the linger_timer
      ---------------------------------------------------------------------*/
      if ( IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_LINGER) )
      {
        timer_ret_val = ps_timer_cancel(scb_ptr->linger_timer);
        ASSERT(timer_ret_val != PS_TIMER_FAILURE);
      }
      break;
  }
  INTFREE();

  /*-------------------------------------------------------------------------
    Do realtime check for socket events for the scb.
  -------------------------------------------------------------------------*/
  dssocki_socket_event_occurred(scb_ptr, NULL);

} /* dssocki_set_sock_state() */

/*===========================================================================
FUNCTION DSSOCKI_NET_EVENT_NOTIFY()

DESCRIPTION
  This function notifies the application that the network event of interest
  has occurred. If the passed parameter is NULL then it notifies all the 
  opened   applications. If passed parameter is a valid one, it notifies
  only that application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_net_event_notify
(
    acb_type* curr_acb_ptr
)
{
  acb_type* acb_ptr;                              /* App Ctl block pointer */
  int       index;                                           /* loop index */
  sint15    app_id;                                      /* application ID */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if the currenet acb_ptr is NULL. If yes, then inform all the 
    Applications about the network event.
  -------------------------------------------------------------------------*/
  if(curr_acb_ptr == NULL)
  { 

    /*-----------------------------------------------------------------------
      Loop through acb array to find which acb is waiting for a network event
      Protect access to global array through use of INTLOCK()/INTFREE().
    -----------------------------------------------------------------------*/
     for (index=0; index<DSS_MAX_APPS; index++)
     {
       INTLOCK();
       acb_ptr = &dssocki_acb_array[index];
       INTFREE();

       /*--------------------------------------------------------------------
         If the app. is waiting for a net event and is valid app_id then, 
         call the callback function.
       --------------------------------------------------------------------*/
       if( acb_ptr->app_id != APPAVAIL) 
       { 
         if (acb_ptr->net_callback_fcn != NULL)
          {
            app_id = acb_ptr->app_id;
            acb_ptr->net_callback_fcn(&app_id);
          }
          else
          {
            MSG_MED("Net cback null, so not called",0,0,0);
          }
       }
      } /* end of the for ()                                               */
  }
  /*-------------------------------------------------------------------------
    Since curr_acb_ptr != NULL, inform the application of the network event
    if the network event notification flag is TRUE.
  -------------------------------------------------------------------------*/
  else
  {
    if( (curr_acb_ptr->net_callback_fcn != NULL)  &&
        (curr_acb_ptr->app_id != APPAVAIL) )
    {
      app_id = curr_acb_ptr->app_id;
      curr_acb_ptr->net_callback_fcn(&app_id);
    }
    else
    {
       MSG_MED("Net cback/app_id null, so not called",0,0,0);
    }

  }

} /* dssocki_net_event_notify() */

/*===========================================================================

FUNCTION DSSOCKI_SOCKET_EVENT_CHECK()

DESCRIPTION
  This function performs a real-time check, to determine if any of the socket
  events of interest have occurred.  If so, it returns the event mask of the'
  events that have occurred.
  
DEPENDENCIES
  None.

RETURN VALUE
  sint31 event mask of the socket events that have occurred.

SIDE EFFECTS
  None.

===========================================================================*/
sint31 dssocki_socket_event_check
( 
  struct scb_s* scb_ptr,          /* ptr to socket control block structure */
  sint31 requested_event_mask        /* Events the socket is interested in */
)
{
  sint31  event_mask = 0;    /* returned mask of events that have occurred */
  ps_iface_type *ps_iface_ptr;           /* Ptr to iface for this socket   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check to see if the write event has occurred.  If so, set it in the 
    returned event mask.
    WRITE event is TRUE when dss_write/dss_sendto, if called, will return
    either SUCCESS or an error other than EWOULDBLOCK.
    WRITE event is FALSE when dss_write/dss_sendto, if called, will return
    EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if( (requested_event_mask & DS_WRITE_EVENT) == DS_WRITE_EVENT)
  {
    ps_iface_ptr = scb_ptr->routing_cache;

    /*----------------------------------------------------------------------
      If CANTSENDMORE flag is set or if the socket is in closed state then 
      we need not check anything more. A write call is bound to return an 
      error other than EWOULDBLOCK if either of either of these conditions 
      is met.
    ----------------------------------------------------------------------*/
    if ( (scb_ptr->state_flags & DSSOCKI_CANTSENDMORE) ||
         (scb_ptr->socket_state == DSSOCKI_CLOSED) )
    {
      event_mask |= DS_WRITE_EVENT;
    }
    /*----------------------------------------------------------------------
      Check if the interface that this socket is using is flow controlled 
    ----------------------------------------------------------------------*/
    else if ( PS_IFACE_IS_VALID( ps_iface_ptr) &&
         !PS_IFACE_FLOW_ENABLED(ps_iface_ptr) )
    {
      MSG_HIGH( "Socket %d: if%d is flow disabled", 
                scb_ptr->sockfd, 
                ps_iface_ptr->name, 
                0 ); 
    }
    /*----------------------------------------------------------------------
      Check if this is non-system socket and is flow controlled
    ----------------------------------------------------------------------*/
    else if( PS_IFACE_IS_VALID(ps_iface_ptr) &&
             !IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_SYS_SOCK) &&
             !PS_IFACE_NON_SYS_SOCKETS_FLOW_ENABLED(ps_iface_ptr) )
    {
      MSG_HIGH("Non-sys sockfd %d is blocked", scb_ptr->sockfd, 0, 0);
    }
    /*----------------------------------------------------------------------
      If the mobile is out of memory then WRITE event is FALSE and app 
      should get EWOULDBLOCK.
    ----------------------------------------------------------------------*/
    else if( DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_LARGE_ITEM_POOL) &&
             DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) ) 
    {
      MSG_HIGH( "NO dsm items, small=%d large=%d",
                DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
                0 );
    }
    /*----------------------------------------------------------------------
      A socket in init state is writeable only when the above checks pass.

      Note that if the call is dormant and CONNECT is being done on a new 
      TCP socket, i.e., the socket is in INIT state, connect() will be 
      called a few times by app. The first call will cause the re-orig, the 
      second will actually cause the connect to occur and the third will
      provide the connect attempt result. The first connect will keep the 
      TCP socket in the INIT state, but it does need to look for a write 
      event, and so check for INIT state also.
    ----------------------------------------------------------------------*/
    else if (scb_ptr->socket_state == DSSOCKI_INIT)
    {
      event_mask |= DS_WRITE_EVENT;
    }
    /*-----------------------------------------------------------------------
      If connected, make the socket writeable only if there is enough memory 
      available and the socket is writeable. 
    -----------------------------------------------------------------------*/
    else if ( scb_ptr->socket_state == DSSOCKI_OPEN )
    {
      if ((scb_ptr->fcn_ptr_table)->is_writeable != NULL &&
	  ((scb_ptr->fcn_ptr_table)->
	   is_writeable(scb_ptr->protocol_ctl_blk) == 0))
      {
        MSG_HIGH("Socket %d has enough data queued", scb_ptr->sockfd, 0, 0); 
      }
      else
      {
        /*------------------------------------------------------------------
          Everything's okay, enabling write event. 
        ------------------------------------------------------------------*/
        event_mask |= DS_WRITE_EVENT;
      }
    } /* if socket_state == DSSOCKI_OPEN */
  } /* if DS_WRITE_EVENT */

  /*-------------------------------------------------------------------------
    Check to see if the read event has occurred, making the socket readable.
    A socket is readable when socket_state is OPEN and there is data to be
    read or when the socket was CLOSED by the other side.
  -------------------------------------------------------------------------*/
  if( (requested_event_mask & DS_READ_EVENT) == DS_READ_EVENT)
  {
    if (((scb_ptr->socket_state   == DSSOCKI_OPEN ||
          scb_ptr->socket_state   == DSSOCKI_CLOSING)  &&
          scb_ptr->data_available == TRUE &&
          !(scb_ptr->state_flags & DSSOCKI_CANTRECVMORE)) ||

        (scb_ptr->state_flags & DSSOCKI_CANTRECVMORE)  ||

        (scb_ptr->socket_state == DSSOCKI_INIT) || 

        (scb_ptr->socket_state == DSSOCKI_CLOSED) ||
        (scb_ptr->socket_state == DSSOCKI_WAIT_FOR_CLOSE) ||
        (scb_ptr->socket_state == DSSOCKI_LISTEN))
    {
      /*---------------------------------------------------------------------
        Conditions are true - socket is readable - set event in event_mask.
      ---------------------------------------------------------------------*/
      event_mask |= DS_READ_EVENT;      

    } /* if readable */

  } /* if DS_READ_EVENT */

  /*------------------------------------------------------------------------
    Check to see if the accept event has occured, makeing the socket
    accpetable.  A socket is acceptable when socket_state is LISTEN
    and there is data to be read.
  ------------------------------------------------------------------------*/
  if ( (requested_event_mask & DS_ACCEPT_EVENT) == DS_ACCEPT_EVENT )
  {
    if ( ((scb_ptr->socket_state == DSSOCKI_LISTEN) &&
         ((scb_ptr->data_available == TRUE) || 
          (scb_ptr->so_partial == NULL))) ||
          (scb_ptr->socket_state == DSSOCKI_CLOSED) ||
          (scb_ptr->socket_state == DSSOCKI_INIT))    
    {
      event_mask |= DS_ACCEPT_EVENT;
    } /* if acceptable */ 
  }

  /*-------------------------------------------------------------------------
    Check to see if the close event has occurred, making the socket 
    closeable.  A socket is closeable when socket_state is in WAIT_FOR_CLOSE
    or CLOSED or if the socket is lingering.
  -------------------------------------------------------------------------*/
  if( (requested_event_mask & DS_CLOSE_EVENT) == DS_CLOSE_EVENT)
  {
    if ( scb_ptr->socket_state == DSSOCKI_WAIT_FOR_CLOSE ||
         scb_ptr->socket_state == DSSOCKI_CLOSED ||
         (scb_ptr->state_flags & DSSOCKI_LINGER) )
    {
      /*---------------------------------------------------------------------
        Conditions are true - socket is closeable - set event in event_mask
      ---------------------------------------------------------------------*/
      event_mask |= DS_CLOSE_EVENT;      

    } /* if closeable */

  } /* if DS_CLOSE_EVENT */

  return (event_mask);

} /* dssocki_socket_event_check() */

/*===========================================================================
FUNCTION DSSOCKI_NOTIFY_APP()

DESCRIPTION
  This function calls the application callback function to alert the
  application that a socket event has occurred.  Note that the application
  callback will not be called if the application has already been notified.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The application callback function is invoked.
===========================================================================*/
void dssocki_notify_app
( 
  acb_type *acb_ptr                    /* ptr to application control block */
)
{
  boolean notified_cb_flag;  /* indicates if socket cb fcn has been called */
  sint15 app_id;                                         /* application ID */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_LOW("Notify App Called", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Get flag which indicates whether or not the application has been
    notified or not.  Since this is part of a global structure, need to 
    protect access to it through INTLOCK()/INTFREE().
  -------------------------------------------------------------------------*/
  INTLOCK();
  notified_cb_flag = acb_ptr->notified_cb_flag;
  INTFREE();

  /*-------------------------------------------------------------------------
     Application has not yet been notified
  -------------------------------------------------------------------------*/
  if (notified_cb_flag == FALSE)
  {
    /*-----------------------------------------------------------------------
      Change status of application notification, again protecting through
      INTLOCK()/INTFREE().
    -----------------------------------------------------------------------*/
    INTLOCK();
    acb_ptr->notified_cb_flag = TRUE;
    INTFREE();

    /*-----------------------------------------------------------------------
      Call the callback function
    -----------------------------------------------------------------------*/
    if (acb_ptr->socket_callback_fcn != NULL)
    {
      app_id = acb_ptr->app_id;
      acb_ptr->socket_callback_fcn(&app_id);
    }
    else
    {
      MSG_MED("Socket cback null, so not called",0,0,0);
    }
    
  }
} /* dssocki_notify_app() */

/*===========================================================================
FUNCTION DSSOCKI_SOCKET_EVENT_OCCURRED()

DESCRIPTION
  This function notifies all interested sockets that a socket event
  has occurred.  If a scb_ptr is specified, this function notifies only that
  socket. If scb_ptr is NULL and acb_ptr is specified, this function notifies
  all sockets for that app. If both scb_ptr and acb_ptr are not specified, 
  the function assumes it should provide notification for all sockets. The
  function then loops through the socket control block array and checks each
  that may have occurred.  The application is notified upon any events
  having occurred.  If a valid scb_ptr is specified, the function checks
  that particular socket for any events that may have occurred.  Again,
  the application is notified if any of the events that it is interested
  in, have occurred.

DEPENDENCIES

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_socket_event_occurred
( 
  struct scb_s *scb_ptr,                    /* ptr to socket control block */
  acb_type *acb_ptr                         /* ptr to app control block    */
)
{
  int      index;                                            /* loop index */
  word int_save;         /* Local Variable used for saving interrupt state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    A NULL scb_ptr indicates that we should notify across all sockets.  
    In other words, it is a global event.
   ------------------------------------------------------------------------*/
  if ((scb_ptr == NULL)  && (acb_ptr == NULL))
  {
    /*-----------------------------------------------------------------------
      Loop through SCB array - Protect access to global array through use
      of INTLOCK()_SAV/INTFREE_SAV().
     ----------------------------------------------------------------------*/
    for (index=0; index<DSS_MAX_SOCKS; index++)
    {

      INTLOCK_SAV(int_save);
      if (scb_array[index].sockfd == SOCKAVAIL)
      {
        INTFREE_SAV(int_save);
        continue;
      }
      scb_ptr = &scb_array[index];
      INTFREE_SAV(int_save);

      /*---------------------------------------------------------------------
        Check for events and notify application 
      ---------------------------------------------------------------------*/
      if ( dssocki_socket_event_check(scb_ptr, scb_ptr->requested_event_mask) )
      {
        dssocki_notify_app(scb_ptr->acb_ptr);
      }

    } /* for index */

  } /* scb_ptr == NULL && acb_ptr == NULL */
  else if (scb_ptr != NULL) /* Also covers the case where acb_ptr != NULL */
  {
    /*-----------------------------------------------------------------------
      Check for events and notify application 
    -----------------------------------------------------------------------*/
    if ( dssocki_socket_event_check(scb_ptr, scb_ptr->requested_event_mask) )
    {
      dssocki_notify_app(scb_ptr->acb_ptr);
    }

  } /* scb_ptr != NULL */
  else if(acb_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Notify sockets for specified app.
      Loop through SCB array - Protect access to global array through use
      of INTLOCK_SAV()/INTFREE_SAV().
     ----------------------------------------------------------------------*/
    for (index=0; index<DSS_MAX_SOCKS; index++)
    {

      INTLOCK_SAV(int_save);
      if ((scb_array[index].sockfd == SOCKAVAIL) ||
          (scb_array[index].acb_ptr->app_id != acb_ptr->app_id))
      {
        INTFREE_SAV(int_save);
        continue;
      }

      scb_ptr = &scb_array[index];
      INTFREE_SAV(int_save);

      /*---------------------------------------------------------------------
        Check for events and notify application
      ---------------------------------------------------------------------*/
      if (dssocki_socket_event_check(scb_ptr, scb_ptr->requested_event_mask))
      {
        MSG_MED("Notifying app %d",acb_ptr->app_id,0,0);
        dssocki_notify_app(scb_ptr->acb_ptr);
      }
      /*---------------------------------------------------------------------
        Bail out after specified application has been notified
      ---------------------------------------------------------------------*/
      break;

    } /* for index */
  } /* acb_ptr != NULL */
  else
  {
    /*-----------------------------------------------------------------------
      Both scb_ptr and acb_ptr are NULL! 
    -----------------------------------------------------------------------*/
    ASSERT(0);
  }

} /* dssocki_socket_event_occured() */

/*===========================================================================
FUNCTION DSSOCKI_SOCK_SET_STATE_CLOSED()

DESCRIPTION
  This function sets the socket state to DSSOCKI_CLOSED, however, only if 
  the current socket state is set to DSSOCKI_OPEN or DSSOCKI_OPENING.  If a 
  NULL parameter is passed in, it will change all DSSOCKI_OPEN or 
  DSSOCKI_OPENING sockets to DSSOCKI_CLOSED.  Note this is only called
  from the network state routines when the network subsystem has gone
  down.  This sets the closed_state_reason to DS_ENETDOWN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets socket state to DSSOCKI_CLOSED.
===========================================================================*/
void  dssocki_sock_set_state_closed
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
)
{
  boolean  invalid_ptr_flag;     /* flag indicating invalid SCB array item */  
  int      index;                                            /* loop index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*-------------------------------------------------------------------------
    A NULL scb_ptr indicates that we should close all sockets.  
    In other words, it is a global close.
   ------------------------------------------------------------------------*/
   if (scb_ptr == NULL) 
   {
    /*-----------------------------------------------------------------------
      Loop through SCB array - Protect access to global array through use
      of INTLOCK()/INTFREE().
     ----------------------------------------------------------------------*/
     for (index=0; index<DSS_MAX_SOCKS; index++)
     {
       invalid_ptr_flag = FALSE;
 
       INTLOCK();
       if (scb_array[index].sockfd == SOCKAVAIL)
       {
	 invalid_ptr_flag = TRUE;
       }
       scb_ptr = &scb_array[index];
       INTFREE();
 
       /*--------------------------------------------------------------------
         Not a valid array item - continue
         Used to get around the way INTLOCK()/INTFREE() are currently set up
       --------------------------------------------------------------------*/
       if (invalid_ptr_flag == TRUE) 
       {
         continue;
       }
       /*--------------------------------------------------------------------
         Check state and change if necessary - This is tail recursion.
         This can only happend in the case where scb_ptr is null, and
         since this makes the call with scb_ptr != NULL the call can
         only go 1 deep.  This avoid having all the code that is in
         the next section
         duplicated.
       --------------------------------------------------------------------*/
       dssocki_sock_set_state_closed(scb_ptr);
     } /* for index */
 
   } /* scb_ptr == NULL */
   else
   {
     /*----------------------------------------------------------------------
       Individual SCB - check state and change if necessary
     ----------------------------------------------------------------------*/
     if (scb_ptr->socket_state == DSSOCKI_OPEN ||
	 scb_ptr->socket_state == DSSOCKI_LISTEN ||
         scb_ptr->socket_state == DSSOCKI_OPENING)
     {
       dssocki_set_closed_state_reason(scb_ptr, DS_ENETDOWN);
       dssocki_set_sock_state(scb_ptr, DSSOCKI_CLOSED);
     }
 
   } /* scb_ptr != NULL */

} /* dssocki_sock_set_state_closed() */

/*===========================================================================
FUNCTION DSSOCKI_SET_CLOSED_STATE_REASON()

DESCRIPTION
  Sets the closed_state_reason field in the SCB with the appropriate
  reason for the socket transitioning into the CLOSED state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the closed_state_reason field.
===========================================================================*/
void  dssocki_set_closed_state_reason
(
  struct scb_s *scb_ptr,          /* ptr to socket control block structure */
  sint15   reason                /* reason for socket tranistion to CLOSED */
)
{
  /*-------------------------------------------------------------------------
     set the error condition - SCB is a part of a global array, so
     need to protect access through INTLOCK()/INTFREE().
   ------------------------------------------------------------------------*/  
  INTLOCK();
  scb_ptr->closed_state_reason = reason;
  INTFREE();
} /* dssocki_set_closed_state_reason() */

/*===========================================================================
FUNCTION DSSOCKI_GET_ERR_VAL()

DESCRIPTION
  Gets the appropriate error value based on current socket state and 
  type of operation (connect(), close(), read(), write()).  This function
  follows the TCP/UDP socket state machines.  

DEPENDENCIES
  None.

RETURN VALUE
  Error condition.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_get_err_val
(
  struct scb_s *scb_ptr,               /* ptr to socket control block type */
  dssocki_caller_enum_type type,  /* type of function that is calling this
                                      function.                            */
  sint15 *dss_errno                               /* error condition value */
)
{
  sint15 result;                                           /* return value */
  dssocki_state_enum_type state;                   /* current socket state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  ASSERT(dss_errno != NULL);
  *dss_errno = 0;

  result = DSS_ERROR;

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code - SCB is part of 
    global array, so need to protect access through INTLOCK()/INTFREE().
  -------------------------------------------------------------------------*/
  INTLOCK();
  state = scb_ptr->socket_state;
  INTFREE();
  switch (state)
  {
    /*-----------------------------------------------------------------------
      This should never happen.  NULL socket state implies that the socket
      has not been allocated.
    -----------------------------------------------------------------------*/
    case DSSOCKI_NULL:
      ERR_FATAL("Invalid State for sockfd %d", scb_ptr->sockfd, 0, 0);
      break;

    /*-----------------------------------------------------------------------
      Init state - ASSERT protocol type, since it is only valid for TCP.
    -----------------------------------------------------------------------*/
    case DSSOCKI_INIT:
      ASSERT (scb_ptr->type == SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:       
          /*-----------------------------------------------------------------
            Operation would block while TCP opens
          -----------------------------------------------------------------*/
          *dss_errno = DS_EWOULDBLOCK;
          break;

        case DSSOCKI_CLOSE_ROUTINE:
          /*-----------------------------------------------------------------
            Closed - Go back to NULL.
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_INPUT_ROUTINE: 
        case DSSOCKI_OUTPUT_ROUTINE: 
          /*-----------------------------------------------------------------
            Socket has never connected.
          -----------------------------------------------------------------*/
          *dss_errno = DS_ENOTCONN;
          break;
	
        case DSSOCKI_BIND_ROUTINE:
          /*-----------------------------------------------------------------
	    Operation ok. 
          -----------------------------------------------------------------*/
	  result = DSS_SUCCESS;
	  break;
	  
	case DSSOCKI_LISTEN_ROUTINE:
          /*-----------------------------------------------------------------
	    Operation ok. 
          -----------------------------------------------------------------*/
	  result = DSS_SUCCESS;
	  break;

	case DSSOCKI_ACCEPT_ROUTINE:
	  /*-----------------------------------------------------------------
	    Socket is not listening
	  -----------------------------------------------------------------*/
	  *dss_errno = DS_EINVAL;
	  break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          /*lint -e506, -e774 */
          ASSERT(0);
          /*lint +e506, +e774 */

      } /* switch (type) */

      break; /* DSSOCKI_INIT */

    /*-----------------------------------------------------------------------
      Listen state - Only valid for TCP - Passive Open
    -----------------------------------------------------------------------*/
    case DSSOCKI_LISTEN:
      ASSERT (scb_ptr->type == SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:    
	  *dss_errno = DS_EINVAL; 
          break;
        case DSSOCKI_CLOSE_ROUTINE: 
	  result = DSS_SUCCESS;
	  break;
        case DSSOCKI_INPUT_ROUTINE:
        case DSSOCKI_OUTPUT_ROUTINE:
	  *dss_errno = DS_EINVAL;
          break;
	case DSSOCKI_BIND_ROUTINE:
	  *dss_errno = DS_EINVAL;
	  break;
	case DSSOCKI_LISTEN_ROUTINE:
	  *dss_errno = DS_EINVAL;
	  break;
	case DSSOCKI_ACCEPT_ROUTINE:
	  *dss_errno = DS_EWOULDBLOCK;
	  break;
	case DSSOCKI_SHUTDOWN_ROUTINE:
	  *dss_errno = DS_ENOTCONN;
	  break;
        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          /*lint -e506, -e774 */
          ASSERT(0);
          /*lint +e506, +e774 */

      } /* switch (type) */
      break; /* DSSOCKI_LISTEN */ 
    
    /*-----------------------------------------------------------------------
      Opening state - Only valid for TCP - Connection attempt in progress - 
    -----------------------------------------------------------------------*/
    case DSSOCKI_OPENING:
      ASSERT (scb_ptr->type == SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:    
          /*-----------------------------------------------------------------
            TCP connection is already in progress.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINPROGRESS;
          break;

        case DSSOCKI_CLOSE_ROUTINE:  
        case DSSOCKI_INPUT_ROUTINE:
        case DSSOCKI_OUTPUT_ROUTINE:
          /*-----------------------------------------------------------------
            The operations would block.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EWOULDBLOCK;
          break;

        case DSSOCKI_BIND_ROUTINE:
	case DSSOCKI_LISTEN_ROUTINE:
	case DSSOCKI_ACCEPT_ROUTINE:
	  /*-----------------------------------------------------------------
	    Can't accept or listen on a connected socket. 
          -----------------------------------------------------------------*/
	  *dss_errno = DS_EINVAL; 
	  break;
        
        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          /*lint -e506, -e774 */
          ASSERT(0);
          /*lint +e506, +e774 */

      } /* switch (type) */

      break; /* DSSOCKI_OPENING */

    /*-----------------------------------------------------------------------
      Socket is OPEN - For TCP, closing would issue and DS_EWOULDBLOCK, 
      whereas for UDP, returns immediately with success.
    -----------------------------------------------------------------------*/
    case DSSOCKI_OPEN:
      switch(type)
      {

        case DSSOCKI_CONNECT_ROUTINE:
          if (scb_ptr->type == SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              TCP is already connected.
            ---------------------------------------------------------------*/
            *dss_errno = DS_EISCONN;
          }
          else
          {
            /*---------------------------------------------------------------
              UDP always successful.
            ---------------------------------------------------------------*/
            result = DSS_SUCCESS;                 
          }
          break;


        case DSSOCKI_CLOSE_ROUTINE:
          if (scb_ptr->type == SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              TCP initiates an active close.
            ---------------------------------------------------------------*/
            *dss_errno = DS_EWOULDBLOCK;
          }
          else
          {
            /*---------------------------------------------------------------
              UDP closes immediately
            ---------------------------------------------------------------*/
            result = DSS_SUCCESS;       
          }
          break;

        case DSSOCKI_INPUT_ROUTINE:
          if (scb_ptr->state_flags & DSSOCKI_CANTRECVMORE )
          {
            *dss_errno = DS_EEOF;
          }
          else
          {
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_OUTPUT_ROUTINE:
          /*-----------------------------------------------------------------
            If CANTSENDMORE flag is set then it is an error to try to send
            data.
          -----------------------------------------------------------------*/
          if (scb_ptr->state_flags & DSSOCKI_CANTSENDMORE )
          {
            *dss_errno = DS_ESHUTDOWN;
          }
          else
          {
            result = DSS_SUCCESS;
          }
          break;

	case DSSOCKI_BIND_ROUTINE:
	  if (scb_ptr->type == SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              TCP is already connected.
            ---------------------------------------------------------------*/
            *dss_errno = DS_EINVAL;
          }
          else
          {
            /*---------------------------------------------------------------
              UDP always successful.
            ---------------------------------------------------------------*/
            result = DSS_SUCCESS;                 
          }
	  break;

	case DSSOCKI_LISTEN_ROUTINE:
	case DSSOCKI_ACCEPT_ROUTINE:
	  /*-----------------------------------------------------------------
	    Can't listen or accept on an open socket. 
          -----------------------------------------------------------------*/
	  *dss_errno = DS_EINVAL;
	  break;

	case DSSOCKI_SHUTDOWN_ROUTINE:
	  /*-----------------------------------------------------------------
	    shutdown() is permitted only for connected sockets
          -----------------------------------------------------------------*/
          if (scb_ptr->type == SOCK_STREAM)
          {
            result = DSS_SUCCESS;
          }
          else
          {
            if((scb_ptr->servname.address != 0) &&
               (scb_ptr->servname.port    != 0))
            {
              *dss_errno = DSS_SUCCESS;
            }
            else
            {
              *dss_errno = DS_ENOTCONN;
            }
          }
	  break;

        default:
        /*-------------------------------------------------------------------
          Sanity Check - This should never happen.
        -------------------------------------------------------------------*/
          /*lint -e506, -e774 */
          ASSERT(0);
          /*lint +e506, +e774 */

      } /* switch (type) */

      break; /* DSSOCKI_OPEN */


    case DSSOCKI_CLOSED:
    /*-----------------------------------------------------------------------
      Socket is closed - no TCP connection exists or network went down
    -----------------------------------------------------------------------*/
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:     
          if (scb_ptr->type == SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              Determine connection failure status.
            ---------------------------------------------------------------*/
            *dss_errno = dsstcp_get_conn_error(scb_ptr,
                                               DSSOCKI_CONNECT_ROUTINE);
          }
          else
          {
            /*---------------------------------------------------------------
              UDP is only closed through network failure.
            ---------------------------------------------------------------*/
            *dss_errno = DS_ENETDOWN;
          }
          break;

        case DSSOCKI_CLOSE_ROUTINE:                            
          /*-----------------------------------------------------------------
            No error.           
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_INPUT_ROUTINE:
        case DSSOCKI_OUTPUT_ROUTINE:
          if (scb_ptr->type == SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              I/O-specific connection failure status.
            ---------------------------------------------------------------*/
            *dss_errno = dsstcp_get_conn_error(scb_ptr, type);
          }
          else
          {
            /*---------------------------------------------------------------
              UDP is only closed through network failure.
            ---------------------------------------------------------------*/
            *dss_errno = DS_ENETDOWN;     
          }
          break;

	case DSSOCKI_BIND_ROUTINE:
	case DSSOCKI_LISTEN_ROUTINE:
	case DSSOCKI_ACCEPT_ROUTINE:
	  /*-----------------------------------------------------------------
	    Can't listen or accept on a closed socket. 
          -----------------------------------------------------------------*/
	  *dss_errno = DS_EINVAL;
	  break;

	case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
        /*-------------------------------------------------------------------
          Sanity Check - This should never happen.
        -------------------------------------------------------------------*/
          /*lint -e506, -e774 */
          ASSERT(0);
          /*lint +e506, +e774 */

      } /* switch (type) */

      break; /* DSSOCKI_CLOSED */

    /*-----------------------------------------------------------------------
      CLOSING state reached by a close() issued on TCP socket - This state
      is only valid for TCP sockets, thus ASSERT socket state.
    -----------------------------------------------------------------------*/
    case DSSOCKI_CLOSING:
      ASSERT (scb_ptr->type == SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:    
          /*-----------------------------------------------------------------
            No connection.
          -----------------------------------------------------------------*/
          *dss_errno = DS_ENOTCONN;
          break;

        case DSSOCKI_CLOSE_ROUTINE:      
          /*-----------------------------------------------------------------
            Waiting for TCP to fully close.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EWOULDBLOCK;
          break;

        case DSSOCKI_INPUT_ROUTINE:    
          /*-----------------------------------------------------------------
            If cantrecvmore flag is not set, it is okay to read, otherwise
            return EEOF.
          -----------------------------------------------------------------*/
          if (scb_ptr->state_flags & DSSOCKI_CANTRECVMORE)
          {
            *dss_errno = DS_EEOF;
          }
          else
          {
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_OUTPUT_ROUTINE:    
          /*-----------------------------------------------------------------
            Whether we reached CLOSING state due to shutdown() or close() 
            call, it is an error to try to write() because TCP has sent a
            FIN to the peer indicating that it won't send any more data
          -----------------------------------------------------------------*/
          *dss_errno = DS_ESHUTDOWN;
          break;

	case DSSOCKI_BIND_ROUTINE:
	case DSSOCKI_LISTEN_ROUTINE:
	case DSSOCKI_ACCEPT_ROUTINE:
	  /*-----------------------------------------------------------------
	    Can't listen or accept on a closed socket. 
          -----------------------------------------------------------------*/
	  *dss_errno = DS_EINVAL;
	  break;

	case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ESHUTDOWN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          /*lint -e506, -e774 */
          ASSERT(0);
          /*lint +e506, +e774 */

      } /* switch (type) */

    break; /* DSSOCKI_CLOSING */

    /*-----------------------------------------------------------------------
      WAIT_FOR_CLOSE state reached by a close() issued on TCP socket - 
      TCP has fully closed, and is waiting for application to make another 
      call to close() for clean-up.  This state is only valid for TCP
      sockets, thus ASSERT socket state.
    -----------------------------------------------------------------------*/
    case DSSOCKI_WAIT_FOR_CLOSE:
      ASSERT (scb_ptr->type == SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:                     
          /*-----------------------------------------------------------------
            No connection.
          -----------------------------------------------------------------*/
          *dss_errno = DS_ENOTCONN;
          break;

        case DSSOCKI_CLOSE_ROUTINE:           /* no error - just cleans up */
          /*-----------------------------------------------------------------
            No error - just cleans up.
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_INPUT_ROUTINE:    
        case DSSOCKI_OUTPUT_ROUTINE:    
          /*-----------------------------------------------------------------
            For I/O routines, we are no longer connected.
          -----------------------------------------------------------------*/
          *dss_errno = DS_ENOTCONN;
          break;

	case DSSOCKI_BIND_ROUTINE:
	case DSSOCKI_LISTEN_ROUTINE:
	case DSSOCKI_ACCEPT_ROUTINE:
	  /*-----------------------------------------------------------------
	    Can't listen or accept on a closed socket. 
          -----------------------------------------------------------------*/
	  *dss_errno = DS_EINVAL;
	  break;

	case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          /*lint -e506, -e774 */
          ASSERT(0);
          /*lint +e506, +e774 */

      } /* switch (type) */

      break; /* DSSOCKI_WAIT_FOR_CLOSE */
 
    default:
    /*-----------------------------------------------------------------------
      Sanity Check - This should never happen.
    -----------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */

  } /* switch (state) */

  return (result);

} /* dssocki_get_err_val() */

/*===========================================================================

FUNCTION DSSOCKI_IFACE_CHECK

DESCRIPTION
  This function contains procedure that's generic to dss_sendto(), 
  dss_write(), dss_write_dsm_chain() and dss_connect().
  
  This function figures out which interface to use and checks for validity
  conditions on that interface.

DEPENDENCIES
  None.

RETURN VALUE
  On error, DSS_ERROR is returned; DSS_SUCCESS otherwise. 
  Error condition value is returned in the pointer to dss_errno, 

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor specified. 
  DS_ENETDOWN         network is closed. 
  DS_EWOULDBLOCK      not ready to write yet. 
  
SIDE EFFECTS
  None.

===========================================================================*/
sint15 dssocki_iface_check
(
  struct scb_s *scb_ptr,               /* ptr to socket control block type */
  boolean        re_orig,        /* should this cause dormant re-orig?     */
  boolean        check_flow_ctl,         /* TRUE=iface flow ctl is checked */ 
  sint15       *dss_errno                      /*   error condition value. */
)
{
  ps_iface_type *ps_iface_ptr;           /* Ptr to iface for this socket   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Now check to see if this socket has a valid interface to go over.
  -------------------------------------------------------------------------*/
  ps_iface_ptr = scb_ptr->routing_cache;

  if(PS_IFACE_IS_VALID( ps_iface_ptr) == FALSE)
  {
    MSG_ERROR("Sockfd %d iface NOT Found", scb_ptr->sockfd, 0, 0 );
    *dss_errno = DS_ENETDOWN;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Make sure the iface is in the right state. For sys sockets it should be 
    either UP or ROUTEABLE, for non sys sockets it should be UP
  -------------------------------------------------------------------------*/
  if(( ps_iface_state( ps_iface_ptr) != IFACE_UP ) &&
     ( ps_iface_state( ps_iface_ptr) != IFACE_ROUTEABLE ) &&
     ( ps_iface_state( ps_iface_ptr) != IFACE_GOING_DOWN )    
    )
  {
    MSG_HIGH("Sockfd %d iface 0x%x st %d NOT UP/ROUT", 
                  scb_ptr->sockfd, 
                  ps_iface_ptr->name, 
                  ps_iface_state( ps_iface_ptr)
             );
    *dss_errno= DS_ENETDOWN;
    return( DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    If this is a non-system socket then it can be flow controlled when
    system sockets are executing.
    For example, this will happen during MIP registrations.
  -------------------------------------------------------------------------*/
  if ( !IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_SYS_SOCK) ) 
  {
    if ( PS_IFACE_NON_SYS_SOCKETS_FLOW_ENABLED(ps_iface_ptr) == FALSE )
    {
       MSG_HIGH("Non-sys sockfd %d is blocked", scb_ptr->sockfd, 0, 0);
       *dss_errno= DS_EWOULDBLOCK;
       return (DSS_ERROR);
    }

    if ( ps_iface_state( ps_iface_ptr) != IFACE_UP )
    {
      MSG_HIGH("Non-sys sockfd %d iface 0x%x st %d NOT UP %d", 
                  scb_ptr->sockfd, 
                  ps_iface_ptr->name, 
                  ps_iface_state( ps_iface_ptr)
              );
      *dss_errno= DS_ENETDOWN;
      return( DSS_ERROR);
    }
  }

  /*-------------------------------------------------------------------------
    If inbound flow is disabled on this socket's interface, then return
    EWOULDBLOCK to the application
  -------------------------------------------------------------------------*/
  if( (check_flow_ctl == TRUE) && 
      (PS_IFACE_FLOW_ENABLED( ps_iface_ptr) == FALSE))
  {
    //put flow mask here
    MSG_MED("Sock %d if%d FlowControlled", 
             scb_ptr->sockfd, 
             ps_iface_ptr->name, 
             0 
           );
    *dss_errno= DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    If the interface is dormant then ask it to be brought up. The errno 
    returned by ps_iface is returned back to the application - typically this
    will be WOULDBLOCK
  -------------------------------------------------------------------------*/
  if(( re_orig == TRUE) &&
     ( ps_iface_phys_link_state( ps_iface_ptr) == PHYS_LINK_DOWN))
  {
    MSG_MED("Sock %d if%d Dormant Bringing UP", 
             scb_ptr->sockfd, 
             ps_iface_ptr->name, 
             0 
           );
    ps_iface_phys_link_up_cmd( ps_iface_ptr, dss_errno, NULL);
    //check for bad errnos before proceeding
    return( DSS_ERROR);
  }

  return (DSS_SUCCESS);
} /* dssocki_iface_check() */

/*===========================================================================

FUNCTION DSSOCKI_COMMON_WRITE_CHECK

DESCRIPTION
  This function contains procedure that's generic to dss_sendto(), 
  dss_write(), dss_write_dsm_chain().

DEPENDENCIES
  None.

RETURN VALUE
  On error, DSS_ERROR is returned; DSS_SUCCESS otherwise. 
  Error condition value is returned in the pointer to dss_errno, 
  a pointer to socket control block is returned. 

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor specified. 
  DS_ENETDOWN         network is closed. 
  DS_EWOULDBLOCK      not ready to write yet. 
  
SIDE EFFECTS
  None.

===========================================================================*/
sint15 dssocki_common_write_check
(
  sint15       sockfd,                                /* socket descriptor */
  struct scb_s **scb_p_ptr,      /* pointer to scb, can be NULL initially. */
  boolean        re_orig,        /* should this cause dormant re-orig?     */
  boolean        check_flow_ctl,         /* TRUE=iface flow ctl is checked */ 
  sint15       *dss_errno                      /*   error condition value. */
)
{
  sint15 result = 0;                     /*   error checking return value  */
  struct scb_s *scb_ptr;                 /* Socket Ctl Block Ptr           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor specified.
  -------------------------------------------------------------------------*/
  if ( (*scb_p_ptr == NULL)
       &&
       ((*scb_p_ptr=dssocki_sockfdtoscb(sockfd)) == NULL) )
  {
    *dss_errno = DS_EBADF;
    MSG_ERROR("Bad sockfd %d to WRITEcheck", sockfd, 0, 0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check for any error conditions - if it will fail, return immediately
  -------------------------------------------------------------------------*/
  scb_ptr = *scb_p_ptr;
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_OUTPUT_ROUTINE, dss_errno);
  if (result == DSS_ERROR)
  {
    MSG_ERROR("sockfd %d ERR %d in WRITEcheck", sockfd, *dss_errno, 0);
    return (DSS_ERROR);
  }


  /*-------------------------------------------------------------------------
    Make sure socket is writeable before proceeding further. A (TCP) socket 
    is writeable when its sendq is less than the offered RX window. Non-TCP
    sockets are always writeable.
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->is_writeable != NULL &&
      (scb_ptr->fcn_ptr_table->is_writeable(scb_ptr->protocol_ctl_blk)== 0 ))

  {
    MSG_MED("Sockfd %d NOT Writeable - blocked", sockfd, 0, 0 );
    *dss_errno= DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }


 return( dssocki_iface_check( scb_ptr,
                              re_orig,
                              check_flow_ctl,
                              dss_errno
                            ));

}/* dssocki_common_write_check() */


/*===========================================================================
FUNCTION      DSSOCKI_IFACE_EV_CBACK()

DESCRIPTION   Called when the IFACE goes up/down or when IP address changes.
              This posts a command to PS and is processed in PS task context. 

DEPENDENCIES

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_iface_ev_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(event)
  {
    case IFACE_UP_EV:
      if( event_info.state != IFACE_UP )
      {
        MSG_MED("Sending NETWORK_UP_EV_CMD to PS",0,0,0);
        ps_send_cmd(PS_SOCKET_NETWORK_UP_EVENT_CMD, iface_ptr);
      }
      break;

    case IFACE_DOWN_EV:
      if( event_info.state != IFACE_DOWN )
      {
        MSG_MED("Sending NETWORK_DOWN_EV_CMD to PS",0,0,0);
        ps_send_cmd(PS_SOCKET_NETWORK_DOWN_EVENT_CMD, iface_ptr);
      }
      break;

    case IFACE_ADDR_CHANGED_EV:
      MSG_MED("Sending IFACE_ADDR_CHANGED_CMD to PS",0,0,0);
      ps_send_cmd(PS_SOCKET_IFACE_ADDR_CHANGED_CMD, iface_ptr);
      break;

    case IFACE_PHYS_LINK_UP_EV:
      if( event_info.link_state != PHYS_LINK_UP )
      {
        MSG_MED("Sending PHYS_LINK_UP_EV_CMD to PS",0,0,0);
        ps_send_cmd(PS_SOCKET_PHYS_LINK_UP_EVENT_CMD, iface_ptr);
      }
      break;

    case IFACE_PHYS_LINK_DOWN_EV:
      if( event_info.link_state != PHYS_LINK_DOWN )
      {
        MSG_MED("Sending PHYS_LINK_DOWN_EV_CMD to PS",0,0,0);
        ps_send_cmd(PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD, iface_ptr);
      }
      break;

    default:
      ERR("Unexpected IFACE event %d",event,0,0);
      ASSERT(0);
  }
} /* dssocki_iface_ev_cback() */

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_IFACE_UP_EV_CBACK()

DESCRIPTION   Called when the IFACE goes up. This calls the 
              net_callback_fcn() for the specified application.

DEPENDENCIES  None. 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_iface_up_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
)
{
  ps_iface_type *iface_ptr = (ps_iface_type *)user_info_ptr;
  uint32  index=0;
  acb_type *curr_acb_ptr=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Loop through all the apps and notify apps that have the same iface_ptr
    as the one on which the UP event occurred.
  -------------------------------------------------------------------------*/
  for (index=0; index<DSS_MAX_APPS; index++)
  {
    curr_acb_ptr = &dssocki_acb_array[index];
    if(curr_acb_ptr->app_id != APPAVAIL)
    {
      if(curr_acb_ptr->ps_iface_ptr == iface_ptr)
      {    
        /*-------------------------------------------------------------------
          Notify app
        -------------------------------------------------------------------*/
        if(curr_acb_ptr->net_callback_fcn != NULL)
        {
          MSG_LOW("Calling net cback for app %d",
                    curr_acb_ptr->app_id,0,0);
          curr_acb_ptr->net_callback_fcn(&curr_acb_ptr->app_id);
        }
        else
        {
          MSG_MED("NOT calling net cback for app %d",
                   curr_acb_ptr->app_id,0,0);
        }
      }
    
      /*---------------------------------------------------------------------
       Invoke any app callbacks for this event.
      ---------------------------------------------------------------------*/
      dssocki_invoke_app_ev_cbacks(curr_acb_ptr,
                                  iface_ptr,
                                  DSS_IFACE_IOCTL_UP_EV);    
    }
  }   
} /* dssocki_process_iface_up_ev_cback() */

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_IFACE_DOWN_EV_CBACK()

DESCRIPTION   Called when the IFACE goes down. This calls the
              net_callback_fcn() for the specified application.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
#define BRANCH_TASKFREE() {TASKFREE()
void dssocki_process_iface_down_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
)
{
  ps_iface_type *iface_ptr = (ps_iface_type *)user_info_ptr;
  uint32  index=0;
  acb_type *curr_acb_ptr=NULL;
  scb_type *curr_scb_ptr=NULL;
#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
  acl_ipfilter_info_type ip_info;
  acl_policy_info_type   acl_policy;
  sint15                 result;
  sint15                 dss_errno;
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Loop through all the sockets and set the routing cache of all the sockets
    that are using the interface that has gone down to NULL.  
  -------------------------------------------------------------------------*/
  for(index = 0; index < DSS_MAX_SOCKS; index++)
  {
    TASKLOCK();
    curr_scb_ptr = &scb_array[index];
    if(curr_scb_ptr->routing_cache == iface_ptr)
    {
      curr_scb_ptr->routing_cache = NULL;
    }
    TASKFREE();
  }

  /*-------------------------------------------------------------------------
    Loop through all the apps and notify apps that have the same iface_ptr
    as the one on which the DOWN event occurred.
  -------------------------------------------------------------------------*/
  for (index=0; index<DSS_MAX_APPS; index++)
  {
    TASKLOCK();
    curr_acb_ptr = &dssocki_acb_array[index];
    if(curr_acb_ptr->app_id != APPAVAIL)
    {
      if(curr_acb_ptr->ps_iface_ptr == iface_ptr)
      {
#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
        /*-------------------------------------------------------------------
          Skip the network down event for the applications which called
          net_start when the interface is in the process of tearing down.
        -------------------------------------------------------------------*/
        if(DSSOCKI_IS_APP_CTRL_FLAG_SET(curr_acb_ptr,
                                        DSSOCKI_NET_UP_EVENT_WAIT))
        {
          DSSOCKI_RESET_APP_CTRL_FLAG(curr_acb_ptr, DSSOCKI_NET_UP_EVENT_WAIT);
          
          MSG_HIGH("Bring the App %d UP, as DSSOCKI_NET_UP_EVENT_WAIT is SET",
                    curr_acb_ptr->app_id, 0, 0);
          
          memset(&acl_policy, 0, sizeof(acl_policy_info_type));

          if(curr_acb_ptr->app_specified_iface_ptr != NULL)
          {
            MSG_HIGH("Using app spec iface name=0x%x,inst=%d",
                     curr_acb_ptr->app_specified_iface_ptr->name,
                     curr_acb_ptr->app_specified_iface_ptr->instance, 0);
            acl_policy.if_id = curr_acb_ptr->app_specified_iface_ptr->name;
          }
          else
          {
            acl_policy.if_id = ANY_DEFAULT_GROUP;
          }

          acl_policy.orig_from_sock = TRUE;
          /* Get the data session profile id from ACB net policy */
          acl_policy.data_session_profile_id = 
            curr_acb_ptr->acl_netpolicy_info.data_session_profile_id;

          /* Set the IP based routing info */
          memset(&ip_info, 0, sizeof(ip_info));
          ip_info.ip_vsn = IP_V4;
          ip_info.ip_hdr.v4.dest = ROUTE_DEFAULT_DST;
          ip_info.is_secure = TRUE;

          if(route_get(&ip_info,
                       &(acl_policy),
                       FALSE,
                       NULL,
                       &(curr_acb_ptr->ps_iface_ptr)) == 0)
          {
            result = ps_iface_bring_up_cmd(iface_ptr, &dss_errno, NULL);
            if( (result == 0)  || (dss_errno == DS_EWOULDBLOCK) )
            {
              BRANCH_TASKFREE();
              continue;
            }
            else if(dss_errno == DS_ENETCLOSEINPROGRESS)
            {
              MSG_HIGH("Setting Control Flag DSSOCKI_NET_UP_EVENT_WAIT for "
                       "APP ID %d", curr_acb_ptr->app_id, 0, 0);
              DSSOCKI_SET_APP_CTRL_FLAG(curr_acb_ptr,
                                        DSSOCKI_NET_UP_EVENT_WAIT);
              BRANCH_TASKFREE();
              continue;
            }
          } /* End of if(route_get()) */
        } /* End of if(DSSOCKI_IS_APP_CTRL_FLAG_SET) */
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */
	      
        /*-------------------------------------------------------------------
          Notify app
        -------------------------------------------------------------------*/
        if(curr_acb_ptr->net_callback_fcn != NULL)
        {
          MSG_LOW("Calling net cback for app %d",
                  curr_acb_ptr->app_id,0,0);
          curr_acb_ptr->net_callback_fcn(&curr_acb_ptr->app_id);
        }
        else
        {
          MSG_MED("NOT calling net cback for app %d",
                   curr_acb_ptr->app_id,0,0);
        }
        
        dssocki_socket_event_occurred(NULL, curr_acb_ptr);
        
        /*-------------------------------------------------------------------
          Set ps_iface in acb to NULL.
        -------------------------------------------------------------------*/
        curr_acb_ptr->ps_iface_ptr = NULL;
      }
    
      /*---------------------------------------------------------------------
        Invoke any app callbacks for this event.
      ---------------------------------------------------------------------*/
      dssocki_invoke_app_ev_cbacks(curr_acb_ptr,
                                   iface_ptr,
                                   DSS_IFACE_IOCTL_DOWN_EV);  
    }
    TASKFREE();
  }  
} /* dssocki_process_iface_down_ev_cback() */

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_PHYS_LINK_UP_EV_CBACK()

DESCRIPTION   Called when the PHYS_LINK goes up. This calls any app
              registered callbacks for this event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_phys_link_up_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
)
{
  ps_iface_type *iface_ptr = (ps_iface_type *)user_info_ptr;
  uint32  index=0;
  acb_type *curr_acb_ptr=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Loop through all the apps and notify apps that have registered for the 
    the DOWN event on the ps_iface on which the event occured.
  -------------------------------------------------------------------------*/
  for (index=0; index<DSS_MAX_APPS; index++)
  {
    curr_acb_ptr = &dssocki_acb_array[index];
    if(curr_acb_ptr->app_id != APPAVAIL)
    {
      dssocki_invoke_app_ev_cbacks(curr_acb_ptr,
                                   iface_ptr,
                                   DSS_IFACE_IOCTL_PHYS_LINK_UP_EV);
    }
  }
} /* dssocki_process_phys_link_up_ev_cback() */

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_PHYS_LINK_DOWN_EV_CBACK()

DESCRIPTION   Called when the PHYS_LINK goes down. This calls any app
              registered callbacks for this event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_phys_link_down_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
)
{
  ps_iface_type *iface_ptr = (ps_iface_type *)user_info_ptr;
  uint32  index=0;
  acb_type *curr_acb_ptr=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Loop through all the apps and notify apps that have registered for the 
    the DOWN event on the ps_iface on which the event occured.
  -------------------------------------------------------------------------*/
  for (index=0; index<DSS_MAX_APPS; index++)
  {
    curr_acb_ptr = &dssocki_acb_array[index];
    if(curr_acb_ptr->app_id != APPAVAIL)
    {
      dssocki_invoke_app_ev_cbacks(curr_acb_ptr,
                                   iface_ptr,
                                   DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV);
    }
  }
} /* dssocki_process_phys_link_down_ev_cback() */


/*===========================================================================
FUNCTION      DSSOCKI_INVOKE_APP_EV_CBACKS()

DESCRIPTION   This function invokes callbacks registered by the app (using 
              the IOCTL interface) for the specified event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_invoke_app_ev_cbacks
(
  acb_type                        *acb_ptr, 
  ps_iface_type                   *iface_ptr,
  dss_iface_ioctl_event_enum_type event
)
{
  dssocki_event_buf_type *event_buf_ptr;
  q_type *working_q_ptr=NULL;
  dss_iface_ioctl_event_info_union_type event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(acb_ptr != NULL);

  /*------------------------------------------------------------------------
    Set the event info based on the event.
  ------------------------------------------------------------------------*/
  switch(event)
  {
    case DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV:
    case DSS_IFACE_IOCTL_PHYS_LINK_UP_EV:
      event_info.phys_link_state_info = ps_iface_phys_link_state(iface_ptr);
      break;

    case DSS_IFACE_IOCTL_DOWN_EV:
    case DSS_IFACE_IOCTL_UP_EV:
      event_info.iface_state_info = ps_iface_state(iface_ptr);
      break;

    case DSS_IFACE_IOCTL_ADDR_CHANGED_EV:
      event_info.addr_change_info.type    = IPV4_ADDR;
      event_info.addr_change_info.addr.v4 = PS_IFACE_IP_V4_ADDR(iface_ptr);
      /*---------------------------------------------------------------------
        Convert IP address to network byte order.
      ---------------------------------------------------------------------*/
      event_info.addr_change_info.addr.v4 = 
                              dss_htonl(event_info.addr_change_info.addr.v4);
      break;

    default:
      MSG_ERROR("Unsupported event %d", event,0,0);
      ASSERT(0);
  }

  /*------------------------------------------------------------------------
    Notify app callbacks for this event. Note that the app could
    potentially be using a different interface currently.
  ------------------------------------------------------------------------*/
  if(acb_ptr->app_id != APPAVAIL)
  {
    working_q_ptr = (acb_ptr->event_q_array) + event;
    event_buf_ptr = q_check(working_q_ptr);
    while(event_buf_ptr != NULL)
    {
      /*-------------------------------------------------------------------
        Check to see if the callback is for the ps_iface on which the 
        event occurred.
      -------------------------------------------------------------------*/
      if(event_buf_ptr->iface_ptr == iface_ptr)
      {
         MSG_MED("Calling callback for ev%d for app%d", 
                  event,
                  acb_ptr->app_id,
                  0);
         ASSERT(event_buf_ptr->f_ptr != NULL);
         ASSERT(event_buf_ptr->iface_ptr != NULL);
         event_buf_ptr->f_ptr(event,
                              event_info,
                              event_buf_ptr->user_info_ptr,
                              acb_ptr->app_id,
                              (dss_iface_ioctl_name_enum_type)iface_ptr->name,
                              iface_ptr->instance);
         /*------------------------------------------------------------------
           Note that each app can have only one callback for an event on a 
           particular interface. So we can break here.
         ------------------------------------------------------------------*/
         break;
      }
      else
      {
        event_buf_ptr = q_next(working_q_ptr,&(event_buf_ptr->link));
      }
    }
  }
} /* dssocki_invoke_app_ev_cbacks */


/*===========================================================================
FUNCTION      DSSOCKI_IS_IFACE_IN_USE()

DESCRIPTION  This function determines if any other app is using the interface
             used by the specified app.

DEPENDENCIES  None

RETURN VALUE  boolean - TRUE if atleast one other app is using iface.
                        FALSE if no other app is using iface.

SIDE EFFECTS  None.
===========================================================================*/
boolean dssocki_is_iface_in_use
(
  acb_type                *acb_ptr
)
{
  acb_type *curr_acb_ptr;
  int index;
#ifdef FEATURE_DS_MOBILE_IP
  sint15 mip_app_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mip_app_id = mipio_get_app_id();
#endif /* FEATURE_DS_MOBILE_IP */

  /*-----------------------------------------------------------------------
    Loop through acb array to find which acb is waiting for a network event
  -----------------------------------------------------------------------*/
  for (index=0; index<DSS_MAX_APPS; index++)
  {
    curr_acb_ptr = &dssocki_acb_array[index];

    /*--------------------------------------------------------------------
      If the app. is waiting for a net event and is valid app_id then,
      call the callback function.
    --------------------------------------------------------------------*/
    if( (curr_acb_ptr->app_id != APPAVAIL) &&
        (curr_acb_ptr->app_id != acb_ptr->app_id) &&
        (curr_acb_ptr->ps_iface_ptr == acb_ptr->ps_iface_ptr)
#ifdef FEATURE_DS_MOBILE_IP
        && (curr_acb_ptr->app_id != mip_app_id)
#endif /* FEATURE_DS_MOBILE_IP */
      ) // JD -verify
    {
      MSG_MED("Iface 0x%x in use",acb_ptr->ps_iface_ptr,0,0);
      return TRUE;
    }
  } /* end of the for ()                                               */

  MSG_MED("Iface 0x%x not in use",acb_ptr->ps_iface_ptr,0,0);
  return FALSE;

} /* dssocki_is_iface_in_use() */


/*===========================================================================

FUNCTION DSSOCKI_REG_EV()

DESCRIPTION
  This function registers an application callback for the specified event. If
  the event is true, it calls the registered callback immediately.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If registration is successful.
  FALSE     If registration failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssocki_reg_ev
(
  sint15                    app_id,
  ps_iface_event_enum_type  event,
  ps_iface_type             *iface_ptr,                                       
  dss_iface_ioctl_event_cb  f_ptr,
  void                      *user_info_ptr
)
{
  dssocki_event_buf_type *event_buf_ptr=NULL;
  q_type *working_q_ptr=NULL;
  acb_type *acb_ptr=NULL;
  dsm_item_type   *dsm_ptr=NULL;
  boolean need_to_invoke_cback = FALSE;
  dss_iface_ioctl_event_info_union_type event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(sizeof(dssocki_event_buf_type) <= DSM_DS_SMALL_ITEM_SIZ);
  ASSERT(event < DSS_IFACE_IOCTL_EVENT_MAX);
  ASSERT(iface_ptr != NULL);

  acb_ptr = dssocki_appidtoacb(app_id);
  if(acb_ptr == NULL)
  {
    MSG_ERROR("Invalid app_id %d",app_id,0,0); 
    ASSERT(0);
    return FALSE;
  }

  if(f_ptr == NULL)
  {
    MSG_ERROR("App %d passed in Null callback function",app_id,0,0); 
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Check to see if there is a callback already registered for the 
    specified event by this application.
  ------------------------------------------------------------------------*/
  working_q_ptr = (acb_ptr->event_q_array) + event;
  event_buf_ptr = q_check(working_q_ptr);
  while(event_buf_ptr != NULL)
  {
    if(event_buf_ptr->iface_ptr == iface_ptr)
    {
      MSG_ERROR("App %d already has callback for ev%d",app_id,event,0);
      return FALSE;
    }
    else
    {
      event_buf_ptr = q_next(working_q_ptr,&(event_buf_ptr->link));
    }
  }

  /*------------------------------------------------------------------------
    Allocate a small DSM item to enqueue the event callback.
  ------------------------------------------------------------------------*/
  dsm_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if(dsm_ptr == NULL)
  {
     MSG_ERROR("Small item allocation failed",0,0,0);
     return FALSE;
  }

  /*------------------------------------------------------------------------
    Initialize the event_buf_ptr.
  ------------------------------------------------------------------------*/
  event_buf_ptr = (dssocki_event_buf_type *)dsm_ptr->data_ptr;
  q_link(event_buf_ptr, &(event_buf_ptr->link));
  event_buf_ptr->iface_ptr     = iface_ptr;
  event_buf_ptr->user_info_ptr = user_info_ptr;
  event_buf_ptr->f_ptr         = f_ptr;
  event_buf_ptr->dsm_ptr       = dsm_ptr;

  /*-------------------------------------------------------------------------
    Do the book keeping with the DSM item.  
  -------------------------------------------------------------------------*/
  dsm_ptr->used       += sizeof(dssocki_event_buf_type);

  /*-------------------------------------------------------------------------
    Enqueue the event callback.  
  -------------------------------------------------------------------------*/
  q_put(acb_ptr->event_q_array + event, &(event_buf_ptr->link));
  MSG_MED("Ev%d callback registered for app %d",event,app_id,0);


  /*-----------------------------------------------------------------------
    check if the event is true - if so call the callback immediately
  -----------------------------------------------------------------------*/
  switch(event)
  {

    /*---------------------------------------------------------------------
                              IFACE_DOWN_EV
    ---------------------------------------------------------------------*/
    case IFACE_DOWN_EV:
      if(IFACE_DOWN == ps_iface_state(iface_ptr))
      {
        event_info.iface_state_info = IFACE_DOWN;
        need_to_invoke_cback = TRUE;
      }
      break;

    /*---------------------------------------------------------------------
                               IFACE_UP_EV
    ---------------------------------------------------------------------*/
    case IFACE_UP_EV:
      if(IFACE_UP == ps_iface_state(iface_ptr))
      {
        event_info.iface_state_info = IFACE_UP;
        need_to_invoke_cback = TRUE;
      }
      break;

    /*---------------------------------------------------------------------
                         IFACE_PHYS_LINK_DOWN_EV
    ---------------------------------------------------------------------*/
    case IFACE_PHYS_LINK_DOWN_EV:
      if(PHYS_LINK_DOWN == ps_iface_phys_link_state(iface_ptr))
      {
        event_info.phys_link_state_info = PHYS_LINK_DOWN;
        need_to_invoke_cback = TRUE;
      }
      break;

    /*---------------------------------------------------------------------
                          IFACE_PHYS_LINK_UP_EV
    ---------------------------------------------------------------------*/
    case IFACE_PHYS_LINK_UP_EV:
      if(PHYS_LINK_UP == ps_iface_phys_link_state(iface_ptr))
      {
        event_info.phys_link_state_info = PHYS_LINK_UP;
        need_to_invoke_cback = TRUE;
      }
      break;

  default:
    /* do nothing */;

  } /* switch(event) */

  /*-----------------------------------------------------------------------
    If the callback needs to be called, do so
  -----------------------------------------------------------------------*/
  if(need_to_invoke_cback)
  {
    f_ptr((dss_iface_ioctl_event_enum_type)event,
          event_info,
          user_info_ptr,
          app_id,
          (dss_iface_ioctl_name_enum_type)iface_ptr->name,
          iface_ptr->instance);

  }

  return TRUE;
} /* dssocki_reg_ev() */


/*===========================================================================

FUNCTION DSSOCKI_DEREG_EV()

DESCRIPTION
  This function deregisters the application callback for the specified event.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If deregistration is successful.
  FALSE     If deregistration failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssocki_dereg_ev
(
  sint15                    app_id,
  ps_iface_event_enum_type  event,
  ps_iface_type             *iface_ptr
)
{
  dssocki_event_buf_type *event_buf_ptr=NULL;
  q_type *working_q_ptr=NULL;
  acb_type *acb_ptr=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(event < DSS_IFACE_IOCTL_EVENT_MAX);

  acb_ptr = dssocki_appidtoacb(app_id);
  if(acb_ptr == NULL)
  {
    MSG_ERROR("Invalid app_id %d",app_id,0,0);
    ASSERT(0);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Check to see if there is a callback already registered for the
    specified event by this application.
  ------------------------------------------------------------------------*/
  working_q_ptr = (acb_ptr->event_q_array) + event;
  event_buf_ptr = q_check(working_q_ptr);
  while(event_buf_ptr != NULL)
  {
    if(event_buf_ptr->iface_ptr == iface_ptr)
    {
      break;
    }
    else
    {
      event_buf_ptr = q_next(working_q_ptr,&(event_buf_ptr->link));
    }
  }

  if(event_buf_ptr == NULL)
  {
    MSG_ERROR("Could not find callback for ev%d for app%d",event,app_id,0);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Remove this callback from the event queue.
  ------------------------------------------------------------------------*/
#ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete(working_q_ptr, &(event_buf_ptr->link));
#else
  q_delete(&(event_buf_ptr->link));
#endif

  /*------------------------------------------------------------------------
    Free the associated DSM item.
  ------------------------------------------------------------------------*/
  dsm_free_buffer(event_buf_ptr->dsm_ptr);
  MSG_MED("Ev%d callback deregistered for app %d",event,app_id,0);
  return TRUE;
} /* dssocki_dereg_ev() */


/*===========================================================================

FUNCTION DSSOCKI_REG_IP_FILTER()

DESCRIPTION
  This function registers IP filters for the specified iface. The filters are
  passed down to the iface to be added to iface control block while a mapping
  of the iface is added to SCB of the specified socket. This mapping is used 
  to deregister these filters when the socket is closed. 

DEPENDENCIES
  None.

RETURN VALUE
  Num filters added successfully, or
  -1 for error (sets dss_errno appropriately)

SIDE EFFECTS
  None.

===========================================================================*/
int dssocki_reg_ip_filter
(
  sint15                sockfd,
  ps_iface_type         *iface_ptr,
  ip_filter_enum_type   filter_type,
  uint8                 num_filters,
  void                  *filters,
  sint15                *dss_errno
)
{
  struct scb_s *scb_ptr;          /* ptr to socket control block structure */
  int n_filters;
  uint16 index, free_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    ASSERT(0);
    return -1;
  }

  *dss_errno = DSS_SUCCESS;

  if(iface_ptr == NULL)
  {
    *dss_errno = DS_EBADF;
    MSG_ERROR("Null iface to reg_ip_filter", 0, 0, 0);
    return -1;
  }

  if(filters == NULL)
  {
    *dss_errno = DS_EFAULT;
    MSG_ERROR("Null filter to reg_ip_filter", 0, 0, 0);
    return -1;
  }
  
  if((scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    MSG_ERROR("Bad sockfd %d to reg_ip_filter", sockfd, 0, 0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Add the filters to the iface
  -------------------------------------------------------------------------*/

  if ((n_filters = ps_iface_ipfltr_add(iface_ptr, 
                                      filter_type, 
                                      num_filters, 
                                      filters, 
                                      sockfd)) == 0)
  {
    *dss_errno = DS_NOMEMORY;    
    return -1;
  }

  /*-------------------------------------------------------------------------
    Keep a mapping of the iface in the SCB to cleanup filters when the 
    socket is closed. Add the iface to the map table. Scan the map table
    to see if thee iface is already present, also keep track of a free entry
    in the table. 
  -------------------------------------------------------------------------*/
  free_index = MAX_SYSTEM_IFACES;

  for(index=0; index<MAX_SYSTEM_IFACES; index++)
  {
    if(scb_ptr->fi_map[index] == iface_ptr)
    {
      break;
    }

    if(scb_ptr->fi_map[index] == NULL)
    {
      free_index = index;
    }
  }

  /* If iface not found, add it */
  if(index == MAX_SYSTEM_IFACES)
  {
    ASSERT(free_index < MAX_SYSTEM_IFACES);
    scb_ptr->fi_map[free_index] = iface_ptr;
  }

  return n_filters;
} /* dssocki_reg_ip_filter() */


/*===========================================================================

FUNCTION DSSOCKI_DEREG_IP_FILTER()

DESCRIPTION
  This function deregisters all the IP filters previosuly added to an iface
  associated with the specified socket. The filters are deleted from the 
  iface control block and the iface is removed from the filter map.
  
DEPENDENCIES
  None.

RETURN VALUE
  0 for success, -1 for error (sets dss_errno appropriately)

SIDE EFFECTS
  None.

===========================================================================*/
int dssocki_dereg_ip_filter
(
  sint15                sockfd,
  ps_iface_type         *iface_ptr,
  sint15                *dss_errno
)
{
  struct scb_s *scb_ptr;          /* ptr to socket control block structure */
  uint16 index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(dss_errno == NULL)
  {
    MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    ASSERT(0);
    return -1;
  }

  *dss_errno = DSS_SUCCESS;

  if(iface_ptr == NULL)
  {
    *dss_errno = DS_EBADF;
    MSG_ERROR("Null iface to reg_ip_filter", 0, 0, 0);
    return -1;
  }
  
  if((scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    MSG_ERROR("Bad sockfd %d to reg_ip_filter", sockfd, 0, 0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Add the filters to the iface
  -------------------------------------------------------------------------*/

  ps_iface_ipfltr_delete(iface_ptr, sockfd);
  
  /*-------------------------------------------------------------------------
    Delete the  mapping of the iface from the SCB
  -------------------------------------------------------------------------*/

  for(index=0; index<MAX_SYSTEM_IFACES; index++)
  {
    if(scb_ptr->fi_map[index] == iface_ptr)
    {
      scb_ptr->fi_map[index] = NULL;
      break;
    }
  }

  return 0;
} /* dssocki_dereg_ip_filter() */
/*===========================================================================
FUNCTION DSSOCKI_SENDMSG()

DESCRIPTION
  This function is a common write function for all the socket output 
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and destination address for unconnected udp sockets.
  The function writes data from the scattered buffers over the transport 
  specified by the socket descriptor.
DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, in case of tcp it could be less 
  than the number of bytes specified.

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
  DS_EISCONN          if the socket is connected and the destination
                      address is other than it is connected to.

SIDE EFFECTS
  None.
===========================================================================*/

sint15 dssocki_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dssocki_msghdr * msg,     /* Header containing data and dest addr */
  dsm_item_type        ** msg_ptr, /* Optional payload already in DSMs     */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
)
{
  struct scb_s          * scb_ptr;      /* Ptr to socket control block     */
  sint15                  count;        /* count of number of bytes copied */
  sint15                  result = 0;   /* error checking return value     */
  dsm_item_type         * item_ptr=NULL;/* data services memory pool item  */
  sint15                  i;            /* local counter variable          */
  uint32                  nbytes = 0;   /* total number of bytes specified */
  uint32                  ipv4_dst;     /* Destination aadr of datagramm   */
  acl_ipfilter_info_type  ip_info;      /* Storage for IP info for routg.  */
  struct sockaddr_in    * ta_ptr;       /* tmp ptr to toaddr               */
  boolean                 should_re_orig; /* should this cause re-orig     */
  int                     sndq_limit;   /* Limit on send queue size        */
  
  should_re_orig = TRUE;  
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    MSG_HIGH("DSSOCKI_SENDMSG: Null pointer passed for dss_errno\n",0,0,0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  MSG_LOW(" dssocki_sendmsg function called", 0, 0, 0);
  /*-------------------------------------------------------------------------
    Make sure that the flags that are set are within the range that we
    support - if no return an error.  Otherwise get a meta_info block and put
    the flags in there.
    NOTE: the value of MSG_RESERVED bits changes based on the IS2000_SDB
          feature. 
  -------------------------------------------------------------------------*/

  if((flags & MSG_RESERVED_BITS) != 0)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }
#ifdef FEATURE_DATA_IS2000_SDB
  else /* flags valid */
  {
    /*-----------------------------------------------------------------------
      Since any of these options means we shouldn't re-orig, set to false
    -----------------------------------------------------------------------*/
    if ( (flags & (MSG_EXPEDITE | MSG_FAST_EXPEDITE)) != 0)
    {
      should_re_orig = FALSE;
    }
  } /* else(flags valid) */
#endif

  /*-------------------------------------------------------------------------
    Invalid socket descriptor specified.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    MSG_ERROR("Bad sockfd %d to WRITEcheck", sockfd, 0, 0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check for any error conditions - if it will fail, return immediately
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_OUTPUT_ROUTINE, dss_errno);
  if (result == DSS_ERROR)
  {
    MSG_ERROR("sockfd %d ERR %d in WRITEcheck", sockfd, *dss_errno, 0);
    return (DSS_ERROR);
  }
 
  /*-------------------------------------------------------------------------
    Check for iovcount being 0.
  -------------------------------------------------------------------------*/
  if ( msg->msg_iovlen == 0 )
  {
    return (0);
  }

  /*-------------------------------------------------------------------------
    At this point iovcount is non-zero. Hence, iov should be non-NULL.
  -------------------------------------------------------------------------*/
  if ( msg->msg_iov == NULL )
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Validate the specified server address - if it is NULL,or the value is 0,
    check to see if server address has already been specified - if not, 
    return error as DS_EADDRREQ.
  -------------------------------------------------------------------------*/
  if (msg->msg_name != NULL)
  {
    /*-----------------------------------------------------------------------
      check the address family.
    -----------------------------------------------------------------------*/
    if(((struct sockaddr*)msg->msg_name)->sa_family != AF_INET)
    {
      *dss_errno = DS_EAFNOSUPPORT;
      return (DSS_ERROR);
    }
    /*-----------------------------------------------------------------------
      check to ensure valid address length
    -----------------------------------------------------------------------*/
    if (msg->msg_namelen < sizeof(struct sockaddr_in))
    {
      *dss_errno = DS_EFAULT;
      return (DSS_ERROR);  
    }
    ta_ptr = (struct sockaddr_in *)msg->msg_name;
    /*-----------------------------------------------------------------------
      non-NULL to_addr,so check to see if address specified is 0 - If it is,
      check to see if we already have a server address stored in the socket.
      If we do not, then return DS_EADDRREQ indicating that the application
      must specify a server address.
    -----------------------------------------------------------------------*/
  
    if (ta_ptr->sin_addr.s_addr == 0)
    {
      /*---------------------------------------------------------------------
        Valid to_addr pointer, but address is 0 - check to see if server
        address already specified - If not, return error.
      ---------------------------------------------------------------------*/
      if (scb_ptr->servname.address == 0)
      {
        *dss_errno = DS_EADDRREQ;
        return (DSS_ERROR);
      }
      ta_ptr = NULL;
      ipv4_dst = scb_ptr->servname.address;
    }
    else
    {
      /*-----------------------------------------------------------------------
        Check if the socket is connected 
      -----------------------------------------------------------------------*/
      if(scb_ptr->servname.address != 0)
      {
        *dss_errno = DS_EISCONN;
        return DSS_ERROR;
      }
      ipv4_dst = ntohl(ta_ptr->sin_addr.s_addr);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      NULL to_addr, so check to see if address already specified - If so,
      then continue (and do not re-assign), otherwise return DS_EADDRREQ.
      Note, we ignore the addrlen parameter for this condition.
    -----------------------------------------------------------------------*/
    if (scb_ptr->servname.address == 0)
    {
      *dss_errno = DS_EADDRREQ;
      return (DSS_ERROR);
    }
    ta_ptr = NULL;
    ipv4_dst = scb_ptr->servname.address;
  }

  /*-------------------------------------------------------------------------
    Determine if a non-zero number of bytes have been specified. If so,
    then continue, otherwise, return 0 bytes written.
  -------------------------------------------------------------------------*/
  if ( msg_ptr == NULL )
  {
    for (i=0; i< msg->msg_iovlen && nbytes < DSS_WRITE_MAX_BYTES; i++)
    {
      /*---------------------------------------------------------------------
        Return an error if the length is non-zero but the address is NULL.
      ---------------------------------------------------------------------*/
      if ((msg->msg_iov[i].iov_len != 0 && msg->msg_iov[i].iov_base == NULL))
      {
        *dss_errno = DS_EFAULT;
        return (DSS_ERROR);
      }
      nbytes += msg->msg_iov[i].iov_len;
    }
    
    /*-----------------------------------------------------------------------
      All array items have specified zero length, so return 0 bytes written.
    -----------------------------------------------------------------------*/
    if (nbytes == 0)
    {
      return (0);
    }
  }
  else
  {
    if ( *msg_ptr )
    {
      nbytes = dsm_length_packet( *msg_ptr );
    }
    else
    {
      nbytes = 0;
    }
  }

  nbytes = MIN(nbytes,DSS_WRITE_MAX_BYTES);
  sndq_limit = nbytes;

  /*-------------------------------------------------------------------------
    Do early routing and add the result to the SCB's routing cache
    if the cache is not set yet or the UDP destination has changed.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->routing_cache == NULL)||
       (scb_ptr->address_cache.addr.v4 != ipv4_dst) )
  {
    /* Set the address specific routing information */
    memset( &ip_info, 0, sizeof( ip_info ) );
    ip_info.ip_vsn = IP_V4;
    ip_info.ip_hdr.v4.protocol = scb_ptr->protocol;
    ip_info.ip_hdr.v4.dest = ipv4_dst;
    if ( scb_ptr->servname.port > 0 )
    {
      ip_info.ptcl_info.tcp.dst_port = scb_ptr->servname.port;
    }

#ifdef FEATURE_SEC_IPSEC
    /* Don't route IKE packets via the IPSec interface */
    if ( scb_ptr->sockfd == secipsdss_ike_sockfd )
    {
      ip_info.is_secure = TRUE;
    }
#endif /* FEATURE_SEC_IPSEC */

    /* Policy information is coming through socket options if       */
    /* the ACB's routing cache is empty. For now there are none ... */
    result = route_get( &ip_info, 
                        NULL, 
                        FALSE, 
                        NULL, 
                        &(scb_ptr->routing_cache) );
    if ( result == -1 )
    {
      MSG_HIGH( "output call: Rt lookup failed !", 0, 0, 0 );
      *dss_errno = DS_ENOROUTE;
      scb_ptr->routing_cache = NULL;
      scb_ptr->address_cache.addr.v4 = 0x00000000;
      return result;
    }
    else
    {
#ifdef FEATURE_SEC_IPSEC
      if ( scb_ptr->routing_cache == &secips_iface )
      {
        MSG_LOW( "output call: got IPSec pkt", 0, 0, 0 );
        ip_info.is_secure = TRUE;

        /* Do a secondary routing lookup to get the physical iface */
        result = route_get( &ip_info, 
                            NULL, 
                            FALSE, 
                            NULL, 
                            &(scb_ptr->routing_cache) );
        if ( result == -1 )
        {
          MSG_HIGH( "output call: Rt lookup failed !", 0, 0, 0 );
          *dss_errno = DS_ENOROUTE;
          scb_ptr->routing_cache = NULL;
          scb_ptr->address_cache.addr.v4 = 0x00000000;
          return result;
        }        
        else
        {
          MSG_LOW("output call: Rt lookup -> 0x%x",
                  scb_ptr->routing_cache, 0, 0 );
          scb_ptr->address_cache.type = IPV4_ADDR;
          scb_ptr->address_cache.addr.v4 = ipv4_dst;
          scb_ptr->ipsec_hdr_size = SECIPS_ESP_MAX_HDR_SIZE;
        }
      }
      else
#endif /* FEATURE_SEC_IPSEC */
      {
        MSG_LOW("output call: Rt lookup -> 0x%x",
                scb_ptr->routing_cache, 0, 0 );
        scb_ptr->address_cache.type = IPV4_ADDR;
        scb_ptr->address_cache.addr.v4 = ipv4_dst;
      }
    }
  }

 
  result = dssocki_iface_check( scb_ptr,
				should_re_orig,
				should_re_orig,
				dss_errno
				);
  if(result == DSS_ERROR)
  {
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    If we need not reoriginate then register sdb ack status handler 
    function with the ps_iface if the socket option is set
  -------------------------------------------------------------------------*/
  if ( (should_re_orig == FALSE) &&
       IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_SDB_ACK_CB) )
  {
    MSG_LOW("setting sdb handler for iface=%x", 
             scb_ptr->routing_cache, 
             0, 
             0);
    ps_iface_reg_sdb_status_handler( scb_ptr->routing_cache, 
                                     dssocki_sdb_status_handler);
  }


  /*-------------------------------------------------------------------------
    If nbytes + sizeof IP header + size of UDP header is more than MTU of the 
    interface  then return DS_EMSGSIZE.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->type == SOCK_DGRAM)
       && 
       ((nbytes + IPLEN + UDPHLEN) 
        > 
        (scb_ptr->routing_cache->v4_net_info.mtu - scb_ptr->ipsec_hdr_size)) )
  {    
    MSG_HIGH("DS_EMSGSIZE OCCURED.", 0, 0, 0);
    *dss_errno = DS_EMSGSIZE;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Make sure socket is writeable before proceeding further. A (TCP) socket 
    is writeable when its sendq is less than the offered RX window. Non-TCP
    sockets are always writeable.
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->is_writeable != NULL &&
      ((sndq_limit = 
       scb_ptr->fcn_ptr_table->is_writeable(scb_ptr->protocol_ctl_blk)) == 0))
  {
    MSG_MED("Sockfd %d NOT Writeable - blocked", sockfd, 0, 0 );
    *dss_errno= DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }  

  /*-------------------------------------------------------------------------
    Non-zero amount of data specified.  Copy array into data services 
    memory pool
  -------------------------------------------------------------------------*/
  if ( msg_ptr && *msg_ptr )
  {
    count = nbytes;
    item_ptr = *msg_ptr;
    *msg_ptr = NULL;
  }
  else
  {
    count = dssocki_copy_buf_to_dsm(msg->msg_iov, msg->msg_iovlen, 
            MIN(sndq_limit, nbytes), &item_ptr);
  }

  /*-------------------------------------------------------------------------
    For Stream sockets, ensure that bytes were actually copied and that 
    item_ptr is non-NULL,before dispatching it to PS.  Return DS_EWOULDBLOCK 
    since the only way for this function to fail is by running out of buffer 
    space.
  -------------------------------------------------------------------------*/
  if ( scb_ptr->type == SOCK_STREAM && (count <= 0) || (item_ptr == NULL)) 
  {
     *dss_errno = DS_EWOULDBLOCK;
     return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    For datagram sockets, ensure that bytes were actually copied and that 
    item_ptr is non-NULL,before dispatching it to PS.  Release item_ptr and 
    return DS_EWOULDBLOCK since the only way for this function to fail is by 
    running out of buffer space.   
  -------------------------------------------------------------------------*/
  if ( scb_ptr->type == SOCK_DGRAM && (count != nbytes) || (item_ptr == NULL)) 
  {
    MSG_HIGH("NO MEM, EWOULDBLOCK.", 0, 0, 0);
    dsm_free_packet( &item_ptr);
    *dss_errno = DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }
 

  /*-------------------------------------------------------------------------
    Geenrate the meta_info for udp and icmp sockets. This may attach the 
    meta_info to the app_field of the item_ptr in question.
  -------------------------------------------------------------------------*/
  if(scb_ptr->protocol == IPPROTO_UDP || scb_ptr->protocol == IPPROTO_ICMP)
  {
    if (dssocki_generate_meta_info( item_ptr,
        scb_ptr,
        flags,
        dss_errno ) != DSS_SUCCESS )
    {
      MSG_HIGH("MetaInfo gen. failed (%d)", dss_errno, 0, 0 );
      dsm_free_packet( &item_ptr);
      *dss_errno = DS_EWOULDBLOCK;
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Call function to put the item on the queue to PS.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
  item_ptr->tracer = DSM_SCKT_TO_PS;
#endif 
  /*-------------------------------------------------------------------------
    FEATURE_DSM_MEM_CHK 
  -------------------------------------------------------------------------*/ 
  scb_ptr->sndcnt += count;
  dssocki_send_to_ps( scb_ptr, item_ptr, ta_ptr, DSM_PS_SOCKET_PAYLOAD);

  return (count);

} /* dssocki_sendmsg() */

/*===========================================================================
FUNCTION DSSOCKI_RECVMSG()
DESCRIPTION
  This function is a common read function for all the socket input 
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and an address field for filling the source address
  of the received packet.The function reads data into the scattered buffers 
  over the transport specified by the socket descriptor

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
sint15 dssocki_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dssocki_msghdr * msg,      /* Message header for filling in data  */
  dsm_item_type        ** msg_ptr,  /* Optional ptr to recv data in DSMs   */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
)
{
  struct scb_s      * scb_ptr;       /* Ptr to socket control block        */
  sint15              num_read;      /* number of bytes read               */
  sint15              result = 0;    /* error checking return value        */
  struct sockaddr_in  from_addr;     /* source address of the recvd packet */
  int                 i;             /* local loop index                   */
 
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL) 
  {
    MSG_HIGH("DSS_RECVMSG : NULL pointer passed for dss_errno", 0,0,0);
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
    Validate the specified server address - if it is NULL. 
    return error as DS_EADDRREQ.
  -------------------------------------------------------------------------*/
  if (msg->msg_name != NULL)
  {
    /*-----------------------------------------------------------------------
      check to ensure valid address length
    -----------------------------------------------------------------------*/
    if (msg->msg_namelen < sizeof(struct sockaddr_in))
    {
      *dss_errno = DS_EFAULT;
      return (DSS_ERROR);  
    }

  }
  /*-------------------------------------------------------------------------
    Flags are not supported
  -------------------------------------------------------------------------*/

  if(flags != 0)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check for any error conditions - if it will fail, return immediately
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_INPUT_ROUTINE, dss_errno);
  if (result == DSS_ERROR)
  {
    if (*dss_errno == DS_EEOF)
    {
      return (DSS_SUCCESS);
    }
    else
    {
      return (result);
    }
  }

  if ( msg_ptr == NULL )
  {
    /*-----------------------------------------------------------------------
      Check for iovcount being 0. If so, return 0 as the number of bytes read
    -----------------------------------------------------------------------*/
    if ( msg->msg_iovlen == 0 )
    {
      return 0;
    }
    
    /*-----------------------------------------------------------------------
      If we reach this point then iovcount is non-zero. Hence, it is an error
      if iov is NULL.
    ----------------------------------------------------------------------*/
    if ( msg->msg_iov == NULL )
    {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
    }
    
    
    for(i=0;i< msg->msg_iovlen ; i++)
    { 
      /*---------------------------------------------------------------------
        Verify that valid buffer and length values were passed
      ---------------------------------------------------------------------*/
      if((msg->msg_iov[i].iov_base == NULL && msg->msg_iov[i].iov_len != 0))
      {
        *dss_errno=DS_EFAULT;
        return DSS_ERROR;
      }
    }
    
    /*-----------------------------------------------------------------------
      Call protocol specific function to read bytes into an array.
    -----------------------------------------------------------------------*/
    num_read = scb_ptr->fcn_ptr_table->read(scb_ptr, 
                                            msg->msg_iov, 
                                            msg->msg_iovlen,
                                            &from_addr, 
                                            dss_errno);
  }
  else
  {
    /* Keep receive data in a DSM chain */
    num_read = scb_ptr->fcn_ptr_table->read_dsm_chain( scb_ptr,
                                                       msg_ptr,
                                                       &from_addr,
                                                       dss_errno );
  }

  /*-------------------------------------------------------------------------
    Copy the source address into the message header if a valid pointer
    is passed
  -------------------------------------------------------------------------*/
  if(msg->msg_name != NULL)
  {
    *((struct sockaddr_in *)msg->msg_name) = from_addr;
  }
    
  /*-------------------------------------------------------------------------
    Read was not successful... 
  -------------------------------------------------------------------------*/
  if ((num_read == DSS_ERROR) &&
      (*dss_errno == DS_EWOULDBLOCK) &&
      (scb_ptr->servname.address != 0) &&
      (PS_IFACE_IS_VALID(scb_ptr->acb_ptr->ps_iface_ptr) == FALSE))
  {
    /*-----------------------------------------------------------------------
      ENETDOWN because no network...set dss_errno to reflect this.
    -----------------------------------------------------------------------*/
    *dss_errno = DS_ENETDOWN;
  }

  return (num_read);

} /* dssocki_recvmsg() */

/*===========================================================================
FUNCTION DSSOCKI_PROCESS_LINGERING_CLOSE()

DESCRIPTION
  This function is called to when a lingering socket is closed. It brings
  down the interface if no other application is using it and frees up the 
  scb and the associated acb (a lingering socket's acb is allocated 
  internally when the socket enters the lingering state)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frees up the acb and the scb, passed as a parameter
===========================================================================*/
void
dssocki_process_lingering_close
(
  scb_type** scb_ptr_ptr
)
{
  sint15 dss_errno;
  scb_type* scb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(scb_ptr_ptr != NULL);
  scb_ptr = *scb_ptr_ptr;
  ASSERT(scb_ptr != NULL);

  MSG_MED("process_lingering_close for sockfd=%d", scb_ptr->sockfd, 0, 0);

  /*-------------------------------------------------------------------------
    Bring down the phy iface if no other app is using this interface
  -------------------------------------------------------------------------*/
  if ( scb_ptr->acb_ptr == scb_ptr->linger_acb_ptr )
  {
    TASKLOCK();
    if(dssocki_is_iface_in_use(scb_ptr->acb_ptr) == FALSE)
    {
      ps_iface_tear_down_cmd( scb_ptr->acb_ptr->ps_iface_ptr, 
                              &dss_errno, 
                              NULL);
    }
    /*-------------------------------------------------------------------------
      Free up the acb and scb
    -------------------------------------------------------------------------*/
    dssocki_freeacb(scb_ptr->acb_ptr);
    dssocki_freescb(scb_ptr);
    *scb_ptr_ptr = NULL;
    TASKFREE();
  }
  else
  {
    dssocki_freeacb(scb_ptr->linger_acb_ptr);
  }
  return;
}

#ifdef FEATURE_PS_UNSUPPORTED
#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================
FUNCTION      DSSOCKI_MIP_IFACE_IN_USE()

DESCRIPTION   This function determines if any other app is using the
              interface used by the specified app.

DEPENDENCIES  None

RETURN VALUE  boolean - TRUE if atleast one other app is using iface.
                        FALSE if no other app is using iface.

SIDE EFFECTS  None.
===========================================================================*/
boolean dssocki_mip_iface_in_use
(
  ps_iface_type *  go_iface
)
{
  acb_type *  mip_acb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mip_acb_ptr = dssocki_appidtoacb (mipio_get_app_id()); // chcek if either can fail

  ASSERT (mip_acb_ptr->ps_iface_ptr == NULL)

  if (ps_iface_state(go_iface) == IFACE_DOWN)
  {
    mip_acb_ptr->ps_iface_ptr = go_iface;
    return FALSE;
  }
  return TRUE;
}
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_PS_UNSUPPORTED */

#ifdef FEATURE_SEC_IPSEC
/*===========================================================================
FUNCTION      DSSOCKI_SET_IPSEC_ROUTING_CACHES()

DESCRIPTION   This function sets all non-Rm interfaces up for IPSec.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssocki_set_ipsec_routing_caches
(
  sint15  ipsec_fd          /* Ike sockfd, used to find IPSec tx interface */
)
{
  scb_type*  ipsec_scb_ptr;
  int        i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the routing cache of the IKE socket */
  if ( (ipsec_scb_ptr = dssocki_sockfdtoscb( ipsec_fd ) ) == NULL )
  {
    MSG_ERROR( "IPS: Bad IKE fd %d", ipsec_fd, 0, 0 );
    return;
  }

  /* Check through all sockets */
  for( i=0; i<DSS_MAX_SOCKS; i++ )
  {
    if ( scb_array[i].routing_cache 
         &&
         ((scb_array[i].routing_cache->name != SIO_IFACE)
          &&
          (scb_array[i].routing_cache->name != LO_IFACE)) )
    {
      /* Set routing cache for sockets using IPSec */
      scb_array[i].routing_cache = ipsec_scb_ptr->routing_cache;

      /* Adjust MTU by 64 bytes for the IPSec ESP header */
      scb_array[i].ipsec_hdr_size = 64;
    }
  }

} /* dssocki_set_ipsec_routing_caches() */


/*===========================================================================
FUNCTION      DSSOCKI_FLUSH_IPSEC_ROUTING_CACHES()

DESCRIPTION   This function resets all IPSec-controlled routing
              caches to zero.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssocki_flush_ipsec_routing_caches( void )
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check through all sockets */
  for( i=0; i<DSS_MAX_SOCKS; i++ )
  {
    /* Reset routing cache for sockets using IPSec */
    if ( scb_array[i].ipsec_hdr_size > 0 )
    {
      scb_array[i].routing_cache  = NULL;
      scb_array[i].ipsec_hdr_size = 0;
    }
  }

} /* dssocki_flush_ipsec_routing_caches() */
#endif /* FEATURE_SEC_IPSEC */

#endif /* FEATURE_DS_SOCKETS */
