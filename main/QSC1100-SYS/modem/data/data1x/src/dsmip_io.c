/*===========================================================================

     D A T A  S E R V I C E S   M O B I L E   I P   I / O   A P I  F I L E
                   
DESCRIPTION

 The Data Services Mobile IP I/O implementation.  Contains API and internal
 functions for Mobile IP connection control.
 
EXTERNALIZED FUNCTIONS

  mipio_init()
    Initializes all control structures for Mobile IP and opens all the
    sockets to support the specified # of sessions.  Must be called 
    before the mobile IP service is started (i.e. before any function below).

  mipio_write()
    Sends specified data to specified session.

  mipio_handle_socket_event
    Callback to handle events on the mobile IP socket, called by the ps task

Copyright (c) 2000,2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_io.c_v   1.25   19 Feb 2003 15:57:20   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_io.c#2 $ $DateTime: 2008/06/02 08:26:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/02/08    ms     Fixed Critical/High Lint errors
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
09/23/03    jd     Mainlined FEATURE_DATA_MM_SINGLE_MODE_TARGET for 6100
                   branch
08/06/03    jd     Featurized set_routeable_app_opt under 
                   FEATURE_DATA_MM_SINGLE_MODE_TARGET to remove dependency 
                   for 6100 target
07/20/03    usb    Setting routeable option for MIP App.
05/14/03    jd     Force Um interface during pppopen (after iface is created)
05/05/03    jd     Force Um to use 1x interface
                   For multimode builds, removed dependencies on dsiface
05/02/03    usb    Passing errno param to dss_close().
04/23/03    usb    Correctly passing in the from_addr buffer length to 
                   recvfrom() instead of NULL.
12/29/02    jd     Changed mipio_handle_*_event prototypes to match multimode
                   sig handler signature, ret val. ignored in old arch.
11/19/02    jd     For MM, don't call mip_iface_in_use() (done in pppopen),
                   don't post UM_MIP_DOWN to RMSM on net_ev w/ iface UP
11/14/02    jd     call dssocki_mip_iface_in_use in MM instead of ppp_in_use
11/14/02    jd     changed mipioi_app_id unassigned value to APPAVAIL to 
                   ensure dssocki_is_iface_in_use works correctly.
05/10/02    jd     moved definition of rx buffer size here from dsmip.h
01/02/02    aku    dss_ppp_in_use() now returns uint instead of a boolean.
11/12/01    aku    Replaced call to dssocki_is_socket_app_active() with
                   dss_ppp_in_use()
11/09/01    aku    mipio_ppp_open() now returns mipio_pppopen_ret_val type
                   instead of a boolean
09/04/01    aku    Notify MIP meta sm when iface is closed.
08/23/01    aku    Renamed dss_getifacestatus as dss_get_iface_status
08/17/01    aku    Added accessor function to access mipioi_app_id
08/14/01    jd     pass sender's IP address to mip_parse_msg()
08/07/01    jd     mipio_write() returns completion status
08/02/01    jd     added a debug message to report reception of net event
07/31/01    sjy    Renamed RMSM_UM_IFACE_DOWN to RMSM_UM_MIP_DOWN
07/30/01    na     Added some messages and removed an unecessary ASSERT
07/30/01    aku    Posting event to RM SM when Um iface comes up with SIP
07/27/01    na     Fixed mipio_handle_network_event so that we return UM_MIP
                   _UP_EV to rmsm.
07/12/01    jd     now register for write event notification if write fails.
                   removed close event code in mipioi_event_helper (unused).
                   code cleanup (linting and code review).
                   removed fromaddr from mipio_handle_sock_event (unused).
07/06/01    jd     removed mipio_shutdown as we changed the design to 
                   initialize on startup and never call shutdown
06/28/01    mvl    modified some debug messages to reduce debug chatter
06/26/01    jd     use setsockopt to set all mobile IP sockets to system mode 
06/18/01    jd     ignore dss_close_netlib result in mipio_shutdown 
06/13/01    mvl    updated references to match names in session info block.
06/11/01    jd     changed rx buffer declaration to ensure long word 
                   alignment
06/07/01    jd     replaced conn_cb_type with socket descriptor array
05/26/01    jd     removed: fromaddr check in sock event cb, bind to UDP port
09/26/00    jd     Created module.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

#include <string.h>
#include "assert.h"
#include "dssocket.h"
#include "dssicmp.h"
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "task.h"

#include "dsmip_io.h"
#include "dsmip_parse.h"
#include "dsmip_solsm.h"
#include "dsmip_regsm.h"
#include "dsmip_metasm.h"
#include "dsmip_msg.h"

#ifdef FEATURE_DATA_MM
#include "ps_svc.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds707_rmsm.h"
#endif
#include "ps_mip_compat.h"
#include "dss_iface_ioctl.h"
#else
// #include "ps.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dsmip_rmsm.h"
#endif
#endif /* FEATURE_DATA_MM */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  MIP_MN_MAX_RXMSG_LEN is sufficient to hold
  - agent advertisement + expected extensions, or
  - registration reply + expected extensions
