/*=============================================================================

FILE: OEMBatt.c

SERVICES: OEM Battery Interface

GENERAL DESCRIPTION:
   This file implements the OEM Battery API.

   Note that the battery levels and number of annunciator bars (amongst
   other things in this file) are device specific and will likely need
   to be modified when moving to a new device.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002, 2003, 2004, 2005, 2006, 2007 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMBatt.c#27 $
$DateTime: 2008/12/29 01:47:28 $
  $Author: krishnac $
  $Change: 812132 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/15/08   kcp     fixed the battery level update issue - 158381
11/12/07   cvs     fix compile warning
03/29/07   cvs     fix compile warnings
03/28/07   jmb     Fixing issue where BREW APIs are called from the DEM task.
                   This call back will switch into UI task before proceeding.
08/08/06   jas     Fixing a compiler warning
07/27/06   cvs     don't notify of a dead battery if a strong charger is connected.
07/11/06   vkura   Moving the periodic battery check from polling mode to
                   usage of callback.The call back used is registered with DEM.
06/14/06   JD      Made scalefactor and a local signed to prevent warning.
03/14/06   jas     Lint fixes.
06/20/05   jas/bao Fix issue of stale IConfig pointer
03/10/05   cvs     Use ftm_get_mode() instead of variable
01/07/04   sun     Fixed Compilation Warnings
12/08/04   SUN     Fixed the various battery level and cut-off settings for
                   FEATURE_VBATT_CUT_OFF_3P2V.
11/18/04   sun     Added Changes for USB Charging
07/27/04   SUN     Removed 6100 related changes
07/07/04   SUN     Code Review Changes
05/13/04   sun     Do not power down in FTM
04/20/04   sun     Made changes to the power off level
02/25/04   ram     Added initial revision.
02/09/02   mjv     Updated for 2GUI.
09/12/03   jcb     Removed is_charger_overvoltage() check as it's generally
                   not supported or used by any target.
08/20/03   mjv     Added a check to OEM_EnableBattAnnun() to avoid doing more
                   work than we need to.
02/07/03   mjv     Removed lint when OEM_NORMAL_BATT_BARS_4 is not defined.
02/04/03   jgr     Added support for four bars of battery gauging.
01/22/03   tdw     Added OEMPriv_CheckLowBattery and a new feature called
                   FEATURE_CHECK_VBATT_WITH_EXT_PWR for wimpy wall worts.
01/20/03   jgr     Changed bars for LCP.
12/11/02   mjv     Fixed Lint errors when HAS_CHARGER is zero.
12/05/02   jgr     OEM_GetChargerState always returns
                   AEECHG_STATUS_CANNOT_CHARGE if HAS_CHARGER is not defined
10/23/02   jcb     Added the following for charger overvoltage:
                    -Extern function is_charger_overvoltage()
                    -Modification of OEM_GetChargerState()
                    -Modification of OEMPriv_PeriodicBattCheck()
                    -DisplayCurrentBatteryLevel()
10/04/02   jgr     Adjust battery guage values for LCP
09/13/02   mjv     Added a return value to AEE_BatteryLevelChange.
08/30/02   kjm     Enhanced to be cross-target.
08/12/02   jcb     Changed all calls to rex_wait to OEMUITask_rex_wait.
08/06/02   kjm     Removed ASSERT_NOT_REACHABLE nonsense.  If there is no
                   charger, report external power always on.
08/05/02   kjm     Now supports no charger.
07/22/02   mjv     Battery level now visible when external power is applied
                   during a call (the battery does not charge while in
                   this state, so the charging animation is not shown).
06/13/02   mjv     Removed inclusion of OEMComdef.h
06/03/02   mjv     Initial revision

=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#include "AEE_OEM.h"

#include "OEMBatt.h"
#include "vbatt.h"

#ifdef FEATURE_BATTERY_NOTIFICATION
#include "dem.h"
#endif

#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#include "db.h"
#endif

#if defined (FEATURE_LOW_BATT_DETECTION)
  #include "pm.h"
#endif

#include "msg.h"

#if defined(FEATURE_BATTERY_CHARGER) || defined (FEATURE_PM_CHARGING)
#include "charger.h"

#endif

#include "hs.h"

#include "AEEConfig.h"
#include "OEMCFGI.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#endif


#if MIN_BREW_VERSION(3,0)
#include "AEE_OEMDispatch.h"
#endif

//below .h will have the declaration for ui_get_nv
#ifndef USES_MMI
#include "uiutils.h"
#endif
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


////
// How often OEMBattPriv_PeriodicBattCheck() is run (in ms)
#define TIMEOUT_MS_PERIODICBATTCHECK_FULL_BATT 30000
#define TIMEOUT_MS_PERIODICBATTCHECK_LOW_BATT 15000
#define TIMEOUT_MS_PERIODICBATTCHECK_INIT 5000



#define BATT_LEVEL_NOBATT   (-1)

#define UNSET_BATT ((VBATT_SCALE * 2) + 1)     /* battery never read */


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef enum {
   OEMBATTLEVEL_NONE    = 0xFF, // Empty battery bar
   OEMBATTLEVEL_INVALID = 0xFE, // Invalid level
   OEMBATTLEVEL_BARS_0  = 0,
   OEMBATTLEVEL_BARS_1,
   OEMBATTLEVEL_BARS_2,
   OEMBATTLEVEL_BARS_3,
   OEMBATTLEVEL_BARS_4,
   OEMBATTLEVEL_MAXLEVELS
} OEMBattLevel;



