 /*===========================================================================
                            D A T A   S E R V I C E S

                     MULTIPLE SIP PROFILE UTILITY FUNCTIONS

DESCRIPTION

   The Data Services Multiple Simple IP profile utility functions source file.

EXTERNALIZED FUNCTIONS

   ds707_data_session_read_profile_info()
      Get sip profiles from NV item and store it in profile array

Copyright (c) 2000-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_data_session_profile.c#4 $ $DateTime: 2008/11/14 06:41:03 $ $Author: parmjeet $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
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

#endif /* FEATURE_DS_MULTIPLE_PROFILES */
