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

extern void AEESIM_SetBacklight(boolean bOn, AVSType eST);
extern boolean AEESIM_IsBacklightEnabled(AVSType eST);

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
      case AEECLSID_BACKLIGHT_DISPLAY1:
         AEESIM_SetBacklight(TRUE, 0);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY2:
         AEESIM_SetBacklight(TRUE, 1);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY3:
         AEESIM_SetBacklight(TRUE, 2);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY4:
         AEESIM_SetBacklight(TRUE, 3);
         break;

      default:
         nErr = EFAILED;
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
         AEESIM_SetBacklight(FALSE, 0);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY2:
         AEESIM_SetBacklight(FALSE, 1);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY3:
         AEESIM_SetBacklight(FALSE, 2);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY4:
         AEESIM_SetBacklight(FALSE, 3);
         break;

      default:
         nErr = EFAILED;
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
         bStatus = AEESIM_IsBacklightEnabled(0);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY2:
         bStatus = AEESIM_IsBacklightEnabled(1);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY3:
         bStatus = AEESIM_IsBacklightEnabled(2);
         break;

      case AEECLSID_BACKLIGHT_DISPLAY4:
         bStatus = AEESIM_IsBacklightEnabled(3);
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
   FARF(BACKLIGHT, ("==>  AEEBacklight_GetBacklightInfo"));

   // OEMs need to implement this function
   return EUNSUPPORTED;
}

/*===========================================================================

Function: AEEBacklight_GetBrightnessLevel

Description:
   Returns backlight brightness level
 
===========================================================================*/
static int AEEBacklight_GetBrightnessLevel(IBacklight *pme, uint32* pdwBrightnessLevel)
{
   FARF(BACKLIGHT, ("==>  AEEBacklight_GetBrightnessLevel"));

   // OEMs need to implement this function
   return EUNSUPPORTED;
}

/*===========================================================================

Function: AEEBacklight_SetBrightnessLevel

Description:
   Sets backlight brightness level
 
===========================================================================*/
static int AEEBacklight_SetBrightnessLevel(IBacklight *pme, uint32 dwBrightnessLevel)
{
   FARF(BACKLIGHT, ("==>  AEEBacklight_SetBrightnessLevel"));

   // OEMs need to implement this function
   return EUNSUPPORTED;
}