---------------------------------------------------------------------------*/
#define MIP_MN_MAX_RXMSG_LEN   (1024)

/*---------------------------------------------------------------------------
  xCEIL - macro to give "the minimum number of units of size 's' bytes 
          required to contain 'x' bytes. 
  LCEIL - macro to give the minimum number of long words (uint32's) that 
          will contain 'x' bytes.
---------------------------------------------------------------------------*/
#define xCEIL(x,s)  ((x+(s-1))/s)
#define LCEIL(x)    xCEIL(x,sizeof(uint32))

/*---------------------------------------------------------------------------
  The connection mapping table maps mip i/o connections to socket
  descriptors returned by dss_socket().  i.e. 

    mipioi_sd['connection'] = 'socket descriptor'

  The logical connections are numbered as follows:

  connection          purpose
  -----------         ----------
   0                  UDP socket for Mobile IP session #1
   1                  UDP socket for Mobile IP session #2
   ...                ...
   MIP_MAX_SESSIONS-1 UDP socket for Mobile IP session #(MIP_MAX_SESSIONS)
   MIP_MAX_SESSIONS   ICMP socket to rx ICMP msg type 9 (AAM)
   MIP_MAX_SESSIONS+1 ICMP socket to tx ICMP msg type 10 (ASM)

  MIPIO_MAX_SOCKS is the number of sockets used by the mipio module.
---------------------------------------------------------------------------*/
#define MIPIO_MAX_SOCKS  (MIP_MAX_SESSIONS+2)

#if (MIPIO_MAX_SOCKS > 255)
  #error MIPIO module does not support > 255 sockets.
#endif


/*===========================================================================

                             INTERNAL DATA 

===========================================================================*/

/*---------------------------------------------------------------------------
  MIP io connection to socket descriptor mapping
---------------------------------------------------------------------------*/
LOCAL sint15  mipioi_sd[MIPIO_MAX_SOCKS];   

/*---------------------------------------------------------------------------
  Flag to track the initialization status of mobile IP i/o library
---------------------------------------------------------------------------*/
LOCAL boolean  mipioi_initialized = FALSE;   /* flag - true if initialized */

/*---------------------------------------------------------------------------
  Flag to track the initialization status of mobile IP i/o library
---------------------------------------------------------------------------*/
LOCAL sint15  mipioi_app_id = APPAVAIL;                 /* Application ID */

/*---------------------------------------------------------------------------
  Read buffer for incoming messages on MIP sockets
---------------------------------------------------------------------------*/
LOCAL uint32  mipioi_read_buffer[LCEIL(MIP_MN_MAX_RXMSG_LEN)];


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION MIPIOI_CLOSE_ALL_SOCKETS

DESCRIPTION
  Closes all MIPIO socket connections.  Doesn't break if the socket is not 
  open.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mipioi_close_all_sockets
(
  void
)
{
  sint15  result;                 /* holds return value for called fns     */
  uint8   i;                      /* loop counter                          */
  sint15  l_errno;                /* errno returned from dss_*() calls     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Close all connections 
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIPIO_MAX_SOCKS; i++)
  {
    /*-----------------------------------------------------------------------
      If the connection is not open (indicated by sd=DSS_ERROR), skip it
    -----------------------------------------------------------------------*/
    if (mipioi_sd[i] == DSS_ERROR) 
    {
      continue;
    }
  
    /*-----------------------------------------------------------------------
      Close the specified socket, this will never block since we're dealing
      with UDP & ICMP sockets
    -----------------------------------------------------------------------*/
    result = dss_close( mipioi_sd[i], &l_errno );
    if (result == DSS_ERROR)
    {
      ERR ("Error %d in closing MIP i/o connection #%d (sd=%d)",
           l_errno, i, mipioi_sd[i]); 
    }
  
    /*-----------------------------------------------------------------------
      Set the socket descriptor value to DSS_ERROR to indicate 'not in use'
    -----------------------------------------------------------------------*/
    mipioi_sd[i] = DSS_ERROR;
  }
} /* mipioi_close_all_sockets() */



