/*
  ========================================================================

  FILE:       FileSource.c

  SERVICES:   IFile wrapper to implement streams in sub-sections of a file.

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEEFile.h"
#include "AEESource.h"
#include "wutil.h"


typedef struct FileSource {
   const AEEVTBL(ISource) *pvt;
   int            nRefs;
   IFile *        piFile;
   uint32         dwFileOffset;
   uint32         dwSourceSize;
   PFNNOTIFY      pfnNotify;     // notify pfn from AEECallback (for cancel)
   void *         pNotifyData;   // notify context from AEECallback (for cancel)
} FileSource;


#define DECLARE_ME    FileSource *me = ((FileSource*)po)


static void FileSource_Delete(FileSource *me);



uint32 FileSource_AddRef(ISource *po)
{
   DECLARE_ME;
   return ++me->nRefs;
}

static int FileSource_QueryInterface(ISource *po, AEECLSID id, void **ppo)
{
   if (id == AEECLSID_SOURCE || id == AEECLSID_QUERYINTERFACE) {
      *ppo = po;
      IQI_AddRef(*ppo);
      return SUCCESS;
   }

   *ppo = 0;
   return ECLASSNOTSUPPORT;
}


uint32 FileSource_Release(ISource *po)
{
   DECLARE_ME;
   
   if (me->nRefs == 1) { 
      FileSource_Delete(me);
      return 0;
   }
   return --me->nRefs;
}


static void FileSource_Cancel(AEECallback *pcb)
{
   FileSource *me = (FileSource*)pcb->pNotifyData;
   if (me->pfnNotify && me->piFile) {
      IFILE_Cancel(me->piFile, me->pfnNotify, me->pNotifyData);
      me->pfnNotify     = 0;
      me->pNotifyData   = 0;
   }
}

static void FileSource_Readable(ISource * po, AEECallback *pcb)
{
   DECLARE_ME;

   // install cancel callback function
   pcb->pfnCancel    = FileSource_Cancel;
   pcb->pCancelData  = me;
   // save notify function and context
   // so Cancel callback can use it
   me->pfnNotify     = pcb->pfnNotify;
   me->pNotifyData   = pcb->pNotifyData;

   IFILE_Readable(me->piFile, pcb->pfnNotify, pcb->pNotifyData);
}

static int32 FileSource_Read(ISource * po, char *pcBuf, int32 cbBuf)
{
   DECLARE_ME;
   int32 cbRead;
   uint32 dwPos;

   if (!pcBuf) {
      return me->dwSourceSize;
   }

   if ((uint32)cbBuf > me->dwSourceSize) {
      cbBuf = me->dwSourceSize;
   }

   // seek if we're not at the correct position
   dwPos = IFILE_Seek(me->piFile, _SEEK_CURRENT, 0);
   if (dwPos != me->dwFileOffset) {
      IFILE_Seek(me->piFile, _SEEK_START, me->dwFileOffset);
   }

   cbRead = IFILE_Read(me->piFile, (void*)pcBuf, (uint32)cbBuf);

   me->dwSourceSize -= cbRead;
   me->dwFileOffset += cbRead;

   return cbRead;
}


static void FileSource_Delete(FileSource *me)
{
   if (me->pfnNotify && me->piFile) {
      IFILE_Cancel(me->piFile, me->pfnNotify, me->pNotifyData);
   }
   RELEASEIF(me->piFile);
   FREE(me);
}

static void FileSource_Ctor(FileSource *me, AEEVTBL(ISource) *pvt, IFile *piFile, uint32 dwFileOffset, uint32 dwSourceSize)
{
   me->nRefs = 1;
   me->pvt = pvt;

   me->piFile = piFile;
   IFILE_AddRef(me->piFile);

   me->dwFileOffset = dwFileOffset;
   me->dwSourceSize = dwSourceSize;

#define VTFUNC(name)    pvt->name = FileSource_##name
   VTFUNC( AddRef );
   VTFUNC( Release );
   VTFUNC( QueryInterface );
   VTFUNC( Readable );
   VTFUNC( Read );
}


int FileSource_New(ISource **ppo, IFile *piFile, uint32 dwFileOffset, uint32 dwSourceSize)
{
   FileSource *me;

   me = MALLOCREC_VTBL(FileSource, ISource);
   if (!me) {
      return ENOMEMORY;
   }

   FileSource_Ctor(me, GETVTBL(me,ISource), piFile, dwFileOffset, dwSourceSize);
   *ppo = (ISource*)me;

   return SUCCESS;
}

