/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   U T I L   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the UIM UTILS of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_util_command
    Processes a generic UIM server command

  uim_send_util_err_rpt
    Processes an error condition for a util UIM server command

  uim_util_command_response
    This function processes the UIM responses from cdma APDUs.

INITIALIZATION REQUIREMENTS

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/09   ps      Fix 8200 Warnings
12/17/07   rm      Fix to correct the instruction case in some situations
12/11/06   tml     Clear the uim_last_sel_dir for the specific channel and slot
                   upon close channel request
04/03/06   pv      Lint Fixes
03/08/06   pv      Load the status words to the report buffer even if we
                   errored out because of un-known status words
11/14/05   pv      Lint Fixes
08/08/04   tml     Added user data
07/28/05   pv      replaced UIM_MAX_NUM_CDD_ATTEMPTS with UIM_MAX_NUM_ATTEMPTS
                   where not necessary.
07/12/05   tml     Assigned user data in util error
06/08/05   wli	   Replaced UIM_MAX_NUM_ATTEMPTS with UIM_MAX_NUM_CMD_ATTEMPTS 
				   for command retry.    
03/07/05   jk      Set the cmd count to max to avoid sending of failed PIN
                   related APDUs (so that PINs don't get blocked)
05/17/04   ck      Removed the use of logical channel table and instead use the
                   channel information from the command.
01/12/04   ck      Added a header file outside the feature to avoid warning.
12/29/03   ck      Merged in the inteface changes in the data direction
                   parameter of the command to the driver.
02/10/03   jk      Created module.  This module contains the UTIL state
                   machine.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_UTIL)
#include "bit.h"
#include "msg.h"
#include "err.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimutil.h"
#include <memory.h>
#include "uimgen.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* State of the UIM */
LOCAL uim_util_state_type *uim_util_state_ptr;

/* Variable to hold logical channel number for the currently processing
    command */
LOCAL byte uim_util_logical_channel;

/* Pointer to the Global UIM Request buffer */
LOCAL uim_req_buf_type *uim_util_req_buf_ptr = &uim_req_buf;

/* State configuration for the commands. */
LOCAL uim_util_state_type uim_util_states[UIM_MAX_NUMBER_UTIL_STATES];

