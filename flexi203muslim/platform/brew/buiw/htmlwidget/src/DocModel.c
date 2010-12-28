/*
  ========================================================================

  FILE:  DocModel.c
  
  SERVICES: 

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEESource.h"
#include "AEEDocModel.h"
#include "ModelBase.h"
#include "lnode.h"


#define MAKE_ME   DocModel *me = (DocModel*)po

#define BUFFERSIZE   512


/////////////////////////////////////////////////////////////////////////////
// DocModel implementation


typedef struct DocModel {
   ModelBase      base;       // model base struct
   IShell *       piShell;    // for IShell_Resume
   ISource *      piSource;   // ISource for source-based reads
   byte *         pBuffer;    // pointer to buffer
   int            ndxWrite;   // offset from pBuffer to append data
   int            ndxRead;    // offset from pBuffer to get valid data
   LNode          nodeW;      // node for writeable callback list
   LNode          nodeP;      // node for peekable callback list
   AEECallback    cbkRead;    // callback for ISOURCE_Readable
   int            cbSize;
   flg            bDataEndReached   : 1;
   flg            bPeekEnd : 1;
} DocModel;



#define POSREAD(p)      ((p)->pBuffer + (p)->ndxRead)
#define POSWRITE(p)     ((p)->pBuffer + (p)->ndxWrite) 
#define CBAVAIL(p)      ((p)->ndxWrite - (p)->ndxRead)
#define CBFREE(p)       ((p)->cbSize - (p)->ndxWrite)

#define HAVESPACE(p)    ((p)->ndxRead > 10 || (p)->ndxWrite < me->cbSize-10)

#define GETCBK(pnode)   (AEECallback*)(LNODE_INLIST((pnode)->pNext) ? (pnode)->pNext : NULL)
#define WRITECBK(p)     GETCBK(&((p)->nodeW))
#define PEEKCBK(p)      GETCBK(&((p)->nodeP))

#define COMPACTNOM      (me->cbSize / 2)

// forward decls
static void DocModel_ReadSource(DocModel *me);


/////////////////////////////////////////////////////////////////////////////



/*
:: DocModel_CompactBuffer
*/   
static void DocModel_CompactBuffer(DocModel *me, int cbWantFree) 
{
   if (me->ndxRead > 0) {
      
      // if read and write indexes are equal,
      // reset to beginning of buffer
      if (me->ndxWrite == me->ndxRead) {
         me->ndxWrite = me->ndxRead = 0;
      
      } else if (CBFREE(me) < cbWantFree) {
         int cbAvail = CBAVAIL(me);
         MEMMOVE(me->pBuffer, POSREAD(me), cbAvail);
         me->ndxRead = 0;
         me->ndxWrite = cbAvail;
      }
   }
}


/*
:: RemoveCallback
*/   
static void RemoveCallback(AEECallback *pcb)
{
   LNODE_REMOVE((LNode*)pcb);
   pcb->pfnCancel = 0;
}

/*
:: CancelCallbacks
*/   
static __inline void CancelCallbacks(LNode *ph)
{
   while (LNODE_INLIST(ph->pNext)) {
      CALLBACK_Cancel((AEECallback*)ph->pNext);
   }
}


/*
:: DocModel_ResetInternal
*/   
static void DocModel_ResetInternal(DocModel *me)
{
   CALLBACK_Cancel(&me->cbkRead);
   RELEASEIF(me->piSource);
   CancelCallbacks(&me->nodeW);
   me->bDataEndReached = 0;
   me->bPeekEnd = 0;
   me->ndxWrite = 0;
   me->ndxRead = 0;
}


/*
:: InsertCallback
*/   
static __inline void InsertCallback(LNode *ph, AEECallback *pcb)
{
   pcb->pfnCancel = RemoveCallback;
   LNODE_CTOR((LNode*)pcb);
   LNODE_INSERTBEFORE(ph, (LNode*)pcb);
}


/*
:: DocModel_InsertWriteCallback
*/   
static __inline void DocModel_InsertWriteCallback(DocModel *me, AEECallback *pcb)
{
   InsertCallback(&me->nodeW, pcb);
}

/*
:: DocModel_InsertPeekCallback
*/   
static __inline void DocModel_InsertPeekCallback(DocModel *me, AEECallback *pcb)
{
   InsertCallback(&me->nodeP, pcb);
}


