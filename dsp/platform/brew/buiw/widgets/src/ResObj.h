/*
  ========================================================================

  FILE:       ResObj.h

  SERVICES:   ResFile implementation / replacement for ISHELL_LoadResXxx

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __RESOBJ_H__
#define __RESOBJ_H__

#include "AEEShell.h"
#include "AEEFile.h"

// error code not defined for BREW2.1
#ifndef  EBUFFERTOOSMALL
#define  EBUFFERTOOSMALL      38       // Destination buffer given is too small
#endif

typedef struct ResObj ResObj;


int   ResObj_New           (ResObj **ppo, IShell *piShell, const char *pszResFile);
void  ResObj_Delete        (ResObj *me);
int   ResObj_Open          (ResObj *me, const char *pszResFile);
int   ResObj_BufferLoadData(ResObj *me, uint16 nResID, uint16 nType, void *pBuffer, uint32 dwBufSize, uint32 *pdwResSize);
int   ResObj_LoadData      (ResObj *me, uint16 nResID, uint16 nType, void **ppo);
int   ResObj_LoadObject    (ResObj *me, uint16 nResID, AEECLSID clsidHandler, IBase **ppo);
int   ResObj_LoadString    (ResObj *me, uint16 nResID, AECHAR *pwchBuffer, uint32 dwBufSize, uint32 *pdwResSize);
int   ResObj_LoadImage     (ResObj *me, uint16 nResID, IImage *ppo);
int   ResObj_LoadSource    (ResObj *me, uint16 nResID, uint16 nType, ISource **ppo);
int   ResObj_GetMappedData (ResObj *me, uint16 nResID, uint16 nType, uint32 *pdwResSize, void **ppo);

boolean  ResObj_IsMapped   (ResObj *me);
int      ResObj_GetSize    (ResObj *me, uint16 nResID, uint16 nType, uint32 *pdwResSize);


void *ResObj_GetUserPtr    (ResObj *me);
void  ResObj_SetUserPtr    (ResObj *me, void *pUser);


// load a partial resource, offsetting by 'cbOffset' bytes
int   ResObj_BufferLoadDataEx (ResObj *me, uint16 nResID, uint16 nType, void *pBuffer, uint32 dwBufSize, uint32 *pdwResSize, uint32 cbOffset);

static __inline int ResObj_BufferLoadPartial(ResObj *me, uint16 nResID, uint16 nType, void *pBuffer, 
                                             uint32 dwBufSize, uint32 *pdwResSize, uint32 cbOffset) {
   int nErr = ResObj_BufferLoadDataEx (me, nResID, nType, pBuffer, dwBufSize, pdwResSize, cbOffset);
   return (nErr == EBUFFERTOOSMALL) ? SUCCESS : nErr;
}


#endif   // __RESOBJ_H__
