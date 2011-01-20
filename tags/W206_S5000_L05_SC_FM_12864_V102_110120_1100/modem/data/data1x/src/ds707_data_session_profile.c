/*===========================================================================
                            D A T A   S E R V I C E S

                     MULTIPLE SIP PROFILE UTILITY FUNCTIONS

DESCRIPTION

   The Data Services Multiple Simple IP profile utility functions source file.

EXTERNALIZED FUNCTIONS

   ds707_data_session_read_profile_info()
      Get sip profiles from NV item and store it in profile array

Copyright (c) 2000-2010 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_data_session_profile.c#6 $ $DateTime: 2010/04/21 00:25:58 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/19/10   sn      Changes to support 3GPD Ext (n15) disabled scenario in OMH.
19/01/10   ps      Added code changes to reset requesting and current
                   profile IDs when IFACE is brought down.
11/14/08   psng    Added boundary check for OMH profile related parameters.
08/25/08   psng    Added Default Profile id lookup Support
07/11/08   sn      Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
06/24/08   sn      Modified function/variable names to follow naming convention.
04/29/08   psng    Fix the bug in App Type bit mask matching.  
03/10/08   psng    Added new functions for arbitration manager support
02/26/08   psng	   Featurize the module 	 
02/01/08   psng	   Created the module
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MULTIPLE_PROFILES
#include "msg.h"
#include "err.h"
#include "nv.h"
#include "dstask.h"
#include "dstaski.h"
#include "ds707_data_session_profile.h"
#include "ds707_pkt_mgr.h"

#if defined(FEATURE_UIM_SUPPORT_3GPD)
#include "nvruimi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

/*===========================================================================

                             EXTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  Simple IP session info array
---------------------------------------------------------------------------*/
ds707_data_session_profile_info_type 
           ds707_data_session_profile_info[DATA_SESSION_MAX_PROFILES];

/*---------------------------------------------------------------------------
  data session info array
---------------------------------------------------------------------------*/
ds707_data_session_info_type  ds707_data_session[DS707_MAX_PKT_CALLS];

/*---------------------------------------------------------------------------
  Global placeholder for valid no of profiles
---------------------------------------------------------------------------*/
uint8                         num_valid_profiles;

/*===========================================================================
                             FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_REG_IFACE_EVENTS

DESCRIPTION   
  This function registers for iface up and iface down event for v4 iface.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_reg_iface_events(void);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IFACE_EVENT_CB

DESCRIPTION  
  This function is called when ifac_up_ev, iface_down_ev events are generated for 
  registerd v4 ifaces.This function updates requesting and current data session
  profile ids for the given iface.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_iface_event_cb
(
  ps_iface_type                *this_iface_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                         *user_data_ptr
);

/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS707_DATA_SESSION_READ_PROFILE_INFO

DESCRIPTION
  This function gets sip profiles from NV item  for the Data task and stores them in
  the appropriate variable.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
 Each call to 'dsi_nv_get_item' results in a Wait and watchdog kicking
===========================================================================*/
void ds707_data_session_read_profile_info(void)
{
   nv_stat_enum_type  nv_status;      /* status from NV call  */
   nv_item_type       loc_nv_item;
   uint8              index;          /* 8 bit index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_SUPPORT_3GPD
   if( (NV_RTRE_CONTROL_NO_RUIM != nv_rtre_control()) && 
       (!nvruim_data_3gpd_ext_support()) )
   {
     MSG_HIGH("3GPD Ext not supported in RUIM. Don't read profiles.", 0, 0, 0);
     return;
   }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

   nv_status = dsi_get_nv_item(NV_DS_SIP_NUM_VALID_PROFILES_I ,&loc_nv_item);
   if( nv_status == NV_DONE_S )
   {
     num_valid_profiles = loc_nv_item.ds_sip_num_valid_profiles;
     if( num_valid_profiles > DATA_SESSION_MAX_PROFILES )
     {
       MSG_ERROR("Num valid profiles exceeds max supported profiles",0,0,0);
       MSG_ERROR("Limiting num valid profiles to max supported profiles",0,0,0);
       num_valid_profiles = DATA_SESSION_MAX_PROFILES;
     }

     for(index=0;index<num_valid_profiles;index++)
     {
       loc_nv_item.ds_sip_profile.index = index;
       nv_status = dsi_get_nv_item(NV_DS_SIP_PROFILE_I,&loc_nv_item);

       if( nv_status == NV_DONE_S)
       {
         MSG_HIGH( "SUCCESSFUL READING OF SIP PROFILE %d",index,0,0 );
         MSG_HIGH( "APP TYPE=%d APP_PRIORITY=%d nai_entry_index=%d",
                   loc_nv_item.ds_sip_profile.app_type, 
                   loc_nv_item.ds_sip_profile.app_priority,
                   loc_nv_item.ds_sip_profile.index);

         ds707_data_session_profile_info[index].app_priority 
                            = loc_nv_item.ds_sip_profile.app_priority;
         ds707_data_session_profile_info[index].app_type 
                            = loc_nv_item.ds_sip_profile.app_type;
         ds707_data_session_profile_info[index].index 
                            = loc_nv_item.ds_sip_profile.index;
       }
       else
       {
       #ifdef FEATURE_UIM_RUIM
          ERR( "Bad NV sip profile read status %d", nv_status, 0, 0 );
       #else
          ERR_FATAL( "Bad NV sip profile read  status %d", nv_status, 0, 0 );
       #endif /* FEATURE_UIM_RUIM */
       }		
     } /*end of for loop*/
  	 }
    else
    {
      ERR( "Bad NV sip_num_valid_profiles, read status %d", nv_status, 0, 0 ); 
    }  
} /* ds707_data_session_read_profile_info() */

