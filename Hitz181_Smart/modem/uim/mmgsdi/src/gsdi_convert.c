/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U T I L I T Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI
  before the command is being sent to UIM Module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_convert.c#3 $$ $DateTime: 2009/05/15 02:20:16 $

when       who     what, where, why                               
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
11/04/08   nb      Klockwork Fixes
09/02/08   nb      Klockwork Fixes
12/11/07   js      Added conversion function to map gsdi and mmgsdi status 
11/19/07   sun     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "mmgsdilib.h"
#include "mmgsdiutil.h"
#include "gsdi.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_TO_GSDI_STATUS

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_return_enum_type and
     returns the corresponding gsdi_returns_T

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Null Pointer was provided.


   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_gsdi_status(
  mmgsdi_return_enum_type  mmgsdi_status,
  gsdi_returns_T          *gsdi_status_ptr)
{
  if (gsdi_status_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }
  switch ( mmgsdi_status )
  {
    case MMGSDI_SUCCESS:
      *gsdi_status_ptr = GSDI_SUCCESS;
      break;

    case MMGSDI_ERROR:
      *gsdi_status_ptr = GSDI_ERROR;
      break;

    case MMGSDI_INCORRECT_PARAMS:
      *gsdi_status_ptr = GSDI_INCORRECT_PARAMS;
      break;

    case MMGSDI_NOT_INIT:
      *gsdi_status_ptr = GSDI_NOT_INIT;
      break;

    case MMGSDI_NOT_SUPPORTED:
      *gsdi_status_ptr = GSDI_NOT_SUPPORTED;
      break;

    case MMGSDI_CMD_QUEUE_FULL:
     *gsdi_status_ptr = GSDI_CMD_QUEUE_FULL;
     break;

    case MMGSDI_INCORRECT_CODE:
      *gsdi_status_ptr = GSDI_INCORRECT_CODE;
      break;

    case MMGSDI_ACCESS_DENIED:
      *gsdi_status_ptr = GSDI_ACCESS_DENIED;
      break;

    case MMGSDI_INCOMPAT_PIN_STATUS:
      *gsdi_status_ptr = GSDI_INCOMPAT_PIN_STATUS;
      break;

    case MMGSDI_CODE_BLOCKED:
      *gsdi_status_ptr = GSDI_CODE_BLOCKED;
      break;

    case MMGSDI_SIM_TECHNICAL_PROBLEMS:
      *gsdi_status_ptr = GSDI_SIM_TECHNICAL_PROBLEMS;
      break;

    case MMGSDI_PIN_NOT_INITIALIZED:
      *gsdi_status_ptr = GSDI_PIN_NOT_INITIALIZED;
      break;

    case MMGSDI_UNKNOWN_INST_CLASS:
      *gsdi_status_ptr = GSDI_UNKNOWN_INST_CLASS;
      break;

    case MMGSDI_WARNING_NO_INFO_GIVEN:
      *gsdi_status_ptr = GSDI_WARNING_NO_INFO_GIVEN;
      break;

    case MMGSDI_UIM_CMD_TIMEOUT:
      *gsdi_status_ptr = GSDI_UIM_CMD_TIMEOUT;
      break;

    case MMGSDI_CODE_PERM_BLOCKED:
      *gsdi_status_ptr = GSDI_CODE_PERM_BLOCKED;
      break;

    case MMGSDI_SIM_BUSY:
      *gsdi_status_ptr = GSDI_SIM_BUSY;
      break;

    case MMGSDI_INVALIDATION_CONTRADICTION_STATUS:
      *gsdi_status_ptr = GSDI_INVALIDATION_CONTRADICTION_STATUS;
      break;

    default:
      MSG_ERROR("UNRECOGNIZED RETURN: 0x%x",mmgsdi_status,0,0);
      *gsdi_status_ptr = GSDI_ERROR;
      break;
  }

  return MMGSDI_SUCCESS;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_GSDI_STATUS_TO_MMGSDI

   DESCRIPTION:
     This function checks for valid range of the gsdi_returns_T,  and
     returns the corresponding mmgsdi_return_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Null Pointer was provided.


   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_gsdi_status_to_mmgsdi(
  gsdi_returns_T            gsdi_status,
  mmgsdi_return_enum_type  *mmgsdi_status_ptr)
{
  if (mmgsdi_status_ptr == NULL)
  {
    MSG_ERROR("mmgsdi_status_ptr Null",0,0,0);
    return MMGSDI_ERROR;
  }
  switch ( gsdi_status )
  {
    case GSDI_SUCCESS:
      *mmgsdi_status_ptr = MMGSDI_SUCCESS;
      break;

    case GSDI_ERROR:
      *mmgsdi_status_ptr = MMGSDI_ERROR;
      break;

    case GSDI_NOT_FOUND:
      *mmgsdi_status_ptr = MMGSDI_NOT_FOUND;
      break;

    case GSDI_INCORRECT_PARAMS:
      *mmgsdi_status_ptr = MMGSDI_INCORRECT_PARAMS;
      break;

    case GSDI_NOT_INIT:
      *mmgsdi_status_ptr = MMGSDI_NOT_INIT;
      break;

    case GSDI_NOT_SUPPORTED:
      *mmgsdi_status_ptr = MMGSDI_NOT_SUPPORTED;
      break;

    case GSDI_CMD_QUEUE_FULL:
     *mmgsdi_status_ptr = MMGSDI_CMD_QUEUE_FULL;
     break;

    case GSDI_INCORRECT_CODE:
      *mmgsdi_status_ptr = MMGSDI_INCORRECT_CODE;
      break;

    case GSDI_ACCESS_DENIED:
      *mmgsdi_status_ptr = MMGSDI_ACCESS_DENIED;
      break;

    case GSDI_INCOMPAT_PIN_STATUS:
      *mmgsdi_status_ptr = MMGSDI_INCOMPAT_PIN_STATUS;
      break;

    case GSDI_CODE_BLOCKED:
      *mmgsdi_status_ptr = MMGSDI_CODE_BLOCKED;
      break;

    case GSDI_SIM_TECHNICAL_PROBLEMS:
      *mmgsdi_status_ptr = MMGSDI_SIM_TECHNICAL_PROBLEMS;
      break;

    case GSDI_PIN_NOT_INITIALIZED:
      *mmgsdi_status_ptr = MMGSDI_PIN_NOT_INITIALIZED;
      break;

    case GSDI_UNKNOWN_INST_CLASS:
      *mmgsdi_status_ptr = MMGSDI_UNKNOWN_INST_CLASS;
      break;

    case GSDI_WARNING_NO_INFO_GIVEN:
      *mmgsdi_status_ptr = MMGSDI_WARNING_NO_INFO_GIVEN;
      break;

    case GSDI_UIM_CMD_TIMEOUT:
      *mmgsdi_status_ptr = MMGSDI_UIM_CMD_TIMEOUT;
      break;

    case GSDI_CODE_PERM_BLOCKED:
      *mmgsdi_status_ptr = MMGSDI_CODE_PERM_BLOCKED;
      break;

    case GSDI_SIM_BUSY:
      *mmgsdi_status_ptr = MMGSDI_SIM_BUSY;
      break;

    case GSDI_INVALIDATION_CONTRADICTION_STATUS:
      *mmgsdi_status_ptr = MMGSDI_INVALIDATION_CONTRADICTION_STATUS;
      break;

    default:
      MSG_ERROR("UNRECOGNIZED RETURN: 0x%x",gsdi_status,0,0);
      *mmgsdi_status_ptr = MMGSDI_ERROR;
      break;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_gsdi_status_to_mmgsdi */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_FROM_GSDI_PIN_ID

   DESCRIPTION:
     This function converts a mmgsdi PIN ID to a GSDI PIN ID

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_pin_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Null Pointer was provided.


   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_pin_enum_type mmgsdi_util_convert_from_gsdi_pin_id(
  gsdi_pin_ids_T pin_id)
{
  switch ( pin_id )
  {
    case GSDI_PIN1:
      return MMGSDI_PIN1;
    case GSDI_PIN2:
      return MMGSDI_PIN2;
    case GSDI_USIM_UNIV_PIN:
      return MMGSDI_UNIVERSAL_PIN;
    case GSDI_USIM_APP1_PIN1:
      return MMGSDI_PIN1;
    case GSDI_USIM_APP1_PIN2:
      return MMGSDI_PIN2;
    case GSDI_USIM_APP2_PIN1:
      return MMGSDI_PIN1;
    case GSDI_USIM_APP2_PIN2:
      return MMGSDI_PIN2;
    case GSDI_USIM_APP3_PIN1:
      return MMGSDI_PIN1;
    case GSDI_USIM_APP3_PIN2:
      return MMGSDI_PIN2;
    case GSDI_USIM_APP4_PIN1:
      return MMGSDI_PIN1;
    case GSDI_USIM_APP4_PIN2:
      return MMGSDI_PIN2;
    default:
      break;
  }

  return MMGSDI_MAX_PIN_ENUM;
}


/*===========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_REFRESH_MODE

DESCRIPTION
  This function is used to convert refresh mode from mmgsdi enum to
  gsdi enum

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_refresh_mode(
  mmgsdi_refresh_mode_enum_type mmgsdi_refresh,
  gsdi_refresh_modes_enum_type *gsdi_refresh_ptr)
{

  MMGSDIUTIL_RETURN_IF_NULL(gsdi_refresh_ptr);

  switch (mmgsdi_refresh)
  {
  case MMGSDI_REFRESH_RESET:
  case MMGSDI_REFRESH_RESET_AUTO:
    *gsdi_refresh_ptr = GSDI_SIM_RESET;
    break;
  case MMGSDI_REFRESH_NAA_INIT:
    *gsdi_refresh_ptr = GSDI_SIM_INIT;
    break;
  case MMGSDI_REFRESH_NAA_INIT_FCN:
    *gsdi_refresh_ptr = GSDI_SIM_INIT_AND_FILE_CHANGE;
    break;
  case MMGSDI_REFRESH_NAA_FCN:
    *gsdi_refresh_ptr = GSDI_FILE_CHANGE_NOTIFICATION;
    break;
  case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
    *gsdi_refresh_ptr = GSDI_SIM_INIT_AND_FULL_FILE_CHANGE;
    break;
  case MMGSDI_REFRESH_NAA_APP_RESET:
    *gsdi_refresh_ptr = GSDI_USIM_APP_RESET;
    break;
  case MMGSDI_REFRESH_3G_SESSION_RESET:
    *gsdi_refresh_ptr = GSDI_3G_SESSION_RESET;
    break;
  case MMGSDI_REFRESH_MODE_ENUM_MAX:
    *gsdi_refresh_ptr = GSDI_MAX_REFRESH_MODES;
    break;
  }
  return MMGSDI_SUCCESS;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_MMGSDI_TO_GSDI_SLOT

   DESCRIPTION:
     This is a utility function to convert slot of the type mmgsdi to
     gsdi_slot_id_type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_convert_mmgsdi_to_gsdi_slot(
  mmgsdi_slot_id_enum_type mmgsdi_slot,
  gsdi_slot_id_type       *gsdi_slot_ptr)
{
  if (gsdi_slot_ptr == NULL)
    return;
  switch(mmgsdi_slot)
  {
  case MMGSDI_SLOT_1:
    *gsdi_slot_ptr = GSDI_SLOT_1;
    break;
  case MMGSDI_SLOT_2:
    *gsdi_slot_ptr = GSDI_SLOT_2;
    break;
  case MMGSDI_SLOT_AUTOMATIC:
  case MMGSDI_MAX_SLOT_ID_ENUM:
    *gsdi_slot_ptr = GSDI_SLOT_NONE;
    break;
  }
} /* mmgsdi_util_convert_mmgsdi_to_gsdi_slot */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CONVERT_TO_GSDI_PIN_STATUS

DESCRIPTION
  This function converts mmgsdi pin status to gsdi pin status

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_util_convert_to_gsdi_pin_status(
  gsdi_pin_status_T              * pin_status_ptr,
  gsdi_pin_ids_T                   pin_id
)
{
  mmgsdi_app_pin_info_type   *app_pin_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(pin_status_ptr);

  app_pin_ptr = mmgsdi_util_get_sel_app_pin_ptr(pin_id);
  
  MMGSDIUTIL_RETURN_IF_NULL(app_pin_ptr);

  switch(app_pin_ptr->status)
  {
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
    case MMGSDI_PIN_ENABLED_VERIFIED:
      *pin_status_ptr        = GSDI_PIN_ENABLED;
      break;
    case MMGSDI_PIN_DISABLED:
      *pin_status_ptr        =  GSDI_PIN_DISABLED;
      break;
    case MMGSDI_PIN_UNBLOCKED:
    case MMGSDI_PIN_CHANGED:
      *pin_status_ptr        = GSDI_PIN_ENABLED;
      break;
    case MMGSDI_PIN_BLOCKED:
      *pin_status_ptr        = GSDI_PIN_BLOCKED;
      break;
    case MMGSDI_PIN_PERM_BLOCKED:
      *pin_status_ptr        = GSDI_PIN_PERM_BLOCKED;
       break;
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      *pin_status_ptr        = GSDI_PIN_NOT_INITIALISED;
       break;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_to_gsdi_pin_status */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_MMGSDI_PIN_STATUS

DESCRIPTION
  This function converts gsdi_pin status to mmgsdi pin status
  for PIN1 and PIN2 for the selected app.

PARAMETERS:
  pin_id:     GSDI PIN ID
  pin_status: Current GSDI PIN Status 

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  PIN ENabled will set the PIN Status to PIN_ENABLED_VERIFIED because
  only functions that can change the pin status should call this function,
  not something that is enquiring about the status.

  At power up it is determined whethere the PIN is ENABLED or not. If it is
  enabled, the the status is set to ENABLED_NOT_VERIFIED (in 
  mmgsd_uicc_rsp_set_pin_status), thereafter if the PIN is enabled then 
  it is also verified.
  
==========================================================================*/
gsdi_returns_T mmgsdi_util_convert_to_mmgsdi_pin_status(
  gsdi_pin_ids_T              pin_id,
  gsdi_pin_status_T           gsdi_pin_status
)
{
  mmgsdi_app_pin_info_type *pin_info_ptr = NULL;

  pin_info_ptr = mmgsdi_util_get_sel_app_pin_ptr(pin_id);

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(pin_info_ptr, GSDI_ERROR);

  MSG_HIGH("Converting gsdi_pin_status to mmgsdi_pin_status",0,0,0);

  switch(gsdi_pin_status)
  {
    case GSDI_PIN_NOT_INITIALISED:
      pin_info_ptr->status= MMGSDI_PIN_STATUS_NOT_INITIALIZED;
      break;

    case GSDI_PIN_DISABLED:
      pin_info_ptr->status= MMGSDI_PIN_DISABLED;
      break;

    case GSDI_PIN_ENABLED:
      pin_info_ptr->status= MMGSDI_PIN_ENABLED_VERIFIED;
      break;

    case GSDI_PIN_BLOCKED:
      pin_info_ptr->status= MMGSDI_PIN_BLOCKED;
      pin_info_ptr->num_retries =0;
      break;

    case GSDI_PIN_PERM_BLOCKED:
      pin_info_ptr->status= MMGSDI_PIN_PERM_BLOCKED;
      pin_info_ptr->num_unblock_retries =0;
      break;

    case GSDI_MAX_PIN_STATUS:
      MSG_ERROR("GSDI_MAX_PIN_STATUS Status Not converted to mmgsdi pin",0,0,0);
      return GSDI_ERROR;
  }

  return GSDI_SUCCESS;
}


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_SET_CNF_HEADER

DESCRIPTION
  This function will set the cnf header for gsdi confirmation

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_set_cnf_header(
  gsdi_cnf_T                *gsdi_cnf_ptr
)
{
  dword                     msg_size = 0;

  if(gsdi_cnf_ptr == NULL)
    return;

  /* Fill in header information */
  gsdi_cnf_ptr->message_header.message_set     = MS_GSDI;
  gsdi_cnf_ptr->message_header.message_id      = GSDI_CNF;

  msg_size = ((sizeof(gsdi_cnf_T) -        /* max msg size    */
             sizeof(gsdi_rsphdr_T)) -     /* msg header len  */
             GSDI_MAX_DATA_BLOCK_LEN);   /* max data len    */

  gsdi_cnf_ptr->message_header.message_len_lsb = (byte)(msg_size % 256);
  gsdi_cnf_ptr->message_header.message_len_msb = (byte)(msg_size / 256);

}


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_SET_RETRIES

DESCRIPTION
  This function will set the number of retries for all PIN
  functions

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
void mmgsdi_util_gsdi_set_retries(
  mmgsdi_pin_enum_type            pin_id,
  mmgsdi_int_app_info_type       *sel_app_ptr,
  uint8                          *retries_ptr,
  mmgsdi_num_retry_enum_type      retry_type
)
{
  mmgsdi_app_pin_info_type   *app_pin_ptr = NULL;

  if(sel_app_ptr == NULL ||
     retries_ptr == NULL)
  {
    return;
  }

  if (pin_id == MMGSDI_PIN1)
  {
    app_pin_ptr = sel_app_ptr->pin1;
  }
  else if (pin_id == MMGSDI_PIN2)
  {
    app_pin_ptr = &sel_app_ptr->pin2;
  }
  else if (pin_id == MMGSDI_UNIVERSAL_PIN)
  {
    app_pin_ptr = sel_app_ptr->universal_pin;
  }

  if(app_pin_ptr == NULL)
  {
    MSG_ERROR("PIN Ptr is NULL",0,0,0);
    return;
  }

  if(retry_type ==MMGSDI_GET_NUM_UNBLOCK_RETRIES) 
  {
    *retries_ptr = uint32touint8(app_pin_ptr->num_unblock_retries); 
  }
  else if(retry_type == MMGSDI_GET_NUM_RETRIES)
  {
    *retries_ptr = uint32touint8(app_pin_ptr->num_retries);
  }

}

/* ==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_SET_SW

DESCRIPTION
  This function sets the status words given the mmgsdi status
DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_set_sw(
  mmgsdi_return_enum_type        mmgsdi_status,
  gsdi_sw_type                  *status_words_ptr,
  byte                           num_of_retries)
{

  mmgsdi_protocol_enum_type protocol_type;
  if(status_words_ptr == NULL)
    return;

  memset(status_words_ptr, 0x00, sizeof(status_words_ptr));

  switch(mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      status_words_ptr->sw1 = GSDI_SIM_SW1_NORM_END;
      break;
    case MMGSDI_PIN_NOT_INITIALIZED:
      status_words_ptr->sw1 = GSDI_SW1_SECURITY_MANAGEMENT;
      status_words_ptr->sw2 = GSDI_SW2_NO_CHV_INITIALISED;
      break;
    case MMGSDI_INCORRECT_CODE:
      status_words_ptr->sw1 = GSDI_SW1_SECURITY_MANAGEMENT;
      status_words_ptr->sw2 = GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1;
      break;
    case MMGSDI_INCOMPAT_PIN_STATUS:
      status_words_ptr->sw1 = GSDI_SW1_SECURITY_MANAGEMENT;
      status_words_ptr->sw2 = GSDI_SW2_CNTRCTN_W_CHV_STATUS;
      break;
    case MMGSDI_CODE_BLOCKED:
    case MMGSDI_CODE_PERM_BLOCKED:
      if (mmgsdi_util_determine_protocol(&protocol_type) != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Could not get protocol type in mmgsdi_util_gsdi_set_sw",0,0,0);\
        return;
      }

      if(protocol_type == MMGSDI_ICC)
      {
        status_words_ptr->sw1 = GSDI_SW1_SECURITY_MANAGEMENT;
        status_words_ptr->sw2 = GSDI_SW2_CHV_BLOCK_UNSUCCESSFUL_UNBLOCK;
      }
      else if (protocol_type == MMGSDI_UICC)
      {
        status_words_ptr->sw1 = GSDI_SW1_WARNINGS_PART_2;
        status_words_ptr->sw2 = 0xC0 | num_of_retries;
      }
      break;
    default:
      MSG_ERROR("mmgsdi_util_gsdi_set_sw status = 0x%x", mmgsdi_status,0,0);
      break;

  }
}


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_VERIFY_PIN

DESCRIPTION
  This function will convert the mmgsdi verify pin confirmation to a gsdi cnf
  and call the original gsdi callback

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_verify_pin(
       const mmgsdi_cnf_type   *cnf_ptr)
{
  gsdi_verify_pin_req_T    *msg = NULL;
  gsdi_cnf_T                gsdi_cnf;
  mmgsdi_int_app_info_type *sel_app_ptr = NULL;

  msg = (gsdi_verify_pin_req_T  *)cnf_ptr->pin_operation_cnf.response_header.client_data;
  if(msg == NULL)
  {
    MSG_ERROR("mmgsdi_util_gsdi_verify_pin, msg is NULL",0,0,0);
    return;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();

  if(sel_app_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return;
  }

  /*Set the Header*/
  memset(&gsdi_cnf, 0, sizeof(gsdi_cnf_T));
  mmgsdi_util_gsdi_set_cnf_header(&gsdi_cnf);
  gsdi_cnf.message_header.client_ref = msg->client_ref;
  gsdi_cnf.message_header.resp_type  = GSDI_VERIFY_PIN_RSP;
  gsdi_cnf.message_header.slot       =  msg->message_header.slot;
  gsdi_cnf.verify_pin_cnf.pin_id = (gsdi_pin_ids_T)msg->pin_id;

  /*Set the GSDI Status*/
  (void)mmgsdi_util_convert_to_gsdi_status(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                                          &gsdi_cnf.verify_pin_cnf.message_header.gsdi_status);


  /*set the Retries*/
  mmgsdi_util_gsdi_set_retries(cnf_ptr->pin_operation_cnf.pin_info.pin_id,
                               sel_app_ptr,
                               &gsdi_cnf.verify_pin_cnf.num_of_retries,
                               MMGSDI_GET_NUM_RETRIES);

  mmgsdi_util_gsdi_set_sw(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                          &gsdi_cnf.verify_pin_cnf.status_words,
                          gsdi_cnf.verify_pin_cnf.num_of_retries);

  /*Call the GSDI Client Callback*/
  if(msg->message_header.gsdi_async_callback != NULL)
  {
   (msg->message_header.gsdi_async_callback)(&gsdi_cnf);
  }
  MMGSDIUTIL_TMC_MEM_FREE(msg);
}


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_ENABLE_PIN

DESCRIPTION
  This function will convert the mmgsdi enable pin confirmation to a gsdi cnf
  and call the original gsdi callback

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_enable_pin(
       const mmgsdi_cnf_type   *cnf_ptr)
{
  gsdi_enable_pin_req_T    *msg = NULL;
  gsdi_cnf_T                gsdi_cnf;
  mmgsdi_int_app_info_type *sel_app_ptr = NULL;

  msg = (gsdi_enable_pin_req_T  *)cnf_ptr->pin_operation_cnf.response_header.client_data;
  if(msg == NULL)
  {
    MSG_ERROR("mmgsdi_util_gsdi_enable_pin, msg is NULL",0,0,0);
    return;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return;
  }

  /*Set the Header*/
  memset(&gsdi_cnf, 0, sizeof(gsdi_cnf_T));
  mmgsdi_util_gsdi_set_cnf_header(&gsdi_cnf);
  gsdi_cnf.message_header.client_ref = msg->client_ref;
  gsdi_cnf.message_header.resp_type  = GSDI_ENABLE_PIN_RSP;
  gsdi_cnf.message_header.slot       =  msg->message_header.slot;
  gsdi_cnf.enable_pin_cnf.pin_id = (gsdi_pin_ids_T)msg->pin_id;

  /*Set the GSDI Status*/
  (void)mmgsdi_util_convert_to_gsdi_status(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                                           &gsdi_cnf.enable_pin_cnf.message_header.gsdi_status);
  /*set the Retries*/
  mmgsdi_util_gsdi_set_retries(cnf_ptr->pin_operation_cnf.pin_info.pin_id,
                               sel_app_ptr,
                               &gsdi_cnf.enable_pin_cnf.num_of_retries,
                               MMGSDI_GET_NUM_RETRIES);
  /*Set the SW*/
  mmgsdi_util_gsdi_set_sw(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                          &gsdi_cnf.enable_pin_cnf.status_words,
                          gsdi_cnf.enable_pin_cnf.num_of_retries);

  /*Call the GSDI Client Callback*/
  if(msg->message_header.gsdi_async_callback != NULL)
  {
   (msg->message_header.gsdi_async_callback)(&gsdi_cnf);
  }
  MMGSDIUTIL_TMC_MEM_FREE(msg);
}


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_DISABLE_PIN

DESCRIPTION
  This function will convert the mmgsdi disable pin confirmation to a gsdi cnf
  and call the original gsdi callback

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_disable_pin(
       const mmgsdi_cnf_type   *cnf_ptr)
{
  gsdi_disable_pin_req_T    *msg = NULL;
  gsdi_cnf_T                gsdi_cnf;
  mmgsdi_int_app_info_type *sel_app_ptr = NULL;

  msg = (gsdi_disable_pin_req_T  *)cnf_ptr->pin_operation_cnf.response_header.client_data;
  if(msg == NULL)
  {
    MSG_ERROR("mmgsdi_util_gsdi_disable_pin, msg is NULL",0,0,0);
    return;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return;
  }

  /*Set the Header*/
  memset(&gsdi_cnf, 0, sizeof(gsdi_cnf_T));
  mmgsdi_util_gsdi_set_cnf_header(&gsdi_cnf);
  gsdi_cnf.message_header.client_ref = msg->client_ref;
  gsdi_cnf.message_header.resp_type  = GSDI_DISABLE_PIN_RSP;
  gsdi_cnf.message_header.slot       =  msg->message_header.slot;
  gsdi_cnf.disable_pin_cnf.pin_id = (gsdi_pin_ids_T)msg->pin_id;

  /*Set the GSDI Status*/
  (void)mmgsdi_util_convert_to_gsdi_status(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                                           &gsdi_cnf.disable_pin_cnf.message_header.gsdi_status);
  /*set the Retries*/
  mmgsdi_util_gsdi_set_retries(cnf_ptr->pin_operation_cnf.pin_info.pin_id,
                               sel_app_ptr,
                               &gsdi_cnf.disable_pin_cnf.num_of_retries,
                               MMGSDI_GET_NUM_RETRIES);
  /*Set the SW*/
  mmgsdi_util_gsdi_set_sw(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                          &gsdi_cnf.disable_pin_cnf.status_words,
                          gsdi_cnf.disable_pin_cnf.num_of_retries);

  /*Call the GSDI Client Callback*/
  if(msg->message_header.gsdi_async_callback != NULL)
  {
   (msg->message_header.gsdi_async_callback)(&gsdi_cnf);
  }
  MMGSDIUTIL_TMC_MEM_FREE(msg);
}


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_CHANGE_PIN

DESCRIPTION
  This function will convert the mmgsdi change pin confirmation to a gsdi cnf
  and call the original gsdi callback

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_change_pin(
       const mmgsdi_cnf_type   *cnf_ptr)
{
  gsdi_change_pin_req_T    *msg = NULL;
  gsdi_cnf_T                gsdi_cnf;
  mmgsdi_int_app_info_type *sel_app_ptr = NULL;

  msg = (gsdi_change_pin_req_T  *)cnf_ptr->pin_operation_cnf.response_header.client_data;

  if(msg == NULL)
  {
    MSG_ERROR("mmgsdi_util_gsdi_change_pin, msg is NULL",0,0,0);
    return;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return;
  }

  /*Set the Header*/
  memset(&gsdi_cnf, 0, sizeof(gsdi_cnf_T));
  mmgsdi_util_gsdi_set_cnf_header(&gsdi_cnf);
  gsdi_cnf.message_header.client_ref = msg->client_ref;
  gsdi_cnf.message_header.resp_type  = GSDI_CHANGE_PIN_RSP;
  gsdi_cnf.message_header.slot       =  msg->message_header.slot;
  gsdi_cnf.change_pin_cnf.pin_id = (gsdi_pin_ids_T)msg->pin_id;

  /*Set the GSDI Status*/
  (void)mmgsdi_util_convert_to_gsdi_status(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                                           &gsdi_cnf.change_pin_cnf.message_header.gsdi_status);

  /*set the Retries*/
  mmgsdi_util_gsdi_set_retries(cnf_ptr->pin_operation_cnf.pin_info.pin_id,
                               sel_app_ptr,
                               &gsdi_cnf.change_pin_cnf.num_of_retries,
                               MMGSDI_GET_NUM_RETRIES);

  /*Set the SW*/
  mmgsdi_util_gsdi_set_sw(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                          &gsdi_cnf.change_pin_cnf.status_words,
                          gsdi_cnf.change_pin_cnf.num_of_retries);

  /*Call the GSDI Client Callback*/
  if(msg->message_header.gsdi_async_callback != NULL)
  {
   (msg->message_header.gsdi_async_callback)(&gsdi_cnf);
  }
  MMGSDIUTIL_TMC_MEM_FREE(msg);
}


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_UNBLOCK_PIN

DESCRIPTION
  This function will convert the mmgsdi unblock pin confirmation to a gsdi cnf
  and call the original gsdi callback

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_unblock_pin(
       const mmgsdi_cnf_type   *cnf_ptr)
{
  gsdi_unblock_pin_req_T    *msg = NULL;
  gsdi_cnf_T                gsdi_cnf;
  mmgsdi_int_app_info_type *sel_app_ptr = NULL;

  msg = (gsdi_unblock_pin_req_T  *)cnf_ptr->pin_operation_cnf.response_header.client_data;

  if(msg == NULL)
  {
    MSG_ERROR("mmgsdi_util_gsdi_unblock_pin, msg is NULL",0,0,0);
    return;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return;
  }
  /* Fill in header information */
  mmgsdi_util_gsdi_set_cnf_header(&gsdi_cnf);
  gsdi_cnf.message_header.client_ref = msg->client_ref;
  gsdi_cnf.message_header.resp_type  = GSDI_UNBLOCK_PIN_RSP;
  gsdi_cnf.message_header.slot       =  msg->message_header.slot;
  gsdi_cnf.unblock_pin_cnf.pin_id    = (gsdi_pin_ids_T)msg->pin_id;

  /*Set the GSDI Status*/
  (void)mmgsdi_util_convert_to_gsdi_status(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                                           &gsdi_cnf.unblock_pin_cnf.message_header.gsdi_status);

  /*set the Retries*/
  mmgsdi_util_gsdi_set_retries(cnf_ptr->pin_operation_cnf.pin_info.pin_id,
                               sel_app_ptr,
                               &gsdi_cnf.unblock_pin_cnf.num_of_retries,
                               MMGSDI_GET_NUM_UNBLOCK_RETRIES);

  /*Set the SW*/
  mmgsdi_util_gsdi_set_sw(cnf_ptr->pin_operation_cnf.response_header.mmgsdi_status,
                          &gsdi_cnf.unblock_pin_cnf.status_words,
                          gsdi_cnf.unblock_pin_cnf.num_of_retries);

  /*Call the GSDI Client callback*/
  if(msg->message_header.gsdi_async_callback != NULL)
  {
   (msg->message_header.gsdi_async_callback)(&gsdi_cnf);
  }
  MMGSDIUTIL_TMC_MEM_FREE(msg);
}


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CONVERT_TO_GSDI_PIN_INFO

DESCRIPTION
  This function converts mmgsdi pin status to gsdi pin status

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_gsdi_pin_info(
  gsdi_pin_info_item_T     * pin_info_ptr)
{
  mmgsdi_app_pin_info_type   *app_pin_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(pin_info_ptr);

  app_pin_ptr = mmgsdi_util_get_sel_app_pin_ptr(pin_info_ptr->pin_id);
  
  MMGSDIUTIL_RETURN_IF_NULL(app_pin_ptr);
 
  pin_info_ptr->pin_retries_allowed = uint32touint8(app_pin_ptr->num_retries);
  pin_info_ptr->puk_retries_allowed = uint32touint8(app_pin_ptr->num_unblock_retries);

  if(mmgsdi_util_convert_to_gsdi_pin_status(&pin_info_ptr->status, 
                                             pin_info_ptr->pin_id) 
     != MMGSDI_SUCCESS)
  {
     MSG_ERROR("Could not convert to gsdi pin status",0,0,0);
     return MMGSDI_ERROR;
  }
  
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_to_gsdi_pin_info */


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CONVERT_TO_MMGSDI_PIN_INFO

DESCRIPTION
  This function converts mmgsdi pin status to gsdi pin status

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_mmgsdi_pin_info(
  gsdi_pin_ids_T                 pin_id,
  uint8                          num_retries,
  uint32                         req_type)
{
  mmgsdi_app_pin_info_type   *app_pin_ptr = NULL;
  mmgsdi_int_app_info_type   *sel_app_ptr;

  sel_app_ptr = mmgsdi_util_get_sel_app();

  MMGSDIUTIL_RETURN_IF_NULL(sel_app_ptr);

  if(pin_id == GSDI_PIN1)
  {
    app_pin_ptr = sel_app_ptr->pin1;
  }
  else if(pin_id == GSDI_PIN2)
  {
    app_pin_ptr = &sel_app_ptr->pin2;
  }

  MMGSDIUTIL_RETURN_IF_NULL(app_pin_ptr);

  switch(req_type)
  {
    case GSDI_ENABLE_PIN_REQ:
    case GSDI_DISABLE_PIN_REQ:
    case GSDI_CHANGE_PIN_REQ:
    case GSDI_VERIFY_PIN_REQ:
    case GSDI_GET_PIN_STATUS_REQ:
      app_pin_ptr->num_retries         = num_retries;
      if(num_retries == 0)
      {
        if(mmgsdi_util_convert_to_mmgsdi_pin_status(pin_id,GSDI_PIN_BLOCKED)
           != GSDI_SUCCESS)
        {
          MSG_ERROR("Could not convert pin status",0,0,0);
          return MMGSDI_ERROR;
        }
      }
      break;
    case GSDI_UNBLOCK_PIN_REQ:
      app_pin_ptr->num_unblock_retries = num_retries;
      if(num_retries == 0)
      {
        if(mmgsdi_util_convert_to_mmgsdi_pin_status(pin_id,GSDI_PIN_PERM_BLOCKED)
           != GSDI_SUCCESS)
        {
          MSG_ERROR("Could not convert pin status",0,0,0);
          return MMGSDI_ERROR;
        }
      }
      break;
    default:
      MSG_ERROR("Request not handled 0x%x",req_type,0,0);
      return MMGSDI_ERROR;
  }
  
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_to_mmgsdi_pin_info */


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_GET_PIN_STATUS

DESCRIPTION
  This function will convert the gsdi get pin status to a gsdi cnf
  and call the original gsdi callback

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_util_gsdi_get_pin_status(
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  gsdi_get_pin_status_req_T    *msg_ptr = NULL;
  gsdi_cnf_T                    gsdi_cnf;
  mmgsdi_int_app_info_type     *sel_app_ptr = NULL;
  mmgsdi_pin_enum_type          mmgsdi_pin_id = MMGSDI_PIN1;

  msg_ptr = (gsdi_get_pin_status_req_T  *)cnf_ptr->get_all_pin_status_cnf.response_header.client_data;

  if(msg_ptr == NULL)
  {
    MSG_ERROR("GET_PIN_STATUS gsdi ptr is NULL, cannot send cnf to client",0,0,0);
    return;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
  {
    MSG_ERROR("No app is selected",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( msg_ptr);
    return;
  }
  /* Fill in header information */
  mmgsdi_util_gsdi_set_cnf_header(&gsdi_cnf);
  gsdi_cnf.message_header.client_ref = msg_ptr->client_ref;
  gsdi_cnf.message_header.resp_type  = GSDI_GET_PIN_STATUS_RSP;
  gsdi_cnf.message_header.slot       =  msg_ptr->message_header.slot;
  gsdi_cnf.pin_status_cnf.pin_id     = (gsdi_pin_ids_T)msg_ptr->pin_id;
 
  /*Set the GSDI Status*/
  (void)mmgsdi_util_convert_to_gsdi_status(cnf_ptr->get_all_pin_status_cnf.response_header.mmgsdi_status,
                                           &gsdi_cnf.pin_status_cnf.message_header.gsdi_status);

  if(mmgsdi_util_convert_to_gsdi_pin_status(&gsdi_cnf.pin_status_cnf.status, 
                                             gsdi_cnf.pin_status_cnf.pin_id)
     != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not convert to GSDI PIN status",0,0,0);
  }

  mmgsdi_pin_id = mmgsdi_util_convert_from_gsdi_pin_id(gsdi_cnf.pin_status_cnf.pin_id);

  /*set the Retries*/
  mmgsdi_util_gsdi_set_retries(mmgsdi_pin_id,
                               sel_app_ptr,
                               &gsdi_cnf.pin_status_cnf.num_of_retries,
                               MMGSDI_GET_NUM_RETRIES);

  if(gsdi_cnf.pin_status_cnf.num_of_retries == 0)
  {
    mmgsdi_util_gsdi_set_retries(mmgsdi_pin_id,
                                 mmgsdi_util_get_sel_app(),
                                 &gsdi_cnf.pin_status_cnf.num_of_retries,
                                 MMGSDI_GET_NUM_UNBLOCK_RETRIES);
  }


  /*Call the GSDI Client callback*/
  if(msg_ptr->message_header.gsdi_async_callback != NULL)
  {
     (msg_ptr->message_header.gsdi_async_callback)(&gsdi_cnf);
  }
  MMGSDIUTIL_TMC_MEM_FREE( msg_ptr);
}/*mmgsdi_util_gsdi_get_pin_status*/


/* ==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands  generated by MMGSDI

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
----------------------------------------------------------------------------*/
void mmgsdi_util_gsdi_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{

  MSG_HIGH("MMGSDI_INTERNAL_CB: Rev'd Cnf=0x%x, Status=0x%x", cnf, status, 0);
  if (cnf == MMGSDI_MAX_CNF_ENUM)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Cnf Enum", 0, 0, 0);
    return;
  }
  if (cnf_ptr == NULL)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Null Confirmation Ptr", 0, 0, 0);
    return;
  }

  switch(cnf)
  {
    case MMGSDI_PIN_OPERATION_CNF:
      switch(cnf_ptr->pin_operation_cnf.pin_op)
      {
        case MMGSDI_PIN_OP_VERIFY:
          mmgsdi_util_gsdi_verify_pin(cnf_ptr);
          break;
        case MMGSDI_PIN_OP_ENABLE:
          mmgsdi_util_gsdi_enable_pin(cnf_ptr);
          break;
        case MMGSDI_PIN_OP_DISABLE:
          mmgsdi_util_gsdi_disable_pin(cnf_ptr);
          break;
        case MMGSDI_PIN_OP_CHANGE:
          mmgsdi_util_gsdi_change_pin(cnf_ptr);
          break;
        case MMGSDI_PIN_OP_UNBLOCK:
          mmgsdi_util_gsdi_unblock_pin(cnf_ptr);
          break;
        default:
          break;
      }
      break;
    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
      mmgsdi_util_gsdi_get_pin_status(cnf_ptr);
      break;
    case MMGSDI_REFRESH_CNF:
     // cnf_ptr->refresh_cnf.response_header.
    default:
      break;
  }
}/*mmgsdi_util_gsdi_cb*/


/* ==========================================================================
FUNCTION MMGSDI_UTIL_MAP_GSDI_REFRESH_MODE_TO_MMGSDI

DESCRIPTION
  This function maps GSDI refresh mode to MMGSDI refresh mode

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_map_gsdi_refresh_mode_to_mmgsdi(
  gsdi_refresh_modes_enum_type   gsdi_refresh_mode,
  mmgsdi_refresh_mode_enum_type *mmgsdi_refresh_mode_ptr
)
{
  MSG_HIGH("mmgsdi_util_map_gsdi_refresh_mode_to_mmgsdi gsdi_refresh 0x%x",
            gsdi_refresh_mode, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_mode_ptr);
  switch (gsdi_refresh_mode)
  {
    case GSDI_SIM_INIT_AND_FULL_FILE_CHANGE:
      *mmgsdi_refresh_mode_ptr = MMGSDI_REFRESH_NAA_INIT_FULL_FCN;
      return MMGSDI_SUCCESS;

    case GSDI_FILE_CHANGE_NOTIFICATION:
      *mmgsdi_refresh_mode_ptr = MMGSDI_REFRESH_NAA_FCN;
      return MMGSDI_SUCCESS;

    case GSDI_SIM_INIT_AND_FILE_CHANGE:
      *mmgsdi_refresh_mode_ptr = MMGSDI_REFRESH_NAA_INIT_FCN;
      return MMGSDI_SUCCESS;

    case GSDI_SIM_INIT:
      *mmgsdi_refresh_mode_ptr = MMGSDI_REFRESH_NAA_INIT;
      return MMGSDI_SUCCESS;

    case GSDI_SIM_RESET:
      *mmgsdi_refresh_mode_ptr = MMGSDI_REFRESH_RESET;
      return MMGSDI_SUCCESS;

    case GSDI_USIM_APP_RESET:
      *mmgsdi_refresh_mode_ptr = MMGSDI_REFRESH_NAA_APP_RESET;
      return MMGSDI_SUCCESS;

    case GSDI_3G_SESSION_RESET:
      *mmgsdi_refresh_mode_ptr = MMGSDI_REFRESH_3G_SESSION_RESET;
      return MMGSDI_SUCCESS;

    default:
      MSG_ERROR("Invalid gsdi refresh mode 0x%x", gsdi_refresh_mode, 0, 0);
      return MMGSDI_ERROR;
  }
}


#ifdef FEATURE_MMGSDI_CARD_ERROR_INFO
/* ============================================================================
 FUNCTION:      MMGSDI_UTIL_CONVERT_TO_ERROR_INFO_EVENT

 DESCRIPTION:
   A utility function used to convert a generic GSDI_CARD_ERROR into a more
   specific Error Event.
 DEPENDENCIES:
   None

  LIMITATIONS:
   None

  RETURN VALUE:
    None

  SIDE EFFECTS:

============================================================================ */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_convert_to_error_info_event (
  mmgsdi_slot_id_enum_type slot,
  gsdi_sim_events_T        sim_event,
  uim_status_type          error_condition,
  gsdi_sim_events_T      * sim_err_info_event_ptr
)
{
  if ( sim_event != GSDI_CARD_ERROR)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Pointer not valid for funct: 0x%x",sim_event,0,0);
    return;
  }

  if ( sim_err_info_event_ptr == NULL )
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("NULL POINTER: sim_err_info_event_ptr",0,0,0);
    return;
  }

  if ( slot == MMGSDI_SLOT_1 )
  {
    switch ( error_condition )
    {
      case UIM_NO_SUCH_STATUS_S:
        MMGSDIUTIL_DEBUG_MSG_ERR("UIM_ERR_S ==> GSDI_CARD_ERROR",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERROR;
        break;

      case UIM_ERR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR("UIM_ERR_S ==> GSDI_CARD_ERROR",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERROR;
        break;

      case UIM_POLL_ERR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_POLL_ERR_S ==> GSDI_CARD_ERR_POLL_ERROR",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_POLL_ERROR;
        break;

      case UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S ==> GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE;
        break;

      case UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S ==> GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET;
        break;

      case UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S ==> GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES;
        break;

      case UIM_PPS_TIMED_OUT_MAX_TIMES_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_PPS_TIMED_OUT_MAX_TIMES_S ==> GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES;
        break;

      case UIM_VOLTAGE_MISMATCH_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_VOLTAGE_MISMATCH_S ==> GSDI_CARD_ERR_VOLTAGE_MISMATCH",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_VOLTAGE_MISMATCH;
        break;

      case UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S ==> GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS;
        break;

      case UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S ==> GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS;
        break;

      case UIM_MAXED_PARITY_ERROR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAXED_PARITY_ERROR_S ==> GSDI_CARD_ERR_MAXED_PARITY_ERROR",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAXED_PARITY_ERROR;
        break;

      case UIM_MAXED_RX_BREAK_ERROR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAXED_RX_BREAK_ERROR_S ==> GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR;
        break;

      case UIM_MAXED_OVERRUN_ERROR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAXED_OVERRUN_ERROR_S ==> GSDI_CARD_ERR_MAXED_OVERRUN_ERROR",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAXED_OVERRUN_ERROR;
        break;

      case UIM_TRANSACTION_TIMER_EXPIRED_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_TRANSACTION_TIMER_EXPIRED_S ==> GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED;
        break;

      case UIM_POWER_DOWN_CMD_NOTIFICATION_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_POWER_DOWN_CMD_NOTIFICATION_S ==> GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION;
        break;

      case UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S ==> GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS;
        break;

      case UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S ==> GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE;
        break;

      case UIM_MAX_PARITY_IN_PASSIVE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAX_PARITY_IN_PASSIVE_S ==> GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE;
        break;

      case UIM_MAX_RXBRK_IN_PASSIVE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAX_RXBRK_IN_PASSIVE_S ==> GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE;
        break;

      case UIM_MAX_OVERRUN_IN_PASSIVE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAX_OVERRUN_IN_PASSIVE_S ==> GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE;
        break;

      default:
        MMGSDIUTIL_DEBUG_MSG_ERR("Unknown Err Code 0x%x",error_condition,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERROR;
        break;
    }
  }
  else if ( slot == MMGSDI_SLOT_2 )
  {
    switch ( error_condition )
    {
      case UIM_NO_SUCH_STATUS_S:
        MMGSDIUTIL_DEBUG_MSG_ERR("UIM_ERR_S ==> GSDI_CARD_ERROR_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERROR_2;
        break;

      case UIM_ERR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR("UIM_ERR_S ==> GSDI_CARD_ERROR_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERROR_2;
        break;

      case UIM_POLL_ERR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR("UIM_POLL_ERR_S ==> GSDI_CARD_ERR_POLL_ERROR_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_POLL_ERROR_2;
        break;

      case UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S ==> GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2;
        break;

      case UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S ==> GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2;
        break;

      case UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S ==> GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2;
        break;

      case UIM_PPS_TIMED_OUT_MAX_TIMES_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_PPS_TIMED_OUT_MAX_TIMES_S ==> GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2;
        break;

      case UIM_VOLTAGE_MISMATCH_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_VOLTAGE_MISMATCH_S ==> GSDI_CARD_ERR_VOLTAGE_MISMATCH_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_VOLTAGE_MISMATCH_2;
        break;

      case UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S ==> GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2;
        break;

      case UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S ==> GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2;
        break;

      case UIM_MAXED_PARITY_ERROR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAXED_PARITY_ERROR_S ==> GSDI_CARD_ERR_MAXED_PARITY_ERROR_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAXED_PARITY_ERROR_2;
        break;

      case UIM_MAXED_RX_BREAK_ERROR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAXED_RX_BREAK_ERROR_S ==> GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2;
        break;

      case UIM_MAXED_OVERRUN_ERROR_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAXED_OVERRUN_ERROR_S ==> GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2;
        break;

      case UIM_TRANSACTION_TIMER_EXPIRED_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_TRANSACTION_TIMER_EXPIRED_S ==> GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2;
        break;

      case UIM_POWER_DOWN_CMD_NOTIFICATION_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_POWER_DOWN_CMD_NOTIFICATION_S ==> GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2;
        break;

      case UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S ==> GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2;
        break;

      case UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S ==> GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2;
        break;

      case UIM_MAX_PARITY_IN_PASSIVE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAX_PARITY_IN_PASSIVE_S ==> GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2;
        break;

      case UIM_MAX_RXBRK_IN_PASSIVE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAX_RXBRK_IN_PASSIVE_S ==> GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2;
        break;

      case UIM_MAX_OVERRUN_IN_PASSIVE_S:
        MMGSDIUTIL_DEBUG_MSG_ERR(
          "UIM_MAX_OVERRUN_IN_PASSIVE_S ==> GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2",0,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2;
        break;

      default:
        MMGSDIUTIL_DEBUG_MSG_ERR("Unknown Err Code 0x%x",error_condition,0,0);
        *sim_err_info_event_ptr = GSDI_CARD_ERROR_2;
        break;
    }
  }
  else
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Unknown Slot ID: 0x%x",slot,0,0);
    *sim_err_info_event_ptr = GSDI_CARD_ERROR;
  }

}/* mmgsdi_util_convert_to_error_info_event */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_DERIVE_SLOT_FROM_GSDI_EVENT

   DESCRIPTION:
     This function returns an MMGSDI SLOT ID for the event
     provided

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_slot_id_enum_type

   SIDE EFFECTS:

============================================================================*/
mmgsdi_slot_id_enum_type mmgsdi_util_derive_slot_from_gsdi_event(
  gsdi_sim_events_T sim_event
)
{
  switch ( sim_event )
  {
    case GSDI_SIM_INSERTED:
    case GSDI_SIM_REMOVED:
    case GSDI_DRIVER_ERROR:
    case GSDI_CARD_ERROR:
    case GSDI_MEMORY_WARNING:
    case GSDI_NO_SIM_EVENT:
    case GSDI_NO_SIM:
    case GSDI_SIM_INIT_COMPLETED:
    case GSDI_SIM_INIT_COMPLETED_NO_PROV:
    case GSDI_PIN1_VERIFIED:
    case GSDI_PIN1_BLOCKED:
    case GSDI_PIN1_UNBLOCKED:
    case GSDI_PIN1_ENABLED:
    case GSDI_PIN1_DISABLED:
    case GSDI_PIN1_CHANGED:
    case GSDI_PIN1_PERM_BLOCKED:
    case GSDI_PIN2_VERIFIED:
    case GSDI_PIN2_BLOCKED:
    case GSDI_PIN2_UNBLOCKED:
    case GSDI_PIN2_ENABLED:
    case GSDI_PIN2_DISABLED:
    case GSDI_PIN2_CHANGED:
    case GSDI_PIN2_PERM_BLOCKED:
    case GSDI_NO_EVENT:
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL:
    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL:
    case GSDI_REFRESH_SIM_RESET:
    case GSDI_REFRESH_SIM_INIT:
    case GSDI_FDN_ENABLE:
    case GSDI_FDN_DISABLE:
    case GSDI_REFRESH_FAILED:
    case GSDI_START_SWITCH_SLOT:
    case GSDI_FINISH_SWITCH_SLOT:
    case GSDI_PERSO_INIT_COMPLETED:
    case GSDI_PERSO_EVENT_GEN_PROP1:
    case GSDI_SIM_ILLEGAL:
    case GSDI_REFRESH_FCN:
    case GSDI_SIM_UHZI_V2_COMPLETE:
    case GSDI_SIM_UHZI_V1_COMPLETE:
    case GSDI_PERSO_EVENT_GEN_PROP2:
    case GSDI_INTERNAL_SIM_RESET:
    case GSDI_REFRESH_SIM_INIT_FCN:
    case GSDI_CARD_ERR_POLL_ERROR:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE:
    case GSDI_REFRESH_APP_RESET:
    case GSDI_REFRESH_3G_SESSION_RESET:
    case GSDI_APP_SELECTED:
      return MMGSDI_SLOT_1;

    case GSDI_SIM_INSERTED_2:
    case GSDI_SIM_REMOVED_2:
    case GSDI_CARD_ERROR_2:
    case GSDI_MEMORY_WARNING_2:
    case GSDI_NO_SIM_EVENT_2:
    case GSDI_SIM_INIT_COMPLETED_2:
    case GSDI_SIM_INIT_COMPLETED_NO_PROV_2:
    case GSDI_PIN1_VERIFIED_2:
    case GSDI_PIN1_BLOCKED_2:
    case GSDI_PIN1_UNBLOCKED_2:
    case GSDI_PIN1_ENABLED_2:
    case GSDI_PIN1_DISABLED_2:
    case GSDI_PIN1_CHANGED_2:
    case GSDI_PIN1_PERM_BLOCKED_2:
    case GSDI_PIN2_VERIFIED_2:
    case GSDI_PIN2_BLOCKED_2:
    case GSDI_PIN2_UNBLOCKED_2:
    case GSDI_PIN2_ENABLED_2:
    case GSDI_PIN2_DISABLED_2:
    case GSDI_PIN2_CHANGED_2:
    case GSDI_PIN2_PERM_BLOCKED_2:
    case GSDI_REFRESH_SIM_RESET_2:
    case GSDI_REFRESH_SIM_INIT_2:
    case GSDI_FDN_ENABLE_2:
    case GSDI_FDN_DISABLE_2:
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2:
    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2:
    case GSDI_REFRESH_SIM_INIT_FCN_2:
    case GSDI_REFRESH_FCN_2:
    case GSDI_SIM_ILLEGAL_2:
    case GSDI_CARD_ERR_POLL_ERROR_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH_2:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR_2:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2:
    case GSDI_REFRESH_APP_RESET_2:
    case GSDI_REFRESH_3G_SESSION_RESET_2:
    case GSDI_APP_SELECTED_2:
      return MMGSDI_SLOT_2;

    case GSDI_MAX_SIM_EVENTS:
      return MMGSDI_MAX_SLOT_ID_ENUM;
  }

  return MMGSDI_MAX_SLOT_ID_ENUM;
}
#endif /* FEATURE_MMGSDI_CARD_ERROR_INFO */
