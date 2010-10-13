/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   S E S S I O N   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_session.c#5 $$ $DateTime: 2009/05/15 02:20:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/02/09   mib     Wrong pointer is freed in mmgsdi_session_build_select_aid
03/09/09   mib     Fixed error conditions during open session
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/19/08   js      Fixed KW error for null ptr dereferencing
11/05/08   tml     Fixed session release boundary check
09/22/08   js      Fixed Lint Errors
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
06/04/08   tml     Add sw to cnf header, add more debug messages
12/05/07   sun     Mainlined mmgsdi_session_build_status_terminate so that
                   it canbe used for 2G reset as well
11/15/07   sun     Added support for NAA refresh
10/02/07   tml     Added proprietary app session support
09/10/07   tml     Lint changes
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/22/07   nk      Fixed compilation error
05/17/07   wli     Added support for JCDMA RUIM
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/03/07   sp      Send status command before terminating the app
03/19/07   sun     Fixed Lint Error
01/12/07   tml     Allow select AID API for deactivate process default APP
12/12/06   tml     Changed deactivate to use SELECT command to the card with
                   indication for app termination
10/26/06   tml     Decode PIN upon successful Select AID
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     Externalize mmgsdi_session_release_info.
06/02/06   tml     Added BDN support
05/20/06   tml     Cleanup channel when open failed and lint
04/18/06   nk      Featurized code
12/14/05   tml     MMGSDI Cleanup
11/10/05   tml     Added Lint fixes
11/10/05   tml     Revert Lint fixes
11/10/05   tml     Return client ID as part of the session ID if open and
                   close session successfully
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
09/29/05   tml     Client Deregistration support
07/26/05   sst     Fixed lint errors
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
02/24/05   tml     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include <stdlib.h>
#include "gsdi.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdiutil.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_session.h"
#include "mmgsdi_cnf.h"

#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT

static mmgsdi_client_id_type MMGSDI_CLIENT_ID_MASK = 0xFFFFFFFF00000000uLL;

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_POPULATE_SESSION_INFO

   DESCRIPTION:
     This function will populate the session information into the
     client registration table.

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

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_populate_session_info(
  const mmgsdi_uim_report_rsp_type           *uim_rsp_ptr,
  const mmgsdi_request_header_type           *req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type *extra_info_ptr,
  int32                                       client_index,
  int32                                       session_index  )
{
  /* This function will be expanded later to properly fill in session information */
  MSG_HIGH (
    "MMGSDI_SESSION_POPULATE_SESSION_INFO, client_ind = 0x%x, session_ind = 0x%x",
    client_index, session_index, 0);

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  return mmgsdi_uicc_rsp_decode_pin_header_data (
      req_header_ptr,
      extra_info_ptr,
      uim_rsp_ptr->rsp_data.data_ptr,
      uim_rsp_ptr->rsp_data.data_len);

} /*lint !e715*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RELEASE_APP_INFO

   DESCRIPTION:
     This function will release application information in regard to the
     session where the specific application has been deactivated.
     The session info, e.g., the session id, channel id and slot id will
     remain valid since this function does not imply a channel has been
     closed.

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

-----------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type  mmgsdi_session_release_app_info (
  int32                             client_index,
  int32                             session_index
)
{

  int32 i = 0;

  MSG_HIGH (
    "MMGSDI_SESSION_RELEASE_APP_INFO, client_ind = 0x%x, session_ind = 0x%x",
    client_index, session_index, 0);

  if ((client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index >= MMGSDI_MAX_CHANNEL_INFO))
  {
    MMGSDIUTIL_DEBUG_MSG_ERR(
      "Unable to release session data of client_index:0x%x, session_index:0x%x",
      client_index, session_index, 0);
    return MMGSDI_ERROR;
  }

  if (mmgsdi_client_id_reg_table[client_index].num_session == 0)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("mmgsdi_client_id_reg_table[0x%x].num session is 0",
      client_index, 0, 0);
    return MMGSDI_ERROR;
  }

  if (mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr == NULL)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("client_index:0x%x, session_index:0x%x -> app_info_ptr is NULL",
      client_index, session_index, 0);
    return MMGSDI_ERROR;
  }

  /* Reset application ID */
  for (i = 0; i < MMGSDI_MAX_APP_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr ==
        &mmgsdi_app_pin_table[i])
    {
      if (mmgsdi_app_pin_table[i].proprietary_app == TRUE)
      {
        memset(&mmgsdi_app_pin_table[i],
               0x00,
               sizeof(mmgsdi_int_app_info_type));
        break;
      }
    }
  }
  mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr = NULL;

  return MMGSDI_SUCCESS;
}/* mmgsdi_session_release_app_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RELEASE_INFO

   DESCRIPTION:
     This function will release information in regard to the session that
     has been deactivated and closed.

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

-----------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type  mmgsdi_session_release_info (
  int32                             client_index,
  int32                             session_index
)
{

  MSG_HIGH (
    "MMGSDI_SESSION_RELEASE_INFO, client_ind = 0x%x, session_ind = 0x%x",
    client_index, session_index, 0);

  if ((client_index >= MMGSDI_MAX_CLIENT_INFO) || 
      (client_index < 0))
  {
    MMGSDIUTIL_DEBUG_MSG_ERR(
      "unable to release info, client index 0x%x, session index 0x%x",
      client_index, session_index, 0);
    return MMGSDI_ERROR;
  }

  if ( mmgsdi_session_release_app_info(client_index, session_index) != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("unable to release app info, client index 0x%x, session index 0x%x",
      client_index, session_index, 0);
    return MMGSDI_ERROR;
  }

  mmgsdi_client_id_reg_table[client_index].session_info[session_index].channel_id = 0;
  mmgsdi_client_id_reg_table[client_index].session_info[session_index].slot =
    MMGSDI_MAX_SLOT_ID_ENUM;

  mmgsdi_client_id_reg_table[client_index].num_session--;

  return MMGSDI_SUCCESS;
}/*mmgsdi_session_release_info*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_ACTIVATE_APP_INFO

   DESCRIPTION:
     This function will add the app information to the session index of the
     client. Will be used by open session and refresh

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

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_activate_app_info(
  mmgsdi_client_id_type         client_id,
  mmgsdi_static_data_type       app_id)
{
  int32           client_index  = 0;
  int32           session_index = 0;
  mmgsdi_aid_type aid;
  int32           i             = 0;

  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  /* Check if the client table's app info is valid or not, if null, populate
     with the requested data */
  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                  &client_index,
                                                  &session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
    return mmgsdi_status;

  if ((client_index < 0) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index > MMGSDI_MAX_CHANNEL_INFO) ||
      (session_index < 0))
  {
     MMGSDIUTIL_DEBUG_MSG_ERR(
       "Invalid client_index:0x%x, session_index:0x%x",
        client_index, session_index, 0);
        return MMGSDI_ERROR;
  }

  if((session_index < MMGSDI_MAX_CHANNEL_INFO) &&
     (mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr == NULL))
  {
    aid.aid.data_len = app_id.data_len;
    memcpy(aid.aid.data_ptr,
           app_id.data_ptr,
           int32touint32(aid.aid.data_len));

    for (i = 0; i < MMGSDI_MAX_APP_INFO; i++)
    {
      mmgsdi_status = mmgsdi_util_cmp_aid(&mmgsdi_app_pin_table[i].app_data,
                                          &aid);

      if (mmgsdi_status == MMGSDI_SUCCESS)
        break;
    }
    if (i < MMGSDI_MAX_APP_INFO)
    {
      mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr =
        &mmgsdi_app_pin_table[i];
    }
    else
    {
      MSG_ERROR("Reach max channel supported for client 0x%x",
                 client_id, 0, 0);
      return MMGSDI_ERROR;
    }
  }
  else
  {
    MSG_ERROR("Invalid Session ID, cannot activate",0,0,0);
    return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_session_activate_app_info*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_ISIM_AUTH

   DESCRIPTION:
     This function handles the response for ISIM Authenication from the card

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

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_isim_auth(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_return_enum_type               report_status   = MMGSDI_SUCCESS;
  int32                                 index           = 0;
  uim_cmd_type                        * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr = NULL;
  int32                                 offset          = 0;
  int32                                 i               = 0;
  int32                                 j               = 0;
  mmgsdi_data_type                      temp_auth_rsp_buf[3];
  uint32                                auth_data_len   = 0;

  MSG_HIGH("MMGSDI_SESSION_RSP_ISIM_AUTH", 0, 0, 0);

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  memset(&temp_auth_rsp_buf[0], 0x00, sizeof(mmgsdi_data_type));
  memset(&temp_auth_rsp_buf[1], 0x00, sizeof(mmgsdi_data_type));
  memset(&temp_auth_rsp_buf[2], 0x00, sizeof(mmgsdi_data_type));

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
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }

  if (!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response(
      uim_rsp_ptr);
  }

  /* parse authentication response into temporary data buffers */
  offset = 1;
  if (uim_rsp_ptr->rsp_data.data_len > offset)
  {
    if ((mmgsdi_isim_auth_rsp_tag_enum_type)uim_rsp_ptr->rsp_data.data_ptr[0]
          == MMGSDI_ISIM_3G_AUTH_SUCCESS)
    {
      /* Authenication success */
      for (i = 0; i < 3; i++)
      {
        if (uim_rsp_ptr->rsp_data.data_len <= offset)
        {
          for (j = 0; j < i; j++)
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[j].data_ptr);
            temp_auth_rsp_buf[j].data_len = 0;
          }
          report_status = MMGSDI_ERROR;
          break;
        }
        temp_auth_rsp_buf[i].data_len = uim_rsp_ptr->rsp_data.data_ptr[offset];
        auth_data_len = int32touint32(temp_auth_rsp_buf[i].data_len);
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
          temp_auth_rsp_buf[i].data_ptr,
          auth_data_len,
          mmgsdi_status);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          for (j = 0; j < i; j++)
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[j].data_ptr);
          }
          report_status = MMGSDI_ERROR;
          break;
        }
        offset++;
        memcpy(temp_auth_rsp_buf[i].data_ptr,
          &uim_rsp_ptr->rsp_data.data_ptr[offset],
          auth_data_len);
        offset += temp_auth_rsp_buf[i].data_len;
      }
    }
    else if ((mmgsdi_isim_auth_rsp_tag_enum_type)uim_rsp_ptr->rsp_data.data_ptr[0]
             == MMGSDI_ISIM_SYNC_FAIL)
    {
      /* Authenication AUTS failure */
      temp_auth_rsp_buf[0].data_len = uim_rsp_ptr->rsp_data.data_ptr[offset];
      auth_data_len = int32touint32(temp_auth_rsp_buf[0].data_len);
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        temp_auth_rsp_buf[0].data_ptr,
        auth_data_len,
        mmgsdi_status);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        offset++;
        memcpy(temp_auth_rsp_buf[0].data_ptr,
          &uim_rsp_ptr->rsp_data.data_ptr[offset],
          auth_data_len);
        report_status = MMGSDI_AUTS_FAIL;
      }
      else
      {
        report_status = MMGSDI_ERROR;
      }
    }
    else
    {
      if (report_status == MMGSDI_SUCCESS) {
        MSG_ERROR("Have data response, but not Succses or AUTS failure, ignore the data", 0, 0, 0);
        report_status = MMGSDI_ERROR;
      }
    }
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             NULL,
                                             temp_auth_rsp_buf,
                                             FALSE, /* sw not required
                                                       for ISIM auth */
                                             uim_rsp_ptr->status_word);

  /* Free temp auth response data */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[0].data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[1].data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[2].data_ptr);

  MSG_HIGH("SESSION_RSP - ISIM AUTH  status 0x%x", report_status, 0, 0);

  return mmgsdi_status;
} /* mmgsdi_session_rsp_isim_auth */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_BUILD_SELECT_AID_FROM_INDICES

   DESCRIPTION:
     This function will build the select AID command and put it into the
     mmgsdi command queue

   DEPENDENCIES:


   LIMITATIONS:
     SESSION_SELECT_AID_REQ should be used for non default app only,
     for default app, use SELECT_AID_REQ

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

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_build_select_aid_from_indices(
  const mmgsdi_request_header_type *req_header_ptr,
  int32                             client_index,
  int32                             session_index)
{
  mmgsdi_static_data_type  aid_info;

  MSG_HIGH("MMGSDI_SESSION_BUILD_SELECT_AID_FROM_INDICES, client_ind =0x%x, session_ind = 0x%x",
    client_index, session_index, 0);

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr);

  aid_info.data_len =
    mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr->app_data.aid.data_len;

  memcpy(aid_info.data_ptr,
         mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr->app_data.aid.data_ptr,
         int32touint32(aid_info.data_len));

  /* orig_request_type should be OPEN */
  return mmgsdi_session_build_select_aid(req_header_ptr->client_id,
                                         req_header_ptr->slot_id,
                                         req_header_ptr->orig_request_type,
                                         MMGSDI_SESSION_SELECT_AID_REQ,
                                         aid_info,
                                         req_header_ptr->response_cb,
                                         req_header_ptr->client_data);
}


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_SELECT_AID

