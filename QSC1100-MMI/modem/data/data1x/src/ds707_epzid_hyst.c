/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                     E P Z I D   H Y S T E R E S I S

GENERAL DESCRIPTION
  This file contains functions used to handle PZID Hysteresis feature.

EXTERNALIZED FUNCTIONS

 DS707_EPZID_HYSTERESIS_PROCESS_EVENT
   This function is the Hysteresis event machine. It is the access point 
   for all the external files. It is called whenever there is a change in 
   call state to either going active/dormant or if the signal is set for the 
   timer or BS sends an SOCM to turn on/off hysteresis machine.
   
 DS707_EPZID_HYSTERESIS_PROCESS_HT 
    Function to process the hysteresis timer (HT).
 
 DS707_EPZID_HYSTERESIS_ADD_NEW_PZID 
    Funciton to be called while in  hysteresis state to add new pzid to 
 the list and also starts a timer on all the existing entries.
 
 
 DS707_EPZID_HYSTERESIS_PRINT_PZID_LIST
    Funtion which prints the hysteresis list along with the HT timer value.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    Hysteresis state will be turned on by default.
    
   Copyright (c) 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_epzid_hyst.c#2 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $
  
when       who     what, where, why
--------   ---     --------------------------------------------------------
05/02/09   ms     Initial revision.


===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "assert.h"
#include "err.h"
#include "msg.h"
#include "mccsyobj.h"
#include "clk.h"
#include "ds3gtimer.h"
#include "event.h"
#include "ds707_epzid_hyst.h"
#include "ds707_epzid_util.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

typedef enum
{
 EPZIDI_HYS_NULL_STATE = 0,
 EPZIDI_HYS_INIT_STATE = 1,
 EPZIDI_HYS_HAT_STATE  = 2,
 EPZIDI_HYS_HT_STATE   = 3
}epzidi_hysteresis_state_type;

/*------------------------------------------------------------------------
  Enable pzid hysteresis by default.
-------------------------------------------------------------------------*/
static epzidi_hysteresis_state_type epzidi_hys_state = EPZIDI_HYS_INIT_STATE;

/*-------------------------------------------------------------------------
  When Hysteresis is turned on, the maximum list length after the mobile 
  goes dormant becomes 4 (as per standard).
------------------------------------------------------------------------*/
#define EPZIDI_HYSTERESIS_DORM_LIST_LEN       0x04

#define HYST_DEBUG(msg,a,b,c) MSG_MED(msg,a,b,c)
/*-------------------------------------------------------------------------
   Hysteresis activation timer (HAT) and hysteresis timer (HT) 
   as defined by the standard in seconds.
-------------------------------------------------------------------------*/
uint16 ds707_epzid_hysteresis_hat_timer_in_sec = EPZID_HYSTERESIS_HAT_TIMER_IN_SEC;
uint16 ds707_epzid_hysteresis_ht_timer_in_sec =  EPZID_HYSTERESIS_HT_TIMER_IN_SEC;

/*-------------------------------------------------------------------------
 Variable to inform the status of the hysteresis state. By default
 hysteresis is always turned on , hence make it TRUE.
-------------------------------------------------------------------------*/
boolean ds707_epzid_hysteresis_enabled = TRUE;

/*-------------------------------------------------------------------------
  Hysteresis activation indicator : 
  Make it TRUE, when, user data is Transmitted/Received(call active).
  Make it false, after HAT is started or when call is NULL state.
-------------------------------------------------------------------------*/
static boolean epzidi_hysteresis_hai = FALSE;

static dword pzidi_hysteresis_sys_uptime;  /* system uptime*/
static boolean pzidi_timer_started = FALSE;/* hysteresis timer started */
/*===========================================================================
                        INTERNAL FUNCTION DECLARATIONS
===========================================================================*/

static void epzidi_hysteresis_start_hat(void);
static void epzidi_hysteresis_stop_hat(void);
static void epzidi_hysteresis_remove_pzid_index(uint16 );
static void epzidi_hysteresis_null_state(void);
static void epzidi_hysteresis_init_state(void);
static void epzidi_hysteresis_process_data_ready_ev(void);
static void epzidi_hysteresis_process_start_hat(void);
static void epzidi_hysteresis_process_stop_hat(void);
static void epzidi_hysteresis_process_hysteresis_timers_expired_ev(void);
static void epzidi_hysteresis_process_socm_hys_enabled_ev(void);
static void epzidi_hysteresis_process_socm_hys_disabled_ev(void);