/*
:: DocModel_CheckWriteable
*/   
static void DocModel_CheckWriteable(DocModel *me)
{
   AEECallback *pcb = WRITECBK(me);

   // if writeable pending, notify if we have compactable space
   if (pcb && HAVESPACE(me)) {
      CALLBACK_Cancel(pcb);   // removed pcb from our list
      ISHELL_Resume(me->piShell, pcb);
   }
}


/*
:: DocModel_CheckPeekable
*/   
static void DocModel_CheckPeekable(DocModel *me)
{
   AEECallback *pcb = PEEKCBK(me);
   
   if (pcb && (CBAVAIL(me) > 0 || me->bDataEndReached)) {

      if (me->bDataEndReached) {
         me->bDataEndReached = 0;
         me->bPeekEnd = 1;
      }
      CALLBACK_Cancel(pcb);   // removed pcb from our list
      ISHELL_Resume(me->piShell, pcb);
   }
}


/*
:: DocModel_QueueReadable
*/   
static __inline void DocModel_QueueReadable(DocModel *me) {
   if (!CALLBACK_IsQueued(&me->cbkRead)) {
      ISOURCE_Readable(me->piSource, &me->cbkRead);
   }
}


/////////////////////////////////////////////////////////////////////////////
// private interface methods

/*
:: DocModel_Peek
*/   
byte *DocModel_Peek(IDocModel *po, int32 *pcbAvailable, int32 *pnCode)
{
   MAKE_ME;

   if (!me->pBuffer) {
      *pcbAvailable = 0;
      *pnCode = IPEEK_ERROR;
      return NULL;
   }

   *pcbAvailable = CBAVAIL(me);
   *pnCode = me->bPeekEnd ? IPEEK_END : IPEEK_WAIT;
   return POSREAD(me);
}


/*
:: DocModel_Advance
*/   
void DocModel_Advance(IDocModel *po, int32 cbAdvance)
{
   MAKE_ME;
   me->ndxRead = CONSTRAIN(me->ndxRead + cbAdvance, 0, me->ndxWrite);

   if (me->ndxWrite == me->ndxRead) {
      me->ndxRead = me->ndxWrite = 0;
   }

   if (me->piSource) {
      DocModel_QueueReadable(me);
   } else {
      DocModel_CheckWriteable(me);
   }
   
}


/*
:: DocModel_Peekable
*/   
void DocModel_Peekable(IDocModel *po, AEECallback *pcb)
{
   MAKE_ME;
   DocModel_InsertPeekCallback(me, pcb);
   DocModel_CheckPeekable(me);
}


/////////////////////////////////////////////////////////////////////////////
// IDocModel interface (public)

/*
:: DocModel_Reset
*/   
static void DocModel_Reset(IDocModel *po)
{
   MAKE_ME;
   DocModel_ResetInternal(me);
   ModelBase_NotifyEvent((IModel*)&me->base, EVT_HDM_RESET);
}
   

/*
:: DocModel_Stop
*/   
static void DocModel_Stop(IDocModel *po)
{
   MAKE_ME;

   DocModel_ResetInternal(me);
   me->bDataEndReached = 1;
   DocModel_CheckPeekable(me);
   ModelBase_NotifyEvent((IModel*)&me->base, EVT_HDM_STOP);
}


/*
:: DocModel_Writeable
*/
static void DocModel_Writeable(IDocModel *po, AEECallback *pcb)
{
   MAKE_ME;
   
   DocModel_InsertWriteCallback(me, pcb);
   DocModel_CheckWriteable(me);
}


/*
:: DocModel_Write
*/

static int DocModel_Write(IDocModel *po, const byte *pbWrite, int cbLen, int *pcbWrote)
{
   MAKE_ME;
   int nBytesRec;

   if (pcbWrote) {
      *pcbWrote = 0;
   }

   // can't write in middle of an ISource operation
   if (me->piSource) {
      return EBADSTATE;
   }

   // handle "Terminate" signal
   if (!pbWrite) {
      me->bDataEndReached = 1;
      DocModel_CheckPeekable(me);
      return SUCCESS;
   }

   DocModel_CompactBuffer(me, cbLen);
   nBytesRec = MIN(CBFREE(me), cbLen);

   if (nBytesRec) {
      me->bDataEndReached = 0;
      MEMMOVE(POSWRITE(me), pbWrite, nBytesRec);
      me->ndxWrite += nBytesRec;
      DocModel_CheckPeekable(me);
   }

   if (pcbWrote) {
      *pcbWrote = nBytesRec;
   }

   return SUCCESS;
}



