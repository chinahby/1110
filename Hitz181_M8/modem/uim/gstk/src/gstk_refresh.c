/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   R E F R E S H


GENERAL DESCRIPTION
  This source file contains functions to process the refresh proactive command


FUNCTIONS
  gstk_gsdi_refresh_cb
    This is the callback function for GSDI to call while processing the REFRESH
    Request.

  gstk_send_gsdi_refresh_req
    This function will send the GSDI a REFRESH Request.

  gstk_refresh_req
    This function parses the REFRESH Proactive Command.
 
  gstk_refresh_cnf
    This function packs the Refresh terminal response and sends
    the command to UIM server.

  gstk_refresh_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_refresh.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/09   mib     FileList is mandatory for Init+FCN and 3G Session Reset
02/05/09   gg      Fixed error when AID TLV length is zero
02/04/09   mib     Ignore AID in refresh in case of SIM
02/04/09   mib     Fixed AID length passed to mmgsdi in case of refresh
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation issues
07/17/08   xz      1) replace mem_malloc() with gstk_malloc()
                   2) free refresh_files.data_ptr pointer in 
                      gstk_send_mmgsdi_refresh_req() to avoid memory leak
03/17/08   sk      Fixed klockwork errors
02/06/08   sk      Lint fixes
11/30/07   sk      Added option parameter support to NAA refresh
11/07/07   sk      Added support for NAA Refresh
06/12/07   sp      Send error if tag is not valid and comprehension flag is set
                   while processing refresh request
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
03/26/07   sun     Send response with additional files read during refresh if
                   gsdi indicates so.
01/26/07   tml     Fixed not parsing AID issue
12/13/06   tml     Handle Refresh TR for AID not active or invalid AID
10/23/06   sk      Fixed lint errors.
10/19/06   sk      Fixed lint errors.
08/29/06   sk      Fixed lint error
08/28/06   sun     Lint Fixes
08/23/06   sun     Added support for NAA Refresh
06/24/06   sk      Lint fixes
05/11/06   tml     Properly convert GSTK refresh value to GSDI enum
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Fixed error fatal due to requst buffer not being
                   used and set
08/13/04   tml     Centralize release memory condition in the request function
                   and fix memory leak
08/03/04   tml     memory leak fix
05/26/04   tml     Added More debug messages 
01/25/04   tml     Dual slot refresh support
07/07/03   tml     Refresh message fix
05/16/03   tml     linted
05/01/03   jar     Added handling for GSDI_REFRESH_IN_PROGRESS
03/06/03   tml     Update enum names
02/27/03 jar/tml   Map GSDI_PATHS_DECODE_ERROR to COMMAND_BEYOND_THE_ME_CAPABILITIES.
02/07/03   jar     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Task Definition Headers */
#include "tmc.h"

/* Memory Header Files */
#include "memheap.h"

/* GSTK Header Files   */
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

/* Output & String Header Files */
#include "err.h"
#include "string.h"

/* GSDI Header Files */
#include "gsdi_exp.h"

static gstk_status_enum_type gstk_send_mmgsdi_refresh_req(
  uint32                     refresh_mode,
  const gstk_file_list_type *file_list_ptr,
  gsdi_app_id_type           app_id,
  uint32                     cmd_details_ref_id);

/*===========================================================================
FUNCTION gstk_refresh_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  refresh_cnf: [Input] Pointer to message that is required to
                       be processed and sent to UIM
  STK_response: [Input] Pointer to the STK intermediate message

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
static void gstk_refresh_cnf_cleanup(
    gstk_refresh_cnf_type          *refresh_cnf,
    refresh_terminal_response_type *STK_response)
{
  if(refresh_cnf == NULL)
  {
    MSG_ERROR("refresh_cnf ERR:NULL",0,0,0);
    return;
  }
  if(STK_response == NULL)
  {
    MSG_ERROR("STK_response ERR:NULL",0,0,0);
    return;
  }
  /* free refresh tlv */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)refresh_cnf);/*lint !e826 area too small */
  if (STK_response->data.result.additional_info != NULL)
  {
    gstk_free(STK_response->data.result.additional_info);
    STK_response->data.result.additional_info = NULL;
  }
} /* gstk_refresh_cnf_cleanup */

