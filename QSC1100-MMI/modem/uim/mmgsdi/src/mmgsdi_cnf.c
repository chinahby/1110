/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   C N F   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions to build confirmation message for
  all the commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_cnf.c#5 $$ $DateTime: 2009/05/15 02:20:16 $ $Author: c_pkamuj $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/29/09   js      Fixed compiler warning
01/21/09   kk      Added channel id to session_open cnf data
08/20/08   jk      Fixes for CAVE API & Generate Key API
08/08/08   tml     Fixed header rev update issue
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/04/08   tml     Added sw support to the cnf response header
03/24/08   sun     Added support for OMA Bcast
03/14/08   nk      Updated mmgsdi_cnf_build_rehabilitate_data
01/24/08   nk      Updated JCDMA card color cnf with change me esn flag
11/15/07   sun     Added support for NAA refresh
11/15/07   tml     Use regular rex function for critical sections for 
                   mmgsdi_task_rsp_q to prevent gsdi task stuck when synchronous
                   commands were issued
09/27/07   tml     Lint fixes
09/23/07   sk      Put a lock around mmgsdi_task_rsp
09/10/07   tml     Added MFLO Support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
05/17/07   wli     Added support for JCDMA RUIM
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/28/06   tml     Populated offset and data len for write confirmation
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
08/07/06   sun     Send PIN ID regardless of status
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   sun     Check for the right mmgsdi_state while setting Card Reader
                   status
06/08/06   tml     fixed featurized
05/23/06   tml     Lint fixes and BDN, rehabilitate and invalidate supports
04/15/06   tml     moved onchip member to global data structure
04/11/06   jar     Added ONCHIP SIM support
02/22/06   tml     Returned client id as part of the open and close cnf
01/04/06   tml     Updated enum name for SAP and Card powerup and down
02/24/05   tml     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gsdi.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_cnf.h"
#include "uimgen.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_sap_rsp.h"
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
  #include "mmgsdi_ruim_jcdma.h"
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
#ifdef FEATURE_MMGSDI_CDMA
  #include "bit.h"
#endif /* FEATURE_MMGSDI_CDMA */


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_POPULATE_CARD_READER_STATUS

   DESCRIPTION:
     This function populates the Card Reader Status data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_populate_card_reader_status(
  mmgsdi_cmd_enum_type cmd,
  uint8               *card_reader_status_data_ptr);
  
