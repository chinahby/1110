/*======================================================
FILE:  OEMUSBDriver.c

SERVICES:  BREW USB Driver Implementation

GENERAL DESCRIPTION:
   These are APIs to expose USB internals so that BREW applications can
   implement an arbitrary USB driver.  This code interfaces to the BREW
   directly to the USB driver.

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

#ifdef FEATURE_USB
#include "usbcdc.h"
#include "usbdc.h"
#else
#include "hsu_common.h"
#include "err.h"
#endif /* FEATURE_USB */

#include "sio.h"
#include "rdevmap.h"

#include "OEMFeatures.h"

#ifdef FEATURE_BREW_USBDRIVER

#include "AEE.h"
#include "AEEPort.h"
#include "AEEStdLib.h"
#include "BREWVersion.h"

#if MIN_BREW_VERSION(3,0)
#include "AEE_OEMDispatch.h"
#endif
#include "AEE_OEM.h"

#include "AEEUSBDriver.h"
#include "OEMUSBDriver.h"

#ifdef FEATURE_HSU_MTP
  #include "hsu_al_brewext.h"
#else
  #ifdef FEATURE_USBDRIVER_NO_USBDC_API
    #include "usbdc.h"
  #else
    #include "usbdc_api.h"
  #endif /* FEATURE_USBDRIVER_NO_USBDC_API */
#endif /* FEATURE_HSU_MTP */

#ifdef FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP
#include "AEEAppletCtl.h"          /* For checking if MTP app is running */
#include "mtph.bid"                /* For MTP app classid AEECLSID_MTPH */
#include "mtph_interface.h"        /* For MTP app close event EVT_USER_CLOSE */
#endif /* FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP */

#ifdef FEATURE_USB
  #ifdef FEATURE_USBDRIVER_STREAM_BASED
  #define USBDC_BREW_DATA_IFACE USBDC_BREW_STREAM
  #define USBDC_BREW_NOTIFICATION_IFACE USBDC_BREW_NOTIFICATION_STREAM
  typedef usbdc_stream_type usbdc_interface_enum_type;
  #endif /* FEATURE_USBDRIVER_STREAM_BASED */
#endif  /* FEATURE_USB */

//=============================================================================
//   Macro definitions
//=============================================================================

/* To overcome lint error 715:
"Symbol 'Symbol' (Location) not referenced" */
#define USE_PARAM(x) (void)x

/* To overcome lint error 818: 
"Pointer parameter 'Symbol' (Location) could be declared ptr to const" */
#define PARAM_COULD_BE_CONST(x) (void)x


// For BREW 2.1, we need to define AEE_APP_RESUME
#ifndef AEE_APP_RESUME
#define AEE_APP_RESUME(pcb,pac)      AEE_ResumeEx(pcb,0,pac)
#endif

#define USBDRIVER_MIN_WRITE_SIZE 512

// This is the number of blocks we'd like to have free at any given time
#define AEEDSMBUFMGR_LOW_WATERMARK  8
#define AEEDSMBUFMGR_HIGH_WATERMARK 12

// Max pending transfers on the Q
#define OEMUSBDRIVER_MAX_TRANFSER_Q 8

// This is the size of each block
#define AEEDSMBUFMGR_ALLOCSIZE 16384

// Magic number to figure out if a block of mem is ours or not
#define AEEDSMBUFMGR_MAGICNUM 0x6E2A8B23

// Maximum size of a host to device transfer during a request
#define OEMUSBDRIVER_MAX_OUT_CTRL_TRANSFER 65536

// USBDriver states
#define USBDRIVER_STATE_CLOSED     (1)
#define USBDRIVER_STATE_OPENED     (2)
#define USBDRIVER_STATE_OPENING    (3)
#define USBDRIVER_STATE_CLOSING    (4)
#define USBDRIVER_STATE_OPENING_TO (5)

// USBDriver ISR task signalling
#define USBDRIVER_ISR_DSMALLOC       (0x0001) // Allocate DSM buffer pool
#define USBDRIVER_ISR_ENABLECB       (0x0002) // Device configured callback
#define USBDRIVER_ISR_SETUPMSGCB     (0x0004) // Setup Message callback
#define USBDRIVER_ISR_READABLECB     (0x0008) // Trigger readable callbacks
#define USBDRIVER_ISR_CNXNSTATUSCB   (0x0010) // Triger connection sttaus callback
#define USBDRIVER_ISR_FLUSHTXCB      (0x0020) // Trigger Bulk flush Tx callback
#define USBDRIVER_ISR_INTFLUSHTXCB   (0x0040) // Trigger interrupt flush Tx callback
#define USBDRIVER_ISR_WRITE          (0x0080) // Initiate bulk Tx
#define USBDRIVER_ISR_INTWRITE       (0x0100) // Initiate interrupt Tx
#define USBDRIVER_ISR_READ           (0x0200) // Intiate Rx
#define USBDRIVER_ISR_SIGGOOD        (0x0400)
#define USBDRIVER_ISR_RESETDEVICE    (0x0800) // If the current device is not
                                              // the desired device switch
                                              // devices
#ifdef FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP
#define USBDRIVER_ISR_LAUNCHAPP      (0x1000) // Launch MTP app
#define USBDRIVER_ISR_CLOSEAPP       (0x2000) // Close MTP app
#endif // FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP

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

typedef struct AEEDSMBuf AEEDSMBuf;
typedef struct AEEDSMBufMgr AEEDSMBufMgr;
typedef struct SubPort SubPort;
typedef int (*PFNIOCTLHDLR)(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);

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
#define SUBPORT_MAX     (SUBPORT_INT+1)
struct SubPort
{
   IUSBDriver     *pParent;         // Pointer back to parent structure.
                                    // Used in callbacks.

   int             nPort;           // USBDriver port ID

   AEECallback     cbReady;         // Port is ready for use
   AEECallback    *pcbReadables;    // Readable callbacks for this port
   AEECallback    *pcbWriteables;   // Writeable callbacks for this port
   AEECallback    *pcbFlushTx;      // Flush x callbacks for this port

   boolean         bReadable;        // Flag indicates if the port has data to be read
   boolean         bWriteable;       // Flag indicates if data can be written to port
   boolean         bWriteInProgress; // Flag indicates ifi a write is in progress
   int             nLastErr;
   int             nState;
   dsm_item_type  *pRcvBuf;         // DMA buffer for received data
   int32           bRxEnabled;      // Is RX Buf setup?
   int32           nRcvSize;        // Size of the data yet to be read in the global buffer
   AEEDSMBuf      *pTxQ;            // Queue to hold write buffers yet to be tx-ed
   AEEDSMBuf      *pRxQ;            // Queue to hold write buffers yet to be tx-ed
   boolean         bStreamMode;     // Is the device in stream mode?
   char           *pCachedData;     // Data that is cached from a previous write
   int32           nCachedSize;     // Size of pCachedData
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
   AEECallback    *pcbCnxnStatus;    // Connection status callback

   // Setup msg stuff
   AEEUSBSetup_Req SetupMsg;
   PFNUSBSETUPNOTIFIER pfnSetupReq;
   void          *pSetupData;
   boolean        bStallNeeded;

   // Stuff to handle code in ISRs
   int            nISRReason;
   boolean        bInited;
   rex_tcb_type  *pTCB;

#ifdef FEATURE_HSU_MTP
   AEECallback    cbModeChange;            // RDM mode change callback
   brewext_rdm_change_status_t  rdmStatus;
#endif
#if defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)
   byte           ctrlRequestOutData[OEMUSBDRIVER_MAX_OUT_CTRL_TRANSFER];
#endif
   boolean        bInRequestCB;
};

#ifdef FEATURE_HSU_MTP
/* A boolean created to indicate when it's safe to call 
   hsu_al_brewext_unregister_rdm_callback() */
