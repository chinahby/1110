/*=============================================================================

FILE: AEESysClock.c
  
SERVICES: System Clock class implementation. This forms the basis for BREW's
          StdLib time functions and in addition available for application
          creation. 
    
GENERAL DESCRIPTION:
    Inherited from ISysClock interface 

        Copyright ?2006-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEETelephone.h"
#include "AEESysClock.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#ifdef CUST_EDITION	  
#include "qw.h"
#include "clk.h"
#endif /*CUST_EDITION*/
#ifdef FEATURE_PMIC_RTC
#include "clkrtc.h"
#endif

#if defined(FEATURE_USE_TIME_VU)
#include "time_svc.h" /* some targets name this file "time.h" */
#include "time_jul.h"
#else /* defined(FEATURE_USE_TIME_VU) */
#include "qw.h"
#include "clk.h"
typedef qword time_type;
#define time_julian_type           clk_julian_type 
#define time_get_secs(s)           clk_read_secs(s)
#define time_get_ms(ms)            clk_read_ms(ms)
#define time_set_ms(ms)            clk_set_ms(ms)
#define time_jul_from_secs(s,ptr)  clk_secs_to_julian(s,ptr)
#define time_jul_to_secs(ptr)      clk_julian_to_secs(ptr)
#endif /* defined(FEATURE_USE_TIME_VU) */

#include "OEMConfig.h"
#include "OEMSecSysClock.h"

#include "AEESysClock.bid"
#include "AEESecSysClock.bid"

#ifdef FEATURE_SEC_TIME_V2
#include "time_secure.h"
#endif /* FEATURE_SEC_TIME_V2 */
/*=========================================================================
                         Macro Definitions
=========================================================================*/

#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) if (p) {IQI_Release((IQueryInterface *)(p)); (p) = NULL;}

/*=========================================================================
                         Type Declarations
=========================================================================*/

typedef struct _CSysClockCommon {

   IShell         *piShell;         // Pointer to the shell
   uint32         dwRefs;           // Reference count for this object

   AEECallback    *pcbHead;  // Callback for users to register, to 
                                    // be triggered when user time is 
                                    // valid
   boolean        bValidity;        // Validity of SysClock
   AEECallback    cbSecClkChange;   // Callback for user clock to register
                                    // for secure  clock "Time change" 
   ISysClock      *pSecSysClock;    // Secure SysClock object for user
                                    // sysclock's internal use

   AEECallback    cbOnExit;         // Exit callback for clean-up

#if !(defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
   PhoneListener  PhListenerServiceAvail;
   IModel *pModel;
#endif                                      
} CSysClockCommon;


typedef struct _CSysClock {
  const AEEVTBL(ISysClock) *pvt;   // Inherited from ISysClock interface

  CSysClockCommon *pSysClkCommon;  // Common SysClock Object data
} CSysClock;


/*=========================================================================
                       Imported Function Prototypes
=========================================================================*/

/*=========================================================================
                       Exported Function Prototypes
=========================================================================*/

static void CBCancelGeneric(AEECallback *pcb);

static void SysClock_DoCallbacks(CSysClockCommon *meCommon);

int SysClock_New(IShell *piShell, AEECLSID cls, void **ppif);

/*=========================================================================
                        Global Data Definitions
=========================================================================*/

/*=========================================================================
                       Static variable Definitions
=========================================================================*/

static CSysClockCommon *gpSysClkCommon = NULL;   // Common Object data
static CSysClock *gpSysClockSet = NULL;    // Settable SysClock 
static CSysClock *gpSysClockNoSet = NULL;  // Un-settable SysClock 

extern const AEEStaticClass gAEESysClockClasses[] = {
   {AEECLSID_SYSCLOCK, ASCF_UPGRADE, 0, 0, SysClock_New},
   {0, 0, 0, NULL, NULL} 
};

