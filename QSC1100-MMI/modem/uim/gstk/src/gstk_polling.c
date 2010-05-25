/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   P O L L I N G


GENERAL DESCRIPTION
  This source file contains functions to process polling proactive command


FUNCTIONS
  gstk_polling_req
    This function parses the polling Proactive Command from the card
    and sends it to the client.

  gstk_polling_cnf
    This function packs the polling terminal response and send
    the command to UIM server.

  gstk_uim_polling_report
    This is the callback function for polling response from UIM.
    UIM will call this function when it finishes it's polling operation

  gstk_polling_cnf_cleanup
    This function cleans up any allocated pointer on the stack.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_polling.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/23/07   nk      Replaced memcpy with gstk_offset_memcpy
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
03/12/07   sk      Initialized poll duration
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Fixed potential data abort issue
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Centralize release memory condition in the request function
08/03/04   tml     Memory leak fix and changed to use pointer for addtiional
                   info in the terminal response
05/26/04   tml     Added More debug messages 
05/17/04   tml     Changed MAX_POLL_DURATION to UIM_MAX_POLL_DURATION
04/16/04   tml     linted
07/08/03   tml     Remove unnecessary message
05/16/03   tml     linted
04/17/03   tml     lint
03/06/03   tml     Updated enum names
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

#include "err.h"
#include "string.h"
#include "tmc.h"
#include "uim.h"




/* Fixed Bug of #define using only 2 Bytes instead of
** 4 Bytes
*/
#define GSTK_POLL_CMD_DETAILS_REF_ID_MASK 0x0000FFFF
#define GSTK_POLL_CMD_NUM_MASK            (unsigned) 0xFFFF0000
#define GSTK_2_BYTES                      16 /* Bits */


/*===========================================================================
FUNCTION gstk_polling_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  polling_cnf_ptr:  [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_response_ptr: [Input] Pointer to the STK intermediate message

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
static void gstk_polling_cnf_cleanup(
  gstk_poll_interval_cnf_type              *polling_cnf_ptr,
  polling_interval_terminal_response_type  *STK_response_ptr)
{
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK_response_ptr ERR:NULL",0,0,0);
    return;
  }
  if(polling_cnf_ptr == NULL)
  {
    MSG_ERROR("polling_cnf_ptr ERR:NULL",0,0,0);
    return;
  }

  /* free the polling pointer from STK_response and _cnf if needed */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)polling_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info= NULL;
  }
} /* gstk_polling_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_uim_polling_report

DESCRIPTION

  This is the callback function for polling response from UIM.
  UIM will call this function when it finishes it's polling operation

PARAMETERS
  report_ptr: [Input] Pointer to the UIM polling report

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
void gstk_uim_polling_report (uim_rpt_type *report_ptr)
{
  gstk_task_cmd_type   * task_cmd_ptr  = NULL;
  uint32                 length        = 0;

  MSG_HIGH("GSTK UIM polling report_ptr cb", 0, 0, 0);

  if(report_ptr == NULL)
  {
    MSG_ERROR("report_ptr ERR:NULL",0,0,0);
    return;
  }

  if (report_ptr->rpt_type != UIM_POLLING_INTERVAL_R) {
    // build error message to GSTK?????????????
    ERR("POLLING expected in CB", 0, 0, 0);
    return;
  }

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr != NULL ) { /* successfully obtained command buffer */
    /* build the terminal profile response */

    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.command_group
                                             = GSTK_TERMINAL_RSP;
    /* polling off */
    if(report_ptr->rpt.polling_interval.actual_uim_polling_interval
          == (dword)UIM_MAX_POLL_DURATION) {
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.command_id
                                               = (int)GSTK_POLLING_OFF_CNF;
    }
    else { /* polling interval */
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.command_id
                                               = (int)GSTK_POLL_INTERVAL_CNF;
    }
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.user_data
                                             = report_ptr->user_data;

    /* get the command details and command number */
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.cmd_details_ref_id =
        (uint8)(report_ptr->user_data & GSTK_POLL_CMD_DETAILS_REF_ID_MASK);
/*lint -e737 loss of sign in promotion from int to unsigned long */
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_number =
        (uint8)((report_ptr->user_data & GSTK_POLL_CMD_NUM_MASK) >> GSTK_2_BYTES);
