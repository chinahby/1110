/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   F U N C T I O N S


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

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_icc.c#4 $$ $DateTime: 2009/06/09 23:16:51 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/09   kk      Added service table check before sending down SELECT
03/12/09   mib     Fixed issue during onchip procedure
03/02/09   mib     activate_onchip_sim: pass original refresh mode
09/25/08   js      Fixed Klockwork Error
09/22/08   js      Fixed Lint Errors
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   jk      Support for Generate Key API
08/20/08   kk      Calls in create_pkcs15_table made to async
07/31/08   kk      Fixing issues related to path length and path in pkcs table
07/23/08   sp      Initialize the data_ptr
07/20/08   sp      Update the cache properly for read on record based files
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/20/08   sp      Add data offset messages for read
03/03/08   nk      Updated call to mmgsdi_uim_icc_select and
                   mmgsdi_uim_icc_rehabilitate
02/14/08   vs      Added PIN events in mmgsdi_icc_activate_onchip_sim()
01/25/07   vs      Fixed compile error
12/17/07   vs      Featurized function mmgsdi_icc_run_cave()
08/28/07   tml     Fixed onchip sim support.  Ensure events are properly built
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
07/27/07   sun     Fixed Srv Available calculation
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/19/07   sun     Fixed Lint Error
03/10/07   sun     Added support for Disable with Replacement
03/08/07   tml     Compilation fix
03/02/07   sun     Check for file Attributes before proceeding to read/write
02/27/07   sun     Removed Global PIN from GSDI
02/22/07   tml     Fixed on chip sim not able to allow other command to
                   proceed issue
01/17/07   sp      Removed pin_just_verified flag
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access
10/17/06   tml     Check for disable CHV1 allowed or not before Disable CHV1
                   action is sent to the card
10/08/06   tml     Fixed compilation error
10/02/06   sp      Changed me supports fdn from gsdi to mmgsdi global data
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     Lint
05/23/06   tml     BDN, rehabilitate and invalidate supports
04/15/06   tml     moved onchip member to global data structure and compilation
                   warning fix
04/16/06   jar     Added ONCHIP SIM Support
02/15/06   tml     Used read_cache_len to get the cached item len in case
                   user passed in len of 0
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
09/29/05   tml     Greater than 512 byte Cache Read
08/30/05   jar     Added Support for sending Status Commands to the Card
07/26/05   sst     Fixed lint errors
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gsdi.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "uim.h"
#include "mmgsdiutil.h"
#include "mmgsdi_icc_rsp.h"