/*=============================================================================
  FUNCTION: MMGSDI_CNF_GET_ATR_LEN

  DESCRIPTION:
    This is the function called from the main procedure handling function.
    This funciton will validate the important parameters.  Once the
    parameters pass the checking, it will call the GSDI - UIM Function
    to send it to the UIM.

  PARAMETERS:
    slot: The slot to the attribute len of.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    length of the ATR

  SIDE EFFECTS:
    None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static int32 mmgsdi_cnf_get_atr_len(
    mmgsdi_slot_id_enum_type slot_id)
{
  int32 len = 0;

  switch (slot_id) {
    case MMGSDI_SLOT_1:
      len = gsdi_data.card_atr.atr_len;
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case MMGSDI_SLOT_2:
      len = gsdi_data.card_atr_2.atr_len;
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    default:
      break;
  }
  return len;
}/* mmgsdi_cnf_get_atr_len */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*=============================================================================
  FUNCTION: MMGSDI_CNF_POPULATE_ATR

  DESCRIPTION:
    This is the function called from the main procedure handling function.
    This funciton will validate the important parameters.  Once the
    parameters pass the checking, it will call the GSDI - UIM Function
    to send it to the UIM.

  PARAMETERS:
    slot:                The slot to the attribute len of.
    returned_data_len_p: The length of the returned data.
    data_p:              The returned data.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_cnf_populate_atr(
    mmgsdi_slot_id_enum_type slot_id,
    int32*                   returned_data_len_p,
    uint8*                   data_p)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  /*lint -esym(613,returned_data_len_p) */
  MMGSDIUTIL_RETURN_IF_NULL(data_p);

  switch (slot_id) {
    case MMGSDI_SLOT_1:
      *returned_data_len_p = gsdi_data.card_atr.atr_len;
      memcpy(data_p,
             gsdi_data.card_atr.atr_buffer,
             int32touint32(gsdi_data.card_atr.atr_len));
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case MMGSDI_SLOT_2:
      *returned_data_len_p = gsdi_data.card_atr_2.atr_len;
      memcpy(data_p, gsdi_data.card_atr_2.atr_buffer, gsdi_data.card_atr_2.atr_len);
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x", slot_id, 0, 0);
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }
  return mmgsdi_status;
  /*lint +esym(613,returned_data_len_p) */
}/* mmgsdi_cnf_populate_atr */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_MAP_REQ_TO_CNF_ENUM_TYPE

   DESCRIPTION:
     This function maps the request enum type to confirmation enum

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
void mmgsdi_cnf_map_req_to_cnf_enum_type(
  mmgsdi_cmd_enum_type  req_type,
  mmgsdi_cnf_enum_type *cnf_type_ptr
)
{
  if (cnf_type_ptr == NULL)
    return;

  /* initialized to MAX_CNF_ENUM */
  *cnf_type_ptr = MMGSDI_MAX_CNF_ENUM;
  switch (req_type)
  {
  case MMGSDI_CLIENT_ID_REG_REQ:
    *cnf_type_ptr = MMGSDI_CLIENT_ID_REG_CNF;
    break;
  case MMGSDI_CLIENT_EVT_REG_REQ:
    *cnf_type_ptr = MMGSDI_CLIENT_EVT_REG_CNF;
    break;
  case MMGSDI_CLIENT_ID_DEREG_REQ:
    *cnf_type_ptr = MMGSDI_CLIENT_ID_DEREG_CNF;
    break;
  case MMGSDI_CLIENT_EVT_DEREG_REQ:
    *cnf_type_ptr = MMGSDI_CLIENT_EVT_DEREG_CNF;
    break;
  case MMGSDI_CLIENT_INIT_COMP_REQ:
    *cnf_type_ptr = MMGSDI_CLIENT_INIT_COMP_CNF;
    break;
  case MMGSDI_READ_REQ:
    *cnf_type_ptr = MMGSDI_READ_CNF;
    break;
  case MMGSDI_WRITE_REQ:
    *cnf_type_ptr = MMGSDI_WRITE_CNF;
    break;
  case MMGSDI_GET_FILE_ATTR_REQ:
    *cnf_type_ptr = MMGSDI_GET_FILE_ATTR_CNF;
    break;
  case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
    *cnf_type_ptr = MMGSDI_JCDMA_GET_CARD_INFO_CNF;
    break;
  case MMGSDI_SESSION_OPEN_REQ:
    *cnf_type_ptr = MMGSDI_SESSION_OPEN_CNF;
    break;
  case MMGSDI_SESSION_CLOSE_REQ:
    *cnf_type_ptr = MMGSDI_SESSION_CLOSE_CNF;
    break;
  case MMGSDI_ISIM_AUTH_REQ:
    *cnf_type_ptr = MMGSDI_ISIM_AUTH_CNF;
    break;
  case MMGSDI_SEND_APDU_REQ:
    *cnf_type_ptr = MMGSDI_SEND_APDU_CNF;
    break;
  case MMGSDI_SAP_CONNECT_REQ:
    *cnf_type_ptr = MMGSDI_SAP_CONNECT_CNF;
    break;
  case MMGSDI_SAP_DISCONNECT_REQ:
    *cnf_type_ptr = MMGSDI_SAP_DISCONNECT_CNF;
    break;
  case MMGSDI_SAP_GET_ATR_REQ:
    *cnf_type_ptr = MMGSDI_SAP_GET_ATR_CNF;
    break;
  case MMGSDI_SAP_SEND_APDU_REQ:
    *cnf_type_ptr = MMGSDI_SAP_SEND_APDU_CNF;
    break;
  case MMGSDI_SAP_POWER_ON_REQ:
    *cnf_type_ptr = MMGSDI_SAP_POWER_ON_CNF;
    break;
  case MMGSDI_SAP_POWER_OFF_REQ:
    *cnf_type_ptr = MMGSDI_SAP_POWER_OFF_CNF;
    break;
  case MMGSDI_SAP_INIT_REQ:
    *cnf_type_ptr = MMGSDI_SAP_INIT_CNF;
    break;
  case MMGSDI_SAP_DEREG_REQ:
    *cnf_type_ptr = MMGSDI_SAP_DEREG_CNF;
    break;
  case MMGSDI_SAP_PIN_VERIFY_REQ:
    *cnf_type_ptr = MMGSDI_SAP_VERIFY_PIN_CNF;
    break;
  case MMGSDI_CARD_PUP_REQ:
    *cnf_type_ptr = MMGSDI_CARD_PUP_CNF;
    break;
  case MMGSDI_CARD_PDOWN_REQ:
    *cnf_type_ptr = MMGSDI_CARD_PDOWN_CNF;
    break;
  case MMGSDI_CARD_STATUS_REQ:
    *cnf_type_ptr = MMGSDI_CARD_STATUS_CNF;
    break;
  case MMGSDI_CARD_RESET_REQ:
    *cnf_type_ptr = MMGSDI_CARD_RESET_CNF;
    break;
  case MMGSDI_REFRESH_REQ:
    *cnf_type_ptr = MMGSDI_REFRESH_CNF;
    break;
  case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    *cnf_type_ptr = MMGSDI_SAP_CARD_READER_STATUS_CNF;
    break;
  case MMGSDI_SAP_RESET_REQ:
    *cnf_type_ptr = MMGSDI_SAP_RESET_CNF;
    break;
  case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
    break;
  case MMGSDI_DISABLE_BDN_REQ:
    *cnf_type_ptr = MMGSDI_DISABLE_BDN_CNF;
    break;
  case MMGSDI_ENABLE_BDN_REQ:
    *cnf_type_ptr = MMGSDI_ENABLE_BDN_CNF;
    break;
  case MMGSDI_ACL_OPERATION_REQ:
    *cnf_type_ptr = MMGSDI_ACL_OPERATION_CNF;
    break;
  case MMGSDI_REHABILITATE_REQ:
    *cnf_type_ptr = MMGSDI_REHABILITATE_CNF;
    break;
  case MMGSDI_INVALIDATE_REQ:
    *cnf_type_ptr = MMGSDI_INVALIDATE_CNF;
    break;
  case MMGSDI_PIN_OPERATION_REQ:
    *cnf_type_ptr = MMGSDI_PIN_OPERATION_CNF;
    break;
  case MMGSDI_SELECT_AID_REQ:
    *cnf_type_ptr = MMGSDI_SELECT_AID_CNF;
    break;
  case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    *cnf_type_ptr = MMGSDI_GET_ALL_PIN_STATUS_CNF;
    break;
  case MMGSDI_SESSION_DEACTIVATE_REQ:
    *cnf_type_ptr = MMGSDI_SESSION_DEACTIVATE_CNF;
    break;
  case MMGSDI_SESSION_SELECT_AID_REQ:
    *cnf_type_ptr = MMGSDI_SESSION_SELECT_AID_CNF;
    break;
  case MMGSDI_SRV_AVAILABLE_REQ:
    *cnf_type_ptr = MMGSDI_SRV_AVAILABLE_CNF;
    break;
  case MMGSDI_USIM_AUTH_REQ:
    *cnf_type_ptr = MMGSDI_USIM_AUTH_CNF;
    break;
  case MMGSDI_COMPUTE_IP_AUTH_REQ:
    *cnf_type_ptr = MMGSDI_COMPUTE_IP_AUTH_CNF;
    break;
  case MMGSDI_RUN_CAVE_REQ:
    *cnf_type_ptr = MMGSDI_RUN_CAVE_CNF;
    break;
  case MMGSDI_GENERATE_KEY_VPM_REQ:
    *cnf_type_ptr = MMGSDI_GENERATE_KEY_VPM_CNF;
    break;
  case MMGSDI_GET_ATR_REQ:
    *cnf_type_ptr = MMGSDI_GET_ATR_CNF;
    break;
  case MMGSDI_CARD_READER_STATUS_REQ:
    *cnf_type_ptr = MMGSDI_CARD_READER_STATUS_CNF;
    break;
  case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
    *cnf_type_ptr = MMGSDI_MFLO_INIT_ACT_PARAMS_CNF;
    break;
  case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
    *cnf_type_ptr = MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF;
    break;
  case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    *cnf_type_ptr = MMGSDI_MFLO_GET_PUBLIC_KEY_CNF;
    break;
  case MMGSDI_MFLO_SIGN_UCAST_REQ:
    *cnf_type_ptr = MMGSDI_MFLO_SIGN_UCAST_CNF;
    break;
  case MMGSDI_MFLO_VERIFY_UCAST_REQ:
    *cnf_type_ptr = MMGSDI_MFLO_VERIFY_UCAST_CNF;
    break; 
  case MMGSDI_SEARCH_REQ:
    *cnf_type_ptr = MMGSDI_SEARCH_CNF;
    break;
  case MMGSDI_CREATE_PKCS15_TABLE_REQ:
    *cnf_type_ptr = MMGSDI_CREATE_PKCS15_TABLE_CNF;
    break;
  case MMGSDI_BCAST_REQ:
    *cnf_type_ptr = MMGSDI_BCAST_CNF;
    break;
  case MMGSDI_UIM_REPORT_RSP:
  case MMGSDI_CLIENT_ID_REG_RSP:
  case MMGSDI_SAP_REPORT_RSP:
  case MMGSDI_MAX_CMD_ENUM:
  case MMGSDI_CNF:
    *cnf_type_ptr = MMGSDI_MAX_CNF_ENUM;
    break;
  }
}/* mmgsdi_cnf_map_req_to_cnf_enum_type */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_POPULATE_CNF_RSP_HEADER

   DESCRIPTION:
     This function populate the confirmation header information

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
void mmgsdi_cnf_populate_cnf_rsp_header(
  mmgsdi_task_cnf_type              * task_cnf_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
)
{
  int32                              len = 0;
  mmgsdi_file_security_access_type  *file_security_access_ptr = NULL;
  if ((task_cnf_ptr == NULL) || (req_hdr_ptr == NULL))
    return;

  task_cnf_ptr->client_req_cb                                  = req_hdr_ptr->response_cb;
  task_cnf_ptr->mmgsdi_cnf.response_header.client_data         = req_hdr_ptr->client_data;
  task_cnf_ptr->mmgsdi_cnf.response_header.client_id           = req_hdr_ptr->client_id;
  task_cnf_ptr->mmgsdi_cnf.response_header.slot_id             = req_hdr_ptr->slot_id;
  task_cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status       = mmgsdi_status;
  task_cnf_ptr->mmgsdi_cnf.response_header.status_word.present = sw_present;
  task_cnf_ptr->mmgsdi_cnf.response_header.status_word.sw1     = status_word.sw1;
  task_cnf_ptr->mmgsdi_cnf.response_header.status_word.sw2     = status_word.sw2;

  /* response_type is being populated in the build_and_queue function
     before the populate header is being called */
  switch(task_cnf_ptr->mmgsdi_cnf.response_header.response_type)
  {
  case MMGSDI_CLIENT_ID_REG_CNF:
    task_cnf_ptr->mmgsdi_cnf.client_id_reg_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_client_id_reg_cnf_type));
    break;

  case MMGSDI_CLIENT_EVT_REG_CNF:
    task_cnf_ptr->mmgsdi_cnf.client_evt_reg_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_client_evt_reg_cnf_type));
    break;

  case MMGSDI_CLIENT_ID_DEREG_CNF:
    task_cnf_ptr->mmgsdi_cnf.client_id_dereg_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_client_id_dereg_cnf_type));
    break;

  case MMGSDI_CLIENT_EVT_DEREG_CNF:
    task_cnf_ptr->mmgsdi_cnf.client_evt_dereg_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_client_evt_dereg_cnf_type));
    break;

  case MMGSDI_CLIENT_INIT_COMP_CNF:
    task_cnf_ptr->mmgsdi_cnf.client_init_comp_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_client_init_comp_cnf_type));
    break;

  case MMGSDI_READ_CNF:
    task_cnf_ptr->mmgsdi_cnf.read_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_read_cnf_type)) +
      task_cnf_ptr->mmgsdi_cnf.read_cnf.read_data.data_len;
    break;

  case MMGSDI_WRITE_CNF:
    task_cnf_ptr->mmgsdi_cnf.write_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_write_cnf_type));
    break;

  case MMGSDI_GET_FILE_ATTR_CNF:
    switch (task_cnf_ptr->mmgsdi_cnf.get_file_attr_cnf.file_attrib.file_type)
    {
    case MMGSDI_LINEAR_FIXED_FILE:
      file_security_access_ptr =
       &task_cnf_ptr->mmgsdi_cnf.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security;
      break;

    case MMGSDI_CYCLIC_FILE:
      file_security_access_ptr =
       &task_cnf_ptr->mmgsdi_cnf.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security;
      break;

    case MMGSDI_TRANSPARENT_FILE:
      file_security_access_ptr =
        &task_cnf_ptr->mmgsdi_cnf.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security;
      break;

    case MMGSDI_MASTER_FILE:
    case MMGSDI_DEDICATED_FILE:
    case MMGSDI_MAX_FILE_STRUCTURE_ENUM:
      // TODO
      break;
    }
    if (file_security_access_ptr == NULL)
      return;
    len = file_security_access_ptr->read.num_protection_pin * 
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->write.num_protection_pin * 
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->increase.num_protection_pin * 
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->invalidate_deactivate.num_protection_pin * 
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->rehabilitate_activate.num_protection_pin * 
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    task_cnf_ptr->mmgsdi_cnf.get_file_attr_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_get_file_attr_cnf_type)) + len;
    break;

  case MMGSDI_SESSION_OPEN_CNF:
    task_cnf_ptr->mmgsdi_cnf.session_open_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_session_open_cnf_type));
    break;

  case MMGSDI_SESSION_CLOSE_CNF:
    task_cnf_ptr->mmgsdi_cnf.session_close_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_session_close_cnf_type));
    break;

  case MMGSDI_SESSION_DEACTIVATE_CNF:
    task_cnf_ptr->mmgsdi_cnf.session_deactivate_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_session_deactivate_cnf_type));
    break;

  case MMGSDI_SESSION_SELECT_AID_CNF:
    task_cnf_ptr->mmgsdi_cnf.session_select_aid_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_session_select_aid_cnf_type));
    break;

  case MMGSDI_ISIM_AUTH_CNF:
    task_cnf_ptr->mmgsdi_cnf.isim_auth_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_isim_auth_cnf_type));
    break;
  
  case MMGSDI_SAP_INIT_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_VERIFY_PIN_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_DEREG_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_CONNECT_CNF: /*xxx_sap*/
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_DISCONNECT_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_GET_ATR_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SEND_APDU_CNF:
    task_cnf_ptr->mmgsdi_cnf.send_apdu_cnf.response_header.response_len =
        uint32toint32(sizeof(mmgsdi_send_apdu_cnf_type)) +
        task_cnf_ptr->mmgsdi_cnf.send_apdu_cnf.apdu_data.data_len;
    break;
  case MMGSDI_SAP_SEND_APDU_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_POWER_ON_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_POWER_OFF_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_CARD_READER_STATUS_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_RESET_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_CARD_PDOWN_CNF:
    task_cnf_ptr->mmgsdi_cnf.card_pdown_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_card_pdown_cnf_type));
    break;
  case MMGSDI_CARD_PUP_CNF:
    task_cnf_ptr->mmgsdi_cnf.card_pup_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_card_pup_cnf_type));
    break;
  case MMGSDI_CARD_STATUS_CNF:
    task_cnf_ptr->mmgsdi_cnf.status_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_status_cnf_type)) +
      task_cnf_ptr->mmgsdi_cnf.status_cnf.status_data.data_len;;
    break;
  case MMGSDI_REFRESH_CNF:
    task_cnf_ptr->mmgsdi_cnf.refresh_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_refresh_cnf_type));
    break;
  case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#else
    return;
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
  case MMGSDI_DISABLE_BDN_CNF:
    task_cnf_ptr->mmgsdi_cnf.disable_bdn_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_disable_bdn_cnf_type));
    break;
  case MMGSDI_ENABLE_BDN_CNF:
    task_cnf_ptr->mmgsdi_cnf.enable_bdn_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_enable_bdn_cnf_type));
    break;
  case MMGSDI_ACL_OPERATION_CNF:
    task_cnf_ptr->mmgsdi_cnf.acl_operation_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_acl_operation_cnf_type));
    break;
  case MMGSDI_REHABILITATE_CNF:
    task_cnf_ptr->mmgsdi_cnf.rehab_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_rehabilitate_cnf_type));
    break;
  case MMGSDI_INVALIDATE_CNF:
    task_cnf_ptr->mmgsdi_cnf.invalidate_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_invalidate_cnf_type));
    break;
  case MMGSDI_PIN_OPERATION_CNF:
    task_cnf_ptr->mmgsdi_cnf.pin_operation_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_pin_operation_cnf_type));
    break;
  case MMGSDI_SELECT_AID_CNF:
    task_cnf_ptr->mmgsdi_cnf.select_aid_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_select_aid_cnf_type));
    break;
  case MMGSDI_GET_ALL_PIN_STATUS_CNF:
    task_cnf_ptr->mmgsdi_cnf.get_all_pin_status_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_get_all_pin_status_cnf_type));
    break;
  case MMGSDI_SRV_AVAILABLE_CNF:
#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */
    break;
  case MMGSDI_USIM_AUTH_CNF:
#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */
    break;
  case MMGSDI_BCAST_CNF:
#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */
    break;
  case MMGSDI_COMPUTE_IP_AUTH_CNF:
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
    task_cnf_ptr->mmgsdi_cnf.compute_ip_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_compute_ip_auth_cnf_type)) +
      task_cnf_ptr->mmgsdi_cnf.compute_ip_cnf.cmpt_ip_response_data.data_len;
#endif /* FEATURE_UIM_SUPPORT_3GPD ||  FEATURE_UIM_SUPPORT_HRPD_AN */
    break;
  case MMGSDI_RUN_CAVE_CNF:
#ifdef FEATURE_MMGSDI_CDMA
    task_cnf_ptr->mmgsdi_cnf.run_cave_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_run_cave_cnf_type));
#endif /* FEATURE_MMGSDI_CDMA */
    break;
  case MMGSDI_GENERATE_KEY_VPM_CNF:
#ifdef FEATURE_MMGSDI_CDMA
    task_cnf_ptr->mmgsdi_cnf.generate_key_vpm_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_generate_key_vpm_cnf_type)) +
      task_cnf_ptr->mmgsdi_cnf.generate_key_vpm_cnf.octet_data.data_len;
#endif /* FEATURE_MMGSDI_CDMA */
    break;
  case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT 
    task_cnf_ptr->mmgsdi_cnf.get_jcdma_card_info_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_jcdma_get_card_info_cnf_type));