/*===========================================================================

FUNCTION MIPIOI_OPEN_SOCKET

DESCRIPTION
  Opens the socket of the type and protocol specified.

PARAMETERS
  prot - protocol of socket to open (IPPROTO_ICMP | IPPROTO_UDP)
  type - for ICMP -> ICMP msg type to receive with this socket
         for UDP  -> SOCK_DGRAM
  code - ICMP message code to bind to this socket (not used for UDP)

RETURN VALUE
  socket descriptor or DSS_ERROR if the open failed

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static sint15 mipioi_open_socket
( 
  byte  prot,
  uint8 type,
  uint8 code
)
{
  sint31  result;                 /* holds return value for called fns     */
  sint15  sd;                     /* opened socket descriptor              */
  sint15  l_errno;                /* errno returned from dss_*() calls     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd = DSS_ERROR;

  /*-------------------------------------------------------------------------
    This function is called internal to this file, so we know that prot
    will only be ICMP or UDP and thus no range checking is performed here.
    Assertion included for development debugging.
  -------------------------------------------------------------------------*/
  ASSERT (prot == IPPROTO_ICMP || prot == IPPROTO_UDP);

  if (prot == IPPROTO_ICMP)
  {
    sd = dssicmp_socket( mipioi_app_id, type, code, &l_errno);
  }
  else /* (prot == IPPROTO_UDP) */
  {
    sd = dss_socket( mipioi_app_id, AF_INET, type, prot, &l_errno); 
  }

  /*-------------------------------------------------------------------------
    If a socket couldn't be opened, close all and return failure 
  -------------------------------------------------------------------------*/
  if (sd == DSS_ERROR)
  {
    MSG_HIGH ("Couldn't open socket!", 0, 0, 0);
    mipioi_close_all_sockets();
    return (DSS_ERROR);
  } 

  /*-------------------------------------------------------------------------
    Ask to be notified when data arrives on the socket (readable).  
    We assume that the socket is writeable as soon as it is opened.
    If dss_async_select fails, the socket descriptor is corrupt, so return
    and close gracefully (socket will be closed in mipioi_close_sockets).
  -------------------------------------------------------------------------*/
  result = dss_async_select( sd, DS_READ_EVENT, &l_errno );
  if (result != DSS_SUCCESS)
  {
    /*-----------------------------------------------------------------------
      If a socket couldn't be opened, close all and return failure 
    -----------------------------------------------------------------------*/
    MSG_HIGH ("Reg for read event failed! (sd=%d)", sd, 0, 0);
    mipioi_close_all_sockets();
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Return the opened socket descriptor
  -------------------------------------------------------------------------*/
  return (sd);

} /* mipioi_open_socket() */



/*===========================================================================

FUNCTION MIPIOI_EVENT_HELPER

DESCRIPTION
  Processes current events on the specified socket.

  This section of code is seperated simply for clarity and compactness.

PARAMETERS
  index - connection number that event occurred on.
  event - event that occurred (as defined in dssocket.h), 
          (a single event, not the event mask!)

RETURN VALUE
  None.

DEPENDENCIES
  None. 

SIDE EFFECTS
  None.

===========================================================================*/
static void mipioi_event_helper
( 
  int  index,                   /* connection index that event occurred on */
  int  event                    /* current event mask                      */
)
{
  struct sockaddr_in  fa;       /* for recvfrom(), the sender's address    */
  uint16              fa_addr_len; /* sender's address buffer length       */
  sint15              l_errno;  /* errno returned from dss_*() call        */
  sint15              result;   /* return value for called functions       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (event & DS_WRITE_EVENT)
  {
    /*---------------------------------------------------------------------
      A write event indicates that a write to the socket is possible.  

      Send a retry event to the appropriate state machine - 

      connections 0..MAX_SESSIONS-1 are UDP sockets
        which imply kicking the registration state machine, 
      connection  MAX_SESSIONS   is the ICMP RX socket
      connection  MAX_SESSIONS+1 is the ICMP TX socket
        which imply kicking the solicitation state machine.

      MIPIO only registers for this event when a previous write failed.
    ---------------------------------------------------------------------*/
    MSG_LOW( "DS_WRITE_EVENT on connection %d", index, 0, 0 );
    if (index < MIP_MAX_SESSIONS) 
    {
      mip_reg_sm_post_event (INDEX_2_MIP_SES_PTR(index), 
                             RSM_IMMED_RETRY_EV);
    }
    else
    {
      mip_sol_sm_post_event (SSM_IMMED_RETRY_EV);
    }
    event &= ~DS_WRITE_EVENT;
  }
 
  if (event & DS_READ_EVENT)
  {
    /*---------------------------------------------------------------------
      A read event on the ICMP socket indicates data has arrived, so read 
      it.  Pass the data along to the parser.
    ---------------------------------------------------------------------*/

    MSG_LOW( "DS_READ_EVENT on connection %d", index, 0, 0 );

    while (TRUE)
    {
      MSG_MED ("polling for message", 0,0,0);
      /*-------------------------------------------------------------------
        Read data off socket, pass to parser.  Returns length of received
        message (i.e. stored in 'result')
      -------------------------------------------------------------------*/
      fa_addr_len = sizeof(fa);
      result = dss_recvfrom ( mipioi_sd[index], 
                              mipioi_read_buffer, 
                              sizeof(mipioi_read_buffer),
                              0, 
                              (struct sockaddr *) &fa,
                              &fa_addr_len, 
                              (sint15 *) &l_errno );
      if (result == DSS_ERROR)
      {
        if (l_errno == DS_EWOULDBLOCK)
        {
          /*---------------------------------------------------------------
            No more data, stop spinning
          ---------------------------------------------------------------*/
          MSG_LOW("exiting mipioi_event_helper", 0,0,0);
        }
        else
        {
          /*---------------------------------------------------------------
            Unexpected error (busted sockets, etc.) - ignore.
            If this actually happens, the socket should be reinitialized
            (this should never happen though).
          ---------------------------------------------------------------*/
          ERR ("Error on socket read (sd=%d), errno = %d", 
               mipioi_sd[index], l_errno, 0);
        }
        break;
      }

      /*-------------------------------------------------------------------
        pass the data along to the parser.
      -------------------------------------------------------------------*/
      mip_parse_msg (index,mipioi_read_buffer, fa.sin_addr.s_addr, result); 
    }

    MSG_LOW("Broke out of read loop", 0,0,0);

    event &= ~DS_READ_EVENT;
  }

  MSG_MED ("Done processing mip socket event",0,0,0);

} /* mipioi_event_helper() */



