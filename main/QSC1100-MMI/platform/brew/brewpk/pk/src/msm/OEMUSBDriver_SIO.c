/*======================================================
FILE:  OEMUSBDriver_SIO.c

SERVICES:  BREW USB Driver Implementation

GENERAL DESCRIPTION:
   These are APIs to expose USB internals so that BREW applications can
   implement an arbitrary USB driver.  This code interfaces to the BREW
   SIO code as well as directly to the USB driver.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IUSBDriver

       Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#ifndef USES_MMI
#include "ui.h"
#else
#include "oemui.h"
#endif#include "dsm.h"
#include "usbcdc.h"
#include "usbdc.h"
#include "sio.h"
#include "rdevmap.h"

#include "OEMFeatures.h"

#ifdef FEATURE_BREW_USBDRIVER
#ifdef FEATURE_BREW_USBDRIVER_SIO

#include "AEE.h"
#include "AEESIO.h"
#include "AEEPort.h"
#include "AEEStdLib.h"
#include "BREWVersion.h"

#if MIN_BREW_VERSION(3,0)
#include "AEE_OEMDispatch.h"
#endif
#include "AEE_OEM.h"

#include "AEEUSBDriver.h"
#include "OEMUSBDriver.h"

#include "OEMSIO_priv.h"

//=============================================================================
//   Macro definitions
//=============================================================================

// For BREW 2.1, we need to define AEE_APP_RESUME
#ifndef AEE_APP_RESUME
#define AEE_APP_RESUME(pcb,pac)      AEE_ResumeEx(pcb,0,pac)
#endif

// This is the number of blocks we'd like to have free at any given time
#define AEEDSMBUFMGR_LOW_WATERMARK  8
#define AEEDSMBUFMGR_HIGH_WATERMARK 12

// This is the size of each block
#define AEEDSMBUFMGR_ALLOCSIZE 16384

// Magic number to figure out of a block of mem is ours or not
#define AEEDSMBUFMGR_MAGICNUM 0x6E2A8B23

// USBDriver states
#define USBDRIVER_STATE_CLOSED   (1)
#define USBDRIVER_STATE_OPENED   (2)
#define USBDRIVER_STATE_OPENING  (3)
#define USBDRIVER_STATE_CLOSING  (4)

// USBDriver ISR task signalling
#define USBDRIVER_ISR_DSMALLOC   (0x0001)
#define USBDRIVER_ISR_ENABLECB   (0x0002)
#define USBDRIVER_ISR_SETUPMSGCB (0x0004) 
#define USBDRIVER_ISR_SIGGOOD    (0x0008)

// Warning macro
#ifndef AEE_USBDRIVER_SIG

#error  Signal AEE_USBDRIVER_SIG not defined. Please do the following steps to define and use the \
        AEE_USBDRIVER_SIG signal. \
Please do the following steps to define and use the signal. \
1. Define the signal in ui.h to a un-used signal value.\
   ex. #define AEE_USBDRIVER_SIG 0x0800 \
   This in only an example. You may have to choose a value different than 0x0800 \
   You can define/declare it right after the AEE_SIO_SIG used by BREW \
2. Add the following line to the sig mask that rex_wait(in the function ui task) waits on: \
     | AEE_USBDRIVER_SIG (also right next to AEE_SIO_SIG) \
3. Modify the function ui_task() in ui.c or uihsig.c to handle this signal. \
   When this signal is set, the function OEMSIO_TaskHandler() must be invoked. Here is the sequence: \
  Add the following code right after the AEE_APP_SIG if statement. \
    else if ( sigs & AEE_USBDRIVER_SIG) { \
      extern void OEMUSBDriver_TaskHandler(void);\
      (void) rex_clr_sigs( rex_self(), AEE_USBDRIVER_SIG); \
      (void) OEMUSBDriver_TaskHandler();\
    }
#endif

#ifndef FOREACH
#define FOREACH(p,a) for ((p)=(a); (p) < (a) + ARRAY_SIZE(a); (p)++)
#endif /* FOREACH */

#ifndef RELEASEIF
#define RELEASEIF(x)  do { if ((x) != NULL) {IBASE_Release((void*)(x)); (x)=NULL;} } while (0)
#endif

#define IS_32BIT_ALIGNED(x) (!(((uint32)(x)) & 3))

#if (MIN_BREW_VERSION(3,0))
#define INTPORT_TO_USBDRIVER(p)  (IUSBDriver*)(((void**)p) - 1)
#define SETAPPCONTEXT(x)      AEE_EnterAppContext(x)
#define RESTOREAPPCONTEXT(x)  AEE_LeaveAppContext(x)
#else
#define INTPORT_TO_USBDRIVER(p)  (IUSBDriver*)(((AEESysObject*)p) - 1)
#define SETAPPCONTEXT(x)      AEE_SetAppContext(x)
#define RESTOREAPPCONTEXT(x)  AEE_SetAppContext(x)
#endif

//=============================================================================
//   Type definitions
//=============================================================================

typedef int (*PFNIOCTLHDLR)(IUSBDriver *pme, IPort *piBasePort, int nIOCtl, uint32 dwParam);
typedef struct AEEDSMBuf AEEDSMBuf;
typedef struct AEEDSMBufMgr AEEDSMBufMgr;
typedef struct SubPort SubPort;

struct AEEDSMBuf
{
   struct AEEDSMBuf *pNext;   // Used for linked lists
   dsm_item_type *pDSMItem;      // DSM Item containing the buffer
   void          *pBuf;          // Our buffer
};