/*==========================================================================
                      SYS CLOCK FUNCTION DEFINITIONS
============================================================================

FUNCTION:  CSysClock_AddRef()

DESCRIPTION:
   This function increments the reference count of the CSysClock object.
   This allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero). 
   
PARAMETERS:
   
RETURN: 

===========================================================================*/
static uint32 CSysClock_AddRef(ISysClock *po)
{
   return(++((CSysClock *)po)->pSysClkCommon->dwRefs);
}

static void SysClock_Destroy(CSysClockCommon *meCommon)
{
   if (meCommon) {
      CALLBACK_Cancel(&meCommon->cbOnExit);
      CALLBACK_Cancel(&meCommon->cbSecClkChange);
#if !(defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)    
      LISTENER_Cancel(&meCommon->PhListenerServiceAvail);
      RELEASEIF(meCommon->pModel);
#endif
   
      if (meCommon->pcbHead) {
         meCommon->pcbHead->pfnCancel = NULL;
         meCommon->pcbHead = NULL;
      }

      RELEASEIF(meCommon->pSecSysClock); 
      RELEASEIF(meCommon->piShell);
   }
   
   FREEIF(gpSysClkCommon);
   FREEIF(gpSysClockSet);
   FREEIF(gpSysClockNoSet);
}

/*===========================================================================

FUNCTION:  CSysClock_Release()

DESCRIPTION:
   This function decrements the reference count of the CSysClock object.
   This allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero). 
   
PARAMETERS:
   
Return Value: 

===========================================================================*/
static uint32 CSysClock_Release(ISysClock *po)
{
   CSysClock *me = (CSysClock *)po;

   uint32 dwRefs = --me->pSysClkCommon->dwRefs;

   if (0 == dwRefs) {
      SysClock_Destroy(me->pSysClkCommon);
   }

   return dwRefs;
}


static int CSysClock_QueryInterface(ISysClock *po, AEECLSID cls, void **ppo)
{
   switch (cls) {
      case AEECLSID_QUERYINTERFACE:
      case AEEIID_SYSCLOCK:
      case AEECLSID_SYSCLOCK:
         *ppo = (void *)po;
         CSysClock_AddRef(po);
         return SUCCESS;
      default:
         *ppo = NULL;
         return ECLASSNOTSUPPORT;
   }
}


/*==========================================================================
FUNCTION: CSysClock_GetTimeUS()

DESCRIPTION: 
   Returns the value of system clock (CDMA Time/TOD + User Offset) in 
   micro-seconds. Return code if SUCCESS means get time operation is successful
   and time is valid. Return code EBADSTATE means the get time operation is
   successful but the time is not valid
  
==========================================================================*/
static int CSysClock_GetTimeUS(ISysClock *po, uint64struct *pstUS)
{
   CSysClock *me = (CSysClock *)po;
   qword  qwTS;
   uint64 qwUSecTime;
   int nErr = AEE_SUCCESS;

   #ifdef FEATURE_SEC_TIME_V2

   // Read CDMA Time/TOD Clock in milli-seconds
   time_secure_get_user_time_ms(qwTS);
   // Convert qword to uint64 and time to micro-seconds 
   qwUSecTime = QW_CVT_Q2N(qwTS) * 1000;

   #else

   int32  nUsrOffset=0;

   // Read CDMA Time/TOD Clock in milli-seconds
   time_get_ms (qwTS);
   // Convert qword to uint64 and time to micro-seconds 
   qwUSecTime  = QW_CVT_Q2N (qwTS) * 1000;

   nErr = OEM_GetConfig(CFGI_USER_CLOCK_OFFSET, &nUsrOffset, sizeof(int32));
   if (SUCCESS == nErr) {
      // Add the User Clock offset after converting from secs to usecs
      qwUSecTime = (uint64)((int64)qwUSecTime + ((int64)nUsrOffset * 1000000));
   }

   #endif

   {
      uint32 dwSecs;
      
      dwSecs = (uint32)(qwUSecTime/1000000);

      if (dwSecs < ONE_YEAR_SECS) {
         // Don't let users set time less 1981
         me->pSysClkCommon->bValidity = FALSE;
      }
      else {
         //Reinforce that user time is valid
         me->pSysClkCommon->bValidity = TRUE;
      }
   }

   // if pstUS is NULL only the validity of the clock is returned
   if (pstUS) {
      *pstUS = uint64struct_from_uint64(qwUSecTime);
   }
   
   if (me->pSysClkCommon->bValidity) {
      return nErr;
   }
   else {
      return EBADSTATE;
   }
}


/*============================================================================
FUNCTION: CSysClock_SetTimeUS

DESCRIPTION: 
  This function should be called for setting the SysClock which updates the 
  user clock offset. The caller must be either in system context or must have 
  AEEPRIVID_SYSCLOCK_SET privilege to be able to adjust the user clock 
  successfully 
=============================================================================*/
static int CSysClock_SetTimeUS(ISysClock *po, uint64struct stUS)
{
   CSysClock *me = (CSysClock *)po;

   uint64 qwUS;
   uint32 dwSecs;
  
   //Convert from "uint64struct" to uint64
   qwUS = uint64struct_to_uint64(stUS);
   dwSecs = (int32)(qwUS/1000000);

   #ifdef FEATURE_SEC_TIME_V2
   {
      time_secure_set_user_time_secs(dwSecs);

      // Assert Validity of SysClock
      me->pSysClkCommon->bValidity = TRUE;
      // SysClock is set successfully fire the "Time Change" Callbacks
      SysClock_DoCallbacks(me->pSysClkCommon);
         
      return SUCCESS;
   }
   #else
   {
      uint32 dwSecsNow;
      int nErr;
      qword  qwTS;

      // Read CDMA Time/TOD Clock in milli-seconds
      time_get_ms(qwTS);
      dwSecsNow = (uint32)(QW_CVT_Q2N(qwTS) / 1000); //Convert to secs

      // If the user passes value '0' for *pqwUS (nUserOffset=0), just reset the  
      // user offset to '0'
      if (dwSecs) {
      // Update the User Clock Offset in seconds
         dwSecs -= (int32)dwSecsNow;
      }

      nErr = OEM_SetConfig(CFGI_USER_CLOCK_OFFSET, &dwSecs, sizeof(int32));
      if (SUCCESS == nErr) {
         // Assert Validity of SysClock
         me->pSysClkCommon->bValidity = TRUE;
         // SysClock is set successfully fire the "Time Change" Callbacks
         SysClock_DoCallbacks(me->pSysClkCommon);
      }
      else {
         // Assert Validity of SysClock
         me->pSysClkCommon->bValidity = FALSE;
         // SysClock is not set successfully hence "Time Change" callbacks
         // are NOT fired
      }
      return nErr; 
   }
   #endif
      
}

static int CSysClock_NoSetTimeUS(ISysClock *po, uint64struct stUS)
{
   // Caller with no set privilege will come here
   return EPRIVLEVEL;
}


static void CBCancelGeneric(AEECallback *pcb)
{
   // Have the previous's next pointer point to the item after us
   *(AEECallback**)(pcb->pCancelData) = pcb->pNext;

   // Have the next's previous pointer point to the item before us
   if (pcb->pNext) {
      pcb->pNext->pCancelData = pcb->pCancelData;
   }

    // Nullify everything
   pcb->pNext = NULL;
   pcb->pfnCancel = NULL;
   pcb->pCancelData = NULL;
}

static void SysClock_DoCallbacks(CSysClockCommon *meCommon)
{
   AEECallback *pcb,*pcbtemp;

   // Move through the CB list and unlink each registerd CB 
   // if time is valid
   for (pcb = meCommon->pcbHead; pcb;)
   {
      pcbtemp = pcb->pNext;

      pcb->pfnCancel = NULL;
      pcb->pCancelData = NULL;
      pcb->pNext = NULL;
      // Schedule the system callback in system context. 
      // Since this is a Linked System AEECallback, BREW takes care of 
      // resuming the original application callback in the associated 
      // Application Context
      AEE_SYS_RESUME(pcb); 

      pcb = pcbtemp;
   }
   meCommon->pcbHead = NULL;
}

/*==========================================================================
FUNCTION: CSysClock_RegisterOnChangeCB

DESCRIPTION: 
  Registers one or more callback (per object) to be called when user time
  is valid. 
  
==========================================================================*/
static int CSysClock_RegisterOnChangeCB(ISysClock *po, AEECallback *pcb)
{
   CSysClock *me = (CSysClock *)po;
   AEECallback  *pcbLinked = NULL;

   // Cancel pending callbacks.
   // Note: No need to do this as the next step AEE_LinkSysCallback()
   // automatically does it. But just to be sure..
   CALLBACK_Cancel(pcb);

   // Link a system AEECallback with the application AEECallback
   // This is to ensure that 1) the callback is called in the corresponding
   // Application's Context and 2) the callback is cancelled/removed from the 
   // list if the application exits before the callback is resumed 
   pcbLinked = AEE_LinkSysCallback(pcb);

   if (NULL== pcbLinked) {
      return ENOMEMORY;
   }

   // Set up the callback
   pcbLinked->pNext = me->pSysClkCommon->pcbHead;
   me->pSysClkCommon->pcbHead = pcbLinked;
   pcbLinked->pfnCancel = CBCancelGeneric;
   pcbLinked->pCancelData = &(me->pSysClkCommon->pcbHead);
   if (pcbLinked->pNext) {
      // Current node's pCancelData -> Previous node's pNext pointer
      pcbLinked->pNext->pCancelData = &(pcbLinked->pNext);
   }

   return SUCCESS;
}

static void SysClock_SecClkChanged(CSysClockCommon *meCommon)
{
   int32 nUserOffset=0;
       
   CALLBACK_Cancel(&meCommon->cbSecClkChange);

   // SysClock class re-registers with Secure SysClock register
   // notified when Secure clock changes. This is so that we can update the
   // user offset and fire the SysClock "Time Change" callbacks if required
   CALLBACK_Init(&meCommon->cbSecClkChange, (PFNNOTIFY)SysClock_SecClkChanged, (void*)meCommon);
   ISYSCLOCK_RegisterOnChangeCB(meCommon->pSecSysClock, &meCommon->cbSecClkChange);

   if (SUCCESS ==  OEM_GetConfig(CFGI_USER_CLOCK_OFFSET, &nUserOffset, sizeof(int32))) {
        // If nUserOffset is > 0 that means user has set the time before 
        // So the user offset needs to be adjusted so that user time stays the
        // same....
        // Adjusting the user offset when (nUserOffest>0) needs to be done inside
        // Secure SysClock Set function because at this point we do not know what 
        // the previous TOD time was, as we have already adjusted the RTC/TOD...
      if (!nUserOffset) {
         // Assert Validity of SysClock
         meCommon->bValidity = TRUE;
         // SysClock time has changed hence fire the "Time Change" callbacks         
         SysClock_DoCallbacks(meCommon);
      }
   }
   else {
      // User offset not implemented on the handset. Still SysClock will
      // be valid as secure clock has been set.
      
      // Assert Validity of SysClock
      meCommon->bValidity = TRUE;
      // SysClock time has changed hence fire the "Time Change" callbacks
      SysClock_DoCallbacks(meCommon);
   }
}


#if !(defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
static void SysClock_ServiceAvail(CSysClockCommon *meCommon, ModelEvent *pEvent)
{
   AEETSSEvent *pSSevt = (AEETSSEvent *)pEvent;

   if(pSSevt ->evCode == AEET_EVENT_SS_SRV_CHANGED) {
      if ((pSSevt->ss.ss_info.srv_status == AEET_SRV_STATUS_SRV) ||
          (pSSevt->ss.ss_info.srv_status == AEET_SRV_STATUS_LIMITED) ||
          (pSSevt->ss.ss_info.srv_status == AEET_SRV_STATUS_LIMITED_REGIONAL)) {

          LISTENER_Cancel(&meCommon->PhListenerServiceAvail);

          meCommon->bValidity = TRUE;
           
          // Fire the registered IsValid Callbacks once we have acquired service
          SysClock_DoCallbacks(meCommon);
      }
   }
}
#endif

static void SysClock_SetValidity(CSysClockCommon *meCommon)
{
   qword qwTS;
   uint32 dwSecs;

   // Read the CDMA time/time-of-day clock in milli-seconds
   time_get_ms(qwTS);         
   dwSecs = (uint32) (QW_CVT_Q2N(qwTS) / 1000); //Convert to secs

   if (dwSecs > ONE_YEAR_SECS) {
      meCommon->bValidity = TRUE;
   }
   else {
#if (defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
      // TOD clock is not valid check to see if CFGI_USER_OFFSET is set
      // which means that the user application has set the preferred time 
      // before. Hence restore the time to that. 
      LastValidCfg lastvalid;
      int32 nUserOffset=0;
       
      if (SUCCESS == OEM_GetConfig(CFGI_LAST_VALID_TIME, &lastvalid, sizeof(LastValidCfg))) {   
         if (SUCCESS ==  OEM_GetConfig(CFGI_USER_CLOCK_OFFSET, &nUserOffset, sizeof(int32))) {
            if (nUserOffset) {
               dwSecs = (uint32)((lastvalid.qwTime)/1000000);
               nUserOffset += (int32)dwSecs;
               
               OEM_SetConfig(CFGI_USER_CLOCK_OFFSET, &nUserOffset, sizeof(int32));
               meCommon->bValidity = TRUE;
            }
            else {
               meCommon->bValidity = FALSE;
            }
         }
      }
#else //#if (defined FEATURE_CDMA_800) || (defined FEATURE_CDMA_1900)      

      // Step for deciding the Validity of SysClock in CDMA mode
      
      // May be we have service but the clock is not sync'ed with the
      // network time when we queried it. So query ITelephone about the
      // current serving system info 
      ITelephone *pTelephone = NULL;
      AEETSSInfo *pSSInfo = NULL;
      int nErr;

      pSSInfo = (AEETSSInfo *)MALLOC(sizeof(AEETSSInfo));

      if (NULL == pSSInfo) {
         return;
      }

      nErr = ISHELL_CreateInstance(meCommon->piShell, AEECLSID_TELEPHONE, (void **) &pTelephone);

      if (SUCCESS == nErr) {
         (void)ITELEPHONE_GetServingSystemInfo(pTelephone, pSSInfo, sizeof(AEETSSInfo));

         if (pSSInfo->srv_status == AEET_SRV_STATUS_NO_SRV) {
         // Service not available and time is not valid. Add a listener to 
         // ITelephone to be notified when Serving System Info changes i.e when 
         // device acquires service
                  
            LISTENER_Init((ModelListener *)&meCommon->PhListenerServiceAvail, SysClock_ServiceAvail, meCommon);
            nErr = ITELEPHONE_QueryInterface(pTelephone, AEEIID_MODEL, (void **) &meCommon->pModel); 
            if (SUCCESS == nErr ) {
               IMODEL_AddListener(meCommon->pModel, (ModelListener*) &meCommon->PhListenerServiceAvail);
            }
         }
         else {
            // Service is available but the time is not updated just
            // yet and will be in-sync with network time soon so set
            // the validity to TRUE
            meCommon->bValidity = TRUE;
         }                  
         ITELEPHONE_Release(pTelephone);
      }
      FREEIF(pSSInfo)
#endif
   }
}


/*=========================================================================

FUNCTION:  CSysClock_New()

DESCRIPTION:
   This is called from the shell when an app tries to create an instance of
   a class ID associated with IClock module.

PARAMETERS:
   IShell* piShell [In]:  IShell pointer
   AEECLSID cls [In]:     Class to create
   void **ppif [Out]:     Pointer to the created interface object

RETURN:  Status indicating success of failure for create instance

=========================================================================*/
int SysClock_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   CSysClockCommon *meCommon = NULL;
   CSysClock *meSet   = NULL;
   CSysClock *meNoSet = NULL;
      
   static const VTBL(ISysClock) CSysClockSetMethods = {CSysClock_AddRef,
                                                      CSysClock_Release,
                                                      CSysClock_QueryInterface,
                                                      CSysClock_GetTimeUS,
                                                      CSysClock_SetTimeUS,
                                                      CSysClock_RegisterOnChangeCB};

   static const VTBL(ISysClock) CSysClockNoSetMethods = {CSysClock_AddRef,
                                                        CSysClock_Release,
                                                        CSysClock_QueryInterface,
                                                        CSysClock_GetTimeUS,
                                                        CSysClock_NoSetTimeUS,
                                                        CSysClock_RegisterOnChangeCB};
                                                      

   if (AEECLSID_SYSCLOCK != cls) {
      return ECLASSNOTSUPPORT;
   }

 
   if (NULL == gpSysClkCommon) {
      PACONTEXT pacLast;

      pacLast = AEE_EnterAppContext(NULL);
      meCommon = MALLOCREC(CSysClockCommon);
      meSet   =  MALLOCREC(CSysClock);
      meNoSet =  MALLOCREC(CSysClock);
      AEE_LeaveAppContext(pacLast);
 
      if ((NULL == meCommon) || (NULL == meSet) || (NULL == meNoSet)){
         return ENOMEMORY;
      }
   
      MEMSET(meCommon, 0, sizeof(CSysClockCommon));
      MEMSET(meSet, 0, sizeof(CSysClock));
      MEMSET(meNoSet, 0, sizeof(CSysClock));

      meSet->pvt   = &CSysClockSetMethods;   // Vtbl for Settable Class
      meNoSet->pvt = &CSysClockNoSetMethods; // Vtbl for non-Settable Class
      
      meCommon->piShell = piShell;
      ISHELL_AddRef(piShell);
 
      {
         int nErr;

         pacLast = AEE_EnterAppContext(NULL);
         nErr = ISHELL_CreateInstance(piShell, AEECLSID_SECSYSCLOCK, (void **)&meCommon->pSecSysClock);     
         AEE_LeaveAppContext(pacLast);
         
         if (SUCCESS == nErr) {
            // Since Secure SysClock is the controller of the actual hardware
            // clock, SysClock class registers a callback to be called back
            // whenever Secure Clk Time Changes. SysClock updates the "user offset"
            // and fires its registered "Time change" callbacks, if required
            CALLBACK_Init(&meCommon->cbSecClkChange, (PFNNOTIFY)SysClock_SecClkChanged, (void*)meCommon);
            ISYSCLOCK_RegisterOnChangeCB(meCommon->pSecSysClock, &meCommon->cbSecClkChange);
         }
      }
    
      // Step for deciding the Initial Validity of SysClock 
      SysClock_SetValidity(meCommon);

      // Callback to clean up memory associated with this object when BREW exits.
      CALLBACK_Init(&meCommon->cbOnExit, (PFNNOTIFY)SysClock_Destroy, (void*)meCommon);
      ISHELL_OnExit(piShell, &meCommon->cbOnExit);

      gpSysClkCommon  = meCommon;
      gpSysClockSet   = meSet;
      gpSysClockNoSet = meNoSet;

      gpSysClockSet->pSysClkCommon   = gpSysClkCommon;
      gpSysClockNoSet->pSysClkCommon = gpSysClkCommon;
   }

   
   if (ISHELL_CheckPrivLevel(piShell, AEEPRIVID_SYSCLOCK_SET, TRUE)) {
      // Has privileges to set the user clock. Increase the reference count of
      // the corresponding object
      CSysClock_AddRef((ISysClock *)gpSysClockSet);
      *ppif = gpSysClockSet;

   }
   else {
      // Does not have privileges to set the user clock. Increase the reference
      // count of the corresponding object
      CSysClock_AddRef((ISysClock *)gpSysClockNoSet);
      *ppif = gpSysClockNoSet;
   }
   
   return SUCCESS;
}