static boolean        appClosing = FALSE;
#endif

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
static void USBDriver_CBClear(AEECallback **ppList);
static void USBDriver_CBTrigger(AEECallback **ppList);
static int USBDriver_IOCtl_Cmn(IUSBDriver *pme, SubPort *pSubPort, int nOption, uint32 dwVal);
// IOCtl handlers
static int USBDriver_Hdl_GetCnxnStatus(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_NtfyCnxnStatus(IUSBDriver *pme, SubPort *pSubPortt, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_NtfyBufEmpty(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_SetStreamMode(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_GetStreamMode(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_FlushEndpoints(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_StallEndpoints(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);
static int USBDriver_Hdl_ReadCtlData(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam);

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

static void USBDriver_SetupMsgCB(IUSBDriver *pme);
static void USBDriver_EnableCB_ISR(void);
static void USBDriver_EnableCB(IUSBDriver *pme);
static void USBDriver_ProcessSig(int nSigs);
static void USBDriver_SetSig(int nReason);
static void USBDriver_GoToBREWTask(IUSBDriver *pme, int nReason);
static void USBDriver_TryOpen(void *po);

static int USBDriver_Enqueue(AEEDSMBuf **pTxQ, AEEDSMBuf *pItem);
static void USBDriver_Dequeue(AEEDSMBuf **pTxQ);
static int USBDriver_TransferQSize(AEEDSMBuf *pTxQ);
static int USBDriver_Open(SubPort *pSubPort);
static int USBDriver_Close(SubPort *pSubPort);
static void USBDriver_SetWriteStatus(SubPort *pSubPort);
static int USBDriver_ScheduleWrite(SubPort *pSubPort, const char *pcBuf, int nLen );
static void USBDriver_WriteOrFlush(SubPort *pSubPort);
static void USBDriver_RxCB(SubPort *pSubPort);
static void USBDriver_CnxnStatusCB(void);
#ifdef FEATURE_HSU_MTP
static void USBDriver_FlushTxCB(void);
static void USBDriver_IntFlushTxCB(void);
#endif /* FEATURE_HSU_MTP */
static void USBDriver_SetupRxBuf(SubPort *pSubPort);
#ifdef FEATURE_USB
static void USBDriver_USBDCBulkOpen(void);
static void USBDriver_USBDCIntOpen(void);
#endif  /* FEATURE_USB */
static void USBDriver_USBDCClose(int nPort);
static void USBDriver_USBDCSetupRxBuf(int nPort,
                                      byte *rx_buffer,
                                      word rx_buffer_size,
                                      word *rx_data_size);
static void USBDriver_USBDCWrite(int nPort, const char *pcBuf, int nLen );
static void USBDriver_USBDCTxCB(void);
static void USBDriver_USBDCTxErrCB(void);
static void USBDriver_USBDCIntTxCB(void);
static void USBDriver_USBDCIntTxErrCB(void);
#ifdef FEATURE_HSU_MTP
static void USBDriver_USBDCRxCB(mtpfd_pipe_type_t );
static void USBDriver_SetupMsgCB_ISR(void *pSetupMsg);
#else
static void USBDriver_USBDCRxCB(usbdc_interface_enum_type usbdc_iface);
static  void USBDriver_SetupMsgCB_ISR(usbdc_setup_type *pSetupMsg);
static int USBDriver_USBDCSwitchDevice(boolean bBREWMode);
#endif
static void USBDriver_USBDCRxErrCB(void);
static void USBDriver_USBDCIoctl(int nPort,
                                 int nIoctl,
                                 uint32 dwParam);
static void USBDriver_USBDCFlowControl(boolean bEnable);

#ifdef FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP
static void USBDriver_LaunchMTPApp(void);
static void USBDriver_CloseMTPApp(void);
static void USBDriver_RegisterMTPAppCB(void);
#endif // FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP

#if MIN_BREW_VERSION(3,1)
static void USBDriver_UncleanExit(void *po);
#endif

#ifdef FEATURE_HSU_MTP
static void USBDriver_Mode_Change(void *po);
static void USBDriver_RdmEvent_CB(void *po, uint32 rdm_event);
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
   {(int)AEEUSBDRIVER_IOCTL_GETCNXNSTATUS,  USBDriver_Hdl_GetCnxnStatus},
   {(int)AEEUSBDRIVER_IOCTL_NTFYCNXNSTATUS, USBDriver_Hdl_NtfyCnxnStatus},
   {(int)AEEUSBDRIVER_IOCTL_NTFYBUFEMPTY,   USBDriver_Hdl_NtfyBufEmpty},
   {(int)AEEUSBDRIVER_IOCTL_SETSTREAMMODE,  USBDriver_Hdl_SetStreamMode},
   {(int)AEEUSBDRIVER_IOCTL_GETSTREAMMODE,  USBDriver_Hdl_GetStreamMode},
   {(int)AEEUSBDRIVER_IOCTL_FLUSHENDPOINT,  USBDriver_Hdl_FlushEndpoints},
   {(int)AEEUSBDRIVER_IOCTL_STALLENDPOINT,  USBDriver_Hdl_StallEndpoints},
   {(int)AEEUSBDRIVER_IOCTL_READCTRLDATA,   USBDriver_Hdl_ReadCtlData},
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

static boolean gDesiredUSBMode = FALSE;
static int gUSBState = USBDRIVER_STATE_CLOSED;


//=============================================================================
//   Function definitions
//=============================================================================

// Initialize the buffer manager subsystem
void OEMUSBDriver_Init(IShell *piShell)
{
   IUSBDriver *pme = (IUSBDriver*)&gUSBDriver;
   int i;

   (void)MEMSET(pme, 0, sizeof(IUSBDriver));
   for (i=0; i<SUBPORT_MAX; i++)
   {
      SubPort *pSubPort = &pme->subports[i];
      pSubPort->pParent = pme;
      pSubPort->nPort = i;
      CALLBACK_Init(&pSubPort->cbReady, USBDriver_TryOpen, pSubPort);
      
      pSubPort->nState = USBDRIVER_STATE_CLOSED;
      pSubPort->bReadable = FALSE;
      pSubPort->bWriteable = FALSE;
      pSubPort->bWriteInProgress = FALSE;
      pSubPort->nRcvSize = 0;
      pSubPort->pTxQ = NULL;
      pSubPort->pRxQ = NULL;
      pSubPort->bStreamMode = FALSE;
      pSubPort->pCachedData = NULL;
      pSubPort->nCachedSize = 0;
   }

#ifdef FEATURE_HSU_MTP
   CALLBACK_Init(&pme->cbModeChange, USBDriver_Mode_Change, (void*)pme);
#endif
 
   pme->pvt_int = &gUSBDriverIntVtblFuncs;

   // Make sure our signal is properly registered
   pme->pTCB = rex_self();
   USBDriver_GoToBREWTask(pme, USBDRIVER_ISR_SIGGOOD);

#if MIN_BREW_VERSION(3,1)
   // Initialize our cleanup stuff
   CALLBACK_Init(&pme->cbSysObject, USBDriver_UncleanExit, (void*)pme);
#endif 

#ifdef FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP
   // Register with usb driver to get notification when mtp state changes
   USBDriver_RegisterMTPAppCB();
#endif // FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP

   USE_PARAM (piShell);
   PARAM_COULD_BE_CONST(piShell);
}


#ifdef FEATURE_HSU_MTP
/*==============================================================================

FUNCTION      USBDriver_Mode_Change

DESCRIPTION   
    This is a wrapper function for the adaptation layer function 
    hsu_al_brewext_handle_mode_change().
    
RETURN VALUE  
    None 

SIDE EFFECTS  
    None

==============================================================================*/
static void USBDriver_Mode_Change(void *po)
{

   IUSBDriver *pme = (IUSBDriver *)po;
   if (pme == NULL)
   {
      return;
   }
  (void)hsu_al_brewext_handle_mode_change (pme->rdmStatus, appClosing);
}
#endif   /* FEATURE_HSU_MTP  */

// Class instantiation.  Works only once per run
int OEMUSBDriver_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   IUSBDriver *pNew = &gUSBDriver;
   ACONTEXT *pac;

#ifdef FEATURE_HSU_MTP
   // Init the flag to indicate when to call unregister rdm callback
   appClosing = FALSE;
#endif   /* FEATURE_HSU_MTP  */
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
   *((AEEVTBL(IUSBDriver)**)((void *)pNew)) = &gUSBDriverVtblFuncs;

#if (MIN_BREW_VERSION(3,0))
   AEE_LinkSysObject(&pNew->cbSysObject);
#else
   AEE_LinkSysObject(&pNew->so);
#endif

   USE_PARAM(cls);
   USE_PARAM (piShell);
   PARAM_COULD_BE_CONST(piShell);
 
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

   USE_PARAM(pFind);
   
   return pRet;
}

// Does stuff after getting signaled by an ISR.
void OEMUSBDriver_TaskHandler(void)
{
   int nReason;
   uint32 dwIntLock;
   IUSBDriver *pme = &gUSBDriver;


   INTLOCK_SAV(dwIntLock);
   nReason = pme->nISRReason;
   pme->nISRReason = 0;
   INTFREE_SAV(dwIntLock);

   if (nReason & USBDRIVER_ISR_SIGGOOD)
   {
      gbSigGood = TRUE;
   }
   USBDriver_ProcessSig(nReason);
}

// Process our signals
static void USBDriver_ProcessSig(int nSigs)
{
   IUSBDriver *pme = &gUSBDriver;

#ifdef FEATURE_USB
   // Change device mode
   if (nSigs & USBDRIVER_ISR_RESETDEVICE)
   {
      USBDriver_USBDCSwitchDevice(gDesiredUSBMode);
   }
#endif /* FEATURE_USB */

   // Handle MTP app launch/close requests even if USBDriver instance is not created
   #ifdef FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP
   if (nSigs & USBDRIVER_ISR_LAUNCHAPP) {
     USBDriver_LaunchMTPApp();
   }

   if (nSigs & USBDRIVER_ISR_CLOSEAPP) {
     USBDriver_CloseMTPApp();
   }
   #endif // FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP

   if (!pme->bInited)
   {
      return;
   }

   // Allocate dsm buffer pool
   if (nSigs & USBDRIVER_ISR_DSMALLOC)
   {
      AEEDSMBufMgr_Allocate(&pme->bufmgr);

      // We might have free buffers now, set up rx buffer if not already set
      USBDriver_SetupRxBuf(&pme->subports[SUBPORT_BULK]);

      // If we have free buffers, enable writeable for bulk and interrupt ports
      USBDriver_SetWriteStatus(&pme->subports[SUBPORT_BULK]);
      USBDriver_SetWriteStatus(&pme->subports[SUBPORT_INT]);

   }

   // Device configured callback
   if (nSigs & USBDRIVER_ISR_ENABLECB)
   {
      USBDriver_EnableCB(pme);
   }

   // Setup message callback
   if (nSigs & USBDRIVER_ISR_SETUPMSGCB)
   {
      USBDriver_SetupMsgCB(pme);
   }

   // Trigger connection status callback
   if (nSigs & USBDRIVER_ISR_CNXNSTATUSCB)
   {
      USBDriver_CBTrigger(&pme->pcbCnxnStatus);
   }

   // Setup bulk Rx buffer and copy over any received data
   if (nSigs & USBDRIVER_ISR_READ)
   {
      USBDriver_RxCB(&pme->subports[SUBPORT_BULK]);
   }

   // Initiate Bulk Tx
   if (nSigs & USBDRIVER_ISR_WRITE)
   {
      // Dequeue the transmitted data
      USBDriver_Dequeue(&pme->subports[SUBPORT_BULK].pTxQ);
      pme->subports[SUBPORT_BULK].bWriteInProgress = FALSE;
      // Process any scheduled write requests
      USBDriver_WriteOrFlush(&pme->subports[SUBPORT_BULK]);
   }

   // Initiate interrupt Tx
   if (nSigs & USBDRIVER_ISR_INTWRITE)
   {
      // Dequeue the transmitted data
      USBDriver_Dequeue(&pme->subports[SUBPORT_INT].pTxQ);

      pme->subports[SUBPORT_INT].bWriteInProgress = FALSE;
      // Process any scheduled write requests
      USBDriver_WriteOrFlush(&pme->subports[SUBPORT_INT]);
   }

   // Trigger readable callbacks
   if (nSigs & USBDRIVER_ISR_READABLECB)
   {
      USBDriver_CBTrigger(&pme->subports[SUBPORT_BULK].pcbReadables);
   }

   // Trigger bulk flush tx callbacks
   if (nSigs & USBDRIVER_ISR_FLUSHTXCB)
   {
      USBDriver_CBTrigger(&pme->subports[SUBPORT_BULK].pcbFlushTx);
   }

   // Trigger interrupt flush tx callbacks
   if (nSigs & USBDRIVER_ISR_INTFLUSHTXCB)
   {
      USBDriver_CBTrigger(&pme->subports[SUBPORT_INT].pcbFlushTx);
   }
}

void OEMUSBDriver_CloseNotify(int nPort)
{
   IUSBDriver *pme = &gUSBDriver;
   int i=0;

   if (nPort >= SUBPORT_MAX)
   {
      return;
   }
   if (pme->subports[nPort].nState != USBDRIVER_STATE_CLOSING)
   {
      // we thought we were closed already
      return;
   }

   // Mark it closed
   pme->subports[nPort].nState = USBDRIVER_STATE_CLOSED;

   // See if that was the last subport thats in the closed state
   for (i=0; i<SUBPORT_MAX; i++)
   {
      if (pme->subports[i].nState != USBDRIVER_STATE_CLOSED)
      {
         return;
      }
   }

   // If we're still in this function, all ports are now closed
#ifdef FEATURE_HSU_MTP
	// do nothing here since hsu_al_brewext_disconnect() taken care of everytthing.

#else
   // If we're still in this function, all ports are now closed
   USBDriver_USBDCSwitchDevice(FALSE);
#endif  /* FEATURE_HSU_MTP */
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

   (void)dsm_free_buffer(pFree->pDSMItem);
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
         (void)IBASE_AddRef(*ppif);
         return SUCCESS;
      default:
         break;
   }
   return (pme->subports[SUBPORT_BULK].nLastErr = ECLASSNOTSUPPORT);
}

static int32 IUSBDriver_Read(IUSBDriver *pme, char *pcBuf, int32 cbBuf)
{
   int32 dwRead=0, dwCnt;
   SubPort *pSubPort;
   dsm_item_type *pRead;

   // Rx is only supported for the bulk port
   pSubPort = &pme->subports[SUBPORT_BULK];

   // No data? Turn off readable
   if (pSubPort->nRcvSize == 0)
   {
      pSubPort->bReadable = FALSE;
      return AEEPORT_WAIT;
   }

   // Check for good buffer and non-zero request size
   if (!pcBuf || cbBuf == 0) {
      return 0;
   }

#ifdef HSU_MTP_DEBUG_MSG_ON
   mtp_debug_append('R');
#endif
   
   // We do not want to read more than what we have or what the app asks for
   cbBuf = MIN(cbBuf, pSubPort->nRcvSize);

   // Loop through the Q and copy data over to app's buffer
   while (cbBuf > 0 && 
          USBDriver_TransferQSize(pSubPort->pRxQ) > 0)
   {
      // Get the first dsm buffer from the Q
      pRead = pSubPort->pRxQ->pDSMItem;

      if (!pRead || pRead->used == 0) {
         // Error: dsm buffer pointer is NULL or it holds no data
         // Skip this and move on to next entry
         USBDriver_Dequeue(&pSubPort->pRxQ);
         continue;
      }

      // Copy over the data and adjust data size counters
      dwCnt = MIN(cbBuf, pRead->used);
      MEMCPY(pcBuf+dwRead, pSubPort->pRxQ->pBuf, dwCnt);
      pSubPort->nRcvSize -= dwCnt; // Byte count of data yet to be read by app
      pRead->used -= (uint16)dwCnt; // Byte count of data yet to be read from the dsm buffer
      cbBuf -= dwCnt; // Bytes left in the app buffer
      dwRead += dwCnt; // Total bytes copied to app's buffer
      // Advance the data pointer based on how read we read from the dsm buffer
      // If we did not read all of teh dsm buffer's data, this pointer will
      // be used the next time around.
      pSubPort->pRxQ->pBuf = (void *)((byte *)pSubPort->pRxQ->pBuf + dwCnt);

      // If all the data from teh dsm buffer has been read, dequeue it
      if (pRead->used == 0) {
         USBDriver_Dequeue(&pSubPort->pRxQ);
      }
   }

   // If we emptied out the read data, turn off readable
   if (pSubPort->nRcvSize == 0)
   {
      pSubPort->bReadable = FALSE;
   }

   return (dwRead);
}

static void IUSBDriver_Readable(IUSBDriver *pme, AEECallback *pcb)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];

   // If we are in open or opening state and our readable flag is disabled
   // register the callback, so we can trigger it later when we have data
   // to be read.
   if ((pSubPort->nState == USBDRIVER_STATE_OPENING ||
        pSubPort->nState == USBDRIVER_STATE_OPENED) &&
       pSubPort->bReadable == FALSE)
   {
      USBDriver_CBRegister(&pSubPort->pcbReadables, pcb);
   }
   else
   {
      AEE_APP_RESUME(pcb, gUSBDriver.pac);
   }
   USE_PARAM(pme);
}

static int IUSBDriver_GetLastError(IUSBDriver *pme)
{
   PARAM_COULD_BE_CONST(pme);
   return pme->subports[SUBPORT_BULK].nLastErr;
}

static int32 IUSBDriver_Write(IUSBDriver *pme, const char *pcBuf, int32 cbBuf)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];
   int32 dwWrote;

   // Check for right state
   if (pSubPort->nState != USBDRIVER_STATE_OPENED) {
      return AEEPORT_CLOSED;
   }
   else if (pSubPort->nState == USBDRIVER_STATE_OPENING ||
            pSubPort->bWriteable == FALSE) { // Can we handle any more writes?
      return AEEPORT_WAIT;
   }

   dwWrote = USBDriver_ScheduleWrite(pSubPort, pcBuf, cbBuf);
   return dwWrote;
}