/*lint +e715 app_id not used when FEATURE_NAA_REFRESH_NOT_SUPPORTED defined */
/*===========================================================================
FUNCTION gstk_refresh_req

DESCRIPTION
  This function parses the REFRESH Proactive Command.

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
  The functionality of this command centers around the following command
  qualifiers.

  00 -> SIM Initialization and Full File Change Notification
  01 -> File Change Notification
  02 -> SIM Initialization and File Change Notification
  03 -> SIM Initialization
  04 -> SIM Reset
  05 to FF -> reserved values

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_refresh_req (gstk_proactive_cmd_type* command)
{
  uint8                              *cmd_data = NULL;
  int32                               cmd_data_len;
  uint32                              cmd_details_ref_id;
  int32                               curr_offset = 0;
  int32                               next_tlv_offset = 0;
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_additional_info_type           additional_info;
  gstk_terminal_rsp_extra_param_type  extra_param;
  boolean                             more_tlv = FALSE;
  uint8                               refresh_mode = 0xFF;
  gstk_file_list_type                 file_list;
  gsdi_app_id_type                    app_id;
  boolean                             comprehension_flag = FALSE;

  memset(&file_list,0x00,sizeof(gstk_file_list_type));
  memset(&app_id, 0x00, sizeof(gsdi_app_id_type));

  if(command == NULL)
  {
    MSG_ERROR("command ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize additional_info's
  ** length to 0
  */
  additional_info.length = 0;

  /* initialize extra_param */
  extra_param.present = FALSE;

  /* Check and populate command details
  ** into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into
  ** the cmd_data, and specifies the length
  ** of the cmd_data in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command,
                                              &cmd_data,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  if ( gstk_status == GSTK_SUCCESS )
  {
    refresh_mode = command_details_ref_table[cmd_details_ref_id].command_details.qualifier;
  }
  if (cmd_data != NULL)
  {
    more_tlv = TRUE;
    if(gstk_valid_tag( cmd_data[curr_offset], GSTK_FILE_LIST_TAG))
    {
      /* Start Parsing the File List Information */
      gstk_status =  gstk_parser_file_list_tlv( cmd_data,
                                                cmd_data_len,
                                                curr_offset,
                                                &file_list,
                                                &next_tlv_offset,
                                                &more_tlv,
                                                OPTIONAL,
                                                &cmd_details_ref_id);
    }
    else /* can't understand the tag */
    {
      gstk_status = GSTK_ERROR;
    }
    
    if(gstk_status != GSTK_SUCCESS)
    {
      /* check if comprehension flag is set*/
      comprehension_flag = gstk_is_tag_comprehension_req(cmd_data[curr_offset]);

      if(comprehension_flag)
      {  
        /* Send Terminal Response to Card indicating 
           COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
        MSG_ERROR("REFRESH TLV parsing error for GSTK_FILE_LIST_TAG", 0, 0, 0);

        gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                         0,              /* user_data */
                                         cmd_details_ref_id, /* command_details_ref */
                                         command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                         /* command_num */
                                         command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                                         /* command response */
                                         GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                                         &additional_info, /* additional result */
                                         &extra_param /* extra param */ );
        if(gstk_status != GSTK_SUCCESS)
        {
          MSG_ERROR("TR fail, %d", gstk_status, 0, 0);
        }

        return gstk_status;
      }/* if(comprehension_flag)*/
      else
      {
        /* File list TLV is required when refresh mode is FCN,
           Init + FCN or 3G Session Reset */
        if (refresh_mode == (int)GSDI_FILE_CHANGE_NOTIFICATION ||
            refresh_mode == (int)GSDI_SIM_INIT_AND_FILE_CHANGE ||
            refresh_mode == (int)GSDI_3G_SESSION_RESET)
        {
          gstk_status = GSTK_INVALID_COMMAND;
        }
        else
        {
          /* comprehension not required. Set gstk_status to GSTK_SUCCESS to
             carry on */
          MSG_HIGH("File list tag absent or wrong. Refreh mode does not require it so carry on with refresh", 0, 0, 0);
          gstk_status = GSTK_SUCCESS;
        }
      }
    }
  }
  else
  {
    /* File list TLV is required when refresh mode is FCN,
       Init + FCN or 3G Session Reset */
    if (refresh_mode == (int)GSDI_FILE_CHANGE_NOTIFICATION ||
        refresh_mode == (int)GSDI_SIM_INIT_AND_FILE_CHANGE ||
        refresh_mode == (int)GSDI_3G_SESSION_RESET)
    {
      gstk_status = GSTK_INVALID_COMMAND;
    }
  }

