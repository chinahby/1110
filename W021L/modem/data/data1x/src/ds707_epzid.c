/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               E P Z I D 

GENERAL DESCRIPTION
  This file contains functions used to handle EPZID changes.

EXTERNALIZED FUNCTIONS
  DS707_EPZID_RECONNECT_CB
    Called by MC/CM when the system EPZID changes.  Sends a message to DS
    pkt mgr telling it to come out of dormancy.
    
  DS707_EPZID_DISABLE_RECON
    Disables dormant re-origination when EPZID changes.
    
  DS707_EPZID_REG_RECON
    Registers the EPZID reconnect callback with MC/CM.
    
  DS707_EPZID_DEREG_RECON
    De-registers the EPZID reconnect callback with MC/CM.
  
  DS707_EPZID_PARSE_SOCM
    Used for parsing the service option control message which contains the
    EPZID reconnect parameters.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Need to call ds707_epzid_reg_recon() at startup, if want to be notified
    of EPZID changes.

   Copyright (c) 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_epzid.c#2 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   ms      Created the Initial Revision

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DATA_EPZID
#include "assert.h"
#include "ds3gtimer.h"
#include "ds707.h"
#include "ds707_epzid.h"
#include "ds707_pkt_mgr.h"
#include "dsrlp.h"
#include "dstask.h"
#include "err.h"
#include "msg.h"
#include "snm.h"
#include "ds707_epzid_util.h"
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
#include "ds707_sid.h"
#include "ds707_nid.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  Mask to get the EPZID_CTL bits
---------------------------------------------------------------------------*/
#define PZIDI_CNTL_MASK           0xE0

/*---------------------------------------------------------------------------
  Mask to get the LIST_LEN bits
---------------------------------------------------------------------------*/
#define PZIDI_LIST_LEN_MASK       0x0F

/*---------------------------------------------------------------------------
  Values of the EPZID_CNTL field in the Packet Data Zone based Registration/
  Reconnection service option control message.
---------------------------------------------------------------------------*/
#define PZIDI_CNTL_DISABLE          0        /* Disable EPZID recon         */
#define PZIDI_CNTL_ENABLE        0x20        /* Enable  EPZID recon          */
#define PZIDI_CNTL_CLEAR_LIST    0x40        /* Clear   EPZID list            */

/*---------------------------------------------------------------------------
  Variables to maintain the vistied EPZID list.
  The list is organized as an array. The list is sorted by the order in 
  which EPZID zones were visited, with the most recently visited EPZID at the 
  head of the list.
---------------------------------------------------------------------------*/
byte ds707_epzid_oldest_epzid  = 0;           /* Index of oldest EPZID +1     */
byte ds707_epzid_max_list_len = 0;           /* List len allowed by BS      */
ds707_epzid_type ds707_epzid_list[ DS707_EPZID_MAX_LEN];  /* Array to hold EPZID list     */

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================
FUNCTION       EPZIDI_PRINT_EPZID_LIST

DESCRIPTION    This function prints the current visited EPZID list as a
               series of MSG_HIGHs. This is for debug purposes.
  
               This function is called from the MC task context in
               callbacks.
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
LOCAL void epzidi_print_epzid_list
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Go through the EPZID list and print the contents of the list in a MSG.
    All elements are printed - three per MSG_HIGH. At the end of the while 
    loop either 2 or 1 element is left out. These elements are printed in 
    separate MSGs later.
  -------------------------------------------------------------------------*/    
 #ifdef FEATURE_DS_EPZID_HYSTERESIS
  if(ds707_epzid_hysteresis_enabled == TRUE)
  {
    ds707_epzid_hysteresis_print_epzid_list();
    return;
  }
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
  MSG_MED(" PZID :%d , SID :%d NID:%d",ds707_epzid_list[0].pzid,
                                       ds707_epzid_list[0].sid,
                                       ds707_epzid_list[0].nid);
                                      
} /* epzidi_print_list() */

