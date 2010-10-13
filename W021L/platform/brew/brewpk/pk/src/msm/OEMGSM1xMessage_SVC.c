/*======================================================     
FILE:  OEMGSM1xMessage_SVC.c

SERVICES:  OEM GSM1x Messaging interface

GENERAL DESCRIPTION:
   Used by GSM1x intefaces to access SMS routines.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREM/ENTS:


        Copyright © 2003-2006 by QUALCOMM, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "customer.h"
#include "cm.h"
#include "bit.h"
#include "msg.h"

#include "OEMFeatures.h"

/* The following ifdef affects the whole file.*/                          
#if defined(FEATURE_GSM1x)

#include "OEMGSM1xMessage_SVC.h"
#include "OEMGSMSMS_SVC.h"
#include "OEMGSM1xSIG_SVC.h"

#if !defined(FEATURE_GSM1x_WMS)
#include "uasmsi.h"
#endif
#include "OEMConfig.h"
#include "OEMCFGI.h"
#include "OEMCriticalSection.h"

//#include "AEE.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEEHandle.h"

#include "AEEStdLib.h"
#include "AEEGSM1xSig.h"

#include "OEMPhone.h"


/*===========================================================================

                      EXTERNAL FUNCTIONS

===========================================================================*/

// Need this routine from OEMGSMSMS.c
extern void GSMSMS_StatusCallback(void *userData, int status);

// Need this routine from OEMGSM1xControl.c file
//
extern AEEGSM1xSig_Status OEMGSM1xControl_SVC_GetStatus(void);

// Need this routine from OEMGSM1xMessage.c file
//
extern void OEMGSM1xMessage_ScheduleAppClosedCallback(uint32 handle);

// Need this routine from OEMGSM1xMessage.c file
//
extern void OEMGSM1xMessage_CancelAppClosedCallback(uint32 handle);

// Need this routine from OEMGSMSMS.c file
//
int GSMSMS_ScheduleRerouteTapi (const char * pszDst,
                                const char * pszMsg,
                                AEECLSID     clsDst,
                                uint32     * pdwStatus,
                                AEECallback* pcb);


/*===========================================================================

                     DEFINES

===========================================================================*/
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */

// SMS_MESSAGE_TYPE is used to reduce simple ifdef FEATURE GSM1x
//
#ifdef FEATURE_GSM1x_WMS
#define SMS_MESSAGE_TYPE wms_client_message_s_type
#else
#define SMS_MESSAGE_TYPE uasms_client_message_type
#endif


//The auto-disconnect timeout period for the DTC (dedicated traffic channel)
//in seconds
#define SO6_TIMEOUT_LENGTH  35

// This value defines how long a MOSMS transaction can be outstanding before
// it is cleared up. Normally uasms should report status long before this
// time-out value. 
// Value in milli-seconds
#define MAX_MOSMS_TIMER 30000            // 30 seconds


/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

typedef enum {

   GSM1xMESSAGE_MSG_SIG,
   GSM1xMESSAGE_MSG_SMS

} OEMGSM1xMessage_MsgType;

// Used by OEMGSM1xMessage_sendSMS 
//
typedef enum {

   GSM1xMESSAGE_TRAFFIC_CHANNEL,
   GSM1xMESSAGE_ACCESS_CHANNEL

} GSM1xMessage_SMSSendMethod;


// Submitted MO SMS goes through the following states. A MO SMS may not go
// go through all the states, but the states should always increase.
//
typedef enum {

   GSM1x_MOSMS_STATE_FREE = 0,
   GSM1x_MOSMS_STATE_QUEUED,
   GSM1x_MOSMS_STATE_SEND_ON_ACCESS,
   GSM1x_MOSMS_STATE_WAITING_FOR_DTC,
   GSM1x_MOSMS_STATE_SEND_ON_DTC,
   GSM1x_MOSMS_STATE_COMPLETED

} GSM1x_MOSMS_State;

// OEMGSM1xMessage uses this enum to indicate the state of the DTC connection.
typedef enum {

   GSM1xMESSAGE_STATE_NO_DTC_SETUP,  // This doesn't imply that there is no DTC, just
                                     // that there is no DTC setup by OEMGSM1xMessage.
   GSM1xMESSAGE_STATE_CONNECTING,
   GSM1xMESSAGE_STATE_CONNECTED

} GSM1xMessage_DTC_State;


// Callbacks for MO-SMS deliver Notifications
//

typedef struct 
{
   PFNSMSSTATUS                  pfn;     //Notify Function for User
   void *                        pCtx;    //App Context
   void *                        pUser;   //User Data for Notify Function

   AEECallback                   cb;      // Used to report status to user
   GSM1x_MOSMS_State             eState;

#ifdef FEATURE_GSM1x_WMS
   wms_error_class_e_type        nErrorClass;
   wms_status_e_type             nStatus;
   wms_client_message_s_type     cMsg;
#else 
   uasms_error_class_e_type      nErrorClass;
   uasms_status_e_type           nStatus;
   uasms_client_message_type     cMsg;
#endif

} GSM1x_MOSMSCallBack;


typedef struct {

   q_link_type                  link;
   GSM1x_MOSMSCallBack          cEntry;

} GSM1x_MOSMS_Entry;


typedef struct
{

   boolean                      m_bInitialized;

   AEECallback                  m_cbTimer;

#ifndef FEATURE_GSM1x_WMS
void (*uasms_MessageListener) (uasms_transaction_id_type          transaction_id,
                               const uasms_client_message_type *  pm) ;

void (*uasms_StatusListener) (const void *              user_data, 
                              uasms_error_class_e_type  error_class,
                              uasms_status_e_type       status) ;

void (*uasms_EventListener) (uasms_event_e_type      event,
                             uasms_event_info_type * info_ptr);

   uasms_so_e_type               m_smsSOConfig;
#else

   wms_dc_so_e_type              m_smsSOConfig;
   wms_client_id_type            m_clientId;
   wms_cmd_cb_type               m_cmdCB;
#endif

   boolean                       m_sendMethodAccess;
   boolean                       m_sendMethodTraffic;

   // MO SMS support
   // The MOSMS Entry's of the following array should be in
   //    m_cMOSMS_FreeQ queue   or
   //    m_cMOSMS_ActiveQ queue or
   //    m_pMOSMS_Active pointer
   // An entry will be present in only one of the above.
   GSM1x_MOSMS_Entry             m_aMOSMS_Array[MAX_MOSMS_OUTSTANDING];

   // Queue of free MO SMS entries
   // All MOSMS_CallBack in this queue should be in GSM1x_MOSMS_STATE_FREE
   q_type                        m_cMOSMS_FreeQ;

   // Queue of oustanding MO SMS entries
   // All MOSMS_CallBack in this queue should be in GSM1x_MOSMS_STATE_QUEUED
   q_type                        m_cMOSMS_ActiveQ;

   // MO SMS entry currently being sent
   // MOSMS_CallBack referred by this pointer can be in any GSM1x_MOSMS_State
   // except GSM1x_MOSMS_STATE_FREE or GSM1x_MOSMS_STATE_QUEUED
   GSM1x_MOSMS_Entry *           m_pMOSMS_Active;

   GSM1xMessage_DTC_State        m_eState;

   // When a client initiates a MO SMS, it can choose to start a 
   // transaction. Starting a transaction increments the following 
   // counter. Client will indicate the end of transaction by calling
   // OEMGSM1xMessage_SVC_ReleaseSMSTransaction.
   // If the m_OutstandingTransactionCounter is non-zero, OEMGSM1xMessage will
   // not explicitly release a DTC connection. 
   // Note that non-zero counter value doesn't imply that OEMGSM1xMessage will set 
   // up a DTC; DTC may or may not be setup based on other criteria viz. 
   // message is too big to fit in access channel or OEM vendor chooses 
   // a policy of using DTC. 
   // Also note that DTC connection may be release by uasms when the 
   // idle timer expires i.e. there is no transmission on DTC for 
   // SO6_TIMEOUT_LENGTH seconds. 
   //
   int                           m_OutstandingTransactionCounter;

   // OEMPhone instance
   OEMPhone*                     m_pPhone;


} IGSM1xMessage;


/*===========================================================================

                    FUNCTION PROTOTYPES

===========================================================================*/


// MOSMS utility functions
#ifdef FEATURE_GSM1x_WMS
static void    GSM1x_MOSMS_CmdCBFn (wms_cmd_id_e_type cmd, void* user_data, wms_cmd_err_e_type cmd_err );
#endif

// These functions are used with GSM1x_MOSMSCallBack structure
static void GSM1x_MOSMSCallBack_Free(void * userData);
static void GSM1x_MOSMSCallBack_TimeoutHandler(void *userData);

static int  GSM1x_MOSMSCallBack_Enqueue(PFNSMSSTATUS pfn,
                                        void *pUser,
                                        const SMS_MESSAGE_TYPE *pMsg);

static void GSM1x_MOSMSCallBack_ReportStatus(
         GSM1x_MOSMSCallBack       *pe,
#ifdef FEATURE_GSM1x_WMS
         wms_error_class_e_type     errorClass,
         wms_status_e_type          status
#else
         uasms_error_class_e_type   errorClass,
         uasms_status_e_type        status
#endif
         );


static int OEMGSM1xMessage_SVC_sendMessageInternal( PFNSMSSTATUS pfn,
                                                    void *pUser,
                                                    const SMS_MESSAGE_TYPE *pMsg,
                                                    boolean startTransaction);

static void OEMGSM1xMessage_SVC_handleNextCall(void);
static void OEMGSM1xMessage_SVC_sendSMS(GSM1xMessage_SMSSendMethod method);


/*===========================================================================

                      METHOD DECLARATIONS

===========================================================================*/

#ifdef FEATURE_GSM1x_WMS

static void
    OEMGSM1xMessage_SVC_MessageListener(wms_msg_event_e_type event, 
                                        wms_msg_event_info_s_type * info_ptr,
                                        boolean *shared);
static void 
    OEMGSM1xMessage_SVC_EventListener(wms_dc_event_e_type  event,
                                      wms_dc_event_info_s_type *info_ptr);

#else
static void 
    OEMGSM1xMessage_SVC_MessageListener(uasms_transaction_id_type transaction_id,
                                        const uasms_client_message_type * pm) ;
static void 
    OEMGSM1xMessage_SVC_StatusListener(const void * user_data, 
                                       uasms_error_class_e_type error_class,
                                       uasms_status_e_type status) ;
static void     
    OEMGSM1xMessage_SVC_EventListener(uasms_event_e_type event,
                                      uasms_event_info_type * info_ptr) ;
#endif


/*===========================================================================

                     LOCAL/STATIC DATA

===========================================================================*/

static IGSM1xMessage sGSM1xMessage = {  /*lint -save -e785 */
                                        FALSE                            // m_bInitialized
                                     };
                                     /*lint -restore */
                                     // Rest of the fields are initialized later.

static OEMCriticalSection gGSM1xMessageCriticalSection;


// static data that is used for translating GSM1x_MOSMS_Entry* into handles.
// each handle is a value between 1 and MAX_MOSMS_OUTSTANDING.
static AEEInstance gpInstances[MAX_MOSMS_OUTSTANDING];

static AEEInstanceList gGSM1xMessageSMSInstanceList 
            = { gpInstances, sizeof(gpInstances)/sizeof(gpInstances[0]), 0 };

static OEMCriticalSection gGSM1xMessageMSInstanceListCriticalSection;

/*===========================================================================

                     GSM1x_MOSMSCallBack routines

===========================================================================*/


/*=============================================================================
Function: GSM1x_MOSMSCallBack_Enqueue

Description:
  Initialize the MOSMS Callback structure. This structure stores all the
  information needed for a MO SMS and then adds the GSM1x_MOSMS_CallBack structure
  to the outstanding MO SMS queue.

Parameters:
   pfn:       Pointer to callback function to report SMS status.
   pUser:     Pointer to user data send with "pfn" callback invocation.
   pMsg:      Pointer to the SMS to be send out.

Return Value:
   EBADCLASS:  OEMGSM1xMessage not initialized.
   EITEMBUSY:  No more buffers left for oustanding MO SMS.
   SUCCESS:    MO SMS was queued up.

Comments:
   None

Side Effects:
   None

Also See:
  GSM1x_MOSMSCallBack_Free
===========================================================================*/
static int GSM1x_MOSMSCallBack_Enqueue(PFNSMSSTATUS pfn,
                                       void *pUser,
                                       const SMS_MESSAGE_TYPE *pMsg)
{
   GSM1x_MOSMSCallBack *pe;
   GSM1x_MOSMS_Entry *pTempEntry;

   // Obtain a entry to store MO SMS.
   //
   pTempEntry = (GSM1x_MOSMS_Entry *)q_get(&(sGSM1xMessage.m_cMOSMS_FreeQ));
   if (pTempEntry == NULL)
   {
      return(EITEMBUSY);
   }
   pe = &(pTempEntry->cEntry);

   // assert (pe->eState == GSM1x_MOSMS_STATE_FREE)

   // Initialize MO SMS CallBack struct

   MEMSET(pe,0,sizeof(GSM1x_MOSMSCallBack));

   pe->pCtx    = AEE_GetAppContext();
   pe->pfn     = pfn;
   pe->pUser   = pUser;

   if(pe->pCtx)
   {
      // We were invoked from a BREW app. We will register for APP CLOSE
      // This allows us to invalidate the callback function, if the app disappears.
      //
      uint32 handle;
      boolean success;

      // get a new unique handle, a single task at a time ...
      OEMCriticalSection_Enter(&gGSM1xMessageMSInstanceListCriticalSection);
      success = AEEHandle_To(&gGSM1xMessageSMSInstanceList, pTempEntry, &handle);
      OEMCriticalSection_Leave(&gGSM1xMessageMSInstanceListCriticalSection);

      if (success)
      {
          OEMGSM1xMessage_ScheduleAppClosedCallback(handle);
      }
      else
      {
          DBGPRINTF("GSM1xM:Create Handle for %d failed!", pTempEntry);
      }
   }
   else
   {
      // In older implementation we could be called from a non BREW module e.g. 
      // AEEGSM1xSig, but now all calls should be from UI context. 
      // TODO investigate with MSG_ERROR
      //
      MSG_ERROR("SendSMS called with void context",0,0,0);
   }

   CALLBACK_Init(&pe->cb, GSM1x_MOSMSCallBack_Free, pTempEntry); // init status report callback

   MEMCPY(&(pe->cMsg), pMsg, sizeof(*pMsg));
   pe->eState = GSM1x_MOSMS_STATE_QUEUED;

   // Put the message into active queue
   q_put(&(sGSM1xMessage.m_cMOSMS_ActiveQ),&(pTempEntry->link));

   DBGPRINTF("GSM1xM:Queued.Link(%x)(%x)", pTempEntry, pe);
   return(SUCCESS);
}

/*=============================================================================
Function: GSM1x_MOSMSCallBack_ReportStatus

Description:
  Helper function to report the status back to user callback function.

Parameters:
   pe:         Pointer to GSM1x_MOSMSCallBack structure.
   errorClass: Enum value to report wms/uasms error class
   status:     Enum value to report wms/uasms status.

Return Value:
   None.

Comments:
   None

Side Effects:
   None

Also See:
  GSM1x_MOSMSCallBack_Free
===========================================================================*/
void GSM1x_MOSMSCallBack_ReportStatus(
         GSM1x_MOSMSCallBack       *pe,
#ifdef FEATURE_GSM1x_WMS
         wms_error_class_e_type     errorClass,
         wms_status_e_type          status
#else
         uasms_error_class_e_type   errorClass,
         uasms_status_e_type        status
#endif
         )
{
   pe->nErrorClass = errorClass;
   pe->nStatus     = status;
   pe->eState      = GSM1x_MOSMS_STATE_COMPLETED;

   DBGPRINTF("GSM1xM:Reporting status for(%x)",pe);

   // Do the cleanup, reporting etc and put the entry back into free Q
   //

   //only schedule callback if it will actually do something ...
   if (pe->cb.pfnNotify)
   {
       AEE_SYS_RESUME(&pe->cb);
   }
}


/*=============================================================================
Function: GSM1x_MOSMSCallBack_TimeoutHandler

Description:
  This routine is invoked when a MOSMS transaction has been pending for more 
  than MAX_MOSMS_TIMER milli-seconds. This ensures that even if uasms 
  doesn't report the status for some reason, transaction will be cleared up.

Parameters:
   pUserData:      Pointer to the GSM1x_MOSMSCallBack entry.

Return Value:
   None.

Comments:  
   None

Side Effects: 
   None

Also See:
  GSM1x_MOSMSCallBack_Free
===========================================================================*/
static void GSM1x_MOSMSCallBack_TimeoutHandler(void *pUserData) 
{
   GSM1x_MOSMS_Entry *pEntry = (GSM1x_MOSMS_Entry *)pUserData;
   GSM1x_MOSMSCallBack *pe;

   pe = &(pEntry->cEntry);
   DBGPRINTF("GSM1xM:Timeout pEn(%x)pe(%x)",pEntry,pe);
   MSG_ERROR("Timeout pEn(%x)pe(%x)",pEntry,pe,0);

   if (pEntry != sGSM1xMessage.m_pMOSMS_Active ||
       pEntry == 0)
   {
      DBGPRINTF("GSM1xM:Timeout MOSMS not active");
      return;
   }

#ifdef FEATURE_GSM1x_WMS
   pe->nErrorClass = WMS_ERROR_RESERVED_1;
   pe->nStatus     = WMS_PHONE_NOT_READY_S;
#else
   pe->nErrorClass = UASMS_ERROR_RESERVED_1;
   pe->nStatus     = UASMS_OTHER_GENERAL_PROBLEMS_S;
#endif

   pe->eState      = GSM1x_MOSMS_STATE_COMPLETED;

   GSM1x_MOSMSCallBack_Free(pUserData);
}


static int16 GSM1x_XlateSMSError(
#ifdef FEATURE_GSM1x_WMS
                    wms_error_class_e_type error_class,
                    wms_status_e_type status)
#else
                    uasms_error_class_e_type error_class,
                    uasms_status_e_type status)
#endif
{
   PARAM_NOT_REF(error_class)

   DBGPRINTF("GSM1xM:err=%d", status);

#ifdef FEATURE_GSM1x_WMS
   if(status == WMS_OK_S)
#else
   if(status == UASMS_OK_S)
#endif
   {
      return SUCCESS;
   }
   else
   {
      return((int16)status);
   }
}


/*=============================================================================
Function: GSM1x_MOSMSCallBack_FreeAppFunc

Description:
  This routine is called when the app that registered for callback has closed down.
  So all it needs to do is to set the user notify function to NULL. 
  The MOSMS object will be freedwhen the actual status is received from the
  underlying SMS layer.

Parameters:
   userData: Pointer to the GSM1x_MOSMSCallBack entry.

Return Value:
   None.

Comments:
   None

Side Effects:
   None

===========================================================================*/
static void GSM1x_MOSMSCallBack_FreeAppFunc(void* userData)
{
   GSM1x_MOSMS_Entry *pSmsEntry = (GSM1x_MOSMS_Entry *)userData;

   if (pSmsEntry != NULL)
   {
      DBGPRINTF("GSM1xM:Setting app callback to NULL");

      pSmsEntry->cEntry.pfn = NULL;
   }
   
   return;
}


/*=============================================================================
Function: GSM1x_MOSMSCallBack_Free

Description:
  This routine can be called under two situations:
  * The app that registered for callback has closed down. In this case we update
    the MOSMS CallBack structure. We do not free up the GSM1x_MOSMSCallBack entry at this
    point; we wait until UASMS reports the status.
  * It is time to clean up the MOSMS; this is indicated by setting the state of
    GSM1x_MOSMSCallBack to GSM1x_MOSMS_STATE_COMPLETED. In this case we invoke the callback
    on MOSMS, if required, and free up the structure.

Parameters:
   userData:      Pointer to the GSM1x_MOSMSCallBack entry.

Return Value:
   None.

Comments:
   None

Side Effects:
   None

Also See:
  GSM1x_MOSMSCallBack_Enqueue
===========================================================================*/
static void GSM1x_MOSMSCallBack_Free(void * userData)
{
   GSM1x_MOSMS_Entry *pEntry = (GSM1x_MOSMS_Entry *)userData;
   GSM1x_MOSMSCallBack *pe;

   boolean              success;
   uint32               handle;

   if ((!sGSM1xMessage.m_bInitialized) || (pEntry == NULL))
   {
      return;
   }

   pe = &(pEntry->cEntry);

   if (pe->eState == GSM1x_MOSMS_STATE_FREE) 
   {
      DBGPRINTF("GSM1xM:Freeing already free MOSMS");
      return;
   }

   DBGPRINTF("GSM1xM:CB pe(%x)Err(%d)St(%d)", pe,pe->nErrorClass,pe->nStatus);

   CALLBACK_Cancel(&sGSM1xMessage.m_cbTimer);

   //must cancel the wms/uasms status callback, as it may already be scheduled!
   CALLBACK_Cancel(&pe->cb);

   if(pe->pfn) 
   {
      // Call the callback function with user data and status. 
      // GSM1x_XlateSMSError combines error class and status fields into 
      // a single status
      //
      DBGPRINTF("GSM1xM:Invoking user callback");

      pe->pfn(pe->pUser, GSM1x_XlateSMSError(pe->nErrorClass,pe->nStatus));
      pe->pfn = NULL;
   }

   MEMSET(pe,0,sizeof(GSM1x_MOSMSCallBack));
   pe->eState = GSM1x_MOSMS_STATE_FREE;

   // Put the free MO SMS Entry back into Free Q
   q_put(&(sGSM1xMessage.m_cMOSMS_FreeQ),&(pEntry->link));

   OEMCriticalSection_Enter(&gGSM1xMessageMSInstanceListCriticalSection);
   success = AEEHandle_Lookup(&gGSM1xMessageSMSInstanceList, pEntry, &handle);
   OEMCriticalSection_Leave(&gGSM1xMessageMSInstanceListCriticalSection);

   if (success)
   {
      OEMGSM1xMessage_CancelAppClosedCallback(handle);
   }
   else
   {
      DBGPRINTF("GSM1xM:Handle Lookup for %d failed!", pEntry);
   }

   // We are done processing the current MO SMS. Let's check if we have
   // anymore to process
   //
   DBGPRINTF("GSM1xM:Freed resources for (%x)",sGSM1xMessage.m_pMOSMS_Active);

   sGSM1xMessage.m_pMOSMS_Active = NULL;
   OEMGSM1xMessage_SVC_handleNextCall();
}


#ifdef FEATURE_GSM1x_WMS
// This routine is used only in WMS 
/*=============================================================================
FUNCTION:  GSM1x_MOSMS_CmdCBFn

DESCRIPTION: Callback function for comman. All WMS_CMD_* messages

PARAMETERS:
   None

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void GSM1x_MOSMS_CmdCBFn (wms_cmd_id_e_type  cmd,
                                 void              *user_data,
                                 wms_cmd_err_e_type cmd_err )
{
   PARAM_NOT_REF(cmd)


   GSM1x_MOSMS_Entry *pEntry = (GSM1x_MOSMS_Entry *)user_data;
   GSM1x_MOSMSCallBack *pe;
  
   if (!sGSM1xMessage.m_bInitialized)
   {
       return;
   }

   if ((sGSM1xMessage.m_pMOSMS_Active == NULL) ||
       (sGSM1xMessage.m_pMOSMS_Active != pEntry))
   {
      // Not related to active MOSMS
      if (cmd_err != WMS_CMD_ERR_NONE) 
      {
         DBGPRINTF("GSM1xM:CMDCB: Cmd %d, Err code %d", cmd, cmd_err);
      }
      return;
   }

   pe = &(pEntry->cEntry);

   if (cmd_err != WMS_CMD_ERR_NONE) 
   {
      GSM1x_MOSMSCallBack_ReportStatus( pe,
                                        WMS_ERROR_RESERVED_1,
                                        WMS_PHONE_NOT_READY_S );
   }
}

#endif          //#ifdef FEATURE_GSM1x_WMS



/*===========================================================================

===========================================================================*/
#if !defined(FEATURE_GSM1x_WMS)

void OEMGSM1xMessage_SVC_ConfigListeners(
       uasms_message_listener_type    message_listener_f,
       uasms_status_listener_type     status_listener_f,
       uasms_event_listener_type      event_listener_f)
{
   if (! sGSM1xMessage.m_bInitialized)
   {
      return;
   }
   sGSM1xMessage.uasms_MessageListener = message_listener_f;
   sGSM1xMessage.uasms_StatusListener  = status_listener_f;
   sGSM1xMessage.uasms_EventListener   = event_listener_f;
}

#endif  //if !defined(FEATURE_GSM1x_WMS)


/*=============================================================================
Function: OEMGSM1xMessage_SVC_Init()
===========================================================================*/
void OEMGSM1xMessage_SVC_Init() 
{
   byte     pref_sms_so;
   int      i;

   sGSM1xMessage.m_eState        = GSM1xMESSAGE_STATE_NO_DTC_SETUP;
   sGSM1xMessage.m_pMOSMS_Active = NULL;
   sGSM1xMessage.m_OutstandingTransactionCounter = 0;
  
#ifdef FEATURE_GSM1x_WMS

   // Register to become a WMS client
   //
   //initialise clientID and commnand callback
   //
   sGSM1xMessage.m_clientId = WMS_CLIENT_TYPE_GSM1x;
   sGSM1xMessage.m_cmdCB    = GSM1x_MOSMS_CmdCBFn;

   if (WMS_CLIENT_ERR_NONE != (wms_client_init (WMS_CLIENT_TYPE_GSM1x,
                                                &(sGSM1xMessage.m_clientId))))
   {
      MSG_ERROR("Unable to register GSM1x client",0,0,0);
      return;
   }
   if ((wms_client_activate (sGSM1xMessage.m_clientId)) != WMS_CLIENT_ERR_NONE )
   {
      MSG_ERROR("Unable to activate wms client",0,0,0);
      return;
   }

   if (WMS_CLIENT_ERR_NONE != (wms_client_reg_msg_cb(sGSM1xMessage.m_clientId, 
                                                     OEMGSM1xMessage_SVC_MessageListener))) 
   {
      MSG_ERROR("Unable to register message listener",0,0,0);
      return;
   }

   if ( WMS_CLIENT_ERR_NONE != (wms_client_reg_dc_cb(sGSM1xMessage.m_clientId, 
                                                     OEMGSM1xMessage_SVC_EventListener))) 
   {
      MSG_ERROR("Unable to register event listener",0,0,0);
      return;
   }
  
   //Turn on the auto-disconnect feature of the DTC call.
   (void) wms_dc_enable_auto_disconnect(sGSM1xMessage.m_clientId, 
                                        sGSM1xMessage.m_cmdCB,
                                        NULL, 
                                        SO6_TIMEOUT_LENGTH);

#else   // #ifdef FEATURE_GSM1x_WMS
   /*  We should use OEMSMS routines instead of uasms directly.
   OEM_uasms_config_listeners(OEMGSM1xMessage_SVC_MessageListener,
                              OEMGSM1xMessage_SVC_StatusListener,
                              OEMGSM1xMessage_SVC_EventListener);
   */
   if (UASMS_OK_S != uasms_config_listeners(OEMGSM1xMessage_SVC_MessageListener,
                                            OEMGSM1xMessage_SVC_StatusListener,
                                            OEMGSM1xMessage_SVC_EventListener))
   {
      MSG_ERROR("Unable to register event listener",0,0,0);
      return;
   }
   sGSM1xMessage.uasms_EventListener   = 0;
   sGSM1xMessage.uasms_MessageListener = 0;
   sGSM1xMessage.uasms_StatusListener  = 0;

   //Turn on the auto-disconnect feature of the DTC call.
   (void) uasms_DTC_enable_auto_disconnect(SO6_TIMEOUT_LENGTH);

#endif  // #ifdef FEATURE_GSM1x_WMS


   if( SUCCESS != OEM_GetConfig(CFGI_SMS_MO_ON_ACCESS_CHANNEL, 
				&(sGSM1xMessage.m_sendMethodAccess),
				sizeof(sGSM1xMessage.m_sendMethodAccess)))
     {
       sGSM1xMessage.m_sendMethodAccess = TRUE;
     }

   if( SUCCESS != OEM_GetConfig(CFGI_SMS_MO_ON_TRAFFIC_CHANNEL, 
				&(sGSM1xMessage.m_sendMethodTraffic),
				sizeof(sGSM1xMessage.m_sendMethodTraffic)))
     {
       sGSM1xMessage.m_sendMethodTraffic = FALSE;
     }


   // sendMethod will be either set to OEMNV_SMS_MO_ACCESS_AND_TRAFFIC, or
   // OEMNV_SMS_MO_ACCESS_ONLY or OEMNV_SMS_MO_TRAFFIC_ONLY. If it is traffic
   // only then we don't make any attempt to send the given message on the
   // access channel. In all other cases, we send it on the access channel.
   // (If the sendMethod is set to OEMNV_SMS_MO_ACCESS_ONLY then we don't make
   // any subsequent attempts to send message on the traffic channel. This
   // logic is implemented in the OEMGSM1xMessage_SVC_localStatusListener() function)


   if (EUNSUPPORTED == OEM_GetConfig(CFGI_SMS_SERVICE_OPTION, 
                                     &(pref_sms_so),
                                     sizeof(pref_sms_so)))
   {
#ifdef FEATURE_GSM1x_WMS
      sGSM1xMessage.m_smsSOConfig = WMS_DC_SO_6;
#else
      sGSM1xMessage.m_smsSOConfig = UASMS_SO_NONE;
#endif
   }
   else
   {
       // Get the service option (SO6 or SO14) from the configuration database
       // and map it to the WMS service option.
       switch(pref_sms_so)
       {

#ifdef FEATURE_GSM1x_WMS

          case OEMNV_SMS_SO_NONE:
             sGSM1xMessage.m_smsSOConfig = WMS_DC_SO_AUTO;
             break;
          case OEMNV_SMS_SO_SO6:
             sGSM1xMessage.m_smsSOConfig = WMS_DC_SO_6;
             break;
          case OEMNV_SMS_SO_SO14:
             sGSM1xMessage.m_smsSOConfig = WMS_DC_SO_14;
             break;
          default:
             // Invalid default value? Set the service option to WMS_SO_6.
             sGSM1xMessage.m_smsSOConfig = WMS_DC_SO_6;
             break;

#else           //#ifdef FEATURE_GSM1x_WMS

          case OEMNV_SMS_SO_NONE:
             sGSM1xMessage.m_smsSOConfig = UASMS_SO_NONE;
             break;
          case OEMNV_SMS_SO_SO6:
             sGSM1xMessage.m_smsSOConfig = UASMS_SO_6;
             break;
          case OEMNV_SMS_SO_SO14:
             sGSM1xMessage.m_smsSOConfig = UASMS_SO_14;
             break;
          default:
             // Invalid default value? Set the service option to WMS_SO_6.
             sGSM1xMessage.m_smsSOConfig = UASMS_SO_6;
             break;

#endif          //#ifdef FEATURE_GSM1x_WMS

       }
   }
       
   // Init the MO SMS queue
   //
   (void) q_init(&( sGSM1xMessage.m_cMOSMS_FreeQ));
   (void) q_init(&( sGSM1xMessage.m_cMOSMS_ActiveQ));

   for (i = 0; i < MAX_MOSMS_OUTSTANDING; i++) {
      (void) q_link(&(sGSM1xMessage.m_aMOSMS_Array[i]), &(sGSM1xMessage.m_aMOSMS_Array[i].link));
      q_put(&(sGSM1xMessage.m_cMOSMS_FreeQ), &(sGSM1xMessage.m_aMOSMS_Array[i].link));
   }

   CALLBACK_Init(&sGSM1xMessage.m_cbTimer, NULL, NULL);

   if (OEMPhone_New(&sGSM1xMessage.m_pPhone) != SUCCESS)
   {
       MSG_ERROR("Unable to create Ihpone instance",0,0,0);
       return;
   }

   // This is how we indicate that we have successfully initialized.
   sGSM1xMessage.m_bInitialized = TRUE;
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_AppClosedNotification()

Description:
   handle a notification that an applet that has sent an SMS has closed.

Parameters:
   smsEntryHandle - handle that corresponds to an SMS entry.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

===========================================================================*/
void OEMGSM1xMessage_SVC_AppClosedNotification(uint32 smsEntryHandle)
{
   GSM1x_MOSMS_Entry *pEntry = NULL;
   boolean            success;

   OEMCriticalSection_Enter(&gGSM1xMessageMSInstanceListCriticalSection);
   success = AEEHandle_From(&gGSM1xMessageSMSInstanceList, smsEntryHandle, (OEMINSTANCE*)(&pEntry));
   OEMCriticalSection_Leave(&gGSM1xMessageMSInstanceListCriticalSection);

   if (success) 
   {
      // call the function that actually handles the app closed notification
      GSM1x_MOSMSCallBack_FreeAppFunc(pEntry);

      OEMCriticalSection_Enter(&gGSM1xMessageMSInstanceListCriticalSection);
      (void)AEEHandle_Clear(&gGSM1xMessageSMSInstanceList, smsEntryHandle);
      OEMCriticalSection_Leave(&gGSM1xMessageMSInstanceListCriticalSection);
   }
   else
   {        
      DBGPRINTF("GSM1xM:Bad Handle %d received!", smsEntryHandle);
   }
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_RerouteTapi()

Description:
  Just a wrapper function around OEMGSMSMS_RerouteTapi. We don't want ITAPI
  or OEMGSM1xMessage calling IGSMSMS directly.

Parameters:
   None.
   pszDst     - Destination address.
   pszMsg     - Message to be sent out.
   clsDst     - Class destination. Set to non-zero value only for BREW 
                directed messages.
   pdwStatus  - Place to be filled with the async operation's status.
   pcb        - User mode callback to report the async operation status.

Return Value:
   EUNSUPPORTED  - GSM1x mode not activated
   SUCCESS       - Did the rerouting succesfully
   anything else - general failure

Comments:  
   None

Side Effects: 
   None

Also See:
  OEMGSMSMS_RerouteTAPI()
===========================================================================*/
int OEMGSM1xMessage_SVC_RerouteTapi (const char * pszDst,
                                     const char * pszMsg,
                                     AEECLSID     clsDst,
                                     uint32     * pdwStatus,
                                     AEECallback* pcb)
{
   if (OEMGSM1xControl_SVC_GetStatus() == GSM1xSIG_INACTIVE) 
   {
      return EUNSUPPORTED;
   }
  
   return GSMSMS_ScheduleRerouteTapi(pszDst, pszMsg, clsDst, pdwStatus, pcb);
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_StartSMSTransaction()

Description:
  See description for OEMGSM1xMessage_SVC_SendMessage for details.

Parameters:
   None.

Return Value:
   None.

Comments:  
   None

Side Effects: 
   None

Also See:
  OEMGSM1xMessage_SVC_SendMessage()
===========================================================================*/
static void OEMGSM1xMessage_SVC_StartSMSTransaction()
{
   sGSM1xMessage.m_OutstandingTransactionCounter++;

   DBGPRINTF("GSM1xM:StartSMSTransaction.Counter(%d)",
               sGSM1xMessage.m_OutstandingTransactionCounter);
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_ReleaseSMSTransaction()

Description:
  See description for OEMGSM1xMessage_SVC_SendMessage for details.

Parameters:
   None.

Return Value:
   None.

Comments:  
   None

Side Effects: 
   None

Also See:
  OEMGSM1xMessage_SVC_SendMessage()
===========================================================================*/
void OEMGSM1xMessage_SVC_ReleaseSMSTransaction()
{
   /*
   if (!sGSM1xMessage.m_bInitialized)
   {
      return(EBADCLASS);
   }
   */

   if (sGSM1xMessage.m_OutstandingTransactionCounter > 0) 
   {
      sGSM1xMessage.m_OutstandingTransactionCounter--;
   }

   DBGPRINTF("GSM1xM:ReleaseSMSTrans.Counter(%d)", 
               sGSM1xMessage.m_OutstandingTransactionCounter);

   if (sGSM1xMessage.m_OutstandingTransactionCounter != 0) 
   {
      return;
   }


   // The counter is zero. Check to see if the DTC connection 
   // is being kept open just because of pending transaction.
   //
   if (!sGSM1xMessage.m_pMOSMS_Active)
   {
      if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTED) 
      {
         DBGPRINTF("GSM1xM:Releasing DTC connection");
         sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP;

#ifdef FEATURE_GSM1x_WMS
         (void) wms_dc_disconnect(sGSM1xMessage.m_clientId, sGSM1xMessage.m_cmdCB, NULL);
#else
         (void) uasms_DTC_disconnect();
#endif
      }
   }
}


#ifdef FEATURE_GSM1x_WMS
/*=============================================================================
FUNCTION: OEMGSM1xMessage_SVC_wms_sendSignalingSMS                   INTERNAL FUNCTION

DESCRIPTION:
     Used by OEMGSM1xMessage_SVC_SendMessage for WMS. 
        
PARAMETERS:
    * pSigMsg   [in] : signaling message to be send as SMS

RETURN VALUE:
  EBADCLASS       - OEMGSM1xMessage not initialized.
  EITEMBUSY       - No more buffers left for oustanding MO SMS.
  SUCCESS         - MO SMS was queued up.

SIDE EFFECTS:
    None
=============================================================================*/
static int OEMGSM1xMessage_SVC_wms_sendSignalingSMS( PFNSMSSTATUS  pfn,
                                                     void         *pUser,
                                                     OEMGSM1xMessage_MsgType type,
                                                     const byte    bearerDataLen,
                                                     const byte   *bearerData,
                                                     boolean       startTransaction)
{
    byte * data;
    word bit_pos;

    wms_client_message_s_type pMsg;
    wms_cdma_message_s_type *pCDMA_msg;
    wms_raw_ts_data_s_type *pRaw;
    pCDMA_msg = &(pMsg.u.cdma_message);

    pMsg.msg_hdr.tag          = WMS_TAG_MO_NOT_SENT;
    pMsg.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
    pMsg.msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
    pMsg.msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;

    pCDMA_msg->is_mo               = TRUE;
    pCDMA_msg->is_tl_ack_requested = FALSE;
    pCDMA_msg->service             = WMS_SRV_UNKOWN;

    if (type == GSM1xMESSAGE_MSG_SIG) 
    {
       pCDMA_msg->teleservice         = WMS_TELESERVICE_GSM1x_01;
    }
    else if (type == GSM1xMESSAGE_MSG_SMS) 
    {
       pCDMA_msg->teleservice         = WMS_TELESERVICE_GSM1x_02;
    }
    else
    {
       return EFAILED;
    }

    pCDMA_msg->is_service_present  = FALSE;

    // pMsg.is_broadcast = FALSE;  is_broadcast removed in WMS

    // Fill in a dummy address
    pCDMA_msg->address.digit_mode       = WMS_DIGIT_MODE_8_BIT;
    pCDMA_msg->address.number_mode      = WMS_NUMBER_MODE_DATA_NETWORK;
    pCDMA_msg->address.number_type      = WMS_NUMBER_UNKNOWN;
    pCDMA_msg->address.number_plan      = WMS_NUMBER_PLAN_UNKNOWN;
    pCDMA_msg->address.number_of_digits = 1;
    pCDMA_msg->address.digits[0]        = 0;

    // We don't have a sub-address
    pCDMA_msg->subaddress.number_of_digits = 0;


    // Create the bearer data
    pRaw         = &(pCDMA_msg->raw_ts); 
    pRaw->format = WMS_FORMAT_CDMA;

    data    = pRaw->data;

    // Add the message id sub-param
    data[0] = WMS_BD_MSG_ID; 
    data[1] = 3;                        // length of BD_MSG_ID param
    data[2] = WMS_BD_TYPE_SUBMIT <<4;
    data[3] = 0;                        // dummy message id number
    data[4] = 0;                        // Set udh_present to false

    // Add user data sub-param
    data[5] = WMS_BD_USER_DATA; 
    data[6] = bearerDataLen + 3; 

    bit_pos=7*8;
    b_packb(WMS_ENCODING_OCTET,data,bit_pos,5);  // Pack the data encoding
    bit_pos+=5;
    b_packb(bearerDataLen+1,data,bit_pos,8);
    bit_pos+=8;
    b_packb(0,data,bit_pos,3);       // Adding padding bits
    bit_pos+=3;

    MEMCPY(&data[9],bearerData,bearerDataLen);
    data[9+bearerDataLen]=0;         // Penalty for byte alignment.

    pRaw->len = 5 +                // Msg id sub-param
                2 +                // user data sub-param hdr 
                2 +                // encoding+length+3 pading bits
                bearerDataLen +    // our payload
                1;                 // penalty for byte alignment

    return (OEMGSM1xMessage_SVC_sendMessageInternal(pfn,       // Pointer to callback function
                                                    pUser,     // User data for callback function
                                                    &pMsg,
                                                    startTransaction)); 

}
#else   // ifdef FEATURE_GSM1x_WMS

/*=============================================================================
FUNCTION: OEMGSM1xMessage_SVC_uasms_sendSignalingSMS                 INTERNAL FUNCTION

DESCRIPTION:
     Used by OEMGSM1xMessage_SVC_SendMessage for UASMS. 
        
PARAMETERS:
    * pSigMsg   [in] : signaling message to be send as SMS

RETURN VALUE:
  EBADCLASS       - OEMGSM1xMessage not initialized.
  EITEMBUSY       - No more buffers left for oustanding MO SMS.
  SUCCESS         - MO SMS was queued up.

SIDE EFFECTS:
    None
=============================================================================*/
static int OEMGSM1xMessage_SVC_uasms_sendSignalingSMS( PFNSMSSTATUS  pfn,
                                                       void         *pUser,
                                                       OEMGSM1xMessage_MsgType  type,
                                                       const byte    bearerDataLen,
                                                       const byte   *bearerData,
                                                       boolean       startTransaction)
{
    uasms_client_message_type pMsg;
    uasms_raw_bd_type *pRaw;
    byte * data;
    word bit_pos;

    pMsg.tag = UASMS_MO_NOT_SENT;
    pMsg.is_broadcast = FALSE;
    pMsg.is_tl_ack_requested = FALSE;
    pMsg.service = UASMS_SRV_UNKOWN;

    if (type == GSM1xMESSAGE_MSG_SIG) 
    {
       pMsg.teleservice = GSM1xMESSAGE_TELESERVICE_SIG;  /*lint !e64*/
                                                         // We are assigning a teleservice value that
                                                         // is not defined in uasms enums.
    }
    else if (type == GSM1xMESSAGE_MSG_SMS) 
    {
       pMsg.teleservice = GSM1xMESSAGE_TELESERVICE_SMS;  /*lint !e64*/
    }
    else
    {
       return EFAILED;
    }

    // Fill in a dummy address
    pMsg.address.digit_mode  = UASMS_DIGIT_MODE_8_BIT; // DTMF digits
    pMsg.address.number_mode = UASMS_NUMBER_MODE_DATA_NETWORK;
    pMsg.address.number_type = UASMS_NUMBER_UNKNOWN;
    pMsg.address.number_plan = UASMS_NUMBER_PLAN_UNKNOWN;
    pMsg.address.number_of_digits = 1;
    pMsg.address.digits[0]=0;

    pMsg.subaddress.number_of_digits = 0;

    // Create the bearer data
    pRaw = &pMsg.raw_bd;
    pRaw->format = UASMS_FORMAT_CDMA;

    data    = pRaw->bd_data;

    // Add the message id sub-param
    data[0] = UASMS_BD_MSG_ID; 
    data[1] = 3;                        // length of BD_MSG_ID param
    data[2] = UASMS_BD_TYPE_SUBMIT <<4;
    data[3] = 0;                        // dummy message id number
    data[4] = 0;                        // Set reserved field to 0

    // Add user data sub-param
    data[5] = UASMS_BD_USER_DATA; 
    data[6] = bearerDataLen + 3; 

    bit_pos=7*8;
    b_packb(UASMS_ENCODING_OCTET,data,bit_pos,5);  // Pack the data encoding
    bit_pos+=5;
    b_packb(bearerDataLen+1,data,bit_pos,8);
    bit_pos+=8;
    b_packb(0,data,bit_pos,3);       // Adding padding bits
    bit_pos+=3;

    MEMCPY(&data[9],bearerData,bearerDataLen);
    data[9+bearerDataLen]=0;         // Penalty for byte alignment.

    pRaw->bd_len = 5 +                // Msg id sub-param
                   2 +                // user data sub-param hdr 
                   2 +                // encoding+length+3 pading bits
                   bearerDataLen +    // our payload
                   1;                 // penalty for byte alignment


    return (OEMGSM1xMessage_SVC_sendMessageInternal(pfn,       // Pointer to callback function
                                                    pUser,     // User data for callback function
                                                    &pMsg,
                                                    startTransaction)); 

}
#endif     // ifdef FEATURE_GSM1x_WMS


/*=============================================================================
Function: OEMGSM1xMessage_SVC_SendMessage()

Description:

  When a client initiates a MO SMS, it can choose to start a transaction; by 
  setting startTransaction parameter to true.
  Starting a transaction increments a counter. Client will indicate the end of 
  transaction by calling OEMGSM1xMessage_SVC_ReleaseMOSMSTransaction. If the 
  m_OutstandingTransactionCounter is non-zero, OEMGSM1xMessage will not explicitly 
  release a DTC connection. 
  Note that non-zero counter value doesn't imply that OEMGSM1xMessage will setup a 
  DTC; DTC may or may not be setup based on other criteria viz. message is too big 
  to fit in access channel or OEM vendor chooses a policy of using DTC. 
  Also note that DTC connection may be release by uasms when the idle timer 
  expires i.e. there is no tranmission on DTC for SO6_TIMEOUT_LENGTH seconds. 

  If this caller choose to start a transaction and the function returns status 
  SUCCESS, client is responsible for calling OEMGSM1xMessage_SVC_ReleaseSMSTransaction. If
  the function returns
  

  Here is how this works:

  1. IGSM1xSig or IGSMSMS calls OEMGSM1xMessage_SVC_SendMessage. The calling routine
     just supplies the GSM1x payload. OEMGMS1xMessage will construct the appropriate 
     wms/uasms struct and populate it with the required fields.
  2. Puts the MOSMS into outstanding queue by calling GSM1x_MOSMSCallBack_Enqueue.
     OEMGSM1xMessage maintains a list of outstanding MO SMSs; if there is no space on the
     queue, ITAPI_sendSMS will return a busy status.
     If the enqueueing is successful, OEMGSM1xMessage_SVC_handleNextCall is called to process the
     outstanding MOSMSs.
  3. Calling OEMGSM1xMessage_SVC_handleNextCall starts off a state machine that will keep
     looping until all oustanding MO SMSs have been handled.
  4. Actual sending of the message is done by the function OEMGSM1xMessage_SVC_SendMessage() that
     is invoked from OEMGSM1xMessage_SVC_handleNextCall.
     In OEMGSM1xMessage_SVC_SendMessage, there are two main means to send SMS message:

       Method 1: Send message over Access Channel. In certain networks, there is
       a very stringent limitation on the size of the message that can be sent
       on Access Channel. In such cases you can try sending on Traffic Channel.
       (Currently, we always try to send any given SMS message on the access
       channel. If this first attempt fails, then we send it on the traffic
       channel)

       Method 2: Send message over Traffic Channel.

  6. Flow of Control in sending message over Traffic Channel:
      a) If a traffic channel is already present, send a SMS message.
      b) If no traffic channel is present, invoke DTC connect routine. 
      c) When the traffic channel is setup, the EventListener callback function
         is invoked. Here, check to see if our intention was indeed to send 
         the SMS message. If yes, send out a SMS message. The user data passed to 
         this function is our MOSMSCB structure. This comes back as a parameter 
         when the msg has been delivered. We use this parameter to
         validate the CB.
      d) When the message is delivered (or failed), the StatusListener/MessageListener 
         CB function is invoked. First, validate the user data to see if its the 
         MOSMSCB structure that we passed it. 
      e) If the user data matches we will clear up the outstanding MOSMS transaction.
         If needed user callbck function is invoked to report the status of the MOSMS
         transaction. 
      f) If there is another oustanding MOSMS, send the message and resume from step "d".
      g) If there is no outstanding MOSMS bring down the Traffic Channel, if we had 
         set it up. 

      e) If message is beint sent on Access Channel, step "a" above is skipped
         and we directly jump to step "b".


Parameters:
   pfn:               Notification function that is invoked to inform the status 
                      of the SMS message sent.
   pUser:             User data to be sent to the notification function.
   type :             Indicates if message is GSM1x Signaling or SMS.
   bearerDataLen:     Length of the GSM1x payload
   bearerData:        GSM1x payload
   startTransaction:  If true, caller want to start a transaction.

Return Value:
   EBADCLASS:  OEMGSM1xMessage not initialized.
   EITEMBUSY:  No more buffers left for oustanding MO SMS.
   SUCCESS:    MO SMS was queued up. If caller has requested start of a transaction,
               it is now required to call OEMGSM1xMessage_SVC_ReleaseMOSMSTransaction in a 
               timely manner.

Comments:
   May be called in the context of several tasks.

Side Effects:
   None

Also See:
  OEMGSM1xMessage_SVC_SendMessage()
  OEMGSM1xMessage_SVC_ReleaseSMSTransaction()
===========================================================================*/
static int OEMGSM1xMessage_SVC_SendMessage( PFNSMSSTATUS pfn,
                                            void *pUser,
                                            OEMGSM1xMessage_MsgType type,
                                            byte  bearerDataLen,
                                            const byte *bearerData,
                                            boolean startTransaction)
{
#ifdef FEATURE_GSM1x_WMS
   return OEMGSM1xMessage_SVC_wms_sendSignalingSMS(pfn,pUser,type,bearerDataLen,bearerData,startTransaction);
#else
   return OEMGSM1xMessage_SVC_uasms_sendSignalingSMS(pfn,pUser,type,bearerDataLen,bearerData,startTransaction);
#endif
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_SendSIGMessage()
===========================================================================*/
int OEMGSM1xMessage_SVC_SendSIGMessage( byte bearerDataLen,
                                        const byte *bearerData,
                                        boolean startTransaction)
{
   byte bearerDataBuffer[255];
   COPY_FROM_USER(bearerDataBuffer, bearerData, MIN(sizeof(bearerDataBuffer), bearerDataLen));

   return OEMGSM1xMessage_SVC_SendMessage( NULL, NULL, GSM1xMESSAGE_MSG_SIG,
                                           bearerDataLen, bearerDataBuffer,
                                           startTransaction);                                               
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_SendSMSMessage()
===========================================================================*/
int OEMGSM1xMessage_SVC_SendSMSMessage( boolean sendStatusCallback,
                                        void * pUserData,
                                        byte bearerDataLen,
                                        const byte *bearerData,
                                        boolean startTransaction)
{
   byte bearerDataBuffer[255];
   PFNSMSSTATUS statusCallback = NULL;
   
   COPY_FROM_USER(bearerDataBuffer, bearerData, MIN(sizeof(bearerDataBuffer), bearerDataLen));

   if (sendStatusCallback == TRUE)
   {
       statusCallback = GSMSMS_StatusCallback; // hard coded callback PFN value for SMS
   }

   return OEMGSM1xMessage_SVC_SendMessage( statusCallback, pUserData,
                                           GSM1xMESSAGE_MSG_SMS, bearerDataLen,
                                           bearerDataBuffer, startTransaction);
}
   

/*=============================================================================
Function: OEMGSM1xMessage_SVC_sendMessageInternal()

Description:

Parameters:
   None.

Return Value:
   None.

Comments:
   None

Side Effects:
   None

Also See:
  OEMGSM1xMessage_SVC_SendMessage()
===========================================================================*/
static int OEMGSM1xMessage_SVC_sendMessageInternal( PFNSMSSTATUS pfn,
                                                    void *pUser,
                                                    const SMS_MESSAGE_TYPE *pMsg,
                                                    boolean startTransaction)
{
   int status;

   if(!sGSM1xMessage.m_bInitialized)
   {
      return(EBADCLASS);
   }

   DBGPRINTF("GSM1xM:SendWMSInternal.Flag(%d)", startTransaction);

   // Add the message into OEMGSM1xMessage outsanding MO SMS queue.

   status = GSM1x_MOSMSCallBack_Enqueue(pfn,pUser,pMsg);
   if (status != SUCCESS)
   {
      return(status);
   }

   if (startTransaction) 
   {
      OEMGSM1xMessage_SVC_StartSMSTransaction();
   }
   OEMGSM1xMessage_SVC_handleNextCall();
   return SUCCESS;
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_handleNextCall()

Description:
  This function kicks off the state machine that will handle the outstanding
  MO SMS messages.
  This function picks up the a message from the oustanding queue and makes it the
  active outstanding message. This function is called again whenever the processing
  for the active MO SMS is completed.

Parameters:
   None.

Return Value:
   None.

Comments:
   None

Side Effects:
   None

Also See:
  OEMGSM1xMessage_SVC_SendMessage()
===========================================================================*/
static void OEMGSM1xMessage_SVC_handleNextCall(void)
{
   if (!sGSM1xMessage.m_bInitialized)
   {
      return;
   }

   OEMCriticalSection_Enter(&gGSM1xMessageCriticalSection);

   // Check if a MO SMS is already being processed
   //
   if (sGSM1xMessage.m_pMOSMS_Active)
   {
      OEMCriticalSection_Leave(&gGSM1xMessageCriticalSection);
      DBGPRINTF("GSM1xM:HndleNxtCl.Alrdy active(%x)", sGSM1xMessage.m_pMOSMS_Active);
      return;
   }

   // If we are here, there is no active MOSMS. Let's look in the activeQ for
   // any outstanding MO SMS
   //
   sGSM1xMessage.m_pMOSMS_Active = (GSM1x_MOSMS_Entry *)q_get(&(sGSM1xMessage.m_cMOSMS_ActiveQ));

   OEMCriticalSection_Leave(&gGSM1xMessageCriticalSection);

   DBGPRINTF("GSM1xM:New activeMOSMS(%x)", sGSM1xMessage.m_pMOSMS_Active);
   // If there are no more MO SMS in activeQ, it is time to close the DTC
   // connection if it is up.
   //
   if (!sGSM1xMessage.m_pMOSMS_Active)
   {
      if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTED)
      {
         // If there are any outstanding transaction we want to keep
         // the DTC open. 
         //
         if (sGSM1xMessage.m_OutstandingTransactionCounter == 0)
         {
            sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP;
#ifdef FEATURE_GSM1x_WMS
            (void) wms_dc_disconnect(sGSM1xMessage.m_clientId, sGSM1xMessage.m_cmdCB, NULL);
#else   //#ifdef FEATURE_GSM1x_WMS
            (void)uasms_DTC_disconnect();
#endif  //#ifdef FEATURE_GSM1x_WMS
         }
      }
      return;
   }

   DBGPRINTF("GSM1xM:Set timer activeMOSMS(%x)", sGSM1xMessage.m_pMOSMS_Active);

   CALLBACK_Init(
       &sGSM1xMessage.m_cbTimer, 
       GSM1x_MOSMSCallBack_TimeoutHandler, 
       (void *)sGSM1xMessage.m_pMOSMS_Active
       );

   if (AEE_SetSysTimerCallback(MAX_MOSMS_TIMER, &sGSM1xMessage.m_cbTimer) != TRUE)
   {
      MSG_ERROR("Unable to set MOSMS timer. Proceeding.",0,0,0);
   } 

   // sendMethod will be either set to OEMNV_SMS_MO_ACCESS_AND_TRAFFIC, or
   // OEMNV_SMS_MO_ACCESS_ONLY or OEMNV_SMS_MO_TRAFFIC_ONLY. If it is traffic
   // only then we don't make any attempt to send the given message on the
   // access channel. In all other cases, we send it on the access channel.
   // (If the sendMethod is set to OEMNV_SMS_MO_ACCESS_ONLY then we don't make
   // any subsequent attempts to send message on the traffic channel. This
   // logic is implemented in the OEMGSM1xMessage_SVC_StatusListener() function)

   if(sGSM1xMessage.m_sendMethodTraffic == TRUE &&
      sGSM1xMessage.m_sendMethodAccess == FALSE)
   {
      OEMGSM1xMessage_SVC_sendSMS( GSM1xMESSAGE_TRAFFIC_CHANNEL);
   }
   else
   {
      // First, we try to send this message over an access channel. If this
      // attempt fails, then the phone will let us know with an error status
      // set to UASMS_ACCESS_TOO_LARGE_S. At that time (depending upon the
      // sendMethod), we will set up a DTC and will try to send this same
      // message on a traffic channel.
      OEMGSM1xMessage_SVC_sendSMS( GSM1xMESSAGE_ACCESS_CHANNEL);
   }
}


/*=============================================================================
Function: OEMGSM1xMessage_SVC_sendSMS()

Description:
   This function is used to initiate an asynchronous SMS mobile origination.

   If method == GSM1xMESSAGE_TRAFFIC_CHANNEL:
      this function establishes a SO6/14 connection and relies on the
      EventListener callback function to originate the transmission.

   If method == GSM1xMESSAGE_ACCESS_CHANNEL:
      this function will immediately attempt to send the message over
      the access channel.  Note that it performs no checking to determine
      if the message will fit on the access channel or not.  If it does
      not fit, a NMASK_PH_SMS_STATUS notify event will be generated with
      a status code of UASMS_ACCESS_TOO_LARGE_S.

Parameters:
   po:          Pointer to the object
   method:      Transmission method (access or traffic?)

Return Value:
   ENOTYPE:  po (self pointer is NULL). This should never happen.
   EFAILED:  General failure while sending MO SMS.
   SUCCESS : To indicate that the message is successfully handed over to
             the UASMS layer.
Comments:
   None

Side Effects:
   None

Also See:
  OEMGSM1xMessage_SVC_SendMessage()
===========================================================================*/
static void OEMGSM1xMessage_SVC_sendSMS(GSM1xMessage_SMSSendMethod method)
{
   GSM1x_MOSMS_Entry   *pEntry;
   GSM1x_MOSMSCallBack *pe;

#ifdef FEATURE_GSM1x_WMS
   wms_status_e_type   wmsStatus;
#else
   uasms_status_e_type smsStatus;
#endif

   //Ensure we have been initialized
   if (! sGSM1xMessage.m_bInitialized)
   {
      return;
   }

   DBGPRINTF("GSM1xM:sendSMS.Method(%d)", method);

   //assert (sGSM1xMessage.m_pMOSMS_Active != NULL)

   pEntry = sGSM1xMessage.m_pMOSMS_Active;
   pe     = &(pEntry->cEntry);


   if(method == GSM1xMESSAGE_ACCESS_CHANNEL)
   {
      // assert (pe.eState == GSM1x_MOSMS_STATE_QUEUED)

      pe->eState = GSM1x_MOSMS_STATE_SEND_ON_ACCESS;

#ifdef FEATURE_GSM1x_WMS
      wmsStatus = wms_msg_send(sGSM1xMessage.m_clientId, 
                               sGSM1xMessage.m_cmdCB,
                               (void *)pEntry,
                               WMS_SEND_MODE_CLIENT_MESSAGE,
                               &pe->cMsg);
      DBGPRINTF("GSM1xM:Send on ACCESS.Status(%d)", wmsStatus);
      if (wmsStatus != WMS_OK_S)
      {
         GSM1x_MOSMSCallBack_ReportStatus(pe,WMS_ERROR_NONE,wmsStatus);
      }
#else
      smsStatus = uasms_send_message((void *)pEntry,
                                     &(pe->cMsg));
      DBGPRINTF("GSM1xM:Send on ACCESS.Status(%d)", smsStatus);
      if (smsStatus != UASMS_OK_S)
      {
         GSM1x_MOSMSCallBack_ReportStatus(pe,UASMS_ERROR_NONE,smsStatus);
      }
#endif

   }
   else if(method == GSM1xMESSAGE_TRAFFIC_CHANNEL)
   {
      //assert ((pe->eState == GSM1x_MOSMS_STATE_QUEUED) ||
      //        (pe->eState == GSM1x_MOSMS_STATE_SEND_ON_ACCESS))

      // Before we try to setup a DTC, let's make sure that the phone is not in
      // the conversation state. If so, we just simply pass this message to the
      // CM and it will construct the data burst message and will send it on the
      // already established traffic channel.

      if (OEMPhone_InConversation(sGSM1xMessage.m_pPhone))
      {
         // Looks like the traffic channel is already present. Let's just send
         // this message without setting up a DTC connection.
         pe->eState = GSM1x_MOSMS_STATE_SEND_ON_DTC;

#ifdef FEATURE_GSM1x_WMS
         wmsStatus = wms_msg_send(sGSM1xMessage.m_clientId, 
                                  sGSM1xMessage.m_cmdCB,
                                  (void *)pEntry,
                                  WMS_SEND_MODE_CLIENT_MESSAGE,
                                  &pe->cMsg);
         DBGPRINTF("GSM1xM:Send on Traffic.Status(%d)", wmsStatus);

         if (wmsStatus != WMS_OK_S)
         {
            GSM1x_MOSMSCallBack_ReportStatus(pe,WMS_ERROR_NONE,wmsStatus);
         }
#else
         smsStatus = uasms_send_message((void *)pEntry,
                                        &(pe->cMsg));
         DBGPRINTF("GSM1xM:Send on Traffic.Status(%d)", smsStatus);

         if (smsStatus != UASMS_OK_S)
         {
            GSM1x_MOSMSCallBack_ReportStatus(pe,UASMS_ERROR_NONE,smsStatus);
         }
#endif
      }
      else
      {
         //assert (sGSM1xMessage.m_eState != GSM1xMESSAGE_STATE_CONNECTING)
         //assert (sGSM1xMessage.m_eState != GSM1xMESSAGE_STATE_CONNECTED)
         // Not possible to be in GSM1xMESSAGE_STATE_CONNECTED state. The GetCallInfo test above
         // should have caught this.

         sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_CONNECTING;
         pe->eState = GSM1x_MOSMS_STATE_WAITING_FOR_DTC;

#ifdef FEATURE_GSM1x_WMS
         wmsStatus = wms_dc_connect(sGSM1xMessage.m_clientId, 
                                    sGSM1xMessage.m_cmdCB,
                                    (const void *)pEntry,
                                    sGSM1xMessage.m_smsSOConfig);
         DBGPRINTF("GSM1xM:Called DTC connect.Status(%d)", wmsStatus);

         if (wmsStatus != WMS_OK_S)
         {
            GSM1x_MOSMSCallBack_ReportStatus(pe,WMS_ERROR_NONE,wmsStatus);
         }
#else
         smsStatus =  uasms_DTC_connect_i(sGSM1xMessage.m_smsSOConfig);
         DBGPRINTF("GSM1xM:Called DTC connect.Status(%d)", smsStatus);

         if (smsStatus != UASMS_OK_S)
         {
            GSM1x_MOSMSCallBack_ReportStatus(pe,UASMS_ERROR_NONE,smsStatus);
         }
#endif

      }
   }
   else
   {
      DBGPRINTF("GSM1xM:Unexpected method to sendSMS: %d",method);
   }

}


#ifdef FEATURE_GSM1x_WMS
/*=============================================================================
FUNCTION: OEMGSM1xMessage_SVC_processWMSMessage                  INTERNAL FUNCTION

DESCRIPTION:
   Do some basic validation on incoming GSM1x WMS message and then forward
   to the appropriate routine.
        
PARAMETERS:
  pm   : IS-637 message in WMS format
  pfn  : Function pointer to process message to be invoked.

RETURN VALUE:
  none

SIDE EFFECTS:
  none 
=============================================================================*/
static void OEMGSM1xMessage_SVC_processWMSMessage(const wms_client_message_s_type * pm,
                                                  PFN_ProcessMessage                pfn)
{
   const wms_cdma_message_s_type *pCDMA_msg;
   byte bd_data[WMS_MAX_LEN];

   pCDMA_msg = &pm->u.cdma_message;

   if (pCDMA_msg->raw_ts.len < 7) 
   {
      DBGPRINTF("GSM1xM:Received an invalid message. Payload less than 7 bytes.");
      return;
   }

   if ((pCDMA_msg->raw_ts.data[0]          != WMS_BD_MSG_ID) ||
       (pCDMA_msg->raw_ts.data[1]          != 3) ||
       ((pCDMA_msg->raw_ts.data[2] & 0xf0) != (WMS_BD_TYPE_DELIVER << 4)) ||
       //(pCDMA_msg->raw_ts.data[3]   ||   // These bytes contain msg Id. We don't
       //(pCDMA_msg->raw_ts.data[4]   ||   // care what they contain. 
       (pCDMA_msg->raw_ts.data[5]          != WMS_BD_USER_DATA) ||
       (pCDMA_msg->raw_ts.data[6]          != pCDMA_msg->raw_ts.len - 7))
   {
      DBGPRINTF("GSM1xM:Received an invalid message.");
      return;
   }
   //pm->raw_bd.bd_data[7,8]  contain encoding & msg len. We don't confirm this
   //pm->raw_bd.bd_data[.. last byte contains dummy data].

   MEMCPY(bd_data, &(pCDMA_msg->raw_ts.data[9]) , pCDMA_msg->raw_ts.len-10);
   pfn(bd_data,pCDMA_msg->raw_ts.len-10);
}

#else  // #ifdef FEATURE_GSM1x_WMS
/*=============================================================================
FUNCTION: OEMGSM1xMessage_SVC_processUASMSMessage                INTERNAL FUNCTION

DESCRIPTION:
     Do some basic validation on incoming GSM1x UASMS message and then forward
to the appropriate routine.
        
PARAMETERS:
  pm   : IS-637 message in uasms format
  pfn  : Function pointer to process message to be invoked.

RETURN VALUE:
  none

SIDE EFFECTS:
  none 
=============================================================================*/
static void OEMGSM1xMessage_SVC_processUASMSMessage(const uasms_client_message_type * pm,
                                                    PFN_ProcessMessage                pfn)
{
   byte bd_data[UASMS_MAX_LEN];

   if (pm->raw_bd.bd_len < 7) 
   {
      DBGPRINTF("GSM1xM:Received an invalid message. Payload less than 7 bytes.");
      return;
   }

   if ((pm->raw_bd.bd_data[0]           != UASMS_BD_MSG_ID) ||
       (pm->raw_bd.bd_data[1]           != 3) ||
       ((pm->raw_bd.bd_data[2] & 0xf0)  != (UASMS_BD_TYPE_DELIVER << 4)) ||
       //(pm->raw_bd.bd_data[3]   ||    // These bytes contain msg Id. We don't
       //(pm->raw_bd.bd_data[4]   ||    // care what they contain. 
       (pm->raw_bd.bd_data[5]           != UASMS_BD_USER_DATA) ||
       (pm->raw_bd.bd_data[6]           != (pm->raw_bd.bd_len - 7)))
   {
      DBGPRINTF("GSM1xM:Received an invalid message.");
      return;
   }
   //pm->raw_bd.bd_data[7,8]  contain encoding & msg len. We don't confirm this
   //pm->raw_bd.bd_data[.. last byte contains dummy data].

   MEMCPY(bd_data, &(pm->raw_bd.bd_data[9]) , pm->raw_bd.bd_len-10);
   pfn(bd_data, pm->raw_bd.bd_len-10);
}


#endif  // #ifdef FEATURE_GSM1x_WMS
/*===========================================================================

===========================================================================*/

#if defined(FEATURE_GSM1x_WMS)

static void 
    OEMGSM1xMessage_SVC_MessageListener(wms_msg_event_e_type event, 
                                        wms_msg_event_info_s_type * info_ptr,
                                        boolean *shared)
{
   // See if the interface is running...
   if (! sGSM1xMessage.m_bInitialized)
   {
      return;
   }

   switch (event) {

      case WMS_MSG_EVENT_RECEIVED_MESSAGE:
      {
         // TBD - Changed type for uint32 to wms_transaction_id_type.
         wms_transaction_id_type  transaction_id;
         wms_ack_info_s_type      ackInfo;
         const wms_client_message_s_type *pWMSClientMsg;
         wms_cdma_message_s_type *pm;
         
         // initialize transaction id in ack info
         // srao - Move this inside Event processing ?
         transaction_id = info_ptr->mt_message_info.transaction_id;
         
         MEMSET(&ackInfo,0,sizeof(wms_ack_info_s_type));   // TODO use sizeof(ackInfo)
         ackInfo.transaction_id = transaction_id;
         ackInfo.message_mode = info_ptr->mt_message_info.message.msg_hdr.message_mode;

         pWMSClientMsg = &info_ptr->mt_message_info.message;

         if (pWMSClientMsg->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) {

            //initialise  cdma message type
            pm = (wms_cdma_message_s_type*)&info_ptr->mt_message_info.message.u.cdma_message;

            if (pm->teleservice == GSM1xMESSAGE_TELESERVICE_SMS)
            {
               DBGPRINTF("GSM1xM:Got ts=%d",pm->teleservice);
               // GSM1x SMS is send with TL Ack turned off, however if n/w
               // request it we can ack immediately.
               //
               if(pm->is_tl_ack_requested)
               {
                  DBGPRINTF("GSM1xM: Ack sent (id:%d)", transaction_id);
                  ackInfo.u.cdma.error_class = WMS_ERROR_NONE;
                  (void) wms_msg_ack(sGSM1xMessage.m_clientId, sGSM1xMessage.m_cmdCB, NULL,&ackInfo);
               }

               OEMGSM1xMessage_SVC_processWMSMessage(pWMSClientMsg, OEMGSMSMS_SVC_ProcessMessage);
               return;
            }

            if (pm->teleservice == GSM1xMESSAGE_TELESERVICE_SIG) 
            {
               DBGPRINTF("GSM1xM:Got ts=%d",pm->teleservice);
               // GSM1x signaling is send with TL Ack turned off, however if n/w
               // request it we can ack immediately.
               //
               if(pm->is_tl_ack_requested)
               {
                  DBGPRINTF("GSM1xM: Ack sent (id:%d)", transaction_id);
                  ackInfo.u.cdma.error_class = WMS_ERROR_NONE;
                  (void) wms_msg_ack(sGSM1xMessage.m_clientId, sGSM1xMessage.m_cmdCB, NULL,&ackInfo);
               }

               OEMGSM1xMessage_SVC_processWMSMessage(pWMSClientMsg, OEMGSM1xSig_SVC_ProcessMessage);
               return;
            }

         }

         break;

      }   // WMS_MSG_EVENT_RECEIVED_MESSAGE

      case WMS_MSG_EVENT_SEND :
      {
         //This event is handled in QIS files, but we are
         //ignoring this event.
         return;
         break;
      } // WMS_MSG_EVENT_SEND

      case WMS_MSG_EVENT_SUBMIT_REPORT:
      {
         GSM1x_MOSMS_Entry *pEntry = (GSM1x_MOSMS_Entry *)
                                      info_ptr->submit_report_info.user_data;
         GSM1x_MOSMSCallBack *pe;

         if ((sGSM1xMessage.m_pMOSMS_Active == NULL) ||
             (sGSM1xMessage.m_pMOSMS_Active != (GSM1x_MOSMS_Entry *)pEntry))
         {
            // Somebody else's message status
            return;
         }

         pe = &(pEntry->cEntry);

         // Check the Message Mode.
         if (info_ptr->submit_report_info.message_mode == WMS_MESSAGE_MODE_CDMA) 
         {

#ifdef FEATURE_CDSMS
            DBGPRINTF("GSM1xM:StatLis pe(%x)Stat(%d)Err(%d)", 
                      pe,
                      info_ptr->submit_report_info.report_status,
                      info_ptr->submit_report_info.cause_info.error_class);

            switch(info_ptr->submit_report_info.report_status) {
               //if access too large event info
               case WMS_RPT_ACCESS_TOO_LARGE:
                  // If the status is WMS_ACCESS_TOO_LARGE_S then it means that we can
                  // not send this message on the access channel and we need to setup
                  // a DTC to send this message.
                  if (pe->eState == GSM1x_MOSMS_STATE_SEND_ON_ACCESS)
                  {
                     // We need to setup a DTC connection to send this message on a
                     // dedicated traffic channel (if and only if the method is set
                     // to OEMNV_SMS_MO_ACCESS_AND_TRAFFIC). Othewise, we treat this
                     // as a failure and fall through to report an error to the
                     // application.
                     if(sGSM1xMessage.m_sendMethodAccess == TRUE &&
			sGSM1xMessage.m_sendMethodTraffic == TRUE)
                     {
                        // We failed to send this message on an access channel. Let's try to
                        // send it on a traffic channel. (The OEMGSM1xMessage_SVC_sendSMS method takes
                        // care of setting up the DTC for us)
      
                        OEMGSM1xMessage_SVC_sendSMS(GSM1xMESSAGE_TRAFFIC_CHANNEL);
                        return;
                     }
                  }
                  break;
            
               /*TBD case WMS_RPT_LL_ERROR:   // TODO do we need to handle this separately
                  if (OEMGSM1xMessage_SVC_sendSMS( GSM1xMESSAGE_TRAFFIC_CHANNEL) == SUCCESS)
                  {
                     return;
                  }
                  break;
                */
               default:
                  break;

            }
            // In some instances, error class can be equal to WMS_ERROR_NONE and
            // the status will not be WMS_OK_S (an example is when the lower layer
            // is awaiting a TL ACK).  But, these two conditions (WMS_OK_S, or a
            // WMS_ERROR_... of some type) indicate a pass or a fail type of
            // notification.
            if (info_ptr->submit_report_info.report_status == WMS_RPT_OK ||
                info_ptr->submit_report_info.cause_info.error_class != WMS_ERROR_NONE)
            {
               GSM1x_MOSMSCallBack_ReportStatus(pe,
                                   info_ptr->submit_report_info.cause_info.error_class,
                                   info_ptr->submit_report_info.report_status);
            }
            else
            {
               DBGPRINTF("GSM1xM:st=%x err=%d",
                          info_ptr->submit_report_info.report_status,
                          info_ptr->submit_report_info.cause_info.error_class);
            }
            return;
#endif // FEATURE_CDSMS

         }
         else 
         {
            DBGPRINTF("GSM1xM:Unknown msg mode %d in SubmitReport",
                      info_ptr->submit_report_info.message_mode);
            return;
         }
         break;
      }  // WMS_MSG_EVENT_SUBMIT_REPORT

      default:
      {
          DBGPRINTF("GSM1xM:event%d Ignored",event);
          return;
          break;
      } // default

   } // switch (event)

   DBGPRINTF("GSM1xM:Unexpected event %d",event);
   return;
}


static void 
    OEMGSM1xMessage_SVC_EventListener(wms_dc_event_e_type  event,
                                      wms_dc_event_info_s_type *info_ptr)
{
   PARAM_NOT_REF(info_ptr)

   // Null pointer check (do not remove, as this function could be invoked
   // before BREW is initialised)
   if (!sGSM1xMessage.m_bInitialized)
   {
      return;
   }

   switch(event)
   {
      case WMS_DC_EVENT_CONNECTING:
         return;

      case WMS_DC_EVENT_CONNECTED:
      {
         wms_status_e_type wmsStatus;
         DBGPRINTF("GSM1xM:Got connected event");

         if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTING)
         {
            GSM1x_MOSMS_Entry   *pEntry = sGSM1xMessage.m_pMOSMS_Active;
            GSM1x_MOSMSCallBack *pe     = &(pEntry->cEntry);

            if (sGSM1xMessage.m_pMOSMS_Active == 0) 
            {
               DBGPRINTF("GSM1xM:*err* no active mosms");
               return;
            }
            if (pe->eState != GSM1x_MOSMS_STATE_WAITING_FOR_DTC) 
            {
               DBGPRINTF("GSM1xM:*err* pe(%x)not waiting for dtc",pe);
               return;
            }

            pe->eState = GSM1x_MOSMS_STATE_SEND_ON_DTC;
            sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_CONNECTED;

            wmsStatus = wms_msg_send(sGSM1xMessage.m_clientId, 
                                     sGSM1xMessage.m_cmdCB,
                                     (void *)pEntry,
                                     WMS_SEND_MODE_CLIENT_MESSAGE,
                                     &pe->cMsg);
            DBGPRINTF("GSM1xM:SMS on traffic.Status(%d)",wmsStatus);

            if (wmsStatus != WMS_OK_S)
            {
               GSM1x_MOSMSCallBack_ReportStatus(pe,WMS_ERROR_NONE,wmsStatus);
            }
          }
          // else this event was generated by some other client
          // we can ignore this.
         break;
      }

      case WMS_DC_EVENT_DISCONNECTED:
      case WMS_DC_EVENT_ABORTED:

         DBGPRINTF("GSM1xM:Got %d event",event);
         if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTING)
         {
            // If we were trying to connect and wms fails to
            // connect it can send this event
            sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP;

            // Since our connect attempt failed, we need to
            // report error for oustanding MO SMS.
            if (sGSM1xMessage.m_pMOSMS_Active)
            {
               GSM1x_MOSMSCallBack* pe = &(sGSM1xMessage.m_pMOSMS_Active->cEntry);

               // Do the cleanup, reporting etc and put the entry back into
               // free Q
               GSM1x_MOSMSCallBack_ReportStatus(pe,
                                    WMS_ERROR_NONE,WMS_NETWORK_NOT_READY_S);
             }
         }
         else if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTED)
         {
            // Some other app closed our connection or there
            // was a timeout
            //
            sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP;

            // If there is an outstanding MO SMS, wms will report
            // the status. We will handle the clearing up then.
         }
         break;

      default:
         // Currently, not interested in handling any other events...
         break;
   }
}

#else           //#if defined(FEATURE_GSM1x_WMS)


/*===========================================================================

Function: OEMGSM1xMessage_SVC_MessageListener

Description:
   The message listener invoked from the UASMS/WMS whenever a new SMS comes in.
   If the teleservice is a GSM1x teleservice, we will handle the SMS, else we 
   will pass the SMS to IPhone. 
   
Parameters:
   transaction_id [in]:    Transaction id of the uasms message
   pm [in]:                SMS in uasms format.

Return Value:
   None

Side Effects:
   None
===========================================================================*/
static void 
    OEMGSM1xMessage_SVC_MessageListener(uasms_transaction_id_type transaction_id,
                                        const uasms_client_message_type * pm)
{

   if (! sGSM1xMessage.m_bInitialized)
   {
      MSG_ERROR("Status Listener called before init",0,0,0);
      return;
   }

   if (pm->teleservice == GSM1xMESSAGE_TELESERVICE_SMS)
   {
      DBGPRINTF("GSM1xM:Got ts=%d",pm->teleservice);
      // GSM1x SMS is send with TL Ack turned off, however if n/w
      // request it we can ack immediately.
      //
      if (pm->is_tl_ack_requested) 
      {
         (void) uasms_ack_message(transaction_id,
                                  UASMS_ERROR_NONE,
                                  UASMS_OK_S);
      }

      OEMGSM1xMessage_SVC_processUASMSMessage(pm, OEMGSMSMS_SVC_ProcessMessage);
      return;
   }

   if (pm->teleservice == GSM1xMESSAGE_TELESERVICE_SIG) 
   {
      DBGPRINTF("GSM1xM:Got ts=%d",pm->teleservice);
      // GSM1x signaling is send with TL Ack turned off, however if n/w
      // request it we can ack immediately.
      //
      if (pm->is_tl_ack_requested) 
      {
         (void) uasms_ack_message(transaction_id,
                                  UASMS_ERROR_NONE,
                                  UASMS_OK_S);
      }

      OEMGSM1xMessage_SVC_processUASMSMessage(pm, OEMGSM1xSig_SVC_ProcessMessage);
      return;
   }


   // Call the IPhone message listener
   if (sGSM1xMessage.uasms_MessageListener)
   {
      sGSM1xMessage.uasms_MessageListener(transaction_id, pm);
   }

}

/*===========================================================================

Function: OEMGSM1xMessage_SVC_StatusListener()

Description:
   The status listener callback is invoked to pass along the status of the 
   SMS data burst operation.

   This callback function is executed within the context of the UASMS task.  
   If we need to report status we save the state and then do a resumeCallback
   to deal with it.

Dependencies:
   BREW must be initialized.

Parameters:
   user_data [in]:      User data for which status is being reported.
   error_class [in]:    Permanent/Temporary/No error
   status [in]:         The status of the MO SMS

Return Value:
   TRUE:      Handled the status locally.
   FALSE:     Status not handled. Pass the status to IPhone.

Side Effects:
   None 
===========================================================================*/
static boolean 
    OEMGSM1xMessage_SVC_localStatusListener(const void * user_data, 
                                            uasms_error_class_e_type error_class,
                                            uasms_status_e_type status)
{
   GSM1x_MOSMS_Entry   *pEntry = (GSM1x_MOSMS_Entry *)user_data;
   GSM1x_MOSMSCallBack *pe;
   //byte sendMethod;

   if ((sGSM1xMessage.m_pMOSMS_Active == NULL) || 
       (sGSM1xMessage.m_pMOSMS_Active != (GSM1x_MOSMS_Entry *)user_data)) 
   {
      // Somebody else's message status
      return FALSE;
   }

   DBGPRINTF("GSM1xM:pe=%x st=%x err=%d",user_data,status, error_class); 

   pe = &(pEntry->cEntry);

   if (status == UASMS_WAITING_FOR_TL_ACK_S) 
   {
      // assert (pe->eState != GSM1x_MOSMS_STATE_WAITING_FOR_TL_ACK ) 
      // pe->eState = GSM1x_MOSMS_STATE_WAITING_FOR_TL_ACK;
      // We no longer have WAITING_FOR_TL_ACK state.
      return TRUE;
   }

   if (status == UASMS_ACCESS_TOO_LARGE_S)
   {
      // If the status is UASMS_ACCESS_TOO_LARGE_S then it means that we can
      // not send this message on the access channel and we need to setup 
      // a DTC to send this message.
      if (pe->eState == GSM1x_MOSMS_STATE_SEND_ON_ACCESS) 
      {
         // We need to setup a DTC connection to send this message on a 
         // dedicated traffic channel (if and only if the method is set 
         // to OEMNV_SMS_MO_ACCESS_AND_TRAFFIC). Othewise, we treat this 
         // as a failure and fall through to report an error to the 
         // application.
	if(sGSM1xMessage.m_sendMethodAccess == TRUE &&
	   sGSM1xMessage.m_sendMethodTraffic == TRUE)
         {
            // We failed to send this message on an access channel. Let's try to
            // send it on a traffic channel. (The OEMGSM1xMessage_SVC_sendSMS method takes 
            // care of setting up the DTC for us)

            OEMGSM1xMessage_SVC_sendSMS(GSM1xMESSAGE_TRAFFIC_CHANNEL);
            return TRUE;
         }
      }
   }

   // If we are here we have to report the status
   //

   // In some instances, error class can be equal to UASMS_ERROR_NONE and
   // the status will not be UASMS_OK_S (an example is when the lower layer
   // is awaiting a TL ACK).  But, these two conditions (UASMS_OK_S, or a
   // UASMS_ERROR_... of some type) indicate a pass or a fail type of
   // notification.
   if (status == UASMS_OK_S || error_class != UASMS_ERROR_NONE)
   {
      GSM1x_MOSMSCallBack_ReportStatus(pe,error_class,status);
   }
   else
   {
      DBGPRINTF("GSM1xM:Ignoring status");
   }
   return TRUE;
}


/*===========================================================================

Function: OEMGSM1xMessage_SVC_StatusListener

Description:
   The status listener is invoked from the UASMS/WMS. It will do local handling
   with _localStatusListener function; if the local function doesn't handle 
   the status is passed to IPhone.
   
Parameters:
   user_data [in]:      User data for which status is being reported.
   error_class [in]:    Permanent/Temporary/No error
   status [in]:         The status of the MO SMS

Return Value:
   None

Side Effects:
   None
===========================================================================*/
static void 
    OEMGSM1xMessage_SVC_StatusListener(const void * user_data, 
                                       uasms_error_class_e_type error_class,
                                       uasms_status_e_type status)
{
   if (! sGSM1xMessage.m_bInitialized)
   {
      MSG_ERROR("Status Listener called before init",0,0,0);
      return;
   }

   if (OEMGSM1xMessage_SVC_localStatusListener(user_data,error_class,status)) 
   {
      return;
   }

   if (sGSM1xMessage.uasms_StatusListener)
   {
      sGSM1xMessage.uasms_StatusListener(user_data,error_class,status);
   }
}


/*===========================================================================

Function: OEMGSM1xMessage_SVC_localEventListener

Description:
   The event listener is invoked to pass along the status of the DTC. If 
   OEMGSM1xMessage initiated the DTC, update the status.

   The assumption is made that the "CONNECTED" status is always followed by
   a "DISCONNECTED" Status, and that it is safe to send when connected.

   This callback function is executed within the context of the UASMS/WMS task.  
   If there was some error condition we need to report status. We save the status 
   and then get TAPI's callback to deal with it.


Parameters:
   event [in]:       The event associated with the DTC Status
   info_ptr [in]:    Unused

Return Value:
   None

Side Effects:
   None
===========================================================================*/
static void OEMGSM1xMessage_SVC_localEventListener(
                                   uasms_event_e_type event,
                                   uasms_event_info_type * info_ptr)
{
    PARAM_NOT_REF(info_ptr)

    if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_NO_DTC_SETUP)
    {
       // We didn't initiate any connect or disconnet, 
       // Ignore this message.
       return;
    }

    DBGPRINTF("GSM1xM:Got event %d",event);

    //Originate the call
    if (event == UASMS_EVENT_DTC_CONNECTED)
    {
       uasms_status_e_type smsStatus;

       if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTING) 
       {
          GSM1x_MOSMS_Entry   *pEntry = sGSM1xMessage.m_pMOSMS_Active;
          GSM1x_MOSMSCallBack *pe     = &(pEntry->cEntry);

          if (sGSM1xMessage.m_pMOSMS_Active == 0) 
          {
             DBGPRINTF("GSM1xM:*err* no active mosms");
             return;
          }
          if (pe->eState != GSM1x_MOSMS_STATE_WAITING_FOR_DTC) 
          {
             DBGPRINTF("GSM1xM:*err* pe(%x)not waiting for dtc",pe);
             return;
          }

          sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_CONNECTED;
          pe->eState = GSM1x_MOSMS_STATE_SEND_ON_DTC;

          smsStatus = uasms_send_message((void *)pEntry,&(pe->cMsg));
          DBGPRINTF("GSM1xM:Send on DTC.Status(%d)", smsStatus);

          if (smsStatus != UASMS_OK_S)
          {
             GSM1x_MOSMSCallBack_ReportStatus(pe,UASMS_ERROR_NONE,smsStatus);
             return;
          }
       }
       // else this event was generated by some other client
       // we can ignore this.
       return;
    }
    else if (event == UASMS_EVENT_DTC_DISCONNECTED)
    {
       if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTING)
       {
          // If we were trying to connect and uasms fails to 
          // connect it can send this event
          sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP;

          // Since our connect attempt failed, we need to 
          // report error for oustanding MO SMS. We will drop
          // to the end of this function to handle the error case.
       }
       else if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTED)
       {
          // Some other app closed our connection or there
          // was a timeout
          //
          sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP; 

          // If there is an outstanding MO SMS, uasms will report
          // the status. We will handle the clearing up then.
          //
          return;
       }
    }
    else if (event == UASMS_EVENT_DTC_ABORTED)
    {
       if (sGSM1xMessage.m_eState == GSM1xMESSAGE_STATE_CONNECTED) 
       {
          // Somebody else's message
          // We don't need to send any callback clearing
          // but we need to update the state.

          sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP;
          return;
       }

       sGSM1xMessage.m_eState = GSM1xMESSAGE_STATE_NO_DTC_SETUP;

       // We got this event because we were trying to connect 
       // and uasms failed to connect

       // Since our connect attempt failed, we need to 
       // report error for oustanding MO SMS. We will drop
       // to the end of this function to handle the error case.
    }
    else 
    {
        // We may get some events that we are not interested in.
        return;
    }

    // This is an error case. We were trying to setup a DTC to 
    // send MO SMS and the connect failed. 
    // Create a dummy status and report an error.
    //
    
    if (sGSM1xMessage.m_pMOSMS_Active) 
    {
       GSM1x_MOSMSCallBack* pe = &(sGSM1xMessage.m_pMOSMS_Active->cEntry);

       // Do the cleanup, reporting etc and put the entry back into free Q
       //
       GSM1x_MOSMSCallBack_ReportStatus(pe,UASMS_ERROR_NONE,UASMS_NETWORK_NOT_READY_S);
    }
    // else. Can't think how we can come here.
}


/*===========================================================================

Function: OEMGSM1xMessage_SVC_EventListener

Description:
   The event listener is invoked from the UASMS/WMS. It will do local handling
   with _localEventListener function and then pass on the event to IPhone.
   
Parameters:
   event [in]:       The event associated with the DTC Status
   info_ptr [in]:    Unused

Return Value:
   None

Side Effects:
   None
===========================================================================*/
static void     
    OEMGSM1xMessage_SVC_EventListener(uasms_event_e_type event,
                                      uasms_event_info_type * info_ptr) 
{
   //Not initialized check (do not remove, as this function could be invoked
   //before BREW is initialised)
   //
   if (! sGSM1xMessage.m_bInitialized)
   {
      MSG_ERROR("Status Listener called before init",0,0,0);
      return;
   }
   OEMGSM1xMessage_SVC_localEventListener(event,info_ptr);

   if (sGSM1xMessage.uasms_EventListener) 
   {
      sGSM1xMessage.uasms_EventListener(event,info_ptr);
   }
}

#endif  //#if defined(FEATURE_GSM1x_WMS)

#endif  // ifdef FEATURE_GSM1x
