/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               P Z I D 

GENERAL DESCRIPTION
  This file contains functions used to handle PZID changes.

EXTERNALIZED FUNCTIONS
  DS707_PZID_RECONNECT_CB
    Called by MC/CM when the system PZID changes.  Sends a message to DS
    pkt mgr telling it to come out of dormancy.
    
  DS707_PZID_DISABLE_RECON
    Disables dormant re-origination when PZID changes.
    
  DS707_PZID_REG_RECON
    Registers the PZID reconnect callback with MC/CM.
    
  DS707_PZID_DEREG_RECON
    De-registers the PZID reconnect callback with MC/CM.
  
  DS707_PZID_PARSE_SOCM
    Used for parsing the service option control message which contains the
    PZID reconnect parameters.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Need to call ds707_pzid_reg_recon() at startup, if want to be notified
    of PZID changes.

   Copyright (c) 1999 - 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_pzid.c#4 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   ms      Added Support for EPZID Hysteresis.
01/21/07   sn      Changed ASSERT in PZID change handler function to a message.
09/08/04   vr      Changed PZID_HYS_CALL_ACTIVE_EVENT to 
                   PZID_HYS_DATA_READY_EVENT.
                   Added ds707_pzid_rlp_data_transfer_cb() that notifies the
                   PZID hysterisis engine when PPP data is sent/received.
06/02/04   gr      Fixed problem with PZID changes when HAT not expired
11/17/03   igt     Remove the Start for the PERF_TOTAL_HANDOFF_DELAY
                   timer. This has been located elsewhere.
10/14/03   sy      Registering pzid hysteresis(init state) only if maximum 
                   pzid list lenght is > 1.
10/01/03   ak      When dormant timer expires, start pzid delay timer.  this
                   gives DS a chance to see if there is data to send when
                   orig'ing.
09/23/03   ak      When first registering PZID, add current PZID to list.
09/13/03   ak      Added code to check for a new PZID when holddown timer
                   expires.
09/02/03   sy      Added support for pzid hysteresis under 
                   FEATURE_DS_PZID_HYSTERESIS.
08/24/03   ak      Fixes for registering/deregistering PZID correctly on
                   call session active/inactive.
01/24/03   ak      Now register one PZID callback with MC.  Always try to
                   re-originate, if the PZID is new.
11/17/02   ak      Updated file header comments.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#ifndef FEATURE_DATA_EPZID
#include "assert.h"
#include "ds3gtimer.h"
#include "ds707.h"
#include "ds707_pzid.h"
#include "ds707_pkt_mgr.h"
#include "dsrlp.h"
#include "dstask.h"
#include "err.h"
#include "msg.h"
#include "snm.h"
#ifdef FEATURE_DS_PZID_HYSTERESIS
#include "ds707_pzid_hyst.h"
#endif /* FEATURE_DS_PZID_HYSTERESIS */
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  Mask to get the PZID_CTL bits
---------------------------------------------------------------------------*/
#define PZIDI_CNTL_MASK           0xE0

/*---------------------------------------------------------------------------
  Mask to get the LIST_LEN bits
---------------------------------------------------------------------------*/
#define PZIDI_LIST_LEN_MASK       0x0F

/*---------------------------------------------------------------------------
  Values of the PZID_CNTL field in the Packet Data Zone based Registration/
  Reconnection service option control message.
---------------------------------------------------------------------------*/
#define PZIDI_CNTL_DISABLE          0        /* Disable PZID recon         */
#define PZIDI_CNTL_ENABLE        0x20        /* Enable PZID recon          */
#define PZIDI_CNTL_CLEAR_LIST    0x40        /* Clear PZID list            */

/*---------------------------------------------------------------------------
  Variables to maintain the vistied PZID list.
  The list is organized as an array. The list is sorted by the order in 
  which PZID zones were visited, with the most recently visited PZID at the 
  head of the list.
---------------------------------------------------------------------------*/
byte ds707_pzid_oldest_pzid  = 0;           /* Index of oldest PZID +1     */
byte ds707_pzid_max_list_len = 0;           /* List len allowed by BS      */
byte ds707_pzid_list[ DS707_PZID_MAX_LEN];  /* Array to hold PZID list     */

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================
FUNCTION       PZIDI_PRINT_PZID_LIST

