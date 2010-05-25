/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   U S I M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the ISIM protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_isim_command
    Processes a isim UIM server command

  uim_process_isim_response
    Processes the responses for a isim UIM server command

  uim_send_isim_err_rpt
    Processes an error condition for a isim UIM server command

  uim_isim_command
    This function contains the state machine that processes APDUs
    for isim commands.

  uim_isim_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2001, 2005-2006, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uimisim.c_v   1.1   04 Dec 2001 15:10:38   tsummers  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimisim.c#2 $ $DateTime: 2009/05/04 18:36:44 $ $Author: shanmugh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/09   ps      Fix 8200 Warnings
04/03/06   pv      Lint Fixes
05/16/05   pv      Created module.  This module contains the ISIM state
                   machine.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_ISIM)

#include "msg.h"
#include "err.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimisim.h"
#include <memory.h>

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* State of the UIM */
LOCAL uim_isim_state_type *uim_isim_state;

/* Array to hold the different command states */
LOCAL uim_isim_state_type uim_isim_states[UIM_MAX_NUMBER_ISIM_STATES];

/* Pointer to the Global UIM Request buffer */
LOCAL uim_req_buf_type *uim_isim_req_buf_ptr = &uim_req_buf;

/*===========================================================================
  I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
LOCAL void uim_process_isim_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type *cmd_ptr
);

LOCAL void uim_isim_command
(
  uim_cmd_type *cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_ISIM_ERR_RPT

DESCRIPTION
  This function sends an error report for a isim command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_isim_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  q_type *save_done_q_ptr;
  q_link_type *save_link_ptr;

  /* Copy user data from received command to report buffer */
  uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;

  save_done_q_ptr = cmd_ptr->hdr.cmd_hdr.done_q_ptr;
  save_link_ptr   = &cmd_ptr->hdr.cmd_hdr.link;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_rpt_buf.rpt_status = UIM_FAIL;

  switch(cmd_ptr->hdr.command)
  {
     case UIM_ISIM_AUTHENTICATE_F:

        if (cmd_ptr->hdr.rpt_function != NULL)
        {
            uim_rpt_buf.rpt_type = UIM_ISIM_AUTHENTICATE_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting error for ISIM Authenticate cmd ", 0, 0, 0 );
        }

     break;

     default:
        ERR( " Unknown command %d in uim_send_isim_err_rpt",
                                               cmd_ptr->hdr.command, 0, 0 );
     break;

  }

  /* return buffer to free queue */
  if (save_done_q_ptr != NULL)
  {
    /* place command on requested queue */
    q_put( save_done_q_ptr, save_link_ptr );
  }
} /* uim_send_isim_err_rpt */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_ISIM_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a isim command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/

LOCAL void uim_process_isim_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type *cmd_ptr
)
{
  /* save the following info to place the buffer back on the requested queue */
  q_type *save_done_q_ptr;
  q_link_type *save_link_ptr;

  save_done_q_ptr = cmd_ptr->hdr.cmd_hdr.done_q_ptr;
  save_link_ptr   = &cmd_ptr->hdr.cmd_hdr.link;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Set the status to UIM_PASS unless something failed */
  uim_rpt_buf.rpt_status = UIM_PASS;

  switch (cmd_ptr->hdr.command)
  {
    case UIM_ISIM_AUTHENTICATE_F:           /* Authenticate for ISIM */
      {
        /* Report back that Access was completed, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_ISIM_AUTHENTICATE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the response that we got
            from UIM into the command */
          uim_rpt_buf.rpt.isim_autn.data_length = rsp_ptr->cmd_rsp_size;

          /* copy the data into the buffer for response */
          memcpy(uim_rpt_buf.rpt.isim_autn.data, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting Authenticate for ISIM ", 0, 0, 0 );

        } /* (cmd_ptr->hdr.rpt_function != NULL) */
      } /* end case - ISIM_AUTHENTICATE_F */
      break;
      default:
         ERR( " Unknown command %d in uim_process_isim_response",
                                                 cmd_ptr->hdr.command, 0, 0 );
      break;
  }

  /* return buffer to free queue */
  if (save_done_q_ptr != NULL)
  {
     /* place command on requested queue */
     q_put( save_done_q_ptr, save_link_ptr );
  } /* end if - save_done_q_ptr != NULL */

} /* uim_process_isim_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_ISIM_COMMAND

DESCRIPTION
  This procedure is the main state machine for ISIM UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

LOCAL void uim_isim_command
(
  uim_cmd_type *cmd_ptr
)
{
  /* Set the protocol from the command to the UIM request buffer */
  uim_isim_req_buf_ptr->protocol = cmd_ptr->hdr.protocol;

  switch(*uim_isim_state)
  {
    case UIM_ISIM_AUTHENTICATE_ST:              /* Authenticate for ISIM */
      {
        uim_isim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        uim_isim_req_buf_ptr->apdu_hdr.instrn = ISIM_AUTHENTICATE;
        uim_isim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

        /* Indicates that no info on the algorithm is given. The algo is
           implicitly known in the context of the selected application */
        uim_isim_req_buf_ptr->apdu_hdr.p1 = 0;

        /* b8 of p2 should always be 1 for ISIM authenticate*/
        uim_isim_req_buf_ptr->apdu_hdr.p2 = UIM_ISIM_AUTN_SPECIFIC_REF_DATA_P2;
        
        /* Though b1 could be 0, 0 is currently reserved */
        uim_isim_req_buf_ptr->apdu_hdr.p2 |= ((byte) cmd_ptr->isim_autn.auth_context &
                                              UIM_ISIM_AUTN_REF_DATA_NUMBER_MASK);

        uim_isim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->isim_autn.num_bytes;
                      /* bytes of data sent in the command */

        uim_isim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_isim_req_buf_ptr->rpt_function = uim_command_response_callback;

        /* Copy the data from the command for the specificed number of bytes */
        memcpy((byte*) uim_isim_req_buf_ptr->req.data, cmd_ptr->isim_autn.data,
                cmd_ptr->isim_autn.num_bytes);

        uim_isim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_isim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_isim_req_buf_ptr);
      }
    break;

    case UIM_ISIM_DONE_ST:                         /* Done State */
      {
        uim_process_isim_response(uim_rsp_buf_ptr, cmd_ptr);
      } /* end case - UIM_DONE_ST */
      break;

    default:

      MSG_HIGH("Invalid state in uim_isim_command %ld",*uim_isim_state, 0,0);
      break;

  } /* end of main switch */

  MSG_LOW("UIM state in uim_isim_command %ld",uim_isim_state, 0,0);

} /* uim_isim_command */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_ISIM_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM state variable amongst others.

