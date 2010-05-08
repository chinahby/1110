/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   G S M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the gsm protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_gsm_command
    Processes a gsm UIM server command

  uim_process_gsm_response
    Processes the responses for a gsm UIM server command

  uim_send_gsm_err_rpt
    Processes an error condition for a gsm UIM server command

  uim_gsm_command
    This function contains the state machine that processes APDUs
    for gsm commands.

  uim_gsm_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2001-2006, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uimgsm.c_v   1.3   19 Dec 2001 15:28:38   ckrishna  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimgsm.c#2 $ $DateTime: 2009/05/11 12:03:29 $ $Author: shanmugh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/09   ps      Fixed 8200 compiler warnings
04/03/06   pv      Lint Fixes
11/14/05   pv      Lint Fixes
04/28/05   jk      Added support to properly transfer the user data from the 
                   incoming command to the report
02/24/05   tml     Populate user data for Run GSM Algorithm command
05/17/04   ck      Moved the call to uim_send_gsm_err_rpt() from default case 
                   in the function uim_process_gsm_command() to the end of the
                   function based on tbe status flag.
02/20/04   ts      Fixed lint errors
03/28/03   ts      Replaced ME_TO_UIM and UIM_TO_ME with instruction case in
                   UIM driver commands.
02/02/03   ck      Lint changes.
10/24/02   ts      Remove response timeout processing as timeout is handled
                   in the driver.
12/18/01   ck      Added the include file memory.h
12/04/01   ts      Added support for UIM toolkit.
05/25/01   ck      Added support for Multi mode server.
01/29/01   ck      Created module.  This module contains the gsm state
                   machine.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_GSM)
#error code not present
#endif /* FEATURE_UIM && FEATURE_UIM_GSM */
