/*===========================================================================
                              OEMDisp.c

  This file contains sample source to the IOEMDisp interface.

  Please search for OEM-Note in the file for points of interest.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "OEMFeatures.h"

#if defined(OEMDISP)

#include "OEMConfig.h"
#include "OEMDisp.h"
#include "AEEStdLib.h"
#include "AEEBacklight.h"
#include "AEE_OEM.h"

#ifdef CUST_EDITION
#include "disp.h"
#endif

#if defined(AEE_SIMULATOR)
extern void    AEESIM_SetAnnunciators(unsigned int wVal, unsigned int wMask);
#endif

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32 OEMDisp_AddRef(IOEMDisp *po);
static uint32 OEMDisp_Release(IOEMDisp *po);
static int OEMDisp_GetDeviceBitmap(IOEMDisp *po, IBitmap **ppIBitmap);
static int OEMDisp_Update(IOEMDisp *po, boolean bDefer);
static int OEMDisp_SetAnnunciators(IOEMDisp *po, unsigned int wVal, unsigned int wMask);
static int OEMDisp_Backlight(IOEMDisp *po, boolean bOn);
static int OEMDisp_GetDefaultColor(IOEMDisp *po, AEEClrItem clr, RGBVAL *pRGB);
static int OEMDisp_SetPaletteEntry(IOEMDisp *po, unsigned int index, RGBVAL RGBColor);
static int OEMDisp_GetPaletteEntry(IOEMDisp *po, RGBVAL *pRGBColor, unsigned int index);
static int OEMDisp_MapPalette(IOEMDisp *po, unsigned int cntRGB, uint32 *pRGB);
static int OEMDisp_GetSymbol(IOEMDisp *po, AECHAR *pChar, AEESymbol sym, AEEFont font);
static int OEMDisp_GetSystemFont(IOEMDisp *po, AEEFont nFont, IFont **ppFont);


static const VTBL(IOEMDisp) gOEMDispFuncs = {
   OEMDisp_AddRef,
   OEMDisp_Release,
   OEMDisp_GetDeviceBitmap,
   OEMDisp_Update,
   OEMDisp_SetAnnunciators,
   OEMDisp_Backlight,
   OEMDisp_GetDefaultColor,
   OEMDisp_SetPaletteEntry,
   OEMDisp_GetPaletteEntry,
   OEMDisp_MapPalette,
   OEMDisp_GetSymbol,
   OEMDisp_GetSystemFont
};


struct IOEMDisp {
   AEEVTBL(IOEMDisp)   *pvt;
   uint32               uRefs;
};

#ifdef CUST_EDITION

#ifdef DISP_DEVICE_16BPP
extern unsigned char roundTo5[256];
extern unsigned char roundTo6[256];
#elif defined DISP_DEVICE_8BPP
extern unsigned char roundTo2[256];
extern unsigned char roundTo3[256];
#endif 

#if !defined(WIN32)
static boolean dispInfoInitialized = FALSE;
static disp_info_type dispInfo;
#ifdef FEATURE_SECONDARY_DISPLAY
static disp_info_type dispInfo2;
#endif
#endif //!defined(AEE_SIMULATOR)
#endif /*CUST_EDITION*/


#ifdef CUST_EDITION
#if !defined(WIN32)
void OEM_InitDispInfo(void)
{
   int i;

   if (dispInfoInitialized) return;

   /* Intialize display device info.
   */
   dispInfo = disp_get_info();

#ifdef FEATURE_SECONDARY_DISPLAY
   dispInfo2 = disp_get_info2();
#endif

   /* Used to generate the round tables that convert 8-bit color into
   ** 2, 3, 5 or 6 bits with proper round up or down WRT the LSBs
   */
   for (i = 0; i < 256; i++)
   {
#ifdef DISP_DEVICE_16BPP
     if (i < 0xF8)
     {
       roundTo5[i] = (i + 0x04) >> 3;
     }
     else
     {
       roundTo5[i] = i >> 3;
     }
     if (i < 0xFC)
     {
       roundTo6[i] = (i + 0x02) >> 2;
     }
     else
     {
       roundTo6[i] = i >> 2;
     }
#elif defined DISP_DEVICE_8BPP
     if (i < 0xC0)
     {
       roundTo2[i] = (i + 0x20) >> 6;
     }
     else
     {
       roundTo2[i] = i >> 6;
     }
     if (i < 0xE0)
     {
       roundTo3[i] = (i + 0x10) >> 5;
     }
     else
     {
       roundTo3[i] = i >> 5;
     }
#endif /* DISP_DEVICE_16BPP */
      }

   dispInfoInitialized = TRUE;
}
#endif
#endif /*CUST_EDITION*/

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

int
OEMDisp_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
   IOEMDisp *pMe;

   if ((pMe = (IOEMDisp*)MALLOC(sizeof(IOEMDisp))) == NULL) {
         return ENOMEMORY;
   }

   INIT_VTBL(pMe, IOEMDisp, gOEMDispFuncs);

   pMe->uRefs = 1;

   *ppif = pMe;
   return SUCCESS;
}


static uint32
OEMDisp_AddRef(IOEMDisp *po)
{
   return ++po->uRefs;
}


