#ifndef DS707_RMSM_H
#define DS707_RMSM_H
/*===========================================================================

                          D S 7 0 7 _ R M S M . H

DESCRIPTION

  The Data Services IS707 Rm State Machine source file. This state machine 
  manages the Um and Rm interfaces for certain IS707 packet calls.  
  Specifically RMSM is used to support traditional Relay, SIP Network Model 
  and Mobile IP type IS707 calls which are a special case of the 
  generic multimode PS architecture.

EXTERNALIZED FUNCTIONS
  ds707_rmsm_init()
    Initialize the Rm State Machine.
  ds707_rmsm_post_event()
    Post an event to Rm State Machine.
  ds707_rmsm_process_cmd()  
    Process a cmd posted to Rm State Machine.
  ds707_rmsm_is_packet_call()
    Is RM in IS707 laptop pkt data call.
  ds707_rmsm_is_mip_up()
    Is RM in IS707 laptop MobileIP pkt data call.
  ds707_rmsm_dial_str_cb()
    Dial string handler for IS707 pkt data calls.
  ds707_rmsm_is_iface_in_use()
    Is IS707 iface in a laptop pkt data call.
  ds707_rmsm_rt_acl_post_proc()
    Post processing function for IS707 iface ACL.
  

Copyright (c) 2001 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rmsm.h_v   1.6   23 Jan 2003 16:34:28   randrew  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_rmsm.h#2 $ $DateTime: 2009/05/27 05:07:18 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
05/05/03    usb    Incorporated Code review comments. Moved state enum and
                   RMSM info block to .c file since all that is private.
04/11/03    ak     Updated all vars/functions to include ds707_, as there
                   were naming collisions with GPRS.
01/20/03    ar     Added dsat_dial_modifier_info_type to 
                   ds707_rmsm_dial_str_cb interface.
11/19/02    usb    Added ACL processing and post processing functions to 
                   check if UM iface is in use
11/14/02    jd/usb Renamed RMSM_WAITING_FOR_RM_UM_DOWN_STATE to 
                   RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE 
            jd     Removed um_mip_changed, added rmsm_is_packet_call()
09/10/02    ak     Updated for new dial string handler.
08/28/02    usb    Added/renamed some event names
05/14/02    usb    Re-created module (original filename - dsmip_rmsm.h)
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef FEATURE_DATA_IS707
#include "dstask.h"
#include "dsat.h"
#include "ps_iface.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   
/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  DS707_RMSM_MIN_EV = -1,
  DS707_RMSM_RM_WANTS_PKT_CALL_EV = 0,     /* Rm wants packet call         */
  DS707_RMSM_RM_IFACE_DOWN_EV = 1,         /* Rm interface is down         */
  DS707_RMSM_UM_IFACE_DOWN_EV = 2,         /* Um interface is down         */
  DS707_RMSM_UM_PHYS_LINK_UP_EV = 3,       /* Traffic channel is up        */
  DS707_RMSM_UM_PHYS_LINK_DOWN_EV = 4,     /* Traffic channel is down      */
  DS707_RMSM_UM_PPP_DOWN_EV = 5,           /* Um PPP is down               */
  DS707_RMSM_RM_PPP_UP_EV = 6,             /* Rm PPP is up                 */
  DS707_RMSM_UM_PPP_UP_EV = 7,             /* Um PPP is up                 */
  DS707_RMSM_UM_MIP_UP_EV = 8,             /* Um MIP is up                 */
  DS707_RMSM_UM_MIP_DOWN_EV = 9,           /* Um MIP is down               */
  DS707_RMSM_MAX_EV
} ds707_rmsm_event_type;
           
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS707_RMSM_INIT()

  DESCRIPTION
    This function initializes the Rm state machine.
                                                                                        
  PARAMETERS
    iface_ptr: Pointer to the interface which is used by this instance 
               of Rm state machine to bring up pkt calls.

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean ds707_rmsm_init
(
  ps_iface_type *iface_ptr
);


/*===========================================================================
  FUNCTION DS707_RMSM_POST_EVENT()

  DESCRIPTION
    This function receives posted event and queues the corresponding 
    command to be processed in the ps task.
    
  PARAMETERS
    event: the event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
extern void ds707_rmsm_post_event
(
  ds707_rmsm_event_type event
);


/*===========================================================================
  FUNCTION DS707_RMSM_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to RMSM in DS task and calls 
    an appropriate function to handle it.

  PARAMETERS
    ds_cmd_ptr: Cmd buffer passed with the cmd 

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
extern void ds707_rmsm_process_cmd
(
  ds_cmd_type *ds_cmd_ptr
);


/*===========================================================================
  FUNCTION DS707_RMSM_IS_PACKET_CALL()

  DESCRIPTION
    Returns true if we're in a packet call

  PARAMETERS
    None

  RETURN VALUE
    TRUE - If RM is in IS707 laptop pkt data call
    FALSE - Otherwise

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean ds707_rmsm_is_packet_call
(
  void
);


/*===========================================================================
  FUNCTION DS707_RMSM_IS_MIP_UP()

  DESCRIPTION
    Returns true if we're in a laptop MobileIP pkt data call

  PARAMETERS
    None

  RETURN VALUE
    TRUE - If laptop Mobile IP call is up
    FALSE - Otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean ds707_rmsm_is_mip_up
(
  void
);


/*===========================================================================
  FUNCTION DS707_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function processes the dial string it has registered for and
    initiates the appropriate data call over IS707_PKT_IFACE.

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only
    modifiers: Flags passed as part of the dial string

  RETURN VALUE
    Abort handler callback.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern dsat_call_abort_cb_type ds707_rmsm_dial_str_cb
(
  const byte *dial_string,
  boolean    digit_mode,
  dsat_dial_modifier_info_type  * modifiers
);


/*===========================================================================
FUNCTION DS707_RMSM_IS_IFACE_IN_USE

DESCRIPTION
  This function returns whether IS707 iface is already in a laptop pkt 
  data call or not ie iface is being used by another entity or not.
  
  PARAMETERS
    this_if_ptr: iface ptr to process on  

DEPENDENCIES
  None

RETURN VALUE
  TRUE if in use, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean ds707_rmsm_is_iface_in_use
(
  void *this_if_ptr
);


/*===========================================================================
FUNCTION DS707_RMSM_RT_ACL_POST_PROC

DESCRIPTION
  This function performs the post processing for the ACL lookup for IS707 
  iface.  It sets the in use flag to true if the iface is in a call.  This 
  allows only one application (presumably MIPIO) to use this iface whne we are 
  in laptop call.  This check also works fine during  simple IP laptop call 
  since ACLs are anyway disabled and we don't even come here in that case.
  
  PARAMETERS
    ipfilter_info_ptr: IP packet information
    policy_info_ptr: Policy information
    this_if_ptr: iface ptr to process on

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void ds707_rmsm_rt_acl_post_proc
(
  acl_ipfilter_info_type *ipfilter_info_ptr,
  acl_policy_info_type *policy_info_ptr,
  void *this_if_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_RM_IFACE_UP

DESCRIPTION   Returns TRUE if the RM iface is up.

DEPENDENCIES  phys_link should be up or coming up.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_rmsm_is_rm_iface_up( void );

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_RMSM_H */