/*===========================================================================

FUNCTION MIPIOI_SOCK_EVENT_CB

DESCRIPTION
  Socket event callback.  Sends signal to PS to call MIP socket event 
  handler.

PARAMETERS
  dummy   - ignored

RETURN VALUE
  None.

DEPENDENCIES
  mipio_open() must be called successfully before mipioi_sock_event_cb()
  is called.

SIDE EFFECTS
  None.

===========================================================================*/
static void mipioi_sock_event_cb
(
  void *  dummy                                                /* not used */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void) rex_set_sigs(&ps_tcb, PS_MIP_SOCK_EVENT_SIG);

} /* mipioi_sock_event_cb() */



/*===========================================================================

FUNCTION MIPIOI_NET_EVENT_CB

DESCRIPTION
  Network event callback.  Sends signal to PS to call MIP socket event 
  handler.

PARAMETERS
  dummy   - ignored

RETURN VALUE
  None.

DEPENDENCIES
  mipio_open() must be called successfully before mipioi_net_event_cb()
  is called.

SIDE EFFECTS
  None.

===========================================================================*/
static void mipioi_net_event_cb
(
  void *  dummy                                                /* not used */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void) rex_set_sigs(&ps_tcb, PS_MIP_NET_EVENT_SIG);

} /* mipioi_net_event_cb() */



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION MIPIO_HANDLE_SOCKET_EVENT

DESCRIPTION
  Called by PS to process socket event on a mobile IP socket

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mipio_handle_socket_event
(
#ifdef FEATURE_DATA_MM
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
#else
  void
#endif /* FEATURE_DATA_MM */
)
{
  sint31  result;                     /* return value for called functions */
  sint15  l_errno;                    /* errno returned from dss_*() call  */
  uint8   i;                          /* loop counter                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Clear the event signal in the ps context
  -------------------------------------------------------------------------*/
  (void) rex_clr_sigs(&ps_tcb, PS_MIP_SOCK_EVENT_SIG);

  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (mipioi_initialized == FALSE)
  {
    ERR("MIPIO not initialized!", 0, 0, 0);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Check each socket for an event, handle as appropriate
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIPIO_MAX_SOCKS; i++)
  {
    /*-----------------------------------------------------------------------
      Poll this socket to see if an event occurred. 
    -----------------------------------------------------------------------*/
    result = dss_getnextevent(mipioi_app_id, &mipioi_sd[i], &l_errno);
    if (result == DSS_ERROR)
    {
      /*---------------------------------------------------------------------
        If failed, either the application ID or socket descriptor is corrupt.
      ---------------------------------------------------------------------*/
      MSG_ERROR ("Error %d getting event on connection %p", 
                 l_errno, mipioi_sd[i], 0);
      ASSERT(0);
      return TRUE;
    }

    /*-----------------------------------------------------------------------
      If result is non-zero, some event occurred on the socket, so handle it.
    -----------------------------------------------------------------------*/
    if (result != 0)
    {
      MSG_MED( "Socket event %d on connection %d", result, i, 0);
      mipioi_event_helper (i, result);
    }
    else
    {
      MSG_HIGH( "No Socket event on connection %d", i, 0, 0);
    }

    /*-----------------------------------------------------------------------
      Rearm the read event notification on this socket
    -----------------------------------------------------------------------*/
    result = dss_async_select(mipioi_sd[i], DS_READ_EVENT, &l_errno );
    ASSERT (result == DSS_SUCCESS);

  } /* for (all MIP sessions) */

  return TRUE;
} /* mipio_handle_socket_event() */