#ifndef FEATURE_NAA_REFRESH_NOT_SUPPORTED
  if ( gstk_status == GSTK_SUCCESS )
  {
    /*Find out if APP ID exists in the tag list*/
    if(more_tlv)
    {
      curr_offset = next_tlv_offset;
      if(gstk_valid_tag( cmd_data[curr_offset], GSTK_AID_TAG))
      {
        if(gstk_curr_app_type != GSTK_APP_USIM)
        {
          /* Error - no aid should be provided for 2G platform */
          MSG_HIGH("Not USIM app.Current app type is 0x%x",gstk_curr_app_type,0,0);
        }
 
        /* Start Parsing the AID Information */
        gstk_status =  gstk_parser_aid_tlv( cmd_data,
                                            cmd_data_len,
                                            curr_offset,
                                            &app_id,
                                            &next_tlv_offset,
                                            &more_tlv,
                                            OPTIONAL,
                                            &cmd_details_ref_id);
      }
      else /* can't understand the tag */
      {
        gstk_status = GSTK_ERROR;
      }

      if(gstk_status != GSTK_SUCCESS || gstk_curr_app_type != GSTK_APP_USIM)
      {
        /* reset AID value */
        memset(&app_id, 0x00, sizeof(gsdi_app_id_type));

        /* check if comprehension flag is set*/
        comprehension_flag = gstk_is_tag_comprehension_req(cmd_data[curr_offset]);

        if(comprehension_flag)
        {  
          /* Send Terminal Response to Card */
          /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
          MSG_ERROR("REFRESH TLV parsing errors for GSTK_AID_TAG", 0, 0, 0);
       
          gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                          0,                        /* user_data */
                                          cmd_details_ref_id,       /* command_details_ref */
                                          command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                          /* command_num */
                                          command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                                          /* command response */
                                          GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                                          &additional_info, /* additional result */
                                          &extra_param /* extra param */ );
          if(gstk_status != GSTK_SUCCESS)
          {
            MSG_ERROR("TR fail, %d", gstk_status, 0, 0);
          }
          return gstk_status;
        } /* if(comprehension_flag) */
        else
        {
          /* comprehension not required. Set gstk_status to GSTK_SUCCESS to
             carry on */
          MSG_HIGH("AID tag absent or wrong. Carry on with refresh", 0, 0, 0);
          gstk_status = GSTK_SUCCESS;
        }
      }
    }
  }