DESCRIPTION:
  This function will build the select AID command and put it into the
  mmgsdi command queue

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

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_build_select_aid(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               orig_req_type,
  mmgsdi_cmd_enum_type               curr_req_type,
  mmgsdi_static_data_type            aid,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr  = NULL;
  mmgsdi_access_type                    file_name;
  mmgsdi_return_enum_type               mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type        *get_file_attr_msg  = NULL;
  int32                                 total_mem_len      = 0;
  int32                                 var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                                 temp_mem           = 0;
  uint32                                task_cmd_len       = 0;

  MSG_HIGH("MMGSDI_SESSION_BUILD_SELECT_AID, client_id 0x%x", client_id, 0, 0);

  if ((curr_req_type != MMGSDI_SESSION_DEACTIVATE_REQ) &&
      (curr_req_type != MMGSDI_SESSION_SELECT_AID_REQ))
  {
    MSG_ERROR("Curr Req Type should be either Deactivate or Select AID 0x%x",
               curr_req_type, 0, 0);
    return MMGSDI_ERROR;
  }

  file_name.access_method = MMGSDI_BY_APP_ID_ACCESS;
  file_name.file.app_id.data_len = aid.data_len;

  memcpy(file_name.file.app_id.data_ptr,
         aid.data_ptr,
         int32touint32(file_name.file.app_id.data_len));

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  var_len[0] = file_name.file.app_id.data_len;

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len[0],
                              &temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len[0],temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);


  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_file_attr_req,
                         curr_req_type);

  get_file_attr_msg = &task_cmd_ptr->cmd.cmd.get_file_attr_req;

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
  task_cmd_ptr->cmd.cmd_enum                          = curr_req_type;
  get_file_attr_msg->request_header.client_id         = client_id;
  get_file_attr_msg->request_header.request_type      = curr_req_type;
  get_file_attr_msg->request_header.orig_request_type = orig_req_type;
  get_file_attr_msg->request_header.request_id_ptr    = rex_self();
  get_file_attr_msg->request_header.slot_id           = slot_id;
  get_file_attr_msg->request_header.client_data       = client_ref;
  get_file_attr_msg->request_header.response_cb       = response_cb_ptr;
  get_file_attr_msg->request_header.request_len       = total_mem_len;
  get_file_attr_msg->request_header.payload_len       = get_file_attr_msg->request_header.request_len -
                                                          uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1.  access_method       -  Access type specified in the function paramters
    2.  file_enum           -  File name specified in the function paramters
                           OR
    2a. path                -  Path specified in the function paramters
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&get_file_attr_msg->access,
    &file_name);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Get File Attr Lib Fail: Access Copy Fail 0x%x",
                             mmgsdi_status, 0, 0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    return mmgsdi_status;
  }

  if (curr_req_type == MMGSDI_SESSION_DEACTIVATE_REQ)
  {
    get_file_attr_msg->activate_aid = FALSE;
  }
  else if (curr_req_type == MMGSDI_SESSION_SELECT_AID_REQ)
  {
    get_file_attr_msg->activate_aid = TRUE;
  }

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  return mmgsdi_status;
}


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_BUILD_CLOSE_REQ

   DESCRIPTION:
     This function build the close channel management request and put it onto
     the MMGSDI task queue.

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

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_build_close_req (
  const mmgsdi_request_header_type           * req_header_ptr
)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr  = NULL;
  mmgsdi_session_close_req_type        *close_msg      = NULL;
  mmgsdi_return_enum_type               mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                 total_mem_len  = 0;
  uint32                                task_cmd_len   = 0;

  MSG_HIGH("MMGSDI_SESSION_BUILD_CLOSE_REQ", 0, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  close_msg = &task_cmd_ptr->cmd.cmd.session_close_req;

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
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_SESSION_CLOSE_REQ;
  close_msg->request_header.client_id         = req_header_ptr->client_id;
  close_msg->request_header.request_type      = MMGSDI_SESSION_CLOSE_REQ;
  close_msg->request_header.orig_request_type = req_header_ptr->orig_request_type;
  close_msg->request_header.request_id_ptr    = rex_self();
  close_msg->request_header.slot_id           = req_header_ptr->slot_id;
  close_msg->request_header.client_data       = req_header_ptr->client_data;
  close_msg->request_header.response_cb       = req_header_ptr->response_cb;
  close_msg->request_header.request_len       = total_mem_len;
  close_msg->request_header.payload_len       = total_mem_len -
                                                uint32toint32(sizeof(mmgsdi_request_header_type));

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  return mmgsdi_status;
}

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_OPEN_FAILED_CLEANUP

   DESCRIPTION:
     This function based on whether a channel has been opened successfully
     or not, it will build a request to close the channel.  In additionl,
     It will perform clean up on the session info table

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     client id in the req_header_ptr will be updated to remove the session ID

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_open_failed_cleanup (
  mmgsdi_request_header_type       *req_header_ptr,
  boolean                           channel_opened,
  int32                             client_index,
  int32                             session_index,
  mmgsdi_return_enum_type           report_status
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_client_id_type   session_id    = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_sw_status_type   status_word;

  MSG_HIGH("Open Channel Failed, Performing cleanup 0x%x, 0x%x",
             client_index, session_index, 0);

  /* status word not applicable to session open failed */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);
  /* A channel is opened, but either during open rsp processing or select aid process
     fail, therefore, the complete open procedure failed.
     Need to close the channel and send back a failure.  The confirmation and cleanup
     process are being handled in close response processing.
     If a channel failed to open, only required to cleanup and build
     confirmation to client */
  if (channel_opened)
  {
    mmgsdi_status = mmgsdi_session_build_close_req(req_header_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Unable to build the close channel request 0x%x", mmgsdi_status, 0, 0);
      MSG_ERROR("Potential lingering channels and session table info", 0, 0, 0);
    }
    return mmgsdi_status;
  }

  /* overwrite the client_id so that the return confirmation will not have session ID in it */
  req_header_ptr->client_id =
    (req_header_ptr->client_id & MMGSDI_CLIENT_ID_MASK);

  /* channel was not opened, just need to perform client id reg table cleanup */
  /* client and session index check here, in case of channel already opened, the
     client index and session index check will be taken care of in the close
     channel request */
  if ((client_index < 0) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index < 0) ||
      (session_index >=  MMGSDI_MAX_CHANNEL_INFO))
  {
    MSG_ERROR("Invalid Client index 0x%x or session index 0x%x",
               client_index, session_index, 0);
    return MMGSDI_ERROR;
  }

  /* Cleanup session information data */
  mmgsdi_status = mmgsdi_session_release_info(client_index, session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Release was not completed */
    MSG_ERROR("Unable to release the info for clientindex 0x%x, sessionindex 0x%x",
      client_index, session_index, 0);
  }
  else
  {
    mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id =
      0;
    /* build the confirmation */
    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                               req_header_ptr,
                                               NULL,
                                               &session_id,
                                               FALSE, /* sw not required */
                                               status_word);
    MSG_HIGH("SESSION_RSP - OPEN FAILED cleanup   status 0x%x",
      report_status, 0, 0);
  }
  return mmgsdi_status;
}/*mmgsdi_session_open_failed_cleanup*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_SELECT_AID

   DESCRIPTION:
     This function handles the response for Select on AID returned by the
     card.  It also builds a confirmation message to the client.

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

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_select_aid(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_return_enum_type               report_status   = MMGSDI_ERROR;
  int32                                 index           = 0;
  int32                                 client_index    = MMGSDI_MAX_CLIENT_INFO;
  int32                                 session_index   = MMGSDI_MAX_CHANNEL_INFO;
  uim_cmd_type                        * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr = NULL;
  mmgsdi_client_id_type                 session_id      = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_cnf_add_info_type      select_aid_cnf_info;

  MSG_HIGH("MMGSDI_SESSION_RSP_SELECT_AID", 0, 0, 0);

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
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    /* no way to perform open cleanup since client req table entry is invalid */
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_client_id_index(req_header.client_id,
                                                    &client_index,
                                                    &session_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                        TRUE,
                                                        MMGSDI_MAX_CLIENT_INFO,
                                                        MMGSDI_MAX_CHANNEL_INFO,
                                                        MMGSDI_ERROR);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Open Failed, cleanup error and not able to build confirmation0x%x", mmgsdi_status, 0, 0);
    }

    return mmgsdi_status;
  }
  if ((client_index < 0) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index < 0) ||
      (session_index >=  MMGSDI_MAX_CHANNEL_INFO))
  {
    /* Invalid client index and session index */
    mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                        TRUE,
                                                        client_index,
                                                        session_index,
                                                        MMGSDI_ERROR);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Open Failed, cleanup error and not able to build confirmation0x%x", mmgsdi_status, 0, 0);
    }

    return mmgsdi_status;
  }

  if (!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = MMGSDI_SUCCESS;
  }

  if (req_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ)
  {
    if (report_status == MMGSDI_SUCCESS)
    {
      /* Populate table */
      mmgsdi_status = mmgsdi_session_populate_session_info(
        uim_rsp_ptr,
        &req_header,
        (const mmgsdi_get_file_info_extra_info_type*)&extra_param_ptr->get_file_attr_data,
        client_index,
        session_index);

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        session_id =
            mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id;
        MSG_HIGH("SESSION_RSP - SELECT AID  status 0x%x", mmgsdi_status, 0, 0);
        return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                          &req_header,
                                          NULL,
                                          &session_id,
                                          FALSE, /* sw not required */
                                          uim_rsp_ptr->status_word);
      }
    }
    mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                     TRUE,
                                                     client_index,
                                                     session_index,
                                                     report_status);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Open Failed, cleanup error and not able to build confirmation0x%x", mmgsdi_status, 0, 0);
    }
    return mmgsdi_status;
  }
  else if (req_header.orig_request_type == MMGSDI_SESSION_SELECT_AID_REQ)
  {
    /* SESSION_SELECT_AID should be used for non default app only,
       for default app, use SELECT_AID */
    memcpy(&select_aid_cnf_info.app_info,
           &mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr->app_data,
           sizeof(mmgsdi_aid_type));

    select_aid_cnf_info.session_id = mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id;

    if (report_status != MMGSDI_SUCCESS)
    {
      /* Cleanup session information data */
      if (mmgsdi_session_release_app_info(client_index, session_index) != MMGSDI_SUCCESS)
      {
        /* Release was not completed */
        MSG_ERROR("Unable to release the info for clientindex 0x%x, sessionindex 0x%x",
          client_index, session_index, 0);
      }
    }
    MSG_HIGH ("SESSION_RSP - SELECT_AID  status 0x%x", report_status, 0, 0);
    return mmgsdi_cnf_build_and_queue(report_status,
                                      &req_header,
                                      NULL,
                                      (void*)&select_aid_cnf_info,
                                      FALSE, /* sw not required for session
                                                select aid */
                                      uim_rsp_ptr->status_word);
  }
  else
  {
    /* can't build a "valid" confirmation from the context of this function,
       return to the process_response so that the static_cnf can be used to
       build a generic confirmation */
    MSG_ERROR("Unknown orig_req in select_aid_rsp 0x%x",
               req_header.orig_request_type, 0, 0);
    return MMGSDI_ERROR;
  }
} /* mmgsdi_session_rsp_select_aid */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_SELECT_AID

   DESCRIPTION:
     This function builds command to select the AID

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

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_select_aid (
  mmgsdi_get_file_attr_req_type * msg_ptr
)
{

  mmgsdi_uim_additional_params_type add_params;
  mmgsdi_get_file_attr_cnf_type   * cnf_ptr         = NULL;
  MSG_HIGH("MMGSDI_SESSION_SELECT_AID",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr->request_header.response_cb);

  switch (msg_ptr->access.access_method)
  {
  case MMGSDI_BY_APP_ID_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Access Type 0x%x",
                            msg_ptr->access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  add_params.path_array_ptr = NULL;
  add_params.path_len       = 0;

  MSG_HIGH("Sending down UICC SELECT AID command to MMGSDI UIM len 0x%x, 0x0x",
    msg_ptr->access.file.app_id.data_len,msg_ptr->access.file.app_id.data_ptr,0);

  /* Check if the client table's app info is valid or not, if null, populate
     with the requested data */
  if (msg_ptr->activate_aid)
  {
    if ( MMGSDI_SUCCESS != mmgsdi_session_activate_app_info(
                                             msg_ptr->request_header.client_id,
                                             msg_ptr->access.file.app_id))
    {
      MSG_ERROR("Activate App Info failed",0,0,0);	
    }
  }
  return mmgsdi_uim_uicc_select ( msg_ptr,
                                  &add_params,
                                  FALSE,
                                  &cnf_ptr );
}/*mmgsdi_session_select_aid*/



/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_CLOSE

   DESCRIPTION:
     This function handles the response for Close Channel Management.  It also
     builds the confirmation message and put it ont ot he MMGSDI task queue.

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

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_close(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_SUCCESS;
  int32                                 index           = 0;
  int32                                 client_index    = MMGSDI_MAX_CLIENT_INFO;
  int32                                 session_index   = MMGSDI_MAX_CHANNEL_INFO;
  uim_cmd_type                        * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr = NULL;
  mmgsdi_client_id_type                 session_id      = MMGSDI_INVALID_SESSION_ID;

  MSG_HIGH("MMGSDI_SESSION_RSP_CLOSE", 0, 0, 0);

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
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }

  if (uim_rsp_ptr->is_uim_success)
  {
    mmgsdi_status = mmgsdi_util_get_client_id_index(req_header.client_id,
                                                    &client_index,
                                                    &session_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_HIGH("SESSION_RSP - CLOSE  Unable to get client id index", 0, 0, 0);
      return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL,
                                        &session_id, FALSE, uim_rsp_ptr->status_word);
    }
    if ((client_index >= MMGSDI_MAX_CLIENT_INFO) ||
        (session_index >=  MMGSDI_MAX_CHANNEL_INFO))
    {
      MMGSDIUTIL_DEBUG_MSG_ERR("Invalid client_index 0x%x and session_index 0x%x",
                                client_index, session_index, 0);
      return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL,
                                        &session_id, FALSE, uim_rsp_ptr->status_word);
    }

    /* assign the session id to temporary variable before releasing session info */
    session_id =
      mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id;

    /* Successful, build the open confirmation */
    mmgsdi_status = mmgsdi_session_release_info(client_index, session_index);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Release was not completed, set the temp session id to invalid session id */
      session_id = MMGSDI_INVALID_SESSION_ID;
    }
    else
    {
      mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id =
        0;
    }
  }
  else
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    mmgsdi_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  if (req_header.orig_request_type == MMGSDI_SESSION_CLOSE_REQ)
  {
    MSG_HIGH("SESSION_RSP - CLOSE  status 0x%x", mmgsdi_status, 0, 0);
    return mmgsdi_cnf_build_and_queue(mmgsdi_status, &req_header, NULL,
                                      &session_id, FALSE, uim_rsp_ptr->status_word);
  }
  else
  {
    session_id = MMGSDI_INVALID_SESSION_ID;
    /* processing a close req as a result of OPEN req, therefore,
       there is no need to send the session id in the confirmation */
    req_header.client_id = (req_header.client_id & MMGSDI_CLIENT_ID_MASK);

    MSG_HIGH("SESSION_RSP - CLOSE  non orig_request_type != SESSION_REQ",
              0, 0, 0);
    return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL,
                                      &session_id, FALSE,
                                      uim_rsp_ptr->status_word);
  }
} /* mmgsdi_session_rsp_close */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_DEACTIVATE

   DESCRIPTION:
     This function handles the response for Deactivating an application via
     a non-0 channel.  It also builds the confirmation message and put it
     onto the MMGSDI task queue.

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

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_deactivate (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr )
{
  mmgsdi_request_header_type              req_header;
  mmgsdi_return_enum_type                 mmgsdi_status   = MMGSDI_SUCCESS;
  int32                                   index           = 0;
  uim_cmd_type                          * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type     * extra_param_ptr = NULL;
  int32                                   client_index    = MMGSDI_MAX_CLIENT_INFO;
  int32                                   session_index   = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_client_id_type                   session_id      = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_cnf_add_info_type        deactivate_cnf_info;
  mmgsdi_return_enum_type                 report_status   = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type              * sel_app_ptr     = NULL;

  MSG_HIGH("MMGSDI_SESSION_RSP_DEACTIVATE", 0, 0, 0);

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_client_id_index(req_header.client_id,
                                                  &client_index,
                                                  &session_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_HIGH("SESSION_RSP - DEACTIVATE  status 0x%x", mmgsdi_status, 0, 0);
    return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                      &req_header,
                                      NULL,
                                      &session_id,
                                      FALSE,
                                      uim_rsp_ptr->status_word);
  }

  if (uim_rsp_ptr->is_uim_success)
  {
    report_status = MMGSDI_SUCCESS;
  }
  else
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (req_header.orig_request_type == MMGSDI_SESSION_CLOSE_REQ)
  {
    if (report_status == MMGSDI_SUCCESS)
    {
      if ((client_index >= MMGSDI_MAX_CLIENT_INFO) ||
          (session_index >= MMGSDI_MAX_CHANNEL_INFO))
      {

        MMGSDIUTIL_DEBUG_MSG_ERR("Invalid client_index 0x%x or session_index 0x%x",
                             client_index, session_index, 0);
        return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                          &req_header,
                                          NULL,
                                          &session_id,
                                          FALSE, /* sw not required */
                                          uim_rsp_ptr->status_word);
      }

      /* Build the Close command message */
      mmgsdi_status = mmgsdi_session_build_close_req(&req_header);

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      /* If not able to successfully build the close req, send the confirmation */
    }
    MSG_HIGH("SESSION_RSP - DEACTIVATE  report_status 0x%x, mmgsdi_status 0x%x",
      report_status, mmgsdi_status, 0);
    return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL,
                                      &session_id, FALSE, uim_rsp_ptr->status_word);
  }
  else if (req_header.orig_request_type == MMGSDI_SESSION_DEACTIVATE_REQ)
  {
    if ((client_index >= MMGSDI_MAX_CLIENT_INFO) ||
        (session_index > MMGSDI_MAX_CHANNEL_INFO))
    {
      MMGSDIUTIL_DEBUG_MSG_ERR("Invalid client_index 0x%x or session_index 0x%x",
                                client_index, session_index, 0);
      return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL,
                                        &session_id, FALSE, uim_rsp_ptr->status_word);
    }
    if (session_index == MMGSDI_MAX_CHANNEL_INFO)
    {
      MSG_HIGH("Default Channel Deactivation", 0, 0, 0);
      sel_app_ptr = mmgsdi_util_get_sel_app();
      if(sel_app_ptr == NULL)
        return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL,
                                          &session_id, FALSE, uim_rsp_ptr->status_word);
      memcpy(&deactivate_cnf_info.app_info, &sel_app_ptr->app_data, sizeof(mmgsdi_aid_type));
      deactivate_cnf_info.session_id = 0;
    }
    else
    {
      memcpy(&deactivate_cnf_info.app_info,
            &mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr->app_data,
            sizeof(mmgsdi_aid_type));

      deactivate_cnf_info.session_id = mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id;
    }

    if (report_status == MMGSDI_SUCCESS)
    {
      if (session_index < MMGSDI_MAX_CHANNEL_INFO) {
        /* Channel relate info needs to be kept, but the application info
           needs to be removed */
        mmgsdi_status = mmgsdi_session_release_app_info(client_index, session_index);

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          /* Release was not completed, set the temp session id to invalid session id */
          MSG_ERROR("Unable to clean up session client_index 0x%x, session_index 0x%x, session_id 0x%x",
            client_index, session_index, session_id);
        }
      }
      // TODO if session_index == MMGSDI_MAX_CHANNEL_INFO, make the pointer
      //   to global sel_app_ptr NULL
    }
    MSG_HIGH("SESSION_RSP - DEACTIVATE  status 0x%x", report_status, 0, 0);
    return mmgsdi_cnf_build_and_queue(report_status, &req_header, NULL,
                                     (void*)&deactivate_cnf_info, FALSE,
                                     uim_rsp_ptr->status_word);
  }
  else
  {
    /* can't build a "valid" confirmation from the context of this function,
       return to the process_response so that the static_cnf can be used to
       build a generic confirmation */
    MSG_ERROR("Unknown orig_req in deactivate_rsp 0x%x",
               req_header.orig_request_type, 0, 0);
    return MMGSDI_ERROR;
  }
} /* mmgsdi_session_rsp_deactivate*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_OPEN

   DESCRIPTION:
     This function check the return data from UIM in regard to the Manage
     Channel Open operation.  If the channel was open successfully, it
     builds the Select AID command, else it builds the confirmation type

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

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_open(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr )
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_return_enum_type               report_status       = MMGSDI_SUCCESS;
  int32                                 index               = 0;
  uim_cmd_type                        * uim_cmd_ptr         = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr     = NULL;
  int32                                 client_index        = MMGSDI_MAX_CLIENT_INFO;
  int32                                 session_index       = MMGSDI_MAX_CHANNEL_INFO;

  MSG_HIGH("MMGSDI_SESSION_RSP_OPEN", 0, 0, 0);

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
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_client_id_index(req_header.client_id,
                                                  &client_index,
                                                  &session_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                       uim_rsp_ptr->is_uim_success,
                                                       MMGSDI_MAX_CLIENT_INFO,
                                                       MMGSDI_MAX_CHANNEL_INFO,
                                                       MMGSDI_ERROR);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Open Failed, cleanup error and not able to build confirmation0x%x", mmgsdi_status, 0, 0);
    }

    return mmgsdi_status;
  }
  if ((client_index < 0) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index < 0) ||
      (session_index >=  MMGSDI_MAX_CHANNEL_INFO))
  {
    mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                       uim_rsp_ptr->is_uim_success,
                                                       client_index,
                                                       session_index,
                                                       MMGSDI_ERROR);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Open Failed, cleanup error and not able to build confirmation0x%x", mmgsdi_status, 0, 0);
    }

    return mmgsdi_status;
  }

  if (uim_rsp_ptr->is_uim_success)
  {
    mmgsdi_client_id_reg_table[client_index].session_info[session_index].channel_id =
      uim_rsp_ptr->rsp_data.data_ptr[0];

    /* Build the Select AID message */
    mmgsdi_status = mmgsdi_session_build_select_aid_from_indices(&req_header,
                                                                 client_index,
                                                                 session_index);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Unable to buidl a select AID request, close the channel and
         send back a failure */
      mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                         uim_rsp_ptr->is_uim_success,
                                                         client_index,
                                                         session_index,
                                                         MMGSDI_ERROR);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Open Failed, cleanup error and not able to build confirmation0x%x", mmgsdi_status, 0, 0);
      }

      return mmgsdi_status;
    }
    return mmgsdi_status;
  }
  else
  {
   /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response( uim_rsp_ptr);

    /* Open was unsuccessful, clean up the session info, there is no need to
       send out an explicit CLOSE Channel request to the card since the
       channel was never opened */
    mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                       uim_rsp_ptr->is_uim_success,
                                                       client_index,
                                                       session_index,
                                                       report_status);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Open Failed, cleanup error and not able to build confirmation0x%x", mmgsdi_status, 0, 0);
    }

    return mmgsdi_status;
  }
} /* mmgsdi_session_rsp_open */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_OPEN

   DESCRIPTION:
     This function will perform parameter checks, generate session ID, save
     the data into the client reg table and proceed with the Manage Channel
     Open command to the card

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

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_open(
  mmgsdi_session_open_req_type *msg_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_client_id_type   session_id        = 0;
  int                     j                 = 0;
  mmgsdi_client_id_type   client_id         = 0;
  int32                   client_index      = 0;
  int32                   session_index     = 0;

  MSG_HIGH("MMGSDI_SESSION_OPEN", 0, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr->request_header.response_cb);

  /* -----------------------------------------------------------
  1) Check for valid client ID
  2) Check for application ID
  3) Obtain client index
  -------------------------------------------------------------- */
  client_id = msg_ptr->request_header.client_id;
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
    return mmgsdi_status;

  /* To remove the lower int32 to ensure there is no session id */
  client_id = (client_id & MMGSDI_CLIENT_ID_MASK);

  if ( msg_ptr->app_id_data.data_len < 0 )
    return MMGSDI_ERROR;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                 &client_index,
                                                 &session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
    return mmgsdi_status;

  /* -----------------------------------------------------------
  1) Generate session ID
  3) Save the Application ID
  4) Send the open channel command to UIM
  -------------------------------------------------------------- */
  session_id = (mmgsdi_client_id_type)(int32touint32(rand()));
  while(j < MMGSDI_MAX_CLIENT_INFO)
  {
    mmgsdi_client_id_type temp_client_id;
    temp_client_id = client_id | session_id;
    
    if (mmgsdi_util_is_valid_client_id(temp_client_id) == MMGSDI_SUCCESS)
    {
      /* Search for next one */
      session_id++;
      session_id = (session_id & (uint64)MMGSDI_SESSION_ID_MASK);
    }
    else
    {
      client_id = temp_client_id;
      break;
    }
    j++;
  }
  
  if (j >= MMGSDI_MAX_CLIENT_INFO)
  {
    MSG_ERROR("Unable to generate session id", 0, 0, 0);
    return MMGSDI_ERROR;
  }

  if (mmgsdi_client_id_reg_table[client_index].num_session >= MMGSDI_MAX_CHANNEL_INFO)
  {
    MSG_ERROR("Reach max channel supported for client 0x%x", client_id, 0, 0);
    return MMGSDI_ERROR;
  }

  session_index = mmgsdi_client_id_reg_table[client_index].num_session;

  mmgsdi_client_id_reg_table[client_index].num_session++;
  mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id = session_id;
  mmgsdi_client_id_reg_table[client_index].session_info[session_index].slot =
    msg_ptr->request_header.slot_id;

  /* Populate the client ID and newly created session ID to the msg_ptr */
  msg_ptr->request_header.client_id = client_id;

  mmgsdi_status = mmgsdi_session_activate_app_info(client_id,msg_ptr->app_id_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Reset session info because app info was not added yet 
       and mmgsdi_session_release_info() would fail causing
       a pending session */
    mmgsdi_client_id_reg_table[client_index].num_session--;
    mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id = 0;
    mmgsdi_client_id_reg_table[client_index].session_info[session_index].slot =
      MMGSDI_MAX_SLOT_ID_ENUM;
    mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr =
      NULL;

    MSG_ERROR("Activate App Info failed",0,0,0);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_uim_uicc_open_channel(msg_ptr);
  
  return mmgsdi_status;
} /* mmgsdi_session_open */

#endif /*FEATURE_MMGSDI_SESSION_MANAGEMENT*/

/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_STATUS_TERMINATE

DESCRIPTION:
  This function is used to build a status command for a particular
  application termination

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
  If status command is executed succesfully, it builds up a command to
  terminate the app also.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_build_status_terminate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  int32                          client_index       = MMGSDI_MAX_CLIENT_INFO;
  int32                          session_index      = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_return_enum_type        mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_task_cmd_type          *task_cmd_ptr       = NULL;
  mmgsdi_status_req_type        *status_msg_ptr     = NULL;
  int32                          total_mem_len      = 0;
  uint32                         task_cmd_len       = 0;

  mmgsdi_cmd_enum_type           curr_req_cmd       = MMGSDI_CARD_STATUS_REQ;


  MSG_HIGH("MMGSDI_SESSION_BUILD_STATUS, client 0x%x, slot 0x%x",
           client_id, card_slot, 0);

  /* get the client id index from client_id*/
  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                  &client_index,
                                                  &session_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Invalid client id 0x%x, status 0x%x",
               client_id, mmgsdi_status, 0);
    return mmgsdi_status;
  }

  /* check for client id & session id index*/
  if ((client_index < 0) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index < 0) ||
      (session_index >  MMGSDI_MAX_CHANNEL_INFO))
  {
    MSG_ERROR("Invalid client index 0x%x, or session index 0x%x",
               client_index, session_index, 0);
    return MMGSDI_ERROR;
  }
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.status_req,
                         curr_req_cmd);
  status_msg_ptr = &task_cmd_ptr->cmd.cmd.status_req;

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
  task_cmd_ptr->cmd.cmd_enum                       = curr_req_cmd;
  status_msg_ptr->request_header.client_id         = client_id;
  status_msg_ptr->request_header.request_type      = curr_req_cmd;
  status_msg_ptr->request_header.orig_request_type = orig_req_cmd;
  status_msg_ptr->request_header.request_id_ptr    = rex_self();
  status_msg_ptr->request_header.slot_id           = card_slot;
  status_msg_ptr->request_header.client_data       = client_ref;
  status_msg_ptr->request_header.response_cb       = response_cb_ptr;
  status_msg_ptr->request_header.request_len       = total_mem_len;
  status_msg_ptr->request_header.payload_len       = status_msg_ptr->request_header.request_len -
                                                       uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1.  me_app_status       -  MMGSDI_STATUS_APP_TERMINATED
                               ( indicate app termination )
    2.  ret_data_struct     -  MMGSDI_STATUS_NO_DATA
                               ( no need to return any data except status words)
  ---------------------------------------------------------------------------*/
  status_msg_ptr->me_app_status   = MMGSDI_STATUS_APP_TERMINATED;
  status_msg_ptr->ret_data_struct = MMGSDI_STATUS_NO_DATA;

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  return mmgsdi_status;

} /* mmgsdi_session_build_status_terminate */


