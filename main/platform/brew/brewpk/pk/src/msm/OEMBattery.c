/*===========================================================================
                              OEMBattery.c

  OEM REFERENCE BATTERY SUPPORT ROUTINES

  This file contains a reference implementation of Battery support.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*===========================================================================
                      IMPORTED DECLARATIONS
===========================================================================*/
#include "OEMFeatures.h"

#ifdef FEATURE_BREW_BATTERY
#include "comdef.h"
#include "charger.h"
#include "vbatt.h"
#ifdef CUST_EDITION
#include "chg_common.h"
#endif
#endif // FEATURE_BREW_BATTERY

#include "OEMHeap.h"
#include "OEMBattery.h"
#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "AEEBattery.h"
#include "AEETelephone.h"
#include "AEE_OEM.h"

////
// How often to check the battery when
// the phone is in-use or has external power
// (in milliseconds)
#define TIMEOUT_MS_PERIODICBATTCHECK_FAST 1200

////
// How often to check the battery when
// the phone is idle (in milliseconds)
#define TIMEOUT_MS_PERIODICBATTCHECK_SLOW  (60 * 1000)   // one minute


#ifdef FEATURE_BREW_BATTERY
#define BATTERY_LEVEL_WARN                            ((VBATT_SCALE*35)/100)   //modify by mxm
#define BATTERY_LEVEL_POWERDOWN                       ((VBATT_SCALE*15)/100)
#endif // FEATURE_BREW_BATTERY

#ifdef CUST_EDITION
#define BATT_MV_MIN 3200
#define BATT_MV_MAX 4200
#endif

#define VBATT_MEAN_RANGE                              12

/*===========================================================================
                 PRIVATE DATA DECLARATIONS
===========================================================================*/

struct _OEMBatteryMgr
{
   uint32                        dwBatteryStatus;
   uint32                        dwBatteryLevel;
   uint32                        dwChargerStatus;
   uint32                        dwExternalPower;
   uint32                        nRefs;
   AEECallback *                 pcbOnInfoUpdate[AEEBATTERY_ITEM_LAST];

   IModel                       *pTelephoneModel;
   ModelListener                 telephoneListener;
   ITelephone                   *pTelephone;
   boolean                       phoneInUse;
};

/*===========================================================================
                 PRIVATE METHOD DECLARATIONS
===========================================================================*/
static void OEMBattery_CancelBatteryItemListener(AEECallback *pcb);

static void OEMBattery_Init(void);
static void OEMBattery_Delete(void);
static void OEMBattery_SetPeriodicBatteryCheckTimer(void);
static void OEMBattery_TelephoneModelListener(void *unused, ModelEvent *pEvent);
static void OEMBattery_PeriodicBatteryCheck(void * pUser);
static void OEMBattery_CheckBatteryStatus(uint32 * pdwBatteryStatus);
static void OEMBattery_CheckBatteryLevel(uint32 * pdwBatteryLevel);
static void OEMBattery_CheckChargerStatus(uint32 * pdwChargerStatus);
static void OEMBattery_CheckExternalPower(uint32 * pdwExternalPower);
static void OEMBattery_CheckInUse(boolean *pbInuse);
/*===========================================================================
                 PRIVATE DATA DEFINITIONS
===========================================================================*/
static OEMBatteryMgr *gpBattery;
static boolean gOEMBatteryDebugOn = FALSE;
/*===========================================================================
                 PUBLIC METHOD DEFINITIONS
===========================================================================*/

/*===========================================================================
Manages singleton instance of gpBattery. Initializes gpBattery for first time and
increases its reference count subsequently.
===========================================================================*/
int OEMBattery_New(OEMBatteryMgr **ppme)
{
   *ppme = 0;

   FARF(BATTERY, ("==>  OEMBattery_New"));

   if (gpBattery)
   {
      ++gpBattery->nRefs;
   }
   else
   {
      gpBattery = (OEMBatteryMgr *)sys_malloc(sizeof(OEMBatteryMgr));

      if (!gpBattery)
      {
         return ENOMEMORY;
      }

      gpBattery->nRefs = 1;

      OEMBattery_Init();
   }

   *ppme = gpBattery;

   return SUCCESS;
}

