/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   M F L O   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the MFLO protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_mflo_command
    Processes a UIM MFLO server command

  uim_send_mflo_err_rpt
    Processes an error condition for a mflo UIM server command

  uim_mflo_command_response
    This function processes the UIM responses from mflo APDUs.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimmflo.c#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/07   tml     Lint fixes
09/04/07   tml     Initial Version

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

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_MFLO)
#error code not present
#endif /* FEATURE_UIM && FEATURE_UIM_MFLO */
