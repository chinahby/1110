#ifndef  OEMDISPLAYDEV_H
#define  OEMDISPLAYDEV_H
/*=================================================================================
FILE:          OEMDisplayDev.h

SERVICES:      IDisplayDev interface

DESCRIPTION:   Low level display functions.

PUBLIC CLASSES:

INITIALIAZTION AND SEQUENCEING REQUIREMENTS:

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEBitmap.h"
#include "AEEDisp.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

typedef struct IDisplayDev    IDisplayDev;
#ifdef CUST_EDITION	
#define SIM_REND//wlh ¿ªÆô3DÐ§¹û£¬Ä¿Ç°Õë¶ÔÈ«ÆÁÏÔÊ¾
#endif /*CUST_EDITION*/
/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/

#define AEEIID_DISPLAYDEV              0x0101260f
#define AEECLSID_DISPLAYDEV1           0x01012610
#define AEECLSID_DISPLAYDEV2           0x01012611
#define AEECLSID_DISPLAYDEV3           0x0101261b
#define AEECLSID_DISPLAYDEV4           0x0101261c


// Device IBitmaps

// There is only one instance of each of these.
#define AEECLSID_DEVBITMAP1            0x0101261f  // primary display
#define AEECLSID_DEVBITMAP2            0x01012620  // secondary display
#define AEECLSID_DEVBITMAP3            0x01012621  // tertiary display
#define AEECLSID_DEVBITMAP4            0x01012622  // quaternary display

// These class IDs are used to obtain new IBitmap instances that are children
// of the IBitmaps above.
#define AEECLSID_DEVBITMAP1_CHILD      0x01012b5b
#define AEECLSID_DEVBITMAP2_CHILD      0x01012b5c
#define AEECLSID_DEVBITMAP3_CHILD      0x01012b5d
#define AEECLSID_DEVBITMAP4_CHILD      0x01012b5e


//*********************************************************************************
//
// IDisplayDev Interface
//
//*********************************************************************************

#define INHERIT_IDisplayDev(iname) \
   INHERIT_IQueryInterface(iname); \
   int  (*Update)                 (iname *po, IBitmap *pbmSrc, AEERect *prc)


AEEINTERFACE(IDisplayDev)
{
   INHERIT_IDisplayDev(IDisplayDev);
};


// Access macros for IDisplayDev
#define  IDISPLAYDEV_AddRef(p)                        AEEGETPVTBL(p,IDisplayDev)->AddRef(p)
#define  IDISPLAYDEV_Release(p)                       AEEGETPVTBL(p,IDisplayDev)->Release(p)
#define  IDISPLAYDEV_QueryInterface(p,i,p2)           AEEGETPVTBL(p,IDisplayDev)->QueryInterface(p,i,p2)
#define  IDISPLAYDEV_Update(p,b,r)                    AEEGETPVTBL(p,IDisplayDev)->Update(p,b,r)