DESCRIPTION    This function prints the current visited PZID list as a
               series of MSG_HIGHs. This is for debug purposes.
  
               This function is called from the MC task context in
               callbacks.
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
LOCAL void pzidi_print_pzid_list
(
  void
)
{
  byte i=0;                                 /* index to traverse PZID list */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Go through the PZID list and print the contents of the list in a MSG.
    All elements are printed - three per MSG_HIGH. At the end of the while 
    loop either 2 or 1 element is left out. These elements are printed in 
    separate MSGs later.
  -------------------------------------------------------------------------*/    
 #ifdef FEATURE_DS_PZID_HYSTERESIS
  if(ds707_pzid_hysteresis_enabled == TRUE)
  {
    ds707_pzid_hysteresis_print_pzid_list();
    return;
  }
#endif /* FEATURE_DS_PZID_HYSTERESIS */
  while ( (i+2) < ds707_pzid_oldest_pzid)
  {
    MSG_HIGH( " Current PZID list = %d, %d, %d", ds707_pzid_list[ i], 
                                                 ds707_pzid_list[ i+1], 
                                                 ds707_pzid_list[ i+2]);
    i += 3;
  }

  /*-------------------------------------------------------------------------
    If 2 elements of the list were not printed above, print them in a 
    separate MSG.
  -------------------------------------------------------------------------*/    
  if ( (i+1) < ds707_pzid_oldest_pzid)
  {
    MSG_HIGH( " Current PZID list = %d, %d", ds707_pzid_list[ i],
                                             ds707_pzid_list[ i+1], 
                                             0);
    i += 2;
  }

  /*-------------------------------------------------------------------------
    If the last element of the list was not printed, then print it in a 
    MSG.
  -------------------------------------------------------------------------*/    
  else if ( i < ds707_pzid_oldest_pzid)
  {
    MSG_HIGH( " Current PZID list = %d", ds707_pzid_list[ i], 0, 0);
    i += 1;
  }

  ASSERT( i == ds707_pzid_oldest_pzid);
} /* pzidi_print_list() */