/*===========================================================================

FUNCTION MIPIO_HANDLE_NETWORK_EVENT

DESCRIPTION
  Called by PS to perform MIP related processing related to network event

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mipio_handle_network_event
(
#ifdef FEATURE_DATA_MM
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
#else
  void
#endif /* FEATURE_DATA_MM */
)
{
#ifdef FEATURE_DATA_MM
  dss_iface_ioctl_id_type     ioctl_id;
  dss_iface_ioctl_state_type  iface_state;
  sint15                      ps_errno;
#else
  dss_iface_status_type status;       /* stores status of network */
#endif /* FEATURE_DATA_MM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH ("Network Event",0,0,0);

  /*-------------------------------------------------------------------------
    Clear the event signal in the ps context
  -------------------------------------------------------------------------*/
  (void) rex_clr_sigs(&ps_tcb, PS_MIP_NET_EVENT_SIG);

  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (mipioi_initialized == FALSE)
  {
    ERR("MIPIO not initialized!", 0, 0, 0);
    return TRUE;
  }

#ifdef FEATURE_DATA_MM

  /*-------------------------------------------------------------------------
    check the interface state - for now assume UM SN

    can we use dss_netstatus() here?
  -------------------------------------------------------------------------*/
  ioctl_id.name     = DSS_IFACE_IOCTL_CDMA_SN;   
  ioctl_id.instance = 0;

  if( (dss_iface_ioctl( &ioctl_id,
                        DSS_IFACE_IOCTL_GET_STATE,
                        &iface_state,
                        &ps_errno ) < 0) )
  {
    MSG_ERROR("Unable to get iface %d:%d state",
              ioctl_id.name, ioctl_id.instance, 0);
    return TRUE;
  }

  if ( iface_state == IFACE_DOWN )
  {
    /*-----------------------------------------------------------------------
      Notify RMSM and MIP METASM that iface didn't come up with MIP or
      has come down.

      Future - reverse ordering below and work out details for efficiency
    -----------------------------------------------------------------------*/
    mip_rm_sm_post_event( RMSM_UM_MIP_DOWN );
    mip_meta_sm_post_event ( MSM_IFACE_DOWN_EV );
  }
  else if( iface_state == IFACE_UP && mip_reg_sm_session_active(NULL) )
  {
    /*-----------------------------------------------------------------------
      If we are registered, send event to RM State machine that Um came
      up with MIP.

      Note, need to replace NULL above with session pointer if multiple 
      MIP sessions are supported.
    -----------------------------------------------------------------------*/
    mip_rm_sm_post_event( RMSM_UM_MIP_UP );
  }
  else
  {
    /*-----------------------------------------------------------------------
      For laptop calls, QCMIP=2 is always forced, thus we don't need to 
      notify RMSM about up with SIP.
    -----------------------------------------------------------------------*/
    // ASSERT (0);
  }

#else /* !FEATURE_DATA_MM */

  /*-------------------------------------------------------------------------
    Check status of Um interface and notify Rm of MIP registration result
  -------------------------------------------------------------------------*/
  dss_get_iface_status ( IFACE_UM_PPP_STACK, &status );

  switch(status.state)
  {
    case IFACE_CLOSED:
      /*---------------------------------------------------------------------
        Send event to RM State machine that Um is down
      ---------------------------------------------------------------------*/
      MSG_MED("UM PPP down: post RMSM_UM_MIP_DOWN", 0, 0, 0);
      mip_rm_sm_post_event(RMSM_UM_MIP_DOWN);

      /*---------------------------------------------------------------------
        Close Mobile IP client
      ---------------------------------------------------------------------*/
      MSG_MED("Post MSM_IFACE_DOWN_EV to Meta SM ", 0, 0, 0);
      mip_meta_sm_post_event(MSM_IFACE_DOWN_EV);
      break;

    case IFACE_OPEN:
      if(status.iface_has_mip == TRUE)
      {
        /*-------------------------------------------------------------------
          Send event to RM State machine that Um is up with MIP
        -------------------------------------------------------------------*/
        MSG_MED("UM up with MIP: post RMSM_UM_MIP_UP", 0, 0, 0);
        mip_rm_sm_post_event( RMSM_UM_MIP_UP);
      }
      else 
      {
        /*-------------------------------------------------------------------
          Notify RM State machine that Um is up with SIP (i.e w/o M. IP)
        -------------------------------------------------------------------*/
        MSG_MED("UM up with SIP: post RMSM_UM_MIP_DOWN", 0, 0, 0);
        mip_rm_sm_post_event(RMSM_UM_MIP_DOWN);
      } 
      break;

    default:
      MSG_MED("Ignoring NetEV state=%d has_mip=%d",
               status.state,
               status.iface_has_mip,
               0);  
  } /* end switch */
#endif /* FEATURE_DATA_MM */

  return TRUE;

} /* mipio_handle_network_event */