/*================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

Interface Name: IDisplayDev

Description:
   This interface provides low level access to the display.  The only function
   of note is IDISPLAYDEV_Update.  Currently, this interface is only used by the
   OEMBitmap reference implementation.  Is instantiated with AEECLSID_DISPLAYDEV1
   for the primary display, AEECLSID_DISPLAYDEV2 for the secondary display, and so
   on.  AEEIID_DISPLAYDEV is the interface ID.

   Although this interface is not currently used by BREW, no changes should be
   made to this interface with out changing the value of the interface ID.

===H2>     
DISPLAY OVERVIEW
===/H2>
===Body>

   BREW accesses displays through three interfaces: IBitmap, IBitmapDev, and
   IBitmapCtl.  These interfaces are all bitmap interfaces and implementations may be
   found in OEMBitmap_generic.h.  The IBitmap interface is exported by all bitmaps,
   while IBitmapDev and IBitmapCtl are only exported by device bitmaps.  Generally,
   the OEM allocates static memory to hold the pixel data for each display.  In BREW
   terminology, this is the device bitmap buffer.  BREW expects that there is exactly
   one device bitmap buffer per display.  A bitmap object is something that exports
   an IBitmap interface and accesses a pixel buffer.  If it accesses a device bitmap
   buffer, it is called a device bitmap.  A display may have multiple device bitmaps,
   but they all have a pointer to the same device bitmap buffer.
===pre>
      +--------------------------------+
      |      device bitmap buffer      |
      +--------------------------------+
        ^              ^             ^
        |              |             |
        |              |             |
    +--------+    +--------+    +--------+
    | device |    | device |    | device |
    | bitmap |    | bitmap |    | bitmap |
    +--------+    +--------+    +--------+
===/pre>
   The OEM is expected to maintain one global device bitmap per display, which is
   obtained by BREW with AEECLSID_DEVBITMAPn.  (Calling AEE_CreateInstanceSys()
   multiple times with AEECLSID_DEVBITMAP1 should always return the same IBitmap,
   AddRef'd.)  AEECLSID_DEVBITMAPn_CHILD is used to create a child of the
   corresponding global device bitmap.  The OEM should always create a brand new
   device bitmap for each AEECLSID_DEVBITMAPn_CHILD request.

   A display update in BREW terminology refers to the process of copying all or part
   of a device bitmap buffer to the corresponding display, thus allowing the user to
   see the image in the device bitmap buffer.  A synchronous update is triggered by
   IBITMAPDEV_Update(), which can be called by either BREW or a BREW application.
   In the case of a child device bitmap, IBITMAPDEV_Update() is implemented as a
   call to the global (parent) device bitmap's IBITAMPDEV_Update().  The reference
   implementation implements the global device bitmap's IBITMAPDEV_Update() by
   calling IDISPLAYDEV_Update(), where the real work occurs.

   The reference OEMBitmap implementation maintains a dirty rectangle for each
   device bitmap.  This rectangle covers all of the pixels that have changed in the
   device bitmap since the last display update, allowing updates to be optimized by
   not copying parts of the bitmap that have not changed.  Child device bitmaps pass
   their dirty rectangles to the global device bitmap by calling IBITMAP_Invalidate()
   before calling IBITMAPDEV_Update(), allowing the global device bitmap's dirty
   rectangle to be expanded to include the child's dirty rectangle.  The global
   device bitmap then passes this dirty rectangle as a parameter to
   IDISPLAYDEV_Update().
===pre>
         +-------------+
         | IDisplayDev |
         +-------------+
                ^
                |
                | IDISPLAYDEV_Update()
                |
           +--------+
           | global |
           | device |
           | bitmap |
           +--------+
            ^      ^
            |      | IBITMAP_Invalidate()
            |      | IBITMAPDEV_Update()
            |      |
    +--------+    +--------+
    | child  |    | child  |
    | device |    | device |
    | bitmap |    | bitmap |
    +--------+    +--------+
                       ^
                       |
                       |
                       |
               IBITMAPDEV_Update()
===/pre>
   In addition to the above interfaces, the OEM is expected to put system fonts into
   the OEM mod table with the class IDs AEECLSID_FONTSYSNORMAL, AEECLSID_FONTSYSLARGE,
   and AEECLSID_FONTSYSBOLD.  The OEM is also expected to implement
   AEE_DEVICEITEM_SYS_COLORS_DISPn and AEE_DEVICEITEM_DISPINFOn in OEM_GetDeviceInfoEx
   for each display.  (See OEMAppFuncs.c for an example.)


Display Access Priority

   Display access is granted differently for the primary display and any secondary
   displays.  For the primary display, access is granted to the currently active BREW
   app.  The OEM layer may take over the primary display by calling AEE_Suspend(), and
   return control of the display to BREW with AEE_Resume().  Secondary display access
   is granted first to the currently active app, and if that app is not trying to use
   the display, BREW goes through the background app list, starting with the most
   recently backgrounded app, and ending with the app that has been running in the
   background the longest.  An app that is trying use a display is defined as an app
   that has a reference to device bitmap for that display. The OEM layer may take over
   a secondary display by calling AEE_EnableDisplay() and return control of the display
   to BREW by calling AEE_EnabledDisplay() again.  For both the primary and the
   secondary displays, BREW calls IBITMAPCTL_Enable() to enable or disable the
   application's instance of the device bitmap for a particular display.  This causes
   any callbacks registered with IBITMAPDEV_NotifyEnable() to be triggered.


Migrating From Older Ports

   Prior to BREW 2.1, IBitmapDev did not exist, and BREW used IOEMDisp_Update() to
   trigger screen updates.  As of version 3.1, BREW will no longer fall back on this
   method.  All displays must implement IBitmapDev.

   The following tables shows which functions have been replaced:

   IOEMDisp Method            Change in BREW 3.1
   ------------------------   ------------------------------------------------
   IOEMDISP_GetDeviceBitmap   Deprecated, CreateInstance is used instead.
   IOEMDISP_Update            Deprecated, IBITMAPDEV_Update() is used instead.
   IOEMDISP_SetAnnunciators   No change.
   IOEMDISP_Backlight         No change.
   IOEMDISP_GetDefaultColor   Deprecated, OEM_GetDeviceInfoEx is used instead.
   IOEMDISP_SetPaletteEntry   Deprecated, no replacement.
   IOEMDISP_GetPaletteEntry   Deprecated, no replacement.
   IOEMDISP_MapPalette        Deprecated, no replacement.
   IOEMDISP_GetSymbol         Deprecated, no replacement.
   IOEMDISP_GetSystemFont     Deprecated, CreateInstance is used instead.


==============================================================================
Function: IDISPLAYDEV_Update()

Description:
   This function copies a region of a bitmap to the display.  It is currently
   only used by OEMBitmap reference implementation, so it is only necessary to
   support that type of bitmap.

Prototype:
   IDISPLAYDEV_Update(IDisplayDev *po, IBitmap *pbmSrc, AEERect *prc)

Parameters:
   po       [in]: Pointer to IDisplayDev interface.
   pbmSrc   [in]: Bitmap that will be copied to display
   prc      [in]: Rectangle that will be copied

Return Value:
   SUCCESS if function executed correctly
   EUNSUPPORTED if the source bitmap is of an unsupported type

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None.

==============================================================================*/


#endif /* OEMDISPLAYDEV_H */