/*===========================================================================
Inrecases reference count of gpBattery
===========================================================================*/
uint32 OEMBattery_AddRef(OEMBatteryMgr *pme)
{
   FARF(BATTERY, ("==>  OEMBattery_AddRef"));

   return ++pme->nRefs;
}

/*===========================================================================
Decreases reference count of gpBattery. Frees object when reference count reaches
zero
===========================================================================*/
uint32 OEMBattery_Release(OEMBatteryMgr *pme)
{
   FARF(BATTERY, ("==>  OEMBattery_Release"));

   if( --pme->nRefs )
      return pme->nRefs;

   OEMBattery_Delete();
   return 0;
}

/*===========================================================================
Decreases reference count of gpBattery. Frees object when reference count reaches
zero
===========================================================================*/
int OEMBattery_GetBatteryItem(OEMBatteryMgr *pme, uint32 dwItemID, uint32 *pdwData)
{
   int nErr = SUCCESS;

   if (!gpBattery)
   {
      return EFAILED;
   }

   switch (dwItemID)
   {
      case AEEBATTERY_ITEM_STATUS:
         *pdwData = gpBattery->dwBatteryStatus;
         break;

      case AEEBATTERY_ITEM_LEVEL:
         *pdwData = gpBattery->dwBatteryLevel;
         break;

      case AEEBATTERY_ITEM_CHARGERSTATUS:
         *pdwData = gpBattery->dwChargerStatus;
         break;

      case AEEBATTERY_ITEM_EXTPWR:
         *pdwData = gpBattery->dwExternalPower;
         break;

      default:
         nErr = EBADPARM;
         break;
   }
   return nErr;
}

/*===========================================================================
Called by AEEBattery to register callback to be invoked on change in battery
status
===========================================================================*/
void OEMBattery_OnBatteryItemUpdate(OEMBatteryMgr *pme, uint32 dwItemID, AEECallback *pcb)
{
   FARF(BATTERY, ("==>  OEMBattery_OnBatteryStatusUpdate"));

   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = OEMBattery_CancelBatteryItemListener;
   pcb->pCancelData = (void *)dwItemID;
   pme->pcbOnInfoUpdate[dwItemID] = pcb;
}

