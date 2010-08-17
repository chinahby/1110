#ifndef OEMPBM_H
#define OEMPBM_H
/*=============================================================================

FILE: OEMPBM.h
 
SERVICES: 
   OEM Phone book manager Interface provide services to add, delete the call 
   history in to OEM phone book. It also provide services to verify call 
   history numbers. 
 
GENERAL DESCRIPTION:
   Definitions to support synching call history to an OEM phone book manager.
 
   FEATURE_SYNCED_CALLHISTORY determines if OEMPBM.h is pulled in or not. If 
   any OEM wanted to define FEATURE_SYNCED_CALLHISTORY, they need to implement
   the functions in OEMPBM.h 

PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2007,2008 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================


=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEEComdef.h"
#include "AEECallHistory.h"

/*===========================================================================

                    Phone book manager FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: OEMPBM_AddEntry

DESCRIPTION:
   Translate and add AEE Call History Entry to an OEM phone book manager.

PARAMETERS:
   pEntry      [in]: Pointer to AEECallHistoryEntry

RETURN VALUE:
   SUCCESS, if successful

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   Otherwise, Brew Error codes.
   
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_AddEntry(const AEECallHistoryEntry *pEntry);

/*=============================================================================
FUNCTION: OEMPBM_Clear

DESCRIPTION:
   Clears call history from an OEM phone book manager.

PARAMETERS:
   None

RETURN VALUE:
   SUCCESS, if successful

   EFAILED, if otherwise unsuccessful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_Clear(void);

/*=============================================================================
FUNCTION: OEMPBM_VerifyNumber

DESCRIPTION:
   Verifies if an ASCII character can be in AEECALLHISTORY_FIELD_NUMBER fields.

PARAMETERS:
   ucChar   [in]: byte to verify

RETURN VALUE:
   SUCCESS, if character is valid for AEECALLHISTORY_FIELD_NUMBER field.

   EFAILED, Otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_VerifyNumber(byte ucChar);

/*=============================================================================
FUNCTION: OEMPBM_VerifyNumberEx

DESCRIPTION:
   Verifies if an ASCII charcter can be in AEECALLHISTORY_FIELD_NUMBER_EX fields.

PARAMETERS:
   usChar   [in]: character to verify

RETURN VALUE:
   SUCCESS, if character is valid for AEECALLHISTORY_FIELD_NUMBER_EX field.

   EFAILED, Otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_VerifyNumberEx(uint16 usChar);

#endif //OEMPBM_H