// This is the structure for the buffer manager.  At the present time, there
// should be at most one in the system.
struct AEEDSMBufMgr
{
   AEEDSMBuf     *pAvailBufs;
   int            nAvailBufs;
   AEEDSMBuf     *pUsedBufs;
   
};

#define SUBPORT_BULK    0
#define SUBPORT_INT     1
#define SUBPORT_MAX     SUBPORT_INT+1
struct SubPort
{
   IUSBDriver     *pParent;         // Pointer back to parent structure.
                                    // Used in callbacks.

   IPort          *piPort;          // The underlying IPort object
   char           *pszOpenStr;      // Open string to use
   int             nSIOPort;        // AMSS port ID

   AEECallback     cbReady;         // Port is ready for use
   AEECallback    *pcbReadables;    // Readable callbacks for this port
   AEECallback    *pcbWriteables;   // Writeable calblacks for this port

   int             nLastErr;
   int             nState;
};

struct IUSBDriver
{
#if (MIN_BREW_VERSION(3,1))
   AEEVTBL(IUSBDriver)  *pvt;
#else
   AEESysObject          so;
#endif
   AEEVTBL(IPort)       *pvt_int;

   uint32         dwRefs;

   PACONTEXT      pac;   

   AEEDSMBufMgr   bufmgr;

   // This interface provides two seperate IPort-derived objects that each
   // have an associated SIO IPort object.  Store those mappings here.
   SubPort        subports[SUBPORT_MAX];

   boolean        bUseInterrupt;

#if (MIN_BREW_VERSION(3,0))
   AEECallback    cbSysObject;
#endif

   // Setup msg stuff
   AEEUSBSetup_Req SetupMsg;
   PFNUSBSETUPNOTIFIER pfnSetupReq;
   void          *pSetupData;
   boolean        bStallNeeded;

   // Stuff to handle code in ISRs
   boolean        bISRCBScheduled;
   int            nISRReason;
   boolean        bInited;
   rex_tcb_type  *pTCB;
};

typedef struct USBDriver_IOCtl_Handler
{
   int            nIOCtl;  // IOCtl that we handle
   PFNIOCTLHDLR   pfn;     // Handler function for the data
} USBDriver_IOCtl_Handler;

//=============================================================================
// Function declarations
//=============================================================================
static void AEEDSMBufMgr_Allocate(AEEDSMBufMgr *pBufMgr);
static AEEDSMBuf* AEEDSMMgr_DequeueByDSMPtr(AEEDSMBuf **ppList, 
                                            dsm_item_type *pFind);
static void AEEDSMBufMgr_Free(AEEDSMBuf *pBuf);
static void USBDriver_Destroy(IUSBDriver *pme, boolean bIgnoreCB);
static void USBDriver_CBRegister(AEECallback **ppList, AEECallback *pcb);
static void USBDriver_CBCancel(AEECallback *pcb);
static int USBDriver_IOCtl_Cmn(IUSBDriver *pme, IPort *piBasePort, int nOption, uint32 dwVal);

