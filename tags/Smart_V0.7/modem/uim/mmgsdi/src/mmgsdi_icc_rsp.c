/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC response processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors. All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_icc_rsp.c#5 $$ $DateTime: 2009/06/10 06:04:29 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/09   ssr     Fixed unblock retry counter for incorrect pin
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/29/09   js      Included header mmgsdi_icc_rsp.h
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/19/08   nb      Klockwork Fixes
11/05/08   ssr     Resolved compiler error for RUIM enablement
11/03/08   tml     Returned error if the get all pin status command processing
                   was erroneous and lint fixes
10/20/08   sun     Allow protocol to be sent as part of card reset
09/22/08   js      Fixed Lint Errors
09/08/08   sun     Fixed Compilation
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/26/08   jk      Fixed 1X Auth featurization for LTK Compilation
08/21/08   sun     Select should happen on RUIM DIR rather than GSM DIR for CDMA
                   protocol
08/21/08   sun     Featurized code, code review comments
08/20/08   jk      Support for Generate Key API & Compile Fixes related to PKCS15
08/20/08   kk      Calls in create_pkcs15_table made to async
07/30/08   SSR     Fixed MMGSDI status for ICC Search, for SW1 WRONG Length
07/20/08   sp      Update cache properly for read on record based files
07/15/08   jk      Fixed Compile Errors / Warnings
06/12/08   ssr     Support of MMGSDI Search functionality
06/05/08   tml     Added sw in confirmation and messages cleanup
05/23/08   vs      Fixed file status info bit decoding
03/17/08   nk      Fixed Klocwork Issues.
03/03/08   nk      Added synchronous support in mmgsdi_icc_rsp_get_file_attr
                   and mmgsdi_icc_rsp_rehabitate
02/29/08   vs      Changed pin_status to not initialized if bit is set to zero
12/11/07   sun     Build Card Reset if the original request is for CARD_RESET
11/15/07   sun     Added support for NAA refresh
11/05/07   tml     Fixed null pointer access when the file read was from
                   gsdi cache
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/04/07   sun     Fixed Double Freeing of pin_info_ptr
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/20/07   sun     Select the first index of app_pin1 by default
03/19/07   sun     Fixed Lint Error
02/21/07   sun     Removed Global PIN from GSDI
01/23/07   sp      Set the cnf_status with uim report status.
01/18/07   sun     First Handle the Select Response and then check the
                   retries
08/07/06   sun     Added PIN and Valid PIN to mmgsdi_app_pin_info_type
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     lint
06/08/06   tml     fixed featurized
05/23/06   tml     BDN, Rehabilitate and invalidate supports and lint fixes
04/18/06   nk      Featurized code
04/13/06   jar     Added ONCHIP SIM Support
03/08/06   sun     Do not parse the UIM Response data for Streaming APDU
02/15/06   tml     Added data fr card info to write cache item
02/14/06   nk      Merge for: ARR caching and by path changes
12/21/05   sun     Return Proper error ISIM Authentication failure
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
11/03/05   sun     For SAP APDU, return success even file is not found
08/30/05   jar     Added support to handle a ICC Status Response
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   sk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "intconv.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "uim.h"
#include "gsdi_exp.h"
#include "gsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_refresh.h"
#include "gsdi_convert.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_icc_rsp.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_PARSE_UIM_RESPONSE

   DESCRIPTION:
     This function will parse write access to any file using the ICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_icc_rsp_parse_uim_response(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uim_report_code_type    rpt_code      = UIM_NO_SUCH_REPORT_R;
  boolean                 is_success    = FALSE;
  int32                   sw1           = 0;
  int32                   sw2           = 0;
  mmgsdi_return_enum_type error_status  = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  rpt_code     = (uim_report_code_type)uim_rsp_ptr->uim_rpt_code;
  is_success   = uim_rsp_ptr->is_uim_success;
  sw1          = uim_rsp_ptr->status_word.sw1;
  sw2          = uim_rsp_ptr->status_word.sw2;
  error_status = uim_rsp_ptr->mmgsdi_error_status;

  /* Was the request successful */
  if (is_success == TRUE)
  {
    switch ( sw1 )
    {
    case GSDI_SIM_SW1_NORM_END: /* 0x90 */
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    default: /* other sw1 when rpt_status == PASS */
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
  }
  else
  {
    /* UIM_FAIL */
    switch ( sw1 )
    {
    case GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO: /* 0x91 XX*/
      MSG_HIGH("GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO", 0, 0, 0);

      switch (rpt_code)
      {
      case UIM_CACHED_SEL_R:
      case UIM_SELECT_R:
      case UIM_SEEK_R:
      case UIM_RUN_GSM_ALGO_R:
        /* also Increase.... */
        MSG_ERROR("Invalid report code %x", rpt_code, 0, 0);
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break;

      default:
        /* Unhandled Error Condition */
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break;
      }
      break; /* GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO  0x91 XX*/

    case GSDI_SW1_MEMORY_MANAGEMENT: /* 0x92 */
      switch ( sw2 )
      {
      case GSDI_SW2_MEMORY_PROBLEM: /* 0x40 */
        MSG_HIGH("GSDI_SW2_MEMORY_PROBLEM", 0, 0, 0);
        /* Memory Management Error          */
        /* SIM Memory Problem           */
        mmgsdi_status = MMGSDI_SIM_TECHNICAL_PROBLEMS;
        break;

      default:
        MSG_HIGH("Success After Card Internal Update", 0, 0, 0);
        if ((sw2 & 0xF0) == 0)  /* 0x0X */
        {
          switch (rpt_code)
          {
          case UIM_CACHED_SEL_R:
          case UIM_SELECT_R:
          case UIM_STATUS_R:
          case UIM_SEEK_R:
          case UIM_RUN_GSM_ALGO_R:
            /* read binary, read record is not valid */
            MSG_ERROR("Invalid Report Code %x", rpt_code, 0, 0);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          default:
            /* command success only after X times internal retries */
            /* May need to set to GSDI SUCCESS because action did  */
            /* eventually succeed                                  */
            mmgsdi_status = MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE;
            break;
          }
        }
        else
        {
          /* Can't determine error but    */
          /* Error doesn't need to create */
          /* an event to the UI           */
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        }
        break;
      }
      break; /* GSDI_SW1_MEMORY_MANAAGEMENT 0x92 */

    case GSDI_SW1_SIM_APP_TOOLKIT_BUSY: /* 0x93 */
      MSG_HIGH("GSDI_SW1_SIM_APP_TOOLKIT_BUSY", 0, 0, 0);
      switch (rpt_code)
      {
      case UIM_ENVELOPE_R:
        /* Sim is busy */
        mmgsdi_status = MMGSDI_SIM_BUSY;
        break;
      default:
        MSG_ERROR("Invalid report code %x", rpt_code, 0, 0);
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break;
      }
      break; /* GSDI_SW1_SIM_APP_TOOLKIT_BUSY 0x93 */

    case GSDI_SW1_REFERENCE_MANAGEMENT:  /* 0x94 */
      /* Reference Management Error, Now  */
      /* Determine the specific Error     */
      switch ( sw2 )
      {
      case GSDI_SW2_NO_EF_SELECTED: /* 0x00 */
        MSG_HIGH("GSDI_SW2_NO_EF_SELECTED", 0, 0, 0);
        switch (rpt_code)
        {
        case UIM_ACCESS_R:
        case UIM_READ_BIN_CMD_R:
        case UIM_READ_REC_CMD_R:
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
        case UIM_SEEK_R:
        case UIM_INVALIDATE_R:
        case UIM_REHABILITATE_R:
          /* No EF Selected               */
          mmgsdi_status = MMGSDI_NO_EF_SELECTED;
          break;

        default:
          MSG_ERROR("Invalid report code %x", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_OUT_OF_RANGE: /* 0x02 */
        MSG_HIGH("GSDI_SW2_OUT_OF_RANGE", 0, 0, 0);
        switch (rpt_code)
        {
        case UIM_ACCESS_R: /* only update record and read record */
        case UIM_READ_BIN_CMD_R:
        case UIM_READ_REC_CMD_R:
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
          /* Out of Range or invalid      */
          /* address                      */
          mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
          break;
        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND: /* 0x04 */
        MSG_HIGH("GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND",0,0,0);
        switch (rpt_code)
        {
        case UIM_ACCESS_R:
        case UIM_READ_BIN_CMD_R:
        case UIM_READ_REC_CMD_R:
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
        case UIM_CACHED_SEL_R:
        case UIM_SELECT_R:
        case UIM_SEEK_R:
          /* File ID not found or pattern */
          /* not found                    */
          mmgsdi_status = MMGSDI_NOT_FOUND;
          break;
        default:
          MSG_ERROR("Invalid report code %x", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_FILE_INCONSISTENT_WITH_CMD: /* 0x08 */
        MSG_HIGH("GSDI_SW2_FILE_INCONSISTENT_WITH_CMD", 0, 0, 0);
        switch (rpt_code)
        {
        case UIM_ACCESS_R:
        case UIM_READ_BIN_CMD_R:
        case UIM_READ_REC_CMD_R:
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
        case UIM_SEEK_R:
        case UIM_RUN_GSM_ALGO_R:
          /* File is inconsistend with    */
          /* command                      */
          mmgsdi_status = MMGSDI_EF_INCONSISTENT;
          break;
        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      default:
        /* Can't determine error but    */
        /* Error doesn't need to create */
        /* an event to the UI           */
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break;
      }
      break; /* GSDI_SW1_REFERENCE_MANAGEMENT 0x94 */

    case GSDI_SW1_SECURITY_MANAGEMENT: /* 0x98 */
      /* Security Management Errors       */
      switch ( sw2 )
      {
      case GSDI_SW2_NO_CHV_INITIALISED: /* 0x02 */
        MSG_HIGH("GSDI_SW2_NO_CHV_INITIALISED", 0, 0, 0);
        switch (rpt_code)
        {
        case UIM_VERIFY_CHV_R:
        case UIM_CHANGE_CHV_R:
        case UIM_DISABLE_CHV_R:
        case UIM_ENABLE_CHV_R:
        case UIM_UNBLOCK_CHV_R:
          /* No CHV Initialized           */
          mmgsdi_status = MMGSDI_PIN_NOT_INITIALIZED;
          break;
        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1: /* 0x04 */
        MSG_HIGH("GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1",0,0,0);
        switch (rpt_code)
        {
        case UIM_VERIFY_CHV_R:
        case UIM_CHANGE_CHV_R:
        case UIM_DISABLE_CHV_R:
        case UIM_ENABLE_CHV_R:
        case UIM_UNBLOCK_CHV_R:
          MSG_HIGH("GSDI_INCORRECT_CODE", 0, 0, 0);
          mmgsdi_status = MMGSDI_INCORRECT_CODE;
          break;

        case UIM_ACCESS_R:
        case UIM_READ_BIN_CMD_R:
        case UIM_READ_REC_CMD_R:
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
        case UIM_SEEK_R:
        case UIM_INVALIDATE_R:
        case UIM_REHABILITATE_R:
          MSG_HIGH("GSDI_ACCESS_DENIED", 0, 0, 0);
          /* Access conditon not fullfilled  */
          /* or CHV Verification not success */
          /* or CHV Unblock not success      */
          /* or authentication failed        */
          mmgsdi_status = MMGSDI_ACCESS_DENIED;
          break;

        case UIM_RUN_GSM_ALGO_R:
          MSG_HIGH("GSDI_AUTH_FAIL", 0, 0, 0);
          mmgsdi_status = MMGSDI_AUTH_FAIL;
          break;

        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_CNTRCTN_W_CHV_STATUS: /* 0x08 */
        MSG_HIGH("GSDI_SW2_CNTRCTN_W_CHV_STATUS", 0, 0, 0);
        switch (rpt_code)
        {
        case UIM_VERIFY_CHV_R:
        case UIM_CHANGE_CHV_R:
        case UIM_DISABLE_CHV_R:
        case UIM_ENABLE_CHV_R:
        case UIM_UNBLOCK_CHV_R:
          /* Access conditon not fullfilled  */
          /* or CHV Verification not success */
          /* or CHV Unblock not success      */
          /* or authentication failed        */
          mmgsdi_status = MMGSDI_INCOMPAT_PIN_STATUS;
          break;
        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS: /* 0x10 */
        MSG_HIGH("GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS", 0, 0, 0);
        switch (rpt_code)
        {
        case UIM_ACCESS_R:
        case UIM_READ_BIN_CMD_R:
        case UIM_READ_REC_CMD_R:
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
        case UIM_SEEK_R:
        case UIM_INVALIDATE_R:
        case UIM_REHABILITATE_R:
          /* Access conditon not fullfilled  */
          /* or CHV Verification not success */
          /* or CHV Unblock not success      */
          /* or authentication failed        */
          mmgsdi_status = MMGSDI_INVALIDATION_CONTRADICTION_STATUS;
          break;
        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_CHV_BLOCK_UNSUCCESSFUL_UNBLOCK: /* or GSDI_SW2_CHV_UNBLOCKED 0x40 */
        switch (rpt_code)
        {
        case UIM_VERIFY_CHV_R:
        case UIM_CHANGE_CHV_R:
        case UIM_DISABLE_CHV_R:
        case UIM_ENABLE_CHV_R:
          MSG_HIGH("GSDI_CODE_BLOCKED", 0, 0, 0);
          mmgsdi_status = MMGSDI_CODE_BLOCKED;
          break;

        case UIM_UNBLOCK_CHV_R:
          /* Access conditon not fullfilled  */
          /* or CHV Verification not success */
          /* or CHV Unblock not success      */
          /* or authentication failed        */
          MSG_HIGH("GSDI_CODE_PERM_BLOCKED", 0, 0, 0);
          mmgsdi_status = MMGSDI_CODE_PERM_BLOCKED;
          break;

        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_INCREASED_FAIL_MAX_REACHED: /* 0x50 */
        MSG_HIGH("GSDI_SW2_INCREASED_FAIL_MAX_REACHED", 0, 0, 0);
        switch (rpt_code)
        {
        case UIM_ACCESS_R: /* Increase only */
        case UIM_READ_BIN_CMD_R:
        case UIM_READ_REC_CMD_R:
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
          /* Access conditon not fullfilled  */
          /* or CHV Verification not success */
          /* or CHV Unblock not success      */
          /* or authentication failed        */
          mmgsdi_status = MMGSDI_INCREASE_MAX_REACHED;
          break;
        default:
          MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_AUTH_ERROR_APP_SPECIFIC: /* 0x62 */
        MSG_HIGH("GSDI_SW2_AUTH_ERROR_APP_SPECIFIC", 0, 0, 0);
        switch (rpt_code)
        {
          case UIM_ISIM_AUTHENTICATE_R:
            /* Authentication failed        */
            mmgsdi_status = MMGSDI_AUTH_ERROR_INCORRECT_MAC;
            break;
          default:
            MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
        }
        break;

      default:
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break;
      }
      break; /* GSDI_SW1_SECURITY_MANAGEMENT 0x98 */

    case GSDI_SW1_WRONG_LENGTH: /* 0x67 */
      MSG_HIGH("GSDI_SW1_WRONG_LENGTH", 0, 0, 0);
      /* or GSDI_SW1_INCORRECT_P3 - doesn't have specific sw2 */
      mmgsdi_status = MMGSDI_INCORRECT_LENGTH;
      break; /* GSDI_SW1_WRONG_LENGTH 0x67 */

    case GSDI_SW1_INCORRECT_P1_OR_P2: /* 0x6B */
      MSG_HIGH("GSDI_SW1_INCORRECT_P1_OR_P2", 0, 0, 0);
      /* Application Independent Errors   */
      /* Incorrect Parameters P3          */
      /* or Incorrect Parameters P1 & P2  */
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break; /* GSDI_SW1_INCORRECT_P1_OR_P2 0x6B */

    case GSDI_SW1_UNKNOWN_INSTRUCTION_CODE: /* 0x6D */
    case GSDI_SW1_WRONG_INSTRUCTION_CLASS:  /* 0x6E */
      MSG_HIGH("GSDI_UNKNOWN_INST_CLASS", 0, 0, 0);
      /* Unknown or Wrong Instruction class  */
      mmgsdi_status = MMGSDI_UNKNOWN_INST_CLASS;
      break; /* GSDI_SW1_WRONG_INSTRUCTION_CLASS 0x6E */

    case GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN: /* 0x6F */
      MSG_HIGH("GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN", 0, 0, 0);
      /* Application Independent Erros    */
      mmgsdi_status = MMGSDI_SIM_TECHNICAL_PROBLEMS;
      break; /* GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN 0x6F */

    case GSDI_SIM_SW1_NORM_END: /* 0x90 */
      MSG_HIGH("GSDI_SIM_SW1_NORM_END : UIM CMD TIME OUT", 0, 0, 0);
      mmgsdi_status = MMGSDI_UIM_CMD_TIMEOUT;
      break; /* GSDI_SIM_SW1_NORM_END 0x90 */

    case GSDI_SW1_UIM_TIMEOUT: /* 0x00 */
      MSG_HIGH("GSDI_SW1_UIM_TIMEOUT", 0, 0, 0);
      switch ( sw2 ) {
      case GSDI_SW2_UIM_TIMEOUT: /* 0x00 */
        mmgsdi_status = MMGSDI_UIM_CMD_TIMEOUT;
        break;
      default:
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break;
      }
      break; /* GSDI_SW1_UIM_TIMEOUT 0x00 */

    case GSDI_SW1_INT_ERROR:
      MSG_HIGH("MMGSDI INT ERROR 0x%x", error_status,0,0);
      mmgsdi_status = error_status;
      break;

    default:
      /* Unhandled Error Condition */
      mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
      break;
    }
  } /* UIM Fail */
  return mmgsdi_status;
} /* mmgsdi_icc_rsp_parse_uim_response */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_POPULATE_ICC_FILE_STATUS_INFO

  DESCRIPTION:
    This function populates the File Status into mmgsdi file status
    type according to GSM spec 11.11

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static void mmgsdi_populate_icc_file_status_info(
  mmgsdi_slot_id_enum_type     slot,
  const uint8                * header_status_attr,
  mmgsdi_file_status_type    * file_status_ptr
)
{
  uint8 *card_apps_ptr = NULL;
  uint8  phase = 0;

  if((header_status_attr == NULL) || (file_status_ptr == NULL))
  {
    MSG_ERROR("Null input params: header_status_attr 0x%x or file_status_ptr 0x%x",
               header_status_attr, file_status_ptr, 0);
    return;
  }

  switch (slot)
  {
  case MMGSDI_SLOT_1:
    phase         = gsdi_data.sim_phase;
    card_apps_ptr = &gsdi_data.card_applications.apps_enabled;
    break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case MMGSDI_SLOT_2:
    phase         = gsdi_data.sim_phase_2;
    card_apps_ptr = &gsdi_data.card_applications.apps2_enabled;
    break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
  default:
    MSG_ERROR("Unknown slot %d ", slot, 0, 0);
    return;
  }
  if((!(*card_apps_ptr & GSDI_GSM_SIM_MASK )) &&
        (*card_apps_ptr & GSDI_RUIM_APP_MASK))
  {
    phase = 2;
  }

  file_status_ptr->file_invalidated = header_status_attr[11] & 0x01;
  if(phase >= 2)
  {
    /* Per 3GPP TS 11.11, section 9.3 if bit b3 is set to 1 (bit mask 0x04)
       the file is readable and updateable when invalidated else not */
    if ((header_status_attr[11] & 0x04) == 0x04)
    {
      file_status_ptr->read_write_when_invalidated = TRUE;
    }
    else
    {
      file_status_ptr->read_write_when_invalidated = FALSE;
    }
  }
} /* mmgsdi_populate_icc_file_status_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_POPULATE_ICC_SECURITY_INFO

  DESCRIPTION:
    This function populates security information according to GSM spec 11.11

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_populate_icc_security_info(
  uint8                      card_access_cond,
  mmgsdi_file_security_type *protection_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_pin_enum_type    pin_id        = MMGSDI_MAX_PIN_ENUM;
  uint32                  pin_len       = 0;

  MMGSDIUTIL_RETURN_IF_NULL(protection_ptr);
  protection_ptr->num_protection_pin = 0;
  protection_ptr->protection_pin_ptr = NULL;
  protection_ptr->protection_method = MMGSDI_NEVER_ALLOWED;
  switch (card_access_cond)
  {
  case 0x00:
    protection_ptr->protection_method = MMGSDI_ALWAYS_ALLOWED;
    return mmgsdi_status;
  case 0x01:
    pin_id = MMGSDI_PIN1;
    break;
  case 0x02:
    pin_id = MMGSDI_PIN2;
    break;
  case 0x03:
    protection_ptr->protection_method = MMGSDI_NEVER_ALLOWED;
    return mmgsdi_status;
  case 0x04:
    pin_id = MMGSDI_ADM1;
    break;
  case 0x05:
    pin_id = MMGSDI_ADM2;
    break;
  case 0x06:
    pin_id = MMGSDI_ADM3;
    break;
  case 0x07:
    pin_id = MMGSDI_ADM4;
    break;
  case 0x08:
    pin_id = MMGSDI_ADM5;
    break;
  case 0x09:
    pin_id = MMGSDI_ADM6;
    break;
  case 0x0A:
    pin_id = MMGSDI_ADM7;
    break;
  case 0x0B:
    pin_id = MMGSDI_ADM8;
    break;
  case 0x0C:
    pin_id = MMGSDI_ADM9;
    break;
  case 0x0D:
    pin_id = MMGSDI_ADM10;
    break;
  case 0x0E:
    pin_id = MMGSDI_ADM11;
    break;
  case 0x0F:
  default:
    protection_ptr->protection_method = MMGSDI_NEVER_ALLOWED;
    return mmgsdi_status;
  }

  /* Single PIN is required conditions */
  protection_ptr->num_protection_pin = 1;

  pin_len = (int32touint32(protection_ptr->num_protection_pin)
             * sizeof(mmgsdi_pin_enum_type));

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      (protection_ptr->protection_pin_ptr),
      pin_len,
      mmgsdi_status);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    protection_ptr->num_protection_pin = 0;
  }
  else
  {
    protection_ptr->protection_method = MMGSDI_SINGLE_ALLOWED;
    protection_ptr->protection_pin_ptr[0] = pin_id;
  }
  return mmgsdi_status;
} /* mmgsdi_populate_icc_security_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_POPULATE_ICC_SEC_ATTR_FROM_UIM_TO_MMGSDI

  DESCRIPTION:
    This function populate the Security Attribute into mmgsdi file protection
    type according to GSM spec 11.11

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static void mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(
  uint8                            * sim_header_attr,
  mmgsdi_file_security_access_type * file_security_access_ptr
)
{
  if ((file_security_access_ptr == NULL) || (sim_header_attr == NULL))
  {
    MSG_ERROR("Sim header attr 0x%x of File Sec Acc Pt 0x%x is null",
               sim_header_attr, file_security_access_ptr, 0);
    return;
  }

  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[8] / 0x10),
    &file_security_access_ptr->read);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[8]& 0x0F),
    &file_security_access_ptr->write);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[9] / 0x10),
    &file_security_access_ptr->increase);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[10] / 0x10),
    &file_security_access_ptr->rehabilitate_activate);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[10]& 0x0F),
    &file_security_access_ptr->invalidate_deactivate);
} /* mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_RSP_EXTRACT_FILE_ATTR

  DESCRIPTION:
    This function is used to decode and map the Header information from the SIM
    format as define in GSM 11.11 as follows:

    byte 01:    rfu
    byte 02:    rfu
    byte 03:    file size MSB
    byte 04:    file size LSB
    byte 05:    file id MSB
    byte 06:    file id LSB
    byte 07:    type of file
              00 --> RFU
              01 --> MF
              02 --> DF
              04 --> EF
    byte 08:    RFU for Linear and Transparent Files
              bit7 = 1 Indicates Cyclic Increase allowed
    byte 09:    B8-B5 Read/Seek Permissions
              B4-B1 Update Permissions
    byte 10:    B8-B5 Increase Permissions
              B4-B1 RFU
    byte 11:    B8-B5 RehabilitatePermissions
              B4-B1 Invalidate Permissions
    byte 12:    File Status
              B8-B4 RFU
              B3 = 0 Not Readable/Writeable when invalidate
              B3 = 1 Readable/Writeable when invalidated
              B2 RFU
              B1 = 0 (Invalidated)
              B1 = 1 (Not Invalideated - Valid)
    byte 13:    Length of the following Information (Byte 14 to end]
    byte 14:    Structure of EF
              00 --> TRANSPARENT
              01 --> LINEAR FIXED
              03 --> CYCLIC
    byte 15:    Length of Record.  For cyclic and linear fixed, this denotes
              the length of the record.  For a transparent file it is 0x00.
    byte 16+:   (optional when defined)

    and it is populated into mmgsdi_file_attributes_type

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_icc_rsp_extract_file_attr (
  mmgsdi_slot_id_enum_type       slot,
  uint8 *                        sim_header_attr,
  mmgsdi_len_type                sim_header_data_len,
  mmgsdi_file_attributes_type  * attr_ptr
)
{
  mmgsdi_file_status_type          *file_status_ptr = NULL;
  mmgsdi_file_security_access_type *file_security_access_ptr = NULL;

  if (attr_ptr == NULL)
  {
    MSG_ERROR("Null attr_ptr ", 0, 0, 0);
    return;
  }

  if (sim_header_attr == NULL)
  {
    MSG_ERROR("Null sim_header_attr ", 0, 0, 0);
    return;
  }

  memset(attr_ptr, 0x00, int32touint32(sizeof(mmgsdi_file_attributes_type)) );

  attr_ptr->file_size = ((int32)sim_header_attr[2] << 8) | (int32)sim_header_attr[3];
  attr_ptr->file_id[0]= sim_header_attr[4];
  attr_ptr->file_id[1]= sim_header_attr[5];

  // TODO, check for sim_header_data_len for appropriate len info

  switch(sim_header_attr[6])
  {
  case 1:
    attr_ptr->file_type = MMGSDI_MASTER_FILE;
    break;
  case 2:
    attr_ptr->file_type = MMGSDI_DEDICATED_FILE;
    break;
  case 4:
    switch(sim_header_attr[13])
    {
    case 0x03: /* CYCLIC FILE */
      attr_ptr->file_type = MMGSDI_CYCLIC_FILE;
      attr_ptr->file_info.cyclic_file.increase_allowed =
                                        ( sim_header_attr[7] & 0x40 ) / 0x40;

      file_status_ptr = &attr_ptr->file_info.cyclic_file.file_status;
      mmgsdi_populate_icc_file_status_info(slot,
                                          sim_header_attr,
                                          file_status_ptr);

      file_security_access_ptr = &attr_ptr->file_info.cyclic_file.file_security;
      mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(sim_header_attr,
                                                      file_security_access_ptr);

      attr_ptr->file_info.cyclic_file.rec_len =  sim_header_attr[14];
      attr_ptr->file_info.cyclic_file.num_of_rec = 0;
      if (attr_ptr->file_info.cyclic_file.rec_len > 0)
      {
        attr_ptr->file_info.cyclic_file.num_of_rec =
          attr_ptr->file_size/attr_ptr->file_info.cyclic_file.rec_len;
      }
      break;

    case 0x01: /* LINEAR_FILE */
      attr_ptr->file_type = MMGSDI_LINEAR_FIXED_FILE;

      file_status_ptr = &attr_ptr->file_info.linear_fixed_file.file_status;
      mmgsdi_populate_icc_file_status_info(slot,
                                          sim_header_attr,
                                          file_status_ptr);

      file_security_access_ptr = &attr_ptr->file_info.linear_fixed_file.file_security;
      mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(sim_header_attr,
                                                      file_security_access_ptr);

      attr_ptr->file_info.linear_fixed_file.rec_len = sim_header_attr[14];
      attr_ptr->file_info.linear_fixed_file.num_of_rec = 0;
      if (attr_ptr->file_info.linear_fixed_file.rec_len > 0)
      {
        attr_ptr->file_info.linear_fixed_file.num_of_rec =
          attr_ptr->file_size/attr_ptr->file_info.linear_fixed_file.rec_len;
      }
      break;

    case 0x00: /* TRANSPARNET */
      attr_ptr->file_type = MMGSDI_TRANSPARENT_FILE;

      file_status_ptr = &attr_ptr->file_info.transparent_file.file_status;
      mmgsdi_populate_icc_file_status_info(slot,
                                          sim_header_attr,
                                          file_status_ptr);

      file_security_access_ptr = &attr_ptr->file_info.transparent_file.file_security;
      mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(sim_header_attr,
                                                      file_security_access_ptr);
      break;
    default:
      MSG_ERROR("Unable to determine file structure 0x%x",
        sim_header_attr[13], 0, 0);
    }
    break;
  default:
    attr_ptr->file_type = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
    break;
  }
} /* mmgsdi_icc_rsp_extract_file_attr *//*lint !e715 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{

  mmgsdi_request_header_type             req_header;
  int32                                  index                = 0;
  mmgsdi_return_enum_type                mmgsdi_status        = MMGSDI_SUCCESS;
  uim_cmd_type                         * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type    * extra_param_ptr      = NULL;
  mmgsdi_return_enum_type                report_status       = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI_ICC_RSP_WRITE", 0, 0, 0);

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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
         write to cache if item is in the cache
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status == MMGSDI_SUCCESS)
  {
    /* Writing to cyclic file always updates the 1st record,
     so we don't need to check the record num here since we cache the 1st record */
    mmgsdi_status = mmgsdi_util_write_cache(req_header.slot_id,
      &extra_param_ptr->write_data.access,
      extra_param_ptr->write_data.data,
      extra_param_ptr->write_data.offset,
      GSDI_DATA_FROM_CARD);
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    /* unsuccess processing of command */
    if (extra_param_ptr->write_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      MSG_HIGH("UIM_FAIL For MMGSDI EF %x",
               extra_param_ptr->write_data.access.file.file_enum, 0, 0);
    }

    MSG_HIGH("UIM_FAIL sw1 %x sw2 %x status %d", uim_rsp_ptr->status_word.sw1,
                                                 uim_rsp_ptr->status_word.sw2,
                                                 mmgsdi_status);
  } /* Parsing UIM Status */

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             uim_rsp_ptr->cmd_transacted,
                                                  /* return sw1 and sw2 for write */
                                             uim_rsp_ptr->status_word);

  MSG_HIGH("ICC RSP - WRITE status is 0x%x for file 0x%x",
               report_status,extra_param_ptr->write_data.access.file.file_enum, 0);

  return mmgsdi_status;

} /* mmgsdi_icc_rsp_write */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_READ

   DESCRIPTION:
     This function will build the response to Read request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type  mmgsdi_icc_rsp_read (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_read_cnf_type                 ** mmgsdi_cnf_ptr,
  boolean                                 synch
)
{
  mmgsdi_request_header_type            req_header;
  int32                                 index            = 0;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_read_cnf_type                * cnf_ptr          = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr  = NULL;
  uim_cmd_type                        * uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               report_status    = MMGSDI_SUCCESS;
  uim_items_enum_type                   uim_file         = UIM_NO_SUCH_ITEM;
  boolean                               sw_present       = FALSE;

  MSG_HIGH("MMGSDI_ICC_RSP_READ", 0, 0, 0);

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_cnf_ptr);

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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status == MMGSDI_SUCCESS)
  {

    if(extra_param_ptr != NULL)
    {
      if (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
      {
        if ( MMGSDI_SUCCESS != mmgsdi_util_convert_to_uim_items_enum(
                extra_param_ptr->read_data.access.file.file_enum,
                &uim_file))
        {
          MSG_ERROR("Can not convert to uim enum 0x%x",
                     extra_param_ptr->read_data.access.file.file_enum,0,0);
        }
        else
        {	
          /*Check the FCN List in case additional files are being read during refresh*/
          mmgsdi_util_check_fcn_list(uim_file);
        }  
      }

      /* write to cache. If it is record based file,
         update only if it is 1st rec */
      if( (extra_param_ptr->read_data.file_type == MMGSDI_TRANSPARENT_FILE) ||
          (( (extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE) ||
            (extra_param_ptr->read_data.file_type == MMGSDI_CYCLIC_FILE)) &&
          (extra_param_ptr->read_data.rec_num == 1)))
      {
        mmgsdi_status = mmgsdi_util_write_cache(req_header.slot_id,
                                              &extra_param_ptr->read_data.access,
                                              uim_rsp_ptr->rsp_data,
                                              extra_param_ptr->read_data.offset,
                                              GSDI_DATA_FROM_CARD);
      }
    }
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    /* unsuccess processing of command */
    if (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      MSG_HIGH("UIM_FAIL For MMGSDI EF %x",
               extra_param_ptr->read_data.access.file.file_enum, 0, 0);
    }
    MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d",
              uim_rsp_ptr->status_word.sw1,
              uim_rsp_ptr->status_word.sw2,
              report_status);
  } /* Parsing UIM Status */

  if (report_status == MMGSDI_SUCCESS)
  {
    /* if read from cache, and the operation is successful,
       the cmd_transacted will be FALSE.  if the operation is not successful,
       the command will go to uim */
    sw_present = TRUE;
  }
  else
  {
    sw_present = uim_rsp_ptr->cmd_transacted;
  }
  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status, &req_header, extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      sw_present,/* return sw1 and sw2 for read */
      uim_rsp_ptr->status_word);
  }
  else
  {
    if (mmgsdi_cnf_ptr == NULL)
    {
      MSG_ERROR("Null cnf Ptr", 0, 0, 0);
      return MMGSDI_ERROR;
    }

    /* Obtain memory for confirmation data */
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR((*mmgsdi_cnf_ptr),
                                            sizeof(mmgsdi_read_cnf_type));

    cnf_ptr = *mmgsdi_cnf_ptr;

    mmgsdi_status = mmgsdi_cnf_build_read_data(
      cnf_ptr,
      report_status,
      extra_param_ptr,
      (mmgsdi_data_type*)&uim_rsp_ptr->rsp_data);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Populate Header */
      /* Header population should be done AFTER data populate
        because any pointer length will be assigned in the
        individual confirmation population and used in the
        populate_cnf_rsp_header for assigning the response
        length */
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                          &cnf_ptr->response_header,
                                          &req_header,
                                          MMGSDI_READ_CNF,
                                          sw_present,
                                          uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
  }
  MSG_HIGH("ICC RSP - READ status is 0x%x for file 0x%x",
            report_status,
            extra_param_ptr->read_data.access.file.file_enum, 0);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_read */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SET_PIN_STATUS

   DESCRIPTION:
     This function sets the pin status for the current GSM/RUIM App

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_set_pin_status should be used

   RETURN VALUE:
   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static void mmgsdi_icc_rsp_set_pin_status(
  mmgsdi_pin_enum_type        pin_id,
  mmgsdi_int_app_info_type   *sel_app_ptr,
  byte                        pin_status_offset,
  const mmgsdi_data_type     *add_data_ptr)
{
  mmgsdi_pin_status_enum_type         pin_status   = MMGSDI_PIN_ENABLED_VERIFIED;
  boolean                             pin_verified = TRUE;
  mmgsdi_app_pin_info_type           *pin_info_ptr = NULL;

  if(sel_app_ptr == NULL || add_data_ptr == NULL)
  {
    MSG_ERROR("Bad Parameters",0,0,0);
    return;
  }

  if(pin_id == MMGSDI_PIN1)
  {
    pin_info_ptr = sel_app_ptr->pin1;
  }
  else if( pin_id == MMGSDI_PIN2)
  {
    pin_info_ptr = &sel_app_ptr->pin2;
  }
  else
  {
    MSG_ERROR("Invalid PIN %d",pin_id,0,0);
    return;
  }

  if(pin_info_ptr == NULL)
  {
    MSG_ERROR("Bad Parameters",0,0,0);
    return;
  }

  if ((pin_info_ptr->status == MMGSDI_PIN_ENABLED_NOT_VERIFIED) ||
      (pin_info_ptr->status == MMGSDI_PIN_STATUS_NOT_INITIALIZED))
    pin_verified = FALSE;

  /* --------------------------------------------------------------------------
     1. Check to see if the PIN is Initialized
     1.1 If the Permanently blocked, then set the status to PERM_BLOCKED.
     1.2 If the num Retries is zero, then the pin is blocked.
     1.3 if not and the disabled mask is set, pin is disabled
     1.4.Else PIn is Enabled and not Verified
     ------------------------------------------------------------------------*/
  if ( add_data_ptr->data_ptr[pin_status_offset] & GSDI_CODE_INIT_MASK )
  {
   if ( ( pin_id == MMGSDI_PIN1 ) &&
        ( add_data_ptr->data_ptr[GSDI_FILE_CHARAC_OFFSET] &
          GSDI_CHV1_DISABLED_MASK ) )
    {
      pin_status  = MMGSDI_PIN_DISABLED;
      MSG_HIGH("GSM PIN %x DISABLED",((uint32)pin_id),0,0);
    }
    else
    {
      /* Case for PIN1 if Enabled */
      if (pin_verified)
        pin_status = MMGSDI_PIN_ENABLED_VERIFIED;
      else
        pin_status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
      MSG_HIGH("GSM PIN %x ENABLED",((uint32)pin_id),0,0);
    }

    if ( pin_info_ptr->num_unblock_retries == 0)
    {
      pin_status   = MMGSDI_PIN_PERM_BLOCKED;
      MSG_HIGH("PIN %d is perm blocked", pin_id,0,0);
    }
    else if ( pin_info_ptr->num_retries == 0 )
    {
      pin_status   = MMGSDI_PIN_BLOCKED;
      MSG_HIGH("PIN %d is blocked", pin_id,0,0);
    }

  }
  else
  {
    /* Per spec 3GPP TS 11.11, if bit 8 of the CHV byte of the SELECT command
       response is not set then the pin_status is set to not initialized */
    pin_status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
  }

  pin_info_ptr->status = pin_status;
} /* mmgsdi_icc_rsp_set_pin_status */


/*===========================================================================
FUNCTION MMGSDI_ICC_PARSE_PIN_RSP

DESCRIPTION
  This function parses the pin rsp based on the uim rsp

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_icc_rsp_parse_pin(
  mmgsdi_return_enum_type            report_status,
  const mmgsdi_pin_extra_info_type  *pin_extra_info_ptr,
  const mmgsdi_data_type            *add_data_ptr)
{
  uint32                               num_retries           = 0;
  uint32                               num_unblock_retries   = 0;
  mmgsdi_int_app_info_type           * sel_app_ptr           = NULL;
  mmgsdi_pin_status_enum_type          pin_status            = MMGSDI_PIN_ENABLED_VERIFIED;
  mmgsdi_app_pin_info_type           * pin_info_ptr          = NULL;
  uint32                               index                 = 0;
  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  if(pin_extra_info_ptr == NULL || add_data_ptr == NULL)
  {
    MSG_ERROR("Null input ptr: pin_extra_info_ptr 0x%x, add_data_ptr 0x%x",
      pin_extra_info_ptr, add_data_ptr, 0);
    return;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
     return;

  if (pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN1)
  {
    pin_info_ptr   = sel_app_ptr->pin1;
    if(pin_info_ptr == NULL)
    {
      /*Select the First Index by Default*/
      sel_app_ptr->pin1 = &mmgsdi_app_pin1_table[index];
      sel_app_ptr->pin1->key_ref = MMGSDI_CHV1_KEY;
      pin_info_ptr = sel_app_ptr->pin1;

      sel_app_ptr->pin2.key_ref = MMGSDI_CHV2_KEY;
    }
  }
  else if (pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN2){
    pin_info_ptr   = &sel_app_ptr->pin2;
    pin_info_ptr->key_ref = MMGSDI_CHV2_KEY;
  }
  else
  {
    MSG_ERROR("Invalid PIN ID %d",pin_extra_info_ptr->pin_info.pin_id ,0,0);
    return;
  }

  pin_info_ptr->pin_id = pin_extra_info_ptr->pin_info.pin_id;

  switch(report_status)
  {
    case MMGSDI_SUCCESS:
      if(pin_extra_info_ptr->retry_type == MMGSDI_GET_NUM_RETRIES)
      {
        /* Get number of retry left for PINs and PUKs */
        sel_app_ptr->pin1->num_retries         = add_data_ptr->data_ptr[GSDI_PIN1_STATUS_OFFSET] & 0x0F;
        sel_app_ptr->pin1->num_unblock_retries = add_data_ptr->data_ptr[GSDI_PUK1_STATUS_OFFSET] & 0x0F;
        sel_app_ptr->pin2.num_retries          = add_data_ptr->data_ptr[GSDI_PIN2_STATUS_OFFSET] & 0x0F;
        sel_app_ptr->pin2.num_unblock_retries  = add_data_ptr->data_ptr[GSDI_PUK2_STATUS_OFFSET] & 0x0F;

        mmgsdi_icc_rsp_set_pin_status(MMGSDI_PIN1,sel_app_ptr,GSDI_PIN1_STATUS_OFFSET,
                                      add_data_ptr);
        mmgsdi_icc_rsp_set_pin_status(MMGSDI_PIN2,sel_app_ptr,GSDI_PIN2_STATUS_OFFSET,
                                      add_data_ptr);
      }
      /*If the PIN already exists, then change the valid FLAG TO TRUE*/
      if(pin_info_ptr->pin_data.data_len != 0)
      {
         pin_info_ptr->valid_pin = TRUE;
      }
      break;
    case MMGSDI_PIN_NOT_INITIALIZED:
    case MMGSDI_INCOMPAT_PIN_STATUS:
      //TODO - What should be done here?
      break;
    case MMGSDI_INCORRECT_CODE:  /* 0x9804 */
      pin_status            = pin_info_ptr->status;
      num_retries           = pin_info_ptr->num_retries;
      num_unblock_retries   = pin_info_ptr->num_unblock_retries;

      if (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_UNBLOCK)
      {
        /* For unblock operation, Decrementing unblock retry counter */
        num_unblock_retries= pin_info_ptr->num_unblock_retries > 0?pin_info_ptr->num_unblock_retries-1:0;
        if (num_unblock_retries == 0)
          pin_status      = MMGSDI_PIN_PERM_BLOCKED;
      }
      else
      {
        /* For other PIN operation, Decrementing PIN retry counter */	      
        num_retries = pin_info_ptr->num_retries > 0?pin_info_ptr->num_retries-1:0;
        if (num_retries == 0)
          pin_status      = MMGSDI_PIN_BLOCKED;
      }
      pin_info_ptr->status              =  pin_status;
      pin_info_ptr->num_retries         =  num_retries;
      pin_info_ptr->num_unblock_retries =  num_unblock_retries;
      break; /* INCORRECT_CODE */
    case MMGSDI_CODE_BLOCKED:
      pin_info_ptr->status       = MMGSDI_PIN_BLOCKED;
      pin_info_ptr->num_retries  = 0;
      break;

    case MMGSDI_CODE_PERM_BLOCKED:
      pin_info_ptr->status               = MMGSDI_PIN_PERM_BLOCKED;
      pin_info_ptr->num_unblock_retries  = 0;
      break; /* CODE BLOCKED */
    default:
      MSG_ERROR("Unhandled pin status 0x%x", report_status, 0, 0);
      return;
   } /* Parsing UIM Status */

   /*-------------------------------------------------------------------------------------------------
     If the PIN Is set to ENABLED NOT VERIFIED, and the operation was a Succeful Verification,
     Succeful Unblock, or A Successful Change then change the state to ENABLED_VERIFIED
     -------------------------------------------------------------------------------------------------*/
   if((report_status == MMGSDI_SUCCESS) && (pin_extra_info_ptr->retry_type == MMGSDI_RETRY_NONE))
   {
     if((pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_VERIFY) ||
        (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_UNBLOCK)||
        (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_CHANGE) ||
        (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_ENABLE))
     {
       if(pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN1 &&
          sel_app_ptr->pin1->status == MMGSDI_PIN_ENABLED_NOT_VERIFIED)
       {
         sel_app_ptr->pin1->status = MMGSDI_PIN_ENABLED_VERIFIED;
       }
       else if(pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN2 &&
               sel_app_ptr->pin2.status == MMGSDI_PIN_ENABLED_NOT_VERIFIED)
       {
         sel_app_ptr->pin2.status = MMGSDI_PIN_ENABLED_VERIFIED;
       }
     }
   }

} /* mmgsdi_icc_rsp_parse_pin*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_GET_FILE_ATTR

   DESCRIPTION:
     This function will build the response to Get File Attribute request

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function

   LIMITATIONS:
     This function is limited to the use of ICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_get_file_attr(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_get_file_attr_cnf_type        ** attr_cnf_ptr,
  boolean                                 synch
)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;
  mmgsdi_get_file_attr_cnf_type     *cnf_ptr          = NULL;

  MSG_HIGH("MMGSDI_ICC_RSP_GET_FILE_ATTR, synch: %d", synch, 0, 0);

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(attr_cnf_ptr);
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();

  MMGSDIUTIL_RETURN_IF_NULL(sel_app_ptr);
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
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  MSG_HIGH("Select status %d",report_status,0,0);

  if(req_header.orig_request_type == MMGSDI_PIN_OPERATION_REQ ||
     req_header.orig_request_type == MMGSDI_GET_ALL_PIN_STATUS_REQ)
  {
    mmgsdi_icc_rsp_parse_pin(report_status,
                            (mmgsdi_pin_extra_info_type*)extra_param_ptr->get_file_attr_data.int_client_data,
                            (mmgsdi_data_type*)&uim_rsp_ptr->rsp_data);

    if(sel_app_ptr->pin1 == NULL)
    {
      MSG_ERROR("ICC RSP - PIN1 Null",0,0,0);
      mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                                 &req_header,
                                                 extra_param_ptr,
                                                 (void*)&uim_rsp_ptr->rsp_data,
                                                 FALSE, /* internal parsing failed */
                                                 uim_rsp_ptr->status_word);
      return mmgsdi_status;

    }
    /*Set all flags to cached*/
    sel_app_ptr->pin1->cached = TRUE;
    sel_app_ptr->pin2.cached  = TRUE;

  }
  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                        &req_header,
                                        extra_param_ptr,
                                        (void*)&uim_rsp_ptr->rsp_data,
                                        FALSE, /* status word not required
                                                  for get file attr */
                                        uim_rsp_ptr->status_word);
  }
  else
  {
    /* Obtain memory for confirmation data */
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR((*attr_cnf_ptr),
                                            sizeof(mmgsdi_get_file_attr_cnf_type));

    cnf_ptr = *attr_cnf_ptr;

    mmgsdi_status = mmgsdi_cnf_build_get_file_attr_data(
      cnf_ptr,
      report_status, extra_param_ptr, &req_header, &uim_rsp_ptr->rsp_data);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Populate Header */
      /* Header population should be done AFTER data populate
        because any pointer length will be assigned in the
        individual confirmation population and used in the
        populate_cnf_rsp_header for assigning the response
        length */
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                          &cnf_ptr->response_header,
                                          &req_header,
                                          MMGSDI_GET_FILE_ATTR_CNF,
                                          FALSE, /* status word not
                                                 required for gfa */
                                          uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
  }

  MSG_HIGH("ICC RSP - GET_FILE_ATTR status is 0x%x for file 0x%x",
           report_status,
           extra_param_ptr->get_file_attr_data.access.file.file_enum, 0);
  return mmgsdi_status;
} /* mmgsdi_icc_rsp_get_file_attr */


/* Needs to evaluate if whole file can be featurized */


/*----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SEND_APDU

   DESCRIPTION:
     This function will build the response to SEND_APDU request

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_send_apdu should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_rsp_send_apdu(
  const mmgsdi_uim_report_rsp_type* uim_rsp_ptr)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI_ICC_RSP_SEND_APDU", 0, 0, 0);
  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status =
    mmgsdi_util_get_client_request_table_info(
      uim_rsp_ptr->rsp_data_index,
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
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for send apdu */
                                             uim_rsp_ptr->status_word);

  MSG_HIGH("ICC_RSP - SEND APDU   status is 0x%x", report_status, 0, 0);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_send_apdu*/ /*lint !e715 */


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{

  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type             report_status    = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI_ICC_RSP_STATUS", 0, 0, 0);

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* ------------------------------------------------------------------------
       Command processing returned Status words other than 0x90 0x00
       ----------------------------------------------------------------------*/
   MMGSDIUTIL_DEBUG_MSG_ERROR("UIM_FAIL sw1:%x sw2:%x status:%d",
                              uim_rsp_ptr->status_word.sw1,
                              uim_rsp_ptr->status_word.sw2,
                              report_status);
  } /* Parsing UIM Status */

  if ( req_header.orig_request_type == MMGSDI_CARD_RESET_REQ)
  {
    /* Build Reset command message */
    mmgsdi_status = mmgsdi_gen_build_card_reset(req_header.client_id,
                                                req_header.slot_id,
                                                req_header.orig_request_type,
                                                UIM_NO_SUCH_MODE,
                                                req_header.response_cb,
                                                (mmgsdi_client_data_type)req_header.client_data);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
       return mmgsdi_status;
    }
    /* If not able to successfully build the close req, send the confirmation */
    report_status = MMGSDI_ERROR;
  } /* orig_request_type == MMGSDI_CARD_RESET_REQ */


  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             uim_rsp_ptr->cmd_transacted,
                                               /* status word required for status cmd */
                                             uim_rsp_ptr->status_word);

  MSG_HIGH("UICC RSP - STATUS CNF   status is 0x%x",report_status,0,0);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_status */

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_REHABILITATE

   DESCRIPTION:
     This function will build the response to Rehabilitate request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_rehabilitate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_rehabitate (
  const mmgsdi_uim_report_rsp_type      *  uim_rsp_ptr,
  mmgsdi_rehabilitate_cnf_type          ** rehab_cnf_ptr,
  boolean                                  synch
)
{

  mmgsdi_request_header_type            req_header;
  int32                                 index            = 0;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr  = NULL;
  uim_cmd_type                        * uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               report_status    = MMGSDI_SUCCESS;
  mmgsdi_rehabilitate_cnf_type        * cnf_ptr          = NULL;

  MSG_HIGH("MMGSDI_ICC_RSP_REHABITATE, synch: %d", synch, 0, 0);

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(rehab_cnf_ptr);
  }
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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* unsuccess processing of command */
    if (extra_param_ptr->rehab_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      MSG_HIGH("UIM_FAIL For MMGSDI EF %x",
               extra_param_ptr->rehab_data.access.file.file_enum, 0, 0);
    }
    MSG_HIGH("UIM_FAIL sw1 %x sw2 %x status %d", uim_rsp_ptr->status_word.sw1,
                                                 uim_rsp_ptr->status_word.sw2,
                                                 report_status);
  } /* Parsing UIM Status */

  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(
        report_status,
        &req_header,
        extra_param_ptr,
        (void*)&uim_rsp_ptr->rsp_data,
        FALSE, /* status word not required for rehabilitate cmd */
        uim_rsp_ptr->status_word);
  }
  else
  {
    /* Obtain memory for confirmation data */
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR((*rehab_cnf_ptr),
                                         sizeof(mmgsdi_rehabilitate_cnf_type));
    cnf_ptr = *rehab_cnf_ptr;
    mmgsdi_status = mmgsdi_cnf_build_rehabilitate_data(cnf_ptr,
                                                       extra_param_ptr);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Populate Header */
      /* Header population should be done AFTER data populate
        because any pointer length will be assigned in the
        individual confirmation population and used in the
        populate_cnf_rsp_header for assigning the response
        length */
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                          &cnf_ptr->response_header,
                                          &req_header,
                                          MMGSDI_REHABILITATE_CNF,
                                          FALSE, /* sw not required for
                                                    rehabilitate cmd */
                                          uim_rsp_ptr->status_word);
      cnf_ptr->response_header.mmgsdi_status = report_status;
    } /* mmgsdi_cnf_build_rehabilitate_data status check */
  } /* synch process */


  MSG_HIGH("ICC RSP - REHABILITATE status is 0x%x for file 0x%x",
            report_status,
            extra_param_ptr->rehab_data.access.file.file_enum, 0);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_rehabitate */



