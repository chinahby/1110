#ifndef OEMPBM_H
#define OEMPBM_H
/*=============================================================================

FILE: OEMPBM.h
 
SERVICES: OEM PBM Interface 
 
GENERAL DESCRIPTION:
  Definitions to support syncing PBM.
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMPBM.h#1 $
$DateTime: 2007/12/03 13:25:30 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/07   clm     Create file.

=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEEComdef.h"
#include "AEECallHistory.h"
#include "AEEDB.h"

/*===========================================================================

                       AEE PBM FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: OEMPBM_AddEntry

DESCRIPTION:
   Translate and add AEE Call History Entry to pbm.

PARAMETERS:
   pEntry

RETURN VALUE:
   BREW Error Code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_AddEntry(const AEECallHistoryEntry *pEntry);

/*=============================================================================
FUNCTION: OEMPBM_Clear

DESCRIPTION:
   Clears pbm call history.

PARAMETERS:
   None

RETURN VALUE:
   BREW Error Code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_Clear(void);

/*=============================================================================
FUNCTION: OEMPBM_VerifyNumber

DESCRIPTION:
   Verifies if a number can be in a NUMBER item.

PARAMETERS:
   None

RETURN VALUE:
   BREW Error Code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_VerifyNumber(byte ucChar);

/*=============================================================================
FUNCTION: OEMPBM_VerifyNumberEx

DESCRIPTION:
   Verifies if a character can be in a NUMBER_EX item.

PARAMETERS:
   None

RETURN VALUE:
   BREW Error Code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_VerifyNumberEx(uint16 usChar);

#endif //OEMPBM_H

