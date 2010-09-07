/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   R E F R E S H   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of all refresh related commands

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2007 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_refresh.c#10 $$ $DateTime: 2009/05/15 02:20:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp       Demand Paging Changes
04/29/09   js       Added prototype for static function, fixed compiler warnings
03/19/09   js       Support for Enhanced NAA Refresh by file path
02/23/09   mib      Changed behaviour for 3G Session Reset to avoid
                    terminating the application. Added original mode in events
02/16/09   kk       INIT_FFCN mode sent to clients, to enable re-read all the
                    cached items that are not part of re-init
02/12/09   kk       Add files to refresh file list for re-reading during FCN
02/10/08   ssr      Clear Refresh timer as soon all refresh responses are received
12/05/08   jar      Fixed a remaining compilation error.
12/02/08   ssr      Removed Compilation Error
12/01/08   ssr      Handled error condition for some MMGSDI Functions
11/18/08   kk       Added extra logs for better logging of refresh events
09/22/08   js       Fixed Lint Errors
08/28/08   sun      If command fails in the original state, then return original
                    error mmgsdi status to client
08/14/08   js       Fixed compiler warnings for unused variable
06/23/08   sp       Update the gsdi_util_preinit_gsdi function argument
                    in mmgsdi_process_refresh_evt.
06/07/08   sun      Updated additional file flag even if the reads were
                    happening via old GSDI
05/26/08   kk       Fixed compilation warning
05/23/08   kk       Fixed Data Assertion case during Refresh - FCN
04/08/08   nk       Added CDMA Refresh Support
03/24/08   sun      Fixed memory overwrite
03/19/08   nk       Fixed Klocwork issues
03/17/08   sun      Fixed the order of refresh - first handle the request type
                    and then handle the state
02/28/08   sun      New state is only relevant for Reset Mode, do not set it
                    for FCN.
02/26/08   sun      Fixed warning
02/21/08   sun      Read CPHS files on Refresh
02/13/08   kk       Removed an incorrect check for a NULL ptr in function
                    mmgsdi_refresh_req_process_dereg
01/29/08   sun      Handle REFRESH_AUTO same as a REFRESH_RESET mode
01/25/08   tml      Fixed null pointer access in case of onchip
01/11/08   jar      Handle MMGSDI_REFRESH_ORIG_PUP_REQ in mmgsdi_refresh_main
                    for MMGSDI_REFRESH_READY_STATE
12/11/07   js       Fixed RAT FCN support
12/10/07   sun      Fixed sending of events, fixed FCN list check
12/07/07   sun      Fixed Memcpy
12/05/07   sun      Mainlined mmgsdi_session_build_status_terminate so that
                    it canbe used for 2G reset as well
12/04/07   sun      Fixed compile error
12/02/07   sun      Fixed compile error
11/12/07   sun      Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gsdi.h"
#include "mmgsdi.h"
#include "uim.h"
#include "mmgsdiutil.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"
#include "mmgsdi_session.h"
#include "gsdi_convert.h"
#ifdef FEATURE_MMGSDI_CPHS
#include "gsdi_cphs.h"
#endif /*FEATURE_MMGSDI_CPHS*/


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

static mmgsdi_return_enum_type mmgsdi_refresh_send_evt(
  mmgsdi_client_req_extra_info_type *extra_param_ptr,
  const mmgsdi_request_header_type  *request_hdr_ptr
);

static void mmgsdi_refresh_auto_reset_timer_expiry (unsigned long ptr);

/*=========================================================================
  Refresh Specific Utility Functions
===========================================================================*/

/* ==========================================================================
  FUNCTION MMGSDI_INTERNAL_REFRESH_CB

  DESCRIPTION
    This function, defined as mmgsdi_callback_type, will handle
    MMGSDI_DEACTIVATE CNF and MMGSDI_ACTIVATE_CNF cnfs generated by MMGSDI

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_internal_refresh_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;

#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
  mmgsdi_data_type                   aid             = {0};
  mmgsdi_static_data_type            static_aid      = {0};
#endif /*FEATURE_MMGSDI_SESSION_MANAGEMENT*/

  MSG_HIGH("mmgsdi_internal_refresh_cb",0,0,0);

  if(mmgsdi_refresh_info_ptr == NULL)
  {
    MSG_ERROR("Refresh:  Invalid data, nothing can be done, Refresh HUNG",0,0,0);
    return;
  }

  if (cnf_ptr == NULL)
  {
    MSG_ERROR("Refresh:  Cnf Ptr is NULL",0,0,0);
    /*Build the Confirmation only when we get to one of these stages*/
    mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
    (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
            &mmgsdi_refresh_info_ptr->request_header);
    return;
  }

  if (status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh:  Refresh fail during SESSION_SELECT_AID/DEACTIVATE step 0x%x",
               status, 0, 0);
    /*Build the Confirmation only when we get to one of these stages*/
    mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
    (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
            &mmgsdi_refresh_info_ptr->request_header);
    return;
  }

  switch(cnf)
  {
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
    case MMGSDI_SESSION_DEACTIVATE_CNF:
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
                    extra_param_ptr,
                    sizeof(mmgsdi_card_refresh_extra_info_type),
                    mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh:  Could not allocate memory for extr info ptr",0,0,0);
        /*Build the Confirmation only when we get to one of these stages*/
        mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
        (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                &mmgsdi_refresh_info_ptr->request_header);
        return;
      }

      /*put back the app_info_ptr for this client into its session now*/
      mmgsdi_status = mmgsdi_session_activate_app_info(
                        cnf_ptr->session_deactivate_cnf.response_header.client_id,
                        cnf_ptr->session_deactivate_cnf.app_id.aid);

      if(mmgsdi_status != MMGSDI_ERROR)
      {
        extra_param_ptr->refresh_data.send_evt = TRUE;
        extra_param_ptr->refresh_data.stage    =
            MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED;

        (void)mmgsdi_refresh_send_evt(extra_param_ptr,
                                      &mmgsdi_refresh_info_ptr->request_header);
      }

      MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);

      static_aid.data_len  = cnf_ptr->session_deactivate_cnf.app_id.aid.data_len;
      if (static_aid.data_len > MMGSDI_MAX_AID_LEN)
      {
        MSG_ERROR("Refresh:  Invalid AID len 0x%x, cap at MMGSDI_MAX_AID_LEN",
                  static_aid.data_len, 0, 0);
        static_aid.data_len = MMGSDI_MAX_AID_LEN;
      }
      memcpy(static_aid.data_ptr,
             cnf_ptr->session_deactivate_cnf.app_id.aid.data_ptr,
             int32touint32(static_aid.data_len));

      if (cnf_ptr->response_header.client_id == mmgsdi_generic_data.client_id)
      {
        /* Default provisioning app refresh */
#ifndef PLATFORM_LTK
        /* Send cmd to UIM to reset cache */
        gsdi_uim_server_clear_uim_cache(GSDI_SLOT_1, 0, NULL);
#endif /*PLATFORM_LTK*/

        aid.data_len = static_aid.data_len;
        aid.data_ptr = static_aid.data_ptr;

        /* Need to go back to as if just received link est state so that
           pin verification can occur and the corresponding post pin1 init
           procedure as well */
        mmgsdi_status = mmgsdi_select_aid (cnf_ptr->response_header.client_id,
                                           cnf_ptr->response_header.slot_id,
                                           aid,
                                           mmgsdi_internal_refresh_cb,
                                           cnf_ptr->response_header.client_data);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Refresh:  Unable to build Select AID command 0x%x",
                    mmgsdi_status, 0, 0);
          /*Build the Confirmation only when we get to one of these stages*/
          mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
          (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                  &mmgsdi_refresh_info_ptr->request_header);
          return;
        }
      }
      else
      {
        /* Activate the application */
        mmgsdi_status = mmgsdi_session_build_select_aid(
          cnf_ptr->session_deactivate_cnf.response_header.client_id,
          cnf_ptr->session_deactivate_cnf.response_header.slot_id,
          MMGSDI_SESSION_SELECT_AID_REQ,
          MMGSDI_SESSION_SELECT_AID_REQ,
          cnf_ptr->session_deactivate_cnf.app_id.aid,
          mmgsdi_internal_refresh_cb,
          cnf_ptr->session_deactivate_cnf.response_header.client_data);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Refresh:  Unable to build Select AID command 0x%x",
                    mmgsdi_status, 0, 0);
          /*Build the Confirmation only when we get to one of these stages*/
          mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
          (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                  &mmgsdi_refresh_info_ptr->request_header);
          return;
        }
      }
      break;
#endif /*FEATURE_MMGSDI_SESSION_MANAGEMENT*/

    case MMGSDI_SESSION_SELECT_AID_CNF:
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
                    extra_param_ptr,
                    sizeof(mmgsdi_card_refresh_extra_info_type),
                    mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh:  Could not allocate memory for extr info ptr",0,0,0);
        /*Build the Confirmation only when we get to one of these stages*/
        mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
        (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                &mmgsdi_refresh_info_ptr->request_header);
        return;
      }
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    =
          MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED;

      mmgsdi_status = mmgsdi_refresh_send_evt(extra_param_ptr,
                        &mmgsdi_refresh_info_ptr->request_header);

      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;

      (void)mmgsdi_refresh_send_evt(extra_param_ptr,
                                    &mmgsdi_refresh_info_ptr->request_header);

      MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
      /* pending for client to finish initialization before sending TR */
      break;
    default:
      break;
  }
}/*mmgsdi_internal_refresh_cb*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_DEACTIVATE_DEFAULT_APP

  DESCRIPTION
    This function deactivates the default app and clears out all cache

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Will cause the protocol stack to go down
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_deactivate_default_app(
  const mmgsdi_slot_data_type    *slot_ptr,
  mmgsdi_refresh_mode_enum_type  mmgsdi_refresh_mode
)
{
  mmgsdi_return_enum_type       mmgsdi_status     = MMGSDI_ERROR;
  gsdi_refresh_modes_enum_type  gsdi_refresh_mode = GSDI_REFRESH_MODE_NONE;
  mmgsdi_cmd_enum_type          cmd_type          = MMGSDI_MAX_CMD_ENUM;

  MSG_HIGH("mmgsdi_refresh_deactivate_default_app",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);

  mmgsdi_status = mmgsdi_util_convert_refresh_mode(
                        mmgsdi_refresh_mode,
                        &gsdi_refresh_mode);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not convert to correct refresh mode 0x%x",
               mmgsdi_refresh_mode,0,0);
    return MMGSDI_CAN_NOT_REFRESH;
   }

  if(mmgsdi_refresh_mode == MMGSDI_REFRESH_RESET ||
     mmgsdi_refresh_mode == MMGSDI_REFRESH_RESET_AUTO)
  {
    cmd_type = MMGSDI_CARD_RESET_REQ;
  }
  else if(mmgsdi_refresh_mode == MMGSDI_REFRESH_NAA_APP_RESET)
  {
    cmd_type = MMGSDI_SESSION_DEACTIVATE_REQ;
  }
  else
  {
    MSG_ERROR("Unhandled Refresh mode 0x%x",mmgsdi_refresh_mode,0,0);
    return MMGSDI_ERROR;
  }

  /*-----------------------------------------------------------------
   Deactivate the default app,
   Remove all cache
   Set the state to CARD_INSERTED
  ------------------------------------------------------------------*/
  /* Send the status command indicating the app termination
     build app deactivate command */
  mmgsdi_status = mmgsdi_session_build_status_terminate(
                    mmgsdi_generic_data.client_id,
                    mmgsdi_refresh_info_ptr->request_header.slot_id,
                    cmd_type,
                    mmgsdi_internal_refresh_cb,
                    0);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Unable to build deactivate command 0x%x", mmgsdi_status, 0, 0);

    /*Build the Confirmation only when we get to one of these stages*/
    mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
    return MMGSDI_CAN_NOT_REFRESH;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_deactivate_default_app*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RESET_NOTIFY_INIT

  DESCRIPTION
    This function resets all notify_init flags of all clients
    related to app that is being refreshed

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void mmgsdi_refresh_reset_notify_init(void)
{
  int32 i =0;

  if(mmgsdi_refresh_info_ptr == NULL)
  {
    /*Nothing to do*/
    MSG_ERROR("No Refresh in progress, cannot reset init flag",0,0,0);
    return;
  }

  for(i=0;i<MMGSDI_MAX_CLIENT_INFO;i++)
  {
    if(mmgsdi_util_get_session_ptr(i, mmgsdi_refresh_info_ptr->aid) == NULL)
    {
      continue;
    }
    mmgsdi_client_id_reg_table[i].init_complete = FALSE;
  }
}/*mmgsdi_refresh_reset_notify_init*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_MALLOC_AND_COPY_FILE_LIST

  DESCRIPTION
    This function mallocs the file list and then copies the file list to the
    global mmgsdi_refresh_info_ptr

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS                     -   In case of successful execution
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED -   If there is error in heap memory
                                           allocation
    MMGSDI_ERROR                       -   For all other error cases

  SIDE EFFECTS
    None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_malloc_and_copy_file_list(
  mmgsdi_internal_refresh_file_list_type  refresh_files)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(refresh_files.file_list_ptr);

  /*----------------------------------------------------------------
  Allocate memory for the File List
  ----------------------------------------------------------------*/
  if(refresh_files.file_list_ptr != NULL)
  {
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_list_ptr,
      sizeof(mmgsdi_file_enum_type) * refresh_files.num_files,
      mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not allocate memory for Refresh Info Ptr",
        0,0,0);
      return mmgsdi_status;
    }

    /*----------------------------------------------------------------
      Copy the Files over
    ----------------------------------------------------------------*/
    memcpy(mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_list_ptr,
          refresh_files.file_list_ptr,
          refresh_files.num_files *sizeof(mmgsdi_file_enum_type));
  }
  if(refresh_files.file_path_ptr != NULL)
  {
    /* Copy file path array */
    mmgsdi_status =
      mmgsdi_util_malloc_and_copy_file_path_array(
        &(mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_path_ptr),
        refresh_files.file_path_ptr,
        refresh_files.num_file_paths);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Copying file path array failed 0x%x",
                 mmgsdi_status, 0, 0);
      return mmgsdi_status;
    }
  }

  mmgsdi_refresh_info_ptr->refresh_data.refresh_files.num_files =
    refresh_files.num_files;

  mmgsdi_refresh_info_ptr->refresh_data.refresh_files.num_file_paths =
    refresh_files.num_file_paths;

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_malloc_and_copy_file_list*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_AUTO_RESET_TIMER_EXPIRY

  DESCRIPTION
    This function builds a response if an external module that MMGSDI is
    dependent on does not respond. This is only applicable to refresh
    requests generated by mmgsdi itself rather than an actual proactive
    command

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static void mmgsdi_refresh_auto_reset_timer_expiry (unsigned long ptr)
{
  mmgsdi_refresh_info_type     *refresh_ptr   = (mmgsdi_refresh_info_type*)ptr;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type       req           = {{0}};

  MSG_HIGH("mmgsdi_refresh_auto_reset_timer_expiry",0,0,0);

  if(refresh_ptr == NULL)
  {
    MSG_ERROR("Refresh ptr is NULL",0,0,0);
    return;
  }

  if(refresh_ptr->mode  == MMGSDI_REFRESH_RESET_AUTO)
  {
    req.refresh_req_type = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
    memset(&req.refresh.card_req.aid, 0x00, sizeof(mmgsdi_aid_type));
    memset(&req.refresh.card_req.refresh_files,
            0x00,
            sizeof(mmgsdi_data_type));

    req.refresh.card_req.refresh_mode = MMGSDI_REFRESH_RESET_AUTO;
    req.refresh.card_req.num_files    = 0;

    mmgsdi_status = mmgsdi_build_refresh_req(mmgsdi_generic_data.client_id,
                                             refresh_ptr->request_header.slot_id,
                                             &req,
                                             mmgsdi_util_internal_cb,
                                             0);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh:  Cannot build new request",0,0,0);
    }
  }
}/*mmgsdi_refresh_auto_reset_timer_expiry*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_TIMER_EXPIRY

  DESCRIPTION
    This function builds a response if an external module that MMGSDI is
    dependent on does not respond

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_refresh_timer_expiry (unsigned long req_state)
{

  MSG_HIGH("FUNCTION mmgsdi_refresh_timer_expiry",0,0,0);

  if(mmgsdi_refresh_info_ptr == NULL)
  {
    MSG_HIGH("Refresh: Timer Expired but no refresh in progress",0,0,0);
    return;
  }

  MSG_HIGH("Refresh: Timer expired in state 0x%x",
             (mmgsdi_refresh_state_enum_type) req_state,0,0);

  mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_CAN_NOT_REFRESH;
  (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
          &mmgsdi_refresh_info_ptr->request_header);
}/*mmgsdi_refresh_timer_expiry*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifndef PLATFORM_LTK
/*===========================================================================
  FUNCTION MMGSDI_REFRESH_START_TIMER

  DESCRIPTION
    This function starts the timer based on the stage

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
static void mmgsdi_refresh_start_timer(
  const mmgsdi_slot_data_type *slot_ptr)
{
  rex_timer_cnt_type timer = 0;

  if(slot_ptr == NULL)
  {
    MSG_ERROR("Refresh:  Invalid Slot Ptr",0,0,0);
    return;
  }

  /*Start timer and Wait for Processes to Respond*/
  (void)rex_clr_timer(&mmgsdi_refresh_timer);

  rex_def_timer_ex(&mmgsdi_refresh_timer,
                   mmgsdi_refresh_timer_expiry,
                   (uint32)(slot_ptr->refresh_state));

  switch(slot_ptr->refresh_state)
  {
    case MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE:
      timer =  MMGSDI_REFRESH_OK_TO_INIT_TIMER;
      break;
    case MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE:
      timer = MMGSDI_REFRESH_OK_TO_FCN_TIMER;
      break;
    case MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE:
      timer = MMGSDI_REFRESH_INIT_COMPLETE_TIMER;
      break;
    case MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE:
      timer = MMGSDI_REFRESH_FCN_COMPLETE_TIMER;
      break;
    case MMGSDI_REFRESH_READY_STATE:
      /*Nothing to do*/
      return;
    default:
      MSG_ERROR("Refresh:  Invalid State 0x%x",slot_ptr->refresh_state,0,0);
      return;
  }

  (void)rex_set_timer(&mmgsdi_refresh_timer, timer);
}/*mmgsdi_refresh_start_timer*/
#endif /* PLATFORM_LTK */


