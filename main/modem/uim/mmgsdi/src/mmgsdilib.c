/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I   L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that MMGSDI client can call to access
  various SIM/USIM/RUIM functions

  mmgsdi_read_transparent
    Read data from a Transparent file

  mmgsdi_read_record
    Read data from a record in a Linear Fixed or Cyclic file

  mmgsdi_write_transparent
    Write data into a Transparent file

  mmgsdi_write_record
    Write data into a record Linear Fixed or Cyclic file

  mmgsdi_get_file_attr
    Get File Attribute information for the files

  mmgsdi_send_card_status
    Send a Status Command to the card for Detection Purposes

  mmgsdi_verify_pin
    Verify the PIN with the card

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/sqa/mmgsdi/src/6/mmgsdilib.c#2 $$ $DateTime: 2010/11/04 05:20:19 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/10   shr     Fixed input data length check when handling a
                   Compute IP Auth request
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/29/09   js      Fixed compiler warning
03/19/09   js      Support for Enhanced NAA Refresh by file path
01/21/09   kk      Updated length check for read_record API
11/19/08   js      Fixed lint errors
09/25/08   js      Fixed Klockwork errors
09/22/08   js      Removed MFLO featurization in mmgsdi_send_apdu
07/20/08   jk      Support for Generate Key API
07/30/08   SSR     Update search for blocking the search directions belonging
                   to Record Pointer.
07/17/08   kk      Added support for PKCS 15
06/23/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/21/08   kk      Added PKCS15 Support
03/03/08   sun     Added support for OMA Bcast
01/10/08   nk      Fixed lint error, memcpy of esn to qw_equ of esn
11/15/07   sun     Added support for NAA refresh
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
08/14/07   sun     Added MUK_DEL Context
07/02/07   sun     Added Comments for usage
06/01/07   sun     Fixed Featurization for SAP
05/22/07   nk      Added support for ESN ME change and removed featurization
                   for uniform rpc support
05/17/07   wli     Added support for JCDMA RUIM
05/02/07   sun     Added support for Service Available and USIM Auth
04/03/07   sp      Send status command before termuinating an app.
03/26/07   tml     Fixed lint error
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
03/06/07   nk      Added PIN length check for minimum acceptable length
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access.  Add
                   support for cyclic and linear fixed write
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app.  Allow session
                   management to use the mmgsdi_session_build_select_aid api
08/25/06   sun     Fixed slot for Refresh
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     lint
05/23/06   tml     BDN support
04/15/06   tml     Do not copy if length of onchip data is 0
04/13/06   jar     Added ONCHIP SIM Support
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.c
01/17/05   tml     MMGSDI cleanup
01/04/06   tml     Updated enum name for SAP and Card powerup and down
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
11/03/05   sun     Added support for Refresh Command
10/28/05   sun     Fixed Orig Request Types
10/26/05   pv      Allow reads and writes of more than 512 bytes
10/20/05   sun     Added support for BT Deregisteration
09/29/05   tml     Greater than 512 byte Cache Read and client dereg supports
08/30/05   jar     Set the client id before sending to GSDI in Card Status API
08/30/05   jar     Added support for gsdi_send_card_status()
08/27/05   tml     Compilation fix
08/26/05   sun     Added support for BT SAP
08/25/05   pv      API to support CARD PDOWN and PUP
07/26/05   sst     Added MMGSDIBT support
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
02/24/05   sk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "gsdi.h"
#include "uimgen.h"
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================
   FUNCTION:      MMGSDI_WRITE_RECORD

   DESCRIPTION:
     This function will provide write access to any record in a Linear Fixed
     File. This library function is intended to encapsulate the
     details required to write a file to the SIM/USIM/RUIM. The client using
     this library function will only be able to access files that require a
     record number.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will attempt to write as if LINEAR FIXED File.
     LINEAR FIXED FILE: Will write the record of the file provided in the
                        Library call.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     MMGSDI cache will be updated if the requested data is located in the cache
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_write_record (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_file_enum_type              file_name,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_write_data_type             write_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_access_type      file_info;

  file_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_info.file.file_enum = file_name;

  return mmgsdi_write_record_ext(client_id,
                                 card_slot,
                                 file_info,
                                 MMGSDI_LINEAR_FIXED_FILE,
                                 record_number,
                                 write_data,
                                 response_cb_ptr,
                                 client_ref);
} /* mmgsdi_write_record */


/*===========================================================================
   FUNCTION:      MMGSDI_WRITE_RECORD_EXT

   DESCRIPTION:
     This function will provide write access to any record in a Linear Fixed
     Or Cyclic File.
     It will allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will write the record of the file provided in the
                        Library call.
     LINEAR FIXED FILE: Will write the record of the file provided in the
                        Library call.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

     For Record type:
     MMGSDI_LINEAR_FIXED_FILE:       Will use ABSOULTE access to card
     MMGSDI_CYCLIC_FILE:             Will use PREVIOUS access to card
     MMGSDI_TRANSPARENT_FILE:        Return an Error
     MMGSDI_MASTER_FILE:             Return an Error
     MMGSDI_DEDICATED_FILE:          Return an Error
     MMGSDI_MAX_FILE_STRUCTURE_ENUM: Return an Error

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_returns_T

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_write_record_ext (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_access_type                 file_access,
  mmgsdi_file_structure_enum_type    record_type,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_write_data_type             write_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{

  mmgsdi_task_cmd_type       * task_cmd_ptr  = NULL;
  mmgsdi_write_req_type      * msg_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_SUCCESS;
  int32                        var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                        total_mem_len = 0;
  int32                        temp_mem      = 0;
  uint32                       task_cmd_len  = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Slot Check
    3) Response CB function Check
    4) Record number > 0 Check if Linear Fixed
    5) Data len > 0 Check
    6) Check for EF or PATH access
    7) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_NEGATIVE(record_number);
  if ((record_number == 0) && (record_type == MMGSDI_LINEAR_FIXED_FILE))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  if (write_data.data_len <= 0)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Write Data Len: 0x%x <= 0",
                             write_data.data_len,
                             0,
                             0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    MSG_ERROR("Invalid Access Method Type 0x%x", file_access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len[0] = write_data.data_len;

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
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.write_req, MMGSDI_WRITE_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.write_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_write_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_write_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
    - file type
      Either CYCLIC or LINEAR FIXED is OK, defaulted to LINEAR FIXED
      (they are equilvalent in this context because they shared the same
      APDU parameters)
    - record index mode
    - record number
    - data len
    - data
      Mem malloc and validate, if fail, free memory space and return
    [offset is not being populated because this is a write record function]
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }
  msg_ptr->file_type             = record_type;
  msg_ptr->rec_num               = record_number;
  msg_ptr->data.data_len         = write_data.data_len;
  msg_ptr->int_client_data       = 0;

  memcpy(msg_ptr->data.data_ptr,
         write_data.data_ptr,
         int32touint32(msg_ptr->data.data_len));

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of WRITE RECORD command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_write_record_ext */


/*===========================================================================
   FUNCTION:      MMGSDI_WRITE_TRANSPARENT

   DESCRIPTION:
     This function will provide write access to any Transparent File. This
     library function is intended to encapsulate the details required to write
     a file to the SIM/USIM/RUIM. The client using this library function will
     only be able to access files that has a flat data structure.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  write the data provided in the Library call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function


   LIMITATIONS:
     CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                                  Record based File.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     MMGSDI cache will be updated if the requested data is located in the cache
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_write_transparent (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_file_enum_type    file_name,
  mmgsdi_offset_type       offset,
  mmgsdi_write_data_type   write_data,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_access_type      file_info;

  file_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_info.file.file_enum = file_name;

  return mmgsdi_write_transparent_ext(client_id,
                                      card_slot,
                                      file_info,
                                      offset,
                                      write_data,
                                      response_cb_ptr,
                                      client_ref);
} /* mmgsdi_write_transparent */


/*===========================================================================
   FUNCTION:      MMGSDI_WRITE_TRANSPARENT_EXT

   DESCRIPTION:
     This function will provide write access to any Transparent File.
     It will allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  write the data provided in the Library call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_returns_T

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_write_transparent_ext (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_access_type       file_access,
  mmgsdi_offset_type       offset,
  mmgsdi_write_data_type   write_data,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_task_cmd_type       * task_cmd_ptr  = NULL;
  mmgsdi_write_req_type      * msg_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;
  int32                        total_mem_len = 0;
  int32                        var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                        temp_mem      = 0;
  uint32                       task_cmd_len  = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Slot Check
    3) Response CB function Check
    4) offset >= 0 Check
    5) Data len > 0 Check
    6) Check for EF and Path access
    7) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_NEGATIVE(offset);
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    MSG_ERROR("Invalid Access Method Type 0x%x", file_access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  if (write_data.data_len <= 0)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Write Data Len: 0x%x <= 0",
                             write_data.data_len,
                             0,
                             0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len[0] = write_data.data_len;

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
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.write_req, MMGSDI_WRITE_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.write_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_write_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_write_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
    - file type
      TRANSPARENT only
    - offset
    - data len
    - data
      Mem malloc and validate, if fail, free memory space and return
    [record number is not being populated because this is a write transparent
     function]
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }
  msg_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;
  msg_ptr->offset                = offset;
  msg_ptr->data.data_len         = write_data.data_len;
  msg_ptr->int_client_data       = 0;

  memcpy(msg_ptr->data.data_ptr,
         write_data.data_ptr,
         int32touint32(msg_ptr->data.data_len));

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of WRITE TRANS command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_write_transparent_ext */


/*===========================================================================
   FUNCTION:      MMGSDI_READ_TRANSPARENT

   DESCRIPTION:
     This function will provide read access to any Transparent File. This
     library function is intended to encapsulate the details required to read
     a file to the SIM/USIM/RUIM. The client using this library function will
     only be able to access files that has a flat data structure.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return the contents as required in the Library
                        Call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function

   LIMITATIONS:
     CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                                  CYCLIC FILE or CYCLIC FILE.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_read_transparent (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_file_enum_type    file_name,
  mmgsdi_offset_type       offset,
  mmgsdi_len_type          req_len,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_access_type      file_info;

  file_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_info.file.file_enum = file_name;

  return mmgsdi_read_transparent_ext(client_id,
                                     card_slot,
                                     file_info,
                                     offset,
                                     req_len,
                                     response_cb_ptr,
                                     client_ref);
} /* mmgsdi_read_transparent */


/*===========================================================================
   FUNCTION:      MMGSDI_READ_TRANSPARENT_EXT

   DESCRIPTION:
     This function will provide read access to any Transparent File. It will
     allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return the contents as required in the Library
                        Call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function

   LIMITATIONS:
     CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                                  CYCLIC FILE or CYCLIC FILE.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_read_transparent_ext (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_access_type       file_access,
  mmgsdi_offset_type       offset,
  mmgsdi_len_type          req_len,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_task_cmd_type       * task_cmd_ptr = NULL;
  mmgsdi_read_req_type       * msg_ptr = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;
  int32                        total_mem_len = 0;
  uint32                       task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Slot Check
    3) Response CB function Check
    4) offset >= 0 Check
    5) Data len > 0 Check
    6) Check for EF and Path access
    7) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_NEGATIVE(offset);
  MMGSDIUTIL_RETURN_IF_NEGATIVE(req_len);
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    MSG_ERROR("Invalid Access Method Type 0x%x", file_access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.read_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_read_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_READ_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_read_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
    - file type
      TRANSPARENT only
    - offset
    - data
      Mem malloc and validate, if fail, free memory space and return
    [record number is not being populated because this is a read transparent
     function]
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }
  msg_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;
  msg_ptr->offset                = offset;
  msg_ptr->data_len              = req_len;
  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of READ TRANS command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_read_transparent_ext */


/*===========================================================================
   FUNCTION:      MMGSDI_READ_RECORD

   DESCRIPTION:
     This function will provide read access to any record in a Linear Fixed
     Or Cyclic File. This library function is intended to encapsulate the
     details required to read a file to the SIM/USIM/RUIM. The client using
     this library function will only be able to access files that require a
     record number.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will access the record of the file provided in the
                        Library call.
     LINEAR FIXED FILE: Will access the record of the file provided in the
                        Library call.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_read_record (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_file_enum_type              file_name,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_len_type                    request_length,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_access_type      file_info;

  file_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_info.file.file_enum = file_name;

  return mmgsdi_read_record_ext(client_id,
                                card_slot,
                                file_info,
                                record_number,
                                request_length,
                                response_cb_ptr,
                                client_ref);

} /* mmgsdi_read_record */