/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_INVALIDATE

   DESCRIPTION:
     This function will build the response to Invalidate request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_invalidate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_invalidate (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{

  mmgsdi_request_header_type            req_header;
  int32                                 index            = 0;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr  = NULL;
  uim_cmd_type                        * uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               report_status    = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI_ICC_RSP_INVALIDATE", 0, 0, 0);

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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* unsuccess processing of command */
    if (extra_param_ptr->invalidate_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      MSG_HIGH("UIM_FAIL For MMGSDI EF %x",
               extra_param_ptr->invalidate_data.access.file.file_enum, 0, 0);
    }
    MSG_HIGH("UIM_FAIL sw1 %x sw2 %x status %d", uim_rsp_ptr->status_word.sw1,
                                                 uim_rsp_ptr->status_word.sw2,
                                                 report_status);
  } /* Parsing UIM Status */

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      FALSE, /* status word not required for invalidate cmd */
      uim_rsp_ptr->status_word);

  MSG_HIGH("ICC RSP - INVALIDATE status is 0x%x for file 0x%x",
            report_status,
            extra_param_ptr->invalidate_data.access.file.file_enum, 0);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_invalidate */


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_OPERATION

   DESCRIPTION:
     This function will build the response to the PIN Operation Command

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_rsp_pin_operation should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{

  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                       *uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type  *extra_param_ptr       = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type           *sel_app_ptr           = NULL;
  mmgsdi_access_type                  access_type;
  mmgsdi_pin_extra_info_type         *pin_info_ptr          = NULL;

  MSG_HIGH("MMGSDI_ICC_RSP_PIN_OPERATION", 0, 0, 0);

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* ------------------------------------------------------------------------
       Command processing returned Status words other than 0x90 0x00
       ----------------------------------------------------------------------*/
   MMGSDIUTIL_DEBUG_MSG_ERROR("UIM_FAIL sw1:%x sw2:%x status:%d",
                              uim_rsp_ptr->status_word.sw1,
                              uim_rsp_ptr->status_word.sw2,
                              report_status);
  }

  /*Parse the Response from UIM and the Set the Pin Status and NUM Retries
    accordingly*/
  mmgsdi_icc_rsp_parse_pin(report_status,
                           &extra_param_ptr->pin_data,
                          (mmgsdi_data_type*)&uim_rsp_ptr->rsp_data);

  /*Copy the information back to extra param data*/
  if ( extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN1 )
  {
    memcpy(&extra_param_ptr->pin_data.pin_info,
          sel_app_ptr->pin1,
          sizeof(mmgsdi_app_pin_info_type));
  }
  else if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN2)
  {
    memcpy(&extra_param_ptr->pin_data.pin_info,
            &sel_app_ptr->pin2,
            sizeof(mmgsdi_app_pin_info_type));
  }

  if (report_status == MMGSDI_SUCCESS)
  {
    /*----------------------------------------------------------------------------------
     Now get the Status and the Number of Retries for the PINs by doing a Select
     on MMGSDI_GSM_DIR
     ----------------------------------------------------------------------------------*/
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                          pin_info_ptr,
                         sizeof(mmgsdi_pin_extra_info_type));
    extra_param_ptr->pin_data.retry_type      = MMGSDI_GET_NUM_RETRIES;
    memcpy(pin_info_ptr,
           &extra_param_ptr->pin_data,
           sizeof(mmgsdi_pin_extra_info_type));

    access_type.access_method     = MMGSDI_DF_ENUM_ACCESS;
    if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
    {
      access_type.file.df_enum      = MMGSDI_GSM_DIR;
    }
    else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
    {
      access_type.file.df_enum      = MMGSDI_RUIM_DIR;
    }

    /*Get the Number of Retries*/
    mmgsdi_status = mmgsdi_util_build_get_file_attr_req(&req_header,
                                                        access_type,
                                                        (mmgsdi_client_data_type)pin_info_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(pin_info_ptr);
      mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                                 &req_header,
                                                 extra_param_ptr,
                                                 (void*)&uim_rsp_ptr->rsp_data,
                                                 FALSE, /* status word not required
                                                           for pin op */
                                                 uim_rsp_ptr->status_word);

      MSG_HIGH("UICC RSP - PIN OPERATION status is 0x%x for PIN_OP %d,PIN_STATUS = %d",
                report_status,
                extra_param_ptr->pin_data.orig_pin_op,
                extra_param_ptr->pin_data.pin_info.status);

    }
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for pin op */
                                             uim_rsp_ptr->status_word);

  MSG_HIGH("ICC RSP - PIN OPERATION   status is 0x%x for PIN_OP %d,PIN_STATUS = %d",
            report_status,
            extra_param_ptr->pin_data.orig_pin_op,
            extra_param_ptr->pin_data.pin_info.status);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_pin_operation*/


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_STATUS

   DESCRIPTION:
     This function will build the response to the PIN Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_pin_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_pin_extra_info_type        * pin_info_ptr          = NULL;
  mmgsdi_access_type                  access_type;
  mmgsdi_int_app_info_type          * sel_app_ptr           = NULL;

  MSG_HIGH("MMGSDI_ICC_RSP_PIN_STATUS", 0, 0, 0);

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
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

  if (!uim_rsp_ptr->is_uim_success)
  {

    MSG_HIGH("ICC RSP - PIN STATUS  Internal failure (mmgsdi_error_status 0x%x)",
      uim_rsp_ptr->mmgsdi_error_status, 0, 0);

    mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               FALSE, /* sw not required due to
                                                         internal processing error*/
                                               uim_rsp_ptr->status_word);
    return mmgsdi_status;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  if(mmgsdi_util_is_sel_app_pin_cached())
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               FALSE, /* status word not required
                                                         for pin status */
                                               uim_rsp_ptr->status_word);

    MSG_HIGH("ICC RSP - PIN STATUS   status is 0x%x", mmgsdi_status,0,0);

    return mmgsdi_status;
  }

  /* Now do a Select on MMGSM_DIR to get the number of retries and number of
     unblock retries*/
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                            pin_info_ptr,
                            sizeof(mmgsdi_pin_extra_info_type));
  /*PIN ID = 1*/
  pin_info_ptr->orig_pin_op     = MMGSDI_PIN_OP_NONE;
  pin_info_ptr->pin_info.pin_id = MMGSDI_PIN1;
  pin_info_ptr->retry_type      = MMGSDI_GET_NUM_RETRIES;
  pin_info_ptr->pin_info.status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
  access_type.access_method     = MMGSDI_DF_ENUM_ACCESS;

  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    access_type.file.df_enum      = MMGSDI_GSM_DIR;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    access_type.file.df_enum      = MMGSDI_RUIM_DIR;
  }
  mmgsdi_status = mmgsdi_util_build_get_file_attr_req(&req_header,
                                                      access_type,
                                                      (mmgsdi_client_data_type)pin_info_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*PIN Info Ptr would get freed in mmgsdi_util_free_client_request_table_index,
      No need to free it again here*/
    mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               FALSE, /* status word not required
                                                         because of mmgsdi internal
                                                         failure */
                                               uim_rsp_ptr->status_word);

    MSG_HIGH("ICC RSP - PIN OPERATION   Internal error: status 0x%x for PIN_OP %d,PIN_STATUS = %d",
              mmgsdi_status,
              extra_param_ptr->pin_data.orig_pin_op,
              extra_param_ptr->pin_data.pin_info.status);
  }

  return mmgsdi_status;

} /* mmgsdi_icc_rsp_pin_status*/


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


#ifdef FEATURE_MMGSDI_CDMA

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_COMPUTE_IP_AUTH

   DESCRIPTION:
     This function will build the response to a COMPUTE IP AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_compute_ip_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;

  MSG_HIGH("MMGSDI_ICC_RSP_COMPUTE_IP_AUTH", 0, 0, 0);

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
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
  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    MSG_HIGH("UIM_FAIL sw1 %x sw2 %x status %d", uim_rsp_ptr->status_word.sw1,
                                                 uim_rsp_ptr->status_word.sw2,
                                                 report_status);
  } /* Parsing UIM Status */

  /* Print the data being sent from UIM*/
  mmgsdi_util_print_data_to_diag("From Card : ",
                                  uim_rsp_ptr->rsp_data.data_ptr,
                                  uim_rsp_ptr->rsp_data.data_len);

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      FALSE, /* status word not required for compute ip auth */
      uim_rsp_ptr->status_word);

  MSG_HIGH("ICC RSP - Compute IP AUTH status is 0x%x", report_status,0, 0);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_compute_ip_auth */
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */


#ifdef FEATURE_AUTH
/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_RUN_CAVE

   DESCRIPTION:
     This function will build the response to a RUN CAVE command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_run_cave (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;

  MSG_HIGH("MMGSDI_ICC_RUN_CAVE_STATUS", 0, 0, 0);

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
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
  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    MSG_HIGH("UIM_FAIL sw1 %x sw2 %x status %d", uim_rsp_ptr->status_word.sw1,
                                                 uim_rsp_ptr->status_word.sw2,
                                                 report_status);
  } /* Parsing UIM Status */

  /* Print the data being sent from UIM*/
  mmgsdi_util_print_data_to_diag("From Card : ",
                                  uim_rsp_ptr->rsp_data.data_ptr,
                                  uim_rsp_ptr->rsp_data.data_len);

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      FALSE, /* status word not required for run cave */
      uim_rsp_ptr->status_word);

  MSG_HIGH("ICC RSP - RUN CAVE status is 0x%x", report_status,0, 0);

  return mmgsdi_status;
} /* mmgsdi_icc_run_cave */


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_GENERATE_KEY_VPM

   DESCRIPTION:
     This function will build the response to a GENERATE KEY command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_generate_key_vpm (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_data_type                    response_data;

  MSG_HIGH("MMGSDI_ICC_GENERATE_KEY_VPM_STATUS", 0, 0, 0);
  memset(&response_data, 0x00, sizeof(mmgsdi_data_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
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
  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_ptr->status_word.sw1,
                                                 uim_rsp_ptr->status_word.sw2,
                                                 report_status);
  } /* Parsing UIM Status */

  /* Look back in the original command to see what the expected size of the
    response should be. The response should an 8-byte key, plus n VPM Octets
    where n is last - first + 1 according to IS-820-C */

  if((uim_cmd_ptr->gen_keys.vpm_first_octet != 0xFF) &&
     (uim_cmd_ptr->gen_keys.vpm_last_octet != 0xFF) &&
      (uim_cmd_ptr->gen_keys.vpm_last_octet >=
       uim_cmd_ptr->gen_keys.vpm_first_octet))
  {
    response_data.data_len = (uim_cmd_ptr->gen_keys.vpm_last_octet) -
                                     (uim_cmd_ptr->gen_keys.vpm_first_octet) + 1 + MMGSDI_KEY_SIZE;
    if(uim_rsp_ptr->rsp_data.data_len > UIM_MAX_CHARS)
    {
       MSG_HIGH("Invalid response length from GENERATE KEY command. Length: %d",
                 uim_rsp_ptr->rsp_data.data_len,0,0);
       return MMGSDI_ERROR;
    } /* end if Response is too long, something wrong. Limit to UIM_MAX_CHARS */
  } /* end if - the UIM Command data has valid first / last octet data */
  else
  {
    MSG_HIGH("Invalid input data for Generate Key, not returning any octets in response",
             0,0,0);
    return MMGSDI_ERROR;
  } /* end if  - the UIM command Octet Data has invalid first / last info */

  /* Obtain memory for confirmation data */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(response_data.data_ptr,
                                            response_data.data_len);
  memcpy(response_data.data_ptr,uim_rsp_ptr->rsp_data.data_ptr,
          response_data.data_len);
  /* Print the data being sent from UIM*/
  mmgsdi_util_print_data_to_diag("From Card : ",
                                  response_data.data_ptr,
                                  response_data.data_len);

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&response_data,
      FALSE, /* status word not required for generate key */
      uim_rsp_ptr->status_word);

  MSG_HIGH("ICC RSP - GENERATE KEY status is 0x%x", report_status,0, 0);
  MMGSDIUTIL_TMC_MEM_FREE(response_data.data_ptr);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_generate_key_vpm */
#endif /* FEATURE_AUTH */
#endif /* FEATURE_MMGSDI_CDMA */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SEARCH

   DESCRIPTION:
     This function will build the response to search request

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_search (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr
)
{
  mmgsdi_request_header_type            req_header;
  int32                                 index                 = 0;
  mmgsdi_return_enum_type               mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                        * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type               report_status         = MMGSDI_SUCCESS;


  MSG_HIGH("MMGSDI_ICC_RSP_SEARCH", 0, 0, 0);

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

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }
  /* Parse the response status */
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  if (report_status != MMGSDI_SUCCESS)
  {
    /* unsuccessful processing of command */
    MSG_ERROR("UIM Response parsing is fail for ICC Search",0,0,0);
    mmgsdi_util_print_file_information(extra_param_ptr->search_data.access,
                                       TRUE);

    /* -------------------------------------------------------------------
       Command processing returned Status words other than 0x90 0x00
       Log Error Message
       ----------------------------------------------------------------------*/
    MMGSDIUTIL_DEBUG_MSG_ERROR("UIM_FAIL sw1:0x%x sw2:0x%x status:%d",
                                uim_rsp_ptr->status_word.sw1,
                                uim_rsp_ptr->status_word.sw2,
                                report_status);
  }/* End of if (report_status == MMGSDI_SUCCESS) */


  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             uim_rsp_ptr->cmd_transacted,
                                             /* return sw1 and sw2 for search*/
                                             uim_rsp_ptr->status_word);

  MSG_HIGH("UICC RSP - ICC SEARCH status is 0x%x",mmgsdi_status,0,0);
  mmgsdi_util_print_file_information(extra_param_ptr->search_data.access,FALSE);
  return mmgsdi_status;
}/* End of mmgsdi_icc_rsp_search */


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */

