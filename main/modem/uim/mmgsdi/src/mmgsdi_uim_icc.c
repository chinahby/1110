/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U I M    I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI
  before the command is being sent to UIM Module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uim_icc.c#4 $$ $DateTime: 2009/05/15 02:20:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/29/09   js      Fixed compiler warning
09/17/08   ssr     Fixed memory free error from mmgsdi_uim_uicc_search_record
08/20/08   jk      Support for Generate Key API & Fixes for CAVE
08/08/08   ssr	   Add more detailed messages for dispalyed in the log for 
				   mmgsdi icc search
07/31/08   ssr     Update search for blocking the search directions belonging 
                   to Record Pointer.
06/12/08   ssr     Support of MMGSDI Search functionality
06/20/08   sp      Add data offset messages for read
03/06/08   nk      Added synchronous support to mmgsdi_uim_icc_select and
                   mmgsdi_uim_icc_rehabilitate
01/10/08   nk      Fixed lint errors
12/17/07   vs      Featurized function mmgsdi_uim_icc_run_cave()
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access.  Add 
                   support for cyclic and linear fixed write
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
10/26/06   tml     Get Key ref from client ID
08/07/06   sun     Store the PIN on any pin operation
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/08/06   tml     fixed featurized
05/23/06   tml     rehabilitate and invalidate supports
02/14/06   nk      Merged for ARR caching and by path changes
01/17/05   tml     MMGSDI cleanup 
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Fix for not sending proper pointer to malloc function
08/30/05   jar     Added support for ICC Status Commands
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Channel ID support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   sk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "uim.h"
#include "mmgsdiutil.h"
#include "mmgsdi_icc_rsp.h"
#include "gsdi.h"