// OEM Battery context
typedef struct OEMBatt {
  int32                lastbatt;           // Last vbatt battery level
  AEEBattStatus        m_lastStatus;       // Last reported battery status
  AEEChargerStatus m_lastChargerState; // Last reported charger state.
  OEMBattLevel         m_lastBattLevel;    // Last reported battery level
  boolean              m_bExtPower; // Last reported if on external powert
  AEECallback          m_cbBatteryCheck;
} OEMBatt;


/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

// OEM Battery context
OEMBatt sBatt = {
   UNSET_BATT,
   AEEBATTSTATUS_NORMAL,
   AEECHG_STATUS_NO_CHARGE,
   OEMBATTLEVEL_INVALID,
   FALSE,
   {0}
};

//replaced the earlier macros with the var as the values may change from target to target
static int32 batt_level_4_bar;
static int32 batt_level_3_bar;
static int32 batt_level_2_bar;
static int32 batt_level_1_bar;
static int32 batt_level_0_bar;
static int32 batt_level_pwr_off;
static int32 batt_level_warn;








#ifdef FEATURE_BATTERY_NOTIFICATION
boolean OEMBattReg = FALSE;
#endif


/*==========================================================================

                       LOCAL/STATIC FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_BATTERY_NOTIFICATION
static void OEMBattPriv_PeriodicBattCheck_DEMTask(void *pUser);
#endif

static void OEMBattPriv_PeriodicBattCheck(void *pUser);
static int16 OEMBattPriv_GetBatteryLevel(void);
static void OEMBattPriv_DebounceBatteryLevel(int16 level);

static void OEMBattPriv_ReportLevelChange(OEMBattLevel newLevel);
static void OEMBattPriv_ReportExtPowerChange(boolean newStatus);
static void OEMBattPriv_ReportStatusChange(AEEBattStatus newStatus);
static void OEMBattPriv_ReportChargerChange(AEEChargerStatus newState);
static int32 OEMBatt_vbatt_scale(int32 batt);
static void OEMBattPriv_PeriodicBattCheck(void *pUser);
static void OEMBatt_InitBattLevels(void *pUser);



/*=============================================================================
FUNCTION: OEMBatt_Init

DESCRIPTION:
   Initialize the OEM Battery layer.  This must be called before
   AEE uses any other OEM Battery functions.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMBatt_Init(void)
{

  OEMBatt_InitBattLevels(NULL);

  CALLBACK_Init(&sBatt.m_cbBatteryCheck, OEMBattPriv_PeriodicBattCheck, NULL);
  // Start the periodic battery check
  (void) AEE_SetSysTimer(TIMEOUT_MS_PERIODICBATTCHECK_INIT,
                          OEMBattPriv_PeriodicBattCheck,
                          NULL);


}
void OEMBatt_Destory(void)
{
    AEE_CancelTimer(OEMBattPriv_PeriodicBattCheck, NULL);
}
/*=============================================================================
FUNCTION: OEMBatt_InitBattLevels

DESCRIPTION:
   Initialize the OEM Battery layer.  This must be called before
   AEE uses any other OEM Battery functions.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

void OEMBatt_InitBattLevels(void *pUser)
{
  nv_item_type          nv_data_buf;                   /* NV data buffer */
  int range;


  /* Get nv read*/
