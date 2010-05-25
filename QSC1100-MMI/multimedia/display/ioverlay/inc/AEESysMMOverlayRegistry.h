/*
================================================================================

  FILE:  AEESysMMOverlayRegistry.h
  
  SERVICES: Defines the Display Configurator interfaces

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/AEESysMMOverlayRegistry.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
================================================================================
*/
#ifndef __AEESYSMMOVERLAYREGISTRY_H__
#define __AEESYSMMOVERLAYREGISTRY_H__

#include "AEE.h"
#include "AEEBitmap.h"
#include "AEEMMOverlay.h"

////////////////////////////////////////////////////////////////////////////////
// SysMMOverlayRegistry Interface ID

#define AEEIID_ISysMMOverlayRegistry 0x0103f2a7  	

////////////////////////////////////////////////////////////////////////////////
// ISysMMOverlayRegistry interface


#define INHERIT_ISysMMOverlayRegistry(iname) \
   INHERIT_IQueryInterface(iname); \
   int (*RegisterOverlay)       (iname *p, IMMOverlay *pMMOverlay, \
                                 boolean bAdd); \
   int (*AreOverlaysRegistered) (iname *p, AEECLSID displayCls, \
                                 boolean * pbRegistered); \
   int (*Update)                (iname *p, AEECLSID displayCls, const AEERect * pRec)

AEEINTERFACE_DEFINE(ISysMMOverlayRegistry);

static __inline int ISysMMOverlayRegistry_AddRef(ISysMMOverlayRegistry * pMe)
{
   return AEEGETPVTBL((pMe), ISysMMOverlayRegistry)->AddRef(pMe);
}

static __inline int ISysMMOverlayRegistry_Release(ISysMMOverlayRegistry * pMe)
{
   return AEEGETPVTBL((pMe), ISysMMOverlayRegistry)->Release(pMe);
}

static __inline int ISysMMOverlayRegistry_QueryInterface(ISysMMOverlayRegistry * pMe,
                                               AEECLSID clsid, void ** ppo)
{
   return AEEGETPVTBL((pMe), ISysMMOverlayRegistry)->QueryInterface(pMe, clsid, ppo);
}

static __inline int ISysMMOverlayRegistry_RegisterOverlay(ISysMMOverlayRegistry * pMe, 
                                                IMMOverlay *pMMOverlay, 
                                                boolean bAdd)
{
   return AEEGETPVTBL((pMe), ISysMMOverlayRegistry)->RegisterOverlay(pMe, pMMOverlay, bAdd);
}

static __inline int ISysMMOverlayRegistry_AreOverlaysRegistered(ISysMMOverlayRegistry * pMe, 
                                                      AEECLSID displayCls,
                                                      boolean * pbRegistered)
{
   return AEEGETPVTBL((pMe), ISysMMOverlayRegistry)->AreOverlaysRegistered(pMe, 
                                                                 displayCls,
                                                                 pbRegistered);
}

static __inline int ISysMMOverlayRegistry_Update(ISysMMOverlayRegistry * pMe, AEECLSID displayCls, const AEERect * pRec)
{
   return AEEGETPVTBL((pMe), ISysMMOverlayRegistry)->Update(pMe, displayCls, pRec);
}

#endif //__ISysMMOverlayRegistry_H__

/*
================================================================================
 INTERFACE DOCUMENTATION
================================================================================

ISysMMOverlayRegistry Interface

Description:
   ISysMMOverlayRegistry is the interface for managing the BREW display overlays. It
   provides functionality to set/get app region as well as activating a given
   application.

Required header files:
   None.
   
================================================================================

ISysMMOverlayRegistry_AddRef()

This function is inherited from IQI_AddRef().
   
================================================================================

ISysMMOverlayRegistry_Release()

This function is inherited from IQI_Release().
   
================================================================================

ISysMMOverlayRegistry_QueryInterface()

This function is inherited from IQI_QueryInterface().
   
================================================================================

ISysMMOverlayRegistry_RegisterOverlay()

Description: 
This function registers an additional overlay with the ISysMMOverlayRegistry interface.
This overlay will be updated whenever an IDisplay_Update occurs.
This overlay should be pointing to the device bitmap.

Prototype:

int ISysMMOverlayRegistry_RegisterOverlay(ISysMMOverlayRegistry * pMe, IMMOverlay *pMMOverlay, 
                                boolean bAdd)

Parameters:
===pre>
p       :  Pointer to the ISysMMOverlayRegistry interface object.
pOverlay:  Pointer to an IOverlay that will be registered/unregistered with this
           interface. If this is NULL and bAdd is FALSE all additional overlays
           that this application registered are removed.
bAdd    :  TRUE to register the overlay, FALSE to unregister it.
===/pre>

Return Value:
SUCCESS          - Operation Successful
EFAILED          - Generic failure

Comments:  
If there are any overlays registered that are enabled the default display
update behavior will not occur. The user should remove all overlays before
their application is unloaded. The display the overlay is assigned to should
not be changed while the overlay is registered.

Side Effects: 
None

See Also:
None

================================================================================

ISysMMOverlayRegistry_AreOverlaysRegistered()

Description: 
This function retrieves if there are any overlays registered and enabled on
this display.

Prototype:

int ISysMMOverlayRegistry_AreOverlaysRegistered(ISysMMOverlayRegistry * pMe,
                                      AEECLSID displayCls,
                                      boolean * pbRegistered)

Parameters:
===pre>
pMe          :  Pointer to the ISysMMOverlayRegistry interface object.
displayCls   : Class ID of the Display that should be checked.
pbRegistered :  Pointer to a boolean that will contain TRUE if the is at least
                one overlay registered and enabled.
===/pre>

Return Value:
SUCCESS          - Operation Successful
EFAILED          - Generic failure

Comments:  
This function is not intended to be used by applications.

Side Effects: 
None.

See Also:
None


================================================================================

ISysMMOverlayRegistry_Update()

Description: 
   This function causes the specified region of the display to be updated.
   If called in the system context it will update all of the "BREW" overlays
   that are active.

Prototype:

   int ISysMMOverlayRegistry_Update(ISysMMOverlayRegistry *pMe, AEECLSID displayCls, AEERect * prc)

Parameters:
===pre>
   p          : Pointer to the ISysMMOverlayRegistry interface object.
   displayCls : Class ID of the Display that should be updated
   prc        : Pointer to an AEERect that specifies what region to update,
                in the screen coordinate space.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by applications.

Side Effects: 
   None

See Also:
   None

================================================================================
*/
