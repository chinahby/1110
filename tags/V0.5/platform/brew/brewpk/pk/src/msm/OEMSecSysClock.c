/*=============================================================================

FILE: OEMSecSysClock.c
  
SERVICES: OEM Secure Clock class implementation
    
GENERAL DESCRIPTION:
   Reference OEM implementation of the Secure System Clock (SecSysClock) class 
   inherited from ISysClock interface as defined in AEESysClock.h. SecSysClock 
   provides access to the time-of-day (TOD) clock and real-time clock (if 
   available) on the device. Conceptually the secure clock is a single device 
   but the implementation utilizes the TOD and real-time clock (if available).

        Copyright © 2006-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#ifdef AEE_SIMULATOR
#include <windows.h>
#else
#include "customer.h"
#include "OEMNV.h"
#include "nv.h"
#endif

#include "sys.h"

#if defined(FEATURE_USE_TIME_VU)

#include "time_svc.h" /* some targets name this file "time.h" */
#include "time_jul.h"
#include "time_tod.h"

#else /* defined(FEATURE_USE_TIME_VU) */

#include "clk.h"
#include "qw.h"
#define time_julian_type clk_julian_type
#define time_get_ms(ms) clk_read_ms(ms)
#define time_jul_from_secs(s,ptr) clk_secs_to_julian(s,ptr)
#define time_jul_to_secs(ptr) clk_julian_to_secs(ptr)
#if (defined _WIN32 || defined FEATURE_WCDMA || defined FEATURE_GSM)
/* Mask to extract cx32's (lower 16 bits) from timestamp */
#define TIME_CX32_MASK 0x0FFFFuL
/* To convert TS to 1.25ms units, shift out cx32 bits of timestamp */
#define TIME_TO_1p25MS_SHIFT -16
/* Conversion from 1.25ms to 0.25ms */
#define TIME_0p25MS_PER_1p25MS (125/25)
/* Conversion from cx32's to 0.25ms ticks */
#define TIME_CX32_PER_0p25MS ((49152+3)/5)
/* Shift from 0.25ms units to 1ms units */
#define TIME_0p25MS_TO_1MS_SHIFT -2

static void time_to_ms(qword ts_val)
{
  /* Lower 16 bits of the timestamp (chipx32's) */
  uint32 cx32;

  /* Save lower 16 bits (cx32's) of timestamp */
  cx32 = qw_lo( ts_val ) & TIME_CX32_MASK;
  /* Shift down by 16 to convert to get 1.25ms ticks */
  qw_shift( ts_val, TIME_TO_1p25MS_SHIFT);
  /* Multiply by 5 to convert 1.25ms ticks to 0.25ms ticks */
  qw_mul( ts_val, ts_val, TIME_0p25MS_PER_1p25MS);
  /* Convert cx32's to 0.25ms ticks and add, plus 2 for rounding (below) */
  qw_inc( ts_val, (cx32 / TIME_CX32_PER_0p25MS) + 2);
  /* Divide by 4 to get milliseconds ticks (rounded via +2, above) */
  qw_shift( ts_val, TIME_0p25MS_TO_1MS_SHIFT);
}

static void time_tod_set(qword tt)
{
   time_to_ms(tt);
   clk_set_ms(tt);
}
#endif /* #if (defined _WIN32 || defined FEATURE_WCDMA || defined FEATURE_GSM) */

#endif /* defined(FEATURE_USE_TIME_VU) */
#include "nv.h"

#ifdef FEATURE_PMIC_RTC
#include "clkrtc.h"
#endif

#include "OEMFeatures.h"
#include "OEMConfig.h"
#include "OEMSecSysClock.h"

#include "AEESysClock.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEError.h"
#include "AEE_OEM.h"

#include "AEESecSysClock.bid"
#include "AEETelephone.h"

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

typedef struct _CSecSysClockCommon {

   IShell         *piShell;          // Pointer to the shell
   uint32         dwRefs;            // Reference count for this object

   AEECallback    *pcbHead;          // Callback for users to register, to 
                                     // be triggered when secure time changes
   AEECallback    cbOnExit;          // Exit callback for clean-up
   boolean        bValidity;          // Validity of Secure Clock

#if !(defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
   PhoneListener  PhListenerServiceAvail;
   IModel *pModel;
#endif
} CSecSysClockCommon;


