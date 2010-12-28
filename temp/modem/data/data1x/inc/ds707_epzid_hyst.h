#ifndef DS707_EPZID_HYST_H
#define DS707_EPZID_HYST_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                     P Z I D  H Y S T E R E S I S

GENERAL DESCRIPTION
  This file contains functions used to handle PZID Hysteresis feature.

EXTERNALIZED FUNCTIONS

 DS707_PZID_HYSTERESIS_PROCESS_EVENT
   This function is the Hysteresis event machine. It is the access point 
   for all the external files. It is called whenever there is a change in 
   call state to either going active/dormant or if the signal is set for the 
   timer or BS sends an SOCM to turn on/off hysteresis machine.
   
 DS707_PZID_HYSTERESIS_PROCESS_HT 
    Function to process the hysteresis timer (HT).
 
 DS707_PZID_HYSTERESIS_ADD_NEW_PZID 
    Funciton to be called while in  hysteresis state to add new pzid to 
	the list and also starts a timer on all the existing entries.
 
 
 DS707_PZID_HYSTERESIS_PRINT_PZID_LIST
    Funtion which prints the hysteresis list along with the HT timer value.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    Hysteresis state will be turned on by default.
    
   Copyright (c) 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_epzid_hyst.h#2 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $
  
when       who    what, where, why
--------   ---    --------------------------------------------------------
05/26/03   ms     Initial revision.


===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "ds707_pkt_mgr.h"
/*===========================================================================
                       EXTERN VARIABLES
===========================================================================*/
extern rex_timer_type ds_epzid_ht_timer; /* EPZID hysteresis timer           */
extern boolean ds707_epzid_hysteresis_enabled;

/*===========================================================================
                                TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
  These are the inputs to the EPZID Hysteresis machine.
  NULL   : Hysteresis in null state.
  DATA_READY : Leave the Hysteresis state. This is done when there is user
         data ready to be sent/received
  CALL_ENDED :Time to go into hysteresis 
         state by starting the hysteresis activation timer.
  TIMERS_EXPIRED: This input is given when either Hysteresis Activation Timer 
          (HAT) or Hysteresis Timer (HT) expires.
  SOCM_HYS_ENABLED  : SOCM sent to turn on the hysteresis state by having 
           list lenght == 1.
  SOCM_HYS_DISABLED : SOCM message sent to turn off the hysteresis state, by
           having list lenght >1. 
  CALL_CONNECTED/GO_NULL :Time to stop the Hysteresis Activation timer
  SDB_DATA : Leave the hysteresis state and stop and start HAT 
            (i.e. restart HAT)
---------------------------------------------------------------------------*/
typedef enum
{
 EPZID_HYS_NULL_EVENT                   = 0, /* PKT call NULL state         */
 EPZID_HYS_DATA_READY_EVENT             = 1, /* Leave hysteresis state      */
 EPZID_HYS_CALL_ENDED_EVENT             = 2, /* Enter hsyteresis state      */
 EPZID_HYS_TIMER_EXPIRED_EVENT          = 3, /* Hat/HT expired              */
 EPZID_HYS_SOCM_HYS_ENABLED_EVENT       = 4, /* SOCM has list length = 1    */
 EPZID_HYS_SOCM_HYS_DISABLED_EVENT      = 5, /* SOCM has list length > 1    */ 
 EPZID_HYS_CALL_CONNECTED_EVENT         = 6, /* Stop HAT timer              */
 EPZID_HYS_GO_NULL_EVENT                = 7, /* Stop HAT when call goes NULL*/
 EPZID_HYS_SDB_DATA_EVENT               = 8  /* Exit hysteresis, restart HAT*/

}ds707_epzid_hysteresis_event_type;


/*---------------------------------------------------------------------------
   Hysteresis activation timer (HAT) and hysteresis timer (HT) 
   as defined by the standard in seconds.
---------------------------------------------------------------------------*/
#define EPZID_HYSTERESIS_HAT_TIMER_IN_SEC 30 
#define EPZID_HYSTERESIS_HT_TIMER_IN_SEC  60

extern uint16 ds707_epzid_hysteresis_hat_timer_in_sec;
extern uint16 ds707_epzid_hysteresis_ht_timer_in_sec;


/*===========================================================================
                                 EXTERNS
===========================================================================*/
/*===========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_PROCESS_EVENT

DESCRIPTION
  This function is the Hysteresis state machine. This functioni is the access
  point for the external files. It is called from dsmgr.c whenever there is
  a change in call state to either going active/dormant or if the signal is
  set for the timer. When the call goes active, this fuction will clear all
  the hystersis state variables.
  When the call goes dormant, after dormant timer expired, this funciton
     will start the Hysteresis Activation Timer.
  When a signal is set for any hysteresis timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ds707_epzid_hysteresis_process_event
(
  ds707_epzid_hysteresis_event_type hys_ev  
);

/*===========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_PRINT_PZID_LIST

DESCRIPTION
  This function prints the current visited EPZID list as a series of 
  MSG_HIGHs. This is for debug purposes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void ds707_epzid_hysteresis_print_epzid_list
(
  void
);

/*===========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_ADD_NEW_EPZID

DESCRIPTION
  This funciton is called only in hysteresis state and this will add the
  new epzid to the list and also starts a timer on all the existing entries.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ds707_epzid_hysteresis_add_new_epzid
(
  ds707_epzid_type *new_EPZID 
);



/*=========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_POST_EVENT

DESCRIPTION
  Post a command to DS task corresponding to a EPZID hsyterisis event, so that
  all processing of the EPZID hysterisis state machine will happen in the DS
  task context.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ds707_epzid_hysteresis_post_event
(
   ds707_epzid_hysteresis_event_type event
);



/*===========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_PROCESS_HT

DESCRIPTION
  This function is invoked when a new EPZID is added for hysterisis enabled
  state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ds707_epzid_hysteresis_process_ht
(
  void
);
#endif /* FEATURE_EZPID_HYSTERESIS */
#endif /* DS707_EPZID_HYSTERESIS_H */
