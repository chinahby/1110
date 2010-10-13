/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  T C P   F I L E
                   
DESCRIPTION

 The Data Services sockets TCP interface file. This contains all the 
 functions used by the sockets library to access TCP socket function calls.
 
EXTERNALIZED FUNCTIONS
  dsstcp_init_tcb_array():
    This function initializes the array of tcbs to zero. This function 
    should be called at startup. This function is called from the context of 
    the PS task.
                   
  dsstcp_socket()
    TCP specific socket() function.  Sets socket state to INIT.

  dsstcp_connect_setup()
    Sets up specific parameters for TCP sockets.  Called prior 
    dsstcp_connect(), it sets up the appropriate state of the socket
    so that the correct error response will be generated as needed.

  dsstcp_connect()
    Connects a TCP socket.

  dsstcp_listen_setup()
    Allocates child sockets for the backlog of tcp connections for
    passive tcp open.  A message is sent to PS for each of the child
    sockets to open it in passive mode.

  dsstcp_listen()
    Does the passive open for listener socket child sockets. 

  dsstcp_accept()
    Removes and return the a child socket from a listener sockets
    ready queue. This will also try to refill the half open
    connections queue.

  dsstcp_write()
    TCP specific write() function.  Sends the dsm memory item out
    the TCP transport.

  dsstcp_read()
    TCP specific read() function.  Reads specified number of bytes from
    the TCP transport.

  dsstcp_read_dsm_chain()
    TCP specific read_dsm_chain() function.  Reads a DSM item chain from
    the TCP transport.

  dsstcp_close()
    TCP specific close() function.  Initiates the TCP active close.
    If the connection is already closed, cleans up the TCP socket and 
    associated resources.

  dsstcp_abort()
    TCP specific abort() function. Aborts the TCP connection and cleans-up.
    This is usually called when we are dormant.

  dsstcp_get_conn_error()
    Gets the appropriate error code for connection errors.

  dsstcp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.  
    
  dsstcp_lookup_serv()
    Given a connection structure, looks up to determine if there are
    any valid listener sockets(server socket)

  dsstcp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then send a 
    reset to the peer, and then close down sockets and notify the user.       
    
  dsstcp_bind()
    This is the TCP specific bind() function.  It writes the port number into
    the local.conn.port of the socket control block structure.

  Copyright (c) 1998-2009 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.c_v   1.9   13 Feb 2003 12:20:46   ssinghai  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dsstcp.c#6 $ $DateTime: 2009/07/31 07:23:34 $ $Author: alokm $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/31/09    am     Put MSS < MTU check for outgoing SYN in dss_connect.
05/13/09    sp     Fix for calling TASKFREE() in dsstcp_close().
04/03/08    pp     Klocwork fixes.
02/21/08    sn     Fixed lint error.
17/09/07    scb    Added support for the tcp_graceful_dormant_close flag
                   in dsstcp_close()
09/27/04    ks     Changed INTLOCK_SAV() to TASKLOCK() in dsstcp_close().
08/20/04    vp     Removed the ASSERT in dsstcp_shutdown if tcb is NULL to
                   return SUCESS. 
07/28/04    sv     Fixed dsstcp_close to support SILENT_CLOSE option in 
                   socket CLOSING state.
05/17/04    sv     Modified dsstcp_read to read a maximum of DSS_READ_MAX_BYTES
                   in a single read call
11/07/03    sv     Enabled read event if we receive fin from the peer
11/05/03    sv     Added check to make sure we don't enqueue more than
                   sndbuf size of data in tcp sndq.
10/27/03    sv     Passed mss as an additonal argument to open_tcp as the
                   prototype changed.
07/22/03    sv     Fixed the duplicate syn problem.
07/06/03    sv     Modified dsstcp_is_writeable to use sndbuf in scb to
                   determine the number of bytes available in sndq.
06/16/03    ss     Modified dsstcp_close() to return immediately if the 
                   socket state is null, wait for close or closed. 
06/04/03    om     Fixed MSS setting to adjust for IPSec ESP overhead.
05/28/03    ss     Removed SNDQ_LIMIT from dsstcp_is_writeable()
05/13/03    ss     Modified code to insert the scb at the end of the estab
                   queue instead of at front for SYN_RECD->EST transition
05/04/03    ss     Modified state_change_upcall() to support SO_LINGER option
04/24/03    usb    Fixed bug in tcp_read where bytes read were not 
                   getting updated correctly.
04/23/03    ss     Modified code to reflect new socket options handling
04/15/03    sv     Modify dsstcp_read to accept iov and iovcount instead of
                   buffer and length. Added support for tcp half open timer.
04/15/03    ss     Modified dsstcp_close() to transition socket state to 
                   CLOSING only if the socket state is OPEN after close_tcp()
04/10/03    ss     Modified dsstcp_connect() to change the parameter to 
                   getephem_port(). Modified dsstcp_shutdown() to correctly
                   test the value of how parameter.
03/07/03    js     Added event reporting for TCP. 
02/13/03    ss     Added code to support socket shutdown
02/12/03    om     Use SCB's routing cache only.
12/22/02    aku    Changed dsstcp_new_ip_upcall() signature to take 
                   ps_iface_ptr as argument instead of IP addresses.
11/19/02    mvl    removed JCDMA from window size socket option.
11/12/02    ss     Modified dsstcp_abort() so that the clean-up is done when 
                   the socket is CLOSING also.
11/05/02    mvl    Added support for changing default window size.
10/30/02    ss     Moved the ASSERT() later in the code in dsstcp_close() 
                   since tcb_ptr may be NULL for listener socket
10/17/02    ss     When in SynSent state, dsstcp_close() closes the socket
                   right away. data_available flag is reset when TCB is freed
                   so that the user cannot read data from a socket after TCP
                   reset.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
09/25/02    om     Added setting of routing cache for LISTEN sockets.
09/19/02    na     Fixed returning of EEOF when in CLOSE_WAIT state.
09/10/02    aku    Removed global Ip_addr if FEATURE_DATA_MM is defined.
08/16/02    ss     Modified dsstcp_close() to support silent close option
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/19/02    ss     Modified dsstcp_abort() to call dsstcp_close() in all the
                   states except OPEN and OPENING for necessary clean-up.
07/16/02    aku    Fixed the dsm kind names.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
04/17/02    rc     Fixed bug in dsstcp_close() where INTFREE_SAV() was not
                   being called before returning from the function.
03/13/02    pjb    alloc_tcb was leaving tcb in funny state when timer 
                   alloc failed.
03/05/02    pjb    Finished Accept Listen Code Review Items 
02/22/02    pjb    Added accept and listen
02/11/02    ss     Added dsstcpi_data_tx_upcall() to notify application when 
                   sndq can accept more data. Added dsstcp_is_writeable(),
                   accessed via fcn_ptr_table, to find out if sndq has space.
02/01/02    ss     The user now gets an EOF in case of passive close when
                   there is no more data to read.
12/18/01    rsl    Updated tracer field.
12/07/01    ss     Modified dsstcpi_state_change_upcall() not to call 
                   close_tcp() when transitioning to CLOSED_WAIT state
12/07/01    ss     Modified dsstcp_abort() to not to free up scb
11/15/01    aku    dssocki_socket_event_occurred() takes an additonal
                   acb_ptr arg
11/09/01    ss     Changed dsstcpi_state_change_upcall() so that close_timer
                   is started only in FINWAIT2 and TIME_WAIT states. Modified
                   dsstcp_close() to perform TCP close in the PS context.
10/11/01    mt     Add dss_errno argument to dsstcp_connect_setup() and set
                   its contents accordingly. 
07/23/01    pjb    Change location/prefix of command names.   
05/18/01    yll    Removed an unnecessary assert statement in 
                   dsstcp_read_dsm_chain().
04/16/01    js     Added a checking for TCP socket state in 
                   dsstcp_new_ip_upcall() to avoid resetting the socket in
                   inappropriate states. 
03/27/01    mt     Changed type of second argument in dsstcp_read() from
                   byte * to void *.
03/27/01    js     Added extra parameter to function dsstcp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. Though not used, the addition
                   is necessary to comply with dss_fcn_ptr_table format.
02/08/01    yll    Added dsstcp_read_dsm_chain().
11/10/00    snn    Modified dword to uint32 and word to uint16.
                   Modified some comments.
                   Renamed callback functions with a postscript of _cb.
                   Removed errno as the parameter for timer functions, 
                     because they are never used.
                   
10/10/00    na     Added support for Multiple TCP sockets. Added include 
                   files. Removed dsstcp_tcb_ptr because the TCB is now
                   accessed as an array.                                                
                   Added a new array dsstcpi_arr.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. So, a new macro DSM_DS_POOL_SIZE is used to 
                   return pool id based on pool size.
08/03/00    mvl/js Added function dsstcp_bind() to support the protocol
                   specific assignment of local.conn.port.
03/22/00    rc/na  Added function dsstcp_new_ip_upcall() for the case when 
                   ppp resyncs and the IP address changed. In this case, the
                   tcp connection is reset.
11/04/99    rc     Modified function dsstcp_close() to close TCP and clean up
                   if socket is already closed.
09/22/99    rc     Incorporated code review changes and comments.
08/19/99    ak     Removed ARM2.5 compiler warnings.
08/11/99    ak/rc  Added new_ip_upcall, for the case where we do ppp 
                   re-negotiation and get a new IP.  Affects TCP esp.
                   in dormant mode re-originations.  Code for aborting
                   a TCP socket.
04/25/99    hcg    Added ASSERT to dsstcp_read() to guard against NULL tcp
                   receive queue.
04/05/99    hcg    Incorporated code review changes and comments.  Changed 
                   some of the diagnostic message levels.
12/09/98    na/hcg Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DS_SOCKETS
#include "assert.h"
#include "psglobal.h"
#include "netuser.h"
#include "dssocket.h"
#include "dssocki.h"
#include "ps_tcp.h"
#include "ps_iface.h"
#include "ps.h"
#include "dsstcp.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "psi.h"
#include "pstimer.h"
#include "memory.h"

#include "ps_tcp_events.h"
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Array to the TCP control block for the TCP socket. 
---------------------------------------------------------------------------*/

LOCAL struct tcb dsstcpi_arr[ DSS_MAX_TCP_SOCKS];


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  This macro is intended to be used only before return statement in functions
  that have been wrapped in tasklock/taskfree calls
---------------------------------------------------------------------------*/
#define PS_BRANCH_TASKFREE() {TASKFREE()


/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
LOCAL void dsstcpi_data_tx_upcall(struct tcb *tcb_ptr,uint32 cnt);
LOCAL void dsstcpi_data_rx_upcall(struct tcb *tcb_ptr,uint32 cnt);
LOCAL void dsstcpi_state_change_upcall(struct tcb *tcb_ptr,
                                       tcp_state_enum_type old_state,
                                       tcp_state_enum_type new_state);