#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_DEACTIVATE

DESCRIPTION:
  This function is used to build a deactivate command for a particular
  application

DEPENDENCIES:
  The client must have a valid session open.
  Achived by calling mmgsdi_session_management_open.

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
mmgsdi_return_enum_type  mmgsdi_session_build_deactivate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  int32                              client_index  = MMGSDI_MAX_CLIENT_INFO;
  int32                              session_index = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_static_data_type           *aid_ptr       = NULL;

  MSG_HIGH("MMGSDI_SESSION_MANAGEMENT_CLOSE, client 0x%x, slot 0x%x",
           client_id, card_slot, 0);

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                  &client_index,
                                                  &session_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Invalid client id 0x%x, status 0x%x",
               client_id, mmgsdi_status, 0);
    return mmgsdi_status;
  }

  if ((client_index < 0) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index < 0) ||
      (session_index >  MMGSDI_MAX_CHANNEL_INFO))
  {
    MSG_ERROR("Invalid client index 0x%x, or session index 0x%x",
               client_index, session_index, 0);
    return MMGSDI_ERROR;
  }

  aid_ptr = mmgsdi_util_get_aid(client_id);

  if (aid_ptr == NULL)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Null AID for client ID 0x%x", client_id, 0, 0);
    return MMGSDI_ERROR;
  }

  return mmgsdi_session_build_select_aid (
    client_id,
    card_slot,
    orig_req_cmd,
    MMGSDI_SESSION_DEACTIVATE_REQ,
    *aid_ptr,
    response_cb_ptr,
    client_ref);

} /* mmgsdi_session_build_deactivate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN

   DESCRIPTION:
     This function will provide open session using the ICC protocol

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

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main (
  mmgsdi_task_cmd_data_type      * cmd_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_cmd_enum_type              cmd                  = MMGSDI_MAX_CMD_ENUM;

  MSG_HIGH("MMGSDI_SESSION_MAIN", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    cmd_ptr                                - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(cmd_ptr);

  cmd = cmd_ptr->cmd_enum;

  switch (cmd)
  {
  case MMGSDI_SESSION_OPEN_REQ:
    mmgsdi_status = mmgsdi_session_open(&cmd_ptr->cmd.session_open_req);
    break;

  case MMGSDI_SESSION_CLOSE_REQ:
    mmgsdi_status = mmgsdi_uim_uicc_close_channel(&cmd_ptr->cmd.session_close_req);
    break;

  case MMGSDI_SESSION_SELECT_AID_REQ:
    mmgsdi_status = mmgsdi_session_select_aid(&cmd_ptr->cmd.get_file_attr_req);
    break;

  case MMGSDI_SESSION_DEACTIVATE_REQ:
    mmgsdi_status = mmgsdi_session_select_aid(&cmd_ptr->cmd.get_file_attr_req);
    break;

  case MMGSDI_ISIM_AUTH_REQ:
    mmgsdi_status = mmgsdi_uim_uicc_isim_auth(&cmd_ptr->cmd.auth_req);
    break;

  default:
    mmgsdi_status = MMGSDI_ERROR;
    break;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_HIGH("ERROR cmd 0x%x failed with status %d",cmd, mmgsdi_status, 0);
  }
  return mmgsdi_status;
} /* mmgsdi_session_main */



