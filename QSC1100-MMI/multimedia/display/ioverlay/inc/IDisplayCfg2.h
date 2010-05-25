/*
================================================================================

  FILE:  IDisplayCfg2.h
  
  SERVICES: Defines the Display Configurator interfaces

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/IDisplayCfg2.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $
    
================================================================================
================================================================================
*/
#ifndef __IDISPLAYCFG2_H__
#define __IDISPLAYCFG2_H__

#include "AEE.h"
#include "AEEBitmap.h"
#include "AEEModel.h"
#define FEATURE_USE_ENHANCED_OVERLAY
#include "IOverlay.h"

////////////////////////////////////////////////////////////////////////////////
// DisplayCfg2 Interface ID

#define AEEIID_DISPLAYCFG2    	0x0102fa15

////////////////////////////////////////////////////////////////////////////////
// DisplayCfg2 data structures


typedef struct IDisplayCfg2 IDisplayCfg2;

#define DISPCFG_PARAM_DEFAULT_ALPHA_WP_ON   0
#define DISPCFG_PARAM_DEFAULT_ALPHA_WP_OFF  1
#define DISPCFG_PARAM_DEFAULT_TRANSP_WP_ON  2
#define DISPCFG_PARAM_DEFAULT_TRANSP_WP_OFF 3
#define DISPCFG_PARAM_DEFAULT_LAYER_WP_ON   4
#define DISPCFG_PARAM_DEFAULT_LAYER_WP_OFF  5
#define DISPCFG_PARAM_ACTIVATE              6
typedef uint32 DisplayCfg2ParamType;


typedef union
{
   uint32 uint_data;
   float  float_data;
   void * ptr_data;
   OverlayLayerType layer_data;
   RGBVAL rgb_data;
   AEECLSID clsid_data;
} DisplayCfg2TypeUnion;

typedef struct
{
   AEERect rcAnnunOff;
   AEERect rcAnnunOn;
} DispRegionType;

//Model Events
#define EVT_MDL_DISPCFG_RESIZE       (EVT_MDL_DEV_START + 0x1008)

////////////////////////////////////////////////////////////////////////////////
// IDisplayCfg2 interface


#define INHERIT_IDisplayCfg2(iname) \
   INHERIT_IQueryInterface(iname); \
   int        (*RegisterOverlay)  (iname *p, IOverlay *pOverlay, boolean bAdd); \
   IOverlay * (*GetPrimaryOverlay) (iname *p); \
   int        (*Update)           (iname *p, AEERect * pRec); \
   int        (*GetMaxAppRegion)  (iname *p, boolean bAnnunOn, AEERect *pRec); \
   int        (*SetAppConfig)     (iname *p, boolean bAnnunOn, AEERect *pRec, \
                                   IBitmap *pIBitmap); \
   int        (*GetCurrAppRegion) (iname *p, AEERect *pRec, AEERect *pRecFull); \
   int        (*GetAppRegion)     (iname *p, AEECLSID clsID, AEERect *pRec, \
                                                             AEERect *pRecFull); \
   int        (*SetParam)         (iname *p, DisplayCfg2ParamType paramID, \
                                             DisplayCfg2TypeUnion p1, \
                                             DisplayCfg2TypeUnion p2); \
   int        (*GetParam)         (iname *p, DisplayCfg2ParamType paramID, \
                                             DisplayCfg2TypeUnion * pP1, \
                                             DisplayCfg2TypeUnion * pP2); \
   int        (*AddListener)      (iname *p, ModelListener *pListener)

AEEINTERFACE(IDisplayCfg2) {
   INHERIT_IDisplayCfg2(IDisplayCfg2);
};

static __inline int IDISPLAYCFG2_AddRef(IDisplayCfg2 * pMe)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->AddRef(pMe);
}

static __inline int IDISPLAYCFG2_Release(IDisplayCfg2 * pMe)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->Release(pMe);
}

static __inline int IDISPLAYCFG2_QueryInterface(IDisplayCfg2 * pMe,
                                               AEECLSID clsid, void ** ppo)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->QueryInterface(pMe, clsid, ppo);
}

static __inline int IDISPLAYCFG2_RegisterOverlay(IDisplayCfg2 * pMe, 
                                                IOverlay *pOverlay, 
                                                boolean bAdd)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->RegisterOverlay(pMe, pOverlay, bAdd);
}

static __inline IOverlay * IDISPLAYCFG2_GetPrimaryOverlay(IDisplayCfg2 * pMe)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->GetPrimaryOverlay(pMe);
}