/*===========================================================================
   FUNCTION:      MMGSDI_READ_RECORD_EXT

   DESCRIPTION:
     This function will provide access to any file in the SIM/USIM/RUIM.
     It will allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will access the record of the file provided in the
                        Library call.
     LINEAR FIXED FILE: Will access the record of the file provided in the
                        Library call.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_returns_T

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_read_record_ext (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_access_type                 file_access,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_len_type                    request_length,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type       * task_cmd_ptr   = NULL;
  mmgsdi_read_req_type       * msg_ptr        = NULL;
  mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_ERROR;
  int32                        total_mem_len  = 0;
  uint32                       task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Slot Check
    3) Response CB function Check
    4) Record number >= 0 Check
    5) Data len > 0 Check
    6) file access method Check
    7) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_NEGATIVE(record_number);
  if(record_number == 0)
  {
    return MMGSDI_ERROR;
  }
  if (request_length < 0)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Read Data Len: 0x%x <= 0",
                             request_length,
                             0,
                             0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    MSG_ERROR("Invalid Access Method Type 0x%x", file_access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.read_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_read_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_READ_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_read_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
    - file type
      Either CYCLIC or LINEAR FIXED is OK, defaulted to LINEAR FIXED
      (they are equilvalent in this context because they shared the same
      APDU parameters)
    - record index mode
    - record number
    - data len
      Mem malloc and validate, if fail, free memory space and return
    [offset is not being populated because this is a read record function]
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }
  msg_ptr->file_type             = MMGSDI_LINEAR_FIXED_FILE;
  msg_ptr->rec_num               = record_number;
  msg_ptr->data_len              = request_length;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of READ RECORD command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_read_record_ext */


/*===========================================================================
   FUNCTION:      MMGSDI_GET_FILE_ATTR

   DESCRIPTION:
     This function will provide get file attributes to any file in the
     SIM/USIM/RUIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.
     MASTER FILE:       Will get information on the Master File
     DEDICATED FILE:    Will get information on the Dedicated File

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Due to memory allocation failure or due to
                              failure of other functionalities.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_get_file_attr (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_access_type       file_name,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_task_cmd_type         * task_cmd_ptr      = NULL;
  mmgsdi_return_enum_type        mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type *get_file_attr_msg = NULL;
  int32                          total_mem_len     = 0;
  int32                          num_var_len       = 0;
  int32                          var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                          temp_mem          = 0;
  uint32                         task_cmd_len      = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. client_id        -
    2. card_slot        - MMGSDI_SLOT_1
    3. file_name.access - MMGSDI_BY_PATH_ACCESS or MMGSDI_EF_ENUM_ACCESS
    4. response_cb_ptr  - Cannot be NULL
    5. client_ref       - Cannot be NULL
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /*---------------------------------------------------------------------------
    MEMORY ALLOCATION FOR THE REQUEST MESSAGE
    1. Allocate memory for the get file attribute request message
    2. Check for successful memory allocation
    3. Memset the allocated memory with zeros.
  ---------------------------------------------------------------------------*/
  switch(file_name.access_method)
  {
  case MMGSDI_BY_PATH_ACCESS:
    break;
  case MMGSDI_EF_ENUM_ACCESS:
    break;
  case MMGSDI_BY_APP_ID_ACCESS:
    num_var_len = 1;
    var_len[0] = file_name.file.app_id.data_len;
    break;
  default:
    MSG_ERROR("INVALID FILE_ACCESS_METHOD %x",file_name.access_method,0,0);
    return MMGSDI_INCORRECT_PARAMS;

  }

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
  if (num_var_len > 0)
    mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_file_attr_req, MMGSDI_GET_FILE_ATTR_REQ);

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
  task_cmd_ptr->cmd.cmd_enum                          = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_msg->request_header.client_id         = client_id;
  get_file_attr_msg->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_msg->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_msg->request_header.request_id_ptr    = rex_self();
  get_file_attr_msg->request_header.slot_id           = card_slot;
  get_file_attr_msg->request_header.client_data       = client_ref;
  get_file_attr_msg->request_header.response_cb       = response_cb_ptr;
  get_file_attr_msg->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
  get_file_attr_msg->request_header.payload_len     =
    get_file_attr_msg->request_header.request_len -
    uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1.  access_method       -  Access type specified in the function paramters
    2.  file_enum           -  File name specified in the function paramters
                           OR
    2a. path                -  Path specified in the function paramters
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&get_file_attr_msg->access, &file_name);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    MMGSDIUTIL_DEBUG_MSG_ERR("Get File Attr Lib Fail: Access Copy Fail 0x%x",
                             mmgsdi_status, 0, 0);
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  get_file_attr_msg-> activate_aid = TRUE;

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of GET FILE ATTR command %d",mmgsdi_status,0,0);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_get_file_attr */


/*===========================================================================
   FUNCTION:      MMGSDI_CLIENT_EVT_REG

   DESCRIPTION:
     This function allows client registration for new event callback that will
     contain additional info based on specific events being sent.
     The response to this request will return client ID to the calling task.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID
   mmgsdi_callback_type * evt_cb_ptr:       Input.  Pointer to callback for event
                                                    notifications.  If NULL is passed
                                                    the event callback will be
                                                    deregistered.
   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    registration request.
   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

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
mmgsdi_return_enum_type  mmgsdi_client_evt_reg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_evt_callback_type * evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_client_id_reg_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /* No Need to Align Pointer inside client ID Reg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_reg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_EVT_REG_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_EVT_REG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_EVT_REG_REQ;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_req_type content
    - Event call back
    - Client Registration for EVENT
  ---------------------------------------------------------------------------*/
  msg_ptr->evt_cb_ptr = evt_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of CLIENT REG command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_client_evt_reg */


/*===========================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_REG

   DESCRIPTION:
     This function allows client registration for the use of MMGSDI API.
     The response to this request will return client ID to the calling task that
     is required to be used for subsequent MMGSDI API calls.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type  mmgsdi_client_id_reg (
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_option_type option;

  memset(&option,0, sizeof(mmgsdi_option_type));

  return mmgsdi_client_id_reg_set_notify(FALSE,
                                         option,
                                         response_cb_ptr,
                                         client_ref);
} /* mmgsdi_client_id_reg */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_DEREG

   DESCRIPTION:
     This function allows client deregistration for the use of MMGSDI API.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID
   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    registration request.
   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

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
mmgsdi_return_enum_type  mmgsdi_client_id_dereg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_client_id_dereg_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_ERROR;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
    3) Client ID Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /* No Need to Align Pointer inside client ID Dereg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_dereg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_dereg_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_ID_DEREG_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_ID_DEREG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_ID_DEREG_REQ;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_req_type content
    - Event call back (NULL)
    - Client Registration for API
  ---------------------------------------------------------------------------*/
  msg_ptr->evt_cb_ptr = NULL;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of CLIENT DEREG command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_client_id_dereg */


/*===========================================================================
   FUNCTION:      MMGSDI_CLIENT_EVT_DEREG

   DESCRIPTION:
     This function allows client deregistration for new event callback that will
     contain additional info based on specific events being sent.
     The response to this request will return client ID to the calling task.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID
   mmgsdi_callback_type * evt_cb_ptr:       Input.  Pointer to callback for event
                                                    notifications.  If NULL is passed
                                                    the event callback will be
                                                    deregistered.
   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    registration request.
   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

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
mmgsdi_return_enum_type  mmgsdi_client_evt_dereg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_evt_callback_type * evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_client_id_dereg_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_ERROR;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /* No Need to Align Pointer inside client ID Dereg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_dereg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_dereq_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_EVT_DEREG_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_EVT_DEREG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_EVT_DEREG_REQ;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_req_type content
    - Event call back
    - Client Registration for EVENT
  ---------------------------------------------------------------------------*/
  msg_ptr->evt_cb_ptr = evt_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of CLIENT DEREG command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
}


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_MANAGEMENT_OPEN

DESCRIPTION:
  This function allows the client to open a Session to a an Application
  on the card.  The session opened will point to the USIM Application for
  which the Application Identifier provided Pertains to.

  Upon successful completion of this command, the client_id will have an
  associated logical channel and an associated UICC Application Identifier.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client ID
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_data_type         app_id_data:    Input.  Application Identifier
  mmgsdi_client_data_type  client_ref_ptr: Input.  User Data returned upon
                                                  completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
mmgsdi_return_enum_type  mmgsdi_session_management_open (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_data_type         app_id_data,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_session_open_req_type   *msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
    3) APP ID Length and pointer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((app_id_data.data_len <= 0) ||
      (app_id_data.data_len > MMGSDI_MAX_APP_ID_LEN) ||
      (app_id_data.data_ptr == NULL))
    return MMGSDI_ERROR;

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_open_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_OPEN_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OPEN_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OPEN_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type content
    - application ID
  ---------------------------------------------------------------------------*/
  msg_ptr->app_id_data.data_len = app_id_data.data_len;
  memcpy(msg_ptr->app_id_data.data_ptr,
         app_id_data.data_ptr,
         int32touint32(msg_ptr->app_id_data.data_len));

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else {
    MSG_HIGH("Queue of Open command status %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
} /* mmgsdi_session_management_open */


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_MANAGEMENT_CLOSE

DESCRIPTION:
  This function is used to close the currently active session with the
  UICC Application communicating over a UICC Logical Channel.

  When called, the normal UICC Application Deactivation procedures are
  performed on the Application ID Associated with the Client ID.

  The logical channel associated with the client ID will also be released.

  Upon successful completion of this command, the client_id will be valid
  and existent in the client_id_table but the logical channel and
  application id will be removed.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client ID
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                  completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  The client must also have a valid session open.
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
mmgsdi_return_enum_type  mmgsdi_session_management_close (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  MSG_HIGH("MMGSDI_SESSION_MANAGEMENT_CLOSE, client 0x%x, slot 0x%x",
           client_id, card_slot, 0);

#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
  return mmgsdi_session_build_status_terminate (
    client_id,
    card_slot,
    MMGSDI_SESSION_CLOSE_REQ,
    response_cb_ptr,
    client_ref);
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
} /* mmgsdi_session_management_close */


/*===========================================================================
FUNCTION:      MMGSDI_ISIM_AUTHENTICATE

DESCRIPTION:
  This function is used to perform a "Network" Initiated Authentication with
  the ISIM Application.  This is only valid and applicable when an ISIM
  Application is available as specified in 31.103.  This function will
  perform a check for the ISIM Application before sending the command.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_data_type         auth_req:       Input.  Length and Data of auth
                                                   challenge.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  The client must also have a valid session open.
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
mmgsdi_return_enum_type  mmgsdi_isim_authenticate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_data_type         auth_req,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
#ifdef FEATURE_MMGSDI_ISIM
  mmgsdi_task_cmd_type           *task_cmd_ptr  = NULL;
  mmgsdi_auth_req_type           *msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  int32                           var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                           temp_mem      = 0;
  uint32                          task_cmd_len  = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((auth_req.data_len <= 0) || (auth_req.data_ptr == NULL))
    return MMGSDI_ERROR;

  var_len[0] = auth_req.data_len;

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
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.auth_req, MMGSDI_ISIM_AUTH_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.auth_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_isim_auth_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_ISIM_AUTH_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_ISIM_AUTH_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_ISIM_AUTH_REQ;
  msg_ptr->auth_context                     = MMGSDI_AUTN_ISIM_CONTEXT;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type content
    - application ID
  ---------------------------------------------------------------------------*/
  msg_ptr->auth_req.data_len = auth_req.data_len;
  memcpy(msg_ptr->auth_req.data_ptr,
         auth_req.data_ptr,
         int32touint32(msg_ptr->auth_req.data_len));

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }

  MSG_HIGH("Queue of ISIM Auth command status %d",mmgsdi_status,0,0);

  return mmgsdi_status;
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_ISIM */
} /* mmgsdi_isim_authenticate */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT

DESCRIPTION
  This function attempts to perform a SAP connect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) /*xxx_sap*/
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
} /* mmgsdi_sap_connect */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_DISCONNECT

DESCRIPTION
  This function attempts to perform a SAP disconnect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  disconnect_mode: Graceful/Immediate.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_disconnect(
    mmgsdi_client_id_type               client_id,
    mmgsdi_slot_id_enum_type            card_slot,
    mmgsdi_sap_mode_enum_type           sap_mode,
    mmgsdi_disconnect_mode_enum_type    disconnect_mode,
    mmgsdi_callback_type                response_cb_ptr,
    mmgsdi_client_data_type             client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
} /* mmgsdi_sap_disconnect */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
}/*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_SEND_APDU

DESCRIPTION
  This function will send an APDU to the SAP Client.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  channel_id:      The channel id.
  apdu_data:       The APDU to send to the card.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  int32                          channel_id,
  mmgsdi_sap_send_apdu_data_type apdu_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
}/*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_ON

DESCRIPTION
  This function attempts to perform a SAP power on

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_on(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) /*xxx_sap*/
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
} /* mmgsdi_sap_power_on */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_OFF

DESCRIPTION
  This function attempts to perform a SAP power off

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_off(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/

} /* mmgsdi_sap_power_off */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_CARD_READER_STATUS

DESCRIPTION
  This function attempts to perform a SAP card reader status

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_card_reader_status(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/

} /* mmgsdi_sap_card_reader_status */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_RESET

DESCRIPTION
  This function attempts to perform a SAP RESET

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_reset(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
} /* mmgsdi_sap_reset */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_PIN_VERIFY

DESCRIPTION
    This function attempts to perform a SAP PIN Verification.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_pin:         SAP Pin.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_pin_verify(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_bt_bd_address_type bd_addr,
  mmgsdi_data_type          sap_pin,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
  )
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
  /*lint -esym(715, card_slot)*/
}