LOCAL struct tcb *dsstcpi_create_tcb(struct scb_s *scb_ptr); 
LOCAL void dsstcpi_free_tcb(struct tcb* tcb_ptr);
struct scb_s * dsstcpi_alloc_listener(struct scb_s* scb_ptr, 
				      dsm_item_type ** item_ptr);

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSTCPI_HALF_OPEN_TIMER_CB()

DESCRIPTION
  This timer resets and returns half-open tcp connections to the 
  listen state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsstcpi_half_open_timer_cb
(
 void  *in_tcb_ptr                         /* pointer to TCP control block */
)
{
  struct tcb    * tcb_ptr = (struct tcb*)in_tcb_ptr;
  struct scb_s  * lscb_ptr;   /* closing  socket control block             */
  struct scb_s  * scb_ptr;  /* Listening socket control block              */
  struct scb_s ** scb_ptr2;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Get tcb_ptr and make sure it is not NULL.
  -------------------------------------------------------------------------*/
  if( tcb_ptr == NULL)
  {
    ERR("HALF_OPEN_TIMER_CB: NULL TCB \n",0,0,0);
    return;
  }
  lscb_ptr = tcb_ptr->scb_ptr;
  scb_ptr = lscb_ptr->so_head;
  /*-------------------------------------------------------------------------
    Delete the scb from the partial queue
  -------------------------------------------------------------------------*/

  scb_ptr2 = &(scb_ptr->so_partial);
  while(*scb_ptr2 != lscb_ptr) scb_ptr2 = &((*scb_ptr2)->so_partial);
  *scb_ptr2 = lscb_ptr->so_partial;

 
  INTLOCK();
  /*------------------------------------------------------------------------
     Free the half open scb and tcp and allocate new one
  -------------------------------------------------------------------------*/
  setstate(tcb_ptr, TCP_CLOSED);
  dssocki_freescb(lscb_ptr);
  lscb_ptr = dsstcpi_alloc_listener(scb_ptr, NULL);
  INTFREE();

  if (lscb_ptr != NULL)
  {
    lscb_ptr->so_partial = scb_ptr->so_partial;
    scb_ptr->so_partial = lscb_ptr;
    dsstcp_listen(lscb_ptr);
  }
  else
  {
    ERR("HALF_OPEN_TIMER_CB: lscb_ptr is NULL!",0,0,0);
    return;
  }
} /* dsstcpi_half_open_timer_cb() */

/*===========================================================================
FUNCTION DSSTCPI_CLOSE_TIMER_CB()

DESCRIPTION
  This function is invoked when the close timer( 2MSL timeout, etc.), goes
  off for a TCP socket. 
  This function sets the TCP state to CLOSED, which will invoke the 
  CLOSE_EVENT on the interested sockets.
  
  This function is called in the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsstcpi_close_timer_cb
(
  void  *in_tcb_ptr                        /* pointer to TCP control block */
)
{
  struct tcb *tcb_ptr = (struct tcb*)in_tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /*-------------------------------------------------------------------------
     Get tcb_ptr and make sure it is not NULL.
  -------------------------------------------------------------------------*/
  if( tcb_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  MSG_MED(" Close timer off; sockfd=%d; state=%d\n", 
         (int) (tcb_ptr->scb_ptr)->sockfd, 
         (int) tcb_ptr->state,
         0 );

  /*-------------------------------------------------------------------------
    If TCP state is not CLOSED, set it to closed.
  -------------------------------------------------------------------------*/
  if( tcb_ptr->state != TCP_CLOSED)
  {
    setstate( tcb_ptr, TCP_CLOSED);
  }

} /* dsstcpi_close_timer_cb() */

/*===========================================================================
FUNCTION DSSTCPI_CREATE_TCB()

DESCRIPTION
  This function creates a new TCP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL struct tcb *dsstcpi_create_tcb
(
  struct scb_s *scb_ptr                        /* Ptr to socket ctl block. */
)
{
  uint16 loop;                                            /* loop variable */
  static uint16 index =0;                    /* index of last assigned TCB */
  struct tcb *tcb_ptr = NULL;                  /* Ptr to tcb to be created */
  uint32 int_sav;         /* local variable used for saving interrupt state */
  int timer_ret_val;                /* Return value from pstimer functions */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that the input parameters are valid.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL)
  {
      return (NULL);
  }

  /*-------------------------------------------------------------------------
    loop through array to find available tcb - break when one is available.
    Access to the global tcb array is protected through the use of 
    INTLOCK_SAV()/INTFREE_SAV().
  -------------------------------------------------------------------------*/
  for( loop =0; loop< DSS_MAX_TCP_SOCKS; loop++)
  {
    INTLOCK_SAV(int_sav);
    index = (index+1) % DSS_MAX_TCP_SOCKS;
    if ( dsstcpi_arr[ index].scb_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        Assign tcb_ptr.
      ---------------------------------------------------------------------*/
      tcb_ptr = &dsstcpi_arr[index];
      memset( tcb_ptr, 0, sizeof( struct tcb));
      INTFREE_SAV(int_sav);
      break;
    }
    else
    {
      INTFREE_SAV(int_sav);
    }
  }

  /*-------------------------------------------------------------------------
    If no TCP control block is available, return NULL.
  -------------------------------------------------------------------------*/
  if( loop == DSS_MAX_TCP_SOCKS)
  {
    return (NULL);
  }

  /*-------------------------------------------------------------------------
    Now define the timers for the TCP control block.
    TCP needs four timers:
    timer       : For TCP timeouts
    acker       : To send delayed acks
    close_timer : For 2MSL timeouts and other close timeouts.
    half open timer: For detecting failed half-open connections
    Note, it is important to maintain the order of the 4 calls to ps_timer_
    alloc() below.
  -------------------------------------------------------------------------*/
  
  /*-------------------------------------------------------------------------
    Allocate the retransmission timeout.
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_alloc( tcp_timeout_cb, tcb_ptr);
  if( timer_ret_val == PS_TIMER_FAILURE)
  {
    return( NULL);
  }
  else
  {
    tcb_ptr->timer = timer_ret_val;
  }

  /*-------------------------------------------------------------------------
    Allocate the delayed ack timer.
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_alloc( tcp_delayed_cb, tcb_ptr);
  if( timer_ret_val == PS_TIMER_FAILURE)
  {
    timer_ret_val = ps_timer_free( tcb_ptr->timer);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
    return( NULL);
  }
  else
  {
    tcb_ptr->acker = timer_ret_val;
  }

  /*-------------------------------------------------------------------------
    Allocate the close(2MSL) timer.
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_alloc( dsstcpi_close_timer_cb, tcb_ptr);
  if( timer_ret_val == PS_TIMER_FAILURE)
  {
    timer_ret_val = ps_timer_free( tcb_ptr->timer);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

    timer_ret_val = ps_timer_free( tcb_ptr->acker);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

    return( NULL);
  }
  else
  {
    tcb_ptr->close_timer = timer_ret_val;
  }

  /*-------------------------------------------------------------------------
    Allocate the half open timer
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_alloc( dsstcpi_half_open_timer_cb, tcb_ptr);
  if( timer_ret_val == PS_TIMER_FAILURE)
  {
    timer_ret_val = ps_timer_free( tcb_ptr->timer);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

    timer_ret_val = ps_timer_free( tcb_ptr->acker);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

    timer_ret_val = ps_timer_free( tcb_ptr->close_timer);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

    return( NULL);
  }
  else
  {
    tcb_ptr->half_open_timer = timer_ret_val;
  }

  /*-------------------------------------------------------------------------
     Initialize the queues and set the default values.
  -------------------------------------------------------------------------*/
  q_init( &(tcb_ptr->reseq));
  tcb_ptr->state    = TCP_CLOSED;
  tcb_ptr->cwind    = DEF_MSS - scb_ptr->ipsec_hdr_size;
  tcb_ptr->mss      = DEF_MSS - scb_ptr->ipsec_hdr_size;
  tcb_ptr->ssthresh = SSTHRESH;
  tcb_ptr->srtt     = DEF_RTT;  /* mdev = 0 */
  tcb_ptr->minrto   = MIN_RTO;
  tcb_ptr->maxrto   = MAX_RTO;
  tcb_ptr->scb_ptr  = scb_ptr; 

  return (tcb_ptr);

} /* dsstcpi_create_tcb() */

/*===========================================================================
FUNCTION DSSTCPI_FREE_TCB()

DESCRIPTION
  This function frees TCP control block, making it available for re-use.
  
  This function executes in the context of the PS task.
                                                                               
ARGUMENTS:
  tcb_ptr: This is a pointer to the TCP control block that is being freed.
  This argument must point to a valid TCP control block and cannot be NULL.
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the protocol control block of the scb_ptr to NULL.

===========================================================================*/
LOCAL void dsstcpi_free_tcb
(
  struct tcb* tcb_ptr                            /* Ptr to TCB to be freed */
)
{
  dsm_item_type *item_ptr;      /* To hold dsm_items that need to be freed */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  struct scb_s *scb_ptr;              /* Socket control block pointer      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check for valid arguments. tcb_ptr should not be NULL.
  -------------------------------------------------------------------------*/
  if( tcb_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    TCP state for this TCB should always be closed because this function is
    only called when TCP state changes to CLOSED.
  -------------------------------------------------------------------------*/
  ASSERT( tcb_ptr->state == TCP_CLOSED);

  /*-------------------------------------------------------------------------
    Set the protocol control block of the corresponding scb to NULL. This 
    is done to make sure the TCP resources are not held dangling till the 
    socket is freed.
    Note, data_available flag is also set to FALSE. This ensures that the
    app is not able to read data once the TCB is freed.
  -------------------------------------------------------------------------*/
  scb_ptr = tcb_ptr->scb_ptr;
  scb_ptr->data_available = FALSE;
  scb_ptr->protocol_ctl_blk.tcb = NULL;

  /*-------------------------------------------------------------------------
    Cleanup the socket timers for this tcb.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_free( tcb_ptr->timer);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

  timer_ret_val = ps_timer_free( tcb_ptr->acker);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

  timer_ret_val = ps_timer_free( tcb_ptr->close_timer);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

  timer_ret_val = ps_timer_free( tcb_ptr->half_open_timer);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

  /*-------------------------------------------------------------------------
    Cleanup the sendq and recvq for TCP
  -------------------------------------------------------------------------*/
  dsm_free_packet( &(tcb_ptr->sndq));
  dsm_free_packet( &(tcb_ptr->rcvq));

  /*-------------------------------------------------------------------------
    Cleanup TCP reseq. queue.
  -------------------------------------------------------------------------*/
  while(( item_ptr = (dsm_item_type *)q_get( &(tcb_ptr->reseq))) != NULL)
  {
    (void)dsm_free_packet( &item_ptr);
  }

  /*-------------------------------------------------------------------------
    set everything to NULL
  -------------------------------------------------------------------------*/
  memset( tcb_ptr, 0, sizeof( struct tcb));
                          
} /* dsstcpi_free_tcb() */

/*===========================================================================
FUNCTION DSSTCPI_DATA_RX_UPCALL()

DESCRIPTION
  This function will be invoked by the TCP input routine upon completion of
  TCP processing of received payload data bytes. 

DEPENDENCIES
  This function must not remove more than 'rcvcnt' bytes from the TCP Receive
  queue.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsstcpi_data_rx_upcall
(
  struct tcb *tcb_ptr,         /* pointer to TCP control block which has a
                                  list head pointer to payload data        */
  uint32      cnt               /* The number of data payload data bytes   */
)
{
  struct scb_s *scb_ptr;                        /* Ptr to socket ctl block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_LOW(" TCP has data available - call socket cb", 0, 0, 0);

  /*-------------------------------------------------------------------------
    READ EVENT has transition for this socket( dss_read() would now proceed).
    Call function which will call the callback if the application is
    interested in the event. 
  -------------------------------------------------------------------------*/
  scb_ptr = tcb_ptr->scb_ptr; 

  if ( cnt > 0)
  {
    if ( scb_ptr->state_flags & DSSOCKI_CANTRECVMORE )
    {
      dsm_free_packet( &(tcb_ptr->rcvq) );
      tcb_ptr->rcvcnt = 0;
      scb_ptr->data_available = FALSE;

      return;
    }
    /*-----------------------------------------------------------------------
      There are bytes in the data payload, so indicate that a socket event
      has occurred.  
    -----------------------------------------------------------------------*/
    scb_ptr->data_available = TRUE;
    dssocki_socket_event_occurred( scb_ptr, NULL);
  }
  else
  {
    /*-----------------------------------------------------------------------
      No data payload.
    -----------------------------------------------------------------------*/
    MSG_MED(" TCP data rx cb called without data", 0, 0, 0);
  }

} /* dsstcpi_data_rx_upcall() */