/*===========================================================================
FUNCTION      EPZIDI_ADD_EPZID_LIST

DESCRIPTION   This function adds the passed EPZID to the visited EPZID list.
              The list is maintained as an array.  The first element of the
              array is the most recently visited EPZID and ds707_epzid_oldest_epzid
              is the index after the oldest visited EPZID and is the last
              element used in the array.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  IN DS TASK

RETURN VALUE  Returns TRUE if EPZID was already on the list.
              Returns FALSE if EPZID was not on the list.

SIDE EFFECTS  None
===========================================================================*/
LOCAL boolean epzidi_add_epzid_list
(
  ds707_epzid_type *epzid                     /* EPZID to add to list     */
)
{
  boolean ret_val = FALSE;                     /* Return Value             */
  byte    i;                                   /* Index into list          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the first element on the epzid list (most recently added), is the
    passed EPZID, return from function. This happens when we add an old epzid
    passed by SNM.
  -------------------------------------------------------------------------*/
  if ( ds707_epzid_compare_epzids(&ds707_epzid_list[0],epzid) == EPZID_NO_CHANGE)
  {
    MSG_HIGH("EPZID exists at EPZID[0]",0,0,0);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Go through the EPZID list to see if new epzid is already on the list.
    If EPZID was on the list, i would be its index in the EPZID array.
  -------------------------------------------------------------------------*/
   ASSERT( ds707_epzid_oldest_epzid <= ds707_epzid_max_list_len);
   for (i = 0; i < ds707_epzid_oldest_epzid; i++)
   {
    if ( ds707_epzid_compare_epzids(&ds707_epzid_list[i],epzid) == EPZID_NO_CHANGE)
    {
      MSG_HIGH("New EPZID is already existing at %d",i,0,0);
      ret_val = TRUE;
      break;
    }
  }

#ifdef FEATURE_DS_EPZID_HYSTERESIS
   if((ds707_epzid_hysteresis_enabled == TRUE)&& (ret_val == FALSE))
   {
   /*--------------------------------------------------------------------
       Add new entry to the EPZID list while the hysteresis is enabled.
     ----------------------------------------------------------------------*/  
     
     ds707_epzid_hysteresis_add_new_epzid(epzid);
    /*-------------------------------------------------------------------------
     We want to process the Hysteresis timers as
     soon as we add an entry in the table, 
     only if the list has more than one entry
    -------------------------------------------------------------------------*/
     if ( ds707_epzid_max_list_len > 1)
     {
       ds707_epzid_hysteresis_process_ht();
     }
   }
   else if(ds707_epzid_hysteresis_enabled == FALSE)
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
   {
     /*-------------------------------------------------------------------------
      If the new pzid was not on the list, set i to the oldest pzid. i will 
      be used to shift each entry to the left.
     -------------------------------------------------------------------------*/
     if ( ret_val == FALSE)
     { 
       if ( ds707_epzid_oldest_epzid == (ds707_epzid_max_list_len))
       {
         i = ds707_epzid_oldest_epzid-1;
       }
       else
       {
         i = ds707_epzid_oldest_epzid++;
       }
     }

     /*-------------------------------------------------------------------------
      Shift each entry in the EPZID array to the left so that there is room to
      put the most recently visited EPZID.
     -------------------------------------------------------------------------*/
     while ( i > 0)
     {
        ds707_epzid_list[ i] = ds707_epzid_list[ i-1];
        i--;
     }
    
   /*-------------------------------------------------------------------------
    Set the first entry in the list to the new pzid, and print current list.
   -------------------------------------------------------------------------*/
     ds707_epzid_list[0].pzid  =  epzid->pzid;
     ds707_epzid_list[0].sid   =  epzid->sid;
     ds707_epzid_list[0].nid   =  epzid->nid;
   }
   epzidi_print_epzid_list();

   return( ret_val);
} /* pzidi_add_pzid_list() */


/*===========================================================================
FUNCTION      EPZIDI_CHANGED_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
       
              This function follows procedures from IS-707.A.5 sec.2.2.7. 
              If the new EPZID was 0, it resets EPZID based reconnection.
              It adds the new EPZID to the visited EPZID list.
              If the new EPZID is not on the list of the EPZIDs, this function
              sends a command to DS task to originate a packet call.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_changed_cb
(
  ds707_epzid_type *old_epzid,                                   /* Previous Pkt Zone ID */
  ds707_epzid_type *new_epzid                                    /* New Pkt Zone ID      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ds707_epzid_compare_epzids(old_epzid, new_epzid ) == EPZID_NO_CHANGE)
  {
    MSG_MED("Old and new EPZID values are same",0,0,0);
    return;
  }

  /* comment */
  if(ds707_active_epzid_type == NV_EPZID_PZID)
  {
    ds707_sid_change_handler (old_epzid->sid, new_epzid->sid );
    ds707_nid_change_handler (old_epzid->nid, new_epzid->nid);
  }
  /*-------------------------------------------------------------------------
    If EPZID list has not been initialized, log error and return
  -------------------------------------------------------------------------*/
  if ( ds707_epzid_max_list_len == 0)
  {
    ERR(" EPZID change handler called, but EPZID recon not active", 0, 0, 0);
    return;
  }

  if (ds707_pkt_is_pzid_orig_allowed(DS707_DEF_PKT_INSTANCE) == FALSE)
  {
    MSG_HIGH("Originations not allowed, do not process EPZID info",0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    If EPZID sent in signaling message is 0, disable EPZID based reconnection.
  -------------------------------------------------------------------------*/
  if ( new_epzid->pzid == 0 )  
  {
    MSG_MED(" Got new EPZID=0, disabling EPZID reconnection", 0, 0, 0);
    ds707_epzid_disable_recon();  
  }
  /*-------------------------------------------------------------------------
    Add old EPZID to visited EPZID list. Old EPZID is added to keep the first
    EPZID (where call was originated initially) in the list.
    Add new EPZID to the visited EPZID list.
    If the new EPZID wasn't  on the list before this, send a cmd to DS to
    originate pkt call.
  -------------------------------------------------------------------------*/
  else
  {
    epzidi_add_epzid_list(old_epzid);
    if ( epzidi_add_epzid_list( new_epzid) == FALSE)
    {

      MSG_MED(" Originating on new EPZID= %d, SID %d  NID %d after adding to"
              " list", new_epzid->pzid, new_epzid->sid, new_epzid->nid); 
      ds707_epzid_reconnect_cb( );
    }
    else
    {
      MSG_MED(" EPZID= %d %d %d  already on list, no orig", new_epzid->pzid, new_epzid->sid, new_epzid->nid); 
    }
  }  /* EPZID reconnect is  active */

} /* pzidi_change_cb() */


/*===========================================================================
FUNCTION       DS707_EPZID_CLEAR_EPZID_LIST

DESCRIPTION    This function clears the EPZID list from the index specified
               by the first parameter to the index specified by the second
               parameter.
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_clear_epzid_list
(
 byte first_epzid,                /* First list element  to be cleared      */
 byte last_epzid                  /* Last list element  to be cleared       */
)                                
{
  byte i;                      /* Index                                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = first_epzid; i < last_epzid;i++) 
  {
    ds707_epzid_list[i].pzid = 0;
    ds707_epzid_list[i].sid = 0;
    ds707_epzid_list[i].nid = 0;
  }
} /* ds707_epzid_clear_epzid_list */


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_EPZID_RECONNECT_CB

DESCRIPTION   Sends a message to DS asking it to originate a call.  Occurs
              when EPZID change requires reconnection.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_reconnect_cb
(
void
)
{
#ifdef FEATURE_DATA_IS707_PZID_RECON
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MSG_HIGH("EPZID Change - reconnect",0,0,0);
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
  MSG_HIGH("EPZID RECON FEATURE OFF", 0, 0, 0);
#endif /* FEATURE_DATA_IS707_PZID_RECON */
}  /* ds707_pzid_reconnect_cb() */

/*===========================================================================
FUNCTION      DS707_EPZID_DISABLE_RECON

DESCRIPTION   Disables packet zone id based reconnection for packet calls.
              This function deregisters the pzidi_change_cb(). The list of
              visited packet zones is also cleared.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
             (a) BS sends a SOCTL message disable EPZID reconnection
             (b) BS sends a EPZID of 0.
             (c) MS comes out of the active state - PPP session has ended.
             (d) MS detects a change in SID.
  
             This function can be called from either the DS or the MC task
             (from the callbacks).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_disable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Reset the list of visited packet zones.  Make sure that epzid cback is
     enabled first, by checking to see that max_epzid_list_len > 0.
  -------------------------------------------------------------------------*/
  INTLOCK();

  MSG_LOW("Disabling EPZID based reconnection", 0, 0, 0);

  if (ds707_epzid_max_list_len > 0)
  {
    ds707_epzid_oldest_epzid = 0;
    ds707_epzid_max_list_len = 1;
  }
  else
  {
    MSG_HIGH("Trying to disable de-registerd EPZID recon",0,0,0);
  }

  INTFREE();

  return;
} /*  ds707_pzid_disable_recon() */


/*===========================================================================
FUNCTION      DS707_EPZID_REG_RECON

DESCRIPTION   This function registers EPZID based reconnection for packet
              calls.  It registers the pzid change callbacks and
              initializes the EPZID list to one entry.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is initiated (packet
              service goes into active state).
  
              Note, the idle change handler will reoriginate a packet call on
              detecting a EPZID change.
          
              This function can be called from the DS task.  Note that this 
              can be called multiple times in a data call, as the IFACE goes
              between ROUTEABLE and UP (could be Rm MIP call).  Protect
              against losing EPZID list by making callback hasn't been
              registered by checking max_pzid_list_len.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_reg_recon(void)
{
  ds707_epzid_type curr_epzid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  INTLOCK();

  if (ds707_epzid_max_list_len == 0)
  {
    MSG_MED("Starting EPZID based reconnection", 0, 0, 0);
    
    ds707_epzid_oldest_epzid       = 0;
    ds707_epzid_max_list_len      = 1;

#ifdef FEATURE_DS_EPZID_HYSTERESIS
  /*-----------------------------------------------------------------------
     Initialize the EPZID hysteresis state by default
  -------------------------------------------------------------------------*/
    ds707_epzid_hysteresis_enabled = TRUE ;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
    
  }
  else
  {
    MSG_HIGH("EPzid callback already registered",0,0,0);
  }

  /*-----------------------------------------------------------------------
    It's possible the registration happened after the EPZID change callback
    would have occured (meaning we don't get a callback for the current
    EPZID).  So make sure current EPZID is in the list
  -----------------------------------------------------------------------*/
  curr_epzid = ds707_pzid_curr_epzid;
  if (curr_epzid.pzid != 0)
  {
    MSG_HIGH("Just reg'd for EPZID - add curr pzid to list",0,0,0);
    epzidi_add_epzid_list(&curr_epzid);
  }

  INTFREE();
}  /* ds707_pzid_reg_recon() */


