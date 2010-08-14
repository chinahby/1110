
/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I N T E R N A L   F I L E
                   
DESCRIPTION

 The Data Services sockets internal file. This contains all the functions
 used by internal applications.
 
EXTERNALIZED FUNCTIONS

  dssocki_set_non_sys_socket_flow()
    This function flow controls all non-system sockets - mainly used for
    MobileIP calls.

  dss_write_dsm_chain()
     Sends the specified DSM item chain over the TCP transport.  Lower layers
     will be responsible for deallocating the DSM item chain.
  
  dss_read_dsm_chain()
     Reads a DSM item chain from the TCP transport.
     
  dss_set_specified_iface()
     This function is used to set the app specified interface for an 
     application. For instance, MIP and GPS which uses only the 1X interface,
     use this API to set the specified interface in the acb.
                            
Copyright (c) 1998-2002 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssinternal.c#2 $ $DateTime: 2009/02/05 02:33:42 $ $Author: kbhaskar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/05/09    kk     Added a NULL pointer check in dssocki_set_non_sys_socket_flow()
05/19/04    sv     Incremented the sndcnt in dss_write_dsm_chain.
05/02/03    usb    Moved Asserts inside if statements.
04/25/03    aku    Initial version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DS_SOCKETS
#include "err.h"
#include "assert.h"
#include "msg.h"
#include "dssocki.h"
#include "dserrno.h"
#include "dssinternal.h"

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION DSS_WRITE_DSM_CHAIN()

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
sint15 dss_write_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  sint15 *dss_errno                               /* error condition value */
)
{
   struct scb_s *scb_ptr;      /* ptr to socket control block allocated in */
   sint15   nbytes;              /* count of number of bytes that are sent */
   sint15   result = 0;                     /* error checking return value */   

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    ASSERT(0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Do not process any NULL DSM items.
  -------------------------------------------------------------------------*/
  if (item_ptr == NULL || *item_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Reset the app_field similar to dsm_new_buffer() behavior.
  -------------------------------------------------------------------------*/
  (*item_ptr)->app_field = 0;

  scb_ptr = NULL;

  /*-------------------------------------------------------------------------
    Perform common write procedure. 
  -------------------------------------------------------------------------*/
  result = dssocki_common_write_check(
                                       sockfd, 
                                       &scb_ptr, 
                                       TRUE, 
                                       TRUE,
                                       dss_errno
                                       );

  if (result == DSS_ERROR)
  {
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Only support TCP
  -------------------------------------------------------------------------*/
  if (scb_ptr->protocol != IPPROTO_TCP)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }
  /*-------------------------------------------------------------------------
    Check for 0 bytes specified - If so, return 0 bytes written.
  -------------------------------------------------------------------------*/
  nbytes = dsm_length_packet(*item_ptr);
  if (nbytes == 0) 
  {
    return (0);
  }
  scb_ptr->sndcnt += nbytes;

  /*-------------------------------------------------------------------------
    Call function to put the item on the queue to PS.
  -------------------------------------------------------------------------*/
  MSG_LOW("dss_write_dsm_chain() - writing %d bytes",nbytes,0,0);

#ifdef FEATURE_DSM_MEM_CHK
  (*item_ptr)->tracer = DSM_SCKT_TO_PS;
#endif /* FEATURE_DSM_MEM_CHK */
  
  dssocki_send_to_ps( scb_ptr, *item_ptr, NULL, DSM_PS_SOCKET_PAYLOAD);
  *item_ptr = NULL;

  return (nbytes);

} /* dss_write_dsm_chain() */

/*===========================================================================
FUNCTION DSS_READ_DSM_CHAIN()

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
sint15 dss_read_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr; /* ptr to socket control block allocated in       */
  sint15 result = 0;                        /* error checking return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if( dss_errno == NULL )
  {
    ASSERT(0);
    return (DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  MSG_LOW(" dss_read_dsm_chain() called.", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
   ------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

 /*-------------------------------------------------------------------------
    Only support TCP
  -------------------------------------------------------------------------*/
  if (scb_ptr->protocol != IPPROTO_TCP)
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
    if ( *dss_errno == DS_EEOF)
    {
      /*---------------------------------------------------------------------
        Check if there is data available. Return 0, if there is no data 
        available (EOF indication). Otherwise, call the protocol dependent
        read function.
      ---------------------------------------------------------------------*/
      if( scb_ptr->data_available == FALSE) 
      {
        return (DSS_SUCCESS);
      }
    }
    else
    {
      return (result);
    }
  }

  /*-------------------------------------------------------------------------
    Make sure that the passed item_ptr is valid
  -------------------------------------------------------------------------*/
  if ( item_ptr == NULL )
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Call protocol specific function to read bytes into a array.
  -------------------------------------------------------------------------*/
  result = scb_ptr->fcn_ptr_table->read_dsm_chain(scb_ptr, item_ptr, NULL, 
                                                  dss_errno);

  /*-------------------------------------------------------------------------
    Read may return EWOULDBLOCK if the network is down.  Make sure ENETDOWN 
    is returned in the case when the network is down.
  -------------------------------------------------------------------------*/
  if ((result == DSS_ERROR)      && 
      (*dss_errno == DS_EWOULDBLOCK) &&
      (PS_IFACE_IS_VALID(scb_ptr->routing_cache) == FALSE))
  {
    *dss_errno = DS_ENETDOWN;
    result = DSS_ERROR;
  }

  return result;

} /* dss_read_dsm_chain() */


/*===========================================================================
FUNCTION DSSOCKI_SET_NON_SYS_SOCKET_FLOW()

DESCRIPTION
  This function is used to enable and disable flow control for non system
  sockets on a specific interface.

PARAMETERS
  mask:    the flow control mask (defined in dsi.h)
  enabled: whether flow should be enabled
  iface_id_ptr: Interface on which to operate.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_set_non_sys_socket_flow
(
  uint32 mask,
  boolean enabled,
  dss_iface_ioctl_id_type *iface_id_ptr
)
{
  boolean flow_disabled_prev = FALSE;
  ps_iface_type  *iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(iface_id_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Determine the specific ps_iface_ptr based on the interface name and 
    instance passed in. 
  -------------------------------------------------------------------------*/
  if ( ( iface_ptr = ps_iface_get_handle(iface_id_ptr->name, 
                                  iface_id_ptr->instance) ) == NULL )
  {
    MSG_ERROR("No iface found with name:%p instance:%p",
              iface_id_ptr->name,iface_id_ptr->name,0);
    return;
  }

  /*-------------------------------------------------------------------------
    If flow is enabled then remove the bit from the mask otherwise set it
  -------------------------------------------------------------------------*/
  if(enabled == TRUE)
  {
    
    MSG_MED("Allow sock flow 0x%x iface %p", mask,iface_id_ptr,0);
    /*-----------------------------------------------------------------------
      Store the current value of the non sys sockets flow mask.
    -----------------------------------------------------------------------*/
    flow_disabled_prev = PS_IFACE_NON_SYS_SOCKETS_FLOW_ENABLED(iface_ptr);

    /*-----------------------------------------------------------------------
      Enable the non system sockets flow mask in ps_iface.  
    -----------------------------------------------------------------------*/
    ps_iface_enable_non_sys_sockets_flow(iface_ptr,mask);
    
    /*----------------------------------------------------------------------
    If the flow was disabled earlier and has been enabled now, notify the
    sockets.
    -----------------------------------------------------------------------*/
    if ((flow_disabled_prev == FALSE) && 
        (PS_IFACE_NON_SYS_SOCKETS_FLOW_ENABLED(iface_ptr)))
    {
      MSG_MED("Sock flow enabled on %p", iface_id_ptr,0,0);
      dssocki_socket_event_occurred( NULL, NULL );
    }
  }
  else
  {
    MSG_MED("Stop sock flow 0x%x iface %p", mask,iface_id_ptr,0);
    /*-----------------------------------------------------------------------
      Disable the non system sockets flow mask in ps_iface.  
    -----------------------------------------------------------------------*/
    ps_iface_disable_non_sys_sockets_flow(iface_ptr,mask);
  }

} /* dssocki_set_non_sys_socket_flow() */


/*===========================================================================
FUNCTION DSS_SET_SPECIFIED_IFACE()

DESCRIPTION
  This function is used to set the app specified interface for an 
  application. For instance, MIP and GPS which uses only the 1X interface,
  use this API to set the specified interface in the acb.

DEPENDENCIES
  None.

PARAMETERS
  sint15 appid - Application ID.
  dss_iface_ioctl_id_type *iface_id - Parameter which contains the 
  name and instance of the iface that application would like to use.
  sint15 *ds_errno - DS_EBADAPP if invalid appid is specified.
                     DS_EBADF if invalid interface is specified.
 
RETURN VALUE
  0 on success.
  -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int dss_set_specified_iface
(
  sint15                  appid,
  dss_iface_ioctl_id_type *iface_id,
  sint15                  *ds_errno
)
{
  acb_type *acb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds_errno == NULL || iface_id == NULL)
  {
    ASSERT(0);
    return -1;
  }

  *ds_errno = DSS_SUCCESS;

  acb_ptr = dssocki_appidtoacb(appid);
  if(acb_ptr == NULL)
  {
    MSG_HIGH("Invalid appid %d", appid,0,0);
    *ds_errno = DS_EBADAPP;
    return -1;
  }
  
  acb_ptr->app_specified_iface_ptr = ps_iface_get_handle(iface_id->name,
                                                         iface_id->instance);
  if(acb_ptr->app_specified_iface_ptr == NULL)
  {
    MSG_HIGH("Invalid iface name = 0x%x, instance=%x", 
             iface_id->name, iface_id->instance,0);
    *ds_errno = DS_EBADF;
    return -1;
  }

  return 0;

} /* dss_set_specified_iface() */

#endif /* FEATURE_DS_SOCKETS */