/*===========================================================================
FUNCTION MMGSDI_SAP_INITIALIZE

DESCRIPTION
  This function attempts to perform a SAP initialization.  This will set
  the mobile to be either SAP Client mode or SAP Server mode.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_initialize
(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_sap_mode_enum_type sap_mode,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
/*lint -esym(715, card_slot)*/
}


/*===========================================================================
FUNCTION MMGSDI_SAP_DEREGISTER

DESCRIPTION
  This function attempts to perform a SAP deregistration.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_deregister
(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*(FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
  /*lint -esym(715, card_slot)*/
}


/*===========================================================================
FUNCTION:      MMGSDI_CARD_PUP

DESCRIPTION:
  This function is used to perform a power up to the SIM/USIM card.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_callback_type    response_cb_ptr: Input.  Call back function when the
                                                   command execution is done.
  mmgsdi_card_pup_options_enum_type pup_option: Input.  Option as to whether or not
                                                   to notify sim removal event.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_pup(
    mmgsdi_client_id_type             client_id,
    mmgsdi_slot_id_enum_type          card_slot,
    mmgsdi_callback_type              response_cb_ptr,
    mmgsdi_card_pup_options_enum_type pup_option,
    mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_card_pup_req_type*       msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /* Input Parameters Check.
   */
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(pup_option,
                                    MMGSDI_CARD_POWER_UP_SWITCH_UIM_PASSIVE,
                                    MMGSDI_CARD_POWER_UP_INITIAL_PUP);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if (pup_option == MMGSDI_CARD_POWER_UP_INITIAL_PUP)
  {
    mmgsdi_refresh_req_type      refresh_req;

    memset(&refresh_req,0,sizeof(mmgsdi_refresh_req_type));

    refresh_req.refresh_req_type = MMGSDI_REFRESH_ORIG_PUP_REQ;
    refresh_req.refresh.card_req.refresh_mode = MMGSDI_REFRESH_RESET_AUTO;

    /* when a card power up initail pup option is provided, a power
       up command is triggered via a refresh RESET command */
    mmgsdi_status = mmgsdi_build_refresh_req( client_id,
                                              card_slot,
                                              &refresh_req,
                                              response_cb_ptr,
                                              client_ref);

    if ((mmgsdi_status != MMGSDI_SUCCESS) &&
        (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS))
    {
      MMGSDI_DEBUG_MSG_ERROR("UIM Did not accept reset(%u)", mmgsdi_status, 0, 0);
    }
    return mmgsdi_status;
  }
  else
  {
    total_mem_len = sizeof(mmgsdi_task_cmd_type);

    task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                              task_cmd_len);
    /*---------------------------------------------------------------------------
      - No need to align pointers
      ---------------------------------------------------------------------------*/
    msg_ptr = &task_cmd_ptr->cmd.cmd.card_pup_req;

    /*
    * Populating mmgsdi_card_pup_req_type header.
    */
    task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_CARD_PUP_REQ;
    msg_ptr->request_header.client_id          = client_id;
    msg_ptr->request_header.orig_request_type  = MMGSDI_CARD_PUP_REQ;
    msg_ptr->request_header.request_type       = MMGSDI_CARD_PUP_REQ;
    msg_ptr->request_header.slot_id            = card_slot;
    msg_ptr->request_header.request_id_ptr     = rex_self();
    msg_ptr->request_header.client_data        = client_ref;
    msg_ptr->request_header.response_cb        = response_cb_ptr;

    /*
    * Populating mmgsdi_card_pup_req_type content.
    */
    msg_ptr->option = pup_option;

    /*
    * Calculate the payload len and request len.
    */
    msg_ptr->request_header.request_len = total_mem_len;
    msg_ptr->request_header.payload_len = total_mem_len
                                          - uint32toint32(sizeof(mmgsdi_request_header_type));

    /* Put on mmgsdi command queue */
    mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS) {
      /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
      MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
      MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
    }
    else
    {
      MSG_HIGH("Successful queue of Card Pup command %d",mmgsdi_status,0,0);
    }
    return mmgsdi_status;
  }
} /* mmgsdi_card_pup */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_PDOWN

DESCRIPTION:
  This function is used to perform a power down to the SIM/USIM card.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_callback_type    response_cb_ptr: Input.  Call back function when the
                                                   command execution is done.
  mmgsdi_card_pup_options_enum_type pdown_option: Input.Option as to whether or not
                                                   to notify sim insertion.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_pdown(
    mmgsdi_client_id_type                   client_id,
    mmgsdi_slot_id_enum_type                card_slot,
    mmgsdi_callback_type                    response_cb_ptr,
    mmgsdi_card_pdown_options_enum_type     pdown_option,
    mmgsdi_client_data_type                 client_ref
)
{
  mmgsdi_task_cmd_type         * task_cmd_ptr  = NULL;
  mmgsdi_card_pdown_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  int32                          total_mem_len = 0;
  uint32                         task_cmd_len  = 0;

  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(pdown_option,
                                    MMGSDI_CARD_POWER_DOWN_ONLY,
                                    MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);


  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.card_pdown_req;

  /*
   * Populating mmgsdi_card_pdown_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_CARD_PDOWN_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.orig_request_type  = MMGSDI_CARD_PDOWN_REQ;
  msg_ptr->request_header.request_type       = MMGSDI_CARD_PDOWN_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.request_id_ptr     = rex_self();
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;
  /*
   * Populating mmgsdi_card_pdown_req_type content.
   */

  msg_ptr->option = pdown_option;

  /*
   * Calculate the payload len and request len.
   */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Card Pdown command %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
} /* mmgsdi_card_pdown */


/*===========================================================================
FUNCTION MMGSDI_SEND_CARD_STATUS

DESCRIPTION
  GSM 11.11 functionality:
    This function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used to
    give an opportunity for a pro active SIM to indicate that the SIM
    wants to issue a SIM Application Toolkit command to the ME.

    Status and return type is not required when using a GSM SIM Card.

  UMTS 31.102 Functionality:
    This function returns information concerning the current directory or current
    application.  In addition, according to the application specification,
    it may be used to indicate to the application in the UICC that its
    session activation procedure has been successfully executed or that
    its termination procedure will be executed.
    NOTE: These indications may be used to synchronize the applications in the
    terminal and in the UICC.

    A status and return type is required when using a UICC Card with a USIM
    application.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  me_app_status:   Indicates that status of the application in the handset
  ret_data_struct: Structure in which the data should be returned in
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.


DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_card_status(
    mmgsdi_client_id_type            client_id,
    mmgsdi_slot_id_enum_type         card_slot,
    mmgsdi_status_me_app_enum_type   me_app_status,
    mmgsdi_status_ret_data_enum_type ret_data_struct,
    mmgsdi_callback_type             response_cb_ptr,
    mmgsdi_client_data_type          client_ref
)
{
  mmgsdi_task_cmd_type     * task_cmd_ptr  = NULL;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_status_req_type *   msg_ptr       = NULL;
  int32                      total_mem_len = 0;
  uint32                     task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
    2) Card Slot ID check
    3) Verify the Params provided are within the valid range
    4) Client ID Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(me_app_status,
                                    MMGSDI_STATUS_APP_NONE,
                                    MMGSDI_STATUS_APP_TERMINATED);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(ret_data_struct,
                                    MMGSDI_STATUS_NONE,
                                    MMGSDI_STATUS_NO_DATA);

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.status_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CARD_STATUS_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_CARD_STATUS_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CARD_STATUS_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
          - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Populate the command details with the information provided by the client
     ------------------------------------------------------------------------*/
  msg_ptr->me_app_status   = me_app_status;
  msg_ptr->ret_data_struct = ret_data_struct;

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Card Status command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;

} /* mmgsdi_send_card_status */


/*===========================================================================
FUNCTION MMGSDI_ACTIVATE_ONCHIP_SIM

DESCRIPTION
  This function will build a MMGSDI_ACTIVATE_ONCHIP_SIM_REQ and queue it to
  the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  onchip_sim_config: TLV Data Len and TLV Data.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  Access to a real card will not be possible without a power cycle
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_activate_onchip_sim (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
} /* mmgsdi_activate_onchip_sim */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_ENABLE_BDN

DESCRIPTION
  This function enables the BDN

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  BDN according to specification

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
  Will enable the BDN if success
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_enable_bdn (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type     * task_cmd_ptr  = NULL;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_bdn_req_type      * msg_ptr       = NULL;
  int32                      total_mem_len = 0;
  uint32                     task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
    2) Card Slot ID check
    4) Client ID Check
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((card_slot != MMGSDI_SLOT_1) && (card_slot != MMGSDI_SLOT_2))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.enable_bdn_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_ENABLE_BDN_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_ENABLE_BDN_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_ENABLE_BDN_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
          - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Enable BDN Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Enable BDN command %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_DISABLE_BDN

DESCRIPTION
  This function disables the BDN

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  BDN according to specification

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
  Will disable the BDN if success
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_disable_bdn (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type     * task_cmd_ptr  = NULL;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_bdn_req_type      * msg_ptr       = NULL;
  int32                      total_mem_len = 0;
  uint32                     task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
    2) Card Slot ID check
    4) Client ID Check
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((card_slot != MMGSDI_SLOT_1) && (card_slot != MMGSDI_SLOT_2))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.disable_bdn_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_DISABLE_BDN_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_DISABLE_BDN_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_DISABLE_BDN_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
          - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Disable BDN Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Disable BDN command %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PIN_OPERATION

DESCRIPTION
  Populate the core PIN operation request

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_pin_operation(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref,
  mmgsdi_pin_operation_req_type      **msg_ptr,
  mmgsdi_task_cmd_type               **task_cmd_ptr,
  mmgsdi_pin_operation_enum_type     pin_op
)
{
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;
  int32                           total_mem_len  = 0;
  uint32                          task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Slot Check
    3) Response CB function Check
    4) PIN Data Ptr >= 0 Check
    5) Pin Data  len < 0 &  >4 Check
    6) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*task_cmd_ptr,
                                            task_cmd_len);
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  *msg_ptr = &((*task_cmd_ptr)->cmd.cmd.pin_operation_req);

  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  (*task_cmd_ptr)->cmd.cmd_enum                = MMGSDI_PIN_OPERATION_REQ;
  (*msg_ptr)->request_header.client_id         = client_id;
  (*msg_ptr)->request_header.request_type      = MMGSDI_PIN_OPERATION_REQ;
  (*msg_ptr)->request_header.orig_request_type = MMGSDI_PIN_OPERATION_REQ;
  (*msg_ptr)->request_header.slot_id           = card_slot;
  (*msg_ptr)->request_header.request_id_ptr    = rex_self();
  (*msg_ptr)->request_header.client_data       = client_ref;
  (*msg_ptr)->request_header.response_cb       = response_cb_ptr;

  (*msg_ptr)->pin_op                           = pin_op;
  (*msg_ptr)->pin_id                           = pin_id;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  (*msg_ptr)->request_header.request_len = total_mem_len;
  (*msg_ptr)->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  return MMGSDI_SUCCESS;
}


