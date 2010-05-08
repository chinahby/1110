/*=============================================================================

FILE: OEMAlarm.c

SERVICES: Implementation of IAlarm class.

GENERAL DESCRIPTION:
   Provides services to set/clear BREW alarms with PMIC Real Time Clock,
   allows applications to query the alarm status, and receive model events
   indicating when an alarm is set or cleared.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2003, 2004, 2006 QUALCOMM Incorporated.
                         All Rights Reserved.

                         QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/oem/Src/OEMAlarm.c#5 $
  $DateTime: 2006/10/31 12:16:12 $
  $Author: johns $
  $Change: 396827 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/27/06   gb      Temporary hack to get user set time working
                   Awaiting a final fix from PM.
03/14/06   jas     Lint fixes
05/24/05   jas     Fixing include file case for Linux builds.
02/07/05   jas     Featurized for targets without PMIC RTC (Ryan Barnett)
12/21/04   rsb     Initial version.

=============================================================================*/


/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"

#include "SysModelBase_priv.h"

#include "AEEAlarm.h"
#include "OEMAlarm_priv.h"

#if defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK)
#include "pm.h" 
#include "clk.h"
#endif // defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK)

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

/*===========================================================================

                    DATA TYPES

===========================================================================*/

/*===========================================================================

                    DATA STRUCTURES

===========================================================================*/

/**
 * Data structure associated with IAlarm object.
 */
typedef struct _OEMAlarm {
    AEEVTBL(IAlarm) *pvt;
    uint32 m_uRefs;
} OEMAlarm;

/**
 * Data structure associated with IAlarm model.
 */
typedef struct _OEMAlarmModel {
   SysModelBase    base;
   AEEVTBL(IModel) vtModel;
} OEMAlarmModel;

/*===========================================================================

                    LOCAL FUNCTION DECLARATIONS

===========================================================================*/

// Functions used in the IAlarm virtual pointer table.
static uint32 OEMAlarm_AddRef(IAlarm *po);
static uint32 OEMAlarm_Release(IAlarm *po);
static int OEMAlarm_QueryInterface(IAlarm *po, AEECLSID iid, void **ppo);
static boolean OEMAlarm_AlarmIsActive(IAlarm *po);

// Utility functions
static void OEMAlarm_Notify(int event);
#if defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK)
static void OEMAlarm_AlarmISR(void);
#endif // defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK)

/*===========================================================================

                    STATIC DATA

===========================================================================*/

// Virtual function table for IAlarm.
static const AEEVTBL(IAlarm) IAlarmFuncs  =
{
  OEMAlarm_AddRef,
  OEMAlarm_Release,
  OEMAlarm_QueryInterface,
  OEMAlarm_AlarmIsActive
};

// Indicates whether or not the alarm is active. This can
// be set regardless of whether an OEMAlarm object is created.
static boolean sbAlarmIsActive = FALSE;

// A singleton model.
static OEMAlarmModel sOEMAlarmModel;

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