/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_SEND_EVT_TO_SPECIFIC_CLIENTS

  DESCRIPTION:
    This function will build the refresh event specific to every client
    and send the event to that client.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The funtion was successful in getting the file
                             attributes of a particular file.
    MMGSDI_ERROR:            Could not send event to client(s)
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_send_evt_to_specific_clients(
  const mmgsdi_client_req_extra_info_type *extra_info_ptr,
  const mmgsdi_request_header_type        *request_hdr_ptr)
{
  mmgsdi_refresh_node_type *node_ptr      = NULL;
  int32                     client_index  = 0;
  int32                     session_index = 0;
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  uint32                    j,index       = 0;
  int32                     i             = 0;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(extra_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(request_hdr_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr->node_ptr);


  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
                  evt_info_ptr,
                  sizeof(mmgsdi_event_data_type),
                  mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh:  Unable to Alloc memory for evt_info_ptr 0x%x",
               mmgsdi_status, 0, 0);
    return MMGSDI_ERROR;
  }

  node_ptr= mmgsdi_refresh_info_ptr->node_ptr;
  while(node_ptr)
  {
    /*The Stage could be different for eg. for NAA_INIT_FCN,
      some nodes may not need to do OK_TO_FCN*/
    if(node_ptr->rsp_received == FALSE &&
      node_ptr->stage == extra_info_ptr->refresh_data.stage)
    {
      mmgsdi_status = mmgsdi_util_get_client_id_index(node_ptr->client_id,
                                                      &client_index,
                                                      &session_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh:  Invalid Client ID 0x%x", node_ptr->client_id,0,0);
        node_ptr= node_ptr->next_ptr;
        continue;
      }
      memcpy(&evt_info_ptr->data.refresh.aid,
             &mmgsdi_refresh_info_ptr->aid,
             sizeof(mmgsdi_aid_type));

      evt_info_ptr->evt                    = MMGSDI_REFRESH_EVT;
      evt_info_ptr->data.refresh.mode      = extra_info_ptr->refresh_data.mode;
      evt_info_ptr->data.refresh.orig_mode = mmgsdi_refresh_info_ptr->orig_mode;
      evt_info_ptr->data.refresh.slot      = request_hdr_ptr->slot_id;
      evt_info_ptr->data.refresh.stage     = node_ptr->stage;

      MSG_HIGH("Refresh: Sending Event MMGSDI_REFRESH_EVT mode= 0x%x, stage = 0x%x",
               evt_info_ptr->data.refresh.mode,
               evt_info_ptr->data.refresh.stage,0);

      memcpy(&evt_info_ptr->data.refresh.aid,
             &mmgsdi_refresh_info_ptr->aid,
              sizeof(mmgsdi_aid_type));

     /* It is possible to have nodes which may not have any files hence
        check if the node has > 0 files and then check for
        file enum and path pointers */
      if((node_ptr->refresh_files.num_files > 0) ||
         (node_ptr->refresh_files.num_file_paths > 0))
      {
        MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
          node_ptr->refresh_files.file_list_ptr,
          node_ptr->refresh_files.file_path_ptr,
          mmgsdi_status);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Refresh: Client 0x%x has incompatible file path/enum list",
            node_ptr->client_id,0,0);
          /* Free event info ptr */
          MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
          return mmgsdi_status;
        }
      }

      if(node_ptr->refresh_files.file_list_ptr != NULL)
      {
        evt_info_ptr->data.refresh.refresh_files.file_list_ptr =
          node_ptr->refresh_files.file_list_ptr;
        evt_info_ptr->data.refresh.refresh_files.num_files =
          node_ptr->refresh_files.num_files;
        evt_info_ptr->data.refresh.refresh_files.file_path_len = 0;
        evt_info_ptr->data.refresh.refresh_files.file_path_ptr = NULL;
      }

      if(node_ptr->refresh_files.file_path_ptr != NULL)
      {
        index = 0;
        evt_info_ptr->data.refresh.refresh_files.file_list_ptr = NULL;
        evt_info_ptr->data.refresh.refresh_files.num_files = 0;

        /* convert to single array structure of path */
        for (j = 0; j < node_ptr->refresh_files.num_file_paths; j++)
        {
          evt_info_ptr->data.refresh.refresh_files.file_path_len +=
            int32touint32(node_ptr->refresh_files.file_path_ptr[j].path_len)
            * sizeof(uint16);
        }

        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
                  evt_info_ptr->data.refresh.refresh_files.file_path_ptr,
                  evt_info_ptr->data.refresh.refresh_files.file_path_len,
                  mmgsdi_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Refresh:  Unable to Alloc memory for evt_info_ptr.file_path_ptr 0x%x",
              mmgsdi_status, 0, 0);
          /* Free event info ptr */
          MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
          return mmgsdi_status;
        }

        for (j = 0; j < node_ptr->refresh_files.num_file_paths; j++)
        {
          for (i = 0;
               i < node_ptr->refresh_files.file_path_ptr[j].path_len;
               i++)
          {
            if ((index + 1) >= evt_info_ptr->data.refresh.refresh_files.file_path_len)
            {
              MSG_ERROR("Trying to access out of allocated memory for file_path",
                    0, 0, 0);
              MMGSDIUTIL_TMC_MEM_FREE(
                evt_info_ptr->data.refresh.refresh_files.file_path_ptr);
              /* Free event info ptr */
              MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
              return MMGSDI_ERROR;
            }

            evt_info_ptr->data.refresh.refresh_files.file_path_ptr[index++] =
              node_ptr->refresh_files.file_path_ptr[j].path_buf[i] >> 8;
            evt_info_ptr->data.refresh.refresh_files.file_path_ptr[index++] =
              uint16touint8(node_ptr->refresh_files.file_path_ptr[j].path_buf[i] & 0x00FF);
          }
        }
      }

      if(mmgsdi_client_id_reg_table[client_index].evt_cb_ptr)
      {
        evt_info_ptr->client_id = node_ptr->client_id;

        MSG_HIGH("Sending REFRESH event to client - id_low = 0x%x,id_high = 0x%x",
                 (node_ptr->client_id & 0xFFFFFFFF),
                 (node_ptr->client_id>>32 & 0xFFFFFFFF),
                 0);

        (mmgsdi_client_id_reg_table[client_index].evt_cb_ptr)(evt_info_ptr);
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        evt_info_ptr->data.refresh.refresh_files.file_path_ptr);
    }
    node_ptr = node_ptr->next_ptr;
  }
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_send_evt_to_specific_clients*/


/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_SEND_EVT_TO_ALL

  DESCRIPTION:
    This function will send events to all clients that are interested in the
    current application.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The funtion was successful in getting the file
                             attributes of a particular file.
    MMGSDI_ERROR:            Could not send event to clients
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_send_evt_to_all(
  const mmgsdi_client_req_extra_info_type *extra_info_ptr,
  const mmgsdi_request_header_type        *request_hdr_ptr)
{
  mmgsdi_event_data_type   *evt_info_ptr     = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  uint8                     i                = 0;
  uint8                     j                = 0;
  mmgsdi_session_info_type *session_ptr      = NULL;
  mmgsdi_client_id_type     session_id       = 0;
  gsdi_sim_events_T         old_gsdi_event   = GSDI_NO_SIM_EVENT;
  mmgsdi_int_app_info_type *sel_app_ptr      = NULL;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(extra_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(request_hdr_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
                  evt_info_ptr,
                  sizeof(mmgsdi_event_data_type),
                  mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh:  Unable to Alloc memory for REFRESH_START Evt 0x%x",
               mmgsdi_status, 0, 0);
    return MMGSDI_ERROR;
  }

  memcpy(&evt_info_ptr->data.refresh.aid,
           &mmgsdi_refresh_info_ptr->aid,
           sizeof(mmgsdi_aid_type));

  evt_info_ptr->evt                    = MMGSDI_REFRESH_EVT;
  evt_info_ptr->data.refresh.mode      = mmgsdi_refresh_info_ptr->mode;
  evt_info_ptr->data.refresh.orig_mode = mmgsdi_refresh_info_ptr->orig_mode;
  evt_info_ptr->data.refresh.slot      = request_hdr_ptr->slot_id;
  evt_info_ptr->data.refresh.stage     = extra_info_ptr->refresh_data.stage;

  MSG_HIGH("Refresh: Sending Event MMGSDI_REFRESH_EVT mode= 0x%x, stage = 0x%x",
           mmgsdi_refresh_info_ptr->mode,extra_info_ptr->refresh_data.stage,0);

  for(i=0;i<MMGSDI_MAX_CLIENT_INFO;i++)
  {
    session_id =0;

    if(mmgsdi_refresh_info_ptr->mode != MMGSDI_REFRESH_RESET &&
       mmgsdi_refresh_info_ptr->mode != MMGSDI_REFRESH_RESET_AUTO)
    {
      session_ptr = mmgsdi_util_get_session_ptr(i, mmgsdi_refresh_info_ptr->aid);

      if(session_ptr == NULL)
      {
        /*No need to send the event to this app*/
        continue;
      }
      session_id = session_ptr->session_id;

      /*Send only to 1 session*/
      if(mmgsdi_client_id_reg_table[i].evt_cb_ptr)
      {
        evt_info_ptr->client_id = mmgsdi_client_id_reg_table[i].client_id|
                                  session_id;

        MSG_HIGH("Sending REFRESH event to client - id_low = 0x%x, id_high = 0x%x",
                 (evt_info_ptr->client_id & 0xFFFFFFFF),
                 (evt_info_ptr->client_id>>32 & 0xFFFFFFFF),
                 0);

        (mmgsdi_client_id_reg_table[i].evt_cb_ptr)(evt_info_ptr);
      }
    }
    else/*Send to all Sessions*/
    {
       if(mmgsdi_client_id_reg_table[i].evt_cb_ptr)
       {
         for(j =0;j<mmgsdi_client_id_reg_table[i].num_session;j++)
         {
           session_ptr = &(mmgsdi_client_id_reg_table[i].session_info[j]);
           evt_info_ptr->client_id = mmgsdi_client_id_reg_table[i].client_id|
                                     session_ptr->session_id;

           MSG_HIGH("Sending REFRESH event to client-id_low = 0x%x, id_high = 0x%x",
                    (evt_info_ptr->client_id & 0xFFFFFFFF),
                    (evt_info_ptr->client_id>>32 & 0xFFFFFFFF),
                    0);

          (mmgsdi_client_id_reg_table[i].evt_cb_ptr)(evt_info_ptr);
         }

         MSG_HIGH("Sending REFRESH event to client-id_low = 0x%x, id_high = 0x%x",
                  (evt_info_ptr->client_id & 0xFFFFFFFF),
                  (evt_info_ptr->client_id>>32 & 0xFFFFFFFF),
                  0);

         /*Send to Default Session Also*/
         (mmgsdi_client_id_reg_table[i].evt_cb_ptr)(evt_info_ptr);
       }
    }
  }

  /*Setting the mmgsdi_status to error so that unnecessary event does not go to
   GSDI*/
  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr!= NULL)
  {
    /*Selected App Ptr may be null for a Refresh Reset Case*/
    mmgsdi_status = mmgsdi_util_cmp_aid(&sel_app_ptr->app_data,
                                        &mmgsdi_refresh_info_ptr->aid);

    if(sel_app_ptr->app_data.aid.data_len        == 0 ||
       mmgsdi_refresh_info_ptr->aid.aid.data_len == 0)
    {
      mmgsdi_status = MMGSDI_SUCCESS;
    }
  }

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     (mmgsdi_map_gsdi_mmgsdi_event(TRUE, evt_info_ptr, &old_gsdi_event)
           == MMGSDI_SUCCESS))
  {
    (void)gsdi_util_send_mmi_sim_ind_ext(old_gsdi_event);
  }

  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_send_evt_to_all*/


/* ==========================================================================
  FUNCTION MMGSDI_REFRESH_SEND_EVT

  DESCRIPTION
    This function, will send events to all nodes or all clients depending on
    the refresh stage and the refresh mode

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS
    NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_send_evt(
  mmgsdi_client_req_extra_info_type *extra_param_ptr,
  const mmgsdi_request_header_type  * request_hdr_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(request_hdr_ptr);

  if(extra_param_ptr->refresh_data.send_evt)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);

    if(extra_param_ptr->refresh_data.stage ==
        MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT ||
       extra_param_ptr->refresh_data.stage ==
        MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN  ||
       extra_param_ptr->refresh_data.mode  ==
        MMGSDI_REFRESH_NAA_FCN)
    {
      mmgsdi_status = mmgsdi_refresh_send_evt_to_specific_clients(
                      extra_param_ptr,
                      request_hdr_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_refresh_send_evt_to_all(extra_param_ptr,
                                                     request_hdr_ptr);
    }

    if(mmgsdi_status == MMGSDI_ERROR)
    {
      extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
    }
  }
  return mmgsdi_status;
}/*mmgsdi_refresh_send_evt*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_IS_CALL_STACK_FILE

  DESCRIPTION
    This function will be used to check if the file list provided requires
    the MMGSDI Task to take down the call stack.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    boolean:  TRUE:  The callstack needs to be taken down.
              FALSE: The callstack does not need to be taken down.

  SIDE EFFECTS
    NONE
===========================================================================*/
 static boolean mmgsdi_refresh_is_call_stack_file(
   mmgsdi_file_enum_type file)
{
   switch(file)
   {
      /* GSM Related Files which require call stack to
      ** be taken down and then brought back up
      ** No need to take down call stack for EF SPN */
      case MMGSDI_GSM_AD:
      case MMGSDI_GSM_AAEM:
      case MMGSDI_GSM_ACC:
      case MMGSDI_GSM_BCCH:
      case MMGSDI_GSM_EMLPP:
      case MMGSDI_GSM_IMSI:
      case MMGSDI_GSM_KC:
      case MMGSDI_GSM_KCGPRS:
      case MMGSDI_GSM_LOCI:
      case MMGSDI_GSM_LOCIGPRS:
      case MMGSDI_GSM_LP:
      case MMGSDI_GSM_PHASE:
      case MMGSDI_GSM_SST:

      /* USIM Related Files which require call stack to
      ** be taken down and then brought back up
      ** No need to take down call stack for EF SPN */
      case MMGSDI_USIM_AAEM:
      case MMGSDI_USIM_EMLPP:
      case MMGSDI_USIM_EST:
      case MMGSDI_USIM_IMSI:
      case MMGSDI_USIM_KC:
      case MMGSDI_USIM_KEYS:
      case MMGSDI_USIM_KEYSPS:
      case MMGSDI_USIM_UST:

      /* CDMA Related Files which require call stack to
      ** be taken down and then brought back up */
#ifdef FEATURE_RUIM_CDMA_REFRESH
      case MMGSDI_CDMA_ACCOLC:
      case MMGSDI_CDMA_IMSI_M:
      case MMGSDI_CDMA_IMSI_T:
      case MMGSDI_CDMA_TMSI:
      case MMGSDI_CDMA_NAM_LOCK:
      case MMGSDI_CDMA_MSISDN:
      case MMGSDI_CDMA_CC:
      case MMGSDI_CDMA_SF_EUIM_ID:
      case MMGSDI_CDMA_PREF_LANG:
      case MMGSDI_CDMA_ECC:
      case MMGSDI_CDMA_SERVICE_PREF:
      case MMGSDI_CDMA_PRL:
      case MMGSDI_CDMA_RUIM_ID:
      case MMGSDI_CDMA_OTAPA_SPC_ENABLE:
      case MMGSDI_CDMA_SCI:
      case MMGSDI_CDMA_ANALOG_HOME_SID:
      case MMGSDI_CDMA_HOME_SID_NID:
      case MMGSDI_CDMA_ZONE_BASED_REGN_IND:
      case MMGSDI_CDMA_SYS_REGN_IND:
      case MMGSDI_CDMA_DIST_BASED_REGN_IND:
      case MMGSDI_CDMA_CALL_TERM_MODE_PREF:
      case MMGSDI_CDMA_SVC_TABLE:
      case MMGSDI_CDMA_OTASP_OTAPA_FEATURES:
      case MMGSDI_CDMA_UIM_ID_USAGE_IND:
      case MMGSDI_CDMA_SPC_STATUS:
      case MMGSDI_CDMA_3GPD_SIPPAPSS:
      case MMGSDI_CDMA_3GPD_SIPUPP:
      case MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND:
      case MMGSDI_CDMA_3GPD_MIPFLAGS:
      case MMGSDI_CDMA_3GPD_TCPCONFIG:
      case MMGSDI_CDMA_3GPD_MIPUPP:
#endif /* FEATURE_RUIM_CDMA_REFRESH */
        return TRUE;

      default:
        /* Do Nothing...continue in loop and check next file in file
        ** list */
        break;
    }
    return FALSE;
} /* mmgsdi_refresh_is_call_stack_file */


/* ==========================================================================
  FUNCTION:      MMGSDI_REFRESH_CHANGE_MODE_IF_REQUIRED

  DESCRIPTION:
    This function checks the Refresh FCN to see if it involves the provisioning
    app call stack. If so, the refresh mode is modified to an appropriate mode.

  DEPENDENCIES:
    None

  LIMITATIONS:
    Mode change is only allowed for provisioning app.
    Currently called for Refresh - FCN check ONLY

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS:
    None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_change_mode_if_required(
  const mmgsdi_refresh_card_req_type           *req_ptr,
  mmgsdi_refresh_mode_enum_type                *refresh_mode_ptr,
  const mmgsdi_internal_refresh_file_list_type *refresh_file_list_ptr)
{
  uint32     i                        = 0;
  boolean call_stack_file_present     = FALSE;
  boolean non_call_stack_file_present = FALSE;

  /*----------------------------------------------------------------
    Check if it is provisioning app - if it is ,
    then check if the files is on the call stack, if it is
    then change the mode to init
    ----------------------------------------------------------------*/
  /* 1. Check parameters */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(refresh_mode_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(refresh_file_list_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(refresh_file_list_ptr->file_list_ptr);

  if( refresh_file_list_ptr->num_files == 0)
  {
    MSG_HIGH("Refresh: Incorrect file related parameters for Refresh FCN",0,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* 2. Check if the app is provisioning app */
  if(mmgsdi_util_is_sel_app(req_ptr->aid) == FALSE)
  {
    MSG_HIGH("Refresh: Not a provisioning app. Mode change not required.",0,0,0);
    return MMGSDI_SUCCESS;
  }
  /* Check if the file list has files that belong to the call stack and those that
     do not belong to the call stack to get the mode */
  for(i=0;i<refresh_file_list_ptr->num_files;i++)
  {
    if(!mmgsdi_refresh_is_call_stack_file(
        refresh_file_list_ptr->file_list_ptr[i]))
    {
      non_call_stack_file_present = non_call_stack_file_present|TRUE;
    }
    else
    {
      call_stack_file_present = call_stack_file_present|TRUE;
    }
  }
  if( non_call_stack_file_present && call_stack_file_present)
  {
    *refresh_mode_ptr = MMGSDI_REFRESH_NAA_INIT_FCN;
  }
  else if(call_stack_file_present)
  {
    *refresh_mode_ptr = MMGSDI_REFRESH_NAA_INIT;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_change_mode_if_required*/


/*===========================================================================
  FUNCTION mmgsdi_refresh_send_complete

  DESCRIPTION:
    This function will send refresh complete command to itself

  DEPENDENCIES:
     None

  LIMITATIONS:
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 ===========================================================================*/
void mmgsdi_refresh_send_complete(
  mmgsdi_slot_id_enum_type slot,
  boolean                  pass_fail
)
{
  mmgsdi_option_type            option         = {0};
  mmgsdi_slot_data_type        *slot_ptr       = NULL;
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_ERROR;

  MSG_HIGH("FUNCTION mmgsdi_refresh_send_complete , status = 0x%x",
            pass_fail,0,0);

  if(mmgsdi_refresh_info_ptr == NULL)
  {
    /*No Refresh on Progress, return*/
    MSG_ERROR("Refresh: No Refresh is in progress, no event to send",0,0,0);
    return;
  }

  switch(slot)
  {
    case MMGSDI_SLOT_1:
      slot_ptr = &mmgsdi_data_slot1;
      break;
    case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      slot_ptr = &mmgsdi_data_slot2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
      break;
    default:
      MSG_ERROR("Refresh: Bad Slot 0x%x",slot,0,0);
      return;
  }

  if(slot_ptr == NULL )
  {
    MSG_ERROR("Slot is Null",0,0,0);
    return;
  }

  if ( mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_RESET ||
       mmgsdi_refresh_info_ptr->mode  == MMGSDI_REFRESH_NAA_APP_RESET ||
       mmgsdi_refresh_info_ptr->mode  == MMGSDI_REFRESH_RESET_AUTO)
  {
    slot_ptr->refresh_state = MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE;
  }

  mmgsdi_status = mmgsdi_refresh_complete(mmgsdi_generic_data.client_id,
                                          slot,
                                          pass_fail,
                                          option,
                                          mmgsdi_util_internal_cb,
                                          0);
  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    MSG_ERROR("Unable to send Refresh Complete 0x%x", mmgsdi_status, 0, 0);
  }

}/*mmgsdi_refresh_send_complete*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_PROCESS_EVT

  DESCRIPTION
    This function will handle the Refresh event and set the global
    mmgsdi_data_slot1/mmgsdi_data_slot2 if required.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    Events will be sent to clients and client will in turn send
    new commands to MMGSDI that will move the overall refresh
    state
===========================================================================*/
void mmgsdi_refresh_process_evt(
  const mmgsdi_event_data_type *event_ptr)
{
  gsdi_refresh_modes_enum_type  gsdi_refresh_mode     = GSDI_REFRESH_MODE_NONE;
  mmgsdi_slot_data_type         *mmgsdi_data_slot_ptr = NULL;
  mmgsdi_return_enum_type       mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_option_type            option                = {0};
  uint32                        i,j                   = 0;
  boolean                       pass_fail             = FALSE;
  mmgsdi_refresh_node_type     *node_ptr              = NULL;
  uim_items_enum_type           uim_file_items        = UIM_NO_SUCH_ITEM;
  gsdi_slot_id_type             gsdi_slot             = GSDI_SLOT_NONE;
#ifdef FEATURE_UICC_RAT_INDICATOR_SUPPORT
  gsdi_sim_read_req_T          *gsdi_read_req         = NULL;
  byte                          rec_num_accessed      = 0;
  byte                          num_records           = 0;
#endif /* FEATURE_UICC_RAT_INDICATOR_SUPPORT */

#if defined (FEATURE_MMGSDI_CPHS) || defined (FEATURE_UICC_RAT_INDICATOR_SUPPORT)
  uint8                         data_p[GSDI_MAX_DATA_BLOCK_LEN];
  int32                         returned_data_len     = 0;
  gsdi_returns_T                gsdi_status           = GSDI_SUCCESS;
#endif /*FEATURE_MMGSDI_CPHS || FEATURE_UICC_RAT_INDICATOR_SUPPORT*/

 MSG_HIGH("FUNCTION mmgsdi_refresh_process_evt",0,0,0);

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */

  if(event_ptr == NULL)
  {
    MSG_ERROR("Refresh:  event_ptr is NULL",0,0,0);
    return;
  }

  if(event_ptr->evt != MMGSDI_REFRESH_EVT)
  {
    MSG_ERROR("Refresh: event 0x%x is not Refresh event",event_ptr->evt,0,0);
    return;
  }

  switch(event_ptr->data.refresh.slot)
  {
    case MMGSDI_SLOT_1:
      mmgsdi_data_slot_ptr = &mmgsdi_data_slot1;
      break;
    case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      mmgsdi_data_slot_ptr = &mmgsdi_data_slot2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
      break;
    default:
      MSG_ERROR("Refresh: Bad Slot 0x%x",event_ptr->data.refresh.slot,0,0);
      return;
  }

  option.tlv_num = 0;

  /*This event is received only if it is a default app*/
  mmgsdi_status = mmgsdi_util_convert_refresh_mode(
                    event_ptr->data.refresh.mode,
                    &gsdi_refresh_mode);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not convert to correct refresh mode 0x%x",
               event_ptr->data.refresh.mode,0,0);

    mmgsdi_refresh_send_complete(event_ptr->data.refresh.slot,
                                 FALSE);
    return;
  }

  MSG_HIGH("Refresh: Refresh MODE 0x%x, Refresh STAGE: 0x%x, Orig MODE 0x%x",
            event_ptr->data.refresh.mode,
            event_ptr->data.refresh.stage,
            event_ptr->data.refresh.orig_mode);

  switch(event_ptr->data.refresh.stage)
  {
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
      (void)mmgsdi_ok_to_refresh(mmgsdi_generic_data.client_id,
                                 MMGSDI_SLOT_1,
                                 TRUE,
                                 option,
                                 mmgsdi_util_internal_cb,
                                 0);
      break;
    case MMGSDI_REFRESH_STAGE_START:
      switch(event_ptr->data.refresh.mode)
      {
        case MMGSDI_REFRESH_NAA_APP_RESET:
          /*Set the rsp field for all other nodes also to Complete*/
          node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
          while(node_ptr)
          {
            if(node_ptr->client_id != mmgsdi_generic_data.client_id)
            {
              node_ptr->rsp_received = TRUE;
            }
            node_ptr = node_ptr->next_ptr;
          }

          gsdi_util_preinit_gsdi(gsdi_refresh_mode, GSDI_SLOT_1, FALSE);
          mmgsdi_data_slot_ptr->mmgsdi_state = MMGSDI_STATE_CARD_INSERTED;
          break;

        case MMGSDI_REFRESH_RESET:
        case MMGSDI_REFRESH_RESET_AUTO:
          /*Set the rsp field for all other nodes also to Complete*/
          node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
          while(node_ptr)
          {
            if(node_ptr->client_id != mmgsdi_generic_data.client_id)
            {
              node_ptr->rsp_received = TRUE;
            }
            node_ptr = node_ptr->next_ptr;
          }

          gsdi_util_preinit_gsdi(gsdi_refresh_mode, GSDI_SLOT_1, FALSE);
          mmgsdi_data_slot_ptr->mmgsdi_state = MMGSDI_STATE_NO_CARD;
          break;

        case MMGSDI_REFRESH_NAA_INIT:
        case MMGSDI_REFRESH_NAA_INIT_FCN:
        case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
        case MMGSDI_REFRESH_3G_SESSION_RESET:
          /*Set the rsp field for all other nodes also to Complete*/
          node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
          while(node_ptr)
          {
            if(node_ptr->client_id != mmgsdi_generic_data.client_id)
            {
              node_ptr->rsp_received = TRUE;
            }
            node_ptr = node_ptr->next_ptr;
          }
          gsdi_util_preinit_gsdi(gsdi_refresh_mode, GSDI_SLOT_1, FALSE);
          mmgsdi_data_slot_ptr->mmgsdi_state = MMGSDI_STATE_PIN_EVT_SENT;

          /* Perso refresh procedures required */
          gsdi_data.gstk_data.perso_refresh_needed = TRUE;
#ifndef PLATFORM_LTK
          gsdi_uim_server_clear_uim_cache(GSDI_SLOT_1, 0, NULL);
#endif /*PLATFORM_LTK*/
          mmgsdi_build_post_pin1_cmd(event_ptr->data.refresh.slot);
          break;

        case MMGSDI_REFRESH_NAA_FCN:
          mmgsdi_status = MMGSDI_SUCCESS;

          /*Need to selective clear cache*/
          for(i=0;i<event_ptr->data.refresh.refresh_files.num_files;i++)
          {
            for(j=0;j<MMGSDI_REFRESH_REG_FILES;j++)
            {
              if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                 mmgsdi_refresh_reg_file_list[j])
              {
                break;
              }
            }
            if(j ==MMGSDI_REFRESH_REG_FILES)
            {
              /*Skip reading this file*/
              continue;
            }

            /* clear the cache */
            mmgsdi_status = mmgsdi_util_convert_to_uim_items_enum(
              event_ptr->data.refresh.refresh_files.file_list_ptr[i],
              &uim_file_items);
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              MMGSDI_DEBUG_MSG_ERROR("Can not convert to uim enum 0x%x",
                event_ptr->data.refresh.refresh_files.file_list_ptr[i],0,0);
              break;
            }
            /* Obtain GSDI slot */
            mmgsdi_util_convert_mmgsdi_to_gsdi_slot(event_ptr->data.refresh.slot,
                                                    &gsdi_slot);
            gsdi_util_clear_file_from_cache(1,
                                            &uim_file_items,
                                            gsdi_slot);

            /* For UIM_USIM_RAT EF_READ_BY_PATH method is needed */
#ifdef FEATURE_UICC_RAT_INDICATOR_SUPPORT
            if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_USIM_7F66_PROP1_RAT)
            {
              /* Create a GSDI read request */
              MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(gsdi_read_req,
                sizeof(gsdi_sim_read_req_T), mmgsdi_status);
              if ((mmgsdi_status != MMGSDI_SUCCESS) || (gsdi_read_req == NULL))
              {
                MMGSDI_DEBUG_MSG_ERROR("COULD NOT GET HEAP FOR READ REQ",0,0,0);
                break;
              }
              /* Initialize request header */
              memset(gsdi_read_req,0x00,sizeof(gsdi_sim_read_req_T));
              gsdi_read_req->message_header.slot = gsdi_slot;
              gsdi_read_req->index_mode          = GSDI_NO_INDEX;
              gsdi_read_req->sim_filename        = (word)UIM_USIM_RAT;

              /* Issue a read request */
              gsdi_status = gsdi_umts_proc_usim_read((gsdi_sim_read_req_T *)gsdi_read_req,
                                                      &rec_num_accessed,
                                                      &num_records,
                                                      (byte *)&returned_data_len,
                                                      (byte *)data_p );
              if(MMGSDI_SUCCESS != mmgsdi_util_convert_gsdi_status_to_mmgsdi(
                                     gsdi_status, &mmgsdi_status))
              {
                MMGSDI_DEBUG_MSG_ERROR("mmgsdi convert to gsdi status failed",0,0,0);
              }
              MMGSDIUTIL_TMC_MEM_FREE(gsdi_read_req);
            }
            else
#endif /* FEATURE_UICC_RAT_INDICATOR_SUPPORT */

#ifdef FEATURE_MMGSDI_CPHS
            if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_USIM_CPHSI ||
               event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_GSM_CPHSI)
            {
              gsdi_status = gsdi_proc_cphs_post_pin1_init(gsdi_slot,
                             &returned_data_len, data_p, TRUE, FALSE);

              if(MMGSDI_SUCCESS != mmgsdi_util_convert_gsdi_status_to_mmgsdi(
                                     gsdi_status, &mmgsdi_status))
              {
                MMGSDI_DEBUG_MSG_ERROR("mmgsdi convert to gsdi status failed",0,0,0);
              }
            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_USIM_CSP ||
               event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_GSM_CSP)
            {
              gsdi_status= gsdi_proc_cphs_post_pin1_init(gsdi_slot,
                            &returned_data_len, data_p, FALSE, TRUE);

              if(MMGSDI_SUCCESS != mmgsdi_util_convert_gsdi_status_to_mmgsdi(
                                     gsdi_status, &mmgsdi_status))
              {
                MMGSDI_DEBUG_MSG_ERROR("mmgsdi convert to gsdi status failed",0,0,0);
              }
            }
            else
#endif /*FEATURE_MMGSDI_CPHS*/
#if defined(FEATURE_MMGSDI_UMTS) || defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
            if (event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_ACM ||
                event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_ACM)
            {
              mmgsdi_status = mmgsdi_init_cache_record(
                                GSDI_SLOT_1,
                                event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                                1,
                                NULL);
            }
            else
#endif /* FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
            {
              mmgsdi_status = mmgsdi_init_cache_binary(
                                GSDI_SLOT_1,
                                event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                                NULL);
            }
            if(mmgsdi_status != MMGSDI_SUCCESS &&
               mmgsdi_status != MMGSDI_NOT_SUPPORTED)
            {
              /*No need to read any further*/
              break;
            }
          }
          if(mmgsdi_status == MMGSDI_SUCCESS ||
             mmgsdi_status == MMGSDI_NOT_SUPPORTED)
          {
            pass_fail = TRUE;
          }
          mmgsdi_refresh_send_complete(event_ptr->data.refresh.slot,
                                       pass_fail);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}/*mmgsdi_refresh_process_evt*/


/*===========================================================================
  FUNCTION mmgsdi_refresh_register

  DESCRIPTION:
    This function will let MMGSDI register with itself for refresh fcn

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
   None

  SIDE EFFECTS
    None
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_register(
  mmgsdi_client_id_type client_id)
{
  mmgsdi_refresh_file_list_type refresh_files = {0};
  mmgsdi_option_type            option        = {0};

  if(client_id == mmgsdi_generic_data.client_id)
  {
    refresh_files.num_files     = MMGSDI_REFRESH_REG_FILES;
    refresh_files.file_list_ptr = mmgsdi_refresh_reg_file_list;

  }
  (void)mmgsdi_register_for_refresh(client_id,
                                    MMGSDI_SLOT_1,
                                    refresh_files,
                                    TRUE,
                                    option,
                                    mmgsdi_util_internal_cb,
                                    0);

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_register*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_add_node

  DESCRIPTION:
    This function will add node to the link list

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    Only clients added to the node ptr will get refresh
    events
=============================================================================*/
static void mmgsdi_refresh_add_node(
  mmgsdi_client_id_type                         client_id,
  const mmgsdi_internal_refresh_file_list_type  *refresh_file_list_ptr,
  mmgsdi_refresh_stage_enum_type                stage )
{
  mmgsdi_refresh_node_type      *node_ptr                 = NULL;
  mmgsdi_refresh_node_type      *temp_ptr                 = NULL;
  mmgsdi_return_enum_type        mmgsdi_status            = MMGSDI_ERROR;

  if(mmgsdi_refresh_info_ptr == NULL)
  {
    MSG_ERROR("Refresh: Cannot add node to null",0,0,0);
    return;
  }

  /*Create a new node*/
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(node_ptr,
     sizeof(mmgsdi_refresh_node_type),
     mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Could not allocate memory",0,0,0);
    return;
  }

  /* copy the file list */
  if(refresh_file_list_ptr)
  {
    MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
      refresh_file_list_ptr->file_list_ptr,
      refresh_file_list_ptr->file_path_ptr,
      mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Client 0x%x has incompatible file path/enum list",
        node_ptr->client_id,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(node_ptr);
      return;
    }

    if((refresh_file_list_ptr->num_files > 0) &&
       (refresh_file_list_ptr->file_list_ptr))
    {
      node_ptr->refresh_files.file_list_ptr = refresh_file_list_ptr->file_list_ptr;
      node_ptr->refresh_files.num_files = refresh_file_list_ptr->num_files;
      node_ptr->refresh_files.num_file_paths = 0;

    }
    if((refresh_file_list_ptr->num_file_paths > 0) &&
       (refresh_file_list_ptr->file_path_ptr))
    {
      node_ptr->refresh_files.num_file_paths =
        refresh_file_list_ptr->num_file_paths;

      node_ptr->refresh_files.num_files = 0;
      /* Malloc and copy file paths */
      mmgsdi_status = mmgsdi_util_malloc_and_copy_file_path_array(
          &(node_ptr->refresh_files.file_path_ptr),
          refresh_file_list_ptr->file_path_ptr,
          refresh_file_list_ptr->num_file_paths);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Error Copying File Paths to Node",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(node_ptr);
        return;
      }
    }
  }
  node_ptr->client_id    = client_id;
  node_ptr->next_ptr     = NULL;
  node_ptr->stage        = stage;

  if(stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT ||
     stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
  {
    node_ptr->rsp_received = FALSE;
  }
  else
  {
    /*No need to wait for any responses*/
    node_ptr->rsp_received = TRUE;
  }

  temp_ptr = mmgsdi_refresh_info_ptr->node_ptr;

  if(temp_ptr == NULL)
  {
    mmgsdi_refresh_info_ptr->node_ptr = node_ptr;
    return;
  }

  while(temp_ptr->next_ptr)
  {
    temp_ptr= temp_ptr->next_ptr;
  }
  temp_ptr->next_ptr = node_ptr;
}/*mmgsdi_refresh_add_node*/


/*=============================================================================
  FUNCTION: mmgsdi_adjust_refresh_file_list

  DESCRIPTION:
    This function will remove a file from the FCN file of a on-going
    refresh client node

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static void mmgsdi_adjust_refresh_file_list(
  mmgsdi_internal_refresh_file_list_type   *refresh_file_list_ptr,
  mmgsdi_file_enum_type                    mmgsdi_file)
{
  uint32  i,j        = 0;
  boolean file_found = FALSE;

  /* Parameter check */
  if(refresh_file_list_ptr == NULL)
  {
    MSG_HIGH("Refresh: refresh_file_list_ptr  NULL",0,0,0);
    return;
  }

  if(refresh_file_list_ptr->num_files > 0)
  {
    if(refresh_file_list_ptr->file_list_ptr == NULL)
    {
      MSG_ERROR("Refresh:  refresh_file_list_ptr->file_list_ptr  NULL",0,0,0);
      return;
    }
  }

  if(mmgsdi_file == MMGSDI_NO_FILE_ENUM)
  {
    MSG_HIGH("Refresh: mmgsdi file is MMGSDI_NO_SUCH_FILE",0,0,0);
    return;
  }

  /* Find the file in the file list */
  for(i=0;i<refresh_file_list_ptr->num_files;i++)
  {
    if(refresh_file_list_ptr->file_list_ptr[i] == mmgsdi_file)
    {
      file_found = TRUE;
      break;
    }
  }

  /* If file is found, remove the file from the file list */
  if(file_found == TRUE)
  {
    for(j=i;j<refresh_file_list_ptr->num_files - 1;j++)
    {
      refresh_file_list_ptr->file_list_ptr[j] =
        refresh_file_list_ptr->file_list_ptr[j+1];
    }
    refresh_file_list_ptr->file_list_ptr[refresh_file_list_ptr->num_files-1] =
        MMGSDI_NO_FILE_ENUM;
    refresh_file_list_ptr->num_files -= 1;
  }
  if(refresh_file_list_ptr->num_files == 0)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_file_list_ptr->file_list_ptr);
  }
}/*mmgsdi_adjust_refresh_file_list*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*=============================================================================
  FUNCTION: mmgsdi_remove_file_from_node

  DESCRIPTION:
    This function will remove a file from the FCN file of a on-going
    refresh client node

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static void mmgsdi_remove_file_from_node(
  mmgsdi_client_id_type          client_id,
  mmgsdi_file_enum_type          mmgsdi_file)
{
  mmgsdi_refresh_node_type      *node_ptr                 = NULL;

  /* Parameter check */
  if(mmgsdi_file == MMGSDI_NO_FILE_ENUM)
  {
    MSG_HIGH("Refresh: mmgsdi file is MMGSDI_NO_SUCH_FILE",0,0,0);
    return;
  }

  /* Check to see if the Refresh Linked List exists */
  if(mmgsdi_refresh_info_ptr == NULL)
  {
    MSG_ERROR("Refresh:  mmgsdi_refresh_info_ptr is NULL",0,0,0);
    return;
  }

  if(mmgsdi_refresh_info_ptr->node_ptr == NULL)
  {
    MSG_HIGH("Refresh: mmgsdi_refresh_info_ptr->node_ptr is NULL",0,0,0);
    return;
  }

  node_ptr = mmgsdi_refresh_info_ptr->node_ptr;

  while(node_ptr->next_ptr)
  {
    if(node_ptr->client_id == client_id)
    {
      mmgsdi_adjust_refresh_file_list(&(node_ptr->refresh_files),mmgsdi_file);
      break;
    }
    node_ptr= node_ptr->next_ptr;
  }
}/*mmgsdi_remove_file_from_node*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*=============================================================================
  FUNCTION: mmgsdi_refresh_malloc_and_convert_file_path

  DESCRIPTION:
    Mallocs the file path and Converts the File path into a UIM item and
    then to MMGSDI enum

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_malloc_and_convert_file_path(
  const mmgsdi_refresh_card_req_type           *req_ptr,
  mmgsdi_internal_refresh_file_list_type       *refresh_files_ptr
)
{
  uim_items_enum_type           uim_files_ptr[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
  mmgsdi_path_type              *mmgsdi_file_path_ptr = NULL;
  mmgsdi_return_enum_type       mmgsdi_status         = MMGSDI_ERROR;
  uint8                         i                     = 0 ;
  uint8                         num_files             = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Mem set uim_files_ptr */
  memset(uim_files_ptr,0x00,GSDI_MAX_FILE_CHANGE_NOTIFICATIONS*sizeof(uim_items_enum_type));

  gsdi_util_convert_file_paths(
    int32touint8(req_ptr->refresh_files.data_len),
    req_ptr->refresh_files.data_ptr,
    uim_files_ptr,
    &num_files);

  /* Convert file data into file path array */
  mmgsdi_status = mmgsdi_util_convert_to_refresh_file_paths(
                    int32touint32(req_ptr->refresh_files.data_len),
                    req_ptr->refresh_files.data_ptr,
                    &mmgsdi_file_path_ptr,
                    &num_files);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Could not decode and copy file path",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_path_ptr);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(refresh_files_ptr->file_list_ptr,
    sizeof(mmgsdi_file_enum_type)*num_files,
    mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Could not allocate memory for Refresh Info Ptr",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_path_ptr);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_malloc_and_copy_file_path_array(
    &(refresh_files_ptr->file_path_ptr),
    mmgsdi_file_path_ptr,
    num_files);

  /* Free the memory allocated in mmgsdi_util_convert_to_refresh_file_paths */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_path_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Could not copy file path array - status: 0x%x",
               mmgsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files_ptr->file_list_ptr);
    return mmgsdi_status;
  }

  for(i=0;i<num_files;i++)
  {
    mmgsdi_status = mmgsdi_util_convert_from_uim_items_enum(
                     uim_files_ptr[i],
                    &refresh_files_ptr->file_list_ptr[i]);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not convert uim file 0x%x to mmgsdi file",
                uim_files_ptr[i],0,0);
      refresh_files_ptr->file_list_ptr[i] = MMGSDI_NO_FILE_ENUM;
    }
  }

  /* This will indicate the number of file enums */
  refresh_files_ptr->num_files = i;
  refresh_files_ptr->num_file_paths = num_files;
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_malloc_and_convert_file_path*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_wait_for_client_init

  DESCRIPTION:
    Determines if we need to wait for client initialization
    to complete before the terminal response is sent to the card

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    boolean : Wait for Client Init

  SIDE EFFECTS:
    Could potentially delay init process