static void IUSBDriver_Writeable(IUSBDriver *pme, AEECallback *pcb)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];

   // If we are in open or opening state and our writeable flag is disabled
   // register the callback, so we can trigegr it later when we are ready for
   // write
   if ((pSubPort->nState == USBDRIVER_STATE_OPENING ||
        pSubPort->nState == USBDRIVER_STATE_OPENED) &&
       pSubPort->bWriteable == FALSE)
   {
      USBDriver_CBRegister(&pSubPort->pcbWriteables, pcb);
   }
   else
   {
      AEE_APP_RESUME(pcb, gUSBDriver.pac);
   }
}

static int IUSBDriver_IOCtl(IUSBDriver *pme, int nOption, uint32 dwVal)
{
   int nErr;
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];
   nErr = USBDriver_IOCtl_Cmn(pme, pSubPort, nOption, dwVal);
   return nErr;
}

static int IUSBDriver_Close(IUSBDriver *pme)
{
   int i;
   SubPort *pSubPort;
   int nErr=SUCCESS;
#ifdef FEATURE_HSU_MTP
   boolean openState = FALSE;
#endif /*defined FEATURE_HSU_MTP*/

   // Mark each subport as going away
   // NOTE: We go backwards to work around a bug in existing versions
   //   of rdevmap where OEMSIO cannot reliably associate a deferred close
   //   with the correct port.
   for (i=SUBPORT_MAX-1; i>=0; i--)
   {
      pSubPort = &pme->subports[i];

      // Cancel callbacks
      CALLBACK_Cancel(&pSubPort->cbReady);
      USBDriver_CBClear(&pSubPort->pcbWriteables);
      USBDriver_CBClear(&pSubPort->pcbReadables);
      USBDriver_CBClear(&pSubPort->pcbFlushTx);
      if (pSubPort->nState == USBDRIVER_STATE_OPENING ||
          pSubPort->nState == USBDRIVER_STATE_OPENED)
      {
#ifdef FEATURE_HSU_MTP
         // set the flag for later use
         openState = TRUE;
#endif /*defined FEATURE_HSU_MTP*/
         nErr = USBDriver_Close(pSubPort);
      }
   }
   
#ifdef FEATURE_HSU_MTP
   if (openState)
   {
      (void)hsu_al_brewext_disconnect();
      (void)hsu_al_brewext_unregister_app_cbs();
   }
#endif  /* FEATURE_HSU_MTP */
   
   return nErr;
}

static int IUSBDriver_Open(IUSBDriver *pme, const char *cpszPort)
{
   SubPort *pSubPort = &pme->subports[SUBPORT_BULK];
   int status;

   do {
      if (cpszPort == NULL)
      {
         if (pSubPort->nState == USBDRIVER_STATE_OPENING)
         {
            status = AEEPORT_WAIT;
            break;
         }
         if (pSubPort->nState == USBDRIVER_STATE_OPENED)
         {
            status = SUCCESS;
            break;
         }
         status = EFAILED;
         break;
      }
      else if (pSubPort->nState != USBDRIVER_STATE_CLOSED)
      {
         status = EBADSTATE;
         break;
      }
      else if (STRCMP(cpszPort, AEEUSBDRIVER_BULK_BIDIR))
      {
         status = EUNSUPPORTED;
         break;
      }

      pSubPort->nState = USBDRIVER_STATE_OPENING;
      if (pme->bUseInterrupt)
      {
         pme->subports[SUBPORT_INT].nState = USBDRIVER_STATE_OPENING;
      }
#ifdef FEATURE_HSU_MTP
      // reset the debug buffer
      (void)MEMSET(mtp_debug_buf, 0, MTP_DEBUG_COUNT);
      mtp_debug_count = 0;


      /* Register rdm callback everytime entering the application. And unregister
         rdm callback in USBDriver_Destroy() initially. But since IUSBDriver_Close()
         inside USBDriver_Destroy() is async, we only unregister call back when comp
         switch is done. That's why we need a global flag appClosing and unregister 
         is done in hsu_al_brewext_handle_mode_change().
      */
      (void)hsu_al_brewext_register_rdm_callback((void *)pme, 
                                    (brewext_notify_cb_t)USBDriver_RdmEvent_CB);

      (void)hsu_al_brewext_register_app_cbs(USBDriver_EnableCB_ISR,
                                      USBDriver_USBDCTxCB,
                                      USBDriver_USBDCRxCB,
                                      USBDriver_USBDCIntTxCB,
                                      USBDriver_USBDCTxErrCB,
                                      USBDriver_USBDCRxErrCB,
                                      USBDriver_USBDCIntTxErrCB, 
                                      USBDriver_SetupMsgCB_ISR); 
       (void)hsu_al_brewext_connect();   

       status = AEEPORT_WAIT;

#else
      status = USBDriver_USBDCSwitchDevice(TRUE);

      if(status == SUCCESS)
      {
         int i;
         // Call our tryopen functions
         for (i=0; i<SUBPORT_MAX; i++)
         {
            AEE_APP_RESUME(&pme->subports[i].cbReady, pme->pac);
         }
      }
#endif  /* FEATURE_HSU_MTP */
   } while (0);
   return status;
}

#ifdef FEATURE_HSU_MTP
/*==============================================================================

FUNCTION      USBDriver_RdmEvent_CB

DESCRIPTION   
    This is a rdm callback func. It's registered everytime IUSBDriver_Open() is 
    called. This is done via hsu_al_brewext_register_rdm_callback(). Unregister 
    is done in IUSBDriver_Close().

RETURN VALUE  
    None 

SIDE EFFECTS  
    None

==============================================================================*/
static void USBDriver_RdmEvent_CB(void *po, uint32 rdm_event)
{
   IUSBDriver *pme = (IUSBDriver *)po;

   if (pme == NULL)
   {
      return;
   }
   pme->rdmStatus = (brewext_rdm_change_status_t) rdm_event;

   if (pme->subports[SUBPORT_BULK].nState == USBDRIVER_STATE_CLOSING ||
       pme->subports[SUBPORT_BULK].nState == USBDRIVER_STATE_CLOSED  ||
       pme->subports[SUBPORT_INT].nState  == USBDRIVER_STATE_CLOSING ||
       pme->subports[SUBPORT_INT].nState  == USBDRIVER_STATE_CLOSED)
   {
      // App is going away, instead of calling APP_RESUME, call the actual func.
      USBDriver_Mode_Change(po);
   }
   else
   {	 
      AEE_APP_RESUME(&pme->cbModeChange, pme->pac);
   }
}
#endif /* FEATURE_HSU_MTP */