/* #ifdef FEATURE_MMGSDI_GSM */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_SELECT

   DESCRIPTION:
     This function will format the get file attribute request in the form
     required by the UIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). This function is also limited to the use of EFs.


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
mmgsdi_return_enum_type mmgsdi_uim_icc_select (
  const mmgsdi_get_file_attr_req_type   *req_ptr,
  boolean                                sync_process,
  mmgsdi_get_file_attr_cnf_type        **cnf_ptr
)
{
  uim_cmd_type                          * uim_cmd_ptr      = NULL;
  uim_slot_type                           uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type                 mmgsdi_status    = MMGSDI_SUCCESS;
  uim_items_enum_type                     uim_file_items   = UIM_NO_SUCH_ITEM;
  uim_file_type                           type_of_file     = UIM_EF;
  uim_dir_type                            directory        = 0;
  int32                                   index            = 0;
  mmgsdi_client_req_extra_info_type     * extra_param_ptr  = NULL;
  mmgsdi_int_app_info_type              * sel_app_ptr      = NULL;

  MSG_HIGH("MMGSDI UIM ICC SELECT, sync process is %d",sync_process,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  if(req_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    type_of_file = UIM_EF;
  }
  else if(req_ptr->access.access_method == MMGSDI_DF_ENUM_ACCESS)
  {
    type_of_file = UIM_DF;
  }
  else if(req_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS)
  {
    type_of_file = UIM_EF;
  }
  else 
  {
    MSG_ERROR("INVALID ACCESS METHOD %x",req_ptr->access.access_method,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  
  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM directory type */
  mmgsdi_status = mmgsdi_util_determine_uim_dir_type(&(req_ptr->access),
                                                     &directory);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_GET_FILE_ATTR_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command      = UIM_CACHED_SEL_F; 
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MSG_ERROR("INVALID PROTOCOL %x",sel_app_ptr->app_data.app_type,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  uim_cmd_ptr->select.file_type = type_of_file;
  uim_cmd_ptr->select.item      = uim_file_items;
  uim_cmd_ptr->select.dir       = directory;
  uim_cmd_ptr->hdr.slot         = uim_slot;
  uim_cmd_ptr->hdr.channel      = UIM_CHANNEL0;

  mmgsdi_status = mmgsdi_util_convert_to_uim_path(
                    uim_file_items,
                    &(req_ptr->access),
                    NULL,
                    &uim_cmd_ptr->select.item,
                    uim_cmd_ptr->select.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* This is a GSM select, so nil-out ADF specific parameters */
  uim_cmd_ptr->select.aid.aid_length = 0;

  if (req_ptr->activate_aid)
  {
    uim_cmd_ptr->select.session = UIM_ACTIVATION_OR_RESET;
  }
  else
  {
    uim_cmd_ptr->select.session = UIM_TERMINATION;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been 
                                    populated yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been 
                                    populated yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down SELECT command to UIM %d",
           req_ptr->access.file.file_enum,0,0);

  MSG_HIGH("User Data in mmgsdi_uim_icc.c select 0x%x", 
    uim_cmd_ptr->hdr.user_data, 0, 0);

  if (sync_process) 
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_get_file_attr(
               mmgsdi_internal_synch_uim_rpt_rsp, cnf_ptr, TRUE);
        if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
        }
      }
      else
      {
        MSG_ERROR("mmgsdi client request table index = 0x%x!", index, 0, 0);
        MSG_ERROR("File Access: 0x%x", req_ptr->access.file.file_enum, 0, 0);
        MSG_ERROR("mmgsdi_internal_synch_uim_rpt_rsp is null in synch", 0, 0, 0);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;

} /* mmgsdi_uim_icc_select */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This function package the Write Transparent command and send it to 
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_transparent (
  const mmgsdi_write_req_type *  req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  uim_items_enum_type                uim_file_items   = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_WRITE_TRANSPARENT", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (req_ptr->data.data_len <= 0)
    return MMGSDI_ERROR;

  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_WRITE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_WRITE_BIN_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) offset: based on file type, either record number or offset will be sent
    6) data len and data
    7) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_WRITE_BIN_CMD_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->write_bin.item      = uim_file_items;
  uim_cmd_ptr->write_bin.offset    = int32touint16(req_ptr->offset);
  uim_cmd_ptr->write_bin.len       = int32touint16(req_ptr->data.data_len);
  uim_cmd_ptr->write_bin.data      = extra_param_ptr->write_data.data.data_ptr;

  mmgsdi_status = mmgsdi_util_convert_to_uim_path(
                    uim_file_items,
                    &(req_ptr->access),
                    NULL,
                    &uim_cmd_ptr->write_bin.item,
                    uim_cmd_ptr->write_bin.path);
                    
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer write data (Since client table has not been 
                                    populated yet)
    2) Release extra param pointer (Since client table has not been populated 
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    4) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer write data (Since client table has not been 
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated 
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    5) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down WRITE command to UIM %d",
           req_ptr->access.file.file_enum,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  
  return mmgsdi_status;
} /* mmgsdi_uim_icc_write_transparent */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_RECORD

   DESCRIPTION:
     This function package the Write Record command and send it to 
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_record (
  const mmgsdi_write_req_type *  req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  uim_items_enum_type                uim_file_items   = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif  
  MSG_MED("MMGSDI_UIM_ICC_WRITE_RECORD", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (req_ptr->rec_num > MMGSDI_MAX_REC_NUM)
    return MMGSDI_ERROR;

  if (req_ptr->data.data_len <= 0)
    return MMGSDI_ERROR;

  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_WRITE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_WRITE_REC_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) offset: based on file type, either record number or offset will be sent
    6) data len and data
    7) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_WRITE_REC_CMD_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->write_rec.item      = uim_file_items;
  uim_cmd_ptr->write_rec.rec_num   = int32touint8(req_ptr->rec_num);
  if (req_ptr->file_type == MMGSDI_CYCLIC_FILE)
  {
    uim_cmd_ptr->write_rec.recmode   = UIM_PREVIOUS;
  }
  else
  {
    uim_cmd_ptr->write_rec.recmode   = UIM_ABSOLUTE;
  }
  uim_cmd_ptr->write_rec.len       = int32touint16(req_ptr->data.data_len);
  uim_cmd_ptr->write_rec.data      = extra_param_ptr->write_data.data.data_ptr;

  mmgsdi_status = mmgsdi_util_convert_to_uim_path(
                    uim_file_items,
                    &(req_ptr->access),
                    NULL,
                    &uim_cmd_ptr->write_rec.item,
                    uim_cmd_ptr->write_rec.path);
                    
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer write data (Since client table has not been 
                                    populated yet)
    2) Release extra param pointer (Since client table has not been populated 
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    4) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer write data (Since client table has not been 
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated 
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    5) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down WRITE command to UIM %d",
           req_ptr->access.file.file_enum,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  return mmgsdi_status;
} /* mmgsdi_uim_icc_write_record */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_READ_TRANSPARENT

   DESCRIPTION:
     This function package the Read Transparent command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_uim_icc_read_transparent (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_ptr
)
{
  uim_cmd_type *                        uim_cmd_ptr      = NULL;
  uim_slot_type                         uim_slot         = UIM_SLOT_NONE;
  uim_items_enum_type                   uim_file_items   = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  int32                                 index            = 0;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_READ_TRANSPARENT", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (sync_process)
  {
    MMGSDIUTIL_RETURN_IF_NULL(cnf_ptr);
  }

  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_READ_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
    
  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_READ_BIN_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) data len
    6) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_READ_BIN_CMD_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->read_bin.item       = uim_file_items;

  mmgsdi_status = mmgsdi_util_convert_to_uim_path(
                    uim_file_items,
                    &(req_ptr->access),
                    NULL,
                    &uim_cmd_ptr->read_bin.item,
                    uim_cmd_ptr->read_bin.path);
                    
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  uim_cmd_ptr->read_bin.offset     = int32touint16(req_ptr->offset);
  uim_cmd_ptr->read_bin.len        = int32touint16(req_ptr->data_len);

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated 
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    4) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down READ command to UIM %d with file offset %d and length %d",
           req_ptr->access.file.file_enum,
           uim_cmd_ptr->read_bin.offset,
           uim_cmd_ptr->read_bin.len);

  if (sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_read(
          mmgsdi_internal_synch_uim_rpt_rsp, cnf_ptr, sync_process);

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
      }
      else
      {
        MSG_ERROR("gsdi client request table index = 0x%x!", index, 0, 0);
        MSG_ERROR("File Access: 0x%x", req_ptr->access.file.file_enum, 0, 0);
        MSG_ERROR("mmgsdi_internal_synch_uim_rpt_rsp is null in synch", 0, 0, 0);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }

  return mmgsdi_status;

} /* mmgsdi_uim_icc_read_transparent */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_READ_RECORD

   DESCRIPTION:
     This function package the Read Record command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_record (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_ptr
)
{
  uim_cmd_type *                        uim_cmd_ptr      = NULL;
  uim_slot_type                         uim_slot         = UIM_SLOT_NONE;
  uim_items_enum_type                   uim_file_items   = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  int32                                 index            = 0;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_READ_RECORD", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (sync_process) 
  {
    MMGSDIUTIL_RETURN_IF_NULL(cnf_ptr);
  }
  if (req_ptr->rec_num > MMGSDI_MAX_REC_NUM)
  {
    return MMGSDI_ERROR;
  }

  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_READ_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
    
  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_READ_REC_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) data len
    6) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_READ_REC_CMD_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->read_rec.item       = uim_file_items;
  uim_cmd_ptr->read_rec.rec_num    = int32touint8(req_ptr->rec_num);
  uim_cmd_ptr->read_rec.recmode    = UIM_ABSOLUTE;

  mmgsdi_status = mmgsdi_util_convert_to_uim_path(
                    uim_file_items,
                    &(req_ptr->access),
                    NULL,
                    &uim_cmd_ptr->read_rec.item,
                    uim_cmd_ptr->read_rec.path);
                    
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated 
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    4) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down READ command to UIM %d record no %d",
           req_ptr->access.file.file_enum, 
           uim_cmd_ptr->read_rec.rec_num, 
           0);

  if (sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_read(
          mmgsdi_internal_synch_uim_rpt_rsp, cnf_ptr, sync_process);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
      }
      else
      {
        MSG_ERROR("gsdi client request table index = 0x%x!", index, 0, 0);
        MSG_ERROR("File Access: 0x%x", req_ptr->access.file.file_enum, 0, 0);
        MSG_ERROR("mmgsdi_internal_synch_uim_rpt_rsp is null in synch", 0, 0, 0);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }

  return mmgsdi_status;

} /* mmgsdi_uim_icc_read_record */