/*===========================================================================

FUNCTION MIPIO_INIT

DESCRIPTION

  Opens and configures all sockets required for Mobile IP messaging.

  One UDP socket is required for each session, over which registration
  request messages (RRQ's) are sent and registration reply messages (RRP's)
  are received.

  Two ICMP sockets total are required for Mobile IP to handle mobility
  agent (i.e., foreign or home agent) advertisements (AAM's) and
  solicitations (ASM's).

  The DMSS ICMP implementation avoids wasting memory on extraneous messages
  by assigning a type filter to each socket.  The socket will receive ICMP
  messages of that type only, and will assign that type to any messages 
  sent on that socket.  Thus 2 sockets are required since ASM and AAM have 
  unique types.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - initialized ok (or already initialized)
  FALSE - initialization failed

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mipio_init
(
  void
)
{
  uint32  optval;                 /* option value for dss_setsockopt()     */
  uint32  optlen;                 /* length of above option value          */
  sint15  l_errno;                /* errno returned from dss_socket() call */
  uint8   i;                      /* loop counter                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check to be sure this is the first call to mipio_init() as this should
    only be done once
  -------------------------------------------------------------------------*/
  if (mipioi_initialized == TRUE)
  {
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Initialize all socket descriptors to DSS_ERROR to indicate 'not in use'
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIPIO_MAX_SOCKS; i++)
  {
    mipioi_sd[i] = DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    If this hasn't already been done, open the Network Library so we can 
    use sockets.  Note that mobile IP does not open PPP, and thus does not
    receive network events (dssockets only notifies applications via their
    registered net event callback once dss_pppopen() is called).
  -------------------------------------------------------------------------*/
  if (mipioi_app_id == APPAVAIL) 
  {
    mipioi_app_id = dss_open_netlib( mipioi_net_event_cb, 
                                     mipioi_sock_event_cb, 
                                     &l_errno );
    MSG_MED ("MIP callbacks: sock_ev_cb @ %p", mipioi_sock_event_cb, 0, 0);

    if (mipioi_app_id == DSS_ERROR)
    {
      mipioi_app_id = APPAVAIL;
      ERR ("Couldn't start Network services!", 0, 0, 0);
      return (FALSE);
    }
    MSG_MED ("Opened Network library, app id = %d", mipioi_app_id, 0, 0);
  }
  
  /*-------------------------------------------------------------------------
    Open one UDP sockets for each session
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIP_MAX_SESSIONS; i++)
  {
    mipioi_sd[i] = mipioi_open_socket ( IPPROTO_UDP, SOCK_DGRAM, 0 );
    if (mipioi_sd[i] == DSS_ERROR)
    {
      MSG_ERROR ("Couldn't open MIP socket (session %d)!", i, 0, 0);
      /*---------------------------------------------------------------------
        If a socket couldn't be opened, close all and return failure 
      ---------------------------------------------------------------------*/
      mipioi_close_all_sockets();
      return (FALSE);
    } 
    MSG_MED ("Opened app conn %d (reg ses %d), sd=%d", i, i, mipioi_sd[i]);
  } /* for (all sessions) */

  /*-------------------------------------------------------------------------
    Open the ICMP RX socket, type = MIP_AAM_TYPE, code = X (do not care)
  -------------------------------------------------------------------------*/
  mipioi_sd[MIPIO_ICMP_RX_HANDLE] = 
    mipioi_open_socket ( IPPROTO_ICMP,
                         ICMP_MSGTYPE_ROUTER_ADVERTISEMENT,
                         0 );
  if (mipioi_sd[MIPIO_ICMP_RX_HANDLE] == DSS_ERROR)
  {
    mipioi_close_all_sockets();
    return (FALSE);
  }
  MSG_MED ("Opened app conn %d (icmp rx), sd=%d", 
           MIPIO_ICMP_RX_HANDLE, 
           mipioi_sd[MIPIO_ICMP_RX_HANDLE],
           0 );

  /*-------------------------------------------------------------------------
    Open the ICMP TX socket, type = MIP_ASM_TYPE
  -------------------------------------------------------------------------*/
  mipioi_sd[MIPIO_ICMP_TX_HANDLE] = 
    mipioi_open_socket ( IPPROTO_ICMP,
                         ICMP_MSGTYPE_ROUTER_SOLICITATION,
                         0 ); 
  if (mipioi_sd[MIPIO_ICMP_TX_HANDLE] == DSS_ERROR)
  {
    mipioi_close_all_sockets();
    return (FALSE);
  } 
  MSG_MED ("Opened app conn %d (icmp tx), sd=%d", 
           MIPIO_ICMP_TX_HANDLE, 
           mipioi_sd[MIPIO_ICMP_TX_HANDLE],0 );

  /*-------------------------------------------------------------------------
    Set the TTL for all sockets
    ICMP - set permanently to 1 as req'd for agent solicitations 
           (which are the only msgs we send on this socket).
    UDP - set permanently to 255 (default value) as req'd for reverse 
           tunneling RRQ's
  -------------------------------------------------------------------------*/
  optval = 1;
  optlen = sizeof(uint32);
  (void) dss_setsockopt ( mipioi_sd[MIPIO_ICMP_TX_HANDLE], 
                          DSS_IPPROTO_IP, 
                          (int) DSS_IP_TTL, 
                          &optval,
                          &optlen,
                          &l_errno );

  for (i = 0; i < MIPIO_MAX_SOCKS; i++)
  {
    optval = 1;
    optlen = sizeof(uint32);
    (void) dss_setsockopt ( mipioi_sd[i], 
                            DSS_SOCK, 
                            (int) DSS_SO_SYS_SOCK, 
                            &optval,
                            &optlen,
                            &l_errno );
  }

  /*-------------------------------------------------------------------------
    Initialization complete, transition to idle state
  -------------------------------------------------------------------------*/
  mipioi_initialized = TRUE;

  return (TRUE);

} /* mipio_init() */



