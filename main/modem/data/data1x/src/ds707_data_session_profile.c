/*===========================================================================
                            D A T A   S E R V I C E S

                            707 DATA SESSION PROFILE

DESCRIPTION

   The Data Services Multiple Simple IP profile utility functions source file.

EXTERNALIZED FUNCTIONS

   ds707_data_session_read_profile_info()
      Get sip profiles from NV item and store it in profile array

Copyright (c) 2000-2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_data_session_profile.c#9 $ $DateTime: 2011/03/30 08:36:03 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/11   ms      Added support to pick non-default profile for non-OMH apps.
03/09/11   sn      Merged support to pick auth credentials from NV for CT RUIM 
                   cards.
05/28/10   ps      Added support to pick up the profile id with unspecified bit
                   set for Legacy/Non-OMH Applications.
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
#include "ds707_util.h"
#include "dsati.h"
#include "AEEstd.h"
#include "ps_ppp_defs.h"
#include "rex.h"


#if defined(FEATURE_UIM_SUPPORT_3GPD)
#include "nvruimi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

/*===========================================================================

                             EXTERNAL DATA

===========================================================================*/
LOCAL nv_cmd_type  nv_data_cmd;

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
uint8                         num_valid_profiles  = 0;
uint8                         num_valid_nais = 0;
char                          nai_info[PPP_MAX_USER_ID_LEN];

typedef struct
{
  uint32 app_type;
  char  nai[PPP_MAX_USER_ID_LEN];
}ds707_data_session_profile_nai_info;

ds707_data_session_profile_nai_info 
                  special_nai_info[DATA_SESSION_MAX_PROFILES];
typedef enum
{
  DS_APP_TYPE         = 0x0,
  DS_NAI              = 0x1
}ds707_data_session_profile_field_type;
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
FUNCTION  DS707_DATA_SESSION_READ_LEGACY_NAI

DESCRIPTION
  This function will read the LEGACY NAI from NV.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds707_data_session_read_legacy_nai();


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
   nv_stat_enum_type nv_status;      /* status from NV call  */
   nv_item_type      loc_nv_item;
   uint8             index;          /* 8 bit index */
   int8              profile_id = DATA_SESSION_PROFILE_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_SUPPORT_3GPD
   if( (NV_RTRE_CONTROL_NO_RUIM != nv_rtre_control()) && 
       (!nvruim_data_3gpd_ext_support()) && 
       !(NVRUIM_NON_3GPD_CARD == nvruim_check_non_3gpd_cdma_card()) )
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
         
         profile_id = DATA_SESSION_MIN_PROFILE +
                           loc_nv_item.ds_sip_profile.index;
         
         ds707_data_session_profile_info[index].app_priority 
                            = loc_nv_item.ds_sip_profile.app_priority;
         ds707_data_session_profile_info[index].app_type 
                            = loc_nv_item.ds_sip_profile.app_type;
         ds707_data_session_profile_info[index].index 
                            = profile_id;
       }
       else
       {
          MSG_ERROR( "Bad NV sip profile read status %d", nv_status, 0, 0 );
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
      app_profile_id = index + DATA_SESSION_MIN_PROFILE;
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
        app_profile_id = index + DATA_SESSION_MIN_PROFILE;
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
  int8 profile_index = DATA_SESSION_PROFILE_ID_INVALID;
 /*-------------------------------------------------------------------------*/

  profile_index = profile_id - DATA_SESSION_MIN_PROFILE;

  return (ds707_data_session_profile_info[profile_index].app_priority);
}

/*===========================================================================
FUNCTION DS707_DATA_READ_SPECIAL_NAIS_FROM_EFS()

DESCRIPTION
  This function will parese the special_nai.txt file frome efs and fills the profile_info data 
  structure

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  SUCCESS - if parsing is completed correctly.
  FAILURE - if parsing found to be incorrect.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds707_data_read_special_nais_from_efs( )
{
   int copy_len, i;
   char *from, *to;
   ds_efs_token_type fparser;
   ds_efs_token_parse_status_enum_type  ret_val;
   ds707_data_session_profile_field_type profile_field;
   char value[PPP_MAX_USER_ID_LEN];
   uint32 data = 0;
   boolean bad_record = FALSE;
   boolean skipped_port = FALSE;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   ret_val = DS_EFS_TOKEN_PARSE_SUCCESS;
   profile_field = DS_APP_TYPE;
   
   if( ds_efs_file_init ("/nv/item_files/modem/3GPP2/omh_nai_config.txt", &fparser) == -1 )
   {
     MSG_ERROR("Exiting OUT", 0, 0, 0);
     return FALSE;
   }

   /*-------------------------------------------------------------------------
     Parse each token at a time and assign it to the appropriate variable
    ------------------------------------------------------------------------*/
   i = 0;

   while ( DS_EFS_TOKEN_PARSE_EOF != (ret_val = ds_efs_tokenizer(&fparser, &from, &to )))
   {
      MSG_HIGH(" Ret val %d ",ret_val,0,0);
     /*------------------------------------------------------------------------
       Token being read. from points to the beginning of the token and
       to point to the end of the token.
      ------------------------------------------------------------------------*/
     if( DS_EFS_TOKEN_PARSE_SUCCESS == ret_val )
     {
       if(to == from )
       {
         continue ;
       }
       copy_len = 0;
       switch(profile_field)
       {
         case DS_APP_TYPE:
           value[0] = '\0';
           copy_len = (to-from);
           memcpy((void*)&value, (void*)from, copy_len);
           value[copy_len] = '\0';
           (void)dsatutil_atoi((void*)&data,(void*)&value,10);
           special_nai_info[num_valid_nais].app_type = data;           
           profile_field = DS_NAI;
           break;
 
         case DS_NAI:
           value[0] = '\0';
           copy_len = (to-from);
           memcpy((void*)&value, (void*)from, copy_len);
           value[copy_len] = '\0';
           std_strlcpy((char *)(special_nai_info[num_valid_nais].nai),(char*)&value,copy_len+1);
           profile_field = DS_APP_TYPE;
           num_valid_nais++;
           break;
       }
     }
     else if(DS_EFS_TOKEN_PARSE_EOL == ret_val )
     {
       if(profile_field == DS_NAI)
       {
         value[0] = '\0';
         copy_len = (to-from);
         if(copy_len == 0)
         { 
           continue;
         }
         memcpy((void*)&value, (void*)from, copy_len);
         value[copy_len] = '\0';
         std_strlcpy((char *)(special_nai_info[num_valid_nais].nai),(char*)&value,copy_len+1);
         profile_field = DS_APP_TYPE;
         num_valid_nais++;
       }
       if( fparser.eof == 1 )
       { 
         MSG_HIGH("Parsing completed. Valid special NAIs read : %d",num_valid_nais,0,0);
         return TRUE;
       }
     }
   }

   MSG_HIGH("Done Parsing the file",0,0,0);
 } /* ds707_data_read_special_nais_from_efs() */

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
  uint8 i = 0;

  ds707_data_session_reg_iface_events();

  ds707_data_read_special_nais_from_efs();
  for ( i = 0 ; i < num_valid_nais ; i++)
  {
    MSG_HIGH(" App type read from EFS : 0x%x, NAI : %s",
                   special_nai_info[i].app_type,
                   special_nai_info[i].nai,
                   0);
  }
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

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_DEFAULT_PROFILE()

