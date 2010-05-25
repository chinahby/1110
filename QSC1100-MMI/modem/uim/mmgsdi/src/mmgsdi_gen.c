/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the Generic commands that are not protocol dependent

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_gen.c#3 $$ $DateTime: 2009/05/15 02:20:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/20/08   sun     Allow protocol to be sent as part of Card Reset
09/16/08   kk      Updated arg to mmgsdi_util_queue_mmgsdi_uim_report_rsp in
                   create pkcs15 command handler
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
11/15/07   sun     Added support for NAA refresh
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO Supports.
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
01/17/07   sp      Removed pin_just_verified flag
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/19/06   sun     Fixed Slot for Refresh
04/18/06   nk      featurized file and converted macro calls to function calls
03/21/06   tml     lint
03/14/06   sun     If the refresh fails , then remember to refresh
                   after the conditions are satisified
02/23/05   tml     Assign a discard cnf cb pointer to avoid overwriting
                   gsdilib cmd q during the refresh reset process.
12/14/05   tml     MMGSDI Cleanup
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
11/03/05   sun     Changed Refresh to Refresh Command
08/25/05   pv      Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gsdi.h"
#include "mmgsdi.h"
#include "uim.h"
#include "mmgsdiutil.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_uim_uicc.h"

#ifndef FEATURE_MMGSDI_GEN_OPTIMIZATION

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PUP

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

   DEPENDENCIES:

   LIMITATIONS:

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_pup(
  const mmgsdi_card_pup_req_type  *req_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;

  MSG_MED("MMGSDI_GEN_PROCESS_CARD_PUP", 0, 0, 0);
  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  if (req_ptr->option == MMGSDI_CARD_POWER_UP_INITIAL_PUP)
  {
    MSG_ERROR("Should not handle CardPower Up Initial PUP Option in this function", 0, 0, 0);
    return MMGSDI_ERROR;
  }
  else
  {
    uim_cmd_type*                      uim_cmd_ptr     = NULL;
    uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
    int32                              index           = 0;
    mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;

    /*
     * Convert to UIM slot.
     */
    mmgsdi_status = mmgsdi_util_convert_uim_slot(
                      req_ptr->request_header.slot_id, &uim_slot);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
    /*
     * Allocate and populate extra parameter.
     */
    mmgsdi_status =  mmgsdi_util_alloc_and_populate_extra_param(
                       MMGSDI_CARD_PUP_REQ, (void*)req_ptr, &extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
    /*
     * 1) Get a UIM buffer for the request.
     * 2) Check for if the buffer is NULL or not.
     */
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);

    if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      return mmgsdi_status;
    }
    /*
     * Populate UIM information.
     */
    uim_cmd_ptr->hdr.command  = UIM_POWER_UP_UIM_PASSIVE_F;
    uim_cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;
    uim_cmd_ptr->hdr.slot     = uim_slot;
    /*
     * Get a new index from client req info table for response data.
     * Set the uim data pointer to the client request table data buffer.
     */
    mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return mmgsdi_status;
    }
    mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                      index,
                      &req_ptr->request_header,
                      uim_cmd_ptr,
                      extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return mmgsdi_status;
    }
    /*
     * Assign uim_cmd_ptr's user data.
     */
    uim_cmd_ptr->hdr.user_data = int32touint32(index);
    MSG_HIGH("Sending down card power up command to UIM", 0, 0, 0);
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
      MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
    }
  }
  return mmgsdi_status;
} /* mmgsdi_gen_process_card_pup */


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PDOWN

   DESCRIPTION:
     This function will send down a power down command to the UIM
     module to power down the CARD.

   DEPENDENCIES:

   LIMITATIONS:

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_pdown(
  const mmgsdi_card_pdown_req_type *req_ptr)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;

  MSG_MED("MMGSDI_GEN_PROCESS_CARD_PDOW", 0, 0, 0);
  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);
  /*
   * Convert to UIM slot.
   */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(
                    req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * Allocate and populate extra parameter.
   */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                        MMGSDI_CARD_PDOWN_REQ,
                        (void*)req_ptr, &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * 1) Get a UIM buffer for the request.
   * 2) Check for if the buffer is NULL or not.
   */
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                         sizeof(uim_cmd_type),mmgsdi_status);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }
  /*
   * Populate UIM information.
   */
  uim_cmd_ptr->hdr.command        = UIM_POWER_DOWN_F;
  uim_cmd_ptr->hdr.protocol       = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot           = uim_slot;


  /*
   * Populate the power down option to UIM
   */
  if (req_ptr->option == MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI)
  {
    uim_cmd_ptr->pdown.pdown_option = UIM_OPTION_NOTIFY_PDOWN;
  }
  else
  {
    uim_cmd_ptr->pdown.pdown_option = UIM_OPTION_NO_NOTIFY_PDOWN;
  }

  /*
   * Get a new index from client req info table for response data.
   * Set the uim data pointer to the client request table data buffer.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    uim_cmd_ptr,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /*
   * Assign uim_cmd_ptr's user data.
   */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);
  MSG_HIGH("Sending POWER down command to UIM", 0, 0, 0);
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  return mmgsdi_status;
} /* mmgsdi_gen_process_card_pdown */


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_RESET

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

   DEPENDENCIES:

   LIMITATIONS:

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_reset(
  const mmgsdi_card_reset_req_type  *req_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  int32                              index           = 0;

  MSG_MED("MMGSDI_GEN_PROCESS_CARD_RESET", 0, 0, 0);
  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /*
   * Convert to UIM slot.
   */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(
                    req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * 1) Get a UIM buffer for the request.
   * 2) Check for if the buffer is NULL or not.
   */
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    return mmgsdi_status;
  }
  /*
   * Populate UIM information.
   */
  uim_cmd_ptr->hdr.protocol = req_ptr->protocol;
  uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;
  uim_cmd_ptr->hdr.command  = UIM_RESET_F;
  /*
   * Get a new index from client req info table for response data.
   * Set the uim data pointer to the client request table data buffer.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    uim_cmd_ptr,
                    NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /*
   * Assign uim_cmd_ptr's user data.
   */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);
  MSG_HIGH("Sending down reset command to UIM", 0, 0, 0);
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  return mmgsdi_status;
} /* mmgsdi_gen_process_card_reset */


