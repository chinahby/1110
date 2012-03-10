/*=============================================================================

FILE: OEMOS.c
  
SERVICES: OEM Functions for OEM Operating System
    
GENERAL DESCRIPTION:
   A sample implementation of the OEMOS functions declared in
   OEMOS.h that are used in the implementation of AEEFile.
      
PUBLIC CLASSES AND STATIC FUNCTIONS:
        
   OEMOS_GetUpTime
   OEMOS_GetLocalTimeMS
   OEMOS_SetTimer
   OEMOS_Sleep
   OEMOS_SignalDispatch
   OEMOS_CancelDispatch
   OEMOS_BrewHighPriority
   OEMOS_BrewNormalPriority
   OEMOS_ActiveTaskID
   OEMOS_IntLock
   OEMOS_IntFree
          
INITIALIZATION AND SEQUENCING REQUIREMENTS:
   No additional module-level initialization or sequencing requirements.
            
        Copyright © 1999-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#ifdef AEE_SIMULATOR
#include <windows.h>
#endif

#include <stdlib.h>

#if !defined(AEE_SIMULATOR)
# include "customer.h"
# include "OEMNV.h" 
#endif

#include "OEMFeatures.h"

#if !defined(AEE_SIMULATOR)
# include "ui.h"
# include "dog.h"
# include "clki.h"
#include "nv.h"

# if defined(FEATURE_USE_TIME_VU)
#  include "timetick.h"
#  include "time_svc.h"
# endif /* #if defined(FEATURE_USE_TIME_VU) */
  
# endif /* #if !defined(AEE_SIMULATOR) */

#include "rex.h"
#include "qw.h"
#include "clk.h"

#include "err.h"

#include "OEMOS.h"
#include "AEEComdef.h"
#include "AEEError.h"
#include "AEEOS.h"
#include "AEE_OEM.h"

#if defined(FEATURE_PMIC_RTC)
#include "clkrtc.h"
#endif

#ifdef FEATURE_IALARM 
#include "OEMAlarm_priv.h"
#endif

#include "sys.h"

#include "AEETelephone.h"

static PhoneListener   gPhoneListenerOEMReset;

#if !defined AEE_SIMULATOR
extern sys_sys_mode_e_type sd_misc_get_curr_acq_sys_mode(void);
#endif

sys_sys_mode_e_type OEM_SysMode (void);

/*=================================================================
** LOCAL DEFINES
**===============================================================*/
#define BREW_HI_PRIORITY (900)

#ifdef AEE_SIMULATOR

# define AEE_APP_SIG         1
# define AEE_SLEEP_SIG       2
# define AEE_DOG_BIT         1
# define dog_report(x)       0

#else /* #ifdef AEE_SIMULATOR */

# ifndef AEE_APP_SIG
#  define AEE_APP_SIG  UI_APP_SIG
# endif /* # ifndef AEE_APP_SIG */

# ifndef AEE_DOG_BIT
#  define AEE_DOG_BIT  DOG_UI_RPT
# endif  /* #ifndef AEE_DOG_BIT */

#endif /* #ifdef AEE_SIMULATOR */


/*=================================================================
** FILE SCOPED VARIABLES
**===============================================================*/
static rex_tcb_type      *gptcbAEE;
static rex_priority_type  normal_priority;

static clk_cb_type OEMOS_timer = {0};
#ifdef FEATURE_MFLO
static timer_group_type OEMOS_callback_group;
#endif

/*==================================================================
Function: OEMOS_InitLayer

  Description: Gets started after a call to AEE_Init to allow OEM
  layer code to do anything required to provide services.
  Returns 0 for a successful initialization or any other 
  value to indicate error, in which case AEE will not be
  started.
==================================================================*/
int16  OEMOS_InitLayer() 
{
   gptcbAEE = rex_self();


   normal_priority = rex_get_pri();
   SETSTACKBOTTOM(gptcbAEE->stack_limit);

#ifdef FEATURE_IALARM    
  
   if (OEMAlarm_RegisterISR() != AEE_SUCCESS) {
      DBGPRINTF("OEMAlarm_RegisterISR() failed.");
   }
#endif
   
#if !defined(AEE_SIMULATOR)
#ifdef FEATURE_MFLO
   timer_def2(&OEMOS_timer, &OEMOS_callback_group);
#else
   clk_def(&OEMOS_timer);
#endif
#endif

#if defined(FEATURE_BREW_HEAP_TRACKER) || \
    defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || \
    defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE) || \
    defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)
   {
     // The first time AEEkHeap_Realloc() gets called, we check the NV item
     // that governs whether the heap tracker is enabled.  We do this in
     // AEEkHeap_Realloc() instead of in AEEkHeap_Init(), because
     // AEEkHeap_Init() is called before the NV task is initialized.  Because
     // of this, we suffer the minor inefficiency of having to check a boolean
     // flag each time AEEkHeap_Realloc() is called.

     extern nv_item_type heaptracker_nv_data;

     if (OEMNV_Get(NV_HEAPTRACKER_ENABLED_I, &heaptracker_nv_data) == NV_DONE_S)
     {
#ifdef FEATURE_BREW_HEAP_TRACKER
       {
     extern boolean ht_enabled;
         ht_enabled = (heaptracker_nv_data.heaptracker_enabled & (1<<0)) ? TRUE : FALSE;
       }
#endif//FEATURE_BREW_HEAP_TRACKER
#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
       {
         extern boolean df_enabled;
         df_enabled = (heaptracker_nv_data.heaptracker_enabled & (1<<1)) ? TRUE : FALSE;
       }
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
#ifdef FEATURE_BREW_HEAP_INIT_MEM_ON_FREE
       {
         extern boolean zf_enabled;
         zf_enabled = (heaptracker_nv_data.heaptracker_enabled & (1<<2)) ? TRUE : FALSE;
       }
#endif//FEATURE_BREW_HEAP_INIT_MEM_ON_FREE
#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
       {
         extern boolean od_enabled;
         od_enabled = (heaptracker_nv_data.heaptracker_enabled & (1<<3)) ? TRUE : FALSE;
       }
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT
     }
     else
     {
#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
       extern boolean od_enabled;
       od_enabled = FALSE;
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT
       heaptracker_nv_data.heaptracker_enabled = 0;
     }
   }
#endif//defined(FEATURE_BREW_HEAP_TRACKER) ||
      //defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) ||
      //defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE) ||
      //defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

   return AEE_SUCCESS;
}

/*==================================================================
Function: OEMOS_GetUpTime

  Description: Returns time in ms since the device started.
==================================================================*/
uint32 OEMOS_GetUpTime()
{
#if defined(AEE_SIMULATOR)
   
   /* simplest implementation, should work on any version of DMSS,
      but can be *very* slow, and on TIMETICK targets, may jitter */
   
   qword ts;
   
   clk_uptime_ms(ts);
   
   return qw_lo(ts);

#elif defined(FEATURE_USE_TIME_VU)

   /* timetick is fast and is defined to increase at linearly 
      and continuously until rollover */
   
   return timetick_get_ms();

#else /* !defined(FEATURE_USE_TIME_VU) && !defined(AEE_SIMULATOR) */

  /* read the clock out of global clk_uptime_val, super fast */

   uint32 secs,ms;
   volatile clk_uptime_type *p_clk_uptime_val;
   
   p_clk_uptime_val = &clk_uptime_val;

   /* loop obviates need for INTLOCK */
   do {
      secs = p_clk_uptime_val->secs;
      ms   = p_clk_uptime_val->ms;
   } while (secs != p_clk_uptime_val->secs);

   return (secs * 1000) + ms;
#endif
}


/*==================================================================
Function: OEMOS_JulianToSeconds

  Description: Returns seconds equivalent of specified julian date
==================================================================*/
uint32  OEMOS_JulianToSeconds(JulianType* pDate) 
{
   clk_julian_type ts;
   
   ts.day = pDate->wDay;
   ts.day_of_week = pDate->wWeekDay;
   ts.hour = pDate->wHour;
   ts.minute = pDate->wMinute;
   ts.second = pDate->wSecond;
   ts.month = pDate->wMonth;
   ts.year = pDate->wYear;
   
   return (clk_julian_to_secs(&ts));
}
/*==================================================================
Function: OEMOS_SetTimer

  Description: Sets a master timer for BREW libraries.  Should
  supercede any previous calls to OEMOS_SetTimer.
  A call to OEMOS_SetTimer with a value of zero shall
  cancel any previously set timer.  When the timer set
  expires, AEE_Dispatch should be called.
  
  OEMOS_SetTimer() schedules a ONE-SHOT timer

==================================================================*/
void OEMOS_SetTimer(uint32 nMSecs) 
{
   // Deregister any pending timer and reset the timer for the required 
   // number of milliseconds...
   clk_dereg(&OEMOS_timer);

   if (0 != nMSecs) {
      clk_reg(&OEMOS_timer, 
              (void (*)(int4))OEMOS_SignalDispatch, nMSecs, 0, FALSE );
   }

}

/*==================================================================
Function: OEMOS_Sleep

  Description: Delays execution of subsequent code for nMsecs.
==================================================================*/
void OEMOS_Sleep(uint32 nMSecs) // period for timer in milliseconds
{
   rex_sigs_type   sigs = 0;
   rex_timer_type  timer;

   if (rex_self() != gptcbAEE) {
      DBGPRINTF("OEMOS_Sleep() called from an unknown task");
      return;
   }

   // initialize a timer
   rex_def_timer(&timer, gptcbAEE, AEE_SLEEP_SIG);

   while (0 != nMSecs) {
      uint32 nInc;

      // kick the dog
      dog_report(AEE_DOG_BIT);

      // Save and clear all our sigs
      sigs |= rex_clr_sigs(gptcbAEE,(rex_sigs_type)-1);
 
      // kick the dog every 10 seconds...
      nInc = MIN(10000, nMSecs);

      rex_clr_timer(&timer);
      rex_timed_wait(AEE_SLEEP_SIG, &timer, nInc);
      rex_clr_sigs(gptcbAEE, AEE_SLEEP_SIG);

      nMSecs -= nInc;
   }

   //Clear the timer explicitly because REX doesn't seem to do it
   rex_clr_timer(&timer);

   // Restore the sigs...
   if (sigs) {
      rex_set_sigs(gptcbAEE, sigs);
   }

}

/*==================================================================
Function: OEMOS_SignalDispatch

  Description: Signals the OEM layer to call AEE_Dispatch from a
  standard context (ie non-ISR or async signal) at the
  next available time.
==================================================================*/
void OEMOS_SignalDispatch(void)
{
   rex_set_sigs(gptcbAEE, AEE_APP_SIG);
}

/*=================================================================
Function: OEMOS_CancelDispatch

  Description: Cancel any pending dispatch called by
  OEMOS_SignalDispatch().  If there is no pending
  dispatch operation, this function should do nothing.
=================================================================*/
void OEMOS_CancelDispatch() 
{
   rex_clr_sigs(gptcbAEE, AEE_APP_SIG);
}

/*================================================================
Function: OEMOS_BrewHighPriority

  Description: Sets BREW to be a high priority task.  This means
  that any work BREW has to do will be executed before
  most of the other tasks.   Which tasks BREW should
  be higher or lower than is OEM-dependant.
================================================================*/
void OEMOS_BrewHighPriority() 
{
#ifndef AEE_SIMULATOR
   rex_priority_type nPri;
   nPri = rex_set_pri(BREW_HI_PRIORITY);
   if (nPri != BREW_HI_PRIORITY) {
      normal_priority = nPri;
   }
#endif /* AEE_SIMULATOR */
}

/*================================================================
Function: OEMOS_BrewNormalPriority

  Description: Returns BREW's task to its original priority value
  before OEMOS_BrewHighPriority was called.
================================================================*/
void OEMOS_BrewNormalPriority() 
{
#ifndef AEE_SIMULATOR
   rex_set_pri(normal_priority);
#endif /* AEE_SIMULATOR */
}