// IOCtl handlers
static int USBDriver_Hdl_GetCnxnStatus(IUSBDriver *pme, IPort *piBasePort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_NtfyCnxnStatus(IUSBDriver *pme, IPort *piBasePort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_NtfyBufEmpty(IUSBDriver *pme, IPort *piBasePort, int nIOCtl, uint32 dwParam);

// Interfaces for IUSBDriver and the interrupt IPort
static uint32 IUSBDriver_AddRef(IUSBDriver *pme);
static uint32 IUSBDriver_Release(IUSBDriver *pme);
static int IUSBDriver_QueryInterface(IUSBDriver *pme, AEECLSID cls, void **ppif);
static int32 IUSBDriver_Read(IUSBDriver *pme, char *pcBuf, int32 cbBuf);
static void IUSBDriver_Readable(IUSBDriver *pme, AEECallback *pcb);
static int IUSBDriver_GetLastError(IUSBDriver *pme);
static int32 IUSBDriver_Write(IUSBDriver *pme, const char *pcBuf, int32 cbBuf);
static void IUSBDriver_Writeable(IUSBDriver *pme, AEECallback *pcb);
static int IUSBDriver_IOCtl(IUSBDriver *pme, int nOption, uint32 dwVal);
static int IUSBDriver_Close(IUSBDriver *pme);
static int IUSBDriver_Open(IUSBDriver *pme, const char *szPort);
static IPort* IUSBDriver_GetInterruptEndpoint(IUSBDriver *po, int nLatency);
static void IUSBDriver_SetSetupNotifier(IUSBDriver *pme, PFNUSBSETUPNOTIFIER pfn, void *pUser);
static int IUSBDriver_SendSetupResponse(IUSBDriver *po, const byte *pResponse, int nSize);
static uint32 IUSBDriver_Int_AddRef(IPort *pme);
static uint32 IUSBDriver_Int_Release(IPort *pme);
static int IUSBDriver_Int_QueryInterface(IPort *pme, AEECLSID cls, void **ppif);
static int32 IUSBDriver_Int_Read(IPort *pme, char *pcBuf, int32 cbBuf);
static void IUSBDriver_Int_Readable(IPort *pme, AEECallback *pcb);
static int IUSBDriver_Int_GetLastError(IPort *pme);
static int32 IUSBDriver_Int_Write(IPort *pme, const char *pcBuf, int32 cbBuf);
static void IUSBDriver_Int_Writeable(IPort *pme, AEECallback *pcb);
static int IUSBDriver_Int_IOCtl(IPort *pme, int nOption, uint32 dwVal);
static int IUSBDriver_Int_Close(IPort *pme);
static int IUSBDriver_Int_Open(IPort *pme, const char *szPort);

static void USBDriver_SetupMsgCB_ISR(usbdc_setup_type *pSetupMsg);
static void USBDriver_SetupMsgCB(IUSBDriver *pme);
static void USBDriver_EnableCB_ISR(void);
static void USBDriver_EnableCB(IUSBDriver *pme);
static void USBDriver_GoToBREWTask(IUSBDriver *pme, int nReason);
static void USBDriver_TryOpen(void *po);
#if MIN_BREW_VERSION(3,1)
static void USBDriver_UncleanExit(void *po);
#endif

//=============================================================================
//   Globals and file-static variables
//=============================================================================

// Test to see if the USBDRIVER_SIG is working
static boolean gbSigGood = FALSE;

// Our global instance
static IUSBDriver gUSBDriver;

// IOCtl table for the IUSBDriver object
USBDriver_IOCtl_Handler gUSBDriverIOCtlHandlers[] =
{
   {AEEUSBDRIVER_IOCTL_GETCNXNSTATUS, USBDriver_Hdl_GetCnxnStatus},
   {AEEUSBDRIVER_IOCTL_NTFYCNXNSTATUS, USBDriver_Hdl_NtfyCnxnStatus},
   {AEEUSBDRIVER_IOCTL_NTFYBUFEMPTY, USBDriver_Hdl_NtfyBufEmpty},
};

// Vtables
AEEVTBL(IUSBDriver) gUSBDriverVtblFuncs =
   {
      IUSBDriver_AddRef,
      IUSBDriver_Release,
      IUSBDriver_QueryInterface,
      IUSBDriver_Read,
      IUSBDriver_Readable,
      IUSBDriver_GetLastError,
      IUSBDriver_Write,
      IUSBDriver_Writeable,
      IUSBDriver_IOCtl,
      IUSBDriver_Close,
      IUSBDriver_Open,
      IUSBDriver_GetInterruptEndpoint,
      IUSBDriver_SetSetupNotifier,
      IUSBDriver_SendSetupResponse
   };

AEEVTBL(IPort) gUSBDriverIntVtblFuncs = 
   {
      IUSBDriver_Int_AddRef,
      IUSBDriver_Int_Release,
      IUSBDriver_Int_QueryInterface,
      IUSBDriver_Int_Read,
      IUSBDriver_Int_Readable,
      IUSBDriver_Int_GetLastError,
      IUSBDriver_Int_Write,
      IUSBDriver_Int_Writeable,
      IUSBDriver_Int_IOCtl,
      IUSBDriver_Int_Close,
      IUSBDriver_Int_Open
   };

//=============================================================================
//   Function definitions
//=============================================================================

// Initialize the buffer manager subsystem
void OEMUSBDriver_Init(IShell *piShell)
{
   IUSBDriver *pme = (IUSBDriver*)&gUSBDriver;
   int i;

   MEMSET(pme, 0, sizeof(IUSBDriver));
   for (i=0; i<SUBPORT_MAX; i++)
   {
      SubPort *pSubPort = &pme->subports[i];
      pSubPort->pParent = pme;
      CALLBACK_Init(&pSubPort->cbReady, USBDriver_TryOpen, pSubPort);
      pSubPort->nState = USBDRIVER_STATE_CLOSED;
   }
   pme->pvt_int = &gUSBDriverIntVtblFuncs;

   // Make sure our signal is properly registered
   pme->pTCB = rex_self();
   USBDriver_GoToBREWTask(pme, USBDRIVER_ISR_SIGGOOD);

   // Initialize unique indexes
   pme->subports[SUBPORT_BULK].nSIOPort = RDM_USB_BREW_EXTENDED_DATA_DEV;
   pme->subports[SUBPORT_INT].nSIOPort = RDM_USB_BREW_EXTENDED_NOTIFICATION_DEV;

#if MIN_BREW_VERSION(3,1)
   // Initialize our cleanup stuff
   CALLBACK_Init(&pme->cbSysObject, USBDriver_UncleanExit, (void*)pme);
#endif 
}

// Class instantiation.  Works only once per run
int OEMUSBDriver_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   IUSBDriver *pNew = &gUSBDriver;
   int nErr;
   ACONTEXT *pac;

   *ppif = NULL;

   // If the OEM did not properly implement the USBDRIVER signal, prevent
   // the interface from instantiating.
   if (!gbSigGood)
   {
      DBGPRINTF("Implementor Error: Not receiving AEE_USBDRIVER_SIG signals");
      DBGPRINTF("(cont)in OEMUSBDriver_TaskHandler");
      return EFAILED;
   }

   // See if its already been allocated
   if (pNew->dwRefs > 0)
   {
      return EALREADYLOADED;
   }

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_SERIAL,
                                (void**)&pNew->subports[SUBPORT_BULK].piPort);
   if (nErr != SUCCESS)
   {
      USBDriver_Destroy(pNew, TRUE);
      return EFAILED;
   }

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_SERIAL,
                                (void**)&pNew->subports[SUBPORT_INT].piPort);
   if (nErr != SUCCESS)
   {
      USBDriver_Destroy(pNew, TRUE);
      return EFAILED;
   }

   // Assign the pointer
   *ppif = (void*)pNew;
   pNew->dwRefs = 1;

   pNew->pac = AEE_GetAppContext();

   // Set up the BREW DSM buffer manager
   pNew->bInited = TRUE;

   // Do the initial allocation
   pac = SETAPPCONTEXT(0);
   AEEDSMBufMgr_Allocate(&pNew->bufmgr);
   RESTOREAPPCONTEXT(pac);

   // Link it as a system object, in case the caller doesn't cleanly free
   *((AEEVTBL(IUSBDriver)**)pNew) = &gUSBDriverVtblFuncs;

