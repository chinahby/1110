
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   P E R S O N A L I Z A T I O N   E N G I N E


GENERAL DESCRIPTION

  This source file contains the GSDI PERSONALIZATION ENGINE which adheres to
  the personalization interface to do the personalization procedures.

                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_perso_engine.c#4 $$ $DateTime: 2009/04/01 10:26:45 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/09   tml     Enabled Perso procedure to continue at power up after
                   unblock perso is successfully
02/18/09   tml     Fixed memory leak
01/15/09   nb      Changed gsdi_fs_name_test() to gsdi_efs_name_test() and 
                   gsdi_fs_mkdir() to gsdi_efs_mkdir()
09/25/08   kk      Added support for extracting unblock dck retries available
09/04/08   sp      Process get_feature_data and get_feature_key requests in 
                   GSDI_PERSO_ENG_CARD_ERROR_S.
08/27/08   sp      Added support for get_dck_num_retries.
07/21/08   sp      Added debug messages.
06/13/08   KK      Fixed more compilation warnings
06/10/08   kk      Fixed compilation warnings
05/07/08   kk      Handling of set_feature_data when no sim
04/16/08   kk      Added support of get command on sim lock data
11/15/07   sun     Removed unnecessary check
08/01/07   sun     Allow activation/deactivation also when there is no card.
07/20/07   sun     Initialize Data before Sim Lock procedures are run
05/07/07   tml     Sent Perso Complete event as needed
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
03/26/07   sun     Fixed Buffer overflow
02/08/07   sun     Allocate enough memory for Autolock
01/29/07   sun     Checked for NULLS
01/17/07   pv      Save the last sent perso event to gsdi_data.perso_event
                   in gsdi_perso_engine_notify_clients_of_perso_event,
                   send the last saved event when a new registration comes in
                   irrespective of the current perso_engine_state.
                   Get rid of gsdi_perso_notify_clients_of_perso_event and
                   externalize gsdi_perso_engine_notify_clients_of_perso_event.
01/15/07   nk      Updated EFS Path depending if apps mount feature defined
10/31/06   sun     Fixed Warning
10/27/06   sun     Send the entire mask down to the security instead of just 
                   one feature.
10/23/06   sun     Send the entire mask down to the security instead of just 
                   one feature.
09/27/06   sun     Fixed Lint Error
09/13/06   wli     Fixed bugs when converting between masks and features
08/09/06   sun     Send last perso error event to the newly registered 
                   clients
                   In the Card Error State, allow the GET_FEATURE_IND req 
                   to be handled.
07/27/06   sun     Removed the return status for gsdi_perso_phone_init. The 
                   function will be able to write to NV only once.
                   Added new state for card error
05/08/06   nk      Featurized gsdi_perso_engine_init_phone_code call
05/04/06   sun     Fixed Length Conversion
04/21/06   tml     RPC compilation fix
04/28/06   sun     Initialize Phone code values at Perso Init
04/24/06   sun     Passed control keys to security lib for Disable Feature
                   Indicator
04/21/06   tml     Revert the PERSO_ENGINE_GET_SIM_ENCRYPTED DATA for loop
                   change
04/21/06   nk      Fixed GSDI_PERSO_ENGINE_GET_RUIM_ENCRYPTED_DATA for loop
04/06/06   sun     Added support for RUIM Lock
03/21/06   tml     lint
03/11/06   tml     return success if imei is not present in nv
03/10/06   jar     Lint Fixes.
02/01/06   tml     fixed refresh dck logic so that we won't try to deactivate
                   when dck ef is not found
01/25/06   sun     Write to FS only if the data was modified.
01/12/06   sp      Removed early return in gsdi_perso_engine_write_back_fs()
12/21/05   sun     Updated File system when counters are updated
11/22/05   jk      Changes to support multiprocessor RPC
11/17/05   sun     Added support for GSDI_PERSO_ENG_WAIT_FOR_DEPERSO_S
                   and GSDI_PERSO_ENG_INIT_ERROR_S
11/09/05   sun     Fixed lint errors
10/28/05   sun     Fixed Memory leak and return error if file is zero length
10/03/05   sun     Pass the event from the perso engine to the security and back
09/23/5    sun     Skip the length field from data buffer
09/22/05   sun     Copy the data from the file buffer to the sec data pointer
08/30/05   jar     3 Digit MNC Bug Fix
08/19/05   tml     Fixed compilation error
08/17/05   sun     Memset all Encrypted Data to 0 and fixed copy of data from
                   the perso file
07/20/05   sun     Added support with sfs
07/12/05   sun     Fixed Perso File Compatibility issues with Perso-Engine Files
06/13/05   sun     Removed architectural drawing
05/20/05   sun     Fixed return values
05/13/05   sun     Consolidated perso Engine files into one file
05/05/05   sun     Added support for Perso Refresh
05/03/05   sun     NON QC Library Fixes for Check All Locks
05/01/05   jar     Compilation fir for NON QC Security Libraries
04/30/05   jar     Fixed bad Debug Message.  Updated Analyze Indicators to
                   skip autolock if all states are inactive.  Various Logic
                   Fixes
01/05/05   sun     Initial Release


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gsdi_exp.h"
#include "gsdi_perso_engine.h"
#ifdef FEATURE_MMGSDI_PERSO_SFS
#include "sfs_private.h"
#else
#include "gsdi_efs.h"
#endif
#include "gsdi.h"
#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
#include "gsdi_perso_sec.h"
#else
#include "gsdi_perso_shim.h"
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
#ifdef FEATURE_PERSO_RUIM
#include "gsdi_perso_engine_ruim.h"
#endif /*FEATURE_PERSO_RUIM*/
#include "gsdi_nv.h"
#include "uim.h"
#include "tmc.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
#ifndef FEATURE_MMGSDI_LTK
#include "flash_otp.h"
#else
#include "mmgsdiltk.h"
#endif /* FEATURE_MMGSDI_LTK */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#include "intconv.h"

#include "mmgsdiutil.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Personalization Files */
#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
#ifndef FEATURE_MMGSDI_PERSO_SFS
static char *mmgsdi_efs_mmgsdi_dir  = "./apps/mmgsdi";
#endif /* FEATURE_MMGSDI_PERSO_SFS */
static char *mmgsdi_efs_perso_dir   = "./apps/mmgsdi/perso";
#else
#ifndef FEATURE_MMGSDI_PERSO_SFS
static char *mmgsdi_efs_mmgsdi_dir  = "mmgsdi";
#endif /* FEATURE_MMGSDI_PERSO_SFS */
static char *mmgsdi_efs_perso_dir   = "mmgsdi/perso";
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */
#ifdef FEATURE_PERSO_SIM
#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
static char *mmgsdi_efs_perso_file  = "/apps/mmgsdi/perso/perso.txt";
#else
static char *mmgsdi_efs_perso_file  = "mmgsdi/perso/perso.txt";
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */
#endif

/* ----------------------------------------------------------------------------
   Table to hold the list of event handlers
   --------------------------------------------------------------------------*/
gsdi_perso_events_cb gsdi_perso_event_handler_table[GSDI_PERSO_ENGINE_MAX_REG_CLIENTS];

#ifdef FEATURE_PERSO_SIM
/* --------------------------------------------------------------------------
   FUNCTION POINTER: SIM READ

   DESCRIPTION:      Will be used to set a pointer to for a read that has
                     the same prototype.
   ------------------------------------------------------------------------*/
static gsdi_returns_T (*sim_read)(
  gsdi_sim_read_req_T * req_p,
  byte *                rec_num_accessed_p,
  byte *                num_records_p,
  byte *                returned_data_len_p,
  byte *                data_p
);

/*===========================================================================
FUNCTION POINTER SIM_WRITE

DESCRIPTION
  This function pointer is used as the signature to a SIM Write.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/

static gsdi_returns_T (*sim_write)(
  gsdi_sim_write_req_T * req_p,
  byte *                returned_data_len_p
 );
#endif /*FEATURE_PERSO_SIM*/

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_NOTIFY_CLIENTS_OF_PERSO_EVENT

DESCRIPTION
  This function is used to notify the clients of any Personalization
  Events.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
void gsdi_perso_engine_notify_clients_of_perso_event(
  gsdi_perso_event_enum_type event,
  boolean                    additional_info_avail,
  uint32                     num_bytes,
  const uint8 *              data_p)
{
  uint32 index = 0x00;
  gsdi_perso_additional_info_type  additional_info;

  MSG_HIGH("PERSO: Notifying Perso Client 0x%x",event,0,0);


  /* Check to see if any client is registered to receive an event */
  if(gsdi_perso_event_handler_table[index] == NULL )
  {
    MSG_HIGH("PERSO: Don't have any client to send the event: 0x%x",event,0,0);
    return;
  }

  /* --------------------------------------------------------------------------
     Memset additional Information for the purpose of having all values set
     to NULL
     ------------------------------------------------------------------------*/

  memset(&additional_info, 0x00, sizeof(gsdi_perso_additional_info_type));

  /* --------------------------------------------------------------------------
     Perform usual parameter checking
     ------------------------------------------------------------------------*/
  if ( additional_info_avail )
  {
    if ( num_bytes > GSDI_MAX_DATA_BLOCK_LEN )
    {
      /* Reset the num_bytes to be no more that GSDI_MAX_DATA_BLOCK_LEN */
      num_bytes = GSDI_MAX_DATA_BLOCK_LEN;
    }
    if ( data_p == NULL )
    {
      /* Reset the additional_info_avail boolean */
      additional_info_avail = FALSE;
    }
  }

  /* --------------------------------------------------------------------------
     Build the additional information structure if it is still set to true
     ------------------------------------------------------------------------*/
  if ( additional_info_avail )
  {
    /* Get memory for the Data Pointer */
    additional_info.buffer_p = mmgsdi_malloc(num_bytes);
    if ( additional_info.buffer_p == NULL )
    {
      additional_info_avail = FALSE;
    }
    else
    {
      memcpy(additional_info.buffer_p,
             data_p,
             (uint32)num_bytes);
      additional_info.num_bytes = (int32)num_bytes;
    }
  }

  /* --------------------------------------------------------------------------
     Save the event that we are about to send so that if 
     any new registrations come in, we could re-send this last sent event.
     ------------------------------------------------------------------------*/
  gsdi_data.perso_event = event;

  while ( gsdi_perso_event_handler_table[index] != NULL )
  {
    (gsdi_perso_event_handler_table[index])(event,
                                            GSDI_SLOT_1,
                                            additional_info_avail,
                                            &additional_info);
    index++;
    if ( index >= GSDI_PERSO_ENGINE_MAX_REG_CLIENTS )
      break;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(additional_info.buffer_p);

} /* gsdi_perso_engine_notify_clients_of_perso_event */


/* ==========================================================================

FUNCTION GSDI_PERSO_ENGINE_SET_STATE

DESCRIPTION
  This function is used to set the next state.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static void  gsdi_perso_engine_set_state(
  gsdi_perso_eng_init_state_enum_type new_state,
  gsdi_perso_eng_init_state_enum_type curr_state,
  gsdi_perso_eng_init_state_enum_type *set_state)

{

  switch(new_state)
  {
    case GSDI_PERSO_ENG_INIT_S:
#if defined(FEATURE_PERSO_SIM)
     *set_state = GSDI_PERSO_ENG_INIT_S;
#else
     *set_state = GSDI_PERSO_ENG_CDMA_INIT_S;
#endif /*FEATURE_PERSO_SIM*/
      break;

    case GSDI_PERSO_ENG_INIT_ERROR_S:
    default:
      switch(curr_state)
      {
        case GSDI_PERSO_ENG_CDMA_INIT_CHECK_LOCKS_S:
        case GSDI_PERSO_ENG_CDMA_INIT_S:
        case GSDI_PERSO_ENG_CDMA_INIT_AUTOLOCK_S:
        case GSDI_PERSO_ENG_CDMA_WAIT_FOR_DEPERSO_S:
        case GSDI_PERSO_ENG_CDMA_INIT_ERROR_S:
          *set_state = GSDI_PERSO_ENG_CDMA_INIT_ERROR_S;
          break;

        default:
          *set_state = new_state;
          break;
      }
      break;
  }
}

/* ==========================================================================

FUNCTION GSDI_PERSO_ENGINE_SEND_EVENT

DESCRIPTION
  This function is used to send the event and set the Next State Pointer.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static void gsdi_perso_engine_send_event(
  gsdi_perso_eng_init_state_enum_type * next_state_p,
  gsdi_perso_event_enum_type            event
)
{
  /* --------------------------------------------------------------------------
     Validate the Input Parameters
     ------------------------------------------------------------------------*/
  if(NULL == next_state_p)
  {
      MSG_ERROR("PERSO: NULL POINTER next_state_p gsdi_perso_engine_send_event",0,0,0);
      return;
  }

  switch ( event )
  {
    case GSDI_PERSO_NO_EVENT:
      return;
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_NW_FAILURE:
    case GSDI_PERSO_NS_FAILURE:
    case GSDI_PERSO_SP_FAILURE:
    case GSDI_PERSO_CP_FAILURE:
    case GSDI_PERSO_SIM_FAILURE:
      *next_state_p = GSDI_PERSO_ENG_WAIT_FOR_DEPERSO_S;
      break;
    case GSDI_PERSO_PROP1_FAILURE:
    case GSDI_PERSO_GEN_PROP1:
    case GSDI_PERSO_GEN_PROP2:
      *next_state_p = GSDI_PERSO_ENG_WAIT_FOR_DEPERSO_S;
      break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_RUIM_NW1_FAILURE:
    case GSDI_PERSO_RUIM_NW2_FAILURE:
    case GSDI_PERSO_RUIM_HRPD_FAILURE:
    case GSDI_PERSO_RUIM_SP_FAILURE:
    case GSDI_PERSO_RUIM_CP_FAILURE:
    case GSDI_PERSO_RUIM_RUIM_FAILURE:
      *next_state_p = GSDI_PERSO_ENG_CDMA_WAIT_FOR_DEPERSO_S;
      break;
#endif /* FEATURE_PERSO_RUIM */

#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_NW_DEACTIVATED:
    case GSDI_PERSO_NS_DEACTIVATED:
    case GSDI_PERSO_SP_DEACTIVATED:
    case GSDI_PERSO_CP_DEACTIVATED:
    case GSDI_PERSO_SIM_DEACTIVATED:
    case GSDI_PERSO_NCK_UNBLOCKED:
    case GSDI_PERSO_NSK_UNBLOCKED:
    case GSDI_PERSO_SPK_UNBLOCKED:
    case GSDI_PERSO_CCK_UNBLOCKED:
    case GSDI_PERSO_PPK_UNBLOCKED:
    case GSDI_PERSO_PROP1_DEACTIVATED:
    case GSDI_PERSO_PROP1_UNBLOCKED:
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_RUIM_NW1_DEACTIVATED:
    case GSDI_PERSO_RUIM_NW2_DEACTIVATED:
    case GSDI_PERSO_RUIM_HRPD_DEACTIVATED:
    case GSDI_PERSO_RUIM_SP_DEACTIVATED:
    case GSDI_PERSO_RUIM_CP_DEACTIVATED:
    case GSDI_PERSO_RUIM_RUIM_DEACTIVATED:
    case GSDI_PERSO_RUIM_NCK1_UNBLOCKED:
    case GSDI_PERSO_RUIM_NCK2_UNBLOCKED:
    case GSDI_PERSO_RUIM_HNCK_UNBLOCKED:
    case GSDI_PERSO_RUIM_SPCK_UNBLOCKED:
    case GSDI_PERSO_RUIM_CCK_UNBLOCKED:
    case GSDI_PERSO_RUIM_PCK_UNBLOCKED:
#endif /* FEATURE_PERSO_RUIM */
     /* Do not change to GSDI_PERSO_ENG_INIT_S when we are
      ** in the GSDI_PERSO_ENG_INIT_COMPLETED_S to allow
      ** for the setup and chaning of SIMs without locking
      ** to the SIM already inserted prior to a power down
      */
      if (*next_state_p != GSDI_PERSO_ENG_INIT_COMPLETED_S )
      {
        gsdi_perso_engine_set_state(GSDI_PERSO_ENG_INIT_S,
                                    gsdi_data.perso_engine_state,
                                    &gsdi_data.perso_engine_state);

      }
      break;

#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_NCK_BLOCKED:
    case GSDI_PERSO_NSK_BLOCKED:
    case GSDI_PERSO_SPK_BLOCKED:
    case GSDI_PERSO_CCK_BLOCKED:
    case GSDI_PERSO_PPK_BLOCKED:
    case GSDI_PERSO_PROP1_BLOCKED:
#endif /* FEATURE_PERSO_SIM */
    case GSDI_PERSO_SANITY_ERROR:
      *next_state_p = GSDI_PERSO_ENG_INIT_ERROR_S;
      break;
#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_RUIM_NCK1_BLOCKED:
    case GSDI_PERSO_RUIM_NCK2_BLOCKED:
    case GSDI_PERSO_RUIM_HNCK_BLOCKED:
    case GSDI_PERSO_RUIM_SPCK_BLOCKED:
    case GSDI_PERSO_RUIM_CCK_BLOCKED:
    case GSDI_PERSO_RUIM_PCK_BLOCKED:
      *next_state_p = GSDI_PERSO_ENG_CDMA_INIT_ERROR_S;
      break;
#endif /* FEATURE_PERSO_RUIM */
    default:
      break;
  }

  gsdi_perso_engine_notify_clients_of_perso_event(event,
                                                  FALSE,
                                                  0,
                                                  NULL);
}

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_SEND_SANITY_ERROR

DESCRIPTION
  This function is used to send the Sanity Error to all clients as well as set
  the state to GSDI_PERSO_ENG_ERROR_S;

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static void gsdi_perso_engine_send_sanity_error (
  gsdi_perso_eng_init_state_enum_type * next_state_p)
{
  /* --------------------------------------------------------------------------
     Check the parameters for the index and event handler to be stored
     ------------------------------------------------------------------------*/
  if ( next_state_p == NULL )
  {
    MSG_ERROR("PERSO: NULL pointer for next_state_p in gsdi_perso_engine_send_sanity_error",0,0,0);
    MSG_ERROR("PERSO: Notifying clients of SANITY ERROR",0,0,0);
    return;
  }

  *next_state_p =  GSDI_PERSO_ENG_INIT_ERROR_S;
  gsdi_perso_engine_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                           FALSE,
                                           0,
                                           NULL);

} /* gsdi_perso_engine_send_sanity_error */


/*===========================================================================
FUNCTION GSDI_PERSO_SAVE_EVENT_HANDLER

DESCRIPTION
  This function is used to save the event handler provided by the client
  into the perso events event handling table to be called at a later time
  when a personalization event needs to be generated.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_save_event_handler (
  int32                index,
  gsdi_perso_events_cb event_handler_p
)
{
  /* --------------------------------------------------------------------------
     Check the parameters for the index and event handler to be stored
     ------------------------------------------------------------------------*/
  if ( index >= GSDI_PERSO_ENGINE_MAX_REG_CLIENTS ||
       index < GSDI_PERSO_ENGINE_MIN_TABLE_INDEX )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: BAD Index : 0x%x ",
      index,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( event_handler_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: event_handler_p == NULL",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  gsdi_perso_event_handler_table[index] = event_handler_p;
  return GSDI_SUCCESS;

} /* gsdi_perso_save_event_handler */

/*===========================================================================
FUNCTION GSDI_PERSO_GET_FREE_EVENT_HANDLER_INDEX

DESCRIPTION
  This function is used to search the gsdi_perso_event_handler_table global
  for a free index.

  This function will also validate the current event handler pointer against
  all event handlers currently stored to search for duplicate event handlers.

DEPENDENCIES
  None

RETURN VALUE
  int32
===========================================================================*/
static  int32 gsdi_perso_engine_get_free_event_handler_index(
  gsdi_perso_events_cb event_handler_p
)
{
  int32 table_index = 0;
  while ( table_index < GSDI_PERSO_ENGINE_MAX_REG_CLIENTS )
  {
    if ( gsdi_perso_event_handler_table[table_index] == event_handler_p )
    {
      /* Client's Event Handler is already registered for Perso
      ** Events
      */
      MSG_HIGH("PERSO: Client Callback already registered: 0x%x",event_handler_p,0,0);
      return GSDI_PERSO_ENGINE_EVENT_HANDLER_ALREADY_REGISTERD;
    }
    else if ( gsdi_perso_event_handler_table[table_index] == NULL )
    {
      /* An empty slot in the Event Handler Table was Found */
      return table_index;
    }
    else
    {
      table_index++;
    }
  }
  return GSDI_PERSO_ENGINE_NO_EVENT_HANDLER_TABLE_INDEX_AVAILABLE;

} /* gsdi_perso_get_free_event_handler_index */


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_ENGINE_REGISTER_TASK