/*
:: DocModel_ReadSource
*/
static void DocModel_ReadSource(DocModel *me)
{
   int rv;

   DocModel_CompactBuffer(me, COMPACTNOM);

   rv = ISOURCE_Read(me->piSource, (char*)POSREAD(me), CBFREE(me));

   if (rv == ISOURCE_END || 
       rv == ISOURCE_ERROR) {

      me->bDataEndReached = 1;

      DocModel_CheckPeekable(me);
      RELEASEIF(me->piSource);
   
   } else if (rv > 0) {
      me->ndxWrite += rv;
      DocModel_CheckPeekable(me);
      DocModel_QueueReadable(me);
   }
}


/*
:: DocModel_LoadSource
:: 
:: Load data from an ISource into an internal buffer. 
*/
static int DocModel_LoadSource(IDocModel *po, ISource *piSource)
{
   MAKE_ME;

   if (!piSource) {
      return EBADPARM;
   }
   
   DocModel_Reset(po);

   me->piSource = piSource;
   ISOURCE_AddRef(me->piSource);
   DocModel_ReadSource(me);

   return SUCCESS;
}


static int DocModel_SetBufferSize(IDocModel *po, int cbSize)
{
   MAKE_ME;

   if (cbSize != me->cbSize) {
      FREEIF(me->pBuffer);
      me->cbSize = 0;
      me->pBuffer = MALLOC(cbSize);
      if (!me->pBuffer) {
         return ENOMEMORY;
      }
      me->cbSize = cbSize;
   }
   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// IDocModel creation and IBase methods


/* 
:: DocModel_Dtor
*/
static void DocModel_Dtor(DocModel *me)
{
   CancelCallbacks(&me->nodeW);
   CancelCallbacks(&me->nodeP);
   CALLBACK_Cancel(&me->cbkRead);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piSource);
   FREEIF(me->pBuffer);
}


/* 
:: DocModel_Release
*/
static uint32 DocModel_Release(IDocModel *po)
{
   MAKE_ME;

   if (me->base.nRefs == 1) {
      DocModel_Dtor(me);
   }
   return ModelBase_Release((IModel*)me);
}

/* 
:: DocModel_QueryInterface
*/
static int DocModel_QueryInterface(IDocModel *po, AEECLSID id, void **ppo)
{
   if (SUCCESS == ModelBase_QueryInterface((IModel*)po, id, ppo)) {
      return SUCCESS;
   }

   if (id == AEEIID_DOCMODEL) {
      *ppo = po;
      IDOCMODEL_AddRef(po);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}


/* 
:: DocModel_Construct
*/
static int DocModel_Construct(DocModel *me, AEEVTBL(IDocModel) *pvt, IShell *piShell, IModule *piModule)
{
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel)*)pvt, piModule);

   // override vtable methods
   pvt->Release         = DocModel_Release;
   pvt->QueryInterface  = DocModel_QueryInterface;
   
   // initialize IDocModel method(s)
#define VTINIT(name)   pvt->name = DocModel_##name
   VTINIT( LoadSource );
   VTINIT( Write );
   VTINIT( Writeable );
   VTINIT( Reset );
   VTINIT( Stop );
   VTINIT( SetBufferSize );

   LNODE_CTOR(&me->nodeW);
   LNODE_CTOR(&me->nodeP);

   me->piShell = piShell;
   ADDREFIF(piShell);

   CALLBACK_Init(&me->cbkRead, DocModel_ReadSource, me);

   return DocModel_SetBufferSize((IDocModel*)me, BUFFERSIZE);
}


/* 
:: DocModel_New
*/
int DocModel_New(IDocModel **ppo, IShell *piShell, IModule *piModule)
{
   int nErr = 0;
   DocModel *me;
   
   *ppo = 0;
   
   me = MALLOCREC_VTBL(DocModel, IDocModel);
   if (!me) {
      return ENOMEMORY;
   }

   nErr = DocModel_Construct(me, GETVTBL(me,IDocModel), piShell, piModule);
   if (nErr) {
      DocModel_Release((IDocModel*)me);
   } else {
      *ppo = (IDocModel*)me;
   }

   return nErr;
}







