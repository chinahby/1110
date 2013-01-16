/*===========================================================================
                           OEMBacklight.c

  This file contains sample source to the IBacklight interface.

        Copyright ? 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEBacklight.h"
#include "disp.h"
#include "keypad.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "ui.h"
#include "OEMSVC.h"
#include "AEE_OEMDispatch.h"
#include "AEEIDisplay.h"
#include "sleep.h"
#include "clk.h"

#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
#define MSAFTER_DISABLEBACKLIGHT_TIMER    60000
#endif
struct IBacklight {
   const AEEVTBL(IBacklight)     *pvt;
   uint32                        uRef;
   IShell                        *pIShell;
   AEECLSID                      uCls;
#ifdef FEATURE_LED_CONTROL
    sig_led_type                    sigLedType;
    byte                            sigLedSequence;
    hs_sig_led_onoff_type           sigLedOnoff;
#endif
//wangliang add!  2010-06-10
#ifdef FEATURE_TORCH_SUPPORT
	byte							TorchOnoff;
#endif
    byte                            btBKLevel;      // 背光级别，非具体值
};

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
#define FARF_BACKLIGHT 0

// 将背光级别换算到具体值的比例
#define SCAL_KEYPAD_LIGHT   35
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

static void AEEBacklight_CancelDisableTimer(IBacklight *pme);
static void AEEBacklight_SetDisableTimer(IBacklight *pme);
static void AEEBacklight_DisableTimer(void *pUser);
static void AEEBacklight_TurnOn(IBacklight *pme);
static void AEEBacklight_TurnOff(IBacklight *pme);

//wangliang add!  2010-06-10
#ifdef FEATURE_TORCH_SUPPORT
static void AEEBacklight_TurnOnTorch(IBacklight *pme);
static void AEEBacklight_TurnOffTorch(IBacklight *pme);
#endif

static int AEEBacklight_PreDisable(IBacklight *pme);
static void AEEBacklight_PreDisableTimer(void *pUser);
static void AEEBacklight_CancelPreDisableTimer(IBacklight *pme);

#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
static void AEEBacklight_NotifyAutoExitApp(IBacklight *pme);
static void AEEBacklight_SetNotifyTimer(IBacklight *pme);
static void AEEBacklight_CancelNotifyTimer(IBacklight *pme);
#endif //FEATURE_AUTOEXIT_AFTER_BLDISABLE
#ifdef FEATURE_LED_CONTROL
static int AEEBacklight_SigLedEnable(IBacklight *pme,sig_led_type SigLed_type);
static int AEEBacklight_SigLedDisable(IBacklight *pme);
static void AEEBacklight_SigLed_EnableTimer(void *pUser);
static int AEEBacklight_SigLedFlash_Enable(IBacklight *pme);
static int AEEBacklight_SigLedFlash_Disable(IBacklight *pme);
static void AEEBacklight_SigLedFlash_DisableTimer(void *pUser);
static void AEEBacklight_SigLedFlash_EnableTimer(void *pUser);
#endif//#ifdef FEATURE_LED_CONTROL
static const IBacklightVtbl gvtIBacklight = {
   AEEBacklight_AddRef,
   AEEBacklight_Release,
   AEEBacklight_QueryInterface,
   AEEBacklight_Enable,
   AEEBacklight_Disable,
   AEEBacklight_IsEnabled,
   AEEBacklight_GetBacklightInfo,
   AEEBacklight_GetBrightnessLevel,
   AEEBacklight_SetBrightnessLevel,
   AEEBacklight_TurnOn,     
   AEEBacklight_TurnOff  
#ifdef FEATURE_LED_CONTROL 
  ,AEEBacklight_SigLedEnable
  ,AEEBacklight_SigLedDisable
#endif
//wangliang add!  2010-06-10
#ifdef FEATURE_TORCH_SUPPORT
	,AEEBacklight_TurnOnTorch
	,AEEBacklight_TurnOffTorch
#endif

};

static IBacklight  gDisplay1Backlight={0};
static IBacklight  gKeyBacklight={0};
static boolean gbBacklightDisplay1Enabled = TRUE;
static boolean gbBacklightDisplay1Initialized = FALSE;
static uint32  gdwBacklightDisplay1Level = 0;

#ifdef FEATURE_BACKLIGHT_DISPLAY2
static IBacklight  gDisplay21Backlight={0};
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

   //pme = (IBacklight *)MALLOC(sizeof(IBacklight));
   switch (uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         pme = &gDisplay1Backlight;
         break;

#ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         pme = &gDisplay2Backlight;
         break;
#endif

      case AEECLSID_BACKLIGHT_KEYPAD:
         pme = &gKeyBacklight;
         break;

      default:
         return  EUNSUPPORTED;
   }
   
   *ppif = pme;

   if (0 == pme->uRef)
   {
      pme->pvt = &gvtIBacklight;

      pme->uRef = 1;
      pme->pIShell = pIShell;
      ISHELL_AddRef(pIShell);
      pme->uCls = uCls;

#ifdef FEATURE_TORCH_SUPPORT
		pme->TorchOnoff = FALSE;
#endif
      // 背光相关初始化--背光级别需以后单独设置
      OEM_SVCGetConfig(CFGI_BACKLIGHT_LEVEL, 
                       &pme->btBKLevel, 
                       sizeof(pme->btBKLevel));
                               
      if (0 == pme->btBKLevel)
      {
         pme->btBKLevel = OEMNV_BACKLIGHT_LEVEL;
      }
      AEEBacklight_SetBrightnessLevel(pme, pme->btBKLevel);
   }
   else
   {
      pme->uRef++;
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
   
   AEEBacklight_CancelDisableTimer(pme);
   AEEBacklight_CancelPreDisableTimer(pme);
#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
   AEEBacklight_CancelNotifyTimer(pme);
#endif
   RELEASEIF(pme->pIShell);
   //FREEIF(pme);
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

static void AEEBacklight_FlushDisplay(IBacklight *pme, AEECLSID clsDisplay)
{
    IDisplay *pDisplay;
    if(SUCCESS == ISHELL_CreateInstance(pme->pIShell,clsDisplay,(void **)&pDisplay))
    {
        IBitmap *pDevBmp;
        if(SUCCESS == IDisplay_GetDeviceBitmap(pDisplay,&pDevBmp))
        {
            AEEBitmapInfo info;

            if(SUCCESS == IBitmap_GetInfo(pDevBmp,&info,sizeof(info)))
            {
                AEERect rc;
                
                rc.x  = 0;
                rc.y  = 0;
                rc.dx = info.cx;
                rc.dy = info.cy;
                IBitmap_Invalidate(pDevBmp, &rc);
            }
            
            IBitmap_Release(pDevBmp);
        }
        IDisplay_UpdateEx(pDisplay, FALSE);
        IDisplay_Release(pDisplay);
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
   
   AEEBacklight_CancelDisableTimer(pme);
   AEEBacklight_CancelPreDisableTimer(pme);
#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
   AEEBacklight_CancelNotifyTimer(pme);
#endif

   if (AEEBacklight_IsEnabled(pme))
   {
      AEEBacklight_SetDisableTimer(pme);
      switch (pme->uCls)
	  {
	   	  AEEBacklightInfo backlightInfo;
	      case AEECLSID_BACKLIGHT_DISPLAY1:
	      	 if(TRUE == gbBacklightDisplay1Initialized)
	         {
	            disp_set_backlight((byte)gdwBacklightDisplay1Level);
	            nErr = SUCCESS;
	         }
	         else if(SUCCESS == (nErr = IBACKLIGHT_GetBacklightInfo(pme, &backlightInfo)))
	         {
	            disp_set_backlight((byte)backlightInfo.dwDefaultBrightnessLevel);
	         }

	         break;

	      case AEECLSID_BACKLIGHT_KEYPAD:
	         {
	            byte val = KEYPAD_BACKLIGHT_LVL_4;
	            keypad_set_backlight(val);
	         }
	         break;

	      default:
	         break;
	   }
      return SUCCESS;
   }
   
   switch (pme->uCls)
   {
      AEEBacklightInfo backlightInfo;

      case AEECLSID_BACKLIGHT_DISPLAY1:
         disp_on();
		 #if defined(FEATURE_DISP_240X320)
		 clk_busy_wait(150*1000);
		 #elif defined(FEATURE_VERSION_C316)
         clk_busy_wait(100*1000);
		 #endif
         // Update Screen
         AEEBacklight_FlushDisplay(pme, AEECLSID_DISPLAY1);
         
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
#ifdef FEATRUE_KEY_PAD_CTL
         {
            Key_pad_Cfg KeyPad_control;
            JulianType  jDate;
            uint32 current_time;
            uint32 from_time;   
            uint32 to_time; 
            boolean bOn = FALSE;  
                
            OEM_GetConfig(CFGI_KEY_PAD_CTL, (void*)&KeyPad_control, sizeof(Key_pad_Cfg));
            current_time = GETTIMESECONDS();             
            GETJULIANDATE(current_time, &jDate);
            from_time = KeyPad_control.from_Time;
            to_time = KeyPad_control.to_Time;
            ERR( "OEM_GetConfig time is %d  end time is  %d", from_time, to_time, 0 );
            from_time = from_time / 1000;
            to_time = to_time / 1000;
            jDate.wHour = 0;
            jDate.wMinute = 0;
            jDate.wSecond = 0;
            from_time += JULIANTOSECONDS(&jDate);
            to_time += JULIANTOSECONDS(&jDate);    
    		if(TRUE== KeyPad_control.bStateOn)
    		{
    			if(current_time > 630720000)
    			{
    				if(to_time < from_time)
    				{
    					if((current_time < to_time) ||(current_time > from_time))
    					{
    						bOn = TRUE;
    					}
    				}
    				else if((to_time > from_time)&&(current_time > from_time) && (current_time < to_time))
    				{
    					bOn = TRUE;
    				}
    			}
    			else
			    {
    				bOn = TRUE;
    			}
    		}

            
            keypad_set_backlight((bOn ? pme->btBKLevel*SCAL_KEYPAD_LIGHT : 0));
            if (bOn == FALSE)
	        {
	            return SUCCESS;
	        }
         }
#else
         {
            byte val = KEYPAD_BACKLIGHT_LVL_4;
            keypad_set_backlight(val);
         }
#endif
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

   

   AEEBacklight_SetDisableTimer(pme);
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

   AEEBacklight_CancelPreDisableTimer(pme);
   AEEBacklight_CancelDisableTimer(pme);
   
   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         gbBacklightDisplay1Enabled = FALSE;
         disp_set_backlight(0);
         nErr = SUCCESS;
#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
         AEEBacklight_CancelNotifyTimer(pme);
         AEEBacklight_SetNotifyTimer(pme);
#endif
         disp_off();
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

Function: AEEBacklight_Disable

Description:
   Turns backlight OFF

===========================================================================*/
static int AEEBacklight_PreDisable(IBacklight *pme)
{
   int nErr = SUCCESS;
   AEEBacklightInfo backlightInfo;
   
   FARF(BACKLIGHT, ("==>  AEEBacklight_PreDisable"));

   AEE_CancelTimer(AEEBacklight_PreDisableTimer, pme);
   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
      	 if(TRUE == gbBacklightDisplay1Initialized)
         {
            disp_set_backlight((byte)1);
            nErr = SUCCESS;
         }
         else if(SUCCESS == (nErr = IBACKLIGHT_GetBacklightInfo(pme, &backlightInfo)))
         {
            disp_set_backlight((byte)1);
         }
         nErr = SUCCESS;

         break;

#ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         nErr = SUCCESS;
         break;
#endif

      case AEECLSID_BACKLIGHT_KEYPAD:
         {
            byte val = 1;
            keypad_set_backlight(val);
         }

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
   ERR("pme->uCls = %d",pme->uCls,0,0);
   switch (pme->uCls)
   {
      case AEECLSID_BACKLIGHT_DISPLAY1:
         gdwBacklightDisplay1Level = dwBrightnessLevel;
         gbBacklightDisplay1Initialized = TRUE;
         disp_set_backlight((byte)dwBrightnessLevel);
         nErr = SUCCESS;
         pme->btBKLevel = (byte)dwBrightnessLevel;
         break;

   #ifdef FEATURE_BACKLIGHT_DISPLAY2
      case AEECLSID_BACKLIGHT_DISPLAY2:
         gdwBacklightDisplay2Level = dwBrightnessLevel;
         gbBacklightDisplay2Initialized = TRUE;
         disp_set_backlight((byte)dwBrightnessLevel);
         nErr = SUCCESS;
         pme->btBKLevel = (byte)dwBrightnessLevel;
         break;
   #endif

      default:
         nErr = EUNSUPPORTED;
         break;
   }

   return nErr;
}