DESCRIPTION
  This function is used to process the Personalization Request from a client
  to register for Personalization Events.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_INCORRECT_PARMS: Bad input Parameters
                   GSDI_ERROR          : Event Handler Table Full
                   GSDI_SUCCESS        : Successfully saved event handler
                                         - or -
                                         Duplicate Event Handler Detected
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_engine_reg_task (
  const gsdi_perso_reg_task_req_T * req_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  int32          index       = 0x00;
  /* -----------------------------------------------------------------------
     Check the Request Parameters to ensure no corruption during
     the context switching
     ---------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL Request Pointer",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Due to not define FEATURE_MMGSDI_DUAL_SLOT for Dual SIM Target
     builds, the GSDI will default the slot to slot 1 for any task
     that inadvertently leaves this blank
     ------------------------------------------------------------------------*/
  if ( req_p->message_header.slot != GSDI_SLOT_NONE &&
       req_p->message_header.slot != GSDI_SLOT_1 )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: BAD SLOT ID: 0x%x",req_p->message_header.slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( req_p->reg_task_cb_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: null reg_task_cb_p 0x%x",req_p->reg_task_cb_p,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Search the Table which stores event handlers for a free index
     to store the current Event Handler.
     The function will search for duplicate event handlers
     ------------------------------------------------------------------------*/
  index = gsdi_perso_engine_get_free_event_handler_index(req_p->reg_task_cb_p);

 if ( index == GSDI_PERSO_ENGINE_NO_EVENT_HANDLER_TABLE_INDEX_AVAILABLE )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Perso Event Handler Table Full",0,0,0);
    return GSDI_ERROR;
  }
  else if ( index == GSDI_PERSO_ENGINE_EVENT_HANDLER_ALREADY_REGISTERD )
  {
    MSG_HIGH("PERSO: Perso Event Handler already registered",0,0,0);
    return GSDI_SUCCESS;
  }
  else if ( index > GSDI_PERSO_ENGINE_MAX_REG_CLIENTS )
  {
    MMGSDI_DEBUG_ERR("PERSO: Invalid Index received: 0x%x",index,0,0);
    return GSDI_ERROR;
  }
  else
  {
    /* Index is Valid store the Index Received */
    gsdi_status = gsdi_perso_engine_save_event_handler(index,
                                                req_p->reg_task_cb_p);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_ERR("PERSO: Error can't save to Event Handler Table: 0x%x",gsdi_status,0,0);
    }
    else /*Send the Event back to the newly registered task*/
    {
      if(gsdi_data.perso_event        != GSDI_PERSO_NO_EVENT &&
         index                        <  GSDI_PERSO_ENGINE_MAX_REG_CLIENTS)
      {
        gsdi_perso_additional_info_type  additional_info;
        /* --------------------------------------------------------------------------
           Memset additional Information for the purpose of having all values set
           to NULL
           ------------------------------------------------------------------------*/
        memset(&additional_info, 0x00, sizeof(gsdi_perso_additional_info_type));

        (gsdi_perso_event_handler_table[index])(gsdi_data.perso_event,
                                                GSDI_SLOT_1,
                                                FALSE,
                                                &additional_info);
      }
    }
  }
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_INITIALIZE_DATA

DESCRIPTION
  This function is called after the Verification procedures have occurred on
  PIN1.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_initialize_data(
  gsdi_slot_id_type slot
)
{
  uint8          card_apps   = 0;
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

#ifdef FEATURE_PERSO_RUIM
  /* --------------------------------------------------------------------------
     If there is a CDMA Application, run the CDMA get Application.
     ------------------------------------------------------------------------*/
  card_apps = gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK;
  if(card_apps)
  {
    gsdi_status = gsdi_cdma_proc_get_ruim_capabilities(slot);
  }
#endif /* FEATURE_PERSO_RUIM */


#ifdef FEATURE_PERSO_SIM
  #ifdef FEATURE_MMGSDI_UMTS
#error code not present
  #endif /* FEATURE_MMGSDI_UMTS */

  #ifdef FEATURE_MMGSDI_GSM
#error code not present
  #endif /* FEATURE_MMGSDI_GSM */
#endif /* FEATURE_PERSO_SIM */

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: GET Capabilities Failures 0x%x",gsdi_status,0,0);
    (void)gsdi_util_send_mmi_sim_ind(GSDI_CARD_ERROR);
  }

  return gsdi_status;
}

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION GSDI_PERSNALIZATION_PROCS

DESCRIPTION
  This function will perform the required init procedures.  It will
  intiialization the Flash SIM Partion if available.  It will also
  initialize the Secureity Library if such a procedure is required.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_main_init(
  gsdi_slot_id_type slot,
  const boolean   * init_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  /* --------------------------------------------------------------------------
     Run the File System Initialization procedures
     ------------------------------------------------------------------------*/
  if(slot == GSDI_SLOT_NONE)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Incorrect slot: 0x%x",slot,0,0);
    return GSDI_ERROR;
  }

  if(init_p == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR(PERSO: "NULL Pointer in gsdi_perso_engine_main_init",0,0,0);
    return GSDI_ERROR;
  }


  /* --------------------------------------------------------------------------
     Initialize the SIM Partion on the Flash Device
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_simsecure_flash_init(init_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: EFS Sim secure Flash Init Failure 0x%x",
                           gsdi_status,0,0);
    return gsdi_status;
  }

  /* ------------------------------------------------------------------------
     Initialize the Security Library
     ----------------------------------------------------------------------*/
  gsdi_perso_shim_init_lib();

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_main_init */
  #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

/* ---------------------------------------------------------------------------
FUNCTION:  GSDI_PERSO_INITIALIZE_DATA

DESCRIPTION:
  Will initialize the MMGSDI Perisonalization engine with the data required
  to start the Personalization Intialization Procedures.

DEPENDENCIES:
  Function must be called only after PIN1 has been verified.

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_perso_engine_enum_type

-----------------------------------------------------------------------------*/
gsdi_returns_T gsdi_perso_initialize_data(
  gsdi_slot_id_type slot
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif
  /* --------------------------------------------------------------------------
     Verify the slot ID is GSDI_SLOT_1 or GSDI_SLOT_2
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_OUT_OF_RANGE(slot,GSDI_SLOT_1,GSDI_SLOT_2);

#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif /*(FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)*/
  return gsdi_status;
}

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_IMEI

DESCRIPTION:
  Will retrieve the IMEI number from NV.

DEPENDENCIES:
  None

LIMITATIONS:
  The memory allocated in this function will need to be released by the
  caller of this function.

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
gsdi_returns_T gsdi_perso_engine_get_imei (
  boolean parse,
  uint8 * imei_data_p,
  uint32 * imei_len_p
)
{
  nv_item_type       gsdi_nv_data_buf;             /* NV data buffer */
  nv_stat_enum_type  nv_status         = NV_DONE_S;
  uint8              dec_index         = 0;
  uint8              bcd_index         = 0;

  /* ------------------------------------------------------------------------
    Is the IMEI Provided a valid IMEI
    ----------------------------------------------------------------------*/
  nv_status = gsdi_get_nv(NV_UE_IMEI_I,
                          &gsdi_nv_data_buf);

  if ( nv_status != NV_DONE_S )
  {
    *imei_len_p = 0;

    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not get IMEI",0,0,0);
    return GSDI_SUCCESS;
  }

  if ( imei_data_p == NULL )
  {
    *imei_len_p = 0;

    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not get IMEI",0,0,0);
    return GSDI_SUCCESS;
  }

  if ( gsdi_nv_data_buf.ue_imei.ue_imei[0] == 0x00 ||
       gsdi_nv_data_buf.ue_imei.ue_imei[0] == 0xFF ||
       gsdi_nv_data_buf.ue_imei.ue_imei[0] > GSDI_PERSO_MAX_IMEI_LEN )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: IMEI is not set or improperly set",0,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Check the Parsing function to determine whether or not we need to use
     the parsed IMEI or the non parsed IMEI
     ------------------------------------------------------------------------*/
  if ( !parse )
  {
    int i = 0;
    while ( i < GSDI_PERSO_ENGINE_IMEI_UNPARSED_NV_LEN )
    {
      imei_data_p[i] = gsdi_nv_data_buf.ue_imei.ue_imei[i];
      i++;
    }
    *imei_len_p = GSDI_PERSO_ENGINE_IMEI_UNPARSED_NV_LEN;
    return GSDI_SUCCESS;
  }

  bcd_index++;
  /* --------------------------------------------------------------------------
     Convert the IMEI from BCD To Decimal
     ------------------------------------------------------------------------*/
  while ( bcd_index <= gsdi_nv_data_buf.ue_imei.ue_imei[0])
  {
    /* Place High Byte in Index 0 (Even) Low Byte in Index 1 (Odd) ... */
    if ( dec_index % 2 == 0 )
    {
      imei_data_p[dec_index] = 0x30 +
            (((gsdi_nv_data_buf.ue_imei.ue_imei[bcd_index] &
               GSDI_MASK_FOR_HIGH_BYTE) >> 0x04) & 0x0F);
      dec_index++;
      bcd_index++;
    }
    else
    {
      /* Get the Low Byte of BCD  0x0N */
      imei_data_p[dec_index] = 0x30 +
        (gsdi_nv_data_buf.ue_imei.ue_imei[bcd_index] & GSDI_MASK_FOR_LOW_BYTE);
      dec_index++;
    }
  }
  *imei_len_p = dec_index;

  return GSDI_SUCCESS;
  /*lint +esym(613,imei_len_p)*/
}

#ifdef FEATURE_PERSO_SIM
/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_ME_ESN

DESCRIPTION:
  Will gather all of the information required to activate the feature
  indication provided.

  Information gather is as follows:

  1)  Secured / Encrypted Data - Personalization Keys
  2)  Secured / Encrypted Data - Feature Indicators
  3)  Secured / Encrypted Data - Depersonalization Key Status
  4)  Get the IMEI Value
  5)  Get the Mobile Serial Number

DEPENDENCIES:
  Based on the Personalization State and Self Personalization State.
  gsdi_perso_self_perso_state_enum_type
  gsdi_perso_state_enum_type

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_me_esn(
  uint8 * esn_buffer_p,
  uint32 * esn_buffer_len
)
{
  /* --------------------------------------------------------------------------
     Validate the pointers provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(esn_buffer_p);
  MMGSDI_RETURN_IF_NULL(esn_buffer_len);

  /* --------------------------------------------------------------------------
     Populate the memory with the Serial Number
     and set the Length for the caller.
     ------------------------------------------------------------------------*/
  memset(esn_buffer_p, 0x30, GSDI_PERSO_ENGINE_SERIAL_NUMBER_LEN);
  *esn_buffer_len = GSDI_PERSO_ENGINE_SERIAL_NUMBER_LEN;

  return GSDI_SUCCESS;
}


/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_ENCRYPTED_DATA

DESCRIPTION:
  Will gather all of the information required to activate the feature
  indication provided.

  Information gather is as follows:

  1)  Secured / Encrypted Data - Personalization Keys
  2)  Secured / Encrypted Data - Feature Indicators
  3)  Secured / Encrypted Data - Depersonalization Key Status
  4)  Get the IMEI Value
  5)  Get the Mobile Serial Number

DEPENDENCIES:
  Based on the Personalization State and Self Personalization State.
  gsdi_perso_self_perso_state_enum_type
  gsdi_perso_state_enum_type

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_encrypted_data(
  gsdi_perso_sec_me_data_type        * sec_data_p,
  gsdi_perso_engine_mem_pointer_type * mem_pointer_p,
  gsdi_perso_engine_proc_enum_type     feature_type,
  const void *                         req_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  uint32 file_size =0;
  uint8 *file_buffer_p = NULL;
  uint8 *orig_file_buffer_p = NULL;
  int i;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

  /* --------------------------------------------------------------------------
     Perform Paramter checking
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(sec_data_p);
  MMGSDI_RETURN_IF_NULL(mem_pointer_p);

  memset(&sec_data_p->feat_inds,         0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->perm_feat_inds,    0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->sim_lock_codes,    0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->dcks,              0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->dck_counts,        0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->unblock_cks,       0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->unblock_ck_counts, 0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->me_imei,           0x00, sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->me_sn,             0x00, sizeof(gsdi_perso_sec_data_type));

#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  /* --------------------------------------------------------------------------
     Using the Secure FS Interface, get the Perso Data.
     ------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_PERSO_SFS
   file_buffer_p = mmgsdi_malloc(1000);
   MMGSDI_RETURN_IF_NULL(file_buffer_p);
   file_size = sfs_priv_simloc_read((uint8*)file_buffer_p, 1000);
   if (file_size   == 0 )
#else
   gsdi_status = gsdi_fs_get_file_size(mmgsdi_efs_perso_file,(int32*)&file_size);

  if ( gsdi_status != GSDI_SUCCESS ||
       file_size == 0 )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: FILE Status: 0x%x File Size: 0x%x",
                           gsdi_status,file_size,0);
    return GSDI_ERROR;
  }

  file_buffer_p = mmgsdi_malloc(file_size);
  MMGSDI_RETURN_IF_NULL(file_buffer_p);

  gsdi_status =  gsdi_efs_read_file(mmgsdi_efs_perso_file,
                                    (int32*)&file_size,
                                     file_buffer_p);

  if ( gsdi_status != GSDI_SUCCESS ||
       file_size   == 0 )
#endif
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: FS Status: 0x%x Read Size 0x%x",
                           gsdi_status,file_size,0);

    MMGSDIUTIL_TMC_MEM_FREE(file_buffer_p);
    return gsdi_status;
  }

  orig_file_buffer_p = file_buffer_p;
  for(i=0;i<GSDI_PERSO_ENGINE_NUMBER_OF_TAGS;i++)
  {
    /*Switch on the Tags*/
    switch(*file_buffer_p)
  {
      case GSDI_PERSO_ENGINE_TAG_FEATURE_INDS:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->feat_inds.buffer_p  = mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->feat_inds.len      =  (*file_buffer_p) *255 + *(file_buffer_p +1);
        memcpy(sec_data_p->feat_inds.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->feat_inds.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->feat_inds.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->feat_inds.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_PERM_FEATURE_INDS:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->perm_feat_inds.buffer_p = mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->perm_feat_inds.len      =  (*file_buffer_p)*255 + *(file_buffer_p +1);
        memcpy(sec_data_p->perm_feat_inds.buffer_p, file_buffer_p +GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->perm_feat_inds.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->perm_feat_inds.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->perm_feat_inds.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_SIM_LOCK_CODE:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->sim_lock_codes.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->sim_lock_codes.len  =  (*file_buffer_p)*255 + *(file_buffer_p +1);
        memcpy(sec_data_p->sim_lock_codes.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->sim_lock_codes.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->sim_lock_codes.len;

        /* Check to see if new SIM Lock Codes have to be generated*/
        if(feature_type == GSDI_PERSO_ENGINE_AUTO_LOCK)
        {
          /* allocate Enough memory for any additional codes that may be required to be genereated*/
           mem_pointer_p->memory_curr += GSDI_PERSO_BUFFER_LEN -
                                              sec_data_p->sim_lock_codes.len;
        }
        else if(feature_type == GSDI_PERSO_ENGINE_SET_DATA_FEATURE)
        {
          MMGSDI_RETURN_IF_NULL(req_p);
          mem_pointer_p->memory_curr += ((gsdi_perso_set_feature_data_req_T *)req_p)->num_bytes *2;
        }
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->sim_lock_codes.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_DEPERSO_CK:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->dcks.buffer_p =  mem_pointer_p->memory_curr  + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->dcks.len      =  (*file_buffer_p)*255 + *(file_buffer_p +1);
        memcpy(sec_data_p->dcks.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->dcks.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dcks.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dcks.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_DCK_COUNTER:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->dck_counts.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->dck_counts.len      =  (*file_buffer_p)*255 + *(file_buffer_p +1);
        memcpy(sec_data_p->dck_counts.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->dck_counts.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dck_counts.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dck_counts.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_UNBLOCK:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->unblock_cks.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->unblock_cks.len      =  (*file_buffer_p)*255 +*(file_buffer_p +1);
        memcpy(sec_data_p->unblock_cks.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->unblock_cks.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_cks.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_cks.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_UNBLOCK_COUNTER:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->unblock_ck_counts.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->unblock_ck_counts.len      =  (*file_buffer_p)*255 + *(file_buffer_p +1);
        memcpy(sec_data_p->unblock_ck_counts.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->unblock_ck_counts.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_ck_counts.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_ck_counts.len;
        break;
      default:
        MSG_ERROR("PERSO: Invalid Tag",0,0,0);
        return GSDI_ERROR;
    }


  /* --------------------------------------------------------------------------
     The operation was successfull.
     Need to move the current Pointer over so that the data it points to isn't
     accidentally overwritten.  Ensure memory_curr does not exceed
     memory_end.
     ------------------------------------------------------------------------*/
    if ( ( mem_pointer_p->memory_curr ) > mem_pointer_p->memory_end )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY POINTER USAGE ERROR",0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(orig_file_buffer_p);
      return GSDI_ERROR;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(orig_file_buffer_p);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QCT */

  /* --------------------------------------------------------------------------
     Using the NV Items Get IMEI Value
     ------------------------------------------------------------------------*/
  sec_data_p->me_imei.buffer_p = mem_pointer_p->memory_curr;

  gsdi_status = gsdi_perso_engine_get_imei(TRUE,
                       sec_data_p->me_imei.buffer_p,
                                           (uint32 *)&sec_data_p->me_imei.len);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: PERSO ENGINE Get IMEI Failure: 0x%x",
                           gsdi_status,0,0);
    return gsdi_status;
  }
  /* --------------------------------------------------------------------------
     The operation was successfull.
     Need to move the current Pointer over so that the data it points to isn't
     accidentally overwritten.  Ensure memory_curr does not exceed
     memory_end.
     ------------------------------------------------------------------------*/
  if ( ( mem_pointer_p->memory_curr + sec_data_p->me_imei.len ) >
         mem_pointer_p->memory_end )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY POINTER USAGE ERROR",0,0,0);
    return GSDI_ERROR;
  }

  mem_pointer_p->memory_curr += sec_data_p->me_imei.len;

  /* --------------------------------------------------------------------------
     Retrieve the serial number of the Mobile Equipment
     ------------------------------------------------------------------------*/
  sec_data_p->me_sn.buffer_p = mem_pointer_p->memory_curr;

  gsdi_status = gsdi_perso_engine_get_me_esn(sec_data_p->me_sn.buffer_p,
                                             (uint32 *)&sec_data_p->me_sn.len);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: PERSO ENGINE Get IMEI Failure: 0x%x",
                           gsdi_status,0,0);

    return gsdi_status;
  }

  mem_pointer_p->memory_curr += sec_data_p->me_sn.len;



  return gsdi_status;

}

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_READ_SIM

DESCRIPTION
  This is the generic wrapper function used to read data from the SIM.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_read_sim(
  gsdi_slot_id_type                      slot,
  uim_items_enum_type                    file_name,
  void *                                 data_p
)
{
  gsdi_returns_T          gsdi_status     = GSDI_ERROR;
  gsdi_sim_read_req_T *   read_req_p      = NULL;
  byte                    dummy           = 0;
  gsdi_perso_sec_sim_ele_data_type *     sim_data_p
      = (gsdi_perso_sec_sim_ele_data_type *)data_p;

  /* ------------------------------------------------------------------------
     Validate the parameters and perform basic initialization
     ----------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(sim_data_p);

  sim_data_p->len = 0;

  /* --------------------------------------------------------------------------
     Allocate memory for the Read Request.
     Error out of the function if memory can not be allocated.
     ------------------------------------------------------------------------*/
  read_req_p = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));
  MMGSDI_RETURN_IF_NULL(read_req_p);

  memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));

  /* ----------------------------------------------------------------
     Build the Read Request according to
     the file provided.
     --------------------------------------------------------------*/
  read_req_p->message_header.slot = GSDI_SLOT_1;
  read_req_p->sim_filename        = (int32)file_name;
  read_req_p->index_mode          = GSDI_NO_INDEX;
  read_req_p->search_string_len   = 0x00;
  read_req_p->data_offset         = 0x00;
  read_req_p->rec_num             = 0x00;

  MMGSDI_GET_FUNCTION_PTR(sim_read,
                          gsdi_umts_proc_usim_read,
                          gsdi_gsm_proc_sim_read);

  gsdi_status = (*sim_read)( read_req_p,
                             &dummy,
                             &dummy,
                             (byte *)&sim_data_p->len,
                             (byte *)sim_data_p->buffer_p);

  /* --------------------------------------------------------------------------
     Free up the Request Data
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(read_req_p);

  return gsdi_status;
  /*lint -esym(715,slot)*/
}

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_WRITE_DCK

