/*===========================================================================

                          P S _ P P P _ S N P I P . H

DESCRIPTION

  This file provides implementation to support ppp device level snooping on 
  all IP packets. The file contains the state machine implementation and 
  actual snooping functions to snoop on all IP packets including - UDP, 
  ICMP, non compresed TCP, VJUCH and VJCH.  

EXTERNALIZED FUNCTIONS
  snoop_ip_init()
    Initialize the IP Snoop State Machine.
  snoop_ip_post_event()
    Post an event to IP Snoop State Machine.

  Copyright (c) 2001-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_snpip.c_v   1.0   13 Feb 2003 14:21:22   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_snpip.c#3 $ $DateTime: 2008/06/27 15:37:54 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/28/08    pp     Fixed RVCT compiler warnings.
02/22/08    pp     Klocwork fixes.
05/02/03    usb    created file
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#include "assert.h"
#include "err.h"
#include "msg.h"
#include "internet.h"

#include "ps_ppp_snpip.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "ps_iface.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_snoop.h"
#include "ps_ppp_slhc.h"
#include "dssocket.h"
#include "ps_iface_ipfltr.h"
#include "ps_iputil.h"
#include "dsbyte.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*---------------------------------------------------------------------------
  Declaration of the states of the state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  SNOOP_IP_MIN_STATE     = -1,
  SNOOP_IP_NULL_STATE    = 0,    /* Initial state, ppp dev down            */
  SNOOP_IP_NONE_STATE    = 1,    /* PPP dev up, IP filtering not enabled   */
  SNOOP_IP_NONCOMP_STATE = 2,    /* IP filtering enabled, no VJ compreesed */
  SNOOP_IP_ALL_STATE     = 3,    /* Filter all IP including VJ compressed  */
  SNOOP_IP_MAX_STATE
} snoop_ipi_state_type;

/*---------------------------------------------------------------------------
  The IP snoop state machine control block, one per PPP device
---------------------------------------------------------------------------*/
typedef struct
{
  snoop_ipi_state_type state;             /* Current state                 */
  ps_iface_type        *if_ptr;           /* rx iface for this PPP dev     */
  uint8       c_slot;                     /* Current VJ slot id for stack  */
  boolean     l_slot_arr[SLHC_ARRAY_SIZE];/* is slot id for local sock     */
  uint16      n_comp_sock;                /* num local socks doing VJCH    */
  sint15      comp_sock_arr[DSS_MAX_TCP_SOCKS];  /* local socks doing VJCH */
  boolean     f_toss;                     /* toss flag for local VJ slots  */
  void        *ev_buf_ptr;                /* buffer for fltr_update ev cb  */
  uint32      tag_ip;                     /* IP snoop callback tag         */
  uint32      tag_vjuch;                  /* VJUCH snoop callback tag      */
  uint32      tag_vjch;                   /* VJCH snoop callback tag       */
} snoop_ipi_info_type;