/*===========================================================================
  I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
LOCAL void uim_process_util_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type *cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_UTIL_ERR_RPT

DESCRIPTION
  This function sends an error report for a generic command that is being
  requested.

DEPENDENCIES
  This function queues an error report onto a client queue.

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_util_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  q_type *save_done_q_ptr;
  q_link_type *save_link_ptr;
  
  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_send_util_err_rpt cmd_ptr is NULL",0,0,0);
  }

  save_done_q_ptr = cmd_ptr->hdr.cmd_hdr.done_q_ptr;
  save_link_ptr   = &cmd_ptr->hdr.cmd_hdr.link;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Copy user data from received command to report buffer */
  uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_rpt_buf.rpt_status = UIM_FAIL;

  /* Check if the client task has supplied a report back function. */
  if (cmd_ptr->hdr.rpt_function != NULL)
  {
    /* Process the error report based on the command that failed. */
    switch(cmd_ptr->hdr.command)
    {
      case UIM_VERIFY_IN_CHAN_F:   /* Verify data command */
        {

          /* Report back that verify data command was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_VERIFY_IN_CHAN_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting error for Verify Data ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_VERIFY_IN_CHAN_F */
        break;

      case UIM_ENABLE_VER_REQ_IN_CHAN_F:  /* Enable Verification Requirement */
        {
          /* Report back that enabling verification was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_ENABLE_VER_REQ_IN_CHAN_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting error for Enable Verification ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_ENABLE_VER_REQ_IN_CHAN_F */
        break;

      case UIM_DISABLE_VER_REQ_IN_CHAN_F: /* Disable Verification Requirement*/
        {
          /* Report back that disabling verification was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_DISABLE_VER_REQ_IN_CHAN_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting Error for Disable Verification", 0, 0, 0 );
          } /* The client gave a reportback function. */
        } /* end case - UIM_DISABLE_VER_REQ_IN_CHAN_F */
        break;

      case UIM_CHANGE_REF_DATA_IN_CHAN_F:      /* Change Reference Data */
        {
          /* Report back that change reference data was not changed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_CHANGE_REF_DATA_IN_CHAN_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting Error for Change reference data ", 0, 0, 0 );
          } /* end if - reportback function is provided by client. */
        } /* end case - UIM_CHANGE_REF_DATA_IN_CHAN_F */
        break;

      case UIM_RESET_RETRY_COUNTER_IN_CHAN_F: /* Reset Retry Counter */
        {
          /* Report back that reset retry counter was not reset */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_RESET_RETRY_COUNTER_IN_CHAN_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting error for Reset Retry Counter", 0, 0, 0 );
          }
        } /* end case - UIM_RESET_RETRY_COUNTER_IN_CHAN_F */
        break;
      case UIM_SELECT_IN_CHAN_F:          /* Select in Channel */
      {
        /* Report back that Select in Channel was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_SELECT_IN_CHAN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Select in Channel ", 0, 0, 0 );
        }/* The client provided a reportback function */
      } /* end case - UIM_SELECT_IN_CHAN_F */
      break;

    case UIM_ACCESS_IN_CHAN_F:          /* Access in Channel */
      {
        /* Report back that Access in Channel was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_ACCESS_IN_CHAN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Access in Channel ", 0, 0, 0 );
        }/* The client provided a reportback function */
      } /* end case - UIM_ACCESS_IN_CHAN_F */
      break;

    case UIM_MANAGE_CHANNEL_F:
      {
        uim_rpt_buf.rpt_type = UIM_MANAGE_CHANNEL_R;

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Reporting error for Manage Channel ", 0, 0, 0 );
      } /* end case - UIM_MANAGE_CHANNEL_F */
      break;

#ifdef FEATURE_UIM_WIM
      case UIM_MANAGE_SECURITY_ENV_F:           /* Manage Security Environment */
        {
          /* Report back that manage security env was not successful */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_MANAGE_SECURITY_ENV_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting error for Manage Security Environment", 0, 0, 0 );
          }
        } /* end case - UIM_MANAGE_SECURITY_ENV_F */
        break;

      case UIM_PERFORM_SECURITY_OP_F:         /* Perform Security Operation */
        {
          /* Report back that PSO was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_PERFORM_SECURITY_OP_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting error for Perform Security Operation ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_PERFORM_SECURITY_OPERATION_F */
        break;

      case UIM_ASK_RANDOM_F:          /* Ask Random */
        {
          /* Report back that Ask Random was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
            uim_rpt_buf.rpt_type = UIM_ASK_RANDOM_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting error for Ask Random ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_ASK_RANDOM_F */
        break;
#endif /* FEATURE_UIM_WIM */

      default:
        ERR( " Unknown command %d in uim_send_util_err_rpt",
                                               cmd_ptr->hdr.command, 0, 0 );
        break;

    }  /* switch */
  } /* end if - the client has supplied a report back function. */
  /* return buffer to free queue */
  if (save_done_q_ptr != NULL)
  {
      /* place command on requested queue */
      q_put( save_done_q_ptr, save_link_ptr );
  }

} /* uim_send_util_err_rpt */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_UTIL_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM UTIL state machine to process the command.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM_UTIL state variable amongst others.

===========================================================================*/

boolean uim_process_util_command
(
  uim_cmd_type *cmd_ptr  /* Pointer to received command */
)
{
  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_process_util_command cmd_ptr is NULL",0,0,0);
  }

  /* Copy user data from received command to report buffer */
  uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* Let the client know the command is being processed if the client
     provides it's tcb pointer. */
  cmd_ptr->hdr.status = UIM_RECEIVED_S;

  if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
  {
    (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                    cmd_ptr->hdr.cmd_hdr.sigs );
  } /* end if - the client wants to know the command is being processed */
  /* Process the util command. */
  switch (cmd_ptr->hdr.command)
  {
    case UIM_VERIFY_IN_CHAN_F:              /* Verify command */
      {
        MSG_MED( "Received Verify in chan command ", 0, 0, 0 );

        /* Build the Verify list of states. */
        uim_util_states[0] = UIM_UTIL_VERIFY_IN_CHAN_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel =  cmd_ptr->verification_in_chan.channel & 
                                                      UIM_LOGICAL_CHANNEL_MASK;
      } /* end if - UIM_VERIFY_IN_CHAN_F */
      break;

    case UIM_ENABLE_VER_REQ_IN_CHAN_F:   /* Enable Verification Requirement */
      {
        MSG_MED( "Received Enable Verification in chan Command", 0, 0, 0 );

        /* Build the Enable Verification list of states. */
        uim_util_states[0] = UIM_UTIL_ENABLE_VER_IN_CHAN_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel = cmd_ptr->enable_ver_in_chan.channel &
                                                      UIM_LOGICAL_CHANNEL_MASK;

      } /* end case - UIM_ENABLE_VER_REQ_IN_CHAN_F */
      break;

    case UIM_DISABLE_VER_REQ_IN_CHAN_F: /* Disable Verification Requirement */
      {
        MSG_MED( "Received Disable Verification in chan command ", 0, 0, 0 );

        /* Build the Disable Verification list of states. */
        uim_util_states[0] = UIM_UTIL_DISABLE_VER_IN_CHAN_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel = cmd_ptr->disable_ver_in_chan.channel &
                                                UIM_LOGICAL_CHANNEL_MASK;
      } /* end case - UIM_DISABLE_VER_REQ_IN_CHAN_F */
      break;

    case UIM_CHANGE_REF_DATA_IN_CHAN_F:    /* Change Reference Data */
      {
        MSG_MED( "Received Change Reference Data in chan command ", 0, 0, 0 );

        /* Build the Change Reference Data list of states. */
        uim_util_states[0] = UIM_UTIL_CHANGE_REF_IN_CHAN_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel =  cmd_ptr->change_ref_in_chan.channel &
                                                UIM_LOGICAL_CHANNEL_MASK;

      } /* end case - UIM_CHANGE_REF_DATA_IN_CHAN_F */
      break;

    case UIM_RESET_RETRY_COUNTER_IN_CHAN_F:        /* Reset Retry Counter */
      {
        MSG_MED( "Received Reset Retry Counter in chan command ", 0, 0, 0 );

        /* Build the Reset Retry list of states. */
        uim_util_states[0] = UIM_UTIL_RESET_RETRY_IN_CHAN_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel =  cmd_ptr->reset_retry_in_chan.channel &
                                                UIM_LOGICAL_CHANNEL_MASK;

      } /* end case - UIM_RESET_RETRY_COUNTER_IN_CHAN_F */
      break;

    case UIM_SELECT_IN_CHAN_F:              /* Select in Channel */
      {
        MSG_MED( "Received Select in Channel command ", 0, 0, 0 );

        /* Build the Select in Channel list of states. */
        uim_util_states[0] = UIM_UTIL_SELECT_IN_CHAN_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel =  cmd_ptr->select_in_chan.channel &
                                               UIM_LOGICAL_CHANNEL_MASK;

      } /* end case - UIM_SELECT_IN_CHAN_F */
      break;

    case UIM_ACCESS_IN_CHAN_F:              /* Access in Chan */
      {
        MSG_MED( "Received Access in Channel command ", 0, 0, 0 );

        /* Build the Access in Channel list of states. */
        uim_util_states[0] = UIM_UTIL_ACCESS_IN_CHAN_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel = cmd_ptr->access_in_chan.channel &
                                                UIM_LOGICAL_CHANNEL_MASK;

      } /* end case - UIM_ACCESS_IN_CHAN_F */
      break;

    case UIM_MANAGE_CHANNEL_F:       /* Manage Channel */
    {
      MSG_MED( "Received Manage Channel command ", 0, 0, 0 );

      /* Build the Manage Channel list of states. */
      uim_util_states[0] = UIM_UTIL_MANAGE_CHANNEL_ST;
      uim_util_states[1] = UIM_UTIL_DONE_ST;
      uim_util_state_ptr = uim_util_states;
      
      uim_util_logical_channel = cmd_ptr->manage_channel.channel &
                                                UIM_LOGICAL_CHANNEL_MASK;
    } /* end case - UIM_MANAGE_CHANNEL_F */
    break;

#ifdef FEATURE_UIM_WIM
    case UIM_MANAGE_SECURITY_ENV_F:   /* Manage Security Environment */
      {
        MSG_MED( "Received Manage Security Environment command ", 0, 0, 0 );

        /* Build the Manage Security Environment list of states. */
        uim_util_states[0] = UIM_UTIL_MSE_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel = cmd_ptr->mse.channel &
                                                UIM_LOGICAL_CHANNEL_MASK;

      } /* end case - UIM_MANAGE_SECURITY_ENV_F */
      break;

    case UIM_PERFORM_SECURITY_OP_F:        /* Perform Security Operation */
      {
        MSG_MED( "Received Perform Security Operation command ", 0, 0, 0 );

        /* Build the OTASP Commit list of states. */
        uim_util_states[0] = UIM_UTIL_PSO_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel =  cmd_ptr->pso.channel &
                                                UIM_LOGICAL_CHANNEL_MASK;

      } /* end if - UIM_PERFORM_SECURITY_OP_F */
      break;

    case UIM_ASK_RANDOM_F:          /* ASK Random */
      {
        MSG_MED( "Received Ask Random command ", 0, 0, 0 );

        /* Build the Ask Random list of states. */
        uim_util_states[0] = UIM_UTIL_ASK_RANDOM_ST;
        uim_util_states[1] = UIM_UTIL_DONE_ST;
        uim_util_state_ptr = uim_util_states;
        uim_util_logical_channel =  cmd_ptr->ask_random.channel &
                                               UIM_LOGICAL_CHANNEL_MASK;

      } /* end if - UIM_ASK_RANDOM_F */
      break;
#endif /* FEATURE_UIM_WIM */

     default:

      status = FALSE;
      ERR(" Unrecognized cmd recd in UTIL Module %d", cmd_ptr->hdr.command,0,0);

      break;

  } /* end switch (command_type) */

  if (status)
  {
    /* Call the state machine. */
    uim_util_command (cmd_ptr);
  }
  else
  {
    /* Set the status words to 0 so that a higher-level application receiving
    an error report does not inadvertantly get the wrong status words from a
    previous report.  This command was never sent to the UIM, so no meaningful
    status words are available */

    uim_rpt_buf.sw1 = SW1_NO_STATUS;
    uim_rpt_buf.sw2 = SW2_NO_INFO_GIVEN;

    uim_send_util_err_rpt(cmd_ptr);
  }

  return(status);

} /* uim_process_util_command() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_UTIL_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a util command.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/

LOCAL void uim_process_util_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type *cmd_ptr
)
{
  /* save the following info to place the buffer back on the requested queue */
  q_type *save_done_q_ptr;
  q_link_type *save_link_ptr;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_process_util_response cmd_ptr is NULL",0,0,0);
  }

  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_process_util_response rsp_ptr is NULL",0,0,0);
  }

  save_done_q_ptr = cmd_ptr->hdr.cmd_hdr.done_q_ptr;
  save_link_ptr   = &cmd_ptr->hdr.cmd_hdr.link;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Set the status to UIM_PASS unless something failed */
  uim_rpt_buf.rpt_status = UIM_PASS;

  switch (cmd_ptr->hdr.command)
  {
    case UIM_VERIFY_IN_CHAN_F:              /* Verify command */
      {

        /* Report back that Verify was completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_VERIFY_IN_CHAN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Verify Command", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_VERIFY_IN_CHAN_F */
      break;

    case UIM_ENABLE_VER_REQ_IN_CHAN_F: /* Enable Verification Requirement */
      {
        /* Report back that Enable Verification was completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_ENABLE_VER_REQ_IN_CHAN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Enable Verification", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_ENABLE_VER_REQ_IN_CHAN_F */
      break;

    case UIM_DISABLE_VER_REQ_IN_CHAN_F:         /* Disable Verification Requirement */
      {
       /* Report back that Disable Verification was completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_DISABLE_VER_REQ_IN_CHAN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting Disable Verification Requirement" , 0, 0, 0 );
        }
      } /* end case - UIM_DISABLE_VER_REQ_IN_CHAN_F */
      break;

    case UIM_CHANGE_REF_DATA_IN_CHAN_F:      /* Change Reference Data */
      {
        /* Report back Change Reference Data, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_CHANGE_REF_DATA_IN_CHAN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting Change Reference Data", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_CHANGE_REF_DATA_IN_CHAN_F */
      break;

    case UIM_RESET_RETRY_COUNTER_IN_CHAN_F:              /* Reset Retry Counter */
      {
        /* Report back the Retry Counter has been reset, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_RESET_RETRY_COUNTER_IN_CHAN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reset Retry Counter", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_RESET_RETRY_COUNTER_IN_CHAN_F */
      break;

  case UIM_SELECT_IN_CHAN_F:              /* Select in Channel */
    {
      /* Report back the File/ADF has been selected, if requested */
      if (cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_rpt_buf.rpt_type = UIM_SELECT_IN_CHAN_R;

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;
          
          /* copy the data into the report buffer  */
        memcpy(uim_rpt_buf.rpt.select_in_chan.data, rsp_ptr->rsp.data,
               rsp_ptr->cmd_rsp_size);

        /* Report to the Requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Select in Channel", 0, 0, 0 );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_SELECT_IN_CHAN_F */
    break;

  case UIM_ACCESS_IN_CHAN_F:     /* Read or Write from UIM in Logical Channel*/
    {
      /* Report back that Access was completed, if requested */
      if (cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_rpt_buf.rpt_type = UIM_ACCESS_IN_CHAN_R;

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Only copy the data from the reponse if the command
        was a READ command */

        if(cmd_ptr->access_in_chan.access == UIM_READ_F)
        {
          /* copy the data into the buffer from NV */
          if ( cmd_ptr->access_in_chan.num_bytes >= rsp_ptr->cmd_rsp_size )
          {
            /* Fill in the number of bytes in the response that we got
            from UIM into the NV command */
            cmd_ptr->access_in_chan.num_bytes_rsp = rsp_ptr->cmd_rsp_size;
            memcpy(cmd_ptr->access_in_chan.data_ptr, rsp_ptr->rsp.data,
                   rsp_ptr->cmd_rsp_size);
          }
          else
          {
            memcpy(cmd_ptr->access_in_chan.data_ptr, rsp_ptr->rsp.data,
                   cmd_ptr->access_in_chan.num_bytes);
            cmd_ptr->access_in_chan.num_bytes_rsp =
                            cmd_ptr->access_in_chan.num_bytes;
          }
        }
        /* Report to the Requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Reporting UIM Access in Channel", 0, 0, 0 );

      } /* (cmd_ptr->hdr.rpt_function != NULL) */
    } /* end case - UIM_ACCESS_IN_CHAN_F */
    break;

    case UIM_MANAGE_CHANNEL_F:            /* Manage Channel */
      {
        /* Report back that Channel Management was completed, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_MANAGE_CHANNEL_R;
    
          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;
    
          /* This will check to ensure that a valid channel number has been
          received from the card */
    
          uim_rpt_buf.rpt.channel.channel_num = UIM_NO_LOGICAL_CHANNEL_SET;
    
          if(cmd_ptr->manage_channel.mc_action == UIM_MC_OPEN)
          {
            uim_rpt_buf.rpt.channel.channel_num = rsp_ptr->rsp.data[0];
          }
          else
          {
            uim_rpt_buf.rpt.channel.channel_num = UIM_NO_LOGICAL_CHANNEL_SET;

            /* clean up the last directory global info for the current channel
             on the current slot if the close channel management is a successful
             operation */
            if (uim_rpt_buf.rpt_status == UIM_PASS)
            {
              uim_reset_dir_for_slot_channel(uim_drv_slot, 
                                             (uim_channel_type)cmd_ptr->manage_channel.op_close_channel);
            }
          }
    
          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM MANAGE CHANNEL", 0, 0, 0 );
        }
      } /* end case - UIM_MANAGE_CHANNEL_F */
      break;

#ifdef FEATURE_UIM_WIM
    case UIM_MANAGE_SECURITY_ENV_F:           /* Manage Security Environment */
      {
        /* Report back Manage Sec Env info, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_MANAGE_SECURITY_ENV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting Manage Security Environment", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_MANAGE_SECURITY_ENV_F */
      break;

    case UIM_PERFORM_SECURITY_OP_F:              /* Perform Security Op */
      {
        /* Report back the perform security operation results, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_PERFORM_SECURITY_OP_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the report */
          uim_rpt_buf.rpt.sec_op.data_length = rsp_ptr->cmd_rsp_size;

          /* copy the data into the report buffer  */
          memcpy(uim_rpt_buf.rpt.sec_op.data, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting Perform Security Operation ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_PERFORM_SECURITY_OP_F */
      break;

    case UIM_ASK_RANDOM_F:          /* Ask for a random number */
      {
        /* Report back the ask random results, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_ASK_RANDOM_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the result code in the response */
          uim_rpt_buf.rpt.ask_rand.data_length = rsp_ptr->cmd_rsp_size;

          /* copy the data into the report buffer  */
          memcpy((byte*) uim_rpt_buf.rpt.ask_rand.data, rsp_ptr->rsp.data,
                  rsp_ptr->cmd_rsp_size);

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting Ask Random Number ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_ASK_RANDOM_F */
      break;
#endif /* FEATURE_UIM_WIM */

    default:
        ERR( " Unknown command %d in uim_process_cdma_response",
              cmd_ptr->hdr.command, 0, 0 );
      break;
  } /* end switch - cmd_ptr->hdr.command */

  /* return buffer to free queue */
  if (save_done_q_ptr != NULL)
  {
    /* place command on requested queue */
    q_put( save_done_q_ptr, save_link_ptr );
  } /* end if - save_done_q_ptr != NULL */
} /* uim_process_util_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_UTIL_COMMAND

DESCRIPTION
  This procedure is the main state machine for UTIL UIM command processing.

DEPENDENCIES
  This function uses the UIM UTIL state pointer to determine what to process.
  uim_util_logical_channel contains the logical channel which has been
  established for the AID in the command (except MC - Open)

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

void uim_util_command
(
  uim_cmd_type *cmd_ptr  /* Pointer to the received command */
)
{
  /* Set the protocol to UIM_UICC */
  uim_util_req_buf_ptr->protocol = UIM_UICC;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_util_command cmd_ptr is NULL",0,0,0);
  }

  /* Build an APDU based on the UIM util state */
  switch(*uim_util_state_ptr)
  {
    case UIM_UTIL_VERIFY_IN_CHAN_ST:                 /* Verify State */
      {
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                                | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = VERIFY_IN_CHAN;
        uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_util_req_buf_ptr->apdu_hdr.p2 = cmd_ptr->verification_in_chan.ref_qual;
        /* This command simply checks whether Verification is required,
         but does not actually verify */
        uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        if(cmd_ptr->verification_in_chan.ver_data[0] == '\0')
        {
          uim_util_req_buf_ptr->apdu_hdr.p3 = 0;
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
        }
        else
        { /* It is a verification command */
          uim_util_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
          uim_format_chv(cmd_ptr->verification_in_chan.ver_data);
          memcpy((byte*) (uim_util_req_buf_ptr->req.verification_in_chan.verification_data),
                 cmd_ptr->verification_in_chan.ver_data, UIM_MAX_CHV_DIGITS);
        }
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      }  /* end case - UIM_UTIL_VERIFY_IN_CHAN_ST */
      break;

    case UIM_UTIL_ENABLE_VER_IN_CHAN_ST:                   /* Enable Verification State */
      {
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                                    | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = ENABLE_VER_IN_CHAN;
        uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_util_req_buf_ptr->apdu_hdr.p2 = cmd_ptr->enable_ver_in_chan.ref_qual;
        uim_util_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
        uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        /* bytes of data sent in the command */
        uim_format_chv(cmd_ptr->enable_ver_in_chan.ver_data);
        memcpy( (byte*) uim_util_req_buf_ptr->req.enable_verfn_in_chan.verification_data,
                cmd_ptr->enable_ver_in_chan.ver_data, UIM_MAX_CHV_DIGITS);
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_ENABLE_VER_IN_CHAN_ST */
      break;

    case UIM_UTIL_DISABLE_VER_IN_CHAN_ST:   /* Disable Verification State */
      {

        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                                    | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = DISABLE_VER_IN_CHAN;
        uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_util_req_buf_ptr->apdu_hdr.p2 = cmd_ptr->disable_ver_in_chan.ref_qual;
        uim_util_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
        uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        /* bytes of data sent in the command */
        uim_format_chv(cmd_ptr->disable_ver_in_chan.ver_data);
        memcpy( (byte*) uim_util_req_buf_ptr->req.disable_verfn_in_chan.verification_data ,
                cmd_ptr->disable_ver_in_chan.ver_data, UIM_MAX_CHV_DIGITS);
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_DISABLE_VER_IN_CHAN_ST */
      break;

    case UIM_UTIL_CHANGE_REF_IN_CHAN_ST:                 /* Change Reference State */
      {
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                                    | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = CHANGE_REF_IN_CHAN;
        uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_util_req_buf_ptr->apdu_hdr.p2 = cmd_ptr->change_ref_in_chan.ref_qual;
        /* Length of the reference data * 2 */
        uim_util_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;
        uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        /* bytes of data sent in the command */
        uim_format_chv(cmd_ptr->change_ref_in_chan.ver_data);
        memcpy( (byte*) uim_util_req_buf_ptr->req.change_ref_in_chan.verification_data,
                cmd_ptr->change_ref_in_chan.ver_data, UIM_MAX_CHV_DIGITS);
        uim_format_chv(cmd_ptr->change_ref_in_chan.new_ref_data);
        memcpy( (byte*) uim_util_req_buf_ptr->req.change_ref_in_chan.new_reference_data ,
                cmd_ptr->change_ref_in_chan.new_ref_data, UIM_MAX_CHV_DIGITS);
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_CHANGE_REF_IN_CHAN_ST */
      break;

    case UIM_UTIL_RESET_RETRY_IN_CHAN_ST:                     /* Reset Retry State */
      {
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                                | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = RESET_RETRY_IN_CHAN;
        uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_util_req_buf_ptr->apdu_hdr.p2 = cmd_ptr->reset_retry_in_chan.ref_qual;
        /* bytes of data sent in the command */
        uim_util_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS*2;
        uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_format_chv(cmd_ptr->reset_retry_in_chan.unblk_data);
        memcpy( (byte*) uim_util_req_buf_ptr->req.reset_retry_in_chan.unblock_data,
                cmd_ptr->reset_retry_in_chan.unblk_data, UIM_MAX_CHV_DIGITS);
        uim_format_chv(cmd_ptr->reset_retry_in_chan.new_ref_data);
        memcpy( (byte*) uim_util_req_buf_ptr->req.reset_retry_in_chan.new_ref_data ,
                cmd_ptr->reset_retry_in_chan.new_ref_data, UIM_MAX_CHV_DIGITS);
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_RESET_RETRY_IN_CHAN_ST */
      break;

    case UIM_UTIL_SELECT_IN_CHAN_ST:                     /* Select in Channel State */
      {
        uim_util_req_buf_ptr->apdu_hdr.instrn = SELECT;
  
        if (cmd_ptr->select_in_chan.file_type == UIM_ADF)
        { /* Application Selection */
          uim_util_req_buf_ptr->apdu_hdr.uim_class =
              UIM_UTIL_INSTRUCTION_CLASS2;
          uim_util_req_buf_ptr->apdu_hdr.p1 = 0x04;
          uim_util_req_buf_ptr->apdu_hdr.p2 = 0;
          uim_util_req_buf_ptr->apdu_hdr.p3 =
              cmd_ptr->select_in_chan.aid.aid_length;
          memcpy(uim_util_req_buf_ptr->req.data,
              cmd_ptr->select_in_chan.aid.data,
              cmd_ptr->select_in_chan.aid.aid_length);
        }
        else
        {
          uim_util_req_buf_ptr->apdu_hdr.uim_class =
                                  UIM_UTIL_INSTRUCTION_CLASS1;
          uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
          uim_util_req_buf_ptr->apdu_hdr.p2 = 0;
          uim_util_req_buf_ptr->apdu_hdr.p3 = 0x02;
          uim_util_req_buf_ptr->req.mf_df_ef.high_byte =
                              WORD_HI (uim_path_to_be_sel[0]);
          uim_util_req_buf_ptr->req.mf_df_ef.low_byte  =
                              WORD_LO (uim_path_to_be_sel[0]);
        }
  
        uim_util_req_buf_ptr->apdu_hdr.uim_class |= uim_util_logical_channel;
        uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);

    } /* end case - UIM_UTIL_SELECT_IN_CHAN_ST */
    break;

    case UIM_UTIL_ACCESS_IN_CHAN_ST:             /* Access in Channel State */
      {
        if(cmd_ptr->access_in_chan.access == UIM_READ_F)
        {
        /* The selected file is TRANSPARENT.  Use a binary read to read
             from the file. */
          uim_util_req_buf_ptr->apdu_hdr.instrn = READ_BINARY;
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
        }
        else
        { /* cmd_ptr->access_in_chan.access == UIM_WRITE_F */
        /* The selected file is TRANSPARENT.  Use a binary write to read
             from the file. */
          uim_util_req_buf_ptr->apdu_hdr.instrn = UPDATE_BINARY;
          memcpy(uim_util_req_buf_ptr->req.data, cmd_ptr->access_in_chan.data_ptr,
                  cmd_ptr->access_in_chan.num_bytes);
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        }
        /* Offset into the EF -- high byte, low byte */
        uim_util_req_buf_ptr->apdu_hdr.p1 = WORD_HI(cmd_ptr->access_in_chan.offset);
        uim_util_req_buf_ptr->apdu_hdr.p2 = WORD_LO(cmd_ptr->access_in_chan.offset);
        uim_util_req_buf_ptr->apdu_hdr.p3 = (byte) cmd_ptr->access_in_chan.num_bytes;
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                           | uim_util_logical_channel;
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_ACCESS_IN_CHAN_ST */
      break;

    case UIM_UTIL_MANAGE_CHANNEL_ST:            /* Manage Channel State */
      {
        /* Build a Manage Channel APDU */
        uim_util_req_buf_ptr->apdu_hdr.instrn = MANAGE_CHANNEL;
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS2
                                               | uim_util_logical_channel;
  
        uim_util_req_buf_ptr->apdu_hdr.p2 = 
                                       cmd_ptr->manage_channel.op_close_channel;
  
        if(cmd_ptr->manage_channel.mc_action == UIM_MC_OPEN)
        {
          uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
          uim_util_req_buf_ptr->apdu_hdr.p3 = 1;
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
        }
        else
        {
          uim_util_req_buf_ptr->apdu_hdr.p1 = 0x80;
          uim_util_req_buf_ptr->apdu_hdr.p3 = 0x0;
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
        }
  
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_MANAGE_CHANNEL_ST */
      break;
  
#ifdef FEATURE_UIM_WIM

    case UIM_UTIL_MSE_ST:                 /* Manage Security Environment State */
      {
        uim_util_req_buf_ptr->apdu_hdr.uim_class =
                        UIM_UTIL_INSTRUCTION_CLASS1 | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = MSE;
  
        if (cmd_ptr->mse.operation == UIM_MSE_RESTORE)
        {
          uim_util_req_buf_ptr->apdu_hdr.p1 = 0xF3;
          uim_util_req_buf_ptr->apdu_hdr.p2 =
                            cmd_ptr->mse.mse_data.restore_data.se_number;
          uim_util_req_buf_ptr->apdu_hdr.p3 = 0x0;
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
        } /* end if - cmd->ptrmse.operation == UIM_MSE_RESTORE */
        else if (cmd_ptr->mse.operation == UIM_MSE_SET)
        {
          uim_util_req_buf_ptr->apdu_hdr.p1 =
                            cmd_ptr->mse.mse_data.set_data.set_p1;
          uim_util_req_buf_ptr->apdu_hdr.p2 =
                            cmd_ptr->mse.mse_data.set_data.crt;
          uim_util_req_buf_ptr->apdu_hdr.p3 =
                            cmd_ptr->mse.mse_data.set_data.data_length;
          memcpy( (byte*) uim_util_req_buf_ptr->req.mse.crdos,
                cmd_ptr->mse.mse_data.set_data.data,
                cmd_ptr->mse.mse_data.set_data.data_length);
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        }  /* end if - cmd->ptrmse.operation == UIM_MSE_SET */
        else
        { /* Command is of type MSE - Derive Key */
          uim_util_req_buf_ptr->apdu_hdr.p1 = UIM_WIM_MSE_P1_COMPUTE;
          uim_util_req_buf_ptr->apdu_hdr.p2 = UIM_WIM_MSE_P2_CRYPTO_CHECKSUM;
          uim_util_req_buf_ptr->apdu_hdr.p3 =
                            cmd_ptr->mse.mse_data.derive_data.seed_length + 5;
          uim_util_req_buf_ptr->req.mse.crdos[0] = UIM_DATA_KEY_SESS_SY;
          uim_util_req_buf_ptr->req.mse.crdos[1] = 0x01;
          uim_util_req_buf_ptr->req.mse.crdos[2] =
                            cmd_ptr->mse.mse_data.derive_data.secret_key_ref;
          uim_util_req_buf_ptr->req.mse.crdos[3] = UIM_DATA_CHLNG_DATA_ITM;
          uim_util_req_buf_ptr->req.mse.crdos[4] =
                            cmd_ptr->mse.mse_data.derive_data.seed_length;
          memcpy( (byte*) &uim_util_req_buf_ptr->req.mse.crdos[5],
                cmd_ptr->mse.mse_data.derive_data.seed,
                cmd_ptr->mse.mse_data.derive_data.seed_length);
          uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        } /* end else - command is of type MSE- Derive Key */
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_MSE_ST */
      break;

    case UIM_UTIL_PSO_ST:                   /* Perform Security Operation State */
      {
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                                    | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = PSO;
        switch(cmd_ptr->pso.operation)
        {
          case UIM_PSO_ENCIPHER:                 /* PSO - Encipher  */
            {
              uim_util_req_buf_ptr->apdu_hdr.p1 = UIM_PSO_DATA_PADD_IND_CRYPTOGRAM;
              uim_util_req_buf_ptr->apdu_hdr.p2 = 0x0;
              uim_util_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->pso.data_length;
              if(cmd_ptr->pso.data_length == 0)
              {
                  uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
  
              }
              else
              {
                  uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
              }
            } /* end case - cmd_ptr->pso.operation == UIM_PSO_ENCIPHER */
            break;

            case UIM_PSO_DECIPHER: /* PSO - DECIPHER */
            {
              uim_util_req_buf_ptr->apdu_hdr.p1 = UIM_PSO_DATA_PLAIN_VALUE;
              uim_util_req_buf_ptr->apdu_hdr.p2 = UIM_PSO_DATA_PADD_IND_CRYPTOGRAM;
              uim_util_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->pso.data_length;
              memcpy((byte*) uim_util_req_buf_ptr->req.pso.data,
              cmd_ptr->pso.data, cmd_ptr->pso.data_length);
              uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
            }  /* end case - cmd_ptr->pso.operation == UIM_PSO_DECIPHER */
            break;

            case UIM_PSO_COMPUTE_DIG_SIG:   /* PSO - Compute Dig Sig */
            {
              uim_util_req_buf_ptr->apdu_hdr.p1 = UIM_PSO_DATA_DIGITAL_SIGNATURE;
              uim_util_req_buf_ptr->apdu_hdr.p2 = UIM_PSO_DATA_DIGITAL_SIGNATURE_INPUT;
              uim_util_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->pso.data_length;
              memcpy((byte*) uim_util_req_buf_ptr->req.pso.data,
                cmd_ptr->pso.data, cmd_ptr->pso.data_length);   
              uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
            }  /* end case - cmd_ptr->pso.operation == UIM_PSO_COMPUTE_DIG_SIG */
            break;

            case UIM_PSO_VERIFY:         /* PSO - Verify */
            {
              uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_util_req_buf_ptr->apdu_hdr.p2 = UIM_PSO_DATA_INPUT_TEMPLATE_DS;
              uim_util_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->pso.data_length;
              memcpy((byte*) uim_util_req_buf_ptr->req.pso.data,
                cmd_ptr->pso.data, cmd_ptr->pso.data_length);
              uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
            }  /* end case - cmd_ptr->pso.operation == UIM_PSO_VERIFY */
            break;

            case UIM_PSO_COMPUTE_CHECKSUM:     /* PSO - Compute Checksum */
            {
              uim_util_req_buf_ptr->apdu_hdr.p1 = UIM_PSO_DATA_CRYPTOGRAPHIC_CHECKSUM;
              uim_util_req_buf_ptr->apdu_hdr.p2 = UIM_PSO_DATA_PLAIN_VALUE;
              uim_util_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->pso.data_length;
              memcpy((byte*) uim_util_req_buf_ptr->req.pso.data,
              cmd_ptr->pso.data, cmd_ptr->pso.data_length);
              uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
            }  /* end case - cmd_ptr->pso.operation == UIM_PSO_COMPUTE_CHECKSUM */
            break;

          default:
              MSG_HIGH( "Invalid PSO Operation" , 0, 0, 0);
            break;

        } /* end of PSO switch */
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_PSO_ST */
      break;
  
    case UIM_UTIL_ASK_RANDOM_ST:  /* Ask Random State */
      {
        uim_util_req_buf_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS1
                                                    | uim_util_logical_channel;
        uim_util_req_buf_ptr->apdu_hdr.instrn = ASK_RANDOM;
        uim_util_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_util_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_util_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->ask_random.data_length;
        uim_util_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
        uim_util_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_util_req_buf_ptr->rpt_function = uim_command_response_callback;
        uim_send_command(uim_util_req_buf_ptr);
      } /* end case - UIM_UTIL_ASK_RANDOM_ST */
       break;
#endif /* FEATURE_UIM_WIM */

    case UIM_UTIL_DONE_ST:                         /* Done State */
      {
        /* Process the response of the command. */
        uim_process_util_response(uim_rsp_buf_ptr, cmd_ptr);
      } /* end case - UIM_UTIL_DONE_ST */
      break;

    default:

      MSG_HIGH("Invalid state in uim_util_command %ld",*uim_util_state_ptr,0,0);
      break;
  } /* end of main switch */

  MSG_LOW("UIM state in uim_command %ld",*uim_util_state_ptr, 0,0);

} /* uim_util_command */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_UTIL_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_util_state_ptr.

===========================================================================*/

uim_cmd_status_type uim_util_command_response
(
 uim_rsp_buf_type const *rsp_ptr,
 uim_cmd_type     *cmd
)
{
  uim_cmd_status_type status; /* Status of response processing. */
  uim_util_state_type curr_uim_util_state; /* Holds initial util state */
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  /* Assume the command response is proper. */
  status = UIM_CMD_SUCCESS;

  if (cmd == NULL)
  {
    ERR_FATAL("uim_util_command_response cmd is NULL",0,0,0);
  }
  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_util_command_response rsp_ptr is NULL",0,0,0);
  }

  /* Fill the status words into the report buffer. This could be sent for
     both passed and failed reports. If the command is not sent to the
     card, then the status words are cleared before sending the report */
  uim_rpt_buf.sw1 = (uim_sw1_type) rsp_ptr->sw1;
  uim_rpt_buf.sw2 = (uim_sw2_type) rsp_ptr->sw2;
  
  /* UIM_PARITY is used to indicate an unknown response from the UIM. */
  if ( rsp_ptr->cmd_status == UIM_PARITY )
  {
    /* If we get a parity error, we would like to treat it as a command
        error */
    return(UIM_CMD_ERR);
  } /* end if - cmd_status == UIM_PARITY */

  /* Necessary so that we do not change the uim state and switch into that
     case also */
  curr_uim_util_state = *uim_util_state_ptr;

  switch (curr_uim_util_state)
  {
    case UIM_UTIL_VERIFY_IN_CHAN_ST:               /* VERIFY state */
    case UIM_UTIL_DISABLE_VER_IN_CHAN_ST:          /* Disable Verification State */
    case UIM_UTIL_ENABLE_VER_IN_CHAN_ST:           /* Enable Verification State */
    case UIM_UTIL_CHANGE_REF_IN_CHAN_ST:           /* Change Reference Data State */
    case UIM_UTIL_RESET_RETRY_IN_CHAN_ST:          /* Reset Retry State */
      {
        if(rsp_ptr->cmd_status != UIM_DONE)
        {
          /* If it's a PIN-related command, we don'w want to PIN-block, so 
          don't repeat the APDU in the event of a a failure */
          cmd->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
        }
      }
    //lint -fallthrough
    /* Allow Fall to through */
    case UIM_UTIL_ACCESS_IN_CHAN_ST:
    case UIM_UTIL_SELECT_IN_CHAN_ST:
    case UIM_UTIL_MANAGE_CHANNEL_ST:
#ifdef FEATURE_UIM_WIM
    case UIM_UTIL_MSE_ST:                  /* Manage Security Environment State */
    case UIM_UTIL_PSO_ST:                  /* Perform Security Operation State */
    case UIM_UTIL_ASK_RANDOM_ST:           /* Ask Random State */
#endif /* FEATURE_UIM_WIM */
      {
        /* Determine if the UIM command responded with a problem. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Indicate a problem with the command. */
          return (UIM_CMD_ERR);
        }
        else /* The command completed successfully. */
        {
          /* Get the next state. */
          ++uim_util_state_ptr;
        } /* end if - the UIM command responded with a problem. */
      } /* end case */
      break;

    case UIM_UTIL_DONE_ST:                         /* Done State */
      {
        /* Do nothing */
      } /* end case - UIM_UTIL_DONE_ST */
      break;

    default:

      MSG_HIGH( "Invalid State in uim_util_command_response ", 0, 0, 0 );
      status = UIM_CMD_ERR;
      break;

  } /* end switch - curr_uim_util_state */

  /* Continue processing the command only if the response indicates success. */
  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of this command. */
    uim_util_command(cmd);
  } /* end if - command is still in progress */

  /* Determine if the command has completed */
  if (UIM_UTIL_DONE_ST == *uim_util_state_ptr)
  {
    /* Indicate the command has completed successfully. */
    return (completed_status);
  } /* end if - uim_util_state_ptr is UIM_UTIL_DONE_ST */

  return(status);

} /* uim_util_command_response */
/* <EJECT> */

#endif /* FEATURE_UIM && FEATURE_UIM_UTIL */