/*===========================================================================
FUNCTION:      MMGSDI_GEN_BUILD_CARD_RESET

DESCRIPTION:
  This function is used to build a card reset command

DEPENDENCIES:

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_build_card_reset(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               curr_req_type,
  uim_protocol_type                  protocol,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr       = NULL;
  mmgsdi_return_enum_type               mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_card_reset_req_type           *card_reset_ptr     = NULL;
  int32                                 total_mem_len      = 0;
  uint32                                task_cmd_len       = 0;

  MSG_HIGH("MMGSDI_GEN_BUILD_CARD_RESET, client_id 0x%x", client_id, 0, 0);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);


  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.card_reset_req,
                         curr_req_type);

  card_reset_ptr = &task_cmd_ptr->cmd.cmd.card_reset_req;

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1. client_id       - Client ID Obtained
    2. request_type    - Request Command Type
    3. request_len     - Length of message + header
    4. payload_len     - Length of the message payload
    5. request_id_ptr  - Request ID (task_tcb_ptr)
    6. slot_id         - Slot that the request is to be performed on
    7. client_data     - Pointer to Client Data
    8. response_cb     - Response Callback Pointer
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                       = curr_req_type;
  card_reset_ptr->request_header.client_id         = client_id;
  card_reset_ptr->request_header.request_type      = curr_req_type;
  card_reset_ptr->request_header.orig_request_type = curr_req_type;
  card_reset_ptr->request_header.request_id_ptr    = rex_self();
  card_reset_ptr->request_header.slot_id           = slot_id;
  card_reset_ptr->request_header.client_data       = client_ref;
  card_reset_ptr->request_header.response_cb       = response_cb_ptr;
  card_reset_ptr->request_header.request_len       = total_mem_len;
  card_reset_ptr->request_header.payload_len       = card_reset_ptr->request_header.request_len -
                                                          uint32toint32(sizeof(mmgsdi_request_header_type));

  card_reset_ptr->protocol = protocol;

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  return mmgsdi_status;
}


/*=============================================================================
  FUNCTION: MMGSDI_GEN_PROCESS_GET_ATR

  DESCRIPTION:
    Function will get the ATR (Answer to Reset)from the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_get_atr (
  mmgsdi_cmd_enum_type           cmd,
  const mmgsdi_get_atr_req_type* req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;

  MSG_MED("MMGSDI_PROCESS_GET_ATR_REQ", 0, 0, 0);

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  // No Extra Param

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index, &req_ptr->request_header,
                                                                  NULL,
                                                                  NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, cmd, MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_gen_process_get_atr */
/*lint !e715 */


/*===========================================================================
FUNCTION MMGSDI_GEN_PROCESS_CARD_READER_STATUS

DESCRIPTION
  This function is called to send a request to the UIM To retrieve the
  Card Reader Status.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_reader_status (
  mmgsdi_cmd_enum_type                       cmd,
  const mmgsdi_card_reader_status_req_type * req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;

  MSG_MED("MMGSDI_GEN_PROCESS_CARD_READER_STATUS", 0, 0, 0);

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  // No Extra Param
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index,
                                                                  &req_ptr->request_header,
                                                                  NULL,
                                                                  NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, cmd,  MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_gen_process_card_reader_status */


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION*/