static IPort* IUSBDriver_GetInterruptEndpoint(IUSBDriver *pme, int nLatency)
{
   IPort *pRet;

   USE_PARAM(nLatency);
   
   pme->bUseInterrupt = TRUE;
   pRet = (IPort*)(&(pme->pvt_int));
   (void)IPORT_AddRef(pRet);
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

   if(!pme->bInRequestCB)
   {
      return EBADSTATE;
   }

   pme->bStallNeeded = FALSE;


#if defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)
   if((0 == (pme->SetupMsg.wRequestAndType & 0x0080)) ||
      (0 == pme->SetupMsg.wLength))
   {
      if((nSize == sizeof(int)) && 
         (pResponse != NULL) &&
         (*(int *)pResponse == SUCCESS))
      {
         int *pnData = (int *)pResponse;
         pnData = USBDC_API_SUCCESS;
      }
      else
      {
         pResponse = NULL;
         nSize = 0;
      }
   }
#endif /*#defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)*/


   if((pResponse == NULL) && (nSize == 0))
   {
#ifdef FEATURE_USB

      //Special case. Send this to usbdc to cause a stall.
#ifdef FEATURE_USBDRIVER_NO_USBDC_API
      usbdc_send_setup_msg_response(0, NULL);
#else
      usbdc_api_send_setup_msg_response(USBDC_BREW_EXTENDED_DEVICE, 
                                        0, NULL);
#endif /* FEATURE_USBDRIVER_NO_USBDC_API */

#endif /* FEATURE_USB */
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
      pAlignedRsp = MALLOC((dword)nSize);
      if (pAlignedRsp != NULL)
      {
         (void)MEMCPY(pAlignedRsp, pResponse, nSize);
      }
   }

   if (pAlignedRsp != NULL)
   {
#ifdef FEATURE_HSU_MTP
      hsu_al_brewext_send_setup_msg_response(nSize, pAlignedRsp);
#else
  #ifdef FEATURE_USBDRIVER_NO_USBDC_API
      usbdc_send_setup_msg_response(nSize, pAlignedRsp);
  #else
      usbdc_api_send_setup_msg_response(USBDC_BREW_EXTENDED_DEVICE, 
                                        nSize, pAlignedRsp);
  #endif /* FEATURE_USBDRIVER_NO_USBDC_API */
#endif /* FEATURE_HSU_MTP */

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
   IUSBDriver *pme = INTPORT_TO_USBDRIVER((void *)po);

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
         (void)IBASE_AddRef(*ppif);
         return SUCCESS;

      // They want a pointer to our parent
      case AEECLSID_USBDRIVER:
         *ppif = (void*)pme;
         (void)IBASE_AddRef(*ppif);
         return SUCCESS;
   }

   return (pme->subports[SUBPORT_INT].nLastErr = ECLASSNOTSUPPORT);
}

static int32 IUSBDriver_Int_Read(IPort *po, char *pcBuf, int32 cbBuf)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];

   USE_PARAM(pcBuf);
   USE_PARAM(cbBuf);
   PARAM_COULD_BE_CONST(pcBuf);
   
   // Read operations are not supported on the interrupt port
   pSubPort->nLastErr = EUNSUPPORTED;
   return AEEPORT_ERROR;
}

static void IUSBDriver_Int_Readable(IPort *po, AEECallback *pcb)
{
   // We don't support reading, thus the operation does not block
   AEE_APP_RESUME(pcb, gUSBDriver.pac);
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
   int32 dwWrote;

   // Check for right state
   if (pSubPort->nState == USBDRIVER_STATE_OPENING) {
      return AEEPORT_WAIT;
   }
   else if (pSubPort->nState != USBDRIVER_STATE_OPENED) {
      return AEEPORT_CLOSED;
   }

   // Can we handle any more writes?
   if (pSubPort->bWriteable == FALSE) {
      return AEEPORT_WAIT;
   }

   dwWrote = USBDriver_ScheduleWrite(pSubPort, pcBuf, cbBuf);
   return dwWrote;
}

static void IUSBDriver_Int_Writeable(IPort *po, AEECallback *pcb)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];


   // If we are in open or opening state and our writeable flag is disabled
   // register the callback, so we can trigegr it later when we are ready for
   // write
   if ((pSubPort->nState == USBDRIVER_STATE_OPENING ||
        pSubPort->nState == USBDRIVER_STATE_OPENED) &&
       pSubPort->bWriteable == FALSE)
   {
      USBDriver_CBRegister(&pSubPort->pcbWriteables, pcb);
   }
   else
   {
      AEE_APP_RESUME(pcb, gUSBDriver.pac);
   }
}

static int IUSBDriver_Int_IOCtl(IPort *po, int nOption, uint32 dwVal)
{
   IUSBDriver *pme = INTPORT_TO_USBDRIVER(po);
   SubPort *pSubPort = &pme->subports[SUBPORT_INT];

   return USBDriver_IOCtl_Cmn(pme, pSubPort, nOption, dwVal);
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
static int USBDriver_IOCtl_Cmn(IUSBDriver *pme, SubPort *pSubPort, 
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
      return pHandler->pfn(pme, pSubPort, nOption, dwVal);
   }
   else
   {
      // TODO: How are we goign to handle it? Does usbdc handle it?
   }
   
   // No
   return EUNSUPPORTED;
}

static int USBDriver_Hdl_GetCnxnStatus(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam)
{
   if (pSubPort->nState != USBDRIVER_STATE_OPENED) {
      return EBADSTATE;
   }
   USBDriver_USBDCIoctl(SUBPORT_BULK, nIOCtl, dwParam);
   return SUCCESS;
}

static int USBDriver_Hdl_NtfyCnxnStatus(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam)
{
   // Register callback
   USBDriver_CBRegister(&pme->pcbCnxnStatus, (AEECallback *)dwParam);
   if (pSubPort->nState == USBDRIVER_STATE_OPENED) {
      USBDriver_USBDCIoctl(SUBPORT_BULK, nIOCtl, (uint32)USBDriver_CnxnStatusCB);
   }
   return SUCCESS;
}

static int USBDriver_Hdl_NtfyBufEmpty(IUSBDriver *pme, SubPort *pSubPort, int nIOCtl, uint32 dwParam)
{
   // Set up flush transmit callback
   USBDriver_CBRegister(&pSubPort->pcbFlushTx, (AEECallback *)dwParam);
   USBDriver_WriteOrFlush(pSubPort);
   
   return SUCCESS;
}

static int USBDriver_Hdl_SetStreamMode(IUSBDriver *pme, SubPort *pSubPort, 
                                          int nIOCtl, uint32 dwParam)
{
#ifdef FEATURE_USB
   if((dwParam != TRUE) && (dwParam != FALSE))
   {
      return EBADPARM;
   }

   pSubPort->bStreamMode = dwParam;
   return SUCCESS;
#else
   return EUNSUPPORTED;

#endif /* FEATURE_USB */

}

static int USBDriver_Hdl_GetStreamMode(IUSBDriver *pme, SubPort *pSubPort,
                                          int nIOCtl, uint32 dwParam) 
{
#ifdef FEATURE_USB
   boolean *pbStreamMode = (boolean *)dwParam;
   *pbStreamMode = pSubPort->bStreamMode;
   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif /* FEATURE_USB */
}

static int USBDriver_Hdl_FlushEndpoints(IUSBDriver *pme, SubPort *pSubPort,
                                        int nIOCtl, uint32 dwParam) 
{
   int nErr = EUNSUPPORTED;
   if(pSubPort->nPort == SUBPORT_BULK)
   {
      if((dwParam != AEEUSBDRIVER_BM_INENDPOINT) &&
         (dwParam != AEEUSBDRIVER_BM_OUTENDPOINT) &&
         (dwParam != (AEEUSBDRIVER_BM_INENDPOINT|AEEUSBDRIVER_BM_OUTENDPOINT)))
      {
         return EBADPARM;
      }
   }
   else if(dwParam != AEEUSBDRIVER_BM_INENDPOINT)
   {
      return EBADPARM;
   }

#ifdef FEATURE_USB
   if(dwParam & AEEUSBDRIVER_BM_INENDPOINT)
   {
      //Flush Tx Data
      usbdc_interface_enum_type usbdc_iface;
      usbdc_iface = ((pSubPort->nPort == SUBPORT_BULK) ?
                     USBDC_BREW_DATA_IFACE : 
                     USBDC_BREW_NOTIFICATION_IFACE);

      while(pSubPort->pTxQ)
      {
         USBDriver_Dequeue(&pSubPort->pTxQ);
      }

      if(NULL != pSubPort->pCachedData)
      {
         FREE(pSubPort->pCachedData);
      }
      pSubPort->nCachedSize = 0;

#ifdef FEATURE_USBDRIVER_NO_USBDC_API
      usbdc_ioctl(usbdc_iface, USBDC_ABORT_WRITE, NULL);
#else
      usbdc_api_ioctl(usbdc_iface, USBDC_ABORT_WRITE, NULL);
#endif /* FEATURE_USBDRIVER_NO_USBDC_API */

      pSubPort->bWriteInProgress = FALSE;
      USBDriver_WriteOrFlush(pSubPort);
      nErr = SUCCESS;
   }

   if(dwParam & AEEUSBDRIVER_BM_OUTENDPOINT)
   {
      //Flush Rx Data
      usbdc_interface_enum_type usbdc_iface;
      usbdc_iface = ((pSubPort->nPort == SUBPORT_BULK) ?
                     USBDC_BREW_DATA_IFACE : 
                     USBDC_BREW_NOTIFICATION_IFACE);

      #ifdef FEATURE_USBDRIVER_NO_USBDC_API
            usbdc_ioctl(usbdc_iface, USBDC_ABORT_READ, NULL);
      #else
            usbdc_api_ioctl(usbdc_iface, USBDC_ABORT_READ, NULL);
      #endif /* FEATURE_USBDRIVER_NO_USBDC_API */

       //Clear the RXCB sig
      pme->nISRReason &= ~USBDRIVER_ISR_READ;

      (void)brewdsm_free_buffer(pSubPort->pRcvBuf);
      pSubPort->pRcvBuf = NULL;

      while(pSubPort->pRxQ)
      {
         USBDriver_Dequeue(&pSubPort->pRxQ);
      }

      pSubPort->bRxEnabled = FALSE;
      USBDriver_SetupRxBuf(pSubPort);
   
      // turn off readable
      pSubPort->bReadable = FALSE;
      nErr = SUCCESS;
   }

   return nErr;

#else
   return nErr;
#endif
}