DESCRIPTION
  This is the generic wrapper function used to write data from the SIM.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_write_dck(void)
{
  gsdi_sim_write_req_T * write_req_p    = NULL;
  gsdi_returns_T         gsdi_status     = GSDI_ERROR;
  int32                  returned_data_len  = 0;
   uim_items_enum_type   file_name          = UIM_NO_SUCH_ITEM;

  /* --------------------------------------------------------------------------
     Allocate Memory for the Write Request and set the parameters to prepare it
     for the read and verify loop.
  ------------------------------------------------------------------------*/
  write_req_p = mmgsdi_malloc(sizeof(gsdi_sim_write_req_T));
  if ( write_req_p == NULL )
  {
    ERR_FATAL("PERSO: Could not allocate memory for the write Request",0,0,0);
  }
  memset(write_req_p,0x00,sizeof(gsdi_sim_write_req_T));
  /* --------------------------------------------------------------------------
     Build the read request based on the information provided in the
     call.  We'll let the SIM handle any requests with record numbers out of
     bounds with its structure.
     ------------------------------------------------------------------------*/
  write_req_p->message_header.slot = GSDI_SLOT_1;
  write_req_p->index_mode          = (int32)GSDI_NO_INDEX;
  write_req_p->rec_num             = 0;
  write_req_p->search_string_len   = 0;
  write_req_p->data_len            = (byte)GSDI_PERSO_ENGINE_DCK_CK_LEN * 4;
  write_req_p->data_offset         = 0;

  memset(write_req_p->data_buffer,0xFF, GSDI_MAX_DATA_BLOCK_LEN);

  /* --------------------------------------------------------------------------
     Determine which file should be written and write to  it
     ------------------------------------------------------------------------*/
  MMGSDI_SELECT_FILE(file_name,UIM_USIM_DCK, UIM_GSM_DCK);

  write_req_p->sim_filename = (int32)file_name;

  MMGSDI_GET_FUNCTION_PTR(sim_write,
                          gsdi_umts_proc_usim_write,
                          gsdi_gsm_proc_sim_write);

  gsdi_status = (*sim_write)( write_req_p,
                             (byte *)&returned_data_len);

  if ( gsdi_status != GSDI_SUCCESS )
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not clear EF-DCK ",0,0,0);

  MMGSDIUTIL_TMC_MEM_FREE(write_req_p);

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_READ_AND_FORMAT_IMSI

DESCRIPTION
  This function will be called to read the IMSI and convert it to a usable
  format.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_read_and_format_imsi(
  gsdi_slot_id_type                    slot,
  gsdi_perso_sec_sim_ele_data_type     *imsi
)
{
  gsdi_returns_T        gsdi_status       = GSDI_SUCCESS;
  byte                  digit1            = 0;
  byte                  digit2            = 0;
  uint8                 num_mnc_digits    = 0;
  uim_items_enum_type   file_name         = UIM_NO_SUCH_ITEM;
  gsdi_perso_sec_sim_ele_data_type ad_buf;
  uint8 imsi_buffer[8];

  /* --------------------------------------------------------------------------
     Read the Administrative Data Elementary File
     ------------------------------------------------------------------------*/
  MMGSDI_SELECT_FILE(file_name,UIM_USIM_AD, UIM_GSM_AD);

  ad_buf.buffer_p = mmgsdi_malloc(256);

  gsdi_status = gsdi_perso_engine_read_sim(slot,
                                           file_name,
                                           (void*)&ad_buf);


  if ( gsdi_status != GSDI_SUCCESS &&
         gsdi_status != GSDI_NOT_SUPPORTED )
  {
    MMGSDIUTIL_TMC_MEM_FREE(ad_buf.buffer_p);
    MSG_ERROR("PERSO: gsdi_perso_engine_read_sim failed or not supported 0x%x",
               gsdi_status,0,0);
    return gsdi_status;
  }

  if ( gsdi_status == GSDI_SUCCESS )
  {
    num_mnc_digits = ad_buf.buffer_p[3];
    if ( (num_mnc_digits != 0x02 ) &&
         (num_mnc_digits != 0x03 ) )
    {
      num_mnc_digits = 0x02;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(ad_buf.buffer_p);
  MMGSDI_RETURN_IF_NULL(imsi);
  /* --------------------------------------------------------------------------
     Now format the IMSI from:
     B1       B2       B3       B4     B5   B6   B7    B8
     MCC1PB   MCC3MCC2 MNC2MNC1 D1MNC3 D3D2 D5D4 D7D6 D9D8

     TO (2 Digit MNC)
     B1       B2       B3       B4    B5   B6   B7    B8
     MCC2MCC1 MNC1MCC3 FFMNC2   D1NC2 D3D2 D5D4 D7D6 D9D8

     OR (3 Digit MNC)
     B1       B2       B3       B4    B5   B6   B7    B8
     MCC2MCC1 MNC1MCC3 MNC3MNC2 D1NC2 D3D2 D5D4 D7D6 D9D8
     ------------------------------------------------------------------------*/
  digit1 = imsi->buffer_p[GSDI_PERSO_ENG_IMSI_DIGIT_1_OFFSET] & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
  digit2 = imsi->buffer_p[GSDI_PERSO_ENG_IMSI_DIGIT_2_OFFSET] & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
  digit1 = (digit1 >> 4) & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
  digit2 = (digit2 << 4) & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
  imsi_buffer[0] = digit1 | digit2;

  digit1 = imsi->buffer_p[GSDI_PERSO_ENG_IMSI_DIGIT_3_OFFSET] & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
  digit1 = (digit1 >> 4) & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
  if ( num_mnc_digits == 0x02 )
  {
    digit2 = 0xF0;
    imsi_buffer[1] = digit1 | digit2;

    imsi_buffer[2] = imsi->buffer_p[3];
    imsi_buffer[3] = imsi->buffer_p[4];
    imsi_buffer[4] = imsi->buffer_p[5];
    imsi_buffer[5] = imsi->buffer_p[6];
    imsi_buffer[6] = imsi->buffer_p[7];
    imsi_buffer[7] = imsi->buffer_p[8];
  }
  else
  {
    digit2  = imsi->buffer_p[GSDI_PERSO_ENG_IMSI_DIGIT_6_OFFSET] & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
    digit2 = (digit2 << 4) & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[1] = digit1 | digit2;

    imsi_buffer[2] = imsi->buffer_p[GSDI_PERSO_ENG_IMSI_DIGITS_4_5_OFFSET];

    digit1 = ( imsi->buffer_p[4] >> 4 ) & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
    digit2 = ( imsi->buffer_p[5] << 4 ) & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[3] = digit1 | digit2;

    digit1 = ( imsi->buffer_p[5] >> 4 ) & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
    digit2 = ( imsi->buffer_p[6] << 4 ) & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[4] = digit1 | digit2;

    digit1 = ( imsi->buffer_p[6] >> 4 ) & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
    digit2 = ( imsi->buffer_p[7] << 4 ) & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[5] = digit1 | digit2;

    digit1 = ( imsi->buffer_p[7] >> 4 ) & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
    digit2 = ( imsi->buffer_p[8] << 4 ) & GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[6] = digit1 | digit2;

    digit1 = ( imsi->buffer_p[8] >> 4 ) & GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK;
    digit2 = 0xF0;
    imsi_buffer[7] = digit1 | digit2;

  }

  memcpy(imsi->buffer_p, imsi_buffer, 8);

  imsi->len = 8;

  return gsdi_status;
}


/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_SIM_DATA

DESCRIPTION:
  Will gather all of the information from the SIM/USIM/RUIM inserted.
  It will retrieve:
  EF-IMSI
  EF-CNL
  EF-GID1
  EF-GID2

DEPENDENCIES:
  Card must be inserted before being called and the PIN Must be verified.

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_sim_data(
  gsdi_slot_id_type                    slot,
  gsdi_perso_sec_sim_data_type       * sim_data_p,
  gsdi_perso_engine_mem_pointer_type * mem_pointer_p
)
{
  gsdi_returns_T                   gsdi_status = GSDI_SUCCESS;
  uim_items_enum_type              file_name   = UIM_NO_SUCH_ITEM;

  /* --------------------------------------------------------------------------
     Perform Validation Checks
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(mem_pointer_p);
  MMGSDI_RETURN_IF_NULL(mem_pointer_p->memory_curr);

  /* --------------------------------------------------------------------------
     Assign the IMSI Pointer to the current mmgsdi_malloc'ed memory
     block available
     ------------------------------------------------------------------------*/
  sim_data_p->imsi.buffer_p = mem_pointer_p->memory_curr;

  /* --------------------------------------------------------------------------
     Get the IMSI information from the SIM/USIM
     If retrieval of the IMSI information fails,
     the sec_data_p->buffer_p memory allocated for
     this request will already be freed.
     ------------------------------------------------------------------------*/
  MMGSDI_SELECT_FILE(file_name,UIM_USIM_IMSI, UIM_GSM_IMSI);
   gsdi_status = gsdi_perso_engine_read_sim(slot,
                                           file_name,
                                           (void*)&sim_data_p->imsi);

  if ( gsdi_status != GSDI_SUCCESS &&
       gsdi_status != GSDI_NOT_SUPPORTED )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: IMSI read failed 0x%x",gsdi_status,0,0);
    return gsdi_status;
  }

  gsdi_status = gsdi_perso_engine_read_and_format_imsi(slot,&sim_data_p->imsi);

  if ( gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Read and format IMSI failed 0x%x",gsdi_status,0,0);
    return gsdi_status;
  }

  /* ------------------------------------------------------------------------
     The read operation was successful.  Need to move the current pointer
     of the mem_pointer_p provided to ensure it doesn't overwrite IMSI
     as a result of reading additional data
     ----------------------------------------------------------------------*/
  if ( ( mem_pointer_p->memory_curr + sim_data_p->imsi.len ) >
         mem_pointer_p->memory_end )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY USAGE ERROR",0,0,0);
    return GSDI_ERROR;
  }

  mem_pointer_p->memory_curr += sim_data_p->imsi.len;

  /* ---------------------------------------------------------------
     Get the CNL information from the SIM/USIM
     If retrieval of the CNL information fails,
     the sec_data_p->buffer_p memory allocated for
     this request will already be freed.
     -------------------------------------------------------------*/
  sim_data_p->cnl.buffer_p = mem_pointer_p->memory_curr;

  MMGSDI_SELECT_FILE(file_name,UIM_USIM_CNL, UIM_GSM_CNL);
  gsdi_status = gsdi_perso_engine_read_sim(slot,
                                           file_name,
                                           &sim_data_p->cnl);

  if ( gsdi_status != GSDI_SUCCESS)
  {
    sim_data_p->cnl.len      = 0;
    sim_data_p->cnl.buffer_p = NULL;
  }


  /* ------------------------------------------------------------------------
     The read operation was successful.  Need to move the current pointer
     of the mem_pointer_p provided to ensure it doesn't overwrite CNL
     as a result of reading additional data
     ----------------------------------------------------------------------*/
  if ( ( mem_pointer_p->memory_curr + sim_data_p->cnl.len ) >
         mem_pointer_p->memory_end )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY USAGE ERROR",0,0,0);
    return GSDI_ERROR;
  }

  mem_pointer_p->memory_curr += sim_data_p->cnl.len;

  /* ---------------------------------------------------------------
     Get the GID1 information from the SIM/USIM
     If retrieval of the GID1 information fails,
     the sec_data_p->buffer_p memory allocated for
     this request will already be freed.
     -------------------------------------------------------------*/
  sim_data_p->gid1.buffer_p = mem_pointer_p->memory_curr;

  MMGSDI_SELECT_FILE(file_name,UIM_USIM_GID1, UIM_GSM_GID1);
  gsdi_status = gsdi_perso_engine_read_sim(slot,
                                           file_name,
                                           &sim_data_p->gid1);

  if ( gsdi_status != GSDI_SUCCESS)
  {
    sim_data_p->gid1.len      = 0;
    sim_data_p->gid1.buffer_p = NULL;
  }


  /* ------------------------------------------------------------------------
     The read operation was successful.  Need to move the current pointer
     of the mem_pointer_p provided to ensure it doesn't overwrite GID1
     as a result of reading additional data
     ----------------------------------------------------------------------*/
  if ( ( mem_pointer_p->memory_curr + sim_data_p->gid1.len ) >
         mem_pointer_p->memory_end )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY USAGE ERROR",0,0,0);
    return GSDI_ERROR;
  }

  mem_pointer_p->memory_curr += sim_data_p->gid1.len;


 /* ---------------------------------------------------------------
     Get the GID2 information from the SIM/USIM
     If retrieval of the GID2 information fails,
     the sec_data_p->buffer_p memory allocated for
     this request will already be freed.
     -------------------------------------------------------------*/
  sim_data_p->gid2.buffer_p = mem_pointer_p->memory_curr;

  MMGSDI_SELECT_FILE(file_name,UIM_USIM_GID2, UIM_GSM_GID2);
  gsdi_status = gsdi_perso_engine_read_sim(slot,
                                           file_name,
                                           &sim_data_p->gid2);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    sim_data_p->gid2.len      = 0;
    sim_data_p->gid2.buffer_p = NULL;
  }

  /* ------------------------------------------------------------------------
     The read operation was successful.  Need to move the current pointer
     of the mem_pointer_p provided to ensure it doesn't overwrite GID1
     as a result of reading additional data
     ----------------------------------------------------------------------*/
  if ( ( mem_pointer_p->memory_curr + sim_data_p->gid2.len ) >
         mem_pointer_p->memory_end )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY USAGE ERROR",0,0,0);
    return GSDI_ERROR;
  }

  mem_pointer_p->memory_curr += sim_data_p->gid2.len;

  /* ---------------------------------------------------------------
     Get the DCK information from the SIM/USIM
     If retrieval of the DCK information fails,
     the sec_data_p->buffer_p memory allocated for
     this request will already be freed.
     -------------------------------------------------------------*/
  sim_data_p->dck.buffer_p = mem_pointer_p->memory_curr;

  MMGSDI_SELECT_FILE(file_name,UIM_USIM_DCK, UIM_GSM_DCK);
  gsdi_status = gsdi_perso_engine_read_sim(slot,
                                           file_name,
                                           &sim_data_p->dck);

  if ( gsdi_status != GSDI_SUCCESS)
  {
    sim_data_p->dck.len      = 0;
    sim_data_p->dck.buffer_p = NULL;
  }

  /* ------------------------------------------------------------------------
     The read operation was successful.  Need to move the current pointer
     of the mem_pointer_p provided to ensure it doesn't overwrite GID1
     as a result of reading additional data
     ----------------------------------------------------------------------*/
  if ( ( mem_pointer_p->memory_curr + sim_data_p->dck.len ) >
         mem_pointer_p->memory_end )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY USAGE ERROR",0,0,0);
    return GSDI_ERROR;
  }

  mem_pointer_p->memory_curr += sim_data_p->dck.len;
  return GSDI_SUCCESS;

} /* gsdi_perso_engine_get_sim_data */

#endif /* FEATURE_PERSO_SIM */

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_MAP_TO_MASK

DESCRIPTION:
  Will gather all of the information from the SIM/USIM/RUIM inserted.
  It will retrieve:
  EF-IMSI
  EF-CNL
  EF-GID1
  EF-GID2

DEPENDENCIES:
  Card must be inserted before being called and the PIN Must be verified.

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_map_to_mask (
  gsdi_perso_enum_type              feature,
  gsdi_perso_sec_client_req_type *  client_data_p
)
{
  /* --------------------------------------------------------------------------
     validate the parmeters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(client_data_p);

  /* --------------------------------------------------------------------------
     Now set the Mask according to the feature
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_NW:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_NW;
      break;

    case GSDI_PERSO_NS:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_NS;
      break;

    case GSDI_PERSO_SP:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_SP;
      break;

    case GSDI_PERSO_CP:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_CP;
      break;

    case GSDI_PERSO_SIM:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_SIM;
      break;

    case GSDI_PERSO_PROP1:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_PROP1;
      break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_RUIM_NW1:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
      break;

    case GSDI_PERSO_RUIM_NW2:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2;
      break;

    case GSDI_PERSO_RUIM_HRPD:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD;
      break;

    case GSDI_PERSO_RUIM_SP:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP;
      break;

    case GSDI_PERSO_RUIM_CP:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP;
      break;

    case GSDI_PERSO_RUIM_RUIM:
      client_data_p->feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM;
      break;
#endif /* FEATURE_PERSO_RUIM */
    case GSDI_PERSO_NONE:
    default:
      MMGSDI_DEBUG_MSG_ERROR("PERSO: BAD FEATURE RECEIVED: 0x%x",feature,0,0);
      return GSDI_ERROR;
  }

  return GSDI_SUCCESS;
}

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_DCK

DESCRIPTION:
  Will gather the client's depersonalization information from the request to
  the Security Library Structure.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_dck (
  uint32                      len,
  const uint8 *               dck_p,
  gsdi_perso_sec_data_type * client_data_p
)
{
  /* --------------------------------------------------------------------------
     Validate all parameters provided.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(client_data_p);
  MMGSDI_RETURN_IF_NULL(client_data_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(dck_p);
  MMGSDI_RETURN_IF_EXCEEDS(len,GSDI_PERSO_SEC_MAX_DCK_LEN);

  /* -------------------------------------------------------------------------
     Copy over the DCK and complete populating the client_data_p
     -----------------------------------------------------------------------*/
  memcpy(client_data_p->buffer_p,
         dck_p,
         len);

  client_data_p->len = len;

  return GSDI_SUCCESS;
} /* gsdi_perso_engine_get_dck */

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_PHONE_CODE

DESCRIPTION:
  Will gather the client's depersonalization information from the request to
  the Security Library Structure.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_phone_code (
  uint32                     len,
  const uint8 *              phone_code_p,
  gsdi_perso_sec_data_type * client_data_p
)
{
  /* --------------------------------------------------------------------------
     Validate all parameters provided.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(client_data_p);
  MMGSDI_RETURN_IF_NULL(client_data_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(phone_code_p);
  MMGSDI_RETURN_IF_EXCEEDS(len,GSDI_PERSO_SEC_MAX_DCK_LEN);

  /* -------------------------------------------------------------------------
     Copy over the DCK and complete populating the client_data_p
     -----------------------------------------------------------------------*/
  memcpy(client_data_p->buffer_p,
         phone_code_p,
         len);

  client_data_p->len = len;

  return GSDI_SUCCESS;
} /* gsdi_perso_engine_get_phone_code */

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_MASTER_KEY

DESCRIPTION:
  Will gather the client's depersonalization information from the request to
  the Security Library Structure.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_master_key (
  uint32                     len,
  const  uint8 *             master_key_p,
  gsdi_perso_sec_data_type * client_data_p
)
{
  /* --------------------------------------------------------------------------
     Validate all parameters provided.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(client_data_p);
  MMGSDI_RETURN_IF_NULL(client_data_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(master_key_p);
  MMGSDI_RETURN_IF_EXCEEDS(len,GSDI_PERSO_SEC_MAX_DCK_LEN);

  /* -------------------------------------------------------------------------
     Copy over the DCK and complete populating the client_data_p
     -----------------------------------------------------------------------*/
  memcpy(client_data_p->buffer_p,
         master_key_p,
         len);

  client_data_p->len = len;

  return GSDI_SUCCESS;
} /* gsdi_perso_engine_get_master_key */

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_CLIENT_GET_FEATURE_DATA