/*===========================================================================
FUNCTION      DS707_PZID_DEREG_RECON

DESCRIPTION   This function deregisters EPZID based reconnection for packet
              calls.  It sets the EPZID change handlers to NULL and resets
              the visited EPZID list.
   
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

  MSG_MED(" Stopping  EPZID based reconnection", 0, 0, 0);

  INTLOCK();
  /*-------------------------------------------------------------------------
     Reset the listed of visited packet zones.
  -------------------------------------------------------------------------*/
  ds707_epzid_oldest_epzid = 0;
  ds707_epzid_max_list_len = 0;
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  ds707_epzid_hysteresis_enabled = FALSE; 
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
  INTFREE();
}  /* ds707_pzid_dereg_recon() */

/*===========================================================================
FUNCTION      DS707_EPZID_CHECK_FOR_NEW_EPZID

DESCRIPTION   This function starts a 1-second timer, and is called after the
              holddown timer expires.  The 1-second delay is there to allow
              DS to see if there is any data which would cause an orig.  This
              way, MS has a chance to originate with correct DRS.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_check_for_new_epzid(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Start EPZID Delay timer for 1 second",0,0,0);
  ds3gi_start_timer(DS3G_TIMER_PZID_DELAY, 1000);

} /* ds707_pzid_check_for_new_pzid() */