/*===========================================================================
FUNCTION:  MMGSDI_UIM_ICC_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_protocol_enum_type          protocol        = MMGSDI_NO_PROTOCOL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  int32                              index           = 0;
  boolean                            param_valid     = TRUE;

  MSG_MED("MMGSDI_UIM_ICC_SEND_APDU", 0, 0, 0);
  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);
  /*
   * Get the protocol.
   */
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
    return mmgsdi_status;
  }
  /*
   * Convert to UIM slot.
   */
  mmgsdi_status =
    mmgsdi_util_convert_uim_slot(
      req_ptr->request_header.slot_id, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * Allocate and populate extra parameter.
   */
  mmgsdi_status =
    mmgsdi_util_alloc_and_populate_extra_param(
      req_ptr->request_header.request_type, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * 1) Get a UIM buffer for the request.
   * 2) Check for if the buffer is NULL or not.
   */
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }
  
  param_valid &=
    mmgsdi_chnl_mgt_is_slot_valid(req_ptr->request_header.slot_id);
  param_valid &=
    mmgsdi_chnl_mgt_is_channel_id_valid(req_ptr->channel_id);
  param_valid &=
    mmgsdi_chnl_mgt_is_data_len_valid( req_ptr->data.data_len);
  param_valid &=
    mmgsdi_chnl_mgt_is_data_buffer_valid( req_ptr->data.data_ptr);

  if (param_valid == FALSE) 
  {
    return MMGSDI_ERROR;
  }
  /*
    * Populate UIM information.
    */
  uim_cmd_ptr->hdr.command  = UIM_STREAM_ISO7816_APDU_F;
  uim_cmd_ptr->hdr.protocol = gsdi_get_uim_protocol();
  uim_cmd_ptr->hdr.slot     = uim_slot;
  /*
    * Get a new index from client req info table for response data.
    * Set the uim data pointer to the client request table data buffer.
    */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status =
    mmgsdi_util_populate_client_request_table_info(
      index,
      &req_ptr->request_header,
      uim_cmd_ptr,
      extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /*
    * Set UIM Stream APDU IS07816 Channel Command Options.
    */
  if ( req_ptr->data.data_len > GSDI_MAX_CHANNEL_DATA_CMD_LEN ) {
    req_ptr->data.data_len = GSDI_MAX_CHANNEL_DATA_CMD_LEN;
  }
  uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes =
    int32touint16(req_ptr->data.data_len);
  memcpy(
    uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
    req_ptr->data.data_ptr,
    int32touint32(req_ptr->data.data_len));
  /*
    * Assign uim_cmd_ptr's user data.
    */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);
  MSG_HIGH("Sending down send APDU command to UIM", 0, 0, 0);
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    MSG_ERROR("send cmd to server failed (%d)", mmgsdi_status, 0, 0);
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /*lint !e715 */

