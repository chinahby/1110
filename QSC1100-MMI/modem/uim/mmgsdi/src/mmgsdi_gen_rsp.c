/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I M   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UIM response processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2005, 2008, 2009 QUALCOMM, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_gen_rsp.c#2 $$ $DateTime: 2009/04/29 16:02:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/09   js      Included header mmgsdi_gen_rsp.h
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
07/17/08   kk      Added support for PKCS 15
06/04/08   tml     Added sw to cnf rsp header and message cleanup
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO Supports. 
08/07/06   tml     Added ACL support
05/23/06   tml     BDN support
04/18/06   nk      Featurized and converted macro calls to function calls
12/14/05   tml     MMGSDI Cleanup
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
08/26/05   pv      Carry over the result from uim to the confirmation
08/25/05   pv      Initial Version

===========================================================================*/


#include "comdef.h"
#include "gsdi.h"
#include "gsdi_exp.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_sap_rsp.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdiutil.h"
#include "rex.h"
#include "uim.h"
#include "uimgen.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_gen_rsp.h"

#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_GEN_RSP

   DESCRIPTION:
     This function will process the any generic response 
     Note, for BDN and ACL, this function should only come called if it
     is a failed operation, if successful operation, should be handled by 
     the get_file_attribute_response and write_response accordingly

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_gen_rsp(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header;
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type              status_word;

  MSG_HIGH("MMGSDI PROCESS GEN RSP", 0,0,0);
  
  /* status word is not applicable in gen response processing */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info status: 0x%x",
                             mmgsdi_status, 0, 0);
    return mmgsdi_status;
  }

  MSG_HIGH("Generic Response handling for Cmd 0x%x, status 0x%x",
    req_header.request_type, report_status, 0);

  report_status = (uim_rsp_ptr->is_uim_success)? MMGSDI_SUCCESS : MMGSDI_ERROR;
  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status, &req_header, 
                                             extra_param_ptr, NULL,
                                             FALSE, status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp */


/*=============================================================================
  FUNCTION: MMGSDI_GEN_RSP_GET_ATR

  DESCRIPTION:
    The response to the Get ATR command.  The ATR is internally stored in
    MMGSDI and the response is built internally in MMGSDI

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
==============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_rsp_get_atr (
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type              status_word;

  MSG_HIGH("MMGSDI_GEN_RSP_GET_ATR", 0, 0, 0);

  /* status word is not applicable to ATR */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(uim_rsp_ptr->rsp_data_index, 0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info( uim_rsp_ptr->rsp_data_index,
                                                             &req_header,
                                                             &uim_cmd_ptr,
                                                             &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR(
      "Error returned from mmgsdi_util_get_client_request_table_info (%d)",
      mmgsdi_status,
      0,
      0);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr, 
                                             NULL,
                                             FALSE, /* sw not required for 
                                                       atr */
                                             status_word);

  MSG_HIGH("GEN_RSP - GET_ATR   status is 0x%x", report_status, 0, 0);
  return mmgsdi_status;
} /* mmgsdi_gen_rsp_get_atr */


/*===========================================================================
FUNCTION MMGSDI_GEN_RSP_CARD_READER_STATUS

DESCRIPTION
  This function is called to process the response for the Card Reader Status.
  The card Status is being handled internally by MMGSDI.

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
mmgsdi_return_enum_type mmgsdi_gen_rsp_card_reader_status(
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_sw_status_type              status_word;

  MSG_HIGH("MMGSDI_GEN_RSP_CARD_READER_STATUS", 0, 0, 0);

  /* status word is not applicable in get card reader processing */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(uim_rsp_ptr->rsp_data_index, 0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info( uim_rsp_ptr->rsp_data_index,
                                                             &req_header,
                                                             &uim_cmd_ptr,
                                                             &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR(
      "Error returned from mmgsdi_util_get_client_request_table_info (%d)",
      mmgsdi_status,
      0,
      0);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                             &req_header,
                                             NULL,
                                             NULL,
                                             FALSE, /* sw not required for card
                                                       reader status */
                                             status_word);

  MSG_HIGH("GEN_RSP - CARD_READER_STATUS  status 0x%x", mmgsdi_status, 0, 0);

  return mmgsdi_status;

}/* mmgsdi_gen_rsp_card_reader_status */


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */ 

#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION */
