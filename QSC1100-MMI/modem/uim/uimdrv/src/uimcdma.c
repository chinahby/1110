/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   C D M A   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the CDMA protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_cdma_command
    Processes a generic UIM server command

  uim_send_cdma_err_rpt
    Processes an error condition for a cdma UIM server command

  uim_cdma_command_response
    This function processes the UIM responses from cdma APDUs.

Copyright (c) 2001-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uimcdma.c_v   1.6   05 Sep 2002 10:42:42   wli  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimcdma.c#3 $ $DateTime: 2009/06/17 04:05:17 $ $Author: sratnu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/09   ssr     Fixed MN-AAA Auth
03/04/09   ssr     Fixed compilation warnings
11/10/08   sun     Fixed Featurization
10/05/07   vs/nk   Added EUIMID feature support
08/17/06   nk      Fixed casting in processing SSPR Config Request
04/03/06   pv      Lint Fixes
03/08/06   pv      Load the status words to the report buffer even if we
                   errored out because of un-known status words
11/14/05   pv      Lint Fixes
11/04/05   pv      Fixed compilation warning
04/28/05   jk      Added support to properly transfer the user data from the
                   incoming command to the report
08/27/04   jk      Added support for HRPD AN Authentication services in R-UIM
05/17/04   ck      Moved the call to uim_send_cdma_err_rpt() from default case
                   in the function uim_process_cdma_command() to the end of the
                   function based on tbe status flag.
05/13/04   ts      Fixed a bad reference for MN-AAA authentication processing.
02/20/04   ts      Fixed lint errors.
12/19/03   jk      Added processing for R-UIM 3GPD Services.
03/28/03   ts      Replaced ME_TO_UIM and UIM_TO_ME with instruction case in
                   UIM driver commands.
02/06/03   ck      Removed the duplicate signalling of UIM_RECEIVED_S to client
02/02/03   ck      Lint changes.
01/17/03   ck      Included comdef.h outside of FEATURE_UIM_RUIM to avoid
                   compiler warning
10/24/02   ts      Removed timeout processing.  Timeouts are handled in the
                   driver.
09/05/02   wli     Changed P3 to 1 for COMMIT command APDU.
12/13/01   ts      Cleaned up the status word assignment and included memcpy
                   prototype.
                   Featurized Toolkit code.
12/04/01   ts      Added support for UIM application toolkit.
07/27/01   ts      Cleaned up the code.
07/23/01   ck      Added caching of response to store_esn_me command.
06/26/01   ts      Changed CAVE ESN debug feature switch name to have "FEATURE_"
                   prepended.
01/16/01   ts      Created module.  This module contains the generic state
                   machine.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

/*lint -esym(766,..\..\services\utils\comdef.h)
  comdef.h was previously included to get rid of compiler warning about
  no external translation unit.  The best thing to do would be
  to not compile uimcdma.c based on a rule..... but till then.....
  let us calm down the lint
*/

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_RUIM)
#include "bit.h"
#include "msg.h"
#include "err.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimcdma.h"
#include <memory.h>

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* State of the UIM */
LOCAL const uim_cdma_state_type *uim_cdma_state_ptr;

/* Pointer to the Global UIM Request buffer */
LOCAL uim_req_buf_type *uim_cdma_req_buf_ptr = &uim_req_buf;

/* State configuration for the commands. */
LOCAL uim_cdma_state_type uim_cdma_states[UIM_MAX_NUMBER_CDMA_STATES];

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/* Used to keep track of how many apdus have been sent in a chained Compute
   IP Auth Command */