/*===========================================================================
FUNCTION MMGSDI_VERIFY_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to verify the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to verify could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.

  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_verify_pin (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;

  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(pin_data.data_ptr);

  /* check if pin is in the correct length and data range */
  if (!mmgsdi_util_is_pin_valid(pin_data.data_ptr))
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_pin_operation(client_id,
                                       card_slot,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_VERIFY);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      MSG_ERROR("mmgsdi_pin_operation returned error 0x%x",mmgsdi_status,0,0);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_VERIFY
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  memcpy(msg_ptr->pin_puk.data_ptr,
         pin_data.data_ptr,
         int32touint32(pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of PIN VERIFY command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_verify_pin */


/*===========================================================================
FUNCTION MMGSDI_DISABLE_PIN_EXT

DESCRIPTION
  This function will buid a request to the MMGSDI Task to disable the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to disable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  replace_pin:       The Replacement Option
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_disable_pin_ext (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_pin_replace_enum_type       replace_pin,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_pin_operation_enum_type  pin_op         = MMGSDI_PIN_OP_DISABLE;


  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(pin_data.data_ptr);

  /* check if pin is in the correct length and data range */
  if (!mmgsdi_util_is_pin_valid(pin_data.data_ptr))
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  if(replace_pin == MMGSDI_PIN_REPLACED_BY_UNIVERSAL)
  {
    pin_op = MMGSDI_PIN_OP_DISABLE_AND_REPLACE;
    if(pin_id != MMGSDI_PIN1)
    {
      MSG_ERROR("PIN 0x%x cannot be replaced",0,0,0);
      return MMGSDI_INCORRECT_PARAMS;
    }
  }

  mmgsdi_status = mmgsdi_pin_operation(client_id,
                                       card_slot,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       pin_op);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      MSG_ERROR("mmgsdi_pin_operation returned error 0x%x",mmgsdi_status,0,0);
      return mmgsdi_status;
   }

  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_DISABLE
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  memcpy(msg_ptr->pin_puk.data_ptr,
         pin_data.data_ptr,
         int32touint32(pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of PIN DISABLE command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
}/* mmgsdi_disable_pin_ext */


/*===========================================================================
FUNCTION MMGSDI_DISABLE_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to disable the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to disable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_disable_pin (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_disable_pin_ext(client_id,
                                card_slot,
                                pin_id,
                                MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL,
                                pin_data,
                                response_cb_ptr,client_ref);
}/* mmgsdi_disable_pin */


/*===========================================================================
FUNCTION MMGSDI_ENABLE_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to enable the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to enable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_enable_pin (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(pin_data.data_ptr);

  /* check if pin is in the correct length and data range */
  if (!mmgsdi_util_is_pin_valid(pin_data.data_ptr))
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_pin_operation(client_id,
                                       card_slot,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_ENABLE);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      MSG_ERROR("mmgsdi_pin_operation returned error 0x%x",mmgsdi_status,0,0);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_ENABLE
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  memcpy(msg_ptr->pin_puk.data_ptr,
         pin_data.data_ptr,
         int32touint32(pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of PIN ENABLE command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
}/* mmgsdi_enable_pin */


/*===========================================================================
FUNCTION MMGSDI_CHANGE_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to change the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to change could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the current PIN.
  new_pin_data:      Will contain the len and code of the new PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.


This function performs a change  procedure against the Card for either PIN1 or PIN2.

DEPENDENCIES
None

RETURN VALUE
mmgsdi_return_enum_type
MMGSDI_SUCCESS:          The command structure was properly generated
                         and queued onto the MMGSDI Command Queue.
MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                         within appropriate ranges.
MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                         because the max number of commands are already
                         queued.
SIDE EFFECTS
  None
===================================================================*/
mmgsdi_return_enum_type  mmgsdi_change_pin(
  mmgsdi_client_id_type         client_id,
  mmgsdi_slot_id_enum_type      card_slot,
  mmgsdi_pin_enum_type          pin_id,
  mmgsdi_data_type              pin_data,
  mmgsdi_data_type              new_pin_data,
  mmgsdi_callback_type          response_cb_ptr,
  mmgsdi_client_data_type       client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN ||
    new_pin_data.data_len <= 0 || new_pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(pin_data.data_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(new_pin_data.data_ptr);

  /* check if pin is in the correct length and data range */
  if ( (!mmgsdi_util_is_pin_valid(pin_data.data_ptr)) ||
       (!mmgsdi_util_is_pin_valid(new_pin_data.data_ptr)) )
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_pin_operation(client_id,
                                       card_slot,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_CHANGE);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      MSG_ERROR("mmgsdi_pin_operation returned error 0x%x",mmgsdi_status,0,0);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_CHANGE
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  memcpy(msg_ptr->pin_puk.data_ptr,
         pin_data.data_ptr,
         int32touint32(pin_data.data_len));

  msg_ptr->new_pin.data_len = new_pin_data.data_len;

  memcpy(msg_ptr->new_pin.data_ptr,
         new_pin_data.data_ptr,
         int32touint32(new_pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of PIN CHANGE command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /*mmgsdi_change_pin*/


/*===========================================================================
FUNCTION MMGSDI_UNBLOCK_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to unblock the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to unblock could result in a permanently blocked PIN ID if
  the maximum number of attempts to unblock are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  puk_data:          Will contain the len and code of the PUK.
  new_pin_data:      Will contain the len and code of the new PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_unblock_pin(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   puk_data,
  mmgsdi_data_type                   new_pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

  if(puk_data.data_len <= 0 || puk_data.data_len > MMGSDI_PIN_MAX_LEN ||
     new_pin_data.data_len <= 0 || new_pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(puk_data.data_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(new_pin_data.data_ptr);

  /* check if pin is in the correct length and data range */
  if (!mmgsdi_util_is_pin_valid(new_pin_data.data_ptr))
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_pin_operation(client_id,
                                       card_slot,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_UNBLOCK);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      MSG_ERROR("mmgsdi_pin_operation returned error 0x%x",mmgsdi_status,0,0);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_UNBLOCK
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = puk_data.data_len;

  memcpy(msg_ptr->pin_puk.data_ptr,
         puk_data.data_ptr,
         int32touint32(puk_data.data_len));

  msg_ptr->new_pin.data_len = new_pin_data.data_len;

  memcpy(msg_ptr->new_pin.data_ptr,
         new_pin_data.data_ptr,
         int32touint32(new_pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of PIN UNBLOCK command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
}/* mmgsdi_unblock_pin */


/*===========================================================================
   FUNCTION:      MMGSDI_SELECT_AID

   DESCRIPTION:
     This function will let the client select from a list of currently available app IDs within the USIM Card.

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_select_aid (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_data_type                   aid,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                 * task_cmd_ptr  = NULL;
  mmgsdi_select_aid_req_type           * msg_ptr       = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;
  int32                                  total_mem_len = 0;
  uint32                                 task_cmd_len  = 0;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  if ( card_slot != MMGSDI_SLOT_1 &&
       card_slot != MMGSDI_SLOT_2 )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID",0,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if (aid.data_len > MMGSDI_MAX_APP_ID_LEN)
  {
    MMGSDI_DEBUG_MSG_ERROR("Bad AID Len 0x%x", aid.data_len, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if ((aid.data_len > 0) && (aid.data_ptr == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Aid Data Ptr",0,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate 1 large block of memory for the task_cmd_ptr and for the
     onchip config data.
     NOTE:  Side effect of the following total len is that it may not
            match exactly as a result of mmgsdi_task_cmd_data_type being
            a union. Will be addressed by using the correct type size
            rather than sizeof mmgsdi_task_cmd_data_type
     ------------------------------------------------------------------------*/
  total_mem_len =  sizeof(mmgsdi_task_cmd_data_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*-----------------------------------------------------------------------------
    No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.select_aid_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SELECT_AID_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SELECT_AID_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SELECT_AID_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*-----------------------------------------------------------------------------
    Calculate the payload len and request len
    ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len -
                                       uint32toint32(sizeof(mmgsdi_request_header_type));

  msg_ptr->app_data.data_len = aid.data_len;

  if ( aid.data_len > 0 )
  {
    memcpy(msg_ptr->app_data.data_ptr,
           aid.data_ptr,
           int32touint32(aid.data_len));
  }

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr
    ** has not been put onto the command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }

  MSG_HIGH("SELECT AID 0x%x ",mmgsdi_status,0,0);

  return mmgsdi_status;
} /* mmgsdi_select_aid */


/*===========================================================================
FUNCTION MMGSDI_GET_ALL_PIN_STATUS

DESCRIPTION
  This function will buid a request to the MMGSDI Task to request the PIN
  status for all relevant pins applicable to the client ID provided.
  If the client ID provided is in a session, then the PIN IDs applicable
  to the application in the session will be returned.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_all_pin_status (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                 * task_cmd_ptr  = NULL;
  mmgsdi_get_all_pin_status_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;
  int32                                  total_mem_len = 0;
  uint32                                 task_cmd_len  = 0;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  if ( card_slot != MMGSDI_SLOT_1 &&
       card_slot != MMGSDI_SLOT_2 )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID",0,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate 1 large block of memory for the task_cmd_ptr and for the
     onchip config data.
     NOTE:  Side effect of the following total len is that it may not
            match exactly as a result of mmgsdi_task_cmd_data_type being
            a union. Will be addressed by using the correct type size
            rather than sizeof mmgsdi_task_cmd_data_type
     ------------------------------------------------------------------------*/
  total_mem_len =  sizeof(mmgsdi_task_cmd_data_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*-----------------------------------------------------------------------------
    No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.pin_status_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*-----------------------------------------------------------------------------
    Calculate the payload len and request len
    ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len -
                                       uint32toint32(sizeof(mmgsdi_request_header_type));
  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr
    ** has not been put onto the command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }

  MSG_HIGH("GET ALL PIN STATUS REQUEST 0x%x ",mmgsdi_status,0,0);

  return mmgsdi_status;
}/* mmgsdi_get_all_pin_status */


/*===========================================================================
FUNCTION MMGSDI_ENABLE_ACL

DESCRIPTION
  This function enables the ACL

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  ACL according to specification

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
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_enable_acl (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type           * task_cmd_ptr  = NULL;
  mmgsdi_return_enum_type          mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_acl_operation_req_type  * msg_ptr       = NULL;
  int32                            total_mem_len = 0;
  uint32                           task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
    2) Card Slot ID check
    4) Client ID Check
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((card_slot != MMGSDI_SLOT_1) && (card_slot != MMGSDI_SLOT_2))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.acl_operation_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_ACL_OPERATION_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_ACL_OPERATION_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_ACL_OPERATION_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->acl_op                           = MMGSDI_ACL_OP_ENABLE;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
          - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Enable ACL Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Enable ACL command %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_DISABLE_ACL

DESCRIPTION
  This function disables the ACL

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  ACL according to specification

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
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_disable_acl (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type           * task_cmd_ptr  = NULL;
  mmgsdi_return_enum_type          mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_acl_operation_req_type  * msg_ptr       = NULL;
  int32                            total_mem_len = 0;
  uint32                           task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
    2) Card Slot ID check
    4) Client ID Check
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((card_slot != MMGSDI_SLOT_1) && (card_slot != MMGSDI_SLOT_2))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.acl_operation_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_ACL_OPERATION_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_ACL_OPERATION_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_ACL_OPERATION_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->acl_op                           = MMGSDI_ACL_OP_DISABLE;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
          - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Disable ACL Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Disable ACL command %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
}

/*===========================================================================
FUNCTION MMGSDI_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function finds out if the service is available or not given the
  service_type

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Card Slot
  srvc_type:       Service that the client is interested in
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
    None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_is_service_available (
  mmgsdi_client_id_type               client_id,
  mmgsdi_slot_id_enum_type            card_slot,
  mmgsdi_service_enum_type            srvc_type,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_MMGSDI_MBMS*/
}/*mmgsdi_is_service_available*/ /*lint !e715*/


/*===========================================================================
FUNCTION:      MMGSDI_USIM_AUTHENTICATE

DESCRIPTION:
  This function is used to perform a "Network" Initiated Authentication with
  the USIM Application.  This is only valid and applicable when an USIM
  Application is available . This function will perform a check for the USIM
  Application before sending the command.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_data_type         auth_req:       Input.  Length and Data of auth
                                                   challenge.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

  if Context is MMGSDI_AUTN_USIM_GSM_CONTEXT
    Data should be in the following format -
      Rand Length
      Rand Data

  if Context is MMGSDI_AUTN_USIM_3G_CONTEXT/MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT
    Data should be in the following format -
      Rand Length
      Rand Data
      Auth Length
      Auth Data

  if Context is MMGSDI_AUTN_USIM_VGCS_VBS_CONTEXT
    Data should be in the following format -
      Length of VService_Id
      VService_Id
      Length of VK_Id
      VK_Id
      Length of VSTK_RAND
      VSTK_RAND

   if Context is MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT
    Data should be in the following format -
      NAF ID Length
      NAF Data
      IMPI Length
      IMPI

   if Context is MMGSDI_AUTN_USIM_MBMS_MSK_UPDATE_CONTEXT/
                 MMGSDI_AUTN_USIM_MBMS_MTK_GEN_CONTEXT/
                 MMGSDI_AUTN_USIM_MBMS_MSK_DEL_CONTEXT/
                 MMGSDI_AUTN_USIM_MBMS_MUK_DEL_CONTEXT
    Data should be in the following format -
      MIKEY

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  The client must also have a valid session open if the USIM Application
  is not the default application
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
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_usim_authenticate (
  mmgsdi_client_id_type               client_id,
  mmgsdi_slot_id_enum_type            card_slot,
  mmgsdi_autn_context_enum_type       auth_context,
  mmgsdi_data_type                    auth_req,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MBMS */
}/*mmgsdi_usim_authenticate*/ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_JCDMA_GET_CARD_INFO

DESCRIPTION
  This function will buid a request to the MMGSDI Task to request the jcdma
  card information.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_get_card_info (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
  mmgsdi_task_cmd_type                 * task_cmd_ptr  = NULL;
  mmgsdi_jcdma_get_card_info_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;
  int32                                  total_mem_len = 0;
  uint32                                 task_cmd_len  = 0;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  if ( card_slot != MMGSDI_SLOT_1 &&
       card_slot != MMGSDI_SLOT_2 )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID",0,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate 1 large block of memory for the task_cmd_ptr and for the
     onchip config data.
     NOTE:  Side effect of the following total len is that it may not
            match exactly as a result of mmgsdi_task_cmd_data_type being
            a union. Will be addressed by using the correct type size
            rather than sizeof mmgsdi_task_cmd_data_type
     ------------------------------------------------------------------------*/
  total_mem_len =  sizeof(mmgsdi_task_cmd_data_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);
  /*-----------------------------------------------------------------------------
    No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.jcdma_card_info_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_JCDMA_GET_CARD_INFO_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_JCDMA_GET_CARD_INFO_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_JCDMA_GET_CARD_INFO_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*-----------------------------------------------------------------------------
    Calculate the payload len and request len
    ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len -
                                       uint32toint32(sizeof(mmgsdi_request_header_type));
  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr
    ** has not been put onto the command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Failed to queue MMGSDI_JCDMA_GET_CARD_INFO_REQ", 0, 0, 0);
  }

  MSG_HIGH("JCDMA GET CARD INFO REQUEST 0x%x ",mmgsdi_status,0,0);

  return mmgsdi_status;
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
}/* mmgsdi_jcdma_get_card_info */ /*lint !e715*/


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_INFO

DESCRIPTION:
  This function returns card info.

DEPENDENCIES:

LIMITATIONS:

RETURN VALUE:
   mmgsdi_jcdma_card_info_enum_type
SIDE EFFECTS:
===========================================================================*/
mmgsdi_jcdma_card_info_enum_type mmgsdi_jcdma_card_get_info(void)
{
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
  return mmgsdi_data_slot1.jcdma_card_info;
#else
  return MMGSDI_JCDMA_UNKNOWN_CARD;
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
}


/*===========================================================================
FUNCTION MMGSDI_GET_ESN_ME_CHANGE_FLAG

DESCRIPTION
  This function will return the esn ME change flag obtained during post pin1
  init

PARAMS:
  none

DEPENDENCIES:

LIMITATIONS:

RETURN VALUE:
   mmgsdi_esn_me_change_enum_type:
     MMGSDI_ESN_ME_NO_CHANGE
     MMGSDI_ESN_ME_CHANGE
     MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED

SIDE EFFECTS:
===========================================================================*/
mmgsdi_esn_me_change_enum_type mmgsdi_get_esn_me_change_flag (
  mmgsdi_slot_id_enum_type slot)
{
#ifdef FEATURE_UIM_RUIM
  if (slot == MMGSDI_SLOT_1)
  {
     return mmgsdi_data_slot1.esn_me_change_flag;
  }
#endif /* FEATURE_UIM_RUIM */
  return MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED;
}/* mmgsdi_jcdma_get_esn_me_change_flag */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_COMPUTE_IP_AUTH

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a compute
  IP authentication command for 3GPD SIMPLE IP CHAP, Mobile IP, or HRPD
  CHAP Authentication.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  compute_ip_data:   DATA Required for Compuite IP Command
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

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

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_compute_ip_auth (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_compute_ip_data_type        compute_ip_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
  mmgsdi_task_cmd_type           *task_cmd_ptr  = NULL;
  mmgsdi_compute_ip_req_type     *msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  int32                           var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                           temp_mem      = 0;
  uint32                          task_cmd_len  = 0;
  boolean                         operation_type_ok = FALSE;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/

  card_slot = MMGSDI_SLOT_1;
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
#ifdef FEATURE_UIM_SUPPORT_3GPD
  if((compute_ip_data.operation_type == MMGSDI_CMPT_IP_SIP_CHAP) ||
     (compute_ip_data.operation_type == MMGSDI_CMPT_IP_MIP_MN_HA)||
     (compute_ip_data.operation_type == MMGSDI_CMPT_IP_MIP_RRQ)  ||
     (compute_ip_data.operation_type == MMGSDI_CMPT_IP_MIP_MN_AAA))
  {
    operation_type_ok = TRUE;
  }
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
  if(compute_ip_data.operation_type == MMGSDI_CMPT_IP_HRPD_CHAP)
  {
    operation_type_ok = TRUE;
  }
#endif /* FEATURE_UIM_SUPPORT_HRPD */
  if(!operation_type_ok)
  {
    MSG_ERROR("Invalid operation type",0,0,0);
    return MMGSDI_ERROR;
  }
  switch(compute_ip_data.operation_type)
  {
#ifdef FEATURE_UIM_SUPPORT_3GPD
    case MMGSDI_CMPT_IP_SIP_CHAP:
      if((compute_ip_data.data.sip_chap_data.chap_challenge_length >
          MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH) ||
         (compute_ip_data.data.sip_chap_data.chap_challenge_length <= 0))
      {
        MSG_ERROR("Invalid Length for SIP CHAP",0,0,0);
        return MMGSDI_ERROR;
      }
      var_len[0] = 0;
      break;
    case MMGSDI_CMPT_IP_MIP_MN_HA:
      if((compute_ip_data.data.mn_ha_data.registration_data_length >
          MMGSDI_MAX_3GPD_MN_HA_REG_DATA_LENGTH)||
         (compute_ip_data.data.mn_ha_data.registration_data_length <= 0))
      {
        MSG_ERROR("Invalid Length for MIP MN HA",0,0,0);
        return MMGSDI_ERROR;
      }
      var_len[0] = compute_ip_data.data.mn_ha_data.registration_data_length;
      break;
    case MMGSDI_CMPT_IP_MIP_RRQ:
      if((compute_ip_data.data.rrq_data.rrq_data_length >
          MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH) ||
         (compute_ip_data.data.rrq_data.rrq_data_length <= 0))
      {
        MSG_ERROR("Invalid Length for MIP RRQ HASH",0,0,0);
        return MMGSDI_ERROR;
      }
      var_len[0] = compute_ip_data.data.rrq_data.rrq_data_length;
      break;
    case MMGSDI_CMPT_IP_MIP_MN_AAA:
      if((compute_ip_data.data.mn_aaa_data.challenge_length >
          MMGSDI_3GPD_MAX_MIP_CHALLENGE_LENGTH) ||
         (compute_ip_data.data.mn_aaa_data.challenge_length <= 0))
      {
        MSG_ERROR("Invalid Length for MIP MN AAA",0,0,0);
        return MMGSDI_ERROR;
      }
      var_len[0] = compute_ip_data.data.mn_aaa_data.challenge_length;
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
    case MMGSDI_CMPT_IP_HRPD_CHAP:
      if((compute_ip_data.data.hrpd_chap_data.chap_challenge_length >
          MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH) ||
         (compute_ip_data.data.hrpd_chap_data.chap_challenge_length <= 0))
      {
        MSG_ERROR("Invalid Length for HRPD AN CHAP",0,0,0);
        return MMGSDI_ERROR;
      }
      var_len[0] = compute_ip_data.data.hrpd_chap_data.chap_challenge_length;
      break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */
  }

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
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.compute_ip_req,
                         MMGSDI_COMPUTE_IP_AUTH_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.compute_ip_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_compute_ip_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_COMPUTE_IP_AUTH_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_COMPUTE_IP_AUTH_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_COMPUTE_IP_AUTH_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->compute_ip_data.operation_type   = compute_ip_data.operation_type;

   /*---------------------------------------------------------------------------
    Populating Compute IP DATA
  ---------------------------------------------------------------------------*/
  switch(compute_ip_data.operation_type)
  {
#ifdef FEATURE_UIM_SUPPORT_3GPD
    case MMGSDI_CMPT_IP_SIP_CHAP:
      msg_ptr->compute_ip_data.data.sip_chap_data.nai_entry_index =
        compute_ip_data.data.sip_chap_data.nai_entry_index;
      msg_ptr->compute_ip_data.data.sip_chap_data.chap_id =
        compute_ip_data.data.sip_chap_data.chap_id;
      msg_ptr->compute_ip_data.data.sip_chap_data.chap_challenge_length =
        compute_ip_data.data.sip_chap_data.chap_challenge_length;
      memcpy(msg_ptr->compute_ip_data.data.sip_chap_data.chap_challenge,
             compute_ip_data.data.sip_chap_data.chap_challenge,
             compute_ip_data.data.sip_chap_data.chap_challenge_length);
      break;
    case MMGSDI_CMPT_IP_MIP_MN_HA:
      msg_ptr->compute_ip_data.data.mn_ha_data.nai_entry_index =
        compute_ip_data.data.mn_ha_data.nai_entry_index;
      msg_ptr->compute_ip_data.data.mn_ha_data.registration_data_length =
        compute_ip_data.data.mn_ha_data.registration_data_length;
      memcpy(msg_ptr->compute_ip_data.data.mn_ha_data.registration_data_ptr,
             compute_ip_data.data.mn_ha_data.registration_data_ptr,
             compute_ip_data.data.mn_ha_data.registration_data_length);
      break;
    case MMGSDI_CMPT_IP_MIP_RRQ:
      msg_ptr->compute_ip_data.data.rrq_data.rrq_data_length =
        compute_ip_data.data.rrq_data.rrq_data_length;
      memcpy(msg_ptr->compute_ip_data.data.rrq_data.rrq_data_ptr,
             compute_ip_data.data.rrq_data.rrq_data_ptr,
             compute_ip_data.data.rrq_data.rrq_data_length);
      break;
    case MMGSDI_CMPT_IP_MIP_MN_AAA:
      msg_ptr->compute_ip_data.data.mn_aaa_data.nai_entry_index =
        compute_ip_data.data.mn_aaa_data.nai_entry_index;
      msg_ptr->compute_ip_data.data.mn_aaa_data.challenge_length =
        compute_ip_data.data.mn_aaa_data.challenge_length;
      memcpy(msg_ptr->compute_ip_data.data.mn_aaa_data.challenge_ptr,
             compute_ip_data.data.mn_aaa_data.challenge_ptr,
             compute_ip_data.data.mn_aaa_data.challenge_length);
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
    case MMGSDI_CMPT_IP_HRPD_CHAP:
      msg_ptr->compute_ip_data.data.hrpd_chap_data.chap_id =
        compute_ip_data.data.hrpd_chap_data.chap_id;
      msg_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_length =
        compute_ip_data.data.hrpd_chap_data.chap_challenge_length;
      memcpy(msg_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
        compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
        compute_ip_data.data.hrpd_chap_data.chap_challenge_length);
      break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */
  }

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful Queue of COMPUTE IP AUTH command status %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */
}/* mmgsdi_compute_ip_auth */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_RUN_CAVE_ALGO

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a run cave
  operation in the UIM

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  rand_type:         Type of Random Challenge
  rand_chal:         Rand Chal for CAVE Algo
  dig_len:           Length of digits provided
  digits:            Digits for CAVE calculation
  process_control:   Process Control for CAVE
  esn:               ESN for CAVE Calculation
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

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

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_run_cave_algo (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_cave_rand_type_type         rand_type,
  mmgsdi_cave_rand_chal_type         rand_chal,
  mmgsdi_cave_dig_len_type           dig_len,
  mmgsdi_cave_digits_type            digits,
  mmgsdi_cave_process_control_type   process_control,
  mmgsdi_cave_esn_type               esn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#ifdef FEATURE_MMGSDI_CDMA
  mmgsdi_task_cmd_type           *task_cmd_ptr  = NULL;
  mmgsdi_run_cave_req_type       *msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  int32                           var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                           temp_mem      = 0;
  uint32                          task_cmd_len  = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  card_slot = MMGSDI_SLOT_1;
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

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
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.run_cave_req,
                         MMGSDI_RUN_CAVE_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.run_cave_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_compute_ip_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_RUN_CAVE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_RUN_CAVE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_RUN_CAVE_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
   /*---------------------------------------------------------------------------
    Populating Compute IP DATA
  ---------------------------------------------------------------------------*/
  msg_ptr->dig_len                          = dig_len;
  memcpy(msg_ptr->digits, digits, sizeof(mmgsdi_cave_digits_type));
  qw_equ(msg_ptr->esn, esn);
  msg_ptr->process_control                  = process_control;
  msg_ptr->rand_chal                        = rand_chal;
  msg_ptr->rand_type                        = rand_type;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful Queue of RUN CAVE command status %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA */
}/* mmgsdi_run_cave_algo */ /*lint !e715*/ /*lint !e818*/


/*===========================================================================
FUNCTION MMGSDI_GENERATE_KEY_VPM

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a generate
  key/VPM command in the UIM.  This returns a key value, and a set of VPM
  (Voice Privacy Mask) octets from the card.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  first_octet:       First Octet of VPM to be output
  last_octet:        Second Octet of VPM to be output
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_AUTH_ERROR_CAVE_NOT_RUN:  The command cannot be called unless
                             the RUN CAVE command has already been run with
                             SAVE Registers turned on

SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_generate_key_vpm (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_single_vpm_octet_type       first_octet,
  mmgsdi_single_vpm_octet_type       last_octet,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#ifdef FEATURE_MMGSDI_CDMA
  mmgsdi_task_cmd_type           *task_cmd_ptr  = NULL;
  mmgsdi_generate_key_req_type   *msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  int32                           var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                           temp_mem      = 0;
  uint32                          task_cmd_len  = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  if(card_slot != MMGSDI_SLOT_1)
  {
    MSG_ERROR("Card Slot is not 1 for MMGSDI_GENERATE_KEY_VPM!",0,0,0);
    card_slot = MMGSDI_SLOT_1;
  }
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID 0x%x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len[0],
                              &temp_mem);
  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  /* 'task_cmd_len' could possibly roll-over to zero since it is addition
     of two numbers */
  if(task_cmd_len == 0)
  {
    MSG_ERROR("Can not allocate zero length memory to task_cmd_ptr", 0, 0, 0);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.generate_key_req,
                         MMGSDI_GENERATE_KEY_VPM_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.generate_key_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_generate_key_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GENERATE_KEY_VPM_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_GENERATE_KEY_VPM_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GENERATE_KEY_VPM_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
   /*---------------------------------------------------------------------------
    Populating Generate Key DATA
  ---------------------------------------------------------------------------*/
  msg_ptr->first_octet                      = first_octet;
  msg_ptr->last_octet                       = last_octet;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful Queue of GENERATE KEY command status %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA */
}/* mmgsdi_generate_key_vpm */


/* ============================================================================
FUNCTION MMGSDI_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Archived by calling mmgsdi_client_id_reg().

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
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_option_type       option,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_task_cmd_type      * task_cmd_ptr  = NULL;
  mmgsdi_get_atr_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;
  int32                       total_mem_len = 0;
  uint32                      task_cmd_len  = 0;

  /* -------------------------------------------------------------------------
     Validate the Input Parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(card_slot, MMGSDI_SLOT_1, MMGSDI_SLOT_2);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,task_cmd_len);

  /* --------------------------------------------------------------------------
    No need to Align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.get_atr_req;

  /*
   * Populate header.
   */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.request_type       = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.orig_request_type  = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.request_id_ptr     = rex_self();
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
    ** command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing MMGSDI_GET_ATR_REQ Fail 0x%x",
      mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue MMGSDI_GET_ATR_REQ command ",0,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_get_atr */ /*lint !e715*/


/* ============================================================================
FUNCTION MMGSDI_GET_CARD_READER_STATUS

DESCRIPTION
  This Function is used to retrieve the Card Reader Status.  This interface
  is occassionally referred to as a card reader.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Archived by calling mmgsdi_client_id_reg().

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
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_get_card_reader_status(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_option_type        option,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
)
{
  mmgsdi_task_cmd_type               * task_cmd_ptr  = NULL;
  mmgsdi_card_reader_status_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type              mmgsdi_status  = MMGSDI_ERROR;
  int32                                total_mem_len  = 0;
  uint32                               task_cmd_len   = 0;

  /* -------------------------------------------------------------------------
     Validate the Input Parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(card_slot, MMGSDI_SLOT_1, MMGSDI_SLOT_2);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,task_cmd_len);

  /* --------------------------------------------------------------------------
    No need to align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.card_reader_status_req;

  /* --------------------------------------------------------------------------
     Populating mmgsdi_sap_card_reader_status_req_type header.
     ----------------------------------------------------------------------- */
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.client_id           = client_id;
  msg_ptr->request_header.request_type        = MMGSDI_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.orig_request_type   = MMGSDI_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.slot_id             = card_slot;
  msg_ptr->request_header.request_id_ptr      = rex_self();
  msg_ptr->request_header.client_data         = client_ref;
  msg_ptr->request_header.response_cb         = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
    - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing MMGSDI_CARD_READER_STATUS_REQ Fail 0x%x",
      mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue MMGSDI_CARD_READER_STATUS_REQ command ",0,0,0);
  }

  return mmgsdi_status;
}/* mmgsdi_card_reader_status */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SEND_APDU

DESCRIPTION
  This function will send an APDU to the card.  This will cause MMGSDI
  to generate a get response as needed for the client to handle.  If the
  Get Response does not occur when the client is notified, then the data
  considered part of the get response is discarded.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  apdu_data:       The APDU to send to the card.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Archived by calling mmgsdi_client_id_reg().

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
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  mmgsdi_send_apdu_data_type     apdu_data,
  mmgsdi_option_type             option,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr = NULL;
  mmgsdi_send_apdu_req_type     * msg_ptr      = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  int32                          total_mem_len = 0;
  int32                          var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                          temp_mem      = 0;
  uint32                         task_cmd_len  = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /* -------------------------------------------------------------------------
     Validate the Input Parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(card_slot, MMGSDI_SLOT_1, MMGSDI_SLOT_2);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* APDU DATA Must not be NULL */
  MMGSDIUTIL_RETURN_IF_NULL(apdu_data.data_ptr);

  /* APDU DATA Len must be within a certain range */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(apdu_data.data_len,
                                    MMGSDI_MIN_APDU_LEN,
                                    GSDI_MAX_DATA_BLOCK_LEN);

  /* If the APDU is a Select Command, it must meet certain length
  ** Requirements
  */
  if ((apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == SELECT) &&
      (apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET] == UIM_7816_APDU_MIN_SIZE) &&
     ((apdu_data.data_len < MMGSDI_SELECT_APDU_MIN_LEN) ||
      (apdu_data.data_ptr[MMGSDI_SELECT_APDU_LEN_OFFSET] < MMGSDI_SELECT_APDU_MIN_LEN)))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x", client_id, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Calculate the Total Length of this command */
  var_len[0] = apdu_data.data_len;

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  mmgsdi_util_cal_align_size(var_len[0],
                              &temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len[0],temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = int32touint32(total_mem_len) + sizeof(q_link_type);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.send_apdu_req, MMGSDI_SEND_APDU_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.send_apdu_req;

  /*---------------------------------------------------------------------------
    Populate header.
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SEND_APDU_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SEND_APDU_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SEND_APDU_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  /*---------------------------------------------------------------------------
     Populate the payload.
  ---------------------------------------------------------------------------*/
  msg_ptr->data.data_len = apdu_data.data_len;
  memcpy(msg_ptr->data.data_ptr,
         apdu_data.data_ptr,
         int32touint32(msg_ptr->data.data_len));

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    Put on mmgsdi command queue.
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Send APDU command %d",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_send_apdu */ /*lint !e715*/


/*==================================================================
FUNCTION MMGSDI_MFLO_INIT_ACT_PARAMS

DESCRIPTION
  This function will build a MMGSDI_MFLO_INIT_ACT_PARAMS request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  act_params:        Activation parameters, should be encrypted.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_init_act_params (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_data_type                   act_params,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MFLO */
} /* mmgsdi_mflo_init_act_params */ /*lint !e715*/

/*==================================================================
FUNCTION MMGSDI_MFLO_GET_SUBSCRIBER_ID

DESCRIPTION
  This function will build a MMGSDI_MFLO_GET_SUBSCRIBER_ID request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_get_subscriber_id (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MFLO */
} /* mmgsdi_mflo_get_subscriber_id */ /*lint !e715*/


/*==================================================================
FUNCTION MMGSDI_MFLO_GET_PUBLIC_KEY

DESCRIPTION
  This function will build a MMGSDI_MFLO_GET_PUBLIC_KEY request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_get_public_key (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MFLO */
} /* mmgsdi_mflo_get_public_key */ /*lint !e715*/