/*===========================================================================
Cancels Battery item listener callback
===========================================================================*/
static void OEMBattery_CancelBatteryItemListener(AEECallback *pcb)
{
   FARF(BATTERY, ("==>  OEMBattery_CancelBatteryItemListener"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMBattery_CancelBatteryItemListener)
      return;

   if (gpBattery &&
       (gpBattery->pcbOnInfoUpdate[(uint32)pcb->pCancelData] == pcb))
   {
      gpBattery->pcbOnInfoUpdate[(uint32)pcb->pCancelData] = 0;
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel = 0;
}

/*===========================================================================
Starts periodic battery check
===========================================================================*/
static void OEMBattery_Init(void)
{
   // Register for ITelephone changes
   {
      int result = EFAILED;
      IShell *pShell = AEE_GetShell();

      result = ISHELL_CreateInstance(
         pShell,
         AEECLSID_TELEPHONE,
         (void **) &gpBattery->pTelephone
      );

      if (SUCCESS == result) {
         result = ITELEPHONE_QueryInterface(
            gpBattery->pTelephone,
            AEEIID_MODEL,
            (void **) &gpBattery->pTelephoneModel
         );

         if(SUCCESS == result) {
            (void) IMODEL_AddListenerEx(
               gpBattery->pTelephoneModel,
               &gpBattery->telephoneListener,
               (PFNLISTENER) OEMBattery_TelephoneModelListener,
               gpBattery
            );
         }
      }
   }
   
   // Check Battery Status
   OEMBattery_CheckBatteryStatus(&gpBattery->dwBatteryStatus);

   // Check Battery Level
   OEMBattery_CheckBatteryLevel(&gpBattery->dwBatteryLevel);

   // Check Charger Status
   OEMBattery_CheckChargerStatus(&gpBattery->dwChargerStatus);

   // Check External Power
   OEMBattery_CheckExternalPower(&gpBattery->dwExternalPower);
   
   // Check the in-use state of the phone
   OEMBattery_CheckInUse(&gpBattery->phoneInUse);

   OEMBattery_SetPeriodicBatteryCheckTimer();
}

/*===========================================================================
Stops periodic battery check
===========================================================================*/
static void OEMBattery_Delete(void)
{
   uint32 dwItemID = 0;

   // Cancel periodic battery check timer
   AEE_CancelTimer(OEMBattery_PeriodicBatteryCheck, NULL);

   for (dwItemID = 0; dwItemID < AEEBATTERY_ITEM_LAST; dwItemID++)
   {
      if (gpBattery->pcbOnInfoUpdate[dwItemID])
         CALLBACK_Cancel(gpBattery->pcbOnInfoUpdate[dwItemID]);
   }

   // Cancel and free the listener
   LISTENER_Cancel(&gpBattery->telephoneListener);
   if (gpBattery->pTelephoneModel != NULL) {
      IMODEL_Release(gpBattery->pTelephoneModel);
      gpBattery->pTelephoneModel = NULL;
   }


   if (gpBattery->pTelephone != NULL) {
      ITELEPHONE_Release(gpBattery->pTelephone);
      gpBattery->pTelephone = NULL;
   }

   sys_free(gpBattery);
   gpBattery = NULL;
}


/*===========================================================================
Sets the periodic battery check timer
===========================================================================*/
static void OEMBattery_SetPeriodicBatteryCheckTimer(void)
{
   uint32 periodicTimerMs = TIMEOUT_MS_PERIODICBATTCHECK_SLOW;

   // Use the faster timer if we are in-use or if we have external power
   // present.
   if (gpBattery->phoneInUse || gpBattery->dwExternalPower) {
      periodicTimerMs = TIMEOUT_MS_PERIODICBATTCHECK_FAST;
   }

   // Start periodic battery check timer
   (void) AEE_SetSysTimer(periodicTimerMs,
                          OEMBattery_PeriodicBatteryCheck,
                          NULL);
}


/*===========================================================================
Listens for ITelephone changes
===========================================================================*/
static void OEMBattery_TelephoneModelListener(void *unused, ModelEvent *pEvent)
{
   boolean prevInUse = gpBattery->phoneInUse;

   // Check the in-use state of the phone
   OEMBattery_CheckInUse(&gpBattery->phoneInUse);

   // If the in-use state has changed, reset the timer
   if (prevInUse != gpBattery->phoneInUse) {
      OEMBattery_SetPeriodicBatteryCheckTimer();
   }
}

/*===========================================================================
Makes battery check and calls status update callbacks
===========================================================================*/
#include "err.h"
static void OEMBattery_PeriodicBatteryCheck(void * pUser)
{
   uint32 dwBatteryStatus = AEEBATTERY_STATUS_UNKNOWN;
   uint32 dwBatteryLevel = 0;
   uint32 dwChargerStatus = AEEBATTERY_CHARGERSTATUS_UNKNOWN;
   uint32 dwExternalPower = 0;
   int32 nDelta =0;
   
   if (NULL == gpBattery) {
      return;
   }

   if (gOEMBatteryDebugOn) {
      DBGPRINTF_LOW("Battery Periodic Check.. Battery Status=%d Charger Status=%d Ext Pwr=%d", 
                         gpBattery->dwBatteryStatus, gpBattery->dwChargerStatus, gpBattery->dwExternalPower);
   }

   // Check Battery Status
   OEMBattery_CheckBatteryStatus(&dwBatteryStatus);
   if (gOEMBatteryDebugOn) {
      DBGPRINTF_LOW("OEMBattery_CheckBatteryStatus dwBatteryStatus = %d", dwBatteryStatus);
   }
   if (gpBattery->dwBatteryStatus != dwBatteryStatus)
   {
      gpBattery->dwBatteryStatus = dwBatteryStatus;
      if (gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_STATUS])
         AEE_SYS_RESUME(gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_STATUS]);
   }

   // Check Battery Level
   OEMBattery_CheckBatteryLevel(&dwBatteryLevel);
   nDelta = dwBatteryLevel - gpBattery->dwBatteryLevel;
   nDelta = (nDelta) < 0 ? (-(nDelta)) : (nDelta);
//   ERR("miaoxiaoming:gpBattery->dwBatteryLevel =%d, dwBatteryLevel=%d, nDelta=%d",gpBattery->dwBatteryLevel,dwBatteryLevel,nDelta);
   if (nDelta > VBATT_MEAN_RANGE)        /*µç³Ø¶¶¶¯*/
  // if (gpBattery->dwBatteryLevel != dwBatteryLevel)
   {
      gpBattery->dwBatteryLevel = dwBatteryLevel;
      if (gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_LEVEL])
         AEE_SYS_RESUME(gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_LEVEL]);
   }

   // Check Charger Status
   OEMBattery_CheckChargerStatus(&dwChargerStatus);
   if (gOEMBatteryDebugOn) {
      DBGPRINTF_LOW("OEMBattery_CheckChargerStatus dwChargerStatus = %d", dwChargerStatus);
   }
   if (gpBattery->dwChargerStatus != dwChargerStatus)
   {
      gpBattery->dwChargerStatus = dwChargerStatus;
      if (gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_CHARGERSTATUS])
         AEE_SYS_RESUME(gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_CHARGERSTATUS]);
   }

   // Check External Power
   OEMBattery_CheckExternalPower(&dwExternalPower);
   if (gOEMBatteryDebugOn) {
      DBGPRINTF_LOW("OEMBattery_CheckExternalPower dwExternalPower = %d", dwExternalPower);
   }
   if (gpBattery->dwExternalPower != dwExternalPower)
   {
      gpBattery->dwExternalPower = dwExternalPower;
      if (gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_EXTPWR])
         AEE_SYS_RESUME(gpBattery->pcbOnInfoUpdate[AEEBATTERY_ITEM_EXTPWR]);
   }

   OEMBattery_SetPeriodicBatteryCheckTimer();
}

