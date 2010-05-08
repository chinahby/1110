/*===========================================================================
                           OEMBacklight.c

  This file contains sample source to the IBacklight interface.

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEBacklight.h"
#include "disp.h"
#include "keypad.h"

struct IBacklight {
   const AEEVTBL(IBacklight)     *pvt;
   uint32                        uRef;
   IShell                        *pIShell;
   AEECLSID                      uCls;
};

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
#define FARF_BACKLIGHT 0

#ifdef FARF
#undef FARF
#endif // FARF

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

static uint32 AEEBacklight_AddRef(IBacklight *pme);
static uint32 AEEBacklight_Release(IBacklight *pme);
static int AEEBacklight_QueryInterface(IBacklight *pme, AEECLSID id, void **ppo);
static int AEEBacklight_Enable(IBacklight *pme);
static int AEEBacklight_Disable(IBacklight *pme);
static boolean AEEBacklight_IsEnabled(IBacklight *pme);
static int AEEBacklight_GetBacklightInfo(IBacklight *pme, AEEBacklightInfo * pBacklightInfo);
static int AEEBacklight_GetBrightnessLevel(IBacklight *pme, uint32* pdwBrightnessLevel);
static int AEEBacklight_SetBrightnessLevel(IBacklight *pme, uint32 dwBrightnessLevel);

static const IBacklightVtbl gvtIBacklight = {
   AEEBacklight_AddRef,
   AEEBacklight_Release,
   AEEBacklight_QueryInterface,
   AEEBacklight_Enable,
   AEEBacklight_Disable,
   AEEBacklight_IsEnabled,
   AEEBacklight_GetBacklightInfo,
   AEEBacklight_GetBrightnessLevel,
   AEEBacklight_SetBrightnessLevel
};

static boolean gbBacklightDisplay1Enabled = TRUE;
static boolean gbBacklightDisplay1Initialized = FALSE;
static uint32  gdwBacklightDisplay1Level = 0;

#ifdef FEATURE_BACKLIGHT_DISPLAY2
static boolean gbBacklightDisplay2Enabled = TRUE;
static boolean gbBacklightDisplay2Initialized = FALSE;
static uint32  gdwBacklightDisplay2Level = 0;
#endif

static boolean gbBacklightKeypadDisplayEnabled = FALSE;

/*===========================================================================

Function: AEEBacklight_New

Description:
   IBacklight constructor

===========================================================================*/
int AEEBacklight_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{
   int nErr = SUCCESS;
   IBacklight * pme = NULL;

   FARF(BACKLIGHT, ("==>  IBacklight_New"));

   if (!ISHELL_CheckPrivLevel(pIShell, uCls, TRUE)) {
      return EPRIVLEVEL;
   }

   pme = (IBacklight *)MALLOC(sizeof(IBacklight));

   *ppif = pme;

   if (pme)
   {
      pme->pvt = &gvtIBacklight;

      pme->uRef = 1;
      pme->pIShell = pIShell;
      ISHELL_AddRef(pIShell);
      pme->uCls = uCls;
   }
   else
   {
      nErr = ENOMEMORY;
   }

   return nErr;
}

/*===========================================================================

Function: AEEBacklight_AddRef

Description:
   Increases reference count

===========================================================================*/
static uint32 AEEBacklight_AddRef(IBacklight *pme)
{
   FARF(BACKLIGHT, ("==>  AEEBacklight_AddRef"));
   return ++pme->uRef;
}

/*===========================================================================

Function: AEEBacklight_Release

Description:
   Decrements reference count. Frees object when reference count reaches 0.

===========================================================================*/
static uint32 AEEBacklight_Release(IBacklight *pme)
{
   FARF(BACKLIGHT, ("==>  AEEBacklight_Release"));

   if (--pme->uRef) {
      return pme->uRef;
   }

   RELEASEIF(pme->pIShell);
   FREEIF(pme);

   return 0;
}

