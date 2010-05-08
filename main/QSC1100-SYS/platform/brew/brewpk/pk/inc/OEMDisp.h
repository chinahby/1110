/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMDisp.h

SERVICES:  OEM Display Interface

GENERAL DESCRIPTION:
   Definitions, typedefs, etc. for OEM Display

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef OEMDISP_H
#define OEMDISP_H

/*---------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEDisp.h"
#include "AEEBitmap.h"
#include "AEEShell.h"
#include "AEEFont.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int                 OEMDisp_New(IShell *ps, AEECLSID cls,void **ppif);

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

typedef struct IOEMDisp   IOEMDisp;

//=================================================================================
// FUNCTION DEFINITIONS and INLINE FUNCTION DEFINITIONS 
//=================================================================================

//*********************************************************************************
//
// IOEMDisp Interface
//
//*********************************************************************************




// One problem with our old OEMDisp.c is that many functions don't return a status/error
// code.  In this new IOEMDisp interface, we mandate that the function returns an 
// status/error code.

// Functions in this interface will call some handset code directly.  Therefore
// this interface has to be statically linked.
AEEINTERFACE(IOEMDisp)
{
   INHERIT_IBase(IOEMDisp);

   int   (*GetDeviceBitmap)        (IOEMDisp *pMe, IBitmap **ppIBitmap);

   int   (*Update)                 (IOEMDisp *pMe, boolean bDefer);
   int   (*SetAnnunciators)        (IOEMDisp *pMe, unsigned int wVal, unsigned int wMask);
   int   (*Backlight)              (IOEMDisp *pMe, boolean bOn);

   int   (*GetDefaultColor)        (IOEMDisp *pMe, AEEClrItem clr, RGBVAL *pRGB);

   // Set and get an entry in the system palette
   int   (*SetPaletteEntry)        (IOEMDisp *pMe, unsigned int index, RGBVAL rgb);
   int   (*GetPaletteEntry)        (IOEMDisp *pMe, RGBVAL *pRGB, unsigned int index);

   // Set multiple entries in the system palette, this saves round trips to SetPaletteEntry
   int   (*MapPalette)             (IOEMDisp *pMe, unsigned int cntRGB, RGBVAL *pRGB);

   int   (*GetSymbol)              (IOEMDisp *pMe, AECHAR *pChar, AEESymbol sym, AEEFont font);
   int   (*GetSystemFont)          (IOEMDisp *pMe, AEEFont fnt, IFont **pFont);
};


#ifdef __cplusplus
}
#endif


// Access macros for IBitmap
#define  IOEMDISP_AddRef(p)                          AEEGETPVTBL(p, IOEMDisp)->AddRef(p)
#define  IOEMDISP_Release(p)                         AEEGETPVTBL(p, IOEMDisp)->Release(p)
#define  IOEMDISP_GetDeviceBitmap(p, pbm)            AEEGETPVTBL(p, IOEMDisp)->GetDeviceBitmap(p, pbm)
#define  IOEMDISP_Update(p, b)                       AEEGETPVTBL(p, IOEMDisp)->Update(p, b)
#define  IOEMDISP_SetAnnunciators(p, v, m)           AEEGETPVTBL(p, IOEMDisp)->SetAnnunciators(p, v, m)
#define  IOEMDISP_Backlight(p, b)                    AEEGETPVTBL(p, IOEMDisp)->Backlight(p, b)
#define  IOEMDISP_GetDefaultColor(p, i, v)           AEEGETPVTBL(p, IOEMDisp)->GetDefaultColor(p, i, v)
#define  IOEMDISP_SetPaletteEntry(p, i, c)           AEEGETPVTBL(p, IOEMDisp)->SetPaletteEntry(p, i, c)
#define  IOEMDISP_GetPaletteEntry(p, c, i)           AEEGETPVTBL(p, IOEMDisp)->GetPaletterEntry(p, c, i)
#define  IOEMDISP_MapPalette(p, n, pp)               AEEGETPVTBL(p, IOEMDisp)->MapPalette(p, n, pp)
#define  IOEMDISP_GetSymbol(p, c, s, f)              AEEGETPVTBL(p, IOEMDisp)->GetSymbol(p, c, s, f)
#define  IOEMDISP_GetSystemFont(p, f, i)             AEEGETPVTBL(p, IOEMDisp)->GetSystemFont(p, f, i)



// IBitmapCtl is an interface that all OEM IBitmap implementations must support.


#define AEEIID_BITMAPCTL_20      0x0101122c
#define AEEIID_BITMAPCTL_30      0x01012cc9  // adds NotifyRelease()
#define AEEIID_BITMAPCTL         0x0101f1d7  // adds AppToScreen() and ScreenToApp()

#define AEECLSID_BITMAPCTL       AEEIID_BITMAPCTL_20
#define AEECLSID_BITMAPCTL2      AEEIID_BITMAPCTL_30

typedef struct IBitmapCtl IBitmapCtl;

AEEINTERFACE(IBitmapCtl)
{
   INHERIT_IQueryInterface(IBitmapCtl);

   int (*Enable)(IBitmapCtl *po, boolean bOn);
   int (*Restrict)(IBitmapCtl *po, AEERect *prc);
   int (*NotifyRelease)(IBitmapCtl *po, AEECallback *pcb);
   AEEPoint (*AppToScreen)(IBitmapCtl *po, AEEPoint p);
   AEEPoint (*ScreenToApp)(IBitmapCtl *po, AEEPoint p);
};


#define  IBITMAPCTL_AddRef(p)                       AEEGETPVTBL(p,IBitmapCtl)->AddRef(p)
#define  IBITMAPCTL_Release(p)                      AEEGETPVTBL(p,IBitmapCtl)->Release(p)
#define  IBITMAPCTL_QueryInterface(p,i,p2)          AEEGETPVTBL(p,IBitmapCtl)->QueryInterface(p,i,p2)
#define  IBITMAPCTL_Enable(p,c)                     AEEGETPVTBL(p,IBitmapCtl)->Enable(p,c)
#define  IBITMAPCTL_Restrict(p,c)                   AEEGETPVTBL(p,IBitmapCtl)->Restrict(p,c)
#define  IBITMAPCTL_NotifyRelease(p,i)              AEEGETPVTBL(p,IBitmapCtl)->NotifyRelease(p,i)
#define  IBITMAPCTL_AppToScreen(p,pt)               AEEGETPVTBL(p,IBitmapCtl)->AppToScreen(p,pt)
#define  IBITMAPCTL_ScreenToApp(p,pt)               AEEGETPVTBL(p,IBitmapCtl)->ScreenToApp(p,pt)




/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

Interface Name: IOEMDisp

Description: 
  This interface is used by BREW to control the backlight and annunciators.  All
  other functions in this interface have been deprecated, and should simply return
  EUNSUPPORTED.

=======================================================================

Function: OEMDisp_New()

Description:
  This function creates a new instance of the IOEMDisp interface.

Prototype:
  int OEMDisp_New(IShell *ps, AEECLSID cls, void **ppif);

Parameters:
  ps   [in] : Pointer to the IShell interface.
  cls  [in] : Class ID of the new IOEMDisp interface
  ppif [out]: Pointer to the new IOEMDisp interface.

Return Value:
  SUCCESS, or error code.

Comments: 
  None.

Side Effects: 
  None.

See Also:
  None.

===========================================================================

Function: IOEMDISP_GetDeviceBitmap()

Description:
  This function retrieves an interface to the device (screen) bitmap.
  The implementation must create a new instance of the IBitmap interface
  each time this is called.  Each IBitmap interface returned by this
  function should operate on the same pixel buffer, but should have its
  own copy of all other state information.

Prototype:
  int IOEMDISP_GetDeviceBitmap(IOEMDisp *pMe, IBitmap **ppIBitmap);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  ppIBitmap [out]: Pointer to interface of device bitmap.

Return Value:
  SUCCESS is returned if function executed correctly.
  ENOMEMORY if there was not enough memory for the operation.

Comments: 
  This function is deprecated.  BREW now retrieves device bitmaps by
  creating instances of the class IDs AEECLSID_DEVBITMAPn and
  AEECLSID_DEVBITMAPn_CHILD, where n is the display number (1-4).

Side Effects: 
  None.

See Also:
  None.

===========================================================================

Function: IOEMDISP_GetDefaultColor()

Description:
  This function is used to query the default system colors.

Prototype:
  int IOEMDISP_GetDefaultColor(IOEMDisp *pMe, AEEClrItem clr, RGBVAL *pRGB);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  clr        [in]: Item for which to obtain color.
  pRGB      [out]: The RGB value of the corresponding color.

Return Value:
  SUCCESS is returned if function executed correctly.

Comments: 
  This function is deprecated.  BREW now retrieves default colors through
  OEM_GetDeviceInfoEx() with AEE_DEVICEITEM_SYS_COLORS_DISPn, where n is
  the display number (1-4).

Side Effects: 
  None.

See Also:
  OEM_GetDeviceInfoEx()

===========================================================================

Function: IOEMDISP_SetPaletteEntry()

Description:
  Sets an entry in the device's palette table.  If the device does not
  support a dynamic palette, this function should return EUNSUPPORTED.

Prototype:
  int IOEMDisp_SetPaletteEntry(IOEMDisp *pMe, unsigned int index, RGBVAL rgb);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  index      [in]: Index of palette entry to change.
  rgb        [in]: The color to put in the palette.

Return Value:
  SUCCESS on success, EUNSUPPORTED if device does not have a dynamic
  palette.

Comments: 
  This function is deprecated and is not called by BREW.

Side Effects: 
  None.

See Also:
  IOEMDisp_GetPaletteEntry(), IOEMDisp_MapPalette().

===========================================================================

Function: IOEMDISP_GetPaletteEntry()

Description:
  Gets an entry in the device's palette table.  If the device does not
  support a dynamic palette, this function should return EUNSUPPORTED.

Prototype:
  int IOEMDisp_GetPaletteEntry(IOEMDisp *pMe, RGBVAL *pRGB, unsigned int index);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  pRGB      [out]: The value of the palette entry.
  index      [in]: Index of palette entry to retrieve.

Return Value:
  SUCCESS on success, EUNSUPPORTED if device does not have a dynamic
  palette.

Comments: 
  This function is deprecated and is not called by BREW.

Side Effects: 
  None.

See Also:
  IOEMDisp_SetPaletteEntry(), IOEMDisp_MapPalette().

===========================================================================

Function: IOEMDISP_MapPalette()

Description:
  Sets multiple entries in the device's palette table.  This will only set
  a contiguous set on entries, starting from index 0.  If the device does
  not support a dynamic palette, this function should return EUNSUPPORTED.

Prototype:
  int IOEMDisp_MapPalette(IOEMDisp *pMe, unsigned int cntRGB, RGBVAL *pRGB);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  cntRGB     [in]: Number of entries to set.
  pRGB       [in]: Array of colors that are to be used in the palette.

Return Value:
  SUCCESS on success, EUNSUPPORTED if device does not have a dynamic
  palette.

Comments: 
  This function is deprecated and is not called by BREW.

Side Effects: 
  None.

See Also:
  IOEMDisp_SetPaletteEntry(), IOEMDisp_GetPaletteEntry().

===========================================================================

Function: IOEMDISP_Update()

Description:
  The function updates the graphic memory (display) using the contents of 
  the shadow buffer. The function can do either:
  1. Deferred refresh where the update waits for any ongoing draw
  operations to complete.
  2. Forced refresh where the update happens immediately.

Prototype:
  int IOEMDISP_Update(IOEMDisp *pMe, boolean bDefer);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  bDefer     [in]: flag that determines whether to do a deferred update or a
                   forced update.

Return Value:
  SUCCESS is returned if function executed correctly.

Comments: 
   This function is deprecated.  BREW now accomplishes updates by calling
   IBITMAPDEV_Update() on the device bitmap.

Side Effects: 
  None.

See Also:
  IBITMAPDEV_Update()

===========================================================================

Function: IOEMDISP_SetAnnunciators()

Description:
  The function turns annunciators on or off. Two bitmaps are passed as
  parameters; the bits set in dwAnnunMask select the corresponding 
  annunciators, and for each of the selected annunciators, the bits in 
  dwAnnunValue indicate whether the corresponding annunciator needs to be 
  turned on or off.

Prototype:
  int IOEMDISP_SetAnnunciators(IOEMDisp *pMe, unsigned int wVal, unsigned int wMask);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  wVal       [in]: annunciator bitmap values.
  wMask      [in]: annunciator bitmap masks.

Return Value:
  SUCCESS is returned if function executed correctly.

Comments: 
  None.

Side Effects: 
  None.

See Also:
  None.

===========================================================================

Function: IOEMDISP_Backlight()

Description:
  The function turns the handset backlight(s) associated with the handset
display(s) on or off.

Prototype:
  int IOEMDISP_Backlight(IOEMDisp *pMe, boolean bOn);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  bOn        [in]: flag that determines whether to turn the backlight on or off.

Return Value:
  SUCCESS is returned if function executed correctly.

Comments: 
  If a handset supports primary and seconday display, a call to this method
  must ensure that backlight for both of these displays is turned on or off
  depending on "bOn" flag.

Side Effects: 
  None.

See Also:
  None.
   
===========================================================================

Function: IOEMDISP_GetSymbol()

Description:
  The function returns the AECHAR value corresponding to the specified
  symbol value.  NOTE: This function is deprecated in BREW 2.1 and should
  return EUNSUPPORTED.

Prototype:
  int IOEMDISP_GetSymbol(IOEMDisp *pMe, AECHAR *pChar, AEESymbol sym, AEEFont font);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  pChar     [out]: character value associated with the specified symbol.
  sym        [in]: requested symbol.
  font       [in]: requested font.

Return Value:
  SUCCESS is returned if function executed correctly.
  EUNSUPPORTED is returned if function is not implemented.

Comments: 
  This function is deprecated and is not called by BREW.

Side Effects: 
  None.

See Also:
  None.

===========================================================================

Function: IOEMDISP_GetSystemFont()

Description:
  The function retrieves an interface to the font specified.  A new instance
  of the font should be created for each call to this function.

Prototype:
  int IOEMDISP_GetSystemFont(IOEMDisp *pMe, AEEFont font, IFont **pFont);

Parameters:
  pMe        [in]: Pointer to IOEMDisp interface.
  font       [in]: requested font
  pFont     [out]: Pointer to interface of font

Return Value:
  SUCCESS is returned if function executed correctly.
  EUNSUPPORTED if the specified font is not supported

Comments: 
  This function is deprecated.  BREW now retrieves system fonts by creating
  instances of the class IDs AEECLSID_FONTSYSNORMAL, AEECLSID_FONTSYSLARGE,
  and AEECLSID_FONTSYSBOLD.

Side Effects: 
  None.

See Also:
  None.

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IBitmapCtl

Description:
   The IBitmapCtl interface is used to enable and restrict access to the device
   bitmap.  It is used internally by BREW and is not available to applications.
   The purpose is to allow BREW to control which application is allowed to write
   to the screen.  IBitmapCtl is an extension to the IBitmap interface, and is
   obtained through IBITMAP_QueryInterface() with an interface ID of AEEIID_BITMAPCTL3.
   It is a requirement of the OEM that this interface be implemented for the
   device bitmap.  This interface does not need to be implemented for any other
   bitmaps.

=======================================================================

Function: IBITMAPCTL_Enable()

Description:
   This function is used to enable or disable drawing operations to the
   entire bitmap.

Prototype:
   int IBITMAPCTL_Enable(IBitmapCtl *po, boolean bOn);

Parameters:
   po   [in] : Pointer to the IBitmapCtl interface.
   bOn  [in] : TRUE if drawing will be allowed, otherwise FALSE.

Return Value:
   SUCCESS, or error code.

Comments: 
   None.

Side Effects: 
   None.

See Also:
   IBITMAPCTL_Restrict()

===========================================================================

Function: IBITMAPCTL_Restrict()

Description:
   This function restricts drawing operations to a portion of the bitmap.

Prototype:
   int IBITMAPCTL_Restrict(IBitmapCtl *po, AEERect *prc);

Parameters:
   po   [in] : Pointer to IBitmapCtl interface.
   prc  [in] : Pointer to rectangle that specifies where drawing
               is allowed.  The coordinates are absolute, rather than
               relative to the previously restricted region.

Return Value:
   SUCCESS is returned if function executed correctly.
   EBADPARM if prc is NULL.
   EUNSUPPORTED if function is not implemented.

Comments: 
   BREW does not currently use this function, but it is used by the sample
   implementation of IAppFrame (OEMAppFrame.c).

Side Effects: 
   None.

See Also:
  IBITMAPCTL_Enable(), IBITMAPCTL_AppToScreen(), IBITMAPCTL_ScreenToApp()

===========================================================================

Function: IBITMAPCTL_NotifyRelease()

Description:
   This function is used to register for notification of release of the last
   reference to this bitmap.

Prototype:
   int IBITMAPCTL_NotifyRelease(IBitmapCtl *po, AEECallback *pcb);

Parameters:
   po   [in] : Pointer to IBitmapCtl interface.
   pcb  [in] : Pointer to callback stucture.  This callback will be triggered
               when the last reference to this bitmap is released.

Return Value:
   SUCCESS is returned if function executed correctly.
   EBADPARM if pcb is NULL.
   Other values may be returned if other errors occur.

Comments:
   None.

Side Effects: 
   None.

See Also:
   None.

===========================================================================

Function: IBITMAPCTL_AppToScreen()

Description:
   This function translates an app coordinate to a screen coordinate.  This
   translation determined by the region setup with IBITMAPCTL_Restrict().

Prototype:
   AEEPoint IBITMAPCTL_AppToScreen(IBitmapCtl *po, AEEPoint p);

Parameters:
   po   [in] : Pointer to IBitmapCtl interface.
   p    [in] : Coordinate to translate.  The x and y of the rectangle
               passed to IBITMAPCTL_Restrict() will be added to this
               coordinate.

Return Value:
   Translated coordinate.

Comments:
   None.

Side Effects: 
   None.

See Also:
   IBITMAPCTL_Restrict(), IBITMAPCTL_ScreenToApp()

===========================================================================

Function: IBITMAPCTL_ScreenToApp()

Description:
   This function translates a screen coordinate to an app coordinate.  This
   translation determined by the region setup with IBITMAPCTL_Restrict().

Prototype:
   AEEPoint IBITMAPCTL_ScreenToApp(IBitmapCtl *po, AEEPoint p);

Parameters:
   po   [in] : Pointer to IBitmapCtl interface.
   p    [in] : Coordinate to translate.  The x and y of the rectangle
               passed to IBITMAPCTL_Restrict() will be substracted from
               this coordinate.

Return Value:
   Translated coordinate.

Comments:
   None.

Side Effects: 
   None.

See Also:
   IBITMAPCTL_Restrict(), IBITMAPCTL_AppToScreen()

=================================================================================*/


#endif
