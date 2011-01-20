#ifndef UIMUSIM_H
#define UIMUSIM_H
/*===========================================================================

           I N T E R N A L   U S I M    S T A T E   M A C H I N E
                         
                      D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the USIM state machine.

Copyright (c) 2001,2006-2008 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uimusim.h_v   1.0   25 May 2001 17:55:08   ckrishna  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimusim.h#2 $ $DateTime: 2009/02/20 00:19:56 $ $Author: suberoy $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/09   sun     Added a new state - UIM_SELECT_ADF_ST before ODD INS
                   auth
03/03/08   sun     Added support for OMA BCAST
07/05/07   sp      Add a new state - UIM_STATUS_BEFORE_USIM_AUTH_ST to send
                   out a status command before each usim authenticate command.
                   Increase UIM_MAX_NUMBER_USIM_STATES to accommodate the 
                   new UIM_STATUS_BEFORE_USIM_AUTH_ST state.
07/02/07   sun     Added support for ODD INS Class 
04/03/06   pv      Lint Fixes
01/29/01   ck      Created module.  This module contains the usim state 
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#if defined (FEATURE_UIM) && defined (FEATURE_UIM_USIM)
#error code not present
#endif /* FEATURE_UIM && FEATURE_UIM_USIM */
#endif /* UIMUSIM_H */