#ifdef FEATURE_BREW_BATTERY
/*===========================================================================
   Checks Battery Status
===========================================================================*/
static void OEMBattery_CheckBatteryStatus(uint32 * pdwBatteryStatus)
{
   if (pdwBatteryStatus)
   {
#ifdef CUST_EDITION
      chg_plvl_rtn_type wBattMV = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);
      uint16 level;
      
      if(wBattMV < BATT_MV_MIN)
      {
          wBattMV = BATT_MV_MIN;
      }

      if(wBattMV > BATT_MV_MAX)
      {
          wBattMV = BATT_MV_MAX;
      }
      
      level = (((wBattMV - BATT_MV_MIN)*VBATT_SCALE)/(BATT_MV_MAX-BATT_MV_MIN)) + VBATT_SCALED_MIN;
#else
      uint8 level = vbatt_read();
#endif
      if (level < BATTERY_LEVEL_POWERDOWN)
      {
         *pdwBatteryStatus = AEEBATTERY_STATUS_POWERDOWN;
      }
      else if (level < BATTERY_LEVEL_WARN)
      {
         *pdwBatteryStatus = AEEBATTERY_STATUS_LOW;
      }
      else
      {
         *pdwBatteryStatus = AEEBATTERY_STATUS_NORMAL;
      }
   }
}

/*===========================================================================
   Checks Battery Level
===========================================================================*/
static void OEMBattery_CheckBatteryLevel(uint32 * pdwBatteryLevel)
{
   if (pdwBatteryLevel)
   {
#ifdef CUST_EDITION
      chg_plvl_rtn_type wBattMV = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);
      uint16 wBattLevel;
      
      if(wBattMV < BATT_MV_MIN)
      {
          wBattMV = BATT_MV_MIN;
      }

      if(wBattMV > BATT_MV_MAX)
      {
          wBattMV = BATT_MV_MAX;
      }
      
      wBattLevel = (((wBattMV - BATT_MV_MIN)*VBATT_SCALE)/(BATT_MV_MAX-BATT_MV_MIN)) + VBATT_SCALED_MIN;
      *pdwBatteryLevel = ((VBATT_SCALE << 16)|wBattLevel);
#else
      *pdwBatteryLevel = ((VBATT_SCALE << 16)|vbatt_read());
#endif
   }
}

