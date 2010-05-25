/*======================================================
FILE:  AEE_OEMFile.h

SERVICES:  AEE OEM File Interface header

GENERAL DESCRIPTION:

This file contains function prototypes for AEE file 
routines that may be required by the OEM.

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEMFILE_H)
#define AEE_OEMFILE_H  1

#include "AEE_OEMComdef.h"
#include "AEEFile.h"

#if defined(__cplusplus)
extern "C" {
#endif

//
// Definition for Const Files
// 
// Note the following:
//
// - Const files are created as object files and stored in the core flash image
// - They are linked in to a list managed by OEMFS.C that is scanned on relevant operations
// - The files are treated as read-only
// - They cannot be removed but may be overridden by a dynamic file if that attribute is set
//   in this structure
// - They may be stored in any EFS directory
// - If a file exists in a directory, that directory cannot be removed via RmDir
//

typedef struct
{
   const char *   pszName;          // Full pathname for the file (example: "brew/NNNN.mif")
   unsigned       bFixed:1;         // Indicates whether file can be overridden by EFS
   uint32         dwPhysicalSize;   // Size of the input data stored
   uint32         dwPhysicalOffset; // Offset of start of physical file
   uint32         dwDataSize;       // Total size of the data stored
   byte *         pFileData;        // Pointer to data for contents of the file
} AEEConstFile;


// 
//  AEEFile helpers
//

extern AEE_EXPORTS int AEE_CheckPathAccess(const char *cpszFile, 
                                           uint32 dwDesiredRights,
                                           uint32 *pdwActualRights);

extern AEE_EXPORTS int AEE_ResolvePath(const char *cpszIn, char *pszOut, int *pnOutLen);

#if defined(__cplusplus)
}
#endif
#endif /* define AEE_OEMFILE_H  1 */