LOCAL snoop_ipi_info_type snpipi_cb_arr[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  State machine is initialized or not?
---------------------------------------------------------------------------*/
boolean inited = FALSE;

/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*---------------------------------------------------------------------------
  Callbacks registered with various modules
---------------------------------------------------------------------------*/
LOCAL snoop_cb_ret_val_enum_type snoop_ipi_process_ip_cb
(
  ppp_dev_enum_type dev,
  uint16 protocol,      
  dsm_item_type **item_head_ptr
);

LOCAL snoop_cb_ret_val_enum_type snoop_ipi_process_vjuch_cb
(
  ppp_dev_enum_type dev,
  uint16 protocol,      
  dsm_item_type **item_head_ptr
);

LOCAL snoop_cb_ret_val_enum_type snoop_ipi_process_vjch_cb
(
  ppp_dev_enum_type dev,
  uint16 protocol,      
  dsm_item_type **item_head_ptr
);

LOCAL void snoop_ipi_iface_ev_handler_cb
(
  ps_iface_type *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void *user_data_ptr
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION SNOOP_IPI_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition of the SM with the specified 
    instance to the specified state and performs any processing required to
    transition into the new state.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void snoop_ipi_transition_state
(
  ppp_dev_enum_type dev,                        /* device instance of SM   */
  snoop_ipi_state_type new_state                /* state to transition to  */
) 
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(dev < PPP_MAX_DEV);

  MSG_MED("Snoop IP dev %d state %d to %d", 
          dev, snpipi_cb_arr[dev].state, new_state);

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state based upon the 
    current state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    case SNOOP_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        If we are snooping, deregister the snoop functions and reset all
        state variables required for VJCH detetcion. For all cases 
        deregister ps_iface IPFLTR_UPDATED event.
      ---------------------------------------------------------------------*/
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_ALL_STATE:        
        case SNOOP_IP_NONCOMP_STATE:
          snoop_unreg_event(snpipi_cb_arr[dev].tag_ip);
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjuch);
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjch);

          snpipi_cb_arr[dev].c_slot = 255;
          snpipi_cb_arr[dev].n_comp_sock = 0;
          snpipi_cb_arr[dev].f_toss = FALSE;
          
          memset(snpipi_cb_arr[dev].l_slot_arr, 
                 0, 
                 sizeof(snpipi_cb_arr[dev].l_slot_arr));

          memset(snpipi_cb_arr[dev].comp_sock_arr, 
                 0, 
                 sizeof(snpipi_cb_arr[dev].comp_sock_arr));

      
          /* fall through for further cleanup */

        case SNOOP_IP_NONE_STATE:
          ps_iface_event_cback_dereg(snpipi_cb_arr[dev].if_ptr,
                                     IFACE_IPFLTR_UPDATED_EV,
                                     snpipi_cb_arr[dev].ev_buf_ptr);
          
          /* reset the iface ptr since PPP dev is down */
          snpipi_cb_arr[dev].if_ptr = NULL;          
          break;

        case SNOOP_IP_NULL_STATE:
          /* do nothing */
          break;

        default:
          ERR("Invalid state %d", snpipi_cb_arr[dev].state, 0, 0);
          ASSERT(0);
          return;
      }

      break;

    case SNOOP_IP_NONE_STATE:
      /*---------------------------------------------------------------------
        If we are snooping, deregister the snoop functions and reset all
        state variables required for VJCH detetcion. If we were at NULL 
        state register for IPFLTR_UPDATED ev to detect when the filtering
        is enabled on the iface.
      ---------------------------------------------------------------------*/
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_ALL_STATE:    
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjch);

          /* fall through for further cleanup */

        case SNOOP_IP_NONCOMP_STATE:
          snoop_unreg_event(snpipi_cb_arr[dev].tag_ip);
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjuch);


          snpipi_cb_arr[dev].c_slot = 255;
          snpipi_cb_arr[dev].n_comp_sock = 0;
          snpipi_cb_arr[dev].f_toss = FALSE;
          
          memset(snpipi_cb_arr[dev].l_slot_arr, 
                 0, 
                 sizeof(snpipi_cb_arr[dev].l_slot_arr));

          memset(snpipi_cb_arr[dev].comp_sock_arr, 
                 0, 
                 sizeof(snpipi_cb_arr[dev].comp_sock_arr));
      
          break;

        case SNOOP_IP_NULL_STATE:
          /*-----------------------------------------------------------------
            Reg iface IPFLTR_UPDATED ev to detect filtering enable/disable.            
            If event reg fails snooping won't work correctly, so we stay in 
            NULL state. Snooping/IP filtering won't be operational.
          -----------------------------------------------------------------*/
          if (ps_iface_event_cback_reg(snpipi_cb_arr[dev].if_ptr,
                                       IFACE_IPFLTR_UPDATED_EV,
                                       snpipi_cb_arr[dev].ev_buf_ptr) < 0)
          {
            ERR("Event reg failed, cann't snoop IP", 0, 0, 0);
            return;
          }
          break;

        case SNOOP_IP_NONE_STATE:
          /* do nothing */
          break;

        default:
          ERR("Invalid state %d", snpipi_cb_arr[dev].state, 0, 0);
          ASSERT(0);
          return;
      }

      break;

    case SNOOP_IP_NONCOMP_STATE:
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_ALL_STATE:    
          /*-----------------------------------------------------------------
            This happens because all the local sockets doing VJCH are closed
            so there won't be any VJCH packets for stack unless we see 
            anymore VJUCH packets, so stop snooping on VJCH.
          -----------------------------------------------------------------*/
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjch);

          snpipi_cb_arr[dev].c_slot = 255;
          snpipi_cb_arr[dev].n_comp_sock = 0;
          snpipi_cb_arr[dev].f_toss = FALSE;
          
          memset(snpipi_cb_arr[dev].l_slot_arr, 
                 0, 
                 sizeof(snpipi_cb_arr[dev].l_slot_arr));

          memset(snpipi_cb_arr[dev].comp_sock_arr, 
                 0, 
                 sizeof(snpipi_cb_arr[dev].comp_sock_arr));

          break;

        case SNOOP_IP_NULL_STATE:

          /*-----------------------------------------------------------------
            Reg iface IPFLTR_UPDATED ev to detect filtering enable/disable.
            If event reg fails snooping won't work correctly, so we stay in 
            NULL state. Snooping/IP filtering won't be operational.
          -----------------------------------------------------------------*/
          if (ps_iface_event_cback_reg(snpipi_cb_arr[dev].if_ptr,
                                       IFACE_IPFLTR_UPDATED_EV,
                                       snpipi_cb_arr[dev].ev_buf_ptr) < 0)
          {
            ERR("Event reg failed, cann't snoop IP", 0, 0, 0);
            return;
          }

          /* fall through for further cleanup */

        case SNOOP_IP_NONE_STATE:          
          /*-----------------------------------------------------------------
            Now start snooping on IP and VJUCH
          -----------------------------------------------------------------*/
          snpipi_cb_arr[dev].tag_ip = snoop_reg_event(
                                        dev,
                                        PPP_IP_PROTOCOL,
                                        SNOOP_ANY_MSG,
                                        snoop_ipi_process_ip_cb
                                      );

          snpipi_cb_arr[dev].tag_vjuch = snoop_reg_event(
                                           dev,
                                           PPP_VJUCH_PROTOCOL,
                                           SNOOP_ANY_MSG,
                                           snoop_ipi_process_vjuch_cb
                                         );

          break;

        case SNOOP_IP_NONCOMP_STATE:
          /* do nothing */
          break;

        default:
          ERR("Invalid state %d", snpipi_cb_arr[dev].state, 0, 0);
          ASSERT(0);
          return;
      }
      
      break;

    case SNOOP_IP_ALL_STATE:
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_NONCOMP_STATE:
          /*-----------------------------------------------------------------
            This happens when we detect VJUCH packets for stack for the 
            first time, this means that at least one local TCP socket is 
            active doing VJ compression, so start snooping for VJCH.            
          -----------------------------------------------------------------*/
          snpipi_cb_arr[dev].tag_vjch = snoop_reg_event(
                                          dev,
                                          PPP_VJCH_PROTOCOL,
                                          SNOOP_ANY_MSG,
                                          snoop_ipi_process_vjch_cb
                                        );
          break;

        case SNOOP_IP_NULL_STATE:
        case SNOOP_IP_NONE_STATE:          
          /* not allowed */
          ASSERT(0);
          break;

        case SNOOP_IP_ALL_STATE:
          /* do nothing */
          break;

        default:
          ERR("Invalid state %d", snpipi_cb_arr[dev].state, 0, 0);
          ASSERT(0);
          return;
      }
      break;

    default:
      ERR("Invalid state %d", new_state, 0, 0);
      ASSERT(0);
      return;
  } /* switch(new state) */

  snpipi_cb_arr[dev].state = new_state;

} /* snoop_ipi_transition_state() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_PPP_DEV_UP_EV()

  DESCRIPTION
    This function processes SNOOP_IP_PPP_DEV_UP_EV which indicates that
    the underlying PPP device is up and ready to transfer IP packets. 
    If the IP filtering is enabled on the associatd iface, the SM will 
    start snooping.
    
  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void snoop_ipi_process_ppp_dev_up_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{ 
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  ASSERT(dev < PPP_MAX_DEV);

  MSG_LOW("Recvd PPP_DEV_UP_EV, state %d", snpipi_cb_arr[dev].state, 0, 0);

  switch(snpipi_cb_arr[dev].state)
  {      
    case SNOOP_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        PPP device is up, save the iface ptr associated with this PPP dev. 
        If the iface asscociated with this PPP device has IP filtering 
        enabled we transition state to start snooping on all IP 
        (except VJCH TCP) protocol packets. Otherwise just transition state
        to indicate that the device is up. 
      ---------------------------------------------------------------------*/
      snpipi_cb_arr[dev].if_ptr = ppp_cb_array[dev].rx_iface_ptr;

      if(PS_IFACE_NUM_FILTERS(snpipi_cb_arr[dev].if_ptr) > 0)
      {
        snoop_ipi_transition_state(dev, SNOOP_IP_NONCOMP_STATE);
      }
      else
      {
        snoop_ipi_transition_state(dev, SNOOP_IP_NONE_STATE);
      }
      break;

    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:    
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring PPP_DEV_UP_EV, state %d",
              snpipi_cb_arr[dev].state, 0, 0 );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid state %d, dev %d", snpipi_cb_arr[dev].state, dev, 0);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ppp_dev_up_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_PPP_DEV_DOWN_EV()

  DESCRIPTION
    This function processes SNOOP_IP_PPP_DEV_DOWN_EV which indicates that
    the underlying PPP device is down and SM can stop snooping.
    
  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void snoop_ipi_process_ppp_dev_down_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{ 
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  ASSERT(dev < PPP_MAX_DEV);

  MSG_LOW("Recvd PPP_DEV_DOWN_EV, state %d", snpipi_cb_arr[dev].state, 0, 0);

  switch(snpipi_cb_arr[dev].state)
  {      
    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:    
      /*---------------------------------------------------------------------
        PPP device is down, no data to snoop on, go ot NULL state
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NULL_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring PPP_DEV_DOWN_EV, state %d",
              snpipi_cb_arr[dev].state, 0, 0 );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid state %d, dev %d", snpipi_cb_arr[dev].state, dev, 0);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ppp_dev_down_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_FLTR_ENABLED_EV()

  DESCRIPTION
    This function processes SNOOP_IP_FLTR_ENABLED_EV which indicates
    that IP filtering has been enabled on the associated iface, SM will 
    start snooping if it was not already doing so.    
    
  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void snoop_ipi_process_ip_fltr_enabled_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{ 
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  ASSERT(dev < PPP_MAX_DEV);

  MSG_LOW("Recvd IP_FLTR_ENABLED_EV, state %d",
          snpipi_cb_arr[dev].state, 0, 0);

  switch(snpipi_cb_arr[dev].state)
  {      
    case SNOOP_IP_NONE_STATE:
      /*---------------------------------------------------------------------
        IP filtering is enabled on iface when underlying PPP device is 
        already up (transferring data), start snooping.
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NONCOMP_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring IP_FLTR_ENABLED_EV, state %d",
              snpipi_cb_arr[dev].state, 0, 0 );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid state %d, dev %d", snpipi_cb_arr[dev].state, dev, 0);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_fltr_enabled_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_FLTR_DISABLED_EV()

  DESCRIPTION
    This function processes SNOOP_IP_FLTR_DISABLED_EV which indicates
    that IP filtering has been disabled on the associated iface, SM will not
    snoop anymore.
    
  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void snoop_ipi_process_ip_fltr_disabled_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{ 
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  ASSERT(dev < PPP_MAX_DEV);

  MSG_LOW("Recvd IP_FLTR_DISABLED_EV,state %d",snpipi_cb_arr[dev].state,0,0);

  switch(snpipi_cb_arr[dev].state)
  {      
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        IP Filtering has been disabled on iface, stop snooping, don't go 
        NULL since PPP device is still up, to handle the case when filtering
        gets enabled again
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NONE_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONE_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring IP_FLTR_DISABLED_EV, state %d",
              snpipi_cb_arr[dev].state, 0, 0 );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid state %d, dev %d", snpipi_cb_arr[dev].state, dev, 0);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_fltr_disabled_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_COMP_ON_EV()

  DESCRIPTION
    This function processes SNOOP_IP_COMP_ON_EV which indicates that VJ 
    compressed packets destined to the stack have been detected, SM will 
    start snooping on VJCH protocol packets also.
    
  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void snoop_ipi_process_ip_comp_on_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{ 
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  ASSERT(dev < PPP_MAX_DEV);

  MSG_LOW("Recvd IP_COMP_ON_EV, state %d", snpipi_cb_arr[dev].state, 0, 0);

  switch(snpipi_cb_arr[dev].state)
  {      
    case SNOOP_IP_NONCOMP_STATE:    
      /*---------------------------------------------------------------------
        VJ compressed packet detected for stack, sp we need to also snoop on
        VJCH along with all other IP protocols.
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_ALL_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring IP_COMP_ON_EV, state %d",
              snpipi_cb_arr[dev].state, 0, 0 );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid state %d, dev %d", snpipi_cb_arr[dev].state, dev, 0);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_comp_on_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_COMP_OFF_EV()

  DESCRIPTION
    This function processes SNOOP_IP_COMP_OFF_EV which indicates that stack 
    will not be receiving anymore VJCH packets without getting any VJUCH 
    packets, so snooping on compressed packets can be turned off until 
    any VJUCH packets are detected for the stack.
        
  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void snoop_ipi_process_ip_comp_off_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{ 
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  ASSERT(dev < PPP_MAX_DEV);

  MSG_LOW("Recvd IP_COMP_OFF_EV, state %d", snpipi_cb_arr[dev].state, 0, 0);

  switch(snpipi_cb_arr[dev].state)
  {      
    case SNOOP_IP_ALL_STATE:    
      /*---------------------------------------------------------------------
        All local sockets doing VJCH have been closed, so stop snooping on
        VJCH for now.
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NONCOMP_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_NONCOMP_STATE:    
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring IP_COMP_OFF_EV, state %d",
              snpipi_cb_arr[dev].state, 0, 0 );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid state %d, dev %d", snpipi_cb_arr[dev].state, dev, 0);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_comp_off_ev() */


/*===========================================================================
FUNCTION   SNOOP_IPI_PROCESS_IP_CB()

DESCRIPTION
  This callback function is called when any non compressed IP pkt (prot=0x21)
  is received on a PPP device for which IP filtering has been 
  enabled. Pkt is processed through the filters currently installed in the
  iface associated with the PPP dev and based on the processing it is either
  passed up the stack or forwarded to the bridged PPP device. In case there
  is an error in processing the pkt, it is still forwarded to give TE a 
  chance to look at the pkt (in case it is useful).
   
DEPENDENCIES
  This callback should only be registered in SNOOP_IP_NONCOMP_STATE or
  SNOOP_IP_ALL_STATE.
  
RETURN VALUE
  SNOOP_CB_FORWARD_PKT: The pkt is either bad, undetected or does not belong
                        to the stack,so forward it to the PPP bridge device.

  SNOOP_CB_PROCESS_PKT: the packet needs to be sent up the stack since it
                        is detected to be belonging to a local socket.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type snoop_ipi_process_ip_cb
(
  ppp_dev_enum_type dev,               /* device on which pkt arrived      */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **pkt_ptr              /* item containing pkt data         */
)
{
  ip_filter_info_type pkt_info;  /* IP pkt info being processed            */
  uint16 ip_len;                 /* IP header length                       */
  sint15 result;                 /* result of inbound IP filter processing */
  uint8 buf[4];
  boolean is_tcp_rst = FALSE, is_tcp_fin = FALSE;
  uint16 loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(pkt_ptr != NULL && *pkt_ptr != NULL);
  ASSERT(dev < PPP_MAX_DEV);
  ASSERT(protocol == PPP_IP_PROTOCOL);
  
  /* Initialize the ip packet info */
  memset(&pkt_info, 0, sizeof(ip_filter_info_type));

  if(dsm_length_packet(*pkt_ptr) < IPLEN)
  {
    /* The packet is shorter than a legal IP header */
    return SNOOP_CB_FORWARD_PKT;
  }

  /* Sneak a peek at the IP header's IHL field to find its length */
  ip_len = ((*pkt_ptr)->data_ptr[0] & 0xf) << 2;
  
  if(ip_len < IPLEN)
  {
    /* The IP header length field is too small */
    return SNOOP_CB_FORWARD_PKT;
  }

  if(cksum(NULL, *pkt_ptr, ip_len) != 0)
  {
    /* Bad IP header checksum; discard */
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Now extract the IP header, copy only, don't remove the header
  -------------------------------------------------------------------------*/
  if(ntohip(&(pkt_info.ip_hdr.v4), pkt_ptr, TRUE) < 0)
  {
    /* error in extracting IP header */
    return SNOOP_CB_FORWARD_PKT;
  }

  if(pkt_info.ip_hdr.v4.version != IP_V4)
  {
    /* We can't handle IP version other than 4 */
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    If this an IP fragment, don't send it up the stack, we don't expect very
    large IP pkts.  If we need to handle them later, send a copy to stack
    along with forwarding it to the bridged device.
  -------------------------------------------------------------------------*/
  if(!(pkt_info.ip_hdr.v4.flags.mf == 0 && pkt_info.ip_hdr.v4.offset == 0))
  {
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Now extract protocol specific info
  -------------------------------------------------------------------------*/
  memset(buf, 0, sizeof(buf));
  switch(pkt_info.ip_hdr.v4.protocol)
  {
    case TCP_PTCL:
      /* extract src and dst port */
      dsm_extract(*pkt_ptr, ip_len, buf, 2*sizeof(uint16));

      pkt_info.ptcl_info.tcp.src_port = get16(&buf[0]);
      pkt_info.ptcl_info.tcp.dst_port = get16(&buf[2]);

      /* now extract flags to get fin and rst bits */
      buf[0] = 0;
      dsm_extract(*pkt_ptr, ip_len, buf, sizeof(uint8));
      is_tcp_rst = buf[0] & 0x04;
      is_tcp_fin = buf[0] & 0x01;

      break;

    case UDP_PTCL:
      /* extract src and dst port */
      dsm_extract(*pkt_ptr, ip_len, buf, 2*sizeof(uint16));
            
      pkt_info.ptcl_info.udp.src_port = get16(&buf[0]);
      pkt_info.ptcl_info.udp.dst_port = get16(&buf[2]);
      break;

    case ICMP_PTCL:
      /* extract type and code fields */
      dsm_extract(*pkt_ptr, ip_len+13, buf, 2*sizeof(uint8));
                  
      pkt_info.ptcl_info.icmp.type = buf[0];
      pkt_info.ptcl_info.icmp.code = buf[1];
      break;

    default:
      /* unrecognized protocol, forward the pkt */
      MSG_LOW("Unknown protocol 0x%x on dev %d", 
              pkt_info.ip_hdr.v4.protocol, dev, 0);
      return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Now run the pkt through the currently installed filters in the iface on 
    which the pkt is received to see if the pkt need to go up the stack. If
    one of the filters pass, the associated sockfd will be returned.
  -------------------------------------------------------------------------*/
  if((result = ps_iface_ipfltr_execute(snpipi_cb_arr[dev].if_ptr,
                                       &pkt_info)) == 0)
  {
    return SNOOP_CB_FORWARD_PKT;    
  }

  /*-------------------------------------------------------------------------
    If we come here it means the pkt belongs to the stack.  Before we pass 
    If it is a TCP pkt and if either fin or rst bit is set, it indicates a 
    close of TCP socket.  Is that socket happens to be doing VJCH take the 
    socket out of the list since the soocket is going ot be closed.  If no 
    local socket is doing VJ anymore stop snooping on VJCH pkts.
  -------------------------------------------------------------------------*/
  if(pkt_info.ip_hdr.v4.protocol == TCP_PTCL &&
    (is_tcp_rst || is_tcp_fin))
  {
    for(loop=0; loop<DSS_MAX_TCP_SOCKS; loop++)
    {
      if(snpipi_cb_arr[dev].comp_sock_arr[loop] == result)
      {
        snpipi_cb_arr[dev].comp_sock_arr[loop] = 0;
        snpipi_cb_arr[dev].n_comp_sock--;
        break;
      }
    }

    if(snpipi_cb_arr[dev].n_comp_sock == 0)
    {
      snoop_ip_post_event(dev, SNOOP_IP_COMP_OFF_EV);
    }
  }

  /* return appropriate code to pass the pkt up the stack */
  return SNOOP_CB_PROCESS_PKT;

} /* snoop_ipi_process_ip_cb() */


/*===========================================================================
FUNCTION   SNOOP_IPI_PROCESS_VJUCH_CB()

DESCRIPTION
  This callback function is called when an LCP/IPCP C-Req is received on the
  Rm link i.e. when a PPP resync is detected on the Rm link once the call
  has been established. Drop the packet in such event.

DEPENDENCIES
  This callback should only be registered in SNOOP_IP_NONCOMP_STATE or
  SNOOP_IP_ALL_STATE.
  
RETURN VALUE
  SNOOP_CB_IGNORE_PKT: ignore the packet since it has either been dropped
                       or has been forwarded to the PPP bridge device

  SNOOP_CB_PROCESS_PKT: the packet needs to be sent up the stack since it
                        is destiined to a local socket.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type snoop_ipi_process_vjuch_cb
(
  ppp_dev_enum_type dev,               /* device on which pkt arrived      */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **pkt_ptr              /* item containing pkt data         */
)
{
  ip_filter_info_type pkt_info;  /* IP pkt info being processed            */
  uint16 ip_len;                 /* IP header length                       */
  sint15 result;                 /* result of inbound IP filter processing */
  uint8 buf[4];
  uint16 loop, index;
  uint8 slot;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(pkt_ptr != NULL && *pkt_ptr != NULL);
  ASSERT(dev < PPP_MAX_DEV);
  ASSERT(protocol == PPP_VJUCH_PROTOCOL);

  MSG_ERROR("RECEIVED UNCOMPRESSED TCP", 0, 0, 0);
  
  /* Initialize the ip packet info */
  memset(&pkt_info, 0, sizeof(ip_filter_info_type));

  if(dsm_length_packet(*pkt_ptr) < IPLEN)
  {
    /* The packet is shorter than a legal IP header */
    snpipi_cb_arr[dev].f_toss = TRUE;
    return SNOOP_CB_FORWARD_PKT;
  }

  /* Sneak a peek at the IP header's IHL field to find its length */
  ip_len = ((*pkt_ptr)->data_ptr[0] & 0xf) << 2;
  
  if(ip_len < IPLEN)
  {
    /* The IP header length field is too small */
    snpipi_cb_arr[dev].f_toss = TRUE;
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Now extract the IP header, copy only, don't remove the header
  -------------------------------------------------------------------------*/
  if(ntohip(&(pkt_info.ip_hdr.v4), pkt_ptr, TRUE) < 0)
  {
    /* error in extracting IP header */
    snpipi_cb_arr[dev].f_toss = TRUE;
    return SNOOP_CB_FORWARD_PKT;
  }

  if(pkt_info.ip_hdr.v4.version != IP_V4)
  {
    /* We can't handle IP version other than 4 */
    snpipi_cb_arr[dev].f_toss = TRUE;
    return SNOOP_CB_FORWARD_PKT;
  }

  /* extract the slot id from the protocol field */
  slot = pkt_info.ip_hdr.v4.protocol;
  if(slot >= SLHC_ARRAY_SIZE)
  {
    /* The slot id is out of range */
    snpipi_cb_arr[dev].f_toss = TRUE;
    return SNOOP_CB_FORWARD_PKT;
  }

  /* got a valid slot id, use it */
  snpipi_cb_arr[dev].c_slot = slot;
  snpipi_cb_arr[dev].f_toss = FALSE;
  pkt_info.ip_hdr.v4.protocol = TCP_PTCL;

  /*-------------------------------------------------------------------------
    Now extract protocol specific info
  -------------------------------------------------------------------------*/
  memset(buf, 0, sizeof(buf));
  switch(pkt_info.ip_hdr.v4.protocol)
  {
    case TCP_PTCL:
      /* extract src and dst port */
      dsm_extract(*pkt_ptr, ip_len, buf, 2*sizeof(uint16));

      pkt_info.ptcl_info.tcp.src_port = get16(&buf[0]);
      pkt_info.ptcl_info.tcp.dst_port = get16(&buf[2]);

      break;

    case UDP_PTCL:
      /* extract src and dst port */
      dsm_extract(*pkt_ptr, ip_len, buf, 2*sizeof(uint16));
            
      pkt_info.ptcl_info.udp.src_port = get16(&buf[0]);
      pkt_info.ptcl_info.udp.dst_port = get16(&buf[2]);
      break;

    case ICMP_PTCL:
      /* extract type and code fields */
      dsm_extract(*pkt_ptr, ip_len+13, buf, 2*sizeof(uint8));
                  
      pkt_info.ptcl_info.icmp.type = buf[0];
      pkt_info.ptcl_info.icmp.code = buf[1];
      break;

    default:
      /* unrecognized protocol, forward the pkt, unmark the slot */
      MSG_LOW("Unknown protocol 0x%x on dev %d", 
              pkt_info.ip_hdr.v4.protocol, dev, 0); 
      snpipi_cb_arr[dev].l_slot_arr[slot] = FALSE;
      return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Now run the pkt through the currently installed filters in the iface on 
    which the pkt is received to see if the pkt need to go up the stack. If
    one of the filters pass, the associated sockfd will be returned.
  -------------------------------------------------------------------------*/
  if((result = ps_iface_ipfltr_execute(snpipi_cb_arr[dev].if_ptr,
                                       &pkt_info)) == 0)
  {
    /* unmark the slot since the pkt needs to be forwarded */
    snpipi_cb_arr[dev].l_slot_arr[slot] = FALSE;
    return SNOOP_CB_FORWARD_PKT;    
  }

  /*-------------------------------------------------------------------------
    If we come here it means the pkt belongs to the stack. Mark the slot
    as belonging to us, add the sockfd to the list of sockets active with VJ
    compression and post event to start snooping for VJCH if we are not
    doing so already.
  -------------------------------------------------------------------------*/

  snpipi_cb_arr[dev].l_slot_arr[slot] = TRUE;

  index = DSS_MAX_TCP_SOCKS;
  for(loop=0; loop<DSS_MAX_TCP_SOCKS; loop++)
  {
    if(snpipi_cb_arr[dev].comp_sock_arr[loop] == result)
    {
      break;
    }
            
    if(snpipi_cb_arr[dev].comp_sock_arr[loop] == 0)
    {
      index = loop;
    }
  }

  if(loop == DSS_MAX_TCP_SOCKS)
  {
    ASSERT(index < DSS_MAX_TCP_SOCKS);
    snpipi_cb_arr[dev].comp_sock_arr[index] = result;
    snpipi_cb_arr[dev].n_comp_sock++;
    
    /* if first socket doing VJ, enable snooping */
    if(snpipi_cb_arr[dev].n_comp_sock == 1)
    {
      snoop_ip_post_event(dev, SNOOP_IP_COMP_ON_EV);
    }
  }

  /* return appropriate code to pass the pkt up the stack */
  return SNOOP_CB_PROCESS_PKT;

} /* snoop_ipi_process_vjuch_cb() */


/*===========================================================================
FUNCTION   SNOOP_IPI_PROCESS_VJCH_CB()

DESCRIPTION
  This callback function is called when an LCP/IPCP C-Req is received on the
  Rm link i.e. when a PPP resync is detected on the Rm link once the call
  has been established. Drop the packet in such event.

DEPENDENCIES
  This callback should only be registered in SNOOP_IP_ALL_STATE.
  
RETURN VALUE
  SNOOP_CB_IGNORE_PKT: ignore the packet since it has either been dropped
                       or has been forwarded to the PPP bridge device

  SNOOP_CB_PROCESS_PKT: the packet needs to be sent up the stack since it
                        is destiined to a local socket.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type snoop_ipi_process_vjch_cb
(
  ppp_dev_enum_type dev,               /* device on which pkt arrived      */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **pkt_ptr              /* item containing pkt data         */
)
{
  uint8 change_mask, slot;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(pkt_ptr != NULL && *pkt_ptr != NULL);
  ASSERT(dev < PPP_MAX_DEV);
  ASSERT(protocol == PPP_VJCH_PROTOCOL);
  
  MSG_ERROR("RECEIVED VHCH", 0, 0, 0);

  /* we expect at least 3 bytes, 1 byte change mask, 2 bytes IP checksum */
  if(dsm_length_packet(*pkt_ptr) < 3)
  {
    /* The packet is shorter than a legal IP header */
    return SNOOP_CB_FORWARD_PKT;
  }

  /* Sneak at the change mask in the pkt */
  change_mask = (*pkt_ptr)->data_ptr[0];

  /*-------------------------------------------------------------------------
    If the slot id is explicitly included in the pkt (C bit set) use it. If 
    not and if toss flag is set that means we had error in processing 
    previous pkt, hence slot id cannot be assumed, forward the pkt.
  -------------------------------------------------------------------------*/
  if(change_mask & NEW_C)
  {
    dsm_extract(*pkt_ptr, 1, &slot, sizeof(uint8));
    if(slot >= SLHC_ARRAY_SIZE)
    {
      /* The slot id is out of range */
      snpipi_cb_arr[dev].f_toss = TRUE;
      return SNOOP_CB_FORWARD_PKT;
    }

    /* got a valid slot id, use it */
    snpipi_cb_arr[dev].c_slot = slot;
    snpipi_cb_arr[dev].f_toss = FALSE;
  }
  else if(snpipi_cb_arr[dev].f_toss)
  {
    return SNOOP_CB_FORWARD_PKT;  
  }
  
  /*-------------------------------------------------------------------------
    We have a valid slot id, check to see if the slot id belongs to a local
    socket, if not forward the pkt.
  -------------------------------------------------------------------------*/
  if(snpipi_cb_arr[dev].l_slot_arr[snpipi_cb_arr[dev].c_slot])
  {
    return SNOOP_CB_FORWARD_PKT;
  }

  /* return appropriate code to pass the pkt up the stack */
  return SNOOP_CB_PROCESS_PKT;

} /* snoop_ipi_process_vjch_cb() */


/*===========================================================================
FUNCTION   SNOOP_IPI_IFACE_EV_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the events, registered with 
  the iface associated with a PPP device, happen. The events are handled 
  as needed and appropriate event is posted to IP snoop SM.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void snoop_ipi_iface_ev_handler_cb
(
  ps_iface_type *this_iface_ptr,           /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
)
{
  ppp_dev_enum_type dev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW("Recd event %d, i/f 0x%x", event, this_iface_ptr, 0);

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  ASSERT(dev < PPP_MAX_DEV);
  ASSERT(this_iface_ptr == snpipi_cb_arr[dev].if_ptr);

  switch(event)
  {
    case IFACE_IPFLTR_UPDATED_EV:
      /*---------------------------------------------------------------------
        If the number of filters registered with the iface goes from
        0 -> nonzero notify SM that the filtering is enabled. Similarly if 
        the number of filters goes from non zero -> 0, notify SM that the 
        filtering is disabled. In all other cases, the snooping state should
        remain the same.
      ---------------------------------------------------------------------*/
      if(event_info.n_filters == 0 &&
         PS_IFACE_NUM_FILTERS(this_iface_ptr) > 0)
      {
        MSG_HIGH("IP fltr on, dev %d, i/f 0x%x", dev, this_iface_ptr, 0);
        snoop_ip_post_event(dev, SNOOP_IP_FILTERING_ENABLED_EV);
      }
      else if(event_info.n_filters > 0 &&
         PS_IFACE_NUM_FILTERS(this_iface_ptr) == 0)
      {
        MSG_HIGH("IP fltr off, dev %d, i/f 0x%x", dev, this_iface_ptr, 0);
        snoop_ip_post_event(dev, SNOOP_IP_FILTERING_DISABLED_EV);
      }
      else
      {
        MSG_LOW("Ignoring event %d", event, 0, 0);
      }

      break;

    default:    
      /*---------------------------------------------------------------------
        SM never registered for any other events
      ---------------------------------------------------------------------*/
      MSG_ERROR("Unexpected event %d, i/f 0x%x", event, this_iface_ptr, 0);
      ASSERT(0);
      break;

  } /* switch(event) */
} /* snoop_ipi_iface_ev_handler_cb() */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION SNOOP_IP_INIT()

  DESCRIPTION
    This function initializes all the instcances of the IP snooping state 
    machine (one for each PPP device).  It also registers all the command 
    handlers with PS task interface.

  PARAMETERS
    None
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    Resets all the control block variables.
===========================================================================*/
void snoop_ip_init(void)
{
  /* PPP device instance, same as associated SM instance */
  ppp_dev_enum_type dev;              
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inited == TRUE)
  {
    MSG_MED("Snoop IP SM already inited", 0, 0, 0);
    return;
  }

  for(dev = PPP_MIN_DEV; dev < PPP_MAX_DEV; dev++)
  {
    memset(snpipi_cb_arr+dev, 0, sizeof(snoop_ipi_info_type));
    
    snpipi_cb_arr[dev].c_slot = 255;
    snpipi_cb_arr[dev].state = SNOOP_IP_NULL_STATE;
    snpipi_cb_arr[dev].ev_buf_ptr = ps_iface_alloc_event_cback_buf
                                     (
                                       snoop_ipi_iface_ev_handler_cb,
                                       (void *)dev
                                     );

    if(snpipi_cb_arr[dev].ev_buf_ptr == NULL)
    {
      ERR_FATAL("Snoop IP event buf alloc failed", 0, 0, 0);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Now register the cmd handlers corresponding to all the supported events
  -------------------------------------------------------------------------*/

  ps_set_cmd_handler(PS_SNOOP_IP_PPP_DEV_UP_CMD, 
                     snoop_ipi_process_ppp_dev_up_ev);

  ps_set_cmd_handler(PS_SNOOP_IP_PPP_DEV_DOWN_CMD, 
                     snoop_ipi_process_ppp_dev_down_ev);

  ps_set_cmd_handler(PS_SNOOP_IP_FLTR_ENABLED_CMD, 
                     snoop_ipi_process_ip_fltr_enabled_ev);

  ps_set_cmd_handler(PS_SNOOP_IP_FLTR_DISABLED_CMD, 
                     snoop_ipi_process_ip_fltr_disabled_ev);

  ps_set_cmd_handler(PS_SNOOP_IP_COMP_ON_CMD, 
                     snoop_ipi_process_ip_comp_on_ev);
  
  ps_set_cmd_handler(PS_SNOOP_IP_COMP_OFF_CMD, 
                     snoop_ipi_process_ip_comp_off_ev);

  inited = TRUE;
  
  return;
} /* snoop_ip_init() */


/*===========================================================================
  FUNCTION SNOOP_IP_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to the SM and enqueues the
    corresponding command to be processed in the PS task.

  PARAMETERS
    dev: PPP device type to indentify associated SM instance
    event: The event that is being posted to the SM.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
void snoop_ip_post_event
(
  ppp_dev_enum_type     dev,
  snoop_ip_event_type   event
)
{
  ps_cmd_enum_type cmd;                          /* PS Task cmd            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!inited)
  {
    ERR("Snoop IP SM not initialized", 0, 0, 0);
    ASSERT(0);
    return;
  }

  if(dev >= PPP_MAX_DEV)
  {
    ERR("Invalid device %d, post event failed", dev, 0, 0);
    ASSERT(0);
    return;
  }

  MSG_HIGH("Snoop IP dev %d, ev %d in state %d",
           dev, event, snpipi_cb_arr[dev].state);

  /*-------------------------------------------------------------------------
    For each event type post a corresponding cmd to PS
  -------------------------------------------------------------------------*/
  switch(event)
  {
    case SNOOP_IP_PPP_DEV_UP_EV:
      /*---------------------------------------------------------------------
        PPP device is up for traffic, start snooping if IP filtering enabled
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_PPP_DEV_UP_CMD;
      break;

    case SNOOP_IP_PPP_DEV_DOWN_EV:
      /*---------------------------------------------------------------------
        PPP device is going down, stop snooping
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_PPP_DEV_DOWN_CMD;
      break;

    case SNOOP_IP_FILTERING_ENABLED_EV:
      /*---------------------------------------------------------------------
        IP filtering has been enabled on the iface tied to this device
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_FLTR_ENABLED_CMD;
      break;

    case SNOOP_IP_FILTERING_DISABLED_EV:
      /*---------------------------------------------------------------------
        IP filtering has been disabled on the iface tied to this device
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_FLTR_DISABLED_CMD;
      break;

    case SNOOP_IP_COMP_ON_EV:
      /*---------------------------------------------------------------------
        First VJ compressed packet seen on this device, start snooping VJCH
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_COMP_ON_CMD;
      break;

    case SNOOP_IP_COMP_OFF_EV:
      /*---------------------------------------------------------------------
        All sockets doing VJCH have closed, don't snoop VJCH anymore
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_COMP_OFF_CMD;
      break;

    default:
      ERR("Snoop IP invalid event %d", event, 0, 0 );
      ASSERT(0);
      return;
  } /* switch(event) */

  /*-------------------------------------------------------------------------
    Now post the cmd and dev (=SM instance) to be executed in PS context
  -------------------------------------------------------------------------*/
  ps_send_cmd(cmd, (void *)dev);
  
  return;
} /* snoop_ip_post_event() */


#endif /* FEATURE_DATA */