/*===========================================================================

MACRO        : PZIDI_START_TIMER

DESCRIPTION  : Starts PZID hysteresis timer and keeps track the information
               of starting the timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define EPZIDI_START_TIMER( duration )                                        \
             if( pzidi_timer_started == FALSE)                               \
             {                                                               \
                ds3gi_start_timer( DS3G_TIMER_PZID_HYSTERESIS,  duration );  \
                pzidi_timer_started = TRUE;                                  \
             }                                                               \
             else                                                            \
             {                                                               \
                HYST_DEBUG("EPZIDI_START_TIMER, timer already running",0,0,0);\
             }                                                               \

/*===========================================================================
MACRO        : EPZIDI_STOP_TIMER

DESCRIPTION  : Stops PZID hysteresis timer only if it has been started.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
#define EPZIDI_STOP_TIMER()  if( pzidi_timer_started )                        \
                            {                                                \
                               ds3gi_stop_timer( DS3G_TIMER_PZID_HYSTERESIS);\
                               pzidi_timer_started = FALSE;                  \
        }
/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================

FUNCTION     : EPZIDI_HYSTERESIS_START_HAT

DESCRIPTION  : This function starts the hysteresis activation timer for the 
               time specified. It clears the hysteresis activation indicator.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void epzidi_hysteresis_start_hat
(
  void
)
{
  uint16 hat_timer_in_sec = 0;
 /*------------------------------------------------------------------------*/  

  /*-------------------------------------------------------------------------
  If there is no user data, then start hysteresis activation timer for 0 secs
  The timer will expire instantanneously and the mobile gets into 
  HT state without any delay. 
  ---------------------------------------------------------------------------*/
  HYST_DEBUG("start_hat(), HAI = %d, old_pzid_index = %d,epzidi_hys_state = %d",
   epzidi_hysteresis_hai, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  /*-------------------------------------------------------------------------
    IF (HAI == TRUE) implies MS sent some data;
    THEN
        set HAT_TIMER to the user defined value
    ELSE
        retain the default value for the HAT_TIMER = 0
  ---------------------------------------------------------------------------*/
  if(epzidi_hysteresis_hai == TRUE)
  {
    hat_timer_in_sec = ds707_epzid_hysteresis_hat_timer_in_sec;
  }

    /*-------------------------------------------------------------------------
      Before starting hysteresis activation timer, make sure we are in 
      HAT substate.
      Start the timer by making the hysteresis activation indicator false.
    -------------------------------------------------------------------------*/
  epzidi_hysteresis_hai = FALSE;          

  MSG_HIGH("Starting HAT for %d sec, HAI %d",(hat_timer_in_sec),
            epzidi_hysteresis_hai,0);
  EPZIDI_START_TIMER( hat_timer_in_sec*1000 );
  event_report(EVENT_PZID_HAT_STARTED);
  
} /* epzidi_hysteresis_start_hat() */

/*===========================================================================

FUNCTION     : EPZIDI_HYSTERESIS_STOP_HAT

DESCRIPTION  : This function stops the hysteresis activation timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void epzidi_hysteresis_stop_hat
(
  void
)
{
  HYST_DEBUG("stop_hat(), HAI = %d, old_pzid_index = %d,epzidi_hys_state = %d",
             epzidi_hysteresis_hai, ds707_epzid_oldest_epzid, epzidi_hys_state); 

  EPZIDI_STOP_TIMER(); 

  /* Need to add event for HAT stopped     */
  /* event_report(EVENT_PZID_HAT_STOPPED); */
  
} /* epzidi_hysteresis_start_hat() */