/*===========================================================================
FUNCTION DSSTCPI_DATA_TX_UPCALL()

DESCRIPTION
  This function will be invoked by the TCP input routine when an 
  ack is received acknowledging non-zero bytes in the sndq. Essentially, it
  indicates that there is space for at least cnt number of bytes on the 
  sndq.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsstcpi_data_tx_upcall
(
  struct tcb *tcb_ptr,         /* pointer to TCP control block which has a
                                  list head pointer to payload data        */
  uint32      cnt               /* The number of data payload data bytes   */
)
{
  struct scb_s *scb_ptr;                        /* Ptr to socket ctl block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_LOW(" TCP data got acked - call socket cb", 0, 0, 0);

  /*-------------------------------------------------------------------------
    READ EVENT has transition for this socket( dss_read() would now proceed).
    Call function which will call the callback if the application is 
    interested in the event.
  -------------------------------------------------------------------------*/
  scb_ptr = tcb_ptr->scb_ptr; 

  if ( cnt > 0)
  {
    /*-----------------------------------------------------------------------
      Some bytes have been acknowledged. Perhaps, the socket can write more
      data. Indicate that a socket event has occurred.  
    -----------------------------------------------------------------------*/
    scb_ptr->sndcnt -= cnt;
    dssocki_socket_event_occurred( scb_ptr, NULL);
  }
  else
  {
    /*-----------------------------------------------------------------------
      No data payload.
    -----------------------------------------------------------------------*/
    MSG_MED(" TCP data tx cb called without data", 0, 0, 0);
  }

} /* dsstcpi_data_tx_upcall() */


/*===========================================================================
FUNCTION DSSTCPI_STATE_CHANGE_UPCALL()

DESCRIPTION
  This function will be invoked by the TCP input routine when the TCP state
  changes when in sockets mode. This function will use the old and the new 
  TCP states to determine the processing requirements.  For sockets, 
  this function currently does not actually use these values, but rather 
  retains them to support backward compatibility to existing TCP code.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsstcpi_state_change_upcall
(
  struct tcb *tcb_ptr,         /* pointer to TCP control block which has a
                                  list head pointer to payload data        */
  tcp_state_enum_type old_state,  /* previous TCP state                    */
  tcp_state_enum_type new_state   /* new TCP state                         */
)
{
  struct scb_s* scb_ptr;     /* Ptr to socket control block for the socket */
  struct scb_s** scb_ptr_ptr;             /* Ptr for searching socket lists */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  uint32 int_save;       /* local variable used for saving interrupt state */
  tcp_event_payload_type    tcp_event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  ASSERT( tcb_ptr != NULL);
  scb_ptr =  tcb_ptr->scb_ptr;
  ASSERT( scb_ptr != NULL);
  
  MSG_MED("TCP socket=%d oldstate=%d newstate=%d", 
          tcb_ptr->scb_ptr->sockfd, (int) old_state, (int) new_state);

  /*-------------------------------------------------------------------------
    Do processing for relevant TCP state changes only.  Note, this block
    will generate a number of Lint errors, as the state enum is defined 
    within the tcb structure in tcp.h.  This was maintained so as to reduce
    risk of changing any legacy tcp code.
  -------------------------------------------------------------------------*/
  switch(tcb_ptr->state)
  { 
    case TCP_SYN_RECEIVED:
      /*---------------------------------------------------------------------
        Here we need to set a timer to detect failed halfopen connections
      ---------------------------------------------------------------------*/
      timer_ret_val = ps_timer_start( tcb_ptr->half_open_timer, 
                                      DSS_HALF_OPEN_INTERVAL
                                    );
      ASSERT(timer_ret_val != PS_TIMER_FAILURE);

      /*---------------------------------------------------------------------
          Generate TCP opening event
      ---------------------------------------------------------------------*/
      if (old_state != new_state) 
      {
        tcp_event_payload.sock_fd = scb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_OPENING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);
      }

      break;

    case TCP_ESTABLISHED:
      /*---------------------------------------------------------------------
            TCP now Established. Set socket state to OPEN
      ---------------------------------------------------------------------*/
      dssocki_set_sock_state(scb_ptr, DSSOCKI_OPEN);

      /*---------------------------------------------------------------------
          Generate TCP open event
      ---------------------------------------------------------------------*/
      if (old_state != new_state) 
      {
        tcp_event_payload.sock_fd = scb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_OPEN;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);

      }

      /*---------------------------------------------------------------------
        Where are we coming from: If this socket is halfopen (SYN
        sent) then we need to move it to the open queue and generate a
        socket event on the listener socket.

      ---------------------------------------------------------------------*/
      if(old_state == TCP_SYN_RECEIVED) 
      {
        ASSERT(scb_ptr->so_head != NULL);
        /*------------------------------------------------------------------
           Clear the timer
        -------------------------------------------------------------------*/
        timer_ret_val = ps_timer_cancel(tcb_ptr->half_open_timer);
        ASSERT(timer_ret_val != PS_TIMER_FAILURE);
        /*------------------------------------------------------------------
          Find this socket on the parents queue and move it to the open queue.
        -------------------------------------------------------------------*/
        INTLOCK();
        scb_ptr_ptr = &(scb_ptr->so_head->so_partial);
        while((*scb_ptr_ptr) != scb_ptr)
        {   
          /*----------------------------------------------------------------
            If you hit this assert the so_head is corrupt on the current
            item.
          ----------------------------------------------------------------*/
          ASSERT((*scb_ptr_ptr)->so_partial != NULL);
          scb_ptr_ptr = &((*scb_ptr_ptr)->so_partial);
        }   
        (*scb_ptr_ptr) = (*scb_ptr_ptr)->so_partial; 
        scb_ptr->so_partial = NULL;

        /*---------------------------------------------------------------------
          Insert the scb to the end of the established queue
        ---------------------------------------------------------------------*/
        scb_ptr_ptr = &(scb_ptr->so_head);
        while ((*scb_ptr_ptr)->so_estab != NULL)
        {
          scb_ptr_ptr = &((*scb_ptr_ptr)->so_estab);
        }
        scb_ptr->so_estab = NULL;
        (*scb_ptr_ptr)->so_estab = scb_ptr;

        /*---------------------------------------------------------------------
          Set the data_available flag in the LISTENER socket.  This will
          cause the LISTEN event to post when we call event occured. 
        ---------------------------------------------------------------------*/
        scb_ptr->so_head->data_available = TRUE;
        INTFREE();
        dssocki_socket_event_occurred(scb_ptr->so_head, NULL);
      }

      break;

    case TCP_CLOSED:

      /*---------------------------------------------------------------------
          Generate TCP closed event
      ---------------------------------------------------------------------*/
      if (old_state != new_state) 
      {
        tcp_event_payload.sock_fd = scb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_CLOSED;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);

      }
      /*---------------------------------------------------------------------
        This TCP is now closed.
        Set the appropriate closing reason for the socket and then free the 
        TCB.
      ---------------------------------------------------------------------*/

      TASKLOCK();

      dssocki_set_closed_state_reason(scb_ptr, tcb_ptr->reason);
      dsstcpi_free_tcb( tcb_ptr);

      /*---------------------------------------------------------------------
        If socket is CLOSING, set state to WAIT_FOR_CLOSE.  Otherwise, set
        it to CLOSED.
      ---------------------------------------------------------------------*/
      if( scb_ptr->socket_state == DSSOCKI_CLOSING)
      {
        dssocki_set_sock_state( scb_ptr, DSSOCKI_WAIT_FOR_CLOSE);
      }
      else if ( (scb_ptr->socket_state == DSSOCKI_OPEN) ||
                (scb_ptr->socket_state == DSSOCKI_OPENING))
      {
        dssocki_set_sock_state( scb_ptr, DSSOCKI_CLOSED);
      }
      else
      {
        MSG_HIGH(" TCP closed from unknown socket state",0 ,0, 0);
      }

      TASKFREE();

      /*---------------------------------------------------------------------
        If the socket is lingering then we want to handle the socket close
        internally otherwise we do a realtime check for socket events and
        notify the application.
      ---------------------------------------------------------------------*/
      if ( scb_ptr->state_flags & DSSOCKI_LINGER )
      {
        dssocki_process_lingering_close(&scb_ptr);
      }
      else
      {
        dssocki_socket_event_occurred(scb_ptr, NULL);
      }

      break;

    case TCP_FINWAIT2:
      /*---------------------------------------------------------------------
        In these states, start state timer to clean up queues. Note that we 
        want to start the timer for FINWAIT2 as well so that we don't hang
        in this state forever if the peer crashed after acking our FIN and
        before it could send its FIN.
      ---------------------------------------------------------------------*/
      timer_ret_val = ps_timer_start( tcb_ptr->close_timer, 
                                      DSS_LONG_CLOSE_INTERVAL
                                    );

      if( timer_ret_val == PS_TIMER_FAILURE)
      {
        ASSERT( 0);
        dsstcpi_close_timer_cb( tcb_ptr);
      }
      break;

    case TCP_TIME_WAIT: 
      /*---------------------------------------------------------------------
        The other side will not send any more data. If there is nothing to be
        read by the application, we set the data_available flag so that 
        the application gets to know about the EOF.
      ---------------------------------------------------------------------*/
      INTLOCK_SAV(int_save);
      if (tcb_ptr->rcvcnt == 0)
      {
        scb_ptr->data_available = TRUE;
        INTFREE_SAV(int_save);
        dssocki_socket_event_occurred( scb_ptr, NULL);
      }
      else
      {
        INTFREE_SAV(int_save);
      }
      
      /*---------------------------------------------------------------------
        In these states, start state timer to clean up queues. Note that we 
        want to start the timer for FINWAIT2 as well so that we don't hang
        in this state forever if the peer crashed after acking our FIN and
        before it could send its FIN.
      ---------------------------------------------------------------------*/
      timer_ret_val = ps_timer_start( tcb_ptr->close_timer, 
                                      DSS_LONG_CLOSE_INTERVAL
                                    );

      if( timer_ret_val == PS_TIMER_FAILURE)
      {
        ASSERT( 0);
        dsstcpi_close_timer_cb( tcb_ptr);
      }
      break;

    case TCP_CLOSE_WAIT:

      /*---------------------------------------------------------------------
          Generate TCP closing event
      ---------------------------------------------------------------------*/
      if (old_state != new_state) 
      {
        tcp_event_payload.sock_fd = scb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_CLOSING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);

      }

      /*---------------------------------------------------------------------
        The other side will not send any more data. If there is nothing to be
        read by the application, we set the data_available flag so that 
        the application gets to know about the EOF.
      ---------------------------------------------------------------------*/
      INTLOCK_SAV(int_save);
      if (tcb_ptr->rcvcnt == 0)
      {
        scb_ptr->data_available = TRUE;
        INTFREE_SAV(int_save);
        dssocki_socket_event_occurred( scb_ptr, NULL);
      }
      else
      {
        INTFREE_SAV(int_save);
      }
      
      break;

    case TCP_SYN_SENT:
      /*---------------------------------------------------------------------
          Generate TCP opening event
      ---------------------------------------------------------------------*/
      if (old_state != new_state) 
      {
        tcp_event_payload.sock_fd = scb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_OPENING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);
      }
      break;
    
    case TCP_FINWAIT1:
      /*---------------------------------------------------------------------
          Generate TCP CLOSING event
      ---------------------------------------------------------------------*/
      if (old_state != new_state) 
      {
        tcp_event_payload.sock_fd = scb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_CLOSING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);
      }

      break;
   
    case TCP_CLOSING:
      /*---------------------------------------------------------------------
        The other side will not send any more data. If there is nothing to be
        read by the application, we set the data_available flag so that 
        the application gets to know about the EOF.
      ---------------------------------------------------------------------*/
      INTLOCK_SAV(int_save);
      if (tcb_ptr->rcvcnt == 0)
      {
        scb_ptr->data_available = TRUE;
        INTFREE_SAV(int_save);
        dssocki_socket_event_occurred( scb_ptr, NULL);
      }
      else
      {
        INTFREE_SAV(int_save);
      }
      break;
   
    case TCP_LAST_ACK:
    case TCP_LISTEN:

      /*---------------------------------------------------------------------
        In these states, do nothing.
      ---------------------------------------------------------------------*/
      break;
    
    default:
      ASSERT(0);
  } /* switch */

} /* dsstcpi_state_change_upcall() */