#if (MIN_BREW_VERSION(3,0))
   AEE_LinkSysObject(&pNew->cbSysObject);
#else
   AEE_LinkSysObject(&pNew->so);
#endif

   return SUCCESS;
}

// This is called in the UI task when BREW is running in system context.
// It allocates buffers up to the watermark level and places them in the 
// AvailBufs stack.
static void AEEDSMBufMgr_Allocate(AEEDSMBufMgr *pBufMgr)
{
   AEEDSMBuf *pNewBuf;

   // Allocate up to the watermark point
   while (pBufMgr->nAvailBufs < AEEDSMBUFMGR_LOW_WATERMARK)
   {
      // Make a new wrapper object
      pNewBuf = MALLOC(sizeof(*pNewBuf));
      if (pNewBuf == NULL)
      {
         break;
      }

      // Now allocate our buffer
      pNewBuf->pBuf = MALLOC(AEEDSMBUFMGR_ALLOCSIZE);
      if (pNewBuf->pBuf != NULL)
      {
         if (dsm_dup_external_buffer(&pNewBuf->pDSMItem,
                                     pNewBuf->pBuf, 
                                     AEEDSMBUFMGR_ALLOCSIZE)
                  != AEEDSMBUFMGR_ALLOCSIZE)
         {
            pNewBuf->pDSMItem = NULL;
         }
         if (pNewBuf->pDSMItem != NULL)
         {
            pNewBuf->pDSMItem->app_field = AEEDSMBUFMGR_MAGICNUM;
         }
      }

      // Cleanup and break out upon error
      if (pNewBuf->pBuf == NULL || pNewBuf->pDSMItem == NULL)
      {
         FREE(pNewBuf->pBuf);
         FREE(pNewBuf);
      }
      else
      {
         uint32 dwSave;
         INTLOCK_SAV(dwSave);
         pNewBuf->pNext = pBufMgr->pAvailBufs;
         pBufMgr->pAvailBufs = pNewBuf;
         pBufMgr->nAvailBufs++;
         INTFREE_SAV(dwSave);
      }
   }

   // Deallocate to the high watermark
   while (pBufMgr->nAvailBufs > AEEDSMBUFMGR_HIGH_WATERMARK)
   {
      uint32 dwSave;
      AEEDSMBuf *pDel;

      INTLOCK_SAV(dwSave);
      pBufMgr->nAvailBufs--;
      pDel = pBufMgr->pAvailBufs;
      pBufMgr->pAvailBufs = pDel->pNext;
      INTFREE_SAV(dwSave);

      AEEDSMBufMgr_Free(pDel);
   }
}

static AEEDSMBuf* AEEDSMMgr_DequeueByDSMPtr(AEEDSMBuf **ppList, dsm_item_type *pFind)
{
   AEEDSMBuf *pRet;

   // Find the element with the pointer to it.
   for ( ; *ppList != NULL; ppList = &(*ppList)->pNext)
   {
      if ((*ppList)->pDSMItem == pFind)
         break;
   }

   // Didn't find it?
   if (*ppList == NULL)
   {
      return NULL;
   }

   // Dequeue
   pRet = *ppList;
   *ppList = (*ppList)->pNext;

   return pRet;
}

// Does stuff after getting signaled by an ISR.  Right now only meaning
// is to kick off allocation for more memory.
void OEMUSBDriver_TaskHandler(void)
{
   int nReason;
   uint32 dwIntLock;
   IUSBDriver *pme = &gUSBDriver;

   pme->bISRCBScheduled = FALSE;

   INTLOCK_SAV(dwIntLock);
   nReason = pme->nISRReason;
   pme->nISRReason = 0;
   INTFREE_SAV(dwIntLock);

   if (nReason & USBDRIVER_ISR_SIGGOOD)
   {
      gbSigGood = TRUE;
   }

   if (!pme->bInited)
   {
      return;
   }

   if (nReason & USBDRIVER_ISR_DSMALLOC)
   {
      AEEDSMBufMgr_Allocate(&pme->bufmgr);
   }

   if (nReason & USBDRIVER_ISR_ENABLECB)
   {
      USBDriver_EnableCB(pme);
   }

   if (nReason & USBDRIVER_ISR_SETUPMSGCB)
   {
      USBDriver_SetupMsgCB(pme);
   }
}

void OEMUSBDriver_CloseNotify(int nDevice)
{
   IUSBDriver *pme = &gUSBDriver;
   int i;

   // See if its a device we're interested in
   for (i=0; i<SUBPORT_MAX; i++)
   {
      if (pme->subports[i].nSIOPort == nDevice)
      {
         break;
      }
   }
   if (i == SUBPORT_MAX)
   {
      // nope
      return;
   }
   if (pme->subports[i].nState != USBDRIVER_STATE_CLOSING)
   {
      // we thought we were closed already
      return;
   }

   // Mark it closed
   pme->subports[i].nState = USBDRIVER_STATE_CLOSED;

   // See if that was the last subport thats in the closed state
   for (i=0; i<SUBPORT_MAX; i++)
   {
      if (pme->subports[i].nState != USBDRIVER_STATE_CLOSED)
      {
         return;
      }
   }

   // If we're still in this function, all ports are now closed
   usbdc_disable_device(NULL);
}   