#ifndef FEATURE_OEMUI_TASK    
   if(NV_DONE_S != ui_get_nv (NV_VBATT_I, &nv_data_buf))
#else     
    if(NV_DONE_S != OEMNV_Get (NV_VBATT_I, &nv_data_buf))
#endif
  {
    MSG_ERROR("NV read failed", 0, 0, 0);
	return;
  }
  range = nv_data_buf.vbatt.max - nv_data_buf.vbatt.min;
  batt_level_4_bar = OEMBatt_vbatt_scale(nv_data_buf.vbatt.max);
  batt_level_3_bar = OEMBatt_vbatt_scale(nv_data_buf.vbatt.min + ((4 * range)/ 5));
  batt_level_2_bar = OEMBatt_vbatt_scale(nv_data_buf.vbatt.min + ((3 * range)/ 5));
  batt_level_1_bar = OEMBatt_vbatt_scale(nv_data_buf.vbatt.min + ((2 * range) / 5));
  batt_level_0_bar = OEMBatt_vbatt_scale(nv_data_buf.vbatt.min + (range / 5));

  batt_level_warn = batt_level_0_bar;
  batt_level_pwr_off = 0;
}

/*=============================================================================
FUNCTION: OEMBatt_vbatt_scale

DESCRIPTION:
   This API will scale the batt accordingly based on the 
   target specifications.
PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int32 OEMBatt_vbatt_scale(int32 batt)
{
  nv_item_type          nv_data_buf;
#ifndef FEATURE_OEMUI_TASK    
   if(NV_DONE_S != ui_get_nv (NV_VBATT_I, &nv_data_buf))
#else     
    if(NV_DONE_S != OEMNV_Get (NV_VBATT_I, &nv_data_buf))
#endif
  {
    MSG_ERROR("NV read failed", 0, 0, 0);
	return 0;
  }

  return ((batt - nv_data_buf.vbatt.min) * 255 / (nv_data_buf.vbatt.max - nv_data_buf.vbatt.min));
}
/*=============================================================================
FUNCTION: OEMBatt_GetBattLevel

DESCRIPTION:  Returns the current battery level

PARAMETERS:
   None

RETURN VALUE:
   AEEBattLevel:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEBattLevel OEMBatt_GetBattLevel(void)
{
   AEEBattLevel batt;

   batt.level = (int)sBatt.m_lastBattLevel;
   batt.numlevels = (int)OEMBATTLEVEL_MAXLEVELS;

   return batt;
}


/*=============================================================================
FUNCTION: OEMBatt_GetBattStatus

DESCRIPTION:  Returns the current battery status

PARAMETERS:
   None

RETURN VALUE:
   AEEBattStatus:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEBattStatus OEMBatt_GetBattStatus(void)
{
   return sBatt.m_lastStatus;
}



/*=============================================================================
FUNCTION: OEMBatt_GetChargerState

DESCRIPTION:
  Returns the state of the battery charger.

PARAMETERS:
   None

RETURN VALUE:
   AEEChargerStatusType:  current charger status

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEChargerStatus OEMBatt_GetChargerState(void)
{
    /* update the charger information */

    if(!OEMBatt_GetExternalPower())
        return AEECHG_STATUS_NO_CHARGER;