static int USBDriver_Hdl_StallEndpoints(IUSBDriver *pme, SubPort *pSubPort,
                                        int nIOCtl, uint32 dwParam) 
{
   int nErr = EUNSUPPORTED;

   if(pSubPort->nPort == SUBPORT_BULK)
   {
      if((dwParam != AEEUSBDRIVER_BM_INENDPOINT) &&
         (dwParam != AEEUSBDRIVER_BM_OUTENDPOINT) &&
         (dwParam != (AEEUSBDRIVER_BM_INENDPOINT|AEEUSBDRIVER_BM_OUTENDPOINT)))
      {
         return EBADPARM;
      }
   }
   else if(dwParam != AEEUSBDRIVER_BM_INENDPOINT)
   {
      return EBADPARM;
   }

#ifdef FEATURE_USB

   if(dwParam & AEEUSBDRIVER_BM_INENDPOINT)
   {
      //Stall Tx
      usbdc_interface_enum_type usbdc_iface;
      usbdc_iface = ((pSubPort->nPort == SUBPORT_BULK) ?
                     USBDC_BREW_DATA_IFACE : 
                     USBDC_BREW_NOTIFICATION_IFACE);
      #ifdef FEATURE_USBDRIVER_NO_USBDC_API
         usbdc_stall_tx_ep(usbdc_iface);
      #else
         usbdc_api_stall_tx_ep(usbdc_iface);
      #endif /*FEATURE_USBDRIVER_NO_USBDC_API*/
      nErr = SUCCESS;
   }

   if(dwParam & AEEUSBDRIVER_BM_OUTENDPOINT)
   {
      //Stall Rx
      usbdc_interface_enum_type usbdc_iface;
      usbdc_iface = ((pSubPort->nPort == SUBPORT_BULK) ?
                     USBDC_BREW_DATA_IFACE : 
                     USBDC_BREW_NOTIFICATION_IFACE);
      #ifdef FEATURE_USBDRIVER_NO_USBDC_API
         usbdc_stall_rx_ep(usbdc_iface);
      #else
         usbdc_api_stall_rx_ep(usbdc_iface);
      #endif /*FEATURE_USBDRIVER_NO_USBDC_API*/
      nErr = SUCCESS;
   }

   return nErr;

#else
   return nErr;
#endif
}


static int USBDriver_Hdl_ReadCtlData(IUSBDriver *pme, SubPort *pSubPort,
                                     int nIOCtl, uint32 dwParam) 
{
   AEEUSBDriver_ReadType *pReadStruct = (AEEUSBDriver_ReadType *)dwParam;

   if (NULL == pReadStruct)
   {
      return EBADPARM;
   }

   if(!pme->bInRequestCB)
   {
      return EBADSTATE;
   }

   if((0 != (pme->SetupMsg.wRequestAndType & 0x0080)) ||
      (0 == pme->SetupMsg.wLength))
   {
      return EBADSTATE;
   }

   if((NULL == pReadStruct->pData) ||
      (pReadStruct->nSize > pme->SetupMsg.wLength))
   {
      return EBADPARM;
   }

#if defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)
   MEMCPY(pReadStruct->pData, pme->ctrlRequestOutData, pReadStruct->nSize);
   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}

/*********************************************************************/
static void USBDriver_Destroy(IUSBDriver *pme, boolean bIgnoreCB)
{
   ACONTEXT *pac;
   AEEDSMBuf *pDel;
   SubPort *pSubPort;
   int i;

#ifdef FEATURE_HSU_MTP
   // App is going away, set the flag for unregister later
   appClosing = TRUE;
#endif /*defined FEATURE_HSU_MTP*/

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
      pme->subports[i].pcbFlushTx = NULL;
   }

   // Close the device first
   (void)IUSBDriver_Close(pme);

   // Destroy parts of each of the subports
   // NOTE: We go backwards to work around a bug in existing versions
   //   of rdevmap where OEMSIO cannot reliably associate a deferred close
   //   with the correct port.
   for (i=SUBPORT_MAX-1; i >= 0; i--)
   {
      pSubPort = &pme->subports[i];
      CALLBACK_Cancel(&pSubPort->cbReady);
   }

#ifdef FEATURE_HSU_MTP
   CALLBACK_Cancel(&pme->cbModeChange);
#endif /*defined FEATURE_HSU_MTP*/
 
   // Deallocate our preallocated buffers in system context
   pac = SETAPPCONTEXT(0);
   while ((pDel = pme->bufmgr.pAvailBufs) != NULL)
   {
      pme->bufmgr.pAvailBufs = pDel->pNext;
      AEEDSMBufMgr_Free(pDel);
   }

   // Deallocate our used buffers in system context
   while ((pDel = pme->bufmgr.pUsedBufs) != NULL)
   {
      pme->bufmgr.pUsedBufs = pDel->pNext;

      //AEEDSMBufMgr_Free(pDel);
      (void)dsm_free_buffer(pDel->pDSMItem);
      FREE(pDel->pBuf);
      FREE(pDel);
   }
   
   RESTOREAPPCONTEXT(pac);
   pme->bufmgr.nAvailBufs = 0;

   // Reset flags, release ports
   pme->bInited = FALSE;
   pme->dwRefs = 0;
}

static void USBDriver_EnableCB_ISR(void)
{
   USBDriver_SetSig(USBDRIVER_ISR_ENABLECB);
}

static void USBDriver_EnableCB(IUSBDriver *pme)
{
   int i;

   // In the unlikely event they called close before the open was fully
   // done.  Mark things as such and revert the USB substate
   if (pme->subports[SUBPORT_BULK].nState == USBDRIVER_STATE_CLOSED)
   {
   #ifdef FEATURE_USB
      USBDriver_USBDCSwitchDevice(FALSE);
   #endif
      return;
   }

   // Call our tryopen functions
   for (i=0; i<SUBPORT_MAX; i++)
   {
      AEE_APP_RESUME(&pme->subports[i].cbReady, pme->pac);
   }
}

#ifdef FEATURE_HSU_MTP
static void USBDriver_SetupMsgCB_ISR(void *pSetupMsg)
#else
static void USBDriver_SetupMsgCB_ISR(usbdc_setup_type *pSetupMsg)
#endif
{
   IUSBDriver *pme = &gUSBDriver;

   // Copy the setup message then flag it to be handled from within a
   // non-ISR context
   (void)MEMCPY(&pme->SetupMsg, pSetupMsg, sizeof(pme->SetupMsg));
   USBDriver_SetSig(USBDRIVER_ISR_SETUPMSGCB);
}

static void USBDriver_SetupMsgCB(IUSBDriver *pme)
{
   if (pme->pfnSetupReq == NULL)
   {
      return;
   }

   pme->bStallNeeded = TRUE;
   pme->bInRequestCB = TRUE;
   IUSBDRIVER_AddRef(pme);
   (void)pme->pfnSetupReq(pme->pSetupData, &pme->SetupMsg);
   if (pme->bStallNeeded)
   {
      (void)IUSBDriver_SendSetupResponse(pme, NULL, 0);
   }
   pme->bInRequestCB = FALSE;
   IUSBDRIVER_Release(pme);
}

static void USBDriver_SetSig(int nReason)
{
   IUSBDriver *pme = &gUSBDriver;

   if (rex_is_in_irq_mode() ||
       pme->pTCB != rex_self())
   {
      // If we are in irq mode or in a different task context (not UI)
      // set the signal to be processed when UI task gets active
      USBDriver_GoToBREWTask(pme, nReason);
   }
   else
   {
      // If we are in UI task context process the signal
      USBDriver_ProcessSig(nReason);
   }
}

// This funtion is called from within an ISR to signal that the USBDriver
// code needs something done in the UI task.
static void USBDriver_GoToBREWTask(IUSBDriver *pme, int nReason)
{
   uint32 dwIntLock;

   INTLOCK_SAV(dwIntLock);
   pme->nISRReason |= nReason;
   INTFREE_SAV(dwIntLock);

   (void)rex_set_sigs(pme->pTCB, AEE_USBDRIVER_SIG);
}

static void USBDriver_TryOpen(void *po)
{
   SubPort *pSubPort = (SubPort*)po;
   int nErr;

   // If the port is already opened, do nothing
   if (pSubPort->nState == USBDRIVER_STATE_OPENED) {
      return;
   }
   nErr = USBDriver_Open(pSubPort);

   if (nErr == SUCCESS)
   {
      pSubPort->nState = USBDRIVER_STATE_OPENED;

      // Run all writeables, keep all readables
      USBDriver_CBTrigger(&pSubPort->pcbWriteables);
      return;
   }
   else if (nErr == AEEPORT_WAIT)
   {
      USBDriver_CBRegister(&pSubPort->pcbWriteables, &pSubPort->cbReady);
   }
   else
   {
      (void)USBDriver_Close(pSubPort);
   }
}

// Register callback
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

// Run all the registered callbacks
static void USBDriver_CBTrigger(AEECallback **ppList)
{

   AEECallback *pcb;

   while (*ppList != NULL)
   {
      pcb = *ppList;
      CALLBACK_Cancel(pcb);
      AEE_APP_RESUME(pcb, gUSBDriver.pac);
   }
}

// Clear all the registered callbacks
static void USBDriver_CBClear(AEECallback **ppList)
{
   while (*ppList != NULL)
   {
      CALLBACK_Cancel(*ppList);
   }
}

// Cancel callback
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

// Function to add an item to transmit queue
static int USBDriver_Enqueue(AEEDSMBuf **pTxQ, AEEDSMBuf *pItem)
{
   AEEDSMBuf *ptr;

   // Sanity check
   if (!pTxQ || !pItem)
   {
      return EBADPARM;
   }

   pItem->pNext = NULL;

   // If this is the first item, update the Q pointer
   if (*pTxQ == NULL)
   {
      *pTxQ = pItem;
      return SUCCESS;
   }

   // Move till the end of Q and append the new record
   ptr = *pTxQ;
   while (ptr->pNext)
   {
      ptr = ptr->pNext;
   }
   ptr->pNext = pItem;

   return SUCCESS;
}

// Function to delete an item from transmit queue
static void USBDriver_Dequeue(AEEDSMBuf **pTxQ)
{
   // Sanity check
   if (!pTxQ)
   {
      return;
   }

   if (*pTxQ)
   {
      AEEDSMBuf *pFree;
      pFree = *pTxQ;

      // Move the transmit Q header to next item
      *pTxQ = pFree->pNext;

      // Free the node and the data within
      if (pFree->pDSMItem)
      {
         (void)brewdsm_free_buffer(pFree->pDSMItem);
      }
      FREE(pFree);
   }
}

// Function to get the size of transmit queue
static int USBDriver_TransferQSize(AEEDSMBuf *pTxQ)
{
   AEEDSMBuf *ptr;
   int nSize=0;

   // Browse thro the Q and get a count
   ptr = pTxQ;
   while (ptr) {
      nSize++;
      ptr = ptr->pNext;
   }

   return nSize;
}