sys_sys_mode_e_type OEM_SysMode (void)
{
#if !defined AEE_SIMULATOR
   /* GSM/1x Multimode targets */
   sys_sys_mode_e_type sys_mode = sd_misc_get_curr_acq_sys_mode();

   if( sys_mode == SYS_SYS_MODE_GSM || sys_mode == SYS_SYS_MODE_WCDMA 
                                            || sys_mode == SYS_SYS_MODE_GW)
   {
      return SYS_SYS_MODE_GSM;
   }
   else
      return SYS_SYS_MODE_CDMA;
#else
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#if !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA)
   return SYS_SYS_MODE_CDMA;
#endif // !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA)
#else
   return SYS_SYS_MODE_GSM;
#endif // defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#endif // !defined AEE_SIMULATOR
}

/*===============================================================
Function: OEMOS_ActiveTaskID

  Description: Returns a unique identifier to the currently running
  task.  This call is used so that BREW can determine
  it's own task ID as well as conditions where it may
  be called from another task.
===============================================================*/
uint32 OEMOS_ActiveTaskID ()
{
   return (uint32)rex_self();
}        

/*==============================================================
Function: OEMOS_LocalTimeOffset

  Description: Returns the local timezone offset from UTC, in 
  seconds. Optionally returns a flag indicating that 
  daylight savings time is active (if it is, the value 
  of the local timezone offset already takes the shift
  into account; the flag is just for controlling display
  of a timezone name if desired).  The returned value 
  is added to UTC to give local time, or subtracted 
  from local time to give UTC time.
==============================================================*/
int32 OEMOS_LocalTimeOffset 
(
  boolean *bDaylightSavings 
) 
{
   
#ifndef AEE_SIMULATOR

#if defined(FEATURE_MFLO) && \
  (!defined (FEATURE_CDMA_800) && !defined (FEATURE_CDMA_1900))
  {
    extern uint8 mfpt_flo_ltm_off;
    extern boolean mfpt_flo_daylt;
    int32  tzoffset;
    int    ltm_off = (int32)(mfpt_flo_ltm_off & 0x3f);
    if (ltm_off  >= 32) { 
       ltm_off -= 64;
    }

      /* promote timezone offset to seconds from 30 minute units */           

    tzoffset = 60 * 30 * ( ltm_off ); /*lint !e737 */             

    if (bDaylightSavings) {
      //Get daylight savings time flag
      *bDaylightSavings = mfpt_flo_daylt;
    }
    return (tzoffset);
  }
#endif /* FEATURE_MFLO */

  {
    nv_item_type nvi;
    int32 tzoffset = 0;

    // For local time offset
    if(OEMNV_Get(NV_DB_LTM_OFF_I, &nvi) ==  NV_DONE_S){
      int32 ltm_off = 0;
      /* The timezone offset stored in NV_DB_LTM_OFF_I item is 15 min
       * unit stamp & encoded as 8-bit 2's complement. Make it signed
       * integer */
      ltm_off = (int32)(nvi.db_ltm_off & 0xFF);
      if(ltm_off >= 128){
        ltm_off -= 256;
      }

      /* time zone offset is in 15 minutes unit stamp */
      tzoffset = ltm_off * 15 * 60;
    }

    if(NULL != bDaylightSavings) {
      // For Daylight savings time indicator.
      if(OEMNV_Get(NV_DB_DAYLT_I, &nvi) ==  NV_DONE_S){
        *bDaylightSavings = (boolean)nvi.db_daylt;
      }
    }

    return (tzoffset);
  }

#else //AEE_SIMULATOR
   
   TIME_ZONE_INFORMATION   lpTimeZone;
   
   if(GetTimeZoneInformation(&lpTimeZone) == TIME_ZONE_ID_DAYLIGHT){
      if(bDaylightSavings){
         *bDaylightSavings = TRUE;
      }
      return(-60 * (lpTimeZone.Bias + lpTimeZone.DaylightBias));
   }
   
   if(bDaylightSavings){
      *bDaylightSavings = FALSE;
   }
   
   //Need to convert to seconds. Also, BREW ret value is reverse of Windows' 
   //return value. So, negate it,
   return(-60 * lpTimeZone.Bias);
   
#endif
}