/*lint +e737 */

    /* initialize additional_info to zero */
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 0;
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
    
    if(report_ptr->rpt_status == UIM_PASS) {
      /* polling off */
      if(report_ptr->rpt.polling_interval.actual_uim_polling_interval
            == (dword)UIM_MAX_POLL_DURATION) {
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.present = FALSE;
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.length = 0;
      }
      else { /* polling interval */

        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.present = TRUE;
        length = report_ptr->rpt.polling_interval.actual_uim_polling_interval
                                    / GSTK_MILISECONDS_IN_A_MINUTE;
        if (length >= 1)  {
          task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units
                                                   = GSTK_DURATION_MINUTES;
        }
        else { /* unit is less minute */
          length = report_ptr->rpt.polling_interval.actual_uim_polling_interval
                                   / GSTK_MILISECONDS_IN_A_SECOND;
          if (length >= 1) {
            task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units
                                                     = GSTK_DURATION_SECONDS;
          }
          else { /* unit is in ms */
            task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units
                                                     = GSTK_DURATION_TENTHS_OF_SECONDS;
            length = report_ptr->rpt.polling_interval.actual_uim_polling_interval;
          }
        }

        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.length = (uint8)length;
      } /*polling interval */

      if(task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.cmd_details_ref_id < GSTK_MAX_PROACTIVE_COMMAND) {
        if (command_details_ref_table[task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.cmd_details_ref_id].partial_comprehension) {
          task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_result = 
            GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION;
        }
        else {
          task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_result = 
            GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        }
      }
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 0;
    }
    else { /* polling fail */
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.present = FALSE;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.length = 0;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units = 0;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 1;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr =
        gstk_malloc(task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length);
      if (task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 0;
      }
      else {
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr[0] = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      }
    }

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else {
    // build error message to GSTK?????????????
    ERR("No more gstk buf", 0, 0, 0);
  }
}/*lint !e818 *report_ptr suppression for externalized function */
/* gstk_uim_polling_report */

