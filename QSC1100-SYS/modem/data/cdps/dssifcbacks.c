/*===========================================================================

        S O C K E T   I N T E R F A C E   C A L L B A C K S  F I L E
                   
DESCRIPTION

 
EXTERNALIZED FUNCTIONS


Copyright (c) 2000, 2001 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssifcbacks.c_v   1.7   10 Feb 2003 10:51:52   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssifcbacks.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/10/03    aku    Added check for valid APP_ID before calling 
                   dssocki_invoke_app_ev_cbacks().
12/24/02    aku    Register for PHYS_LINK_UP and PHYS_LINK_DOWN events at
                   startup. Also added support for invoking app event 
                   callbacks when IP address changes.
12/22/02    aku    IFACE_UP and IFACE_DOWN are registered on a global basis
                   at powerup. Removed use of global iface_cb array.
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
09/10/02    aku    Removed global Ip_addr.
09/06/02    aku    Added checks for ps_iface function return values.
09/05/02    aku    Updated for ps_iface cback buf interface.
07/17/02    aku    Initialize link pointers in ps_iface_event types.
                   Set IP addr type before invoking ps_iface_get_addr()
07/16/02    aku    Registering for IP_ADDR_CHANGED event.
07/09/02    na    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DS_SOCKETS
#include "ps_iface.h"
#include "dssocki.h"
#include "assert.h"
#include "msg.h"
#include "err.h"
#include "ps_ip_addr.h"
#include "ps_iface.h"
#include "dsstcp.h"

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
                        DEFINES & CONSTANTS
---------------------------------------------------------------------------*/
boolean dssifcb_initialized = FALSE;

struct 
{
  void                       *flow_enabled_cback;
  void                       *ip_addr_changed_cback;
  void                       *iface_up_cback;
  void                       *iface_down_cback;
  void                       *phys_link_up_cback;
  void                       *phys_link_down_cback;
} dssifcb_info;
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION DSSIFCB_FLOW_ENABLED_CBACK()

DESCRIPTION
  Posts signal to PS to process enabling of IFACE flow.

DEPENDENCIES
  None.
  
RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/
void dssifcb_flow_enabled_cback
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("Set PS_SOCKET_FLOW_CHANGE_SIG",0,0,0);
  PS_SET_SIGNAL(PS_SOCKET_FLOW_ENABLED_SIG); 

} /* dssifcb_flow_enabled_cback() */

/*===========================================================================

FUNCTION DSSIFCB_PROCESS_FLOW_ENABLED_CBACK()

DESCRIPTION
  Called when flow is enabled on any interface

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssifcb_process_flow_enabled_cback
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
  int      index;                                            /* loop index */
  struct scb_s *scb_ptr;          /* ptr to socket control block structure */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( sig == PS_SOCKET_FLOW_ENABLED_SIG); 

  MSG_MED("flow_enabled_cb called",0,0,0);

  /*-----------------------------------------------------------------------
    Loop through SCB array - Protect access to global array through use
    of INTLOCK()_SAV/INTFREE_SAV().
   ----------------------------------------------------------------------*/
  for (index=0; index< DSS_MAX_SOCKS; index++)
  {

    scb_ptr = &scb_array[index];

    if(scb_ptr->sockfd == SOCKAVAIL)  
    {
      continue;
    }

    /*---------------------------------------------------------------------
      Check for events and notify application 
    ---------------------------------------------------------------------*/
    if( dssocki_socket_event_check( 
                            scb_ptr, 
                            (scb_ptr->requested_event_mask & DS_WRITE_EVENT)
                                  ))
    {
      MSG_MED("Notify app %d about flow enable",
               scb_ptr->acb_ptr->app_id,0,0); 
      dssocki_notify_app( scb_ptr->acb_ptr);
    }

  } /* for index */

  return (TRUE);

} /* dssifcb_process_flow_enabled_cback() */


/*===========================================================================

FUNCTION DSSIFCB_IP_ADDR_CHANGED_CBACK()

DESCRIPTION
  Called when IP addr is changed on any interface

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void dssifcb_ip_addr_changed_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
)
{
  ps_iface_type *iface_ptr = (ps_iface_type *)user_info_ptr;
  acb_type      *curr_acb_ptr=NULL;
  uint32        index=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Since a new IP address is negotiated with the Base Station, send a reset 
    to the peer, and then close down sockets and notify the user.
  -------------------------------------------------------------------------*/
  dsstcp_new_ip_upcall(iface_ptr);

  /*-------------------------------------------------------------------------
    This function notifies all interested sockets that a socket event has 
    occurred. The function loops through the socket control block array and 
    checks each socket for events that may have occurred. The application is 
    notified upon any events having occurred.  
  -------------------------------------------------------------------------*/
  dssocki_socket_event_occurred(NULL, NULL);

   /*------------------------------------------------------------------------
     Invoke any app callbacks for this event.
   ------------------------------------------------------------------------*/
   for (index=0; index<DSS_MAX_APPS; index++)
   {
     curr_acb_ptr = &dssocki_acb_array[index];
     if(curr_acb_ptr->app_id != APPAVAIL)
     {
       dssocki_invoke_app_ev_cbacks(curr_acb_ptr,
                                    iface_ptr,
                                    DSS_IFACE_IOCTL_ADDR_CHANGED_EV);
     }
   }
} /* dssifcb_ip_addr_changed_cback() */

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