static int USBDriver_Open(SubPort *pSubPort)
{
   pSubPort->nRcvSize = 0;
   pSubPort->bRxEnabled = FALSE;
   pSubPort->bReadable = FALSE;
   pSubPort->bWriteInProgress = FALSE;
   if (pSubPort->nPort == SUBPORT_BULK) { // Bulk Port
#ifndef FEATURE_HSU_MTP
      // Open the bulk port 
      USBDriver_USBDCBulkOpen();
#endif
      pSubPort->nState = USBDRIVER_STATE_OPENED;

      // Setup flushtx and connection status callbacks
      (void)USBDriver_USBDCIoctl(SUBPORT_BULK, (int)AEEUSBDRIVER_IOCTL_NTFYCNXNSTATUS,
                           (uint32)USBDriver_CnxnStatusCB);

      USBDriver_SetupRxBuf(pSubPort);
   }
   else { // Interrupt port
      // Open the port 
#ifndef FEATURE_HSU_MTP
      USBDriver_USBDCIntOpen();
#endif
      pSubPort->nState = USBDRIVER_STATE_OPENED;
   }

   USBDriver_WriteOrFlush(pSubPort);
   // Enable writeable
   USBDriver_SetWriteStatus(pSubPort);

   return SUCCESS;
}

static int USBDriver_Close(SubPort *pSubPort)
{
   int nState = pSubPort->nState;

   // Mark us as closing
   pSubPort->nState = USBDRIVER_STATE_CLOSING;
   if (nState == USBDRIVER_STATE_OPENED) {
      // Clear connection status CB
      //USBDriver_USBDCIoctl(pSubPort->nPort, USBDC_CLEAR_DTRCALLBACK, NULL);
      USBDriver_USBDCClose(pSubPort->nPort);
   }

   // Clear out the transfer Qs
   while(pSubPort->pRxQ)
   {
      USBDriver_Dequeue(&pSubPort->pRxQ);
   }
   while(pSubPort->pTxQ)
   {
      USBDriver_Dequeue(&pSubPort->pTxQ);
   }
   pSubPort->bReadable = FALSE;
   pSubPort->bWriteable = FALSE;
   pSubPort->bRxEnabled = FALSE;
   pSubPort->bWriteInProgress = FALSE;
   pSubPort->nRcvSize = 0;

   // Send close notification
   OEMUSBDriver_CloseNotify(pSubPort->nPort);

   return SUCCESS;
}

static void USBDriver_SetWriteStatus(SubPort *pSubPort)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited)
   {
      return;
   }

   if (pSubPort->nState != USBDRIVER_STATE_OPENED) {
       pSubPort->bWriteable = FALSE;
       return;
   }

   // We have free buffers available
   if ((pme->bufmgr.nAvailBufs > 0) &&
       (USBDriver_TransferQSize(pSubPort->pTxQ) <
          OEMUSBDRIVER_MAX_TRANFSER_Q))
   {
      // Enable writeable if not already enabled
      if (!pSubPort->bWriteable)
      {
         pSubPort->bWriteable = TRUE;
         USBDriver_CBTrigger(&pSubPort->pcbWriteables);
      }
   }
   else
   {
      pSubPort->bWriteable = FALSE;
   }
}

static int USBDriver_ScheduleWrite(SubPort *pSubPort, const char *pcBuf, int nLen )
{
   boolean bTerminate = FALSE;
   int nWritten = 0;
   int nInLen = nLen;
   dsm_item_type *item_ptr = NULL;
   AEEDSMBuf *pWriteData;

   if((pSubPort->bStreamMode == FALSE) || (nLen == 0))
   {
      bTerminate = TRUE;
   }

   //Case Size not large enough for 1 packet
   if(!bTerminate && 
      ((pSubPort->nCachedSize + nLen) < USBDRIVER_MIN_WRITE_SIZE))
   {
      char *pNewBuffer;
      pNewBuffer = REALLOC(pSubPort->pCachedData,
                           ALLOC_NO_ZMEM | (pSubPort->nCachedSize + nLen));

      if(NULL == pNewBuffer)
      {
         pSubPort->nLastErr = ENOMEMORY;
         return AEEPORT_ERROR;
      }
      else
      {
         MEMCPY(pNewBuffer + pSubPort->nCachedSize,
                pcBuf, nLen);
         pSubPort->pCachedData = pNewBuffer;
         pSubPort->nCachedSize += nLen;
         return nLen;
      }
   }

   while((nLen > 0) || (bTerminate && (pSubPort->nCachedSize != 0)))
   {
      int nSize;
      int nWrote = 0;
      // Allocate a dsm buffer to hold the write data
      item_ptr = brewdsm_new_buffer();
      if (item_ptr == NULL)
      {
         pSubPort->nLastErr = ENOMEMORY;
         break;
      }

      // Allocate memory to hold the record in Q
      pWriteData = MALLOC(sizeof(AEEDSMBuf));
      if (pWriteData == NULL) {
         pSubPort->nLastErr = ENOMEMORY;
         brewdsm_free_buffer(item_ptr);
         break;
      }
      pWriteData->pDSMItem = item_ptr;

      // Copy out previous data first
      if(pSubPort->nCachedSize != 0)
      {
         nWrote = MIN(pSubPort->nCachedSize, item_ptr->size);
         if(nWrote == pSubPort->nCachedSize)
         {
            MEMCPY(item_ptr->data_ptr, pSubPort->pCachedData, nWrote);
            FREE(pSubPort->pCachedData);
            pSubPort->pCachedData = NULL;
            pSubPort->nCachedSize = 0;
         }
         else
         {
            char *pNewBuff;
            //Can only copy some data, need to reallocate
            MEMCPY(item_ptr->data_ptr, pSubPort->pCachedData, nWrote);
            pNewBuff = MALLOC(pSubPort->nCachedSize - nWrote);
            if(pNewBuff == NULL)
            {
               pSubPort->nLastErr = ENOMEMORY;
               brewdsm_free_buffer(item_ptr);
               FREE(pWriteData);
               break;
            }
            else
            {
               MEMCPY(pNewBuff, 
                      pSubPort->pCachedData + nWrote, 
                      pSubPort->nCachedSize - nWrote);
               FREE(pSubPort->pCachedData);
               pSubPort->pCachedData = pNewBuff;
               pSubPort->nCachedSize = pSubPort->nCachedSize - nWrote;
            }
         }
      }

      // Copy the data over to the write buffer
      nSize = MIN(nLen, item_ptr->size - nWrote);
      if(!bTerminate && (nSize < (item_ptr->size - nWrote)))
      {
         pSubPort->nCachedSize = (nSize + nWrote) % USBDRIVER_MIN_WRITE_SIZE;
         
         if(pSubPort->nCachedSize != 0)
         {
            //Need to cache some data
            nSize -= pSubPort->nCachedSize;
            pSubPort->pCachedData = MALLOC(pSubPort->nCachedSize);
            if(NULL == pSubPort->pCachedData)
            {
               nLen -= pSubPort->nCachedSize;
               pSubPort->nCachedSize = 0;
            }
            else
            {
               MEMCPY(pSubPort->pCachedData, 
                      (pcBuf + nWritten) + nSize,
                      pSubPort->nCachedSize);
            }
         }
      }
      MEMCPY(item_ptr->data_ptr + nWrote, pcBuf+nWritten, nSize);
      item_ptr->used = nSize + nWrote;
      nWritten += nSize + pSubPort->nCachedSize;
      nLen -= nSize + pSubPort->nCachedSize;

      // Enqueue the buffer
      USBDriver_Enqueue(&pSubPort->pTxQ, pWriteData);
      if (USBDriver_TransferQSize(pSubPort->pTxQ) >=
            OEMUSBDRIVER_MAX_TRANFSER_Q)
      {
         pSubPort->bWriteable = FALSE;
      }
   }

#ifdef FEATURE_USB
   if(bTerminate && (nInLen == nWritten))
   {
      pWriteData = MALLOC(sizeof(AEEDSMBuf));
      if(pWriteData == NULL)
      {
         pSubPort->nLastErr = ENOMEMORY;
         return AEEPORT_ERROR;
      }
      else
      {
         pWriteData->pDSMItem = NULL;
         USBDriver_Enqueue(&pSubPort->pTxQ, pWriteData);
      }
   }
#endif /* FEATURE_USB */

   // Start the write
   USBDriver_WriteOrFlush(pSubPort);

   if((nWritten == 0) && (nInLen != 0))
   {
      return AEEPORT_ERROR;
   }
   else
   {
      return nWritten;
   }
}


static void USBDriver_WriteOrFlush(SubPort *pSubPort)
{
   if (pSubPort->nState == USBDRIVER_STATE_OPENED && // Port is open
       !pSubPort->bWriteInProgress) 
   {
      if(USBDriver_TransferQSize(pSubPort->pTxQ) > 0) // Atleast one buffer Q-ed for Tx
      {
         // Turn on write in progress flag
         pSubPort->bWriteInProgress = TRUE;
   
         // Write the data
         if(NULL != pSubPort->pTxQ->pDSMItem)
         {
            USBDriver_USBDCWrite(pSubPort->nPort,
                                 (const char *)pSubPort->pTxQ->pDSMItem->data_ptr,
                                 (int)pSubPort->pTxQ->pDSMItem->used);
         }
         else
         {
            USBDriver_USBDCWrite(pSubPort->nPort,
                                 NULL,
                                 0);
         }
      }
      else if(pSubPort->nCachedSize == 0)
      {
         //Send the flush callback
         USBDriver_CBTrigger(&pSubPort->pcbFlushTx);
      }
   }

}