/*===========================================================================

Function: AEEBacklight_QueryInterface

Description:
   Query Interface

===========================================================================*/
static int AEEBacklight_QueryInterface(IBacklight *pme, AEECLSID id, void **ppo)
{
   FARF(BACKLIGHT, ("==>  AEEBacklight_QueryInterface"));

   if (!ppo)
   {
      return EBADPARM;
   }

   if ((id == AEECLSID_QUERYINTERFACE) ||
       (id == pme->uCls))
   {
      *ppo = pme;
      IQI_AddRef(*ppo);
      return SUCCESS;
   }
   else
   {
      return ECLASSNOTSUPPORT;
   }
}

/*===========================================================================

Function: AEEBacklight_Enable

Description:
   Turns backlight ON

===========================================================================*/
static int AEEBacklight_Enable(IBacklight *pme)
{
   int nErr = SUCCESS;
   FARF(BACKLIGHT, ("==>  AEEBacklight_Enable"));

   switch (pme->uCls)
   {
      AEEBacklightInfo backlightInfo;

      case AEECLSID_BACKLIGHT_DISPLAY1:
         if(TRUE == gbBacklightDisplay1Initialized)
         {
            disp_set_backlight((byte)gdwBacklightDisplay1Level);
            gbBacklightDisplay1Enabled = TRUE;
            nErr = SUCCESS;
         }
         else if(SUCCESS == (nErr = IBACKLIGHT_GetBacklightInfo(pme, &backlightInfo)))
         {
            disp_set_backlight((byte)backlightInfo.dwDefaultBrightnessLevel);
            gbBacklightDisplay1Enabled = TRUE;
         }
         break;

#ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         if(TRUE == gbBacklightDisplay2Initialized)
         {
            disp_set_backlight((byte)gdwBacklightDisplay2Level);
            gbBacklightDisplay2Enabled = TRUE;
            nErr = SUCCESS;
         }
         else if(SUCCESS == (nErr = IBACKLIGHT_GetBacklightInfo(pme, &backlightInfo)))
         {
            disp_set_backlight((byte)backlightInfo.dwDefaultBrightnessLevel);
            gbBacklightDisplay2Enabled = TRUE;
         }
         break;
#endif

      case AEECLSID_BACKLIGHT_KEYPAD:
#ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
         {
            byte val = KEYPAD_BACKLIGHT_LVL_4;
            keypad_set_backlight(val);
         }
#else
         keypad_set_backlight(TRUE);
#endif
         gbBacklightKeypadDisplayEnabled = TRUE;
         nErr = SUCCESS;
         break;

      default:
         nErr = EUNSUPPORTED;
         break;
   }
   return nErr;
}

/*===========================================================================

Function: AEEBacklight_Disable

Description:
   Turns backlight OFF

===========================================================================*/
static int AEEBacklight_Disable(IBacklight *pme)
{
   int nErr = SUCCESS;
   FARF(BACKLIGHT, ("==>  AEEBacklight_Disable"));

   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         gbBacklightDisplay1Enabled = FALSE;
         disp_set_backlight(0);
         nErr = SUCCESS;
         break;

#ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         gbBacklightDisplay2Enabled = FALSE;
         disp_set_backlight2(0);
         nErr = SUCCESS;
         break;
#endif

      case AEECLSID_BACKLIGHT_KEYPAD:
         gbBacklightKeypadDisplayEnabled = FALSE;
         keypad_set_backlight(0);
         nErr = SUCCESS;
         break;

      default:
         nErr = EUNSUPPORTED;
         break;
   }
   return nErr;
}

/*===========================================================================

Function: AEEBacklight_IsEnabled

Description:
   Determines if backlight is turned ON

===========================================================================*/
static boolean AEEBacklight_IsEnabled(IBacklight *pme)
{
   boolean bStatus = FALSE;
   FARF(BACKLIGHT, ("==>  AEEBacklight_IsEnabled"));

   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         bStatus = gbBacklightDisplay1Enabled;
         break;

#ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         bStatus = gbBacklightDisplay2Enabled;
         break;
#endif

      case AEECLSID_BACKLIGHT_KEYPAD:
         bStatus = gbBacklightKeypadDisplayEnabled;
         break;

      default:
         break;
   }
   return bStatus;
}