/*=============================================================================
FUNCTION: OEMAlarm_AddRef

DESCRIPTION:  Increment reference count for this instance of IAlarm.

PARAMETERS:
   IAlarm *po: Pointer to instance of IAlarm.

RETURN VALUE:
   Updated reference count.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 OEMAlarm_AddRef(IAlarm *po)
{
  OEMAlarm *pMe = (OEMAlarm *)(void *)po;

  if (pMe) {
    return (++pMe->m_uRefs);
  }

  return 0;
}

/*=============================================================================
FUNCTION: OEMAlarm_Free

DESCRIPTION:  Free data associated with an IAlarm. Assumes reference count already hit 0.

PARAMETERS:
   OEMAlarm *pMe: Pointer to IAlarm data structure.

RETURN VALUE:
   None.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMAlarm_Free(OEMAlarm *pMe)
{

   FREE((void *)pMe); //lint !e611 !e740   Suppress unusual/suspicious cast

}

/*=============================================================================
FUNCTION: OEMAlarm_Release

DESCRIPTION:  Free an instance of an IAlarm.

PARAMETERS:
   IAlarm *po: Pointer to IAlarm instance.

RETURN VALUE:
   None.

COMMENTS:
   Only frees the data associated with IAlarm if the reference count hits 0.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 OEMAlarm_Release(IAlarm *po)
{
   OEMAlarm *pMe = (OEMAlarm *)(void *)po;

   if (!pMe) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   // Ref count is zero.  Release memory
   OEMAlarm_Free(pMe);

   return (0);
}

/*=============================================================================
FUNCTION: OEMAlarm_QueryInterface

DESCRIPTION:  Query for an interface of IAlarm.

PARAMETERS:
   IAlarm *po: Pointer to IAlarm instance.
   AEECLSID iid: Interface ID
   void **ppo: Pointer to queried object.

RETURN VALUE:
   SUCCESS or ECLASSNOTSUPPORT

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMAlarm_QueryInterface(IAlarm *po, AEECLSID iid, 
                                         void **ppo)
{
   if ((NULL == po) || (NULL == ppo)) {
      return EFAILED;
   }

   switch(iid) {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_ALARM:
         *ppo = (void*)po;
         (void) IQI_AddRef(*ppo);
         return SUCCESS;
      default:
         break;

   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
}

/*=============================================================================
FUNCTION: OEMAlarm_New

DESCRIPTION:  Create an instance of IAlarm.

PARAMETERS:
   IShell * pIShell: Pointer to the IShell.
   AEECLSID ClsId: Class ID to create.
   void ** ppObj: Pointer to the resulting instance of IAlarm.

RETURN VALUE:

COMMENTS:
   This function must not be static because it is called by BREW
   via OEMModTableExt.c.  The object created by this function is
   NOT singleton.

SIDE EFFECTS:
   None.

SEE ALSO:

=============================================================================*/
int OEMAlarm_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)
{
   *ppObj = NULL;

   if (ClsId == AEECLSID_ALARM)
   {
      // Allocate application memory for the class and initialize the
      // virtual function table.
//lint -save -e611 -e740  Suspicious/unusual ptr cast
//lint -save -e826 Suspicious ptr to ptr conversion
      OEMAlarm *po = (OEMAlarm *)AEE_NewClassEx
          ((AEEVTBL(IBase) *)&IAlarmFuncs, sizeof(OEMAlarm), FALSE);
//lint -restore
//lint -restore
      if (NULL == po)
      {
         return ENOMEMORY;
      }

      // Initialize OEMAlarm variables.
      po->m_uRefs = 1;

      // Return new object.
      *ppObj = po;

      return SUCCESS;
   }
   else
   {
      return EUNSUPPORTED;
   }

//lint -save -esym(818, pIShell)  Suppress "pIShell could be ptr to const"
//lint -save -esym(715, pIShell) Suppress "pIShell not referenced"
}
//lint -restore
//lint -restore

/*===========================================================================

FUNCTION OEMAlarm_AlarmIsActive

DESCRIPTION
  This interface returns TRUE if an alarm is set.

DEPENDENCIES
  None

PROTOTYPE
  int OEMAlarm_AlarmIsActive
  (
    IAlarm *               pIAlarm
  )

PARAMETERS
  pIAlarm                 [In]- Pointer to IAlarm object

RETURN VALUE
  TRUE         At least one or more alarm is set.
  FALSE        No alarms are set.

COMMENTS 

SIDE EFFECTS
  None
===========================================================================*/
static boolean OEMAlarm_AlarmIsActive(IAlarm * po)
{
   return sbAlarmIsActive;
//lint -save -esym(715, po) -esym(818, po) Suppress po not ref'd/ could be ptr to const
}
//lint -restore

/*===========================================================================

FUNCTION OEMAlarm_Notify

DESCRIPTION
  Utility function to send out alarm events to applications that
  are listening to the model.

DEPENDENCIES
  None

PROTOTYPE
  int OEMAlarm_Notify
  (
   int                    event
  )

PARAMETERS
  event                   [In]- The event to send out

RETURN VALUE
  None

COMMENTS 

SIDE EFFECTS
  None
===========================================================================*/
static void OEMAlarm_Notify(int event)
{

   if (SYSMODELBASE_NREFS(&sOEMAlarmModel.base) > 0) {
      ModelEvent  evt;
      IModel     *pModel = SYSMODELBASE_TO_IMODEL(&sOEMAlarmModel.base);

      evt.evCode  = (uint32) event;
      evt.dwParam = (uint32) 0;
      evt.pModel  = pModel;

      IMODEL_Notify(pModel, &evt);
   }

}