/*===========================================================================
FUNCTION DSSTCPI_ALLOC_LISTENER()

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
struct scb_s *
dsstcpi_alloc_listener
(
  struct scb_s* scb_ptr,
  dsm_item_type ** item_ptr
)
{
  struct scb_s* lscb_ptr;       /* Ptr to new scb's as they are allocated */
  if((lscb_ptr = dssocki_allocscb()) == NULL)
  {
    return NULL;
  }
  lscb_ptr->so_head = scb_ptr;
  lscb_ptr->acb_ptr = scb_ptr->acb_ptr;
  lscb_ptr->fcn_ptr_table = scb_ptr->fcn_ptr_table;
  lscb_ptr->localname = scb_ptr->localname;
  lscb_ptr->family = AF_INET;
  lscb_ptr->type = SOCK_STREAM;
  lscb_ptr->protocol = IPPROTO_TCP;
  dssocki_set_sock_state(lscb_ptr, DSSOCKI_INIT);
  
  /*----------------------------------------------------------------------
    Allocate TCB
  ----------------------------------------------------------------------*/
  if((lscb_ptr->protocol_ctl_blk.tcb = 
      dsstcpi_create_tcb(lscb_ptr)) == 
     NULL)
  {
    dssocki_freescb(lscb_ptr);
    return NULL;
  }
  
  /*----------------------------------------------------------------------
    Allocate the dsm message here if the item pointer is not null
  ----------------------------------------------------------------------*/
  if(item_ptr != NULL)
  {
    if(NULL ==
       (*item_ptr =
	dsm_new_buffer( DSM_DS_POOL_SIZE(sizeof( struct scb_s)))))
    {
      ERR("Running out of buffers",0,0,0);
      dsstcpi_free_tcb(lscb_ptr->protocol_ctl_blk.tcb);
      dssocki_freescb(lscb_ptr);
      return NULL;
    }
  }

  return lscb_ptr;
} /* dsstcpi_alloc_listener */

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSSTCP_INIT_TCB_ARRAY()

DESCRIPTION
  This function initializes the array of tcbs to zero.This function should 
  be called at startup.
  
  This function is called from the context of the PS task.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsstcp_init_tcb_array
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize entire tcb array to be zero.
  -------------------------------------------------------------------------*/
  memset( dsstcpi_arr, 0, sizeof( dsstcpi_arr));

}  /* dsstcp_init_tcb_array() */


/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.
  
  This function is called from the context of the application task.

DEPENDENCIES
  None

RETURN VALUE
  On success, returns DSS_SUCCESS
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_EMFILE           too many descriptors open.  A socket is already open or 
                      has not closed compeletely.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dsstcp_socket
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *errno                                   /* error condition value */
)
{

  struct tcb *tcb_ptr;                           /* TCP  control block ptr */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL) 
  {
    *errno = DS_EFAULT;
    return (DSS_ERROR);
  }
  /*-------------------------------------------------------------------------
    Check to see that protocol matches socket type.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->protocol != IPPROTO_TCP) && (scb_ptr->protocol != 0) )
  {
    *errno = DS_EPROTOTYPE;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Create TCP control block.
  -------------------------------------------------------------------------*/
  if ( (tcb_ptr = dsstcpi_create_tcb( scb_ptr)) == NULL)
  {
    MSG_HIGH(" Could not create a TCB", 0, 0, 0);
    *errno = DS_EMFILE;
    return (DSS_ERROR);
  }

  scb_ptr->protocol_ctl_blk.tcb = tcb_ptr;

  /*-------------------------------------------------------------------------
    Set socket state to INIT
  -------------------------------------------------------------------------*/
  dssocki_set_sock_state(scb_ptr, DSSOCKI_INIT);

  return (DSS_SUCCESS);
  
}  /* dsstcp_socket() */

/*===========================================================================
FUNCTION DSSTCP_CONNECT_SETUP()

DESCRIPTION
  This function sets up specific parameters for TCP sockets, and should
  be called prior to making a call to the TCP connect function.  Specifically
  it sets the appropriate state of the socket, within the context of the
  calling application, so that the correct error responses will be generated
  as needed.
  This function is called from the App task context.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS if connection setup succeeds. 
  DSS_ERROR if arguments are invalid of if connection setup fails. 

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_connect_setup
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *dss_errno                    /* address of error condition value */ 
)
{
   dsm_item_type *item_ptr;                      /* item to hold msg to PS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL || dss_errno == NULL) 
  {
    ASSERT(0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    If in INIT state, set the state to OPENING
  -------------------------------------------------------------------------*/
  if (scb_ptr->socket_state == DSSOCKI_INIT)
  {
    dssocki_set_sock_state( scb_ptr, DSSOCKI_OPENING);
  }

  /*-------------------------------------------------------------------------
    Put the scb in an item and send it to the PS task. This ensures that the
    connect happens in the context of the PS task.
  -------------------------------------------------------------------------*/
  item_ptr = dsm_new_buffer( DSM_DS_POOL_SIZE(sizeof( struct scb_s)));
  if (item_ptr == NULL)
  {
    ERR("Running out of buffers",0,0,0);
    *dss_errno = DS_ENOMEM;
    return(DSS_ERROR);
  }

#ifdef FEATURE_DSM_MEM_CHK
  item_ptr->tracer = DSM_SCKT_TO_PS;
#endif //FEATURE_DSM_MEM_CHK

  dssocki_send_to_ps( scb_ptr, item_ptr, NULL, DSM_PS_SOCKET_CONNECT);
  *dss_errno = DS_EWOULDBLOCK;
  return (DSS_ERROR);

} /* dsstcp_connect_setup() */