/*==============================================================================
Function:
    AEEBacklight_CancelDisableTimer

Description:
    Cancel disable back light relative timer.

Parameter:
    pUser [in]: IBacklight *

Return value:
    none

Remark:

==============================================================================*/
static void AEEBacklight_CancelDisableTimer(IBacklight *pme)
{
    AEE_CancelTimer(AEEBacklight_DisableTimer, pme);
}

static void AEEBacklight_CancelPreDisableTimer(IBacklight *pme)
{
    AEE_CancelTimer(AEEBacklight_PreDisableTimer, pme);
}

/*==============================================================================
Function:
    AEEBacklight_SetDisableTimer

Description:
    Setup disable back light relative timer.

Parameter:
    pUser [in]: IBacklight *

Return value:
    none

Remark:

==============================================================================*/
static void AEEBacklight_SetDisableTimer(IBacklight *pme)
{
    byte  nVal=0;
    int32 nMSecs=0;
    
    OEM_SVCGetConfig(CFGI_BACK_LIGHT, &nVal, sizeof(nVal));
    
    if ((nVal > 0) && (nVal <100))
    {
        nMSecs = nVal*1000;
        AEE_SetSysTimer(nMSecs+5000, AEEBacklight_DisableTimer, pme);
        AEE_SetSysTimer(nMSecs, AEEBacklight_PreDisableTimer, pme);
    }
}