#ifdef FEATURE_DOG
#include "dog.h"
#endif /* FEATURE_DOG */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_GET_FILE_ATTR

   DESCRIPTION:
     This function will provide get file attributes to files only in
     SIM/RUIM.

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
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only)


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
mmgsdi_return_enum_type mmgsdi_icc_get_file_attr(
  const mmgsdi_get_file_attr_req_type * req_ptr
)
{
  mmgsdi_return_enum_type          mmgsdi_status   = MMGSDI_SUCCESS;
  boolean                          file_ok_in_svr_table = TRUE;

  MSG_MED("MMGSDI ICC GET FILE ATTR",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  switch (req_ptr->access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
  case MMGSDI_BY_PATH_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Access Type 0x%x",
                            req_ptr->access.access_method, 0, 0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Check status of EF in cached SST or CST */
  file_ok_in_svr_table = mmgsdi_util_is_file_ok_in_svc_table(
    &req_ptr->request_header.slot_id,
    &req_ptr->access );
  if(file_ok_in_svr_table)
  {
    MSG_HIGH("Sending down SELECT command to MMGSDI UIM",0,0,0);
    mmgsdi_util_print_file_information(req_ptr->access,FALSE);

    /* Send select request to the UIM server */
    mmgsdi_status = mmgsdi_uim_icc_select(req_ptr, FALSE, NULL);

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
      MSG_HIGH("ERROR Select status %d",mmgsdi_status,0,0);
    }
  }
  else
  {
    MSG_ERROR("SERVICE not allowed ",0,0,0);
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  }/* if(file_ok_in_svr_table) */
  
  return mmgsdi_status;

} /* mmgsdi_icc_get_file_attr */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_WRITE

   DESCRIPTION:
     This function will provide write access to any file using the ICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

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
mmgsdi_return_enum_type  mmgsdi_icc_write (
  const mmgsdi_write_req_type      * req_ptr
)
{
  boolean                            file_ok_in_svr_table = TRUE;

  MSG_MED("MMGSDI_ICC_WRITE", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /* Check for access method only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_BY_PATH_ACCESS:
  case MMGSDI_EF_ENUM_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Access Type 0x%x",
                            req_ptr->access.access_method, 0, 0);
    return MMGSDI_ERROR;
  }

  /*Check to see if the security attributes allow this file to be updated*/
  if(mmgsdi_util_check_file_attr((void*)req_ptr,
                                  MMGSDI_WRITE_REQ) != MMGSDI_SUCCESS)
  {
    MSG_ERROR("File Read not allowed",0,0,0);
    return MMGSDI_ERROR;
  }

  /* Check status of EF in cached SST or CST */
  file_ok_in_svr_table = mmgsdi_util_is_file_ok_in_svc_table(
    &req_ptr->request_header.slot_id,
    &req_ptr->access );

  MSG_HIGH("Sending down ICC WRITE command to MMGSDI UIM %d",
           req_ptr->access.file.file_enum,0,0);

  if (file_ok_in_svr_table)
  {
    switch (req_ptr->file_type)
    {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      return mmgsdi_uim_icc_write_record(req_ptr);

    case MMGSDI_TRANSPARENT_FILE:
      return mmgsdi_uim_icc_write_transparent(req_ptr);

    default:
      return MMGSDI_INCORRECT_PARAMS;
    }
  }
  else
  {
    MSG_HIGH("MMGSDI COMMAND NOT SUPPORTED BY MMGSDI UIM %d",
             req_ptr->access.file.file_enum,0,0);
    return MMGSDI_NOT_SUPPORTED;
  }
} /* mmgsdi_icc_write */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_READ

   DESCRIPTION:
     This function will provide read access to any file using the ICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

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
mmgsdi_return_enum_type  mmgsdi_icc_read (
  const mmgsdi_read_req_type      * req_ptr
)
{
  boolean                             file_ok_in_svr_table = TRUE;
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_data_type                    temp_data_holder;
  int32                               index                = 0;
  uim_cmd_type                      * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr      = NULL;
  uim_rpt_type                        report_ptr;
  uint32                              temp_data_len        = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
                                              - MMGSDI_BY_PATH_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  MSG_HIGH("MMGSDI_ICC_READ read file with offset %d", req_ptr->offset, 0, 0);

  /* initialize temp data */
  temp_data_holder.data_len = 0;
  temp_data_holder.data_ptr = NULL;

  /* Check for if it is EF access only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
  case MMGSDI_BY_PATH_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Access Type 0x%x",
                            req_ptr->access.access_method, 0, 0);
    return MMGSDI_ERROR;
  }

  if (req_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    /*Check to see if the security attributes allow this file to be read*/
    mmgsdi_status = mmgsdi_util_check_file_attr((void*)req_ptr,
                                                MMGSDI_READ_REQ);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("File Read not allowed",0,0,0);
      return mmgsdi_status;
    }

    /*---------------------------------------------------------------------------
      Check status of EF in cached SST or CST
    ---------------------------------------------------------------------------*/
    file_ok_in_svr_table = mmgsdi_util_is_file_ok_in_svc_table(
      &req_ptr->request_header.slot_id,
      &req_ptr->access );

    if (file_ok_in_svr_table)
    {
      if(req_ptr->data_len == 0)
      {
        /* The entire file has been requested */
        mmgsdi_status = mmgsdi_util_read_cache_len(
          req_ptr->request_header.slot_id,
          &req_ptr->access,
          &temp_data_holder.data_len,
          req_ptr->offset);

      } /* entire file requested */
      else
      {
        temp_data_holder.data_len = req_ptr->data_len;
      }

      /*---------------------------------------------------------------------------
      - Allocate temporary buffer
      - Read from Cache
      ---------------------------------------------------------------------------*/

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        temp_data_len = int32touint32(temp_data_holder.data_len);

        /* 'temp_data_len' could possibly be zero here */
        if(temp_data_len == 0)
        {
          MSG_ERROR("Can not allocate zero length memory to temp_data_holder.data_ptr",
                     0, 0, 0);
          return MMGSDI_ERROR;
        }

        MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(temp_data_holder.data_ptr,
                                                  temp_data_len);
        /* temporarily set the status to not found */
        mmgsdi_status = MMGSDI_NOT_FOUND;

        if( (req_ptr->file_type == MMGSDI_TRANSPARENT_FILE) ||
            (((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE)  ||
             (req_ptr->file_type == MMGSDI_CYCLIC_FILE))
            && (req_ptr->rec_num == 1)))
        {

          mmgsdi_status = mmgsdi_util_read_cache(req_ptr->request_header.slot_id,
                                                &req_ptr->access,
                                                &temp_data_holder,
                                                req_ptr->offset);
        }
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          /*---------------------------------------------------------------------------
            Read from cache failed
            Free temporary buffer
          ---------------------------------------------------------------------------*/
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
        }
      }

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_HIGH("Sending down ICC READ command to MMGSDI UIM %d",
            req_ptr->access.file.file_enum,0,0);
        /*---------------------------------------------------------------------------
          Read from cache failed
          Send to the card
          ---------------------------------------------------------------------------*/
        switch (req_ptr->file_type)
        {
        case MMGSDI_LINEAR_FIXED_FILE:
        case MMGSDI_CYCLIC_FILE:
          return mmgsdi_uim_icc_read_record(req_ptr, FALSE, NULL);

        case MMGSDI_TRANSPARENT_FILE:
          return mmgsdi_uim_icc_read_transparent(req_ptr, FALSE, NULL);

        default:
          return MMGSDI_INCORRECT_PARAMS;
        }

      } /* end if - data did not come from cache */

      /*---------------------------------------------------------------------------
        Read from cache succeed
        1) get client request table index
        2) allocate extra parameter
        3) allocate uim cmd pointer
        4) queue report response
        ---------------------------------------------------------------------------*/

      MSG_HIGH("Getting data from cache ICC READ command %d",
            req_ptr->access.file.file_enum,0,0);

      /* Data came from cache */
      mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Unable to get free client index", 0, 0, 0);
        MSG_ERROR("Unsuccessful ICC READ command from cache %d",
            req_ptr->access.file.file_enum,0,0);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
        return mmgsdi_status;
      } /* not successful */

      /*---------------------------------------------------------------------------
        Allocate and Populate Extra Parameter
        - Free Request Table if error in Extra Parameter
        - Free temp data holder if error
      ---------------------------------------------------------------------------*/

      mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
        MMGSDI_READ_REQ, (void*)req_ptr, &extra_param_ptr);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        /*---------------------------------------------------------------------------
        1) Release the client table
        2) Release temp data  (Since client table has not been populated yet)
        ---------------------------------------------------------------------------*/
        mmgsdi_util_free_client_request_table_index(index);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
        return mmgsdi_status;
      }

      /*---------------------------------------------------------------------------
        Populate Client request table
      ---------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                  index,
                                                  &req_ptr->request_header,
                                                  uim_cmd_ptr,
                                                  extra_param_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {

        MSG_ERROR("Populate to client req table failed 0x%x",
          mmgsdi_status, 0, 0);
        MSG_ERROR("Unsuccessful ICC READ command from cache %d",
          req_ptr->access.file.file_enum,0,0);

        /*---------------------------------------------------------------------------
        1) Release the client table
        2) Release extra param pointer (Since client table has not been
                                        populated yet)
        3) Release temp data  (Since client table has not been populated yet)
        4) Release uim cmd data  (Since client table has not been populated yet)
        ---------------------------------------------------------------------------*/

        mmgsdi_util_free_client_request_table_index(index);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
        return mmgsdi_status;
      }

      /* populate the necessary field in the uim_report_type */
      report_ptr.rpt.read.num_bytes_read =
        int32touint16(temp_data_holder.data_len);
      report_ptr.rpt.read.data           = temp_data_holder.data_ptr;
      report_ptr.rpt_status              = UIM_PASS;

      switch (req_ptr->file_type)
      {
      case MMGSDI_LINEAR_FIXED_FILE:
      case MMGSDI_CYCLIC_FILE:
        report_ptr.rpt_type            = UIM_READ_REC_CMD_R;
        break;

      case MMGSDI_TRANSPARENT_FILE:
        report_ptr.rpt_type            = UIM_READ_BIN_CMD_R;
        break;

      default:
        mmgsdi_util_free_client_request_table_index(index);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
        return MMGSDI_INCORRECT_PARAMS;
      }

      report_ptr.sw1                     = SW1_NORMAL_END; // TODOjk should this be 9000 if never queued on failure?
      report_ptr.sw2                     = SW2_NORMAL_END;
      report_ptr.cmd_transacted          = FALSE;
      report_ptr.user_data               = int32touint32(index);

      mmgsdi_uim_report(&report_ptr);

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);

      mmgsdi_status = MMGSDI_SUCCESS;
    } /* end file in svc table */
    else
    {
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    }
  }
  else if (req_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS)
  {
    MSG_HIGH("Sending down ICC READ command to MMGSDI UIM %d", 0,0,0);
    /*---------------------------------------------------------------------------
      EF accessed by PATH, do not go through cache and send to the card directly
    ---------------------------------------------------------------------------*/
    switch (req_ptr->file_type)
    {
      case MMGSDI_LINEAR_FIXED_FILE:
      case MMGSDI_CYCLIC_FILE:
        return mmgsdi_uim_icc_read_record(req_ptr, FALSE, NULL);

      case MMGSDI_TRANSPARENT_FILE:
        return mmgsdi_uim_icc_read_transparent(req_ptr, FALSE, NULL);

      default:
        return MMGSDI_INCORRECT_PARAMS;
    }
  }
  /* will not be anything other than EF_ENUM or BY_PATH since the beginning of
     the function checks for this criteria */

  return mmgsdi_status;
} /* mmgsdi_icc_read */