static uint32
OEMDisp_Release(IOEMDisp *pMe)
{
   if (--pMe->uRefs != 0) {
      return pMe->uRefs;
   }

   // Ref count is zero. So, release memory associated with this object.
   FREE(pMe);

   return 0;
}


static int
OEMDisp_GetDeviceBitmap(IOEMDisp *pMe, IBitmap **ppIBitmap)
{
   // This function is deprecated.  BREW now retrieves device bitmaps by
   // creating instances of the class IDs AEECLSID_DEVBITMAPn and
   // AEECLSID_DEVBITMAPn_CHILD, where n is the display number (1-4).
   return EUNSUPPORTED;
}


static int
OEMDisp_Update(IOEMDisp *pMe, boolean bDefer)
{
   // This function is deprecated.  BREW now accomplishes updates by calling
   // IBITMAPDEV_Update() on the device bitmap.
   return EUNSUPPORTED;
}


static int
OEMDisp_SetAnnunciators(IOEMDisp *po, unsigned int wVal, unsigned int wMask)
{
#if defined(AEE_SIMULATOR)
   AEESIM_SetAnnunciators(wVal, wMask);
   return SUCCESS;
#else
   // OEMs need to implement this function
   return EUNSUPPORTED;
#endif
}


static int
OEMDisp_Backlight(IOEMDisp *po, boolean bOn)
{
#ifdef FEATURE_UIONE_HDK
   IShell *pShell = AEE_GetShell();

   if (ISHELL_SendURL(pShell, "coreActivities:pingBacklight")) {
      return SUCCESS;
   }

   return EUNSUPPORTED;
#else
   int nErr = EFAILED;
   IBacklight * pi = NULL;
   AEECLSID aClsID[8];
   AEECLSID * pClsID = aClsID;
   ACONTEXT * pac = NULL;

   MEMSET(aClsID, 0, sizeof(aClsID));
   aClsID[0] = AEECLSID_BACKLIGHT_DISPLAY1;
   aClsID[1] = AEECLSID_BACKLIGHT_DISPLAY2;
   aClsID[2] = AEECLSID_BACKLIGHT_DISPLAY3;
   aClsID[3] = AEECLSID_BACKLIGHT_DISPLAY4;

   while (*pClsID)
   {
      IShell* piShell = AEE_GetShell();
      // Create the IBacklight instance in system context to support IDISPLAY_Backlight()
      // which doesn't require any privilege
      pac = AEE_EnterAppContext(0);
      nErr = ISHELL_CreateInstance(piShell, *pClsID, (void**)&pi);          
      AEE_LeaveAppContext(pac);

      if (SUCCESS != nErr) {
         goto Done;
      }

      if (bOn) {
         nErr = IBACKLIGHT_Enable(pi);
      }
      else {
         nErr = IBACKLIGHT_Disable(pi);
      }

      IBACKLIGHT_Release(pi);

      if (SUCCESS != nErr) {
         goto Done;
      }
      pClsID++;
   }

Done:
   return nErr;
#endif
}

static int
OEMDisp_GetDefaultColor(IOEMDisp *po, AEEClrItem clr, RGBVAL *pRGB)
{
   // This function is deprecated.  BREW now retrieves default colors through
   // OEM_GetDeviceInfoEx() with AEE_DEVICEITEM_SYS_COLORS_DISPn, where n is
   // the display number (1-4).
   return EUNSUPPORTED;
}


static int
OEMDisp_SetPaletteEntry(IOEMDisp *po, unsigned int index, RGBVAL RGBColor)
{
   // This function is deprecated.
   return EUNSUPPORTED;
}


static int
OEMDisp_GetPaletteEntry(IOEMDisp *po, RGBVAL *pRGBColor, unsigned int index)
{
   // This function is deprecated.
   return EUNSUPPORTED;
}


static int
OEMDisp_MapPalette(IOEMDisp *po, unsigned int cntRGB, uint32 *pRGB)
{
   // This function is deprecated.
   return EUNSUPPORTED;
}


static int
OEMDisp_GetSymbol(IOEMDisp *po, AECHAR *pChar, AEESymbol sym, AEEFont font)
{
   // This function is deprecated.
   return EUNSUPPORTED;
}


static int OEMDisp_GetSystemFont(IOEMDisp *po, AEEFont nFont, IFont **ppFont)
{
   // This function is deprecated.  BREW now retrieves system fonts by creating
   // instances of the class IDs AEECLSID_FONTSYSNORMAL, AEECLSID_FONTSYSLARGE,
   // and AEECLSID_FONTSYSBOLD.
   return EUNSUPPORTED;
}


/*==================================================================
Function: OEM_GetItemStyle

Description: Retrieves information regarding the drawing style for 
stock objects.

Prototype:
   boolean  OEM_GetItemStyle(AEEItemType t, AEEItemStyle * pn, AEEItemStyle * ps)

Parameter(s):
   t: Item Type
   pn: Normal Style
   ps: Selected Style

Return Value:  None
Comments:      Return FALSE for BREW to pick default values.
Side Effects:  None
See Also:      None

==================================================================*/
boolean  OEM_GetItemStyle(AEEItemType t, AEEItemStyle * pNormal, AEEItemStyle * pSel)
{
   // Returning FALSE, BREW defaults to the best fit for the device.
   return(FALSE);
}


#endif // OEMDISP