/*==============================================================================
Function:
    AEEBacklight_DisableTimer

Description:
    Disable display1 back light callback funtion.

Parameter:
    pUser [in]: IBacklight *

Return value:
    none

Remark:

==============================================================================*/
static void AEEBacklight_DisableTimer(void *pUser)
{
    IBacklight *pMe = (IBacklight*)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    if (AEEBacklight_IsEnabled(pMe) || pMe->uCls == AEECLSID_BACKLIGHT_KEYPAD)
    {
        if(AEE_Event(EVT_NO_CLOSEBACKLIGHT,0,0))
        {
            AEEBacklight_SetDisableTimer(pMe);
        }
        else
        {
            AEEBacklight_Disable(pMe);
        }
    }
}

/*==============================================================================
Function:
    AEEBacklight_DisableTimer

Description:
    Disable display1 back light callback funtion.

Parameter:
    pUser [in]: IBacklight *

Return value:
    none

Remark:

==============================================================================*/
static void AEEBacklight_PreDisableTimer(void *pUser)
{
    IBacklight *pMe = (IBacklight*)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    if (AEEBacklight_IsEnabled(pMe) || pMe->uCls == AEECLSID_BACKLIGHT_KEYPAD)
    {
        if(AEE_Event(EVT_NO_CLOSEBACKLIGHT,1,0))
        {
            AEEBacklight_SetDisableTimer(pMe);
        }
        else
        {
            AEEBacklight_PreDisable(pMe);
        }
    }
}