FUNCTION DSSIFCB_INIT()

DESCRIPTION
  Called on initialization. 
  
  This registers all the callbacks with PSIFACE.

DEPENDENCIES
  None
  
RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void dssifcb_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( dssifcb_initialized == FALSE)
  {

     dssifcb_initialized = TRUE;

    /*-----------------------------------------------------------------------
       Register cbacks for events with psiface for FLOW ENABLED.
    -----------------------------------------------------------------------*/
    if((dssifcb_info.flow_enabled_cback = 
                   ps_iface_alloc_event_cback_buf(dssifcb_flow_enabled_cback,
                                                  NULL)) == NULL)
    {
      MSG_ERROR("Could not alloc event buf",0,0,0);
      ASSERT(0);
    }
    if(ps_iface_event_cback_reg( NULL, 
                                 IFACE_FLOW_ENABLED_EV, 
                                 dssifcb_info.flow_enabled_cback) != 0)
    {
      MSG_ERROR("Could not reg FLOW_ENABLED evt ",0,0,0);
      ASSERT(0);
    }


    /*-----------------------------------------------------------------------
      Register cbacks for the IP_ADDR_CHANGED event with psiface.        
    -----------------------------------------------------------------------*/
    if((dssifcb_info.ip_addr_changed_cback = 
                ps_iface_alloc_event_cback_buf(dssocki_iface_ev_cback,
                                               NULL)) == NULL)
    {
      MSG_ERROR("Could not alloc event buf",0,0,0);
      ASSERT(0);
    }
    if(ps_iface_event_cback_reg( NULL, 
                                 IFACE_ADDR_CHANGED_EV, 
                                 dssifcb_info.ip_addr_changed_cback) != 0)
    {
      MSG_ERROR("Could not reg ADDR_CHANGED evt ",0,0,0);
      ASSERT(0);
    }

    /*-----------------------------------------------------------------------
      Register cbacks for the IFACE_UP event with psiface.
    -----------------------------------------------------------------------*/
    if((dssifcb_info.iface_up_cback =
                ps_iface_alloc_event_cback_buf(dssocki_iface_ev_cback,
                                               NULL)) == NULL)
    {
      MSG_ERROR("Could not alloc event buf",0,0,0);
      ASSERT(0);
    }
    if(ps_iface_event_cback_reg( NULL,
                                 IFACE_UP_EV,
                                 dssifcb_info.iface_up_cback) != 0)
    {
      MSG_ERROR("Could not reg IFACE_UP evt ",0,0,0);
      ASSERT(0);
    }

    /*-----------------------------------------------------------------------
      Register cbacks for the IFACE_DOWN event with psiface.
    -----------------------------------------------------------------------*/
    if((dssifcb_info.iface_down_cback =
                ps_iface_alloc_event_cback_buf(dssocki_iface_ev_cback,
                                               NULL)) == NULL)
    {
      MSG_ERROR("Could not alloc event buf",0,0,0);
      ASSERT(0);
    }
    if(ps_iface_event_cback_reg( NULL,
                                 IFACE_DOWN_EV,
                                 dssifcb_info.iface_down_cback) != 0)
    {
      MSG_ERROR("Could not reg IFACE_DOWN evt ",0,0,0);
      ASSERT(0);
    }
    
    /*-----------------------------------------------------------------------
      Register cbacks for the IFACE_PHYS_LINK_UP_EV event with psiface.
    -----------------------------------------------------------------------*/
    if((dssifcb_info.phys_link_up_cback =
                ps_iface_alloc_event_cback_buf(dssocki_iface_ev_cback,
                                               NULL)) == NULL)
    {
      MSG_ERROR("Could not alloc event buf",0,0,0);
      ASSERT(0);
    }
    if(ps_iface_event_cback_reg( NULL,
                                 IFACE_PHYS_LINK_UP_EV,
                                 dssifcb_info.phys_link_up_cback) != 0)
    {
      MSG_ERROR("Could not reg PHYS_LINK_UP evt ",0,0,0);
      ASSERT(0);
    }

    /*-----------------------------------------------------------------------
      Register cbacks for the IFACE_PHYS_LINK_UP_EV event with psiface. 
    -----------------------------------------------------------------------*/
    if((dssifcb_info.phys_link_down_cback =
                ps_iface_alloc_event_cback_buf(dssocki_iface_ev_cback,
                                               NULL)) == NULL)
    {
      MSG_ERROR("Could not alloc event buf",0,0,0);
      ASSERT(0);
    }
    if(ps_iface_event_cback_reg( NULL,
                                 IFACE_PHYS_LINK_DOWN_EV,
                                 dssifcb_info.phys_link_down_cback) != 0)
    {
      MSG_ERROR("Could not reg PHYS_LINK_DOWN evt ",0,0,0);
      ASSERT(0);
    }

  }

} /* dssifcb_init() */

#endif /* FEATURE_DS_SOCKETS */


