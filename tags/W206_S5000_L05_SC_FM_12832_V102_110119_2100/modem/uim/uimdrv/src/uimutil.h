#ifndef UIMUTIL_H
#define UIMUTIL_H
/*===========================================================================

        E X T E R N A L   U I M  U T I L   S T A T E   M A C H I N E
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM UTIL state machine.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
******************************************     UPDATE THIS
$PVCSPath: O:/src/asw/COMMON/vcs/uimwim.h_v   1.1    Dec 2001 15:27:24   ckrishna  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimutil.h#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/06   pv      Lint Fixes
02/10/03   jk      Created module.  This module contains the util state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#ifdef FEATURE_UIM_UTIL
#include "comdef.h"
#include "uimi.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


#define UIM_MAX_NUMBER_UTIL_STATES 2
#define UIM_MAX_LOGICAL_CHANNELS 4
#define UIM_NO_LOGICAL_CHANNEL_SET 0xFF

typedef enum {
  UIM_UTIL_VERIFY_IN_CHAN_ST,                        /* Store Verify State */
  UIM_UTIL_DISABLE_VER_IN_CHAN_ST,                   /* Disable Verification State */
  UIM_UTIL_ENABLE_VER_IN_CHAN_ST,                    /* Enable Verification State */
  UIM_UTIL_CHANGE_REF_IN_CHAN_ST,                    /* Change Reference Data State */
  UIM_UTIL_RESET_RETRY_IN_CHAN_ST,                   /* Reset Retry State */
  UIM_UTIL_ACCESS_IN_CHAN_ST,                 /* Access in Channel */
  UIM_UTIL_SELECT_IN_CHAN_ST,                /* Select in Channel */
  UIM_UTIL_MANAGE_CHANNEL_ST,
#ifdef FEATURE_UIM_WIM
  UIM_UTIL_MSE_ST,                            /* MSE State */
  UIM_UTIL_PSO_ST,                            /* PSO State */
  UIM_UTIL_ASK_RANDOM_ST,                    /* Ask Random State */
#endif /* FEATURE_UIM_WIM */
  UIM_UTIL_DONE_ST                           /* Done State */
  } uim_util_state_type;

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_UTIL_ERR_RPT

DESCRIPTION
  This function sends an error report for a UTIL command that is being
  requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
extern void uim_send_util_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_UTIL_COMMAND

DESCRIPTION
  This procedure processes a UTIL command sent to the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM UTIL state variable amongst others.

===========================================================================*/

extern boolean uim_process_util_command
(
  uim_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION UIM_UTIL_COMMAND

DESCRIPTION
  This procedure is the main state machine for UTIL UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

extern void uim_util_command
(
  uim_cmd_type *cmd_ptr
);



/*===========================================================================

FUNCTION UIM_INIT_LOGICAL_CHANNEL_TABLE

DESCRIPTION
 This function is called by uim_init, and initializes the logical channel table
 so that all logical channels (1-3) are marked available.  This means that the
 data parts of the AID types are set to 0, the length part is also set to 0

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void uim_init_logical_channel_table
(
 void
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_UTIL_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a UTIL comamnd that has been
  received from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_util_state.

===========================================================================*/

uim_cmd_status_type uim_util_command_response
(
 uim_rsp_buf_type const *rsp_ptr,
 uim_cmd_type     *cmd
);

/* <EJECT> */

#endif /* FEATURE_UIM_UTIL */
#endif /* UIMUTIL_H */
