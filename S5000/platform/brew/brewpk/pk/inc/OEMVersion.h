#ifndef OEMVERSION_H
#define OEMVERSION_H
/*****************************************************************************
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

  This file contains the declaration of the function needed by the GETAEEVERSION
  implementation in the AEE Layer.

        Copyright 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*****************************************************************************/

#include "AEE_OEMComdef.h"
#include "BREWVersion.h"

int OEM_GetUpdateVersion(char *pszVersion, int *pnLen);

/*========================================================================
Function: OEM_GetUpdateVersion()
Description:
   Returns the update version information (patches applied).

Prototype:
   int OEM_GetUpdateVersion(char *pszVersion, int *pnLen);

Parameters:
   pszVersion: Valid ptr of size *pnLen on in. Contains the version string on out. 
               If NULL, *pnLen contains the required length on out.
               *pnLen on out does NOT include terminating '\0' in the length value.
   
   pnLen: Cannot be NULL. On in, contains length of the buffer pszVersion.  
          On out, contains the length of the version information.
          The length does not include the terminating '\0'.

Return Value:
   returns SUCCESS or EFAILED

Comments:
   None

Side Effects:
   None

See Also:
   None
*/

#endif //OEMVERSION_H