/*===========================================================================
FUNCTION DSSTCP_CONNECT()

DESCRIPTION
  This function connects a TCP socket and should only be called when 
  PPP/traffic channel is established.
    
  This function is called from the context of the PS  task.

DEPENDENCIES
  Before this function is called, the destination IP address/port number
  should have been put in the scb in the name and peername fields.

RETURN VALUE
  On success, returns DSS_SUCCESS.
  On failure returns DSS_ERROR

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_connect
(
  struct scb_s* scb_ptr      /* Ptr to socket control block for the socket */
)
{
   struct tcb* tcb_ptr;                       /* Ptr. to TCP control block */
   struct connection sock_conn;                 /* Local/Remote connection */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED(" Connecting socket TCP", 0, 0, 0);

  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL) 
  {
    MSG_HIGH(" SCB pointer is bad", 0, 0, 0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Create the TCP control block and get the TCP control block ptr. Assign 
    it to the scb's protocol control block.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    If address not bound to socket, assign it the local IP
  -------------------------------------------------------------------------*/
  if (scb_ptr->localname.address == INADDR_ANY)
  {
    sock_conn.local.address = 
      PS_IFACE_IP_V4_ADDR(scb_ptr->routing_cache);
    scb_ptr->localname.address  = sock_conn.local.address;
  }

  /*-------------------------------------------------------------------------
    If socket is not already attached to a local address port, assign 
    ephemeral port.
  -------------------------------------------------------------------------*/
  if (scb_ptr->localname.port == 0)
  {
    scb_ptr->localname.port = dssocki_getephem_port(scb_ptr->protocol);
    scb_ptr->localname.port = dss_ntohs(scb_ptr->localname.port);
    sock_conn.local.port = scb_ptr->localname.port;
  }
  else
  {
    sock_conn.local.port    = scb_ptr->localname.port;
  }

  sock_conn.remote        = scb_ptr->servname;

  /*-------------------------------------------------------------------------
    create the TCP control block (TCB).
  -------------------------------------------------------------------------*/
  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
  ASSERT(tcb_ptr != NULL);  

  /*-------------------------------------------------------------------------
    Setup the TCP control block callback functions.
  -------------------------------------------------------------------------*/
  tcb_ptr->r_upcall   = dsstcpi_data_rx_upcall;
  tcb_ptr->s_upcall   = dsstcpi_state_change_upcall;
  tcb_ptr->t_upcall   = dsstcpi_data_tx_upcall;

  /*-------------------------------------------------------------------------
    Open an active TCP connection.
  -------------------------------------------------------------------------*/
  MSG_HIGH( "TCP connect() using iface 0x%x", scb_ptr->routing_cache, 0, 0 );
  tcb_ptr->routing_cache = scb_ptr->routing_cache;

  /*-------------------------------------------------------------------------
    Check MSS value to be negotiated should be relative to MRU/MTU.
  -------------------------------------------------------------------------*/
  tcb_ptr->mss = MIN(tcb_ptr->mss, 
                   tcb_ptr->routing_cache->v4_net_info.mtu - IPLEN - TCPLEN);

  tcb_ptr = open_tcp(&(sock_conn.local),
                     &(sock_conn.remote), 
                     TCP_ACTIVE,
                     scb_ptr->rcvbuf,
                     tcb_ptr->mss,
                     0,
                     -1,
                     tcb_ptr);

  /*-------------------------------------------------------------------------
    See if the TCP connection was successful
  -------------------------------------------------------------------------*/
  if (tcb_ptr == NULL)
  {
    ERR("Could not open TCP connection",0,0,0);
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Set the socket control block's protocol control block 
  -------------------------------------------------------------------------*/
  scb_ptr->protocol_ctl_blk.tcb = tcb_ptr;
  
  return(DSS_SUCCESS);

} /* dsstcp_connect() */

/*===========================================================================
FUNCTION DSSTCP_LISTEN_SETUP()

DESCRIPTION
  This starts a passive open on a tcp socket. 
  This is called from the context of the application.
 
  This allocates TCB and SCB's for each item in the backlog, then
  tells the TCP stack to passively open for each.

DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.

RETURN VALUE 
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

  dss_errno Values
  ----------------
  DS_EINVAL     This indicates that scb_ptr is not set.  This is a 
                        violation of the preconditions for calling this
            function.
  DS_ENOMEM     There are not enough socket control blocks or tcp 
                        control blocks to fill the back log.  
  
SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_listen_setup
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 backlog,   
  sint15 * dss_errno
)
{ 
  /* item to hold msg to PS */
  dsm_item_type *item_ptr[DSS_SOMAXCONN] = { NULL, }; 
  
  struct scb_s* lscb_ptr;       /* Ptr to new scb's as they are allocated */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL) 
  {
    ASSERT(0);  
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    If in INIT state, set the state to LISTENing
  -------------------------------------------------------------------------*/
  if (scb_ptr->socket_state == DSSOCKI_INIT)
  {
    dssocki_set_sock_state( scb_ptr, DSSOCKI_LISTEN);
  }

  /*-------------------------------------------------------------------------
    Set QUEUE length 
  -------------------------------------------------------------------------*/
  scb_ptr->so_qlimit = backlog;

  /*-------------------------------------------------------------------------
    Release the TCB as this is now a listener socket and doesn't have
    its own tcp context.
  -------------------------------------------------------------------------*/
  dsstcpi_free_tcb(scb_ptr->protocol_ctl_blk.tcb);

  /*-------------------------------------------------------------------------
    Loop backlog times allocating scb's and tcb's and item_ptrs.
  -------------------------------------------------------------------------*/
  while(backlog > 0) 
  {
    if(NULL == 
       (lscb_ptr = dsstcpi_alloc_listener(scb_ptr, &(item_ptr[backlog-1]))))
    {
      break;
    }
       
    lscb_ptr->so_partial = scb_ptr->so_partial;
    scb_ptr->so_partial = lscb_ptr;

    backlog--;
  }

  /*-------------------------------------------------------------------------
    If backlog is not 0 then a memory allocation failed somewhere. -
    We need to free the resources we just  allocated.
  -------------------------------------------------------------------------*/
  if(backlog != 0)
  {
    while(scb_ptr->so_partial)
    {
      lscb_ptr = scb_ptr->so_partial;
      scb_ptr->so_partial = lscb_ptr->so_partial;
      if(lscb_ptr->protocol_ctl_blk.tcb != NULL)
      {
    dsstcpi_free_tcb(lscb_ptr->protocol_ctl_blk.tcb);
      }
      dssocki_freescb(lscb_ptr);
      dsm_free_packet(&item_ptr[backlog]);
      backlog ++;
    }
    
    *dss_errno = DS_ENOMEM;
    return (DSS_ERROR);
  }
  
  /*-------------------------------------------------------------------------
    Put the scb in an item and send it to the PS task. This ensures that the
    listen happens in the context of the PS task.
  -------------------------------------------------------------------------*/
  lscb_ptr = scb_ptr->so_partial;
  INTLOCK();
  while(lscb_ptr != NULL)
  {
    dssocki_send_to_ps( lscb_ptr, 
                        item_ptr[backlog++], 
                        NULL, 
                        DSM_PS_SOCKET_LISTEN);
    lscb_ptr = lscb_ptr->so_partial; 
  }
  INTFREE();
  *dss_errno = DS_EEOF;
  return (DSS_SUCCESS);
} /* dsstcp_listen_setup() */


/*===========================================================================
FUNCTION DSSTCP_LISTEN()

DESCRIPTION
  This starts a passive open on a tcp socket.
  This is called from the context of the protocol stack

DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.

RETURN VALUE 
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_listen
(
  struct scb_s* scb_ptr     /* Ptr to socket control block for the socket */
)
{
  struct tcb* tcb_ptr;                       /* Ptr. to TCP control block */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED(" Listening socket TCP", 0, 0, 0);

  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL) 
  {
    MSG_HIGH(" SCB pointer is bad", 0, 0, 0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Get the TCP control block (TCB).
  -------------------------------------------------------------------------*/
  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
  ASSERT(tcb_ptr != NULL);  

  /*-------------------------------------------------------------------------
    Setup the TCP control block callback functions.
  -------------------------------------------------------------------------*/
  tcb_ptr->r_upcall   = dsstcpi_data_rx_upcall;
  tcb_ptr->s_upcall   = dsstcpi_state_change_upcall;
  tcb_ptr->t_upcall   = dsstcpi_data_tx_upcall;

  /*-------------------------------------------------------------------------
    Open an passive TCP connection.
  -------------------------------------------------------------------------*/
  tcb_ptr = open_tcp(&(scb_ptr->localname),
                     NULL, 
                     TCP_PASSIVE,
                     scb_ptr->rcvbuf,
                     tcb_ptr->mss,
                     0,
                     -1,
                     tcb_ptr);

  /*-------------------------------------------------------------------------
    See if the TCP connection was successful
  -------------------------------------------------------------------------*/
  if (tcb_ptr == NULL)
  {
    ERR("Could not open TCP connection",0,0,0);
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Set the socket control block's protocol control block 
  -------------------------------------------------------------------------*/
  scb_ptr->protocol_ctl_blk.tcb = tcb_ptr;

  return (DSS_SUCCESS);
} /* dsstcp_listen() */


/*===========================================================================
FUNCTION DSSTCP_ACCEPT()

DESCRIPTION
  This is called from the context of the application to accept a new
  connection.  

  Verify that there is a connection waiting, otherwise return
  EWOULDBLOCK.  Take the first scb from
  the queue return it.  Allocate another item for the
  queue.  Return this item to the user.


DEPENDENCIES

  The scb that this is called with should be in the LISTEN state, and
  the ACCEPT_EVENT should have been posted.

RETURN VALUE
  The file descriptor of the new connection is returned on success. 

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_accept
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct sockaddr* remote_addr,
  uint16 * addrlen,
  sint15 * dss_errno
)
{
  /* item to hold msg to PS */
  dsm_item_type *item_ptr[DSS_SOMAXCONN] = { NULL, }; 
  struct scb_s* lscb_ptr;               /* Ptr to new scb's to be returned */
  struct scb_s* lscb_ptr2;       /* Ptr to new scb's as they are allocated */
  struct scb_s** lscb_ptr_ptr;          /* Ptr ptr to hold refill position */
  uint16 backlog;                /* A copy of qlimit (ie. backlog) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL) 
  {
    MSG_HIGH(" SCB pointer is bad", 0, 0, 0);
    return (DSS_ERROR);
  }

  INTLOCK();
  /*------------------------------------------------------------------------
     Verify that there is something on the queue.  If so take it off
     and reset the data available flag if the queue is now empty.
  -------------------------------------------------------------------------*/
  if ((lscb_ptr = scb_ptr->so_estab) != NULL) 
  {
    if((scb_ptr->so_estab = scb_ptr->so_estab->so_estab) == NULL)
    {
      scb_ptr->data_available = FALSE;
    }
  }
  INTFREE();
  /*-------------------------------------------------------------------------
     Try to fill the listen queue again.  First find the # we need to
     allocate.
  -------------------------------------------------------------------------*/
  backlog = scb_ptr->so_qlimit;

  INTLOCK();

  lscb_ptr_ptr = &(scb_ptr->so_partial);
  while((backlog > 0) && (*lscb_ptr_ptr) != NULL)
  {
    lscb_ptr_ptr = &((*lscb_ptr_ptr)->so_partial);
    backlog --; 
  }

  INTFREE();

  /*-------------------------------------------------------------------
    Allocate SCB and fill it in with info from listener socket.
  --------------------------------------------------------------------*/
  if(backlog > 0)
  {
    lscb_ptr_ptr = &lscb_ptr2;
    while(backlog > 0)
    {
      /*-------------------------------------------------------------------
        Allocate SCB and fill it in with info from listener socket.
      --------------------------------------------------------------------*/
      if(((*lscb_ptr_ptr) = 
          dsstcpi_alloc_listener(scb_ptr,&(item_ptr[backlog-1])))
         == NULL)
      {
        break;
      }
      lscb_ptr_ptr = &((*lscb_ptr_ptr)->so_partial);
      backlog--;
    }
    /*--------------------------------------------------------------------
      Append our new list to the old one and send the messages to
      PS.
    ----------------------------------------------------------------------*/
    INTLOCK();
    lscb_ptr_ptr = &(scb_ptr->so_partial);
    while(*lscb_ptr_ptr != NULL)
    {
      lscb_ptr_ptr = &((*lscb_ptr_ptr)->so_partial);
    }
    (*lscb_ptr_ptr) = lscb_ptr2;
    while(*lscb_ptr_ptr != NULL)
    {
      ASSERT(backlog < DSS_SOMAXCONN);
      dssocki_send_to_ps( (*lscb_ptr_ptr), 
                          item_ptr[backlog++], 
                          NULL, 
                          DSM_PS_SOCKET_LISTEN);
      lscb_ptr_ptr = &((*lscb_ptr_ptr)->so_partial);
    }
    INTFREE();
  }
  /*-------------------------------------------------------------------------
     If we have nothing to return: If the partial queue is empty return
     ENOMEM, otherwise return EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if (lscb_ptr == NULL)
  {
    *dss_errno = (scb_ptr->so_partial == NULL) ? DS_ENOMEM : DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Fill in the local and the peer address in the scb
    -----------------------------------------------------------------------*/
    lscb_ptr->localname.address = 
      lscb_ptr->protocol_ctl_blk.tcb->conn.local.address;
    lscb_ptr->localname.port = 
      lscb_ptr->protocol_ctl_blk.tcb->conn.local.port;

    lscb_ptr->servname.address = 
      lscb_ptr->protocol_ctl_blk.tcb->conn.remote.address;
    lscb_ptr->servname.port = 
      lscb_ptr->protocol_ctl_blk.tcb->conn.remote.port;

    /*-----------------------------------------------------------------------
      The listening TCB's routing cache points to the interface we received
      the SYN on. Preserve that info in the handling socket's SCB.
    -----------------------------------------------------------------------*/
    lscb_ptr->routing_cache = lscb_ptr->protocol_ctl_blk.tcb->routing_cache;
    MSG_MED( "Accept: Setting RT cache[%d] to 0x%x", 
              lscb_ptr->sockfd, lscb_ptr->routing_cache, 0 );

  }

  /*-------------------------------------------------------------------------
     If remote_addr/addrlen is set extract that from the tcb and return it
  -------------------------------------------------------------------------*/
  if((remote_addr != NULL) && (addrlen != NULL))
  {
    ((struct sockaddr_in*) remote_addr)->sin_family = AF_INET;
    ((struct sockaddr_in*) remote_addr)->sin_port =
      dss_htons (lscb_ptr->protocol_ctl_blk.tcb->conn.remote.port);
    ((struct sockaddr_in*) remote_addr)->sin_addr.s_addr =
      dss_htonl (lscb_ptr->protocol_ctl_blk.tcb->conn.remote.address);
    (*addrlen) = sizeof (struct sockaddr_in);
  }

  return lscb_ptr->sockfd;
} /* dsstcp_accept() */


/*===========================================================================
FUNCTION DSSTCP_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.
  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  Uses return values from send_tcp().  On error, the function returns -1.
  Otherwise, it returns the number of bytes appended to TCP send queue.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dsstcp_write
(
  protocol_ctl_block_type pcb,              /* protocol control block      */
  struct sockaddr_in *ta_ptr,               /* destination socket info     */
  dsm_item_type *dsm_item,                  /* ptr to dsm item chain       */
  ps_meta_info_type *meta_info_ptr          /* meta information - ignored  */
)
{
  struct tcb *tcb_ptr;                         /* ptr to TCP control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Since the meta infomation is not used, free the item
  -------------------------------------------------------------------------*/
  PS_META_INFO_FREE(&meta_info_ptr);

  tcb_ptr = pcb.tcb;

  return (send_tcp(tcb_ptr, dsm_item));

} /* dsstcp_write() */

/*===========================================================================
FUNCTION DSSTCP_READ()

DESCRIPTION
  Reads specified number of bytes into buffer from the TCP transport.
  
  This function does no sanity checking. The sanity checking should be done
  in dss_read().
  
  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of 
      bytes specified.
      
  If there are no bytes to be read in the rcvq, the return value is 
  DSS_ERROR and DS_EWOULDBLOCK is returned in errno.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dsstcp_read
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct iovec  * iov,        /* scatter buffer to which to copy data      */
  uint16 iovcount,               /* length of iovec array                  */
  struct sockaddr_in *fromaddr,           /* destination address - ignored */
  sint15 *errno                                   /* error condition value */
)
{
  struct tcb*   tcb_ptr;       /* Ptr. to TCP control block                */
  uint32        initial_wnd;   /* holds initial Receive Q byte count       */
  sint15        remove_cnt;    /* down counter for removing bytes          */
  sint15        read_cnt=0;    /* # of bytes read from TCP rcvq            */
  sint15        bytes_available;/* # of bytes in the TCP rcvq              */
  sint15        bytes_read = 0; /* total bytes read from TCP rcvq          */
  int           i;              /* local loop variable                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the TCP control block pointer.
  -------------------------------------------------------------------------*/
  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
  ASSERT(tcb_ptr != NULL);

  ASSERT(tcb_ptr != NULL);

  /*-------------------------------------------------------------------------
    Check if there is anything in the TCP receive queue. If not, return
    DS_EWOULDBLOCK. However, we return EOF indication if there is nothing to
    read and tcb is in CLOSE_WAIT state.
  -------------------------------------------------------------------------*/
  if ( tcb_ptr->rcvcnt == 0)
  {
    if (tcb_ptr->state == TCP_CLOSE_WAIT || 
        tcb_ptr->state == TCP_TIME_WAIT  ||
        tcb_ptr->state == TCP_CLOSING    ||
        tcb_ptr->state == TCP_LAST_ACK)
    {
      *errno = DS_EEOF;
      return DSS_SUCCESS;
    }
    else
    {
      *errno = DS_EWOULDBLOCK;
      return (DSS_ERROR);
    }
  }

  /*-------------------------------------------------------------------------
    There is data in the receive queue, remove the number of bytes req'd.
  -------------------------------------------------------------------------*/
  MSG_LOW("rcv.cnt=%d", tcb_ptr->rcvcnt, 0, 0);

  initial_wnd = tcb_ptr->rcv.wnd;
  bytes_available = MIN(tcb_ptr->rcvcnt, DSS_READ_MAX_BYTES);  
  /*-------------------------------------------------------------------------
    Use dsm_pullup to remove the number of bytes that the application wants
    to read. The buffers will be freed by dsm_pullup().
  -------------------------------------------------------------------------*/
  ASSERT(tcb_ptr->rcvq != NULL);

  for(i=0;i<iovcount && bytes_available > 0;i++)
  {   
    remove_cnt = (sint15) MIN( bytes_available, iov[i].iov_len);  
    if(remove_cnt > 0)
    {
      INTLOCK();
      read_cnt = (sint15) dsm_pullup( &(tcb_ptr->rcvq), iov[i].iov_base, 
				      (uint16) remove_cnt);
      tcb_ptr->rcvcnt  -= (uint16) read_cnt;
      tcb_ptr->rcv.wnd += (uint16) read_cnt;
      INTFREE();
    }
    ASSERT(read_cnt == remove_cnt);
    
    bytes_read += read_cnt;
    bytes_available -= read_cnt;
    read_cnt = 0;
  }

  MSG_LOW(" Read from TCP, bytes=%d, rcv.wnd=%d", read_cnt, tcb_ptr->rcv.wnd
          , 0);

  /*-------------------------------------------------------------------------
    If there is no more data in TCP rcvq, set data_available to FALSE. Note
    that we should not modify the data_available flag if we are in 
    CLOSE_WAIT state so that application performs another read and gets the 
    EOF indication.
  -------------------------------------------------------------------------*/
  INTLOCK();
  if ( tcb_ptr->rcvcnt == 0 && tcb_ptr->state != TCP_CLOSE_WAIT )
  {
      scb_ptr->data_available = FALSE;
  }
  INTFREE();

  /*-------------------------------------------------------------------------
    Check for a just re-opened window.
  -------------------------------------------------------------------------*/
  if( initial_wnd == 0 && tcb_ptr->rcv.wnd != 0)
  {
    /*-----------------------------------------------------------------------
      Window was closed. Force back open.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "TCP RX window re-opened", 0, 0, 0);
    tcb_ptr->flags.force = 1;
    tcp_output( tcb_ptr);
  }

  /*-------------------------------------------------------------------------
    Return the number of bytes that were read.
  -------------------------------------------------------------------------*/
  return(bytes_read);

} /* dsstcp_read() */

/*===========================================================================
FUNCTION DSSTCP_READ_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the TCP transport.
  
  This function does no sanity checking. The sanity checking should be done
  in dss_read_dsm_chain().
  
  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read from the DSM item chain.

  If there are no items to be read in the rcvq, the return value is 
  DSS_ERROR and DS_EWOULDBLOCK is returned in errno.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dsstcp_read_dsm_chain
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type **item_ptr,         /* Ptr to DSM item chain to be read in */
  struct sockaddr_in *fromaddr,           /* destination address - ignored */
  sint15 *errno                                   /* error condition value */
)
{
  struct tcb*   tcb_ptr;       /* Ptr. to TCP control block                */
  dword         initial_wnd;   /* holds initial Receive Q byte count       */
  sint15        read_cnt=0;    /* # of bytes read from TCP rcvq            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the TCP control block pointer.
  -------------------------------------------------------------------------*/
  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
  ASSERT(tcb_ptr != NULL);

  /*-------------------------------------------------------------------------
    Check if there is anything in the TCP receive queue. If not, return
    DS_EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if ( tcb_ptr->rcvcnt == 0)
  {
    if (tcb_ptr->state == TCP_CLOSE_WAIT)
    {
      *errno = DS_EEOF;
      return DSS_SUCCESS;
    }
    else
    {
      *errno = DS_EWOULDBLOCK;
      return (DSS_ERROR);
    }
  }

  initial_wnd = tcb_ptr->rcv.wnd;

  /*-------------------------------------------------------------------------
    The TCP receive queue is simply a chain of DSM items.  Pass this chain
    in its entirety to the calling layer.  Reset the receive queue to NULL.
  -------------------------------------------------------------------------*/
  ASSERT(tcb_ptr->rcvq != NULL);

  INTLOCK();
  *item_ptr = (dsm_item_type *)tcb_ptr->rcvq;
  read_cnt = dsm_length_packet(*item_ptr);

  /*-------------------------------------------------------------------------
    There should be no data left on the queue.
  -------------------------------------------------------------------------*/
  tcb_ptr->rcvcnt  -= (uint16) read_cnt;
  ASSERT(tcb_ptr->rcvcnt == 0);

  tcb_ptr->rcv.wnd += (uint16) read_cnt;
  tcb_ptr->rcvq = NULL;

  INTFREE();

  MSG_LOW(" Read from TCP, bytes=%d, rcv.wnd=%d", read_cnt, tcb_ptr->rcv.wnd
          , 0);

  /*-------------------------------------------------------------------------
    If there is no more data in TCP rcvq, set data_available to FALSE.
  -------------------------------------------------------------------------*/
  INTLOCK();
  if ( (tcb_ptr->rcvcnt == 0) && (tcb_ptr->state != TCP_CLOSE_WAIT) )
  {
    scb_ptr->data_available = FALSE;
  }
  INTFREE();

  /*-------------------------------------------------------------------------
    Check for a just re-opened window.
  -------------------------------------------------------------------------*/
  if( initial_wnd == 0 && tcb_ptr->rcv.wnd != 0)
  {
    /*-----------------------------------------------------------------------
      Window was closed. Force back open.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "TCP RX window re-opened", 0, 0, 0);
    tcb_ptr->flags.force = 1;
    tcp_output( tcb_ptr);
  }

  /*-------------------------------------------------------------------------
    Return the number of bytes that were read.
  -------------------------------------------------------------------------*/
  return(read_cnt);

} /* dsstcp_read_dsm_chain() */

/*===========================================================================
FUNCTION DSSTCP_CLOSE()

DESCRIPTION
  Initiates the active close for TCP.  If the connection is already closed,
  cleans up the TCP socket and associated resources.
                         
DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  Initiates active close for TCP connections.

===========================================================================*/
sint15 dsstcp_close
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *errno                                   /* error condition value */
)
{
  struct tcb* tcb_ptr;                        /* Ptr. to TCP control block */
  struct scb_s* lscb_ptr;                     /* Ptr to traverse scb lists */
  sint15 result;                                           /* return value */
  dssocki_state_enum_type socket_state;            /* current socket state */
  int unack_data;               /* Number of bytes of un-acknowledged data */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL) 
  {
    MSG_HIGH("scb pointer is bad", 0, 0, 0);
    return (DSS_ERROR);
  }

  MSG_MED("dsstcp_close() called, sockfd = %d", scb_ptr->sockfd, 0, 0);

  /*-------------------------------------------------------------------------
     Get the current socket state. 
  -------------------------------------------------------------------------*/
  socket_state = scb_ptr->socket_state;

  /*-------------------------------------------------------------------------
    If the socket has moved to one of these states then TCP has already 
    done its job and there is nothing more to be done.
  -------------------------------------------------------------------------*/
  if ( socket_state == DSSOCKI_WAIT_FOR_CLOSE ||
       socket_state == DSSOCKI_CLOSED ||
       socket_state == DSSOCKI_NULL )
  {
    *errno = 0;
    return DSS_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Get the TCP control block pointer.
  -------------------------------------------------------------------------*/
  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
 
  /*-------------------------------------------------------------------------
    Get error code for this routine.
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_CLOSE_ROUTINE, errno);
  
  TASKLOCK();

  if (socket_state == DSSOCKI_INIT) 
  {
    /*-----------------------------------------------------------------------
      If the socket was opened without being connected, free the tcb and set
      the socket state to WAIT_FOR_CLOSE. The application would call 
      dss_close() again and then the scb would be freed in dss_close().
    -----------------------------------------------------------------------*/
    if ( tcb_ptr != NULL )
    {
      dsstcpi_free_tcb( tcb_ptr);
    }

    PS_BRANCH_TASKFREE();

    dssocki_set_sock_state( scb_ptr, DSSOCKI_WAIT_FOR_CLOSE );
    *errno = 0;
    return( DSS_SUCCESS);
  }
  else if( socket_state == DSSOCKI_LISTEN)
  {
    /*-----------------------------------------------------------------------
       Reset all half open connections
    -----------------------------------------------------------------------*/
    while((lscb_ptr = scb_ptr->so_partial) != NULL)
    {
      if(lscb_ptr->protocol_ctl_blk.tcb != NULL)
      {
        reset_tcp(lscb_ptr->protocol_ctl_blk.tcb);
      }
      scb_ptr->so_partial = scb_ptr->so_partial->so_partial;
      dssocki_freescb(lscb_ptr);
    }
    /*-----------------------------------------------------------------------
       Reset all open connections
    -----------------------------------------------------------------------*/
    while((lscb_ptr = scb_ptr->so_estab) != NULL) 
    {
      if(lscb_ptr->protocol_ctl_blk.tcb != NULL)
      {
        reset_tcp(lscb_ptr->protocol_ctl_blk.tcb);
      }
      scb_ptr->so_estab = scb_ptr->so_estab->so_estab;
      dssocki_freescb(lscb_ptr);
    }
    PS_BRANCH_TASKFREE();

    dssocki_set_sock_state( scb_ptr, DSSOCKI_WAIT_FOR_CLOSE );
    *errno = 0;
    return(DSS_SUCCESS);
  }
  else if ( socket_state == DSSOCKI_OPENING || socket_state == DSSOCKI_OPEN )
  {
    ASSERT(tcb_ptr != NULL);
    unack_data = tcb_ptr->sndcnt;
    /*-----------------------------------------------------------------------
      sndcnt is inclusive of FIN. So, decrement it if FIN is sent.
    -----------------------------------------------------------------------*/
    if(tcb_ptr->state == TCP_FINWAIT1 || 
       tcb_ptr->state == TCP_CLOSING || 
       tcb_ptr->state ==TCP_LAST_ACK)
    {
      unack_data --;
    }
  
    ASSERT(unack_data >= 0);
    /*-----------------------------------------------------------------------
      If the call is dormant (Physical link is down or going down and 
      interface is up) and graceful_dormant_close flag is FALSE and there 
      is no un-acknowledged data in the send queue then abort the connection
    -----------------------------------------------------------------------*/
    if((scb_ptr->routing_cache == NULL) ||
       ((((ps_iface_phys_link_state(scb_ptr->routing_cache) ==  
           PHYS_LINK_GOING_DOWN) || 
          (ps_iface_phys_link_state(scb_ptr->routing_cache) ==  
           PHYS_LINK_DOWN)) &&
         (ps_iface_state(scb_ptr->routing_cache) == IFACE_UP)) &&
        (sock_config_cb.tcp_graceful_dormant_close == FALSE) &&
        (unack_data == 0))) 
    {
      close_self(tcb_ptr, NETWORK);
      PS_BRANCH_TASKFREE();
      *errno = 0;
      return DSS_SUCCESS;
    }
    /*-----------------------------------------------------------------------
      Reset TCP if the silent close option is set or so_linger option is set 
      with zero timeout. Do an active close otherwise.
    -----------------------------------------------------------------------*/
    if ( IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_SILENT_CLOSE) ||
         ( IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_LINGER) &&
           scb_ptr->so_linger == 0 )
       )
    {
      reset_tcp(tcb_ptr);
      *errno = 0;
    }
    else
    {
      close_tcp(tcb_ptr);
      /*---------------------------------------------------------------------
        Transition the state to CLOSING only if the socket state is OPEN. The
        reason is that the socket state could have changed (e.g., to CLOSED
        from OPENING) as a result of the close_tcp() call above. 
      ---------------------------------------------------------------------*/
      if ( scb_ptr->socket_state == DSSOCKI_OPEN )
      {
        dssocki_set_sock_state(scb_ptr, DSSOCKI_CLOSING);
      }
    }  /* if SILENT_CLOSE or LINGER option with zero timeout is not set */
  }
  else if ( socket_state == DSSOCKI_CLOSING )
  {
    ASSERT(tcb_ptr != NULL);
    /*-----------------------------------------------------------------------
      Reset TCP if the silent close option is set.
    -----------------------------------------------------------------------*/
    if ( IS_SOCKOPT_SET(scb_ptr->options_mask, DSS_SO_SILENT_CLOSE))
    {
      reset_tcp(tcb_ptr);
      *errno = 0;
    }
  }
  TASKFREE();

  return( result);

} /* dsstcp_close() */

