/*
  ============================================================================

FILE:  OEMMediaMultipleFile.h

SERVICES:  
       Common IMedia - Multiple File functions.  

GENERAL DESCRIPTION:

REVISION HISTORY: 

  ============================================================================
  ============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef OEMMULTIPLEFILE_H
#define OEMMULTIPLEFILE_H

#include "customer.h"

#ifdef   FEATURE_APP_MEDIA_MULTIPLE_FILE

#include "AEE_OEM.h"
#include "AEEMediaFormats.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*-------------------------------------------------------------------
      Macros Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

/*===========================================================================

                      IMediaMultipleFile DECLARATIONS

===========================================================================*/

// Called by AEE_Init() during BREW initialization
void  IMediaMultipleFile_Init(IShell * ps);
// Called by ISHELL_CreateInstance() during object instantiation
int   IMediaMultipleFile_New(IShell * ps, AEECLSID cls, void **ppif);

boolean OEMMediaMultipleFile_GetAudioVideoFileNames(const char *pFile, const char *pRelativePath, 
                                                    char *pImageBuf, char *pSoundBuf, int bufSize);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif  // FEATURE_APP_MEDIA_MULTIPLE_FILE

#endif /* #ifndef OEMMULTIPLEFILE_H*/