DESCRIPTION:
  Copies the original req_p data into the client data structure

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_client_feature_data
(
  uint32 len,
  const uint8 *data_buffer,
  gsdi_perso_sec_data_type *client_data_p

)
{
 /* --------------------------------------------------------------------------
     Validate all parameters provided.
 ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(client_data_p);
  MMGSDI_RETURN_IF_NULL(client_data_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(data_buffer);
  MMGSDI_RETURN_IF_EXCEEDS(len,GSDI_PERSO_MAX_DATA_LEN);

  client_data_p->len = len;
  memcpy(client_data_p->buffer_p, data_buffer, len);

  return GSDI_SUCCESS;
}

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_CLIENT_DATA

DESCRIPTION:
  Will gather all information required for the request made by the
  client and put into a manner acceptable for the security Library.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_client_data(
   gsdi_perso_engine_proc_enum_type           perso_proc,
   gsdi_perso_sec_client_req_type           * client_data_p,
   const gsdi_perso_engine_mem_pointer_type * mem_pointer,
   const void *                               req_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  /* --------------------------------------------------------------------------
     Verify the parameters provided.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(client_data_p);
  MMGSDI_RETURN_IF_NULL(req_p);
  MMGSDI_RETURN_IF_NULL(mem_pointer);

  /* --------------------------------------------------------------------------
     Start transferring the information from the Request
     to the client data
     ------------------------------------------------------------------------*/
  switch ( perso_proc )
  {
    case GSDI_PERSO_ENGINE_ACTIVATE_FEATURE:
      /* Extract the Personalization Feature to activate */
      gsdi_status = gsdi_perso_engine_map_to_mask (
                        ((gsdi_perso_act_feature_ind_req_T *)req_p)->feature,
                        client_data_p);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Map Feature 0x%x",gsdi_status,0,0);
        return gsdi_status;
      }

      client_data_p->dck.buffer_p = mem_pointer->memory_start;

      /* Extract the Depersonalization Control Keys */
      gsdi_status = gsdi_perso_engine_get_dck(
                        (uint32)((gsdi_perso_act_feature_ind_req_T *)req_p)->num_bytes,
                        ((gsdi_perso_act_feature_ind_req_T *)req_p)->ck_buffer,
                        &client_data_p->dck);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Deperso Keys 0x%x",gsdi_status,0,0);
        return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_UNBLOCK_DCK:
      /* Extract the Personalization Feature to deactivate */
      gsdi_status = gsdi_perso_engine_map_to_mask (
                        ((gsdi_perso_unblock_feature_ind_req_T *)req_p)->feature,
                        client_data_p);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Map Feature 0x%x",gsdi_status,0,0);
        return GSDI_ERROR;
      }

      /* Extract the Depersonalization Control Keys */
      client_data_p->dck.buffer_p = mem_pointer->memory_start;

      gsdi_status = gsdi_perso_engine_get_dck(
                    (uint32)((gsdi_perso_unblock_feature_ind_req_T *)req_p)->num_bytes,
                    ((gsdi_perso_unblock_feature_ind_req_T *)req_p)->unblock_ck_buffer,
                    &client_data_p->dck);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Deperso Keys 0x%x",
                               gsdi_status,0,0);
        return GSDI_ERROR;
      }
      break;

    case GSDI_PERSO_ENGINE_DEACTIVATE_FEATURE:
      /* Extract the Personalization Feature to deactivate */
      gsdi_status = gsdi_perso_engine_map_to_mask (
                        ((gsdi_perso_deact_feature_ind_req_T *)req_p)->feature,
                        client_data_p);

      if ( gsdi_status != GSDI_SUCCESS )
      {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Map Feature 0x%x",
                                 gsdi_status,0,0);
          return gsdi_status;
      }

      /* Extract the Depersonalization Control Keys */
      client_data_p->dck.buffer_p = mem_pointer->memory_start;

      gsdi_status = gsdi_perso_engine_get_dck(
                    (uint32)((gsdi_perso_deact_feature_ind_req_T *)req_p)->num_bytes,
                    ((gsdi_perso_deact_feature_ind_req_T *)req_p)->ck_buffer,
                    &client_data_p->dck);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Deperso Keys 0x%x",gsdi_status,0,0);
        return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_SET_DATA_FEATURE:
      /* Extract the Personalization Feature for the data that needs to be set */
      gsdi_status = gsdi_perso_engine_map_to_mask (
                    ((gsdi_perso_set_feature_data_req_T *)req_p)->feature,
                    client_data_p);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Map Feature 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }

      client_data_p->feature_data.buffer_p = mem_pointer->memory_start;

      /* Extract the Data to be Set */
      gsdi_status = gsdi_perso_engine_get_client_feature_data(
                    (uint32)((gsdi_perso_set_feature_data_req_T *)req_p)->num_bytes,
                    ((gsdi_perso_set_feature_data_req_T *)req_p)->data_buffer,
                    &client_data_p->feature_data);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Client Feature Data 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }

      break;

    case GSDI_PERSO_ENGINE_GET_FEATURE_DATA:
      /* Extract the Personalization Feature */
      gsdi_status = gsdi_perso_engine_map_to_mask (
                    ((gsdi_perso_get_feature_data_req_T *)req_p)->feature,
                    client_data_p);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Map Feature 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS:
      client_data_p->feature_mask =
#ifdef FEATURE_PERSO_SIM
      (int)GSDI_PERSO_SEC_MASK_FEAT_NW |
      (int)GSDI_PERSO_SEC_MASK_FEAT_NS |
      (int)GSDI_PERSO_SEC_MASK_FEAT_SP |
      (int)GSDI_PERSO_SEC_MASK_FEAT_CP |
      (int)GSDI_PERSO_SEC_MASK_FEAT_SIM |
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
      (int)GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1 |
      (int)GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2 |
      (int)GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD|
      (int)GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP |
      (int)GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP |
      (int)GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM|
#endif /* FEATURE_PERSO_RUIM */
      0; /*Do not remove, required because of featurization*/
      break;

    case GSDI_PERSO_ENGINE_AUTO_LOCK:
      client_data_p->feature_mask = int32touint32(((gsdi_perso_internal_command_req_T *)req_p)->autolock_mask);
      break;

    case GSDI_PERSO_ENGINE_PERM_DISABLE_FEATURE_IND:
      /* Extract the Personalization Feature to disable */
      gsdi_status = gsdi_perso_engine_map_to_mask (
                       ((gsdi_perso_perm_disabled_feature_req_T *)req_p)->perso_feature,
                       client_data_p);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Map Feature 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }
      client_data_p->dck.buffer_p = mem_pointer->memory_start;

      /* Extract the Depersonalization Control Keys */
      gsdi_status = gsdi_perso_engine_get_dck(
                        (uint32)((gsdi_perso_perm_disabled_feature_req_T *)req_p)->num_bytes,
                        ((gsdi_perso_perm_disabled_feature_req_T *)req_p)->ck_buffer,
                        &client_data_p->dck);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Deperso Keys 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_GET_FEATURE_KEY:
      gsdi_status = gsdi_perso_engine_map_to_mask (
                    ((gsdi_perso_get_feature_key_req_T *)req_p)->feature,
                    client_data_p);

      client_data_p->get_unblock_key = ((gsdi_perso_get_feature_key_req_T *)req_p)->is_unblock_key;

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Map Feature 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_OTA_DERPERSO:

      client_data_p->feature_data.buffer_p = mem_pointer->memory_start;

      /* Extract the Data to be Set */
      gsdi_status = gsdi_perso_engine_get_client_feature_data(
                    (uint32)((gsdi_perso_ota_deperso_req_T *)req_p)->num_bytes,
                    ((gsdi_perso_ota_deperso_req_T *)req_p)->data_buffer,
                    &client_data_p->feature_data);

      if ( gsdi_status != GSDI_SUCCESS )
      {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Client Feature Data 0x%x",
                                 gsdi_status,0,0);
          return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_CHECK_LOCKS_ALL:
    case GSDI_PERSO_ENGINE_RUIM_CHECK_LOCKS_ALL:
      client_data_p->feature_mask = (int32)((gsdi_perso_internal_command_req_T*)req_p)->feature;
      break;

    case GSDI_PERSO_ENGINE_VERIFY_PHONE_CODE:
      /* Extract the phone code */
      client_data_p->phone_code.buffer_p = mem_pointer->memory_start;

      /* Extract the phone code */
      gsdi_status = gsdi_perso_engine_get_phone_code(
                        (uint32)((gsdi_perso_verify_phone_code_req_T *)req_p)->phone_code_len,
                        ((gsdi_perso_verify_phone_code_req_T *)req_p)->phone_code_ptr,
                        &client_data_p->phone_code);

      if ( gsdi_status != GSDI_SUCCESS )
      {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Verify Phone Code 0x%x",
                                 gsdi_status,0,0);
          return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_CHANGE_PHONE_CODE:
      /* Extract the phone code */
      client_data_p->phone_code.buffer_p = mem_pointer->memory_start;

      /* Extract the phone code */
      gsdi_status = gsdi_perso_engine_get_phone_code(
                          int32touint32(((gsdi_perso_change_phone_code_req_T *)req_p)->old_phone_code_len),
                          ((gsdi_perso_change_phone_code_req_T *)req_p)->old_phone_code_ptr,
                          &client_data_p->phone_code);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Deperso Keys 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }
      client_data_p->new_phone_code.buffer_p =
        mem_pointer->memory_start + client_data_p->phone_code.len;

      /* Extract the phone code */
      gsdi_status = gsdi_perso_engine_get_phone_code(
                          int32touint32(((gsdi_perso_change_phone_code_req_T *)req_p)->new_phone_code_len),
                          ((gsdi_perso_change_phone_code_req_T *)req_p)->new_phone_code_ptr,
                          &client_data_p->new_phone_code);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Change Phone Code 0x%x",
                               gsdi_status,0,0);
        return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_UNBLOCK_PHONE_CODE:
      /* Extract the phone code */
      client_data_p->master_key.buffer_p = mem_pointer->memory_start;

      /* Extract the phone code */
      gsdi_status = gsdi_perso_engine_get_master_key(
                        (uint32)((gsdi_perso_unblock_phone_code_req_T *)req_p)->master_key_len,
                        ((gsdi_perso_unblock_phone_code_req_T *)req_p)->master_key_ptr,
                        &client_data_p->master_key);

      if ( gsdi_status != GSDI_SUCCESS )
      {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Get Deperso Keys 0x%x",
                                 gsdi_status,0,0);
          return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENGINE_SET_PHONE_CODE:
      /* Extract the phone code */
      client_data_p->phone_code.buffer_p = mem_pointer->memory_start;

      /* Extract the phone code */
      gsdi_status = gsdi_perso_engine_get_phone_code(
                        int32touint32(((gsdi_perso_set_phone_code_req_T *)req_p)->phone_code_len),
                        ((gsdi_perso_set_phone_code_req_T *)req_p)->phone_code_ptr,
                        &client_data_p->phone_code);

      if ( gsdi_status != GSDI_SUCCESS )
      {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: Could Not Set Phone code 0x%x",
                                 gsdi_status,0,0);
          return gsdi_status;
      }
      break;

    default:
      return gsdi_status;

  }

  return gsdi_status;

}

#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINGE_GET_IMEI_SIZE

DESCRIPTION
  This function is called to calcuate the total memory required for the
  IMEI BUFFER

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_get_imei_size(
  uint32 * len_p
)
{
  MMGSDI_RETURN_IF_NULL(len_p);
  *len_p = GSDI_PERSO_MAX_IMEI_LEN;
  return GSDI_SUCCESS;
} /* gsdi_perso_engine_get_imei_size */

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINGE_GET_SERIAL_NUM_SIZE

DESCRIPTION
  This function is called to calcuate the total memory required for the
  IMEI BUFFER

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_get_serial_num_size(
  int32 * len_p
)
{
  MMGSDI_RETURN_IF_NULL(len_p);
  *len_p = GSDI_PERSO_ENGINE_SERIAL_NUMBER_LEN;
  return GSDI_SUCCESS;
} /* gsdi_perso_engine_get_imei_size */

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CALC_ENC_DATA_LEN

DESCRIPTION
  This function is called to calcuate the total memory required for a
  particular item.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_calc_enc_data_len(
  gsdi_perso_engine_proc_enum_type      feature_ind,
  const void                           * req_p,
   int32                               * len_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_SUCCESS;
  int32          len          = 0;
#ifdef FEATURE_MMGSDI_PERSO_SFS
  uint8         *data_buf_ptr = NULL;
#endif /* FEATURE_MMGSDI_PERSO_SFS */

  /* -----------------------------------------------------------------------
     Do the parameter checking
  ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(len_p);

  #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  /* Ok to just return SUCCESS and have a len of
  ** 0 since this is not used by the SHIM Layer
  */
  *len_p = 0;
  return GSDI_SUCCESS;
  #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */


  /* -----------------------------------------------------------------------
     Retrieve the File Size from FS
     ---------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_PERSO_SFS
  data_buf_ptr = mmgsdi_malloc(1000);
  len= sfs_priv_simloc_read(data_buf_ptr, 1000);

  MMGSDIUTIL_TMC_MEM_FREE(data_buf_ptr);

  if ( len == 0 )
#else
  gsdi_status = gsdi_fs_get_file_size(mmgsdi_efs_perso_file,&len);

  if ( gsdi_status != GSDI_SUCCESS ||
       len == 0 )
#endif /* FEATURE_MMGSDI_PERSO_SFS */
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO:  FILE Status: 0x%x File Size: 0x%x",
                           gsdi_status,len,0);
    return GSDI_ERROR;
  }

  *len_p = len;

  /*Also cater for the new data that may be added*/
  if(feature_ind == GSDI_PERSO_ENGINE_SET_DATA_FEATURE)
  {
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p += ((gsdi_perso_set_feature_data_req_T *)req_p)->num_bytes *2;
  }
  /* Check to see if new SIM Lock Codes have to be generated */
  else if(feature_ind == GSDI_PERSO_ENGINE_AUTO_LOCK)
  {
      *len_p += (int32)(GSDI_PERSO_BUFFER_LEN -
                      (GSDI_PERSO_ENGINE_TAG_SIZE * GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS));
  }

  /* ------------------------------------------------------------------------
    Get the total size of the Mobile's IMEI
    -----------------------------------------------------------------------*/
  len =0;
  gsdi_status = gsdi_perso_engine_get_imei_size((uint32*)&len);

  if ( gsdi_status != GSDI_SUCCESS )
  {
      *len_p = 0;
      MMGSDI_DEBUG_MSG_ERROR("PERSO: get imei size failed: 0x%x",
                             gsdi_status,0,0);
      return gsdi_status;
  }

  /* Accumulate the length of the Deperso keys */
  *len_p = *len_p + len;


  /* ------------------------------------------------------------------------
    Get the total size of the Mobile's Serial Number
    -----------------------------------------------------------------------*/
  len =0;
  gsdi_status = gsdi_perso_engine_get_serial_num_size(&len);

  if ( gsdi_status != GSDI_SUCCESS )
  {
      *len_p = 0;
      MMGSDI_DEBUG_MSG_ERROR("PERSO:  get mobile serial number failed 0x%x",
                             gsdi_status,0,0);
      return gsdi_status;
  }

  /* Accumulate the length of the Deperso keys */
  *len_p = *len_p + len;

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_calc_enc_data_len */

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CALC_SIM_DATA_LEN

DESCRIPTION
  This function is called to calcuate the total memory required for the
  SIM Items to be retrieved.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_calc_sim_data_len(
  int32 * len_p
)
{
  /* ------------------------------------------------------------------------
     Validate all parameters
     ----------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(len_p);

  /* ------------------------------------------------------------------------
     Calculate the total memory required.
     ----------------------------------------------------------------------*/
  *len_p = GSDI_PERSO_ENGINE_IMSI_LEN +
           GSDI_PERSO_ENGINE_GID1_LEN +
           GSDI_PERSO_ENGINE_GID2_LEN +
           GSDI_PERSO_ENGINE_CNL_LEN;

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_calc_sim_data_len */
#endif /*FEATURE_PERSO_SIM*/
/*===========================================================================
FUNCTION GSDI_PERSO_SEC_CALC_TOTAL_MEM_REQUIRED

DESCRIPTION
  This function is called to calcuate the total memory required for a
  particular item.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_calc_client_data_len(
  gsdi_perso_engine_proc_enum_type      feature_ind,
  const void  *                         req_p,
  int32 *                               len_p
)
{
  /* ------------------------------------------------------------------------
     Validate all parameters
     ----------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(len_p);

  /* ------------------------------------------------------------------------
     Conditionally check the input parameters.  OK to have
     req_p of NULL if we are going to get all feature indicators
     since this is an internal request generated by our internal
     initialization procedures.
     ----------------------------------------------------------------------*/
  if ( feature_ind != GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS)
  {
    MMGSDI_RETURN_IF_NULL(req_p);
  }

  /* ------------------------------------------------------------------------
     Calculate the total memory required.
     ----------------------------------------------------------------------*/
  /* ------------------------------------------------------------------------
     GSDI_PERSO_ENGINE_GET_FEATURE_DATA case does not require any data to be
     copied from the client but to enable copying of feature indicator we 
     assign length to be the number of perso features
     ----------------------------------------------------------------------*/
  switch ( feature_ind )
  {
    case   GSDI_PERSO_ENGINE_ACTIVATE_FEATURE:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_act_feature_ind_req_T *)req_p)->num_bytes;
      break;

    case GSDI_PERSO_ENGINE_DEACTIVATE_FEATURE:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_deact_feature_ind_req_T *)req_p)->num_bytes;
      break;

    case GSDI_PERSO_ENGINE_UNBLOCK_DCK:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_unblock_feature_ind_req_T *)req_p)->num_bytes;
      break;

    case GSDI_PERSO_ENGINE_GET_PHONE_CODE_STATUS:
    case GSDI_PERSO_ENGINE_GET_DCK_NUM_RETRIES:
    case GSDI_PERSO_ENGINE_GET_DCK_UNBLOCK_NUM_RETRIES:
      *len_p = 0;
      break;

    case GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS:
    case GSDI_PERSO_ENGINE_GET_FEATURE_IND:
    case GSDI_PERSO_ENGINE_GET_FEATURE_DATA:
      *len_p = NUM_OF_PERSO_SIM_FEATURES + NUM_OF_PERSO_RUIM_FEATURES;
      break;

    case GSDI_PERSO_ENGINE_CHECK_LOCKS_ALL:
       *len_p = NUM_OF_PERSO_SIM_FEATURES;
       break;

    case GSDI_PERSO_ENGINE_RUIM_CHECK_LOCKS_ALL:
      *len_p = NUM_OF_PERSO_RUIM_FEATURES;

      break;

    case GSDI_PERSO_ENGINE_GET_FEATURE_KEY:
      *len_p = 8;
      break;

    case GSDI_PERSO_ENGINE_GET_PERM_FEATURE_IND:
      *len_p = 10;
      break;

    case GSDI_PERSO_ENGINE_PERM_DISABLE_FEATURE_IND:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_perm_disabled_feature_req_T*)req_p)->num_bytes;
      break;

   case GSDI_PERSO_ENGINE_SET_DATA_FEATURE:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_set_feature_data_req_T *)req_p)->num_bytes;
      break;

    case GSDI_PERSO_ENGINE_OTA_DERPERSO:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_ota_deperso_req_T *)req_p)->num_bytes;
      break;

    case GSDI_PERSO_ENGINE_AUTO_LOCK:
    case GSDI_PERSO_ENGINE_REFRESH:
      *len_p = NUM_OF_PERSO_SIM_FEATURES;
      break;

    case GSDI_PERSO_ENGINE_VERIFY_PHONE_CODE:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_verify_phone_code_req_T *)req_p)->phone_code_len;
      break;

    case GSDI_PERSO_ENGINE_RUIM_AUTO_LOCK:
      *len_p = NUM_OF_PERSO_RUIM_FEATURES;
      break;

    case GSDI_PERSO_ENGINE_CHANGE_PHONE_CODE:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_change_phone_code_req_T *)req_p)->old_phone_code_len;
      *len_p += ((gsdi_perso_change_phone_code_req_T *)req_p)->new_phone_code_len;
      break;

    case GSDI_PERSO_ENGINE_UNBLOCK_PHONE_CODE:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_unblock_phone_code_req_T *)req_p)->master_key_len;
      //*len_p += ((gsdi_perso_unblock_phone_code_req_T *)req_p)->new_phone_code_len;
      *len_p += 0;
      break;

    case GSDI_PERSO_ENGINE_SET_PHONE_CODE:
      MMGSDI_RETURN_IF_NULL(req_p);
      *len_p = ((gsdi_perso_set_phone_code_req_T *)req_p)->phone_code_len;
      break;

    default:
      MSG_ERROR("PERSO: UNKNOWN feature_ind: 0x%x",feature_ind,0,0);
      *len_p = 0;
      return GSDI_ERROR;
  }
  return GSDI_SUCCESS;

} /* gsdi_perso_engine_calc_client_data_len */


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CALC_MEM_REQUIRED

DESCRIPTION
  This function is called to calcuate the total memory required for a
  particular item.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_calc_mem_required(
  gsdi_perso_eng_calc_req_mem_enum_type calc_type,
  gsdi_perso_engine_proc_enum_type      feature_ind,
  const void *                          req_p,
  int32 *                               required_len_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_SUCCESS;
  /* -----------------------------------------------------------------------
     Validate the Parameters provided
     ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(required_len_p);
  MMGSDI_RETURN_IF_NULL(req_p);

  /* --------------------------------------------------------------------------
     Initialize all variables
     ------------------------------------------------------------------------*/
  *required_len_p = 0;

  /* --------------------------------------------------------------------------
     Get the total length of all memory which needs to be allocated.
     Please see the notes on each calc_type to understand how the total
     memory size is allocated.
     ------------------------------------------------------------------------*/
  switch ( calc_type )
  {
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_ENG_CALC_ENC_DATA:
      /* ----------------------------------------------------------------------
         The total memory required is the sum of the following:
          Buffer size for the Feature Indicators
          Buffer size for the Depersonalization Keys
          Buffer size for the Depersonalization Counters
          Buffer size for the Mobile's IMEI
          Buffer size for the Mobile's Serial Number
         --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_calc_enc_data_len(feature_ind,
                                                        req_p,
                                                        required_len_p);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        return gsdi_status;
      }

      break;

    case GSDI_PERSO_ENG_CALC_SIM_DATA:
      /* ----------------------------------------------------------------------
         The total memory required is the sum of the following:
          Buffer size for the IMSI
          Buffer size for the CNL
          Buffer size for the GID1
          Buffer size for the GID2
         --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_calc_sim_data_len(required_len_p);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        return gsdi_status;
      }
      break;


#endif /*FEATURE_PERSO_SIM*/
    case GSDI_PERSO_ENG_CALC_CLIENT_DATA:
      /* ----------------------------------------------------------------------
         The total memory required is dependent on the
         Action to be performed.  Please see the following function to
         determine how the block of memory is allocated.
         --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_calc_client_data_len(feature_ind,
                                                           req_p,
                                                           required_len_p);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        return gsdi_status;
      }
      break;

    default:
      *required_len_p = 0x00;
      MSG_ERROR("PERSO: Wrong calc_type 0x%x",calc_type,0,0);
      return GSDI_ERROR;

  } /* switch ( calc_type ) */

  return GSDI_SUCCESS;
}

#ifdef FEATURE_PERSO_SIM
/* ============================================================================
FUNCTION: GSDI_PERSO_ENGINE_WRITE_BACK_FS

DESCRIPTION:
  Function will write back to FS any data indicated to be modified.  The
  indication is set by the Security Library.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

============================================================================ */
static gsdi_returns_T gsdi_perso_engine_write_back_fs(
  gsdi_perso_sec_me_data_type * encrypted_me_data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uint32          len         = 0;
  uint8 *        buffer_p    = NULL;
  gsdi_perso_sec_data_type   *sec_data_p;
  int32          i           = 0;
#ifdef FEATURE_MMGSDI_PERSO_SFS
  int total;
#endif
  /* --------------------------------------------------------------------------
     Validate the Paramaters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(encrypted_me_data_p);

  /* Wipe out DCK if refresh boolean is set.  this occur if
     the DCK is not default and there is no failure returned
     in the function where this variable is set.
     note: Wipe out of DCK does not require EFS update together */
  if ( encrypted_me_data_p->data_mod.refresh )
  {
    gsdi_status = gsdi_perso_engine_write_dck();
    if (gsdi_status == GSDI_SUCCESS)
    {
      MSG_HIGH("PERSO: gsdi_perso_engine_write_dck() success", 0,0,0);
    }
    else
    {
      MSG_HIGH("PERSO: gsdi_perso_engine_write_dck() failed",0,0,0);
      gsdi_status = GSDI_SUCCESS;
    }
  }

  /* If update fs is not required, exist out of the function */
  if ( !encrypted_me_data_p->data_mod.update_fs )
  {
    return GSDI_SUCCESS;
  }
  /* Update to FS is required, continue with the process */
  len = encrypted_me_data_p->feat_inds.len +
        encrypted_me_data_p->perm_feat_inds.len +
        encrypted_me_data_p->sim_lock_codes.len +
        encrypted_me_data_p->dcks.len +
        encrypted_me_data_p->dck_counts.len +
        encrypted_me_data_p->unblock_cks.len +
        encrypted_me_data_p->unblock_ck_counts.len +
        (GSDI_PERSO_ENGINE_TAG_SIZE + GSDI_PERSO_ENGINE_LEN_SIZE) * GSDI_PERSO_ENGINE_NUMBER_OF_TAGS;

  buffer_p= mmgsdi_malloc(len);
  /*Write the Feature Inds*/

  MMGSDI_RETURN_IF_NULL(buffer_p);

  len =0;

  for(i =1;i<=GSDI_PERSO_ENGINE_NUMBER_OF_TAGS;i++)
  {
    switch(i)
    {
      case GSDI_PERSO_ENGINE_TAG_FEATURE_INDS:
        sec_data_p = &encrypted_me_data_p->feat_inds;
        break;
      case GSDI_PERSO_ENGINE_TAG_PERM_FEATURE_INDS:
        sec_data_p = &encrypted_me_data_p->perm_feat_inds;
        break;
      case GSDI_PERSO_ENGINE_TAG_SIM_LOCK_CODE:
        sec_data_p = &encrypted_me_data_p->sim_lock_codes;
        break;
      case GSDI_PERSO_ENGINE_TAG_DEPERSO_CK:
        sec_data_p = &encrypted_me_data_p->dcks;
        break;
      case GSDI_PERSO_ENGINE_TAG_DCK_COUNTER:
        sec_data_p = &encrypted_me_data_p->dck_counts;
        break;
      case GSDI_PERSO_ENGINE_TAG_UNBLOCK:
        sec_data_p = &encrypted_me_data_p->unblock_cks;
        break;
      case GSDI_PERSO_ENGINE_TAG_UNBLOCK_COUNTER:
        sec_data_p = &encrypted_me_data_p->unblock_ck_counts;
        break;
      default:
        MSG_ERROR("PERSO: Incorrect tag",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(buffer_p);
        return GSDI_ERROR;
    }

    buffer_p[len] = (uint8)i;
    len += GSDI_PERSO_ENGINE_TAG_SIZE;
    buffer_p[len] = (uint8)(sec_data_p->len/255);
    buffer_p[len+1] = (uint8)(sec_data_p->len%255);
    len += GSDI_PERSO_ENGINE_LEN_SIZE;

    memcpy(buffer_p + len, sec_data_p->buffer_p, sec_data_p->len);
    len += sec_data_p->len;

  }
#ifdef FEATURE_MMGSDI_PERSO_SFS
  total = sfs_priv_simloc_write(buffer_p, len);
  if ( total != len )
#else
  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_perso_file,
                                    (int32)len,
                                    buffer_p);
  if ( gsdi_status != GSDI_SUCCESS )
#endif /*FEATURE_MMGSDI_PERSO_SFS*/
  {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Failure to write: %s", 
                            mmgsdi_efs_perso_file,0,0);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(buffer_p);

  return gsdi_status;
} /* gsdi_perso_engine_write_back_fs */
#endif /*FEATURE_PERSO_SIM*/

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_GET_TECHNOLOGY

DESCRIPTION
  This function returns either umts or cdma or both based on the
  feature type

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static void gsdi_perso_engine_get_technology(gsdi_perso_engine_proc_enum_type feature_type,
                                             uint32                           feature,
                                             boolean                          *umts,
                                             boolean                          *ruim)
{
  switch(feature_type)
  {
    case GSDI_PERSO_ENGINE_ACTIVATE_FEATURE:
    case GSDI_PERSO_ENGINE_DEACTIVATE_FEATURE:
    case GSDI_PERSO_ENGINE_SET_DATA_FEATURE:
    case GSDI_PERSO_ENGINE_UNBLOCK_DCK:
    case GSDI_PERSO_ENGINE_PERM_DISABLE_FEATURE_IND:
    case GSDI_PERSO_ENGINE_GET_PERM_FEATURE_IND:
    case GSDI_PERSO_ENGINE_GET_FEATURE_KEY:
    case GSDI_PERSO_ENGINE_GET_FEATURE_DATA:
      switch(feature)
      {
#ifdef FEATURE_PERSO_SIM
        case GSDI_PERSO_SEC_MASK_FEAT_NW:
        case GSDI_PERSO_SEC_MASK_FEAT_NS:
        case GSDI_PERSO_SEC_MASK_FEAT_SP:
        case GSDI_PERSO_SEC_MASK_FEAT_CP:
        case GSDI_PERSO_SEC_MASK_FEAT_SIM:
         *umts = TRUE;
         break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
          case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
          *ruim = TRUE;
          break;
#endif /* FEATURE_PERSO_RUIM */
        case GSDI_PERSO_NONE:
        default:
          break;
      }
      break;

    case GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS:
    case GSDI_PERSO_ENGINE_GET_FEATURE_IND:
    case GSDI_PERSO_ENGINE_GET_DCK_NUM_RETRIES:
    case GSDI_PERSO_ENGINE_GET_DCK_UNBLOCK_NUM_RETRIES:
      *umts = TRUE;
      *ruim = TRUE;
      break;
    case GSDI_PERSO_ENGINE_OTA_DERPERSO:
    case GSDI_PERSO_ENGINE_CHECK_LOCKS_ALL:
    case GSDI_PERSO_ENGINE_REFRESH:
    case GSDI_PERSO_ENGINE_AUTO_LOCK:
      *umts = TRUE;
      break;
    case GSDI_PERSO_ENGINE_RUIM_CHECK_LOCKS_ALL:
    case GSDI_PERSO_ENGINE_RUIM_AUTO_LOCK:
      *ruim = TRUE;
      break;
    default:
      break;
  }
}

#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_HANDLE_UMTS_REQUEST

DESCRIPTION
  This function is the main handler for all the perso requests.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_handle_umts_request(
                                      gsdi_perso_engine_proc_enum_type feature_type,
                                      gsdi_perso_sec_client_req_type  *client_data,
                                      const void *                     req_p,
                                      uint8 *                          ret_data_p,
                                      int32*                           ret_data_len_p,
                                      gsdi_perso_event_enum_type       *perso_event_p)
{
  gsdi_perso_sec_sim_data_type       sim_data;
  uint32                             total_mem_req      = 0x00;
  gsdi_returns_T                     gsdi_status;
  gsdi_perso_engine_mem_pointer_type enc_mem_pointer;
  gsdi_perso_engine_mem_pointer_type sim_mem_pointer;
  gsdi_perso_sec_me_data_type        encrypted_data;

  /* --------------------------------------------------------------------------
     Initialize Data as necessary
     ------------------------------------------------------------------------*/
  memset(&enc_mem_pointer,0x00, sizeof(gsdi_perso_engine_mem_pointer_type));
  memset(&sim_mem_pointer,0x00, sizeof(gsdi_perso_engine_mem_pointer_type));
  memset(&encrypted_data.data_mod, 0, sizeof(gsdi_perso_sec_data_mod_inds_type));


  /* --------------------------------------------------------------------------
   The Perso Engine Has an Interface to allow the Personalization
   Engine to determine how much memory needs to be allocated.
   ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_engine_calc_mem_required(GSDI_PERSO_ENG_CALC_ENC_DATA,
                                                    feature_type,
                                                    (void *)req_p,
                                                    (int32*)&total_mem_req);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get total memory required",0,0,0);
    return gsdi_status;
  }

  /* ---------------------------------------------------------------------------
     Allocate the entire buffer required for the encrypted data structure
     Memset it to 0x00 when successful and the length required is greater
     than 0.
     Set memory_pointer.memory_curr = memory_pointer.memory_start
     Set memory_pointer.memory_end  = memory_pointer.memory_start + total_mem_req
     -------------------------------------------------------------------------*/
  if ( total_mem_req > 0 )
  {
    enc_mem_pointer.memory_start = mmgsdi_malloc(total_mem_req);
    MMGSDI_RETURN_IF_NULL(enc_mem_pointer.memory_start);
    memset(enc_mem_pointer.memory_start, 0x00, total_mem_req);

    enc_mem_pointer.memory_curr = enc_mem_pointer.memory_start;
    enc_mem_pointer.memory_end  = enc_mem_pointer.memory_start + total_mem_req;
    /* ---------------------------------------------------------------------------
       Populate the encrypted data into the encrypted_data structure.
      The memory pointer will be used to provide memory for the
      encrypted_data structure buffers
      -------------------------------------------------------------------------*/
    gsdi_status = gsdi_perso_engine_get_encrypted_data(&encrypted_data,
                                                       &enc_mem_pointer,
                                                       feature_type,
                                                       (void *)req_p);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      /* An error occurred getting the Encrypted
         information from FS. All memory will be freed.
      */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
      return gsdi_status;
    }
  }
  else
  {
    MSG_MED("PERSO: Encrypted Data Not Required from GSDI ENGINE",0,0,0);
  }

  /* --------------------------------------------------------------------------
     The Perso Engine Has an Interface to allow the Personalization
     Engine to determine how much memory needs to be allocated.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_engine_calc_mem_required(GSDI_PERSO_ENG_CALC_SIM_DATA,
                                                    feature_type,
                                                    (void *)req_p,
                                                    (int32*)&total_mem_req);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get total memory required",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
    return gsdi_status;
  }

  /* ---------------------------------------------------------------------------
     Allocate the entire buffer required for the sim data structure
     Memset it to 0x00 when successful
     Set memory_pointer.memory_curr = memory_pointer.memory_start
     Set memory_pointer.memory_end  = memory_pointer.memory_start + total_mem_req
     -------------------------------------------------------------------------*/
  sim_mem_pointer.memory_start = mmgsdi_malloc(total_mem_req);
  if ( sim_mem_pointer.memory_start == NULL )
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  memset(sim_mem_pointer.memory_start, 0x00, total_mem_req);

  sim_mem_pointer.memory_curr = sim_mem_pointer.memory_start;
  sim_mem_pointer.memory_end  = sim_mem_pointer.memory_start + total_mem_req;

  /* --------------------------------------------------------------------------
     Get the required SIM Data in the case that the SIM Lock State is in
     AUTOLOCK for the Feature Indicated.
     ------------------------------------------------------------------------*/
  if ( gsdi_data.gsdi_state != GSDI_CARD_ERROR_S )
  {
    gsdi_status = gsdi_perso_engine_get_sim_data(GSDI_SLOT_1,
                                                 &sim_data,
                                                 &sim_mem_pointer);
  }

  if ( gsdi_status != GSDI_SUCCESS )
  {
    /* An error occurred getting the  data
       All memory will be freed.
    */
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get SIM Data",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(sim_mem_pointer.memory_start);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
       Now send the data to the security component
       of the SIM Lock to perform the validation
       of the data provided.
      ------------------------------------------------------------------------*/
#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  gsdi_status = gsdi_perso_security_handle_request(feature_type,
                                                   client_data,
                                                   &encrypted_data,
                                                   &sim_data,
                                                   ret_data_p,
                                                   ret_data_len_p,
                                                   perso_event_p);
  if(gsdi_status == GSDI_SUCCESS)
  {
    MSG_HIGH("PERSO: SECURITY 0x%x:  SUCCESS",feature_type,0,0);
  }
  else
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: SECURITY 0x%x: FAILURE 0x%x",
                           feature_type,gsdi_status,0);
  }

#else
  gsdi_status = gsdi_perso_shim_handle_request(feature_type,
                                               client_data,
                                               &encrypted_data,
                                               &sim_data,
                                               ret_data_p,
                                               ret_data_len_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO:  SHIM   FAILURE",0,0,0);
  }
#endif /* !FEATURE_MMGSDI_PERSONALIZATION_NON_QC */


#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
   /* --------------------------------------------------------------------------
     See if any data was mofified.  If so, write it back to FS
     ------------------------------------------------------------------------*/
  if(encrypted_data.data_mod.update_fs || encrypted_data.data_mod.refresh)
  {
    if(GSDI_SUCCESS !=  gsdi_perso_engine_write_back_fs(&encrypted_data))
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not write back FS",0,0,0);
      gsdi_status = GSDI_ERROR;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(sim_mem_pointer.memory_start);
 #endif /*FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
  return gsdi_status;
}

#endif /*FEATURE_PERSO_SIM*/
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_HANDLE_REQUEST

DESCRIPTION
  This function is the main handler for all the perso requests.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_handle_request(
  gsdi_slot_id_type            slot,
  const void *                 req_p,
  byte                         msg_id,
  uint8 *                      ret_data_p,
  int32*                       ret_data_len_p,
  gsdi_perso_event_enum_type * perso_event_p
)
{
  /*lint -esym(613,ret_data_len_p)*/
  gsdi_returns_T                     gsdi_status        = GSDI_SUCCESS;
  gsdi_perso_sec_client_req_type     client_data;            /* Client info  */
  gsdi_perso_engine_mem_pointer_type client_mem_pointer;
  uint32                             total_mem_req      = 0x00;
  gsdi_perso_engine_proc_enum_type   feature_type;
  boolean                            umts = FALSE;
  boolean                            ruim = FALSE;

  /* ------------------------------------------------------------------------
     Conditionally check the input parameters.  OK to have
     req_p of NULL if we are going to get all feature indicators
     since this is an internal request generated by our internal
     initialization procedures.
     ----------------------------------------------------------------------*/
  if ( msg_id != GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS_REQ)
  {
    MMGSDI_RETURN_IF_NULL(req_p);
  }

  MMGSDI_RETURN_IF_NULL(perso_event_p);

  memset(&client_mem_pointer,0x00, sizeof(gsdi_perso_engine_mem_pointer_type));
  memset(&client_data,0x00, sizeof(gsdi_perso_sec_client_req_type));

  *ret_data_len_p = 0;

  switch(msg_id)
  {

    case GSDI_PERSO_ACT_FEATURE_IND_REQ:
      feature_type = GSDI_PERSO_ENGINE_ACTIVATE_FEATURE;
      break;

    case GSDI_PERSO_DEACT_FEATURE_IND_REQ:
      feature_type = GSDI_PERSO_ENGINE_DEACTIVATE_FEATURE;
      break;

    case GSDI_PERSO_SET_FEATURE_DATA_REQ:
      feature_type = GSDI_PERSO_ENGINE_SET_DATA_FEATURE;
      break;

    case GSDI_PERSO_GET_FEATURE_IND_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_FEATURE_IND;
      break;

    case GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ:
      feature_type = GSDI_PERSO_ENGINE_UNBLOCK_DCK;
      break;

    case GSDI_PERSO_OTA_DERPERSO_REQ:
      feature_type = GSDI_PERSO_ENGINE_OTA_DERPERSO;
      break;

    case GSDI_PERSO_INT_PROC_REQ:
      feature_type = GSDI_PERSO_ENGINE_INT_PROC;
      break;

    case GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ:
      feature_type = GSDI_PERSO_ENGINE_PERM_DISABLE_FEATURE_IND;
      break;

    case GSDI_PERSO_GET_PERM_FEATURE_IND_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_PERM_FEATURE_IND;
      break;

    case GSDI_PERSO_GET_FEATURE_KEY_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_FEATURE_KEY;
      break;

    case GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS;
      break;

    case GSDI_PERSO_ENGINE_CHECK_ALL_LOCKS_REQ:
      feature_type = GSDI_PERSO_ENGINE_CHECK_LOCKS_ALL;
      break;

     case GSDI_PERSO_ENGINE_CHECK_AUTOLOCKS_REQ:
      feature_type = GSDI_PERSO_ENGINE_AUTO_LOCK;
      break;

     case GSDI_PERSO_ENGINE_RUIM_CHECK_ALL_LOCKS_REQ:
      feature_type = GSDI_PERSO_ENGINE_RUIM_CHECK_LOCKS_ALL;
      break;

    case GSDI_PERSO_GET_DCK_NUM_RETRIES_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_DCK_NUM_RETRIES;
      break;

    case GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_DCK_UNBLOCK_NUM_RETRIES;
      break;

    case GSDI_PERSO_ENGINE_RUIM_AUTOLOCK_REQ:
      feature_type = GSDI_PERSO_ENGINE_RUIM_AUTO_LOCK;
      break;

    case GSDI_PERSO_ENGINE_SIM_REFRESH_REQ:
      feature_type = GSDI_PERSO_ENGINE_REFRESH;
      break;

    case GSDI_PERSO_VERIFY_PHONE_CODE_REQ:
      feature_type = GSDI_PERSO_ENGINE_VERIFY_PHONE_CODE;
      break;

    case GSDI_PERSO_CHANGE_PHONE_CODE_REQ:
      feature_type = GSDI_PERSO_ENGINE_CHANGE_PHONE_CODE;
      break;

    case GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ:
      feature_type = GSDI_PERSO_ENGINE_UNBLOCK_PHONE_CODE;
      break;

    case GSDI_PERSO_SET_PHONE_CODE_REQ:
      feature_type = GSDI_PERSO_ENGINE_SET_PHONE_CODE;
      break;

    case GSDI_PERSO_GET_PHONE_CODE_STATUS_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_PHONE_CODE_STATUS;
      break;
    
    case GSDI_PERSO_GET_FEATURE_DATA_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_FEATURE_DATA;
      break;

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

    case GSDI_PERSO_GET_LIB_VERSION_REQ:
      feature_type = GSDI_PERSO_ENGINE_GET_SW_LIB_VERSION;
      break;

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

    default:
      MSG_ERROR("PERSO: Cannot Handle msg_id 0x%x",msg_id,0,0);
      return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     The Perso Engine Has an Interface to allow the Personalization
     Engine to determine how much memory needs to be allocated.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_engine_calc_mem_required(GSDI_PERSO_ENG_CALC_CLIENT_DATA,
                                                    feature_type,
                                                    (void *)req_p,
                                                    (int32*)&total_mem_req);

  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get total memory required",0,0,0);
    return gsdi_status;
  }

  if(total_mem_req > 0)
  {
    /* ---------------------------------------------------------------------------
       Allocate the entire buffer required for the encrypted data structure
       Memset it to 0x00 when successful
       Set memory_pointer.memory_curr = memory_pointer.memory_start
       Set memory_pointer.memory_end  = memory_pointer.memory_start + total_mem_req
       -------------------------------------------------------------------------*/
    client_mem_pointer.memory_start = mmgsdi_malloc(total_mem_req);
    if(client_mem_pointer.memory_start == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL pointer in gsdi_perso_engine_handle_request",0,0,0);
      return GSDI_ERROR;
    }
    memset(client_mem_pointer.memory_start, 0x00, total_mem_req);

    client_mem_pointer.memory_curr = client_mem_pointer.memory_start;
    client_mem_pointer.memory_end  = client_mem_pointer.memory_start + total_mem_req;

    /* --------------------------------------------------------------------------
       Get the client data from the request.
       ------------------------------------------------------------------------*/
    gsdi_status = gsdi_perso_engine_get_client_data(feature_type,
                                                    &client_data,
                                                    &client_mem_pointer,
                                                    (void *)req_p);
    if(gsdi_status != GSDI_SUCCESS)
    {
      /* An error occurred getting the  data
       All memory will be freed.
      */
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get Client Data",0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_mem_pointer.memory_start);

      return gsdi_status;
    }
  }

  gsdi_perso_engine_get_technology(feature_type,
                                   client_data.feature_mask,
                                   &umts,
                                   &ruim);

#ifdef FEATURE_PERSO_SIM
  if(umts)
  {
    gsdi_status = gsdi_perso_engine_handle_umts_request(feature_type,
                                                        &client_data,
                                                        req_p,
                                                        ret_data_p,
                                                        ret_data_len_p,
                                                        perso_event_p);
    if(gsdi_status != GSDI_SUCCESS)
    {
      MSG_ERROR("PERSO: UMTS Personalization failed 0x%x", gsdi_status,0,0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_mem_pointer.memory_start);
      return gsdi_status;
    }
  }
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_RUIM
  if(ruim)
  {
    gsdi_status = gsdi_perso_engine_handle_ruim_request(feature_type,
                                                        &client_data,
                                                        req_p,
                                                        ret_data_p,
                                                        ret_data_len_p,
                                                        perso_event_p);
    if(gsdi_status != GSDI_SUCCESS)
    {
      MSG_ERROR("PERSO: RUIM Personalization failed 0x%x", gsdi_status,0,0);
    }
  }
#endif /*FEATURE_PERSO_RUIM*/

  /* --------------------------------------------------------------------------
     Free Up all memory allocated during these procedures
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_mem_pointer.memory_start);
  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_GET_ALL_LOCKS

DESCRIPTION
  This function will get all of the Personalization Locks and determine
  whether or not a Personalization Check is required for each lock.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_get_all_locks(
  int32                      * ret_data_len_p,
  uint8                      * ret_data_p,
  gsdi_perso_event_enum_type * perso_event_p
)
{
  gsdi_returns_T         gsdi_status        = GSDI_SUCCESS;
  gsdi_perso_get_feature_ind_req_T * req_p        = NULL;
  byte                   msg_id             = 0x00;

  /* --------------------------------------------------------------------------
     Validate the Parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);

  /* --------------------------------------------------------------------------
     build a makeshift get feature indicators req for the purpose of handling
     the request properly
     ------------------------------------------------------------------------*/
  req_p = mmgsdi_malloc(sizeof(gsdi_perso_get_feature_ind_req_T));

  if ( req_p == NULL )
  {
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Now reuse the Perso Engine Handle Request to gather all data
     and send to the security library so that the feature indicators can
     be returned.
     ------------------------------------------------------------------------*/
  msg_id = GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS_REQ;
  req_p->message_header.message_id  = msg_id;

  gsdi_status = gsdi_perso_engine_handle_request(GSDI_SLOT_1,
                                                 req_p,
                                                 msg_id,
                                                 ret_data_p,
                                                 ret_data_len_p,
                                                 perso_event_p);

  MMGSDIUTIL_TMC_MEM_FREE(req_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not get all feature indicators: 0x%x",
                           gsdi_status,0,0);

  }

  return gsdi_status;

}


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_ANALYZE_INDICATORS

