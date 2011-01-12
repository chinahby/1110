/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   U S I M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the USIM protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_usim_command
    Processes a usim UIM server command

  uim_process_usim_response
    Processes the responses for a usim UIM server command

  uim_send_usim_err_rpt
    Processes an error condition for a usim UIM server command

  uim_usim_command
    This function contains the state machine that processes APDUs
    for usim commands.

  uim_usim_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2001-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uimusim.c_v   1.1   04 Dec 2001 15:10:38   tsummers  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimusim.c#4 $ $DateTime: 2009/05/04 18:36:44 $ $Author: shanmugh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/09   ps      Fix 8200 Warnings
03/09/09   sun     Fixed Merge issue
01/12/09   sun     Added a new state - UIM_SELECT_ADF_ST before ODD INS
                   auth
09/18/08   nb      Lint Fixes
03/03/08   sun     Added support for OMA BCAST
02/25/08   sun     Do not change state if the command fails.
08/14/07   sun     Fixed Report Type for ODD_INS
07/05/07   sp      Send out a status command before each USIM authenticate.
07/02/07   sun     Added support for ODD INS Class 
04/03/06   pv      Lint Fixes
05/10/05   sst     Lint fixes 
04/28/05   jk      Added support to properly transfer the user data from the 
                   incoming command to the report
02/24/05   tml     Populate user data for Authentication command
05/17/04   ck      Moved the call to uim_send_usim_err_rpt() from default case 
                   in the function uim_process_usim_command() to the end of the
                   function based on tbe status flag.
03/28/03   ts      Replaced ME_TO_UIM and UIM_TO_ME with instruction case in
                   UIM driver commands.
02/02/03   ck      Lint mods.
01/17/03   ck      Included comdef.h and memory.h to avoid compiler warnings
10/24/02   ts      Removed response timeout processing as timeouts are
                   handled in the driver.
02/29/01   ck      Created module.  This module contains the USIM state
                   machine.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_USIM)
#error code not present
#endif /* FEATURE_UIM && FEATURE_UIM_USIM */