void OEMOS_ResetDevice2(void *pUserData, ModelEvent *pEvent)
{
   IPhoneCtl *pIPhoneCtl = (IPhoneCtl *)pUserData;
   AEETPhoneEvent * pPhoneEvent = (AEETPhoneEvent *) pEvent;
   
   if(pPhoneEvent->evCode == AEET_EVENT_PH_OPRT_MODE) {


      if((pPhoneEvent->ph.oprt_mode == AEET_OPRT_MODE_OFFLINE) ||
         (pPhoneEvent->ph.oprt_mode == AEET_OPRT_MODE_OFFLINE_AMPS) ||
         (pPhoneEvent->ph.oprt_mode == AEET_OPRT_MODE_OFFLINE_CDMA)) {

         if(NULL == pIPhoneCtl) {
            IShell * pShell = AEE_GetShell();
            if(NULL == pShell) {
               DBGPRINTF("No Shell");
               return;
            }

            if(SUCCESS != ISHELL_CreateInstance(pShell, AEECLSID_PHONECTL, (void **) &pIPhoneCtl)) {
               DBGPRINTF("Phone Control create failed");
               return;
            }
         }

         LISTENER_Cancel(&gPhoneListenerOEMReset);

         IMODEL_Release(pPhoneEvent->pModel);

         IPHONECTL_SetOperatingMode(pIPhoneCtl, AEET_OPRT_MODE_RESET);

         IPHONECTL_Release(pIPhoneCtl);
      }
      else {
         if(NULL == pIPhoneCtl) {
            IShell * pShell = AEE_GetShell();
            if(NULL == pShell) {
               DBGPRINTF("No Shell");
               return;
            }

            if(SUCCESS != ISHELL_CreateInstance(pShell, AEECLSID_PHONECTL, (void **) &pIPhoneCtl)) {
               DBGPRINTF("Phone Control create failed");
               return;
            }
         }
         IPHONECTL_SetOperatingMode(pIPhoneCtl, AEET_OPRT_MODE_OFFLINE);
      }
   }
}


void OEMOS_ResetDevice(char * pszMsg, uint32 nCause)
{
   IShell * pShell = AEE_GetShell();

   IPhoneCtl *pIPhoneCtl = NULL;
   IModel *pModel;
   ITelephone *pTelephone = NULL;

   if(NULL == pShell) {
      DBGPRINTF("No Shell");
      return;
   }

   if(SUCCESS != ISHELL_CreateInstance(pShell, AEECLSID_PHONECTL, (void **) &pIPhoneCtl)) {
      DBGPRINTF("Phone Control create failed");
      return;
   }

   if (pIPhoneCtl == NULL) { /* error */
      DBGPRINTF("Phone Control NULL");
      return;
   }

   LISTENER_Init((ModelListener *)&gPhoneListenerOEMReset, OEMOS_ResetDevice2, (void *) pIPhoneCtl);

   if(SUCCESS != ISHELL_CreateInstance(pShell, AEECLSID_TELEPHONE, (void **) &pTelephone)) {
      IPHONECTL_Release(pIPhoneCtl);
      return;
   }

   if(SUCCESS != ITELEPHONE_QueryInterface(pTelephone, AEEIID_MODEL, (void **) &pModel)) {
      DBGPRINTF("Failed to create model");
      ITELEPHONE_Release(pTelephone);
      IPHONECTL_Release(pIPhoneCtl);
      return;
   }
   else
   {
      ITELEPHONE_Release(pTelephone);

      IMODEL_AddListener(pModel,(ModelListener *) &gPhoneListenerOEMReset);

      if(SUCCESS != IPHONECTL_SetOperatingMode(pIPhoneCtl, AEET_OPRT_MODE_OFFLINE)) {
         DBGPRINTF("Offline failed");
      }
   }
}

#ifdef FEATURE_BREW_HEAP_TRACKER
extern boolean print_allocation(const char *fname, void *ptr);
#endif//FEATURE_BREW_HEAP_TRACKER

void OEMOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize)
{
   // You can ASSERT here for debug builds if you wish...
   // For now this will only print a debug message to show proper usage
   // of the incoming parameters.
   switch(dwType){
   case AEEBRK_MEMLEAK:
   case AEEBRK_IFACELEAK:
      {
         AEEAppLeak * pal = (AEEAppLeak *)pData;

         if( pal && nSize == sizeof(AEEAppLeak) ){
            char szBuff[64];

            *szBuff = 0;
            AEE_GetMemGroupName(pal->dwMemGroup, szBuff, sizeof(szBuff));

        // Add an entry to a backtrace file fs:/shared/memleak.<appname>.bt,
        // where <appname> is the string szBuff.

#ifdef FEATURE_BREW_HEAP_TRACKER
            {
              static char filename[256];
              SPRINTF(filename, "/err/memleak.%s.bt", szBuff);

	      // AEEHeapNode structure is 8 bytes in BREW 3.1.5, but it
	      // is 4 bytes in BREW 3.1.4
#if MIN_BREW_VERSIONEx(3,1,5)
              print_allocation(filename, ((char *)pal->pBuffer) - (sizeof(uint32)*2));
#else
              print_allocation(filename, ((char *)pal->pBuffer) - sizeof(uint32));
#endif
            }
#endif//FEATURE_BREW_HEAP_TRACKER
            DBGPRINTF("BPOINT Type %d, %s 0x%p %s", dwType, 
                        (dwType == AEEBRK_MEMLEAK ? "Node" : "IFace"), 
                        pal->pBuffer, szBuff);
         }
      }
      break;
   case AEEBRK_EXCEPTION:
      {
         AEEExceptionType * pet = (AEEExceptionType *)pData;

         if( pet && nSize ){
            DBGPRINTF("BPOINT Type %d, Exception: %d", dwType, *pet);
         }
      }
      break;
   case AEEBRK_CORRUPTNODE:
      {
#if !defined(PLATFORM_LTK) && defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT)
         AEEAppLeak * pal = (AEEAppLeak *)pData;
         if( pal && nSize == sizeof(AEEAppLeak) )
         {

          // The BREW allocation offset is number of bytes of overhead in each
          // BREW allocation.  Currently BREW reserves 4 bytes for the module
          // ID in each allocation that it requests from the OEM heap.  We need
          // to backtrace by that many bytes before we feed the pointer to the
          // OEM heap again.
          //
          // NOTE:  This value must be changed accordingly if the size of the
          //        BREW heap overhead changes!  Unfortunately, BREW does not
          //        expose that information in a header, so we have to do that
          //        manually.

#define BREW_ALLOC_OVERHEAD (sizeof(uint32))

            extern void report_double_free_if_necessary(char *modname, void *ptr);

#ifdef FEATURE_BREW_HEAP_INIT_MEM_ON_FREE

          // When initialize-on-free is enabled, we will overwrite the module
          // id when freeing an allocation. In this case, calling
          // AEE_GetMemGroupName() is pointless.  In this case, we pass NULL
          // for the module-name pointer, which will instruct function
          // report_double_free_if_necessary() to create a file with the
          // allocation address in its name (e.g., /err/doublefree.0x12345678.bt)
          // instead of with the module name (e.g., // /err/doublefree.MessagingApp.bt)

            extern boolean zf_enabled;
            if (zf_enabled)
            {
               report_double_free_if_necessary(NULL,
                                            ((char *)pal->pBuffer) - BREW_ALLOC_OVERHEAD);
            }
            else
#endif
            {
               static char szBuff[64];
               *szBuff = 0;
               AEE_GetMemGroupName(pal->dwMemGroup, szBuff, sizeof(szBuff));
               report_double_free_if_necessary(szBuff,
                                            ((char *)pal->pBuffer) - BREW_ALLOC_OVERHEAD);
            }
         }
#endif//not PLATFORM_LTK && FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
         DBGPRINTF("BPOINT Type %d, Address: 0x%p", dwType, pData);
         // Determine if you want to walk the heap with AEEkHeap_Walk()
      }
      break;
   default:
      {
         DBGPRINTF("BPOINT Unknown!");
      }
      break;
   }
}

uint32 OEMOS_IntLock( void )
{
   uint32 intSave;

   INTLOCK_SAV(intSave);

   return intSave;
}

void OEMOS_IntFree( uint32 intSav )
{
   INTFREE_SAV(intSav);
}