// gets a preallocated buffer from BREW.  This can be called from an
// ISR context.
dsm_item_type* brewdsm_new_buffer(void)
{
   AEEDSMBuf *pRet;
   uint32 dwSave;

   // Make sure we're initialized
   if (!gUSBDriver.bInited)
   {
      return NULL;
   }

   INTLOCK_SAV(dwSave);

   // Try to grab the first item
   pRet = gUSBDriver.bufmgr.pAvailBufs;
   if (pRet == NULL)
   {
      INTFREE_SAV(dwSave);

      // No available buffer, callback has already been scheduled.
      return NULL;
   }

   // Fix the lists
   gUSBDriver.bufmgr.pAvailBufs = pRet->pNext;
   pRet->pNext = gUSBDriver.bufmgr.pUsedBufs;
   gUSBDriver.bufmgr.pUsedBufs = pRet;
   gUSBDriver.bufmgr.nAvailBufs--;

   INTFREE_SAV(dwSave);

   // Schedule a callback to replentish the pool
   USBDriver_GoToBREWTask(&gUSBDriver, USBDRIVER_ISR_DSMALLOC);

   return pRet->pDSMItem;
}

// Free a dsm item.  NOT ISR SAFE.
int brewdsm_free_buffer(dsm_item_type *pDSMItem)
{
   AEEDSMBuf *pFree;
   uint32 dwSave;

   // Quick check to see if its even one of ours.
   if (pDSMItem->pool_id != DSM_DUP_ITEM_POOL ||
       pDSMItem->app_field != AEEDSMBUFMGR_MAGICNUM)
   {
      return EBADITEM;
   }

   // Search through the linked list for the free item
   INTLOCK_SAV(dwSave);
   pFree = AEEDSMMgr_DequeueByDSMPtr(&gUSBDriver.bufmgr.pUsedBufs, pDSMItem);
   INTFREE_SAV(dwSave);

   if (pFree == NULL)
   {
      // Wasn't our entry after all
      return EBADITEM;
   }

   // Add it to the available buffers, possibly free it in a non-ISR context
   INTLOCK_SAV(dwSave);
   pFree->pNext = gUSBDriver.bufmgr.pAvailBufs;
   gUSBDriver.bufmgr.pAvailBufs = pFree;
   gUSBDriver.bufmgr.nAvailBufs++;
   // Note: in certain circumstances we could cancel the allocation
   // callback here.  To avoid bugs, we'll just let the callback fire
   // and do nothing.
   INTFREE_SAV(dwSave);

   USBDriver_GoToBREWTask(&gUSBDriver, USBDRIVER_ISR_DSMALLOC);

   return SUCCESS;
}

static void AEEDSMBufMgr_Free(AEEDSMBuf *pFree)
{
   dsm_free_buffer(pFree->pDSMItem);
   FREE(pFree->pBuf);
   FREE(pFree);
}

/*========================================================================
** IUSBDriver interface functions
**======================================================================*/
static uint32 IUSBDriver_AddRef(IUSBDriver *pme)
{
   return ++pme->dwRefs;
}

static uint32 IUSBDriver_Release(IUSBDriver *pme)
{
   if (--pme->dwRefs > 0)
   {
      return pme->dwRefs;
   }

   // Destroy case.  Call callbacks
   USBDriver_Destroy(pme, FALSE);
   return 0;
}

static int IUSBDriver_QueryInterface(IUSBDriver *pme, AEECLSID cls, void **ppif)
{
   switch (cls)
   {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_USBDRIVER:
         *ppif = (void*)pme;
         IBASE_AddRef(*ppif);
         return SUCCESS;
   }
   return (pme->subports[SUBPORT_BULK].nLastErr = ECLASSNOTSUPPORT);
}

static int32 IUSBDriver_Read(IUSBDriver *pme, char *pcBuf, int32 cbBuf)
{
   int32 dwRead;
   SubPort *pSubPort;

   pSubPort = &pme->subports[SUBPORT_BULK];
   dwRead = IPORT_Read(pSubPort->piPort, pcBuf, cbBuf);
   pSubPort->nLastErr = IPORT_GetLastError(pSubPort->piPort);
   return dwRead;
}

static void IUSBDriver_Readable(IUSBDriver *pme, AEECallback *pcb)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];

   if (pSubPort->nState == USBDRIVER_STATE_OPENING)
   {
      USBDriver_CBRegister(&pSubPort->pcbReadables, pcb);
   }
   else if (pSubPort->nState == USBDRIVER_STATE_OPENED)
   {
      IPORT_Readable(pSubPort->piPort, pcb);
   }
   else
   {
      ISHELL_Resume(AEE_GetShell(), pcb);
   }
}

static int IUSBDriver_GetLastError(IUSBDriver *pme)
{
   return pme->subports[SUBPORT_BULK].nLastErr;
}

static int32 IUSBDriver_Write(IUSBDriver *pme, const char *pcBuf, int32 cbBuf)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];
   int32 dwWrote;

   dwWrote = IPORT_Write(pSubPort->piPort, pcBuf, cbBuf);
   pSubPort->nLastErr = IPORT_GetLastError(pSubPort->piPort);
   return dwWrote;
}

static void IUSBDriver_Writeable(IUSBDriver *pme, AEECallback *pcb)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];

   if (pSubPort->nState == USBDRIVER_STATE_OPENING)
   {
      USBDriver_CBRegister(&pSubPort->pcbWriteables, pcb);
   }
   else if (pSubPort->nState == USBDRIVER_STATE_OPENED)
   {
      IPORT_Writeable(pSubPort->piPort, pcb);
   }
   else
   {
      ISHELL_Resume(AEE_GetShell(), pcb);
   }
}