/*===========================================================================
FUNCTION      PZIDI_ADD_PZID_LIST

DESCRIPTION   This function adds the passed PZID to the visited PZID list.
              The list is maintained as an array.  The first element of the
              array is the most recently visited PZID and ds707_pzid_oldest_pzid
              is the index after the oldest visited PZID and is the last
              element used in the array.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  IN DS TASK

RETURN VALUE  Returns TRUE if PZID was already on the list.
              Returns FALSE if PZID was not on the list.

SIDE EFFECTS  None
===========================================================================*/
LOCAL boolean pzidi_add_pzid_list
(
  byte pzid                                    /* PZID to add to list      */
)
{
  boolean ret_val = FALSE;                     /* Return Value             */
  byte    i;                                   /* Index into list          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the first element on the pzid list (most recently added), is the
    passed PZID, return from function. This happens when we add an old pzid
    passed by SNM.
  -------------------------------------------------------------------------*/
  if ( ds707_pzid_list[0] == pzid)
  {
    MSG_HIGH("PZID exists at PZID[0]: %d : %d", ds707_pzid_list[0],0,0);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Go through the PZID list to see if new pzid is already on the list.
    If PZID was on the list, i would be its index in the PZID array.
  -------------------------------------------------------------------------*/
   ASSERT( ds707_pzid_oldest_pzid <= ds707_pzid_max_list_len);
   for (i = 0; i < ds707_pzid_oldest_pzid; i++)
   {
    if ( ds707_pzid_list[i] == pzid)
    {
      ret_val = TRUE;
      break;
    }
  }

#ifdef FEATURE_DS_PZID_HYSTERESIS
   if((ds707_pzid_hysteresis_enabled == TRUE)&& (ret_val == FALSE))
   {
   /*--------------------------------------------------------------------
       Add new entry to the PZID list while the hysteresis is enabled.
     ----------------------------------------------------------------------*/  
     
     ds707_pzid_hysteresis_add_new_pzid(pzid);
    /*-------------------------------------------------------------------------
     We want to process the Hysteresis timers as
     soon as we add an entry in the table, 
     only if the list has more than one entry
    -------------------------------------------------------------------------*/
     if ( ds707_pzid_max_list_len > 1)
     {
       ds707_pzid_hysteresis_process_ht();
     }
   }
   else if(ds707_pzid_hysteresis_enabled == FALSE)
#endif /* FEATURE_DS_PZID_HYSTERESIS */
   {
     /*-------------------------------------------------------------------------
      If the new pzid was not on the list, set i to the oldest pzid. i will 
      be used to shift each entry to the left.
     -------------------------------------------------------------------------*/
     if ( ret_val == FALSE)
     { 
       if ( ds707_pzid_oldest_pzid == (ds707_pzid_max_list_len))
       {
         i = ds707_pzid_oldest_pzid-1;
       }
       else
       {
         i = ds707_pzid_oldest_pzid++;
       }
     }

     /*-------------------------------------------------------------------------
      Shift each entry in the PZID array to the left so that there is room to
      put the most recently visited PZID.
     -------------------------------------------------------------------------*/
     while ( i > 0)
     {
        ds707_pzid_list[ i] = ds707_pzid_list[ i-1];
        i--;
     }
    
   /*-------------------------------------------------------------------------
    Set the first entry in the list to the new pzid, and print current list.
   -------------------------------------------------------------------------*/
     ds707_pzid_list[ 0] = pzid;
     
   }
   pzidi_print_pzid_list();

   return( ret_val);
} /* pzidi_add_pzid_list() */


/*===========================================================================
FUNCTION      PZIDI_CHANGE_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
       
              This function follows procedures from IS-707.A.5 sec.2.2.7. 
              If the new PZID was 0, it resets PZID based reconnection.
              It adds the new PZID to the visited PZID list.
              If the new PZID is not on the list of the PZIDs, this function
              sends a command to DS task to originate a packet call.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_change_cb
(
  byte old_pzid,                                   /* Previous Pkt Zone ID */
  byte new_pzid                                    /* New Pkt Zone ID      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( old_pzid == new_pzid )
  {
    MSG_MED("Old and new PZID values are same",0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    If PZID list has not been initialized, log error and return
  -------------------------------------------------------------------------*/
  if ( ds707_pzid_max_list_len == 0)
  {
    ERR(" PZID change handler called, but PZID recon not active", 0, 0, 0);
    return;
  }

  if (ds707_pkt_is_pzid_orig_allowed(DS707_DEF_PKT_INSTANCE) == FALSE)
  {
    MSG_HIGH("Originations not allowed, do not process PZID info",0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    If PZID sent in signaling message is 0, disable PZID based reconnection.
  -------------------------------------------------------------------------*/
  if ( new_pzid == 0) 
  {
    MSG_MED(" Got new PZID=0, disabling PZID reconnection", 0, 0, 0);
    ds707_pzid_disable_recon();  
  }
  /*-------------------------------------------------------------------------
    Add old PZID to visited PZID list. Old PZID is added to keep the first
    PZID (where call was originated initially) in the list.
    Add new PZID to the visited PZID list.
    If the new PZID wasn't  on the list before this, send a cmd to DS to
    originate pkt call.
  -------------------------------------------------------------------------*/
  else
  {
    pzidi_add_pzid_list( old_pzid);
    if ( pzidi_add_pzid_list( new_pzid) == FALSE)
    {

      MSG_MED(" Originating on new PZID= %d, after adding to list", 
                          new_pzid, 0, 0); 
      ds707_pzid_reconnect_cb( );
    }
    else
    {
      MSG_MED(" PZID= %d already on list, no orig", new_pzid, 0, 0); 
    }
  }  /* PZID reconnect is  active */

} /* pzidi_change_cb() */


/*===========================================================================
FUNCTION       DS707_PZID_CLEAR_PZID_LIST

DESCRIPTION    This function clears the PZID list from the index specified
               by the first parameter to the index specified by the second
               parameter.
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pzid_clear_pzid_list
(
 byte first_pzid,                /* First list element  to be cleared      */
 byte last_pzid                  /* Last list element  to be cleared       */
)                                
{
  byte i;                      /* Index                                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = first_pzid; i < last_pzid;i++) 
  {
    ds707_pzid_list[i] = 0;
  }
} /* ds707_pzid_clear_pzid_list */


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PZID_RECONNECT_CB

DESCRIPTION   Sends a message to DS asking it to originate a call.  Occurs
              when PZID change requires reconnection.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_reconnect_cb
(
void
)
{
#ifdef FEATURE_DATA_IS707_PZID_RECON
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MSG_HIGH("PZID Change - reconnect",0,0,0);
  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "CM can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_PZID_CHANGE_CMD;
    ds_put_cmd( cmd_ptr );
  }
#else
  MSG_HIGH("PZID RECON FEATURE OFF", 0, 0, 0);
#endif /* FEATURE_DATA_IS707_PZID_RECON */
}  /* ds707_pzid_reconnect_cb() */

/*===========================================================================
FUNCTION      DS707_PZID_DISABLE_RECON

DESCRIPTION   Disables packet zone id based reconnection for packet calls.
              This function deregisters the pzidi_change_cb(). The list of
              visited packet zones is also cleared.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
             (a) BS sends a SOCTL message disable PZID reconnection
             (b) BS sends a PZID of 0.
             (c) MS comes out of the active state - PPP session has ended.
             (d) MS detects a change in SID.
  
             This function can be called from either the DS or the MC task
             (from the callbacks).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_disable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Reset the list of visited packet zones.  Make sure that pzid cback is
     enabled first, by checking to see that max_pzid_list_len > 0.
  -------------------------------------------------------------------------*/
  INTLOCK();

  MSG_LOW("Disabling PZID based reconnection", 0, 0, 0);

  if (ds707_pzid_max_list_len > 0)
  {
    ds707_pzid_oldest_pzid = 0;
    ds707_pzid_max_list_len = 1;
  }
  else
  {
    MSG_HIGH("Trying to disable de-registerd PZID recon",0,0,0);
  }

  INTFREE();

  return;
} /*  ds707_pzid_disable_recon() */