/*=============================================================================
FUNCTION: OEMAlarm_RegisterISR

DESCRIPTION:  Registers the RTC alarm interrupt service routine.

PARAMETERS:
   None.

RETURN VALUE:
   AEE_SUCCESS on success, or EFAILED otherwise.

COMMENTS:
   This function needs to be called during boot-up prior to BREW start-up completion. The
   handler is required to reset the alarm status flag and the IRQ if the alarm expired. Otherwise
   if the alarm expired, the phone will immediately be powered-on if the phone is ever
   powered-off.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMAlarm_RegisterISR(void)
{

#if defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK) 
   if ( pm_set_irq_handle(PM_RTC_ALRM_IRQ_HDL, OEMAlarm_AlarmISR) != 
      PM_ERR_FLAG__SUCCESS) {
      MSG_HIGH("Failed setting RTC alarm ISR.", 0, 0, 0);
      return EFAILED;
   }
#endif // defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK) && defined(IMAGE_MODEM_PROC)

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMAlarm_SetAlarm

DESCRIPTION:  Set the next most recent expiration time of the RTC alarm with Power Management IC.

PARAMETERS:
   uint32 expiration: time offset in milliseconds from present time for the next RTC alarm, or 0 to
                      indicate no expiration time and any existing RTC alarm should be canceled.

RETURN VALUE:
   AEE_SUCCESS on success, or EFAILED otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMAlarm_SetAlarm(uint32 expiration)
{

#if defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK)
   pm_rtc_display_type display_mode;
   pm_rtc_julian_type time_julian;
   dword time_secs;

   // Reset any existing alarm.
   if (pm_reset_rtc_alarm(PM_RTC_ALARM_1) != PM_ERR_FLAG__SUCCESS) {
      MSG_HIGH("Failed resetting RTC alarm.", 0, 0, 0);
      return EFAILED;
   }

   if (0 == expiration) {
      // All we want to do is cancel an existing alarm.

      if (TRUE == sbAlarmIsActive) {
         sbAlarmIsActive = FALSE;
         OEMAlarm_Notify(IALARM_EVT_ALARM_CLEARED);
      }
      
      return AEE_SUCCESS;
   }

   // Check what display mode RTC is using.
   if (pm_get_rtc_display_mode(&display_mode) != PM_ERR_FLAG__SUCCESS) {
      MSG_HIGH("Failed getting display mode.", 0, 0, 0);
      return EFAILED;
   }

   // Must switch to 24-hour display mode if we are to use the clock conversion helper functions.
   if (display_mode != PM_RTC_24HR_MODE) {
      if (pm_set_rtc_display_mode(PM_RTC_24HR_MODE) != PM_ERR_FLAG__SUCCESS) {
         MSG_HIGH("Failed setting display mode to 24hr.", 0, 0, 0);
         return EFAILED;
      }
   }

   // Get current time in Julian format.
   if (pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time_julian) != PM_ERR_FLAG__SUCCESS) {
      MSG_HIGH("Failed getting current time.", 0, 0, 0);

      if (display_mode != PM_RTC_24HR_MODE) {
         pm_set_rtc_display_mode(display_mode);
      }

      return EFAILED;
   }

   // Convert from Julian time to seconds from base.
   time_secs = clk_julian_to_secs((clk_julian_type *)&time_julian);

   // Add expiration of alarm. Round up to the nearest second.
   time_secs += ( (expiration + (CLK_MS_PER_SEC-1)) / CLK_MS_PER_SEC );

   // Convert from seconds back to Julian time.
   clk_secs_to_julian(time_secs, (clk_julian_type *)&time_julian);

   // Set the RTC alarm.
   if (pm_rtc_alarm_rw_cmd(PM_RTC_SET_CMD, PM_RTC_ALARM_1, &time_julian) !=
      PM_ERR_FLAG__SUCCESS) {
      MSG_HIGH("Failed setting RTC alarm.", 0, 0, 0);

         if (display_mode != PM_RTC_24HR_MODE) {
            pm_set_rtc_display_mode(display_mode);
         }

      return EFAILED;
   }

   // Return back to 12-hour mode if originally was in that mode.
   if (display_mode != PM_RTC_24HR_MODE) {
      pm_set_rtc_display_mode(display_mode);
   }

   if (FALSE == sbAlarmIsActive) {
      sbAlarmIsActive = TRUE;
      OEMAlarm_Notify(IALARM_EVT_ALARM_SET);
   }

   return AEE_SUCCESS;

#else

   if (0 == expiration) {
      if (TRUE == sbAlarmIsActive) {
         sbAlarmIsActive = FALSE;
         OEMAlarm_Notify(IALARM_EVT_ALARM_CLEARED);
      }
   }
   else if (FALSE == sbAlarmIsActive) {
      sbAlarmIsActive = TRUE;
      OEMAlarm_Notify(IALARM_EVT_ALARM_SET);
   }

   return AEE_SUCCESS;

#endif // defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK)

}

#if defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK) 
/*=============================================================================
FUNCTION: OEMAlarm_AlarmISR

DESCRIPTION:  Interrupt Service Routine for handling RTC alarm expiration.

PARAMETERS:
   None.

RETURN VALUE:
   None.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMAlarm_AlarmISR(void)
{
	uint8                  alarm_status = 0;

   if (pm_get_rtc_alarm_status(&alarm_status) != PM_ERR_FLAG__SUCCESS) {
      MSG_HIGH("Failed getting RTC alarm status.", 0, 0, 0);
      return;
   }

   if (alarm_status & PM_RTC_ALARM_1)
   {
      // Alarm #1 has triggered.

      if (pm_reset_rtc_alarm(PM_RTC_ALARM_1) != PM_ERR_FLAG__SUCCESS) {
         MSG_HIGH("Failed resetting RTC alarm 1.", 0, 0, 0);
         return;
      }

      // No action is taken other than resetting and clearing the alarm.

   }
   else {
      // For now, reset any other alarms.

      if (pm_reset_rtc_alarm(PM_RTC_ALL_ALARMS) != PM_ERR_FLAG__SUCCESS) {
         MSG_HIGH("Failed resetting alarms.", 0, 0, 0);
      }

   }

   // Must clear the IRQ *after* resetting the alarms.
   if (pm_clear_irq(PM_RTC_ALRM_IRQ_HDL) != PM_ERR_FLAG__SUCCESS) {
      MSG_HIGH("Failed clearing alarm IRQ.", 0, 0, 0);
   }

}

#endif // defined(FEATURE_PMIC_RTC) && !defined(FEATURE_PMIC_NO_RTC_ALARM) && !defined(PLATFORM_LTK) 

/*=============================================================================
FUNCTION: OEMAlarmModel_New

DESCRIPTION:  Create an instance of IMODEL for IAlarm.

PARAMETERS:
   IShell * pIShell: Pointer to the IShell.
   AEECLSID ClsId: Class ID to create.
   void ** ppObj: Pointer to the resulting instance of IMODEL.

RETURN VALUE:

COMMENTS:
   This function must not be static because it is called by BREW
   via OEMModTableExt.c.  The object created by this function is
   singleton.

SIDE EFFECTS:
   None.

SEE ALSO:

=============================================================================*/
int OEMAlarmModel_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)
{
   *ppObj = NULL;

   if (ClsId == AEECLSID_ALARM_MODEL)
   {

      SysModelBase_Ctor(&sOEMAlarmModel.base, 
                        &sOEMAlarmModel.vtModel,
                        NULL);

      *ppObj = SYSMODELBASE_TO_IMODEL(&sOEMAlarmModel.base);

      return SUCCESS;
   }
   else
   {
      return EUNSUPPORTED;
   }

//lint -save -esym(715, pIShell) Suppress "pIShell not ref'd"
}
//lint -restore

//lint -esym(754, _OEMAlarm::pvt)  Suppress _OEMAlarm::pvt not referenced
//lint -efile(766, customer.h)  Suppress customer.h not used