/*===========================================================================
FUNCTION gstk_polling_req

DESCRIPTION
  This function parses the Poll Interval Proactive Command from the card
  and sends it to the client.
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
gstk_status_enum_type gstk_polling_req (gstk_proactive_cmd_type* command_ptr)
{
  uint8                              *cmd_data_ptr          = NULL;
  int32                               cmd_data_len          = 0;
  uint32                              cmd_details_ref_id    = 0;
  int32                               curr_offset           = 0;
  gstk_status_enum_type               gstk_status           = GSTK_SUCCESS;
  boolean                             error_already_raised  = FALSE;
  int32                               new_offset            = 0;
  gstk_additional_info_type           additional_info       = {0};
  gstk_terminal_rsp_extra_param_type  extra_param           = {0};
  boolean                             more_tlv              = FALSE;
  uim_cmd_type                        *uim_cmd_ptr          = NULL;
  gstk_duration_type                  poll_duration         = {0};
  uint32                              poll_ms               = 0;
  boolean                             is_polling_off        = FALSE;

  MSG_HIGH(" IN GSTK_polling_REQ ",0,0,0);

  if(command_ptr == NULL)
  {
    MSG_ERROR("command_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize extra_param */
  extra_param.present = TRUE;

  /* Check and populate command details
  ** into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into
  ** the cmd_data_ptr, and specifies the length
  ** of the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );
  if(gstk_status == GSTK_SUCCESS)
  {
    if(command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type == GSTK_POLLING_OFF_CNF) {
      /* this is for polling off command */
      is_polling_off = TRUE;
      poll_duration.present = FALSE;
    }
    else { /* this is polling interval command */
      if (cmd_data_ptr == NULL) {
        error_already_raised = TRUE;
      }
      else {
        /* parse the proactive command to get the duration information */
        /* duration */
        gstk_status = gstk_parser_duration_tlv( cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &poll_duration,
                                                &new_offset,
                                                &more_tlv,
                                                MANDATORY,
                                                &cmd_details_ref_id
                                                );
        if (gstk_status != GSTK_SUCCESS ) 
        {
          error_already_raised = TRUE;
        }
      }
    }
  } /* GSTK_SUCCESS from preprocess_command */
  else
  {
    error_already_raised = TRUE;
  }

  /* No error has been encountered */
  if(!error_already_raised)
  {
    /* further checking for polling interval */
    /* Build UIM Polling command */
    if(!is_polling_off) { /* polling interval */
      if(!poll_duration.present)
      {
        /* got here because the unit is unknown! */
        MSG_MED("No duration for poll", 0, 0, 0);
        gstk_status = GSTK_ERROR;

        /* Send Terminal Response to Card */
        /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
        extra_param.present = FALSE;
        extra_param.extra_param.poll_duration.present = FALSE;

        gstk_status = gstk_send_terminal_response(
          GSTK_CLIENT_ID,                     /* client_id */
          0,                                  /* user_data */
          cmd_details_ref_id,                 /* command_details_ref */
          command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                              /* command_num */
          GSTK_POLL_INTERVAL_CNF,             /* command response */
          GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,  /* general_result */
          &additional_info,                   /* additional result */
          &extra_param                        /* extra param */
          );
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
        }
        error_already_raised = TRUE;
      } /* polling duration not present */
    } /* not polling off */
  } /* no error yet */

  if(!error_already_raised) /* no error still */
  {
    /* get a buffer for the request */
    uim_cmd_ptr = gstk_get_uim_buf();
    if (uim_cmd_ptr != NULL )
    {
      gstk_status = gstk_util_populate_uim_buf_hdr_info(
                                        UIM_POLLING_INTERVAL_F,
                                        &uim_cmd_ptr);
      if (gstk_status == GSTK_SUCCESS)
      {
        /* set user_data */
        uim_cmd_ptr->hdr.user_data  =
          command_details_ref_table[cmd_details_ref_id].command_details.
          command_number;
        /* Shift to Upper 2 Bytes Location */
        uim_cmd_ptr->hdr.user_data  <<= GSTK_2_BYTES;

        /* Mask Out any unwanted Bits */
        uim_cmd_ptr->hdr.user_data  &= GSTK_POLL_CMD_NUM_MASK;

        /* Lower 2 Bytes are For CMD Details Reference ID */
        uim_cmd_ptr->hdr.user_data |= ( cmd_details_ref_id &
                                        GSTK_POLL_CMD_DETAILS_REF_ID_MASK );

        if(!is_polling_off) {
          /* populate the uim polling info */
          uim_cmd_ptr->polling.command_type = POLLING_INTERVAL;

          switch(poll_duration.units) {
          case GSTK_DURATION_MINUTES:
            poll_ms = poll_duration.length *
                      GSTK_MILISECONDS_IN_A_MINUTE;
            break;
          case GSTK_DURATION_SECONDS:
            poll_ms = poll_duration.length *
                    GSTK_MILISECONDS_IN_A_SECOND;
            break;
          case GSTK_DURATION_TENTHS_OF_SECONDS:
            poll_ms = poll_duration.length *
                    GSTK_MILISECONDS_IN_ONE_TENTH_SEC;
            break;
          default:
            MSG_MED("Bad Duration", 0, 0, 0);
            gstk_status = GSTK_ERROR;
            break;
          }
          uim_cmd_ptr->polling.polling_interval_in_ms = poll_ms;
        }
        else { /* polling off */
          uim_cmd_ptr->polling.command_type = POLLING_OFF;
          uim_cmd_ptr->polling.polling_interval_in_ms = 0;
        }
      }
      else {
          MSG_MED("Populate UIM Hdr Err", 0, 0, 0);
          gstk_status = GSTK_ERROR;
      }

      if (gstk_status != GSTK_SUCCESS) { /* we have already get a uim_buffer but encounter an error */
        if (uim_cmd_ptr != NULL) {
          /* uim_cmd_ptr can be equal to NULL because in the gstk_util_populate_uim_buf_hdr_info, if
          there is error encountered, the uim_cmd_ptr will be set to NULL */
          /* put the queue back to the free queue */
          (void)q_link(uim_cmd_ptr, &uim_cmd_ptr->hdr.cmd_hdr.link);
          q_put(uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr, &(uim_cmd_ptr->hdr.cmd_hdr.link));
        }

        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        ERR("Get polling TLV parsing", 0, 0, 0);

        /* Send Terminal Response to Card */
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        extra_param.present = FALSE;
        extra_param.extra_param.poll_duration.present = FALSE;
        extra_param.present = FALSE;

        gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                      0,              /* user_data */
                                      cmd_details_ref_id, /* command_details_ref */
                                      command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                      /* command_num */
                                      command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                                      /* command response: poll interval or polling off */
                                      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                                      &additional_info, /* additional result */
                                      &extra_param /* extra param */ );
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
        }

      }
      else { /* send to UIM */
        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
        /* Start Timer */
        gstk_util_start_client_response_timer(cmd_details_ref_id);
        /* reset gstk_status to SUCCESS since there has been no error*/
        /* This is needed since some of the TLVs are optional, which might
        ** cause the various parsing functions to return invalid_command */
        gstk_status = GSTK_SUCCESS;
      }
    } /* uim_cmd_ptr != NULL */
    else { /* can't get uim buffer */
      ERR ("no UIM cmd buf", 0, 0, 0);
      gstk_status = GSTK_MEMORY_ERROR;

      /* Send Terminal Response to Card */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      MSG_MED("Get polling TLV parsing", 0, 0, 0);

      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      extra_param.present = FALSE;
      extra_param.extra_param.poll_duration.present = FALSE;
      gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                      0,              /* user_data */
                                      cmd_details_ref_id, /* command_details_ref */
                                      command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                      /* command_num */
                                      command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                                      /* command response: poll interval or polling off */
                                      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                                      &additional_info, /* additional result */
                                      &extra_param /* extra param */ );
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
      }
    }
  } /* ! error raised from parsing */
  else { /* error from parsing */
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    extra_param.present = FALSE;
    extra_param.extra_param.poll_duration.present = FALSE;

    gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                0,              /* user_data */
                                cmd_details_ref_id, /* command_details_ref */
                                command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                /* command_num */
                                command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                                /* command response: poll interval or polling off */
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                                &additional_info, /* additional result */
                                &extra_param /* extra param */ );
    if(gstk_status != GSTK_SUCCESS) {
      MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
    }
  }

  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
    cmd_data_ptr = NULL;
  }
  return gstk_status;
} /* gstk_polling_req */

