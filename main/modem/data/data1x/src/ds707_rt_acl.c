/*===========================================================================

   C D M A   2 0 0 0   ( 1 x )   S P E C I F I C   M O D E   H A N D L E R

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
       F O R   R O U T I N G   V I A   T H E   1 x   I N T E R F  A C E

GENERAL DESCRIPTION
  The 1x specific routing ACL is used to enable policy and address
  based routing across the 1x interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2010 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_rt_acl.c#9 $ $DateTime: 2010/05/28 04:37:56 $ $Author: parmjeet $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/10   ps      Added support to pick up the profile id with unspecified bit
                   set for Legacy/Non-OMH Applications.
04/19/10   sn      Changes to support 3GPD Ext (n15) disabled scenario in OMH.
11/24/08   psng    Added support to use legacy NV items if number of valid SIP
                   profiles is zero.
11/14/08   psng    Made Changes to use legacy credentials for MIP and
                   MIP to SIP fallback when OMH is enabled.
08/25/08   psng    Added boundary check for requesting profile id.
07/11/08   sn      Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
06/24/08   sn      Modified to use updated data session function/variable names.
04/21/08   sn      Added reporting of events for arbitration manager.
03/10/08   psng    Added support for Arbitration manager functionality.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
05/13/03   usb     Added new rules to handle multiple enabled ifaces 
04/11/03   ak      Updated for new 707 RMSM interfaces.
11/19/02   usb     Added a DENY rule to the ACL used by IS707 pkt iface,this
                   rule denies access to the iface if its already in use
08/30/02   om      Created Module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_iface.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds707_rmsm.h"
#endif

#ifdef T_MSM6300
#error code not present
#endif

#ifdef FEATURE_DS_MULTIPLE_PROFILES
#include "ds707_data_session_profile.h"
#include "ds707_event_defs.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif

#if defined(FEATURE_UIM_SUPPORT_3GPD)
#include "nvruimi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#endif
/* <EJECT> */ 
/*===========================================================================

                        FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/* Detrmines the currently pref mode for pkt origination from CM */
boolean ds707_rti_is_pref_mode( void * );

#ifdef FEATURE_DS_MULTIPLE_PROFILES
/* This function will compare priority of requesting and current application */
boolean ds707_rti_process_priority
(                         
  acl_policy_info_type *policy_info_ptr,
  void *this_if_ptr
);

void ds707_rti_post_proc
(
  acl_ipfilter_info_type *ipfilter_info_ptr,
  acl_policy_info_type *policy_info_ptr,
  void *this_if_ptr
);
#endif /* FEATURE_DS_MULTIPLE_PROFILES */

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

ACL_DEF( ds707_rt_acl )
ACL_START
  PASS_QUICK( ACL_DEFAULT_CLASS, IS_POL_INFO_NULL() );
  DENY( DIFFERENT_IFACE_REQUIRED() );
  DENY( NOT( IFACE_IS_PREF_MODE( ds707_rti_is_pref_mode, NULL ) ) );
  #ifndef FEATURE_DATA_STRIP_ATCOP
  DENY( IFACE_IS_IN_USE( ds707_rmsm_is_iface_in_use, ds707_rmsm_rt_acl_post_proc ) );
  #endif
  #ifdef FEATURE_DS_MULTIPLE_PROFILES
   DENY( NOT( APP_PRIORITY_CHECK(ds707_rti_process_priority) ) );
  #endif
  PASS( ACL_IFNAME_CLASS,  REQUIRED_IFACE_IS( CDMA_SN_IFACE ) );
  PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_IS( WWAN_GROUP ) );
  PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_IS( ANY_DEFAULT_GROUP ) );
  PASS( ACL_DEFAULT_CLASS, ALL );
ACL_END

/*===========================================================================
FUNCTION DS707_RTI_IS_PREF_MODE

DESCRIPTION
   This function determines the currently preferred packet origination mode
   from CM.  This is needed for "actual" multimoe targets like Leopard. 
   For the targets which can have only single mode, (presumably) CDMA, 
   this function always returns TRUE.
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  param - iface specific information required to make the decision
          (currently not used)
  
RETURN VALUE 
  TRUE if any falvor of CDMA is the preffered mode for packet data calls,
  FALSE otherwise.
  
SIDE EFFECTS 
  Returns FALSE if CM says SYS_SYS_MODE_NO_SRV ie routing lookup during
  a fade will never pick this iface.
  
===========================================================================*/
boolean ds707_rti_is_pref_mode( void *param )
{
#ifdef T_MSM6300
#error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_MSM6300
#error code not present
#endif

  return TRUE;
} /* ds707_rti_is_pref_mode() */


#ifdef FEATURE_DS_MULTIPLE_PROFILES
/*===========================================================================
FUNCTION DS707_RTI_PROCESS_PRIORITY()

DESCRIPTION
  This function will compare priority of requesting and current application.

DEPENDENCIES
  None.

PARAMETERS
  this_if_ptr     : Current active iface pointer 
  policy_info_ptr : Pointer to current application policy info block
  
RETURN VALUE
  TRUE  : If priority of requesting application is more than or equal to current
          application.
  FALSE : If the priority of requesting profile is less than the priority of 
          current application.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds707_rti_process_priority
(                        
  acl_policy_info_type *policy_info_ptr,
  void                 *this_if_ptr
)
{
   int8          current_profile_id = DATA_SESSION_PROFILE_ID_INVALID; 
   int8          requesting_profile_id = DATA_SESSION_PROFILE_ID_INVALID;
   uint8         current_app_priority;
   uint8         requesting_app_priority; 
   ps_iface_type *iface_ptr=NULL;
   boolean       ret_val = FALSE;
   uint32        pkt_instance = DS707_DEF_PKT_INSTANCE;
   ds707_event_payload_type ds707_event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   if( (num_valid_profiles == 0)
#ifdef FEATURE_DS_MOBILE_IP
       || (dsat707_qcmip_val > 0)
#endif /* FEATURE_DS_MOBILE_IP */
     )
   {
     return TRUE;
   }