/*==================================================================
FUNCTION MMGSDI_MFLO_SIGN_UCAST

DESCRIPTION
  This function will build a MMGSDI_MFLO_SIGN_UCAST request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  msg_type:          Type of unicast message block.
  msg:               Message to be signed
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_sign_ucast (
  mmgsdi_client_id_type                     client_id,
  mmgsdi_slot_id_enum_type                  card_slot,
  mmgsdi_mflo_ucast_msg_enum_type           msg_type,
  mmgsdi_data_type                          msg,
  mmgsdi_option_type                        option,
  mmgsdi_callback_type                      response_cb_ptr,
  mmgsdi_client_data_type                   client_ref
)
{
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MFLO */
} /* mmgsdi_mflo_sign_ucast */ /*lint !e715*/


/*==================================================================
FUNCTION MMGSDI_MFLO_VERIFY_UCAST

DESCRIPTION
  This function will build a MMGSDI_MFLO_VERIFY_UCAST request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  msg_type:          Type of unicast message block.
  msg:               Message to be verified.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

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
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_verify_ucast (
  mmgsdi_client_id_type                     client_id,
  mmgsdi_slot_id_enum_type                  card_slot,
  mmgsdi_mflo_ucast_msg_enum_type           msg_type,
  mmgsdi_data_type                          msg,
  mmgsdi_option_type                        option,
  mmgsdi_callback_type                      response_cb_ptr,
  mmgsdi_client_data_type                   client_ref
)
{
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MFLO */
} /* mmgsdi_mflo_verify_ucast */ /*lint !e715*/