/*===========================================================================
FUNCTION gstk_polling_cnf

DESCRIPTION
  This function packs the poll interval terminal response and send
  the command to UIM server.

PARAMETERS
  polling_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_polling_cnf(
  gstk_poll_interval_cnf_type* polling_cnf_ptr)
{

  gstk_status_enum_type                     gstk_status    = GSTK_SUCCESS;
  polling_interval_terminal_response_type   STK_response   = {0};
  uim_cmd_type                              *uim_cmd_ptr   = NULL;
  uint8                                     header_bytes   = 0;
  int                                       offset         = 0;
  int                                       i              = 0;
  uint32                                    ref_id         = 0;

  MSG_HIGH("IN GSTK_polling_cnf ",0,0,0);

  if(polling_cnf_ptr == NULL)
  {
    MSG_ERROR("polling_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(polling_interval_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      polling_cnf_ptr->cmd_details_ref_id,
      polling_cnf_ptr->message_header.command_id,
      polling_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      polling_cnf_ptr->command_result,
      polling_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* duration */
  if (polling_cnf_ptr->duration.present) {
    gstk_status = gstk_packer_duration_tlv(
      &polling_cnf_ptr->duration,
      &STK_response.duration);
  }


  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
                      + duration data + 2
  */
  header_bytes = STK_response.header.command_details.length +
                 STK_response.header.device_id.device_tag_length +
                 STK_response.header.result.result_tag_length +
                 6;
  if (polling_cnf_ptr->duration.present) {
    STK_response.num_bytes = header_bytes +
                          STK_response.duration.length +
                          2;
  }
  else {
    STK_response.num_bytes = header_bytes;
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        ref_id = polling_cnf_ptr->cmd_details_ref_id;

        /* clear the timer for client response for client response */
        (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(polling_cnf_ptr->cmd_details_ref_id);

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
                              3,  /* result tag +  length + general result */
                              UIM_MAX_CHARS,
                              sizeof(gstk_result_tag_tlv_type));
        offset = offset + 3;
        if(STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                          STK_response.header.result.additional_info,
                          offset,
                          (STK_response.header.result.result_tag_length - 1),  
                          UIM_MAX_CHARS,
                          (polling_cnf_ptr->result_additional_info.length * 
                          sizeof(byte)));
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        /* copy duration */
        if(polling_cnf_ptr->duration.present) {
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                  &STK_response.duration,
                                  header_bytes,
                                  STK_response.duration.length + 2,  
                                  UIM_MAX_CHARS,
                                  sizeof(gstk_duration_tlv_type));
        }

        MSG_MED("Polling: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
        for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) {
          MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->terminal_response.data[i], uim_cmd_ptr->terminal_response.data[i]);
        }

        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
      }
      else {
        MSG_MED("Never here", 0, 0, 0);
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    MSG_ERROR ("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
  return gstk_status;

} /* gstk_polling_cnf*/

