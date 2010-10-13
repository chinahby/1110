#if !defined(OEMFSPATH_PRIV_H)
#define OEMFSPATH_PRIV_H
/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMFSPath_priv.h

SERVICES:  Reverse AEE path to OEM path translation

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required to perform reverse path
translations

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#define OEMFS_BREW_DIR     "brew"
#define OEMFS_CARD0_DIR    "mmc1"
#define OEMFS_RINGERS_DIR  "user/sound/ringer"
#define OEMFS_ODM_BREW_USER_DIR "brew/user"

// FTL Storage Device
#ifdef CUST_EDITION	 
#include "AEEFTL.h"
#endif /*CUST_EDITION*/
#define OEMFS_FTL_DIR    "ftl1"

// USB Host Mass Storage 1
#define OEMFS_UHMS1_DIR    "usb1"  
// USB Host Mass Storage 2
#define OEMFS_UHMS2_DIR    "usb2" 

#if defined(__cplusplus)
extern "C" {
#endif

int     OEMFS_GetBREWPath(const char *cpszIn, char *pszOut, int *pnOutLen);  
boolean OEMFS_IsAutoCreate(const char *cpszIn);

#if defined(__cplusplus)
}
#endif


/*
==================================================================
Function: OEMFS_GetBREWPath

Description:
   This function is called by the OEM layer to map an OEM pathname to 
    a BREW pathname, one that will be acceptable to be passed directly
    to the AEE file handling functions.

Prototype:
   int OEMFS_GetBREWPath(const char *cpszIn, char *pszOut, int *pnOutLen)

Parameters:
   cpszIn:  a "native" OEM path

   pszOut:  [out] output buffer for this function to write into
   pnOutLen: [in/out] if pszOut is non-null, on input this is the length of 
                   pszOut, and on output, is set to the number of bytes 
                   written to pszOut (including null termination)
               if pszOut is null, pnOutLen is ignored on input, and on
                   output is set to the number of bytes required to map the 
                   path.

Return Value:
   SUCCESS if the name could be mapped and was mapped successfully
   EBADFILENAME if the name could not be mapped
   EBUFFERTOOSMALL if pszOut is non-null and *pnOutLen is not big enough 
              to hold the path + null-termination
   
Comments:
   pszOut is always null terminated on output.

   This is a user-mode function.

Side Effects:
   None

See Also:

==================================================================
Function: OEMFS_IsAutoCreate

Description:
   This function is called by the OEM layer to determine whether
   the OEMFS layer should automatically recursively create a 
   directory when a BREW path maps to a nested name in the 
   native filesystem space.

   If, for example, fs:/ringers maps to user/sound/ringers, this
   function should return TRUE for "user/sound/ringers".

   This is done because BREW's automatic recursive mkdir()
   doesn't know about the nested directories in the native 
   filesystem.

Prototype:
   boolean OEMFS_IsAutoCreate(const char *cpszIn)

Parameters:
   cpszIn:  a "native" OEM path

Return Value:
   TRUE if OEMFS_Mkdir should automatically create parent
       directories of the path
   
Comments:
   This is a user-mode function.

Side Effects:
   None

See Also:

========================================================================

*/
#endif /* OEMFSPATH_PRIV_H */