/*===========================================================================

FUNCTION MIPIO_WRITE

DESCRIPTION
  Writes data to a mipio connection.

PARAMETERS
  index     - connection # to write to (see table above for semantics)
  data      - pointer to data to be written
  data_len  - length of data pointed to by 'data'
  dest_ip   - destination IP address
  dest_port - destination port number (ignored for ICMP messages)

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mipio_write
(
  uint8   index,                           /* session number to write to   */
  void *  data,                            /* data to be written           */
  uint16  data_len,                        /* length of above data         */
  uint32  dest_ip,                         /* destination IP address       */
  uint16  dest_port                        /* destination IP port          */
)
{
  struct sockaddr_in  dest;       /* message destination                   */
  sint31              result;     /* holds return value for called fns     */
  sint15              l_errno;    /* errno returned from dss_*() calls     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (mipioi_initialized == FALSE)
  {
    ERR("MIPIO not initialized!", 0, 0, 0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Validate the arguments
  -------------------------------------------------------------------------*/
  if (index >= MIPIO_MAX_SOCKS)
  {
    MSG_ERROR ("Write to invalid session (%d), ignoring!", index, 0, 0);
    ASSERT (0);
    return FALSE;
  };

  /*-------------------------------------------------------------------------
    Initialize variables
  -------------------------------------------------------------------------*/
  dest.sin_family               = AF_INET;
  dest.sin_port                 = dss_htons(dest_port);
  dest.sin_addr.s_addr          = dest_ip;
  ((sint31 *) dest.sin_zero)[0] = 0;
  ((sint31 *) dest.sin_zero)[1] = 0;

  /*-------------------------------------------------------------------------
    Send current data
  -------------------------------------------------------------------------*/
  MSG_MED ("sending %d bytes to %x:%d", data_len, 
                                        ntohl(dest.sin_addr.s_addr), 
                                        dest.sin_port);
  result = dss_sendto ( mipioi_sd[index], 
                        data,  
                        data_len, 
                        0, 
                        (struct sockaddr *) &dest, 
                        sizeof (struct sockaddr), 
                        &l_errno );

  /*-------------------------------------------------------------------------
    Check to see if the write succeeded.  Since we're writing IP packets
    and UDP datagrams, we can assume that if a write doesn't block, it has
    been fully transmitted over the socket.
  -------------------------------------------------------------------------*/
  if(result == DSS_ERROR)
  {
    if(l_errno == DS_EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        Ask to be notified when write is possible, i.e. ppp is synced
      ---------------------------------------------------------------------*/
      MSG_HIGH ("MIPIO can't send data, registering for write event",0,0,0);
      result = dss_async_select (mipioi_sd[index], DS_WRITE_EVENT, &l_errno);
      if (result != DSS_SUCCESS)
      {
        MSG_HIGH ("Reg for write event failed! (sd=%d)", mipioi_sd[index],0,0);
      }
    }
    else
    {
      MSG_HIGH ("Unexpected write error (%d), session %d",l_errno,index,0);
    }
    return FALSE;
  }

  return TRUE;

} /* mipio_write() */



/*===========================================================================

  FUNCTION MIPIO_PPP_OPEN

  DESCRIPTION
    Starts up the network subsystem (CDMA data service and PPP) over the Um 
    interface for all sockets using the M.IP application ID.  

  PARAMETERS
    None.

  RETURN VALUE
    PPP_OPEN_SUCCESS  - PPP brought up
    PPP_OPEN_FAIL - unable to bring up PPP, PPP open failed
    PPP_IN_SOCKET_CALL - PPP in socket call 

  DEPENDENCIES
    mipio_init() must have been called previously.

  SIDE EFFECTS
    None.

===========================================================================*/
#define BRANCH_TASKFREE() {TASKFREE()