static int IUSBDriver_IOCtl(IUSBDriver *pme, int nOption, uint32 dwVal)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];
   return USBDriver_IOCtl_Cmn(pme, pSubPort->piPort, nOption, dwVal);
}

static int IUSBDriver_Close(IUSBDriver *pme)
{
   int i;
   SubPort *pSubPort;
   int nErr;

   // Mark each subport as going away
   // NOTE: We go backwards to work around a bug in existing versions
   //   of rdevmap where OEMSIO cannot reliably associate a deferred close
   //   with the correct port.
   for (i=SUBPORT_MAX-1; i>=0; i--)
   {
      pSubPort = &pme->subports[i];

   // Cancel callbacks
      CALLBACK_Cancel(&pSubPort->cbReady);
      if (pSubPort->nState == USBDRIVER_STATE_OPENING ||
          pSubPort->nState == USBDRIVER_STATE_OPENED)
      {
         // Mark us as closing
         nErr = EFAILED;
         pSubPort->nState = USBDRIVER_STATE_CLOSING;
         if (pSubPort->piPort != NULL)
   {
            nErr = IPORT_Close(pSubPort->piPort);
   }

         // If nErr is SUCCESS then we need to wait to actually mark the
         // port as closed.  We call our CloseNotify function, because it
         // will not call usbdc_disable() until the last port is actually
         // closed.  Note that it will change the nState value to 
         // USBDRIVER_STATE_CLOSED.
         if (nErr != SUCCESS)
   {
            OEMUSBDriver_CloseNotify(pme->subports[i].nSIOPort);
         }
   }

   }
   
   return SUCCESS;
}

static int IUSBDriver_Open(IUSBDriver *pme, const char *cpszPort)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];

   if (cpszPort == NULL)
   {
      if (pSubPort->nState == USBDRIVER_STATE_OPENING)
      {
         return AEEPORT_WAIT;
      }
      if (pSubPort->nState == USBDRIVER_STATE_OPENED)
      {
         return SUCCESS;
      }
      return EFAILED;
   }
   else if (pSubPort->nState != USBDRIVER_STATE_CLOSED)
   {
      return EBADSTATE;
   }
   else if (STRCMP(cpszPort, AEEUSBDRIVER_BULK_BIDIR))
   {
      return EUNSUPPORTED;
   }

   pSubPort->nState = USBDRIVER_STATE_OPENING;
   if (pme->bUseInterrupt)
   {
      pme->subports[SUBPORT_INT].nState = USBDRIVER_STATE_OPENING;
   }

   usbdc_enable_device(USBDC_BREW_EXTENDED_DEVICE,
                       USBDriver_SetupMsgCB_ISR,
                       USBDriver_EnableCB_ISR);

   return AEEPORT_WAIT;
}

static IPort* IUSBDriver_GetInterruptEndpoint(IUSBDriver *pme, int nLatency)
{
   IPort *pRet;

   pme->bUseInterrupt = TRUE;
   pRet = (IPort*)(&pme->pvt_int);
   IPORT_AddRef(pRet);
   return pRet;
}

static void IUSBDriver_SetSetupNotifier(IUSBDriver *pme, 
                                        PFNUSBSETUPNOTIFIER pfn,
                                        void *pUser)
{
   pme->pfnSetupReq = pfn;
   pme->pSetupData = pUser;
}

static int IUSBDriver_SendSetupResponse(IUSBDriver *pme, const byte *pResponse,
                                     int nSize)
{
   byte *pAlignedRsp;

   if((pResponse == NULL) && (nSize == 0))
   {
      //Special case. Send this to usbdc_api to cause a stall.
      usbdc_send_setup_msg_response(0, NULL);
      return SUCCESS;
   }

   if (IS_32BIT_ALIGNED(pResponse))
   {
      pAlignedRsp = (byte*)pResponse;
   }
   else
   {
      // Because the USB subsystem uses DMA, the buffer must be aligned to
      // a 32-bit boundary.  Malloc provides aligned memory.
      pAlignedRsp = MALLOC(nSize);
      if (pAlignedRsp != NULL)
      {
         MEMCPY(pAlignedRsp, pResponse, nSize);
      }
   }

   if (pAlignedRsp != NULL)
   {
      pme->bStallNeeded = FALSE;
      usbdc_send_setup_msg_response(nSize, pAlignedRsp);

      // If we allocated memory to force alignement, we can free it now.
      if (pAlignedRsp != pResponse)
      {
         FREE(pAlignedRsp);
      }
   }
   return SUCCESS;
}

static uint32 IUSBDriver_Int_AddRef(IPort *po)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);

   return IUSBDRIVER_AddRef(pme);
}

static uint32 IUSBDriver_Int_Release(IPort *po)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);

   return IUSBDRIVER_Release(pme);
}

static int IUSBDriver_Int_QueryInterface(IPort *po, AEECLSID cls, void **ppif)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);

   switch (cls)
   {
      // They want a pointer to ourselves
      case AEECLSID_QUERYINTERFACE:
         *ppif = (void*)po;
         IBASE_AddRef(*ppif);
         return SUCCESS;

      // They want a pointer to our parent
      case AEECLSID_USBDRIVER:
         *ppif = (void*)pme;
         IBASE_AddRef(*ppif);
         return SUCCESS;
   }

   return (pme->subports[SUBPORT_INT].nLastErr = ECLASSNOTSUPPORT);
}

static int32 IUSBDriver_Int_Read(IPort *po, char *pcBuf, int32 cbBuf)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];

   // Read operations are not supported on the interrupt port
   pSubPort->nLastErr = EUNSUPPORTED;
   return AEEPORT_ERROR;
}