typedef struct _CSecSysClock {
  const AEEVTBL(ISysClock) *pvt;         // Inherited from ISysClock interface

  CSecSysClockCommon *pSecSysClkCommon;  // Common Secure SysClock Object data
} CSecSysClock;

/*=========================================================================
                       Imported Function Prototypes
=========================================================================*/

extern sys_sys_mode_e_type OEM_SysMode (void);

/*=========================================================================
                       Exported Function Prototypes
=========================================================================*/

static void CBCancelGeneric(AEECallback *pcb);

static void SecSysClock_DoCallbacks(CSecSysClockCommon *meCommon);

int SecSysClock_New(IShell *piShell, AEECLSID cls, void **ppif);

/*=========================================================================
                        Global Data Definitions
=========================================================================*/

/*=========================================================================
                       Static variable Definitions
=========================================================================*/

static CSecSysClockCommon *gpSecSysClkCommon = NULL;   // Common Object data
static CSecSysClock *gpSecSysClockSet = NULL;    // Settable SecSysClock  
static CSecSysClock *gpSecSysClockNoSet = NULL;  // Un-settable SecSysClock 

extern const AEEStaticClass gAEESecSysClockClasses[] = {
   {AEECLSID_SECSYSCLOCK, ASCF_PRIV, 0, 0, SecSysClock_New},
   {0, 0, 0, NULL, NULL} 
};

/*==========================================================================
                      SECURE CLOCK FUNCTION DEFINITIONS
============================================================================

FUNCTION:  CSecSysClock_AddRef()

DESCRIPTION:
   This function increments the reference count of the ISecureClock object.
   This allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero). 
   
PARAMETERS:
   
RETURN: 

===========================================================================*/
static uint32 CSecSysClock_AddRef(ISysClock *po)
{
   return(++((CSecSysClock *)po)->pSecSysClkCommon->dwRefs);
}


static void SecSysClock_Destroy(CSecSysClockCommon *meCommon)
{
   if (meCommon) {
      CALLBACK_Cancel(&meCommon->cbOnExit);
#if !(defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)    
      LISTENER_Cancel(&meCommon->PhListenerServiceAvail);
      RELEASEIF(meCommon->pModel);
#endif

      if (meCommon->pcbHead) {
         meCommon->pcbHead->pfnCancel = NULL;
         meCommon->pcbHead = NULL;
      }
 
      RELEASEIF(meCommon->piShell);
   }

   FREEIF(gpSecSysClkCommon);
   FREEIF(gpSecSysClockSet);
   FREEIF(gpSecSysClockNoSet);
}

/*===========================================================================

FUNCTION:  CSecSysClock_Release()

DESCRIPTION:
   This function decrements the reference count of the CSecSysClock object.
   This allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero). 
   
PARAMETERS:
   
Return Value: 

===========================================================================*/
static uint32 CSecSysClock_Release(ISysClock *po)
{
   CSecSysClock *me = (CSecSysClock *)po;

   uint32 dwRefs = --me->pSecSysClkCommon->dwRefs;

   if (0 == dwRefs) {      
      SecSysClock_Destroy(me->pSecSysClkCommon);
   }

   return dwRefs;
}


static int CSecSysClock_QueryInterface(ISysClock *po, AEECLSID cls, void **ppo)
{
   switch (cls) {
      case AEECLSID_QUERYINTERFACE:
      case AEEIID_SYSCLOCK:
      case AEECLSID_SECSYSCLOCK:
         *ppo = (void *)po;
         CSecSysClock_AddRef(po);
         return SUCCESS;
      default:
         *ppo = NULL;
         return ECLASSNOTSUPPORT;
   }
}


