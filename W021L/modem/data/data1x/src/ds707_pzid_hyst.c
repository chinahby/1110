/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                     P Z I D   H Y S T E R E S I S

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
    
   Copyright (c) 2003 - 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_pzid_hyst.c#3 $ $DateTime: 2009/02/08 20:55:07 $ $Author: ssaha $
  
when       who     what, where, why
--------   ---     --------------------------------------------------------
02/06/09   ss     Lint Critical Fix.
01/28/09   ss     Klocwork Fix.
10/22/04   gr     Changes to make HAT independent of HOLDDOWN Timer and  
                  also to support instantanneous HAT expiry if the mobile 
                  goes dormant without any data transfer. SDB Data command
                  is also processed in the DS context
09/08/04   vr     Changed PZID_HYS_CALL_ACTIVE_EVENT to 
                  PZID_HYS_DATA_READY_EVENT. Also added function to post
                  the DATA_READY_CMD to process the event in DS context
11/17/03   igt    Removed the Start for the PERF_TOTAL_HANDOFF_DELAY 
                  timer from this file. Located elsewhere.
10/14/03   sy     By default hysteresis is turned on & when pzid state is 
                  initialized make sure that pzid feature is registered.
09/17/03   sy     Added support for NV items.
09/02/03   sy     Initial revision.


===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DS_PZID_HYSTERESIS
#ifdef FEATURE_DATA_IS707
#include "assert.h"
#include "err.h"
#include "msg.h"
#include "mccsyobj.h"
#include "clk.h"
#include "ds3gtimer.h"
#include "event.h"
#include  "ds707_pzid.h"
#include "ds707_pzid_hyst.h"
#include "ds707_pkt_mgr.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

typedef enum
{
 PZIDI_HYS_NULL_STATE = 0,
 PZIDI_HYS_INIT_STATE = 1,
 PZIDI_HYS_HAT_STATE  = 2,
 PZIDI_HYS_HT_STATE   = 3
}pzidi_hysteresis_state_type;

/*------------------------------------------------------------------------
  Enable pzid hysteresis by default.
-------------------------------------------------------------------------*/
static pzidi_hysteresis_state_type pzidi_hys_state = PZIDI_HYS_INIT_STATE;

/*-------------------------------------------------------------------------
  When Hysteresis is turned on, the maximum list length after the mobile 
  goes dormant becomes 4 (as per standard).
------------------------------------------------------------------------*/
#define PZIDI_HYSTERESIS_DORM_LIST_LEN       0x04

#define HYST_DEBUG(msg,a,b,c) MSG_MED(msg,a,b,c)
/*-------------------------------------------------------------------------
   Hysteresis activation timer (HAT) and hysteresis timer (HT) 
   as defined by the standard in seconds.
-------------------------------------------------------------------------*/
uint16 ds707_pzid_hysteresis_hat_timer_in_sec = PZID_HYSTERESIS_HAT_TIMER_IN_SEC;
uint16 ds707_pzid_hysteresis_ht_timer_in_sec =  PZID_HYSTERESIS_HT_TIMER_IN_SEC;

/*-------------------------------------------------------------------------
  This array stores the cdma uptime at the time the PZID entry was added.
  This value corresponds to hysteresis timer value.
  dword = Unsigned long = unsigned 32 bit value.
-------------------------------------------------------------------------*/
static dword pzidi_hysteresis_expiration_list[PZIDI_HYSTERESIS_DORM_LIST_LEN];

/*-------------------------------------------------------------------------
 Variable to inform the status of the hysteresis state. By default
 hysteresis is always turned on , hence make it TRUE.
-------------------------------------------------------------------------*/
boolean ds707_pzid_hysteresis_enabled = TRUE;

/*-------------------------------------------------------------------------
  Hysteresis activation indicator : 
  Make it TRUE, when, user data is Transmitted/Received(call active).
  Make it false, after HAT is started or when call is NULL state.
-------------------------------------------------------------------------*/
static boolean pzidi_hysteresis_hai = FALSE;

static dword pzidi_hysteresis_sys_uptime;  /* system uptime*/
static boolean pzidi_timer_started = FALSE;/* hysteresis timer started */
/*===========================================================================
                        INTERNAL FUNCTION DECLARATIONS
===========================================================================*/