mipio_pppopen_ret_val mipio_ppp_open
(
  void  
)
{
  sint15 l_errno;                        /* return error code              */
  sint15 status;                         /* return status value            */
#ifndef FEATURE_DATA_MM
  word int_save;         /* Local Variable used for saving interrupt state */
#else
  dss_iface_ioctl_id_type ioctl_id; 
#endif /* FEATURE_DATA_MM */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (!mipioi_initialized)
  {
    ERR("PPP failed to open - MIPIO not initialized", 0, 0, 0);
    return PPP_OPEN_FAIL; // should never happen, but if so, make MIP fail
  }

  /*-------------------------------------------------------------------------
    Check to see if there are any active socket apps. If any of the socket
    apps are using the iface, then we should not allow MIP laptop calls
    to proceed. Lock out interrupts from the time we execute this check 
    to the time the PPP_OPEN command is enqueued in the PS command queue. 
    This is to gurantee that MIP laptop calls are not rejected if a sockets
    app sets up call in the interval between this check and enqueuing of the
    PPP_OPEN command. 
  -------------------------------------------------------------------------*/
#ifndef FEATURE_DATA_MM
  INTLOCK_SAV(int_save);
  if(dss_ppp_in_use() != 0)
  {
    INTFREE_SAV(int_save);
    return PPP_IN_SOCKET_CALL;
  }
#endif /* FEATURE_DATA_MM */

#ifdef FEATURE_DATA_MM
  /*-------------------------------------------------------------------------
    Force MIP client to use 1X interface on Um
  -------------------------------------------------------------------------*/
  ioctl_id.name     = DSS_IFACE_IOCTL_CDMA_SN; 
  ioctl_id.instance = 0;

  if (dss_set_specified_iface( mipioi_app_id, &ioctl_id, &l_errno ) < 0 )
  {
    ERR ("Error %d, couldn't fix 1X i/f for Um!", l_errno, 0, 0);
    return PPP_OPEN_FAIL;
  }
#endif /* FEATURE_DATA_MM */

  /*-------------------------------------------------------------------------
    Start PPP on the Um link. 
  -------------------------------------------------------------------------*/
  MSG_HIGH ("Mobile IP (app %d) starting...",mipioi_app_id,0,0);
  status = dss_pppopen(mipioi_app_id, &l_errno);
#ifndef FEATURE_DATA_MM
  INTFREE_SAV(int_save);
#endif /* FEATURE_DATA_MM */
  if (status == DSS_SUCCESS || l_errno == DS_EWOULDBLOCK)
  {
    return PPP_OPEN_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Function dss_pppopen() returned DSS_ERROR, so return FALSE.
  -------------------------------------------------------------------------*/
  MSG_MED("PPP failed to open, errno = %d", l_errno, 0, 0);

  return PPP_OPEN_FAIL;

} /* mipio_ppp_open() */


/*===========================================================================

  FUNCTION MIPIO_PPP_CLOSE

  DESCRIPTION
    Initiates termination to bring down PPP and the traffic channel opened
    by M.IP.  Upon successful close of the network subsystem, invokes the 
    network callback function.

  PARAMETERS
    None.

  RETURN VALUE
    TRUE  - PPP brought down
    FALSE - unable to bring down PPP, PPP close failed

  DEPENDENCIES
    mipio_init() must have been called previously.

  SIDE EFFECTS
    None.

===========================================================================*/
boolean mipio_ppp_close
(
  void  
)
{
  sint15 l_errno;                          /* return error code            */
  sint15 status;                           /* return status value          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (!mipioi_initialized)
  {
    ERR("PPP failed to close - MIPIO not initialized", 0, 0, 0);
    return FALSE;  
  }

  /*-------------------------------------------------------------------------
    Bring down PPP and the traffic channel.  Note that close may occur
    sometime after dss_pppclose(), so return TRUE for those cases as well.
  -------------------------------------------------------------------------*/
  MSG_HIGH ("Mobile IP (app %d) stopping...",mipioi_app_id,0,0);
  status = dss_pppclose(mipioi_app_id, &l_errno);
  if( (status == DSS_SUCCESS) || 
      ( (status == DSS_ERROR) && 
          (l_errno == DS_EWOULDBLOCK || l_errno ==DS_ENETCLOSEINPROGRESS)))
  {
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Function dss_pppclose() returned DSS_ERROR, so return FALSE.
  -------------------------------------------------------------------------*/
  MSG_HIGH("PPP failed to close, errno = %d", l_errno, 0, 0);

  return FALSE;

} /* mipio_ppp_close() */


/*===========================================================================

FUNCTION MIPIO_GET_APP_ID

DESCRIPTION
  Accessor function used to retrieve socket appId

PARAMETERS
  None.

RETURN VALUE
  app_id           application ID
  
DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 mipio_get_app_id
(
  void  
)
{
  return mipioi_app_id;
}

#endif  /* FEATURE_DS_MOBILE_IP */