=============================================================================*/
static boolean mmgsdi_refresh_wait_for_client_init(
  const mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  uint8 i = 0;

  if(extra_param_ptr == NULL ||
     mmgsdi_refresh_info_ptr == NULL)
  {
    MSG_ERROR("Parameters are NULL",0,0,0);
    return FALSE;
  }

  if(extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_NAA_FCN ||
     extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_RESET_AUTO)
  {
    return FALSE;
  }

  for(i=0;i<MMGSDI_MAX_CLIENT_INFO;i++)
  {
    if(mmgsdi_util_get_session_ptr(i, mmgsdi_refresh_info_ptr->aid) == NULL)
    {
      continue;
    }

    if(mmgsdi_client_id_reg_table[i].notify_init == TRUE)
    {
      if(!mmgsdi_client_id_reg_table[i].init_complete)
      {
        MSG_HIGH("Refresh: init not completed yet for client - \
                  client_id_low = 0x%x, client_id_high = 0x%x",
                  (mmgsdi_client_id_reg_table[i].client_id & 0xFFFFFFFF),
                  (mmgsdi_client_id_reg_table[i].client_id>>32 & 0xFFFFFFFF),0);
        return TRUE;
      }
    }
  }
  return FALSE;
}/*mmgsdi_refresh_wait_for_client_init*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_start_reset

  DESCRIPTION:
    Starts the reset process -
    1. If it is default app - then send the start event
    2. If it is non default app - deactivate app

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_start_reset(
  mmgsdi_slot_data_type             *slot_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr,
  mmgsdi_refresh_mode_enum_type      refresh_mode)
{
  mmgsdi_int_app_info_type *sel_app_ptr      = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;

  MSG_HIGH("FUNCTION mmgsdi_refresh_start_reset",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr->node_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);

  /*----------------------------------------------------------------------
    Find out if it is the default app
    Deactivate the app
  ------------------------------------------------------------------------*/
  /* Limitation, allow refresh only after an app has been selected */
  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_IF_NULL(sel_app_ptr);

  if (mmgsdi_refresh_info_ptr->aid.aid.data_len == 0)
  {
    /*Default it to Current Selected App*/
    memcpy(&mmgsdi_refresh_info_ptr->aid,
           &sel_app_ptr->app_data,
           sizeof(mmgsdi_aid_type));
    mmgsdi_status = mmgsdi_refresh_deactivate_default_app(slot_ptr,
                                                          refresh_mode);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not deactivate default app",0,0,0);
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
      slot_ptr->refresh_state             = MMGSDI_REFRESH_READY_STATE;
      return MMGSDI_CAN_NOT_REFRESH;
    }
    else
    {
      /* default provisioning app is needed, MMGSDI will deactivate app */
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
    }
  }
  else
  {
   /*----------------------------------------------------------------------
     It is not the default app - search for active sessions on this app
    ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_util_cmp_aid(&sel_app_ptr->app_data,
                                        &mmgsdi_refresh_info_ptr->aid);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /*It is not the default app*/
      mmgsdi_status = mmgsdi_session_build_status_terminate(
                        mmgsdi_refresh_info_ptr->node_ptr->client_id,
                        mmgsdi_refresh_info_ptr->request_header.slot_id,
                        MMGSDI_SESSION_DEACTIVATE_REQ,
                        mmgsdi_internal_refresh_cb,
                        0);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Unable to build deactivate command 0x%x",
                   mmgsdi_status,0,0);
        extra_param_ptr->refresh_data.send_evt = TRUE;
        extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
        slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
        return MMGSDI_CAN_NOT_REFRESH;
      }
    }/*Default App - mmgsdi_status == MMGSDI_SUCCESS*/
    else
    {
      mmgsdi_status = mmgsdi_refresh_deactivate_default_app(slot_ptr,
                                                            refresh_mode);
      /* default provisioning app is needed, MMGSDI will deactivate app */
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Could not deactivate default app",0,0,0);
        extra_param_ptr->refresh_data.send_evt = TRUE;
        extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
        slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
        return MMGSDI_CAN_NOT_REFRESH;
      }
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
    }
  }/*mmgsdi_refresh_info_ptr->aid.aid.data_len != 0*/

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_start_reset*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_start_full_reset

  DESCRIPTION:
    Starts the reset process -
    1. Close all sessions
    2. Send start event

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_start_full_reset(
  mmgsdi_slot_data_type             *slot_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr,
  mmgsdi_refresh_mode_enum_type      refresh_mode)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;

  MSG_HIGH("FUNCTION mmgsdi_refresh_start_full_reset",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr->node_ptr);

#ifdef FEATURE_RUIM_CDMA_REFRESH
  /* NVRUIM Cache Clearing process checks if another task is accessing the cache,
     if it is we want to stop the Refresh Request.
     Clear nvruim cache */
  if (!nvruim_clear_cache(NVRUIM_CLEAR_ALL_CACHE,
                          NULL))
  {
    MSG_ERROR("Refresh: NVRUIM Cache could not be cleared",0,0,0);
    extra_param_ptr->refresh_data.send_evt = TRUE;
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }
#endif /* FEATURE_RUIM_CDMA_REFRESH */
  /*----------------------------------------------------------------------
     Deactivate the default APP first
   ------------------------------------------------------------------------*/
  MSG_HIGH("Refresh: Send the status command indicating app termination",0,0,0);

  mmgsdi_status = mmgsdi_refresh_deactivate_default_app(slot_ptr,
                                                        refresh_mode);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not build a terminate command",0,0,0);
    extra_param_ptr->refresh_data.send_evt = TRUE;
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt = TRUE;

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_start_full_reset*/