//wangliang add!  2010-06-10
#ifdef FEATURE_TORCH_SUPPORT
static void AEEBacklight_TurnOnTorch(IBacklight *pme)
{
	if (NULL == pme)
    {
        return;
    }

    if ( pme->TorchOnoff == TRUE )
    {
		return;
    }

    pme->TorchOnoff = TRUE;
    disp_set_torch();
}

static void AEEBacklight_TurnOffTorch(IBacklight *pme)
{
	if (NULL == pme)
    {
        return;
    }

    if ( pme->TorchOnoff == FALSE )
    {
		return;
    }

	pme->TorchOnoff = FALSE;
    disp_clear_torch();
}
#endif


static void AEEBacklight_TurnOn(IBacklight *pme)
{
    if (NULL == pme)
    {
        return;
    }
    
    if (AEEBacklight_IsEnabled(pme))
    {
#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
        AEEBacklight_CancelNotifyTimer(pme);
#endif
        AEEBacklight_CancelDisableTimer(pme);
        return;
    }
    
    AEEBacklight_Enable(pme);
    AEEBacklight_CancelDisableTimer(pme);
#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
    AEEBacklight_CancelNotifyTimer(pme);
#endif
}

static void AEEBacklight_TurnOff(IBacklight *pme)
{   
    if (NULL == pme)
    {
        return;
    }
    
    AEEBacklight_Disable(pme);
}