/*===========================================================================
FUNCTION DSSTCP_ABORT()

DESCRIPTION
  Aborts the TCP connection and cleans-up. This is usually called when we are
  dormant.
                         
DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  TCB is freed up and the pointer is no longer valid.

===========================================================================*/
sint15 dsstcp_abort
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *errno              /* Error condition value                      */
)
{
  struct tcb* tcb_ptr;                        /* Ptr. to TCP control block */
  dssocki_state_enum_type socket_state;            /* current socket state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  socket_state = scb_ptr->socket_state;

  /*-------------------------------------------------------------------------
    If the socket is in open opening or closing state then we don't want to
    do a normal close by exchanging FINs, etc. However, in the other states,
    we do call dsstcp_close() routine so that the necessary clean-up is done.
  -------------------------------------------------------------------------*/

  if( socket_state == DSSOCKI_OPEN ||
      socket_state == DSSOCKI_OPENING ||
      socket_state == DSSOCKI_CLOSING)
  {
    tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;

    /*-----------------------------------------------------------------------
      If tcb_ptr is non-NULL, silently close TCP - this would also free the
      tcb. The scb would be freed up when dss_close() is called the second
      time.
    -----------------------------------------------------------------------*/
    if(tcb_ptr != NULL)
    {
      close_self( tcb_ptr, RESET);
    }

    dssocki_set_sock_state(scb_ptr, DSSOCKI_CLOSED);
    *errno = 0;
  }
  else
  {
    dsstcp_close (scb_ptr, errno);
  }

  return( DSS_SUCCESS);

} /* dsstcp_abort() */