/*===========================================================================
   FUNCTION MMGSDI_NAA_REFRESH

   DESCRIPTION
     This API is used by the Generic SIM Toolkit Task to request that a REFRESH
     be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
     Refresh Modes (as defined by the enum mmgsdi_refresh_mode_enum_type).

     MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
     MMGSDI_REFRESH_FCN                    (SIM/USIM)
     MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
     MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
     MMGSDI_REFRESH_RESET                  (SIM/USIM)
     MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
     MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)

   PARAMETERS:
     client_id:          Client ID of the caller
     card_slot:          Slot on which refresh has to be performed
     aid:                Application ID (only applicable to 3G)
     refresh_mode:       Refresh Mode as sent by the card
     refresh_files:      Refresh Files if any (optional)
     num_files:          Number of refresh files
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         registration request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.
   DEPENDENCIES
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS
     Depending on the Mode requested, various events are generated as a result
     of the REFRESH Peformed.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_naa_refresh(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_aid_type                    aid,
  mmgsdi_refresh_mode_enum_type      refresh_mode,
  mmgsdi_data_type                   refresh_files,
  uint8                              num_files,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
   mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
   mmgsdi_refresh_req_type req;

   req.refresh_req_type = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
   memcpy(&req.refresh.card_req.aid, &aid, sizeof(mmgsdi_aid_type));
   memcpy(&req.refresh.card_req.refresh_files,
          &refresh_files,
          sizeof(mmgsdi_data_type));

   req.refresh.card_req.refresh_mode = refresh_mode;
   req.refresh.card_req.num_files    = num_files;

   mmgsdi_status = mmgsdi_build_refresh_req( client_id,
                                             card_slot,
                                             &req,
                                             response_cb_ptr,
                                             client_ref);

   if ((mmgsdi_status != MMGSDI_SUCCESS) &&
       (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS))
   {
     MMGSDI_DEBUG_MSG_ERROR("UIM Did not accept reset(%u)", mmgsdi_status, 0, 0);
   }
   return mmgsdi_status;
}/*mmgsdi_naa_refresh*/ /*lint !e715*/


/*=============================================================================
    FUNCTION MMGSDI_REGISTER_FOR_REFRESH

    Description
      This function allows the client register for File change notifications
      for specific files  and/or vote for refresh ( i.e. vote for either taking
      the call stack down or terminating application that the client is
      interested in). When the FCN request is received, then MMGSDI will send
      an event MMGSDI_REFRESH_EVT with mode = MMGSDI_REFRESH_FCN with the file
      list that has changed. MMGSDI will filter out the file list based on what
      the client registered for.
      Some FCNs on default could potentially result in bringing the protocol
      stack down - if the client is interested in these files, then they should
      also register for voting explicitly on default app.

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part
      of the client_id_table, and failure will be returned in the response.
      Achived by calling mmgsdi_client_id_reg().

      To receive events from MMGSDI, client should register for events also with
      MMGSDI by calling mmgsdi_client_evt_reg.


    PARAMS:
      client_id:       Client Id of the caller
      card_slot:       Physical slot for multiple slot targets.
      refresh_files:   Files that the Client is interested in getting
                       notification for
      vote_for_init:   Vote to ok bringing the call stack down/terminating
                       the application
      option:          optional parameter, not used right now
      response_cb_ptr: Callback to this command
      client_ref:      User Data returned upon completion of this cmd.

    LIMITATIONS:
      This API will be limited to the session on which the client is.
      If the client is interested in default app, then the appropriate client
      id should be used.

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
      None
    SIDE EFFECTS:
      Will result in your callback getting called everytime MMGSDI Needs to
      notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_register_for_refresh (
   mmgsdi_client_id_type              client_id,
   mmgsdi_slot_id_enum_type           card_slot,
   mmgsdi_refresh_file_list_type      refresh_files,
   boolean                            vote_for_init,
   mmgsdi_option_type                 option,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};
  uint32                  i             = 0;
  uint8                  *temp_path_ptr = NULL;
  int32                   temp_len      = 0;

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_REG_REQ;

  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(refresh_files.file_list_ptr,
    refresh_files.file_path_ptr, mmgsdi_status);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(refresh_files.file_list_ptr != NULL)
  {
    req.refresh.reg_req.refresh_files.file_list_ptr =
      refresh_files.file_list_ptr;
    req.refresh.reg_req.refresh_files.num_files =
      refresh_files.num_files;

    /* set file path ptr to NULL and num_file_paths to 0 */
    req.refresh.reg_req.refresh_files.file_path_ptr = NULL;
    req.refresh.reg_req.refresh_files.num_file_paths = 0;
  }

  /* copy the file path */
  if(refresh_files.file_path_ptr != NULL)
  {
    /* Assign file path pointer to request structure, allocate temporary
       buffer to account for number of path as first byte */
    /* require this temp to satisfy lint and compilation warning fixes */
    temp_len = uint32toint32((refresh_files.file_path_len + 1) * sizeof(uint8));

    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(temp_path_ptr,
                                              temp_len);

    /* Determine the number of file path */
    for (i = 0; i < (refresh_files.file_path_len - 1); i+=2)
    {
      if ((refresh_files.file_path_ptr[i] == MMGSDIUTIL_MF_HIGH_BYTE) &&
          (refresh_files.file_path_ptr[i+1] == 0x00))
      {
        temp_path_ptr[0] += 1;
      }
    }
    memcpy(&temp_path_ptr[1],
           refresh_files.file_path_ptr,
           refresh_files.file_path_len);

    /* Convert file data into file path array */
    mmgsdi_status = mmgsdi_util_convert_to_refresh_file_paths(
                      refresh_files.file_path_len+1,
                      temp_path_ptr,
                      &req.refresh.reg_req.refresh_files.file_path_ptr,
                      (uint8*)&req.refresh.reg_req.refresh_files.num_file_paths);

    MMGSDIUTIL_TMC_MEM_FREE(temp_path_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not decode and copy file path",0,0,0);
      return MMGSDI_ERROR;
    }

    /* Set file enum list ptr to NULL and num files to  0*/
    req.refresh.reg_req.refresh_files.file_list_ptr = NULL;
    req.refresh.reg_req.refresh_files.num_files = 0;
  }

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_REG_REQ;
  req.refresh.reg_req.vote_for_init = vote_for_init;

  mmgsdi_status = mmgsdi_build_refresh_req( client_id,
                                            card_slot,
                                            &req,
                                            response_cb_ptr,
                                            client_ref);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Command Queuing Failed 0x%x ,mmgsdi_register_for_refresh",
                mmgsdi_status, 0, 0);
  }

  /* free the file_path if required */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    req.refresh.reg_req.refresh_files.file_path_ptr);

   return mmgsdi_status;
}/*mmgsdi_register_for_refresh*/ /*lint !e715*/


