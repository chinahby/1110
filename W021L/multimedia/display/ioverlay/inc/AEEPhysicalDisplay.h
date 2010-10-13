/*
================================================================================

  FILE:  AEEPhysicalDisplay.h
  
  SERVICES: Provides the Physical Display  interface

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/AEEPhysicalDisplay.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
================================================================================
*/
/*lint -save -e611 -e740 -e550*/
#ifndef __AEEPHYSICALDISPLAY_H__
#define __AEEPHYSICALDISPLAY_H__

#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEBitmap.h"

////////////////////////////////////////////////////////////////////////////////
// PhysicalDisplay Interface ID

#define AEEIID_PHYSICALDISPLAY_v1    0x0103a2b0
#define AEEIID_PHYSICALDISPLAY_v2    0x01044936
#define AEEIID_PHYSICALDISPLAY AEEIID_PHYSICALDISPLAY_v2

////////////////////////////////////////////////////////////////////////////////
// IPhysicalDisplay interface


#define INHERIT_IPhysicalDisplay(iname) \
   INHERIT_IQueryInterface(iname); \
   int      (*GetDisplayInfo)   (iname *p, AEEBitmapInfo *pBitmapInfo); \
   int      (*Enable)           (iname *p, boolean bEnable); \
   boolean  (*IsEnabled)        (iname *p); \
   int      (*IsNewConfigValid) (iname *p, boolean bEnable); \
   int      (*Map)              (iname *p, AEECLSID PhysicalDisplay, boolean bMap); \
   AEECLSID (*GetDisplayHandler)(iname *p); \
   int      (*CaptureDisplay)   (iname *p, IBitmap **ppIBitmap, const AEERect *pRect)


AEEINTERFACE_DEFINE(IPhysicalDisplay);

static __inline int IPHYSICALDISPLAY_AddRef(IPhysicalDisplay * pMe)
{
   if(pMe == NULL)
   {
      return EBADPARM;
   }

   return (int) AEEGETPVTBL((pMe), IPhysicalDisplay)->AddRef(pMe);
}

static __inline int IPHYSICALDISPLAY_Release(IPhysicalDisplay * pMe)
{
   if(pMe == NULL)
   {
      return EBADPARM;
   }

   return (int) AEEGETPVTBL((pMe), IPhysicalDisplay)->Release(pMe);
}

static __inline int IPHYSICALDISPLAY_QueryInterface(IPhysicalDisplay * pMe,
                                               AEECLSID clsid, void ** ppo)
{
   if(pMe == NULL)
   {
      return EBADPARM;
   }

   return (int) AEEGETPVTBL((pMe), IPhysicalDisplay)->QueryInterface(pMe, clsid, 
                                                                 ppo);
}

static __inline int IPHYSICALDISPLAY_GetDisplayInfo(IPhysicalDisplay * pMe,
                                                     AEEBitmapInfo *pBitmapInfo)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplay)->GetDisplayInfo(pMe, pBitmapInfo);
}

static __inline int IPHYSICALDISPLAY_Enable(IPhysicalDisplay * pMe,
                                            boolean bEnable)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplay)->Enable(pMe, bEnable);
}

static __inline boolean IPHYSICALDISPLAY_IsEnabled(IPhysicalDisplay * pMe)
{
   return (boolean) AEEGETPVTBL((pMe), IPhysicalDisplay)->IsEnabled(pMe);
}

static __inline int IPHYSICALDISPLAY_IsNewConfigValid(IPhysicalDisplay * pMe,
                                            boolean bEnable)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplay)->IsNewConfigValid(pMe, bEnable);
}

static __inline int IPHYSICALDISPLAY_Map(IPhysicalDisplay * pMe,
                                         AEECLSID PhysicalDisplay,
                                         boolean bMap)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplay)->Map(pMe,
                                                         PhysicalDisplay, bMap);
}

static __inline AEECLSID IPHYSICALDISPLAY_GetDisplayHandler(IPhysicalDisplay * pMe)
{
   return (AEECLSID) AEEGETPVTBL((pMe), IPhysicalDisplay)->GetDisplayHandler(pMe);
}