static void IUSBDriver_Int_Readable(IPort *po, AEECallback *pcb)
{
   // We don't support reading, thus the operation does not block
   ISHELL_Resume(AEE_GetShell(), pcb);
}

static int IUSBDriver_Int_GetLastError(IPort *po)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   return pme->subports[SUBPORT_INT].nLastErr;
}

static int32 IUSBDriver_Int_Write(IPort *po, const char *pcBuf, int32 cbBuf)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];
   uint32 dwWrote;

   dwWrote = IPORT_Write(pSubPort->piPort, pcBuf, cbBuf);
   pSubPort->nLastErr = IPORT_GetLastError(pSubPort->piPort);
   return dwWrote;
}

static void IUSBDriver_Int_Writeable(IPort *po, AEECallback *pcb)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];

   if (pSubPort->nState == USBDRIVER_STATE_OPENING)
   {
      USBDriver_CBRegister(&pSubPort->pcbWriteables, pcb);
   }
   else if (pSubPort->nState == USBDRIVER_STATE_OPENED)
   {
      IPORT_Writeable(pSubPort->piPort, pcb);
   }
   else
   {
      ISHELL_Resume(AEE_GetShell(), pcb);
   }
}

static int IUSBDriver_Int_IOCtl(IPort *po, int nOption, uint32 dwVal)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];

   return USBDriver_IOCtl_Cmn(pme, pSubPort->piPort, nOption, dwVal);
}

static int IUSBDriver_Int_Close(IPort *po)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   return (pme->subports[SUBPORT_INT].nLastErr = SUCCESS);
}

static int IUSBDriver_Int_Open(IPort *po, const char *pszOpenStr)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];
   int nErr;

   if (pszOpenStr != NULL)
   {
      return (pSubPort->nLastErr = EBADSTATE);
   }

   switch (pSubPort->nState)
   {
      case USBDRIVER_STATE_OPENED:
         nErr = SUCCESS;
         break;

      case USBDRIVER_STATE_OPENING:
         nErr = AEEPORT_WAIT;
         break;

      default:
         nErr = EBADSTATE;
         break;
   }

   pSubPort->nLastErr = nErr;
   return nErr;
}
/*********************************************************************/
static int USBDriver_IOCtl_Cmn(IUSBDriver *pme, IPort *piBasePort, 
                                int nOption, uint32 dwVal)
{
   USBDriver_IOCtl_Handler *pHandler;
   
   // See if we can handle it
   FOREACH(pHandler, gUSBDriverIOCtlHandlers)
   {
      if (pHandler->nIOCtl == nOption)
      {
         break;
      }
   }

   // Did we find it?
   if (ARRAY_MEMBER(pHandler, gUSBDriverIOCtlHandlers))
   {
      // Yes
      return pHandler->pfn(pme, piBasePort, nOption, dwVal);
   }
   else if (piBasePort != NULL)
   {
      // No, but the underlying port might support it.
      return IPORT_IOCtl(piBasePort, nOption, dwVal);
   }
   
   // No
   return EUNSUPPORTED;
}

static int USBDriver_Hdl_GetCnxnStatus(IUSBDriver *pme, IPort *piBasePort, int nIOCtl, uint32 dwParam)
{
   int nErr;

   nIOCtl = AEESIO_IOCTL_GET_DTRSTATUS;

   // Always use the bulk port for connection status
   nErr = IPORT_IOCtl(pme->subports[SUBPORT_BULK].piPort, nIOCtl, dwParam);
   if (nErr == EUNSUPPORTED)
   {
      nErr = EBADSTATE;
   }
   return nErr;
}

static int USBDriver_Hdl_NtfyCnxnStatus(IUSBDriver *pme, IPort *piBasePort, int nIOCtl, uint32 dwParam)
{
   int nErr;

   // Always use the Bulk port for connecton status
   nIOCtl = AEESIO_IOCTL_DTR_NOTIFIER;
   nErr = IPORT_IOCtl(pme->subports[SUBPORT_BULK].piPort, nIOCtl, dwParam);
   if (nErr == EUNSUPPORTED)
   {
      nErr = EBADSTATE;
   }
   
   return nErr;
}

static int USBDriver_Hdl_NtfyBufEmpty(IUSBDriver *pme, IPort *piBasePort, int nIOCtl, uint32 dwParam)
{
   int nErr;

   // Pass it through to whatever port the user called it on.
   nIOCtl = AEESIO_IOCTL_FLUSH_NOTIFIER;
   nErr = IPORT_IOCtl(piBasePort, nIOCtl, dwParam);
   if (nErr == EUNSUPPORTED)
   {
      nErr = EBADSTATE;
   }
   
   return nErr;
}

/*********************************************************************/
static void USBDriver_Destroy(IUSBDriver *pme, boolean bIgnoreCB)
{
   ACONTEXT *pac;
   AEEDSMBuf *pDel;
   SubPort *pSubPort;
   int i;

   // Tell the system to stop watching us
#if (MIN_BREW_VERSION(3,0))
   CALLBACK_Cancel(&pme->cbSysObject);
#else
   AEE_UnlinkSysObject(&pme->so);
#endif

   // Ignore any unregisterd callbacks (if we were closed uncleanly)
   for (i=0; i<SUBPORT_MAX; i++)
   {
      pme->subports[i].pcbReadables = NULL;
      pme->subports[i].pcbWriteables = NULL;
   }

   // Close the device first
   IUSBDriver_Close(pme);

   // Destroy parts of each of the subports
   // NOTE: We go backwards to work around a bug in existing versions
   //   of rdevmap where OEMSIO cannot reliably associate a deferred close
   //   with the correct port.
   for (i=SUBPORT_MAX-1; i >= 0; i--)
   {
      pSubPort = &pme->subports[i];
      CALLBACK_Cancel(&pSubPort->cbReady);
      RELEASEIF(pSubPort->piPort);
   }

   // Deallocate our preallocated buffers in system context
   pac = SETAPPCONTEXT(0);
   while ((pDel = pme->bufmgr.pAvailBufs) != NULL)
   {
      pme->bufmgr.pAvailBufs = pDel->pNext;
      AEEDSMBufMgr_Free(pDel);
   }
   RESTOREAPPCONTEXT(pac);
   pme->bufmgr.nAvailBufs = 0;

   // Reset flags, release ports
   pme->bInited = FALSE;
   pme->dwRefs = 0;
}