#endif
    break;
  case MMGSDI_GET_ATR_CNF:
    task_cnf_ptr->mmgsdi_cnf.get_atr_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_get_atr_cnf_type)) +
      task_cnf_ptr->mmgsdi_cnf.get_atr_cnf.atr_data.data_len;
    break;
  case MMGSDI_CARD_READER_STATUS_CNF:
    task_cnf_ptr->mmgsdi_cnf.card_reader_status_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_card_reader_status_cnf_type));
    break;
  case MMGSDI_MFLO_INIT_ACT_PARAMS_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO*/
    break;
  case MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_GET_PUBLIC_KEY_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO*/
    break;
  case MMGSDI_MFLO_SIGN_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_VERIFY_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO*/
    break; 
  case MMGSDI_CARD_RESET_CNF:    
    task_cnf_ptr->mmgsdi_cnf.card_reset_cnf.response_header.response_len =
      uint32toint32(sizeof(mmgsdi_card_reset_cnf_type));
    break;

  case MMGSDI_SEARCH_CNF:
    task_cnf_ptr->mmgsdi_cnf.search_cnf.response_header.response_len = 
      uint32toint32(sizeof(mmgsdi_search_cnf_type)) +
      task_cnf_ptr->mmgsdi_cnf.search_cnf.searched_record_nums.data_len;
    break;
  case MMGSDI_CREATE_PKCS15_TABLE_CNF:
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif
    break;
  case MMGSDI_MAX_CNF_ENUM:
    return;
  }

  task_cnf_ptr->mmgsdi_cnf.response_header.payload_len = 
    task_cnf_ptr->mmgsdi_cnf.response_header.response_len - 
    uint32toint32(sizeof(mmgsdi_response_header_type));
} /* mmgsdi_cnf_populate_cnf_rsp_header */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_DEACTIVATE_DATA

   DESCRIPTION:
     This function builds the deactivate confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_deactivate_data(
  mmgsdi_session_deactivate_cnf_type             *deactivate_cnf_ptr,
  mmgsdi_client_id_type                           client_id,
  const mmgsdi_session_cnf_add_info_type         *add_info_ptr)
{

  MSG_HIGH("MMGSDI_CNF_BUILD_SESSION_DEACTIVATE_DATA",  0, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(deactivate_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(add_info_ptr);

  deactivate_cnf_ptr->session_id = client_id | add_info_ptr->session_id;
  memcpy(&deactivate_cnf_ptr->app_id, &add_info_ptr->app_info,
         sizeof(mmgsdi_aid_type));

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_session_deactivate_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_SELECT_AID_DATA

   DESCRIPTION:
     This function builds the session select aid confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_select_aid_data(
  mmgsdi_session_select_aid_cnf_type             *select_aid_cnf_ptr,
  mmgsdi_client_id_type                           client_id,
  const mmgsdi_session_cnf_add_info_type         *add_info_ptr)
{

  MSG_HIGH("MMGSDI_CNF_BUILD_SESSION_SELECT_AID_DATA",  0, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(select_aid_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(add_info_ptr);

  select_aid_cnf_ptr->session_id = client_id | add_info_ptr->session_id;
  memcpy(&select_aid_cnf_ptr->app_id, &add_info_ptr->app_info,
         sizeof(mmgsdi_aid_type));

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_session_select_aid_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_ACL_OPERATION_DATA

   DESCRIPTION:
     This function build the ACL operation confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.     

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_cnf_build_acl_operation_data(
  mmgsdi_acl_operation_cnf_type                    * acl_op_cnf_ptr,
  const mmgsdi_client_req_extra_info_type          * extra_param_ptr,
  mmgsdi_cmd_enum_type                               req_performed,
  mmgsdi_return_enum_type                            mmgsdi_status
)
{  
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(acl_op_cnf_ptr);

  if (req_performed == MMGSDI_WRITE_REQ)
  {
    /* update global*/
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      if ((mmgsdi_acl_operation_enum_type)extra_param_ptr->write_data.int_client_data 
           == MMGSDI_ACL_OP_ENABLE)
        gsdi_data.sim_capability.acl_enabled = TRUE;
      else
        gsdi_data.sim_capability.acl_enabled = FALSE;
    }
    acl_op_cnf_ptr->acl_op = 
      (mmgsdi_acl_operation_enum_type)extra_param_ptr->write_data.int_client_data;
  }
  else if (req_performed == MMGSDI_ACL_OPERATION_REQ)
  {
    acl_op_cnf_ptr->acl_op = extra_param_ptr->acl_operation_data.acl_op;
  } 
  else
  {
    MSG_ERROR("Invalid req performed for ACL cnf 0x%x", req_performed, 0, 0);
    return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_acl_operation_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REHABILITATE_DATA

   DESCRIPTION:
     This function build the rehabilitate confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_rehabilitate_data(
  mmgsdi_rehabilitate_cnf_type                     * rehabilitate_cnf_ptr,
  const mmgsdi_client_req_extra_info_type          * extra_param_ptr
)
{  
  mmgsdi_return_enum_type build_cnf_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(rehabilitate_cnf_ptr);
    /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(&rehabilitate_cnf_ptr->access,
    (mmgsdi_access_type*)&extra_param_ptr->rehab_data.access);
  
  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Copy Access type failed in rehabilitate 0x%x",
                            build_cnf_status, 0, 0);
  }
  return build_cnf_status;
}/* mmgsdi_cnf_build_rehabilitate_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_INVALIDATE_DATA

   DESCRIPTION:
     This function build the invalidate confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.     

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_cnf_build_invalidate_data(
  mmgsdi_invalidate_cnf_type                     * invalidate_cnf_ptr,
  const mmgsdi_client_req_extra_info_type        * extra_param_ptr
)
{  
  mmgsdi_return_enum_type build_cnf_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(invalidate_cnf_ptr);
    /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(&invalidate_cnf_ptr->access,
    (mmgsdi_access_type*)&extra_param_ptr->invalidate_data.access);
  
  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Copy Access type failed in invalidate 0x%x",
                            build_cnf_status, 0, 0);
  }
  return build_cnf_status;
}/* mmgsdi_cnf_build_invalidate_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_AND_QUEUE

   DESCRIPTION:
     This function build the task command for confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     
   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_and_queue(
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_request_header_type  * req_header_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const void                        * add_data_ptr,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
)
{
  mmgsdi_task_cmd_type    *task_cmd_ptr        = NULL;
  int32                    mem_size            = 0;
  mmgsdi_return_enum_type  build_status        = MMGSDI_SUCCESS;
  uint32                   task_cmd_len        = 0;
  
  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);
  /* It is possible that extra param ptr or add_data_ptr are NULL for various command */

  /*---------------------------------------------------------------------------
    - Calculate the total memory size
    - malloc the total memory size
  ---------------------------------------------------------------------------*/
  mmgsdi_util_cal_align_size(sizeof(mmgsdi_task_cmd_type), 
                               &mem_size);
  // TODO if there are extra data, need to be allocated later as 1 big memory

  task_cmd_len = sizeof(q_link_type) + int32touint32(mem_size);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr, 
    task_cmd_len);
  
  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  task_cmd_ptr->cmd.cmd_enum = MMGSDI_CNF;

  mmgsdi_cnf_map_req_to_cnf_enum_type(
    req_header_ptr->orig_request_type,
    &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.response_type);

  switch(task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.response_type)
  {
  case MMGSDI_WRITE_CNF:
    build_status = mmgsdi_cnf_build_write_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.write_cnf,
      mmgsdi_status, extra_param_ptr);
    break;
  case MMGSDI_READ_CNF:
    build_status = mmgsdi_cnf_build_read_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.read_cnf,
      mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
    break;
  case MMGSDI_GET_FILE_ATTR_CNF:
    build_status = mmgsdi_cnf_build_get_file_attr_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.get_file_attr_cnf,
      mmgsdi_status, extra_param_ptr, req_header_ptr, (mmgsdi_data_type*)add_data_ptr);
    break;
  case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
    build_status = mmgsdi_cnf_build_jcdma_get_card_info(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.get_jcdma_card_info_cnf);
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
    break;
  case MMGSDI_SESSION_CLOSE_CNF:
    build_status = mmgsdi_cnf_build_close_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.session_close_cnf,
      mmgsdi_status, req_header_ptr->client_id, 
      (mmgsdi_client_id_type*)add_data_ptr);
    break;
  case MMGSDI_SESSION_OPEN_CNF:
    build_status = mmgsdi_cnf_build_open_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.session_open_cnf,
      mmgsdi_status, req_header_ptr->client_id,  
      (mmgsdi_client_id_type*)add_data_ptr);
    break;
  case MMGSDI_SESSION_DEACTIVATE_CNF:
    build_status = mmgsdi_cnf_build_session_deactivate_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.session_deactivate_cnf,
      req_header_ptr->client_id, 
      (mmgsdi_session_cnf_add_info_type*)add_data_ptr);
    break;
  case MMGSDI_SESSION_SELECT_AID_CNF:
    build_status = mmgsdi_cnf_build_session_select_aid_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.session_select_aid_cnf,
      req_header_ptr->client_id, 
      (mmgsdi_session_cnf_add_info_type*)add_data_ptr);
    break;
  case MMGSDI_ISIM_AUTH_CNF:
    build_status = mmgsdi_cnf_build_isim_auth_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.isim_auth_cnf,
      mmgsdi_status, (mmgsdi_data_type*)add_data_ptr);
    break;
  case MMGSDI_SEND_APDU_CNF:
    build_status = mmgsdi_cnf_build_send_apdu_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.send_apdu_cnf,
      mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
    break;
  case MMGSDI_SAP_SEND_APDU_CNF:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_POWER_ON_CNF:
    break;
  case MMGSDI_SAP_POWER_OFF_CNF:
    break;
  case MMGSDI_SAP_CONNECT_CNF:
    break;
  case MMGSDI_SAP_INIT_CNF:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_VERIFY_PIN_CNF:
    break;
  case MMGSDI_SAP_DEREG_CNF:
    break;
  case MMGSDI_SAP_DISCONNECT_CNF:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_GET_ATR_CNF:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_CARD_READER_STATUS_CNF: 
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_SAP_RESET_CNF: 
    break;
  case MMGSDI_CARD_PUP_CNF:
    if (extra_param_ptr == NULL)
      return MMGSDI_ERROR;
    build_status = mmgsdi_cnf_build_card_pup_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.card_pup_cnf,
      mmgsdi_status, extra_param_ptr->pup_data.option);
    break;
  case MMGSDI_CARD_PDOWN_CNF:
    if (extra_param_ptr == NULL)
      return MMGSDI_ERROR;
    build_status = mmgsdi_cnf_build_card_pdown_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.card_pdown_cnf,
      mmgsdi_status, extra_param_ptr->pdown_data.option);
    break;
  case MMGSDI_CARD_STATUS_CNF:
    build_status = mmgsdi_cnf_build_status_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.status_cnf,
      mmgsdi_status, (mmgsdi_data_type*)add_data_ptr);
    break;
  case MMGSDI_REFRESH_CNF:
    build_status = mmgsdi_cnf_build_refresh_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.refresh_cnf,
      extra_param_ptr,
      mmgsdi_status);
    break;
  case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
    break;
  case MMGSDI_CLIENT_ID_REG_CNF:
  case MMGSDI_CLIENT_EVT_REG_CNF:
  case MMGSDI_CLIENT_ID_DEREG_CNF:
  case MMGSDI_CLIENT_EVT_DEREG_CNF:
  case MMGSDI_CLIENT_INIT_COMP_CNF:
    break;
  case MMGSDI_DISABLE_BDN_CNF:
    /* update global*/
    if (mmgsdi_status == MMGSDI_SUCCESS)
      gsdi_data.sim_capability.bdn_enabled = FALSE;
    break;
  case MMGSDI_ENABLE_BDN_CNF:
    /* update global*/
    if (mmgsdi_status == MMGSDI_SUCCESS)
      gsdi_data.sim_capability.bdn_enabled = TRUE;
    break;
  case MMGSDI_ACL_OPERATION_CNF:
    build_status = mmgsdi_cnf_build_acl_operation_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.acl_operation_cnf,
      extra_param_ptr,
      req_header_ptr->request_type,
      mmgsdi_status);
    break;
  case MMGSDI_REHABILITATE_CNF:
    build_status = mmgsdi_cnf_build_rehabilitate_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.rehab_cnf,
      extra_param_ptr);
    break;
  case MMGSDI_INVALIDATE_CNF:
    build_status = mmgsdi_cnf_build_invalidate_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.invalidate_cnf,
      extra_param_ptr);
    break;
  case MMGSDI_PIN_OPERATION_CNF:
     build_status = mmgsdi_cnf_build_pin_operation_data(
       req_header_ptr->request_type,
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.pin_operation_cnf,
       mmgsdi_status,
       extra_param_ptr);
    break;
  case MMGSDI_GET_ALL_PIN_STATUS_CNF:
     build_status = mmgsdi_cnf_build_get_all_pin_status_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.get_all_pin_status_cnf,
       mmgsdi_status);
    break;
  case MMGSDI_SELECT_AID_CNF:
    build_status = mmgsdi_cnf_build_select_aid_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.select_aid_cnf,
       mmgsdi_status,
       extra_param_ptr);
    break;
  case MMGSDI_SRV_AVAILABLE_CNF:
#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */
    break;
  case MMGSDI_USIM_AUTH_CNF:
#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */
    break;
  case MMGSDI_COMPUTE_IP_AUTH_CNF:
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
     build_status = mmgsdi_cnf_build_compute_ip_auth_data(
       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.compute_ip_cnf,
       mmgsdi_status,
       (mmgsdi_data_type*)add_data_ptr);
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */
    break;
  case MMGSDI_RUN_CAVE_CNF:
#ifdef FEATURE_MMGSDI_CDMA
    build_status = mmgsdi_cnf_build_run_cave_data(
       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.run_cave_cnf,
       mmgsdi_status,
       (mmgsdi_data_type*)add_data_ptr);
#endif /* FEATURE_MMGSDI_CDMA */
    break;
  case MMGSDI_GENERATE_KEY_VPM_CNF:
#ifdef FEATURE_MMGSDI_CDMA
    build_status = mmgsdi_cnf_build_generate_key_vpm_data(
       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.generate_key_vpm_cnf,
       mmgsdi_status,
       (mmgsdi_data_type*)add_data_ptr);
#endif /* FEATURE_MMGSDI_CDMA */
    break;
  case MMGSDI_GET_ATR_CNF:
    build_status = mmgsdi_cnf_build_get_atr_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.get_atr_cnf,
      mmgsdi_status, req_header_ptr->slot_id);
    break;
  case MMGSDI_CARD_READER_STATUS_CNF:
    build_status = mmgsdi_cnf_build_card_reader_status_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.card_reader_status_cnf,
      mmgsdi_status);
    break;
  case MMGSDI_MFLO_INIT_ACT_PARAMS_CNF:
  case MMGSDI_MFLO_VERIFY_UCAST_CNF:
    break;
  case MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_GET_PUBLIC_KEY_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_SIGN_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_CARD_RESET_CNF:
    break;
  case MMGSDI_BCAST_CNF:
#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */
    break;

  case MMGSDI_SEARCH_CNF:
    build_status = mmgsdi_cnf_build_search_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.search_cnf,
      mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
    break;
  case MMGSDI_CREATE_PKCS15_TABLE_CNF:
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif
    break;
  case MMGSDI_MAX_CNF_ENUM:
    return MMGSDI_ERROR;
  }
  
  if (build_status == MMGSDI_SUCCESS) 
  {
    /* Populate Header */
    /* Header population should be done AFTER data populate
       because any pointer length will be assigned in the 
       individual confirmation population and used in the
       populate_cnf_rsp_header for assigning the response
       length */

    mmgsdi_cnf_populate_cnf_rsp_header(
      &(task_cmd_ptr->cmd.cmd.mmgsdi_cnf),
      req_header_ptr,
      mmgsdi_status,
      sw_present,
      status_word);
    
    rex_enter_crit_sect(&mmgsdi_crit_sect);
    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);
    /* Put the message on the queue */
    q_put(&mmgsdi_task_rsp_q, &task_cmd_ptr->link);

    /* Set the command queue signal for */
    /* receive task                     */
    (void) rex_set_sigs(&gsdi_task_tcb, MMGSDI_TASK_RSP_Q_SIG);
    rex_leave_crit_sect(&mmgsdi_crit_sect);
  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }
  return build_status;
} /* mmgsdi_cnf_build_and_queue */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_WRITE_DATA

   DESCRIPTION:
     This function build the write confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.     

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_write_data(
  mmgsdi_write_cnf_type                   * write_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  
  MSG_HIGH("MMGSDI_CNF_BUILD_WRITE_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(write_cnf_ptr);
  
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    write_cnf_ptr->accessed_rec_num = extra_param_ptr->write_data.rec_num;
    write_cnf_ptr->accessed_offset  = extra_param_ptr->write_data.offset;
    write_cnf_ptr->written_data_len = extra_param_ptr->write_data.data.data_len;
  }

  /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(
    &write_cnf_ptr->access,
    &extra_param_ptr->write_data.access);
  
  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Do not return since we want to populate the rest of the data,
       including callback pointer */
    MMGSDIUTIL_DEBUG_MSG_ERR("Copy Access type failed in write 0x%x",
                            build_cnf_status, 0, 0);
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_write_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_READ_DATA

   DESCRIPTION:
     This function build the read confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_read_data(
  mmgsdi_read_cnf_type                    * read_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * data_read_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  
  MSG_HIGH("MMGSDI_CNF_BUILD_READ_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(read_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(data_read_ptr);
  
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    read_cnf_ptr->accessed_rec_num = extra_param_ptr->read_data.rec_num;
    
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      read_cnf_ptr->read_data.data_ptr,
      data_read_ptr->data_len,
      build_cnf_status);

    /* even if can't malloc for read data, still want to populate
       the cnf header to notify client */
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      return build_cnf_status;
    }
    memcpy(read_cnf_ptr->read_data.data_ptr,
          data_read_ptr->data_ptr,
          int32touint32(data_read_ptr->data_len));

    read_cnf_ptr->read_data.data_len = data_read_ptr->data_len;
  }

  /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(
    &read_cnf_ptr->access,
    &extra_param_ptr->read_data.access);

  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Free the read data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
    MMGSDIUTIL_DEBUG_MSG_ERR("Copy Access type failed in read 0x%x",
                            build_cnf_status, 0, 0);
  }

  return build_cnf_status;
} /* mmgsdi_cnf_build_read_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_FILE_ATTR_DATA

   DESCRIPTION:
     This function build the get file attribute confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_get_file_attr_data(
  mmgsdi_get_file_attr_cnf_type           * get_file_attr_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_request_header_type        * req_header_ptr,
  const mmgsdi_data_type                  * data_read_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  uint8                   *app                 = NULL;
  
  MSG_HIGH("MMGSDI_CNF_BUILD_GET_FILE_ATTR_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(get_file_attr_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(data_read_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);
  
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Check if UICC or ICC */
    switch (req_header_ptr->slot_id)
    {
    case MMGSDI_SLOT_1:
      app = &gsdi_data.card_applications.apps_enabled;
      break;

    case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      app = &gsdi_data.card_applications.apps2_enabled;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
      break;

    default:
      return MMGSDI_ERROR;
    }

    if (app == NULL)
      return MMGSDI_ERROR;

    if ((*app & GSDI_USIM_APP_MASK) == GSDI_USIM_APP_MASK)
    {
      /* Extract the important file attributes from the file header */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
    }
    else
    {
      /* Extract the important file attributes from the file header */
      mmgsdi_icc_rsp_extract_file_attr(req_header_ptr->slot_id,
                                      data_read_ptr->data_ptr,
                                      data_read_ptr->data_len,
                                      &get_file_attr_cnf_ptr->file_attrib );
    }
  }

  /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(&get_file_attr_cnf_ptr->access,
    &extra_param_ptr->get_file_attr_data.access);
  
  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Free any data */
    // TODO free info...
    MMGSDIUTIL_DEBUG_MSG_ERR("Copy Access type failed in get file attr 0x%x",
                            build_cnf_status, 0, 0);
  }
  return build_cnf_status;

} /* mmgsdi_cnf_build_get_file_attr_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_JCDMA_GET_CARD_INFO

   DESCRIPTION:
     This function build the get file attribute confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_jcdma_get_card_info(
  mmgsdi_jcdma_get_card_info_cnf_type     * jcdma_get_card_info_cnf_ptr
)
{    
   MMGSDIUTIL_RETURN_IF_NULL(jcdma_get_card_info_cnf_ptr);

   jcdma_get_card_info_cnf_ptr->card_info = mmgsdi_jcdma_card_get_info();
   jcdma_get_card_info_cnf_ptr->me_esn_change = 
        mmgsdi_get_esn_me_change_flag(
            jcdma_get_card_info_cnf_ptr->response_header.slot_id);

   return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_jcdma_get_card_info */
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */


/* =============================================================================
   FUNCTION:      MMGSDI_SESSION_BUILD_CLOSE_CNF

   DESCRIPTION:
     This function builds the close confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_close_data(
  mmgsdi_session_close_cnf_type *close_cnf_ptr,
  mmgsdi_return_enum_type        mmgsdi_status,
  mmgsdi_client_id_type          client_id,
  const mmgsdi_client_id_type   *session_id_ptr)
{
 
  MSG_HIGH("MMGSDI_CNF_BUILD_CLOSE_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(close_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(session_id_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    close_cnf_ptr->session_id = client_id | *session_id_ptr;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_close_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_SESSION_BUILD_OPEN_CNF

   DESCRIPTION:
     This function builds the open confirmation data
     
   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_open_data(
  mmgsdi_session_open_cnf_type  *open_cnf_ptr,
  mmgsdi_return_enum_type        mmgsdi_status,
  mmgsdi_client_id_type          client_id,
  const mmgsdi_client_id_type   *session_id_ptr)
{
  mmgsdi_return_enum_type  build_cnf_status = MMGSDI_SUCCESS;
  
  MSG_HIGH("MMGSDI_CNF_BUILD_OPEN_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(open_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(session_id_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    open_cnf_ptr->session_id = client_id | *session_id_ptr;
    build_cnf_status = mmgsdi_util_get_channel_id(open_cnf_ptr->session_id,
                                                  &open_cnf_ptr->channel_id);
  }
  MSG_HIGH("open_session cnf build status = 0x%x",build_cnf_status,0,0);
  return build_cnf_status;
} /* mmgsdi_cnf_build_open_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_ISIM_AUTH_DATA

   DESCRIPTION:
     This function builds the ISIM authentication confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_isim_auth_data(
  mmgsdi_isim_auth_cnf_type   *isim_auth_cnf_ptr,
  mmgsdi_return_enum_type      mmgsdi_status,
  const mmgsdi_data_type      *auth_rsp_data_ptr)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  uint32                   auth_len            = 0;

  MSG_HIGH ("MMGSDI_CNF_BUILD_ISIM_AUTH_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);
 
  MMGSDIUTIL_RETURN_IF_NULL(isim_auth_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(auth_rsp_data_ptr);

  
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MSG_HIGH ("Auth Rsp Success", 0, 0, 0);

    /*-------------------------------------------------------------------------
      Authentication response is successful
      - Mem malloc corresponding data pointers for RES, CK and IK
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
    auth_len = int32touint32(auth_rsp_data_ptr[0].data_len);

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      isim_auth_cnf_ptr->res.data_ptr, 
      auth_len, 
      build_cnf_status);
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      return build_cnf_status;
    }

    auth_len = int32touint32(auth_rsp_data_ptr[1].data_len);
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        isim_auth_cnf_ptr->ck.data_ptr, 
        auth_len, 
        build_cnf_status);
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      /* Free the auth data */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(isim_auth_cnf_ptr->res.data_ptr);
      return build_cnf_status;
    }
    
    auth_len = int32touint32(auth_rsp_data_ptr[2].data_len);
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      isim_auth_cnf_ptr->ik.data_ptr, 
      auth_len, 
      build_cnf_status);
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      /* Free the auth data */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(isim_auth_cnf_ptr->res.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(isim_auth_cnf_ptr->ck.data_ptr);
      return build_cnf_status;
    }

    isim_auth_cnf_ptr->res.data_len = 
      auth_rsp_data_ptr[0].data_len;
    isim_auth_cnf_ptr->ck.data_len = 
      auth_rsp_data_ptr[1].data_len;
    isim_auth_cnf_ptr->ik.data_len = 
      auth_rsp_data_ptr[2].data_len;
    memcpy(isim_auth_cnf_ptr->res.data_ptr,
      auth_rsp_data_ptr[0].data_ptr, 
      int32touint32(auth_rsp_data_ptr[0].data_len));
    memcpy(isim_auth_cnf_ptr->ck.data_ptr,
      auth_rsp_data_ptr[1].data_ptr, 
      int32touint32(auth_rsp_data_ptr[1].data_len));
    memcpy(isim_auth_cnf_ptr->ik.data_ptr,
      auth_rsp_data_ptr[2].data_ptr, 
      int32touint32(auth_rsp_data_ptr[2].data_len));
  }
  if (mmgsdi_status == MMGSDI_AUTS_FAIL)
  {
    MSG_HIGH ("Auth AUTS Fail", 0, 0, 0);

    /*-------------------------------------------------------------------------
      Authentication response is AUTS failure
      - Mem malloc corresponding data pointer for AUTS
      - Populate the data pointer
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
    auth_len = int32touint32(auth_rsp_data_ptr[0].data_len);
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      isim_auth_cnf_ptr->auts.data_ptr, 
      auth_len, build_cnf_status);
    if (build_cnf_status == MMGSDI_SUCCESS)
    {
      isim_auth_cnf_ptr->auts.data_len = 
        auth_rsp_data_ptr[0].data_len;
      mmgsdi_status = MMGSDI_AUTS_FAIL;
      memcpy(isim_auth_cnf_ptr->auts.data_ptr,
        auth_rsp_data_ptr[0].data_ptr, 
        int32touint32(auth_rsp_data_ptr[0].data_len));
    }
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_isim_auth_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEND_APDU_DATA

   DESCRIPTION:
     This function builds the send APDU confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_send_apdu_data(
  mmgsdi_send_apdu_cnf_type               * apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * uim_rsp_data_ptr)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  uint32                   apdu_len            = 0;
 
  MSG_HIGH("MMGSDI_CNF_BUILD_SEND_APDU_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(apdu_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    apdu_len =  int32touint32(uim_rsp_data_ptr->data_len);
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      apdu_cnf_ptr->apdu_data.data_ptr,
      apdu_len,
      build_cnf_status);
    
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      return build_cnf_status;
    }
    apdu_cnf_ptr->implicit_get_rsp_sw1 =
      uim_rsp_data_ptr->data_ptr[uim_rsp_data_ptr->data_len-2];
    apdu_cnf_ptr->implicit_get_rsp_sw2 =
      uim_rsp_data_ptr->data_ptr[uim_rsp_data_ptr->data_len-1];
    apdu_cnf_ptr->channel_id =
      extra_param_ptr->send_apdu_data.channel_id;
    apdu_cnf_ptr->apdu_data.data_len = uim_rsp_data_ptr->data_len - 2;
    memcpy(
      apdu_cnf_ptr->apdu_data.data_ptr,
      uim_rsp_data_ptr->data_ptr,
      int32touint32(apdu_cnf_ptr->apdu_data.data_len));
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_send_apdu_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PUP_DATA

   DESCRIPTION:
     This function builds the Card Power Up confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pup_data(
  mmgsdi_card_pup_cnf_type               * card_pup_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pup_options_enum_type        pup_options)
{
  MSG_HIGH("MMGSDI_CNF_BUILD_CARD_PUP_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(card_pup_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {  
    card_pup_cnf_ptr->option = pup_options;
  }
  
  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_card_pup_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PDOWN_DATA

   DESCRIPTION:
     This function builds the Card Power Down confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None    

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pdown_data(
  mmgsdi_card_pdown_cnf_type             * card_pdown_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pdown_options_enum_type      pdown_options)
{
  MSG_HIGH("MMGSDI_CNF_BUILD_CARD_PDOWN_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(card_pdown_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {  
    card_pdown_cnf_ptr->option = pdown_options;
  }
  
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_card_pdown_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REFRESH_DATA

   DESCRIPTION:
     This function builds cnf for the REFRESH

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not

   SIDE EFFECTS:

=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_refresh_data(
  mmgsdi_refresh_cnf_type                 * refresh_cnf_ptr,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status)
{
  MSG_HIGH("MMGSDI_CNF_BUILD_REFRESH_DATA, status = 0x%x",
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(refresh_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  refresh_cnf_ptr->orig_refresh_req = extra_param_ptr->refresh_data.refresh_req_type;
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_refresh_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_DATA

   DESCRIPTION:
     This function builds the Status confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_status_data(
  mmgsdi_status_cnf_type            * status_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_data_type            * status_data_ptr)
{
  mmgsdi_return_enum_type             build_cnf_status = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI_CNF_BUILD_STATUS_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(status_cnf_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_RETURN_IF_NULL(status_data_ptr);
    /* -----------------------------------------------------------------------
       Allocate memory for the Status Response data.
       If the malloc fails, continue with building the confirmation so that
       the client isn't hung.

       Copy the response data if mem is available or indicate in the
       confirmation that the memory was exhausted.

       mmgsdi_status will contain the status we want to return to the client
       for confirmation purposes.
       ---------------------------------------------------------------------*/
    build_cnf_status = mmgsdi_util_alloc((void **)&status_cnf_ptr->status_data.data_ptr,
      status_data_ptr->data_len);

    if ( build_cnf_status == MMGSDI_SUCCESS )
    {
      memcpy(status_cnf_ptr->status_data.data_ptr,
             status_data_ptr->data_ptr,
             int32touint32(status_data_ptr->data_len));

      status_cnf_ptr->status_data.data_len = status_data_ptr->data_len;
    }
    else
    {
      MSG_MED("Setting Return Data Len to 0 since data_ptr is NULL",0,0,0);
      status_cnf_ptr->status_data.data_len = 0;
    }
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_status_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_PIN_OPERATION_DATA

   DESCRIPTION:
     This function builds the PIN Operation confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_pin_operation_data(
  mmgsdi_cmd_enum_type                req_type,
  mmgsdi_pin_operation_cnf_type     * pin_operation_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_client_req_extra_info_type * extra_info_ptr)
{
  mmgsdi_return_enum_type             build_cnf_status = MMGSDI_SUCCESS;
  mmgsdi_pin_extra_info_type          *pin_info_ptr    = NULL;
  mmgsdi_int_app_info_type            *sel_app_ptr     = NULL;

  MSG_HIGH("MMGSDI_CNF_BUILD_PIN_OPERATION_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(pin_operation_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_info_ptr);

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
  
  if(req_type == MMGSDI_GET_FILE_ATTR_REQ)
  {
    pin_info_ptr =  (mmgsdi_pin_extra_info_type*)extra_info_ptr->get_file_attr_data.int_client_data;
  }
  else if(req_type == MMGSDI_PIN_OPERATION_REQ)
  {
    pin_info_ptr = &extra_info_ptr->pin_data;
  }

  MMGSDIUTIL_RETURN_IF_NULL(pin_info_ptr);
  pin_operation_cnf_ptr->pin_info.pin_id = pin_info_ptr->pin_info.pin_id;
  
  if(pin_info_ptr->pin_info.pin_id == MMGSDI_PIN1)
  {
    if(sel_app_ptr->pin1)
    {
      pin_operation_cnf_ptr->pin_info.num_retries         = sel_app_ptr->pin1->num_retries;
      pin_operation_cnf_ptr->pin_info.num_unblock_retries = sel_app_ptr->pin1->num_unblock_retries;
      pin_operation_cnf_ptr->pin_info.pin_id              = sel_app_ptr->pin1->pin_id;
      pin_operation_cnf_ptr->pin_info.status              = sel_app_ptr->pin1->status;
      pin_operation_cnf_ptr->pin_info.pin_replacement     = sel_app_ptr->pin1->pin_replacement;
    }
  }
  else if(pin_info_ptr->pin_info.pin_id == MMGSDI_PIN2)
  {
    pin_operation_cnf_ptr->pin_info.num_retries           = sel_app_ptr->pin2.num_retries;
    pin_operation_cnf_ptr->pin_info.num_unblock_retries   = sel_app_ptr->pin2.num_unblock_retries;
    pin_operation_cnf_ptr->pin_info.pin_id                = sel_app_ptr->pin2.pin_id;
    pin_operation_cnf_ptr->pin_info.status                = sel_app_ptr->pin2.status;
    pin_operation_cnf_ptr->pin_info.pin_replacement       = sel_app_ptr->pin2.pin_replacement;
  }
  else if(pin_info_ptr->pin_info.pin_id == MMGSDI_UNIVERSAL_PIN)
  {
    if(sel_app_ptr->universal_pin)
    {
      pin_operation_cnf_ptr->pin_info.num_retries         = sel_app_ptr->universal_pin->num_retries;
      pin_operation_cnf_ptr->pin_info.num_unblock_retries = sel_app_ptr->universal_pin->num_unblock_retries;
      pin_operation_cnf_ptr->pin_info.pin_id              = sel_app_ptr->universal_pin->pin_id;
      pin_operation_cnf_ptr->pin_info.status              = sel_app_ptr->universal_pin->status;
      pin_operation_cnf_ptr->pin_info.pin_replacement     = sel_app_ptr->universal_pin->pin_replacement;
    }
  }

  pin_operation_cnf_ptr->pin_op = pin_info_ptr->orig_pin_op;

  return build_cnf_status;
} /* mmgsdi_cnf_build_pin_operation_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ALL_PIN_STATUS_DATA

   DESCRIPTION:
     This function builds the PIN Status confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_get_all_pin_status_data(
  mmgsdi_get_all_pin_status_cnf_type     * get_all_pin_status_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status)
{
  mmgsdi_int_app_info_type          * sel_app_ptr;

  MSG_HIGH("MMGSDI_CNF_BUILD_GET_ALL_PIN_STATUS_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(get_all_pin_status_cnf_ptr);

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*Copy Over PIN1*/
    mmgsdi_util_set_pin_info(&get_all_pin_status_cnf_ptr->pin1,sel_app_ptr->pin1);

    /*Copy Over PIN2*/
    mmgsdi_util_set_pin_info(&get_all_pin_status_cnf_ptr->pin2,&sel_app_ptr->pin2);

    /*Copy Over Universal Pin*/
    mmgsdi_util_set_pin_info(&get_all_pin_status_cnf_ptr->universal_pin,sel_app_ptr->universal_pin);
  
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_get_all_pin_status_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SELECT_AID_DATA

   DESCRIPTION:
     This function builds the Select AID confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_select_aid_data(
  mmgsdi_select_aid_cnf_type             * select_aid_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  const mmgsdi_client_req_extra_info_type *extra_info_ptr)
{
  mmgsdi_int_app_info_type       * sel_app_ptr  = NULL;
  mmgsdi_event_data_type         * evt_info_ptr = NULL;
  mmgsdi_return_enum_type          evt_malloc_status;
  uint32                           i;

  MSG_HIGH("MMGSDI_CNF_BUILD_GET_ALL_PIN_STATUS_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(select_aid_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_info_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    for (i=0; i<MMGSDI_MAX_APP_INFO; i++)
    {
      /* Find out which app got selected */
      if (mmgsdi_data_slot1.app_info_ptr[i].app_data.aid.data_len != 
          extra_info_ptr->get_file_attr_data.access.file.app_id.data_len)
        continue;

      if(memcmp((void*)mmgsdi_data_slot1.app_info_ptr[i].app_data.aid.data_ptr, 
                (void*)extra_info_ptr->get_file_attr_data.access.file.app_id.data_ptr,
                int32touint32(mmgsdi_data_slot1.app_info_ptr[i].app_data.aid.data_len )) == 0) 
      {
        mmgsdi_data_slot1.sel_aid_index = i;
        mmgsdi_data_slot1.app_info_ptr[i].app_selected = TRUE;
        sel_app_ptr = &mmgsdi_data_slot1.app_info_ptr[i];
        break;
      }
    }
    MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
    
    select_aid_cnf_ptr->app_data.app_type     = sel_app_ptr->app_data.app_type;
    select_aid_cnf_ptr->app_data.aid.data_len = sel_app_ptr->app_data.aid.data_len;

    memcpy(select_aid_cnf_ptr->app_data.aid.data_ptr, 
           sel_app_ptr->app_data.aid.data_ptr,
           int32touint32(sel_app_ptr->app_data.aid.data_len));
  
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      evt_info_ptr,
      sizeof(mmgsdi_event_data_type),
      evt_malloc_status);

    if(evt_malloc_status != MMGSDI_SUCCESS){
      return MMGSDI_ERROR;
    }
    if(mmgsdi_util_build_select_aid_evt(MMGSDI_SLOT_1,evt_info_ptr) == MMGSDI_SUCCESS)
    {
      (void)mmgsdi_util_notify_client_evt(evt_info_ptr);
    }
    MMGSDIUTIL_TMC_MEM_FREE( evt_info_ptr);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_select_aid_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_COMPUTE_IP_AUTH_DATA

   DESCRIPTION:
     This function builds the Comppute IP authentication confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_compute_ip_auth_data(
  mmgsdi_compute_ip_auth_cnf_type   *compute_ip_auth_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *compute_ip_auth_rsp_data_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  uint32                   compute_ip_len            = 0;

  MSG_HIGH ("MMGSDI_CNF_BUILD_COMPUTE_IP_AUTH_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);
 
  MMGSDIUTIL_RETURN_IF_NULL(compute_ip_auth_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(compute_ip_auth_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MSG_HIGH ("Compute IP Rsp Success", 0, 0, 0);

    /*-------------------------------------------------------------------------
      Compute IP Auth response is successful
      - Mem malloc 
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
   compute_ip_len = int32touint32(compute_ip_auth_rsp_data_ptr->data_len);

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      compute_ip_auth_cnf_ptr->cmpt_ip_response_data.data_ptr, 
      compute_ip_len, 
      build_cnf_status);
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      return build_cnf_status;
    }
    compute_ip_auth_cnf_ptr->cmpt_ip_response_data.data_len = 
      compute_ip_auth_rsp_data_ptr->data_len;
    memcpy(compute_ip_auth_cnf_ptr->cmpt_ip_response_data.data_ptr,
      compute_ip_auth_rsp_data_ptr->data_ptr, 
      compute_ip_len);
  }
  else
  {
    MSG_HIGH("COMPUTE IP Auth Fail, status %x", mmgsdi_status, 0, 0);
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_compute_ip_auth_data */
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */


#ifdef FEATURE_MMGSDI_CDMA
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_RUN_CAVE_DATA

   DESCRIPTION:
     This function builds the Run Cave confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_run_cave_data(
  mmgsdi_run_cave_cnf_type          *run_cave_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *run_cave_rsp_data_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MSG_HIGH ("MMGSDI_CNF_BUILD_RUN_CAVE_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);
 
  MMGSDIUTIL_RETURN_IF_NULL(run_cave_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(run_cave_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MSG_HIGH ("RUN CAVE Rsp Success", 0, 0, 0);

    /*-------------------------------------------------------------------------
      RUN CAVE response is successful
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
   run_cave_cnf_ptr->run_cave_response_data = 
       b_unpackd (run_cave_rsp_data_ptr->data_ptr, 0, run_cave_rsp_data_ptr->data_len*8);
  }
  else
  {
    MSG_ERROR("RUN CAVE Fail, status %x", mmgsdi_status, 0, 0);
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_run_cave_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GENERATE_KEY_VPM_DATA

   DESCRIPTION:
     This function builds the Generate Key confirmation data:  An 8-byte Key and
     the requested set of VPM (Voice Privacy Mask) Octets

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_generate_key_vpm_data(
  mmgsdi_generate_key_vpm_cnf_type  *generate_key_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *generate_key_rsp_data_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status            = MMGSDI_SUCCESS;
  uint32                   generate_key_vpm_len        = 0;

  MSG_HIGH ("MMGSDI_CNF_BUILD_GENERATE_KEY_VPM_DATA, status = 0x%x", 
    mmgsdi_status, 0, 0);
 
  MMGSDIUTIL_RETURN_IF_NULL(generate_key_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(generate_key_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MSG_HIGH ("GENERATE KEY VPM Rsp Success", 0, 0, 0);

    /*-------------------------------------------------------------------------
      GENERATE KEY response is successful
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
   if(generate_key_rsp_data_ptr->data_len < MMGSDI_KEY_SIZE)
   {
     /* As per spec, the response should begin with an 8 byte key */
     MSG_ERROR("Bad response length for generate key! Length: %d",
              generate_key_rsp_data_ptr->data_len,0,0);
     return MMGSDI_ERROR;
   } /* end if generate_key_vpm_len < MMGSDI_KEY_SIZE */
   generate_key_vpm_len = int32touint32(generate_key_rsp_data_ptr->data_len);

   memcpy(generate_key_cnf_ptr->key_data,
      generate_key_rsp_data_ptr->data_ptr, 
      MMGSDI_KEY_SIZE);
   if(generate_key_vpm_len > MMGSDI_KEY_SIZE)
   {
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
          generate_key_cnf_ptr->octet_data.data_ptr, 
          generate_key_vpm_len - MMGSDI_KEY_SIZE, 
          build_cnf_status);
      if (build_cnf_status != MMGSDI_SUCCESS)
      {
        return build_cnf_status;
      }
      memcpy(generate_key_cnf_ptr->octet_data.data_ptr,
             &(generate_key_rsp_data_ptr->data_ptr[MMGSDI_KEY_SIZE]), 
             generate_key_vpm_len - MMGSDI_KEY_SIZE);
      generate_key_cnf_ptr->octet_data.data_len = generate_key_vpm_len - MMGSDI_KEY_SIZE;
   }
   else
   {
     generate_key_cnf_ptr->octet_data.data_len = 0;
   }
  }
  else
  {
    MSG_HIGH("GENERATE KEY VPM Fail, status 0x%x", mmgsdi_status, 0, 0);
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_generate_key_vpm_data */
#endif /* FEATURE_MMGSDI_CDMA */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_POPULATE_CARD_READER_STATUS

   DESCRIPTION:
     This function populates the Card Reader Status data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_cnf_populate_card_reader_status(
  mmgsdi_cmd_enum_type cmd,
  uint8               *card_reader_status_data_ptr)
{
  
  MMGSDIUTIL_RETURN_IF_NULL(card_reader_status_data_ptr);
  /*bits 1-3              = identity of card reader x.
    bit 4               0 = Card reader is not removable
                        1 = Card reader is removable
    bit 5               0 = Card reader is not present
                        1 = Card reader is present
    bit 6               0 = Card reader present is not ID-1 size 
                        1 = Card reader present is ID-1 size
    bit 7               0 = No card present 
                        1 = Card is present in reader 
    bit 8               0 = No card powered
                        1 = Card in reader is powered
  */

  *card_reader_status_data_ptr = 0x00;
  /*Identity of the card reader  = 0 <b1-b3>*/
  /*Card Reader is Not Removable = 0 <b4>*/
  /*Card Reader is  Present      = 1 <b5>*/
  *card_reader_status_data_ptr |= 0x10;
  /*Card Reader is ID-1 size     = 1 <b6>*/
  *card_reader_status_data_ptr |= 0x20;
  /*Card is Present in the reader - Check gsdi_data*/
  /*Card in Reader is powered if it is present*/

  switch (cmd)
  {
  case MMGSDI_SAP_CARD_READER_STATUS_REQ:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case MMGSDI_CARD_READER_STATUS_REQ:
    if(mmgsdi_data_slot1.mmgsdi_state != MMGSDI_STATE_NO_CARD)
    {
      *card_reader_status_data_ptr |= 0xC0;
    }
    break;
  default:
    MSG_HIGH("Invalid Command for Get Card Reader 0x%x", cmd, 0, 0);
    return MMGSDI_ERROR;
  }

  MSG_HIGH("Card Reader Status 0x%x", *card_reader_status_data_ptr, 0, 0);
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_populate_card_reader_status */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_READER_STATUS_DATA

   DESCRIPTION:
     This function builds the Card Reader Status confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_card_reader_status_data(
  mmgsdi_card_reader_status_cnf_type  * card_reader_status_cnf_ptr,
  mmgsdi_return_enum_type               mmgsdi_status)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI CNF BUILD CARD READER STATUS, status = 0x%x",
    mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(card_reader_status_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {      
    card_reader_status_cnf_ptr->card_reader_status_data.data_len = 1;
    build_cnf_status = mmgsdi_cnf_populate_card_reader_status(
      MMGSDI_CARD_READER_STATUS_REQ,
      &card_reader_status_cnf_ptr->card_reader_status_data.data_ptr[0]);
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_card_reader_status_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ATR_DATA

   DESCRIPTION:
     This function builds the get ATR confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_get_atr_data(
  mmgsdi_get_atr_cnf_type       * get_atr_cnf_ptr,
  mmgsdi_return_enum_type         mmgsdi_status,
  mmgsdi_slot_id_enum_type        slot_id)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  uint32                   atr_len             = 0;

  MSG_HIGH("MMGSDI CNF BUILD GET ATR DATA CNF",0,0,0);

  MMGSDIUTIL_RETURN_IF_NULL(get_atr_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    atr_len = int32touint32(mmgsdi_cnf_get_atr_len(slot_id));
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      get_atr_cnf_ptr->atr_data.data_ptr,
      atr_len,
      build_cnf_status);
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }
    build_cnf_status =
      mmgsdi_cnf_populate_atr(
        slot_id,
        &get_atr_cnf_ptr->atr_data.data_len,
        get_atr_cnf_ptr->atr_data.data_ptr);
    if (build_cnf_status != MMGSDI_SUCCESS) 
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(get_atr_cnf_ptr->atr_data.data_ptr);
    }
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_get_atr_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO*/


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_GET_ALL_PIN_STATUS_EVT

   DESCRIPTION:
     This function generates all the pin events as a result of the 
     get_all_pin_status confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     void

   SIDE EFFECTS:
     None
=============================================================================*/
void mmgsdi_cnf_generate_get_all_pin_status_evt(
  void)
{
  mmgsdi_int_app_info_type *sel_app_ptr    = NULL;
  mmgsdi_event_data_type   *mmgsdi_evt_ptr = NULL;
  mmgsdi_return_enum_type   event_status = MMGSDI_SUCCESS;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
     return;
 
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_evt_ptr,
      sizeof(mmgsdi_event_data_type),
      event_status);


  if (event_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Event Info Ptr is NULL",0,0,0);
    return;
  }

  if (sel_app_ptr->pin1 != NULL)
  {
    event_status =  mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                              MMGSDI_SLOT_1,
                                              MMGSDI_PIN1,
                                              sel_app_ptr->pin1->status,
                                              mmgsdi_evt_ptr);
    if(event_status != MMGSDI_SUCCESS){
      MSG_ERROR("Could not Send PIN 1Event to Clients",0,0,0);
    }
    else
    {
      event_status = mmgsdi_util_notify_client_evt(mmgsdi_evt_ptr);
      if(event_status != MMGSDI_SUCCESS)
        MSG_ERROR("Could not Send PIN 1 Event to Clients",0,0,0);
    }
  }

  event_status =  mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                            MMGSDI_SLOT_1,
                                            MMGSDI_PIN2,
                                            sel_app_ptr->pin2.status,
                                            mmgsdi_evt_ptr);
  if(event_status != MMGSDI_SUCCESS){
    MSG_ERROR("Could not Send PIN 2 Event to Clients",0,0,0);
  }
  else
  {
    event_status = mmgsdi_util_notify_client_evt(mmgsdi_evt_ptr);
    
    if(event_status != MMGSDI_SUCCESS)
      MSG_ERROR("Could not Send PIN 2 Event to Clients",0,0,0);
  }
    
  if (sel_app_ptr->universal_pin != NULL)
  {
    event_status =  mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                              MMGSDI_SLOT_1,
                                              MMGSDI_UNIVERSAL_PIN,
                                              sel_app_ptr->universal_pin->status,
                                              mmgsdi_evt_ptr);
    if(event_status != MMGSDI_SUCCESS){
      MSG_ERROR("Could not Send UNIV PIN Event to Clients",0,0,0);
    }
    else
    {
      event_status = mmgsdi_util_notify_client_evt(mmgsdi_evt_ptr);
      if(event_status != MMGSDI_SUCCESS)
        MSG_ERROR("Could not Send UNIV PIN Event to Clients",0,0,0);
    }
  }
  MMGSDIUTIL_TMC_MEM_FREE( mmgsdi_evt_ptr);
} /* mmgsdi_cnf_generate_get_all_pin_status_evt */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_PIN_OP_EVT

   DESCRIPTION:
     This function generates pin events as a result of the 
     pin_op confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     void

   SIDE EFFECTS:
     None
=============================================================================*/
static void mmgsdi_cnf_generate_pin_op_evt(
  mmgsdi_return_enum_type              mmgsdi_status,
  const mmgsdi_pin_operation_cnf_type *pin_cnf_ptr)
{
  mmgsdi_event_data_type     *mmgsdi_evt_ptr = NULL;
  mmgsdi_return_enum_type     event_status   = MMGSDI_SUCCESS;
  mmgsdi_pin_status_enum_type pin_status;
  if (pin_cnf_ptr == NULL)
    return;

  pin_status = pin_cnf_ptr->pin_info.status;

  if ((pin_status != MMGSDI_PIN_BLOCKED) && (pin_status != MMGSDI_PIN_PERM_BLOCKED))
  {
    /* for non blocked or perm blocked status, only a success operation should
       generate an event */
    if (mmgsdi_status != MMGSDI_SUCCESS)
      return;
    switch (pin_cnf_ptr->pin_op)
    {
      case MMGSDI_PIN_OP_CHANGE:
        pin_status = MMGSDI_PIN_CHANGED;
        break;
      case MMGSDI_PIN_OP_UNBLOCK:
        pin_status = MMGSDI_PIN_UNBLOCKED;
        break;
      case MMGSDI_PIN_OP_VERIFY:
      case MMGSDI_PIN_OP_ENABLE:
        break;
      case MMGSDI_PIN_OP_DISABLE:
      case MMGSDI_PIN_OP_DISABLE_AND_REPLACE:
        pin_status = MMGSDI_PIN_DISABLED;
        break;
      case MMGSDI_PIN_OP_NONE:
        return;
    }
  }

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_evt_ptr,
      sizeof(mmgsdi_event_data_type),
      event_status);

  if(event_status == MMGSDI_SUCCESS)
  {
    event_status = mmgsdi_util_build_pin_evt(
                     pin_cnf_ptr->response_header.client_id,
                     pin_cnf_ptr->response_header.slot_id,
                     pin_cnf_ptr->pin_info.pin_id,
                     pin_status,
                     mmgsdi_evt_ptr);
    if (event_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Could not Send PIN Event to Clients",0,0,0);
    }
    else
    {
     event_status = mmgsdi_util_notify_client_evt(mmgsdi_evt_ptr);
     if(event_status != MMGSDI_SUCCESS)
       MSG_ERROR("Could not Send PIN Event to Clients",0,0,0);
    }
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_evt_ptr); 
  }
} /* mmgsdi_cnf_generate_pin_op_evt */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_EVT

   DESCRIPTION:
     This function based on the confirmation, generate the corresponding event
     as needed

   DEPENDENCIES:
     None

   LIMITATIONS:
     None     

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
void mmgsdi_cnf_generate_evt(
  mmgsdi_return_enum_type    mmgsdi_status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr)
{
  switch(cnf)
  {
  case MMGSDI_PIN_OPERATION_CNF:
    switch (cnf_ptr->pin_operation_cnf.pin_info.status)
    {
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
        return;
      case MMGSDI_PIN_ENABLED_VERIFIED:
      case MMGSDI_PIN_DISABLED:
      case MMGSDI_PIN_BLOCKED:
      case MMGSDI_PIN_PERM_BLOCKED:
      case MMGSDI_PIN_UNBLOCKED:
      case MMGSDI_PIN_CHANGED:
        mmgsdi_cnf_generate_pin_op_evt(mmgsdi_status, &cnf_ptr->pin_operation_cnf);
        break;
    }
    break;
  default:
    return;
  }
}/* mmgsdi_cnf_generate_evt */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEARCH_DATA

   DESCRIPTION:
     This function build the search confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS : The command processing was successful.
     MMGSDI_ERROR   : The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_cnf_build_search_data(
  mmgsdi_search_cnf_type                   * search_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * data_search_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  
  MSG_HIGH("MMGSDI_CNF_BUILD_SEARCH_DATA, status = 0x%x", 
           mmgsdi_status, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(search_cnf_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(data_search_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Copy the response data */
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      search_cnf_ptr->searched_record_nums.data_ptr,
      data_search_ptr->data_len,
      build_cnf_status);

    /* even if can't malloc for read data, still want to populate
       the cnf header to notify client */
    if (build_cnf_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Unable to allocate memory to search response data",0,0,0);
      return build_cnf_status;
    }
    memcpy(search_cnf_ptr->searched_record_nums.data_ptr,
           data_search_ptr->data_ptr,
           int32touint32(data_search_ptr->data_len));
    search_cnf_ptr->searched_record_nums.data_len = data_search_ptr->data_len;

  } /* End of if (mmgsdi_status == MMGSDI_SUCCESS) */

  /* Copy the access data info from request pointer to confirmation pointer */
  build_cnf_status = mmgsdi_util_copy_access_type(&search_cnf_ptr->access,
                                                  &extra_param_ptr->search_data.access);

  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Free the search data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(search_cnf_ptr->searched_record_nums.data_ptr);
    MMGSDIUTIL_DEBUG_MSG_ERR("Copy Access type failed in read 0x%x",
                             build_cnf_status, 0, 0);
  }

  return build_cnf_status;

}/* mmgsdi_cnf_build_search_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


