/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMSIO.c

SERVICES:  OEMSIO Serial I/O Interface if FEATURE_RUNTIME_DEVMAP is defined

GENERAL DESCRIPTION:
   
        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "OEMFeatures.h"

#ifdef FEATURE_BREW_SIO


//DMSS header files
#include "queue.h"
#include "sio.h"
#include "dsm.h"
#include "rdevmap.h"
#include "ui.h"

//BREW header files   
#include "OEMSIO.h"
#include "AEE_OEMDispatch.h"
#include "AEE.h"
#include "AEEStdLib.h"

//=============================================================================
//  VARIABLE AND TYPE DEFINITIONS
//=============================================================================

//-----------------------------------------------------------------------------
//          Defines
//-----------------------------------------------------------------------------

#define SIO_READ_BUFFER_SIZE       6*1024
#define SIO_WRITE_BUFFER_SIZE      2*1024

#define SIO_INBOUND_OVERFLOW_ALLOWANCE  512
#define SIO_INBOUND_HYSTERESIS  (SIO_INBOUND_OVERFLOW_ALLOWANCE + 512)


// index into port map, used to identify this port to RDM and the SIO services
#define RDM_SRVC              0
#define RDM_DEVICE            1
#define SIO_PORT              2

#define OEMSIO_MAX_PORTS      3
#define MAIN_SIO_PORT         0
#define AUX_SIO_PORT          1
#define USB_SIO_PORT          2

#define RDM_BUSY_REATTEMPT_TIMER 2000 //2seconds

#define GETSIO(nPort)  (nPort < OEMSIO_MAX_PORTS ) ? \
                            &(gSio.sio[nPort]) : NULL

#define CNVRT_FLOW_CNTRL(flow) ( (flow) == AEESIO_FLOW_HARDWARE ) ? \
                                  SIO_CTSRFR_FCTL : ( (flow) == AEESIO_FLOW_SOFTWARE ) ? \
                                  SIO_XONXOFF_STRIP_FCTL_FS : SIO_FCTL_OFF

//
// Conditional Debug Output
//

#define FARF_SIO 0

#ifdef FARF
#undef FARF
#undef FARF_ALWAYS
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 
#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

  
///----------------------------------------------------------------------------
//          Type Declarations
//-----------------------------------------------------------------------------
// Circular Input/Output buffers
typedef struct {
   char *    pData;
   int       cb;
   int       iOut;   // output index position
   int       iIn;    // input index position
} CBuf;

typedef enum {
   OEMSIO_ST_OPEN_RDM_BUSY,
   OEMSIO_ST_OPEN_IN_RDM,
   OEMSIO_ST_OPENED,
   OEMSIO_ST_OPEN_FAILED,
   OEMSIO_ST_CLOSE_RDM_BUSY,
   OEMSIO_ST_CLOSE_IN_RDM,
   OEMSIO_ST_CLOSED
} OEMSIO_State;

// OEMSIO state data
typedef struct {

   sio_stream_id_type      nStreamID;
   int                     bBrewMode;
   int                     nOEMState;
   int                     map[3];
   rdm_assign_status_type  nRDMStatus;
   rdm_service_enum_type   prevService;

   dsm_watermark_type      outgoingWMQ;      // WaterMark Queue used when data is sent 
   q_type                  outgoingQ;        // Queue around which outgoingWMQ is built 
   dsm_item_type           *tx_item_ptr;

   AEESIOConfig            SIOConfig;
  
   // Circular Read buffer
   CBuf                    cbufRead;
   char                    aRead[SIO_READ_BUFFER_SIZE];
   sio_rx_func_ptr_type    rx_func_ptr;
   sio_vv_func_ptr_type    dtr_event_func_ptr;
   int                     cbReadTrigger;
   int                     cbWriteTrigger;
   int                     nReadIdleTimeout;
   uint32                  tLastReceived;
   int                     bWriteable;
   uint32                  nPort;
   char                    szPortName[32];

   AEECallback             TimerCB;
   AEECallback             OpenCB;
   AEECallback             CloseCB;
   AEECallback             StatusCB;
   OEMSIO_State            currentState;
   flg                     bOpenRequested:1;
   flg                     bInboundFlowDisabled:1;

   int                     OpenSubStates;
   int                     CloseSubStates;
   int                     closesvccb_called;

#define SIO_NOTIFY       1
#define SIO_IDLE_TIMER   2
   
   //For ISR to Supervisor communication
   int nTaskOp;
   
} SIO;


typedef struct _OEMSio OEMSio;
struct _OEMSio{
   SIO sio[ OEMSIO_MAX_PORTS ];
   PFNSIONOTIFY   pfnNotify;
   AEECallback    cbk;
   rex_tcb_type*  gpTCB;
};

static OEMSio  gSio = {0};

// this function is external and defined in rdevmap.c but not declared in rdevmap.h
// so I have to declare it here
rdm_service_enum_type rdm_get_service( rdm_device_enum_type device );




///////////////////////////////////////////////////////////////////////////////
// CBuf macros

#define CBuf_GetFree(me)            ( ((me)->cb-1) - CBuf_GetCount(me) )
#define CBuf_IsEmpty(me)            ( (me)->iOut == (me)->iIn )
#define CBuf_IsFull(me)             ( 0 == CBuf_GetFree(me) )

#define CBuf_Flush(me)              (me)->iOut = (me)->iIn = 0

#define CBuf_AdvanceIn(pme, n)      (pme)->iIn = ((pme)->iIn + (n)) % (pme)->cb
#define CBuf_AdvanceOut(pme, n)     (pme)->iOut = ((pme)->iOut + (n)) % (pme)->cb

// Initializes the cbuf object
void     CBuf_Ctor                  (CBuf * pme, char * pBuffer, int nSize);
int      CBuf_GetCount              (CBuf * pme);

// Returns a pointer to the output data in CBuf and the
// count of contiguous (without wrapping) data. 
char *   CBuf_GetContiguousOutput   (CBuf * pme, int * pn);

// Returns a pointer to the free (input) space in CBuf and the
// count of contiguous (without wrapping) space. 
char *   CBuf_GetContiguousInput    (CBuf * pme, int * pn);

// Fills the CBuf with data (presumably to write). Updates the
// iIn index.
int      CBuf_Fill                  (CBuf * pme, const char * pData, int cbData);


///----------------------------------------------------------------------------
//          Private function declarations
//-----------------------------------------------------------------------------

static void OEMSIO_NotifyState(SIO *psio);
static void OEMSIO_RDM_SioOpenCB( void *pv);
static void OEMSIO_RDM_SioCloseCB( void *pv);
static void OEMSIO_SetTaskWorker(SIO *psio, int nOp);
static void OEMSIO_RDMStatusBrewCB(void *pv);


// handling functions for the Main UART (UART1)

#if defined (FEATURE_BREW_SIO_PORT1_SUPPORTED)
#ifndef FEATURE_BREW_SIO_NEW_RDM
static void OEMSIO_UART1_rdm_close(void );
#endif
static void OEMSIO_UART1Rx(dsm_item_type** item_ptr );
static void OEMSIO_UART1_LowWatermarkFunc (void);
static void OEMSIO_UART1_HighWatermarkFunc(void);
#endif //defined (FEATURE_BREW_SIO_PORT1_SUPPORTED)