/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN_RESPONSE

   DESCRIPTION:
     This function will handle the session management response processing.

   DEPENDENCIES:


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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main_response (
  mmgsdi_cmd_enum_type              cmd_type,
  const mmgsdi_uim_report_rsp_type * rsp_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI_SESSION_MAIN_RESPONSE", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    cmd_ptr                                - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(rsp_ptr);

  switch (cmd_type)
  {
  case MMGSDI_SESSION_OPEN_REQ:
    mmgsdi_status = mmgsdi_session_rsp_open(rsp_ptr);
    break;

  case MMGSDI_SESSION_SELECT_AID_REQ:
    mmgsdi_status = mmgsdi_session_rsp_select_aid(rsp_ptr);
    break;

  case MMGSDI_SESSION_DEACTIVATE_REQ:
    mmgsdi_status = mmgsdi_session_rsp_deactivate(rsp_ptr);
    break;

  case MMGSDI_SESSION_CLOSE_REQ:
    mmgsdi_status = mmgsdi_session_rsp_close(rsp_ptr);
    break;

  case MMGSDI_ISIM_AUTH_REQ:
    mmgsdi_status = mmgsdi_session_rsp_isim_auth(rsp_ptr);
    break;

  default:
    mmgsdi_status = MMGSDI_ERROR;
    break;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_HIGH("ERROR cmd 0x%x failed with status %d",cmd_type, mmgsdi_status, 0);
  }
  return mmgsdi_status;
} /* mmgsdi_session_main_response */
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