/*===========================================================================
   Checks Charger Status
===========================================================================*/
static void OEMBattery_CheckChargerStatus(uint32 * pdwChargerStatus)
{
   if (pdwChargerStatus)
   {

      *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_UNKNOWN;
      
      if (!chg_is_charger_valid()) {
         // Charger is not valid so return no charger and exit
         *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_NO_CHARGER;
         return;
      }

#if defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK) || defined (FEATURE_PM_CHARGING)
      MSG_FATAL("***chg_ui_event_read=%d***", chg_ui_event_read(), 0, 0);
      switch(chg_ui_event_read())
      {
         // No Charger
         case CHG_UI_EVENT__IDLE:
            *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_NO_CHARGE;
            break;


          //Very Weak Charger
          case CHG_UI_EVENT__NO_POWER:
             if(!chg_is_battery_valid()) {
                /* A bad/weak battery + Weak Charger is present. Phone cannot operate in this state.*/
         *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_NO_CHARGER;
             } else {
              *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_DEAD_BATT;
             }
             break;



          //Weak Charger
          case CHG_UI_EVENT__VERY_LOW_POWER:
#ifdef FEATURE_GSM
             if(!chg_is_battery_valid()) {
                *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_NO_CHARGER;
             } else {
                *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_DEAD_BATT;
             }
#else
             // Simply display no battery. Phone is still operable
             if(!chg_is_battery_valid()) {
                *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_NO_BATT;
             } else {
                 //Stable charging condition
         *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_CHARGING;
   }
#endif
             break;


          // Strong Charger
          case CHG_UI_EVENT__LOW_POWER  :
#ifdef FEATURE_GSM
             *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_DEAD_BATT;
#else
             *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_CHARGING;
#endif
             break;

          // Normal Charger
          case CHG_UI_EVENT__NORMAL_POWER:
             *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_CHARGING;
             break;

          //Very Strong Charger
          case CHG_UI_EVENT__DONE:
             *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_FULLY_CHARGE;
             break;
          
          default:
             *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_UNKNOWN;
             break;
       }
#else
      if (chg_is_charging() == TRUE ) {
         *pdwChargerStatus = AEEBATTERY_CHARGERSTATUS_CHARGING;
      }
#endif // FEATURE_BATTERY_CHARGER || FEATURE_CHG_TASK || FEATURE_PM_CHARGING
   }
}

/*===========================================================================
   Checks External Power
===========================================================================*/
static void OEMBattery_CheckExternalPower(uint32 * pdwExternalPower)
{
   if (pdwExternalPower)
   {
      *pdwExternalPower = (chg_is_charger_valid() ? 1 : 0);
   }
}

/*===========================================================================
   Checks the in-use state of the phone
===========================================================================*/
static void OEMBattery_CheckInUse(boolean *pbInuse)
{
   if (pbInuse) {
      AEETPhInfo phInfo;

      if ( (gpBattery->pTelephone != NULL) &&
           (SUCCESS == ITELEPHONE_GetPhoneInfo(gpBattery->pTelephone, &phInfo, sizeof(phInfo))) ) {
         *pbInuse = phInfo.is_in_use ? TRUE : FALSE;
      } else {
         *pbInuse = FALSE;
      }
   }
}

// Forces OEMBattery to check the battery/charger state
void OEMBattery_Refresh(void)
{
   OEMBattery_PeriodicBatteryCheck(NULL);
}
#else
static void OEMBattery_CheckBatteryStatus(uint32 * pdwBatteryStatus)
{
}

static void OEMBattery_CheckBatteryLevel(uint32 * pdwBatteryLevel)
{
}

static void OEMBattery_CheckChargerStatus(uint32 * pdwChargerStatus)
{
}

static void OEMBattery_CheckExternalPower(uint32 * pdwExternalPower)
{
}

static void OEMBattery_CheckInUse(boolean *pbInuse)
{
}

void OEMBattery_Refresh(void)
{
}
#endif // FEATURE_BREW_BATTERY

#ifdef CUST_EDITION
#define OEMBATTERY_NOTIFY_NUM   3
static AEECallback gBattNotify[OEMBATTERY_NOTIFY_NUM];
static int gBattNotifyIdx = 0;

static void OEMBatt_OnBatteryChange(void *unused)
{
    OEMBattery_Refresh();
    gBattNotifyIdx--;
}

void oembatt_notify_chg_event(void)
{
    if(AEE_IsInitialized() && gBattNotifyIdx < OEMBATTERY_NOTIFY_NUM)
    {
        AEECallback* pCb = &gBattNotify[gBattNotifyIdx++];
        CALLBACK_Init(pCb,OEMBatt_OnBatteryChange,NULL);
        AEE_SYS_RESUME(pCb);
    }
}
#endif