/* ============================================================================
  FUNCTION:      MMGSDI_ICC_STATUS

  DESCRIPTION:
    This function will send a status command to card.  It will stay compliant
    to GSM 11.11 or IS820C

    TThis function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used
    to give an opportunity for a pro active SIM to indicate that the
    SIM wants to issue a SIM Application Toolkit command to the ME.

    The information is provided asynchronously in the response cnf.
    Which will contain information containing:  file ID, total memory space
    available, CHV enabled/disabled indicator, CHV status and other
    GSM/CDMA specific data (identical to SELECT).

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_status should be used.

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

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_status (
  mmgsdi_status_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  MSG_MED("MMGSDI_ICC_STATUS", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /* --------------------------------------------------------------------------
     The me_app_status and ret_data_struct types are not applicable to a
     GSM SIM.  Therfore, ignore the inputs.
     ------------------------------------------------------------------------*/
  if ( ( req_ptr->me_app_status   != MMGSDI_STATUS_APP_NO_INDICATION ) &&
       ( req_ptr->ret_data_struct != MMGSDI_STATUS_NONE ) )
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Ignoring Inputs:  Invalid ME Status Indicator: 0x%x",
                             req_ptr->me_app_status,0,0);

    /* Reset the values */
    req_ptr->me_app_status   = MMGSDI_STATUS_APP_NO_INDICATION;
    req_ptr->ret_data_struct = MMGSDI_STATUS_NONE;
  }

  /* ---------------------------------------------------------------------------
     Move into the MMGSDI and UIM Interface to Queue the status command onto
     the UIM Task Command Queue
     -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_uim_icc_status(req_ptr);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("Failed UIM ICC STATUS: 0%x",mmgsdi_status,0,0);
  }

  return mmgsdi_status;
}

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */

/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PROCESS_BDN

  DESCRIPTION:
    This function will enable/disable BDN by doing a rehabiliate or
    invalidate to UIM.


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_process_bdn (
  const mmgsdi_bdn_req_type   *req_ptr
)
{
  mmgsdi_return_enum_type          mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_rehabilitate_req_type     rehab_req;
  mmgsdi_invalidate_req_type       invalidate_req;

  MSG_MED("MMGSDI ICC PROCESS BDN",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  if (req_ptr->request_header.request_type == MMGSDI_ENABLE_BDN_REQ)
  {
    memcpy(&rehab_req.request_header, &req_ptr->request_header,
      sizeof(mmgsdi_request_header_type));

    /* overwrite the request type to invalidate */
    rehab_req.request_header.request_type      = MMGSDI_REHABILITATE_REQ;
    rehab_req.access.access_method             = MMGSDI_EF_ENUM_ACCESS;
    rehab_req.access.file.file_enum            = MMGSDI_TELECOM_BDN;

    /* Send Rehabilitate request to the UIM server */
    mmgsdi_status = mmgsdi_uim_icc_rehabilitate(&rehab_req, FALSE, NULL);
  }
  else if (req_ptr->request_header.request_type == MMGSDI_DISABLE_BDN_REQ)
  {
    memcpy(&invalidate_req.request_header, &req_ptr->request_header,
      sizeof(mmgsdi_request_header_type));

    /* overwrite the request type to invalidate */
    invalidate_req.request_header.request_type      = MMGSDI_INVALIDATE_REQ;
    invalidate_req.access.access_method             = MMGSDI_EF_ENUM_ACCESS;
    invalidate_req.access.file.file_enum            = MMGSDI_TELECOM_BDN;

    /* Send Invalidate request to the UIM server */
    mmgsdi_status = mmgsdi_uim_icc_invalidate(&invalidate_req);
  }
  else
  {
    return MMGSDI_ERROR;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_HIGH("ERROR BDN process status %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;

} /* mmgsdi_icc_process_bdn */



/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_REHABILITATE

  DESCRIPTION:
    This function will rehabilitate the EF


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_rehabilitate (
  const mmgsdi_rehabilitate_req_type   *req_ptr
)
{
  mmgsdi_return_enum_type          mmgsdi_status   = MMGSDI_SUCCESS;

  MSG_MED("MMGSDI ICC REHABILITATE",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /* Check for if it is EF access only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
  case MMGSDI_BY_PATH_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Access Type 0x%x",
                            req_ptr->access.access_method, 0, 0);
    return MMGSDI_ERROR;
  }

  /* Send rehabilitate request to the UIM server */
  mmgsdi_status = mmgsdi_uim_icc_rehabilitate(req_ptr, FALSE, NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_HIGH("ERROR rehabilitate process status %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;

} /* mmgsdi_icc_rehabilitate */

/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_INVALIDATE

  DESCRIPTION:
    This function will invalidate the EF


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_invalidate (
  const mmgsdi_invalidate_req_type   *req_ptr
)
{
  mmgsdi_return_enum_type          mmgsdi_status   = MMGSDI_SUCCESS;

  MSG_MED("MMGSDI ICC INVALIDATE",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /* Check for if it is EF access only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
  case MMGSDI_BY_PATH_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Access Type 0x%x",
                            req_ptr->access.access_method, 0, 0);
    return MMGSDI_ERROR;
  }

  /* Send Invalidate request to the UIM server */
  mmgsdi_status = mmgsdi_uim_icc_invalidate(req_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_HIGH("ERROR Invalidate process status %d",mmgsdi_status,0,0);
  }
  return mmgsdi_status;

} /* mmgsdi_icc_invalidate */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_IS_DISABLE_CHV1_ALLOWED

  DESCRIPTION:
    This function will check if disable CHV1 is allowed or not in the Service
    Table

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC

  RETURN VALUE:
    TRUE/FALSE

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
static boolean  mmgsdi_icc_is_disable_chv1_allowed (
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_data_type                    sst_data;
  uint8                               sst_data_byte   = 0;
  mmgsdi_access_type                  file_access;
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_SUCCESS;

  sst_data.data_len = 1;
  sst_data.data_ptr = &sst_data_byte;

  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  if ((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK) ==
       GSDI_GSM_SIM_MASK)
  {
    file_access.file.file_enum = MMGSDI_GSM_SST;
  }
  else if ((gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK) ==
            GSDI_RUIM_APP_MASK)
  {
    file_access.file.file_enum = MMGSDI_CDMA_SVC_TABLE;
  }
  mmgsdi_status = mmgsdi_util_read_cache(slot_id,
                                         &file_access,
                                         &sst_data,
                                         SST_PIN_DISABLING_OFFSET);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
      Read from cache failed
    ---------------------------------------------------------------------------*/
    return TRUE;
  }

  if ( ( sst_data.data_ptr[0] & SST_PIN_DISABLING_MASK ) == SST_PIN_DISABLING_MASK )
  {
    MSG_HIGH("Disable CHV1 Allowed, byte value: 0x%x", sst_data.data_ptr[0], 0, 0);
    return TRUE;
  }
  MSG_HIGH("Disable CHV1 Not Allowed, byte value: 0x%x", sst_data.data_ptr[0], 0, 0);
  return FALSE;
}


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PIN_OPERATION

  DESCRIPTION:
    This function will perform pin operations on ICC card

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_pin_operation should be used

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
mmgsdi_return_enum_type  mmgsdi_icc_pin_operation (
  const mmgsdi_pin_operation_req_type     * req_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status   = MMGSDI_ERROR;
  boolean                           disable_allowed = TRUE;

  MSG_MED("MMGSDI_ICC_PIN_OPERATION", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /* Check for access method only */
  switch(req_ptr->pin_op)
  {
    case MMGSDI_PIN_OP_VERIFY:
      mmgsdi_status = mmgsdi_uim_icc_verify_pin(req_ptr,MMGSDI_RETRY_NONE);
      break;
    case MMGSDI_PIN_OP_ENABLE:
      mmgsdi_status = mmgsdi_uim_icc_enable_pin(req_ptr);
      break;
    case MMGSDI_PIN_OP_DISABLE:
      disable_allowed = mmgsdi_icc_is_disable_chv1_allowed(req_ptr->request_header.slot_id);
      if (disable_allowed)
      {
        mmgsdi_status = mmgsdi_uim_icc_disable_pin(req_ptr);
      }
      else
      {
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
      break;
    case MMGSDI_PIN_OP_CHANGE:
       mmgsdi_status = mmgsdi_uim_icc_change_pin(req_ptr);
      break;
    case MMGSDI_PIN_OP_UNBLOCK:
      mmgsdi_status = mmgsdi_uim_icc_unblock_pin(req_ptr,MMGSDI_RETRY_NONE);
      break;
    case MMGSDI_PIN_OP_NONE:
    case MMGSDI_PIN_OP_DISABLE_AND_REPLACE:
      return MMGSDI_INCORRECT_PARAMS;
  }
  return mmgsdi_status;
} /* mmgsdi_icc_pin_operation */


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_PIN_STATUS

  DESCRIPTION:
    This function will get the Pin Status from the cache or send the command to
    the card if the PIN Status in not available
  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_pin_status should be used.

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

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_pin_status (
  const mmgsdi_get_all_pin_status_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_ERROR;
  int32                               index                = 0;
  mmgsdi_client_req_extra_info_type * extra_param_ptr      = NULL;

  MSG_MED("MMGSDI_ICC_PIN_STATUS", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_GET_ALL_PIN_STATUS_REQ,
                                                            (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index,
                                                                  &req_ptr->request_header,
                                                                  NULL,
                                                                  extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, MMGSDI_GET_ALL_PIN_STATUS_REQ,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_ICC_BUILD_GET_PIN_STATUS

DESCRIPTION
  This function will buid a request to the MMGSDI Task to request the PIN
  status for all relevant pins applicable to the client ID provided.
  If the client ID provided is in a session, then the PIN IDs applicable
  to the application in the session will be returned.

  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_build_get_pin_status should be used.

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
mmgsdi_return_enum_type mmgsdi_icc_build_get_pin_status(void)
{
  mmgsdi_get_all_pin_status_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;

  /* --------------------------------------------------------------------------
     Allocate 1 large block of memory for the task_cmd_ptr and for the
     onchip config data.
     NOTE:  Side effect of the following total len is that it may not
            match exactly as a result of mmgsdi_task_cmd_data_type being
            a union. Will be addressed by using the correct type size
            rather than sizeof mmgsdi_task_cmd_data_type
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(msg_ptr,
                                            sizeof(mmgsdi_get_all_pin_status_req_type));
  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_type      = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data.client_id;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_1;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = mmgsdi_util_internal_cb;

  mmgsdi_status = mmgsdi_icc_pin_status(msg_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(msg_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr
    ** has not been put onto the command queue
    */
    MSG_ERROR("CACHE PIN STATUS Failed 0x%x", mmgsdi_status, 0, 0);
  }

  MSG_HIGH("GET ALL PIN STATUS REQUEST 0x%x ",mmgsdi_status,0,0);

  return mmgsdi_status;
}/* mmgsdi_icc_build_get_pin_status */


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


#ifdef FEATURE_MMGSDI_CDMA
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/*===========================================================================
  FUNCTION:      MMGSDI_ICC_COMPUTE_IP_AUTH

  DESCRIPTION:
    This function will allow COMPUTE IP AUTH command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_compute_ip_auth (
  const mmgsdi_compute_ip_req_type   *req_ptr
)
{
   MSG_MED("MMGSDI ICC PROCESS COMPUTE IP AUTH",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /* Check for access method only */
  return mmgsdi_uim_icc_compute_ip_auth(req_ptr);

} /* mmgsdi_icc_compute_ip_auth */
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */


#ifdef FEATURE_AUTH
/*===========================================================================
  FUNCTION:      MMGSDI_ICC_RUN_CAVE

  DESCRIPTION:
    This function will allow RUN CAVE command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_run_cave (
  const mmgsdi_run_cave_req_type   *req_ptr
)
{
   MSG_MED("MMGSDI ICC PROCESS RUN CAVE",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  /* Check for access method only */
  return mmgsdi_uim_icc_run_cave(req_ptr);

} /* mmgsdi_icc_run_cave */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GENERATE_KEY_VPM

  DESCRIPTION:
    This function will allow GENERATE KEY / VPM command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_generate_key_vpm (
  const mmgsdi_generate_key_req_type   *req_ptr
)
{
   MSG_HIGH("MMGSDI ICC PROCESS GENERATE KEY VPM",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  return mmgsdi_uim_icc_generate_key_vpm(req_ptr);

} /* mmgsdi_icc_generate_key_vpm */
#endif /* FEATURE_AUTH */

#endif /* FEATURE_MMGSDI_CDMA */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_SEARCH

  DESCRIPTION:
    This function will provide search access to record file using the ICC
    protocol

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support.
                              When the Search Type is not MMGSDI_ICC_SEARCH.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_search (
  mmgsdi_search_req_type      * req_ptr
)
{
  boolean file_ok_in_svr_table = TRUE;
  mmgsdi_return_enum_type mmgsdi_status;
  MSG_HIGH("MMGSDI_ICC_SEARCH", 0, 0, 0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->search_type                     - should be MMGSDI_ICC_SEARCH
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if(MMGSDI_ICC_SEARCH != req_ptr->search_type)
  {
    /* This condition will be true when search type contains
    MMGSDI_UICC_SIMPLE_SEARCH MMGSDI_UICC_ENHANCED_SEARCH value */
    MSG_HIGH("MMGSDI SEARCH TYPE IS INVALID IN MMGSDI_ICC_SEARCH  0x%x",
              req_ptr->search_type,0,0);
    return MMGSDI_NOT_SUPPORTED;
  }/* end of if(MMGSDI_ICC_SEARCH != req_ptr->search_type)*/

  mmgsdi_status = mmgsdi_util_check_file_attr((void*)req_ptr,
                                              MMGSDI_SEARCH_REQ);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("File Searching is not allowed",0,0,0);
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /* Check status of EF in cached SST */
  file_ok_in_svr_table = mmgsdi_util_is_file_ok_in_svc_table(
                           &req_ptr->request_header.slot_id,
                           &req_ptr->access );
  if (file_ok_in_svr_table)
  {
    MSG_HIGH("Sending down ICC SEARCH command to MMGSDI UIM ",0,0,0);
    mmgsdi_util_print_file_information(req_ptr->access,FALSE);
    return mmgsdi_uim_icc_search_record(req_ptr);
  }
  else
  {
    MSG_ERROR("MMGSDI ICC SEARCH COMMAND IS NOT SUPPORTED BY MMGSDI UIM ",0,0,0);
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return MMGSDI_NOT_SUPPORTED;
  }
}/*end of mmgsdi_icc_search*/