/*===========================================================================
FUNCTION      DS707_EPZID_COMPARE_EPZIDS

DESCRIPTION   Given two EPZIDs, this calls the change_cb() if EPZID is
              registered with MC (i.e, still enabled).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_compare_pzids
(
  ds707_epzid_type    *old_epzid,
  ds707_epzid_type    *new_epzid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  INTLOCK();
  if (ds707_epzid_max_list_len > 0)
  {
    if (old_epzid->pzid != new_epzid->pzid || 
        old_epzid->sid != new_epzid->sid ||
        old_epzid->nid != new_epzid->nid)
    {
      MSG_HIGH("Poke EPZID on potential new EPZID",0,0,0);
      ds707_epzid_changed_cb(old_epzid,new_epzid);
    }
  }
  else
  {
    MSG_HIGH("EPZID no longer registered",0,0,0);
  }
  INTFREE();

} /* ds707_pzid_compare_pzids() */


/*===========================================================================
FUNCTION      DS707_EPZID_PROCESS_DELAY_TIMER_EXPIRED

DESCRIPTION   This function process the expiration of the EPZID delay timer.
              At this point, it is safe to orig on a EPZID change which 
              occured during the holddown timer period.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_process_delay_timer_expired(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("EPZID delay timer expired... see if new EPZID",0,0,0);

  ds707_pzid_compare_pzids(&ds707_epzid_list[0],&ds707_pzid_curr_epzid);

} /* ds707_pzid_process_delay_timer_expired() */