static __inline int IDISPLAYCFG2_Update(IDisplayCfg2 * pMe, AEERect * pRec)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->Update(pMe, pRec);
}

static __inline int IDISPLAYCFG2_GetMaxAppRegion(IDisplayCfg2 * pMe, 
                                                boolean bAnnunOn, 
                                                AEERect *pRec)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->GetMaxAppRegion(pMe, bAnnunOn, 
                                                                pRec);
}

static __inline int IDISPLAYCFG2_SetAppConfig(IDisplayCfg2 * pMe, 
                                             boolean bAnnunOn, 
                                             AEERect *pRec,
                                             IBitmap *pIBitmap)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->SetAppConfig(pMe, bAnnunOn, pRec, 
                                                       pIBitmap);
}

static __inline int IDISPLAYCFG2_GetCurrAppRegion(IDisplayCfg2 * pMe,
                                                 AEERect *pRec,
                                                 AEERect *pRecFull)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->GetCurrAppRegion(pMe, pRec, pRecFull);
}

static __inline int IDISPLAYCFG2_GetAppRegion(IDisplayCfg2 * pMe,
                                             AEECLSID clsID,
                                             AEERect *pRec,
                                             AEERect *pRecFull)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->GetAppRegion(pMe, clsID, pRec, pRecFull);
}

static __inline int IDISPLAYCFG2_SetParam(IDisplayCfg2 * pMe, 
                                         DisplayCfg2ParamType paramID, 
                                         DisplayCfg2TypeUnion p1,
                                         DisplayCfg2TypeUnion p2)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->SetParam(pMe, paramID, p1, p2);
}

static __inline int IDISPLAYCFG2_Activate(IDisplayCfg2 * pMe, boolean bOn)
{
   DisplayCfg2TypeUnion p1, p2;
   p1.uint_data = bOn ? 1 : 0;
   p2.uint_data = 0;
   return IDISPLAYCFG2_SetParam(pMe, DISPCFG_PARAM_ACTIVATE, p1, p2);
}

static __inline int IDISPLAYCFG2_GetParam(IDisplayCfg2 * pMe, 
                                         DisplayCfg2ParamType paramID, 
                                         DisplayCfg2TypeUnion * pP1,
                                         DisplayCfg2TypeUnion * pP2)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->GetParam(pMe, paramID, pP1, pP2);
}

static __inline int IDISPLAYCFG2_AddListener(IDisplayCfg2 * pMe, 
                                            ModelListener *pListener)
{
   return AEEGETPVTBL((pMe), IDisplayCfg2)->AddListener(pMe, pListener);
}

#endif //__IDISPLAYCFG2_H__
/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================
DisplayCfg2ParamType

Description:
   This data type is used to specify the parameter to get/set.
   
   
Definition:
#define DISPCFG_PARAM_DEFAULT_ALPHA_WP_ON   0
#define DISPCFG_PARAM_DEFAULT_ALPHA_WP_OFF  1
#define DISPCFG_PARAM_DEFAULT_TRANSP_WP_ON  2
#define DISPCFG_PARAM_DEFAULT_TRANSP_WP_OFF 3
#define DISPCFG_PARAM_DEFAULT_LAYER_WP_ON   4
#define DISPCFG_PARAM_DEFAULT_LAYER_WP_OFF  5
#define DISPCFG_PARAM_ACTIVATE              6
typedef uint32 DisplayCfg2ParamType

Members:
===pre>
   None.
===/pre>

Comments:
This table describes the parameters that can be set/get through this interface.
These parameters are used to set the defaults for all applications.

nParamID                               Ops               p1/p2
--------------------------------------------------------------------------------
DISPCFG_PARAM_DEFAULT_ALPHA_WP_ON    Get/Set              p1
                                                      (Type Float)
                                                 Specifies alpha percentage of
                                                 this image, when the wallpaper
                                                 is enabled.
                                                 0.0 is equal to 0% and 
                                                 1.0 is equal to 100%
                                                 Default: 0.75
                                                         
                                                          p2
                                                          N/A

DISPCFG_PARAM_DEFAULT_ALPHA_WP_OFF   Get/Set              p1
                                                      (Type Float)
                                                 Specifies alpha percentage of
                                                 this image, when the wallpaper
                                                 is disabled.
                                                 0.0 is equal to 0% and 
                                                 1.0 is equal to 100%
                                                 Default: 1.0

                                                          p2
                                                          N/A