static void USBDriver_SetupRxBuf(SubPort *pSubPort)
{
   int nErr=SUCCESS;
   IUSBDriver *pme = &gUSBDriver;

   do {

      if (pSubPort->nState != USBDRIVER_STATE_OPENED) {
         break;
      }

      // If we already set up the Rx Buf,
      // or we hit the max limit on queue size, do nothing
      if (pSubPort->bRxEnabled ||
          (pme->bufmgr.nAvailBufs == 0) ||
          (USBDriver_TransferQSize(pSubPort->pRxQ) >=
             OEMUSBDRIVER_MAX_TRANFSER_Q)) {
         break;
      }

      // We should not hit this case, but as a backup just free the buffer
      if (pSubPort->pRcvBuf) {
         (void)brewdsm_free_buffer(pSubPort->pRcvBuf);
      }

      //  Allocate receive dsm buffer
      pSubPort->pRcvBuf = brewdsm_new_buffer();
      if (pSubPort->pRcvBuf == NULL)
      {
         nErr = ENOMEMORY;
#ifdef FEATURE_HSU_MTP
         HSU_MSG_ERROR("USBDriver_SetupRxBuf: no memory", 0, 0, 0);
#endif         
         break;
      }

#ifdef HSU_MTP_DEBUG_MSG_ON
      mtp_debug_append('S');
#endif
      // Enable flow control
      USBDriver_USBDCFlowControl(TRUE);

      // Re-set the Rx buffer
      
      /* There is a race condition for bRxEnable. When USBDriver_USBDCRxCB()
         is called, bRxEnabled may be still FALSE, we will have to return right away
         from this func. 
         In order to prevent the above race condition, we will set the flag to
         TRUE first, then call USBDCSetupRxBuf().
      */
      pSubPort->bRxEnabled = TRUE;
      USBDriver_USBDCSetupRxBuf(pSubPort->nPort,
                                (byte *)pSubPort->pRcvBuf->data_ptr,
                                pSubPort->pRcvBuf->size,
                                (word *)&pSubPort->pRcvBuf->used);
   } while (0);

   if (nErr != SUCCESS) {

      // Keep track of the error
      pSubPort->nLastErr = ENOMEMORY;

      // Disable flow control
      USBDriver_USBDCFlowControl(FALSE);
   }
}

static void USBDriver_RxCB(SubPort *pSubPort)
{
#ifdef HSU_MTP_DEBUG_MSG_ON
      mtp_debug_append('X');
#endif

   if (pSubPort->nState != USBDRIVER_STATE_OPENED) {
       return;
   }

   if (pSubPort->bRxEnabled && pSubPort->pRcvBuf) {
      AEEDSMBuf *pReadData;

      // Allocate memory to hold the record in Q
      pReadData = MALLOC(sizeof(AEEDSMBuf));
      if (pReadData == NULL) {
         pSubPort->nLastErr = ENOMEMORY;
         (void)brewdsm_free_buffer(pSubPort->pRcvBuf);
         pSubPort->pRcvBuf = NULL;
      }
      else {
         pReadData->pDSMItem = pSubPort->pRcvBuf;
         pReadData->pBuf = (void *)pSubPort->pRcvBuf->data_ptr;

         pSubPort->nRcvSize += pSubPort->pRcvBuf->used;

         // Enqueue the buffer
         (void)USBDriver_Enqueue(&pSubPort->pRxQ, pReadData);
         pSubPort->pRcvBuf = NULL;
      }
   }

   pSubPort->bRxEnabled = FALSE;
   USBDriver_SetupRxBuf(pSubPort);

   // If we have data to be read by the app, turn on readable
   if (pSubPort->nRcvSize > 0)
   {
      pSubPort->bReadable = TRUE;
      USBDriver_SetSig(USBDRIVER_ISR_READABLECB);
   }
}

static void USBDriver_CnxnStatusCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited)
   {
      return;
   }

   USBDriver_SetSig(USBDRIVER_ISR_CNXNSTATUSCB);
}

#ifdef FEATURE_HSU_MTP 
static void USBDriver_FlushTxCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited)
   {
      return;
   }

   USBDriver_SetSig(USBDRIVER_ISR_FLUSHTXCB);
}


static void USBDriver_IntFlushTxCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited)
   {
      return;
   }

   USBDriver_SetSig(USBDRIVER_ISR_INTFLUSHTXCB);
}
#endif  /* FEATURE_HSU_MTP */

/*********************************************************************
* Functions interfacing with usbdc
*********************************************************************/
#ifdef FEATURE_USB
static void USBDriver_USBDCBulkOpen()
{
   // Open the stream and set up the rx callback function
#ifdef FEATURE_USBDRIVER_NO_USBDC_API
   usbdc_open(USBDC_BREW_DATA_IFACE,           // Select specified USB stream
              USBDriver_USBDCRxCB,             // rx packet callback function
              NULL,                            // rx watermark queue
              USBDriver_USBDCRxErrCB,          // rx error callback function
              USBDriver_USBDCTxErrCB,          // tx error callback function
              NULL,                            // tx buffer queue.
              USBDriver_USBDCTxCB);            // tx packet callback function
#else
   usbdc_api_open(USBDC_BREW_DATA_IFACE,           // Select specified USB stream
                  USBDriver_USBDCRxCB,             // rx packet callback function
                  NULL,                            // rx watermark queue
                  USBDriver_USBDCRxErrCB,          // rx error callback function
                  USBDriver_USBDCTxErrCB,          // tx error callback function
                  NULL,                            // tx buffer queue.
                  USBDriver_USBDCTxCB);            // tx packet callback function
#endif  /* FEATURE_USBDRIVER_NO_USBDC_API */

   // Enable flow control
   USBDriver_USBDCFlowControl(TRUE);
}

static void USBDriver_USBDCIntOpen()
{
   // Open the stream
   // We dont expect any Rx data on the interrupt port.
#ifdef FEATURE_USBDRIVER_NO_USBDC_API
   usbdc_open(USBDC_BREW_NOTIFICATION_IFACE,    // Select specified USB stream
              NULL,                             // rx packet callback function
              NULL,                             // rx watermark queue
              NULL,                             // rx error callback function
              USBDriver_USBDCIntTxErrCB,        // tx error callback function
              NULL,                             // tx buffer queue.
              USBDriver_USBDCIntTxCB);          // tx packet callback function
#else
   usbdc_api_open(USBDC_BREW_NOTIFICATION_IFACE,    // Select specified USB stream
                  NULL,                             // rx packet callback function
                  NULL,                             // rx watermark queue
                  NULL,                             // rx error callback function
                  USBDriver_USBDCIntTxErrCB,        // tx error callback function
                  NULL,                             // tx buffer queue.
                  USBDriver_USBDCIntTxCB);          // tx packet callback function
#endif
}
#endif /* FEATURE_HSU_MTP */

static void USBDriver_USBDCSetupRxBuf(int nPort,
                                      byte *rx_buffer,
                                      word rx_buffer_size,
                                      word *rx_data_size)
{
#ifdef FEATURE_HSU_MTP
      // use word instead of juint16_t in the following
      hsu_al_brewext_read_request((void *)rx_buffer, 
                                   (word) rx_buffer_size,
                                  (word *)rx_data_size);
      
#else
   usbdc_interface_enum_type usbdc_iface;
   usbdc_iface = ((nPort == SUBPORT_BULK) ?
                  USBDC_BREW_DATA_IFACE : 
                  USBDC_BREW_NOTIFICATION_IFACE);

   // Set up Rx buffer
  #ifdef FEATURE_USBDRIVER_NO_USBDC_API
    #ifdef FEATURE_USBDRIVER_STREAM_BASED
      usbdc_read_request(usbdc_iface,
                      rx_buffer,
                      rx_buffer_size,
                      rx_data_size);
    #else
       usbdc_read_request(usbdc_iface,
                      USBDC_DATA_STREAM,
                      rx_buffer,
                      rx_buffer_size,
                      rx_data_size);
    #endif /* FEATURE_USBDRIVER_STREAM_BASED */
  #else
   usbdc_api_read_request(usbdc_iface,
                          USBDC_DATA_STREAM,
                          rx_buffer,
                          rx_buffer_size,
                          rx_data_size);
  #endif /* FEATURE_USBDRIVER_NO_USBDC_API */
#endif /* FEATURE_HSU_MTP */
}

static void USBDriver_USBDCFlowControl(boolean bEnable)
{
#ifdef FEATURE_USB
  #ifdef FEATURE_USBDRIVER_NO_USBDC_API
     usbdc_ioctl(USBDC_BREW_DATA_IFACE,
               (bEnable ? USBDC_ENABLE_INFLOW : USBDC_DISABLE_INFLOW),
               NULL);
  #else
     usbdc_api_ioctl(USBDC_BREW_DATA_IFACE,
                   (bEnable ? USBDC_ENABLE_INFLOW : USBDC_DISABLE_INFLOW),
                   NULL);
  #endif /* FEATURE_USBDRIVER_NO_USBDC_API */
#endif /* FEATURE_USB */
}

static void USBDriver_USBDCClose(int nPort)
{
#ifdef FEATURE_HSU_MTP
   /* Do we need to process similar stuff in usbdc_api_close()? It included abort read, 
      abort write and set all callbacks to NULL.

      NO. We don't need to do anything here since mtpfd_disable() takes care of it.
   */

#else
   usbdc_interface_enum_type usbdc_iface;
   usbdc_iface = ((nPort == SUBPORT_BULK) ?
                  USBDC_BREW_DATA_IFACE : 
                  USBDC_BREW_NOTIFICATION_IFACE);

#ifdef FEATURE_USBDRIVER_NO_USBDC_API
   usbdc_close(usbdc_iface);
#else
   usbdc_api_close(usbdc_iface);
#endif

#endif /* FEATURE_HSU_MTP */
}

static void USBDriver_USBDCWrite(int nPort, const char *pcBuf, int nLen )
{
#ifdef FEATURE_HSU_MTP
      mtpfd_pipe_type_t pipe_type;
      pipe_type = ((nPort == SUBPORT_BULK) ?
                  MTPFD_PIPE_BULK : MTPFD_PIPE_INTR);
      hsu_al_brewext_write_request(pipe_type, (byte *)pcBuf, nLen);
  
#else
   usbdc_interface_enum_type usbdc_iface;
   usbdc_iface = ((nPort == SUBPORT_BULK) ?
                  USBDC_BREW_DATA_IFACE : 
                  USBDC_BREW_NOTIFICATION_IFACE);

#ifdef FEATURE_USBDRIVER_NO_USBDC_API
   usbdc_write_data(usbdc_iface, (byte *)pcBuf, nLen);
#else
   usbdc_api_write_data(usbdc_iface, (byte *)pcBuf, nLen);
#endif

#endif /* FEATURE_HSU_MTP */
}

static void USBDriver_USBDCTxCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited || !pme->subports[SUBPORT_BULK].bWriteInProgress)
   {
      return;
   }

   USBDriver_SetSig(USBDRIVER_ISR_WRITE);
}

static void USBDriver_USBDCTxErrCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited)
   {
      return;
   }

   // Note the error condition
   pme->subports[SUBPORT_BULK].nLastErr = EFAILED;

   // Call the regular Tx done callback
   USBDriver_USBDCTxCB();
}

static void USBDriver_USBDCIntTxCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited || !pme->subports[SUBPORT_INT].bWriteInProgress)
   {
      return;
   }

   USBDriver_SetSig(USBDRIVER_ISR_INTWRITE);
}

static void USBDriver_USBDCIntTxErrCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   // Make sure we're initialized
   if (!pme->bInited)
   {
      return;
   }

   // Note the error condition
   pme->subports[SUBPORT_INT].nLastErr = EFAILED;

   // Call the regular Tx done callback
   USBDriver_USBDCIntTxCB();
}