/*===========================================================================
FUNCTION      DS707_EPZID_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_epzid_parse_socm
(
  byte                       cntl,            /* SOCM CNTL field           */
  ds707_pzid_socm_msg_type*  epzid_socm_msg    /* SOCM with EPZID info       */
)
{
  snm_socm_return_type  result = SNM_VALID;  /* return value               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If cntl field tells mobile to disable EPZID based reconnection, reset
    the list.
  -------------------------------------------------------------------------*/
  if ((cntl & PZIDI_CNTL_MASK) == PZIDI_CNTL_DISABLE)
  {
    MSG_MED("Rx'ed SO Ctl Msg to disable zone reg.", 0, 0, 0);
    ds707_epzid_disable_recon();
  }

  /*-------------------------------------------------------------------------
    If cntl field tells mobile to enable EPZID recon, initialize the list and
    set the maximum elements of the list.
  -------------------------------------------------------------------------*/
  else if ((cntl & PZIDI_CNTL_MASK) == PZIDI_CNTL_ENABLE)
  {                
    ds707_epzid_max_list_len = (epzid_socm_msg->list_len &
                                                        PZIDI_LIST_LEN_MASK);
    MSG_MED("Rx'ed SO Ctl Msg to enable EPZID, len=%d", 
             ds707_epzid_max_list_len , 
             0, 
             0
           );
 
 #ifdef FEATURE_DS_EPZID_HYSTERESIS
    /*--------------------------------------------------------------------
      Enable Epzid hysteresis when max list length is 1 
    ---------------------------------------------------------------------*/
    ds707_epzid_hysteresis_process_event( ds707_epzid_max_list_len == 1 ? 
	  EPZID_HYS_SOCM_HYS_ENABLED_EVENT : EPZID_HYS_SOCM_HYS_DISABLED_EVENT );
 #endif /* FEATURE_DS_EPZID_HYSTERESIS */
   
   /*------------------------------------------------------------------------
     If the size of the EPZID list has shrunk,remove the oldest entries.
   ------------------------------------------------------------------------*/
    if ( ds707_epzid_oldest_epzid > ds707_epzid_max_list_len)
    {
      /*---------------------------------------------------------------------
        Clear the oldest entries 
      ---------------------------------------------------------------------*/
      ds707_epzid_clear_epzid_list(ds707_epzid_max_list_len, ds707_epzid_oldest_epzid);
      ds707_epzid_oldest_epzid = ds707_epzid_max_list_len;
      epzidi_print_epzid_list();
    }
  }

  /*-------------------------------------------------------------------------
   If cntl field tells mobile to clear the EPZID list, clear the list and
   reset the oldest index to 0.
  -------------------------------------------------------------------------*/
  else if ((cntl & PZIDI_CNTL_MASK) == PZIDI_CNTL_CLEAR_LIST)
  {
    ds707_epzid_max_list_len = (epzid_socm_msg->list_len & PZIDI_LIST_LEN_MASK);
    MSG_MED("Rx'ed SO Ctl Msg to clear EPZID list, max_len = %d", 
             ds707_epzid_max_list_len, 
             0, 
             0
           );
    ds707_epzid_clear_epzid_list(0,ds707_epzid_oldest_epzid);
    ds707_epzid_oldest_epzid = 0;
  }

  /*-------------------------------------------------------------------------
    Unknown EPZID_CNTL value. Return with SNM_INVALID_MSG_TYP. This will
    result in MS sending a Mobile Station Reject Order as per spec.
  -------------------------------------------------------------------------*/
  else
  {
    MSG_HIGH("Unknown EPZID_CNTL value: 0x%x",
              (cntl & PZIDI_CNTL_MASK) >> 5, 
              0, 
              0
             );
    result = SNM_INVALID_MSG_TYP;
  }

  return(result);
} /* ds707_pzid_parse_socm() */


/*===========================================================================
FUNCTION      DS707_EPZID_RLP_DATA_RCVD_CB

DESCRIPTION   This callback function should be called when a (new) RLP packet
              is sent/received after dormancy. It deregisters itself with
              RLP and notifies the EPZID hysterisis engine of the data
              transfer.

DEPENDENCIES  This callback should be registered with RLP when the data 
              session goes dormant and should be called by RLP when the first 
              PPP packet is sent received after dormancy.

              This function is only applicable for 1X RLP3.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_rlp_data_transfer_cb( byte rlp_instance )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("DS707 EPZID RLP DATA TRANSFER CB - RLP isntance %d",
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
    Notify the EPZID hysterisis engine of data transfer and 
    deregister this function with RLP3. The function will be
    registered again when the call goes dormant.
  ----------------------------------------------------------------*/
  dsrlp_reg_ppp_data_transfer_notify_func(rlp_instance, NULL);

#ifdef FEATURE_DS_EPZID_HYSTERESIS
  ds707_epzid_hysteresis_post_event(EPZID_HYS_DATA_READY_EVENT);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
} /* ds707_epzid_rlp_data_transfer_cb() */


#endif /* FEATURE_DATA_EPZID */
#endif /* FEATURE_DATA_IS707 */