/*===========================================================================
FUNCTION DS_GET_APP_PROFILE_INDEX()

DESCRIPTION
  This function will return the application profile index based on the application type 
  passed as the parameter to it.

DEPENDENCIES
  None.

PARAMETERS
  app type : type of the application for which you want profile index
  
RETURN VALUE
  int8 :  Valid profile id on success.
         -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int8 ds_get_app_profile_index(uint32 app_type)
{
  boolean default_profile_found = FALSE;
  int8    app_profile_id = DATA_SESSION_PROFILE_ID_INVALID;
  int8    index=0 ;         /* 8 bit index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  for(index=0; index<num_valid_profiles; index++)
  {
    if( app_type & ds707_data_session_profile_info[index].app_type )
    {
      app_profile_id = index;
      break;
    }
  }

  if( app_profile_id == DATA_SESSION_PROFILE_ID_INVALID )
  {
    for( index=0; index<num_valid_profiles; index++ )
    {
      if( ds707_data_session_profile_info[index].app_type & DEFAULT_PROFILE )
      {
        default_profile_found = TRUE;
        app_profile_id = index;
        break;
      }
    }

    if( default_profile_found == TRUE )    /* Default profile is available */
    {
      MSG_HIGH( "App will use default profile. Profile id is %d", 
                app_profile_id, 0, 0 );
    }
    else
    {
      MSG_ERROR( "App profile id lookup failed.", 0, 0, 0 );
    }
  }
  else
  {
    MSG_HIGH( "APP profile lookup succeeded. Profile ID is %d App type is %d",
               app_profile_id, app_type, 0 );
  }

  return app_profile_id;
} /* ds_get_app_profile_index() */


/*===========================================================================
FUNCTION DS707_DATA_SESSION_SET_REQUESTING_PROFILE()

DESCRIPTION
  This function will set the requesting profile_id to new application profile_id

DEPENDENCIES
  None.

PARAMETERS
  ps_iface_type : Access the client_data_ptr for this iface
  int8          : Profile id of the new requesting application

RETURN VALUE
 None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds707_data_session_set_requesting_profile
(
 ps_iface_type *iface_ptr,
 int8          new_profile_id
)
{
  uint32  pkt_instance = DS707_DEF_PKT_INSTANCE;
/*--------------------------------------------------------------------------*/

  pkt_instance = (uint32) ( iface_ptr->client_data_ptr );
  ds707_data_session[pkt_instance].requesting_profile_id = new_profile_id;

  return;
}

/*===========================================================================
FUNCTION DS707_DATA_SESSION_SET_CURRENT_PROFILE()

DESCRIPTION
  This function will set the current profile_id to new application profile_id

DEPENDENCIES
  None.

PARAMETERS
   ps_iface_type : current active iface pointer

RETURN VALUE
 None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds707_data_session_set_current_profile
(
  ps_iface_type *iface_ptr
)
{
  uint32  pkt_instance = DS707_DEF_PKT_INSTANCE;
/*---------------------------------------------------------------------------*/

  pkt_instance = (uint32) ( iface_ptr->client_data_ptr );
  ds707_data_session[pkt_instance].current_profile_id = 
                    ds707_data_session[pkt_instance].requesting_profile_id;

  MSG_MED( "Now the current profile id is %d", 
           ds707_data_session[pkt_instance].current_profile_id, 0, 0);

  return;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INFO_INIT

DESCRIPTION   This function initializes the data_session_info type structure.