static __inline int IPHYSICALDISPLAY_CaptureDisplay(IPhysicalDisplay * pMe,
                                                    IBitmap ** ppIBitmap,
                                                    const AEERect *pRect)
{
   return AEEGETPVTBL((pMe), IPhysicalDisplay)->CaptureDisplay(pMe, 
                                                               ppIBitmap,
                                                               pRect);
}

/*
================================================================================
 INTERFACE DOCUMENTATION
================================================================================

IPhysicalDisplay Interface

Description:
   IPhysicalDisplay is the interface for controlling the physical display

Required header files:
   None.
   
================================================================================

IPHYSICALDISPLAY_AddRef()

This function is inherited from IQI_AddRef().

================================================================================

IPHYSICALDISPLAY_Release()

This function is inherited from IQI_Release().

================================================================================

IPHYSICALDISPLAY_QueryInterface()

This function is inherited from IQI_QueryInterface(). 

================================================================================

IPHYSICALDISPLAY_GetDisplayInfo()

Description: 
   This function retrieves the pixel depth and size of the specified physical 
   display.

Prototype:

   int IPHYSICALDISPLAY_GetDisplayInfo(IPhysicalDisplay * pMe, 
                                       AEEBitmapInfo *pBitmapInfo)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   pBitmapInfo     : Pointer to a bitmap info structure that will be filled in 
                     for this display
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================

IPHYSICALDISPLAY_Enable()

Description: 
   This function enables or disables the physical display.

Prototype:

   int IPHYSICALDISPLAY_Enable(IPhysicalDisplay * pMe, 
                               boolean bEnable)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   bEnable         : TRUE to enable the display, FALSE to disable it
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================

IPHYSICALDISPLAY_IsEnabled()

Description: 
   This function returns a boolean if the display is enabled

Prototype:

   boolean IPHYSICALDISPLAY_IsEnabled(IPhysicalDisplay * pMe)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
===/pre>

Return Value:
   TRUE             - Display is enabled
   FALSE            - Display is disabled

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================

IPHYSICALDISPLAY_Map()

Description: 
   This function allows the physical display to be redirected to another display.
   This will use what ever underlying support is availible, this function is
   responsible for updating the GetDisplayHandler function for the mapped
   display.

Prototype:

   int IPHYSICALDISPLAY_Map(IPhysicalDisplay * pMe,
                            AEECLSID PhysicalDisplay,
                            boolean bMap)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   PhysicalDisplay : The ClassID of the display to map to this display
   bMap            : If TRUE this display will be handle update to the specfied
                     display, if FALSE this display will stop handling updates
                     for the specified display.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   EUNSUPPORTED     - The specified display can not be mapped to this display
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   IPHYSICALDISPLAY_GetDisplayHandler

================================================================================

IPHYSICALDISPLAY_GetDisplayHandler()

Description: 
   This function allows the user to query which display is handling updates for
   this physical display. This is modified with the IPHYSICALDISPLAY_MAP interface.

Prototype:

   AEECLSID IPHYSICALDISPLAY_GetDisplayHandler(IPhysicalDisplay * pMe)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
===/pre>

Return Value:
   The ClassID of the display that is handling updates to this display.
   If this value is 0, then this display will can not be updated.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   IPHYSICALDISPLAY_Map

================================================================================

IPHYSICALDISPLAY_CaptureDisplay()

Description: 
   This function creates a bitmap that contains a copy of the contents of the
   display.

Prototype:

   int IPHYSICALDISPLAY_CaptureDisplay(IPhysicalDisplay * pMe,
                                            IBitmap **ppIBitmap,
                                            AEERect *pRect)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   ppIBitmap       : Pointer to an IBitmap pointer that will be created by this
                     call
   pRect           : Pointer to a rectangle that discribes the region that 
                     should be captured.
===/pre>
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   EUNSUPPORTED     - The specified display can not be captured
   Other error codes depend on application.

Return Value:

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None.

================================================================================
*/

/*lint -restore*/
#endif //__IPHYSICALDISPLAY_H__