/*===========================================================================
FUNCTION      DS707_PZID_REG_RECON

DESCRIPTION   This function registers PZID based reconnection for packet
              calls.  It registers the pzid change callbacks and
              initializes the PZID list to one entry.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is initiated (packet
              service goes into active state).
  
              Note, the idle change handler will reoriginate a packet call on
              detecting a PZID change.
          
              This function can be called from the DS task.  Note that this 
              can be called multiple times in a data call, as the IFACE goes
              between ROUTEABLE and UP (could be Rm MIP call).  Protect
              against losing PZID list by making callback hasn't been
              registered by checking max_pzid_list_len.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_reg_recon(void)
{
  byte curr_pzid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  INTLOCK();

  if (ds707_pzid_max_list_len == 0)
  {
    MSG_MED("Starting PZID based reconnection", 0, 0, 0);
    
  ds707_pzid_oldest_pzid       = 0;
  ds707_pzid_max_list_len      = 1;

#ifdef FEATURE_DS_PZID_HYSTERESIS
  /*-----------------------------------------------------------------------
   Initialize the PZID hysteresis state by default
  -------------------------------------------------------------------------*/
    ds707_pzid_hysteresis_enabled = TRUE ;
#endif /* FEATURE_DS_PZID_HYSTERESIS */
    
  }
  else
  {
    MSG_HIGH("Pzid callback already registered",0,0,0);
  }

  /*-----------------------------------------------------------------------
    It's possible the registration happened after the PZID change callback
    would have occured (meaning we don't get a callback for the current
    PZID).  So make sure current PZID is in the list
  -----------------------------------------------------------------------*/
  curr_pzid = ds707_pzid_curr_epzid.pzid;
  if (curr_pzid != 0)
  {
    MSG_HIGH("Just reg'd for PZID - add curr pzid to list",0,0,0);
    pzidi_add_pzid_list(curr_pzid);
  }

  INTFREE();
}  /* ds707_pzid_reg_recon() */