LOCAL word uim_compute_ip_auth_num_blocks_sent = 0;
LOCAL boolean uim_compute_ip_auth_more_blocks_req = FALSE;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_CDMA_ERR_RPT

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
void uim_send_cdma_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  q_type *save_done_q_ptr;
  q_link_type *save_link_ptr;

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
      case UIM_STORE_ESN_ME_F:              /* Store ESN_ME command */
        {
          uim_store_esn_me_rsp.status = UIM_FAIL;

          /* Report back that store ESN_ME was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
             uim_rpt_buf.rpt_type = UIM_STORE_ESN_ME_R;

             /* Fill in necessary fields of command block */
             uim_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to requesting task */
             (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
             MSG_MED( "Reporting error for Store ESN_ME ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_STORE_ESN_ME_F */
        break;

#ifdef FEATURE_AUTH
      case UIM_SSD_UPDATE_F:                /* Begin process to update SSD */
        {
          /* Report back that store ESN_ME was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
             uim_rpt_buf.rpt_type = UIM_SSD_UPDATE_R;

             /* Fill in necessary fields of command block */
             uim_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to requesting task */
             (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
             MSG_MED( "Reporting error for Store ESN_ME ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_SSD_UPDATE_F */
        break;

      case UIM_CONFIRM_SSD_F:         /* Finish process to update SSD */
        {
          /* Did the client give a reportback functiom. */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_CONFIRM_SSD_R;
              /* Indicate the SSD update failed. */
              uim_rpt_buf.rpt.ssd_update_ok = FALSE;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting Error for Finish SSD Update", 0, 0, 0 );
          } /* The client gave a reportback function. */
        } /* end case - UIM_CONFIRM_SSD_F */
        break;

      case UIM_RUN_CAVE_F:                 /* Perform auth signature */
        {
          /* Report back auth signature, if requested */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_RUN_CAVE_R;
              uim_rpt_buf.rpt.run_cave.auth = 0;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting Error for Auth Sig ", 0, 0, 0 );
          } /* end if - reportback function is provided by client. */
        } /* end case - UIM_RUN_CAVE_F */
        break;

      case UIM_GENERATE_KEYS_F:              /* Generate CMEA key and VPM */
        {
          /* Report back the result, if requested */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_GENERATE_KEYS_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for key generation", 0, 0, 0 );
          }
        } /* end case - UIM_GENERATE_KEYS_F */
        break;

      case UIM_BS_CHAL_F:                   /* Do a Base station Challenge */
        {
          /* Report back the result, if requested */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_BS_CHAL_R;
              uim_rpt_buf.rpt.bs_chal.randbs = 0;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for BS challenge", 0, 0, 0 );
          }
        } /* end case - UIM_BS_CHAL_F */
        break;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_OTASP
      case UIM_COMMIT_F:              /* Commit Request */
        {
          /* Report back that Commit was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_COMMIT_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for Commit ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_COMMIT_F */
        break;

      case UIM_MS_KEY_REQ_F:          /* MS Key Request */
        {
          /* Report back that MS Key Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_MS_KEY_REQ_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for MS Key Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_MS_KEY_REQ_F */
        break;

      case UIM_KEY_GEN_REQ_F:         /* Key Generation Request */
        {
          /* Report back that Key Generation Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_KEY_GEN_REQ_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for Key Gen Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_KEY_GEN_REQ_F */
        break;

      case UIM_VALIDATE_F:            /* Validation Request */
        {
          /* Report back that Validation Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_VALIDATE_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for Validation Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_VALIDATE_F */
        break;

      case UIM_CONFIG_REQ_F:             /* Configuration Request */
        {
          /* Report back that Config Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_CONFIG_REQ_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Fill the block id in the response from the command */
              uim_rpt_buf.rpt.configuration.block_id =
                cmd_ptr->configuration.block_id;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for Config Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_CONFIG_REQ_F */
        break;

      case UIM_DOWNLOAD_REQ_F:           /* Download Request */
        {
          /* Report back that Download Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_DOWNLOAD_REQ_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for Download Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_DOWNLOAD_REQ_F */
        break;

      case UIM_SSPR_CONFIG_REQ_F:        /* SSPR Configuration Request */
        {
          /* Report back that SSPR Config Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_SSPR_CONFIG_REQ_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for SSPR Config Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_SSPR_CONFIG_REQ_F */
        break;

      case UIM_SSPR_DOWNLOAD_REQ_F:      /* SSPR Download Reqeust */
        {
          /* Report back that SSPR Download Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
             uim_rpt_buf.rpt_type = UIM_SSPR_DOWNLOAD_REQ_R;

             /* Fill in necessary fields of command block */
             uim_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to requesting task */
             (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
             MSG_MED( "Reporting error for SSPR Download Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_SSPR_DOWNLOAD_REQ_F */
        break;

      case UIM_OTAPA_REQ_F:           /* Otapa Request */
        {
          /* Report back that OTAPA Request was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_OTAPA_REQ_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for OTAPA Request ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_OTAPA_REQ_F */
        break;

#endif /* FEATURE_OTASP */

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
      case UIM_COMPUTE_IP_AUTH_F:
        {
          /* Report back that Compute IP Auth was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
              uim_rpt_buf.rpt_type = UIM_COMPUTE_IP_AUTH_R;

              /* Fill in necessary fields of command block */
              uim_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
              MSG_MED( "Reporting error for Compute IP Auth ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_COMPUTE_IP_AUTH_F */
        break;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN*/

#ifdef FEATURE_UIM_EUIMID
        case UIM_STORE_ESN_MEID_ME_F:              /* Store ESN_MEID_ME command */
        {
          uim_store_esn_me_rsp.status = UIM_FAIL;

          /* Report back that store ESN_MEID_ME was not completed */
          if (cmd_ptr->hdr.rpt_function != NULL)
          {
             uim_rpt_buf.rpt_type = UIM_STORE_ESN_MEID_ME_R;

             /* Fill in necessary fields of command block */
             uim_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to requesting task */
             (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
             MSG_MED( "Reporting error for Store ESN_MEID_ME ", 0, 0, 0 );
          }/* The client provided a reportback function */
        } /* end case - UIM_STORE_ESN_MEID_ME_F */
        break;
#endif /* FEATURE_UIM_EUIMID */

      default:
        ERR( " Unknown command %d in uim_send_cdma_err_rpt",
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

} /* uim_send_cdma_err_rpt */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_CDMA_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM_CDMA state variable amongst others.

===========================================================================*/

boolean uim_process_cdma_command
(
  uim_cmd_type *cmd_ptr  /* Pointer to received command */
)
{
  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  /* Copy user data from received command to report buffer */
  uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* Process the generic command. */
  switch (cmd_ptr->hdr.command)
  {
    case UIM_STORE_ESN_ME_F:              /* Store ESN_ME command */
      {
        MSG_MED( "Received STORE_ESN_ME command ", 0, 0, 0 );

        /* Build the Store ESN list of states. */
        uim_cdma_states[0] = UIM_RUIM_STORE_ESN_ME_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end if - UIM_STORE_ESN_ME_F */
      break;

#ifdef FEATURE_AUTH
    case UIM_SSD_UPDATE_F:                /* Begin process to update SSD */
      {
        MSG_MED( "Received SSD update command", 0, 0, 0 );

        /* Build the Start SSD Update list of states. */
        uim_cdma_states[0] = UIM_RUIM_UPDATE_SSD_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end case - UIM_SSD_UPDATE_F */
      break;

    case UIM_CONFIRM_SSD_F:         /* Finish process to update SSD */
      {
        MSG_MED( "Received Confirm SSD command ", 0, 0, 0 );

        /* Build the Finish SSD Update list of states. */
        uim_cdma_states[0] = UIM_RUIM_CONFIRM_SSD_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end case - UIM_CONFIRM_SSD_F */
      break;

    case UIM_RUN_CAVE_F:             /* Perform Run Cave algorithm */
      {
        MSG_MED( "Received Auth Signature command ", 0, 0, 0 );

        /* Build the Auth Signature list of states. */
        uim_cdma_states[0] = UIM_RUIM_RUN_CAVE_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end case - UIM_RUN_CAVE_F */
      break;

    case UIM_GENERATE_KEYS_F:              /* Generate CMEA key and VPM */
      {
        MSG_MED( "Received Generate Keys command ", 0, 0, 0 );

        /* Build the Generate Key list of states. */
        uim_cdma_states[0] = UIM_RUIM_GENERATE_KEY_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end case - UIM_GENERATE_KEYS_F */
      break;

    case UIM_BS_CHAL_F:                   /* Do a Base station Challenge */
      {
        MSG_MED( "Received Base station Challenge command ", 0, 0, 0 );

        /* Build the BS Challenge list of states. */
        uim_cdma_states[0] = UIM_RUIM_BS_CHALLENGE_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end case - UIM_BS_CHAL_F */
      break;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_OTASP
    case UIM_COMMIT_F:              /* Commit Request */
      {
        MSG_MED( "Received OTASP Commit command ", 0, 0, 0 );

        /* Build the OTASP Commit list of states. */
        uim_cdma_states[0] = UIM_RUIM_COMMIT_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end if - UIM_COMMIT_F */
      break;

    case UIM_MS_KEY_REQ_F:          /* MS Key Request */
      {
        MSG_MED( "Receive MS Key Request command ", 0, 0, 0 );

        /* Build the MS Key Request list of states. */
        uim_cdma_states[0] = UIM_RUIM_MS_KEY_REQ_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end if - UIM_MS_KEY_REQ_F */
      break;

      case UIM_KEY_GEN_REQ_F:         /* Key Generation Request */
        {
          MSG_MED( "Received Key Generation command ", 0, 0, 0 );

          /* Build the Key Generation list of states. */
          uim_cdma_states[0] = UIM_RUIM_KEY_GEN_REQ_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_KEY_GEN_REQ_F */
        break;

      case UIM_VALIDATE_F:            /* Validation Request */
        {
          MSG_MED( "Received Validation command ", 0, 0, 0 );

          /* Build the OTASP Validation list of states. */
          uim_cdma_states[0] = UIM_RUIM_VALIDATE_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_VALIDATE_F */
        break;

      case UIM_CONFIG_REQ_F:             /* Configuration Request */
        {
          MSG_MED( "Received Config Request command ", 0, 0, 0 );

          /* Build the OTASP COnfig Request list of states. */
          uim_cdma_states[0] = UIM_RUIM_CFG_REQ_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_CONFIG_REQ_F */
        break;

      case UIM_DOWNLOAD_REQ_F:           /* Download Request */
        {
          MSG_MED( "Received Download Request command ", 0, 0, 0 );

          /* Build the OTASP Download Request list of states. */
          uim_cdma_states[0] = UIM_RUIM_DLOAD_REQ_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_DOWNLOAD_REQ_F */
        break;

      case UIM_SSPR_CONFIG_REQ_F:        /* SSPR Configuration Request */
        {
          MSG_MED( "Received SSPR Config Req command ", 0, 0, 0 );

          /* Build the OTASP SSPR Config Request list of states. */
          uim_cdma_states[0] = UIM_RUIM_SSPR_CFG_REQ_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_SSPR_CONFIG_REQ_F */
        break;

      case UIM_SSPR_DOWNLOAD_REQ_F:      /* SSPR Download Reqeust */
        {
          MSG_MED( "Received SSPR Download command ", 0, 0, 0 );

          /* Build the OTASP SSPR Download Request list of states. */
          uim_cdma_states[0] = UIM_RUIM_SSPR_DLOAD_REQ_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_SSPR_DOWNLOAD_REQ_F */
        break;

      case UIM_OTAPA_REQ_F:           /* Otapa Request */
        {
          MSG_MED( "Received OTAPA Request command ", 0, 0, 0 );

          /* Build the OTASP OTAPA Request list of states. */
          uim_cdma_states[0] = UIM_RUIM_OTAPA_REQ_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_OTAPA_REQ_F */
        break;

#endif /* FEATURE_OTASP */

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
      case UIM_COMPUTE_IP_AUTH_F:           /* Compute IP Authentication */
        {
          MSG_MED( "Received Compute IP Authentication command ", 0, 0, 0 );
          /* Initialize variable to keep track of number of blocks sent in a
             chained command */
          uim_compute_ip_auth_num_blocks_sent = 0;
          uim_compute_ip_auth_more_blocks_req = FALSE;
          /* Build the Compute IP Auth list of states. */
          uim_cdma_states[0] = UIM_RUIM_COMPUTE_IP_AUTH_ST;
          uim_cdma_states[1] = UIM_RUIM_DONE_ST;
          uim_cdma_state_ptr = uim_cdma_states;
        } /* end if - UIM_COMPUTE_IP_AUTH_F */
        break;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */

#ifdef FEATURE_UIM_EUIMID
      case UIM_STORE_ESN_MEID_ME_F:              /* Store ESN_MEID_ME command */
      {
        MSG_MED( "Received STORE_ESN_MEID_ME command ", 0, 0, 0 );
        /* Build the Store ESN MEID list of states. */
        uim_cdma_states[0] = UIM_RUIM_STORE_ESN_MEID_ME_ST;
        uim_cdma_states[1] = UIM_RUIM_DONE_ST;
        uim_cdma_state_ptr = uim_cdma_states;
      } /* end if - UIM_STORE_ESN_MEID_ME_F */
      break;
#endif /* FEATURE_UIM_EUIMID */

      default:
        {
          status = FALSE;
          ERR("Unrecognized cmd recd in CDMA Module %d",
                                                   cmd_ptr->hdr.command,0,0);
        }
        break;

  } /* end switch (command_type) */

  if (status)
  {
    /* Call the state machine. */
    uim_cdma_command (cmd_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_rpt_buf.sw1 = (uim_sw1_type) 0;
    uim_rpt_buf.sw2 = (uim_sw2_type) 0;

    uim_send_cdma_err_rpt(cmd_ptr);
  }

  return(status);

} /* uim_process_cdma_command() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_CDMA_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a generic command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/

LOCAL void uim_process_cdma_response
(
  uim_rsp_buf_type *rsp_ptr,
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
    case UIM_STORE_ESN_ME_F:              /* Store ESN_ME command */
      {
        uim_store_esn_me_rsp.status = UIM_PASS;
        uim_store_esn_me_rsp.rsp_data =  rsp_ptr->rsp.store_esn.usage_ind_conf;

        /* Report back that store ESN_ME was completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_STORE_ESN_ME_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           uim_rpt_buf.rpt.store_esn.usage_ind_conf =
                       rsp_ptr->rsp.store_esn.usage_ind_conf;

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting result for Store ESN_ME ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_STORE_ESN_ME_F */
      break;

#ifdef FEATURE_AUTH
    case UIM_SSD_UPDATE_F:                /* Begin process to update SSD */
      {
        /* Report back that store ESN_ME was completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_SSD_UPDATE_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting result for SSD Update", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_SSD_UPDATE_F */
      break;

    case UIM_CONFIRM_SSD_F:         /* Finish process to update SSD */
      {
        /* -------------------------------------
        ** Now, if required, report back whether
        ** or not SSD update was successful.
        ** ------------------------------------- */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_CONFIRM_SSD_R;
          /* Indicate the SSD update passed. */
          uim_rpt_buf.rpt.ssd_update_ok = TRUE;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting SSD update complete", 0, 0, 0 );
        }
      } /* end case - UIM_CONFIRM_SSD_F */
      break;

    case UIM_RUN_CAVE_F:                 /* Run CAVE algorithm */
      {
        /* Report back auth signature, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_RUN_CAVE_R;
           uim_rpt_buf.rpt.run_cave.auth = b_unpackd(rsp_ptr->rsp.data, 0,
                  FSIZ(uim_run_cave_rsp_siz_type, auth_sig));

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Report to the Requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting auth Signature", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_RUN_CAVE_F */
      break;

    case UIM_GENERATE_KEYS_F:              /* Generate CMEA key and VPM */
      {
        /* Report back that keys are generated, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_GENERATE_KEYS_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Copy only the bytes received from the UIM. */
           memcpy(uim_rpt_buf.rpt.generate_keys.data,
                  rsp_ptr->rsp.data,
                  rsp_ptr->cmd_rsp_size);

           /* Report to the Requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting keys generated", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_GENERATE_KEYS_F */
      break;

    case UIM_BS_CHAL_F:                   /* Do a Base station Challenge */
      {
        /* Report back that keys are generated, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_BS_CHAL_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Copy only the bytes received from the UIM. */
           uim_rpt_buf.rpt.bs_chal.randbs = b_unpackd(rsp_ptr->rsp.data,0,32);

           /* Report to the Requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting BS challenge", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_BS_CHAL_F */
      break;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_OTASP
    case UIM_COMMIT_F:              /* Commit Request */
      {
        /* Report back the commit results, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_COMMIT_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill in the result code in the response */
           uim_rpt_buf.rpt.commit.result = rsp_ptr->rsp.commit.result;

           /* Report to the Requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting OTASP Commit Req ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_COMMIT_F */
      break;

    case UIM_MS_KEY_REQ_F:          /* MS Key Request */
      {
        /* Report back the MS key results, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_MS_KEY_REQ_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill in the result code in the response */
           uim_rpt_buf.rpt.ms_key.result = rsp_ptr->rsp.ms_key.result;

           /* Report to the Requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting OTASP MS Key Req ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_MS_KEY_REQ_F */
      break;

    case UIM_KEY_GEN_REQ_F:         /* Key Generation Request */
      {
        /* Report back the MS key results, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_KEY_GEN_REQ_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill in the result code in the response */
           uim_rpt_buf.rpt.key_gen.result = rsp_ptr->rsp.key_gen.result;

           /* Fill the length of the MS result */
           uim_rpt_buf.rpt.key_gen.ms_result_len =
                                 rsp_ptr->rsp.key_gen.ms_result_len;

           /* Fill the MS result */
           memcpy(uim_rpt_buf.rpt.key_gen.ms_result,
                    (byte *) rsp_ptr->rsp.key_gen.ms_result,
                    rsp_ptr->rsp.key_gen.ms_result_len);

           /* Report to the Requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting OTASP Key Gen Req ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_KEY_GEN_REQ_F */
      break;

    case UIM_VALIDATE_F:            /* Validation Request */
      {
        /* Report back the result of the Validation Request */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_VALIDATE_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill the block id in the response */
           uim_rpt_buf.rpt.validate.block_id = rsp_ptr->rsp.validate.block_id;

           /* Fill in the result code in the response */
           uim_rpt_buf.rpt.validate.result = rsp_ptr->rsp.validate.result_code;

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting Validation Request ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_VALIDATE_F */
      break;

    case UIM_CONFIG_REQ_F:             /* Configuration Request */
      {
        /* Report back the result of Config Request */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_CONFIG_REQ_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill the block id in the response */
           uim_rpt_buf.rpt.configuration.block_id =
             rsp_ptr->rsp.configuration.block_id;

           /* Fill in the block length in the response */
           uim_rpt_buf.rpt.configuration.block_length =
             rsp_ptr->rsp.configuration.block_length;

           /* Fill in the result code in the response */
           uim_rpt_buf.rpt.configuration.result =
             rsp_ptr->rsp.configuration.result_code;

           /* Fill in the data of the Configuration Response */
           /* Skip the first 3 bytes */
           memcpy(uim_rpt_buf.rpt.configuration.data,
                  (byte *)rsp_ptr->rsp.configuration.data ,
                  rsp_ptr->rsp.configuration.block_length);

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting Configuration Request ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_CONFIG_REQ_F */
      break;

    case UIM_DOWNLOAD_REQ_F:           /* Download Request */
      {
        /* Report back the result of Download Request */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
            uim_rpt_buf.rpt_type = UIM_DOWNLOAD_REQ_R;

            /* Fill in necessary fields of command block */
            uim_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Fill the block in the response */
            uim_rpt_buf.rpt.download.block_id = rsp_ptr->rsp.download.block_id;

            /* Fill in the result code in the response */
            uim_rpt_buf.rpt.download.result = rsp_ptr->rsp.download.result_code;

            /* Report to requesting task */
            (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
            MSG_MED( "Reporting Download Request ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_DOWNLOAD_REQ_F */
      break;

    case UIM_SSPR_CONFIG_REQ_F:        /* SSPR Configuration Request */
      {
        /* Report the result of SSPR Config Request */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_SSPR_CONFIG_REQ_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill the block id in the response */
           uim_rpt_buf.rpt.sspr_config.block_id =
             rsp_ptr->rsp.sspr_config.block_id;

           /* Fill in the result code in the response */
           uim_rpt_buf.rpt.sspr_config.result =
             rsp_ptr->rsp.sspr_config.result_code;

           /* Fill in the block length */
           uim_rpt_buf.rpt.sspr_config.block_length =
               rsp_ptr->rsp.sspr_config.block_length;

           /* Fill in the data of the SSPR Configuration Response */
           memcpy(uim_rpt_buf.rpt.sspr_config.data,
                 (byte*) rsp_ptr->rsp.sspr_config.data,
                  rsp_ptr->rsp.sspr_config.block_length);

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting SSPR Config Request ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_SSPR_CONFIG_REQ_F */
      break;

    case UIM_SSPR_DOWNLOAD_REQ_F:      /* SSPR Download Reqeust */
      {
        /* Report back the result of SSPR Download Request */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_SSPR_DOWNLOAD_REQ_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill the block id in the response */
           uim_rpt_buf.rpt.sspr_download.block_id =
                            rsp_ptr->rsp.sspr_download.block_id;

           /* Fill the result code in the response */
           uim_rpt_buf.rpt.sspr_download.result =
                            rsp_ptr->rsp.sspr_download.result_code;

           /* Fill the segment offset from the command */
           uim_rpt_buf.rpt.sspr_download.segment_offset =
                      (word) FLIPW(rsp_ptr->rsp.sspr_download.segment_offset);

           /* Fill the segment size from the command */
           uim_rpt_buf.rpt.sspr_download.segment_size =
                            rsp_ptr->rsp.sspr_download.segment_size;

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting SSPR Download Request ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_SSPR_DOWNLOAD_REQ_F */
      break;

    case UIM_OTAPA_REQ_F:           /* Otapa Request */
      {
        /* Report back the OTAPA results, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_OTAPA_REQ_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Fill in the result code in the response */
           uim_rpt_buf.rpt.otapa.result = rsp_ptr->rsp.otapa.result;

           /* Fill tbe NAM Lock Indicator */
           uim_rpt_buf.rpt.otapa.nam_lock_ind =
                               rsp_ptr->rsp.otapa.nam_lock_ind;

           /* get the rand otapa as a dword from the data stream */
           uim_rpt_buf.rpt.otapa.rand_otapa =
                  b_unpackd( (byte*) rsp_ptr->rsp.otapa.rand_otapa, 0, 32 );

           /* Report to the Requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting OTAPA Req ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_OTAPA_REQ_F */
      break;
#endif /* FEATURE_OTASP */

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
    case UIM_COMPUTE_IP_AUTH_F:             /* Configuration Request */
      {
        /* Report back the result of Config Request */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_COMPUTE_IP_AUTH_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           /* Copy only the bytes received from the UIM. */
           memcpy(uim_rpt_buf.rpt.compute_ip_auth.data,
                  rsp_ptr->rsp.data,
                  rsp_ptr->cmd_rsp_size);

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting Configuration Request ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_COMPUTE_IP_AUTH_F */
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */


#ifdef FEATURE_UIM_EUIMID
    case UIM_STORE_ESN_MEID_ME_F:         /* Store ESN_MEID_ME command */
      {
        uim_store_esn_me_rsp.status = UIM_PASS;
        uim_store_esn_me_rsp.rsp_data =  rsp_ptr->rsp.store_esn.usage_ind_conf;

        /* Report back that store ESN_ME was completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
           uim_rpt_buf.rpt_type = UIM_STORE_ESN_MEID_ME_R;

           /* Fill in necessary fields of command block */
           uim_rpt_buf.rpt_hdr.task_ptr = NULL;

           uim_rpt_buf.rpt.store_esn.usage_ind_conf =
                       rsp_ptr->rsp.store_esn.usage_ind_conf;

           /* Report to requesting task */
           (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
           MSG_MED( "Reporting result for Store ESN_MEID_ME ", 0, 0, 0 );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_STORE_ESN_MEID_ME_F */
      break;
#endif /* FEATURE_UIM_EUIMID */

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
} /* uim_process_cdma_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CDMA_COMMAND

DESCRIPTION
  This procedure is the main state machine for CDMA UIM command processing.

DEPENDENCIES
  This function uses the UIM CDMA state pointer to determine what to process.

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

void uim_cdma_command
(
  uim_cmd_type *cmd_ptr  /* Pointer to the received command */
)
{
  byte i;         /* index variable */
#ifdef FEATURE_AUTH
  word pos = 0;   /* block position  */
#endif /* FEATURE_AUTH */
#ifdef FEATURE_UIM_SUPPORT_3GPD
#ifdef FEATURE_UIM_3GPD_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP
  word number_bytes_left    = 0;
  word number_bytes_sent    = 0;
  byte *compute_ip_data_ptr = NULL;  
  word preceding_rrq_length = 0;  
  word offset               = 0;  /* start of 237 bytes of challenge    */
  word challenge_len        = 0;
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_UIM_3GPD_MOBILE_IP */
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  /* Set the protocol from the command to the UIM request buffer */
  uim_cdma_req_buf_ptr->protocol = cmd_ptr->hdr.protocol;

  /* Build an APDU based on the UIM generic state */
  switch(*uim_cdma_state_ptr)
  {
    case UIM_RUIM_STORE_ESN_ME_ST:                 /* Store ESN_ME state */
#ifdef FEATURE_UIM_EUIMID
    case UIM_RUIM_STORE_ESN_MEID_ME_ST:                 /* Store ESN_MEID_ME state */
#endif /* FEATURE_UIM_EUIMID */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = STORE_ESN_ME;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;

        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 8;

       /* If the uim state is to send store_esn_meid command then check if
          service 9 (meid) is allocated and activated.  If it is then the value
          of p1 is to be set to 1 and the meid needs to be sent instead of the
          esn */
#ifdef FEATURE_UIM_EUIMID
        if (*uim_cdma_state_ptr == UIM_RUIM_STORE_ESN_MEID_ME_ST)
        {
          if (cmd_ptr->store_meid.meid_available)
          {
            uim_cdma_req_buf_ptr->apdu_hdr.p1 = 1;
            for(i=0; i<UIM_RUIM_UIMID_MAX_SIZE; i++)
            {
              uim_cdma_req_buf_ptr->req.store_meid.meid[i] =
              QW_BYTE(cmd_ptr->store_meid.meid ,i);
            }
            uim_cdma_req_buf_ptr->req.store_meid.len_and_usage =
                                   cmd_ptr->store_meid.len_and_usage;
          }
          else
          {
            uim_cdma_req_buf_ptr->req.store_esn.len_and_usage =
                                   cmd_ptr->store_esn.len_and_usage;
            for ( i = UIM_RUIM_UIMID_OFFSET; i < UIM_RUIM_UIMID_MAX_SIZE; i++)
            {
              uim_cdma_req_buf_ptr->req.store_esn.esn[i] = 0;
            }
            memcpy( (byte*) uim_cdma_req_buf_ptr->req.store_esn.esn ,
                    (byte*) &(cmd_ptr->store_esn.esn), UIM_RUIM_UIMID_SIZE);
          }
        }
        else
#endif /* FEATURE_UIM_EUIMID */
        {
          uim_cdma_req_buf_ptr->req.store_esn.len_and_usage =
                                   cmd_ptr->store_esn.len_and_usage;
          for ( i = UIM_RUIM_UIMID_OFFSET; i < UIM_RUIM_UIMID_MAX_SIZE; i++)
          {
            uim_cdma_req_buf_ptr->req.store_esn.esn[i] = 0;
          }
          memcpy( (byte*) uim_cdma_req_buf_ptr->req.store_esn.esn ,
                  (byte*) &(cmd_ptr->store_esn.esn), UIM_RUIM_UIMID_SIZE);
        }


        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);
      }
      break;

#ifdef FEATURE_AUTH
    case UIM_RUIM_UPDATE_SSD_ST:                   /* Update SSD State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = UPDATE_SSD;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;

        /* form the data stream */
        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        b_packq(cmd_ptr->ssd_update.randssd,
                uim_cdma_req_buf_ptr->req.data,
                pos,
                FSIZ(uim_update_ssd_req_siz_type, randssd));

        pos+= FSIZ(uim_update_ssd_req_siz_type, randssd);

        b_packb(cmd_ptr->ssd_update.process_control,
                uim_cdma_req_buf_ptr->req.data,
                pos,
                FSIZ(uim_update_ssd_req_siz_type, proc_control));

        pos+= FSIZ(uim_update_ssd_req_siz_type, proc_control);

#ifdef FEATURE_RUIM_DEBUG_CAVE_ESN
        b_packd(cmd_ptr->ssd_update.esn, uim_cdma_req_buf_ptr->req.data, pos,
                FSIZ(uim_update_ssd_req_siz_type, esn));
#else
        memcpy( uim_cdma_req_buf_ptr->req.data + (pos/8),
                (byte*) &(cmd_ptr->ssd_update.esn), 4);
#endif /* FEATURE_RUIM_DEBUG_CAVE_ESN */

        pos+= FSIZ(uim_update_ssd_req_siz_type, esn);

        /* Pad 2 bytes with zeroes */

        b_packw( 0, uim_cdma_req_buf_ptr->req.data, pos,
               FSIZ(uim_update_ssd_req_siz_type, esn_pad1));

        pos+= FSIZ(uim_update_ssd_req_siz_type, esn_pad1);

        /* Pad 1 byte with zero */
        b_packb( 0, uim_cdma_req_buf_ptr->req.data, pos,
               FSIZ(uim_update_ssd_req_siz_type, esn_pad2));

        pos+= FSIZ(uim_update_ssd_req_siz_type, esn_pad2);

        uim_cdma_req_buf_ptr->apdu_hdr.p3 = (byte) pos / 8;
        /* bytes of data sent in the command */

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_UPDATE_SSD_ST */
      break;

    case UIM_RUIM_CONFIRM_SSD_ST:                  /* Confirm SSD State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = CONFIRM_SSD;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;

        /* form the data stream */
        b_packd(cmd_ptr->confirm_ssd.authbs, uim_cdma_req_buf_ptr->req.data,
                pos, FSIZ(uim_conf_ssd_req_siz_type, authbs));
        pos+= FSIZ(uim_conf_ssd_req_siz_type, authbs);

        uim_cdma_req_buf_ptr->apdu_hdr.p3 = (byte) pos/8;
        /* bytes of data sent in the command */

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_CONFIRM_SSD_ST */
      break;

    case UIM_RUIM_GENERATE_KEY_ST:                 /* Generate KEY State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = GEN_KEY_VPM;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        /* bytes of data sent in the command */
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 0x2;

        uim_cdma_req_buf_ptr->req.data[0] = cmd_ptr->gen_keys.vpm_first_octet;
        uim_cdma_req_buf_ptr->req.data[1] = cmd_ptr->gen_keys.vpm_last_octet;

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_GENERATE_KEY_ST */
      break;

    case UIM_RUIM_RUN_CAVE_ST:                     /* Run CAVE State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = RUN_CAVE;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        /* bytes of data sent in the command */
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 0x11;

        /* form the data stream */
        b_packb(cmd_ptr->run_cave.rand_type,
                uim_cdma_req_buf_ptr->req.data,
                pos,
                FSIZ(uim_run_cave_req_siz_type, rand_type));

        pos+= FSIZ(uim_run_cave_req_siz_type, rand_type);

        if (cmd_ptr->run_cave.rand_type == UIM_UNIQUE_RAND_CHAL)
        {
            pos += 8;

            b_packd(cmd_ptr->run_cave.rand_chal,
                    uim_cdma_req_buf_ptr->req.data,
                    pos,
                    FSIZ(uim_run_cave_req_siz_type, randu));

            pos+= FSIZ(uim_run_cave_req_siz_type, randu);
        }
        else
        {
            b_packd(cmd_ptr->run_cave.rand_chal,
                    uim_cdma_req_buf_ptr->req.data,
                    pos,
                    FSIZ(uim_run_cave_req_siz_type, rand));
            pos+= FSIZ(uim_run_cave_req_siz_type, rand);
        }

        b_packb(cmd_ptr->run_cave.dig_len, uim_cdma_req_buf_ptr->req.data, pos,
                FSIZ(uim_run_cave_req_siz_type, dig_len));

        pos+= FSIZ(uim_run_cave_req_siz_type, dig_len);

        for (i = 0; i < UIM_RUN_CAVE_DIGITS ; i++)
        {
            b_packb(cmd_ptr->run_cave.digits[i],
                    uim_cdma_req_buf_ptr->req.data,
                    pos,
                    FSIZ(uim_run_cave_req_siz_type, digits));
            pos += FSIZ(uim_run_cave_req_siz_type, digits);
        }

        b_packb(cmd_ptr->run_cave.process_control,
                uim_cdma_req_buf_ptr->req.data,
                pos,
                FSIZ(uim_run_cave_req_siz_type, proc_control));

        pos+= FSIZ(uim_run_cave_req_siz_type, proc_control);

#ifdef FEATURE_RUIM_DEBUG_CAVE_ESN

        b_packd(cmd_ptr->run_cave.esn, uim_cdma_req_buf_ptr->req.data, pos,
                FSIZ(uim_run_cave_req_siz_type, esn));
#else
        memcpy( uim_cdma_req_buf_ptr->req.data + (pos/8),
                                      (byte*) (cmd_ptr->run_cave.esn), 4);

#endif /* FEATURE_RUIM_DEBUG_CAVE_ESN */

        pos+= FSIZ(uim_run_cave_req_siz_type, esn);

        /* Pad 2 bytes with zeroes */
        b_packw( 0, uim_cdma_req_buf_ptr->req.data, pos,
               FSIZ(uim_run_cave_req_siz_type, esn_pad1));

        pos+= FSIZ(uim_run_cave_req_siz_type, esn_pad1);

        /* Pad 1 byte with zeroes */
        b_packb( 0, uim_cdma_req_buf_ptr->req.data, pos,
               FSIZ(uim_run_cave_req_siz_type, esn_pad2));

        pos+= FSIZ(uim_run_cave_req_siz_type, esn_pad2);

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_RUN_CAVE_ST */
      break;

    case UIM_RUIM_BS_CHALLENGE_ST:                 /* BS Challenge State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = BS_CHALLENGE;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;

        /* form the data stream */
        b_packd(cmd_ptr->bs_chal.randseed, uim_cdma_req_buf_ptr->req.data, pos,
                FSIZ(uim_bs_chal_req_siz_type, rands));

        pos+= FSIZ(uim_bs_chal_req_siz_type, rands);

        uim_cdma_req_buf_ptr->apdu_hdr.p3 = (byte) pos/8;
        /* bytes of data sent in the command */

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_BS_CHALLENGE_ST */
      break;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_OTASP
    case UIM_RUIM_MS_KEY_REQ_ST:                   /* MS Key Request State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = MS_KEY_REQUEST;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 0x6B;

        memcpy((byte *)uim_cdma_req_buf_ptr->req.ms_key.randseed,
                cmd_ptr->ms_key_req.randseed, UIM_RAND_SEED);

        uim_cdma_req_buf_ptr->req.ms_key.a_key_p_rev =
          cmd_ptr->ms_key_req.a_key_p_rev;

        uim_cdma_req_buf_ptr->req.ms_key.param_p_len =
          cmd_ptr->ms_key_req.param_p_len;

        uim_cdma_req_buf_ptr->req.ms_key.param_g_len =
          cmd_ptr->ms_key_req.param_g_len;

        memcpy((byte *) uim_cdma_req_buf_ptr->req.ms_key.param_p,
                cmd_ptr->ms_key_req.param_p, UIM_PARAM_P);

        memcpy((byte *) uim_cdma_req_buf_ptr->req.ms_key.param_g,
                cmd_ptr->ms_key_req.param_g, UIM_PARAM_G);

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_MS_KEY_REQ_ST */
      break;

    case UIM_RUIM_KEY_GEN_REQ_ST:  /* Key Generation Request State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = KEY_GEN_REQUEST;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->key_gen.bs_result_len + 1;

        uim_cdma_req_buf_ptr->req.key_gen.bs_result_len =
                                     cmd_ptr->key_gen.bs_result_len;

        memcpy((byte *) uim_cdma_req_buf_ptr->req.key_gen.bs_result,
                cmd_ptr->key_gen.bs_result, cmd_ptr->key_gen.bs_result_len);

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_KEY_GEN_REQ_ST */
      break;

    case UIM_RUIM_CFG_REQ_ST:  /* Configuration Request State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = CONFIG_REQUEST;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 1;

        uim_cdma_req_buf_ptr->req.config_req.block_id =
          cmd_ptr->configuration.block_id;

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_CFG_REQ_ST */
      break;

    case UIM_RUIM_DLOAD_REQ_ST:                    /* Download Request State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = DOWNLOAD_REQUEST;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->download.block_len + 2;

        uim_cdma_req_buf_ptr->req.download_req.block_id =
          cmd_ptr->download.block_id;

        uim_cdma_req_buf_ptr->req.download_req.block_len =
          cmd_ptr->download.block_len;

        memcpy((byte *) uim_cdma_req_buf_ptr->req.download_req.data,
                cmd_ptr->download.data,
                cmd_ptr->download.block_len);

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_DLOAD_REQ_ST */
      break;

    case UIM_RUIM_SSPR_CFG_REQ_ST:  /* SSPR Config Request State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = SSPR_CONFIG_REQUEST;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 4;

        uim_cdma_req_buf_ptr->req.sspr_cfg_req.block_id =
                            cmd_ptr->sspr_config.block_id;

        FLOPW (uim_cdma_req_buf_ptr->req.sspr_cfg_req.req_offset ,
                            cmd_ptr->sspr_config.req_offset );

        uim_cdma_req_buf_ptr->req.sspr_cfg_req.req_max_size =
                            cmd_ptr->sspr_config.req_max_size;

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_SSPR_CFG_REQ_ST */
      break;

    case UIM_RUIM_SSPR_DLOAD_REQ_ST:  /* SSPR Download Request State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = SSPR_DOWNLOAD_REQUEST;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;

        uim_cdma_req_buf_ptr->req.sspr_download_req.block_id =
                          cmd_ptr->sspr_download.block_id;

        uim_cdma_req_buf_ptr->req.sspr_download_req.block_len =
                          cmd_ptr->sspr_download.block_len;

        uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                          cmd_ptr->sspr_download.block_len + 2 ;

        /* Copy the contents of the data from the command */
        memcpy((byte *) uim_cdma_req_buf_ptr->req.sspr_download_req.data,
               cmd_ptr->sspr_download.data, cmd_ptr->sspr_download.block_len);

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_SSPR_DLOAD_REQ_ST */
      break;

    case UIM_RUIM_OTAPA_REQ_ST:                    /* OTAPA Request State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = OTAPA_REQUEST;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 5;

        uim_cdma_req_buf_ptr->req.otapa_req.start_stop =
          cmd_ptr->otapa_req.start_stop;

        uim_cdma_req_buf_ptr->req.otapa_req.rand_seed =
          cmd_ptr->otapa_req.randseed;

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_OTAPA_REQ_ST */
      break;

    case UIM_RUIM_COMMIT_ST:                       /* Commit State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = COMMIT;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = 1;

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_COMMIT_ST */
      break;

    case UIM_RUIM_VALIDATE_ST:                     /* Validation State */
      {
        uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_cdma_req_buf_ptr->apdu_hdr.instrn = VALIDATE;
        uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
        uim_cdma_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_cdma_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->validate.block_length + 2;

        uim_cdma_req_buf_ptr->req.validate.block_id =
          cmd_ptr->validate.block_id;

        uim_cdma_req_buf_ptr->req.validate.block_len =
          cmd_ptr->validate.block_length;

        memcpy((byte *) uim_cdma_req_buf_ptr->req.validate.data,
               cmd_ptr->validate.data, cmd_ptr->validate.block_length);

        uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_cdma_req_buf_ptr);

      } /* end case - UIM_RUIM_VALIDATE_ST */
      break;
#endif /* FEATURE_OTASP */

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
    case UIM_RUIM_COMPUTE_IP_AUTH_ST:
    {
      uim_cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS2;
      uim_cdma_req_buf_ptr->apdu_hdr.instrn = COMPUTE_IP_AUTHENTICATION;

      switch(cmd_ptr->compute_ip_auth.cmpt_ip_operation)
      {
#ifdef FEATURE_UIM_SUPPORT_3GPD
        case UIM_CMPT_IP_CHAP:
        {
          uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
          uim_cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_CHAP;
          uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
          uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.
                         chap_challenge_length + 2;
          uim_cdma_req_buf_ptr->req.data[0] =
                cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_id;
          uim_cdma_req_buf_ptr->req.data[1] =
                cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.nai_entry_index;
          memcpy(&(uim_cdma_req_buf_ptr->req.data[2]),
            cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge,
            cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.
                     chap_challenge_length);
        }  /* end case - UIM_CMPT_IP_CHAP */
        break;

#ifdef FEATURE_UIM_3GPD_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP
        case UIM_CMPT_IP_MN_HA_AUTH:
        {
          number_bytes_sent = uim_compute_ip_auth_num_blocks_sent *
                                UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE;
          uim_cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_MN_HA;


          if(uim_compute_ip_auth_num_blocks_sent == 0)
          {
            /* This is the first or only block for this command */
            if(cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                registration_data_length > UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE)
            {
              /* Multiple (chained) blocks are required */
              uim_compute_ip_auth_more_blocks_req = TRUE;
              uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_FIRST_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                                          UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE+1;
              uim_cdma_req_buf_ptr->req.data[0] =
               cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.nai_entry_index;
              memcpy(&(uim_cdma_req_buf_ptr->req.data[1]),
                 cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                     registration_data, UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE);
            } /* end if - Multiple APDUs are required */
            else
            {
              /* Only one APDU is required */
              uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_SNGL_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                         registration_data_length + 1;
              uim_cdma_req_buf_ptr->req.data[0] =
               cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.nai_entry_index;
              memcpy(&(uim_cdma_req_buf_ptr->req.data[1]),
                 cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                          registration_data,
                 cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                          registration_data_length);
            } /* end else - only one APDU required */
          } /* end if - It's the first or only block to be sent */
          else
          {
            /* This is an intermediate or last block of the command */
            number_bytes_left=cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                                registration_data_length - number_bytes_sent;
            compute_ip_data_ptr =
                &(cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                           registration_data[number_bytes_sent]);
            if(number_bytes_left > UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE)
            {
              /* This is not the last block - it is an intermediate block */
              uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_NEXT_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                                           UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE;
              memcpy(uim_cdma_req_buf_ptr->req.data, compute_ip_data_ptr,
                                          UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE);
            } /* end if - Intermediate Block */
            else if(number_bytes_left > 0)
            {
              /* This is the last block */
              uim_compute_ip_auth_more_blocks_req = FALSE;
              uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_LAST_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 = number_bytes_left;
              memcpy(uim_cdma_req_buf_ptr->req.data, compute_ip_data_ptr,
                     number_bytes_left);
            } /* end else if - last block */
            else {
              /* Should not get here - No more blocks needed for MN-HA */
              uim_compute_ip_auth_more_blocks_req = FALSE;
              MSG_HIGH("Err-No more blks to send in RUIM_CMP_IP_AUTH ST",0,0,0);
            } /* end else - no more blocks to send */
          } /* end else - intermediate or last block */
        } /* end case - UIM_CMPT_IP_MN_HA_AUTH */
        break;

        case UIM_CMPT_IP_MIP_RRQ:
        {
          preceding_rrq_length = cmd_ptr->compute_ip_auth.cmpt_ip_data.
              mip_rrq_data.rrq_data_length - UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH;
          number_bytes_sent = uim_compute_ip_auth_num_blocks_sent *
                    UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE;
          uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
          uim_cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_MIP_RRQ;

          /* In the MIP-RRQ command, the data length can be up to 255 bytes for
             each block.  The last 8 bytes of the last block must be the MN-AAA
             Extention header.  In order to simplify this software, and to
             ensure that the MN-AAA Extention header does not get split up, each
             block only sends at most 247 bytes of registration data.  The last
             block can then insert the 8 byte MN-AAA Extension header after the
             registration data without fear that there will not be room for the
             entire header */

          if(uim_compute_ip_auth_num_blocks_sent == 0)
          {
            /* This is the first or only block for this command */
            if(preceding_rrq_length > UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE)
            {
              /* Multiple (chained) blocks are required */
              uim_compute_ip_auth_more_blocks_req = TRUE;
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_FIRST_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                                             UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE;
              memcpy(uim_cdma_req_buf_ptr->req.data,
                 cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
                 UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE);
            } /* end if - Multiple APDUs are required */
            else
            {
              /* Only one APDU (block) is required */
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_SNGL_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 = preceding_rrq_length +
                                             UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH;
              memcpy(uim_cdma_req_buf_ptr->req.data,
                 cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
                 cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.
                          rrq_data_length);
            } /* end else - only one APDU required */
          } /* end if - It's the first or only block to be sent */
          else
          {
            /* This is an intermediate or last block of the command */
            number_bytes_left = preceding_rrq_length - number_bytes_sent;
            compute_ip_data_ptr =
                &(cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.
                           rrq_data[number_bytes_sent]);
            if(number_bytes_left > UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE)
            {
              /* This is not the last block - it is an intermediate block */
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_NEXT_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                                            UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE;
              memcpy(uim_cdma_req_buf_ptr->req.data, compute_ip_data_ptr,
                     UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE);
            } /* end if - Intermediate Block */
            else if(number_bytes_left > 0)
            {
              /* This is the last block */
              uim_compute_ip_auth_more_blocks_req = FALSE;
              uim_cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_LAST_BLK;
              uim_cdma_req_buf_ptr->apdu_hdr.p3 = number_bytes_left +
                                             UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH;
              memcpy(uim_cdma_req_buf_ptr->req.data, compute_ip_data_ptr,
                     number_bytes_left + UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH);
            } /* end else if - last block */
            else {
              /* Should not get here - No more blocks needed for MIP-RRQ HASH */
              uim_compute_ip_auth_more_blocks_req = FALSE;
              MSG_HIGH("Err-No more blks to send in RUIM_CMP_IP_AUTH ST",0,0,0);
            } /* end else - no more blocks to send */
          } /* end else - intermediate or last block */
        }/* end case - UIM_CMPT_IP_MIP_RRQ */
        break;
        case UIM_CMPT_IP_MN_AAA_AUTH:
        {
          /*--------------------------------------------------------------------
          Calculate the byte offset into challenge where the 237
          least-significant bytes begin
          --------------------------------------------------------------------*/

          challenge_len = cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.
                                   challenge_length;
          MSG_LOW ("Challenge len = %d", challenge_len,0,0);
          if (challenge_len > UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE -1 )
          {
            /* When challenge length is greater than 237 byte, then
               it will send the highest-order byte and 
               least significant 237 bytes to the R-UIM. */		  
            uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                                 UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE + 1;
            /* starting offset of the least significant 237 bytes */	    
            offset = challenge_len -
                               (UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE - 1);
          }
          else
          {
            /* When the challenge has fewer than 238 bytes then
               R-UIM card shall include the high-order byte in the computation twice, 
               but ensures that the challenge is used exactly as is. 
               Here ME only need to send challenge as it is */
            uim_cdma_req_buf_ptr->apdu_hdr.p3 = challenge_len + 1;
            /* First byte is taking separately as highest-order byte */
            offset = 1;
          }
          uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
          uim_cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_MN_AAA;
          uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;

          uim_cdma_req_buf_ptr->req.data[0] =
              cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.nai_entry_index;

          if(challenge_len != 0)
          {
            uim_cdma_req_buf_ptr->req.data[1] =
             cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge[0];
            memcpy(&(uim_cdma_req_buf_ptr->req.data[2]),
                 &(cmd_ptr->compute_ip_auth.cmpt_ip_data.
                   mn_aaa_data.challenge[offset]), challenge_len - offset);
          }
        }/* end case - UIM_CMPT_IP_MN_AAA_AUTH */
        break;
#endif /*FEATURE_DS_MOBILE_IP*/
#endif /* FEATURE_UIM_3GPD_MOBILE_IP */
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
        case UIM_CMPT_IP_HRPD_AN_CHAP:
        {
          uim_cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
          uim_cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_HRPD;
          uim_cdma_req_buf_ptr->apdu_hdr.p2 = 0;
          uim_cdma_req_buf_ptr->apdu_hdr.p3 =
                cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.
                         chap_challenge_length + 1;
          uim_cdma_req_buf_ptr->req.data[0] =
                cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_id;
          memcpy(&(uim_cdma_req_buf_ptr->req.data[1]),
            cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge,
            cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.
                     chap_challenge_length);
        }  /* end case - UIM_CMPT_IP_HRPD_AN_CHAP */
        break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */
      } /* end of Compute IP Auth operation type switch */

      uim_cdma_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
      uim_cdma_req_buf_ptr->rpt_function = uim_command_response_callback;
      ++uim_compute_ip_auth_num_blocks_sent;
      MSG_LOW("Nmbr of Blks Sent: %d",uim_compute_ip_auth_num_blocks_sent,0,0);
      uim_send_command(uim_cdma_req_buf_ptr);

    } /* end case - UIM_RUIM_COMPUTE_IP_AUTH_ST */
    break;

#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */

    case UIM_RUIM_DONE_ST:                         /* Done State */
      {
        /* Process the response of the command. */
        uim_process_cdma_response(uim_rsp_buf_ptr, cmd_ptr);
      } /* end case - UIM_RUIM_DONE_ST */
      break;

    default:

      MSG_HIGH("Invalid state in uim_cdma_command %ld",
               *uim_cdma_state_ptr, 0,0);
      break;
  } /* end of main switch */

  MSG_LOW("UIM state in uim_command %ld",*uim_cdma_state_ptr, 0,0);

} /* uim_cdma_command */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CDMA_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_cdma_state_ptr.

===========================================================================*/

uim_cmd_status_type uim_cdma_command_response
(
 uim_rsp_buf_type const *rsp_ptr,
 uim_cmd_type     *cmd
)
{
  uim_cmd_status_type status; /* Status of response processing. */
  uim_cdma_state_type curr_uim_cdma_state; /* Holds initial cdma state */
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  /* Assume the command response is proper. */
  status = UIM_CMD_SUCCESS;

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
  curr_uim_cdma_state = *uim_cdma_state_ptr;

  switch (curr_uim_cdma_state)
  {
    case UIM_RUIM_STORE_ESN_ME_ST:        /* Store ESN_ME state */
#ifdef FEATURE_UIM_EUIMID
    case UIM_RUIM_STORE_ESN_MEID_ME_ST:   /* Store ESN_MEID_ME state */
#endif /* FEATURE_UIM_EUIMID */
#ifdef FEATURE_AUTH
    case UIM_RUIM_UPDATE_SSD_ST:          /* Update SSD State */
    case UIM_RUIM_CONFIRM_SSD_ST:         /* Confirm SSD State */
    case UIM_RUIM_GENERATE_KEY_ST:        /* Generate KEY State */
    case UIM_RUIM_RUN_CAVE_ST:            /* Run CAVE State */
    case UIM_RUIM_BS_CHALLENGE_ST:        /* BS Challenge State */
#endif /* FEATURE_AUTH */
#ifdef FEATURE_OTASP
    case UIM_RUIM_MS_KEY_REQ_ST:          /* MS Key Request State */
    case UIM_RUIM_KEY_GEN_REQ_ST:         /* Key Generation Request State */
    case UIM_RUIM_CFG_REQ_ST:             /* Configuration Request State */
    case UIM_RUIM_DLOAD_REQ_ST:           /* Download Request State */
    case UIM_RUIM_SSPR_CFG_REQ_ST:        /* SSPR Config Request State */
    case UIM_RUIM_SSPR_DLOAD_REQ_ST:      /* SSPR Download Request State */
    case UIM_RUIM_OTAPA_REQ_ST:           /* OTAPA Request State */
    case UIM_RUIM_COMMIT_ST:              /* Commit State */
    case UIM_RUIM_VALIDATE_ST:            /* Validation State */
#endif /* FEATURE_OTASP */
      {
        /* Determine if the UIM command responded with a problem. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Indicate a problem with the command. */
          return (UIM_CMD_ERR);
        }
        else
        /* The command completed successfully */
        {
          /* Get the next state. */
          ++uim_cdma_state_ptr;
        } /* end if - the UIM command responded with a problem. */
      } /* end case */
      break;

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
    case UIM_RUIM_COMPUTE_IP_AUTH_ST:
      {
        /* Determine if the UIM command responded with a problem. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Indicate a problem with the command. */
          return (UIM_CMD_ERR);
        }
        else if (!uim_compute_ip_auth_more_blocks_req)
        /* The command completed successfully and is complete.
           Otherwise, the command requires another block, repeat the state */
        {
          /* Get the next state. */
          ++uim_cdma_state_ptr;
        } /* end if - the UIM command responded with a problem. */
      } /* end case */
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN*/

    case UIM_RUIM_DONE_ST:                         /* Done State */
      {
        /* Do nothing */
      } /* end case - UIM_RUIM_DONE_ST */
      break;

    default:

      MSG_HIGH( "Invalid State in uim_cdma_command_response ", 0, 0, 0 );
      status = UIM_CMD_ERR;
      break;

  } /* end switch - curr_uim_cdma_state */

  /* Continue processing the command only if the response indicates success. */
  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of this command. */
    uim_cdma_command(cmd);
  } /* end if - command is still in progress */

  /* Determine if the command has completed */
  if (UIM_RUIM_DONE_ST == *uim_cdma_state_ptr)
  {
    /* Indicate the command has completed successfully. */
    return (completed_status);
  } /* end if - uim_cdma_state_ptr is UIM_RUIM_DONE_ST */

  return(status);

} /* uim_cdma_command_response */
#endif /* FEATURE_UIM && FEATURE_UIM_RUIM */
