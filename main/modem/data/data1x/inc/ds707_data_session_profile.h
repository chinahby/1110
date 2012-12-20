#ifndef DS707_DATA_SESSION_PROFILE_H
#define DS707_DATA_SESSION_PROFILE_H

/*===========================================================================
                            D A T A   S E R V I C E S

                     MULTIPLE SIP PROFILE UTILITY FUNCTIONS

DESCRIPTION

  The Data Services Multiple Simple IP profile utility functions source file.

EXTERNALIZED DATA
  ds707_data_session_profile_info - Profile info array
  ds707_data_session              - Data Session info array
  num_valid_profiles              - No of Valid Profiles

EXTERNALIZED FUNCTIONS
  ds707_data_session_read_profile_info
     Get sip profiles from NV item and store it in profile array
     
Copyright (c) 2000-2010 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_data_session_profile.h#8 $ $DateTime: 2011/03/30 08:44:12 $ $Author: msankar $

  when      who    what, where, why
--------    ---    ----------------------------------------------------------
03/30/11   ms      Added support to pick non-default profile for non-OMH apps.
05/28/10   ps      Added support to pick up the profile id with unspecified bit
                   set for Legacy/Non-OMH Applications.
19/01/10   ps      Added code changes to reset requesting and current
                   profile IDs when IFACE is brought down.
11/20/08   psng    Added multiple profile id look up support for Laptop call
08/25/08   psng    Added Default Profile id lookup Support
07/11/08   sn      Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
06/24/08   sn      Modified to follow naming convention.
03/10/08   psng    Added new functions for Arbiration manager support 
02/01/08   psng	   created the module

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ds707_pkt_mgr.h"

#ifdef FEATURE_DS_MULTIPLE_PROFILES

/*===========================================================================

                             PUBLIC CONSTANTS

===========================================================================*/
#define  DATA_SESSION_MIN_PROFILE          1
#define  DATA_SESSION_MAX_PROFILES        16
#define  DATA_SESSION_PROFILE_ID_INVALID  -1
#define DATA_SESSION_PROFILE_INIT_VAL      0

#define  DEFAULT_PROFILE                   0x00000001
#define  TETHERED_MODE_APP_TYPE            0x00000040

/*===========================================================================

                         PUBLIC DATA STRUCTURES

===========================================================================*/
/*------------------------------------------------------------------------
structure to store multiple sip profile
--------------------------------------------------------------------------*/
typedef struct
{  
  uint8  index;
  uint8  app_priority;
  uint32 app_type;
} ds707_data_session_profile_info_type;
	
typedef struct
{
  int8 current_profile_id;
  int8 requesting_profile_id;
} ds707_data_session_info_type;


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Session info array, each entry holds all parameters related to a single 
  SIP session
---------------------------------------------------------------------------*/
extern ds707_data_session_profile_info_type 
                  ds707_data_session_profile_info[DATA_SESSION_MAX_PROFILES];
extern ds707_data_session_info_type ds707_data_session[DS707_MAX_PKT_CALLS];
extern uint8                        num_valid_profiles;
extern uint8                        num_valid_nais;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*==========================================================================
FUNCTION ds707_data_session_read_profile_info
	
DESCRIPTION
  This function gets sip profiles from NV item	for the Data task and stores them in
  the appropriate variable.
	
DEPENDENCIES
  The NV task must be started and running.
	
RETURN VALUE
  None
	
SIDE EFFECTS
  Each call to 'dsi_nv_get_item' results in a Wait and watchdog kicking
===========================================================================*/
void ds707_data_session_read_profile_info(void);

/*===========================================================================
FUNCTION DS_GET_APP_PROFILE_INDEX()

DESCRIPTION
  This function will return the application profile index based on the application type 
  passed as the parameter to it.

DEPENDENCIES
  None.

PARAMETERS
  app type:- type of the application for which u want profile index
  
RETURN VALUE
  profile index  on success.
  -1 			 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int8 ds_get_app_profile_index(uint32 app_type);

/*===========================================================================
FUNCTION DS707_DATA_SESSION_SET_REQUESTING_PROFILE()

DESCRIPTION
  This function will set the requesting profile_id to new application profile_id

DEPENDENCIES
  None.

PARAMETERS
  ps_iface_type *:  To access the session_info_ptr for this iface 
  new_profile_id:    profile id of the new requesting application

RETURN VALUE
 None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds707_data_session_set_requesting_profile
(
  ps_iface_type *iface_ptr,
  int8 new_profile_id
);

/*===========================================================================
FUNCTION DS707_DATA_SESSION_SET_CURRENT_PROFILE()

DESCRIPTION
  This function will set the current profile_id to new application profile_id

DEPENDENCIES
  None.

PARAMETERS
  ps_iface_type : To access the session_info_ptr for this iface

RETURN VALUE
 None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds707_data_session_set_current_profile
(
  ps_iface_type *iface_ptr
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INFO_INIT

DESCRIPTION   This function initializes the data_session_info type structure.

PARAMETERS    iface pointer 

DEPENDENCIES  None

RETURN VALUE   0 - on success
	              -1 - on failure

SIDE EFFECTS  None
===========================================================================*/
int8 ds707_data_session_info_init
(
  ps_iface_type    *iface_ptr
);

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_APP_PRIORITY()

DESCRIPTION
  This function will return the priority corresponding to the passed profile 
  index.

DEPENDENCIES
  None.

PARAMETERS
profile_id= index of the profile 

RETURN VALUE
  app_priority
  
SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds707_data_session_get_app_priority(int8 profile_id);

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
void ds707_data_session_init ( void );

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
int8 ds707_data_session_get_default_profile(void);

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_PROFILE_ID_FOR_SPECIAL_NAI

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
int8 ds707_data_session_get_profile_id_for_special_nai(void);

#endif /* FEATURE_DS_MULTIPLE_PROFILES */
#endif /* DS707_DATA_SESSION_PROFILE_H */