#if defined(FEATURE_BATTERY_CHARGER) || defined (FEATURE_PM_CHARGING)
   switch(chg_ui_event_read() )
   {
     // No Charger
     case CHG_UI_EVENT__IDLE:
       return AEECHG_STATUS_NO_CHARGE;

      //No/Weak Battery + Weak Charger.
     case CHG_UI_EVENT__NO_POWER:

/*if there is no battery& the charger is also weak,
  client should display information appropriately*/
       if(!chg_is_battery_valid())
          return AEECHG_STATUS_NO_CHARGE_NO_BATT;

/*A bad/weak battery is present. Phone cannot operate in this state.*/
       return AEECHG_STATUS_DEAD_BATT;

      //No/Weak Battery + Strong Charger
     case CHG_UI_EVENT__VERY_LOW_POWER:
#ifdef FEATURE_GSM
#error code not present
#else
// Simply display no battery. Phone is still operable
       if(!chg_is_battery_valid())
          return AEECHG_STATUS_NO_BATT;
//Stable charging condition
      return AEECHG_STATUS_CHARGING;
#endif

//Low Battery + Strong Char
     case CHG_UI_EVENT__LOW_POWER  :
#ifdef FEATURE_GSM
#error code not present
#else
    return AEECHG_STATUS_CHARGING;
#endif


     case CHG_UI_EVENT__NORMAL_POWER  :
       return AEECHG_STATUS_CHARGING;

     case CHG_UI_EVENT__DONE:
       return AEECHG_STATUS_FULLY_CHARGE;

     default:
       return AEECHG_STATUS_UNKNOWN_BATT;
   }
#else // FEATURE_BATTERY_CHARGER || FEATURE_PM_CHARGING
   return AEECHG_STATUS_UNKNOWN_BATT;
#endif // FEATURE_BATTERY_CHARGER || FEATURE_PM_CHARGING
}


/*=============================================================================
FUNCTION: OEMBatt_GetExternalPower

DESCRIPTION: Returns the state of external power

PARAMETERS:
   None

RETURN VALUE:
   boolean:  TRUE  if external power is attached
             FALSE if external power is not attached.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMBatt_GetExternalPower(void)
{
  return HS_EXT_PWR_ON();
}

#ifdef FEATURE_BATTERY_NOTIFICATION
/*=============================================================================
FUNCTION: OEMBattPriv_PeriodicBattCheck_DEMTask

DESCRIPTION:
  Periodically update the battery annunciator and report battery related
  changes to the AEE layer.

PARAMETERS:
   *pUser: Unused

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMBattPriv_PeriodicBattCheck_DEMTask(void *pUser)
{
   AEE_SYS_RESUME(&sBatt.m_cbBatteryCheck);
}
#endif /* FEATURE_BATTERY_NOTIFICATION */

/*=============================================================================
FUNCTION: OEMBattPriv_PeriodicBattCheck

DESCRIPTION:
  Periodically update the battery annunciator and report battery related
  changes to the AEE layer.

PARAMETERS:
   *pUser: Unused

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMBattPriv_PeriodicBattCheck(void *pUser)
{
   int16 level = batt_level_0_bar;
#ifndef FEATURE_BATTERY_NOTIFICATION

   int timer = TIMEOUT_MS_PERIODICBATTCHECK_FULL_BATT;

#endif

   PARAM_NOT_REF(pUser)

   boolean m_bExtPower = OEMBatt_GetExternalPower();
   AEEChargerStatus cstate = OEMBatt_GetChargerState();
   OEMBattPriv_ReportExtPowerChange(m_bExtPower);
   OEMBattPriv_ReportChargerChange(cstate);




   if (m_bExtPower) {
      OEMBattLevel         bicon = OEMBATTLEVEL_INVALID;

#ifdef FEATURE_CHECK_VBATT_WITH_EXT_PWR
      // Check for low battery even if we have external power,
      // for wimpy wall worts
      (void) OEMBattPriv_GetBatteryLevel();
#endif

      switch (cstate) {
      case AEECHG_STATUS_FULLY_CHARGE:  // Battery is fully charged
         bicon = (OEMBattLevel) ( (int)(OEMBATTLEVEL_MAXLEVELS) - 1 );
         break;

      case AEECHG_STATUS_CANNOT_CHARGE: // Cannot charge for the moment.
      case AEECHG_STATUS_CHARGING:      // Battery is being charged
      case AEECHG_STATUS_OVERVOLTAGE:   // Over-voltage state
         {
            // Report current battery level
            level = OEMBattPriv_GetBatteryLevel();
            OEMBattPriv_DebounceBatteryLevel(level);
         }
         break;

      case AEECHG_STATUS_NO_BATT:     // Charger couldn't detect a battery
      case AEECHG_STATUS_NO_CHARGE_NO_BATT:
        bicon = OEMBATTLEVEL_INVALID;
        break;

      case AEECHG_STATUS_UNKNOWN_BATT:// Charger found an unrecognized battery
      case AEECHG_STATUS_DEAD_BATT:   // Charger found a dead battery
      case AEECHG_STATUS_NO_CHARGE:   // Nothing's wrong.  Just haven't started
      case AEECHG_STATUS_NO_CHARGER:  // Charger isn't working now
      case AEECHG_STATUS_INIT:        // Initialized state
      default:
         bicon = OEMBATTLEVEL_NONE;
         break;

      }



      if (OEMBATTLEVEL_INVALID != bicon) {
         // Report the new battery level without debounce.
         OEMBattPriv_ReportLevelChange(bicon);
      }

   } else {
      level = OEMBattPriv_GetBatteryLevel();
      OEMBattPriv_DebounceBatteryLevel(level);
   }

#ifdef FEATURE_BATTERY_NOTIFICATION
   if (OEMBattReg == FALSE)
   {
      dem_battery_reg(0x05,OEMBattPriv_PeriodicBattCheck_DEMTask,pUser);
      OEMBattReg = TRUE;
   }
#else /* FEATURE_BATTERY_NOTIFICATION */
   // Reset periodic battery check timer
   if (level < batt_level_2_bar)
   {
      timer = TIMEOUT_MS_PERIODICBATTCHECK_LOW_BATT;
   }
   else
   {
      timer = TIMEOUT_MS_PERIODICBATTCHECK_FULL_BATT;
   }


   (void)AEE_SetSysTimer(timer, OEMBattPriv_PeriodicBattCheck,
                         NULL);