/*=============================================================================
    FUNCTION MMGSDI_DEREGISTER_FOR_REFRESH

    Description
      This function allows the client deregister for File change notifications
      for specific files or voting or both

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part
      of the client_id_table, and failure will be returned in the response.
      Achived by calling mmgsdi_client_id_reg().

    PARAMS:
      client_id:        Client Id of the caller
      card_slot:        Physical slot for multiple slot targets.
      Refresh_files:    Files that the Client is no longer interested in
                        getting notification for
      vote_for_init:    Deregister from Voting,
                        FALSE ? NO changes to Current Registration,
                        TRUE ? DeRegister from Voting
      option       :   optional parameter, not used right now
      Response_cb_ptr:  Callback to this command
      client_ref:       User Data returned upon completion of this cmd.

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
         Will result in your callback getting called everytime MMGSDI Needs to
         notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_deregister_for_refresh (
   mmgsdi_client_id_type              client_id,
   mmgsdi_slot_id_enum_type           card_slot,
   mmgsdi_refresh_file_list_type      refresh_files,
   boolean                            vote_for_init,
   mmgsdi_option_type                 option,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};
  uint32                  i             = 0;
  uint8                  *temp_path_ptr = NULL;
  int32                   temp_len      = 0;

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_DEREG_REQ;

  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(refresh_files.file_list_ptr,
    refresh_files.file_path_ptr, mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* copy the file list */
  if(refresh_files.file_list_ptr != NULL)
  {
    req.refresh.reg_req.refresh_files.file_list_ptr =
      refresh_files.file_list_ptr;
    req.refresh.reg_req.refresh_files.num_files =
      refresh_files.num_files;

    /* set file path ptr to NULL and num_file_paths to 0 */
    req.refresh.reg_req.refresh_files.file_path_ptr =
      NULL;
    req.refresh.reg_req.refresh_files.num_file_paths = 0;
  }

  /* copy the file path */
  if(refresh_files.file_path_ptr != NULL)
  {
    /* Assign file path pointer to request structure, allocate temporary
       buffer to account for number of path as first byte */
    /* require this temp to satisfy lint and compilation warning fixes */
    temp_len = uint32toint32((refresh_files.file_path_len + 1) * sizeof(uint8));

    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(temp_path_ptr,
                                              temp_len);
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(temp_path_ptr, temp_len);

    /* Determine the number of file path */
    for (i = 0; i < (refresh_files.file_path_len - 1); i+=2)
    {
      if ((refresh_files.file_path_ptr[i] == MMGSDIUTIL_MF_HIGH_BYTE) &&
          (refresh_files.file_path_ptr[i+1] == 0x00))
      {
        temp_path_ptr[0] += 1;
      }
    }
    memcpy(&temp_path_ptr[1],
           refresh_files.file_path_ptr,
           refresh_files.file_path_len);

    /* Convert file data into file path array */
    mmgsdi_status = mmgsdi_util_convert_to_refresh_file_paths(
                      refresh_files.file_path_len+1,
                      temp_path_ptr,
                      &req.refresh.reg_req.refresh_files.file_path_ptr,
                      (uint8*)&req.refresh.reg_req.refresh_files.num_file_paths);

    MMGSDIUTIL_TMC_MEM_FREE(temp_path_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Refresh: Could not decode and copy file path",0,0,0);
      return MMGSDI_ERROR;
    }

    /* Set file enum list ptr to NULL and num files to  0*/
    req.refresh.reg_req.refresh_files.file_list_ptr = NULL;
    req.refresh.reg_req.refresh_files.num_files = 0;
  }

  req.refresh.reg_req.vote_for_init = vote_for_init;

  mmgsdi_status = mmgsdi_build_refresh_req( client_id,
                                            card_slot,
                                            &req,
                                            response_cb_ptr,
                                            client_ref);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Command Queuing Failed 0x%x, mmgsdi_deregister_for_refresh",
               mmgsdi_status, 0, 0);
  }

  /* free the file_path if required */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    req.refresh.reg_req.refresh_files.file_path_ptr);

  return mmgsdi_status;
}/*mmgsdi_deregister_for_refresh*/ /*lint !e715*/


/*=============================================================================
    FUNCTION MMGSDI_REFRESH_COMPLETE

    DESCRIPTION
      This function allows the client to inform MMGSDI when it has finished all
      the files that were refreshed.

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part of
      the client_id_table, and failure will be returned in the response.
      Achieved by calling mmgsdi_client_id_reg(). The client should have also
      registered for FCNs and should have also received a notification for FCN
      before they call this function.

    PARAMS:
      client_id:      Client Id of the caller
      card_slot:      Physical slot for multiple slot targets.
      pass_fail:      Boolean indicating whether client was able to
                      successfully read all the files that were refreshed
                      This could also be applicable to init cases, where the
                      client would inform MMGSDI that it was able to perform
                      its init properly
      option:         optional parameter, not used right now
      response_cb_ptr:Callback to this command
      client_ref:     User Data returned upon completion of this cmd.

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
         Will result in your callback getting called everytime MMGSDI Needs to
         notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_complete (
   mmgsdi_client_id_type              client_id,
   mmgsdi_slot_id_enum_type           card_slot,
   boolean                            pass_fail,
   mmgsdi_option_type                 option,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref)
{
   mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
   mmgsdi_refresh_req_type req           = {{0}};

   req.refresh_req_type = MMGSDI_REFRESH_ORIG_COMPLETE_REQ;

   req.refresh.pass_fail = pass_fail;

   mmgsdi_status = mmgsdi_build_refresh_req( client_id,
                                             card_slot,
                                             &req,
                                             response_cb_ptr,
                                             client_ref);

   if (mmgsdi_status != MMGSDI_SUCCESS)
   {
     MSG_ERROR("Command Queuing Failed 0x%x, mmgsdi_refresh_complete",
                mmgsdi_status, 0, 0);
   }
   return mmgsdi_status;
}/*mmgsdi_refresh_complete*/ /*lint !e715*/


/*=============================================================================
    FUNCTION MMGSDI_OK_TO_REFRESH

    DESCRIPTION
      This function will have to be called by clients if they get a Refresh
      event (REFRESH_STAGE_WAIT_FOR_OK_TO_INIT,
      REFRESH_STAGE_WAIT_FOR_OK_TO_FCN). Unless this API is called, the refresh
      procedures will not start. With this API call, the client will be able to
      inform MMGSDI if it is ok to terminate the application that they are
      communicating with OR it is ok to continue with File Change.

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part of
      the client_id_table, and failure will be returned in the response.
      Achieved by calling mmgsdi_client_id_reg(). The client should have also
      registered for FCNs and should have also received a notification for FCN
      before they call this function.
      The client must register with mmgsdi to receive events.

    PARAMS:
      client_id:       Client Id of the caller
      card_slot:       Physical slot for multiple slot targets.
      Ok_to_refresh:   Client informs MMGSDI if it is ok to continue with
                       refresh or not. For clients on default application this
                       would imply bringing the stack down and for others it
                       would apply to app termination
      option:          optional parameter, not used right now
      Response_cb_ptr: Callback to this command
      client_ref:      User Data returned upon completion of this cmd.

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
      Will result in your callback getting called everytime MMGSDI Needs to
      notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_ok_to_refresh (
   mmgsdi_client_id_type          client_id,
   mmgsdi_slot_id_enum_type       card_slot,
   boolean                        ok_to_refresh,
   mmgsdi_option_type             option,
   mmgsdi_callback_type           response_cb_ptr,
   mmgsdi_client_data_type        client_ref)
{
  mmgsdi_refresh_req_type req  = {{0}};

   req.refresh_req_type = MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ;
   req.refresh.ok_req   = ok_to_refresh;

   return mmgsdi_build_refresh_req( client_id,
                                    card_slot,
                                    &req,
                                    response_cb_ptr,
                                    client_ref);

}/*mmgsdi_ok_to_refresh*/ /*lint !e715*/


/*=============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_REG_SET_NOTIFY

   DESCRIPTION:
     This function allows client registration for the use of MMGSDI API.
     In addition to client registration, it also allows client to tell
     MMGSDI that MMGSDI will be notified whenever the client finishes
     its initialization. This will be useful in determining when to send
     TR for Refresh.

     For clients that do not want to inform MMGSDI, can still use this API and
     set notify_init to FALSE.

     The response to this request will return client ID to the calling task that
     is required to be used for subsequent MMGSDI API calls.

   PARAMETERS:
     notify_init:        Client will also inform MMGSDI when its init completes
     option:             Optional parameter, not used right now
     response_cb_ptr:    Callback for MMGSDI to call upon completion of the
                         registration request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to
     notify any client of any events.
     The longer the client init takes, the longer it could take to send the
     TR for refresh. Also, if the client init takes very long, then it could
     result in the timer expiring and thus sending the TR at the wrong time.
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type  mmgsdi_client_id_reg_set_notify (
  boolean                    notify_init,
  mmgsdi_option_type         option,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_client_id_reg_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /* No Need to Align Pointer inside client ID Reg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_reg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_ID_REG_REQ;
  msg_ptr->request_header.client_id         = 0;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_ID_REG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_ID_REG_REQ;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_req_type content
    - Event call back (NULL)
    - Client Registration for API
  ---------------------------------------------------------------------------*/
  msg_ptr->evt_cb_ptr  = NULL;
  msg_ptr->notify_init = notify_init;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Failed 0x%x, mmgsdi_client_id_reg_set_notify",
               mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of CLIENT REG command %d with notifiy init 0x%x",
              mmgsdi_status,notify_init,0);
  }

  return mmgsdi_status;
} /* mmgsdi_client_id_reg_set_notify */ /*lint !e715*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*=============================================================================
   FUNCTION:      MMGSDI_CLIENT_INIT_COMPLETE

   DESCRIPTION:
     This function will be called by the clients when they finish their
     initialization. This will be applicable to only those clients that
     register using  mmgsdi_client_id_reg_set_notify and notify_init set to
     TRUE.

   PARAMETERS:
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         registration request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to
     notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_client_init_complete (
  mmgsdi_client_id_type      client_id,
  mmgsdi_slot_id_enum_type   card_slot,
  mmgsdi_option_type         option,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_client_id_reg_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /* No Need to Align Pointer inside client ID Reg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_reg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_reg_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Request Task TCB
      5) Client Data Pointer
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_INIT_COMP_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_INIT_COMP_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_INIT_COMP_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                           - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
       command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Failed 0x%x, mmgsdi_client_init_complete",
                mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of CLIENT INIT COMPLETE command %d",
              mmgsdi_status,0,0);
  }

  return mmgsdi_status;
} /* mmgsdi_client_init_complete */ /*lint !e715*/


/*===========================================================================
FUNCTION:      MMGSDI_OMA_BCAST

DESCRIPTION:
  This function is used to perform certain OMA Bcast specific commands.

  SPE Audit Mode is used by the terminal to retrieve in the Smartcard the
  SEK/PEK ID Key group or all SPE instances corresponding to a specific
  SEK/PEK ID Key group.

  Record Signalling Mode is used by the terminal to signal to the Smartcard
  the recording of content and to allow the Smartcard to retrieve the
  SEK/PEKs used for the protection of this recorded content.

  Recording Audit Mode is used by the terminal to retrieve all the recordings
  that have been signalled in the Smartcard and their associated flagged SPEs.


  Description of the parameters:
     client_id:          Client ID of the caller
     card_slot:          Slot on which command has to be sent
     mode:               Mode in which the command will operate
     bcast_req:          Input data
     option:             Not used yet
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.


DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().


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
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_oma_bcast (
  mmgsdi_client_id_type               client_id,
  mmgsdi_slot_id_enum_type            card_slot,
  mmgsdi_bcast_mode_enum_type         mode,
  mmgsdi_data_type                    bcast_req,
  mmgsdi_option_type                  option,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MBMS */
}/*mmgsdi_oma_bcast*/ /*lint !e715*/