/*===========================================================================
FUNCTION DSSTCP_GET_CONN_ERROR()

DESCRIPTION
  Gets the appropriate error code from the closed_state_reason.  Depending
  on the type of call (connect(), or read()/write()), different error
  values can be returned.

DEPENDENCIES
  None.

RETURN VALUE
  Error condition for connection state.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_get_conn_error
(
  struct scb_s *scb_ptr,                    /* ptr to socket control block */
  dssocki_caller_enum_type type             /* type of the caller function */
)
{
  sint15 reason;                            /* reason for connection error */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reason is set in the SCB whenever TCP state changes to CLOSED.
    Based on reason for close - return appropriate error condition based
    on the type of call it is being returned for.
  -------------------------------------------------------------------------*/  
  reason = scb_ptr->closed_state_reason;
  
  MSG_MED("TCP Closed Reason: %d", reason, 0, 0);

  switch (reason)
  {
    case DS_ENETDOWN:
    case DS_EIPADDRCHANGED:
      /*---------------------------------------------------------------------
        These are the same as the reason, and are already set, so we do not 
        need to do anything - We simply return the reason.
      ---------------------------------------------------------------------*/  
      break;

    case NORMAL:
      /*---------------------------------------------------------------------
        The normal TCP close of connection - No connection exists
      ---------------------------------------------------------------------*/  
      if (type == DSSOCKI_CONNECT_ROUTINE) 
      {
        reason = DS_ENOTCONN;
      }
      else if ( type == DSSOCKI_INPUT_ROUTINE ||
                type == DSSOCKI_OUTPUT_ROUTINE )
      {
        /*-------------------------------------------------------------------
          We have received an EOF, so assume it was a passive close
        -------------------------------------------------------------------*/  
        reason = DS_EEOF;
      }
      break;
  
    case RESET:
      /*---------------------------------------------------------------------
        Connection was reset
      ---------------------------------------------------------------------*/  
      if (type == DSSOCKI_CONNECT_ROUTINE)
      {
        reason = DS_ECONNREFUSED;
      }
      else if ( type == DSSOCKI_INPUT_ROUTINE ||
                type == DSSOCKI_OUTPUT_ROUTINE )
      {
        reason = DS_ECONNRESET;
      }
      else
      {
        ERR_FATAL("Invalid type for reason", 0, 0, 0);
      }
      break;

    case TIMEOUT:
      /*---------------------------------------------------------------------
        Connection was aborted
      ---------------------------------------------------------------------*/  
      if (type == DSSOCKI_CONNECT_ROUTINE)
      {
        reason = DS_ETIMEDOUT;
      }
      else if ( type == DSSOCKI_INPUT_ROUTINE ||
                type == DSSOCKI_OUTPUT_ROUTINE )
      {
        reason = DS_ECONNABORTED;
      }
      else
      {
        ERR_FATAL("Invalid type for reason", 0, 0, 0);
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid closed state reason.
      ---------------------------------------------------------------------*/
      ASSERT(0);
  }

  return (reason);

} /* dsstcp_get_conn_error() */

/*===========================================================================
FUNCTION DSSTCP_LOOKUP_CONN()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a 
  valid connection.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.  
  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcp_lookup_conn
(
  struct connection *conn                    /* connection to be looked up */
)
{
  struct tcb *tcb_ptr;                       /* ptr to TCP control block   */
  uint16            i;                       /* loop counter               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look up the connection and look at IP address and port numbers for
    both source and destination.  
  -------------------------------------------------------------------------*/
  for( i=0; i< DSS_MAX_TCP_SOCKS; i++)
  {
    if( dsstcpi_arr[ i].scb_ptr != NULL)
    {
      tcb_ptr = &dsstcpi_arr[ i];
      if (conn->remote.port      == tcb_ptr->conn.remote.port
         && conn->local.port     == tcb_ptr->conn.local.port
         && conn->remote.address == tcb_ptr->conn.remote.address
         && conn->local.address  == tcb_ptr->conn.local.address)
      {
        return( tcb_ptr);
      }
    } /* if valid tcb */
  } /* for */

  /*-------------------------------------------------------------------------
      Connection not found.
  -------------------------------------------------------------------------*/
  return( NULL);

} /* dsstcp_lookup_conn() */


/*===========================================================================
FUNCTION DSSTCP_LOOKUP_SERV()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a 
  valid listener.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.  
  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcp_lookup_serv
(
  struct connection *conn                    /* connection to be looked up */
)
{
  struct tcb *ltcb_ptr;                      /* ptr to TCP control block   */
  uint16            i;                       /* loop counter               */
  struct tcb *tcb_ptr = NULL;                /* ptr to TCP control block   */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look up the connection and look at IP address and port numbers for
    both source and destination.  
  -------------------------------------------------------------------------*/
  for( i=0; i< DSS_MAX_TCP_SOCKS; i++)
  {
    if( dsstcpi_arr[i].scb_ptr != NULL)
    {
      ltcb_ptr = &dsstcpi_arr[i];
      if (conn->remote.port      == ltcb_ptr->conn.remote.port
         && conn->local.port     == ltcb_ptr->conn.local.port
         && conn->remote.address == ltcb_ptr->conn.remote.address
         && conn->local.address  == ltcb_ptr->conn.local.address)
      {
        return (ltcb_ptr);
      }


      if (tcb_ptr == NULL 
         && ltcb_ptr->conn.remote.port == 0
         && conn->local.port == ltcb_ptr->conn.local.port
         && ltcb_ptr->conn.remote.address == 0
         && (ltcb_ptr->conn.local.address == INADDR_ANY  
         || conn->local.address == ltcb_ptr->conn.local.address) )
      {
        tcb_ptr = ltcb_ptr;
      }
    } /* if valid tcb */
  } /* for */

  /*-------------------------------------------------------------------------
      Connection not found.
  -------------------------------------------------------------------------*/
  return tcb_ptr;

} /* dsstcp_lookup_serv() */


/*===========================================================================
FUNCTION DSSTCP_NEW_IP_UPCALL()

DESCRIPTION
  This function will be invoked by PPPIPCP when a new IP address is 
  negotiated with the Base Station.  If that is the case, then 
  send a reset to the peer, and then close down sockets and notify 
  the user.
  This function is called from the PS task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_new_ip_upcall
(
  ps_iface_type *iface_ptr
)
{
  struct tcb *tcb_ptr;                       /* ptr to TCP control block   */
  uint16            i;                       /* loop counter               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If there are any TCP sockets which use the specified interface, reset 
    them as the IP address on that interface has changed. We are assuming 
    that an interface is capable of allocating only one IP address. If that 
    is not true, we would have to check to see if the old address of the 
    interface is same as the current one the socket is using.
  -------------------------------------------------------------------------*/
  for( i=0; i< DSS_MAX_TCP_SOCKS; i++)
  {
     if(( dsstcpi_arr[ i].scb_ptr != NULL) &&
        ( dsstcpi_arr[ i].scb_ptr->routing_cache == iface_ptr))
     {
        tcb_ptr = &dsstcpi_arr[ i];
        if ( (tcb_ptr->state != TCP_CLOSED) ) 
        {
          /*---------------------------------------------------------------
            Reset active tcp connections
          ---------------------------------------------------------------*/
          reset_tcp( tcb_ptr);
        }
     } /* if valid tcb */
  } /* for */

} /* dsstcp_new_ip_upcall() */


/*===========================================================================
FUNCTION DSSTCP_BIND()

DESCRIPTION
  This is the TCP specific bind() function. It writes the port number into
  the local.conn.port of the socket control block structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_bind
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct sockaddr_in *la     /* Ptr to the structure contains the local 
                                address.                                   */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Set the conn.local.port value to the port number passed in
  -------------------------------------------------------------------------*/
  ASSERT(scb_ptr->protocol_ctl_blk.tcb != NULL);
  
  scb_ptr->protocol_ctl_blk.tcb->conn.local.port = dss_ntohs(la->sin_port);

} /* dsstcp_bind() */


/*===========================================================================
FUNCTION DSSTCP_ISWRITEABLE()

DESCRIPTION
  This function determines if one can write more data to the specified tcb.
  The objective is that we want to we not only want to limit the number of
  outstanding segments but also the amount of data queued up by the 
  application.

DEPENDENCIES
  None.

RETURN VALUE
  Return the number of bytes available in sndq.

SIDE EFFECTS
  None.
===========================================================================*/
int dsstcp_is_writeable
(
  protocol_ctl_block_type pcb                    /* protocol control block */
)
{
  struct tcb *tcb_ptr;                         /* ptr to TCP control block */
  int         bytes_available;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tcb_ptr = pcb.tcb;

  INTLOCK();
  bytes_available = tcb_ptr->scb_ptr->sndbuf - tcb_ptr->scb_ptr->sndcnt;
  INTFREE();

  return bytes_available;
} /* dsstcp_is_writeable() */

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This 
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_cleanup_all_reseq()
{
  uint16 index;

  for( index=0; index < DSS_MAX_TCP_SOCKS; index++)
  {
    tcp_cleanup_reseq(&dsstcpi_arr[index]);
  }
} /* dsstcp_cleanup_all_reseq() */

/*===========================================================================
FUNCTION DSSTCP_SHUTDOWN()

DESCRIPTION
  This function implements the half-close functionality for tcp sockets.

DEPENDENCIES
  None.

PARAMETERS
  scb_ptr - pointer to scb which needs to be shut down
  item_ptr - a dsm item containing information about what kind of shutdown 
             needs to be done (read, write or both)
  dss_errno - error number returned in case of error

RETURN VALUE
  On success, returns DSS_SUCCESS otherwise returns DSS_ERROR and places
  the error number in dss_errno.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_shutdown
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type*  item_ptr,  /* dsm item containing the call specific info */
  sint15* dss_errno                                    /* dss error number */
)
{
  uint16 how;
  struct tcb *tcb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( scb_ptr == NULL )
  {
    ASSERT(0);
    MSG_ERROR("dsstcp_shutdown() called for NULL scb_ptr", 0, 0, 0);
    return DSS_ERROR;
  }

  if ( (tcb_ptr = scb_ptr->protocol_ctl_blk.tcb) == NULL )
  {
    MSG_HIGH("dsstcp_shutdown() called for NULL tcb_ptr", 0, 0, 0);
    *dss_errno = 0;
    return DSS_SUCCESS;
  }

  if ( dsm_pullup( &item_ptr, &how, sizeof(how)) != sizeof(how) )
  {
    dsm_free_packet(&item_ptr);
    ASSERT(0);
    ERR("Error while pulling up, discarding packet", 0, 0, 0);
    return DSS_ERROR;
  }
 
  /*-----------------------------------------------------------------------
    Free the DSM packet since we have extracted the necessary information
    from it and have no more use of it.
  -----------------------------------------------------------------------*/
  dsm_free_packet(&item_ptr);

  /*-----------------------------------------------------------------------
    If the read-half needs to be shut down, clean-up the receive and the 
    resequencing queues. Note that shut down of read-half does not 
    result in any communication with the peer.
  -----------------------------------------------------------------------*/
  if ( how == DSS_SHUT_RD || how == DSS_SHUT_RDWR  ) 
  {
    dsm_free_packet( &(tcb_ptr->rcvq) );
    tcb_ptr->rcvcnt = 0;
    scb_ptr->data_available = FALSE;
    tcp_cleanup_reseq( tcb_ptr );
  }

  /*-----------------------------------------------------------------------
    If the write-half needs to be shut down, call close_tcp() which will
    send a FIN after sending out all the queued up data.
  -----------------------------------------------------------------------*/
  if ( how == DSS_SHUT_WR || how == DSS_SHUT_RDWR) 
  {
    if ( close_tcp(tcb_ptr) == 0 )
    {
      dssocki_set_sock_state(scb_ptr, DSSOCKI_CLOSING);
    }
  }

  return (DSS_SUCCESS);
}

#endif /* FEATURE_DS_SOCKETS */
