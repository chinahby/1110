#ifndef UIMCDMA_H
#define UIMCDMA_H
/*===========================================================================

        E X T E R N A L   U I M  C D M A   S T A T E   M A C H I N E
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM cdma state machine.

Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uimcdma.h_v   1.1   19 Dec 2001 15:27:24   ckrishna  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimcdma.h#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/07   nk      Added EUIMID support
04/03/06   pv      Lint Fixes
08/27/04   jk      Added support for HRPD AN Authentication services in R-UIM
12/19/03   jk      Added processing for R-UIM 3GPD Services.
02/20/03   ck      Removed the UIM_INSTRUCTION_CLASS defintion as it was already
                   defined in uimi.h
12/19/01   ck      Deleted the commented out definitions.
03/19/01   ts      Created module.  This module contains the cdma state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#ifdef FEATURE_UIM
#include "comdef.h"
#include "uimi.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIM_MAX_NUMBER_CDMA_STATES 2

#define UIM_RUIM_UIMID_SIZE        4
#define UIM_RUIM_UIMID_OFFSET      4

typedef enum {
  UIM_RUIM_STORE_ESN_ME_ST,                 /* Store ESN_ME state */
#ifdef FEATURE_AUTH
  UIM_RUIM_UPDATE_SSD_ST,                   /* Update SSD State */
  UIM_RUIM_CONFIRM_SSD_ST,                  /* Confirm SSD State */
  UIM_RUIM_GENERATE_KEY_ST,                 /* Generate KEY State */
  UIM_RUIM_RUN_CAVE_ST,                     /* Run CAVE State */
  UIM_RUIM_BS_CHALLENGE_ST,                 /* BS Challenge State */
#endif /* FEATURE_AUTH */
#ifdef FEATURE_OTASP
  UIM_RUIM_MS_KEY_REQ_ST,                   /* MS Key Request State */
  UIM_RUIM_KEY_GEN_REQ_ST,                  /* Key Generation Request State */
  UIM_RUIM_CFG_REQ_ST,                      /* Configuration Request State */
  UIM_RUIM_DLOAD_REQ_ST,                    /* Download Request State */
  UIM_RUIM_SSPR_CFG_REQ_ST,                 /* SSPR Config Request State */
  UIM_RUIM_SSPR_DLOAD_REQ_ST,               /* SSPR Download Request State */
  UIM_RUIM_OTAPA_REQ_ST,                    /* OTAPA Request State */
  UIM_RUIM_COMMIT_ST,                       /* Commit State */
  UIM_RUIM_VALIDATE_ST,                     /* Validation State */
#endif /* FEATURE_OTASP */
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
  UIM_RUIM_COMPUTE_IP_AUTH_ST,              /* Compute IP Auth State */
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */
#ifdef FEATURE_UIM_EUIMID                  
  UIM_RUIM_STORE_ESN_MEID_ME_ST,            /* Store ESN_MEID_ME state */
#endif /* FEATURE_UIM_EUIMID */
  UIM_RUIM_DONE_ST                          /* Done State */
} uim_cdma_state_type;

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_CDMA_ERR_RPT

DESCRIPTION
  This function sends an error report for a CDMA command that is being
  requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
extern void uim_send_cdma_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_CDMA_COMMAND

DESCRIPTION
  This procedure processes a CDMA command sent to the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM CDMA state variable amongst others.

===========================================================================*/

extern boolean uim_process_cdma_command
(
  uim_cmd_type *cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CDMA_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a CDMA comamnd that has been
  received from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_cdma_state.

===========================================================================*/

uim_cmd_status_type uim_cdma_command_response
(
 uim_rsp_buf_type const *rsp_ptr,
 uim_cmd_type     *cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CDMA_COMMAND

DESCRIPTION
  This procedure is the main state machine for CDMA UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

extern void uim_cdma_command
(
  uim_cmd_type *cmd_ptr
);


/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_CDMA_CMD_STATE

DESCRIPTION
  This procedure sets the CDMA UIM state and is called during custom command
  processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the state of uim_ruim_state variable in auth task.

===========================================================================*/

extern void uim_set_cdma_cmd_state
(
  uim_cdma_state_type state
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_CDMA_SEL_ST_CTR

DESCRIPTION
  This procedure sets the RUIM Select state Counter and is called during
  custom command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the value of select_state_ctr variable in auth task.

===========================================================================*/

extern void uim_set_cdma_sel_st_ctr
(
  byte sel_st_ctr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_CDMA_CMD_PROCESSOR

DESCRIPTION
  This procedure processes the custom CDMA commands( implemented by the customers)
  sent to the Authentication task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void uim_custom_cdma_cmd_processor
(
  uim_cmd_type *cmd_ptr,
    /* Pointer to received command */
  uim_req_buf_type *req_ptr,
    /* Pointer to request buffer */
  uim_rsp_buf_type *rsp_ptr,
    /* Pointer to response buffer */
  uim_resp_callback_type callback_fn
  /* Callback function pointer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_CDMA_COMMAND

DESCRIPTION
  This procedure is the state machine for the custom CDMA RUIM commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void uim_custom_cdma_command
(
  uim_cmd_type *cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_CDMA_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the RUIM.  This
  procedure is used for custom CDMA command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern uim_cmd_status_type uim_custom_cdma_command_response
(
 uim_rsp_buf_type *rsp_ptr,
 uim_cmd_type     *cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_PROCESS_CDMA_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the RUIM
  at the completion of the command processing for a custom CDMA response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void uim_custom_process_cdma_response
(
  uim_rsp_buf_type *rsp_ptr,
  uim_cmd_type *cmd_ptr
);

/*========================================================================

FUNCTION UIM_CUSTOM_SEND_CDMA_ERR_RPT

DESCRIPTION
  This function sends an error report for the custom CDMA command that is being
  requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_custom_send_cdma_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
);

#endif /* FEATURE_UIM */
#endif /* UIMCDMA_H */