===========================================================================*/

boolean uim_process_isim_command
(
  uim_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  /* Copy user data from received command to report buffer */
  uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;

  switch (cmd_ptr->hdr.command)
  {
    case UIM_ISIM_AUTHENTICATE_F:            /* Authenticate for ISIM */
      {
        MSG_MED( "Received ISIM Authenticate command ", 0, 0, 0 );

        /* Build the ISIM Authenticate list of states. */
        uim_isim_states[0] = UIM_ISIM_AUTHENTICATE_ST;
        uim_isim_states[1] = UIM_ISIM_DONE_ST;
        uim_isim_state = uim_isim_states;

      } /* end case - UIM_ISIM_AUTHENTICATE_F */
    break;

    default:

      status = FALSE;
      ERR(" Unrecognized cmd recd in ISIM Module %d", cmd_ptr->hdr.command,0,0);

    break;
  }

  if (status)
  {
    /* Call the state machine. */
    uim_isim_command (cmd_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_rpt_buf.sw1 = (uim_sw1_type) 0;
    uim_rpt_buf.sw2 = (uim_sw2_type) 0;

    uim_send_isim_err_rpt(cmd_ptr);
  }

  return(status);

} /* uim_process_isim_command() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_ISIM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a isim comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_isim_state.

===========================================================================*/

uim_cmd_status_type uim_isim_command_response
(
 uim_rsp_buf_type const *rsp_ptr,
 uim_cmd_type     *cmd
)
{
  uim_cmd_status_type status;
  /* check to see what the status from the UIM is */

  uim_isim_state_type curr_uim_isim_state;
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  status = UIM_CMD_SUCCESS;

  /* Fill the status words into the report buffer. This could be used for
     both passed and failed reports. If the command is not sent to the
     card, then the status words are cleared before sending the report */

  uim_rpt_buf.sw1 = (uim_sw1_type)rsp_ptr->sw1;
  uim_rpt_buf.sw2 = (uim_sw2_type)rsp_ptr->sw2;

#ifdef FEATURE_UIM_TOOLKIT
  /* Change fetch responses to normal end responses */
  if (SW1_END_FETCH == uim_rpt_buf.sw1)
  {
    uim_rpt_buf.sw1 = SW1_NORMAL_END;
    uim_rpt_buf.sw2 = SW2_NORMAL_END;
    /* If the command is completed, indicate there is a proactive command
       pending. */
    completed_status = UIM_CMD_FETCH;
  } /* end if - fetch response */
#endif /* FEATURE_UIM_TOOLKIT */

  /* Necessary so that we do not change the uim state and switch into that
     case also */
  curr_uim_isim_state = *uim_isim_state;

  switch (curr_uim_isim_state)
  {
    case UIM_ISIM_AUTHENTICATE_ST:        /* ISIM Authenticate state */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
           status = UIM_CMD_ERR;
        }
        else
        {
          /* Skip to the next state. */
           ++uim_isim_state;
        }
      } /* end case - UIM_ISIM_AUTHENTICATE_ST */
      break;

    default:
        MSG_HIGH( "Invalid State in uim_isim_command_response ", 0, 0, 0 );
        status = UIM_CMD_ERR;
      break;
  }
  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of the command */
    uim_isim_command(cmd);
  }

  if (UIM_ISIM_DONE_ST == *uim_isim_state)
  {
    /* Indicate that the command has been completed */
    return (completed_status);
  }

  return(status);

} /* uim_isim_command_response */

#endif /* FEATURE_UIM && FEATURE_UIM_ISIM */