/*===========================================================================

FUNCTION    :    PZIDI_HYSTERESIS_PROCESS_HAT

DESCRIPTION :   Called when the hysteresis activation timer is expired.The
                list lenght is changed from 1 to 4 and goes into HYSTERESIS state.
  
DEPENDENCIES:  None

RETURN VALUE:  None

SIDE EFFECTS:  None
===========================================================================*/
static void ds707_epzidi_hysteresis_process_hat
(
  void
)
{
  HYST_DEBUG("process_hat(), hyst_enabled = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_hysteresis_enabled, ds707_epzid_oldest_epzid, epzidi_hys_state); 
 /*------------------------------------------------------------------------*/  
    
  /*-------------------------------------------------------------------------
    Cancel timer before processing it.
  -------------------------------------------------------------------------*/
  EPZIDI_STOP_TIMER(); 
  
  /*-----------------------------------------------------------------------
   If the EPZID hysteresis feature was disabled and the timer signal was 
  disabeld make sure we exit without processing as the state is lost
  ------------------------------------------------------------------------*/
  if(ds707_epzid_hysteresis_enabled == FALSE)
  {
   MSG_HIGH("EPZID Hysteresis feature is disabled",0,0,0);
   return;
  }

  /*----------------------------------------------------------------------
     Hysteresis Activation Timer expired, 
     increase the EPZID List length to 4. 
   -----------------------------------------------------------------------*/
  INTLOCK()
  
  ds707_epzid_max_list_len  = EPZIDI_HYSTERESIS_DORM_LIST_LEN;
  
  INTFREE()

   
  MSG_HIGH("HAT Expired. List : %d",ds707_epzid_max_list_len,0,0);
  event_report(EVENT_PZID_HAT_EXPIRED);

} /* epzidi_hysteresis_process_hat() */


/*===========================================================================

FUNCTION    : DS707_EPZID_HYSTERESIS_PROCESS_HT

DESCRIPTION : This function processes the hysteresis timer(HT) .Whenever HT 
              expires it removes that EPZID entry from the list. It starts the 
			  HT timer on the pzid entry with the shortest timer value.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_epzid_hysteresis_process_ht
(
  void
)
{
  rex_timer_cnt_type tm_cnt = 0;   /* Timer used to start HT  */
  byte n                    = 1;   /* Loop counter variable */ 
  int32  time_left;              
  ds707_epzid_type ht_start_pzid;   /* EPZID on which HT starts */
 /*------------------------------------------------------------------------*/  
    
   HYST_DEBUG("process_ht(), hyst_enabled = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_hysteresis_enabled, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  /*-------------------------------------------------------------------------
    Cancel timer before processing it.
  -------------------------------------------------------------------------*/
  EPZIDI_STOP_TIMER(); 

  /*-----------------------------------------------------------------------
  If the EPZID hysteresis feature was disabled and the timer signal was 
  disabeld make sure we exit without processing as the state is lost
  ------------------------------------------------------------------------*/
  if(ds707_epzid_hysteresis_enabled == FALSE)
  {
   MSG_HIGH("EPZID Hysteresis feature is disabled",0,0,0);
   return;
  }
  
  /*---------------------------------------------------------------------
     Go through the list and remove expired. 
     Start HT on the smallest timer value.
    
     The uptime is the time elapsed in seconds since the phone started. 
     Uptime returns dword which is unsigned 32 bit value,which is a big
     number for seconds and it will take many many years to overflow. 
     That's why we don't need any seperate logic to cover the edge cases.
   -----------------------------------------------------------------------*/
  pzidi_hysteresis_sys_uptime = clk_uptime() ; 
    
  while( n < ds707_epzid_oldest_epzid ) 
  {
  
     /*-------------------------------------------------------------------
     Go through the list and see if the HT expired on
     any EPZID, If true, then remove the element from
     the list and start HT on the smallest timer value.
     --------------------------------------------------------------------*/
     time_left = ds707_epzid_list[n].hyst_exp_time- pzidi_hysteresis_sys_uptime;
     ht_start_pzid = ds707_epzid_list[n];
  
    /*---------------------------------------------------------------------
     Haven't used time_left here because for comparision because if the 
     comparision results in a 32bit negative number then it would be a 
     problem, so using this expression the compiler would takecare of it.
    -----------------------------------------------------------------------*/
     if( ds707_epzid_list[n].hyst_exp_time <= pzidi_hysteresis_sys_uptime)
     {
       /*----------------------------------------------------------------
         We don't increment n here because removing 
         the PZID takes an element out at that location 
         and the next element is moved into that location.
       ----------------------------------------------------------------*/
       epzidi_hysteresis_remove_pzid_index( n );
     }
     else
     {
       /*--------------------------------------------------------------------
        Get the smallest timer value.
       ---------------------------------------------------------------------*/
       tm_cnt = ( (0 == tm_cnt ) ? (time_left) : ( MIN(tm_cnt,time_left) ) );
       n++;
     }
  }/* while */
    
  /*------------------------------------------------------------------------- 
   If new expiration time exists start the time. 
   ------------------------------------------------------------------------- */
  if( tm_cnt ) 
  {
     MSG_HIGH("Starting HT for %d sec, uptime %d",tm_cnt, 
                                 pzidi_hysteresis_sys_uptime, 0);
     EPZIDI_START_TIMER(tm_cnt*1000);

     event_report_payload( EVENT_PZID_HT_STARTED,sizeof(ht_start_pzid),
                                                      &ht_start_pzid );
  }
  else if(n > 1)
  {
     MSG_FATAL( " HT trying to start with zero timer",0,0,0);
  }

} /* ds707_pzid_hysteresis_process_ht() */

/*===========================================================================

FUNCTION    :EPZIDI_HYSTERESIS_REMOVE_PZID_INDEX

DESCRIPTION : This function removes a EPZID at the specified index. If the
              current EPZID zone where the mobile is residing is same as the 
	      one that expired then the EPZID is added again to the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void epzidi_hysteresis_remove_pzid_index
(
  uint16 index 
)
{
  ds707_epzid_type ht_exp_pzid ;             /* Stores the pzid to be removed       */
  ds707_epzid_type pzidi_hysteresis_current_pzid ;  /* Current PZID of the MS       */
/*-----------------------------------------------------------------------*/
  if (index >= DS707_EPZID_MAX_LEN)
  {
    MSG_ERROR("Invalid index %d for the pzid list is passed", index,0,0);
    return;
  }
  HYST_DEBUG("remove_pzid_index(), list = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  /*--------------------------------------------------------------------- 
   Store the PZID that you are going to remove.
  -------------------------------------------------------------------------*/
  ht_exp_pzid = ds707_epzid_list[index];
  
  /*------------------------------------------------------------------------
   Since a PZID entry is going to be removed,decrement the 
   ds707_pzid_oldest_pzid.
  -------------------------------------------------------------------------*/
  --ds707_epzid_oldest_epzid;
  
  /*------------------------------------------------------------------------
    Remove the pzid from the list by moving the elements up and then assign
    the last pzid to zero.
  -------------------------------------------------------------------------*/
  while( (index <= ds707_epzid_oldest_epzid) && (index < (DS707_EPZID_MAX_LEN - 1))
          && (index < (EPZIDI_HYSTERESIS_DORM_LIST_LEN - 1)) )
  {
    MSG_HIGH("HT Expired on pzid_list[%d]: Pzid %d Sid %d",index, ht_exp_pzid.pzid,ht_exp_pzid.sid);
    MSG_HIGH(" Nid %d",ht_exp_pzid.nid,0,0);
    event_report_payload(EVENT_PZID_HT_EXPIRED, sizeof(ht_exp_pzid), &ht_exp_pzid);
    ds707_epzid_list[index] = ds707_epzid_list[index+1];
    index++;
  }
  
  /*---------------------------------------------------------------------------
   If the removed PZID is same as the current then add the PZID to the list at 
   the top and request for an origination.
  -----------------------------------------------------------------------------*/
  pzidi_hysteresis_current_pzid = ds707_pzid_curr_epzid;
  if(ds707_epzid_compare_epzids(&ht_exp_pzid, &pzidi_hysteresis_current_pzid)
                                == EPZID_NO_CHANGE)
  {
    ds707_epzid_hysteresis_add_new_epzid(&pzidi_hysteresis_current_pzid);

    MSG_MED("Originating on new PZID= %d, SID=%d NID=%d after adding to list", 
             pzidi_hysteresis_current_pzid.pzid, 
             pzidi_hysteresis_current_pzid.sid,
             pzidi_hysteresis_current_pzid.nid); 
    ds707_epzid_reconnect_cb( );
  }
  else
  {
    MSG_MED("PZID= %d SID = %d NID = %d already on list, no orig",
             pzidi_hysteresis_current_pzid.pzid, 
             pzidi_hysteresis_current_pzid.sid,
             pzidi_hysteresis_current_pzid.nid); 
  }
  ds707_epzid_hysteresis_print_epzid_list();
  return;

} /* pzidi_hysteresis_remove_pzid_index()  */


/*===========================================================================

FUNCTION PZIDI_HYSTERESIS_INIT_STATE

DESCRIPTION
  This function is called when Hysteresis SM enters INIT state. The Hys SM
  enters init state when hysteresis is enabled and pkt call is active.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void epzidi_hysteresis_init_state
(
   void
)
{
/*-----------------------------------------------------------------------*/
  HYST_DEBUG("init_state(), list = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
             ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  /*---------------------------------------------------------------------
    Check to see if PZID feature is registered before we 
	go to the init state for hysteresis.
  -----------------------------------------------------------------------*/
  if(ds707_epzid_max_list_len == 0)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Clears Hysteresis activation timer or Hysteresis timers 
    depending on the state. 
  -------------------------------------------------------------------------*/
    EPZIDI_STOP_TIMER();
    
  /*----------------------------------------------------------------------
      Clear the elements from the list starting at index 1 to maximum 
      elements pointed in the list.
    -----------------------------------------------------------------------*/
    ds707_epzid_clear_epzid_list(1,ds707_epzid_oldest_epzid);
    
    INTLOCK();
  /*----------------------------------------------------------------------
      In Hyseteresis Init state the list length should be made 1.
      
      Sometimes this can be called from different context, so add INTLOCK
      and INTFREE.
    -----------------------------------------------------------------------*/
    ds707_epzid_max_list_len       = 1;       
    ds707_epzid_oldest_epzid        = 1;
    epzidi_hysteresis_hai          = TRUE;
    epzidi_hys_state               = EPZIDI_HYS_INIT_STATE;
    HYST_DEBUG("Init state :HAI = %d, epzidi_hys_state = %d, max list length = %d ",
                epzidi_hysteresis_hai, epzidi_hys_state, ds707_epzid_max_list_len); 
    INTFREE();

} /* ds707_pzid_hysteresis_init_state() */

/*===========================================================================

FUNCTION    : PZIDI_HYSTERESIS_NULL_STATE

DESCRIPTION : Enters null state when packet call is closed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
static void epzidi_hysteresis_null_state
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
     Reset the pzid hysteresis variables
  -------------------------------------------------------------------------*/
  
  HYST_DEBUG("null_state(), list = %d, old_pzid_ind = %d",
              ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, 0); 
  INTLOCK();
  epzidi_hysteresis_hai = FALSE;
  EPZIDI_STOP_TIMER();
  epzidi_hys_state      = EPZIDI_HYS_NULL_STATE;
  INTFREE();
  
  HYST_DEBUG("HAI = %d, epzidi_hys_state = %d ",
                epzidi_hysteresis_hai, epzidi_hys_state, 0); 

}/* pzidi_hysteresis_null_state() */

/*===========================================================================

FUNCTION    : PZIDI_HYSTERESIS_PROCESS_DATA_READY_EV

DESCRIPTION : This function is called whenever data is ready to be 
              transmitted/received

Before State:   NULL    INIT   HAT     HT
After  State:   NULL    INIT   INIT    INIT

DEPENDENCIES

RETURN VALUE
           NONE
SIDE EFFECTS
           NONE
===========================================================================*/
static void epzidi_hysteresis_process_data_ready_ev
(
  void
)
{
  HYST_DEBUG("data_ready_ev(), list = %d, hyst_enabled = %d, epzidi_hys_state = %d", 
   ds707_epzid_max_list_len, ds707_epzid_hysteresis_enabled, epzidi_hys_state); 
  /*-----------------------------------------------------------------------*/
  switch(epzidi_hys_state)
  {
     case EPZIDI_HYS_NULL_STATE:
       if( ds707_epzid_hysteresis_enabled == TRUE ) 
       {
 	        epzidi_hysteresis_init_state();
       }
       break;
        
     case EPZIDI_HYS_INIT_STATE:
     case EPZIDI_HYS_HAT_STATE:
     case EPZIDI_HYS_HT_STATE:
       if( ds707_epzid_hysteresis_enabled == TRUE ) 
       {
         epzidi_hysteresis_init_state();
       }
       else 
       {
         epzidi_hysteresis_null_state();
       }
       break;
  }
} /* pzidi_hysteresis_process_data_ready_ev() */