/*===========================================================================
  FUNCTION MMGSDI_SEEK

  DESCRIPTION:
    This function performs GSM Seek command on card.
    The input parameters contain following information.

  PARAMETRES:
    client_id               : Client ID of the caller
    card_slot               : Slot on which command has to be sent
    file_access             : File Information
    seek_direction          : Search Direction
    seek_pattern            : Search Data
    option                  : RFU
    response_cb_ptr         : Callback for MMGSDI to call  upon completion
                              of the request.
    client_ref              : User Data returned to the user upon completion
                              of the command.
  DEPENDENCIES
    None

  LIMITATIONS:
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are
                              not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL   : The command was not queued to the MMGSDI Task
                              because the max numbers of commands are already
                              queued.
    MMGSDI_NOT_SUPPORTED    : when card is not ICC card.

  SIDE EFFECTS
    None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_seek(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        card_slot,
  mmgsdi_access_type              file_access,
  mmgsdi_seek_direction_enum_type seek_direction,
  mmgsdi_seek_data_type           seek_pattern,
  mmgsdi_option_type              option,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
)
{
  mmgsdi_task_cmd_type       * task_cmd_ptr  = NULL;
  mmgsdi_search_req_type     * msg_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;
  int32                        total_mem_len = 0;
  uint32                       task_cmd_len  = 0;
  int32                        var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                        temp_mem      = 0;

  /* Void out unused input params */
  (void)option;
  /*---------------------------------------------
    Input Parameters Checking
   -----------------------------------------------*/
  /* client_id checking */
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID 0x%x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }/* End of if (mmgsdi_status != MMGSDI_SUCCESS)*/

  /* card_slot checking */
  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);

  /* file_access checking */
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
    (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    MSG_ERROR("Invalid Access Method Type 0x%x",
               file_access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* seek_direction checking */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(seek_direction,
                                    MMGSDI_SEEK_BEGINNING_FORWARDS,
                                    MMGSDI_SEEK_END_BACKWARDS);

  /* seek_pattern checking */
  MMGSDIUTIL_RETURN_IF_EXCEEDS(seek_pattern.data_len, MMGSDI_MAX_SEEK_PATTERN_LENGTH);

  /*Option: RFU, Checking not required */
  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* client_ref checking Not required */
  /*------------------------------------------------------------------------
  - Allocating memory to Task Command Pointer
  -------------------------------------------------------------------------*/
  var_len[0] = seek_pattern.data_len;
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len[0],&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len[0],temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,task_cmd_len);
  /*------------------------------------------------------------------------
    - Align pointers of the various structures members
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.search_req, MMGSDI_SEARCH_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SEARCH_REQ;
  msg_ptr = &task_cmd_ptr->cmd.cmd.search_req;

  /*------------------------------------------------------------------------
    Populate Search Request::
    Updating request_header:
      client_id:
      orig_request_type:
      request_type:
      request_len:
      payload_len:
      *request_id_ptr:
      slot_id:
      client_data:
      response_cb:
    Updating access:
    Updating search_type:
    Updating rec_num:
    Updating seek_direction:
    Updating search_direction:
    Updating offset:
    Updating data:
 -------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.orig_request_type  = MMGSDI_SEARCH_REQ;
  msg_ptr->request_header.request_type       = MMGSDI_SEARCH_REQ;
  msg_ptr->request_header.request_len        = total_mem_len;
  msg_ptr->request_header.payload_len        = total_mem_len
                        - uint32toint32(sizeof(mmgsdi_request_header_type));
  msg_ptr->request_header.request_id_ptr     = rex_self();
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;

  /* Updating access  */
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Copying of Access Type is Fail in MMGSDI_SEEK",0,0,0);
    mmgsdi_util_print_file_information(file_access,TRUE);
    return mmgsdi_status;
  }

  /* Updating search_type */
  msg_ptr->search_type                       = MMGSDI_ICC_SEARCH;
  /* Updating rec_num to default value: Not used for mmgsdi seek */
  msg_ptr->rec_num                           = 0x00;
  /* Updating seek_direction */
  msg_ptr->seek_direction                     = seek_direction;
  /* Updating search_direction: Not used for mmgsdi_seek */
  msg_ptr->search_direction                   = MMGSDI_SEARCH_DIRECTION_NONE;
  /* Updating offset: Not used for mmgsdi_seek */
  msg_ptr->enhanced_search_offset.offset_type = MMGSDI_SEARCH_OPTION_NONE;
  msg_ptr->enhanced_search_offset.offset_data = 0x00;
  /* Updating data */
  msg_ptr->data.data_len                      = seek_pattern.data_len;
  memcpy(msg_ptr->data.data_ptr,
         seek_pattern.data_ptr,
         int32touint32(msg_ptr->data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of Seek command %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;

}/* end of mmgsdi_seek function */


/*===========================================================================
  FUNCTION MMGSDI_SEARCH

  DESCRIPTION
    This function performs Search command on UICC card.
    The input parameters contain following information.

  PARAMETER:
    client_id                 : Client ID of the caller
    card_slot                 : Slot on which command has to be sent
    file_access               : File Information
    search_type               : UICC Search Type
                                Valid Range:
                                  MMGSDI_UICC_ENHANCE_SEARCH
                                  MMGSDI_UICC_SIMPLE_SEARCH
    search_record_num         : Record Number, from where search will
                                start in file.
                                Valid Range:
                                   0x01-0xFE: Physical record Number
    search_direction          : Search Direction
    enhance_search_offset_data: Offset, from where search start in a record.
                                Valid Range for offset data
                                  0x00-0xFF for MMGSDI_SEARCH_BY_CHAR type.
                                  0x00-0xFE for MMGSDI_SEARCH_BY_OFFSET type.
    search_pattern            : Data to be search
    option                    : RFU
    response_cb_ptr           : Callback for MMGSDI to call  upon completion
                                of the request.
    client_ref                : User Data returned to the user upon completion
                                of the command.

  DEPENDENCIES
    None

  LIMITATIONS:
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS           : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS  : The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL    : The command was not queued to the MMGSDI Task
                               because the max numbers of commands are already
                               queued.
    MMGSDI_NOT_SUPPORTED     : when card is not UICC card.

  SIDE EFFECTS
    None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_search(
  mmgsdi_client_id_type             client_id,
  mmgsdi_slot_id_enum_type          card_slot,
  mmgsdi_access_type                file_access,
  mmgsdi_search_enum_type           search_type,
  mmgsdi_rec_num_type               search_record_num,
  mmgsdi_search_direction_enum_type search_direction,
  mmgsdi_search_offset_data_type    enhance_search_offset_data,
  mmgsdi_search_data_type           search_pattern,
  mmgsdi_option_type                option,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type       * task_cmd_ptr  = NULL;
  mmgsdi_search_req_type     * msg_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;
  int32                        total_mem_len = 0;
  uint32                       task_cmd_len  = 0;
  int32                        var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                        temp_mem      = 0;

  /* Void out unused input params */
  (void)option;

  /*---------------------------------------------
    Input Parameters Checking
   -----------------------------------------------*/
  /* client_id checking */
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID 0x%x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }/* End of if (mmgsdi_status != MMGSDI_SUCCESS)*/

  /* card_slot checking */
  card_slot = MMGSDI_SLOT_1;
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(card_slot, TRUE);

  /* file_access checking */
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
    (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    MSG_ERROR("Invalid Access Method Type 0x%x", file_access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  /* search_type checking */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(search_type,
                                    MMGSDI_UICC_SIMPLE_SEARCH,
                                    MMGSDI_UICC_ENHANCED_SEARCH);

  /* search_record_num checking */
  MMGSDIUTIL_RETURN_IF_NEGATIVE(search_record_num);
  MMGSDIUTIL_RETURN_IF_EXCEEDS(search_record_num, MMGSDI_MAX_SEARCH_RECORD_NUMBER);
  if (0x00 == search_record_num)
  {
    MSG_ERROR("Invalid record number 0x00", 0, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* search_direction checking */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(search_direction,
                                    MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,
                                    MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM);

  /* enhance_search_offset_data checking */
  if (MMGSDI_UICC_ENHANCED_SEARCH == search_type )
  {
    switch(enhance_search_offset_data.offset_type )
    {
      case MMGSDI_SEARCH_BY_OFFSET:
        MMGSDIUTIL_RETURN_IF_EXCEEDS(enhance_search_offset_data.offset_data,
                                     MMGSDI_MAX_ENHANCED_SEARCH_RECORD_OFFSET);
        break;

      case MMGSDI_SEARCH_BY_CHAR:
        MMGSDIUTIL_RETURN_IF_EXCEEDS(enhance_search_offset_data.offset_data,
                                     MMGSDI_MAX_ENHANCED_SEARCH_CHARACTER_VALUE);
        break;

      default:
        MSG_ERROR("Invalid Offset Type for enchance search 0x%x",
                   enhance_search_offset_data.offset_type, 0, 0);
        return MMGSDI_INCORRECT_PARAMS;
    }/*End of switch(enhance_search_offset_data.offset_type ) */
  }/* End of  if (MMGSDI_UICC_ENHANCE_SEARCH == search_type ) */
  else
  {
    /* offset will be ignore for Simple Search and
       we are setting a default value */
    enhance_search_offset_data.offset_type = MMGSDI_SEARCH_OPTION_NONE;
    enhance_search_offset_data.offset_data = 0x00; /*Starting Offset */
  }

  /* option checking RFU*/
  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* client_ref checking Not required */

  /*------------------------------------------------------------------------
  - Allocating memory to Task Command Pointer
  -------------------------------------------------------------------------*/
  var_len[0] = search_pattern.data_len;
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len[0],&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len[0],temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr, task_cmd_len);
  /*------------------------------------------------------------------------
    - Align pointers of the various structures members
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.search_req, MMGSDI_SEARCH_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SEARCH_REQ;
  msg_ptr = &task_cmd_ptr->cmd.cmd.search_req;

  /*------------------------------------------------------------------------
    Populate Search Request::
    Updating request_header:
      client_id:
      orig_request_type:
      request_type:
      request_len:
      payload_len:
      *request_id_ptr:
      slot_id:
      client_data:
      response_cb:
    Updating access:
    Updating search_type:
    Updating rec_num:
    Updating seek_direction:
    Updating search_direction:
    Updating offset:
    Updating data:
  -------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.client_id            = client_id;
  msg_ptr->request_header.orig_request_type    = MMGSDI_SEARCH_REQ;
  msg_ptr->request_header.request_type         = MMGSDI_SEARCH_REQ;
  msg_ptr->request_header.request_len          = total_mem_len;
  msg_ptr->request_header.payload_len          = total_mem_len
                        - uint32toint32(sizeof(mmgsdi_request_header_type));
  msg_ptr->request_header.request_id_ptr       = rex_self();
  msg_ptr->request_header.slot_id              = card_slot;
  msg_ptr->request_header.client_data          = client_ref;
  msg_ptr->request_header.response_cb          = response_cb_ptr;

  /* Updating access  */
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Copying of Access Type is Fail in MMGSDI_SEARCH",0,0,0);
    mmgsdi_util_print_file_information(file_access,TRUE);
    return mmgsdi_status;
  }

  /* Updating search_type */
  msg_ptr->search_type                         = search_type;
  /* Updating rec_num */
  msg_ptr->rec_num                             = search_record_num;
  /* Updating seek_direction : Not used for mmgsdi search */
  msg_ptr->seek_direction                      = MMGSDI_SEEK_DIRECTION_NONE;
  /* Updating search_direction */
  msg_ptr->search_direction                    = search_direction;
  /* Updating offset */
  msg_ptr->enhanced_search_offset.offset_type  =
                                      enhance_search_offset_data.offset_type;
  msg_ptr->enhanced_search_offset.offset_data  =
                                      enhance_search_offset_data.offset_data;
  /* Updating data */
  msg_ptr->data.data_len                       = search_pattern.data_len;
  memcpy(msg_ptr->data.data_ptr,
    search_pattern.data_ptr,
    int32touint32(msg_ptr->data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    MSG_HIGH("Successful queue of SEARCH RECORD command 0x%x",mmgsdi_status,0,0);
  }
  return mmgsdi_status;
}/* end of function mmgsdi_search*/


/*===========================================================================
FUNCTION:MMGSDI_CREATE_PKCS15_LOOKUP_TABLE

DESCRIPTION:
     Client invokes this API to create a PKCS15 lookup table which is further
     used to access PKCS15 EFs. The lookup table created by mmgsdi is shared
     with the client in the response call back pointer provided by the client.


  Description of the parameters:
     client_id:          Client ID of the caller
     card_slot:          Slot on which command has to be sent
     app_id_data:        application identifier,
     option:             Not used yet
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         request.
     client_ref:         User Data returned to the user upon completion of
                         the command.


DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg(). the client must have also
  opened a session with the desired PKCS15 application on the card.


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
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_create_pkcs15_lookup_table(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_option_type       option,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#else
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_PKCS15 */
}/*mmgsdi_create_pkcs15_lookup_table*/