/*===========================================================================

Function: AEEBacklight_GetBacklightInfo

Description:
   Returns information about backlight

===========================================================================*/
static int AEEBacklight_GetBacklightInfo(IBacklight *pme, AEEBacklightInfo * pBacklightInfo)
{
   int nErr;
   disp_info_type disp_info;
   FARF(BACKLIGHT, ("==>  AEEBacklight_GetBacklightInfo"));

   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         disp_info = disp_get_info();
         nErr = SUCCESS;
         break;

   #ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         disp_info = disp_get_info2();
         nErr = SUCCESS;
         break;
   #endif

      default:
         nErr = EUNSUPPORTED;
         break;
   }

   if(SUCCESS == nErr)
   {
      pBacklightInfo->dwBacklightColor = disp_info.contrast_default;
      pBacklightInfo->dwDefaultBrightnessLevel = disp_info.backlight_default;
      pBacklightInfo->dwMaxBrightnessLevel = disp_info.backlight_max;
      pBacklightInfo->dwMinBrightnessLevel = disp_info.backlight_min;
   }


   return nErr;
}

/*===========================================================================

Function: AEEBacklight_GetBrightnessLevel

Description:
   Returns backlight brightness level

===========================================================================*/
static int AEEBacklight_GetBrightnessLevel(IBacklight *pme, uint32* pdwBrightnessLevel)
{
   int nErr = EFAILED;
   FARF(BACKLIGHT, ("==>  AEEBacklight_GetBrightnessLevel"));

   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         if(FALSE == gbBacklightDisplay1Enabled)
         {
            *pdwBrightnessLevel = 0;
            nErr = SUCCESS;
         }
         else if(TRUE == gbBacklightDisplay1Initialized)
         {
            *pdwBrightnessLevel = gdwBacklightDisplay1Level;
            nErr = SUCCESS;
         }
         break;

   #ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         if(FALSE == gbBacklightDisplay2Enabled)
         {
            *pdwBrightnessLevel = 0;
            nErr = SUCCESS;
         }
         else if(TRUE == gbBacklightDisplay2Initialized)
         {
            *pdwBrightnessLevel = gdwBacklightDisplay2Level;
            nErr = SUCCESS;
         }
         break
   #endif

      default:
         nErr = EUNSUPPORTED;
         break;
   }

   if(EFAILED == nErr)
   {
      //Backlight status not initialized assuming default
      AEEBacklightInfo backlightInfo;
      if(SUCCESS == (nErr = IBACKLIGHT_GetBacklightInfo(pme, &backlightInfo)))
      {
         *pdwBrightnessLevel = backlightInfo.dwDefaultBrightnessLevel;
      }
   }

   return nErr;
}

/*===========================================================================

Function: AEEBacklight_SetBrightnessLevel

Description:
   Sets backlight brightness level

===========================================================================*/
static int AEEBacklight_SetBrightnessLevel(IBacklight *pme, uint32 dwBrightnessLevel)
{
   int nErr;
   FARF(BACKLIGHT, ("==>  AEEBacklight_SetBrightnessLevel"));

   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         gdwBacklightDisplay1Level = dwBrightnessLevel;
         gbBacklightDisplay1Initialized = TRUE;
         disp_set_backlight((byte)dwBrightnessLevel);
         nErr = SUCCESS;
         break;

   #ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         gdwBacklightDisplay2Level = dwBrightnessLevel;
         gbBacklightDisplay2Initialized = TRUE;
         disp_set_backlight((byte)dwBrightnessLevel);
         nErr = SUCCESS;
         break;
   #endif

      default:
         nErr = EUNSUPPORTED;
         break;
   }

   return nErr;
}