#endif /*ifndef FEATURE_NAA_REFRESH_NOT_SUPPORTED*/

  if(gstk_status == GSTK_SUCCESS)
  {
    if ((refresh_mode == (int)GSDI_FILE_CHANGE_NOTIFICATION) &&
        (file_list.num_of_files == 0) )
    {
      MSG_HIGH("No files to be updated", 0, 0, 0);
      gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                              0,              /* user_data */
                              cmd_details_ref_id, /* command_details_ref */
                              command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                              /* command_num */
                              command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                              /* command response */
                              GSTK_COMMAND_PERFORMED_SUCCESSFULLY, /* general_result */
                              &additional_info, /* additional result */
                              &extra_param /* extra param */ );

      if(gstk_status != GSTK_SUCCESS)
      {
        MSG_ERROR("TR fail, %d", gstk_status, 0, 0);
      }
    }
    else
    {
      /* Send Request to GSDI */
      gstk_status = gstk_send_mmgsdi_refresh_req(refresh_mode,
                                                 &file_list,
                                                 app_id,
                                                 cmd_details_ref_id);

      if ( gstk_status != GSTK_SUCCESS )
      {
        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        MSG_ERROR("Got REFRESH TLV parsing errors", 0, 0, 0);
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

        gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                     0,              /* user_data */
                                     cmd_details_ref_id, /* command_details_ref */
                                     command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                     /* command_num */
                                     command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                                     /* command response */
                                     GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                                     &additional_info, /* additional result */
                                     &extra_param /* extra param */ );
        if(gstk_status != GSTK_SUCCESS)
        {
          MSG_ERROR("TR fail, %d", gstk_status, 0, 0);
        }          
      }
    }
  }
  else
  {
    /* Parsing Error */
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    MSG_ERROR("Got REFRESH TLV parsing errors", 0, 0, 0);

    gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                 0,              /* user_data */
                                 cmd_details_ref_id, /* command_details_ref */
                                 command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                                 /* command_num */
                                 command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                                 /* command response */
                                 GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                                 &additional_info, /* additional result */
                                 &extra_param /* extra param */ );

    if(gstk_status != GSTK_SUCCESS)
    {
      MSG_ERROR("TR fail, %d", gstk_status, 0, 0);
    }
  }
  /* release memory allocation for refresh cmd      */
  if (cmd_data != NULL)
  {
    gstk_free(cmd_data);
    cmd_data = NULL;
  }
  if (file_list.file_paths != NULL)
  {
    gstk_free(file_list.file_paths);
    file_list.file_paths = NULL;
  }
  return gstk_status;
}