#if defined (FEATURE_SECOND_UART)

#ifndef FEATURE_BREW_SIO_NEW_RDM
static void OEMSIO_UART2_rdm_close(void );
#endif
static void OEMSIO_UART2Rx(dsm_item_type** item_ptr );
static void OEMSIO_UART2_LowWatermarkFunc (void);
static void OEMSIO_UART2_HighWatermarkFunc(void);
#endif

#if defined (FEATURE_USB_CDC_ACM)

#ifndef FEATURE_BREW_SIO_NEW_RDM
static void OEMSIO_USB_rdm_close(void );
#endif
static void OEMSIO_USBRx(dsm_item_type** item_ptr );
static void OEMSIO_USB_LowWatermarkFunc (void);
static void OEMSIO_USB_HighWatermarkFunc(void);
#endif

static void OEMSIO_SetDefaultConfig(AEESIOConfig *pConfig);
static void OEMSIO_TryOpen(void *pv);
static void OEMSIO_TryClose(void *pv);
static void OEMSIO_StartReadIdelTimer(SIO *psio);
static int  OEMSIO_IsReadable(SIO *psio);

//Open functions
#ifdef FEATURE_BREW_SIO_NEW_RDM
static void OEMSIO_RDM_multi_open  (sio_port_id_type port_id, void *pv);
static void OEMSIO_RDM_multi_close (sio_port_id_type port_id, void *pv);
#else
static void OEMSIO_RDM_open_func(sio_port_id_type port_id );
#endif


/*=============================================================================
The signal AEE_SIO_SIG must be defined and used. This signal is used to communicate 
between the ISR and tha ui task. When the ISR is invoked, it sets this signal to inform
the ui task to invoke a function in the ui task context.

Please do the following steps to define and use the signal. 
1. Define the signal in ui.h to a un-used signal value.
   ex. #define AEE_SIO_SIG 0x40000
   This in only an example. You may have to choose a value different than 0x40000

   You can define/declare it right after the AEE_APP_SIG used by BREW

2. Add the following line to the sig mask that rex_wait(in the function ui task) waits on:
     | AEE_SIO_SIG (also right next to AEE_APP_SIG)

3. Modify the function ui_task() in ui.c or uihsig.c to handle this signal. 
   When this signal is set, the function OEMSIO_TaskHandler() must be invoked. 
   Here is the sequence: 

  Add the following right after the AEE_APP_SIG if statement.

    else if ( sigs & AEE_SIO_SIG) {
      // Handle AEE_SIO_SIG 
      extern void OEMSIO_TaskHandler(void);

      (void) rex_clr_sigs( rex_self(), AEE_SIO_SIG);
      (void) OEMSIO_TaskHandler();
    }
================================================================================*/

#ifndef AEE_SIO_SIG

#error  SIO Signal AEE_SIO_SIG not defined. Please do the following steps to define and use the \
        SIO signal. \
Please do the following steps to define and use the signal. \
1. Define the signal in ui.h to a un-used signal value.\
   ex. #define AEE_SIO_SIG 0x40000 \
   This in only an example. You may have to choose a value different than 0x40000 \
   You can define/declare it right after the AEE_APP_SIG used by BREW \
2. Add the following line to the sig mask that rex_wait(in the function ui task) waits on: \
     | AEE_SIO_SIG (also right next to AEE_APP_SIG) \
3. Modify the function ui_task() in ui.c or uihsig.c to handle this signal. \
   When this signal is set, the function OEMSIO_TaskHandler() must be invoked. Here is the sequence: \
  Add the following code right after the AEE_APP_SIG if statement. \
    else if ( sigs & AEE_SIO_SIG) { \
      extern void OEMSIO_TaskHandler(void);\
      (void) rex_clr_sigs( rex_self(), AEE_SIO_SIG); \
      (void) OEMSIO_TaskHandler();\
    }
#endif


/*=============================================================================
                  FUNCTION DEFINITIONS
=============================================================================*/


//This function will get called when the AEE_SIO_SIG is set by an ISR
void OEMSIO_TaskHandler(void) 
{
   int i = 0;

   for(i = 0; i < OEMSIO_MAX_PORTS; i++) {
      SIO *psio = &gSio.sio[i];
      
      if(!psio->nTaskOp)
         continue;
       
      if(psio->nTaskOp & SIO_NOTIFY) {
         psio->nTaskOp &= ~SIO_NOTIFY;
         OEMSIO_NotifyState(psio);
      }

      if(psio->nTaskOp & SIO_IDLE_TIMER) {
         psio->nTaskOp &= ~SIO_IDLE_TIMER;
         OEMSIO_StartReadIdelTimer(psio);
      }
   }
}


static void OEMSIO_SetTaskWorker(SIO *psio, int nOp)
{
   psio->nTaskOp |= nOp;
   rex_set_sigs(gSio.gpTCB, AEE_SIO_SIG);
}


/*=============================================================================
   Function: OEMSIO_Init
 
   Parameters:
      pfnNotify = function to call with notifications
 
   Return Value:
      SUCCESS = success (serial IO supported)
      EFAILED = failure (serial IO unsupported)
=============================================================================*/
int OEMSIO_Init( PFNSIONOTIFY pfnNotify )
{
   int  i;
   SIO *psio;
   
   if(!pfnNotify)
      return EFAILED;


   gSio.gpTCB = rex_self();

   for( i=0; i< OEMSIO_MAX_PORTS; i++ ) {
      psio = &(gSio.sio[i]);
      MEMSET(psio, 0, sizeof(SIO));

      psio->map[SIO_PORT]  = SIO_PORT_NULL;
      psio->map[RDM_DEVICE]= RDM_NULL_DEV;

#ifdef FEATURE_BREW_SIO_NEW_RDM
      psio->map[RDM_SRVC]  = RDM_BREW_SRVC;
#endif
      
      psio->nStreamID      = SIO_NO_STREAM_ID;
      psio->prevService    = RDM_NULL_SRVC;
      psio->nRDMStatus     = RDM_ASSIGN_STATUS_MAX_S;
      psio->nPort       = i;
      psio->cbReadTrigger  = 1;
      psio->cbWriteTrigger = 1;
      psio->bWriteable  = 1;
      psio->currentState= OEMSIO_ST_CLOSED;
      psio->bInboundFlowDisabled = 1;
      OEMSIO_SetDefaultConfig(&psio->SIOConfig);
      
      // Set up Queue for sent data
      (void)q_init( &psio->outgoingQ );
      psio->outgoingWMQ.q_ptr                = &psio->outgoingQ;
      psio->outgoingWMQ.dont_exceed_cnt      = SIO_WRITE_BUFFER_SIZE+1024;
      psio->outgoingWMQ.hi_watermark         = SIO_WRITE_BUFFER_SIZE;
      psio->outgoingWMQ.lo_watermark         = psio->outgoingWMQ.hi_watermark 
                                                         - psio->cbWriteTrigger;
   }
   
#if defined (FEATURE_BREW_SIO_PORT1_SUPPORTED)
   // setup the main UART
   psio = GETSIO(MAIN_SIO_PORT);

   psio->map[RDM_DEVICE]               = RDM_UART1_DEV;
   psio->map[SIO_PORT]                 = SIO_PORT_UART_MAIN;

   psio->rx_func_ptr                   = OEMSIO_UART1Rx;
   psio->outgoingWMQ.lowater_func_ptr  = OEMSIO_UART1_LowWatermarkFunc;
   psio->outgoingWMQ.hiwater_func_ptr  = OEMSIO_UART1_HighWatermarkFunc;
   (void)STRLCPY(psio->szPortName, AEESIO_PORT_SIO1, sizeof(psio->szPortName));

#ifndef FEATURE_BREW_SIO_NEW_RDM
   psio->map[SIO_PORT] = rdm_register_new_service(
                              (rdm_device_enum_type)psio->map[RDM_DEVICE],
                              OEMSIO_RDM_open_func, OEMSIO_UART1_rdm_close,
                              (rdm_service_enum_type*)&(psio->map[RDM_SRVC]));
#endif
#endif //defined (FEATURE_BREW_SIO_PORT1_SUPPORTED)

#if defined (FEATURE_SECOND_UART)   
   // setup the AUX UART
   psio = GETSIO(AUX_SIO_PORT);
   psio->map[RDM_DEVICE]               = RDM_UART2_DEV;
   psio->map[SIO_PORT]                 = SIO_PORT_UART_AUX;

   psio->rx_func_ptr                   = OEMSIO_UART2Rx;
   psio->outgoingWMQ.lowater_func_ptr  = OEMSIO_UART2_LowWatermarkFunc;
   psio->outgoingWMQ.hiwater_func_ptr  = OEMSIO_UART2_HighWatermarkFunc;
   (void)STRLCPY(psio->szPortName, AEESIO_PORT_SIO2, sizeof(psio->szPortName));

#ifndef FEATURE_BREW_SIO_NEW_RDM
   psio->map[SIO_PORT] = rdm_register_new_service(
                              (rdm_device_enum_type)psio->map[RDM_DEVICE],
                              OEMSIO_RDM_open_func, OEMSIO_UART2_rdm_close,
                              (rdm_service_enum_type*)&(psio->map[RDM_SRVC]));
#endif
#endif

#if defined (FEATURE_USB_CDC_ACM)
   // setup the USB port
   psio = GETSIO(USB_SIO_PORT);
   psio->map[RDM_DEVICE]               = RDM_USB_CDC_DEV;
   psio->map[SIO_PORT]                 = SIO_PORT_USB_CDC_ACM;

   psio->rx_func_ptr                   = OEMSIO_USBRx;
   psio->outgoingWMQ.lowater_func_ptr  = OEMSIO_USB_LowWatermarkFunc;
   psio->outgoingWMQ.hiwater_func_ptr  = OEMSIO_USB_HighWatermarkFunc;
   (void)STRLCPY(psio->szPortName, AEESIO_PORT_USB1, sizeof(psio->szPortName));

#ifndef FEATURE_BREW_SIO_NEW_RDM
   psio->map[SIO_PORT] = rdm_register_new_service(
                              (rdm_device_enum_type)psio->map[RDM_DEVICE],
                              OEMSIO_RDM_open_func, OEMSIO_USB_rdm_close,
                              (rdm_service_enum_type*)&(psio->map[RDM_SRVC]));
#endif
#endif

#ifdef FEATURE_BREW_SIO_NEW_RDM
   if(!rdm_set_srv_compat_mask(RDM_BREW_SRVC,
		(1 << RDM_UART1_DEV) | (1 << RDM_UART2_DEV) | (1 << RDM_USB_CDC_DEV)))
      return EFAILED;

   if (!rdm_register_func_multi_dev(RDM_BREW_SRVC, OEMSIO_RDM_multi_open, OEMSIO_RDM_multi_close, NULL) )
      return EFAILED;
#endif


   gSio.pfnNotify    = pfnNotify;

   return(SUCCESS);
}


/*=============================================================================
   Function: OEMSIO_Exit
 
   Parameters:
      None.
    
   Return Value:
      None.
=============================================================================*/
void OEMSIO_Exit( void )
{
   int i;
   gSio.pfnNotify = NULL;
   
   //Close all open ports as well...
   for(i=0; i<OEMSIO_MAX_PORTS; i++) {
      OEMSIO_Close(i);
#ifndef FEATURE_BREW_SIO_NEW_RDM
	  {
             SIO *psio = GETSIO(i);
	     if(psio->map[SIO_PORT] != SIO_PORT_NULL) {
	        rdm_unregister_service(psio->map[RDM_SRVC], 
                                  (rdm_device_enum_type)psio->map[RDM_DEVICE]);
             }
	  }
#endif

   }
}

/*=============================================================================
   Function: OEMSIO_GetPortID
 
   Parameters:
      pszName - Name of the port
      *phPort - pointer to a port handle
    
   Return Value:
      SUCCESS - Supports the requested port name, *phPort has valid port handle
      EFAILED - No support for the request port name. *phPort is not updated
=============================================================================*/
int OEMSIO_GetPortID(const char *pszName, uint32 *phPort)
{
   SIO *psio;
   int  i;

   for( i=0; i< OEMSIO_MAX_PORTS; i++ ) {
      psio = &(gSio.sio[i]);
      if(STRICMP(pszName, psio->szPortName) == 0) {
         if(psio->map[SIO_PORT] != SIO_PORT_NULL) {
            *phPort = psio->nPort;
            return SUCCESS;
         }
      }
   }

   return EFAILED;
}

SIO *OEMSIO_GetSIO_ByOEMID(sio_port_id_type sio_port_id)
{
   int i;

   for(i=0; i<OEMSIO_MAX_PORTS; i++) {
      if(gSio.sio[i].map[SIO_PORT] == sio_port_id)
         return &(gSio.sio[i]);
   }
   return NULL;
}

static void OEMSIO_NotifyState(SIO *psio)
{
   int nState = 0;
   if(gSio.pfnNotify) {
      if(psio->bBrewMode) {
         nState |= OEMSIO_ST_BREW;
         if(OEMSIO_IsReadable(psio)) {
            nState |= OEMSIO_ST_READABLE;
         }
         if (psio->bWriteable) {
            nState |= OEMSIO_ST_WRITEABLE;
         }
      } else {
         if(!CBuf_IsEmpty(&psio->cbufRead)) {
            nState |= OEMSIO_ST_READABLE;
         }

         if(psio->currentState == OEMSIO_ST_OPEN_FAILED) {
            nState |= OEMSIO_ST_WRITEABLE;
         }
      }
      if(nState != psio->nOEMState) {
         FARF(SIO, ("oemst: %d", nState));
         psio->nOEMState = nState;
         gSio.pfnNotify(psio->nPort, nState);
      }
   }
}

static void OEMSIO_SetState(SIO *psio, int nState)
{
   int nPrevState = psio->currentState;

   if(psio->currentState != (OEMSIO_State)nState) {
      FARF(SIO, ("OemPort %d: %d <- %d", psio->nPort, nState, psio->currentState));
      psio->currentState = (OEMSIO_State)nState;

      switch(nState) {
      
      case OEMSIO_ST_CLOSED:
         //Preprocessing for getting into CLOSED state
         if(psio->nStreamID != SIO_NO_STREAM_ID) {
            sio_close( psio->nStreamID, NULL );
         }
         psio->nStreamID = SIO_NO_STREAM_ID;
         psio->prevService = RDM_NULL_SRVC;
         psio->bBrewMode = 0;
         OEMSIO_SetDefaultConfig(&psio->SIOConfig);

         OEMSIO_NotifyState(psio);

         if((psio->bOpenRequested == TRUE) && 
            (nPrevState == OEMSIO_ST_CLOSE_IN_RDM)) {  

            OEMSIO_TryOpen( (void*)psio);
         }
         break;

      case OEMSIO_ST_OPENED:
         
         FARF(SIO, ("OemOpen: rate %d, flow %d", psio->SIOConfig.dwBaudRate, 
            psio->SIOConfig.fFlowControl));

         OEMSIO_NotifyState(psio);

         //If AEE has no open pending and this open is initiated by us,
         //then close
         if(psio->bOpenRequested == FALSE)
         {
            if (nPrevState == OEMSIO_ST_OPEN_IN_RDM) {
            OEMSIO_TryClose( (void*)psio);
         }
            else {
               // We must have got opened by ATCOP.  Make
               // a note of that.
               psio->prevService = RDM_DATA_SRVC;
            }
         }
         break;

      case OEMSIO_ST_OPEN_FAILED:
         ASSERT_FARF(psio->bBrewMode == 0);
         //Open failed, have to call Open again to retry...
         psio->bOpenRequested = FALSE;
         OEMSIO_NotifyState(psio);
         //As the AEESIO is notified of the failure, move to CLOSED state
         OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
         break;

      case OEMSIO_ST_OPEN_IN_RDM:
         psio->OpenSubStates = 2;
         break;

      case OEMSIO_ST_CLOSE_IN_RDM:
         psio->CloseSubStates = 2;
         psio->closesvccb_called = 0;
         psio->bBrewMode = 0;
         psio->bWriteable = 0;
         OEMSIO_NotifyState(psio);
         break;
      }
   }
}


#ifdef FEATURE_BREW_SIO_NEW_RDM
static void OEMSIO_RDM_multi_open(sio_port_id_type port_id, void *pv ) 
{ 
   SIO * psio = OEMSIO_GetSIO_ByOEMID(port_id);

   if(!psio) {
      rdm_notify(RDM_NULL_SRVC, RDM_NOT_ALLOWED_S);
      return;
   } 

   CALLBACK_Init(&psio->OpenCB, OEMSIO_RDM_SioOpenCB, psio);
   AEE_SYS_RESUME(&psio->OpenCB);
}

#else
static void OEMSIO_RDM_open_func(sio_port_id_type port_id ) 
{ 
   SIO * psio = OEMSIO_GetSIO_ByOEMID(port_id);

   if(!psio) {
      rdm_notify(NULL, RDM_NOT_ALLOWED_S);
      return;
   } 

   CALLBACK_Init(&psio->OpenCB, OEMSIO_RDM_SioOpenCB, psio);
   AEE_SYS_RESUME(&psio->OpenCB);
}
#endif

static void OEMSIO_RDMStatusBrewCB(void *pv)
{
   SIO *psio = (SIO *)pv;

   switch(psio->currentState) {
      
      case OEMSIO_ST_OPEN_IN_RDM:
         
         if(psio->nRDMStatus == RDM_DEVMAP_BUSY_S) {
            //Wait for RDM to be able to service us
            CALLBACK_Init(&psio->TimerCB, OEMSIO_TryOpen, psio);
            AEE_SetSysTimerCallback(RDM_BUSY_REATTEMPT_TIMER, &psio->TimerCB);
            OEMSIO_SetState(psio, OEMSIO_ST_OPEN_RDM_BUSY);
         
         } else if(psio->nRDMStatus == RDM_DONE_S) {
            //State will be automatically set to OPEN when
            //the port is really opened...
              psio->OpenSubStates--;
              if(!psio->OpenSubStates) {
               OEMSIO_SetState(psio, OEMSIO_ST_OPENED);
              }
         } else {
            FARF(SIO, ("Bad RDM status"));
            //do what
            OEMSIO_SetState(psio, OEMSIO_ST_OPEN_FAILED);
         }
      break;

      case OEMSIO_ST_CLOSE_IN_RDM:
         psio->closesvccb_called = 0;

         if(psio->nRDMStatus == RDM_DEVMAP_BUSY_S) {
            //Start a timer to re-try
            CALLBACK_Init(&psio->TimerCB, OEMSIO_TryClose, pv);
            AEE_SetSysTimerCallback(RDM_BUSY_REATTEMPT_TIMER, &psio->TimerCB);
            OEMSIO_SetState(psio, OEMSIO_ST_CLOSE_RDM_BUSY);
         
         } else if(psio->nRDMStatus == RDM_DONE_S) {
         
            //This must be set in the port is really closed...
            psio->CloseSubStates--;
            if(!psio->CloseSubStates) {
               OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
            }
         
         } else {
            
            //Failed to close..
            //so set to CLOSED state any way...
            FARF(SIO, ("Bad RDM status"));
            OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
         }
      break;

      default:
         FARF(SIO, ("ERR_FATAL - Wrong SIO state in RDM status: %d",
                    psio->currentState ));
   }

}



void OEMSIO_RDMStatusCB(rdm_assign_status_type status,
                         rdm_service_enum_type service,
                         rdm_device_enum_type device)
{
   SIO *psio = NULL; 
   int i;

   for( i=0; i< OEMSIO_MAX_PORTS; i++ ) {
      psio = &(gSio.sio[i]);
         FARF(SIO, ("OEMSIO_RDMStatusCB, device: %d, %d, %d",
                     device, psio->currentState,
                    psio->closesvccb_called));

      if(device == RDM_NULL_DEV && psio->currentState == OEMSIO_ST_CLOSE_IN_RDM &&
         !psio->closesvccb_called)
      {
        break;
      }
      else if( (device != RDM_NULL_DEV) &&
               (device == gSio.sio[i].map[RDM_DEVICE])) {
         break;
      }
   }

   if(psio) {
      psio->nRDMStatus = status;
      CALLBACK_Init(&psio->StatusCB, OEMSIO_RDMStatusBrewCB, psio);
      psio->closesvccb_called = 1;
      AEE_SYS_RESUME(&psio->StatusCB);
   }
}


static void OEMSIO_TryOpen(void *pv)
{
   SIO *psio = (SIO *)pv;
   
   psio->prevService = rdm_get_service( (rdm_device_enum_type)psio->map[RDM_DEVICE] );
      
   //RDM is set in motion. Preset this state as calling assign_port 
   //can set the async calls in progress before even it returns
   OEMSIO_SetState(psio, OEMSIO_ST_OPEN_IN_RDM);

   ASSERT_FARF(psio->nRDMStatus == RDM_ASSIGN_STATUS_MAX_S);
   rdm_assign_port( (rdm_service_enum_type) psio->map[RDM_SRVC], 
                    (rdm_device_enum_type) psio->map[RDM_DEVICE], 
                    OEMSIO_RDMStatusCB); 
}


/*=============================================================================
   Function: OEMSIO_Open
    
   Parameters:
      nPort    COMM port to open
   
   Return Value:
     SUCCESS   port successfully opened
     EFAILED   failed to open port
=============================================================================*/
int OEMSIO_Open(uint32 nPort)
{
   SIO *psio   = GETSIO(nPort);

   if(!psio) {
      FARF(ALWAYS, ("Tring to open a non-exisiting port %d", nPort));
      return ENOSUCH;
   }

   FARF(SIO, ("OEMSIO_Open() called"));

   if(psio->currentState == OEMSIO_ST_OPEN_FAILED) {
      FARF(SIO, ("Moving to CLOSED from OPEN_FAILED in open()"));
      OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
   }

   psio->bOpenRequested = TRUE;
   psio->bInboundFlowDisabled = 1;

   switch(psio->currentState) {
      case OEMSIO_ST_OPEN_RDM_BUSY:
      case OEMSIO_ST_OPEN_IN_RDM:
      case OEMSIO_ST_CLOSE_IN_RDM:
         //Keep going... will be handled later
         break;

      case OEMSIO_ST_OPENED:
         ASSERT_FARF(psio->nStreamID != SIO_NO_STREAM_ID);
         //already open...
         return SUCCESS;

      case OEMSIO_ST_CLOSE_RDM_BUSY:
         //Cancel the timer and goback to being open...
         CALLBACK_Cancel(&psio->TimerCB);
         OEMSIO_SetState(psio, OEMSIO_ST_OPENED);
         return SUCCESS;

      case OEMSIO_ST_CLOSED:
         OEMSIO_TryOpen((void *)psio);
         break;
   }

   return IPORT_WAIT;
}


sio_bitrate_type OEMSIO_ConvertBitRateTOSioRate(int dwBaudRate, int *pNormBaudRate)
{
   sio_bitrate_type sio_rate;
   int bitrate;

   //Normalize given rate to near-by SIO_BITRATE
   if(dwBaudRate <= 450) {
      sio_rate = SIO_BITRATE_300;
      bitrate = 300;
   } else if(dwBaudRate <= 900) {
      sio_rate = SIO_BITRATE_600;
      bitrate = 600;
   } else if(dwBaudRate <= 1800) {
      sio_rate = SIO_BITRATE_1200;
      bitrate = 1200;
   } else if(dwBaudRate <= 3600) {
      sio_rate = SIO_BITRATE_2400;
      bitrate = 2400;
   } else if(dwBaudRate <= 7200) {
      sio_rate = SIO_BITRATE_4800;
      bitrate = 4800;
   } else if(dwBaudRate <= 14400) {
      sio_rate = SIO_BITRATE_9600;
      bitrate = 9600;
   } else if(dwBaudRate <= 28800) {
      sio_rate = SIO_BITRATE_19200;
      bitrate = 19200;
   } else if(dwBaudRate <= 48000) {
      sio_rate = SIO_BITRATE_38400;
      bitrate = 38400;
   } else if(dwBaudRate <= 86400) {
      sio_rate = SIO_BITRATE_57600;
      bitrate = 57600;
   } else {
      sio_rate = SIO_BITRATE_115200;
      bitrate = 115200;
   }
   if(pNormBaudRate) {
      *pNormBaudRate = bitrate;
   }
   
   return sio_rate;
}

/*=============================================================================
   Function: OEMSIO_SetConfig
   
   Parameters:
      port           COM port to configure
      pSIOConfig    Pointer to AEESIOConfig containing configuration data.
   
   Return Value:
      SUCCESS
      EFAILED
=============================================================================*/
int OEMSIO_SetConfig( uint32 nPort, AEESIOConfig *pConfig )
{
   SIO                  *psio = GETSIO(nPort);
   sio_ioctl_param_type  param;
   int                   dwNormalizedBaudRate;

   if(NULL == psio) {
      return EFAILED;
   }

   param.bitrate = 
      OEMSIO_ConvertBitRateTOSioRate(pConfig->dwBaudRate, &dwNormalizedBaudRate);

   if(( psio->nStreamID != SIO_NO_STREAM_ID) &&
        sio_is_baud_valid(param.bitrate)     &&
        (psio->SIOConfig.dwBaudRate != dwNormalizedBaudRate)) {

      sio_ioctl(psio->nStreamID, SIO_IOCTL_CHANGE_BAUD_NOW, &param);
      FARF(SIO, ("OemFlow: Baud %d", dwNormalizedBaudRate));
   }

   if((psio->nStreamID != SIO_NO_STREAM_ID) &&
      (psio->SIOConfig.fFlowControl != pConfig->fFlowControl)) {

      param.flow_ctl.tx_flow = param.flow_ctl.rx_flow =
         CNVRT_FLOW_CNTRL(pConfig->fFlowControl);
      sio_ioctl(psio->nStreamID, SIO_IOCTL_SET_FLOW_CTL, &param);
      FARF(SIO, ("OemFlow: flow %d", pConfig->fFlowControl));
   }

   psio->SIOConfig.fFlowControl = pConfig->fFlowControl;
   psio->SIOConfig.dwBaudRate   = dwNormalizedBaudRate;

   return SUCCESS;
}


/*=============================================================================
   Function: OEMSIO_GetConfig
   
   Parameters:
      port           COMM port to configure
      pSIOConfig    Pointer to AEESIOConfig containing configuration data.
   
   Return Value:
      SUCCESS
      EFAILED
=============================================================================*/
int OEMSIO_GetConfig( uint32 nPort, AEESIOConfig *pSIOConfig )
{
   SIO *psio = GETSIO(nPort);

   if( pSIOConfig && psio ) {
      *pSIOConfig = psio->SIOConfig;
      return SUCCESS;
   }
   return EFAILED;
}


int OEMSIO_IOCtl(uint32 nPort, int nOption, uint32 dwVal)
{
   SIO * psio = GETSIO(nPort);

   if(!psio)
      return EFAILED;

   if(nOption == ((int)AEESIO_IOCTL_SCONFIG))
   {
      return OEMSIO_SetConfig(nPort, (AEESIOConfig *)dwVal);
   }
   else if(nOption == ((int)AEESIO_IOCTL_GCONFIG))
   {
      return OEMSIO_GetConfig(nPort, (AEESIOConfig *)dwVal);
   }
   else if(nOption == ((int)AEESIO_IOCTL_SET_TRIGGERS))
      {
         AEESIOTriggers *pTriggers = (AEESIOTriggers *)dwVal;
         psio->nReadIdleTimeout   = pTriggers->nReadIdleTimeout;
         if(psio->nReadIdleTimeout) {
         psio->cbReadTrigger  = CONSTRAIN(pTriggers->cbReadTrigger , 1, SIO_READ_BUFFER_SIZE);
         psio->cbWriteTrigger = CONSTRAIN(pTriggers->cbWriteTrigger, 1, SIO_WRITE_BUFFER_SIZE);
      }

      FARF(SIO, ("ReadIdle %d ReadTrig % WriteTrig %d", psio->nReadIdleTimeout,
                     psio->cbReadTrigger, psio->cbWriteTrigger));
            
      return SUCCESS;
      }
   else
         {
   return EUNSUPPORTED;
}
}


/*=============================================================================
   Function: OEMSIO_Read
   
   Parameters:
   
   Return Value:
     The number of bytes removed from the receive buffer.
   
     If the receive buffer is empty, no bytes should be copied and this function
     should return 0.
   
     If the port is not in BREW serial mode, -1 should be returned.
   
=============================================================================*/
int OEMSIO_Read( uint32 nPort, char *pcBuf, int nLen )
{
   int   cbAvail;
   int   i;
   int   cbRead   = 0;
   SIO  *psio     = GETSIO(nPort);
   dword dwSave   = 0;

   if (!psio || !psio->bBrewMode || nPort != psio->nPort)
      return -1;

   //Lock to not cause conflicts with the RX ISR
   INTLOCK_SAV(dwSave);
   
   // perform a maximum of two reads from circular buffer
   for (i=0; i < 2; i++) {
      char * p = CBuf_GetContiguousOutput(&psio->cbufRead, &cbAvail);
      if ((cbAvail > 0) && (nLen > 0)) {
         int cb = MIN((int)cbAvail, nLen);
         MEMCPY(pcBuf+cbRead, p, cb);
         CBuf_AdvanceOut(&psio->cbufRead, cb);
         cbRead += cb;
         nLen -= cb;
      }
   }

   //FARF(SIO, ("OemRead %d", cbRead));

   //Enable inbound flow if enough free space
   if(psio->bInboundFlowDisabled && (psio->nStreamID != SIO_NO_STREAM_ID) &&
      (CBuf_GetFree(&psio->cbufRead) > SIO_INBOUND_HYSTERESIS)) {
      sio_ioctl_param_type param= {0};

      FARF(SIO, ("Inbound flow enabled %d",psio->bInboundFlowDisabled));
      sio_ioctl(psio->nStreamID, SIO_IOCTL_INBOUND_FLOW_ENABLE, &param);
      psio->bInboundFlowDisabled = FALSE;
   }

   OEMSIO_StartReadIdelTimer(psio);
   OEMSIO_NotifyState(psio);

   INTFREE_SAV(dwSave);


   return cbRead;
}


/*=============================================================================
   Function: OEMSIO_Write
   
   Parameters:
   
   Return Value:
   
     The number of bytes appended to the transmit buffer.
   
     If the receive buffer is full, no bytes should be copied and this function
     should return 0.
   
     If the port is not in BREW serial mode, -1 should be returned.
   
=============================================================================*/
int OEMSIO_Write( uint32 nPort, const char *pcBuf, int nLen )
{
   SIO *psio = GETSIO(nPort);
   dsm_item_type *item_ptr;
   int nWritten = 0;

   if (!psio || !psio->bBrewMode || nPort != psio->nPort)
      return -1;
   
   while( psio->bWriteable &&
         ((item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)) != NULL)) {

      if(nLen > item_ptr->size) {
         MEMCPY(item_ptr->data_ptr, pcBuf+nWritten, item_ptr->size);
         item_ptr->used = item_ptr->size;
         nWritten += item_ptr->size;
         nLen -= item_ptr->size;
         sio_transmit(psio->nStreamID, item_ptr);
      
      } else {
         MEMCPY(item_ptr->data_ptr, pcBuf+nWritten, nLen);
         item_ptr->used = nLen;
         nWritten += nLen;
         nLen = 0;
         sio_transmit(psio->nStreamID, item_ptr);
         break;
      }
   }

   if(!psio->bWriteable) {
      OEMSIO_NotifyState(psio);
   }
   return nWritten;
}


/*=============================================================================
   Function: OEMSIO_RDM_SioCloseCB
   
     This callback is called when RDM wants BREW to close a port.

   Parameters:
   Return Value:  None.
   
=============================================================================*/
static void OEMSIO_RDM_SioCloseCB( void *pv)
{
   SIO *psio  = (SIO *)pv;

   
   if(psio->currentState == OEMSIO_ST_CLOSE_IN_RDM) {
      //Close the port before we tell RDM
      if(psio->nStreamID != SIO_NO_STREAM_ID) {
         
         dword dwSave   = 0;
         wm_cb_type wmfn;
         //Lock to not cause conflicts with the RX ISR
         INTLOCK_SAV(dwSave);

         //Make sure the lowater function does not get 
         //called while emptying the queue
         wmfn = psio->outgoingWMQ.lowater_func_ptr;
         //Make sure the outgoing queue is empty...
         dsm_empty_queue(&psio->outgoingWMQ);
         psio->outgoingWMQ.lowater_func_ptr = wmfn;

         INTFREE_SAV(dwSave);

         sio_close( psio->nStreamID, NULL );
         psio->nStreamID = SIO_NO_STREAM_ID;
      }

      rdm_notify((rdm_service_enum_type)psio->map[RDM_SRVC], RDM_DONE_S);

      //As RDM is informed, set state to closed...
      psio->CloseSubStates--;
      if(!psio->CloseSubStates) {
         OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
      }
   } else {
      // This will close the port and notify the app
      rdm_notify((rdm_service_enum_type)psio->map[RDM_SRVC], RDM_DONE_S);
      OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
   }

}

   


/*=============================================================================
   Function: OEMSIO_RDM_SioOpenCB
   
     This callback is called when RDM wants BREW to open a port.

   Parameters:
   Return Value:  None.
   
=============================================================================*/
static void OEMSIO_RDM_SioOpenCB( void *pv)
{
   SIO            *psio  = (SIO *)pv;
   sio_open_type   openCfg;
   
   CBuf_Ctor(&psio->cbufRead, psio->aRead, SIO_READ_BUFFER_SIZE);
   
   // setup serial port opening parameters.
   openCfg.port_id         = (sio_port_id_type)psio->map[SIO_PORT];
   openCfg.stream_mode     = SIO_GENERIC_MODE;
   openCfg.rx_func_ptr     = psio->rx_func_ptr;
   openCfg.rx_queue        = NULL;
   openCfg.tx_queue        = &psio->outgoingWMQ;

   openCfg.tail_char_used  = FALSE;
   openCfg.tail_char       = 0;

   
   if(psio->bOpenRequested) {
      sio_bitrate_type sio_rate =
         OEMSIO_ConvertBitRateTOSioRate(psio->SIOConfig.dwBaudRate, NULL);
      openCfg.rx_bitrate = openCfg.tx_bitrate = sio_rate;
      openCfg.rx_flow = openCfg.tx_flow = CNVRT_FLOW_CNTRL( psio->SIOConfig.fFlowControl );

   } else {
      openCfg.rx_bitrate = openCfg.tx_bitrate = SIO_BITRATE_115200;
      openCfg.rx_flow = openCfg.tx_flow        = SIO_FCTL_OFF; 
   }

   //Ready for a new connection
   dsm_empty_queue(&psio->outgoingWMQ);
   psio->bWriteable  = 1;

   // open port here
   psio->nStreamID = sio_open( &openCfg );

   if( psio->nStreamID != SIO_NO_STREAM_ID ){
      // tell brew we have connected
      
      rdm_notify((rdm_service_enum_type)psio->map[RDM_SRVC], RDM_DONE_S);

      psio->bBrewMode = 1;

      if(psio->currentState == OEMSIO_ST_OPEN_IN_RDM) {
         psio->OpenSubStates--;
         if(!psio->OpenSubStates)
            OEMSIO_SetState(psio, OEMSIO_ST_OPENED);
      } else {
         OEMSIO_SetState(psio, OEMSIO_ST_OPENED);
      }
      
      //Schedule for a DTR event
      if(psio->dtr_event_func_ptr) {
         sio_ioctl_param_type param;
         param.enable_dte_ready_event = psio->dtr_event_func_ptr;
         sio_ioctl(psio->nStreamID, SIO_IOCTL_ENABLE_DTR_EVENT, &param);
      }

   } else {
      psio->bBrewMode = 0;
      //Wait for RDM to notify if we have initated this open()
      FARF(SIO, ("sio_open() failed"));

      //Change to OPENED irrespective of the rdm_notify as 
      //when it gets called is not predictable
      OEMSIO_SetState(psio, OEMSIO_ST_OPEN_FAILED);
      
      //for some reason the port can not be open
      //choose the best available error code for RDEVMAP
      rdm_notify((rdm_service_enum_type)psio->map[RDM_SRVC], RDM_APP_BUSY_S);
   }
   
}


static void OEMSIO_TryClose(void *pv)
{
   SIO *psio = (SIO *)pv;

   ASSERT_FARF(psio->nStreamID != SIO_NO_STREAM_ID);

   OEMSIO_SetState(psio, OEMSIO_ST_CLOSE_IN_RDM);
   ASSERT_FARF(psio->nRDMStatus == RDM_ASSIGN_STATUS_MAX_S);
   FARF(ALWAYS, ("psio->nRDMStatus: %d", psio->nRDMStatus));

   /*
      When closing the port, it should be given back to the previous owner.
      If there was no previous owner it should be closed.
   */

   if (rdm_get_service((rdm_device_enum_type)psio->map[RDM_DEVICE]) == psio->map[RDM_SRVC])
   {
#ifdef FEATURE_BREW_SIO_NEW_RDM
   if( psio->prevService == RDM_NULL_SRVC)
   {
         (void)rdm_close_device( RDM_BREW_SRVC, 
                                 (rdm_device_enum_type)psio->map[RDM_DEVICE], 
                              OEMSIO_RDMStatusCB );

         if( (rdm_device_enum_type)psio->map[RDM_DEVICE] == RDM_UART1_DEV 
          && rdm_get_device(RDM_DATA_SRVC) == RDM_NULL_DEV)
      {
            (void)rdm_assign_port( RDM_NULL_SRVC, 
                                   (rdm_device_enum_type)psio->map[RDM_DEVICE], 
                                OEMSIO_RDMStatusCB );
      }
#else
   if((psio->prevService == RDM_NULL_SRVC) 
         || ((rdm_device_enum_type)psio->map[RDM_DEVICE] == RDM_UART1_DEV 
         && rdm_get_device(RDM_DATA_SRVC) == RDM_NULL_DEV))
   {
         (void)rdm_assign_port( RDM_DATA_SRVC, 
                                (rdm_device_enum_type)psio->map[RDM_DEVICE], 
                             OEMSIO_RDMStatusCB );
#endif
   } else {
         (void)rdm_assign_port( psio->prevService, 
                                (rdm_device_enum_type)psio->map[RDM_DEVICE], 
                                OEMSIO_RDMStatusCB );
   }
}
}


/*=============================================================================
   Function: OEMSIO_Close
   
   Parameters:
      port        COM port to close
   
   Return Value:
      none
=============================================================================*/
void OEMSIO_Close( uint32 nPort )
{
   // get the sio struct
   SIO *psio = GETSIO(nPort);

   if(!psio) {
      FARF(ALWAYS, ("Tring to close a non-existing port %d", nPort));
      return;
   }

   FARF(SIO, ("OEMSIO_Close() called"));
   psio->bOpenRequested = FALSE;

   switch(psio->currentState) {
      case OEMSIO_ST_OPEN_RDM_BUSY:
         CALLBACK_Cancel(&psio->TimerCB);
         OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
         break;

      case OEMSIO_ST_OPEN_IN_RDM:
      case OEMSIO_ST_CLOSE_RDM_BUSY:
      case OEMSIO_ST_CLOSE_IN_RDM:
         //Keep going... will be handled later
         break;

      case OEMSIO_ST_OPENED:
         OEMSIO_TryClose( (void *)psio);
         break;

      case OEMSIO_ST_OPEN_FAILED:

         //Transient state..
         ASSERT_FARF(psio->currentState != OEMSIO_ST_OPEN_FAILED);
         //OEMSIO_SetState(psio, OEMSIO_ST_CLOSED);
         break;

      case OEMSIO_ST_CLOSED:
         //We are done...
         break;
   }
}

                    
#ifdef FEATURE_BREW_SIO_NEW_RDM
static void OEMSIO_RDM_multi_close(sio_port_id_type port_id, void *pv)
{
   SIO * psio = OEMSIO_GetSIO_ByOEMID(port_id);
   pv = pv; //ignore pv
 
   if(NULL != psio) {
      CALLBACK_Init(&psio->CloseCB, OEMSIO_RDM_SioCloseCB, psio);
      AEE_SYS_RESUME(&psio->CloseCB);
   }
}

#else

/*=============================================================================
   Function: OEMSIO_UART1_rdm_close

      This callback is called when RDM wants BREW to close its port.

   Parameters:

   Return Value:  None.

=============================================================================*/
static void OEMSIO_UART1_rdm_close( void )
{
   SIO *psio = GETSIO(MAIN_SIO_PORT);

   CALLBACK_Init(&psio->CloseCB, OEMSIO_RDM_SioCloseCB, psio);
   AEE_SYS_RESUME(&psio->CloseCB);
}

#if defined (FEATURE_SECOND_UART)
static void OEMSIO_UART2_rdm_close( void )
{
   SIO *psio = GETSIO(AUX_SIO_PORT);

   CALLBACK_Init(&psio->CloseCB, OEMSIO_RDM_SioCloseCB, psio);
   AEE_SYS_RESUME(&psio->CloseCB);
}
#endif


#if defined (FEATURE_USB_CDC_ACM)
static void OEMSIO_USB_rdm_close( void )
{
   SIO *psio = GETSIO(USB_SIO_PORT);

   CALLBACK_Init(&psio->CloseCB, OEMSIO_RDM_SioCloseCB, psio);
   AEE_SYS_RESUME(&psio->CloseCB);
}
#endif

#endif //#ifdef FEATURE_BREW_SIO_NEW_RDM


/*=============================================================================
   Function: OEMSIO_Rx

   Parameters:
   
   Return Value:  None.
   
=============================================================================*/
static void OEMSIO_Rx(SIO *psio, dsm_item_type** ppItem)
{

   dsm_item_type* pItem;
   if(!psio || !ppItem || (NULL == *ppItem))
   {
      return;
   }

   pItem = *ppItem;

   while (pItem)
   {
      int RxSize = (int)(pItem)->used;
      dsm_item_type* pItem_next = (dsm_item_type*)(pItem)->pkt_ptr;
      //FARF(SIO, ("RxSize %d", RxSize));

      RxSize -= CBuf_Fill(&psio->cbufRead, (const char *)(pItem)->data_ptr, RxSize);

      if(RxSize) {
         FARF(ALWAYS, ("Rx Data Overflowed, lost %d bytes", RxSize));
      }

      dsm_free_buffer(pItem);
      pItem = pItem_next;
   }
   *ppItem = NULL;

   if(CBuf_GetFree(&psio->cbufRead) < SIO_INBOUND_OVERFLOW_ALLOWANCE) {
      sio_ioctl_param_type param={0};

      //Do Flow control
      FARF(SIO, ("Inflow disabled-free %d -flg %d",
         CBuf_GetFree(&psio->cbufRead),psio->bInboundFlowDisabled));
      psio->bInboundFlowDisabled = TRUE;
      sio_ioctl(psio->nStreamID, SIO_IOCTL_INBOUND_FLOW_DISABLE, &param);
   }

   OEMSIO_SetTaskWorker(psio, SIO_NOTIFY | SIO_IDLE_TIMER);
}




static void OEMSIO_ReadIdelTimerExpired(SIO *psio)
{
   OEMSIO_NotifyState(psio);
}

static void OEMSIO_StartReadIdelTimer(SIO *psio)
{
   psio->tLastReceived = GETUPTIMEMS();
   if(psio->nReadIdleTimeout && (CBuf_GetCount(&psio->cbufRead) < psio->cbReadTrigger)) {

      CALLBACK_Init(&psio->TimerCB, OEMSIO_ReadIdelTimerExpired, psio);
      AEE_SetSysTimerCallback(psio->nReadIdleTimeout, &psio->TimerCB);
   }
}


static int OEMSIO_IsReadable(SIO *psio)
{
   int cbRead = CBuf_GetCount(&psio->cbufRead);
   //FARF(SIO,("Is_readable -- RdSize %d", cbRead));

   //Valid Read Idle time makes the triggers active as well
   if(psio->nReadIdleTimeout) {

	  uint32 tNow = GETUPTIMEMS();
      if( (cbRead >= psio->cbReadTrigger) ||
          ((cbRead > 0) && ((tNow - psio->tLastReceived) >= psio->nReadIdleTimeout))) {

         return TRUE;
      }
	
   } else if(cbRead) {
   
      //if no idle time is specified, any bytes in the read buf makes it readable
	
		return TRUE;
	}
   
	return FALSE;
}




#if defined (FEATURE_BREW_SIO_PORT1_SUPPORTED)
/*=============================================================================
   Function: OEMSIO_UART1Rx

   Parameters:

   Return Value:  None.

=============================================================================*/
static void OEMSIO_UART1Rx( dsm_item_type** ppItem )
{
   SIO *psio = GETSIO(MAIN_SIO_PORT);
   OEMSIO_Rx(psio, ppItem);
}

/*=============================================================================
   Function: OEMSIO_UART1_LowWatermarkFunc
=============================================================================*/
static void OEMSIO_UART1_LowWatermarkFunc( void)
{
   SIO *psio = GETSIO(MAIN_SIO_PORT);

   psio->bWriteable = 1;
   OEMSIO_SetTaskWorker(psio, SIO_NOTIFY);
}

/*=============================================================================
   Function: OEMSIO_UART1_HighWatermarkFunc
=============================================================================*/
static void OEMSIO_UART1_HighWatermarkFunc( void)
{
   SIO *psio = GETSIO(MAIN_SIO_PORT);

   psio->bWriteable = 0; //Not writeable
   OEMSIO_SetTaskWorker(psio, SIO_NOTIFY);
}
#endif //defined (FEATURE_BREW_SIO_PORT1_SUPPORTED)

#if defined (FEATURE_SECOND_UART)
/*=============================================================================
   Function: OEMSIO_UART2Rx

   Parameters:

   Return Value:  None.

=============================================================================*/
static void OEMSIO_UART2Rx( dsm_item_type** ppItem )
{
   SIO *psio = GETSIO(AUX_SIO_PORT);
   OEMSIO_Rx(psio, ppItem);
}

/*=============================================================================
   Function: OEMSIO_UART2_LowWatermarkFunc
=============================================================================*/
static void OEMSIO_UART2_LowWatermarkFunc( void )
{
   SIO *psio = GETSIO(AUX_SIO_PORT);

   psio->bWriteable = 1;
   OEMSIO_SetTaskWorker(psio, SIO_NOTIFY);
}

/*=============================================================================
   Function: OEMSIO_UART2_HighWatermarkFunc
=============================================================================*/
static void OEMSIO_UART2_HighWatermarkFunc( void )
{
   SIO *psio = GETSIO(AUX_SIO_PORT);

   psio->bWriteable = 0; //Not writeable
   OEMSIO_SetTaskWorker(psio, SIO_NOTIFY);
}
#endif


#if defined (FEATURE_USB_CDC_ACM)
/*=============================================================================
   Function: OEMSIO_USBRx

   Parameters:

   Return Value:  None.

=============================================================================*/
static void OEMSIO_USBRx( dsm_item_type** ppItem )
{
   SIO *psio = GETSIO(USB_SIO_PORT);
   OEMSIO_Rx(psio, ppItem);
}

/*=============================================================================
   Function: OEMSIO_USB_LowWatermarkFunc
=============================================================================*/
static void OEMSIO_USB_LowWatermarkFunc( void )
{
   SIO *psio = GETSIO(USB_SIO_PORT);

   psio->bWriteable = 1;
   OEMSIO_SetTaskWorker(psio, SIO_NOTIFY);
}

/*=============================================================================
   Function: OEMSIO_USB_HighWatermarkFunc
=============================================================================*/
static void OEMSIO_USB_HighWatermarkFunc( void )
{
   SIO *psio = GETSIO(USB_SIO_PORT);

   psio->bWriteable = 0;
   OEMSIO_SetTaskWorker(psio, SIO_NOTIFY);
}
#endif



static void OEMSIO_SetDefaultConfig(AEESIOConfig *pConfig)
{
   pConfig->dwBaudRate     = 115200;
   pConfig->fFlowControl   = AEESIO_FLOW_NONE;
   pConfig->fParity        = AEESIO_PARITY_NONE;
   pConfig->nDataBits      = AEESIO_DATABITS_8;
   pConfig->nStopBits      = AEESIO_STOPBITS_1;
}


/* CBuf Implementation */
#define Advance(v, n, cb)           (v) = ((v) + (n)) % (cb)

void CBuf_Ctor(CBuf * pme, char * pBuffer, int nSize)
{
   pme->pData  = pBuffer;
   pme->cb     = nSize;
   pme->iOut   = 0;
   pme->iIn    = 0;
}

int CBuf_GetCount(CBuf * pme)
{
   int n = pme->iIn - pme->iOut;
   return(n < 0) ? (pme->cb + n) : n;
}

char * CBuf_GetContiguousOutput(CBuf * pme, int * pn)
{
   int nCount = CBuf_GetCount(pme);
   *pn = MIN(nCount, (pme->cb - pme->iOut));
   return pme->pData + pme->iOut;
}

char * CBuf_GetContiguousInput(CBuf * pme, int * pn)
{
   int nFree = CBuf_GetFree(pme);
   *pn = MIN(nFree, (pme->cb - pme->iIn));
   return pme->pData + pme->iIn;
}

int CBuf_Fill(CBuf * pme, const char * pData, int cbData)
{
   int cbAvail;
   int i;
   int cbFill = 0;

   // write into buffer - may write twice if
   // the circular buffer needs to wrap.
   for (i=0; i < 2; i++) {
      char * p = CBuf_GetContiguousInput(pme, &cbAvail);
      if ((cbAvail > 0) && (cbData > 0)) {
         int cb = MIN((int)cbAvail, cbData);
         MEMCPY(p, pData, cb);
         CBuf_AdvanceIn(pme, cb);
         pData += cb;
         cbFill += cb;
         cbData -= cb;
      }
   }
   return cbFill;
}
#endif // FEATURE_BREW_SIO

// end OEMSIO.c