/* =============================================================================
  FUNCTION:      MMGSDI_COPY_TO_REFRESH_PTR

  DESCRIPTION:
    This function will copy all data to mmgsi_refresh_info_ptr

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
 static mmgsdi_return_enum_type mmgsdi_copy_to_refresh_ptr(
  const mmgsdi_request_header_type    *req_header_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr)
{
  if(req_header_ptr == NULL ||
     req_ptr        == NULL)
  {
    MSG_ERROR("NULL Pointer",0,0,0);
    return MMGSDI_ERROR;
  }

  /*Free any prev data if it is still around*/
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_refresh_info_ptr);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(mmgsdi_refresh_info_ptr,
                                         sizeof(mmgsdi_refresh_info_type));

  memcpy(&mmgsdi_refresh_info_ptr->request_header,
         req_header_ptr,
         sizeof(mmgsdi_request_header_type));

  mmgsdi_refresh_info_ptr->mode      = req_ptr->refresh_mode;
  mmgsdi_refresh_info_ptr->orig_mode = req_ptr->refresh_mode;

  memcpy(&mmgsdi_refresh_info_ptr->aid,
         &req_ptr->aid,
         sizeof(mmgsdi_aid_type));
  return MMGSDI_SUCCESS;

}/*mmgsdi_copy_to_refresh_ptr*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_reset_default_app

  DESCRIPTION:
    This function will check to see if refresh was issued for default app. If
    it was then it will create the linked list with nodes of all clients that
    have voted for init on any session so that all of them can determine if
    it is a good time to do a reset.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          It is default app and the linked list has been
                             created.
    MMGSDI_ERROR:            It is not the default app.

  SIDE EFFECTS:
    All clients regardless of the aid in the refresh command will be asked if
    it is ok to init.
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_reset_default_app(
  const mmgsdi_refresh_card_req_type  *req_ptr,
  boolean                             *wait_for_rsp_ptr)
{
  int32                     i             = 0;
  int32                     client_index  = 0;
  mmgsdi_session_info_type *session_ptr   = NULL;

  MSG_HIGH("FUNCTION mmgsdi_refresh_reset_default_app",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(wait_for_rsp_ptr);

  for(client_index=0;client_index<MMGSDI_MAX_CLIENT_INFO;client_index++)
  {
    if(mmgsdi_client_id_reg_table[client_index].client_id == 0)
    {
      continue;
    }
    session_ptr = &mmgsdi_client_id_reg_table[client_index].default_app;
    if(session_ptr->refresh_data.vote_for_init)
    {
      *wait_for_rsp_ptr = TRUE;
      mmgsdi_refresh_add_node(mmgsdi_client_id_reg_table[client_index].client_id,
                              NULL,
                              MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
    }

    /*Find out all clients connected that have any session open*/
    for(i =0;i<mmgsdi_client_id_reg_table[client_index].num_session;i++)
    {
      session_ptr = &(mmgsdi_client_id_reg_table[client_index].session_info[i]);
      if(session_ptr->refresh_data.vote_for_init)
      {
        *wait_for_rsp_ptr = TRUE;
        mmgsdi_refresh_add_node((mmgsdi_client_id_reg_table[client_index].client_id|
                                 session_ptr->session_id),
                                NULL,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
      }
    }
  }
  mmgsdi_refresh_info_ptr->mode = req_ptr->refresh_mode;
  return MMGSDI_SUCCESS;
} /*mmgsdi_refresh_reset_default_app*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_process_reset

  DESCRIPTION:
    A Refresh Reset Command has been received
       * Build Request based on all clients that are interested in this app
       * If no client is interested, then complete refresh without sending any
         events
       * Only clients that have a session open with the current app will
         receive any events and the session will also only close for only
         those clients

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_reset(
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr)
{
  uint8                              i    = 0;
  mmgsdi_session_info_type *session_ptr   = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  boolean                   wait_for_rsp  = FALSE;

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_reset",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /*----------------------------------------------------------------
  Build Linked List with all clients that have registered for ok to init
  ----------------------------------------------------------------*/
  mmgsdi_refresh_info_ptr->refresh_data.vote_for_init = TRUE;

  for(i=0;i<MMGSDI_MAX_CLIENT_INFO;i++)
  {
    session_ptr = mmgsdi_util_get_session_ptr(i,req_ptr->aid);
    if(session_ptr == NULL)
    {
      /*This client is not interested in this app*/
      continue;
    }
    if(session_ptr->refresh_data.vote_for_init)
    {
      wait_for_rsp = TRUE;
      /*Add Node to Linked List*/
       mmgsdi_refresh_add_node((mmgsdi_client_id_reg_table[i].client_id |
                                session_ptr->session_id),
                               NULL,
                               MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
    }
    else
    {
      mmgsdi_refresh_add_node((mmgsdi_client_id_reg_table[i].client_id |
                               session_ptr->session_id),
                              NULL,
                              MMGSDI_REFRESH_STAGE_START);
    }
  }

  if(mmgsdi_refresh_info_ptr->node_ptr == NULL)
  {
    /*There is no AID active*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
  }

  if(wait_for_rsp)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else
  {
    mmgsdi_status = mmgsdi_refresh_start_reset(slot_ptr,
                                               extra_param_ptr,
                                               req_ptr->refresh_mode);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not start reset, status 0x%x",mmgsdi_status,0,0);
      return mmgsdi_status;
    }
    slot_ptr->refresh_state  = MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_reset*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_FULL_RESET

  DESCRIPTION:
    A Refresh Reset Command has been received
       * Build Request based on all clients on all apps that have voted for
         init
       * All sessions will be terminated as part of this refresh command

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_full_reset(
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  boolean                   wait_for_rsp  = FALSE;

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_full_reset",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  /*----------------------------------------------------------------
  Build Linked List with all clients that have registered for ok to init
  ----------------------------------------------------------------*/
  mmgsdi_refresh_info_ptr->refresh_data.vote_for_init = TRUE;

  mmgsdi_status = mmgsdi_refresh_reset_default_app(req_ptr, &wait_for_rsp);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Refresh reset failed",0,0,0);
    extra_param_ptr->refresh_data.send_evt = TRUE;
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }

  if(mmgsdi_refresh_info_ptr->node_ptr == NULL)
  {
    /*There is no AID active*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    MSG_ERROR("ERROR CARD RESET WITH NO NODES",0,0,0);
    return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
  }

  if(wait_for_rsp)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else
  {
    mmgsdi_status = mmgsdi_refresh_start_full_reset(slot_ptr,
                                                    extra_param_ptr,
                                                    req_ptr->refresh_mode);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not start reset, status 0x%x",mmgsdi_status,0,0);
      return mmgsdi_status;
    }
    slot_ptr->refresh_state  = MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_full_reset*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_process_init

  DESCRIPTION:
    A Refresh INIT/Reset Command has been received
       * Build Request based on all clients that are interested in this app
       * If no client is interested, then send ok to init event and set stage

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_init(
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr,
  mmgsdi_refresh_mode_enum_type        mode)
{
  uint8                     i            = 0;
  mmgsdi_session_info_type *session_ptr  = NULL;
  boolean                   wait_for_rsp = FALSE;

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_init",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);

/*----------------------------------------------------------------
  Build Linked List with all clients that have registered for ok to init
  ----------------------------------------------------------------*/
  mmgsdi_refresh_info_ptr->refresh_data.vote_for_init = TRUE;
  mmgsdi_refresh_info_ptr->mode = mode;

  for(i=0;i<MMGSDI_MAX_CLIENT_INFO;i++)
  {
    session_ptr = mmgsdi_util_get_session_ptr(i,req_ptr->aid);
    if(session_ptr == NULL)
    {
      /*This client is not interested in this app*/
      continue;
    }

    if(session_ptr->refresh_data.vote_for_init)
    {
      wait_for_rsp = TRUE;
      /*Add Node to Linked List*/
       mmgsdi_refresh_add_node((mmgsdi_client_id_reg_table[i].client_id |
                                session_ptr->session_id),
                               NULL,
                               MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
    }
    else
    {
      mmgsdi_refresh_add_node((mmgsdi_client_id_reg_table[i].client_id |
                               session_ptr->session_id),
                              NULL,
                              MMGSDI_REFRESH_STAGE_START);
    }
  }

  if(mmgsdi_refresh_info_ptr->node_ptr == NULL)
  {
    MSG_HIGH("Refresh - No AID Active",0,0,0);
    /*There is no AID active*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;

    return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
  }

  if(wait_for_rsp)
  {
    MSG_HIGH("Refresh - Need to wait for OK_TO_INIT",0,0,0);

    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else
  {
    MSG_HIGH("Refresh - No wait for OK_TO_INIT required, staring refresh",0,0,0);

    /*No client has voted for init*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_START;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_init*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_MALLOC_AND_CALC_CLIENT_SPEC_FILES

  DESCRIPTION:
    Calculates client specific refresh file list

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_FOUND:        When session can not be located in the session
                             info table with given client index and aid.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_malloc_and_calc_client_spc_files(
 const mmgsdi_refresh_card_req_type       *req_ptr,
 mmgsdi_session_info_type                 **session_pptr,
 const mmgsdi_internal_refresh_file_list_type      *refresh_files_ptr,
 mmgsdi_internal_refresh_file_list_type            *client_spc_refresh_files_ptr,
 mmgsdi_refresh_stage_enum_type           *stage_ptr,
 const int32                              *client_index_ptr,
 boolean                                  *need_to_send_fcn_flag_ptr
)
{
  boolean                       is_reg_by_path           = FALSE;
  uint32                        num_files                = 0;
  mmgsdi_return_enum_type       mmgsdi_status            = MMGSDI_SUCCESS;
  uint8                         i=0,j=0,k                = 0;
  mmgsdi_session_info_type      *session_ptr             = NULL;
  uint32                        path_len                 = 0;

  /* Check for null input parameters */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(session_pptr);
  MMGSDIUTIL_RETURN_IF_NULL(refresh_files_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(client_spc_refresh_files_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(stage_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(client_index_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(need_to_send_fcn_flag_ptr);

  client_spc_refresh_files_ptr->num_files = 0;
  client_spc_refresh_files_ptr->num_file_paths = 0;

  session_ptr = mmgsdi_util_get_session_ptr(*client_index_ptr,req_ptr->aid);

  if(session_ptr == NULL)
  {
    return MMGSDI_NOT_FOUND;
  }

  /* check if client registered using file path or enums */
  is_reg_by_path = session_ptr->refresh_data.reg_by_path;

  if(!is_reg_by_path)
  {
    for(j=0;j<session_ptr->refresh_data.refresh_files.num_files;j++)
    {
      for(k=0;k<refresh_files_ptr->num_files;k++)
      {
        /* check if the file is not a stack file */
        if(!mmgsdi_refresh_is_call_stack_file(refresh_files_ptr->file_list_ptr[k]))
        {
          if(session_ptr->refresh_data.refresh_files.file_list_ptr[j] ==
            refresh_files_ptr->file_list_ptr[k])
          {
            num_files++;
          }
        }
      }/*end for*/
    }/*end for*/
  } /* if (!is_reg_by_path) */
  else /* client has registered by file path */
  {
    if (refresh_files_ptr->num_file_paths != refresh_files_ptr->num_files)
    {
      MSG_ERROR("refresh req path num_file 0x%x != enum num_file 0x%x",
                 refresh_files_ptr->num_file_paths, refresh_files_ptr->num_files, 0);
      return MMGSDI_ERROR;
    }
    /* Find out how many files are in intersection set. This will help us in
        determining how much memory should we allocate for file buffer */
    for(j=0;j<session_ptr->refresh_data.refresh_files.num_file_paths;j++)
    {
      for(k=0; k < refresh_files_ptr->num_file_paths; k++)
      {
        /* check if the file is not a stack file */
        /* refresh_files_ptr will contain the same number of files in the file_list_ptr
           and file_path_ptr and they are 1 to 1 mapping to each other.  Hence, we
           can check if the file is a call stack file by looking at the corresponding
           index location in the file_list_ptr */
        if(!mmgsdi_refresh_is_call_stack_file(refresh_files_ptr->file_list_ptr[k]))
        {
          /* compare path length */
          if(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len ==
             refresh_files_ptr->file_path_ptr[k].path_len)
          {
            if(refresh_files_ptr->file_path_ptr[k].path_len == 0)
            {
              MSG_ERROR("Zero path_len at file_path_ptr[0x%x].path_len",k,0,0);
              return MMGSDI_ERROR;
            }
            path_len = int32touint32(refresh_files_ptr->file_path_ptr[k].path_len);
            /* compare path buffers */
            if(!memcmp(
                session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
                refresh_files_ptr->file_path_ptr[k].path_buf,
                MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
            {
              num_files++;
            }
          }
        } /*if the file is not a call stack file */
      }/*end for k*/
    }/*end for j*/
  } /* is_reg_by_path */

  if(num_files > 0)
  {
    if(!is_reg_by_path)
    {
      /*Allocate memory for the file list*/
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        client_spc_refresh_files_ptr->file_list_ptr,
        sizeof(mmgsdi_file_enum_type)* num_files,
        mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Malloc Error for Refresh File List Ptr",0,0,0);
        return MMGSDI_ERROR;
      }

      /* Set file path pointer to null */
      client_spc_refresh_files_ptr->file_path_ptr = NULL;

      i = 0;
      for(j=0;j<session_ptr->refresh_data.refresh_files.num_files;j++)
      {
        for(k=0;k<refresh_files_ptr->num_files;k++)
        {
          /* check if the file is not a stack file */
          if(!mmgsdi_refresh_is_call_stack_file(refresh_files_ptr->file_list_ptr[k]))
          {
            if(session_ptr->refresh_data.refresh_files.file_list_ptr[j] ==
              refresh_files_ptr->file_list_ptr[k])
            {
              if( i < num_files )
              {
                client_spc_refresh_files_ptr->file_list_ptr[i] =
                  refresh_files_ptr->file_list_ptr[k];
                i++;
              }
            }
          }
        }/*end for k*/
      }/*end for j*/
      client_spc_refresh_files_ptr->num_files       = num_files;
      client_spc_refresh_files_ptr->num_file_paths  = 0;
    } /* if (!is_reg_by_path) */
    else
    {
      /*Allocate memory for the file path list*/
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        client_spc_refresh_files_ptr->file_path_ptr,
        sizeof(mmgsdi_path_type)* num_files,
        mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* mmgsdi_refresh_info_ptr will be cleaned up later*/
        MSG_ERROR("Refresh: Malloc Error for Refresh File Path Ptr",0,0,0);
        return MMGSDI_ERROR;
      }

      /* Set file enum list pointer to null */
      client_spc_refresh_files_ptr->file_list_ptr = NULL;

      i = 0;
      /*Copy the files over now*/
      for(j=0;j<session_ptr->refresh_data.refresh_files.num_file_paths;j++)
      {
        for(k = 0; k < refresh_files_ptr->num_file_paths; k++)
        {
          /* compare path length */
          if(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len ==
             refresh_files_ptr->file_path_ptr[k].path_len)
          {
            if(refresh_files_ptr->file_path_ptr[k].path_len == 0)
            {
              MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                         k,0,0);
              MMGSDIUTIL_TMC_MEM_FREE(client_spc_refresh_files_ptr->file_path_ptr);
              return MMGSDI_ERROR;
            }

            path_len = int32touint32(refresh_files_ptr->file_path_ptr[k].path_len);

            /* compare path buffers */
            if(!memcmp(
                session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
                refresh_files_ptr->file_path_ptr[k].path_buf,
                MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
            {
              if(i < num_files)
              {
                /* Copy file path */
                client_spc_refresh_files_ptr->file_path_ptr[i].path_len =
                  refresh_files_ptr->file_path_ptr[k].path_len;
                memcpy(client_spc_refresh_files_ptr->file_path_ptr[i].path_buf,
                  refresh_files_ptr->file_path_ptr[k].path_buf,
                  MIN(path_len, MMGSDI_MAX_PATH_LEN)* sizeof(uint16));

                i++;
              }
            }
          }
        }/*end for j*/
      }/*end for i*/
      client_spc_refresh_files_ptr->num_files       = 0;
      client_spc_refresh_files_ptr->num_file_paths  = num_files;
    } /* end of else is_reg_by_path */

    *stage_ptr = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;

    if(mmgsdi_client_id_reg_table[*client_index_ptr].client_id !=
        mmgsdi_generic_data.client_id)
    {
      *need_to_send_fcn_flag_ptr |=TRUE;
    }
  } /* if num_files > 0 */
  *session_pptr = session_ptr;
  return mmgsdi_status;
} /* mmgsdi_refresh_malloc_and_calc_client_spc_files */


/*=============================================================================
  FUNCTION: mmgsdi_refresh_process_init_fcn

  DESCRIPTION:
    A Refresh INIT FCN command has been received

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_init_fcn(
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr)
{
  mmgsdi_session_info_type                *session_ptr             = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status            = MMGSDI_ERROR;
  mmgsdi_internal_refresh_file_list_type  refresh_files            = {0};
  mmgsdi_internal_refresh_file_list_type  client_spc_refresh_files = {0};
  int32                                   client_index             = 0;
  mmgsdi_refresh_stage_enum_type          stage                    = MMGSDI_REFRESH_STAGE_START;
  boolean                                 need_to_send_fcn_flag    = FALSE;
  mmgsdi_refresh_mode_enum_type           modified_refresh_mode    = MMGSDI_REFRESH_MODE_ENUM_MAX;

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_init_fcn",0,0,0);

  /* Parameter check */
  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);

  MSG_HIGH("Refresh: Refresh Mode is INIT_FCN",0,0,0);
  /*----------------------------------------------------------------
    Convert to MMGSDI File Enums
  ----------------------------------------------------------------*/
  /* Extract file list from refresh request */
  if(mmgsdi_refresh_malloc_and_convert_file_path(req_ptr,&refresh_files)
     != MMGSDI_SUCCESS)
  {
    MSG_HIGH("Refresh: mmgsdi_refresh_malloc_and_convert_file_path() failed",
              0,0,0);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(refresh_files.file_list_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(refresh_files.file_path_ptr);

  /*----------------------------------------------------------------
    Check if it is provisioning app - if it is ,
    then check if the FCN files are on the call stack, if it is
    then change the mode to init if not leave it as init+fcn
    ----------------------------------------------------------------*/
  modified_refresh_mode = req_ptr->refresh_mode;
  mmgsdi_status = mmgsdi_refresh_change_mode_if_required(req_ptr,
                                                        &modified_refresh_mode,
                                                        &refresh_files);

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     modified_refresh_mode == MMGSDI_REFRESH_NAA_INIT)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);

    MSG_HIGH("Refresh: Mode changed from FCN to INIT",0,0,0);
    mmgsdi_status = mmgsdi_refresh_process_init(slot_ptr,
                                                req_ptr,
                                                extra_param_ptr,
                                                modified_refresh_mode);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_refresh_malloc_and_copy_file_list(refresh_files);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
    MSG_ERROR("Refresh: Error in malloc_and_copy_file_list status=0x%x",
               mmgsdi_status,0,0);
    return mmgsdi_status;
  }
  mmgsdi_refresh_info_ptr->refresh_data.vote_for_init = TRUE;

  /* Mode here may be different from the original refresh request
     mode req_ptr->refresh_mode */
  mmgsdi_refresh_info_ptr->mode                       = MMGSDI_REFRESH_NAA_INIT_FCN;

  for(client_index=0;client_index<MMGSDI_MAX_CLIENT_INFO;client_index++)
  {
    stage = MMGSDI_REFRESH_STAGE_START;
    mmgsdi_status = mmgsdi_refresh_malloc_and_calc_client_spc_files(
                      req_ptr,
                      &session_ptr,
                      &refresh_files,
                      &client_spc_refresh_files,
                      &stage,
                      &client_index,
                      &need_to_send_fcn_flag
                   );
    if(mmgsdi_status == MMGSDI_NOT_FOUND)
    {
      continue;
    }
    else if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Clean up refresh files */
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
      return mmgsdi_status;
    }
    if(session_ptr->refresh_data.vote_for_init)
    {
      stage = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;
    }

    /*Add Node to Linked List*/
    mmgsdi_refresh_add_node((mmgsdi_client_id_reg_table[client_index].client_id |
                             session_ptr->session_id),
                            &client_spc_refresh_files,
                            stage);

    /*Do not free client_spc_refresh_files - it will be used by individual nodes
      and freed at the end of refresh*/
  }

  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);

  if(need_to_send_fcn_flag == FALSE)
  {
    mmgsdi_refresh_info_ptr->mode = MMGSDI_REFRESH_NAA_INIT;
  }

  if(mmgsdi_refresh_info_ptr->node_ptr == NULL)
  {
    MSG_HIGH("Refresh: No Node interested in FCN file",0,0,0);

    /*There is no AID active*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
  }

  MSG_HIGH("Refresh: Wait for OK_TO_INIT",0,0,0);
  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;
  extra_param_ptr->refresh_data.send_evt = TRUE;
  slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;

  return MMGSDI_REFRESH_IN_PROGRESS;

}/*mmgsdi_refresh_process_init_fcn*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_process_fcn

  DESCRIPTION:
    A Refresh FCN Command has been received
    1.  If the mode is FCN and it is prov app and it is a call stack file,
        then change the mode to INIT
    2.  Build the Linked list for Clients that will be involved in this refresh
        mode
    3.  Transition into one of the following states –
       •    WAIT_FOR_OK_TO_FCN_STATE:
           a.   If it is FCN node and there are clients interested in files in
                FCN
           b.   Send OK_TO_FCN event to relevant clients
       •    READY_STATE:
           Success Case  - If no clients are interested in any files listed in
                           FCN, clean up, send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
  =============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_fcn(
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr)
{
  uint8                          i,j,k                    = 0;
  mmgsdi_session_info_type      *session_ptr              = NULL;
  mmgsdi_return_enum_type        mmgsdi_status            = MMGSDI_ERROR;
  uint8                          num_files                = 0;
  mmgsdi_refresh_mode_enum_type  modified_refresh_mode    = MMGSDI_REFRESH_MODE_ENUM_MAX;
  int32                          client_index             = 0;
  boolean                        is_reg_by_path           = FALSE;
  uint32                         path_len                 = 0;
  mmgsdi_internal_refresh_file_list_type  refresh_files            = {0};
  mmgsdi_internal_refresh_file_list_type  client_spc_refresh_files = {0};

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_fcn",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);

  /*----------------------------------------------------------------
    Convert to MMGSDI File Enums
  ----------------------------------------------------------------*/
  /* Extract file list from refresh request */
  if(mmgsdi_refresh_malloc_and_convert_file_path(req_ptr,&refresh_files)
     != MMGSDI_SUCCESS)
  {
    MSG_HIGH("Refresh: mmgsdi_refresh_malloc_and_convert_file_path() failed"
             ,0,0,0);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(refresh_files.file_list_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(refresh_files.file_path_ptr);

  /*----------------------------------------------------------------
    Check if it is provisioning app - if it is ,
    then check if the files is on the call stack, if it is
    then change the mode to init or init+fcn
    ----------------------------------------------------------------*/
  modified_refresh_mode = req_ptr->refresh_mode;
  mmgsdi_status = mmgsdi_refresh_change_mode_if_required(req_ptr,
                                                        &modified_refresh_mode,
                                                        &refresh_files);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
    MSG_HIGH("Refresh: mmgsdi_refresh_change_mode_if_required() FAILED",0,0,0);
    return mmgsdi_status;
  }
  if(modified_refresh_mode != req_ptr->refresh_mode )
  {
    MSG_HIGH("Refresh: Refresh mode has changed orig_mode:0x%x modified_mode 0x%x ",
              req_ptr->refresh_mode,modified_refresh_mode,0);
    if(modified_refresh_mode == MMGSDI_REFRESH_NAA_INIT_FCN)
    {
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
      mmgsdi_status = mmgsdi_refresh_process_init_fcn(slot_ptr,
                                                      req_ptr,
                                                      extra_param_ptr);
      return mmgsdi_status;
    }
    else if(modified_refresh_mode == MMGSDI_REFRESH_NAA_INIT)
    {
      (void)mmgsdi_refresh_malloc_and_copy_file_list(refresh_files);
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
      mmgsdi_status = mmgsdi_refresh_process_init(slot_ptr,
                                                 req_ptr,
                                                 extra_param_ptr,
                                                 modified_refresh_mode);
      return mmgsdi_status;
    }
    else
    {
      MSG_HIGH("Refresh: Bad refresh mode at this point 0x%x",modified_refresh_mode,0,0);
    }
  }

  /* At this point, the refresh mode is still FCN only */
  mmgsdi_status = mmgsdi_refresh_malloc_and_copy_file_list(refresh_files);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
    MSG_ERROR("Refresh: Error in malloc_and_copy_file_list status=0x%x",
               mmgsdi_status,0,0);
    return mmgsdi_status;
  }

  /*--------------------------------------------------------------------
  Build List of Clients interested in this file
  ----------------------------------------------------------------------*/
  for(client_index=0;client_index<MMGSDI_MAX_CLIENT_INFO;client_index++)
  {
    num_files = 0;

    session_ptr = mmgsdi_util_get_session_ptr(client_index,req_ptr->aid);

    if(session_ptr == NULL)
    {
      /*Invalid Client or Session, Continue to next*/
      continue;
    }

    /* check if client registered using file path or enums */
    is_reg_by_path = session_ptr->refresh_data.reg_by_path;

    if(!is_reg_by_path)
    {
      /*Find out how many files are in intersection set*/
      for(j=0;j<session_ptr->refresh_data.refresh_files.num_files;j++)
      {
        for(k=0;k<refresh_files.num_files;k++)
        {
          if(session_ptr->refresh_data.refresh_files.file_list_ptr[j] ==
               refresh_files.file_list_ptr[k])
          {
            num_files++;
          }
        }/*end for*/
      }/*end for*/
    }
    else /* client has registered by file path */
    {
      /* Find out how many files are in intersection set. This will help us in
         determining how much memory should we allocate for file buffer */
      for(j=0;j<session_ptr->refresh_data.refresh_files.num_file_paths;j++)
      {
        for(k = 0; k < refresh_files.num_file_paths ; k++)
        {
          /* compare path length */
          if(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len ==
             refresh_files.file_path_ptr[k].path_len)
          {

            if(refresh_files.file_path_ptr[k].path_len == 0)
            {
              MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                         k,0,0);
              MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
              MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
              return MMGSDI_ERROR;
            }
            /* compare path buffers */
            path_len = int32touint32(refresh_files.file_path_ptr[k].path_len);
            if(!memcmp(
                session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
                refresh_files.file_path_ptr[k].path_buf,
                MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16))
              )
            {
              num_files++;
            }
          }
        }/*end for j*/
      }/*end for i*/
    }

    if(num_files == 0)
    {
      /*Skip this client - client has no interest in this file or
        client has not registered for any files*/
      continue;
    }

    if(!is_reg_by_path)
    {
      /*Allocate memory for the file list*/
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        client_spc_refresh_files.file_list_ptr,
        sizeof(mmgsdi_file_enum_type)* num_files,
        mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /*mmgsdi_refresh_info_ptr will be cleaned up later*/
        MSG_ERROR("Refresh: Malloc Error for Refresh File List Ptr",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
        return MMGSDI_ERROR;
      }

      /* set file path pointer to null */
      client_spc_refresh_files.file_path_ptr = NULL;

      i = 0;
      /*Copy the files over now*/
      for(j=0;j<session_ptr->refresh_data.refresh_files.num_files;j++)
      {
        for(k=0;k<refresh_files.num_files;k++)
        {
          if(session_ptr->refresh_data.refresh_files.file_list_ptr[j] ==
             refresh_files.file_list_ptr[k])
          {
            if( i <= num_files )
            {
              client_spc_refresh_files.file_list_ptr[i] =
                refresh_files.file_list_ptr[k];
              i++;
            }
          }
        }/*end for*/
      }/*end for*/
      client_spc_refresh_files.num_file_paths = 0;
      client_spc_refresh_files.num_files      = num_files;
    } /* if(!is_reg_by_path) */

    else /* client registered by file path */
    {
      /*Allocate memory for the file list*/
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        client_spc_refresh_files.file_path_ptr,
        sizeof(mmgsdi_path_type)* num_files,
        mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /*mmgsdi_refresh_info_ptr will be cleaned up later*/
        MSG_ERROR("Refresh: Malloc Error for Refresh File Path Ptr",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
        return MMGSDI_ERROR;
      }

      /* Set file enum list pointer to null */
      client_spc_refresh_files.file_list_ptr = NULL;

      /* mem init file path buffer to zero */
      memset(client_spc_refresh_files.file_path_ptr, 0x00,
        sizeof(mmgsdi_path_type)* num_files);

      i = 0;
      /*Copy the files over now*/
      for(j=0;j<session_ptr->refresh_data.refresh_files.num_file_paths;j++)
      {
        for(k = 0;k < refresh_files.num_file_paths; k++)
        {
          /* compare path length */
          if(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len ==
             refresh_files.file_path_ptr[k].path_len)
          {
            if(refresh_files.file_path_ptr[k].path_len == 0)
            {
              MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                         k,0,0);

              /* Free previous memory allocations */
              MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
              MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
              MMGSDIUTIL_TMC_MEM_FREE(client_spc_refresh_files.file_path_ptr);
              return MMGSDI_ERROR;
            }
            /* compare path buffers */
            path_len = int32touint32(refresh_files.file_path_ptr[k].path_len);
            if(!memcmp(
                session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
                refresh_files.file_path_ptr[k].path_buf,
                MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
            {
              if(i <= num_files)
              {
                /* Copy file path */
                client_spc_refresh_files.file_path_ptr[i].path_len =
                  refresh_files.file_path_ptr[k].path_len;
                memcpy(client_spc_refresh_files.file_path_ptr[i].path_buf,
                  refresh_files.file_path_ptr[k].path_buf,
                  MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16));

                i++;
              }
            }
          }
        }/* end for k */
      }/* end for j */
      client_spc_refresh_files.num_file_paths = num_files;
      client_spc_refresh_files.num_files      = 0;
    } /* end of else is_reg_by_path */

    /*Add Node to Linked List*/
    mmgsdi_refresh_add_node((mmgsdi_client_id_reg_table[client_index].client_id |
                             session_ptr->session_id),
                            &client_spc_refresh_files,
                            MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN);

    /*Do not free client_spc_refresh_files - it will be used by individual nodes
      and freed at the end of refresh*/
  }/*end for*/

  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);

  if(mmgsdi_refresh_info_ptr->node_ptr == NULL)
  {
    /*There is no AID active*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_REFRESH_SUCCESS;
  }

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
  extra_param_ptr->refresh_data.send_evt = TRUE;
  slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;


  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_fcn*/


/*=============================================================================
     Processing Refresh Requests
 =============================================================================*/
/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_REG_BY_PATH

  DESCRIPTION:
    This function will process the registrations sent by the client using file
    paths

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_reg_by_path
(
  const mmgsdi_refresh_reg_req_type           *reg_req_ptr,
  mmgsdi_path_type                            **file_path_pptr,
  mmgsdi_session_info_type                    **session_pptr,
  boolean                                     is_reg_before
)
{
  uint8                     i                = 0;
  mmgsdi_path_type          *file_path_ptr   = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_info_type  *session_ptr     = NULL;
  uint32                    path_len         = 0;

  MMGSDIUTIL_RETURN_IF_NULL(reg_req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(file_path_pptr);
  MMGSDIUTIL_RETURN_IF_NULL(session_pptr);

  file_path_ptr = *file_path_pptr;
  session_ptr   = *session_pptr;

  if((file_path_ptr == NULL) && (is_reg_before == FALSE) &&
     (reg_req_ptr->refresh_files.file_path_ptr != NULL ))
  {
    /* Client registers for the first time using file paths */
    session_ptr->refresh_data.reg_by_path = TRUE;
    mmgsdi_status = mmgsdi_util_malloc_and_copy_file_path_array(
                      &file_path_ptr,
                      reg_req_ptr->refresh_files.file_path_ptr,
                      reg_req_ptr->refresh_files.num_file_paths);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not copy file paths - status=0x%x",
        mmgsdi_status,0,0);
      return MMGSDI_ERROR;
    }

    session_ptr->refresh_data.refresh_files.file_path_ptr = file_path_ptr;

    /*This is the first time that the client is registering with
      MMGSDI for Refresh*/
    session_ptr->refresh_data.refresh_files.num_file_paths =
      reg_req_ptr->refresh_files.num_file_paths;
    session_ptr->refresh_data.refresh_files.num_files = 0;

  } /* file_path_ptr == NULL is_reg_before == FALSE
        && req.file_path_ptr != NULL */
  else if((file_path_ptr != NULL) && (is_reg_before == TRUE) &&
    (reg_req_ptr->refresh_files.file_path_ptr != NULL))
  {

    /* Client tries to modify existing registration using file paths */
    /*Need to append to the list*/
    uint32 num_dup_files    = 0;
    uint32 total_files      = session_ptr->refresh_data.refresh_files.num_file_paths;
    uint32 orig_total_files = session_ptr->refresh_data.refresh_files.num_file_paths;
    uint32 j                = 0,k=0;
    boolean dup             = FALSE;

    /*Find out the actual number of files*/
    for(k=0;k<session_ptr->refresh_data.refresh_files.num_file_paths;k++)
    {
      for(j=0;j<reg_req_ptr->refresh_files.num_file_paths;j++)
      {
        if(session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_len ==
            reg_req_ptr->refresh_files.file_path_ptr[j].path_len)
        {

          if(reg_req_ptr->refresh_files.file_path_ptr[j].path_len == 0)
          {
            MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                        j,0,0);
            return MMGSDI_ERROR;
          }
          path_len =
            int32touint32(reg_req_ptr->refresh_files.file_path_ptr[j].path_len);
          if(!memcmp(
              session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_buf,
              reg_req_ptr->refresh_files.file_path_ptr[j].path_buf,
              MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
          {
            num_dup_files++;
          }
        }
      }
    }

    session_ptr->refresh_data.refresh_files.num_file_paths +=
      reg_req_ptr->refresh_files.num_file_paths - num_dup_files;

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      file_path_ptr,
      sizeof(mmgsdi_path_type) * session_ptr->refresh_data.refresh_files.num_file_paths,
      mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not malloc for File Path pointer",0,0,0);
      return MMGSDI_ERROR;
    }

    for(j = 0; j < total_files; j++)
    {
      if(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len
          == 0)
      {
        MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                    j,0,0);
        /* Free previous memory allocation */
        MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
        return MMGSDI_ERROR;
      }
      path_len =
        int32touint32(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len);
      memcpy(file_path_ptr[j].path_buf,
        session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
        MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16));

      file_path_ptr[j].path_len =
        session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len;
    }

    /*Now Merge the two lists.
      Take the new list, loop through the old list and check any duplicate files
      If any duplicate is found, do not copy*/
    for(i=0;i<reg_req_ptr->refresh_files.num_file_paths;i++)
    {
      dup = FALSE;

      if(reg_req_ptr->refresh_files.file_path_ptr[i].path_len == 0)
      {
        MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                    i,0,0);
        /* Free previous memory allocation */
        MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
        return MMGSDI_ERROR;
      }
      for(j = 0; j < orig_total_files; j++)
      {
        if(file_path_ptr[j].path_len ==
            reg_req_ptr->refresh_files.file_path_ptr[i].path_len)
        {
          path_len =
            int32touint32(reg_req_ptr->refresh_files.file_path_ptr[i].path_len);
          if(!memcmp(
              session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
              reg_req_ptr->refresh_files.file_path_ptr[i].path_buf,
              MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
          {
            dup = TRUE;
            break;
          }
        }
      }

      /* The non duplicate files will be added to the end of the list */
      if(!dup && (total_files < session_ptr->refresh_data.refresh_files.num_file_paths))
      {
        path_len =
          int32touint32(reg_req_ptr->refresh_files.file_path_ptr[i].path_len);
        file_path_ptr[total_files].path_len =
          reg_req_ptr->refresh_files.file_path_ptr[i].path_len;
        memcpy(file_path_ptr[total_files].path_buf,
                reg_req_ptr->refresh_files.file_path_ptr[i].path_buf,
                MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16));
        total_files++;
      }
    }
    MMGSDIUTIL_TMC_MEM_FREE(session_ptr->refresh_data.refresh_files.file_path_ptr);
    session_ptr->refresh_data.refresh_files.file_path_ptr = file_path_ptr;
    session_ptr->refresh_data.refresh_files.num_file_paths = total_files;

  } /* file_path_ptr != NULL && is_reg_before == FALSE &&
        req.file_path_ptr != NULL */
  /* Set number of file enums to zero */
  session_ptr->refresh_data.refresh_files.num_files = 0;
  *file_path_pptr = file_path_ptr;
  *session_pptr = session_ptr;
  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_req_process_reg_by_path */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_REG

  DESCRIPTION:
    This function will process the registrations sent by the client.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does
                             not have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_reg(
  mmgsdi_refresh_req_type           *req_ptr)
{
  mmgsdi_return_enum_type        mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_session_info_type      *session_ptr    = NULL;
  mmgsdi_file_enum_type         *file_list_ptr  = NULL;
  mmgsdi_path_type              *file_path_ptr  = NULL;
  uint8                          i              = 0;
  int32                          client_index   = 0;
  int32                          session_index  = 0;
  mmgsdi_refresh_reg_req_type   *reg_req_ptr    = NULL;
  boolean                       is_reg_by_path  = FALSE;
  boolean                       is_reg_before   = FALSE;


  MSG_MED("MMGSDI_REFRESH_PROCESS_REG", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                   - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  reg_req_ptr = &req_ptr->refresh.reg_req;
  MMGSDIUTIL_RETURN_IF_NULL(reg_req_ptr);

  /*Get the Session Pointer*/
  mmgsdi_status = mmgsdi_util_get_client_id_index(req_ptr->request_header.client_id,
                                                  &client_index,
                                                  &session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Bad Client ID",0,0,0);
    return MMGSDI_ERROR;
  }

  /*It could be the default app*/
  if (session_index == MMGSDI_MAX_CHANNEL_INFO)  /* channel ID default to 0 */
  {
    session_ptr = &(mmgsdi_client_id_reg_table[client_index].default_app);
    session_ptr->app_info_ptr = mmgsdi_util_get_sel_app();
  }
  else
  {
    session_ptr =
      &(mmgsdi_client_id_reg_table[client_index].session_info[session_index]);
  }

  /* Client should register or deregister either using file enum or file
     path only */
  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
    reg_req_ptr->refresh_files.file_list_ptr,
    reg_req_ptr->refresh_files.file_path_ptr,
    mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Client 0x%x has incompatible file path/enum list",
      req_ptr->request_header.client_id,0,0);
    return mmgsdi_status;
  }

  /* At least file path list or file enum list should hold
     one or more files */
  if(((reg_req_ptr->refresh_files.num_files != 0) ||
      (reg_req_ptr->refresh_files.num_file_paths != 0)) &&
     ((reg_req_ptr->refresh_files.file_list_ptr!= NULL) ||
      (reg_req_ptr->refresh_files.file_path_ptr!= NULL)))
  {
    file_list_ptr = session_ptr->refresh_data.refresh_files.file_list_ptr;
    file_path_ptr = session_ptr->refresh_data.refresh_files.file_path_ptr;

    /* If either file path ptr or file enum ptr is not null then client
       has registered before */
    if((file_list_ptr != NULL) || (file_path_ptr != NULL))
    {
      is_reg_before = TRUE;
      is_reg_by_path = session_ptr->refresh_data.reg_by_path;
    }
    /* If client registered earlier by file enums and is
       now trying to register by path or vice versa then
       return error */
    if(((reg_req_ptr->refresh_files.file_list_ptr != NULL) &&
        (is_reg_by_path == TRUE) && (is_reg_before)) ||
       ((reg_req_ptr->refresh_files.file_path_ptr != NULL) &&
        (is_reg_by_path == FALSE) && (is_reg_before)))
    {
      MSG_ERROR("Refresh: Changing reg mode not allowed, curr mode - reg_by_path=0x%x",
        is_reg_by_path,0,0);
      return MMGSDI_ERROR;
    }

    if(reg_req_ptr->refresh_files.file_path_ptr != NULL)
    {
      mmgsdi_status = mmgsdi_refresh_req_process_reg_by_path(
                        reg_req_ptr,
                        &file_path_ptr,
                        &session_ptr,
                        is_reg_before);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: proc_reg_by_path failed, status=0x%x",
                   mmgsdi_status,0,0);
        return mmgsdi_status;
      }
    }

    if((file_list_ptr == NULL) && (is_reg_before == FALSE) &&
       (reg_req_ptr->refresh_files.file_list_ptr != NULL))
    {
      /* Registering for the first time using file enums */
      session_ptr->refresh_data.reg_by_path = FALSE;

      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        file_list_ptr,
        sizeof(mmgsdi_file_enum_type)*reg_req_ptr->refresh_files.num_files,
        mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Could not malloc for File List pointer",0,0,0);
        return MMGSDI_ERROR;
      }
      session_ptr->refresh_data.refresh_files.file_list_ptr = file_list_ptr;

      /*This is the first time that the client is registering with
        MMGSDI for Refresh*/
      session_ptr->refresh_data.refresh_files.num_files =
        reg_req_ptr->refresh_files.num_files;

      memcpy(file_list_ptr,reg_req_ptr->refresh_files.file_list_ptr,
             sizeof(mmgsdi_file_enum_type) * reg_req_ptr->refresh_files.num_files);

    } /* file_list_ptr == NULL && is_reg_before == FALSE &&
         req.file_list_ptr != NULL */
    else if((file_list_ptr != NULL) && (is_reg_before == TRUE) &&
            (reg_req_ptr->refresh_files.file_list_ptr != NULL))
    {
      /* Client is trying to modify existing registration using file enums */

      /*Need to append to the list*/
      uint32 num_dup_files    = 0;
      uint32 total_files      = session_ptr->refresh_data.refresh_files.num_files;
      uint32 orig_total_files = session_ptr->refresh_data.refresh_files.num_files;
      uint32 j                = 0,k=0;
      boolean dup             = FALSE;

      /*Find out the actual number of files*/
      for(k=0;k<session_ptr->refresh_data.refresh_files.num_files;k++)
      {
        for(j=0;j<reg_req_ptr->refresh_files.num_files;j++)
        {
          if(session_ptr->refresh_data.refresh_files.file_list_ptr[k] ==
             reg_req_ptr->refresh_files.file_list_ptr[j])
          {
            num_dup_files++;
          }
        }
      }

      session_ptr->refresh_data.refresh_files.num_files +=
        reg_req_ptr->refresh_files.num_files - num_dup_files;

      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        file_list_ptr,
        sizeof(mmgsdi_file_enum_type) * session_ptr->refresh_data.refresh_files.num_files,
        mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Could not malloc for File List pointer",0,0,0);
        return MMGSDI_ERROR;
      }

      memcpy(file_list_ptr,
             session_ptr->refresh_data.refresh_files.file_list_ptr,
             sizeof(mmgsdi_file_enum_type)*total_files);

      /*Now Merge the two lists.
        Take the new list, loop through the old list and check any duplicate files
        If any duplicate is found, do not copy*/
      for(i=0;i<reg_req_ptr->refresh_files.num_files;i++)
      {
        dup = FALSE;
        for(j=0;j<orig_total_files;j++)
        {
          if(file_list_ptr[j] ==
               reg_req_ptr->refresh_files.file_list_ptr[i])
          {
            dup = TRUE;
            break;
          }
        }
        if(!dup && total_files < session_ptr->refresh_data.refresh_files.num_files)
        {
          file_list_ptr[total_files++] =
            reg_req_ptr->refresh_files.file_list_ptr[i];
        }
      }
      MMGSDIUTIL_TMC_MEM_FREE(
        session_ptr->refresh_data.refresh_files.file_list_ptr);
      session_ptr->refresh_data.refresh_files.file_list_ptr = file_list_ptr;
      session_ptr->refresh_data.refresh_files.num_files     = total_files;

    }/* file_list_ptr != NULL && is_reg_before == TRUE &&
        req.file_list_ptr != NULL */

  }/*if(req_ptr->refresh_files.num_files != 0) &&
     (req_ptr->refresh_files.file_list_ptr != NULL) ||
     (req_ptr->refresh_files.file_path_ptr != NULL) */
  session_ptr->refresh_data.vote_for_init = reg_req_ptr->vote_for_init;
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_req_process_reg*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_DEREG

  DESCRIPTION:
    This function will registration refresh requests

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_dereg(
  mmgsdi_refresh_req_type           *req_ptr)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_info_type      *session_ptr   = NULL;
  mmgsdi_file_enum_type         *file_list_ptr = NULL;
  mmgsdi_path_type              *file_path_ptr    = NULL;
  int32                          client_index  = 0;
  int32                          session_index = 0;
  mmgsdi_refresh_reg_req_type  *dereg_req_ptr  = NULL;
  boolean                        is_reg_by_path   = FALSE;
  uint32                         path_len         = 0;


  MSG_MED("MMGSDI_REFRESH_PROCESS_DEREG", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                   - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  dereg_req_ptr = &req_ptr->refresh.reg_req;
  MMGSDIUTIL_RETURN_IF_NULL(dereg_req_ptr);

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /*Get the Session Pointer*/
  mmgsdi_status = mmgsdi_util_get_client_id_index(
                   req_ptr->request_header.client_id,
                   &client_index,
                   &session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Bad Client ID",0,0,0);
    return MMGSDI_ERROR;
  }

  /*It could be the default app*/
  if (session_index == MMGSDI_MAX_CHANNEL_INFO)  /* channel ID default to 0 */
  {
    session_ptr = &(mmgsdi_client_id_reg_table[client_index].default_app);
    session_ptr->app_info_ptr = mmgsdi_util_get_sel_app();
  }
  else
  {
    session_ptr =
      &(mmgsdi_client_id_reg_table[client_index].session_info[session_index]);
  }

  is_reg_by_path = session_ptr->refresh_data.reg_by_path;

  /* Only file enum ptr or file path ptr should be valid ptr */
  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
    dereg_req_ptr->refresh_files.file_list_ptr,
    dereg_req_ptr->refresh_files.file_path_ptr,
    mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Client 0x%x has incompatible file path/enum list",
      req_ptr->request_header.client_id,0,0);
    return mmgsdi_status;
  }

  /* If client sends file enum for dereg and is registered by path
     or vice versa then return an error */
  if(((dereg_req_ptr->refresh_files.file_list_ptr != NULL) &&
      (is_reg_by_path)) ||
     ((dereg_req_ptr->refresh_files.file_path_ptr != NULL) &&
      (!is_reg_by_path)))
  {
    MSG_ERROR("Refresh: Enum/path mode mismatch compared to reg",0,0,0);
    return MMGSDI_ERROR;
  }
  if(dereg_req_ptr->refresh_files.num_files != 0 &&
     dereg_req_ptr->refresh_files.file_list_ptr != NULL)
  {
    /* Client is currently registered using file enums */
    file_list_ptr = session_ptr->refresh_data.refresh_files.file_list_ptr;

    if(file_list_ptr != NULL)
    {
      /*Need to remove files from the list*/
      uint32 total_files   = 0;
      uint32 i=0, j=0, k=0;
      boolean file_found =FALSE;

      /*Count the total number of common files*/
      for(i=0;i<session_ptr->refresh_data.refresh_files.num_files;i++)
      {
        for(j=0;j<dereg_req_ptr->refresh_files.num_files;j++)
        {
          if(session_ptr->refresh_data.refresh_files.file_list_ptr[i] ==
             dereg_req_ptr->refresh_files.file_list_ptr[j])
          {
            total_files++;
          }
        }
      }

      /*This is now the new total number of files that the client
        is interested in*/
      total_files =
        session_ptr->refresh_data.refresh_files.num_files - total_files;

      if(total_files == 0)
      {
        /*All files dergistered*/
        MMGSDIUTIL_TMC_MEM_FREE(
          session_ptr->refresh_data.refresh_files.file_list_ptr);
        session_ptr->refresh_data.refresh_files.num_files     = 0;
      }
      else
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
          file_list_ptr,
          sizeof(mmgsdi_file_enum_type) * total_files,
          mmgsdi_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Refresh: Could not malloc for File List pointer",0,0,0);
          return MMGSDI_ERROR;
        }

        /*Now add only those files that are not in the new list but are in the
          original list*/
        for(i=0;i<session_ptr->refresh_data.refresh_files.num_files;i++)
        {
          file_found = FALSE;
          for(j=0;j<dereg_req_ptr->refresh_files.num_files;j++)
          {
            if(session_ptr->refresh_data.refresh_files.file_list_ptr[i] ==
               dereg_req_ptr->refresh_files.file_list_ptr[j])
            {
              file_found = TRUE;
              break;
            }
          }
          if(!file_found && k<total_files)
          {
            file_list_ptr[k++] =
              session_ptr->refresh_data.refresh_files.file_list_ptr[i];
          }
        }

        MMGSDIUTIL_TMC_MEM_FREE(session_ptr->refresh_data.refresh_files.file_list_ptr);
        session_ptr->refresh_data.refresh_files.file_list_ptr   = file_list_ptr;
        session_ptr->refresh_data.refresh_files.num_files       = total_files;
      }/*total_files > 0*/

    }/*file_list_ptr != NULL*/
  }/*if(req_ptr->refresh_files.num_files != 0 &&
     req_ptr->refresh_files.file_list_ptr != NULL)*/

  /*******************************************************
    Process File path Request
  *******************************************************/
  if(dereg_req_ptr->refresh_files.num_file_paths != 0 &&
     dereg_req_ptr->refresh_files.file_path_ptr != NULL)
  {
    file_path_ptr = session_ptr->refresh_data.refresh_files.file_path_ptr;

    if(file_path_ptr != NULL)
    {
      /*Need to remove files from the list*/
      uint32 total_files   = 0;
      uint32 i=0, j=0, k=0;
      boolean file_found =FALSE;

      /*Count the total number of common files*/
      for(i=0;i<session_ptr->refresh_data.refresh_files.num_file_paths;i++)
      {
        if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len
           == 0)
        {
          MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                      j,0,0);
          return MMGSDI_ERROR;
        }
        for(j=0;j<dereg_req_ptr->refresh_files.num_file_paths;j++)
        {
          if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len ==
             dereg_req_ptr->refresh_files.file_path_ptr[j].path_len)
          {
            path_len =
              int32touint32(dereg_req_ptr->refresh_files.file_path_ptr[j].path_len);
            if(!memcmp(
                  session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_buf,
                  dereg_req_ptr->refresh_files.file_path_ptr[j].path_buf,
                  MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
            {
              total_files++;
            }
          }
        }
      }

      /*This is now the new total number of files that the client
        is interested in*/
      total_files =
        session_ptr->refresh_data.refresh_files.num_file_paths - total_files;

      if(total_files == 0)
      {
        /*All files dergistered*/
        MMGSDIUTIL_TMC_MEM_FREE(
          session_ptr->refresh_data.refresh_files.file_path_ptr);
        session_ptr->refresh_data.refresh_files.num_file_paths     = 0;
      }
      else
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
          file_path_ptr,
          sizeof(mmgsdi_path_type) * total_files,
          mmgsdi_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Refresh: Could not malloc for File Path pointer",0,0,0);
          return MMGSDI_ERROR;
        }

        /*Now add only those files that are not in the new list but are in the
          original list*/
        for(i=0;i<session_ptr->refresh_data.refresh_files.num_file_paths;i++)
        {
          file_found = FALSE;

          if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len
             == 0)
          {
            MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                        i,0,0);
            MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
            return MMGSDI_ERROR;
          }

          for(j=0;j<dereg_req_ptr->refresh_files.num_file_paths;j++)
          {
            if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len ==
               dereg_req_ptr->refresh_files.file_path_ptr[j].path_len)
            {

              path_len =
                int32touint32(dereg_req_ptr->refresh_files.file_path_ptr[j].path_len);
              if(!memcmp(
                  session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_buf,
                  dereg_req_ptr->refresh_files.file_path_ptr[j].path_buf,
                  MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
              {
                file_found = TRUE;
                break;
              }
            }
          }
          if(!file_found && k<total_files)
          {
            /* Copy matching file */
            path_len =
              int32touint32(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len);
            file_path_ptr[k].path_len =
              session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len;
            memcpy(file_path_ptr[k].path_buf,
              session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_buf,
              MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16));
            k++;
          }
        }

        MMGSDIUTIL_TMC_MEM_FREE(
          session_ptr->refresh_data.refresh_files.file_path_ptr);
        session_ptr->refresh_data.refresh_files.file_path_ptr   = file_path_ptr;
        session_ptr->refresh_data.refresh_files.num_files       = total_files;
      }/*total_files > 0*/

    }/*file_list_ptr != NULL*/
  }/*if(req_ptr->refresh_files.num_files != 0 &&
     req_ptr->refresh_files.file_path_ptr != NULL)*/
  /*Change Voting only if client is specifically deregistering*/
  if(dereg_req_ptr->vote_for_init)
    session_ptr->refresh_data.vote_for_init = FALSE;
  return mmgsdi_status;
}/*mmgsdi_refresh_req_process_dereg*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_COMPLETE_INIT

  DESCRIPTION:
    This state will perform the following operations:
    1.  Wait for all responses to be received
    2.  Wait for PBM and WMS also to complete if it is a prov app
    3.  If all have been received or any of them has resulted in error
        then transition to next state
        • READY_STATE:
          a. Send END_SUCCESS/END_FAILED Event to all relevant clients
          b. Error Case - If any client has voted not to init,
              i. clean up
              ii.Send TR
          c.Success Case
              i. Clean up
              ii.Send TR
        •WAIT_FOR_FCN_COMPLETE_STATE
          a.If FCN >0
             i. Send START event to relevant clients

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_complete_init(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  mmgsdi_refresh_node_type *node_ptr      = NULL;
  boolean                   wait_for_rsp  = FALSE;

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_complete_init",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);

  MSG_HIGH("REFRESH: COMPLETE INIT response received from client id_low = 0x%x,\
            id_high = 0x%x, with status = 0x%x",
           (req_ptr->request_header.client_id & 0xFFFFFFFF),
           (req_ptr->request_header.client_id>>32 & 0xFFFFFFFF),
           req_ptr->refresh.pass_fail);

  extra_param_ptr->refresh_data.stage       = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt    = FALSE;

  node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
  while(node_ptr)
  {
    if(node_ptr->client_id    == req_ptr->request_header.client_id &&
       node_ptr->rsp_received == FALSE)
    {
      node_ptr->rsp_received = TRUE;
      mmgsdi_refresh_info_ptr->refresh_data.vote_for_init &=
        req_ptr->refresh.pass_fail;
    }

    if(!mmgsdi_refresh_info_ptr->refresh_data.vote_for_init)
    {
      /*Break from loop - no need to wait for other responses*/
      break;
    }

    if(node_ptr->rsp_received == FALSE &&
       node_ptr->stage == MMGSDI_REFRESH_STAGE_START)
    {
      /*All responses have not been received, keep waiting*/
      wait_for_rsp = TRUE;
    }
    node_ptr = node_ptr->next_ptr;
  }

  if(!mmgsdi_refresh_info_ptr->refresh_data.vote_for_init)
  {
    MSG_HIGH("REFRESH Failed, received failure for refresh complete",0,0,0);

    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    mmgsdi_refresh_info_ptr->refresh_status= MMGSDI_CAN_NOT_REFRESH;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }

  /*Either all the responses have been received or
    Someone could not do the init successfully*/
  if(!wait_for_rsp &&
     !mmgsdi_refresh_wait_for_client_init(extra_param_ptr))
  {
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer);

    MSG_HIGH("REFRESH END SUCCESS, sending evt MMGSDI_REFRESH_SUCCESS",0,0,0);

    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    mmgsdi_refresh_info_ptr->refresh_status= MMGSDI_REFRESH_SUCCESS;
    slot_ptr->refresh_state  = MMGSDI_REFRESH_READY_STATE;

    /*Send the End SUCCESS event first*/
    (void)mmgsdi_refresh_send_evt(extra_param_ptr,
                                  &req_ptr->request_header);

     extra_param_ptr->refresh_data.send_evt = FALSE;

    /*All responses have been received, continue with refresh*/
    if(mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_NAA_INIT_FCN)
    {
      /*Start the FCN*/
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.mode     = MMGSDI_REFRESH_NAA_FCN;
      slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE;

      node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
      while(node_ptr)
      {
        /*Reset the response received flag only for nodes that have unread files*/
        if(((node_ptr->refresh_files.file_list_ptr != NULL) ||
            (node_ptr->refresh_files.file_path_ptr != NULL)) &&
           ((node_ptr->refresh_files.num_files > 0) ||
            ((node_ptr->refresh_files.num_file_paths > 0))))
        {
          node_ptr->rsp_received = FALSE;
        }
        node_ptr->stage        = MMGSDI_REFRESH_STAGE_START;
        node_ptr               = node_ptr->next_ptr;
      }
    }
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_complete_init*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_COMPLETE_FCN

  DESCRIPTION:
    This state will perform the following operations:
    1. Wait for all responses to be received
    2. If all have been received or any of them has resulted in error then transition to next state
       • READY_STATE:
          a.Send END_SUCCESS/END_FAILED to all relevant clients
          b. Error Case - If any client has voted not to init
             i.  clean up
             ii. Send TR
          c.Success Case –
             i. Clean up
             ii.Send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_complete_fcn(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  mmgsdi_refresh_node_type *node_ptr      = NULL;
  boolean                   wait_for_rsp  = FALSE;

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_complete_fcn",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);

  MSG_HIGH("REFRESH: COMPLETE FCN response received from client id_low = 0x%x,\
            id_high = 0x%x, with status = 0x%x",
           (req_ptr->request_header.client_id & 0xFFFFFFFF),
           (req_ptr->request_header.client_id>>32 & 0xFFFFFFFF),
           req_ptr->refresh.pass_fail);

  extra_param_ptr->refresh_data.stage       = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt    = FALSE;

  node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
  while(node_ptr)
  {
    if(node_ptr->client_id    == req_ptr->request_header.client_id &&
       node_ptr->rsp_received == FALSE)
    {
      MSG_HIGH("Client Id 0x%x responded with pass =0x%x",
                node_ptr->client_id,req_ptr->refresh.pass_fail,0);
      node_ptr->rsp_received = TRUE;

      if(!req_ptr->refresh.pass_fail)
      {
        /*Break from loop , no need to look any further*/
        break;
      }
    }
    if(node_ptr->rsp_received == FALSE &&
       node_ptr->stage == MMGSDI_REFRESH_STAGE_START)
    {
      /*All responses have not been received, keep waiting*/
      wait_for_rsp = TRUE;
    }
    node_ptr = node_ptr->next_ptr;
  }

  /* All the responses have been received */
  if(!wait_for_rsp)
  {
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer);
    MSG_HIGH("All clients have responded, completed refresh",0,0,0);
    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    mmgsdi_refresh_info_ptr->refresh_status= MMGSDI_REFRESH_SUCCESS;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr->stage = extra_param_ptr->refresh_data.stage;
      node_ptr = node_ptr->next_ptr;
    }
  }

  if(!req_ptr->refresh.pass_fail)
  {
    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    mmgsdi_refresh_info_ptr->refresh_status= MMGSDI_CAN_NOT_REFRESH;
    node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr->stage = extra_param_ptr->refresh_data.stage;
      node_ptr = node_ptr->next_ptr;
    }
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_complete_fcn*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_OK_INIT_RSP

  DESCRIPTION:
    This function will process the clients response to ok_to_init.

    This state will perform the following operations:
    1. If all have been received or any of them has resulted in error then transition to next state
       •WAIT_FOR_OK_TO_FCN_STATE:
           a. If it is FCN node and there are clients interested in files in FCN
           b. Send OK_TO_FCN event to relevant clients
        •READY_STATE:
           a.Error Case - If any client has voted not to init, clean up , send TR
        •WAIT_FOR_INIT_COMPLETE_STATE:
           a.If all clients have responded with OK
              i.  Send START Event to all relevant clients
              ii. Send command to GSDI to start init if it is prov app
              iii.Transition to this state
        •   APP_DEACTIVATE_STATE
           a.If the mode is APP_RESET or 3G Session reset and it is not default app

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_ok_init_rsp(
  mmgsdi_slot_data_type              *slot_ptr,
  const mmgsdi_refresh_req_type      *req_ptr,
  mmgsdi_client_req_extra_info_type  *extra_param_ptr)
{
  mmgsdi_refresh_node_type           *node_ptr     = NULL;
  boolean                             wait_for_rsp = FALSE;
  mmgsdi_return_enum_type             mmgsdi_status= MMGSDI_ERROR;

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_ok_init_rsp",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr->node_ptr);

  MSG_HIGH("REFRESH: OK_TO_INIT response received from client id_low = 0x%x,\
            id_high = 0x%x, with status = 0x%x",
           (req_ptr->request_header.client_id & 0xFFFFFFFF),
           (req_ptr->request_header.client_id>>32 & 0xFFFFFFFF),
           req_ptr->refresh.ok_req);

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;

  /*Check if all the responses have been received*/
  node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
  while( node_ptr != NULL)
  {
    /*Handle only OK_TO_INIT CASES, some nodes maybe OK_TO_INIT and some
      maybe OK_TO_FCN in case the mode is NAA_INIT_FCN*/
    if(node_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT)
    {
      if(node_ptr->client_id    == req_ptr->request_header.client_id &&
         node_ptr->rsp_received == FALSE)
      {
        node_ptr->rsp_received = TRUE;
        node_ptr->stage        = MMGSDI_REFRESH_STAGE_START;

        if(mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_NAA_INIT_FCN &&
           ((node_ptr->refresh_files.file_list_ptr != NULL) ||
           (node_ptr->refresh_files.file_path_ptr != NULL)))
        {
          node_ptr->stage = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
        }
        mmgsdi_refresh_info_ptr->refresh_data.vote_for_init &=
           req_ptr->refresh.ok_req;

        if(!mmgsdi_refresh_info_ptr->refresh_data.vote_for_init)
        {
          MSG_HIGH("REFRESH: OK_TO_INIT failure response received",0,0,0);

          /*Break, no need to look any further, we have a failure already*/
          break;
        }
      }
      if(node_ptr->rsp_received == FALSE)
      {
        /*All responses have not been received, keep waiting*/
        wait_for_rsp = TRUE;
      }
    }
    node_ptr = node_ptr->next_ptr;
  }

  if(!mmgsdi_refresh_info_ptr->refresh_data.vote_for_init)
  {
    if(mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_RESET_AUTO)
    {
      /*Start timer and Queue the command again, SMS may be in progress*/
      rex_def_timer_ex(&mmgsdi_refresh_auto_trigger_timer,
                       mmgsdi_refresh_auto_reset_timer_expiry,
                       (uint32)(mmgsdi_refresh_info_ptr));

      (void)rex_set_timer(&mmgsdi_refresh_auto_trigger_timer, MMGSDI_AUTO_REFRESH_TIMER);
      mmgsdi_status = MMGSDI_REFRESH_LATER;
    }
    else
    {
      mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
    }

    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    mmgsdi_refresh_info_ptr->refresh_status= mmgsdi_status;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return mmgsdi_status;
  }

  if(!wait_for_rsp)
  {
    /*------------------------------------------------------------------------
     •  WAIT_FOR_OK_TO_FCN_STATE:
       a.    If it is FCN node and there are clients interested in files in FCN
       b.   Send OK_TO_FCN event to relevant clients
    ------------------------------------------------------------------------*/
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer);

    if(mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_NAA_INIT_FCN)
    {
      /*Wait for ok_to_fcn*/
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    =
        MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
      slot_ptr->refresh_state                =
        MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;
    }
    else if(mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_RESET ||
            mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_RESET_AUTO)
    {
      mmgsdi_status = mmgsdi_refresh_start_full_reset(slot_ptr,
                                                 extra_param_ptr,
                                                 mmgsdi_refresh_info_ptr->mode);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Could not start reset, status 0x%x",mmgsdi_status,0,0);
        return mmgsdi_status;
      }
      slot_ptr->refresh_state =
        MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }
    else if(mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_NAA_APP_RESET)
    {
      mmgsdi_status = mmgsdi_refresh_start_reset(slot_ptr,
                                                 extra_param_ptr,
                                                 mmgsdi_refresh_info_ptr->mode);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Refresh: Could not start reset, status 0x%x",mmgsdi_status,0,0);
        return mmgsdi_status;
      }
      slot_ptr->refresh_state =
        MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }/*mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_NAA_APP_RESET*/
    else
    {
      /*----------------------------------------------------------------------
       •    WAIT_FOR_INIT_COMPLETE_STATE:
           a.   If all clients have responded with OK
              i.    Send START Event to all relevant clients
              ii.   Transition to this state
       ------------------------------------------------------------------------*/
#ifdef FEATURE_RUIM_CDMA_REFRESH
       /* NVRUIM Cache Clearing process checks if another task is accessing the cache,
       if it is we want to stop the Refresh Request.
       Clear nvruim cache only if default app */
       if (mmgsdi_refresh_info_ptr->aid.aid.data_len == 0)
       {
         if (!nvruim_clear_cache(NVRUIM_CLEAR_ALL_CACHE,
                                 NULL))
         {
           MSG_ERROR("Refresh: NVRUIM Cache could not be cleared",0,0,0);
           extra_param_ptr->refresh_data.send_evt = TRUE;
           extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
           slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
           return MMGSDI_CAN_NOT_REFRESH;
         }
      }
#endif /* FEATURE_RUIM_CDMA_REFRESH */
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      slot_ptr->refresh_state                =
        MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }

    /*Walk through all nodes and change the rsp received to FALSE*/
    node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr               = node_ptr->next_ptr;
    }
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_ok_init_rsp*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_OK_FCN_RSP

  DESCRIPTION:
    This function will process the clients response to ok_to_fcn.

    This state will perform the following operations:
    1. If all have been received or any of them has resulted in error then
       transition to next state
       •WAIT_FOR_INIT_COMPLETE_STATE
           a.If the mode is INIT/RESET(default)/INIT_FCN/INIT_FFCN/3G
             Session Reset(default)
           b.Send START event to relevant clients
           c.Send command to GSDI to start init if it is provisioning
             application.
        •WAIT_FOR_FCN_COMPLETE_STATE:
           a.If all clients have responded with OK and mode is FCN
              i. Send START Event to all relevant clients
              ii.Transition to this state
        •READY_STATE:
           a.Error Case - If any client has voted not to init
              i. clean up
              ii.send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_ok_fcn_rsp(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  boolean                   ok_for_fcn     = TRUE;
  boolean                   wait_for_rsp   = FALSE;
  mmgsdi_refresh_node_type *node_ptr       = NULL;
#ifdef FEATURE_RUIM_CDMA_REFRESH
  int                       i              = 0;
  mmgsdi_return_enum_type   mmgsdi_status  = MMGSDI_SUCCESS;
  uim_items_enum_type       uim_file_items = UIM_NO_SUCH_ITEM;
#endif /* FEATURE_RUIM_CDMA_REFRESH */

  MSG_HIGH("FUNCTION mmgsdi_refresh_process_ok_init_rsp",0,0,0);

  /* Parameter Check */
  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr->node_ptr);

  MSG_HIGH("REFRESH: OK_TO_FCN response received from client id_low = 0x%x, id_high = 0x%x, with status = 0x%x",
           (req_ptr->request_header.client_id & 0xFFFFFFFF),
           (req_ptr->request_header.client_id>>32 & 0xFFFFFFFF),
           req_ptr->refresh.ok_req);

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;

  /*Check if all the responses have been received*/
  node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
  while( node_ptr != NULL)
  {
    if(node_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
    {
      if(node_ptr->client_id    == req_ptr->request_header.client_id &&
         node_ptr->rsp_received == FALSE)
      {
        node_ptr->rsp_received = TRUE;
      }
      if(node_ptr->rsp_received == TRUE &&
         req_ptr->refresh.ok_req == FALSE)
      {
        ok_for_fcn = FALSE;
        break;
      }
      if(node_ptr->rsp_received == FALSE)
      {
        /*All responses have not been received, keep waiting*/
        wait_for_rsp = TRUE;
      }

    }
    node_ptr = node_ptr->next_ptr;
  }

  /* Client rejected the fcn */
  if(!ok_for_fcn)
  {
    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    mmgsdi_refresh_info_ptr->refresh_status= MMGSDI_CAN_NOT_REFRESH;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }

  if(!wait_for_rsp)
  {
    /*Walk through all nodes and change the rsp received to FALSE*/
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer);
    node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr->stage        = MMGSDI_REFRESH_STAGE_START;
      node_ptr               = node_ptr->next_ptr;
    }
    if(mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_NAA_INIT_FCN)
    {
      /*----------------------------------------------------------------------
       •    WAIT_FOR_INIT_COMPLETE_STATE:
           a.   If all clients have responded with OK
              i.    Send START Event to all relevant clients
              ii.   Transition to this state
      ------------------------------------------------------------------------*/
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }
    else
    {
      /*----------------------------------------------------------------------
       •    MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE:
           a.   If all clients have responded with OK
              i.    Send START Event to all relevant clients
              ii.   Transition to this state
       ------------------------------------------------------------------------*/
#ifdef FEATURE_RUIM_CDMA_REFRESH
       /* NVRUIM Cache Clearing process checks if another task is accessing the cache,
       if it is we want to stop the Refresh Request.
       Clear nvruim cache only if default app */
       if (mmgsdi_refresh_info_ptr->aid.aid.data_len == 0)
       {
         for (i=0; i<mmgsdi_refresh_info_ptr->refresh_data.refresh_files.num_files; i++)
         {
           mmgsdi_status = mmgsdi_util_convert_to_uim_items_enum(
              mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_list_ptr[i],
              &uim_file_items);
           if (mmgsdi_status != MMGSDI_SUCCESS)
           {
             MMGSDI_DEBUG_MSG_ERROR("Can not convert to uim enum 0x%x",
               mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_list_ptr[i],0,0);
             break;
           }
           if (!nvruim_clear_cache(1, &uim_file_items))
           {
             MSG_ERROR("Refresh: NVRUIM Cache could not be cleared",0,0,0);
             extra_param_ptr->refresh_data.send_evt = TRUE;
             extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
             slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
             return MMGSDI_CAN_NOT_REFRESH;
           }
         }
      }
#endif /* FEATURE_RUIM_CDMA_REFRESH */
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE;
    }
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_ok_fcn_rsp*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_req_process_start

  DESCRIPTION:
    A Refresh Command has been received

    This state will perform the following operations:
    1.  Figure out if it is prov app
    2.  Figure what mode it is
    3.  If the mode is FCN and it is prov app and it is a call stack file, the
        change the mode to INIT
    4.  Build the Linked list for Clients that will be involved in this refresh mode
    5.  Transition into one of the following states –
       •    WAIT_FOR_OK_TO_INIT_STATE:
           a.   INIT/RESET/3G SESSION RESET/INIT_FCN will change the state to this state
           b.   Send OK_TO_INIT events to relevant clients
       •    WAIT_FOR_OK_TO_FCN_STATE:
           a.   If it is FCN node and there are clients interested in files in FCN
           b.   Send OK_TO_FCN event to relevant clients
       •    READY_STATE:
           i.   Error Case - If it is unknown mode/ app is not active, clean up Send TR
           ii.  Success Case  - If Refresh is complete and no one is interested in this
                mode. , clean up, send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_start(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr
 )
{
  mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_SUCCESS;
  uint32                     aid_index      = 0;

  MSG_HIGH("FUNCTION mmgsdi_refresh_req_process_start",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);

  /* check if the aid is in the EF-DIR */
  if(req_ptr->refresh.card_req.aid.aid.data_len != 0 )
  {
    aid_index = mmgsdi_util_find_aid_index(MMGSDI_SLOT_1,
                                           req_ptr->refresh.card_req.aid);
    if (!mmgsdi_util_is_valid_aid_index(aid_index))
    {
      MSG_HIGH("Refresh: Refresh for app not in the EF-DIR, Refresh failed", 0, 0, 0);
      extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
      slot_ptr->refresh_state             = MMGSDI_REFRESH_READY_STATE;
      return MMGSDI_REFRESH_FAIL_INVALID_AID;
    }
  }

  if(mmgsdi_copy_to_refresh_ptr(&req_ptr->request_header,
                                &req_ptr->refresh.card_req) != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh:  Could not copy to refresh ptr",0,0,0);
    return MMGSDI_CAN_NOT_REFRESH;
  }

  /*----------------------------------------------------
     1. Figure out what mode it is
  ------------------------------------------------------*/
  switch(req_ptr->refresh.card_req.refresh_mode)
  {
    case MMGSDI_REFRESH_RESET:
    case MMGSDI_REFRESH_RESET_AUTO:
      mmgsdi_status = mmgsdi_refresh_process_full_reset(slot_ptr,
                                                        &req_ptr->refresh.card_req,
                                                        extra_param_ptr);
      break;
    case MMGSDI_REFRESH_NAA_APP_RESET:
      mmgsdi_status = mmgsdi_refresh_process_reset(slot_ptr,
                                                   &req_ptr->refresh.card_req,
                                                   extra_param_ptr);
      break;
    case MMGSDI_REFRESH_NAA_INIT:
    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
      mmgsdi_status = mmgsdi_refresh_process_init(slot_ptr,
                                                  &req_ptr->refresh.card_req,
                                                  extra_param_ptr,
                                                  req_ptr->refresh.card_req.refresh_mode);
      break;
    case MMGSDI_REFRESH_NAA_INIT_FCN:
    case MMGSDI_REFRESH_3G_SESSION_RESET:
      mmgsdi_status = mmgsdi_refresh_process_init_fcn(slot_ptr,
                                                      &req_ptr->refresh.card_req,
                                                      extra_param_ptr);
      break;
    case MMGSDI_REFRESH_NAA_FCN:
      mmgsdi_status = mmgsdi_refresh_process_fcn(slot_ptr,
                                                 &req_ptr->refresh.card_req,
                                                 extra_param_ptr);
      break;
    default:
      MSG_ERROR("Refresh: Invalid Mode 0x%x",req_ptr->refresh.card_req.refresh_mode,0,0);
      return MMGSDI_ERROR;
  }

  if(mmgsdi_refresh_info_ptr != NULL)
  {
    mmgsdi_refresh_info_ptr->refresh_status = mmgsdi_status;
    extra_param_ptr->refresh_data.mode      = mmgsdi_refresh_info_ptr->mode;

    if(mmgsdi_status == MMGSDI_REFRESH_IN_PROGRESS)
    {
      /*Reset all Notify inits since we are about to start refresh now*/
      mmgsdi_refresh_reset_notify_init();
    }
  }
  return mmgsdi_status;
} /* mmgsdi_refresh_req_process_start */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_MAIN

  DESCRIPTION:
    This function handles all refresh requests  - it is also the entry into the
    refresh state machine

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_main(
  mmgsdi_task_cmd_type *task_cmd_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_client_req_extra_info_type *extra_param_ptr      = NULL;
  int32                              index                = 0;
  mmgsdi_refresh_state_enum_type     orig_state           = MMGSDI_REFRESH_READY_STATE;
  mmgsdi_slot_data_type             *mmgsdi_data_slot_ptr = NULL;
  mmgsdi_protocol_enum_type          protocol             = MMGSDI_NO_PROTOCOL;

  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  switch(task_cmd_ptr->cmd.cmd.refresh_req.request_header.slot_id)
  {
    case MMGSDI_SLOT_1:
      mmgsdi_data_slot_ptr = &mmgsdi_data_slot1;
      break;
    case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      mmgsdi_data_slot_ptr = &mmgsdi_data_slot2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
      break;
    default:
      MSG_ERROR("Refresh: Invalid Slot 0x%x",
                 task_cmd_ptr->cmd.cmd.refresh_req.request_header.slot_id,0,0);
      return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_data_slot_ptr);

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                   MMGSDI_REFRESH_REQ,
                   (void*)&task_cmd_ptr->cmd.cmd.refresh_req,
                   &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Could not allocate memory",0,0,0);
    return mmgsdi_status;
  }

  /*Fill the client Table*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Could not get free index",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &task_cmd_ptr->cmd.cmd.refresh_req.request_header,
                    NULL,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Could not allocate request table",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  extra_param_ptr->refresh_data.refresh_req_type =
    task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type;

  if(mmgsdi_refresh_info_ptr != NULL)
  {
    extra_param_ptr->refresh_data.mode = mmgsdi_refresh_info_ptr->mode;
  }

  orig_state = mmgsdi_data_slot_ptr->refresh_state;

  MSG_HIGH("Refresh: Refresh State is 0x%x, Req Type is 0x%x",
            mmgsdi_data_slot_ptr->refresh_state,
            task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type,0);

  /*Reset the mmgsdi_status to ERROR*/
  mmgsdi_status = MMGSDI_ERROR;

  switch (task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type)
  {
    case MMGSDI_REFRESH_ORIG_REFRESH_REQ:
    case MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ:
      switch(mmgsdi_data_slot_ptr->refresh_state)
      {
       /*--------------------------------------------------
         Ready to receive Refresh Commands from the card
       ---------------------------------------------------*/
        case MMGSDI_REFRESH_READY_STATE:
          mmgsdi_status = mmgsdi_refresh_req_process_start(
                        mmgsdi_data_slot_ptr,
                        &task_cmd_ptr->cmd.cmd.refresh_req,
                        extra_param_ptr);
          break;
        default:
          /*Refresh Request came in a state when there was a refresh possibly in
            progress, send out an error. Error will be sent to the originating
            client in mmgsdi_refresg_rsp.
            No events need to be sent*/
          MSG_ERROR("New Refresh Request in Bad State",0,0,0);

          extra_param_ptr->refresh_data.mode =
            task_cmd_ptr->cmd.cmd.refresh_req.refresh.card_req.refresh_mode;
          extra_param_ptr->refresh_data.stage =
            MMGSDI_REFRESH_STAGE_END_FAILED;

          mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
          break;
      }
      break;

     case MMGSDI_REFRESH_ORIG_PUP_REQ:
       mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
       if(mmgsdi_status != MMGSDI_SUCCESS)
       {
         MSG_ERROR("Problem determining protocol, cannot procees with refresh",0,0,0);
         extra_param_ptr->refresh_data.mode =
           task_cmd_ptr->cmd.cmd.refresh_req.refresh.card_req.refresh_mode;
         extra_param_ptr->refresh_data.stage =
           MMGSDI_REFRESH_STAGE_END_FAILED;

         mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
         break;
       } /* mmgsdi_status is not SUCCESS */

       /*CARD PUP can be received in any state. It needs to be handled even
         if the first refresh was not complete*/

       /*A refresh may have been in progress and a new refresh command is sent
        First Clean up*/
       if(mmgsdi_refresh_info_ptr != NULL)
       {
         extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
         extra_param_ptr->refresh_data.send_evt = TRUE;

         (void)mmgsdi_refresh_send_evt(extra_param_ptr,
                                       &task_cmd_ptr->cmd.cmd.refresh_req.request_header);

         mmgsdi_refresh_rsp_clean_up();
       }

       /*Now restart the command*/
       mmgsdi_status = mmgsdi_refresh_req_process_start(
                        mmgsdi_data_slot_ptr,
                        &task_cmd_ptr->cmd.cmd.refresh_req,
                        extra_param_ptr);
       break;

     case MMGSDI_REFRESH_ORIG_REG_REQ:
       mmgsdi_status = mmgsdi_refresh_req_process_reg(
                       &task_cmd_ptr->cmd.cmd.refresh_req);
       break;

     case MMGSDI_REFRESH_ORIG_DEREG_REQ:
       mmgsdi_status = mmgsdi_refresh_req_process_dereg(
                      &task_cmd_ptr->cmd.cmd.refresh_req);
       break;

     case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
       switch(mmgsdi_data_slot_ptr->refresh_state)
       {
         case MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE:
           /*--------------------------------------------------------
            Wait for OK_TO_INIT /OK_TO_FCN Responses from all relevant
            clients.  Only mmgsdi_ok_to_refresh can be handled
            in this state.
           ---------------------------------------------------------*/
           mmgsdi_status = mmgsdi_refresh_process_ok_init_rsp(
                             mmgsdi_data_slot_ptr,
                            &task_cmd_ptr->cmd.cmd.refresh_req,
                             extra_param_ptr);
           break;

         case MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE:
           mmgsdi_status = mmgsdi_refresh_process_ok_fcn_rsp(
                           mmgsdi_data_slot_ptr,
                           &task_cmd_ptr->cmd.cmd.refresh_req,
                           extra_param_ptr);
           break;

         default:
           MSG_ERROR("Orig Ok to Refresh received in bad state 0x%x",
                      mmgsdi_data_slot_ptr->refresh_state,0,0);
           break;
       }
       break;

     case MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
       switch(mmgsdi_data_slot_ptr->refresh_state)
       {
         /*------------------------------------------------------------
          Wait for Refresh Complete commands from clients who are
          involved in the INIT Process or the FCN Process
          mmgsdi_refresh_complete can be handled in this state:
         ------------------------------------------------------------*/
         case MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE:
         case MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE:
           mmgsdi_status = mmgsdi_refresh_process_complete_init(
                           mmgsdi_data_slot_ptr,
                           &task_cmd_ptr->cmd.cmd.refresh_req,
                           extra_param_ptr);
           break;
         case MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE:
           mmgsdi_status = mmgsdi_refresh_process_complete_fcn(
                              mmgsdi_data_slot_ptr,
                             &task_cmd_ptr->cmd.cmd.refresh_req,
                             extra_param_ptr);
           break;
         default:
           MSG_ERROR("Orig Complete received in bad state 0x%x",
                      mmgsdi_data_slot_ptr->refresh_state,0,0);
           break;

       }
       break;
     default:
       MSG_ERROR("Refresh: Bad Request, cannot process anything 0x%x",
                  task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type,0,0);
       break;
  }

  (void)mmgsdi_refresh_send_evt(extra_param_ptr,
                                &task_cmd_ptr->cmd.cmd.refresh_req.request_header);

  if(orig_state != mmgsdi_data_slot_ptr->refresh_state)
  {
#ifndef PLATFORM_LTK
    mmgsdi_refresh_start_timer(mmgsdi_data_slot_ptr);
#endif /* PLATFORM_LTK */
  }

  if(mmgsdi_status == MMGSDI_SUCCESS ||
     mmgsdi_status == MMGSDI_REFRESH_IN_PROGRESS)
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                   index, MMGSDI_REFRESH_REQ,
                   mmgsdi_status);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
}/*mmgsdi_refresh_main*/