DESCRIPTION
  This function is used to analyze the indicators.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_analyze_indicators(
  gsdi_perso_eng_init_state_enum_type curr_state,
  const uint8 *                       lock_data_p,
  const int32 *                       lock_ret_data_len_p,
  gsdi_perso_internal_command_req_T   *req_p,
  gsdi_perso_eng_init_state_enum_type *next_state_p
)
{
  int32 index = 0;
  boolean nok   = FALSE;

  /* --------------------------------------------------------------------------
     Validate all parameters provided to this function
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_p);
  MMGSDI_RETURN_IF_NULL(next_state_p);
  MMGSDI_RETURN_IF_NULL(lock_data_p);
  MMGSDI_RETURN_IF_NULL(lock_ret_data_len_p);
  if ( *lock_ret_data_len_p > GSDI_PERSO_SEC_MAX_NUM_INDS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Too many locks indicated 0x%x",
                           *lock_ret_data_len_p,0,0);
  }

  /* --------------------------------------------------------------------------
     Initialize the req_p lock masks
     ------------------------------------------------------------------------*/
  req_p->active_lock_mask = 0x00;
  req_p->autolock_mask    = 0x00;

  /* --------------------------------------------------------------------------
     Now validate the Data Provided
     ------------------------------------------------------------------------*/
  while ( index < *lock_ret_data_len_p )
  {
    switch ( lock_data_p[index] )
    {
      case GSDI_PERSO_ENG_NOT_ACTIVE_STATE:
      case GSDI_PERSO_ENG_AUTOLOCK_STATE:
      case GSDI_PERSO_ENG_ACTIVE_STATE:
      case GSDI_PERSO_ENG_UNLOCKED_STATE:
      case GSDI_PERSO_ENG_DISABLED_STATE:
        nok |= FALSE;
        break;

      default:
        nok |= TRUE;
        break;
    }
    index++;
  }

  /* --------------------------------------------------------------------------
     Check to see if the data is ok.  if not, exit out but first set the
     next state to the Error State
     ------------------------------------------------------------------------*/
  if ( nok == TRUE )
  {
#ifdef FEATURE_PERSO_RUIM
    /*Error - GSM Could not be processed, continue with RUIM*/
    if(curr_state == GSDI_PERSO_ENG_INIT_S)
    {
      *next_state_p = GSDI_PERSO_ENG_CDMA_INIT_S;
      return GSDI_ERROR;
    }
#endif /*FEATURE_PERSO_RUIM*/
    *next_state_p = GSDI_PERSO_ENG_INIT_ERROR_S;
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Now set the autolock lock states as indicated by the data
     ------------------------------------------------------------------------*/
  index = 0;
  req_p->autolock_mask =0;

  while ( index < *lock_ret_data_len_p )
  {
    if ( lock_data_p[index] == (int)GSDI_PERSO_ENG_AUTOLOCK_STATE)
    {
      switch ( index )
      {
#ifdef FEATURE_PERSO_SIM
        case GSDI_PERSO_SEC_IND_OFFSET_NW_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_NW;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_NS_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_NS;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_SP_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_SP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_CP_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_CP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_SIM_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_SIM;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_PROP1_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_PROP1;
          break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW1_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW2_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_HRPD_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_SP_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_CP_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_RUIM_OFFSET:
          req_p->autolock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM;
          break;
#endif /* FEATURE_PERSO_RUIM */
        default:
          break;
      }
    }

    index++;
  }


  /* --------------------------------------------------------------------------
     Move the state to the next state.
     If there is an item set in the autolock mask, then it needs to be updated
     against the data in the File Syste.
     ------------------------------------------------------------------------*/
  if ( req_p->autolock_mask != 0 )
  {
    if(curr_state == GSDI_PERSO_ENG_INIT_S)
    {
      *next_state_p = GSDI_PERSO_ENG_INIT_AUTOLOCK_S;
    }
#ifdef FEATURE_PERSO_RUIM
    if(curr_state == GSDI_PERSO_ENG_CDMA_INIT_S)
    {
      *next_state_p = GSDI_PERSO_ENG_CDMA_INIT_AUTOLOCK_S;
    }
#endif /*FEATURE_PERSO_RUIM*/
    return GSDI_SUCCESS;
  }


  /* --------------------------------------------------------------------------
     Now set the active lock states as indicated by the data
     ------------------------------------------------------------------------*/
  index = 0;
  req_p->active_lock_mask =0;

  while ( index < *lock_ret_data_len_p )
  {
    if ( lock_data_p[index] == (int)GSDI_PERSO_ENG_ACTIVE_STATE)
    {
      switch ( index )
      {
#ifdef FEATURE_PERSO_SIM
        case GSDI_PERSO_SEC_IND_OFFSET_NW_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_NW;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_NS_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_NS;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_SP_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_SP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_CP_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_CP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_SIM_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_SIM;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_PROP1_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_PROP1;
          break;

#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW1_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW2_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_HRPD_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD;
          break;

        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_SP_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_CP_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP;
          break;


        case GSDI_PERSO_SEC_IND_OFFSET_RUIM_RUIM_OFFSET:
          req_p->active_lock_mask |= GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM;
          break;
#endif /* FEATURE_PERSO_RUIM */


        default:
          break;
      }
    }

    index++;
  }
  /* --------------------------------------------------------------------------
     Move the state to the next state.
     If there is an item set in the lock mask, then it needs to be "checked"
     against the data in the File sytem.
     If there are not items set in the lock mask, then we need to check for
     Autolocks by moving to the Autolock state.
     ------------------------------------------------------------------------*/
  if ( req_p->active_lock_mask != 0 )
  {
    if(curr_state == GSDI_PERSO_ENG_INIT_S)
    {
      *next_state_p = GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S;
    }

#ifdef FEATURE_PERSO_RUIM
    if(curr_state == GSDI_PERSO_ENG_CDMA_INIT_S)
    {
      *next_state_p = GSDI_PERSO_ENG_CDMA_INIT_CHECK_LOCKS_S;
    }
#endif /*FEATURE_PERSO_RUIM*/
    return GSDI_SUCCESS;
  }

  /* --------------------------------------------------------------------------
     If we get this far, this means that the indicators are not in Active State
     or in Autolock State.  This means that we are pretty much done with
     the Personalization Intitialization procedures
     ------------------------------------------------------------------------*/
#ifdef FEATURE_PERSO_RUIM
  if(curr_state == GSDI_PERSO_ENG_INIT_S)
  {
    *next_state_p = GSDI_PERSO_ENG_CDMA_INIT_S;
    return GSDI_SUCCESS;
  }
#endif /*FEATURE_PERSO_RUIM*/

  *next_state_p = GSDI_PERSO_ENG_INIT_COMPLETED_S;
  return GSDI_SUCCESS;
  /*lint -esym(715,curr_state)*/
}

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_SIM_SECURE_STORE

DESCRIPTION
  This function is called when file needed to be written to the secured EFS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_sim_secure_store(
  gsdi_sim_secure_store_req_T *secure_store_req,
  int32                       *ret_data_len_p)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uint8 *        data_p      = NULL;
  int32            file_size    = 0;
  int32          write_len   = 0;
  int32          ret_len     = 0;

  MMGSDI_RETURN_IF_NULL(secure_store_req);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(secure_store_req->data_buf_ptr);

  gsdi_status = gsdi_simsecure_get_file_size(&file_size);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: gsdi_simsecure_get_file_size failed 0x%x",
                           gsdi_status,0,0);
    return gsdi_status;
  }

  data_p = mmgsdi_malloc(file_size);

  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL pointer in gsdi_perso_engine_sim_secure_store",0,0,0);
    return GSDI_ERROR;
  }
  memset(data_p,0x00,file_size);

  if ( ( secure_store_req->data_len + GSDI_SIMSECURE_PARTITION_STATE_SIZE ) > GSDI_SIMSECURE_MAX_FILE_SIZE ||
         secure_store_req->data_len == 0 )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO:  Wrong data length %d",
                           secure_store_req->data_len,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Adust the write length to write the hdr + data
     ------------------------------------------------------------------------*/
  write_len = secure_store_req->data_len + GSDI_SIMSECURE_PARTITION_STATE_SIZE;

  data_p[GSDI_SIMSECURE_UINT8_STATE_OFFSET] = GSDI_SIM_PARTITION_READ_ONLY_S;

  /* --------------------------------------------------------------------------
     When a readonly on the partition is ready for enforcement, the code
     to do so will require an if around the memcpy and the write to
     flash
     ------------------------------------------------------------------------*/
  memcpy(&data_p[GSDI_SIMSECURE_PARTITION_STATE_SIZE],
         secure_store_req->data_buf_ptr,
         secure_store_req->data_len);



  MSG_HIGH("PERSO: Engine SIM Secure Store", 0, 0, 0);

  #ifdef FEATURE_MMGSDI_LTK

  gsdi_status = mmgsdi_ltk_simsecure_write_to_flash(data_p,
                                                    write_len,
                                                    &ret_len);

  #else

  gsdi_status =  gsdi_simsecure_write_to_flash(data_p,
                                               write_len,
                                               &ret_len);

  #endif /* FEATURE_MMGSDI_LTK */

  if ( gsdi_status == GSDI_SUCCESS )
  {
    *ret_data_len_p = ret_len - GSDI_SIMSECURE_PARTITION_STATE_SIZE;
  }
  memset(data_p,0x00,file_size);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data_p);

  return gsdi_status;

} /* gsdi_perso_engine_sim_secure_store */

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#ifdef FEATURE_MMGSDI_DEBUG_SIM_PARTITION
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_SIM_SECURE_READ

DESCRIPTION
  This function is called when file needed to be read from the secured EFS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_sim_secure_read(
  gsdi_sim_secure_read_req_T  *secure_read_req,
  int32                       *ret_data_len_p,
  uint8                       *ret_data_p)
{
  gsdi_returns_T  gsdi_status   = GSDI_SUCCESS;
  int32           len           = GSDI_PERSO_SHIM_TOTAL_SIM_PARTITION_SIZE;
  int32           ret_len       = 0;
  uint8 *         temp_buffer_p = NULL;

  MMGSDI_RETURN_IF_NULL(secure_read_req);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);

  MSG_HIGH("PERSO: Engine SIM Secure Read", 0, 0, 0);

  /* --------------------------------------------------------------------------
     Secure Memory space to hold the entire SIM Partition Contents
     ------------------------------------------------------------------------*/
  temp_buffer_p = mmgsdi_malloc(GSDI_PERSO_SHIM_TOTAL_SIM_PARTITION_SIZE);
  if ( temp_buffer_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL pointer in gsdi_perso_engine_sim_secure_read",0,0,0);
    return GSDI_ERROR;
  }

  memset(temp_buffer_p,0x00,GSDI_PERSO_SHIM_TOTAL_SIM_PARTITION_SIZE);

  gsdi_status =  gsdi_simsecure_read_from_flash(temp_buffer_p,
                                                GSDI_PERSO_SHIM_TOTAL_SIM_PARTITION_SIZE,
                                                &ret_len);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(temp_buffer_p);
    *ret_data_len_p = 0;
    MMGSDI_DEBUG_MSG_ERROR("PERSO: gsdi_simsecure_read_from_flash failed 0x%x",
                           gsdi_status,0,0);
    return gsdi_status;
  }

  /* Adjust the read contents to remove the header information */
  *ret_data_len_p = ret_len - GSDI_PERSO_SHIM_HDR_PARTITION_SIZE;

  memcpy(ret_data_p,
         (temp_buffer_p + GSDI_PERSO_SHIM_HDR_PARTITION_SIZE),
         (ret_len - GSDI_PERSO_SHIM_HDR_PARTITION_SIZE));

  MMGSDIUTIL_TMC_MEM_FREE(temp_buffer_p);

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_sim_secure_read */

#endif /* FEATURE_MMGSDI_DEBUG_SIM_PARTITION */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_ROOT_KEY_WRITE

DESCRIPTION
  This function is called when root key is to be written to the OTP

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_root_key_write(
  gsdi_perso_root_key_write_req_T *root_key_write_req,
  uint8                           *data_p,
  int32                           *ret_data_len_p)
{
  FlashOTP_ReturnType                 status;

  MMGSDI_RETURN_IF_NULL(root_key_write_req);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(data_p);
  MMGSDI_RETURN_IF_NULL(root_key_write_req->data_buf_ptr);

  MSG_HIGH("PERSO: Engine root key write", 0, 0, 0);

  #ifdef FEATURE_MMGSDI_LTK

  status = mmgsdi_ltk_flash_otp_status();

  #else

  status = flash_otp_status();

  #endif /* FEATURE_MMGSDI_LTK */

  if ( status != FLASH_OTP_PROTECTION_INACTIVE )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Incorrect FlashOTP status 0x%x",
                           status,0,0);
    return GSDI_ERROR;
  }
  #ifdef FEATURE_MMGSDI_LTK

  status = mmgsdi_ltk_flash_otp_operation(FLASH_OTP_OP_ROOTKEY_WRITE,
                                          (uint8 *)root_key_write_req->data_buf_ptr);

  #else

  status = flash_otp_operation(FLASH_OTP_OP_ROOTKEY_WRITE,
                                 (uint8 *)root_key_write_req->data_buf_ptr);

  #endif /* FEATURE_MMGSDI_LTK */

  if (status == FLASH_OTP_IO_SUCCESS)
  {
    *ret_data_len_p = 260;
    memcpy(data_p,root_key_write_req->data_buf_ptr,260);
    return GSDI_SUCCESS;
  }
  else
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: ROOTKEY_WRITE FAILURE: 0x%x",
                           status,0,0);
    *ret_data_len_p = 0;
    return GSDI_ERROR;
  }

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_root_key_write */

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_ROOT_KEY_READ

DESCRIPTION
  This function is called when root key is to be READ from the OTP

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_root_key_read(
  gsdi_perso_root_key_read_req_T  *root_key_read_req,
  uint8                           *data_p,
  int32                           *data_len_p)
{
  FlashOTP_ReturnType                 status;

  MMGSDI_RETURN_IF_NULL(root_key_read_req);
  MMGSDI_RETURN_IF_NULL(data_p);

  MSG_HIGH("PERSO: Engine root key read", 0, 0, 0);

  #ifdef FEATURE_MMGSDI_LTK

  status = mmgsdi_ltk_flash_otp_operation(FLASH_OTP_OP_ROOTKEY_READ,
                                          data_p);

  #else

  status = flash_otp_operation(FLASH_OTP_OP_ROOTKEY_READ,
                                data_p);

  #endif /* FEATURE_MMGSDI_LTK */

  if (status == FLASH_OTP_IO_SUCCESS)
  {
    *data_len_p = 260;
    return GSDI_SUCCESS;
  }

  MMGSDI_DEBUG_MSG_ERROR("PERSO: ROOTKEY read failed 0x%x",status,0,0);
  return GSDI_ERROR;
} /* gsdi_perso_engine_root_key_read*/

/* ============================================================================
FUNCTION:  GSDI_PERSO_ENGINE_OTP_IS_ACTIVATED

DESCRIPTION:
  Function is scalled to determine whether or not the OTP has been activated.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  boolean:         TRUE:   OTP is activated or status unknonw.
                   FALSE:  OTP is not activated

=============================================================================*/
boolean gsdi_perso_engine_otp_is_activated(
  void
)
{
  FlashOTP_ReturnType                 status;
  /* --------------------------------------------------------------------------
     Call the Flash OTP Drivers to determine the state of the  activation
     flag maintained in the OTP Block
     ------------------------------------------------------------------------*/
  status = flash_otp_status();

  if ( status == FLASH_OTP_PROTECTION_INACTIVE )
  {
    return FALSE;
  }

  return TRUE;
}


#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
/* ============================================================================
FUNCTION:  GSDI_PERSO_ENGINE_SET_REQ

DESCRIPTION:
  Based on the current state , the msg_id is set appropriately

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T
=============================================================================*/
static gsdi_returns_T gsdi_perso_engine_set_req(
  gsdi_perso_eng_init_state_enum_type curr_state,
  byte                                *msg_id)
{
  switch(curr_state)
  {
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S:
      *msg_id = GSDI_PERSO_ENGINE_CHECK_ALL_LOCKS_REQ;
      break;
    case GSDI_PERSO_ENG_INIT_AUTOLOCK_S:
      *msg_id = GSDI_PERSO_ENGINE_CHECK_AUTOLOCKS_REQ;
       break;
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_ENG_CDMA_INIT_CHECK_LOCKS_S:
      *msg_id = GSDI_PERSO_ENGINE_RUIM_CHECK_ALL_LOCKS_REQ;
      break;
    case GSDI_PERSO_ENG_CDMA_INIT_AUTOLOCK_S:
      *msg_id = GSDI_PERSO_ENGINE_RUIM_AUTOLOCK_REQ;
      break;
#endif /*FEATURE_PERSO_RUIM*/
    default:
      return GSDI_ERROR;
  }
  return GSDI_SUCCESS;
} /* gsdi_perso_engine_set_req */


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_BUILD_AND_QUEUE_INIT_PROC_REQ