static void pzidi_hysteresis_start_hat(void);
static void pzidi_hysteresis_stop_hat(void);
static void pzidi_hysteresis_remove_pzid_index(uint16 );
static void pzidi_hysteresis_null_state(void);
static void pzidi_hysteresis_init_state(void);
static void pzidi_hysteresis_process_data_ready_ev(void);
static void pzidi_hysteresis_process_start_hat(void);
static void pzidi_hysteresis_process_stop_hat(void);
static void pzidi_hysteresis_process_hysteresis_timers_expired_ev(void);
static void pzidi_hysteresis_process_socm_hys_enabled_ev(void);
static void pzidi_hysteresis_process_socm_hys_disabled_ev(void);

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
#define PZIDI_START_TIMER( duration )                                        \
             if( pzidi_timer_started == FALSE)                               \
             {                                                               \
                ds3gi_start_timer( DS3G_TIMER_PZID_HYSTERESIS,  duration );  \
                pzidi_timer_started = TRUE;                                  \
             }                                                               \
             else                                                            \
             {                                                               \
                HYST_DEBUG("PZIDI_START_TIMER, timer already running",0,0,0);\
             }                                                               \

/*===========================================================================
MACRO        : PZIDI_STOP_TIMER

DESCRIPTION  : Stops PZID hysteresis timer only if it has been started.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
#define PZIDI_STOP_TIMER()  if( pzidi_timer_started )                        \
                            {                                                \
                               ds3gi_stop_timer( DS3G_TIMER_PZID_HYSTERESIS);\
                               pzidi_timer_started = FALSE;                  \
        }
/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================

FUNCTION     : PZIDI_HYSTERESIS_START_HAT

DESCRIPTION  : This function starts the hysteresis activation timer for the 
               time specified. It clears the hysteresis activation indicator.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pzidi_hysteresis_start_hat
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
  HYST_DEBUG("start_hat(), HAI = %d, old_pzid_index = %d,pzidi_hys_state = %d",
   pzidi_hysteresis_hai, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  /*-------------------------------------------------------------------------
    IF (HAI == TRUE) implies MS sent some data;
    THEN
        set HAT_TIMER to the user defined value
    ELSE
        retain the default value for the HAT_TIMER = 0
  ---------------------------------------------------------------------------*/
  if(pzidi_hysteresis_hai == TRUE)
  {
    hat_timer_in_sec = ds707_pzid_hysteresis_hat_timer_in_sec;
  }

    /*-------------------------------------------------------------------------
      Before starting hysteresis activation timer, make sure we are in 
      HAT substate.
      Start the timer by making the hysteresis activation indicator false.
    -------------------------------------------------------------------------*/
  pzidi_hysteresis_hai = FALSE;          

  MSG_HIGH("Starting HAT for %d sec, HAI %d",(hat_timer_in_sec),
            pzidi_hysteresis_hai,0);
  PZIDI_START_TIMER( hat_timer_in_sec*1000 );
  event_report(EVENT_PZID_HAT_STARTED);
  
} /* pzidi_hysteresis_start_hat() */

/*===========================================================================

FUNCTION     : PZIDI_HYSTERESIS_STOP_HAT

DESCRIPTION  : This function stops the hysteresis activation timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pzidi_hysteresis_stop_hat
(
  void
)
{
  HYST_DEBUG("stop_hat(), HAI = %d, old_pzid_index = %d,pzidi_hys_state = %d",
   pzidi_hysteresis_hai, ds707_pzid_oldest_pzid, pzidi_hys_state); 

  PZIDI_STOP_TIMER(); 

  /* Need to add event for HAT stopped     */
  /* event_report(EVENT_PZID_HAT_STOPPED); */
  
} /* pzidi_hysteresis_start_hat() */

/*===========================================================================

FUNCTION    : PZIDI_HYSTERESIS_PROCESS_HAT

DESCRIPTION : Called when the hysteresis activation timer is expired.The
              list lenght is changed from 1 to 4 and goes into HYSTERESIS
			  state.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pzidi_hysteresis_process_hat
(
  void
)
{
  HYST_DEBUG("process_hat(), hyst_enabled = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_hysteresis_enabled, ds707_pzid_oldest_pzid, pzidi_hys_state); 
 /*------------------------------------------------------------------------*/  
    
  /*-------------------------------------------------------------------------
    Cancel timer before processing it.
  -------------------------------------------------------------------------*/
  PZIDI_STOP_TIMER(); 
  
  /*-----------------------------------------------------------------------
   If the PZID hysteresis feature was disabled and the timer signal was 
  disabeld make sure we exit without processing as the state is lost
  ------------------------------------------------------------------------*/
  if(ds707_pzid_hysteresis_enabled == FALSE)
  {
   MSG_HIGH("PZID Hysteresis feature is disabled",0,0,0);
   return;
  }

  /*----------------------------------------------------------------------
     Hysteresis Activation Timer expired, 
     increase the PZID List length to 4. 
   -----------------------------------------------------------------------*/
  INTLOCK()
  ds707_pzid_max_list_len = PZIDI_HYSTERESIS_DORM_LIST_LEN;
  INTFREE()

   
  MSG_HIGH("HAT Expired. List : %d",ds707_pzid_max_list_len,0,0);
  event_report(EVENT_PZID_HAT_EXPIRED);

} /* pzidi_hysteresis_process_hat() */


/*===========================================================================

FUNCTION    : DS707_PZID_HYSTERESIS_PROCESS_HT

DESCRIPTION : This function processes the hysteresis timer(HT) .Whenever HT 
              expires it removes that PZID entry from the list. It starts the 
			  HT timer on the pzid entry with the shortest timer value.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pzid_hysteresis_process_ht
(
  void
)
{
  rex_timer_cnt_type tm_cnt = 0;   /* Timer used to start HT  */
  byte n                    = 1;   /* Loop counter variable */ 
  int32  time_left;              
  byte ht_start_pzid        = 0;   /* PZID on which HT starts */
 /*------------------------------------------------------------------------*/  
    
   HYST_DEBUG("process_ht(), hyst_enabled = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_hysteresis_enabled, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  /*-------------------------------------------------------------------------
    Cancel timer before processing it.
  -------------------------------------------------------------------------*/
  PZIDI_STOP_TIMER(); 

  /*-----------------------------------------------------------------------
   If the PZID hysteresis feature was disabled and the timer signal was 
  disabeld make sure we exit without processing as the state is lost
  ------------------------------------------------------------------------*/
  if(ds707_pzid_hysteresis_enabled == FALSE)
  {
   MSG_HIGH("PZID Hysteresis feature is disabled",0,0,0);
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
    
  while( n < ds707_pzid_oldest_pzid ) 
  {
  
     /*-------------------------------------------------------------------
     Go through the list and see if the HT expired on
     any PZId, If true, then remove the element from
     the list and start HT on the smallest timer value.
     --------------------------------------------------------------------*/
     time_left = pzidi_hysteresis_expiration_list[n] - pzidi_hysteresis_sys_uptime;
     ht_start_pzid = ds707_pzid_list[n];
  
    /*---------------------------------------------------------------------
     Haven't used time_left here because for comparision because if the 
     comparision results in a 32bit negative number then it would be a 
  problem, so using this expression the compiler would takecare of it.
    -----------------------------------------------------------------------*/
     if( pzidi_hysteresis_expiration_list[n] <= pzidi_hysteresis_sys_uptime)
     {
       /*----------------------------------------------------------------
         We don't increment n here because removing 
         the PZID takes an element out at that location 
         and the next element is moved into that location.
       ----------------------------------------------------------------*/
       pzidi_hysteresis_remove_pzid_index( n );
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
     PZIDI_START_TIMER(tm_cnt*1000);

     event_report_payload( EVENT_PZID_HT_STARTED,sizeof(ht_start_pzid),
                                                      &ht_start_pzid );
  }
  else if(n > 1)
  {
     MSG_FATAL( " HT trying to start with zero timer",0,0,0);
  }

} /* ds707_pzid_hysteresis_process_ht() */

/*===========================================================================

FUNCTION    :PZIDI_HYSTERESIS_REMOVE_PZID_INDEX

DESCRIPTION : This function removes a PZID at the specified index. If the
              current PZID zone where the mobile is residing is same as the 
			  one that expired then the PZID is added again to the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pzidi_hysteresis_remove_pzid_index
(
  uint16 index 
)
{
  byte ht_exp_pzid = 0;             /* Stores the pzid to be removed       */
  byte pzidi_hysteresis_current_pzid = 0;  /* Current PZID of the MS       */
/*-----------------------------------------------------------------------*/
  if (index >= DS707_PZID_MAX_LEN)
  {
    MSG_ERROR("Invalid index %d for the pzid list is passed", index,0,0);
    return;
  }
  HYST_DEBUG("remove_pzid_index(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  /*--------------------------------------------------------------------- 
   Store the PZID that you are going to remove.
  -------------------------------------------------------------------------*/
  ht_exp_pzid = ds707_pzid_list[index];
  
  /*------------------------------------------------------------------------
   Since a PZID entry is going to be removed,decrement the 
   ds707_pzid_oldest_pzid.
  -------------------------------------------------------------------------*/
  --ds707_pzid_oldest_pzid;
  
  /*------------------------------------------------------------------------
    Remove the pzid from the list by moving the elements up and then assign
    the last pzid to zero.
  -------------------------------------------------------------------------*/
  while( (index <= ds707_pzid_oldest_pzid) && (index < (DS707_PZID_MAX_LEN - 1))
          && (index < (PZIDI_HYSTERESIS_DORM_LIST_LEN - 1)) )
  {
    MSG_HIGH("HT Expired on pzid_list[%d]: %d",index, ht_exp_pzid,0);
    event_report_payload(EVENT_PZID_HT_EXPIRED, sizeof(ht_exp_pzid), &ht_exp_pzid);
    ds707_pzid_list[index] = ds707_pzid_list[index+1];
    pzidi_hysteresis_expiration_list[index] = pzidi_hysteresis_expiration_list[index+1];
    index++;
  }
  
  /*---------------------------------------------------------------------------
   If the removed PZID is same as the current then add the PZID to the list at 
   the top and request for an origination.
  -----------------------------------------------------------------------------*/
  pzidi_hysteresis_current_pzid = ds707_pzid_curr_epzid.pzid;
  if( ht_exp_pzid == pzidi_hysteresis_current_pzid ) 
  {
     ds707_pzid_hysteresis_add_new_pzid(pzidi_hysteresis_current_pzid);
     

     MSG_MED(" Originating on new PZID= %d, after adding to list", 
                          pzidi_hysteresis_current_pzid, 0, 0); 
     ds707_pzid_reconnect_cb( );
  }
  else
  {
    MSG_MED(" PZID= %d already on list, no orig",
                            pzidi_hysteresis_current_pzid, 0, 0); 
  }
  ds707_pzid_hysteresis_print_pzid_list();
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
static void pzidi_hysteresis_init_state
(
   void
)
{
/*-----------------------------------------------------------------------*/
  HYST_DEBUG("init_state(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
             ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  /*---------------------------------------------------------------------
    Check to see if PZID feature is registered before we 
	go to the init state for hysteresis.
  -----------------------------------------------------------------------*/
  if(ds707_pzid_max_list_len == 0)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Clears Hysteresis activation timer or Hysteresis timers 
    depending on the state. 
  -------------------------------------------------------------------------*/
    PZIDI_STOP_TIMER();
    
  /*----------------------------------------------------------------------
      Clear the elements from the list starting at index 1 to maximum 
      elements pointed in the list.
    -----------------------------------------------------------------------*/
    ds707_pzid_clear_pzid_list(1,ds707_pzid_oldest_pzid);
    
    INTLOCK();
  /*----------------------------------------------------------------------
      In Hyseteresis state the list length should be made 1.
      
      Sometimes this can be called from different context, so add INTLOCK
      and INTFREE.
    -----------------------------------------------------------------------*/
    ds707_pzid_max_list_len       = 1;       
    ds707_pzid_oldest_pzid        = 1;
    pzidi_hysteresis_hai          = TRUE;
    pzidi_hys_state               = PZIDI_HYS_INIT_STATE;
    HYST_DEBUG("Init state :HAI = %d, pzidi_hys_state = %d, max list length = %d ",
                pzidi_hysteresis_hai, pzidi_hys_state, ds707_pzid_max_list_len); 
    INTFREE();

} /* ds707_pzid_hysteresis_init_state() */

/*===========================================================================

FUNCTION    : PZIDI_HYSTERESIS_NULL_STATE

DESCRIPTION : Enters null state when packet call is closed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
static void pzidi_hysteresis_null_state
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
     Reset the pzid hysteresis variables
  -------------------------------------------------------------------------*/
  
  HYST_DEBUG("null_state(), list = %d, old_pzid_ind = %d",
              ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, 0); 
  INTLOCK();
  pzidi_hysteresis_hai = FALSE;
  PZIDI_STOP_TIMER();
  pzidi_hys_state      = PZIDI_HYS_NULL_STATE;
  INTFREE();
  
  HYST_DEBUG("HAI = %d, pzidi_hys_state = %d ",
                pzidi_hysteresis_hai, pzidi_hys_state, 0); 

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
static void pzidi_hysteresis_process_data_ready_ev
(
  void
)
{
  HYST_DEBUG("data_ready_ev(), list = %d, hyst_enabled = %d, pzidi_hys_state = %d", 
   ds707_pzid_max_list_len, ds707_pzid_hysteresis_enabled, pzidi_hys_state); 
  /*-----------------------------------------------------------------------*/
  switch(pzidi_hys_state)
  {
     case PZIDI_HYS_NULL_STATE:
       if( ds707_pzid_hysteresis_enabled == TRUE ) 
       {
 	        pzidi_hysteresis_init_state();
       }
       break;
        
     case PZIDI_HYS_INIT_STATE:
     case PZIDI_HYS_HAT_STATE:
     case PZIDI_HYS_HT_STATE:
       if( ds707_pzid_hysteresis_enabled == TRUE ) 
       {
         pzidi_hysteresis_init_state();
       }
       else 
       {
         pzidi_hysteresis_null_state();
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

 static void pzidi_hysteresis_process_start_hat
(
  void
)
{
 /*-----------------------------------------------------------------------*/
 HYST_DEBUG("start_hat(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  switch(pzidi_hys_state)
  {
     case PZIDI_HYS_NULL_STATE:
       break;
     
     case PZIDI_HYS_INIT_STATE:
       pzidi_hysteresis_start_hat();
       pzidi_hys_state = PZIDI_HYS_HAT_STATE;
       break;
     
     case PZIDI_HYS_HAT_STATE:
     case PZIDI_HYS_HT_STATE:
       MSG_ERROR("Start HAT event -> %d state", pzidi_hys_state,0,0);
       break;       
  }
  HYST_DEBUG("HAI = %d",pzidi_hysteresis_hai, 0, 0); 
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

 static void pzidi_hysteresis_process_stop_hat
(
  void
)
{
 /*-----------------------------------------------------------------------*/
 HYST_DEBUG("stop_hat(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  switch(pzidi_hys_state)
  {
     case PZIDI_HYS_NULL_STATE:
       break;
     
     case PZIDI_HYS_INIT_STATE:
       break;
     
     case PZIDI_HYS_HAT_STATE:
       pzidi_hysteresis_stop_hat();
       pzidi_hys_state = PZIDI_HYS_INIT_STATE;
       break;

     case PZIDI_HYS_HT_STATE:
       MSG_ERROR("Stop HAT -> %d state", pzidi_hys_state,0,0);
       break;       
  }
  HYST_DEBUG("HAI = %d",pzidi_hysteresis_hai, 0, 0); 
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

static void pzidi_hysteresis_process_hysteresis_timers_expired_ev
(
  void
)
{
 /*-----------------------------------------------------------------------*/
  
  HYST_DEBUG("hyst_tmrs_exp(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  switch(pzidi_hys_state)
  {
     case PZIDI_HYS_NULL_STATE:
     case PZIDI_HYS_INIT_STATE:
       MSG_FATAL("Never happen expired event:%d",pzidi_hys_state, 0, 0);
       break;
     
     case PZIDI_HYS_HAT_STATE:
       pzidi_hysteresis_process_hat();
       pzidi_hys_state = PZIDI_HYS_HT_STATE;
       break;

     case PZIDI_HYS_HT_STATE:
       ds707_pzid_hysteresis_process_ht();
       pzidi_hys_state = PZIDI_HYS_HT_STATE;
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
static void pzidi_hysteresis_process_socm_hys_enabled_ev
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  ds707_pzid_hysteresis_enabled = TRUE; 
  HYST_DEBUG("socm_hyst_en(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  switch(pzidi_hys_state)
  {
     case PZIDI_HYS_NULL_STATE:
       pzidi_hysteresis_init_state();
       break;

     case PZIDI_HYS_INIT_STATE:
       break;

     case PZIDI_HYS_HAT_STATE:
       MSG_ERROR("Hysteresis Enable -> %d state", pzidi_hys_state,0,0);
       break;       

     case PZIDI_HYS_HT_STATE:
    /*-----------------------------------------------------------------
     If SOCM is received with list_len=1 during HT state, the list size
     shrinks to 1 and also the pzidi_hys_state should be reverted to
     INIT state after canceling the HT timers. Otherwise, the mobile 
     would be left in a weird state hysteresis state with list size as
     1. The list will also never grow to 4
    -----------------------------------------------------------------*/
       PZIDI_STOP_TIMER(); 
       pzidi_hys_state = PZIDI_HYS_INIT_STATE;
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

static void pzidi_hysteresis_process_socm_hys_disabled_ev
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  HYST_DEBUG("socm_hys_dis(), list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  
  switch(pzidi_hys_state)
  {
     case PZIDI_HYS_NULL_STATE:
       break;
     
     case PZIDI_HYS_INIT_STATE:
     case PZIDI_HYS_HAT_STATE:
     case PZIDI_HYS_HT_STATE:
       pzidi_hysteresis_null_state();
    /*--------------------------------------------
     Hysteresis feature is disabled through SOCM.
    ----------------------------------------------*/
       ds707_pzid_hysteresis_enabled = FALSE; 
       break;
  }
}/* pzidi_hysteresis_process_socm_hys_disabled_ev() */

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================

FUNCTION DS707_PZID_HYSTERESIS_PROCESS_EVENT

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
void  ds707_pzid_hysteresis_process_event
(
   ds707_pzid_hysteresis_event_type event
)
{
 /*-------------------------------------------------------------------------*/
 
 
 
 HYST_DEBUG("hyst_process_ev(),EVENT: %d list = %d, pzidi_hys_state = %d",event,
   ds707_pzid_max_list_len,  pzidi_hys_state); 
 switch(event)
 { 
   case PZID_HYS_NULL_EVENT:
     
     pzidi_hysteresis_null_state();
     break;

   case PZID_HYS_DATA_READY_EVENT:
     /*-------------------------------------------------------------------
       Data has become ready, clear all the hysteris state variables.
     -------------------------------------------------------------------- */
     pzidi_hysteresis_process_data_ready_ev();
     break;
   
   case PZID_HYS_CALL_ENDED_EVENT:
     /*-------------------------------------------------------------------
       Mobile has gone dormant. Start hysteresis activation timer.
     -----------------------------------------------------------------------*/
     pzidi_hysteresis_process_start_hat();
     break;

   case PZID_HYS_TIMER_EXPIRED_EVENT:
     /*-------------------------------------------------------------------
       Timer signal set. Process hysteresis hat/ht timer
     -----------------------------------------------------------------------*/
     pzidi_hysteresis_process_hysteresis_timers_expired_ev();
     break;
 
   case PZID_HYS_SOCM_HYS_ENABLED_EVENT: 
     /*-------------------------------------------------------------------
      BS sent SOCM to enable the hysteresis feature.
      -----------------------------------------------------------------------*/
     pzidi_hysteresis_process_socm_hys_enabled_ev();
     break;
   
   case PZID_HYS_SOCM_HYS_DISABLED_EVENT:
     /*-------------------------------------------------------------------
      BS sent SOCM to disable the hysteresis feature.
     -----------------------------------------------------------------------*/
     pzidi_hysteresis_process_socm_hys_disabled_ev();
     break;
   
   case PZID_HYS_CALL_CONNECTED_EVENT:
   case PZID_HYS_GO_NULL_EVENT:
     /*-------------------------------------------------------------------
       Mobile has come out of dormancy or Going NULL. 
       Stop hysteresis activation timer.
     -----------------------------------------------------------------------*/
     pzidi_hysteresis_process_stop_hat();
     break;

   case PZID_HYS_SDB_DATA_EVENT:
     /*-------------------------------------------------------------------
       SDB data transmitted/received - Leave hysteresis state and restart
       the Hysteresis Activation Timer (HAT)
     -------------------------------------------------------------------- */
     pzidi_hysteresis_process_data_ready_ev();
     pzidi_hysteresis_process_stop_hat();
     pzidi_hysteresis_process_start_hat();
     break;

   default:
     MSG_ERROR( "UNKNOWN EVENT: %d, Hysteresis: %d", event, ds707_pzid_hysteresis_enabled, 0);
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

void ds707_pzid_hysteresis_print_pzid_list
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
  
  while(i < ds707_pzid_oldest_pzid)
  {  
    exp_duration = pzidi_hysteresis_expiration_list[i]
                - pzidi_hysteresis_sys_uptime;
    exp_duration = ((exp_duration > 0)? (exp_duration):0);
     MSG_HIGH( "PZID[%d]:%d  HT time:%d ", i, ds707_pzid_list[i],
             exp_duration);
    
     i ++;
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
void ds707_pzid_hysteresis_add_new_pzid
(
  uint16 new_PZID 
)
{ 
  dword  uptime;   /* Variable for the system uptime.*/
  uint16 n = 0;    /* counter                        */
/*------------------------------------------------------------------------*/
  
  HYST_DEBUG("Add new PZID, list = %d, old_pzid_ind = %d,pzidi_hys_state = %d",
   ds707_pzid_max_list_len, ds707_pzid_oldest_pzid, pzidi_hys_state); 
  /*-----------------------------------------------------------------------
   ds707_pzid_oldest_pzid is incremented because we are going to add
   a new PZID.
  ------------------------------------------------------------------------*/
  n = ds707_pzid_oldest_pzid = MIN( ds707_pzid_oldest_pzid+1, 
                                    ds707_pzid_max_list_len );
  
  if(ds707_pzid_oldest_pzid == ds707_pzid_max_list_len && (ds707_pzid_list[n-1]!=0))
  {
   MSG_HIGH( "Deleting PZID[%d]: %d to add new PZID", (n-1), 
                                  ds707_pzid_list[n-1], 
                                  pzidi_hysteresis_expiration_list[n-1]);
  }
  /*-----------------------------------------------------------------------
   New PZID is always added on the top. So move all the existing 
   elements down by one index. 
   Since n is incremented already, need to decrement before we use it.
  --------------------------------------------------------------------------*/
  if( n == 0)
  {
   ASSERT(n!=0)
   MSG_ERROR("ds707_pzid_oldest_pzid should never be zero.",0,0,0);
   return;
  }
  while( --n )
  {
    ds707_pzid_list[n] = ds707_pzid_list[n-1];
    pzidi_hysteresis_expiration_list[n]= pzidi_hysteresis_expiration_list[n-1];
  }
  /*------------------------------------------------------------------------
   Place the new PZID at the top of the list (zeroth location).
  --------------------------------------------------------------------------*/
  ds707_pzid_list[0] = new_PZID;
  pzidi_hysteresis_expiration_list[0] = 0;

  MSG_HIGH("Adding new PZID[0]:%d,ds707_pzid_oldest_pzid:%d ", new_PZID,
                                                 ds707_pzid_oldest_pzid,
                                                                      0 );
  
  /*-----------------------------------------------------------------------
   Add timer on the previously first PZID.
   The uptime is the time elapsed in seconds since the phone started. 
   Uptime returns dword which is unsigned 32 bit value,which is a big
   number in seconds and it will take many many years to overflow. 
   That's why we don't need any seperate logic to cover the edge cases.
  --------------------------------------------------------------------------*/
  uptime = clk_uptime();
  pzidi_hysteresis_expiration_list[n+1]=uptime + 
                                        ds707_pzid_hysteresis_ht_timer_in_sec;
  
} /* ds707_pzid_hysteresis_add_new_pzid() */

/*===========================================================================

FUNCTION DS707_PZID_HYSTERESIS_POST_EVENT

DESCRIPTION
  Post a command to DS task corresponding to a PZID hsyterisis event, so that
  all processing of the PZID hysterisis state machine will happen in the DS
  task context.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ds707_pzid_hysteresis_post_event
(
   ds707_pzid_hysteresis_event_type event
)
{
  ds_cmd_type     *cmd_ptr;                      /* DS Task msg            */
 /*-------------------------------------------------------------------------*/

 HYST_DEBUG("hyst_post_ev(),EVENT: %d list = %d, pzidi_hys_state = %d",event,
   ds707_pzid_max_list_len,  pzidi_hys_state); 
 switch(event)
 { 
   /*------------------------------------------------------------------------
    Currently only the DATA ready and SDB data event are posted this way. 
    Ignore all other events for now.
   ------------------------------------------------------------------------*/
   case PZID_HYS_DATA_READY_EVENT:
   case PZID_HYS_SDB_DATA_EVENT:
     if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
     {
       ERR_FATAL( "PZID hyst cant get cmd buf from DS task", 0, 0, 0 );
     }
     else
     {
       if (event == PZID_HYS_DATA_READY_EVENT)
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

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DS_PZID_HYSTERESIS */