#ifdef FEATURE_LED_CONTROL
/*===========================================================================

Function: AEEBacklight_SigLedEnable

Description:
   Turns led ON

===========================================================================*/
static int AEEBacklight_SigLedEnable(IBacklight *pme,sig_led_type SigLed_type)
{
    byte led_ctr;
    int nErr = SUCCESS;
    int32 repeattimer=0;
    FARF(BACKLIGHT, ("==>  AEEBacklight_LEDEnable"));
    
    if (pme->uCls != AEECLSID_BACKLIGHT)
    {
        return nErr;
    }
    
    OEM_SVCGetConfig(CFGI_LED_CONTROL, &led_ctr, sizeof(byte));

    switch (SigLed_type)
    {
        case SIG_LED_SMS:
            if (led_ctr&0x1)
            {
                repeattimer = 10*1000;
                pme->sigLedSequence = 3;
            }
            else 
            {
                return FALSE;
            }
            break;

        case SIG_LED_INCOMING_CALL:
            if (led_ctr&0x2)
            {
                repeattimer = 1300;
                pme->sigLedSequence = 4;
            }
            else 
            {
                return FALSE;
            }        
            break;

        case SIG_LED_MISSED_CALL:
            if (led_ctr &0x2)
            {
                repeattimer = 10*1000;
                pme->sigLedSequence = 3;
            }
            else 
            {
                return FALSE;
            }        
            break;

        case SIG_LED_NETWORK:
#if defined( FEATURE_HAS_NETWORK_LED_CONTROL)
            if (led_ctr &0x4)
            {
                repeattimer = 8*1000;
                pme->sigLedSequence = 0;
            }
            else
            {
                return FALSE;
            }
            break;
#else
            return FALSE;
#endif

        case SIG_LED_ALARM:
            if (led_ctr &0x8)
            {
#if defined( FEATURE_HAS_NETWORK_LED_CONTROL)
                pme->sigLedSequence = 3;
                repeattimer = 1900;
#else
                pme->sigLedSequence = 2;
                repeattimer = 1300;
#endif
            }
            else 
            {
                return FALSE;
            }              
            break;
            
        default:
            break;
    }

    // if return in switch(), pme->sigLedType should not be changed
    pme->sigLedType = SigLed_type;

    AEEBacklight_SigLedFlash_Enable(pme);
    AEE_SetSysTimer(repeattimer, AEEBacklight_SigLed_EnableTimer, pme);
    
    return nErr;
}

/*===========================================================================

Function: AEEBacklight_SigLedDisable

Description:
   Turns led off

===========================================================================*/
static int AEEBacklight_SigLedDisable(IBacklight *pme)
{
    int nErr = SUCCESS;
    FARF(BACKLIGHT, ("==>  AEEBacklight_LEDDisable"));
    
    if (pme->uCls != AEECLSID_BACKLIGHT)
    {
        return nErr;
    }

    AEEBacklight_SigLedFlash_Disable(pme);
    AEE_CancelTimer(AEEBacklight_SigLed_EnableTimer, pme);
    return nErr;    
}

/*==============================================================================
Function:
    AEEBacklight_SigLedEnableTimer

Description:
    Disable sig led callback funtion.

Parameter:
    pUser [in]: IBacklight *

Return value:
    none

Remark:

==============================================================================*/
static void AEEBacklight_SigLed_EnableTimer(void *pUser)
{
    IBacklight *pme = (IBacklight*)pUser;

    if (NULL == pme)
    {
        return;
    }
    
    if (pme->uCls != AEECLSID_BACKLIGHT)
    {
        return;
    }

    AEEBacklight_SigLedEnable(pme,pme->sigLedType);
}

