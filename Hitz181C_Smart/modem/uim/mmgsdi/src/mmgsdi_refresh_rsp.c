/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   R E F R E S H   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the refresh response processing.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2007-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_refresh_rsp.c#6 $$ $DateTime: 2009/05/15 02:20:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp       Demand Paging Changes
04/29/07   js       Included header mmgsdi_refresh_rsp.h
03/30/09   xz       1) Fix issue of not sending specific error code to client 
                       when REFRESH fails
                    2) Change mmgsdi_refresh_rsp_send_cnf_to_gstk() return type
03/19/09   js       Support for Enhanced NAA Refresh by file path
12/02/08   ssr      Removed Compilation Error
12/01/08   ssr      Handled error condition for some MMGSDI Functions
09/22/08   js       Fixed Lint Errors
06/04/08   tml      Added sw to cnf header
03/17/08   sun      Allow cleanup to be done during a refresh
11/12/07   sun      Initial version

===========================================================================*/


#include "comdef.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_refresh_rsp.h"

/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_RSP_CLEAN_UP

  DESCRIPTION:
    This function will clean up the refresh linked list and the global
    refresh pointer

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
void mmgsdi_refresh_rsp_clean_up(void)
{
  mmgsdi_refresh_node_type *node_ptr = NULL;
  mmgsdi_slot_data_type    *slot_ptr = NULL;

  if(mmgsdi_refresh_info_ptr == NULL)
  {
    MSG_HIGH("Nothing to cleanup, refresh ptr is NULL",0,0,0);
    return;
  }

  switch(mmgsdi_refresh_info_ptr->request_header.slot_id)
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
      MSG_ERROR("Invalid Slot 0x%x",
                 mmgsdi_refresh_info_ptr->request_header.slot_id,0,0);
      return;
  }

  while(mmgsdi_refresh_info_ptr->node_ptr)
  {
    node_ptr = mmgsdi_refresh_info_ptr->node_ptr;
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(node_ptr->refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(node_ptr->refresh_files.file_path_ptr);
    mmgsdi_refresh_info_ptr->node_ptr = node_ptr->next_ptr;
    MMGSDIUTIL_TMC_MEM_FREE(node_ptr);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    mmgsdi_refresh_info_ptr->refresh_data.refresh_files.file_path_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_refresh_info_ptr);

  slot_ptr->refresh_state = MMGSDI_REFRESH_READY_STATE;

  (void)rex_clr_timer(&mmgsdi_refresh_timer);
  (void)rex_clr_timer(&mmgsdi_refresh_auto_trigger_timer);
}/*mmgsdi_refresh_rsp_clean_up*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_RSP_SEND_CNF_TO_GSTK

  DESCRIPTION:
    This function will send confirmation to GSTK

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          Succeed in send confirmation to GSTK
    MMGSDI_ERROR:            Fail to send confirmation to GSTK
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_refresh_rsp_send_cnf_to_gstk(
  mmgsdi_request_header_type            *request_hdr_ptr)
{
  mmgsdi_return_enum_type             refresh_status = MMGSDI_CAN_NOT_REFRESH;
  mmgsdi_request_header_type          *req_hdr_ptr   = request_hdr_ptr;
  mmgsdi_client_req_extra_info_type   extra_param;
  mmgsdi_return_enum_type             mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_sw_status_type               status_word;

  if ( req_hdr_ptr == NULL  )
  {                          
    MMGSDIUTIL_DEBUG_MSG_ERR("Attempt to use NULL POINTER",0,0,0); 
    return mmgsdi_status;                                
  }
  
  /* status word not applicable in refresh to gstk */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  if(mmgsdi_refresh_info_ptr != NULL)
  {
    refresh_status = mmgsdi_refresh_info_ptr->refresh_status;
    if(mmgsdi_refresh_info_ptr->additional_files_read)
    {
      refresh_status = MMGSDI_REFRESH_ADDITIONAL_FILES_READ;
    }
    req_hdr_ptr    = &mmgsdi_refresh_info_ptr->request_header;
  }
  extra_param.refresh_data.refresh_req_type = MMGSDI_REFRESH_ORIG_REFRESH_REQ;

  /*Find the index of the original request*/
  mmgsdi_status = mmgsdi_cnf_build_and_queue(refresh_status,
                                             req_hdr_ptr,
                                             &extra_param,
                                             NULL,
                                             FALSE, /* sw not required
                                                       for refresh */
                                             status_word);
  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    MSG_ERROR("mmgsdi_refresh_rsp_send_cnf_to_gstk is failed 0x%x",
               mmgsdi_status, 0, 0);
  }    
  mmgsdi_refresh_rsp_clean_up();
  return mmgsdi_status;
}/*mmgsdi_refresh_rsp_send_cnf_to_gstk*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_MAIN_RSP

  DESCRIPTION:
    This function will process any refresh response

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The function was successful is sending the response
    MMGSDI_ERROR:            Could not get client index/
                             Could not build confirmation/
                             Other error
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_main_rsp(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header       = {0};
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_refresh_orig_enum_type      refresh_req      = MMGSDI_REFRESH_ORIG_REFRESH_REQ;

  MSG_HIGH("MMGSDI REFRESH MAIN RSP", 0,0,0);

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

  report_status = (uim_rsp_ptr->is_uim_success)?
                  MMGSDI_SUCCESS : 
                  uim_rsp_ptr->mmgsdi_error_status;
  
  if(req_header.request_type == MMGSDI_REFRESH_REQ &&
     extra_param_ptr         != NULL)
  {
    refresh_req = extra_param_ptr->refresh_data.refresh_req_type;

    if(refresh_req == MMGSDI_REFRESH_ORIG_REFRESH_REQ &&
      (extra_param_ptr->refresh_data.stage != MMGSDI_REFRESH_STAGE_END_SUCCESS &&
       extra_param_ptr->refresh_data.stage != MMGSDI_REFRESH_STAGE_END_FAILED))
    {
      /*Confirmation will be built later*/
      return MMGSDI_SUCCESS;
    }

    /* Build the REFRESH Confirmation only when we get to one of these stages
     *
     * If REFRESH fails before start, we should get status from uim_rsp_ptr
     * instead of mmgsdi_refresh_info_ptr, which is null in this case.
     */
    if((refresh_req == MMGSDI_REFRESH_ORIG_REFRESH_REQ ||
       refresh_req == MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ ||
       refresh_req == MMGSDI_REFRESH_ORIG_COMPLETE_REQ ) &&
      (extra_param_ptr->refresh_data.stage == MMGSDI_REFRESH_STAGE_END_SUCCESS ||
       extra_param_ptr->refresh_data.stage == MMGSDI_REFRESH_STAGE_END_FAILED) &&
      (mmgsdi_refresh_info_ptr != NULL))
    {       
       mmgsdi_status = mmgsdi_refresh_rsp_send_cnf_to_gstk(&req_header);
       
       /* above func will also call mmgsdi_cnf_build_and_queue(),
        * return here to avoid dup mesg to GSTK
        */
       return mmgsdi_status;
    }
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status, &req_header,
                                             extra_param_ptr, NULL,
                                             FALSE, /* sw not required for refresh */
                                             uim_rsp_ptr->status_word);

  MSG_HIGH("Generic Response handling for Cmd 0x%x, status 0x%x",
    req_header.request_type, report_status, 0);

  return mmgsdi_status;
}/*mmgsdi_refresh_main_rsp*/