PARAMETERS    
   *iface_ptr : current active iface pointer

DEPENDENCIES  None

RETURN VALUE    0  - on success
               -1  - on failure

SIDE EFFECTS  None
===========================================================================*/
int8 ds707_data_session_info_init
(
  ps_iface_type    *iface_ptr
)
{
  uint32  pkt_instance = DS707_DEF_PKT_INSTANCE;
/*-------------------------------------------------------------------------*/

  if( iface_ptr == NULL )
  {
     ASSERT(0);
     return -1;
  }
  else
  {  
    /* Initialize both current and requesting id to 
       DATA_SESSION_PROFILE_ID_INVALID */

    pkt_instance = (uint32) ( iface_ptr->client_data_ptr );
    ds707_data_session[pkt_instance].current_profile_id = 
                                  DATA_SESSION_PROFILE_ID_INVALID;
    ds707_data_session[pkt_instance].requesting_profile_id = 
                                  DATA_SESSION_PROFILE_ID_INVALID;

    return 0;
  }
}

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_APP_PRIORITY()

DESCRIPTION
  This function will return the priority corresponding to the paased profile index

DEPENDENCIES
  None.

PARAMETERS
profile_id= index of the profile 

RETURN VALUE
  app_priority
  
SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds707_data_session_get_app_priority(int8 profile_id)
{
  return (ds707_data_session_profile_info[profile_id].app_priority);
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INIT

DESCRIPTION  
  This function called at power up and register for IFACE_DOWN and IFACE_UP
  events to enable reseting of requesting and current profile IDs when IFACE
  is brought down.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_init ( void )
{
  ds707_data_session_reg_iface_events();
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_REG_IFACE_EVENTS

DESCRIPTION   
  This function registers for iface up and iface down event for v4 iface.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_reg_iface_events(void)
{
  ps_iface_type  * iface_ptr;    /* ptr to 1x iface */
  void           * iface_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  iface_ptr = GET_PS_IFACE_PTR( DS707_DEF_PKT_INSTANCE );
  if( iface_ptr == NULL )
  {
    MSG_ERROR(" NULL iface pointer ",0,0,0);
    ASSERT(0);
  }

  /*-----------------------------------------------------------------------
    Initialize and register the iface down ind callback info.
  -----------------------------------------------------------------------*/

  iface_buf_ptr = ps_iface_alloc_event_cback_buf (
    ds707_data_session_iface_event_cb, (void *)DS707_DEF_PKT_INSTANCE );
  
  if ( iface_buf_ptr == NULL )
  {
    MSG_ERROR("Event buf allocation failed",0,0,0);
    ASSERT(0);
  }

  if( ps_iface_event_cback_reg( iface_ptr, IFACE_DOWN_EV,
                                iface_buf_ptr  ) < 0 )
  {
    MSG_ERROR("IFACE_DOWN event reg failed", 0, 0, 0 );
  }

 /*-----------------------------------------------------------------------
    Initialize and register the iface up ind callback info.
  -----------------------------------------------------------------------*/
 iface_buf_ptr = ps_iface_alloc_event_cback_buf (
   ds707_data_session_iface_event_cb, (void *)DS707_DEF_PKT_INSTANCE );
  
  if ( iface_buf_ptr == NULL )
  {
    MSG_ERROR("Event buf allocation failed",0,0,0);
    ASSERT(0);
  }

  if( ps_iface_event_cback_reg( iface_ptr, IFACE_UP_EV,
                                iface_buf_ptr  ) < 0 )
  {
    MSG_ERROR( "IFACE_UP event reg failed", 0, 0, 0 );
  }
   
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IFACE_EVENT_CB

DESCRIPTION  
  This function is called when iface_down_ev event are generated for registerd 
  v4 ifaces.This function updates requesting and current data session profile
  ids for the given iface.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_iface_event_cb
(
  ps_iface_type                *this_iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_data_ptr
)
{
  ps_iface_type     *iface_ptr;  /* ptr to 1x iface */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 iface_ptr = GET_PS_IFACE_PTR( DS707_DEF_PKT_INSTANCE );

  switch(event)
  {
    case IFACE_DOWN_EV:
    {
      MSG_HIGH( "Resetting the current and requesting profile IDs", 0,0,0 );
      ds707_data_session_info_init( iface_ptr );
      break;
    }
    case IFACE_UP_EV:
    {
       /* Do Nothing */ 
       break;
    }
    default:
    {
      MSG_ERROR( "Recieved unknown event", 0,0,0 );
    }
  }
}

#endif /* FEATURE_DS_MULTIPLE_PROFILES */
