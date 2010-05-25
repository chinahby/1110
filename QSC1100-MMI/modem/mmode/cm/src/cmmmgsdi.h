#ifndef CMMMGSDI_H
#define CMMMGSDI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 C A L L   M A N A G E R   MMGSDI   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager MMGSDI Interface block, which groups
  all the functionality that is a associated with CM<->MMGSDI interfacing.




EXTERNALIZED FUNCTIONS
  *cm_mmgsdi_client_id_ptr
  cmmmgsdi_ok_to_refresh
  cmmmgsdi_refresh_files_read_proc
  cmmmgsdi_read
  cmmmgsdi_read_preferred_plmn_list
  cmmmgsdi_write_preferred_plmn_list
  cmmmgsdi_refresh_init
  cmmmgsdi_validate_received_file_list
  cmmmgsdi_update_rat_mode
  cmmmgsdi_cmals_mmgsdi_async_sim_read
  cmmmgsdi_cmals_mmgsdi_async_sim_write
  cm_mmgsdi_in_traffic_channel_w_mode
  cmmmgsdi_card_status_cb
  cmmmgsdi_client_id_reg_cb
  cmmmgsdi_ini



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cm_init_after_task_start() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2008 - 2012 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmmmgsdi.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   rm      Changing signature of cmmmgsdi_rat_mode_init
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
11/25/08   am      Fixing compilation warnings
11/21/08   am      Fixing compilation error for Running off-target tests
11/17/08   am      Using MMGSDI API in CM.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#if ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI))
#include "comdef.h"
#include "mmgsdilib.h"
#include "cm.h"
#include "gsdi_exp.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_GSTK
#define CM_MMGSDI_NUM_REFRESH_FILES       6
#define CM_MMGSDI_NUM_REGISTRED_FILES     7
#endif /* #ifdef FEATURE_GSTK */

#define cyclic 1
#define transparent 2

#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#endif /*  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

 /* stores the cient ID provided by MMGSDI to CM */ 

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cm_mmgsdi_client_id_ptr

DESCRIPTION

  Return a pointer to cm_mmgsdi_client_id, which contains the client ID 
  provided by MMGSDI to CM.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to client ID provided by MMGSDI to CM.

SIDE EFFECTS
  none

===========================================================================*/
mmgsdi_client_id_type  *cm_mmgsdi_client_id_ptr( void );


#ifdef FEATURE_GSTK
/*===========================================================================

FUNCTION CMMMGSDI_OK_TO_REFRESH

DESCRIPTION
  This function called when Refresh Voting is needed. This function
  sends Refresh vote to MMGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_ok_to_refresh
(
  mmgsdi_slot_id_enum_type       slot,
  boolean                        ok_to_refresh_to_mmgsdi
);


#endif /* FEATURE_GSTK */

#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#endif /* #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

/*===========================================================================
FUNCTION CM_MMGSDI_IN_TRAFFIC_CHANNEL_W_MODE

DESCRIPTION
  Inform GSDI that call is connected/disconnected, so GSDI can start/stop
  in call polling.

  in_traffic is a boolean which indicates whether we are in traffic or not.
  mode is a indicates whether the call is GSM / UMTS or 1X.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  None
===========================================================================*/
void cm_mmgsdi_in_traffic_channel_w_mode(
  boolean in_traffic,
  sys_sys_mode_e_type mode);

/*===========================================================================
FUNCTION CMMMGSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function for MMGSDI generated event.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_card_status_cb(
  const mmgsdi_event_data_type *event
);

/*===========================================================================
FUNCTION CMMMGSDI_CLIENT_ID_REG_CB

DESCRIPTION
  Callback function from MMGSDI to acknowledge the CM registration for events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmmmgsdi_client_id_reg_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================
FUNCTION CMMMGSDI_INIT

DESCRIPTION
  Initilize MGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_init(void);

/*==============================================================================

FUNCTION NAME CMGSDI_IS_REFRESH_LIST_USED_BY_CM

DESCRIPTION
  Returns the list of SIM files 

RETURN VALUE

  TRUE   - Given list contains files that CM uses during a call
  FALSE  - Given list does NOT CONTAIN files that CM uses during a call.  

DEPENDENCIES
  None
==============================================================================*/


boolean cmmmgsdi_is_refresh_list_used_by_cm  
(

  const mmgsdi_file_enum_type  *file_list_p, 
    /* List of files */

  uint8                       file_list_len
    /* Length of the file list */

);
#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#endif

#endif /* ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */

#endif   /* CMMMGSDI_H */