/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_STATUS

   DESCRIPTION
     This function is called to Queue the command to the UIM for
     a Status command.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_status(
  const mmgsdi_status_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_HIGH("MMGSDI UIM UICC STATUS",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
    2. me_app_status    - Must be 0 if not overwrite
    3. ret_data_struct  - Must be 0 if not overwrite
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (req_ptr->me_app_status   != MMGSDI_STATUS_APP_NO_INDICATION &&
      req_ptr->ret_data_struct != MMGSDI_STATUS_NONE)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("APP STATUS 0x%x or DATA STRUCT INFO 0x%x",
                               req_ptr->me_app_status,
                               req_ptr->ret_data_struct,0);
    return MMGSDI_ERROR;
  }

  /* ---------------------------------------------------------------------------
     Convert the slot from an MMGSDI SLOT ID to a UIM SLOT ID
     -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("BAD SLOT ID: 0x%x",
                               req_ptr->request_header.slot_id,0,0);
    return mmgsdi_status;
  }

  /* ---------------------------------------------------------------------------
     Get a UIM buffer for the request
     -------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr,
    sizeof(uim_cmd_type),
    mmgsdi_status);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Not much we can do...could not get memory */
    return mmgsdi_status;
  }

  /* ----------------------------------------------------------------------------
     Set the uim_cmd_ptr paramaters accordingly
     --------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command        = UIM_STATUS_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot           = uim_slot;

  /* ----------------------------------------------------------------------------
     Set the UIM Status Status accordingly
     --------------------------------------------------------------------------*/
  uim_cmd_ptr->status.status      = UIM_NO_INDICATION;

  /* -----------------------------------------------------------------------------
     Indicate to the card the expected return Data Structure.
     ---------------------------------------------------------------------------*/
  uim_cmd_ptr->status.return_data = UIM_RETURN_NONE;

  /* ---------------------------------------------------------------------------
    CLIENT REQUEST TABLE :- Store information
    1) Get the index
    2) If not able to get the index, free the uim_cmd_q, but keep the request
       ptr and the uim_rsp_data buffer so that it can be freed in the
       mmgsdi_uicc_rsp.c
    3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
    ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Provide to the UIM the client ID Entry to be returned in the Status
     Report from the UIM Task
     ------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("Queue cmd to UIM Failure",0,0,0);
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* --------------------------------------------------------------------------
     The memory allocated for the UIM will not be freed until 
     processed by MMGSDI Task Process Response
     ------------------------------------------------------------------------*/

  return mmgsdi_status;

} /* mmgsdi_uim_icc_status */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_REHABILITATE

   DESCRIPTION:
     This function will rehabilitate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). 

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the rehabilitate
                              or invalidate the BDN file
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_rehabilitate (
  const mmgsdi_rehabilitate_req_type     *req_ptr,
  boolean                                sync_process,
  mmgsdi_rehabilitate_cnf_type           **cnf_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  uim_items_enum_type                uim_file_items   = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_HIGH("MMGSDI_UIM_ICC_REHABILITATE, sync_process: %d", sync_process, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  
  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_REHABILITATE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    //Gemsea Remove MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_REHABILITATE_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_REHABILITATE_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->rehab.item          = uim_file_items;
  
  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 (mmgsdi_request_header_type*)&req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated 
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    4) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down REHABILITATE command to UIM %d",
           req_ptr->access.file.file_enum,0,0);

  if (sync_process) 
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process) 
    {
      if (mmgsdi_internal_synch_uim_rpt_rsp != NULL) 
      {
        mmgsdi_internal_synch_uim_rpt_rsp->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_rehabitate(
               mmgsdi_internal_synch_uim_rpt_rsp, cnf_ptr, TRUE);
        if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
        } /* free mmgsdi_internal_synch_uim_rpt_rsp */
      } /* if mmgsdi_internal_synch_uim_rpt_rsp != NULL*/
      else
      {
        MSG_ERROR("gsdi client request table index = 0x%x!", index, 0, 0);
        MSG_ERROR("File Access: 0x%x", req_ptr->access.file.file_enum, 0, 0);
        MSG_ERROR("mmgsdi_internal_synch_uim_rpt_rsp is null in synch", 0, 0, 0);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    } /* if synch_process */

  } /* else mmgsdi_status == SUCCESS cmd to uim*/
  
  return mmgsdi_status;

} /* mmgsdi_uim_icc_rehabilitate */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_INVALIDATE

   DESCRIPTION:
     This function will invalidate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). 

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the invalidate 
                              command
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_invalidate (
  const mmgsdi_invalidate_req_type *  req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  uim_items_enum_type                uim_file_items   = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_INVALIDATE", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  
  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_INVALIDATE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    // Gemsea Remove MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = MMGSDI_INVALIDATE_REQ
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_INVALIDATE_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->invalidate.item     = uim_file_items;
  
  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 (mmgsdi_request_header_type*)&req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated 
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    4) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down INVALIDATE command to UIM %d",
           req_ptr->access.file.file_enum,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  
  return mmgsdi_status;

} /* mmgsdi_uim_icc_invalidate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_VERIFY_PIN

   DESCRIPTION:
     This function packages the VERIFY PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_verify_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_verify_pin (
  const mmgsdi_pin_operation_req_type       * req_ptr,
  mmgsdi_num_retry_enum_type                  retry_type
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_VERIFY_PIN", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->pin_puk.data_ptr);

  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN) 
  {
     MSG_ERROR("Invalid PIN Length, Not sending the Command down",0,0,0);
     return MMGSDI_INCORRECT_PARAMS;
  }
  
  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  extra_param_ptr->pin_data.retry_type = retry_type;

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS) 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  (void) memcpy(uim_cmd_ptr->verify_chv.chv_digits,
                req_ptr->pin_puk.data_ptr,
                int32touint32(req_ptr->pin_puk.data_len));

   /*Store the PIN in the table*/
   mmgsdi_util_store_pin(req_ptr->pin_id, req_ptr->pin_puk );

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_VERIFY_CHV_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) verify_chv
    5) data len and data
    6) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_VERIFY_CHV_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
    
  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.client_id,
                                          req_ptr->pin_id,
                                         &uim_cmd_ptr->verify_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

 /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been 
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated 
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    5) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down VERIFY CHV command to UIM ",
           0,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  return mmgsdi_status;
} /* mmgsdi_uim_icc_verify_pin */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_ENABLE_PIN
   DESCRIPTION:
     This function packages the ENABLE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_enable_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_enable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_ENABLE_PIN", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN)
  {
     MSG_ERROR("ENABLE_PIN_REQ - PIN LENGTH invalid",0,0,0);
     return MMGSDI_INCORRECT_PARAMS;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS) 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_ENABLE_CHV_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_ENABLE_CHV_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  
  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.client_id,
                                          req_ptr->pin_id,
                                         &uim_cmd_ptr->enable_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
 
  (void) memcpy(uim_cmd_ptr->enable_chv.chv_digits,
                req_ptr->pin_puk.data_ptr,
                int32touint32(req_ptr->pin_puk.data_len));

   /*Store the PIN in the table*/
   mmgsdi_util_store_pin(req_ptr->pin_id, req_ptr->pin_puk );

   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been 
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated 
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    5) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down ENABLE CHV command to UIM ",
           0,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  
  return mmgsdi_status;

} /* mmgsdi_uim_icc_enable_pin */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_DISABLE_PIN
   DESCRIPTION:
     This function packages the DISABLE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_disable_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_disable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_DISABLE_PIN", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  
  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN)
  {
     MSG_ERROR("DISABLE_PIN_REQ - PIN invalid",0,0,0);
     return MMGSDI_INCORRECT_PARAMS;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS) 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_DISABLE_CHV_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_DISABLE_CHV_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  
  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.client_id,
                                          req_ptr->pin_id,
                                         &uim_cmd_ptr->disable_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  (void) memcpy(uim_cmd_ptr->disable_chv.chv_digits,
                req_ptr->pin_puk.data_ptr,
                int32touint32(req_ptr->pin_puk.data_len));
 
  /*Store the PIN in the table*/
   mmgsdi_util_store_pin(req_ptr->pin_id, req_ptr->pin_puk );
  
 /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been 
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated 
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    5) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down DISABLE CHV command to UIM ",
           0,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  
  return mmgsdi_status;

} /* mmgsdi_uim_icc_disable_pin */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_CHANGE_PIN
   DESCRIPTION:
     This function packages the CHANGE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_change_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_change_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_CHANGE_PIN", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN ||
      req_ptr->new_pin.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->new_pin.data_len > MMGSDI_PIN_MAX_LEN) 
  {
     MSG_ERROR("CHANGE_PIN_REQ - PIN invalid",0,0,0);
     return MMGSDI_INCORRECT_PARAMS;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS) 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_CHANGE_CHV_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_CHANGE_CHV_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  
  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.client_id,
                                          req_ptr->pin_id,
                                         &uim_cmd_ptr->change_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  (void) memcpy(uim_cmd_ptr->change_chv.chv_digits,
                req_ptr->pin_puk.data_ptr,
                int32touint32(req_ptr->pin_puk.data_len));

  (void) memcpy(uim_cmd_ptr->change_chv.new_chv_digits,
                req_ptr->new_pin.data_ptr,
                int32touint32(req_ptr->new_pin.data_len));

  /*Store the PIN in the table*/
   mmgsdi_util_store_pin(req_ptr->pin_id, req_ptr->new_pin );

   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been 
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated 
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    5) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down CHANGE CHV command to UIM",
           0,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  
  return mmgsdi_status;

} /* mmgsdi_uim_icc_change_pin */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_UNBLOCK_PIN
   DESCRIPTION:
     This function packages the UNBLOCK PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_unblock_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_unblock_pin (
  const mmgsdi_pin_operation_req_type       * req_ptr,
  mmgsdi_num_retry_enum_type                  retry_type
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif
  MSG_MED("MMGSDI_UIM_ICC_UNBLOCK_PIN", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN ||
      req_ptr->new_pin.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->new_pin.data_len > MMGSDI_PIN_MAX_LEN)
  {
     MSG_ERROR("UNBLOCK_PIN_REQ - PIN invalid",0,0,0);
     return MMGSDI_INCORRECT_PARAMS;
  }
  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  extra_param_ptr->pin_data.retry_type = retry_type;

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS) 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_UNBLOCK_CHV_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_UNBLOCK_CHV_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot            = uim_slot;
  
  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.client_id,
                                          req_ptr->pin_id,
                                         &uim_cmd_ptr->unblk_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  (void) memcpy(uim_cmd_ptr->unblk_chv.chv_digits,
                req_ptr->pin_puk.data_ptr,
                int32touint32(req_ptr->pin_puk.data_len));

  (void) memcpy(uim_cmd_ptr->unblk_chv.new_chv_digits,
                req_ptr->new_pin.data_ptr,
                int32touint32(req_ptr->new_pin.data_len));

  /*Store the PIN in the table*/
   mmgsdi_util_store_pin(req_ptr->pin_id, req_ptr->new_pin );
 
   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    3) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been 
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated 
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    5) Release uim rsp data pointer  (Since client table has not been 
                                      populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  extra_param_ptr->pin_data.retry_type = retry_type;

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  MSG_HIGH("Sending down UNBLOCK CHV command to UIM ",
           0,0,0);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the 
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("send cmd to UIM fail table index = 0x%x!", index, 0, 0);
  }
  
  return mmgsdi_status;

} /* mmgsdi_uim_icc_unblock_pin */