DESCRIPTION
  This function is used to build a PERSO INIT PROC request and put it in 
  the GSDI command queue

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_build_and_queue_init_proc_req(
  gsdi_slot_id_type                    slot,
  gsdi_perso_internal_command_req_T   *req_ptr
)
{
  gsdi_returns_T                      gsdi_status  = GSDI_ERROR;

  MMGSDI_RETURN_IF_NULL(req_ptr);

  /* ----------------------------------------------------------------------
     Queue an internal command to process in the INIT S so that the
     Engine can retrieve the Lock Indicators
  --------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN(sizeof(gsdi_perso_internal_command_req_T),&req_ptr->message_header);
  req_ptr->message_header.message_set = MS_GSDI;
  req_ptr->message_header.message_id  = GSDI_PERSO_INT_PROC_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  req_ptr->message_header.tcb_ptr     = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  req_ptr->message_header.wait_sig    = 0x00;
  req_ptr->message_header.slot        = slot;

  gsdi_status = gsdi_cmd_ext((void *)req_ptr);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: COULD NOT QUEUE PERSO INIT PROC COMMAND, 0x%x",
      gsdi_status,0,0);
  }
  return gsdi_status;
} /* gsdi_perso_engine_build_and_queue_init_proc_req */


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_MAIN

DESCRIPTION
  This function is called after the Verification procedures have occurred on
  PIN1.

  POSSIBLE STATES:
  GSDI_PERSO_ENG_NOT_INIT_S         -> GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S
  GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S -> GSDI_PERSO_ENG_INIT_AUTOLOCK_S
  GSDI_PERSO_ENG_INIT_AUTOLOCK_S    -> GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S
  GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S -> GSDI_PERSO_ENG_INIT_S

  or
  GSDI_PERSO_ENG_NOT_INIT_S         -> GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S
  GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S -> GSDI_PERSO_ENG_INIT_S

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_main(
  gsdi_slot_id_type slot,
  const void *      cmd_p,
  byte              msg_id,
  uint8 *           ret_data_p,
  int32 *           ret_data_len_p
)
{
  gsdi_returns_T                      gsdi_status  = GSDI_ERROR;
  gsdi_perso_internal_command_req_T   req;
  gsdi_perso_event_enum_type          perso_event  = GSDI_PERSO_NO_EVENT;
  gsdi_perso_eng_init_state_enum_type next_state   = GSDI_PERSO_ENG_NOT_INIT_S;
  gsdi_perso_eng_init_state_enum_type curr_state   = gsdi_data.perso_engine_state;
  #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  boolean                             sim_partition_init = TRUE;
  #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

  MSG_HIGH("PERSO: MMGSDI PERSO ENG MAIN: Perso state 0x%x",
           gsdi_data.perso_engine_state,0,0);

  if(gsdi_data.gsdi_state == GSDI_CARD_ERROR_S)
  {
     gsdi_data.perso_engine_state = GSDI_PERSO_ENG_CARD_ERROR_S;
  }

  /* --------------------------------------------------------------------------
     Check the Personalization State of MMGSDI.
     ------------------------------------------------------------------------*/
  switch ( gsdi_data.perso_engine_state )
  {
    case GSDI_PERSO_ENG_NOT_INIT_S:
       MSG_HIGH("PERSO: GSDI_PERSO_ENG_NOT_INIT_S",0,0,0); 
   /* ----------------------------------------------------------------------
         Check the cmd_p.  Personalization Commands are not allowed in this
         state.
         --------------------------------------------------------------------*/
      if ( cmd_p != NULL )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Cannot process command in state 0x%x",
                               gsdi_data.perso_engine_state,0,0);
        return GSDI_ERROR;
      }

      #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

      /* ------------------------------------------------------------------------
         Init the SIM Secure Partition and Security Library
         ----------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_main_init(slot,
                                                &sim_partition_init);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: File System Error",0,0,0);
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        return gsdi_status;
      }

      gsdi_perso_eng_efs_int = TRUE;
      /* ------------------------------------------------------------------------
         At this state, the mobile equipment will initialize
         Personalization Procedures.
         ----------------------------------------------------------------------*/
      if ( gsdi_data.gsdi_state != GSDI_CARD_ERROR_S)
      {
        gsdi_status = gsdi_perso_engine_initialize_data(slot);

        if (gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: PERSO INITIALIZE DATA FAILED: 0x%x",
                                 gsdi_status,0,0);
          /* ------------------------------------------------------------------
             Not much else we can do but go to Card Error as
             important initialization data can not be read.
             ----------------------------------------------------------------*/
          gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                       GSDI_PERSO_SANITY_ERROR);
          return gsdi_status;
        }
      }
      /* ------------------------------------------------------------------------
         Move the State to GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S
         ----------------------------------------------------------------------*/
      if ( sim_partition_init )
      {
        gsdi_data.perso_engine_state = GSDI_PERSO_ENG_INIT_S;
      }
      else
      {
        gsdi_data.perso_engine_state = GSDI_PERSO_ENG_INIT_COMPLETED_S;

        #ifdef FEATURE_MMGSDI_REQUIRE_SIM_PARTITION
        /* Not acceptable to have a SIM Partition that is not
        ** initialized
        */
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        return GSDI_ERROR;

        #endif /* FEATURE_MMGSDI_REQUIRE_SIM_PARTITION */
      }

      #else
      gsdi_status = gsdi_perso_engine_initialize_data(slot);
      if (gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: PERSO INITIALIZE DATA FAILED: 0x%x",
                                gsdi_status,0,0);
        /* ------------------------------------------------------------------
         Not much else we can do but go to Card Error as
         important initialization data can not be read.
        ----------------------------------------------------------------*/
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                    GSDI_PERSO_SANITY_ERROR);
        return gsdi_status;
      }

      gsdi_perso_engine_set_state(GSDI_PERSO_ENG_INIT_S,
                                  curr_state,
                                  &gsdi_data.perso_engine_state);

      #endif /* !FEATURE_MMGSDI_PERSONALIZATION_NON_QC */


      /* ----------------------------------------------------------------------
         Queue an internal command to process in the INIT S so that the
         Engine can retrieve the Lock Indicators
         --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_build_and_queue_init_proc_req(
                      GSDI_SLOT_1, &req);
      break;

    case GSDI_PERSO_ENG_CARD_ERROR_S:
      MSG_HIGH("PERSO: GSDI_PERSO_ENG_CARD_ERROR_S",0,0,0);

      if ( cmd_p == NULL )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL Pointer in gsdi_perso_engine_main",0,0,0);
        return GSDI_ERROR;
      }

      /* Do not process any Commands except get_feature_ind/activate/deactivate/
         set_feature_data /get_feature_data & get_feature_key in this state */
      if (((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id !=
            GSDI_PERSO_GET_FEATURE_IND_REQ &&
          ((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id !=
            GSDI_PERSO_ACT_FEATURE_IND_REQ &&
          ((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id !=
            GSDI_PERSO_DEACT_FEATURE_IND_REQ &&
          ((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id !=
            GSDI_PERSO_GET_FEATURE_DATA_REQ &&
          ((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id !=
            GSDI_PERSO_GET_FEATURE_KEY_REQ &&
          ((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id !=
            GSDI_PERSO_SET_FEATURE_DATA_REQ)
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Cannot process command in state 0x%x",
                               gsdi_data.perso_engine_state,0,0);
        return GSDI_ERROR;
      }

      /* ---------------------------------------------------------------
         This state indicates that the card is not present. No operations 
         can be performed but the feature indicators can be requested
         -------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_handle_request(slot,
                                                     cmd_p,
                                                     msg_id,
                                                     ret_data_p,
                                                     ret_data_len_p,
                                                     &perso_event);
      if ( gsdi_status == GSDI_ERROR )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        MSG_ERROR("PERSO: gsdi_perso_engine_handle_request failed 0x%x : msg_id 0x%x",
                    gsdi_status, msg_id, 0);
        return GSDI_ERROR;
      }

      if ( perso_event != GSDI_PERSO_NO_EVENT )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     perso_event);
      }
      break;
    case GSDI_PERSO_ENG_INIT_S:
    case GSDI_PERSO_ENG_CDMA_INIT_S:
      MSG_HIGH("PERSO: GSDI_PERSO_ENG_INIT_S",0,0,0);
      /* ------------------------------------------------------------------------
         Now perform the checks for all locks.
         If GSDI_STATUS == GSDI_ERROR, this is a Procedural failure while trying
         to get to the security library.  Need to go to NO Card.
         Otherwise, check the event generated by the procedure and send to the
         clients if set.
         ----------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_get_all_locks(ret_data_len_p,
                                                    ret_data_p,
                                                    &perso_event);
      if ( gsdi_status == GSDI_ERROR )
      {
        MSG_ERROR("PERSO: Could not get all the locks",0,0,0); 
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        return GSDI_ERROR;
      }

      if ( perso_event != GSDI_PERSO_NO_EVENT )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     perso_event);
      }

      /* ----------------------------------------------------------------------
         Pass all information to the analyze indicators function.  It will
         do a couple of things.
         1.  Analyze the Feature Indicators provided to ensure they are valid
         2.  Once valid, the req_p is populated to set the feature mask.
         3.  The next state is then determined.
         --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_analyze_indicators(gsdi_data.perso_engine_state,
                                                         ret_data_p,
                                                         ret_data_len_p,
                                                         &req,
                                                         &next_state);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        MSG_ERROR("PERSO: Failed to Analyze Locks, status = 0x%x",
                  gsdi_status,0,0); 
        /* --------------------------------------------------------------------
           Failed to analyze the lock types, i.e. may be corrupted
           Move the state to Error State
           ------------------------------------------------------------------*/
          gsdi_perso_engine_send_sanity_error((gsdi_perso_eng_init_state_enum_type*)&gsdi_data.perso_engine_state);
      }

      gsdi_data.perso_engine_state = next_state;

       /* ----------------------------------------------------------------------
           Set the rest of the req_p data and Queue to the head of the
           MMGSDI Task Command Queue
           --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_build_and_queue_init_proc_req(
                      GSDI_SLOT_1, &req);
      break;

    case GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S:
    case GSDI_PERSO_ENG_CDMA_INIT_CHECK_LOCKS_S:
      MSG_HIGH("PERSO: GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S",0,0,0);
      /* ----------------------------------------------------------------------
         Check the cmd_p.  if  NULL...this is an indication that there is
         a problem somewhere.
         --------------------------------------------------------------------*/
      if ( cmd_p == NULL )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Cannot process NULL Command",
                               gsdi_data.perso_engine_state,0,0);
        return GSDI_ERROR;
      }

      req.feature =  (gsdi_perso_enum_type)(((gsdi_perso_internal_command_req_T*)cmd_p)->active_lock_mask);
      
      gsdi_status = gsdi_perso_engine_set_req(gsdi_data.perso_engine_state, &msg_id);
      if(gsdi_status != GSDI_SUCCESS)
      {
         MSG_ERROR("PERSO: Invalid request, Status = 0x%x",
                   gsdi_data.perso_engine_state,0,0);
         return gsdi_status;
      }


      /* ----------------------------------------------------------------------
        Send the command to the Perso Engine Handler.  If an error occurs...
        exit out immediately and go to Error State.

        Otherwise, determine if an event was generated by the Library and
        notify the clients.
      --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_handle_request(slot,
                                                     &req,
                                                     msg_id,
                                                     ret_data_p,
                                                     ret_data_len_p,
                                                     &perso_event);
      if ( gsdi_status == GSDI_ERROR )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Error occurred while doing a Check Locks",0,0,0);

        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        return GSDI_ERROR;
      }

      if ( perso_event != GSDI_PERSO_NO_EVENT )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     perso_event);
      }

      /* An Event was not generated or did not change state...therefore,
         if successful move to the next state.  */
      if ( ( gsdi_status == GSDI_SUCCESS ||
             gsdi_status == GSDI_PERSO_CURRENTLY_ACTIVE ) &&
              perso_event == GSDI_PERSO_NO_EVENT )
      {
        /* Reset the status to GSDI_SUCCESS */
        gsdi_status = GSDI_SUCCESS;
        gsdi_data.perso_engine_state = GSDI_PERSO_ENG_INIT_COMPLETED_S;
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_EVT_INIT_COMPLETED);
      }
      break;

    case GSDI_PERSO_ENG_INIT_AUTOLOCK_S:
    case GSDI_PERSO_ENG_CDMA_INIT_AUTOLOCK_S:
      MSG_HIGH("PERSO: GSDI_PERSO_ENG_INIT_AUTOLOCK_S",0,0,0);
      gsdi_status = gsdi_perso_engine_set_req(gsdi_data.perso_engine_state, &msg_id);
      if(gsdi_status != GSDI_SUCCESS)
      {
         MSG_ERROR("PERSO: Invalid request, Status = 0x%x",
                   gsdi_data.perso_engine_state,0,0);
         return gsdi_status;
      }

     /* ----------------------------------------------------------------------
         Check the cmd_p.  if  NULL...this is an indication that there is
         a problem somewhere.
         --------------------------------------------------------------------*/
      if ( cmd_p == NULL )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: Cannot process NULL Command",
                               gsdi_data.perso_engine_state,0,0);
        return GSDI_ERROR;

      }

      req.autolock_mask = ((gsdi_perso_internal_command_req_T*)cmd_p)->autolock_mask;
      req.message_header.message_id = msg_id;

      gsdi_status = gsdi_perso_engine_handle_request(slot,
                                                     &req,
                                                     msg_id,
                                                     ret_data_p,
                                                     ret_data_len_p,
                                                     &perso_event);
      if ( gsdi_status == GSDI_ERROR )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        MSG_ERROR("PERSO: gsdi_perso_engine_handle_request failed 0x%x msg_id 0x%x",
                    gsdi_status,msg_id,0);
        return GSDI_ERROR;
      }

      if ( perso_event != GSDI_PERSO_NO_EVENT )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     perso_event);
      }

      if ( gsdi_status == GSDI_SUCCESS             ||
           gsdi_status == GSDI_PERSO_PERM_DISABLED ||
           gsdi_status == GSDI_PERSO_CURRENTLY_DEACT)
      {

        gsdi_perso_engine_set_state(GSDI_PERSO_ENG_INIT_S,
                                    curr_state,
                                    &gsdi_data.perso_engine_state);
        /* ----------------------------------------------------------------------
           Set the rest of the req_p data and Queue to the head of the
           MMGSDI Task Command Queue
          --------------------------------------------------------------------*/
        gsdi_status = gsdi_perso_engine_build_and_queue_init_proc_req(
                        GSDI_SLOT_1, &req);
      }
      else
      {
        gsdi_perso_engine_send_sanity_error((gsdi_perso_eng_init_state_enum_type*)&gsdi_data.perso_engine_state);
      }
      break;

    case GSDI_PERSO_ENG_INIT_COMPLETED_S:
      MSG_HIGH("PERSO: GSDI_PERSO_ENG_INIT_COMPLETED_S",0,0,0);
      /* ---------------------------------------------------------------
         This state indicates that the Personalization procedures are now
         active.  The mobile will perform according to the personalization
         indicators.
         -------------------------------------------------------------*/
      if ( cmd_p == NULL )
      {
        /* NULL Command pointers are not acceptable in this state */
        MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL pointer in gsdi_perso_engine_main",0,0,0);
        return GSDI_ERROR;
      }

      if (((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id ==
          GSDI_PERSO_INT_PROC_REQ)
      {
        /* An Internal Command is taking us to Init Completed which
        ** is done as a result of the initialization procedures
        ** which signals the end of the Perso Init Procedures
        */
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_EVT_INIT_COMPLETED);
        return GSDI_SUCCESS;
      }

      /* ----------------------------------------------------------------------
         Process the command provided in the init completed state
         --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_handle_request(slot,
                                                     cmd_p,
                                                     msg_id,
                                                     ret_data_p,
                                                     ret_data_len_p,
                                                     &perso_event);
      if ( gsdi_status == GSDI_ERROR )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        MMGSDI_DEBUG_MSG_ERROR("PERSO: gsdi_perso_engine_handle_request failed 0x%x msg_id 0x%x"
                               ,gsdi_status,msg_id,0);
        return GSDI_ERROR;
      }

      if ( perso_event != GSDI_PERSO_NO_EVENT )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     perso_event);
      }

      return gsdi_status;

    case GSDI_PERSO_ENG_INIT_ERROR_S:
    case GSDI_PERSO_ENG_CDMA_INIT_ERROR_S:
      MSG_HIGH("PERSO: GSDI_PERSO_ENG_INIT_ERROR_S",0,0,0);

      #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

      if ( gsdi_perso_eng_efs_int == FALSE )
      {
        gsdi_perso_eng_efs_int = TRUE;

        /* --------------------------------------------------------------------
           Init the SIM Secure Partition and Security Library
        ------------------------------------------------------------------*/
        gsdi_status = gsdi_perso_engine_main_init(GSDI_SLOT_1,
                                                  &sim_partition_init);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: File System Error",0,0,0);
          return gsdi_status;
        }
      }

      #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
      /* ----------------------------------------------------------------------
         Send the command to the Perso Engine Handler
         --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_handle_request(slot,
                                                     cmd_p,
                                                     msg_id,
                                                     ret_data_p,
                                                     ret_data_len_p,
                                                     &perso_event);
      if ( gsdi_status == GSDI_ERROR )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        MSG_ERROR("PERSO: gsdi_perso_engine_handle_request failed 0x%x msg_id:",
                  gsdi_status,msg_id,0);
        return GSDI_ERROR;
      }
      else if (( gsdi_status == GSDI_SUCCESS) &&
               ( msg_id      == GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ ))
      {
        /* Successfully unblock a feature */
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     perso_event);
 
        /* ----------------------------------------------------------------------
          Set the rest of the req_p data and Queue to the head of the
          MMGSDI Task Command Queue
         --------------------------------------------------------------------*/
        gsdi_status = gsdi_perso_engine_build_and_queue_init_proc_req(
                        GSDI_SLOT_1, &req);
      }
      break;

    case GSDI_PERSO_ENG_WAIT_FOR_DEPERSO_S:
    case GSDI_PERSO_ENG_CDMA_WAIT_FOR_DEPERSO_S:
      MSG_HIGH("PERSO: GSDI_PERSO_ENG_WAIT_FOR_DEPERSO_S",0,0,0);

      if ( cmd_p == NULL )
      {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL pointer in gsdi_perso_engine_main",0,0,0);
        return GSDI_ERROR;
      }

      /* Do not process Internal Commands in this state */
      if (((gsdi_perso_internal_command_req_T *)cmd_p)->message_header.message_id ==
            GSDI_PERSO_INT_PROC_REQ)
      {
        /* An Internal Command is taking us to Init Completed which
        ** is done as a result of the initialization procedures
        ** which signals the end of the Perso Init Procedures
        */
        return GSDI_SUCCESS;
      }

      /* ---------------------------------------------------------------
         This state indicates that the Personalization procedures are now
         active.  The mobile will perform according to the personalization
         indicators.
         -------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_handle_request(slot,
                                                     cmd_p,
                                                     msg_id,
                                                     ret_data_p,
                                                     ret_data_len_p,
                                                     &perso_event);
      if ( gsdi_status == GSDI_ERROR )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     GSDI_PERSO_SANITY_ERROR);
        MSG_ERROR("PERSO: gsdi_perso_engine_handle_request failed 0x%x msg_id",
                  gsdi_status,msg_id,0);
        return GSDI_ERROR;
      }

      if ( perso_event != GSDI_PERSO_NO_EVENT )
      {
        gsdi_perso_engine_send_event(&gsdi_data.perso_engine_state,
                                     perso_event);
      }

      if (   gsdi_status == GSDI_SUCCESS &&
           ( msg_id      == GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ ||
             msg_id      == GSDI_PERSO_DEACT_FEATURE_IND_REQ        ||
             msg_id      == GSDI_PERSO_OTA_DERPERSO_REQ ))
      {
        /* ----------------------------------------------------------------------
          Set the rest of the req_p data and Queue to the head of the
          MMGSDI Task Command Queue
         --------------------------------------------------------------------*/
        gsdi_status = gsdi_perso_engine_build_and_queue_init_proc_req(
                        GSDI_SLOT_1, &req);
      }
      break;

    default:
      MSG_ERROR("PERSO: Unknown Perso State: 0x%x",gsdi_data.perso_engine_state,0,0);
      return GSDI_ERROR;
  }

  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_FS_DIR_TEST

DESCRIPTION
  This function is used to test whether the directory already exists
  in FS.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static boolean gsdi_perso_engine_fs_dir_test(
  const char * dir_to_test
)
{
#ifdef FEATURE_MMGSDI_PERSO_SFS
  uint8 buf[100];
  int len=100;

  len = sfs_priv_simloc_read(buf,len);

  if(len == 0)
    return FALSE;

  return TRUE;
#else
  gsdi_returns_T gsdi_status = GSDI_ERROR;
  if ( dir_to_test == NULL )
    return FALSE;

  gsdi_status = gsdi_efs_name_test(dir_to_test); /* Pointer to Directory Name */

  if ( gsdi_status == GSDI_SUCCESS )
    return TRUE;
  else
    return FALSE;
#endif /* FEATURE_MMGSDI_PERSO_SFS */

}


#ifndef FEATURE_MMGSDI_PERSO_SFS
#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_FS_FILE_TEST