/*===========================================================================

Function: AEEBacklight_SigLedEnable

Description:
   Turns led ON

===========================================================================*/
static int AEEBacklight_SigLedFlash_Enable(IBacklight *pme)
{
    int nErr = SUCCESS;
    int32 lightlong=0;
    FARF(BACKLIGHT, ("==>  AEEBacklight_SigLedFlash_Eable"));
    
    if (pme->uCls != AEECLSID_BACKLIGHT)
    {
        return nErr;
    }
    
    switch(pme->sigLedType)
    {
        case SIG_LED_SMS:
#if defined(FEATURE_HAS_NETWORK_LED_CONTROL)
            pme->sigLedOnoff = HS_SIG_LED_COLOR_RED_ON;
#else
            pme->sigLedOnoff = HS_SIG_LED_COLOR_WHITE_ON;
#endif
            lightlong = 500;
            break;

        case SIG_LED_INCOMING_CALL:
            pme->sigLedOnoff = HS_SIG_LED_COLOR_ORANGE_ON;
            lightlong = 200;
            break;

        case SIG_LED_MISSED_CALL:
            pme->sigLedOnoff = HS_SIG_LED_COLOR_ORANGE_ON;
            lightlong = 500;
            break;

        case SIG_LED_NETWORK:
            pme->sigLedOnoff = HS_SIG_LED_COLOR_WHITE_ON;
            lightlong = 100;
            break;

        case SIG_LED_ALARM:
            {
#if defined(FEATURE_HAS_NETWORK_LED_CONTROL)
            hs_sig_led_onoff_type led[] =
				{
                    HS_SIG_LED_COLOR_ORANGE_ON,
                    HS_SIG_LED_COLOR_WHITE_ON,
                    HS_SIG_LED_COLOR_RED_ON
            	};
                pme->sigLedOnoff = led[pme->sigLedSequence - 1];
#else
                hs_sig_led_onoff_type led[] = 
                        {
                            HS_SIG_LED_COLOR_WHITE_ON,
                            HS_SIG_LED_COLOR_ORANGE_ON
                        };
                pme->sigLedOnoff = led[pme->sigLedSequence - 1];
#endif
                lightlong = 500;
            }
            break;
            
        default:   
            break;
    }

    nErr = disp_set_sigled_cmd(pme->sigLedOnoff);
    if (pme->sigLedSequence != 0)
    {
        pme->sigLedSequence--;
        AEE_SetSysTimer(lightlong, AEEBacklight_SigLedFlash_DisableTimer, pme);
    }
    else
    {
        AEEBacklight_SigLedFlash_Disable(pme);
    }
    
    return nErr;
}

/*===========================================================================

Function: AEEBacklight_SigLedFlash_Disable

Description:
   Turns led off

===========================================================================*/
static int AEEBacklight_SigLedFlash_Disable(IBacklight *pme)
{
    int nErr = SUCCESS; 
    FARF(BACKLIGHT, ("==>  AEEBacklight_SigLedFlash_Disable"));
    
    if (pme->uCls != AEECLSID_BACKLIGHT)
    {
        return nErr;
    }
    
    switch( pme->sigLedType)
    {
        case SIG_LED_SMS:
#if defined(FEATURE_HAS_NETWORK_LED_CONTROL)
            pme->sigLedOnoff = HS_SIG_LED_COLOR_RED_OFF;
#else
            pme->sigLedOnoff = HS_SIG_LED_COLOR_WHITE_OFF;
#endif
            break;

        case SIG_LED_INCOMING_CALL:
            pme->sigLedOnoff = HS_SIG_LED_COLOR_ORANGE_OFF;
            break;

        case SIG_LED_MISSED_CALL:
            pme->sigLedOnoff = HS_SIG_LED_COLOR_ORANGE_OFF;
            break;

        case SIG_LED_NETWORK:
            pme->sigLedOnoff = HS_SIG_LED_COLOR_WHITE_OFF;
            break;

        case SIG_LED_ALARM:
            {
#if defined( FEATURE_HAS_NETWORK_LED_CONTROL)
                hs_sig_led_onoff_type led[] = 
                        {
                            HS_SIG_LED_COLOR_WHITE_OFF,
                            HS_SIG_LED_COLOR_ORANGE_OFF,
                            HS_SIG_LED_COLOR_RED_OFF
                        };
                pme->sigLedOnoff = led[pme->sigLedSequence];
#else
                hs_sig_led_onoff_type led[] = 
                        {
                            HS_SIG_LED_COLOR_WHITE_OFF,
                            HS_SIG_LED_COLOR_ORANGE_OFF
                        };
                pme->sigLedOnoff = led[pme->sigLedSequence];
#endif
            }
            break;
            
        default:   
            break;            
    }

    nErr = disp_set_sigled_cmd(pme->sigLedOnoff);
    AEE_CancelTimer(AEEBacklight_SigLedFlash_EnableTimer, pme);
    AEE_CancelTimer(AEEBacklight_SigLedFlash_DisableTimer, pme);    
    
    return nErr;
}

/*==============================================================================
Function:
    AEEBacklight_SigLedFlash_DisableTimer

Description:
    Disable sig led callback funtion.

Parameter:
    pUser [in]: IBacklight *

Return value:
    none

Remark:

==============================================================================*/
static void AEEBacklight_SigLedFlash_DisableTimer(void *pUser)
{
    IBacklight *pme = (IBacklight*)pUser;

    if (NULL == pme)
    {
        return;
    }
    
    if (pme->uCls != AEECLSID_BACKLIGHT)
    {
        return;
    }
    
    AEEBacklight_SigLedFlash_Disable(pme);
    if (pme->sigLedSequence != 0)
    {
        AEE_SetSysTimer(100, AEEBacklight_SigLedFlash_EnableTimer, pme); 
    }
}

/*==============================================================================
Function:
    AEEBacklight_SigLedFlash_EnableTimer

Description:
    Disable sig led callback funtion.

Parameter:
    pUser [in]: IBacklight *

Return value:
    none

Remark:

==============================================================================*/
static void AEEBacklight_SigLedFlash_EnableTimer(void *pUser)
{
    IBacklight *pme = (IBacklight*)pUser;

    if (NULL == pme)
    {
        return;
    }
    
    if (pme->uCls != AEECLSID_BACKLIGHT)
    {
        return;
    }

    if (pme->sigLedSequence != 0)
    {
        AEEBacklight_SigLedFlash_Enable(pme);
    }
}

#endif //#ifdef FEATURE_LED_CONTROL
#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
/*===========================================================================

Function: AEEBacklight_NotifyAutoExitApp

Description:
   notify active app to exit after disable backlight to decrease power,because 848 

===========================================================================*/
static void AEEBacklight_NotifyAutoExitApp(IBacklight *pme)
{
    AEECLSID nClsId = 0;
    nClsId = ISHELL_ActiveApplet(pme->pIShell);
    (void)ISHELL_PostEvent(pme->pIShell,
                           nClsId,
                           EVT_APP_EXIT,
                           0,
                           0);
}
/*===========================================================================

Function: AEEBacklight_SetNotifyTimer
Description:
   setTimer to notify after disable backlight 

===========================================================================*/

static void AEEBacklight_SetNotifyTimer(IBacklight *pme)
{
     //AEE_SetSysTimer(MSAFTER_DISABLEBACKLIGHT_TIMER, (PFNNOTIFY)AEEBacklight_NotifyAutoExitApp, pme);
     AEE_SetTimer(MSAFTER_DISABLEBACKLIGHT_TIMER, (PFNNOTIFY)AEEBacklight_NotifyAutoExitApp, pme);
}
/*===========================================================================

Function: AEEBacklight_SetNotifyTimer
Description:
   cancelTimer to notify when enable backlight 

===========================================================================*/
static void AEEBacklight_CancelNotifyTimer(IBacklight *pme)
{
    AEE_CancelTimer((PFNNOTIFY)AEEBacklight_NotifyAutoExitApp, pme);
}
#endif