/*==========================================================================
FUNCTION: CSecSysClock_GetTimeUS()

DESCRIPTION: 
   Returns the value of secure clock (TOD clock) in micro-seconds 
   from the start of GPS epoch
  
==========================================================================*/
static int CSecSysClock_GetTimeUS(ISysClock *po, uint64struct *pstUS)
{
   CSecSysClock *me = (CSecSysClock *)po;
   qword  qwTS;
   uint64 qwUSecTime;

   #ifdef FEATURE_SEC_TIME_V2

   // Read TOD Clock in milli-seconds
   time_secure_get_secure_time_ms(qwTS);

   #else

   // Read TOD Clock in milli-seconds
   time_get_ms(qwTS);

   #endif

   // Convert qword to uint64 and time to micro-seconds 
   qwUSecTime  = QW_CVT_Q2N (qwTS) * 1000;
   
   #if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
   // Add leap seconds to convert to UTC time
   if (me->pSecSysClkCommon->bValidity) {
       nv_item_type nvi;
       
       // For Leap seconds
       if(OEMNV_Get(NV_DB_LP_SEC_I, &nvi) ==  NV_DONE_S){
          if(nvi.db_lp_sec){
             qwUSecTime -= (nvi.db_lp_sec * 1000000);
          }
       }
   }
   #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

   // if pstUS is NULL only the validity of the clock is returned
   if (pstUS) {
      *pstUS = uint64struct_from_uint64(qwUSecTime);
   }

   // For CDMA bValidity is automatically set when service is acquired
   // Return SUCCESS if time is valid, EBADSTATE if it is not valid
   if (me->pSecSysClkCommon->bValidity) {
      return SUCCESS;
   }
   else {
      return EBADSTATE;
   }
}

#if !(defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
static void SecSysClock_ServiceAvail(CSecSysClockCommon *meCommon, ModelEvent *pEvent)
{
   AEETSSEvent *pSSevt = (AEETSSEvent *)pEvent;

   if(pSSevt ->evCode == AEET_EVENT_SS_SRV_CHANGED) {
      if ((pSSevt->ss.ss_info.srv_status == AEET_SRV_STATUS_SRV) ||
          (pSSevt->ss.ss_info.srv_status == AEET_SRV_STATUS_LIMITED) ||
          (pSSevt->ss.ss_info.srv_status == AEET_SRV_STATUS_LIMITED_REGIONAL)) {

          LISTENER_Cancel(&meCommon->PhListenerServiceAvail);

          meCommon->bValidity = TRUE;
           
          // Fire the registered IsValid Callbacks once we have acquired service
          SecSysClock_DoCallbacks(meCommon);
      }
   }
}
#endif