/*===========================================================================
FUNCTION gstk_refresh_cnf

DESCRIPTION
  This function packs the Refresh terminal response and sends
  the command to UIM server.

PARAMETERS
  refresh_cnf: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_refresh_cnf (
  gstk_refresh_cnf_type* refresh_cnf
)
{
  gstk_status_enum_type                     gstk_status = GSTK_SUCCESS;
  refresh_terminal_response_type            STK_response;
  uim_cmd_type                             *uim_cmd_ptr;
  int                                       offset = 0;
  int                                       i = 0;

  MSG_HIGH("IN GSTK REFRESH CNF ",0,0,0);

  if(refresh_cnf == NULL)
  {
    MSG_ERROR("refresh_cnf ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0, sizeof(refresh_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
     refresh_cnf->cmd_details_ref_id,
     refresh_cnf->message_header.command_id,
     refresh_cnf->command_number,
     &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
     refresh_cnf->command_result,
     refresh_cnf->result_additional_info,
     &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
    return gstk_status;
  }

  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  STK_response.num_bytes = STK_response.data.command_details.length +
                          STK_response.data.device_id.device_tag_length +
                          STK_response.data.result.result_tag_length +
                          6;

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL )
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
    {
      /*
      ** Clear the command detail table
      ** reference slot, we are about to
      ** reply to the card
      */
      gstk_util_cmd_details_ref_table_free_slot(refresh_cnf->cmd_details_ref_id);

      /* Set user_data */
      uim_cmd_ptr->hdr.user_data               = 0;
      /* populate the terminal_response info */
      uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

      /* Copy command details */
      memcpy(uim_cmd_ptr->terminal_response.data,
            &STK_response.data.command_details,
            (int)(STK_response.data.command_details.length + 2));

      offset = (int)(offset + STK_response.data.command_details.length + 2);

      /* Copy device identities*/
      memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
            &STK_response.data.device_id,
            (int)(STK_response.data.device_id.device_tag_length + 2));

      offset = (int)(offset + STK_response.data.device_id.device_tag_length + 2);

      /* Copy result */
      memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
            &STK_response.data.result,
            3); /* result tag +  length + general result */
      offset = offset + 3;
      memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
            STK_response.data.result.additional_info,
            (int)(STK_response.data.result.result_tag_length - 1));
      offset = (int)(offset + STK_response.data.result.result_tag_length - 1);

      MSG_MED("Refresh: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
      for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) {
        MSG_MED("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
      }

      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    }
    else
    {
      MSG_MED("Never here", 0, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
  return gstk_status;
} /* gstk_refresh_cnf */

/*===========================================================================
FUNCTION gstk_handle_mmgsdi_refresh_cnf

DESCRIPTION
  This is the function that is called from gstk callback invoked by  MMGSDI to
  process refresh cnf.

PARAMETERS
  mmgsdi_return_enum_type  mmgsdi_status - MMGSDI status
  mmgsdi_refresh_cnf_type *refresh_cnf   - Refresh conf

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
gstk_status_enum_type gstk_handle_mmgsdi_refresh_cnf (
  mmgsdi_return_enum_type        mmgsdi_status,
  const mmgsdi_refresh_cnf_type *refresh_cnf_ptr
)
{
  gstk_additional_info_type           additional_info      = {0}; 
  gstk_terminal_rsp_extra_param_type  extra_param          = {0};
  uint32                              cmd_details_ref_id   = 0;
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  gstk_general_result_enum_type       me_response          = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

  MSG_HIGH("IN GSTK / MMGSDI REFRESH CNF ",0,0,0);

  if(refresh_cnf_ptr == NULL)
  {
    MSG_ERROR("refresh_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_ERROR;
  }

  extra_param.present     = FALSE;
  additional_info.length  = 0;

  switch (mmgsdi_status)
  {
  case MMGSDI_SUCCESS:
    MSG_HIGH("Sending COMMAND_PERFORMED_SUCCESSFULLY",0,0,0);
    break;
  case MMGSDI_REFRESH_IN_PROGRESS:
    /* Do Nothing */
    break;
  case MMGSDI_NOT_SUPPORTED:
  case MMGSDI_PATHS_DECODE_ERROR:
    MSG_HIGH("MMGSDI - GSTK COMMAND_BEYOND_THE_ME_CAPABILITIES",0,0,0);
    me_response = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
    break;
  case MMGSDI_ERROR:
  case MMGSDI_CAN_NOT_REFRESH:
    MSG_HIGH("MMGSDI - GSTK ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND",0,0,0);
    me_response = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    break;
  case MMGSDI_REFRESH_SUCCESS:
    MSG_HIGH("MMGSDI - GSTK COMMAND_PERFORMED_SUCCESSFULLY",0,0,0);
    me_response = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    break;
  case MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE:
    MSG_HIGH("MMGSDI - GSTK REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE",0,0,0);
    me_response = GSTK_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE;
    break;
  case MMGSDI_REFRESH_FAIL_INVALID_AID:
    MSG_HIGH("MMGSDI - GSTK COMMAND_DATA_NOT_UNDERSTOOD_BY_ME",0,0,0);
    me_response = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    break;
  case MMGSDI_REFRESH_ADDITIONAL_FILES_READ:
    MSG_HIGH("MMGSDI - GSTK ADDITIONAL EFS READ DURING FCN",0,0,0);
    me_response = GSTK_PCMD_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ;
    break;
  default:
    MSG_ERROR("Unknown MMGSDI Status %x",mmgsdi_status,0,0);
    MSG_HIGH("MMGSDI - GSTK ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND",0,0,0);
    me_response = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    break;
  }

  /*
  ** Need to send back a Terminal Response
  ** indicating the status of the Refresh
  */
  cmd_details_ref_id = refresh_cnf_ptr->response_header.client_data;
  if ( me_response == GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND )
  {
    gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
  }
  gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                  0,                  /* user_data */
                  cmd_details_ref_id, /* command_details_ref */
                  command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                  /* command_num */
                  command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type,
                  /* command response: poll interval or polling off */
                  me_response,       /* general_result */
                  &additional_info,  /* additional result */
                  &extra_param       /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) 
  {
    MSG_ERROR("Unable to send Terminal Response, %d", gstk_status, 0, 0);
    return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
}/* gstk_handle_mmgsdi_refresh_cnf */