/*===========================================================================
FUNCTION      DS707_PZID_DEREG_RECON

DESCRIPTION   This function deregisters PZID based reconnection for packet
              calls.  It sets the PZID change handlers to NULL and resets
              the visited PZID list.
   
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is ended (packet
              service goes into Inactive state) - not called when packet data
              service goes dormant.
   
              This function can be called from the DS task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_dereg_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED(" Stopping  PZID based reconnection", 0, 0, 0);

  INTLOCK();
  /*-------------------------------------------------------------------------
     Reset the listed of visited packet zones.
  -------------------------------------------------------------------------*/
  ds707_pzid_oldest_pzid = 0;
  ds707_pzid_max_list_len = 0;
#ifdef FEATURE_DS_PZID_HYSTERESIS
  ds707_pzid_hysteresis_enabled = FALSE; 
#endif /* FEATURE_DS_PZID_HYSTERESIS */
  INTFREE();
}  /* ds707_pzid_dereg_recon() */

/*===========================================================================
FUNCTION      DS707_PZID_CHECK_FOR_NEW_PZID

DESCRIPTION   This function starts a 1-second timer, and is called after the
              holddown timer expires.  The 1-second delay is there to allow
              DS to see if there is any data which would cause an orig.  This
              way, MS has a chance to originate with correct DRS.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_check_for_new_pzid(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Start PZID Delay timer for 1 second",0,0,0);
  ds3gi_start_timer(DS3G_TIMER_PZID_DELAY, 1000);

} /* ds707_pzid_check_for_new_pzid() */

/*===========================================================================
FUNCTION      DS707_PZID_COMPARE_PZIDS

DESCRIPTION   Given two PZIDs, this calls the change_cb() if PZID is
              registered with MC (i.e, still enabled).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_compare_pzids
(
  byte    old_pzid,
  byte    new_pzid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  INTLOCK();
  if (ds707_pzid_max_list_len > 0)
  {
    if (old_pzid != new_pzid)
    {
      MSG_HIGH("Poke PZID on potential new PZID",0,0,0);
      ds707_pzid_change_cb(old_pzid,new_pzid);
    }
  }
  else
  {
    MSG_HIGH("PZID no longer registered",0,0,0);
  }
  INTFREE();

} /* ds707_pzid_compare_pzids() */


/*===========================================================================
FUNCTION      DS707_PZID_PROCESS_DELAY_TIMER_EXPIRED

DESCRIPTION   This function process the expiration of the PZID delay timer.
              At this point, it is safe to orig on a PZID change which 
              occured during the holddown timer period.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_process_delay_timer_expired(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("PZID delay timer expired... see if new PZID",0,0,0);

  ds707_pzid_compare_pzids(ds707_pzid_list[0],ds707_pzid_curr_epzid.pzid);

} /* ds707_pzid_process_delay_timer_expired() */

/*===========================================================================
FUNCTION      DS707_PZID_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a PZID
              message.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_pzid_parse_socm
(
  byte                       cntl,            /* SOCM CNTL field           */
  ds707_pzid_socm_msg_type*  pzid_socm_msg    /* SOCM with PZID info       */
)
{
  snm_socm_return_type  result = SNM_VALID;  /* return value               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If cntl field tells mobile to disable PZID based reconnection, reset
    the list.
  -------------------------------------------------------------------------*/
  if ((cntl & PZIDI_CNTL_MASK) == PZIDI_CNTL_DISABLE)
  {
    MSG_MED("Rx'ed SO Ctl Msg to disable zone reg.", 0, 0, 0);
    ds707_pzid_disable_recon();
  }

  /*-------------------------------------------------------------------------
    If cntl field tells mobile to enable PZID recon, initialize the list and
    set the maximum elements of the list.
  -------------------------------------------------------------------------*/
  else if ((cntl & PZIDI_CNTL_MASK) == PZIDI_CNTL_ENABLE)
  {                
    ds707_pzid_max_list_len = (pzid_socm_msg->list_len &
                                                        PZIDI_LIST_LEN_MASK);
    MSG_MED("Rx'ed SO Ctl Msg to enable PZID, len=%d", 
             ds707_pzid_max_list_len , 
             0, 
             0
           );
 
 #ifdef FEATURE_DS_PZID_HYSTERESIS
    /*--------------------------------------------------------------------
	   Enable pzid hysteresis when max list length is 1 
	---------------------------------------------------------------------*/
    ds707_pzid_hysteresis_process_event( ds707_pzid_max_list_len == 1 ? 
	  PZID_HYS_SOCM_HYS_ENABLED_EVENT : PZID_HYS_SOCM_HYS_DISABLED_EVENT );
 
 #endif /* FEATURE_DS_PZID_HYSTERESIS */
   
   /*------------------------------------------------------------------------
     If the size of the PZID list has shrunk,remove the oldest entries.
   ------------------------------------------------------------------------*/
    if ( ds707_pzid_oldest_pzid > ds707_pzid_max_list_len)
    {
      /*---------------------------------------------------------------------
        Clear the oldest entries 
      ---------------------------------------------------------------------*/
      ds707_pzid_clear_pzid_list(ds707_pzid_max_list_len, ds707_pzid_oldest_pzid);
      ds707_pzid_oldest_pzid = ds707_pzid_max_list_len;
      pzidi_print_pzid_list();
    }
  }

  /*-------------------------------------------------------------------------
   If cntl field tells mobile to clear the PZID list, clear the list and
   reset the oldest index to 0.
  -------------------------------------------------------------------------*/
  else if ((cntl & PZIDI_CNTL_MASK) == PZIDI_CNTL_CLEAR_LIST)
  {
    ds707_pzid_max_list_len = (pzid_socm_msg->list_len &
                                                        PZIDI_LIST_LEN_MASK);
    MSG_MED("Rx'ed SO Ctl Msg to clear PZID list, max_len = %d", 
             ds707_pzid_max_list_len, 
             0, 
             0
           );
    ds707_pzid_clear_pzid_list(0,ds707_pzid_oldest_pzid);
    ds707_pzid_oldest_pzid = 0;
  }

  /*-------------------------------------------------------------------------
    Unknown PZID_CNTL value. Return with SNM_INVALID_MSG_TYP. This will
    result in MS sending a Mobile Station Reject Order as per spec.
  -------------------------------------------------------------------------*/
  else
  {
    MSG_HIGH("Unknown PZID_CNTL value: 0x%x",
              (cntl & PZIDI_CNTL_MASK) >> 5, 
              0, 
              0
             );
    result = SNM_INVALID_MSG_TYP;
  }

  return(result);
} /* ds707_pzid_parse_socm() */


/*===========================================================================
FUNCTION      DS707_PZID_RLP_DATA_RCVD_CB

DESCRIPTION   This callback function should be called when a (new) RLP packet
              is sent/received after dormancy. It deregisters itself with
              RLP and notifies the PZID hysterisis engine of the data
              transfer.

DEPENDENCIES  This callback should be registered with RLP when the data 
              session goes dormant and should be called by RLP when the first 
              PPP packet is sent received after dormancy.

              This function is only applicable for 1X RLP3.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_rlp_data_transfer_cb( byte rlp_instance )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("DS707 PZID RLP DATA TRANSFER CB - RLP isntance %d",
           rlp_instance,0,0);

  /*-------------------------------------------------------------------------
    Sanity check - Since this should only be called for 1X calls, this must
    always be called with the fixed SR_ID index. If this assumption changes
    in the future, remove this ASSERT.
  -------------------------------------------------------------------------*/
  ASSERT(rlp_instance == DSRLP_FIXED_SR_ID_INDEX);

  /*----------------------------------------------------------------
    Add code here if we need to change the processing based on which 
    RLP the packet was received on.

    For now, rlp_instance is always passed as FIXED_SR_ID_INDEX
  ----------------------------------------------------------------*/

  /*----------------------------------------------------------------
    PPP received data from/to RLP after coming out of dormancy.
    Notify the PZID hysterisis engine of data transfer and 
    deregister this function with RLP3. The function will be
    registered again when the call goes dormant.
  ----------------------------------------------------------------*/
  dsrlp_reg_ppp_data_transfer_notify_func(rlp_instance, NULL);

#ifdef FEATURE_DS_PZID_HYSTERESIS
  ds707_pzid_hysteresis_post_event(PZID_HYS_DATA_READY_EVENT);
#endif /* FEATURE_DS_PZID_HYSTERESIS */
} /* ds707_pzid_rlp_data_transfer_cb() */


#endif /* FEATURE_DATA_EPZID */
#endif /* FEATURE_DATA_IS707 */