/*============================================================================
FUNCTION: CSecSysClock_SetTimeUS

DESCRIPTION: 
   This function should only be called from a trusted sub-system like MA. 
   The function sets the secure clock to the input value in microseconds. 
   It also updates the TOD and RTC (if available). This also updates the last 
   valid time(in microseconds) information (LastValidCfg *) stored in 
   non-volatile storage. The Caller must be either in system context or must be 
   a member of the group class ID: AEECLSID_SECSYSCLOCK, to be able to adjust
   the secure clock successfully
=============================================================================*/
static int CSecSysClock_SetTimeUS(ISysClock *po, uint64struct stUS)
{
#if (defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
   {
      // don't check for mode in the BREW Simulator and set time anyway
  #if !(defined AEE_SIMULATOR)       
      if (OEM_SysMode() != SYS_SYS_MODE_CDMA) {
  #endif
     #ifndef FEATURE_SEC_TIME_V2
         uint64 qwMSTime;
     #endif
         CSecSysClock *me = (CSecSysClock *)po;
         AEECLSID callerCLSID = AEE_GetAppContextCls(AEE_GetAppContext());
         uint64 qwUS;

         // Convert from "uint64struct" to uint64
         qwUS = uint64struct_to_uint64(stUS);
         
         #ifdef FEATURE_SEC_TIME_V2
         {
            uint32 dwS = (uint32)(qwUS/1000000); // Convert input time to seconds

            //Set the clock
            time_secure_set_secure_time_secs(dwS);
         }
         #else
         {
            uint64struct stTime;
            qword qwTS;
            uint64 qwUSecTime=0;
 
         (void) CSecSysClock_GetTimeUS(po, &stTime);
         
         // Convert from "uint64struct" to uint64
         qwUSecTime = uint64struct_to_uint64(stTime);

         // Once the secure clock is set the user offset (if set) is adjusted
         // so that the user time stays the same
         {
            int64 nOffset = (int64)(qwUS - qwUSecTime);
            int32 nUserOffset=0;
            int nErr;

            nErr = OEM_GetConfig(CFGI_USER_CLOCK_OFFSET, &nUserOffset, sizeof(int32));

            if (SUCCESS == nErr) {
               if (nUserOffset) {
                  // Convert to seconds since the User clock offset is in secs
                  // New Offset = Old Offset + Old time - New time
                  nUserOffset -= (int32)(nOffset / 1000000); 
                  OEM_SetConfig(CFGI_USER_CLOCK_OFFSET, &nUserOffset, sizeof(int32));
               }
            }
         }
 
         // Convert input time to milli-seconds
         qwMSTime = (uint64) (qwUS/1000);

         /* convert from micro-seconds to time_type */
         {
            uint64 qwMS;

            /* convert input time to 1.25 ms ticks */
            qwMS = qwUS / 1250;

            /* transform into a time stamp - see time.c for format description */
            qwMS = qwMS << 16;

            /* convert a 'native 64-bit type (uint64)' to a 'qword' */
            MEMCPY(qwTS, &qwMS, sizeof(qword));
         }

         /* set the TOD clock */
         time_tod_set(qwTS);
         }
         #endif

         if (qwUS > ((uint64)ONE_YEAR_SECS * 10000000)) {
            // Update the last valid time in non-volatile storage
            // only if time is valid. Update the Validity also accordingly
             int nErr; 
             LastValidCfg lastvalid;
      
             nErr = OEM_GetConfig(CFGI_LAST_VALID_TIME, &lastvalid, sizeof(LastValidCfg));
   
             if (SUCCESS == nErr) {
                if (qwUS >= lastvalid.qwTime) {
                    me->pSecSysClkCommon->bValidity = TRUE;
                    // Update the Last Valid Time
                    lastvalid.qwTime   = qwUS;
                    lastvalid.OwnerCls = callerCLSID;
                    OEM_SetConfig(CFGI_LAST_VALID_TIME, &lastvalid, sizeof(LastValidCfg)); 
                }
                else {
                    me->pSecSysClkCommon->bValidity = FALSE;
                }
             }
             else {
                me->pSecSysClkCommon->bValidity = FALSE;
             }
         }
         else {
           me->pSecSysClkCommon->bValidity = FALSE;
         }
      
      #ifndef FEATURE_SEC_TIME_V2

         // Set the RTC if it's available
      #ifdef FEATURE_PMIC_RTC
         {
            clk_julian_type julian_time;
            uint32 dwSecs = (uint32) (qwMSTime/1000);
            time_jul_from_secs(dwSecs,&julian_time);
            clk_rtc_set_time(&julian_time);
         }
      #endif // FEATURE_PMIC_RTC

      #endif // FEATURE_SEC_TIME_V2

          // Secure time is set hence fire the Time Change callbacks 
         SecSysClock_DoCallbacks(me->pSecSysClkCommon);

         return SUCCESS;
   #if !(defined AEE_SIMULATOR)
      }
      return EUNSUPPORTED;
   #endif
   }
#endif 
   // Not in GSM or WCDMA mode so do not allow adjusting the clock 
   return EUNSUPPORTED;
   
}