#ifdef FEATURE_UIM_SUPPORT_3GPD
   if( (NV_RTRE_CONTROL_NO_RUIM != nv_rtre_control()) && 
       (!nvruim_data_3gpd_ext_support()) )
   {
     MSG_HIGH("3GPD Ext not supported in RUIM. Use legacy behavior.", 0, 0, 0);
     return TRUE;
   }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

   iface_ptr = (ps_iface_type*) this_if_ptr;
   pkt_instance = (uint32) ( iface_ptr->client_data_ptr );

   current_profile_id = ds707_data_session[pkt_instance].current_profile_id;
   requesting_profile_id = policy_info_ptr->data_session_profile_id;
   
   if( requesting_profile_id <= DATA_SESSION_PROFILE_ID_INVALID ||
       requesting_profile_id >  num_valid_profiles )
   {
     MSG_HIGH( "Bad requesting app profile id=%d ",
               requesting_profile_id, 0, 0 );
     return FALSE;
   }

   if( requesting_profile_id == DATA_SESSION_PROFILE_INIT_VAL )
   {
     requesting_profile_id = ds707_data_session_get_default_profile();
     policy_info_ptr->data_session_profile_id = requesting_profile_id;
   }

   MSG_MED( "Current App Profile ID=%d; Requesting App Profile ID=%d", 
             current_profile_id, requesting_profile_id, 0);

   if( current_profile_id != DATA_SESSION_PROFILE_ID_INVALID )
   { 
      current_app_priority = 
                  ds707_data_session_get_app_priority(current_profile_id);
      requesting_app_priority = 
                   ds707_data_session_get_app_priority(requesting_profile_id);
      MSG_MED( "Current app priority is %d; Requesting app priority is %d", 
               current_app_priority,requesting_app_priority, 0 );

      /* Priority - 0 is highest and 255 is lowest */
      if(requesting_app_priority < current_app_priority)
      {    
        MSG_HIGH("Requesting app wins", 0, 0, 0);
        policy_info_ptr->post_process_fptr = ds707_rti_post_proc;
        ret_val = TRUE;
      }			
      else if(requesting_app_priority == current_app_priority)
      {          
        MSG_HIGH("Both app will share the data connection", 0, 0, 0);
        ret_val = TRUE;
      }
      else
      {
        MSG_HIGH("Requesting app is of low priority.", 0, 0, 0);
        MSG_HIGH("Rejecting requesting app.", 0, 0, 0);
      }
   }
   else
   { 
      MSG_HIGH( "Requesting app is first app. Profile ID is %d.", 
               policy_info_ptr->data_session_profile_id, 0, 0 );
      policy_info_ptr->post_process_fptr = ds707_rti_post_proc ;
      ret_val = TRUE;
   }
   
   if( ret_val )
   {
     ds707_event_payload.ds707_event = EVENT_DS707_DATA_CALL_REQ_ACCEPTED;
   }
   else
   {
     ds707_event_payload.ds707_event = EVENT_DS707_DATA_CALL_REQ_REJECTED;
   }

   ds707_event_payload.info.zero = 0;
   event_report_payload( EVENT_DS707, sizeof(ds707_event_payload), 
                         &ds707_event_payload );

   return ret_val;
}

/*===========================================================================
FUNCTION DS707_RTI_POST_PROC

DESCRIPTION
  This function performs the post processing for the priority check ACL  for 
  IS707 iface.

DEPENDENCIES
  None

PARAMETERS
  ipfilter_info_ptr : IP packet information needed by ipfilter 
                      (IP header + extra info)
  this_if_ptr       : Current active iface pointer 
  policy_info_ptr   : Pointer to current application policy info block

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rti_post_proc
(
  acl_ipfilter_info_type *ipfilter_info_ptr,
  acl_policy_info_type   *policy_info_ptr,
  void                   *this_if_ptr
)
{
   ps_iface_type *iface_ptr;
   int16         ps_iface_errno;
   uint32        pkt_instance = DS707_DEF_PKT_INSTANCE;
   int           ret_val = 0;         /* iface tear down cmd ret val */
/*-----------------------------------------------*/

   iface_ptr = (ps_iface_type *) this_if_ptr;
   pkt_instance = (uint32) ( iface_ptr->client_data_ptr );

   MSG_MED( "Post proc - iface is 0x%x::%d", 
            iface_ptr->name, iface_ptr->instance, 0 );

   if( ds707_data_session[pkt_instance].current_profile_id != 
                                        DATA_SESSION_PROFILE_ID_INVALID )
   {  
     MSG_HIGH("Low priority app exists with Profile id %d. Tearing down.", 
              ds707_data_session[pkt_instance].current_profile_id , 0, 0 );

     ret_val=ps_iface_tear_down_cmd( iface_ptr, &ps_iface_errno, NULL );
     if((ret_val < 0) && (ps_iface_errno != DS_EWOULDBLOCK))
     {
       MSG_ERROR("iface tear down fail, errno: %d", ps_iface_errno, 0, 0);
     }
     else
     {
       MSG_MED("iface tear down intiated", 0, 0, 0);
     }
   }

   ds707_data_session_set_requesting_profile( iface_ptr, 
                                     policy_info_ptr->data_session_profile_id);

  return;
}/* ds707_rti_post_proc() */

#endif /* FEATURE_DS_MULTIPLE_PROFILES */