#ifdef FEATURE_HSU_MTP
static void USBDriver_USBDCRxCB(mtpfd_pipe_type_t type )
   {
    IUSBDriver *pme = &gUSBDriver;
 
//    HSU_MSG_HIGH("USBDCRxCB: entered", 0, 0, 0);
    if (!pme->bInited ||      // Make sure we're initialized
       (type != MTPFD_PIPE_BULK) || // Rx is only supported on Bulk port
     !pme->subports[SUBPORT_BULK].bRxEnabled) // if we did not set up rx buffer, ignore callback
    {
       HSU_MSG_ERROR("USBDCRxCB: bRxEnabled=%d, type=%d", pme->subports[SUBPORT_BULK].bRxEnabled,type,0);
       return;
    }
 
    // Disable flow control till we set up next rx buffer
    USBDriver_USBDCFlowControl(FALSE);
 
    // Trigger the generic Rx callback function in UI task context
    USBDriver_SetSig(USBDRIVER_ISR_READ);

#ifdef HSU_MTP_DEBUG_MSG_ON
    mtp_debug_append('B');  // go to BREW task from USB task
#endif

  }

#else
static void USBDriver_USBDCRxCB(usbdc_interface_enum_type usbdc_iface)
{
   IUSBDriver *pme = &gUSBDriver;

   if (!pme->bInited || // Make sure we're initialized
       usbdc_iface != USBDC_BREW_DATA_IFACE || // Rx is only supported on Bulk port
       !pme->subports[SUBPORT_BULK].bRxEnabled) // if we did not set up rx buffer, ignore callback
   {
      return;
   }

   // Disable flow control till we set up next rx buffer
   USBDriver_USBDCFlowControl(FALSE);

   // Trigger the generic Rx callback function in UI task context
   USBDriver_SetSig(USBDRIVER_ISR_READ);
}
#endif	/* FEATURE_HSU_MTP */

static void USBDriver_USBDCRxErrCB(void)
{
   IUSBDriver *pme = &gUSBDriver;

   if (!pme->bInited) // Make sure we're initialized
   {
      return;
   }

   // Note the error condition
   pme->subports[SUBPORT_BULK].nLastErr = EFAILED;
}

static void USBDriver_USBDCIoctl(int nPort,
                                 int nIoctl,
                                 uint32 dwParam)
{
#ifdef  FEATURE_HSU_MTP
   if (nPort == SUBPORT_BULK)
   {
     hsu_al_brewext_flush_tx(MTPFD_PIPE_BULK, USBDriver_FlushTxCB);
   }
   else
   {
     hsu_al_brewext_flush_tx(MTPFD_PIPE_INTR, USBDriver_IntFlushTxCB);
   }

  
 #else
   usbdc_interface_enum_type usbdc_iface;

   usbdc_iface = ((nPort == SUBPORT_BULK) ?
                  USBDC_BREW_DATA_IFACE : 
                  USBDC_BREW_NOTIFICATION_IFACE);

   if(nIoctl == ((int)AEEUSBDRIVER_IOCTL_GETCNXNSTATUS))
   {
      if (dwParam)
      {
         boolean dte_ready;
         sio_ioctl_param_type param;
         param.dte_ready_asserted = &dte_ready;
#ifdef FEATURE_USBDRIVER_NO_USBDC_API
         usbdc_ioctl(usbdc_iface, USBDC_GET_DTR, (void *)&param);
#else
         usbdc_api_ioctl(usbdc_iface, USBDC_GET_DTR, (void *)&param);
#endif
         *((boolean *)dwParam) = dte_ready;
      }
   }
   else if(nIoctl == ((int)AEEUSBDRIVER_IOCTL_NTFYCNXNSTATUS))
   {
#ifdef FEATURE_USBDRIVER_NO_USBDC_API
      usbdc_ioctl(usbdc_iface, USBDC_SET_DTRCALLBACK, (void *)&dwParam);
#else
      usbdc_api_ioctl(usbdc_iface, USBDC_SET_DTRCALLBACK, (void *)&dwParam);
#endif
   }
#endif  /* FEATURE_HSU_MTP */
}

static void USBDRIVER_USBDC_EnableTimeout(void *po)
{
   gUSBState = USBDRIVER_STATE_OPENING_TO;

   if(!gDesiredUSBMode)
   {
      USBDriver_SetSig(USBDRIVER_ISR_RESETDEVICE);
   }
}

static void USBDRIVER_USBDC_EnableCB(void)
{
   gUSBState = USBDRIVER_STATE_OPENED;
   AEE_CancelTimer(USBDRIVER_USBDC_EnableTimeout, NULL);

   if(gDesiredUSBMode)
   {
      USBDriver_EnableCB_ISR();
   }
   else
   {
      USBDriver_SetSig(USBDRIVER_ISR_RESETDEVICE);
   }
}

static void USBDRIVER_USBDC_DisableCB(void)
{
   gUSBState = USBDRIVER_STATE_CLOSED;
   if(gDesiredUSBMode)
   {
      USBDriver_SetSig(USBDRIVER_ISR_RESETDEVICE);
   }
}

#ifdef  FEATURE_USB 
static int USBDriver_USBDCSwitchDevice(boolean bBREWMode)
{
#if defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)
   IUSBDriver *pme = &gUSBDriver;
#endif /*defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)*/
   gDesiredUSBMode = bBREWMode;

   if(bBREWMode)
   {
      if(gUSBState == USBDRIVER_STATE_CLOSED)
      {
         int status = AEEPORT_WAIT;
         gUSBState = USBDRIVER_STATE_OPENING;
#ifdef FEATURE_USBDRIVER_NO_USBDC_API
         usbdc_enable_device(USBDC_BREW_EXTENDED_DEVICE,
                             USBDriver_SetupMsgCB_ISR,
                             USBDRIVER_USBDC_EnableCB);
#else
         status = usbdc_api_enable_device(USBDC_BREW_EXTENDED_DEVICE,
                                          USBDriver_SetupMsgCB_ISR,
                                          USBDRIVER_USBDC_EnableCB);
         if (status == USBDC_API_NOTALLOWED)
         {
            status = ENOTALLOWED;
         }
         else if (status != USBDC_API_SUCCESS)
         {
            status = EFAILED;
         }
         else
         {
            status = AEEPORT_WAIT;
         }
#endif
         if(status == AEEPORT_WAIT)
         {
#if defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)
            usbdc_api_set_setup_msg_data_buffer(USBDC_BREW_EXTENDED_DEVICE,
                                                      pme->ctrlRequestOutData,
                                                      sizeof(pme->ctrlRequestOutData));
#endif /*defined(FEATURE_USB) && !defined(FEATURE_USBDRIVER_NO_USBDC_API)*/
            AEE_SetSysTimer(30000, USBDRIVER_USBDC_EnableTimeout, NULL);
         } 

         return status;
      }
      else if(gUSBState == USBDRIVER_STATE_OPENED)
      {
         return SUCCESS;
      }
   }
   else
   { 
      if((gUSBState == USBDRIVER_STATE_OPENED) ||
         (gUSBState == USBDRIVER_STATE_OPENING_TO))
      {
         gUSBState = USBDRIVER_STATE_CLOSING;
         AEE_CancelTimer(USBDRIVER_USBDC_EnableTimeout, NULL);
#ifdef FEATURE_USBDRIVER_NO_USBDC_API
         usbdc_disable_device(USBDRIVER_USBDC_DisableCB);
#else
         usbdc_api_disable_device(USBDC_BREW_EXTENDED_DEVICE,
                                  USBDRIVER_USBDC_DisableCB);
#endif
         return AEEPORT_WAIT;
      }
      else if(gUSBState == USBDRIVER_STATE_CLOSED)
      {
         return SUCCESS;
      }
   }
   return AEEPORT_WAIT;
}
#endif /* FEATURE_USB */

#ifdef FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP

void USBDriver_MTPAppCB(boolean bLaunch)
{
  // Set signal to self to handle it in BREW/UI context
  USBDriver_GoToBREWTask(&gUSBDriver,
                         (bLaunch ?
                          USBDRIVER_ISR_LAUNCHAPP : USBDRIVER_ISR_CLOSEAPP));
}

void USBDriver_RegisterMTPAppCB()
{
#ifndef FEATURE_USBDRIVER_NO_USBDC_API
  // Register for mtp state change notification
  usbdc_api_register_mtp_callback((usbdc_api_mtp_cb_type)USBDriver_MTPAppCB);
#endif
}

/*==============================================================================

FUNCTION      USBDriver_MTPAppRunning

DESCRIPTION   
    This function is called from UI context. It will return TRUE
    if Teleca background app MTPH is running.
    
RETURN VALUE  
    None 

SIDE EFFECTS  
    None

==============================================================================*/
static boolean USBDriver_MTPAppRunning(void)
{
  IShell* piShell = AEE_GetShell();

  if (NULL != piShell)
  {
    IAppletCtl *pAppCtl = NULL;
    uint32 nState;
    int nSize = sizeof(nState);

    // Create an instance of appletctl
    (void)ISHELL_CreateInstance(piShell, AEECLSID_APPLETCTL, (void **)&pAppCtl);
    if (pAppCtl) {

      // Read the state of the app
      if (SUCCESS == IAPPLETCTL_GetRunningInfo(pAppCtl, AEECLSID_MTPH,
                                               AEE_APPITEM_STATE, &nState, &nSize)
          && nState == APPSTATE_BACKGROUND) {

        // App is running in background mode, so return TRUE
        return TRUE;
      }
    }
  }

  // No App information OR app is not running in background mode
  return FALSE;
}

/*==============================================================================

FUNCTION      USBDriver_LaunchMTPApp

DESCRIPTION   
    This function is called from UI context. It will start up Teleca
    background app MTPH.
    
RETURN VALUE  
    None 

SIDE EFFECTS  
    None

==============================================================================*/
static void USBDriver_LaunchMTPApp(void)
{
  IShell* piShell = AEE_GetShell();

  // Start MTP app in background mode if not already started
  if (NULL != piShell && !USBDriver_MTPAppRunning())
  {
    (void)ISHELL_StartBackgroundApplet(piShell, AEECLSID_MTPH, NULL);
  }
}

/*==============================================================================

FUNCTION      USBDriver_CloseMTPApp

DESCRIPTION   
    This function is called from UI context. It will close Teleca
    background app MTPH.
    
RETURN VALUE  
    None 

SIDE EFFECTS  
    None

==============================================================================*/
static void USBDriver_CloseMTPApp(void)
{
  IShell* piShell = AEE_GetShell();

  // Stop MTP app if it is running in background mode
  if (NULL != piShell && USBDriver_MTPAppRunning())
  {
    (void)ISHELL_PostEvent(piShell, AEECLSID_MTPH, EVT_USER_CLOSE, 0, 0);
  }
}
#endif // FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP

#endif /* FEATURE_BREW_USBDRIVER */