DISPCFG_PARAM_DEFAULT_TRANSP_WP_ON   Get/Set              p1
                                                 Specifies if the color
                                                 specified in p2 should be 
                                                 made transparent when
                                                 the wallpaper is enabled.
                                                 1 = Transparency On
                                                 0 = Transparency Off
                                                 Default: 1

                                                          p2
                                                 (Type RGBVAL) specifies the
                                                 color to be made transparent
                                                 when the wallpaper is enabled.

                                                 Default:
                                                   MAKE_RGB(0xFF, 0xff, 0xCF)

DISPCFG_PARAM_DEFAULT_TRANSP_WP_OFF  Get/Set              p1
                                                 Specifies if the color
                                                 specified in p2 should be 
                                                 made transparent when
                                                 the wallpaper is disabled.
                                                 1 = Transparency On
                                                 0 = Transparency Off
                                                 Default: 0

                                                          p2
                                                 (Type RGBVAL) specifies the
                                                 color to be made transparent
                                                 when the wallpaper is disabled.

                                                 Default: N/A


DISPCFG_PARAM_DEFAULT_LAYER_WP_ON    Get/Set              p1
                                                 (Type OverlayLayerType) 
                                                 Layer ID when wallpaper is 
                                                 enabled.

                                                 Default: 
                                                 The second lowest layer(0x7F)

                                                          p2
                                                          N/A

DISPCFG_PARAM_DEFAULT_LAYER_WP_OFF   Get/Set              p1
                                                 (Type OverlayLayerType)
                                                 Layer ID when wallpaper is
                                                 disabled.

                                                 Default: OV_LAYER_BASE_LAYER

                                                          p2
                                                          N/A

DISPCFG_PARAM_ACTIVATE               Get/Set              p1
                                                 Specifies if this app/display
                                                 is active.
                                                 1 - Enabled
                                                 0 - Disabled

                                                          p2
                                                          N/A

See Also:
   IDISPLAYCFG2_SetParam()
   IDISPLAYCFG2_GetParam()

================================================================================

DispRegionType

Description:
   This data type is used to return available screen size to an application when
   a EVT_MDL_DISPCFG_RESIZE event occurs


Definition:
typedef structure
{
   AEERect rcAnnunOff;
   AEERect rcAnnunOn;
} DispRegionType;

Members:
===pre>
rcAnnunOff: Size that the Application has available when the annunciator bar
            is off.
rcAnnunOn:  Size that the Application has available when the annunciator bar
            is on.
===/pre>

Comments:

See Also:
   IDISPLAYCFG2_AddListener()

================================================================================
 INTERFACE DOCUMENTATION
================================================================================

IDisplayCfg2 Interface

Description:
   IDisplayCfg2 is the interface for managing the BREW display overlays. It
   provides functionality to set/get app region as well as activating a given
   application.

Required header files:
   None.
   
================================================================================

IDISPLAYCFG2_AddRef()

Description:

   This function increments the reference count of the IDisplayCfg2 interface 
   object, allowing the interface to be shared by multiple callers. The object 
   is freed when the reference count reaches 0 (zero).

Prototype:

   uint32 IDISPLAYCFG2_AddRef(IDisplayCfg2 *pMe);

Parameters:
===pre>
   p:  Pointer to the IDisplayCfg2 interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IDisplayCfg2 interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IDISPLAYCFG2_Release()
   
================================================================================

IDISPLAYCFG2_Release()

Description:

   This function decrements the reference count of the IDisplayCfg2 interface 
   object.  The object is freed from memory and is no longer valid once its 
   reference count reaches 0 (zero).

Prototype:

   uint32 IDISPLAYCFG2_Release(IDisplayCfg2 *pMe);

Parameters:
===pre>
   p:  Pointer to the IDisplayCfg2 interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IDisplayCfg2 interface object.
   If the returned reference count is 0 (zero), the object has been freed and 
   will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IDISPLAYCFG2_AddRef()
   
================================================================================

IDISPLAYCFG2_QueryInterface()

Description: 
   This function asks an object for another API contract from the object in 
   question.