/*===========================================================================

FUNCTION PZIDI_HYSTERESIS_PROCESS_START_HAT

DESCRIPTION : This function is called when the mobile goes dormant.  If 
              hysteresis is enabled then hysteresis activation timer should
              be started irrespective of the hysteresis activation 
              indicator(HAI). HAI is set whenever there is user data.

Before State:  NULL     INIT    HAT     HT
After  State:  NULL     HAT     n/a     n/a    

DEPENDENCIES

RETURN VALUE
            NONE
SIDE EFFECTS
           NONE
===========================================================================*/

 static void epzidi_hysteresis_process_start_hat
(
  void
)
{
 /*-----------------------------------------------------------------------*/
 HYST_DEBUG("start_hat(), list = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  switch(epzidi_hys_state)
  {
     case EPZIDI_HYS_NULL_STATE:
       break;
     
     case EPZIDI_HYS_INIT_STATE:
       epzidi_hysteresis_start_hat();
       epzidi_hys_state = EPZIDI_HYS_HAT_STATE;
       break;
     
     case EPZIDI_HYS_HAT_STATE:
     case EPZIDI_HYS_HT_STATE:
       MSG_ERROR("Start HAT event -> %d state", epzidi_hys_state,0,0);
       break;       
  }
  HYST_DEBUG("HAI = %d",epzidi_hysteresis_hai, 0, 0); 
} /* pzidi_hysteresis_process_start_hat_ev() */

/*===========================================================================

FUNCTION PZIDI_HYSTERESIS_PROCESS_STOP_HAT

DESCRIPTION : This function is called when the mobile comes out of dormancy
              or goes NULL. Hysteresis Activation timer should be 
              canceled here

Before State:  NULL     INIT    HAT     HT
After  State:  NULL     n/a     INIT    n/a    

DEPENDENCIES

RETURN VALUE
            NONE
SIDE EFFECTS
           NONE
===========================================================================*/

 static void epzidi_hysteresis_process_stop_hat
(
  void
)
{
 /*-----------------------------------------------------------------------*/
 HYST_DEBUG("stop_hat(), list = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  switch(epzidi_hys_state)
  {
     case EPZIDI_HYS_NULL_STATE:
       break;
     
     case EPZIDI_HYS_INIT_STATE:
       break;
     
     case EPZIDI_HYS_HAT_STATE:
       epzidi_hysteresis_stop_hat();
       epzidi_hys_state = EPZIDI_HYS_INIT_STATE;
       break;

     case EPZIDI_HYS_HT_STATE:
       MSG_ERROR("Stop HAT -> %d state", epzidi_hys_state,0,0);
       break;       
  }
  HYST_DEBUG("HAI = %d",epzidi_hysteresis_hai, 0, 0); 
} /* pzidi_hysteresis_process_stop_hat_ev() */

/*===========================================================================

FUNCTION PZIDI_HYSTERESIS_PROCESS_HYSTERESIS_TIMERS_EXPIRED_EV

DESCRIPTION : This function is called whenever the hysteresis timer signal is
              set. This function  will process the hysteresis timer, HAT/HT
              depending on the hysteresis stae it is in right now.

Before State    NULL    INIT    HAT    HT
After State      n/a    n/a     HT     HT

DEPENDENCIES

RETURN VALUE 
             NONE
SIDE EFFECTS
             NONE
===========================================================================*/

static void epzidi_hysteresis_process_hysteresis_timers_expired_ev
(
  void
)
{
 /*-----------------------------------------------------------------------*/
  
  HYST_DEBUG("hyst_tmrs_exp(), list = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  switch(epzidi_hys_state)
  {
     case EPZIDI_HYS_NULL_STATE:
     case EPZIDI_HYS_INIT_STATE:
       MSG_FATAL("Never happen expired event:%d",epzidi_hys_state, 0, 0);
       break;
     
     case EPZIDI_HYS_HAT_STATE:
       ds707_epzidi_hysteresis_process_hat();
       epzidi_hys_state = EPZIDI_HYS_HT_STATE;
       break;

     case EPZIDI_HYS_HT_STATE:
       ds707_epzid_hysteresis_process_ht();
       epzidi_hys_state = EPZIDI_HYS_HT_STATE;
       break;
  }
}/* pzidi_hysteresis_process_hysteresis_timers_expired_ev() */

/*===========================================================================
FUNCTION PZIDI_HYSTERESIS_PROCESS_SOCM_ENABLED_EV

DESCRIPTION : This function is called whenever BS sends SOCM to turn on
              hysteresis. All the hysteresis state variables are initialized.

Before State:    NULL   INIT   HAT     HT
After  State :   INIT   INIT   n/a     n/a                   

DEPENDENCIES

RETURN VALUE
             NONE

SIDE EFFECTS
             NONE
===========================================================================*/
static void epzidi_hysteresis_process_socm_hys_enabled_ev
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  ds707_epzid_hysteresis_enabled = TRUE; 
  HYST_DEBUG("socm_hyst_en(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  switch(epzidi_hys_state)
  {
     case EPZIDI_HYS_NULL_STATE:
       epzidi_hysteresis_init_state();
       break;

     case EPZIDI_HYS_INIT_STATE:
       break;

     case EPZIDI_HYS_HAT_STATE:
       MSG_ERROR("Hysteresis Enable -> %d state",epzidi_hys_state,0,0);
       break;       

     case EPZIDI_HYS_HT_STATE:
    /*-----------------------------------------------------------------
     If SOCM is received with list_len=1 during HT state, the list size
     shrinks to 1 and also the pzidi_hys_state should be reverted to
     INIT state after canceling the HT timers. Otherwise, the mobile 
     would be left in a weird state hysteresis state with list size as
     1. The list will also never grow to 4
    -----------------------------------------------------------------*/
       EPZIDI_STOP_TIMER(); 
       epzidi_hys_state = EPZIDI_HYS_INIT_STATE;
       break;
  }
} /* pzidi_hysteresis_process_socm_hys_enabled_ev() */

/*===========================================================================
FUNCTION PZIDI_HYSTERESIS_PROCESS_SOCM_HYS_DISABLED_EV

DESCRIPTION : This function is called when the BS sends SOCM to disable the
              the hysteresis feature. Whenever hysteresis is disabled the
              hysteresis state variables are reset.

Before State   NULL     INIT    HAT    HT
After  State    n/a     NULL   NULL   NULL

DEPENDENCIES

RETURN VALUE
              NONE
SIDE EFFECTS
              NONE
===========================================================================*/

static void epzidi_hysteresis_process_socm_hys_disabled_ev
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  HYST_DEBUG("socm_hys_dis(), list = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  
  switch(epzidi_hys_state)
  {
     case EPZIDI_HYS_NULL_STATE:
       break;
     
     case EPZIDI_HYS_INIT_STATE:
     case EPZIDI_HYS_HAT_STATE:
     case EPZIDI_HYS_HT_STATE:
       epzidi_hysteresis_null_state();
    /*--------------------------------------------
     Hysteresis feature is disabled through SOCM.
    ----------------------------------------------*/
       ds707_epzid_hysteresis_enabled = FALSE; 
       break;
  }
}/* pzidi_hysteresis_process_socm_hys_disabled_ev() */

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_PROCESS_EVENT

DESCRIPTION
  This function is the Hysteresis event machine. This function is the access
  point for the external files. It is called whenever there is a change in 
  call state to either going active/dormant or if the signal is set for the 
  timer or BS sends an SOCM to turn on/off hysteresis machine.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ds707_epzid_hysteresis_process_event
(
   ds707_epzid_hysteresis_event_type event
)
{
 /*-------------------------------------------------------------------------*/
 
 
 
 HYST_DEBUG("hyst_process_ev(),EVENT: %d list = %d, epzidi_hys_state = %d",event,
   ds707_epzid_max_list_len,  epzidi_hys_state); 
 switch(event)
 { 
   case EPZID_HYS_NULL_EVENT:
     
     epzidi_hysteresis_null_state();
     break;

   case EPZID_HYS_DATA_READY_EVENT:
     /*-------------------------------------------------------------------
       Data has become ready, clear all the hysteris state variables.
     -------------------------------------------------------------------- */
     epzidi_hysteresis_process_data_ready_ev();
     break;
   
   case EPZID_HYS_CALL_ENDED_EVENT:
     /*-------------------------------------------------------------------
       Mobile has gone dormant. Start hysteresis activation timer.
     -----------------------------------------------------------------------*/
     epzidi_hysteresis_process_start_hat();
     break;

   case EPZID_HYS_TIMER_EXPIRED_EVENT:
     /*-------------------------------------------------------------------
       Timer signal set. Process hysteresis hat/ht timer
     -----------------------------------------------------------------------*/
     epzidi_hysteresis_process_hysteresis_timers_expired_ev();
     break;
 
   case EPZID_HYS_SOCM_HYS_ENABLED_EVENT: 
     /*-------------------------------------------------------------------
      BS sent SOCM to enable the hysteresis feature.
      -----------------------------------------------------------------------*/
     epzidi_hysteresis_process_socm_hys_enabled_ev();
     break;
   
   case EPZID_HYS_SOCM_HYS_DISABLED_EVENT:
     /*-------------------------------------------------------------------
      BS sent SOCM to disable the hysteresis feature.
     -----------------------------------------------------------------------*/
     epzidi_hysteresis_process_socm_hys_disabled_ev();
     break;
   
   case EPZID_HYS_CALL_CONNECTED_EVENT:
   case EPZID_HYS_GO_NULL_EVENT:
     /*-------------------------------------------------------------------
       Mobile has come out of dormancy or Going NULL. 
       Stop hysteresis activation timer.
     -----------------------------------------------------------------------*/
     epzidi_hysteresis_process_stop_hat();
     break;

   case EPZID_HYS_SDB_DATA_EVENT:
     /*-------------------------------------------------------------------
       SDB data transmitted/received - Leave hysteresis state and restart
       the Hysteresis Activation Timer (HAT)
     -------------------------------------------------------------------- */
     epzidi_hysteresis_process_data_ready_ev();
     epzidi_hysteresis_process_stop_hat();
     epzidi_hysteresis_process_start_hat();
     break;

   default:
     MSG_ERROR( "UNKNOWN EVENT: %d, Hysteresis: %d", event, ds707_epzid_hysteresis_enabled, 0);
     break;
  }
}/* ds707_pzid_hysteresis_process_event() */

/*===========================================================================

FUNCTION DS707_PZID_HYSTERESIS_PRINT_PZID_LIST

DESCRIPTION
  This function prints the current visited PZIDs as a series of messages.
  This is for debug purposes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds707_epzid_hysteresis_print_epzid_list
(
  void
)
{
  byte i=0;                                 /* index to traverse PZID list */
  int exp_duration;
/*-------------------------------------------------------------------------*/
   
  /*-------------------------------------------------------------------------
    Go through the PZID list and print the contents of the PZID list along
   with their timer values.
  -------------------------------------------------------------------------*/    
  
  while(i < ds707_epzid_oldest_epzid)
  {  
    exp_duration = ds707_epzid_list[i].hyst_exp_time
                - pzidi_hysteresis_sys_uptime;
    exp_duration = ((exp_duration > 0)? (exp_duration):0);
    
    MSG_HIGH(" Index %d : PZID:%d SID %d ",i,ds707_epzid_list[i].pzid,
	     ds707_epzid_list[i].sid);	
    MSG_HIGH("NID: %d HT time : %d",ds707_epzid_list[i].nid,
             exp_duration,0);
     i++;
  }
  return;
} /* ds707_pzid_hysteresis_print_pzid_list() */


/*===========================================================================

FUNCTION DS707_PZID_HYSTERESIS_ADD_NEW_PZID

DESCRIPTION
  This funciton is called only in hysteresis state and this will add the
  new pzid to the list and also starts a timer on all the existing entries.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_epzid_hysteresis_add_new_epzid
(
  ds707_epzid_type *new_PZID 
)
{ 
  dword  uptime;   /* Variable for the system uptime.*/
  uint16 n = 0;    /* counter                        */
/*------------------------------------------------------------------------*/
  
  HYST_DEBUG("Add new PZID, Max list = %d, old_pzid_ind = %d,epzidi_hys_state = %d",
   ds707_epzid_max_list_len, ds707_epzid_oldest_epzid, epzidi_hys_state); 
  /*-----------------------------------------------------------------------
   ds707_pzid_oldest_pzid is incremented because we are going to add
   a new PZID.
  ------------------------------------------------------------------------*/
  n = ds707_epzid_oldest_epzid = MIN( ds707_epzid_oldest_epzid+1, 
                                    ds707_epzid_max_list_len );
  
  if(ds707_epzid_oldest_epzid == ds707_epzid_max_list_len && (ds707_epzid_list[n-1].pzid!=0))
  {
   MSG_HIGH( "Deleting PZID[%d],SID[%d], NID[%d]",
   	      ds707_epzid_list[n-1].pzid, 
              ds707_epzid_list[n-1].sid, 
              ds707_epzid_list[n-1].nid);
   MSG_HIGH( "And Hyst time %d , Index %d to add New PZID ",
              ds707_epzid_list[n-1].hyst_exp_time,
                                  (n-1),0 );
  }
  /*-----------------------------------------------------------------------
   New PZID is always added on the top. So move all the existing 
   elements down by one index. 
   Since n is incremented already, need to decrement before we use it.
  --------------------------------------------------------------------------*/
  if( n == 0)
  {
   ASSERT(n!=0)
   MSG_ERROR("ds707_epzid_oldest_epzid should never be zero.",0,0,0);
   return;
  }
  while( --n )
  {
    ds707_epzid_list[n].pzid = ds707_epzid_list[n-1].pzid;
    ds707_epzid_list[n].sid = ds707_epzid_list[n-1].sid;
    ds707_epzid_list[n].nid = ds707_epzid_list[n-1].nid;
    ds707_epzid_list[n].hyst_exp_time =     ds707_epzid_list[n-1].hyst_exp_time; 
  }
  /*------------------------------------------------------------------------
   Place the new PZID at the top of the list (zeroth location).
  --------------------------------------------------------------------------*/
  ds707_epzid_list[0].pzid = new_PZID->pzid;
  ds707_epzid_list[0].sid = new_PZID->sid;
  ds707_epzid_list[0].nid = new_PZID->nid;
  ds707_epzid_list[0].hyst_exp_time = 0;

  MSG_HIGH("Adding new PZID:%d %d %d", new_PZID->pzid,
                                 new_PZID->sid,
                                 new_PZID->nid);
   MSG_HIGH("oldest PZID:index %d", ds707_epzid_oldest_epzid,0,0);
  
  /*-----------------------------------------------------------------------
   Add timer on the previously first PZID.
   The uptime is the time elapsed in seconds since the phone started. 
   Uptime returns dword which is unsigned 32 bit value,which is a big
   number in seconds and it will take many many years to overflow. 
   That's why we don't need any seperate logic to cover the edge cases.
  --------------------------------------------------------------------------*/
  uptime = clk_uptime();
  ds707_epzid_list[n+1].hyst_exp_time = uptime + 
                                        ds707_epzid_hysteresis_ht_timer_in_sec;
  
} /* ds707_pzid_hysteresis_add_new_pzid() */

/*===========================================================================

FUNCTION DS707_PZID_HYSTERESIS_POST_EVENT

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
)
{
  ds_cmd_type     *cmd_ptr;                      /* DS Task msg            */
 /*-------------------------------------------------------------------------*/

 HYST_DEBUG("hyst_post_ev(),EVENT: %d list = %d, epzidi_hys_state = %d",event,
            ds707_epzid_max_list_len,  epzidi_hys_state); 
 switch(event)
 { 
   /*------------------------------------------------------------------------
    Currently only the DATA ready and SDB data event are posted this way. 
    Ignore all other events for now.
   ------------------------------------------------------------------------*/
   case EPZID_HYS_DATA_READY_EVENT:
   case EPZID_HYS_SDB_DATA_EVENT:
     if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
     {
       ERR_FATAL( "PZID hyst cant get cmd buf from DS task", 0, 0, 0 );
     }
     else
     {
       if (event == EPZID_HYS_DATA_READY_EVENT)
       {
         cmd_ptr->hdr.cmd_id = DS_707_PKT_PZID_HYS_DATA_READY_CMD;
       }
       else
       {
         cmd_ptr->hdr.cmd_id = DS_707_PKT_PZID_HYS_SDB_DATA_CMD;
       }
       ds_put_cmd(cmd_ptr);
     }
     break;

   default:
     MSG_HIGH("Ignoring request to post event %d",event, 0, 0);
     break;
 }
}

#endif /* FEATURE_DS_EPZID_HYSTERESIS */
#endif /* FEATURE_DATA_IS707 */

