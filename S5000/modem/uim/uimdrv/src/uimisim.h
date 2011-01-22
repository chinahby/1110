#ifndef UIMISIM_H
#define UIMISIM_H
/*===========================================================================

           I N T E R N A L   U S I M    S T A T E   M A C H I N E
                         
                      D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the ISIM state machine.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uimisim.h_v   1.0   25 May 2001 17:55:08   ckrishna  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimisim.h#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/06   pv      Lint Fixes
05/16/05   pv      Created module.  This module contains the isim state 
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_ISIM)
#include "comdef.h"

/* Maximum number of states transitioned while processing a ISIM command */
#define UIM_MAX_NUMBER_ISIM_STATES  2

#define UIM_ISIM_AUTN_SPECIFIC_REF_DATA_P2 0x80

#define UIM_ISIM_AUTN_REF_DATA_NUMBER_MASK 0x1

typedef enum {
  UIM_ISIM_AUTHENTICATE_ST,              /* Authenticate state */
  UIM_ISIM_DONE_ST                       /* Done State */    
} uim_isim_state_type;

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_ISIM_ERR_RPT

DESCRIPTION
  This function sends an error report for a isim command that is being 
  requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer. 

===========================================================================*/

extern void uim_send_isim_err_rpt 
(
  uim_cmd_type *cmd_ptr
);

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

extern boolean uim_process_isim_command
(
  uim_cmd_type *cmd_ptr
    /* Pointer to received command */
);

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
);

#endif /* FEATURE_UIM && FEATURE_UIM_ISIM */
#endif /* UIMISIM_H */