#endif /* FEATURE_BATTERY_NOTIFICATION */

//lint -save -esym(715, pUser)  Suppress "pUser not used"
//lint -save -esym(818, pUser)  Suppress "pUser could be const"
}
//lint -restore
//lint -restore



/*=============================================================================
FUNCTION: OEMBattPriv_GetBatteryLevel

DESCRIPTION:
  Function that checks for a low battery status and reports it to AEE if
  necessary.

PARAMETERS:
   None

RETURN VALUE:
   int16:  The current battery level

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int16 OEMBattPriv_GetBatteryLevel(void)
{



#ifdef FEATURE_PM_CHARGING
  int32 batt = chg_battery_volt();
#else
  int32 batt = vbatt_read();
 #endif
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  if( batt != sBatt.lastbatt || OEMBatt_GetExternalPower() != sBatt.m_bExtPower )
  {
    db_items_value_type dbi; /* DB item */
    dbi.battery_level = batt;
    db_put( DB_BATTERY_LEVEL, &dbi);
  }
#endif

  sBatt.lastbatt = batt;

  //
  // Report changes in battery status
  //
   if (batt <= batt_level_pwr_off
#ifdef FEATURE_LOW_BATT_DETECTION
      && (pm_is_low_batt_alarm_triggered() == TRUE)
#endif
#ifdef FEATURE_FACTORY_TESTMODE
      && (ftm_get_mode() != FTM_MODE)
#endif
      && (OEMBatt_GetChargerState() != AEECHG_STATUS_CHARGING)
      )
   {
      OEMBattPriv_ReportStatusChange(AEEBATTSTATUS_POWERDOWN);
   }
   else if (batt <= batt_level_warn
#ifdef FEATURE_LOW_BATT_DETECTION
      && (pm_is_low_batt_alarm_triggered() == TRUE)
#endif

     )
   {
      OEMBattPriv_ReportStatusChange(AEEBATTSTATUS_LOW);
   }
   else
   {
      OEMBattPriv_ReportStatusChange(AEEBATTSTATUS_NORMAL);
   }

  return (int16)batt;

}