/* #endif */
/*FEATURE_MMGSDI_GSM*/


#ifdef FEATURE_MMGSDI_CDMA

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_COMPUTE_IP_AUTH

   DESCRIPTION
     This function is called for a Compute IP Authentication request.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_compute_ip_auth(
  const mmgsdi_compute_ip_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
    
  MSG_HIGH("MMGSDI UIM ICC COMPUTE IP",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Invalid Slot %x",req_ptr->request_header.slot_id,0,0);
    return mmgsdi_status;
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(uim_cmd_ptr, sizeof(uim_cmd_type));
  uim_cmd_ptr->hdr.command               = UIM_COMPUTE_IP_AUTH_F;
  uim_cmd_ptr->hdr.protocol              = UIM_CDMA;
  uim_cmd_ptr->hdr.slot                  = uim_slot;

  /*---------------------------------------------------------------------------
  CLIENT REQUEST TABLE :- Store information
  1) Get the index
  2) If not able to get the index, free the uim_cmd_q, but keep the request
  ptr and the uim_rsp_data buffer so that it can be freed in the
  mmgsdi_uicc_rsp.c
  3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("No Client Request Entry Available",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* ------------------------------------------------------------------------
      Set COMPUTE IP Authentication data
    ----------------------------------------------------------------------*/
  switch(req_ptr->compute_ip_data.operation_type)
  {
#ifdef FEATURE_UIM_SUPPORT_3GPD
    case MMGSDI_CMPT_IP_SIP_CHAP:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_CHAP;
      memcpy(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge,
             req_ptr->compute_ip_data.data.sip_chap_data.chap_challenge,
             req_ptr->compute_ip_data.data.sip_chap_data.chap_challenge_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge_length =
        req_ptr->compute_ip_data.data.sip_chap_data.chap_challenge_length;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_id =
        req_ptr->compute_ip_data.data.sip_chap_data.chap_id;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.nai_entry_index =
        req_ptr->compute_ip_data.data.sip_chap_data.nai_entry_index;
      /* Print the data being sent to UIM out onto diag */
      mmgsdi_util_print_data_to_diag("SIP CHAP CHALLENGE : ",
       uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge,
       uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge_length);
      break;
    case MMGSDI_CMPT_IP_MIP_MN_HA:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MN_HA_AUTH;
      memcpy(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data,
             req_ptr->compute_ip_data.data.mn_ha_data.registration_data_ptr,
             req_ptr->compute_ip_data.data.mn_ha_data.registration_data_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.nai_entry_index = 
        req_ptr->compute_ip_data.data.mn_ha_data.nai_entry_index;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data_length = 
        req_ptr->compute_ip_data.data.mn_ha_data.registration_data_length;
      /* Print the data being sent to UIM out onto diag */
      mmgsdi_util_print_data_to_diag("MIP MN-HA : ",
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data,
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data_length);
      break;
    case MMGSDI_CMPT_IP_MIP_RRQ:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MIP_RRQ;
      memcpy(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
             req_ptr->compute_ip_data.data.rrq_data.rrq_data_ptr,
             req_ptr->compute_ip_data.data.rrq_data.rrq_data_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data_length = 
        req_ptr->compute_ip_data.data.rrq_data.rrq_data_length;
      /* Print the data being sent to UIM out onto diag */
      mmgsdi_util_print_data_to_diag("MIP-RRQ HASH : ",
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data_length);
      break;
    case MMGSDI_CMPT_IP_MIP_MN_AAA:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MN_AAA_AUTH;
      memcpy(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge,
             req_ptr->compute_ip_data.data.mn_aaa_data.challenge_ptr,
             req_ptr->compute_ip_data.data.mn_aaa_data.challenge_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge_length = 
        req_ptr->compute_ip_data.data.mn_aaa_data.challenge_length;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.nai_entry_index = 
        req_ptr->compute_ip_data.data.mn_aaa_data.nai_entry_index;
      /* Print the data being sent to UIM out onto diag */
      mmgsdi_util_print_data_to_diag("MIP MN-AAA : ",
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge,
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge_length);
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
    case MMGSDI_CMPT_IP_HRPD_CHAP:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_HRPD_AN_CHAP;
      memcpy(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge,
             req_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
             req_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge_length = 
        req_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_length;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_id = 
        req_ptr->compute_ip_data.data.hrpd_chap_data.chap_id;
      /* Print the data being sent to UIM out onto diag */
      mmgsdi_util_print_data_to_diag("MIP MN-AAA : ",
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge,
           uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge_length);
      break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not Queue Compute IP Command to UIM",0,0,0);
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
}
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */

#ifdef FEATURE_AUTH
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_RUN_CAVE

   DESCRIPTION
     This function is called for a RUN CAVE ALGORITHM request.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_run_cave(
  const mmgsdi_run_cave_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  mmgsdi_cave_esn_type*      temp_esn_ptr     = NULL;
    
  MSG_HIGH("MMGSDI UIM ICC RUN CAVE",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Invalid Slot %x",req_ptr->request_header.slot_id,0,0);
    return mmgsdi_status;
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(uim_cmd_ptr, sizeof(uim_cmd_type));
   
  uim_cmd_ptr->hdr.command               = UIM_RUN_CAVE_F;
  uim_cmd_ptr->hdr.protocol              = UIM_CDMA;
  uim_cmd_ptr->hdr.slot                  = uim_slot;

  /* ------------------------------------------------------------------------
      Set COMPUTE IP Authentication data
    ----------------------------------------------------------------------*/

  uim_cmd_ptr->run_cave.dig_len             = req_ptr->dig_len;
  /* req_ptr->digits is a an array of uint8s */
  memcpy(uim_cmd_ptr->run_cave.digits, req_ptr->digits, sizeof(mmgsdi_cave_digits_type));
  /* Need to move data to temp variable since qw_esn can't take const variable */
  temp_esn_ptr = (mmgsdi_cave_esn_type*)&(req_ptr->esn);
  qw_equ(uim_cmd_ptr->run_cave.esn, (*temp_esn_ptr));
  uim_cmd_ptr->run_cave.process_control     = req_ptr->process_control;
  uim_cmd_ptr->run_cave.rand_chal           = req_ptr->rand_chal;
  uim_cmd_ptr->run_cave.rand_type           = req_ptr->rand_type;

  /*---------------------------------------------------------------------------
  CLIENT REQUEST TABLE :- Store information
  1) Get the index
  2) If not able to get the index, free the uim_cmd_q, but keep the request
  ptr and the uim_rsp_data buffer so that it can be freed in the
  mmgsdi_uicc_rsp.c
  3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("No Client Request Entry Available",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not Queue Compute IP Command to UIM",0,0,0);
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
} /* mmgsdi_uim_icc_run_cave */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_GENERATE_KEY_VPM

   DESCRIPTION
     This function is called for a GENERATE KEY VPM request.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_generate_key_vpm(
  const mmgsdi_generate_key_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
    
  MSG_HIGH("MMGSDI UIM ICC GENERATE KEY VPM",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Invalid Slot 0x%x",req_ptr->request_header.slot_id,0,0);
    return mmgsdi_status;
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(uim_cmd_ptr, sizeof(uim_cmd_type));
   
  uim_cmd_ptr->hdr.command               = UIM_GENERATE_KEYS_F;
  uim_cmd_ptr->hdr.protocol              = UIM_CDMA;
  uim_cmd_ptr->hdr.slot                  = uim_slot;

  /* ------------------------------------------------------------------------
      Set Generate Key / VPM data
    ----------------------------------------------------------------------*/

  uim_cmd_ptr->gen_keys.vpm_first_octet    = req_ptr->first_octet;
  uim_cmd_ptr->gen_keys.vpm_last_octet     = req_ptr->last_octet;


  /*---------------------------------------------------------------------------
  CLIENT REQUEST TABLE :- Store information
  1) Get the index
  2) If not able to get the index, free the uim_cmd_q, but keep the request
  ptr and the uim_rsp_data buffer so that it can be freed in the
  mmgsdi_uicc_rsp.c
  3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("No Client Request Entry Available",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not Queue Generate Key VPM Command to UIM",0,0,0);
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
} /* mmgsdi_uim_icc_generate_key_vpm */
#endif /* FEATURE_AUTH */

#endif /* FEATURE_MMGSDI_CDMA */


/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_SEARCH_RECORD

   DESCRIPTION:
     This function package the Search Record command and send it to UIM 
     for further processing

   DEPENDENCIES:

   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_search_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.
     MMGSDI_NOT_SUPPORTED    : When Search Type is not valid for ICC Card.

   SIDE EFFECTS:

----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_search_record (
  const mmgsdi_search_req_type             * req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  uim_items_enum_type                uim_file_items   = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  int32                              index            = 0;
#ifdef CUST_EDITION
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);
#endif

  MSG_HIGH("MMGSDI_UIM_ICC_SEARCH_RECORD", 0, 0, 0);

  /*Checking of Input Parameter*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM items enum */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&(req_ptr->access),
                                                       &uim_file_items,
                                                       FALSE);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Invalid file access in UIM ICC Search ",0,0,0);
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Invalid conversion of uim slot in UIM ICC Search 0x%x", 
               req_ptr->request_header.slot_id ,0,0);
    return mmgsdi_status;
  }
  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_SEARCH_REQ, 
                    (void*)req_ptr, 
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Unable to allocate memory to extra param in UIM ICC Search",
			  0,0,0);
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /*-------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  --------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, 
                                         sizeof(uim_cmd_type),
                                         mmgsdi_status);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (uim_cmd_ptr == NULL))
  {
    MSG_ERROR("Memory Allcoation for uim cmd is failed in UIM ICC Search",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);

   /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /*-------------------------------------------------
  hdr                            : Command header 
   cmd_hdr                       : command header
    command                      : The command 
    status                       : Status of the R-UIM 
    options                      : Indicate command handling options 
    protocol                     : Protocol of the command 
    slot                         : Slot to execute the command 
    uim_rpt_type                 : Pointer to Function to send report 
    cmd_count                    :  
    user_data                    : User defined data
    channel                      : 
  item                           : EF to be searched  
  path[UIM_MAX_PATH_ELEMENTS - 1]: Path for accessing an item 
  srch_pattern[UIM_MAX_CHARS ]   : Search string 
  num_bytes                      : Number of bytes in search pattern  
  seek_type                      : Type 1 or Type 2 Seek 
  mode                           : Search direction for seek 
  aid                            : AID for USIM 
  srch_mode                      : Search Type for UICC Search 
  rec_number                     : Record Number for UICC Search 
  srch_dir                       : Search Direction for UICC Search 
  offset_type                    : offset as absolute position or character 
                                   For Enhance UICC Search
  offset                         : One Byte Value for UICC Search 
  srch_rec_mode_type             : Search Direction for UICC Search 
 ----------------------------------------------------*/
  uim_cmd_ptr->hdr.command             = UIM_SEEK_F;
#ifdef CUST_EDITION
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_GSM;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    uim_cmd_ptr->hdr.protocol     = UIM_CDMA;
  }
  else 
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }
#else
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
#endif
  uim_cmd_ptr->hdr.slot                = uim_slot;
  uim_cmd_ptr->hdr.channel             = UIM_CHANNEL0;

  /* Updating item:  EF to be searched */
  uim_cmd_ptr->seek.item               = uim_file_items;

  /* Updating seek_type */
  uim_cmd_ptr->seek.seek_type          = UIM_SEEK_TYPE_2;
  
  /* Updating mode */
  switch(req_ptr->seek_direction)
  {
    case MMGSDI_SEEK_BEGINNING_FORWARDS:
      uim_cmd_ptr->seek.mode           = UIM_SEEK_BEGINNING_FORWARDS;
      MSG_HIGH("ICC SEARCH DIRECTION: MMGSDI_SEEK_BEGINNING_FORWARDS",0,0,0);
      break;

    case MMGSDI_SEEK_END_BACKWARDS:
      uim_cmd_ptr->seek.mode           = UIM_SEEK_END_BACKWARDS;
      MSG_HIGH("ICC SEARCH DIRECTION: MMGSDI_SEEK_END_BACKWARDS",0,0,0);
      break;
     
    default:
      MSG_HIGH("MMGSDI SEEK DIRECTION IS NOT SUPPORTED in UIM ICC Search  0x%x",
                req_ptr->seek_direction,0,0);
      return MMGSDI_INCORRECT_PARAMS;
  }/* end of switch(req_ptr->seek_direction)*/
  
  /* Updating rec_number: Not useful for seek command */
  uim_cmd_ptr->seek.rec_number         = 0x00;

  /* Updating srch_mode: Not useful for seek command */
  uim_cmd_ptr->seek.srch_mode          = UIM_NO_SUCH_SRCH_MODE;

  /* Updating srch_dir: Not useful for seek command */
  uim_cmd_ptr->seek.srch_dir           = UIM_SRCH_FORWARD;

  /* Updating offset_type: Not used for seek command */
  uim_cmd_ptr->seek.offset_type        = UIM_SRCH_ABSOLUTE;

  /*Updating offset: Not used for seek command */
  uim_cmd_ptr->seek.offset             = 0x00;

  /* Updating srch_rec_mode_type: Not used for seek command */
  uim_cmd_ptr->seek.srch_rec_mode_type = UIM_SRCH_P1_FORWARD;

  /* Updating num_bytes: Number of bytes in search pattern */
  if (req_ptr->data.data_len > MMGSDI_MAX_SEEK_PATTERN_LENGTH ) 
  {
    uim_cmd_ptr->seek.num_bytes        = MMGSDI_MAX_SEEK_PATTERN_LENGTH;
    MSG_HIGH("Change seek data length 0x%x to MMGSDI_MAX_SEEK_PATTERN_LENGTH[0x10]",
              req_ptr->data.data_len,0,0);
    /* Maximum Length is bound to 16 byte */

  }
  else
  {
    uim_cmd_ptr->seek.num_bytes        = (byte)req_ptr->data.data_len;
  }
  
  /* Update srch_pattern[UIM_MAX_CHARS ]; Search string */
  memcpy((byte *)uim_cmd_ptr->seek.srch_pattern,
         (byte *)req_ptr->data.data_ptr,
         int32touint32(uim_cmd_ptr->seek.num_bytes));

  /* Print Search Pattern Length and Pattern */
  MSG_HIGH(" ICC SEARCH PATTERN LENGTH: 0x%x",uim_cmd_ptr->seek.num_bytes,0,0);
  mmgsdi_util_print_data_to_diag(" UICC SEARCH PATTERN:0x",
								   uim_cmd_ptr->seek.srch_pattern,
								   uim_cmd_ptr->seek.num_bytes);

  /*Updating path[UIM_MAX_PATH_ELEMENTS - 1]: Path for accessing an item */
  mmgsdi_status = mmgsdi_util_convert_to_uim_path(uim_file_items,
                                                  &(req_ptr->access),
                                                  NULL,
                                                  &uim_cmd_ptr->seek.item,
                                                  uim_cmd_ptr->seek.path);
                    
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    MSG_ERROR("Convesion to UIM path is failed in UIM ICC Search ",0,0,0);
    
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table 
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated 
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    MSG_ERROR(" Finding free index in client request table is failed in UIM ICC Search",
                0,0,0);
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }/* End of if (mmgsdi_status != MMGSDI_SUCCESS)*/

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    uim_cmd_ptr,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been 
                                    populated yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);

    MSG_ERROR("Populating information from client request table is failed in UIM ICC Search",
               0,0,0);
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  /*Printing File Information */
  mmgsdi_util_print_file_information(req_ptr->access,FALSE);
  MSG_HIGH("Sending down seek command to UIM",0,0,0);
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) No need to release extra param pointer, It is already removed thru client Table
    3) No need to release uim cmd pointer, It is already removed thru client Table
  ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MSG_ERROR("Sending ICC Search Record command to UIM is failed",0,0,0);
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
  }
  return mmgsdi_status;
}/* end of function mmgsdi_uim_icc_search_record */