/*===========================================================================
FUNCTION gstk_send_mmgsdi_refresh_req

DESCRIPTION
  This function will send a REFRESH Request to MMGSDI

PARAMETERS
  refresh_mode        [Input] : Refresh mode
  file_list_ptr       [Input] : File List
  app_id              [Input] : App id type
  cmd_details_ref_id  [Input] : GSTK reference data

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:  GSTK_SUCCESS:  Indicates that Heap Space was properly allocated
                                         for the request.  Also indicates the request was
                                         properly queued onto MMGSDI.
                          GSTK_ERROR:    Indicates that either Heap Space was not
                                         available or that the Message was not properly
                                         queued onto GSDI.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_send_mmgsdi_refresh_req(
  uint32                     refresh_mode,
  const gstk_file_list_type *file_list_ptr,
  gsdi_app_id_type           app_id,
  uint32                     cmd_details_ref_id)
{
  mmgsdi_return_enum_type       mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_refresh_mode_enum_type mmgsdi_refresh_mode = MMGSDI_REFRESH_MODE_ENUM_MAX ;
  mmgsdi_data_type              refresh_files       = {0};
  mmgsdi_aid_type               aid;
  mmgsdi_option_type            option;

  if ( file_list_ptr == NULL)
  {
    MSG_ERROR("File List is NULL",0,0,0);
    return GSTK_ERROR;
  }

  /* Initialize aid */
  aid.app_type = MMGSDI_APP_NONE;
  memset(&aid.aid,0x00,sizeof(mmgsdi_static_data_type));
  memset(&aid.label,0x00,sizeof(mmgsdi_static_data_type));

  /* Initialize option */
  memset(&option,0x00,sizeof(mmgsdi_option_type));

  if(file_list_ptr->num_of_files > 0)
  {
    refresh_files.data_len = file_list_ptr->length;
    if((file_list_ptr->length == 0) || (file_list_ptr->file_paths == NULL))
    {
      MSG_ERROR("File List is empty",0,0,0);
      return GSTK_ERROR;
    }
    refresh_files.data_ptr = (uint8*)gstk_malloc(sizeof(uint8) * file_list_ptr->length);
    if(refresh_files.data_ptr == NULL)
    {
      MSG_HIGH("refresh_files.data_ptr is NULL",0,0,0);
      return GSTK_MEMORY_ERROR;
    }
    gstk_memcpy((void*)refresh_files.data_ptr, 
                (void*)file_list_ptr->file_paths, 
                sizeof(uint8) * file_list_ptr->length,
                sizeof(uint8) * (int32touint32(refresh_files.data_len)), 
                sizeof(uint8) * file_list_ptr->length);
  }

  gstk_memcpy(aid.aid.data_ptr,app_id.aid_data,app_id.aid_len,sizeof(aid.aid.data_ptr),sizeof(app_id.aid_data));
  aid.aid.data_len = (mmgsdi_len_type)app_id.aid_len;

  switch (refresh_mode)
  {
  case 0:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT_FULL_FCN;
    break;
  case 1:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_FCN;
    break;
  case 2:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT_FCN;
    break;
  case 3:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT;
    break;
  case 4:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_RESET;
    break;
  case 5:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_APP_RESET;
    break;
  case 6:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_3G_SESSION_RESET;
    break;
  default:
    MSG_ERROR("Unknown refresh mode 0x%x",refresh_mode,0,0);
    gstk_free(refresh_files.data_ptr);
    return GSTK_ERROR;
  }  

  mmgsdi_status = mmgsdi_naa_refresh(gstk_mmgsdi_client_id,
                                     MMGSDI_SLOT_1,
                                     aid,
                                     mmgsdi_refresh_mode,
                                     refresh_files,
                                     file_list_ptr->num_of_files,
                                     option,
                                     gstk_mmgsdi_cmd_cb,
                                     cmd_details_ref_id);

  gstk_free(refresh_files.data_ptr);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_ERROR("COULD NOT SEND REFRESH",0,0,0);
    return GSTK_ERROR;
  }

  MSG_MED("SENDING REFRESH REQ TO MMGSDI",0,0,0);
  return GSTK_SUCCESS;
} /* gstk_send_mmgsdi_refresh_req */