/*=============================================================================
FUNCTION: OEMBattPriv_DebounceBatteryLevel

DESCRIPTION:
  Displays the current battery level.

PARAMETERS:
   None

RETURN VALUE:
   none

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMBattPriv_DebounceBatteryLevel(int16 level)
{
   OEMBattLevel bicon = sBatt.m_lastBattLevel;

   // The code below makes sure the battery level never
   // bounce up to have high bar level with out charging first.

   if (level < batt_level_1_bar)
   {
      // display 0 batt bars
      bicon = OEMBATTLEVEL_BARS_0;

   }
   else if (level < batt_level_2_bar)
   {
      if ( (sBatt.m_lastBattLevel != OEMBATTLEVEL_BARS_0) ||
           (level > batt_level_1_bar))
      {
         // display 1 batt bar
         bicon = OEMBATTLEVEL_BARS_1;
      }
   }
   else if (level < batt_level_3_bar)
   {
      if ( (sBatt.m_lastBattLevel != OEMBATTLEVEL_BARS_1) ||
           (level > batt_level_2_bar) )
      {
         // display 2 batt bars
         bicon = OEMBATTLEVEL_BARS_2;
      }
   }
   else if (level < batt_level_4_bar)
   {
      if ( (sBatt.m_lastBattLevel != OEMBATTLEVEL_BARS_2) ||
           (level > batt_level_3_bar) )
      {
         // display 3 batt bars
         bicon = OEMBATTLEVEL_BARS_3;
      }
   }
   else if ( (sBatt.m_lastBattLevel >= OEMBATTLEVEL_BARS_4) ||
               (level >= batt_level_4_bar) )
   {
      // display 4 batt bars
      bicon = OEMBATTLEVEL_BARS_4;
   }
   else
   {
      bicon = sBatt.m_lastBattLevel;
   }

   OEMBattPriv_ReportLevelChange(bicon);
}


/*=============================================================================
FUNCTION: OEMBattPriv_ReportLevelChange

DESCRIPTION: Report a change in the battery level

PARAMETERS:
   newLevel:  New relative battery level

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMBattPriv_ReportLevelChange(OEMBattLevel newLevel)
{
   if (newLevel != sBatt.m_lastBattLevel) {
      AEEBattLevel batt;

      sBatt.m_lastBattLevel = newLevel;

      batt.level = (byte)newLevel;
      batt.numlevels = (int)OEMBATTLEVEL_MAXLEVELS;

      AEEBatt_BatteryLevelChange(batt);


   }
}


/*=============================================================================
FUNCTION: OEMBattPriv_ReportExtPowerChange

DESCRIPTION:  Report a change in the external power

PARAMETERS:
   newStatus: New External power status

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMBattPriv_ReportExtPowerChange(boolean newStatus)
{
  if (sBatt.m_bExtPower!= newStatus) {
      IConfig *pIConfig = NULL;
      IShell *shell_ptr = AEE_GetShell();

      AEEBatt_ExternalPowerChange(newStatus);
      sBatt.m_bExtPower = newStatus;

      if (shell_ptr == NULL) {
         MSG_ERROR("Shell ptr null", 0, 0, 0);
         return;
      }

      (void)ISHELL_CreateInstance(shell_ptr, AEECLSID_CONFIG, (void **) &pIConfig);
      if (pIConfig) {
         (void)ICONFIG_SetItem(pIConfig, CFGI_EXT_PWR, &newStatus, 1);
         (void)ICONFIG_Release(pIConfig);
      }

   }
}

/*=============================================================================
FUNCTION: OEMBattPriv_ReportStatusChange

DESCRIPTION:  Report a change in the battery status

PARAMETERS:
   newStatus: New battery status

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMBattPriv_ReportStatusChange(AEEBattStatus newStatus)
{
   if (sBatt.m_lastStatus != newStatus) {
      AEEBatt_BatteryStatusChange(newStatus);
      sBatt.m_lastStatus = newStatus;
   }
}


/*=============================================================================
FUNCTION: OEMBattPriv_ReportChargerChange

DESCRIPTION:  Report a change in the charger status

PARAMETERS:
   newState:  New charger state

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMBattPriv_ReportChargerChange(AEEChargerStatus newState)
{
   if (newState != sBatt.m_lastChargerState) {
      // Report the charger state change
      AEEBatt_ChargerStateChange(newState);
      sBatt.m_lastChargerState = newState;
   }
}

