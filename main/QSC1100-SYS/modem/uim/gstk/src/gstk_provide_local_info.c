/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           G S T K  P R O V I D E  L O C A L  I N F O R M A T I O N


GENERAL DESCRIPTION
  This source file contains functions to process provide local information
  proactive command


FUNCTIONS
  gstk_send_provide_local_info_terminal_rsp
    This function populate the gstk_provide_local_info_cnf_type and put the
    terminal response confirm type to the GSTK queue.

  gstk_provide_local_info_req
    This function parses the provide local info Proactive Command from the card
    and sends it to the client.

  gstk_provide_local_info_cnf
    This function packs the provide local info terminal response and send
    the command to UIM server.

  gstk_provide_local_info_cnf_cleanup
    This function cleans up any allocated pointer on the stack.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_provide_local_info.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/09   sg      Added static keyword to remove warnings in gstk_process_pli_utran_nmr_req
04/07/09   xz      Fix issue of DATE/TIME/TIME ZONE failure
03/06/09   gg      If in limited service terminal response result for NW
                   measurement must be set to
                   GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE
09/14/08   sk      Fixed lint errors
09/09/08   xz      Fix for compilation warning
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/07/08   sk      Fix for PLI UTRAN NMR Limited Service
03/17/08   sk      Fixed GSTK issue for No payload with Terminal Response for
                   PLI NMR
02/29/08   yb      Fixed malloc failure if BCCH list from network is empty
02/13/08   sk      Fixed compilation error on 1X
02/06/08   sk      Support for PLI UTRAN NMR
01/30/08   yb      Fixed Compiler warning for clk_rtc_get_time()
10/03/07   sk      Fixed support for PLI NMR UTRAN
08/07/07   sk      Fixed compilation problem / warning
05/11/07   sp      Featurize for LTK
05/02/07   nk      Added Featurization
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
03/13/07   sk      Initialized rtc_julian
02/01/07   sk      Added Null ptr checks
12/14/06   sk      Lint fixes
10/23/06   sk      Fixed lint errors.
08/25/06   sk      Fixed typo
08/22/06   sk      Fixed PLI-ESN to obtain info from HW
07/25/06   nk      Fixed potential integer underflow problem
06/30/06   sk      Lint warning
06/24/06   sk      Lint fixes
06/13/06   sk      Fixed compilation problems for 1X build.
06/06/06   sk      Added support for network search mode
05/18/06  jar/nk   Fixed compile error in CDMA build as a result of libprot.h
05/17/06   sk      Lint fixes
05/11/06   sk      Removed warnings
05/03/06   sk      Removed cell id checks
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information.
04/04/06   sk      Added support for pli - Battery status, ESN, MEID, IMEISV
11/21/05   sp      Fixed lint errors
11/17/05   sk      Removed warnings
11/16/05   sk      Changed additional info to conform to the specs.
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
11/03/05   sk      Fixed compilation warning
10/10/05   tml     Used cached rat value for Access Tech.  Add support to use
                   real time info from MM, RR, RRC, and CM if no event has
                   ever received from MM IDle yet
10/08/05   tml     Used most updated cached info for Location Status
10/05/05   sk      Memset cell id field when in Limited service.
10/06/05   tml     Added FEATURE_SEC_TIME support
09/30/05   sk      Added support for two tables, one for RRC location information
                   and one for MM location information
09/27/05   sk      Added check for service status type in
                   gstk_provide_local_info_req()
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/19/05   sk      Fix for IMEI last byte
07/13/05   sk      Added support for Access Technology
05/25/05   sk      Added support for event downloads when changing to cells,
                   all withing limited service.
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/24/04  sk/tml   memset request buffer before assignment
08/03/04   tml     Memory leak fix and changed to use pointer for additional
                   info in terminal response
05/26/04   tml     Added More debug messages
04/16/04   tml     linted
04/02/04   tml     linted
03/01/04   tml     Added language, NMR, BCCH and Date and Time fixes
02/24/04   tml     Fixed compilation error
02/23/04   tml     Fixed compilation error
02/06/04   wli     Featurization for CDMA only environment
09/12/03   tml     Fixed IMEI local info issue
06/23/03   tml     removed compiler error for 1x/gsm
03/25/03   tml     Fixed typo
03/21/03   tml     Featurization fix for WCDMA single mode build
03/07/03   tml     Updated RTC info
03/06/03   tml     Updated enum names, Added FEATURE_GSTK_FOR_NON_LTK_BUILD
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */

#include "err.h"
#include "string.h"
#include "tmc.h"
#ifdef FEATURE_PMIC_RTC
#include "clkrtc.h"
#endif

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/


#ifdef FEATURE_SEC_TIME
extern void sectime_get_local_time(uint64 *ret_val);
#endif /* #ifdef FEATURE_SEC_TIME */

#ifdef FEATURE_PMIC_RTC
extern void time_set_julian( time_julian_type *jul_time );
extern void time_get_julian( time_julian_type *jul_time );
#endif /* FEATURE_PMIC_RTC */
#endif /*(#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

#ifdef FEATURE_GSTK_REMOTE_TIME
#include "time_remote.h"
#endif /* FEATURE_GSTK_REMOTE_TIME */

/*===========================================================================
FUNCTION gstk_provide_local_info_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  provide_local_info_cnf_ptr: [Input] Pointer to message that is required to
                                      be processed and sent to UIM
  STK_response_ptr          : [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_provide_local_info_cnf_cleanup(
  gstk_provide_local_info_cnf_type           *provide_local_info_cnf_ptr,
  provide_local_info_terminal_response_type  *STK_response_ptr)
{
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK_response_ptr ERR:NULL",0,0,0);
    return;
  }
  if(provide_local_info_cnf_ptr == NULL)
  {
    MSG_ERROR("provide_local_info_cnf_ptr ERR:NULL",0,0,0);
    return;
  }
  /* free provide local info cnf and tlv */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info = NULL;
  }
  if (provide_local_info_cnf_ptr->local_info_rsp.info_type == GSTK_NMR_BCCH_INFO) {
    if (STK_response_ptr->local_info_rsp.nmr_bcch.bcch_ch_list.channel_list != NULL ) {
      gstk_free(STK_response_ptr->local_info_rsp.nmr_bcch.bcch_ch_list.channel_list);
      STK_response_ptr->local_info_rsp.nmr_bcch.bcch_ch_list.channel_list= NULL;
    }
  }
  if (provide_local_info_cnf_ptr->local_info_rsp.info_type == GSTK_NMR_UTRAN_INFO) {
    if (STK_response_ptr->local_info_rsp.nmr_utran.measurement_report_ptr != NULL ) {
      gstk_free(STK_response_ptr->local_info_rsp.nmr_utran.measurement_report_ptr);
      STK_response_ptr->local_info_rsp.nmr_utran.measurement_report_ptr = NULL;
    }
  }
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)provide_local_info_cnf_ptr);/*lint !e826 area too small */
} /* gstk_provide_local_info_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_send_provide_local_info_terminal_rsp

DESCRIPTION
  This function populate the gstk_provide_local_info_cnf_type and put the
  terminal response confirm type to the GSTK queue.

PARAMETERS
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  cmd_num           : [Input] This is the same value as that was being passed to
                              the client in the corresponding proactive command
                              request
  general_result    : [Input] Specifies the result of the proactive command
                              operation
  additional_result_ptr: [Input] Specifies the optional additional result for the
                             proactive command, the length field in the
                             gstk_additonal_info_type specifies number of
                             additional_info in the structure
  other_info_ptr: [Input] Pointer to the gstk_local_info_rsp_type

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type  gstk_send_provide_local_info_terminal_rsp(
  uint32                                  cmd_details_ref_id,
  uint8                                   cmd_num,
  gstk_general_result_enum_type           general_result,
  const gstk_additional_info_type         *additional_result_ptr,
  const gstk_local_info_rsp_type          *other_info_ptr)
{

  gstk_task_cmd_type              *task_cmd_ptr = NULL;

  if(additional_result_ptr == NULL)
  {
    MSG_ERROR("additional_result_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if(other_info_ptr == NULL)
  {
    MSG_ERROR("other_info_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if (general_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY) {
    if (command_details_ref_table[cmd_details_ref_id].partial_comprehension) {
      general_result = GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION;
    }
  }

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr != NULL ) { /* successfully obtained command buffer */
    /* build the terminal response */

    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.command_id = (int)GSTK_PROVIDE_LOCAL_INFO_CNF;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.user_data = 0;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_number = cmd_num;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result = general_result;

    /* initialize additional info length to zero */
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

    if (additional_result_ptr->length != 0) {
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr =
                      gstk_malloc(additional_result_ptr->length);
      if (task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
        MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
        /* put the queue back to the free queue */
        (void)q_link(task_cmd_ptr, &task_cmd_ptr->cmd_hdr.link);
        q_put(task_cmd_ptr->cmd_hdr.done_q_ptr, &(task_cmd_ptr->cmd_hdr.link));
        return GSTK_MEMORY_ERROR;
      }
      memcpy(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr,
            additional_result_ptr->additional_info,
            additional_result_ptr->length);
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = additional_result_ptr->length;
    }

    if((general_result != GSTK_COMMAND_PERFORMED_SUCCESSFULLY) &&
       (general_result != GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE) &&
       (general_result != GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION)) {
      /* no need to include extra parameter */
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;
    }
    else {
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = other_info_ptr->info_type;

      switch(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type)
      {
      case GSTK_GSM_UMTS_LOCATION_INFO:
      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
      #else /* if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
        MSG_HIGH("Loc Info Type: GSTK_GSM_UMTS_LOCATION_INFO - Feature CCAT not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
        break;
      case GSTK_CDMA_LOCATION_INFO:
      #ifdef FEATURE_CAT_REL6
      #ifdef FEATURE_CCAT
        memset(&(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.cdma_loc_info),
              0x00,
              sizeof(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.cdma_loc_info));
        /* This is a structure to structure copy */
        task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.cdma_loc_info =
          other_info_ptr->info_data.cdma_loc_info;
      #else /*#ifdef FEATURE_CCAT */
        MSG_HIGH("Loc Info Type : GSTK_CDMA_LOCATION_INFO - Feature CCAT not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /*#ifdef FEATURE_CCAT */
      #else  /* #ifdef FEATURE_CAT_REL6 */
        MSG_HIGH("Loc Info Type : GSTK_CDMA_LOCATION_INFO - FEATURE_CAT_REL6 not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /* #ifdef FEATURE_CAT_REL6 */
        break;
      case GSTK_NO_SRVC_LOCATION_INFO:
        break;
      case GSTK_IMEI_INFO:
      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
      #else /* if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
        MSG_HIGH("Loc Info Type: GSTK_IMEI_INFO - Feature CCAT not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
        break;
      case GSTK_IMEISV_INFO:
      #ifdef FEATURE_CAT_REL6
      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
      #else /* if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
        MSG_HIGH("Loc Info Type: GSTK_IMEISV_INFO - Feature GSM/WCDMA not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
      #else  /* #ifdef FEATURE_CAT_REL6 */
        MSG_HIGH("Loc Info Type: GSTK_IMEISV_INFO - FEATURE_CAT_REL6 not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /* #ifdef FEATURE_CAT_REL6 */
        break;
      case GSTK_ESN_INFO:
      #ifdef FEATURE_CAT_REL6
      #ifdef FEATURE_CCAT
        memcpy(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.esn,
               other_info_ptr->info_data.esn,
               GSTK_ESN_LEN);
      #else /* #ifdef FEATURE_CCAT */
        MSG_HIGH("Loc Info Type : GSTK_ESN_INFO -Feature CCAT not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /*#ifdef FEATURE_CCAT */
      #else  /* #ifdef FEATURE_CAT_REL6 */
        MSG_HIGH("Loc Info Type : GSTK_ESN_INFO -FEATURE_CAT_REL6 not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /* #ifdef FEATURE_CAT_REL6 */
        break;
      case GSTK_MEID_INFO:
      #ifdef FEATURE_CAT_REL6
      #ifdef FEATURE_CCAT
        memcpy(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.meid,
               other_info_ptr->info_data.meid,
               GSTK_MEID_LEN);
      #else /* #ifdef FEATURE_CCAT */
        MSG_HIGH("Loc Info Type : GSTK_MEID_INFO - Feature CCAT not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /*#ifdef FEATURE_CCAT */
      #else  /* #ifdef FEATURE_CAT_REL6 */
        MSG_HIGH("Loc Info Type : GSTK_MEID_INFO - FEATURE_CAT_REL6 not supported",0,0,0);
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      #endif /* #ifdef FEATURE_CAT_REL6 */
        break;
      case GSTK_NMR_BCCH_INFO:
      #if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
      #ifdef FEATURE_GSM
#error code not present
      #endif /* FEATURE_GSM*/
      #endif /*FEATURE_GSTK_FOR_NON_LTK_BUILD*/
        break; /* case GSTK_NMR_BCCH_INFO */
      case GSTK_NMR_UTRAN_INFO:
        MSG_ERROR("Unexpected/unhandled case for UTRAN NMR that does asynchronous call to RRC",0,0,0);
        break;
      case GSTK_DATE_TIME_ZONE_INFO:
        memcpy(&task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.time,
               &other_info_ptr->info_data.time,
               sizeof(gstk_date_time_rsp_type));
        break;
      case GSTK_LANGUAGE_SETTING_INFO:
        task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.command_id = (int) GSTK_PROVIDE_LANG_INFO_CNF;
        memcpy(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.lang_code,
               other_info_ptr->info_data.lang_code,
               2);
        break;
      case GSTK_TIMING_ADVANCE_INFO:
        memcpy(&task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.time_adv,
               &other_info_ptr->info_data.time_adv,
               sizeof(gstk_time_advance_rsp_type));
        break;
      case GSTK_ACCESS_TECHNOLOGY_INFO:
        task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.gstk_access_tech =
          other_info_ptr->info_data.gstk_access_tech;
        break;
      default:
        break;

      }
    } /* command performed successfully */
    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
    return GSTK_SUCCESS;
  }
  else {
    MSG_ERROR("No more gstk buf", 0, 0, 0);
    return GSTK_MEMORY_ERROR;
  }

} /* gstk_send_provide_local_info_terminal_rsp */

/*===========================================================================
FUNCTION gstk_process_pli_utran_nmr_req

DESCRIPTION
  This function processes the pli utran nmr request sent from the card

PARAMETERS
  cmd_data_ptr:                   [Input] Pointer to command data buffer
  cmd_data_len:                   [Input] Length of cmd_data_ptr buffer
  orig_offset:                    [Input] Original offset in the buffer
  next_tlv_offset_ptr:            [Input/Output] Pointer to next tlv offset
                                  in the buffer
  more_tlvs_ptr:                  [Input/Output] Pointer to more tlv flag
  ref_id_ptr:                     [Input] Pinter to reference id in the
                                  command_details_ref_table
  nmr_utran_qualifier_ptr:        [Input/Output] Pointer to nmr qualifier flag
  need_to_send_immediate_tr_ptr:  [Input/Output] Pointer to flag that indicates the
                                  need to send immediate terminal response
  general_result_ptr:             [Input/Output] Pointer to general result
  additional_info_code_ptr:       [Input/Output] Pointer to additional info

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_BAD_PARAM: Bad Input paramaters passed in
    GSTK_SUCCESS:   Processing was successful

COMMENTS
  None

SIDE EFFECTS
  If the processing is successful, a request to get the UTRAN NMR informatiom
  will be sent to RRC.RRC response for UTRAN NMR will be received
  asycnhronously by GSTK

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_process_pli_utran_nmr_req (
  uint8                                *cmd_data_ptr,
  int32                                 cmd_data_len,
  int32                                 orig_offset,
  int32                                *next_tlv_offset_ptr,
  boolean                              *more_tlvs_ptr,
  uint32                               *ref_id_ptr,
  gstk_nmr_utran_qualifier_enum_type   *nmr_utran_qualifier_ptr,
  boolean                              *need_to_send_immediate_tr_ptr,
  gstk_general_result_enum_type        *general_result_ptr,
  gstk_additional_info_type            *additional_info_code_ptr
)
{
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_WCDMA
#error code not present
#else /* FEATURE_WCDMA*/
  (void)cmd_data_ptr;
  (void)cmd_data_len;
  (void)orig_offset;
  (void)next_tlv_offset_ptr;
  (void)more_tlvs_ptr;
  (void)ref_id_ptr;
  (void)nmr_utran_qualifier_ptr;
  (void)need_to_send_immediate_tr_ptr;
  MSG_HIGH(" WCDMA undefined. NMR not supported", 0, 0, 0);
  *general_result_ptr = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
  return GSTK_SUCCESS;
#endif /* FEATURE_WCDMA */
#else
  (void)cmd_data_ptr;
  (void)cmd_data_len;
  (void)orig_offset;
  (void)next_tlv_offset_ptr;
  (void)more_tlvs_ptr;
  (void)ref_id_ptr;
  (void)nmr_utran_qualifier_ptr;
  (void)need_to_send_immediate_tr_ptr;
  (void)general_result_ptr;
  (void)additional_info_code_ptr;
#endif /* #if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD) */
} /* gstk_process_pli_utran_nmr_req */

/*===========================================================================
FUNCTION gstk_provide_local_info_req

DESCRIPTION
  This function parses the Provide Local Info Proactive Command from the card
  and get the information from the various module and send the terminal
  response to the Card.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_provide_local_info_req (
  gstk_proactive_cmd_type* command_ptr)
{
  uint8                              *cmd_data_ptr                  = NULL;
  int32                               cmd_data_len                  = 0;
  uint32                              cmd_details_ref_id            = 0;
  gstk_status_enum_type               gstk_status                   = GSTK_SUCCESS;
  gstk_additional_info_type           additional_info               = {0};
  gstk_general_result_enum_type       general_result                = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  gstk_local_info_enum_type           local_info_type_req_from_card = GSTK_NO_LOCAL_INFO;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */
  boolean                             need_to_send_terminal_rsp     = TRUE;
  int32                               curr_offset                   = 0;
  int32                               new_offset                    = 0;
  boolean                             more_tlv                      = TRUE;
  gstk_nmr_utran_qualifier_enum_type  nmr_utran_qualifier           = GSTK_NMR_NONE_MEAS;
  gstk_generic_data_type              gen_data                      = {0};

  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;
  gstk_local_info_rsp_type            local_info;

#ifdef FEATURE_CAT_REL6
  cm_client_status_e_type             cm_reg_return_status          = CM_CLIENT_ERR_NONE;
  uint32                              data_blk                      = 0;
#ifdef FEATURE_CCAT
#ifdef FEATURE_UIM_RUIM
  dword                               esn_buf                       = 0;
#endif /* FEATURE_UIM_RUIM */
  dword                               meid_buf[2]                   = {0,0};
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */

#if defined(FEATURE_SEC_TIME) || defined(FEATURE_GSTK_REMOTE_TIME)
#if defined(FEATURE_SEC_TIME)
  uint64                              ms                            = 0;
#endif /* FEATURE_SEC_TIME */
  dword                               secs                          = 0;
#endif /* FEATURE_SEC_TIME || FEATURE_GSTK_REMOTE_TIME */
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /*  (FEATURE_GSM) || (FEATURE_WCDMA)*/
#if (defined FEATURE_SEC_TIME) || (defined FEATURE_PMIC_RTC)
  clk_julian_type                     rtc_julian                   = {0};
#endif /* FEATURE_SEC_TIME || FEATURE_PMIC_RTC */

  MSG_HIGH(" IN GSTK_PROVIDE_LOCAL_INFO_REQ ",0,0,0);

  if(command_ptr == NULL)
  {
    MSG_ERROR("command_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize request to 0 */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));
  memset(&local_info, 0x00, sizeof(gstk_local_info_rsp_type));
  local_info.info_type = GSTK_INVALID_LOC_INFO;

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  /* GSTK_SUCCESS from preprocess_command */
  if(gstk_status == GSTK_SUCCESS)
  {
    /* No error has been encountered in parsing */
    /* general_result default to success at the beginning of this function */

    /* get info from respective modules */
    local_info_type_req_from_card =
      (gstk_local_info_enum_type)command_details_ref_table[cmd_details_ref_id].command_details.qualifier;

    /* Note: This piece of code is here to determine the NMR type (whether BCCH or UTRAN)
       **if** the PLI type is for Network Measurement Results.
       This is currently determined by checking for extra tlv
       **the utran measurement qual tlv** that is present for UTRAN NMR
       For all others PLI, the cmd_data_ptr will be NULL
    */
    if((cmd_data_len > 0) && (cmd_data_ptr != NULL))
    {
      gen_data.data_len = GSTK_UTRAN_NMR_QUAL_LEN;
      /* For PLI NMR, the first element of cmd_data_ptr will carry the UTRAN
         measurement qualifier tlv tag
      */
      gen_data.data_buffer_ptr = cmd_data_ptr;
    }
    gstk_map_card_local_info_to_gstk_local_info(
        local_info_type_req_from_card,
        &(local_info.info_type),
        &gen_data);

    switch(local_info.info_type)
    {
    case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
      MSG_MED(" .... CDMA Location Info", 0, 0, 0);
      /* initialize to zero */
      memset(&local_info.info_data.location_info, 0, sizeof(gstk_location_info_rsp_type));

      general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

      need_to_send_terminal_rsp = TRUE;
      if(cdma_location_info_cache.is_loc_info_valid == FALSE)
      {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        MSG_ERROR("Invalid CDMA location info", 0, 0, 0);
      }
      else
      {
        /* This is a structure to structure copy */
        local_info.info_data.cdma_loc_info = cdma_location_info_cache.loc_info;
      }
#else  /*#ifdef FEATURE_CCAT */
       MSG_HIGH("Loc Info Type : GSTK_CDMA_LOCATION_INFO - Feature CCAT not supported",0,0,0);
       general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /*#ifdef FEATURE_CCAT */
#else  /* #ifdef FEATURE_CAT_REL6 */
       MSG_HIGH("Loc Info Type : GSTK_CDMA_LOCATION_INFO - FEATURE_CAT_REL6 not supported",0,0,0);
       general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_GSM_UMTS_LOCATION_INFO:
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#else
      MSG_HIGH("Loc Info Type: GSTK_GSM_UMTS_LOCATION_INFO - Feature CCAT not supported",0,0,0);
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /* if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
      break;

    case GSTK_NO_SRVC_LOCATION_INFO:
      MSG_HIGH(" .... GSTK_NO_SRVC_LOCATION_INFO", 0, 0, 0);
      general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      gstk_set_additional_info(&additional_info, GSTK_NO_SERVICE_AVAILABLE);
      break;

    case GSTK_IMEI_INFO:
      MSG_HIGH(" .... IMEI", 0, 0, 0);
      /* initialize to zero */
      memset(local_info.info_data.imei, 0, GSTK_IMEI_LEN);

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#else /* if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
      MSG_HIGH("Loc Info Type: GSTK_GSM_UMTS_LOCATION_INFO - Feature GSM/WCDMA not supported",0,0,0);
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/
      break;
    case GSTK_ESN_INFO:
      MSG_HIGH(" .... ESN", 0, 0, 0);
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if (defined FEATURE_CAT_REL6) && (defined FEATURE_CCAT) && (defined FEATURE_UIM_RUIM)
      /* Get ESN information from HW and not from NV as NV can have either
         RUIM-ID or actual HW ESN
      */
      esn_buf = uim_get_esn_me();
      if(esn_buf == 0x00)
      {
        /* ESN info from the HW was not cached properly by TMC */
        MSG_HIGH("HW ESN info not available", 0, 0, 0);
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
      else
      {
        /* initialize to zero */
        memset(local_info.info_data.esn, 0, GSTK_ESN_LEN);
        memcpy(local_info.info_data.esn, &esn_buf, GSTK_ESN_LEN);
      }
#else /* #if (defined FEATURE_CAT_REL6) && (defined FEATURE_CCAT) && (defined FEATURE_UIM_RUIM) */
      MSG_HIGH("Loc Info Type : GSTK_ESN_INFO - Feature CCAT/CAT_REL6/UIM_RUIM not supported",0,0,0);
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      break;
#endif /* #if (defined FEATURE_CAT_REL6) && (defined FEATURE_CCAT) && (defined FEATURE_UIM_RUIM) */
#else  /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/
        break;
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/
    case GSTK_IMEISV_INFO:
      MSG_HIGH(" .... ESN", 0, 0, 0);
#ifdef FEATURE_CAT_REL6
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#else /* if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
      MSG_HIGH("Loc Info Type : GSTK_ESN_INFO - Feature GSM/WCDMA not supported",0,0,0);
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#else  /* #ifdef FEATURE_CAT_REL6 */
      MSG_HIGH("Loc Info Type : GSTK_ESN_INFO - FEATURE_CAT_REL6 not supported",0,0,0);
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_MEID_INFO:
      MSG_HIGH(" .... ESN", 0, 0, 0);
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
      gstk_status = gstk_get_nv(NV_MEID_I,(nv_item_type *)meid_buf);

      if ( gstk_status != GSTK_SUCCESS )
      {
        MSG_ERROR("NV_MEID_I ERROR",0,0,0);
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        break;
      }
      /* initialize to zero */
      memset(local_info.info_data.meid, 0, GSTK_MEID_LEN);
      memcpy(local_info.info_data.meid, meid_buf, GSTK_MEID_LEN);
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/
#else  /*#ifdef FEATURE_CCAT */
       MSG_HIGH("Loc Info Type : GSTK_MEID_INFO - Feature CCAT not supported",0,0,0);
       general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /*#ifdef FEATURE_CCAT */
#else  /* #ifdef FEATURE_CAT_REL6 */
       MSG_HIGH("Loc Info Type : GSTK_MEID_INFO - FEATURE_CAT_REL6 not supported",0,0,0);
       general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /* #ifdef FEATURE_CAT_REL6 */
       break;

    case GSTK_NMR_BCCH_INFO:
      MSG_HIGH(" PLI NMR BCCH", 0, 0, 0);
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_GSM
#error code not present
#else /* FEATURE_GSM */
      MSG_HIGH(" GSM undefined. NMR not supported", 0, 0, 0);
      general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
#endif /* FEATURE_GSM */
#endif /* FEATURE_GSTK_FOR_NON_LTK_BUILD */
      break; /* case GSTK_NMR_BCCH_INFO */
    case GSTK_NMR_UTRAN_INFO:
      MSG_HIGH(" PLI NMR UTRAN", 0, 0, 0);
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
      gstk_status = gstk_process_pli_utran_nmr_req(cmd_data_ptr,
                                                   cmd_data_len,
                                                   curr_offset,
                                                   &new_offset,
                                                   &more_tlv,
                                                   &cmd_details_ref_id,
                                                   &nmr_utran_qualifier,
                                                   &need_to_send_terminal_rsp,
                                                   &general_result,
                                                   &additional_info);
      if(gstk_status != GSTK_SUCCESS)
      {
        MSG_HIGH("gstk_process_pli_utran_nmr_req() did not return success", 0, 0, 0);
        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        /* Send Terminal Response to Card */
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        need_to_send_terminal_rsp = TRUE;
      }
#endif /* FEATURE_GSTK_FOR_NON_LTK_BUILD */
      break; /* case GSTK_NMR_UTRAN_INFO */
    case GSTK_DATE_TIME_ZONE_INFO:
      MSG_MED(" .... Data time", 0, 0, 0);
      /* initialize to zero */
      memset(&local_info.info_data.time, 0, sizeof(gstk_date_time_rsp_type));

#ifdef FEATURE_SEC_TIME
      sectime_get_local_time(&ms);
      if(ms == 0)
      {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        break;
      }
      secs = (dword)(ms/1000);
      clk_secs_to_julian(secs, &rtc_julian);
#elif (defined(FEATURE_PMIC_RTC) && !defined(FEATURE_GSTK_REMOTE_TIME))
      clk_rtc_get_time(&rtc_julian);
#elif (defined(FEATURE_PMIC_RTC) && defined(FEATURE_GSTK_REMOTE_TIME))
      time_remote_get_apps_user_time(&secs);
      clk_secs_to_julian(secs, &rtc_julian);
      MSG_HIGH("Remote User Time: %d seconds", secs, 0, 0);
#else
      MSG_HIGH("FEATURE_PMIC_RTC not supported",0,0,0);
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /* FEATURE_SEC_TIME */

#if (defined FEATURE_SEC_TIME) || (defined FEATURE_PMIC_RTC)
      local_info.info_data.time.day += ((uint8)rtc_julian.day/10);
      local_info.info_data.time.day += (((uint8)rtc_julian.day%10) << 4);

      local_info.info_data.time.hour += ((uint8)rtc_julian.hour/10);
      local_info.info_data.time.hour += (((uint8)rtc_julian.hour%10) << 4);

      local_info.info_data.time.minute += ((uint8)rtc_julian.minute/10);
      local_info.info_data.time.minute += (((uint8)rtc_julian.minute%10) << 4);

      local_info.info_data.time.month += ((uint8)rtc_julian.month/10);
      local_info.info_data.time.month += (((uint8)rtc_julian.month%10) << 4);

      local_info.info_data.time.second += ((uint8)rtc_julian.second/10);
      local_info.info_data.time.second += (((uint8)rtc_julian.second%10) << 4);

      local_info.info_data.time.year += (uint8)(((word)rtc_julian.year%100) / 10);
      local_info.info_data.time.year += (uint8)((((word)rtc_julian.year%100) % 10) << 4);
#endif /*(defined FEATURE_SEC_TIME) || (defined FEATURE_PMIC_RTC) */
      break;

    case GSTK_LANGUAGE_SETTING_INFO:
      request.hdr_cmd.command_id = GSTK_PROVIDE_LANG_INFO_REQ;
      request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;
      request.hdr_cmd.command_number =
          command_details_ref_table[cmd_details_ref_id].command_details.command_number;

      MSG_MED(" .... Lang", 0, 0, 0);
      /* initialize to zero */
      memset(local_info.info_data.lang_code, 0, 2);
      /* Send command to clients */
      gstk_status = gstk_util_send_message_to_clients(&request);

      if(gstk_status != GSTK_SUCCESS)
      {
        MSG_HIGH("Provide Local Lang not able to send to client", 0, 0, 0);
        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        /* Send Terminal Response to Card */
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
      else
      {
        /* Start Timer */
        gstk_util_start_client_response_timer(cmd_details_ref_id);
        /* reset gstk_status to SUCCESS since there has been no error*/
        /* This is needed since some of the TLVs are optional, which might
        ** cause the various parsing functions to return invalid_command */
        need_to_send_terminal_rsp = FALSE;
        gstk_status = GSTK_SUCCESS;
      }
      break;

    case GSTK_TIMING_ADVANCE_INFO:

      MSG_MED(" .... Timing Adv", 0, 0, 0);
      /* initialize to zero */
      memset(&local_info.info_data.time_adv, 0, sizeof(gstk_time_advance_rsp_type));

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_GSM
#error code not present
#else
      MSG_HIGH("PROVIDE LOCAL INFO unsupported GSM call", 0, 0, 0);
#endif /* FEATURE_GSM */
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/
      break;

    case GSTK_ACCESS_TECHNOLOGY_INFO:
      MSG_HIGH(" Current Access Technology", 0, 0, 0);

      if (gstk_next_curr_rat == GSTK_ACCESS_NONE)
        {
        general_result = GSTK_ACCESS_TECH_UNABLE_TO_PROCESS_CMD;
      }
      else
      {
        general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        local_info.info_data.gstk_access_tech = gstk_next_curr_rat;
      }
      need_to_send_terminal_rsp = TRUE;
      break;
    case GSTK_BATTERY_STATUS_INFO:
#ifdef FEATURE_CAT_REL6
      request.hdr_cmd.command_id = GSTK_PROVIDE_BATTERY_STATUS_REQ;
      request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;
      request.hdr_cmd.command_number =
          command_details_ref_table[cmd_details_ref_id].command_details.command_number;

      MSG_MED(" .... Provide Local Info - Battery Status", 0, 0, 0);
      /* initialize to zero */
      memset(&local_info.info_data.battery_status, 0, sizeof(uint8));
      /* Send command to clients */
      gstk_status = gstk_util_send_message_to_clients(&request);
      if(gstk_status != GSTK_SUCCESS)
      {
        MSG_HIGH("Provide Local Info - Battery Status not able to send to client", 0, 0, 0);
        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        /* Send Terminal Response to Card */
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
      else
      {
        /* Start Timer */
        gstk_util_start_client_response_timer(cmd_details_ref_id);
        /* reset gstk_status to SUCCESS since there has been no error*/
        /* This is needed since some of the TLVs are optional, which might
        ** cause the various parsing functions to return invalid_command */
        need_to_send_terminal_rsp = FALSE;
        gstk_status = GSTK_SUCCESS;
      }
#else  /* #ifdef FEATURE_CAT_REL6 */
      MSG_HIGH("GSTK_BATTERY_STATUS_INFO : FEATURE_CAT_REL6 not supported",0,0,0);
      general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_NETWORK_SEARCH_MODE_INFO:
#ifdef FEATURE_CAT_REL6
      MSG_HIGH(" Network Search Mode", 0, 0, 0);

      /* Register with CM to receive the CM_PH_EVENT_SYS_SEL_PREF*/
      cm_reg_return_status = cm_client_ph_reg(
        gstk_client_id_from_cm,               /* pointer to the client struct */
        gstk_cm_ph_event_cb,                  /* pointer to a cb function for ph events */
        CM_CLIENT_EVENT_REG,                  /* event registration */
        CM_PH_EVENT_INFO,                     /* from this call event */
        CM_PH_EVENT_INFO,                     /* to this call event */
        NULL );                               /* pointer to a cb function for errors */
      if (cm_reg_return_status != CM_CLIENT_OK)
      {
        MSG_ERROR("REGISTRATION UNSUCCESSFUL WITH CM", 0, 0, 0);
        /* Do we send a terminal response to the card for this */
      }

      MSG_HIGH(" REGISTRATION SUCCESSFUL WITH CM ",0,0,0);

      data_blk = cmd_details_ref_id;

      if(cm_ph_cmd_get_ph_info(
           gstk_cm_network_search_mode_event_cb, /* cmd callback */
           (void *)&data_blk,                                                  /* data block ptr */
           gstk_client_id_from_cm) == FALSE)
      {
        need_to_send_terminal_rsp = TRUE;
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      }
      else
      {
        need_to_send_terminal_rsp = FALSE;
      }

#else  /* #ifdef FEATURE_CAT_REL6 */
      MSG_HIGH("GSTK_NETWORK_SEARCH_MODE_INFO : FEATURE_CAT_REL6 not supported",0,0,0);
      general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;

#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    default:
      MSG_HIGH("PROVIDE LOCAL INFO TLV parsing", 0, 0, 0);
      local_info.info_type = GSTK_INVALID_LOC_INFO;
      /* Send Terminal Response to Card */
      /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
      general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
      break;
    }
  } /* GSTK_SUCCESS from preprocess_command */
  else { /* parsing error */

     ERR("PROVIDE LOCAL INFO TLV parsing err", 0, 0, 0);
     /* Send Terminal Response to Card */
     /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
     general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  } /* if no error from preprocess command */

  /* send the Terminal Response */
  if (need_to_send_terminal_rsp) {
    gstk_status = gstk_send_provide_local_info_terminal_rsp(
                           cmd_details_ref_id, /* command_details_ref */
                           command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                           /* command_num */
                           general_result, /* general_result */
                           &additional_info, /* additional result */
                           &local_info /* local info union pointer */ );
    if (gstk_status != GSTK_SUCCESS) {
      MSG_MED("Can't send term rsp", 0, 0, 0);
    }
  }

  /* nothing to release in memory location */
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }

  return gstk_status;

} /* gstk_provide_local_info_req */

/*===========================================================================
FUNCTION gstk_provide_local_info_cnf

DESCRIPTION
  This function packs the provide local info terminal response and send
  the command to UIM server.

PARAMETERS
  provide_local_info_cnf_ptr: [Input] Pointer to message that is required to
                                      be processed and sent to UIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_provide_local_info_cnf(
  gstk_provide_local_info_cnf_type* provide_local_info_cnf_ptr)
{

  gstk_status_enum_type                       gstk_status   = GSTK_SUCCESS;
  provide_local_info_terminal_response_type   STK_response  = {0};
  uim_cmd_type                                *uim_cmd_ptr  = NULL;
  uint8                                       header_len    = 0;
  int                                         offset        = 0;
  int                                         i             = 0;
  uint32                                      ref_id        = 0;

  MSG_HIGH("IN GSTK provide local info cnf ",0,0,0);

  if(provide_local_info_cnf_ptr == NULL)
  {
    MSG_ERROR("provide_local_info_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response_ptr */
  memset(&STK_response, 0x00, sizeof(provide_local_info_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      provide_local_info_cnf_ptr->cmd_details_ref_id,
      provide_local_info_cnf_ptr->message_header.command_id,
      provide_local_info_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      provide_local_info_cnf_ptr->command_result,
      provide_local_info_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
    return gstk_status;
  }


  /* header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  header_len = STK_response.header.command_details.length +
                          STK_response.header.device_id.device_tag_length +
                          STK_response.header.result.result_tag_length +
                          6;

  /* Local Info */
  switch(provide_local_info_cnf_ptr->local_info_rsp.info_type) {
    case GSTK_GSM_UMTS_LOCATION_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
      break;
    case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
      gstk_status = gstk_packer_cdma_location_info_tlv(
        &provide_local_info_cnf_ptr->local_info_rsp.info_data.cdma_loc_info,
        &STK_response.local_info_rsp.cdma_location_information);
      STK_response.num_bytes = header_len +
        STK_response.local_info_rsp.cdma_location_information.length
        + 2;
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_NO_SRVC_LOCATION_INFO:
      STK_response.num_bytes = header_len;
      break;
    case GSTK_IMEI_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
      break;
    case GSTK_ESN_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
      gstk_status = gstk_packer_esn_tlv(
        provide_local_info_cnf_ptr->local_info_rsp.info_data.esn,
        &STK_response.local_info_rsp.esn);
      STK_response.num_bytes = header_len + GSTK_ESN_LEN + 2;
#endif /*#ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_IMEISV_INFO:
#ifdef FEATURE_CAT_REL6
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_MEID_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
      gstk_status = gstk_packer_meid_tlv(
        provide_local_info_cnf_ptr->local_info_rsp.info_data.meid,
        &STK_response.local_info_rsp.meid);
      STK_response.num_bytes = header_len + GSTK_MEID_LEN + 2;
#endif /*#ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_NMR_BCCH_INFO:
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_GSM
#error code not present
#endif /*FEATURE_GSM */
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD) */
      break;
    case GSTK_NMR_UTRAN_INFO:
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
#endif /* FEATURE_GSTK_FOR_NON_LTK_BUILD */
      break;
    case GSTK_DATE_TIME_ZONE_INFO:
      gstk_status = gstk_packer_date_time_tlv(
          &provide_local_info_cnf_ptr->local_info_rsp.info_data.time,
          &STK_response.local_info_rsp.date_time_zone);
      STK_response.num_bytes = header_len + 7 + 2;
      break;
    case GSTK_LANGUAGE_SETTING_INFO:
      gstk_status = gstk_packer_lang_sel_tlv(
          provide_local_info_cnf_ptr->local_info_rsp.info_data.lang_code,
          &STK_response.local_info_rsp.language);
      STK_response.num_bytes = header_len + 2 + 2;
      break;
    case GSTK_TIMING_ADVANCE_INFO:
      gstk_status = gstk_packer_timing_advance_tlv(
          &provide_local_info_cnf_ptr->local_info_rsp.info_data.time_adv,
          &STK_response.local_info_rsp.timing_advance);
      STK_response.num_bytes = header_len + 2 + 2;
      break;
    case GSTK_ACCESS_TECHNOLOGY_INFO:
      MSG_HIGH("Packing access tech tlv to send down to the card...",0,0,0);
      gstk_status = gstk_packer_access_technology_tlv(
          (uint8*)&provide_local_info_cnf_ptr->local_info_rsp.info_data.gstk_access_tech,
          &STK_response.local_info_rsp.access_technology);
      STK_response.num_bytes = header_len + 2 + 1;
      break;
    case GSTK_BATTERY_STATUS_INFO:
#ifdef FEATURE_CAT_REL6
      MSG_HIGH("Packing battery status tlv to send down to the card...",0,0,0);
      gstk_status = gstk_packer_battery_status_tlv(
          (uint8*)&provide_local_info_cnf_ptr->local_info_rsp.info_data.battery_status,
          &STK_response.local_info_rsp.battery_status);
      STK_response.num_bytes = header_len + 2 + 1;
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    default:
      MSG_MED("PROVIDE LOCAL INFO TLV packing", 0, 0, 0);
      STK_response.num_bytes = header_len;
      break;
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F,  &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        ref_id = provide_local_info_cnf_ptr->cmd_details_ref_id;

        /* clear the timer for client response */
        (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(provide_local_info_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = 0;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                            &STK_response.header.command_details,
                            offset,
                            (STK_response.header.command_details.length + 2),
                            UIM_MAX_CHARS,
                            sizeof(gstk_command_details_tlv_type));
        offset = (int)(offset + STK_response.header.command_details.length + 2);
        /* copy device */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                       &STK_response.header.device_id,
                       offset,
                       (STK_response.header.device_id.device_tag_length + 2),
                       UIM_MAX_CHARS,
                       sizeof(gstk_device_identities_tag_tlv_type));
        offset = (int)(offset + STK_response.header.device_id.device_tag_length + 2);
        /* copy result */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                               &STK_response.header.result,
                               offset,
                               3, /* result tag +  length + general result */
                               UIM_MAX_CHARS,
                               sizeof(gstk_result_tag_tlv_type));
        offset = offset + 3;
        if(STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                     STK_response.header.result.additional_info,
                     offset,
                     (STK_response.header.result.result_tag_length - 1),
                     UIM_MAX_CHARS,
                     (provide_local_info_cnf_ptr->result_additional_info.length *
                      sizeof(byte)));
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        /* local info */
        if (STK_response.num_bytes > header_len) { /* has local info */
          switch(provide_local_info_cnf_ptr->local_info_rsp.info_type) {
            case GSTK_GSM_UMTS_LOCATION_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
              break;
            case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                      &STK_response.local_info_rsp.cdma_location_information,
                      header_len,
                      (STK_response.num_bytes - header_len),
                      UIM_MAX_CHARS,
                      sizeof(gstk_cdma_location_information_tlv_type));
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
              break;
            case GSTK_NO_SRVC_LOCATION_INFO:
              break;
            case GSTK_IMEI_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
              break;

          case GSTK_IMEISV_INFO:
#ifdef FEATURE_CAT_REL6
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /* #ifdef FEATURE_CAT_REL6 */
              break;
            case GSTK_ESN_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &STK_response.local_info_rsp.esn,
                                      header_len,
                                      (STK_response.num_bytes - header_len),
                                      UIM_MAX_CHARS,
                                      sizeof(gstk_esn_tlv_type));
#endif /*#ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
              break;
            case GSTK_MEID_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &STK_response.local_info_rsp.meid,
                                      header_len,
                                      (STK_response.num_bytes - header_len),
                                      UIM_MAX_CHARS,
                                      sizeof(gstk_meid_tlv_type));
#endif /*#ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
              break;
            case GSTK_NMR_BCCH_INFO:
#ifdef FEATURE_GSM
#error code not present
#else  /* FEATURE_GSM */
              MSG_HIGH("FEATURE GSM Not Defined",0,0,0);
#endif /* FEATURE_GSM */
              break;
            case GSTK_NMR_UTRAN_INFO:
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
              break;
            case GSTK_DATE_TIME_ZONE_INFO:
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 &STK_response.local_info_rsp.date_time_zone,
                                 header_len,
                                 (STK_response.num_bytes - header_len),
                                 UIM_MAX_CHARS,
                                 sizeof(gstk_date_time_tlv_type));
              break;
            case GSTK_LANGUAGE_SETTING_INFO:
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &STK_response.local_info_rsp.language,
                                      header_len,
                                      (STK_response.num_bytes - header_len),
                                      UIM_MAX_CHARS,
                                      sizeof(gstk_lang_tlv_type));
              break;
            case GSTK_TIMING_ADVANCE_INFO:
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 &STK_response.local_info_rsp.timing_advance,
                                 header_len,
                                 (STK_response.num_bytes - header_len),
                                 UIM_MAX_CHARS,
                                 sizeof(gstk_timing_advance_tlv_type));
              break;
            case GSTK_ACCESS_TECHNOLOGY_INFO:
              MSG_HIGH("Popultaing uim cmd ptr to send to the card...",0,0,0);
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                              &STK_response.local_info_rsp.access_technology,
                              header_len,
                              (STK_response.num_bytes - header_len),
                              UIM_MAX_CHARS,
                              sizeof(gstk_access_technology_tlv_type));
              break;
            case GSTK_BATTERY_STATUS_INFO:
#ifdef FEATURE_CAT_REL6
              MSG_HIGH("Popultaing uim cmd ptr to send to the card...",0,0,0);
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 &STK_response.local_info_rsp.battery_status,
                                 header_len,
                                 (STK_response.num_bytes - header_len),
                                 UIM_MAX_CHARS,
                                 sizeof(gstk_battery_status_tlv_type));
#endif /* #ifdef FEATURE_CAT_REL6 */
              break;
            default:
              break;
          }
        }

        MSG_HIGH("Provide Local Info: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
        for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) {
          MSG_HIGH("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
        }

        MSG_HIGH("Sending terminal response cmd to uim server..",0,0,0);
        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
      }
      else {
        MSG_MED("Never here", 0, 0, 0);
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    ERR ("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
  return gstk_status;

} /* gstk_provide_local_info_cnf*/

/*===========================================================================
FUNCTION gstk_map_card_local_info_to_gstk_local_info

DESCRIPTION
  This function maps the local info type request from the card to the
  gstk local info type

PARAMETERS
   local_info_type_req_from_card: [Input] info type request from the card
   *info_type_ptr               : [Input]Pointer togstk info type request
   *data_ptr                    : [Input] Pointer to data

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_map_card_local_info_to_gstk_local_info(
  gstk_local_info_enum_type       local_info_type_req_from_card,
  gstk_local_info_gen_enum_type  *info_type_ptr,
  const gstk_generic_data_type   *data_ptr
)
{
  if(info_type_ptr == NULL)
  {
    MSG_HIGH("local_info ptr NULL",0,0,0);
    return;
  }
  switch(local_info_type_req_from_card)
  {
  case GSTK_LOCATION_INFO:
    if(gstk_next_curr_rat == GSTK_ACCESS_NONE)
    {
      *info_type_ptr = GSTK_NO_SRVC_LOCATION_INFO;
    }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /* #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) */
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
    else if(gstk_next_curr_rat == GSTK_ACCESS_TECH_CDMA)
    {
      *info_type_ptr = GSTK_CDMA_LOCATION_INFO;
    }
#endif /* FEATURE_CCAT */
#endif /* FEATURE_CAT_REL6 */
    break;
  case GSTK_IMEI:
    *info_type_ptr = GSTK_IMEI_INFO;
    break;
  case GSTK_NMR:
    /* check if extra tlv is present */
    if(data_ptr)
    {
      if(data_ptr->data_len > 0 && data_ptr->data_buffer_ptr != NULL)
      {
        /* checking if extra tlv is utran tlv */
        if(gstk_valid_tag(data_ptr->data_buffer_ptr[0],GSTK_UTRAN_MEASUREMENT_TAG))
        {
          *info_type_ptr = GSTK_NMR_UTRAN_INFO;
          break;
        }
      }
    }
    *info_type_ptr = GSTK_NMR_BCCH_INFO;
    break;
  case GSTK_DATE_TIME_ZONE:
    *info_type_ptr = GSTK_DATE_TIME_ZONE_INFO;
    break;
  case GSTK_LANGUAGE_SETTING:
    *info_type_ptr = GSTK_LANGUAGE_SETTING_INFO;
    break;
  case GSTK_TIMING_ADVANCE:
    *info_type_ptr = GSTK_TIMING_ADVANCE_INFO;
    break;
  case GSTK_ACCESS_TECHNOLOGY:
    *info_type_ptr = GSTK_ACCESS_TECHNOLOGY_INFO;
    break;
  case GSTK_ESN:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
    *info_type_ptr = GSTK_ESN_INFO;
#else /* #ifdef FEATURE_CCAT */
    MSG_HIGH("GSTK_ESN : Feature CCAT not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CCAT */
#else  /* #ifdef FEATURE_CAT_REL6 */
    MSG_HIGH("GSTK_ESN : Feature CAT_REL6 not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;
  case GSTK_IMEISV:
#ifdef FEATURE_CAT_REL6
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#else /* #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
    MSG_HIGH("GSTK_IMEISV : Feature GSM/WCDMA not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#else  /* #ifdef FEATURE_CAT_REL6 */
    MSG_HIGH("GSTK_IMEISV : Feature CAT_REL6 not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;
  case GSTK_BATTERY_STATUS:
#ifdef FEATURE_CAT_REL6
    *info_type_ptr = GSTK_BATTERY_STATUS_INFO;
#else /* #ifdef FEATURE_CAT_REL6 */
    MSG_HIGH("GSTK_BATTERY_STATUS : Feature CCAT not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;
  case GSTK_MEID:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
    *info_type_ptr = GSTK_MEID_INFO;
#else /* #ifdef FEATURE_CCAT */
    MSG_HIGH("GSTK_MEID : Feature CCAT not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CCAT */
#else /* #ifdef FEATURE_CAT_REL6 */
    MSG_HIGH("GSTK_MEID : Feature CAT_REL6 not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;
    case GSTK_NETWORK_SEARCH_MODE:
#ifdef FEATURE_CAT_REL6
    *info_type_ptr = GSTK_NETWORK_SEARCH_MODE_INFO;
#else /* #ifdef FEATURE_CAT_REL6 */
    MSG_HIGH("GSTK_NETWORK_SEARCH_MODE : Feature CAT_REL6 not supported",0,0,0);
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;
  default:
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
    break;
  }
}/* gstk_map_card_local_info_to_gstk_local_info */

/*===========================================================================
FUNCTION gstk_provide_local_information_cnf

DESCRIPTION
  This function packs the provide local info terminal response and send
  the command to UIM server.

PARAMETERS
  provide_local_info_cnf_ptr: [Input] Pointer to message that is required to
                                      be processed and sent to UIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_provide_local_information_cnf(
  const gstk_provide_local_info_cnf_type* provide_local_info_cnf_ptr)
{
  gstk_status_enum_type                         gstk_status        = GSTK_SUCCESS;
  uim_cmd_type                                  *uim_cmd_ptr       = NULL;
  uint32                                        term_resp_len      = 0x00;
  int32                                         offset             = 0x00;
  int                                           i                  = 0x00;

  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_term_rsp_common_type                 mandatory_tlv_info;

#ifdef FEATURE_CAT_REL6
  gstk_nw_search_mode_enum_type                  nw_search_mode_data;
#endif /* FEATURE_CAT_REL6 */

  MSG_HIGH("IN gstk_provide_local_information_cnf ",0,0,0);

  memset(&mandatory_tlv_info, 0x00, sizeof(gstk_cmd_term_rsp_common_type));

  if(provide_local_info_cnf_ptr == NULL)
  {
    MSG_ERROR("provide_local_info_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  uim_cmd_ptr = gstk_get_uim_buf();

  if(uim_cmd_ptr == NULL)
  {
    ERR ("no UIM cmd buf", 0, 0, 0);
    return GSTK_MEMORY_ERROR;
  }

  gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F,  &uim_cmd_ptr);
  if (gstk_status != GSTK_SUCCESS)
  {
    /* Release the upper cmd layer memory and the uim_cmd_ptr */
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type*)provide_local_info_cnf_ptr);/*lint !e826 area too small */
    gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
    uim_cmd_ptr = NULL;
    return GSTK_ERROR;
  }

  /* set user_data */
  uim_cmd_ptr->hdr.user_data               = 0;
  /* populate the terminal_response info */

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */

  offset = 0;

  /* Populate
   ** Command details tlv
   ** Device identity tlv
   ** Result tlv
  */

  /* Command details info */
  mandatory_tlv_info.cmd_info_data.ref_id           = provide_local_info_cnf_ptr->cmd_details_ref_id;
  mandatory_tlv_info.cmd_info_data.command_rsp_type =
    int32touint8((int32)GSTK_PROVIDE_NW_SEARCH_MODE_CNF);
  mandatory_tlv_info.cmd_info_data.command_number   = provide_local_info_cnf_ptr->command_number;

  /* device ID */
  mandatory_tlv_info.dev_id_data.src = GSTK_ME_DEVICE;
  mandatory_tlv_info.dev_id_data.dest = GSTK_UICC_SIM_DEVICE;

  /* result tag */
  mandatory_tlv_info.result_info_data.command_result = provide_local_info_cnf_ptr->command_result;
  mandatory_tlv_info.result_info_data.result_additional_info =
      provide_local_info_cnf_ptr->result_additional_info;


  /* Populate the command details, device identity and the result tlvs */
  gstk_status = gstk_util_populate_term_rsp_common_tlv(
                  offset,
                  &term_resp_len,
                  mandatory_tlv_info,
                  uim_cmd_ptr->terminal_response.data);

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type*)provide_local_info_cnf_ptr);/*lint !e826 area too small */
    gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
    uim_cmd_ptr = NULL;
    return GSTK_ERROR;
  }

  offset = offset + uint32toint32(term_resp_len);

  /* populate the terminal_response info - total number of bytes*/
  uim_cmd_ptr->terminal_response.num_bytes = uint32touint8(term_resp_len);

  /* Local Info */
  switch(provide_local_info_cnf_ptr->local_info_rsp.info_type)
  {
    case GSTK_NETWORK_SEARCH_MODE_INFO:
#ifdef FEATURE_CAT_REL6
      MSG_HIGH("Packing Network Search Mode tlv to send down to the card...",0,0,0);
      nw_search_mode_data = provide_local_info_cnf_ptr->local_info_rsp.info_data.nw_search_mode;
      gstk_status = gstk_util_populate_tlv(
                  GSTK_NW_SEARCH_MODE_TAG,
                  offset,
                  (gstk_cmd_to_uim_type*)&nw_search_mode_data,
                  uim_cmd_ptr->terminal_response.data,
                  &offset); /*lint !e826 nw_search_mode_data suppress area too small */

      if (gstk_status != GSTK_SUCCESS)
      {
        gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)(void*)provide_local_info_cnf_ptr);
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        uim_cmd_ptr = NULL;
        return GSTK_ERROR;
      }

      uim_cmd_ptr->terminal_response.num_bytes = uim_cmd_ptr->terminal_response.num_bytes +
        int32touint8(offset) - uint32touint8(term_resp_len);
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    default:
      MSG_MED("PROVIDE LOCAL INFO TLV packing", 0, 0, 0);
      break;
  }


  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  MSG_HIGH("Provide Local Info: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
  for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++)
  {
    MSG_HIGH("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
  }

  /* clear the command detail table reference slot before since we are about to reply to
     the card */
  gstk_util_cmd_details_ref_table_free_slot(provide_local_info_cnf_ptr->cmd_details_ref_id);

  MSG_HIGH("Sending terminal response cmd to uim server..",0,0,0);
  gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
  return gstk_status;

} /* gstk_provide_local_info_cnf*/