static int CSecSysClock_NoSetTimeUS(ISysClock *po, uint64struct stUS)
{
   // Dummy function to be called by the class with no set priveleges..
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

static void SecSysClock_DoCallbacks(CSecSysClockCommon *meCommon)
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
FUNCTION: CSecSysClock_RegisterOnChangeCB

DESCRIPTION: 
  Registers more than one callback (per object) to be called when secure time
  validity changes. 
==========================================================================*/

static int CSecSysClock_RegisterOnChangeCB(ISysClock *po, AEECallback *pcb)
{
   CSecSysClock *me = (CSecSysClock *)po;
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
   pcbLinked->pNext = me->pSecSysClkCommon->pcbHead;
   me->pSecSysClkCommon->pcbHead = pcbLinked;
   pcbLinked->pfnCancel = CBCancelGeneric;
   pcbLinked->pCancelData = &(me->pSecSysClkCommon->pcbHead);
   if (pcbLinked->pNext) {
      // Current node's pCancelData -> Previous node's pNext pointer
      pcbLinked->pNext->pCancelData = &(pcbLinked->pNext);
   }

   return SUCCESS;
}


static int SecSysClock_InitSequence(CSecSysClockCommon *meCommon)
{
   int nErr = SUCCESS;
   qword qwTS;

#if (defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
/* Initialisation sequence for UMTS devices and/or devices using RTC.
 * Secure Clock gets initialized during AEE_Init. If the RTC 
 * time is greater than the last valid time stored in non-volatile 
 * storage, this function updates the last valid time storage and sets 
 * the TOD value with the RTC's current value. The last valid time 
 * information is stored in a config item CFGI_LAST_VALID_TIME as a  
 * LastValidCfg* - this has the last valid time as well as the caller 
 * who set the secure time last. 
 */
   LastValidCfg lastvalid;
   uint64 qwUSecTime=0;
   uint64 qwSysTime=0;

 #ifdef FEATURE_PMIC_RTC
   clk_julian_type julian_time;

   //Read time from the RTC Clock, if available
   clk_rtc_get_time(&julian_time);
   qwSysTime = (uint64) clk_julian_to_secs(&julian_time);
   qwSysTime *= 1000;               // Time in milliseconds
 #else // for (AEE_SIMULATOR)
   time_get_ms(qwTS);
   qwSysTime = QW_CVT_Q2N(qwTS);    // Time in milliseconds
 #endif // ifdef FEATURE_PMIC_RTC

   qwUSecTime = qwSysTime * 1000;   // Time in microseconds
 
   nErr = OEM_GetConfig(CFGI_LAST_VALID_TIME, &lastvalid, sizeof(LastValidCfg));

   if (SUCCESS == nErr) {
      // last valid time is stored in micro-seconds
      if (qwUSecTime > lastvalid.qwTime) {
         // If real-time clock is valid update the last valid time with this value
         lastvalid.qwTime = qwUSecTime;
         lastvalid.OwnerCls = AEE_GetAppContextCls(AEE_GetAppContext());

         OEM_SetConfig(CFGI_LAST_VALID_TIME, &lastvalid, sizeof(LastValidCfg));
      #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
        
         /* convert from milli-seconds to time_type & set the TOD clock*/
         {
            uint64 qwMS;

            /* convert input time to 1.25 ms ticks */
            qwMS = (qwSysTime * 1000) / 1250;

            /* transform into a time stamp - see time.c for format description */
            qwMS = qwMS << 16;

            /* convert a 'native 64-bit type (uint64)' to a 'qword' */
            MEMCPY(qwTS, &qwMS, sizeof(qword));

            /* set the TOD clock */
            time_tod_set(qwTS);
         }

      #endif
         meCommon->bValidity = TRUE;
      }
   }
  #else //#if (defined FEATURE_CDMA_800) || (defined FEATURE_CDMA_1900)
   // Step for deciding the Validity of Secure Clock in CDMA mode.
   uint32 dwSecs;

   // Read the CDMA time/time-of-day clock in milli-seconds
   time_get_ms(qwTS);         
   dwSecs = (uint32) (QW_CVT_Q2N(qwTS) / 1000); //Convert to secs

   if (dwSecs > ONE_YEAR_SECS) {
      meCommon->bValidity = TRUE;
   }
   else {
      // May be we have service but the clock is not sync'ed with the
      // network time when we queried it. So query ITelephone about the
      // current serving system info 
      ITelephone *pTelephone = NULL;
      AEETSSInfo *pSSInfo = NULL;

      pSSInfo = (AEETSSInfo *)MALLOC(sizeof(AEETSSInfo));

      if (NULL == pSSInfo) {
         return ENOMEMORY;
      }

      nErr = ISHELL_CreateInstance(meCommon->piShell, AEECLSID_TELEPHONE, (void **) &pTelephone);

      if (SUCCESS == nErr) {
         (void)ITELEPHONE_GetServingSystemInfo(pTelephone, pSSInfo, sizeof(AEETSSInfo));

         if (pSSInfo->srv_status == AEET_SRV_STATUS_NO_SRV) {
         // Service not available and time is not valid. Add a listener to 
         // ITelephone to be notified when Serving System Info changes i.e when 
         // device acquires service
                  
            LISTENER_Init((ModelListener *)&meCommon->PhListenerServiceAvail, SecSysClock_ServiceAvail, meCommon);
            nErr = ITELEPHONE_QueryInterface(pTelephone, AEEIID_MODEL, (void **) &meCommon->pModel); 
            if (SUCCESS == nErr ) {
               IMODEL_AddListener(meCommon->pModel,(ModelListener *) &meCommon->PhListenerServiceAvail);
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
	  else {

         // Don't return a failure if ITelephone cannot be created. 
		 // Otherwise, clock functions will not work. 
		 nErr = 0; 
	  }
     FREEIF(pSSInfo);
   }
#endif // #if (defined AEE_SIMULATOR || defined FEATURE_WCDMA || defined FEATURE_GSM)
   return nErr;
}

/*=========================================================================

FUNCTION:  CSecSysClock_New()

DESCRIPTION:
   This is called from the shell when an app tries to create an instance of
   a class ID associated with IClock module.

PARAMETERS:
   IShell* piShell [In]:   IShell pointer
   AEECLSID cls [In]:     Class to create
   void **ppif [Out]:    Pointer to the created interface object

RETURN:  Status indicating success of failure for create instance

=========================================================================*/
int SecSysClock_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   CSecSysClockCommon *meCommon = NULL;
   CSecSysClock *meSet   = NULL;
   CSecSysClock *meNoSet = NULL;

   static const VTBL(ISysClock) CSecSysClockSetMethods = {CSecSysClock_AddRef,
                                                       CSecSysClock_Release,
                                                       CSecSysClock_QueryInterface,
                                                       CSecSysClock_GetTimeUS,
                                                       CSecSysClock_SetTimeUS,
                                                       CSecSysClock_RegisterOnChangeCB};

  static const VTBL(ISysClock) CSecSysClockNoSetMethods = {CSecSysClock_AddRef,
                                                        CSecSysClock_Release,
                                                        CSecSysClock_QueryInterface,
                                                        CSecSysClock_GetTimeUS,
                                                        CSecSysClock_NoSetTimeUS,
                                                        CSecSysClock_RegisterOnChangeCB};


   if (AEECLSID_SECSYSCLOCK != cls) {
      return ECLASSNOTSUPPORT;
   }

   // Requires AEECLSID_SECSYSCLOCK priv to be able to create this class
   if (!ISHELL_CheckPrivLevel(piShell, AEECLSID_SECSYSCLOCK, TRUE)) {
      // Does not have privileges to adjust the secure clock
      return EPRIVLEVEL;
   }
   
   if (NULL == gpSecSysClkCommon) {
      PACONTEXT pacLast;
 
      pacLast = AEE_EnterAppContext(NULL);    
      meCommon = MALLOCREC(CSecSysClockCommon);
      meSet   =  MALLOCREC(CSecSysClock);
      meNoSet =  MALLOCREC(CSecSysClock);
      AEE_LeaveAppContext(pacLast);

      if ((NULL == meCommon) || (NULL == meSet) || (NULL == meNoSet)){
         return ENOMEMORY;
      }
   
      MEMSET(meCommon, 0, sizeof(CSecSysClockCommon));
      MEMSET(meSet, 0, sizeof(CSecSysClock));
      MEMSET(meNoSet, 0, sizeof(CSecSysClock));

      meSet->pvt   = &CSecSysClockSetMethods;   // Vtbl for Settable Class
      meNoSet->pvt = &CSecSysClockNoSetMethods; // Vtbl for non-Settable Class

      meCommon->piShell = piShell;
      ISHELL_AddRef(piShell);

      // Beginning of Secure SysClock Initialization sequence. This sets the 
      // last time validity of secure clock on both CDMA and UMTS devices
      {
         int nErr;
         nErr = SecSysClock_InitSequence(meCommon);

         if (SUCCESS != nErr) {
            RELEASEIF(meCommon->piShell);
            FREE(meCommon);
            gpSecSysClkCommon  = NULL;
            gpSecSysClockSet   = NULL;
            gpSecSysClockNoSet = NULL;
            *ppif = NULL;
            return nErr;
         }
      }

      // Callback to clean up memory associated with this object when BREW exits.
      CALLBACK_Init(&meCommon->cbOnExit, (PFNNOTIFY)SecSysClock_Destroy, (void*)meCommon);
      ISHELL_OnExit(piShell, &meCommon->cbOnExit);

      gpSecSysClkCommon  = meCommon;
      gpSecSysClockSet   = meSet;
      gpSecSysClockNoSet = meNoSet;

      gpSecSysClockSet->pSecSysClkCommon   = gpSecSysClkCommon;
      gpSecSysClockNoSet->pSecSysClkCommon = gpSecSysClkCommon;
   }

   if (ISHELL_CheckPrivLevel(piShell, AEEPRIVID_SECSYSCLOCK_SET, TRUE)) {
      // Has privileges to set the user clock. Increase the reference count of
      // the corresponding object
      CSecSysClock_AddRef((ISysClock *)gpSecSysClockSet);
      *ppif = gpSecSysClockSet;

   }
   else {
      // Does not have privileges to set the user clock. Increase the reference
      // count of the corresponding object
      CSecSysClock_AddRef((ISysClock *)gpSecSysClockNoSet);
      *ppif = gpSecSysClockNoSet;
   }

   return SUCCESS;
}