Prototype:

   int IDISPLAYCFG2_QueryInterface(IDisplayCfg2 *pMe, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IDisplayCfg2 interface object.
   c:  Requested class ID exposed by the object
   d:  Pointer to the returned interface object.
===/pre>

Return Value:
   SUCCESS          - Interface found
   ENOMEMORY        - Insufficient memory
   ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
   The IDisplayCfg2 pointer in 'p' is set to the new interface (with refcount 
   positive), or NULL if the ClassID is not supported by the object.  'p' must 
   _not_ be set to NULL.

   Setting the classID to AEEIID_MODEL will cause the IMODEL interface for this
   interface to be returned.

Side Effects: 
   None

See Also:
   IQI_QueryInterface()
   
================================================================================

IDISPLAYCFG2_RegisterOverlay()

Description: 
This function registers an additional overlay with the IDisplayCfg2 interface.
This overlay will be updated whenever an IDisplay_Update occurs.
This overlay should be pointing to the device bitmap.

Prototype:

int IDISPLAYCFG2_RegisterOverlay(IDisplayCfg2 * pMe, IOverlay *pOverlay, 
                                boolean bAdd)

Parameters:
===pre>
p       :  Pointer to the IDisplayCfg2 interface object.
pOverlay:  Pointer to an IOverlay that will be registered/unregistered with this
           interface. If this is NULL and bAdd is FALSE all additional overlays
           that are registered with this displaycfg are removed.
bAdd    :  TRUE to register the overlay, FALSE to unregister it.
===/pre>

Return Value:
SUCCESS          - Operation Successful
EFAILED          - Generic failure

Comments:  
None

Side Effects: 
None

See Also:
None

================================================================================

IDISPLAYCFG2_GetPrimaryOverlay()

Description: 
This function registers an additional overlay with the IDisplayCfg2 interface.
This overlay will be updated whenever an IDisplay_Update occurs.
This overlay should be pointing to the device bitmap.

Prototype:

IOverlay * IDISPLAYCFG2_GetPrimaryOverlay(IDisplayCfg2 * pMe)

Parameters:
===pre>
p       :  Pointer to the IDisplayCfg2 interface object.
===/pre>

Return Value:
The primary overlay for the BREW IDisplay for this application.
NULL if not yet created.

Comments:  
None.

Side Effects: 
None

See Also:
None

================================================================================

IDISPLAYCFG2_Update()

Description: 
   This function causes the specified region of the display to be updated.
   If called in the system context it will update all of the "BREW" overlays
   that are active.

Prototype:

   int IDISPLAYCFG2_Update(IDisplayCfg2 *pMe, AEERect * prc)

Parameters:
===pre>
   p:    Pointer to the IDisplayCfg2 interface object.
   prc:  Pointer to an AEERect that specifies what region to update, in the 
         application's coordinate space.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by Applications.

Side Effects: 
   None

See Also:
   None

================================================================================

IDISPLAYCFG2_Activate()

Description: 
   This function causes the current apps overlay to be made active/inactive.
   This enables/disables the overlay.

Prototype:

   int IDISPLAYCFG2_Activate(IDisplayCfg2 *pMe, boolean bOn)

Parameters:
===pre>
   p:    Pointer to the IDisplayCfg2 interface object.
   bOn:  TRUE if the current app should be activated.
         FALSE if the current app should be deactivated.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by Applications.

Side Effects: 
   None

See Also:
   None

================================================================================

IDISPLAYCFG2_GetMaxAppRegion()

Description: 
   This function retrieves the maximium region that the application can use.

Prototype:

   int IDISPLAYCFG2_GetMaxAppRegion(IDisplayCfg2 *pMe, boolean bAnnunOn,
                                                   AEERect *pRec)

Parameters:
===pre>
   p:         Pointer to the IDisplayCfg2 interface object.
   bAnnunOn:  TRUE is current app should have the annunciator bar displayed.
   pRec:      Pointer to an AEERect that will have the maximium application
              region set in it.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by Applications.

Side Effects: 
   None

See Also:
   None

================================================================================

IDISPLAYCFG2_SetAppConfig()

Description: 
This function sets the bitmap, the region of the source bitmap, and if the
annunciator should be activated for the current application.

Prototype:

int IDISPLAYCFG2_SetAppConfig(IDisplayCfg2 *pMe, boolean bAnnunOn, AEERect *pRec,
IBitmap *pIBitmap)

Parameters:
===pre>
p:         Pointer to the IDisplayCfg2 interface object.
bAnnunOn:  TRUE is current app should have the annunciator bar displayed.
pRec:      Pointer to an AEERect that describes the application region.
pIBitmap:  Pointer to an IBitmap that contains the image data for this app
===/pre>

Return Value:
SUCCESS          - Operation Successful
EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by Applications.

Side Effects: 
None

See Also:
None

================================================================================

IDISPLAYCFG2_GetCurrAppRegion()

Description: 
   This function retrieves the current region that this application is being
   displayed on.

Prototype:

   int IDISPLAYCFG2_GetCurrAppRegion(IDisplayCfg2 *pMe, AEERect *pRec, AEERect *pRecFull)

Parameters:
===pre>
   p:         Pointer to the IDisplayCfg2 interface object.
   pRec:      Pointer to an AEERect that will describe the application region.
              In the screen coordinate space.
   pRecFull:  Pointer to the rectangle the describes the current window that this
              application is on.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by Applications.

Side Effects: 
   None

See Also:
   None

================================================================================

IDISPLAYCFG2_GetAppRegion()

Description: 
   This function retrieves the current region that the specified application is
   being displayed on.

Prototype:

   int IDISPLAYCFG2_GetAppRegion(IDisplayCfg2 *pMe, AEECLSID clsID, 
                                AEERect *pRec, AEERect *pRecFull)

Parameters:
===pre>
   p:         Pointer to the IDisplayCfg2 interface object.
   clsID:     Class ID we are interested in.
   pRec:      Pointer to an AEERect that will describe the application region.
              In the screen coordinate space.
   pRecFull:  Pointer to the rectangle the describes the current window that this
              application is on.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by Applications.

Side Effects: 
   None

See Also:
   None

================================================================================

IDISPLAYCFG2_SetParam()

Description: 
   This function sets the specified IDisplayCfg2 Interface object parameter. 

Prototype:

   int IDISPLAYCFG2_SetParam(IDisplayCfg2 *pMe, DisplayCfg2ParamType paramID, 
                                            DisplayCfg2TypeUnion p1,
                                            DisplayCfg2TypeUnion p2);

Parameters:
===pre>
   p        :  Pointer to the IDisplayCfg2 interface object.
   paramID  :  Parameter we are interested in.
   p1       :  Depends on paramID.
   p2       :  Depends on paramID.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The table in "comments section of DisplayCfg2ParamType" documentation gives 
   the list of IDisplayCfg2 supported settable parameters.
   This function is not intended to be used by Applications.

Side Effects: 
   None

See Also:
   IDISPLAYCFG2_GetParam()
   DisplayCfg2ParamType

================================================================================

IDISPLAYCFG2_GetParam()

Description: 
   This function gets the specified IDisplayCfg2 Interface object parameter. 

Prototype:

   int IDISPLAYCFG2_GetParam(IDisplayCfg2 *pMe, DisplayCfg2ParamType paramID, 
                                            DisplayCfg2TypeUnion * pP1, 
                                            DisplayCfg2TypeUnion * pP2);

Parameters:
===pre>
   p        :  Pointer to the IDisplayCfg2 interface object.
   paramID  :  Parameter we are interested in.
   pP1      :  Depends on paramID.
   pP2      :  Depends on paramID.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The table in "comments section of DisplayCfg2ParamType" documentation gives 
   the list of IDisplayCfg2 supported gettable parameters.
   This function is not intended to be used by Applications.

Side Effects: 
   None

See Also:
   IDISPLAYCFG2_SetParam()
   DisplayCfg2ParamType

================================================================================

IDISPLAYCFG2_AddListener()

Description:
This function allows objects to attach a listener to the given interface.  The 
listener includes a callback function that BREW will call whenever the interface
has changed state, thereby notifying the owner of the listener that data has
changed.
The list of events that can be sent are listed in the comments section.

Prototype:
int IDISPLAYCFG2_AddListener(IDisplayCfg2 *pMe, ModelListener *pListener); 

Parameters:
===pre>
p        :  Pointer to the IOverlay interface object.
pListener:  Pointer to the ModelListener that will be attached to the object.  
The ModelListener will contain the PFNLISTENER callback function 
BREW will call to notify the registering object of changes/events 
to the IDisplayCfg2.
===/pre>

Return Value:
None

Comments:  
This is a table of valid events from this interface.

EVENT             Description                          dwParam
--------------------------------------------------------------------------------
EVT_MDL_DISPCFG_RESIZE
                  A different size screen is           Pointer to a
                  available to BREW.                   DispRegionType structure
                                                       that describes the 
                                                       available app region.

Side Effects: 
None

See Also:
ModelListener
DispRegionType
PFNLISTENER

================================================================================
*/