DESCRIPTION
  This function will return the profile ID corresponds to un-specified bit set
  in app_type bit mask.

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  Valid profile id on success.
  -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int8 ds707_data_session_get_default_profile(void)
{

  boolean default_profile_found = FALSE;
  int8    app_profile_id = DATA_SESSION_PROFILE_ID_INVALID;
  int8    index=0 ;         /* 8 bit index */
/*------------------------------------------------------------------------- */

  for( index = 0; index<num_valid_profiles; index++ )
  {
    if( ds707_data_session_profile_info[index].app_type & DEFAULT_PROFILE )
    {
      default_profile_found = TRUE;
      app_profile_id = index + DATA_SESSION_MIN_PROFILE;
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
    
  return app_profile_id;
}

/*===========================================================================
FUNCTION  DS707_DATA_SESSION_GET_PROFILE_ID_FOR_SPECIAL_NAI

DESCRIPTION
  This function will return the priority for the special NAI.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  int8 - Profile ID
  
SIDE EFFECTS
  None.
===========================================================================*/
int8 ds707_data_session_get_profile_id_for_special_nai()
{
  uint8 i =0;
  int8 app_profile_id = DATA_SESSION_PROFILE_ID_INVALID;

  if( !nvruim_data_3gpd_ext_support() || ( num_valid_nais == 0 ))
  {
    MSG_HIGH("Num valid NAIs is zero",0,0,0);
    return DATA_SESSION_PROFILE_ID_INVALID;
  }
  else
  {
    ds707_data_session_read_legacy_nai();

    for ( i = 0; i < num_valid_nais; i++ )
    {
      if ( std_strcmp( (char*)(special_nai_info[i].nai), (char*)&nai_info) == 0 ) 
      {
        MSG_HIGH("Nai %s matches @index %d",special_nai_info[i].nai,i,0);
        break;
      }
      else
      {
        continue;
      }
    }

    if( i == num_valid_nais)
    {
      MSG_HIGH(" No matching profile found in efs file " ,0,0,0);
      return DATA_SESSION_PROFILE_ID_INVALID;
    }
    else
    {
      app_profile_id = ds_get_app_profile_index(special_nai_info[i].app_type);
      MSG_HIGH("Returning %d as profile ID",app_profile_id,0,0);
      return app_profile_id;
    }
  }
} /* ds707_data_session_get_profile_id_for_special_nai() */

/*===========================================================================
FUNCTION  DS707_DATA_SESSION_READ_LEGACY_NAI

DESCRIPTION
  This function will read the LEGACY NAI from NV.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds707_data_session_read_legacy_nai()
{
   nv_item_type         nv_item;
   uint8                nai_len;

  /*-------------------------------------------------------------------------
   Read the PPP User ID from NV. 
  -------------------------------------------------------------------------*/
  memset((void*)&nai_info, 0, PPP_MAX_USER_ID_LEN);
  nv_data_cmd.tcb_ptr = NULL;    /* notify this task when done */
  nv_data_cmd.sigs = NULL;
  nv_data_cmd.done_q_ptr = NULL;       /* command goes on no queue when done */

  nv_data_cmd.item = NV_PPP_USER_ID_I;             /* item to read */
  nv_data_cmd.cmd = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  nv_data_cmd.data_ptr = &nv_item;

  nv_cmd( &nv_data_cmd );
  
  /*-------------------------------------------------------------------------
    If NV read succeeds, load the user ID into the provided ppp config
  -------------------------------------------------------------------------*/
  if( nv_data_cmd.status == NV_DONE_S)
  {
    nai_len = nv_item.ppp_user_id.user_id_len;
    memcpy( nai_info,
           (char *)nv_item.ppp_user_id.user_id,
            nai_len );
    MSG_HIGH(" NAI read : %s ", nai_info,0,0);
  } 
  else
  {
    MSG_ERROR( "PPP user ID never written", 0, 0, 0 );
  }
} /* ds707_data_session_read_legacy_nai() */

#endif /* FEATURE_DS_MULTIPLE_PROFILES */
