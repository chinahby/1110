/*
  ========================================================================

  FILE:  ResFile.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Internal functions for ResFile

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __RESFILE_H__
#define __RESFILE_H__

#include "AEEResFile.h"
#include "ResObj.h"
#include "ResDecoderCache.h"

#include "bid/AEECLSID_RESFILE.bid"

typedef struct ResFile ResFile;

#include "AEEBase.h"

// IParameters1 interface support
struct IParameters1 {
   AEEBASE_INHERIT(IParameters1, ResFile);
};

struct ResFile {
   AEEVTBL(IResFile) *  pvt;              // vtbl
   uint32               nRefs;
   IModule *            piModule;
   IShell  *            piShell;
                                          // 
   ResObj **            ppResObj;         // array of ResObj ptrs
   int                  nFileCount;       // count of active files/entries in 'pRes'

   char *               pszLookup;        // parsed arglist name strings: zz-terminated
   int                  cbAlloc;          // allocation size of pszLookup (for MemGrow)
   ResDecoderCache *    pResDecoderCache;

   // IParameters1 interface support
   IParameters1            param;
   AEEVTBL(IParameters1)   vtParameters;

   ICharsetConv *          piConv;        // charset converter
   const char *            pszConvTo;     // "convert-to" string
};



int    ResFile_New(IResFile **ppo, IModule *piModule, IShell *piShell);
int    ResFile_Construct(ResFile *me, AEEVTBL(IResFile) *pvt, IModule *piModule, IShell *piShell);
void   ResFile_Dtor(ResFile *me);
uint32 ResFile_AddRef(IResFile *po);
uint32 ResFile_Release(IResFile *po);
int    ResFile_QueryInterface(IResFile *po, AEECLSID clsid, void **ppo);

int    ResFile_Open(IResFile *po, const char *fname);
void   ResFile_Close(IResFile *po);
int    ResFile_Get(IResFile *po, uint32 resid, uint16 restype, void *pbuf, uint32 *plen);
int    ResFile_GetSource(IResFile *po, uint32 resid, uint16 restype, ISource **ppo);
int    ResFile_GetObject(IResFile *po, uint32 resid, AEECLSID clsid, void **ppo);
int    ResFile_GetNamedV(IResFile *po, uint16 restype, void *pbuf, uint32 *plen, VaListPtrType args);
int    ResFile_GetNamedObjectV(IResFile *po, AEECLSID clsid, void **ppo, VaListPtrType args);
int    ResFile_GetNamedIdV(IResFile *po, uint16 restype, uint32 *presid, VaListPtrType args);

#endif /* __RESFILE_H__ */