DESCRIPTION
  This function is used to test whether the file already exists
  in FS.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static boolean gsdi_perso_engine_fs_file_test(
  const char * file_to_test
)
{
  gsdi_returns_T gsdi_status = GSDI_ERROR;

  gsdi_status = gsdi_efs_name_test(file_to_test); /* Pointer to File Name */

  if ( gsdi_status == GSDI_SUCCESS )
    return TRUE;
  else
    return FALSE;
}
#endif /*FEATURE_PERSO_SIM*/
#endif /* FEATURE_MMGSDI_PERSO_SFS */


#ifndef FEATURE_MMGSDI_PERSO_SFS
/*===========================================================================
FUNCTION GSDI_PERSO_ARE_ALL_FILES_PRESENT

DESCRIPTION
  This function is used to verify whether or not all files are present
  that are required for use in the file system in the personalization
  solution.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_are_all_files_present(
  void
)
{
#ifdef FEATURE_PERSO_SIM
  boolean present = TRUE;

  /* --------------------------------------------------------------------------
     Determine if the Feature Indicators File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_engine_fs_file_test(mmgsdi_efs_perso_file);
  if ( !present )
  {
      MMGSDI_DEBUG_ERR("EFS PERSO_FILE MISSING",0,0,0);
      return GSDI_NOT_FOUND;
  }
#endif

#ifdef FEATURE_PERSO_RUIM
  return (gsdi_perso_engine_ruim_fs_file_test());
#else
  return GSDI_SUCCESS;
#endif

}
#endif /*FEATURE_MMGSDI_PERSO_SFS */


#ifndef FEATURE_MMGSDI_PERSO_SFS
/*===========================================================================
FUNCTION GSDI_PERSO_ARE_ALL_OLD_FILES_PRESENT

DESCRIPTION
  This function is used to verify whether or not all files are present
  that are required for use in the file system in the personalization
  solution.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_are_all_old_files_present(
  void
)
{
#ifdef FEATURE_PERSO_SIM
#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
  char* mmgsdi_efs_feature_ind="/apps/mmgsdi/perso/feature_ind.txt";
  char * array_of_ck_files[5]  = {"/apps/mmgsdi/perso/nw_ck.txt",
                                  "/apps/mmgsdi/perso/ns_ck.txt",
                                  "/apps/mmgsdi/perso/sp_ck.txt",
                                  "/apps/mmgsdi/perso/cp_ck.txt",
                                  "/apps/mmgsdi/perso/sim_ck.txt"};
  char * array_of_code_files[5] = {"/apps/mmgsdi/perso/nw_codes.txt",
                                   "/apps/mmgsdi/perso/ns_codes.txt",
                                   "/apps/mmgsdi/perso/sp_codes.txt",
                                   "/apps/mmgsdi/perso/cp_codes.txt",
                                   "/apps/mmgsdi/perso/sim_codes.txt"};
  char *mmgsdi_efs_retries_rem = "/apps/mmgsdi/perso/num_retries_remaining.txt";
  char *mmgsdi_efs_perm_dis_ind= "/apps/mmgsdi/perso/perm_dis_feature_ind.txt";
  char *mmgsdi_efs_unblock_retries_rem = "/apps/mmgsdi/perso/unblk_retry_rem.txt";
  char * array_of_unblock_ck_files[5]  = {"/apps/mmgsdi/perso/nw_unblock_ck.txt",
                                          "/apps/mmgsdi/perso/ns_unblock_ck.txt",
                                          "/apps/mmgsdi/perso/sp_unblock_ck.txt",
                                          "/apps/mmgsdi/perso/cp_unblock_ck.txt",
                                          "/apps/mmgsdi/perso/sim_unblock_ck.txt"};
#else
  char* mmgsdi_efs_feature_ind="mmgsdi/perso/feature_ind.txt";
  char * array_of_ck_files[5]  = {"mmgsdi/perso/nw_ck.txt",
                                  "mmgsdi/perso/ns_ck.txt",
                                  "mmgsdi/perso/sp_ck.txt",
                                  "mmgsdi/perso/cp_ck.txt",
                                  "mmgsdi/perso/sim_ck.txt"};
  char * array_of_code_files[5] = {"mmgsdi/perso/nw_codes.txt",
                                   "mmgsdi/perso/ns_codes.txt",
                                   "mmgsdi/perso/sp_codes.txt",
                                   "mmgsdi/perso/cp_codes.txt",
                                   "mmgsdi/perso/sim_codes.txt"};
  char *mmgsdi_efs_retries_rem = "mmgsdi/perso/num_retries_remaining.txt";
  char *mmgsdi_efs_perm_dis_ind= "mmgsdi/perso/perm_dis_feature_ind.txt";
  char *mmgsdi_efs_unblock_retries_rem = "mmgsdi/perso/unblk_retry_rem.txt";
  char * array_of_unblock_ck_files[5]  = {"mmgsdi/perso/nw_unblock_ck.txt",
                                          "mmgsdi/perso/ns_unblock_ck.txt",
                                          "mmgsdi/perso/sp_unblock_ck.txt",
                                          "mmgsdi/perso/cp_unblock_ck.txt",
                                          "mmgsdi/perso/sim_unblock_ck.txt"};
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */
  boolean present = TRUE;
  uint32  index   = 0x00;

  /* --------------------------------------------------------------------------
     Determine if the Feature Indicators File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_engine_fs_file_test(mmgsdi_efs_feature_ind);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS FEATURE IND MISSING",0,0,0);
    return GSDI_NOT_FOUND;
  }
  /* --------------------------------------------------------------------------
     Determine if the Permanetly Disabled Feature Indicators File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_engine_fs_file_test(mmgsdi_efs_perm_dis_ind);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS PERM DISABLED FEATURE IND MISSING",0,0,0);
    return GSDI_NOT_FOUND;
  }

  /* --------------------------------------------------------------------------
     Determine if the Number of retries remaining File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_engine_fs_file_test(mmgsdi_efs_retries_rem);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS RETRIES REMAINING MISSING",0,0,0);
    return GSDI_NOT_FOUND;
  }
  /* --------------------------------------------------------------------------
     Determine if the Number of retries remaining for Unblock File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_engine_fs_file_test(mmgsdi_efs_unblock_retries_rem);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS UNBLOCK RETRIES REMAINING MISSING",0,0,0);
    return GSDI_NOT_FOUND;
  }

  /* --------------------------------------------------------------------------
     Determine if all control keys were created
     ------------------------------------------------------------------------*/
  while ( index < GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS )
  {
    present = gsdi_perso_engine_fs_file_test(array_of_ck_files[index]);
    if ( !present )
    {
      MMGSDI_DEBUG_ERR("EFS %s CK MISSING",array_of_ck_files[index],0,0);
      return GSDI_NOT_FOUND;
    }
    index++;
  }

  /* --------------------------------------------------------------------------
     Determine if all FS Files used to store codes is properly created
     ------------------------------------------------------------------------*/
  index = 0x00;
  while ( index < GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS )
  {
    present = gsdi_perso_engine_fs_file_test(array_of_unblock_ck_files[index]);
    if ( !present )
    {
      MMGSDI_DEBUG_ERR("EFS NW UNBLOCK CODES",0,0,0);
      return GSDI_NOT_FOUND;
    }
    index++;
  }

  /* --------------------------------------------------------------------------
     Determine if all FS Files used to store codes is properly created
     ------------------------------------------------------------------------*/
  index = 0x00;
  while ( index < GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS )
  {
    present = gsdi_perso_engine_fs_file_test(array_of_code_files[index]);
    if ( !present )
    {
      MMGSDI_DEBUG_ERR("EFS NW CODES",0,0,0);
      return GSDI_NOT_FOUND;
    }
    index++;
  }
#endif
  return GSDI_SUCCESS;
}
#endif /* FEATURE_MMGSDI_PERSO_SFS */

#ifndef FEATURE_MMGSDI_PERSO_SFS
/*===========================================================================
FUNCTION GSDI_PERSO_CREATE_DIR

DESCRIPTION
  This function is used to create the directories and files.  This function
  should only be called once during the personalization phase.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_create_dir(void)
{
#ifndef FEATURE_MMGSDI_PERSO_SFS
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  /* --------------------------------------------------------------------------
     We now have to create the file Structure since this is the first
     time it is being created.  Once created, it will exist permanently
     ------------------------------------------------------------------------*/
  /* --------------------------------------------------------------------------
     Create the directory for the first file
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_mkdir(mmgsdi_efs_mmgsdi_dir);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Perso directory",0,0,0);
    return gsdi_status;
  }
  /* --------------------------------------------------------------------------
     Create the directory for the Perso Dir
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_mkdir(mmgsdi_efs_perso_dir);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Perso directory",0,0,0);
    return gsdi_status;
  }
#endif
  return GSDI_SUCCESS;
}
#endif /* FEATURE_MMGSDI_PERSO_SFS */ 

#ifdef FEATURE_PERSO_SIM
#ifndef FEATURE_MMGSDI_PERSO_SFS
/*===========================================================================
FUNCTION GSDI_PERSO_CREATE_FILE

DESCRIPTION
  This function is used to create the directories and files.  This function
  should only be called once during the personalization phase.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_create_file(void)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  /* --------------------------------------------------------------------------
     Create the Perso File
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_create_file(mmgsdi_efs_perso_file);
    if ( gsdi_status != GSDI_SUCCESS )
    {
    MMGSDI_DEBUG_ERR("Could not create Perso File",0,0,0);
      return gsdi_status;
    }

  return gsdi_status;
}
#endif /* FEATURE_MMGSDI_PERSO_SFS */

/*===========================================================================
FUNCTION gsdi_perso_engine_set_mem_data

DESCRIPTION
  This function is used to create all of the relevant Personalization Files
  used for storing information in EFS.

  This procedure is ran every time the mobile is powered up, but will not
  recreate any files already created.

  If all personalization files are not created properly or upon entering
  this function again, only some of the files exist...then an Error Fatal
  will occur.  This could be evidence of tampering.

  It will be acceptable for the file to be empty.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_set_mem_data
(
 gsdi_perso_sec_me_data_type *enc_data
)
{
  gsdi_perso_engine_mem_pointer_type mem_pointer;
  uint32 total_len;

  MMGSDI_RETURN_IF_NULL(enc_data);

  memset(enc_data, 0x00, sizeof(gsdi_perso_sec_me_data_type));

  /*------------------------------------------------------------------------
    Feature Inds
  --------------------------------------------------------------------------*/
  enc_data->feat_inds.len = GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS * 2;
  total_len = enc_data->feat_inds.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
    Perm Feature Inds
  --------------------------------------------------------------------------*/
  enc_data->perm_feat_inds.len = GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS * 2;
  total_len += enc_data->perm_feat_inds.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
    SIM Lock Codes
  --------------------------------------------------------------------------*/
  enc_data->sim_lock_codes.len = (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE) *
                                     GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS;
  total_len += enc_data->sim_lock_codes.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
    DCK
  --------------------------------------------------------------------------*/
  enc_data->dcks.len = (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE) *
                                    GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS;
  enc_data->dcks.len += 8 * 2* GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS;
  total_len +=  enc_data->dcks.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
    DCK Counter
  --------------------------------------------------------------------------*/
  enc_data->dck_counts.len = GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS* 2;
  total_len += enc_data->dck_counts.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
    UNBLOCK CK
  --------------------------------------------------------------------------*/
  enc_data->unblock_cks.len = (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE) * GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS;
  enc_data->unblock_cks.len += 8 * 2* GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS;
  total_len +=  enc_data->unblock_cks.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);


  /*------------------------------------------------------------------------
    UNBLOCK CK Counter
  --------------------------------------------------------------------------*/
  enc_data->unblock_ck_counts.len = GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS *2;
  total_len += enc_data->unblock_ck_counts.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

 /* ---------------------------------------------------------------------------
     Allocate the entire buffer required for the encrypted data structure
     Memset it to 0x00 when successful
     Set memory_pointer.memory_curr = memory_pointer.memory_start
     Set memory_pointer.memory_end  = memory_pointer.memory_start + total_mem_req
    -------------------------------------------------------------------------*/
  mem_pointer.memory_start = mmgsdi_malloc(total_len);
  mem_pointer.memory_curr  = mem_pointer.memory_start;
  mem_pointer.memory_end   = mem_pointer.memory_start + total_len;

  MMGSDI_RETURN_IF_NULL(mem_pointer.memory_start);
  memset(mem_pointer.memory_start, 0x00, total_len);

  /*-------------------------------------------------------------------------
    Set the Feat Ind Buffer Pointer
    -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  enc_data->feat_inds.buffer_p = mem_pointer.memory_curr;

  if ((mem_pointer.memory_curr + enc_data->feat_inds.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->feat_inds.len;

  /*-------------------------------------------------------------------------
    Set the Perm Feat Ind Buffer Pointer
    -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->perm_feat_inds.buffer_p = mem_pointer.memory_curr;

  if ((mem_pointer.memory_curr + enc_data->perm_feat_inds.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->perm_feat_inds.len;

  /*-------------------------------------------------------------------------
    Set the SIM Lock Code Buffer Pointer
    -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->sim_lock_codes.buffer_p = mem_pointer.memory_curr;

  if ((mem_pointer.memory_curr + enc_data->sim_lock_codes.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->sim_lock_codes.len;

  /*-------------------------------------------------------------------------
    Set the DCK Memory Buffer pointer
    -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->dcks.buffer_p = mem_pointer.memory_curr;

  if ((mem_pointer.memory_curr + enc_data->dcks.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->dcks.len;

  /*-------------------------------------------------------------------------
    Set the DCK Counter Memory Buffer pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->dck_counts.buffer_p = mem_pointer.memory_curr;

  if ((mem_pointer.memory_curr + enc_data->dck_counts.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->dck_counts.len;


  /*-------------------------------------------------------------------------
    Set the UNblock CK Memory Buffer pointer
    -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->unblock_cks.buffer_p = mem_pointer.memory_curr;

  if ((mem_pointer.memory_curr + enc_data->unblock_cks.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->unblock_cks.len;


  /*-------------------------------------------------------------------------
    Set the Unblock CK Counter Memory Buffer pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->unblock_ck_counts.buffer_p = mem_pointer.memory_curr;

  if ((mem_pointer.memory_curr + enc_data->unblock_ck_counts.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->unblock_ck_counts.len;


  if(mem_pointer.memory_curr != mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }

  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_INIT_PHONE_CODE

DESCRIPTION
  This function is used to initialize the phone code -
  - Retries for lock
  - UNBlock Code.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static void gsdi_perso_engine_init_phone_code(void)
{
  nv_item_type             gsdi_nv_lock_code_ext_digits_buf; /* NV data buffer */
  nv_item_type             gsdi_nv_lock_code_ext_buf;        /* NV data buffer */
  nv_stat_enum_type        write_status;
  int                      i =0;

  gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries = GSDI_PERSO_MAX_CK_RETRIES;

  /* --------------------------------------------------------------------------
    Update the counter
    -------------------------------------------------------------------------*/
  write_status = gsdi_put_nv(NV_LOCK_CODE_EXT_I, &gsdi_nv_lock_code_ext_buf);

  if(write_status != NV_DONE_S)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failure to update counters 0x%x",
                           write_status,0,0);
    return;
  }

  for (i=0;i<GSDI_PERSO_MAX_MASTER_PHONE_CODE_LEN;i++)
  {
    gsdi_nv_lock_code_ext_digits_buf.lock_code_ext_digits[i] = '1';
  }

  write_status = gsdi_put_nv(NV_LOCK_CODE_EXT_DIGITS_I, &gsdi_nv_lock_code_ext_digits_buf);

  if(write_status != NV_DONE_S)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failure to update default master key 0x%x",
                           write_status,0,0);
  }

}
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CREATE_UMTS_DATA

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_create_umts_data(
 int32        * returned_data_len_p,
 const uint8  * data_p
)
{
  gsdi_perso_engine_mem_pointer_type mem_pointer;
  gsdi_perso_sec_me_data_type        encrypted_data;
  gsdi_returns_T                     gsdi_status;
  /* --------------------------------------------------------------------------
    The Perso Engine Has an Interface to allow the Personalization
    Engine to determine how much memory needs to be allocated.
  ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_engine_set_mem_data(&encrypted_data);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get total memory required",0,0,0);
    return GSDI_ERROR;
  }
  memset( &mem_pointer, 0, sizeof(gsdi_perso_engine_mem_pointer_type) );
  mem_pointer.memory_start = encrypted_data.feat_inds.buffer_p;
  mem_pointer.memory_start -= (GSDI_PERSO_ENGINE_TAG_SIZE +  GSDI_PERSO_ENGINE_LEN_SIZE);


 #ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

  gsdi_status = gsdi_perso_security_create_data(&encrypted_data,
                                                data_p,
                                                returned_data_len_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_ERR("PERSO: Could not encrypt data",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mem_pointer.memory_start);
    return gsdi_status;
  }

 #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#ifndef FEATURE_MMGSDI_PERSO_SFS
  gsdi_status = gsdi_perso_engine_create_file();
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_ERR("PERSO: Could not create dirs or files",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mem_pointer.memory_start);
    return gsdi_status;
  }
#endif /*FEATURE_MMGSDI_PERSO_SFS*/

  gsdi_status = gsdi_perso_engine_write_back_fs(&encrypted_data);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not write back FS",0,0,0);
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mem_pointer.memory_start);
  return gsdi_status;
}
#endif /*FEATURE_PERSO_SIM*/

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CREATE_DATA

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_create_data(int32       * returned_data_len_p,
                                                    const uint8 * data_p)
{
  gsdi_returns_T gsdi_status;

#ifdef FEATURE_PERSO_SIM
  gsdi_status = gsdi_perso_engine_create_umts_data(returned_data_len_p,
                                                   data_p);
  if(gsdi_status != GSDI_SUCCESS)
    return gsdi_status;
#endif

#ifdef FEATURE_PERSO_RUIM
  gsdi_status = gsdi_perso_engine_create_ruim_data(returned_data_len_p,
                                                   data_p);
  if(gsdi_status != GSDI_SUCCESS)
    return gsdi_status;
#endif
  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_PERSO_INIT

DESCRIPTION
  This function is used to create all of the relevant Personalization Files
  used for storing information in EFS.

  This procedure is ran every time the mobile is powered up, but will not
  recreate any files already created.

  If all personalization files are not created properly or upon entering
  this function again, only some of the files exist...then an Error Fatal
  will occur.  This could be evidence of tampering.

  It will be acceptable for the file to be empty.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_init(
  int32       * returned_data_len_p,
  const uint8 * data_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_SUCCESS;
  boolean        sanity_check = FALSE;
#ifndef FEATURE_MMGSDI_PERSO_SFS
  boolean        files_present = FALSE;
#endif

  /*Initialize the perso_event*/
   gsdi_data.perso_event = GSDI_PERSO_NO_EVENT;
  /* --------------------------------------------------------------------------
     Does the Directory structure already exist
     ------------------------------------------------------------------------*/
  sanity_check = gsdi_perso_engine_fs_dir_test(mmgsdi_efs_perso_dir);

  /* --------------------------------------------------------------------------
     If the directory structure exists...then the file system was at one time
     created.  If any of the files required for this feature are not existent
     we must assume it was tampered with.
     If the directory does not exist, it must be created, and all files must
     also be created.
     ------------------------------------------------------------------------*/
  if ( sanity_check )
  {
#ifdef FEATURE_MMGSDI_PERSO_SFS
    return GSDI_SUCCESS;
#else
   gsdi_status = gsdi_perso_engine_are_all_files_present();

    if (gsdi_status == GSDI_SUCCESS)
    {
      files_present = TRUE;
    }
#endif /*FEATURE_MMGSDI_PERSO_SFS*/
  }

#ifndef FEATURE_MMGSDI_PERSO_SFS
/*The Directory may be present with the old set of files. */
  if(files_present == FALSE && sanity_check)
  {
    gsdi_status = gsdi_perso_engine_are_all_old_files_present();
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: File System Sanity Check Failed",0,0,0);
      return gsdi_status;
    }
  }

  /*The old files may be present but the new ones are definitely not there!*/
  if(files_present == FALSE)
#endif /*FEATURE_MMGSDI_PERSO_SFS*/
  {
    MSG_HIGH("PERSO: Creating the perso file structure for the first time",0,0,0);
#ifdef FEATURE_PERSO_SIM
    gsdi_perso_engine_init_phone_code();
#endif /*FEATURE_PERSO_SIM*/
#ifndef FEATURE_MMGSDI_PERSO_SFS
    gsdi_status = gsdi_perso_engine_create_dir();
#endif /*FEATURE_MMGSDI_PERSO_SFS*/

    if ( gsdi_status != GSDI_SUCCESS )
       return GSDI_ERROR;

    return gsdi_perso_engine_create_data(returned_data_len_p,
                                         data_p);
  }
#ifndef FEATURE_MMGSDI_PERSO_SFS
  return GSDI_SUCCESS;
#endif /* FEATURE_MMGSDI_PERSO_SFS */ 
}

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