static void USBDriver_EnableCB_ISR(void)
{
   USBDriver_GoToBREWTask(&gUSBDriver, USBDRIVER_ISR_ENABLECB);
}

static void USBDriver_EnableCB(IUSBDriver *pme)
{
   int i;

   // Initial call requires that we specify the port
   pme->subports[SUBPORT_BULK].pszOpenStr = OEMUSBDRIVER_PORT_BIBULK1;
   pme->subports[SUBPORT_INT].pszOpenStr = OEMUSBDRIVER_PORT_INTERRUPT1;

   // In the unlikely event they called close before the open was fully
   // done.  Mark things as such and revert the USB substate
   if (pme->subports[SUBPORT_BULK].nState == USBDRIVER_STATE_CLOSED)
   {
      usbdc_disable_device(NULL);
      return;
   }

   // Call our tryopen functions
   for (i=0; i<SUBPORT_MAX; i++)
   {
      AEE_APP_RESUME(&pme->subports[i].cbReady, pme->pac);
   }
}

static void USBDriver_SetupMsgCB_ISR(usbdc_setup_type *pSetupMsg)
{
   IUSBDriver *pme = &gUSBDriver;

   // Copy the setup message then flag it to be handled from within a
   // non-ISR context
   MEMCPY(&pme->SetupMsg, pSetupMsg, sizeof(pme->SetupMsg));
   USBDriver_GoToBREWTask(pme, USBDRIVER_ISR_SETUPMSGCB);
}

static void USBDriver_SetupMsgCB(IUSBDriver *pme)
{
   if (pme->pfnSetupReq == NULL)
   {
      return;
   }

   pme->bStallNeeded = TRUE;
   pme->pfnSetupReq(pme->pSetupData, &pme->SetupMsg);
   if (pme->bStallNeeded)
   {
      IUSBDriver_SendSetupResponse(pme, NULL, 0);
   }
}

// This funtion is called from within an ISR to signal that the USBDriver
// code needs something done in the UI task.
static void USBDriver_GoToBREWTask(IUSBDriver *pme, int nReason)
{
   pme->nISRReason |= nReason;

   if (!pme->bISRCBScheduled)
   {
      pme->bISRCBScheduled = TRUE;
      rex_set_sigs(pme->pTCB, AEE_USBDRIVER_SIG);
   }
}

static void USBDriver_TryOpen(void *po)
{
   SubPort *pSubPort = (SubPort*)po;
   int nErr;

   nErr = IPORT_Open(pSubPort->piPort, pSubPort->pszOpenStr);
   pSubPort->pszOpenStr = NULL;

   if (nErr == SUCCESS)
   {
      AEECallback *pcb;

      pSubPort->nState = USBDRIVER_STATE_OPENED;

      // Run all writeables, move all readables to lower level IPORT
      while (pSubPort->pcbWriteables != NULL)
      {
         pcb = pSubPort->pcbWriteables;
         CALLBACK_Cancel(pcb);
         ISHELL_Resume(AEE_GetShell(), pcb);
      }
      while (pSubPort->pcbReadables != NULL)
      {
         pcb = pSubPort->pcbReadables;
         CALLBACK_Cancel(pcb);
         IPORT_Readable(pSubPort->piPort, pcb);
      }
      return;
   }
   else if (nErr == AEEPORT_WAIT)
   {
      IPORT_Writeable(pSubPort->piPort, &pSubPort->cbReady);
   }
   else
   {
      IPORT_Close(pSubPort->piPort);
   }
}

static void USBDriver_CBRegister(AEECallback **ppList, AEECallback *pcb)
{
   CALLBACK_Cancel(pcb);
   pcb->pNext = *ppList;
   *ppList = pcb;
   pcb->pfnCancel = USBDriver_CBCancel;
   pcb->pCancelData = (void*)ppList;
   if (pcb->pNext)
   {
      pcb->pNext->pCancelData = &(pcb->pNext);
   }
}

static void USBDriver_CBCancel(AEECallback *pcb)
{
   AEECallback **ppPrev;

   ppPrev = (AEECallback**)pcb->pCancelData;
   *ppPrev = pcb->pNext;
   if (pcb->pNext)
   {
      pcb->pNext->pCancelData = (void*)ppPrev;
   }
   pcb->pNext = NULL;
   pcb->pCancelData = NULL;
   pcb->pfnCancel = NULL;
}

#if MIN_BREW_VERSION(3,1)
static void USBDriver_UncleanExit(void *po)
{
   IUSBDriver *pme = (IUSBDriver*)po;
   
   USBDriver_Destroy(pme, TRUE);
}
#endif

#endif /* FEATURE_BREW_USBDRIVER_SIO */
#endif /* FEATURE_BREW_USBDRIVER */