/*===========================================================================
  FUNCTION mmgsdi_util_check_fcn_list

  DESCRIPTION:
    This function checks the fcn list for any additional files
    being read during a fcn refresh

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
   None

  SIDE EFFECTS:
    Additional File read flag is set if any files other than the ones
    listed in FCN command are read from the card
 ===========================================================================*/
 #ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_check_fcn_list(
  uim_items_enum_type         uim_file
)
{
  gsdi_refresh_fcn_notify_node_type  * current_node_ptr = NULL;
  uint8                                i                = 0;
  boolean                              file_found       = FALSE;
  boolean                              refresh_pending  = FALSE;

  if(uim_file == UIM_NO_SUCH_ITEM)
  {
    /*Nothing to do*/
    MSG_ERROR("Refresh:  FCN Check cannot be performed, filename invalid",0,0,0);
    return;
  }

  current_node_ptr = &refresh_fcn_notify_list_node;

  while(current_node_ptr!= NULL  &&
        refresh_fcn_notify_list_node.additional_files_read == FALSE &&
        file_found == FALSE)
  {
    /*the num_files_in_notify_file_list will have only those files for which there is
      a refresh in progress AND the client is interested*/
    for(i=0;i<current_node_ptr->fcn_notify_data.num_files_in_notify_file_list &&
            i<GSDI_MAX_FILE_CHANGE_NOTIFICATIONS;i++)
    {
      refresh_pending = TRUE;
      if(uim_file == current_node_ptr->fcn_notify_data.notify_file_list[i])
      {
         /*Refresh is being performed on this file*/
         file_found = TRUE;
         break;
      }
    }
    current_node_ptr = current_node_ptr->next_p;
  }

  /*ensure that we are actually going through the FCN list*/
  if(file_found == FALSE && refresh_pending)
  {
    MSG_HIGH("Refresh: Additional Files are being read",0,0,0);
    /*The information is only required in the head node. Ignore the others*/
    refresh_fcn_notify_list_node.additional_files_read = TRUE;
    if(mmgsdi_refresh_info_ptr != NULL)
    {
      mmgsdi_refresh_info_ptr->additional_files_read = TRUE;
    }
  }

  file_found = FALSE;
  refresh_pending = FALSE;
}/*mmgsdi_util_check_fcn_list*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
  FUNCTION mmgsdi_util_check_client_fcn_list

  DESCRIPTION:
    This function checks the fcn list for any additional files
    being read during a fcn refresh

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    Additional File read flag is set if any files other than the ones
    listed in FCN command are read from the card
 ===========================================================================*/
 #ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_check_client_fcn_list(
  mmgsdi_client_id_type       client_id,
  uim_items_enum_type         uim_file
)
{
  uint8                                i                = 0;
  boolean                              file_found       = FALSE;
  boolean                              refresh_pending  = FALSE;
  mmgsdi_file_enum_type                mmgsdi_file      = MMGSDI_NO_FILE_ENUM;
  mmgsdi_refresh_node_type           * current_node_ptr = NULL;
  int32                                client_index     = 0;
  int32                                session_index    = 0;
  mmgsdi_session_info_type           * session_ptr      = NULL;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;

  if(uim_file == UIM_NO_SUCH_ITEM)
  {
    MSG_ERROR("Refresh:  FCN Check cannot be performed, filename invalid",0,0,0);
    return;
  }

  if(refresh_fcn_notify_list_node.head_p != NULL)
  {
    mmgsdi_util_check_fcn_list(uim_file);
  }
  if((mmgsdi_refresh_info_ptr) &&
     (mmgsdi_refresh_info_ptr->node_ptr) &&
     (mmgsdi_refresh_info_ptr->orig_mode == MMGSDI_REFRESH_NAA_FCN) &&
     (mmgsdi_refresh_info_ptr->refresh_data.refresh_files.num_files > 0) &&
     (mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_list_ptr) &&
     (mmgsdi_refresh_info_ptr->refresh_status == MMGSDI_REFRESH_IN_PROGRESS))
  {
   /*the num_files_in_notify_file_list will have only those files for which there is
     a refresh in progress AND the client is interested*/

    for(i=0;i<mmgsdi_refresh_info_ptr->refresh_data.refresh_files.num_files;i++)
    {
      refresh_pending  = TRUE;
      mmgsdi_file      = MMGSDI_NO_FILE_ENUM;

      if(mmgsdi_util_convert_from_uim_items_enum (uim_file, &mmgsdi_file) !=
         MMGSDI_SUCCESS)
      {
        MSG_HIGH("Refresh: mmgsdi_util_convert_from_uim_items_enum() failed",0,0,0);
      }
      if(mmgsdi_file ==
           mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_list_ptr[i])
      {
        mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                       &client_index,
                                                       &session_index);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
           MSG_ERROR("Invalid Client Id 0x0x",client_id,0,0);
           break;
        }
        session_ptr = mmgsdi_util_get_session_ptr(client_index,
                                                  mmgsdi_refresh_info_ptr->aid);

        if (session_ptr == NULL)
        {
          MSG_ERROR("NULL pointer returned by mmgsdi_util_get_session_ptr",
                    0,0,0);
          break;
        }

        if(session_ptr->app_info_ptr != NULL)
        {
          mmgsdi_status = mmgsdi_util_cmp_aid(&session_ptr->app_info_ptr->app_data,
                                              &mmgsdi_refresh_info_ptr->aid);

          /*If session_ptr is NULL, then we do not need the checks given below,
            the selected app does not have any aid*/
        }

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          /*===================================================================
            1. AID1 could be different from AID2
           ===================================================================*/
          if(session_ptr->app_info_ptr != NULL &&
             mmgsdi_refresh_info_ptr->aid.aid.data_len != 0 &&
             session_ptr->app_info_ptr->app_data.aid.data_len != 0)
          {
            /*Same File, different session*/
            break;
          }
          /*===================================================================
           2.  AID1 has length but AID2 has no length
               Check if AID1 is same as default app
           ===================================================================*/
          else if(mmgsdi_util_get_sel_app() == session_ptr->app_info_ptr  &&
                  mmgsdi_refresh_info_ptr->aid.aid.data_len == 0)
          {
            /*Both are default apps, it is not an additional file read*/
            file_found = TRUE;
          }
          /*===================================================================
           3. AID1 has no length but AID2 has length --> Not possible
              because if there is a session, then the length is same as
              default app
              - refer to mmgsdi_util_get_session_ptr

           4. AID1 has no length and AID2 has no length
           =================================================================*/
          else if(mmgsdi_refresh_info_ptr->aid.aid.data_len == 0 &&
                  session_ptr->app_info_ptr != NULL &&
                  session_ptr->app_info_ptr->app_data.aid.data_len == 0)
          {
            /*Both are default apps, it is not an additional file read*/
            file_found = TRUE;
          }
        }
        else
        {
          /*File is being read by some client from the same aid*/
          file_found = TRUE;
        }
        current_node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
        while(current_node_ptr!= NULL)
        {
          if(client_id == current_node_ptr->client_id)
          {
            /*Remove the file from the respective client */
            mmgsdi_remove_file_from_node(client_id, mmgsdi_file);
            break;
          }
          current_node_ptr = current_node_ptr->next_ptr;
        }/* End of while */
      }/* End if if */
      if(file_found == TRUE)
      {
        break;
      }
    }/* End of for */

    /*ensure that we are actually going through the FCN list*/
    if(file_found == FALSE && refresh_pending)
    {
      MSG_HIGH("Refresh: Additional Files are being read",0,0,0);
      /*The information is only required in the head node. Ignore the others*/
      mmgsdi_refresh_info_ptr->additional_files_read = TRUE;
    }
  }
}/*mmgsdi_util_check_client_fcn_list*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


