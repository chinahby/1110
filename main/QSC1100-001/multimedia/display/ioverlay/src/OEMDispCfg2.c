/*
================================================================================

FILE:  OEMDisplayCfg2.c

SERVICES: Provides Display Configuration related services. This is used to link
          the BREW display with IOverlay


================================================================================

Copyright © 1999-2007 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMDispCfg2.c#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $


================================================================================
*/
/*lint -save -e611 -e740 */


#include "OEMFeatures.h"

#include "IDisplayCfg2.h"
#define FEATURE_USE_ENHANCED_OVERLAY
#include "IOverlay2.h"
#include "OVERLAY2MGR.BID"

//skc #include "IWallpaper.h"
//skc #include "Wallpaper.bid"

#include "AEEDisp.h"
#include "AEE_OEM.h"
#include "AEEComdef.h"
#include "AEEStdLib.h"
#include "AEEModel.h"
#include "AEEBase.h"
#include "DISPLAYCFG2.BID"
#include "AEEAnnunciator.h"
#include "OEMDisp.h"
#include "AEELogicalDisplayCfg.h"
#include "AEECLSID_LOGICALDISPLAYCFG.BID"
//skc #include "IPhysicalDisplay.h"
#include "OEMOverlayHelper.h"
#include "OEMDisp.h"
#include "AEEBitmap.h"


//DisplayCfg2 
struct IDisplayCfg2
{
   const AEEVTBL(IDisplayCfg2) *pvt;        // Function Table

   IShell*               pShell;           // Pointer to the shell
   uint32                dwRefs;           // Reference count for this object

   //IOverlay*             pOverlay;         // Pointer to the main overlay
   //OverlayListType *     pHeadOverlayList; // List of additional overlays

   AEECLSID              classID;          // Class ID of this interface
   /*AEECLSID              ownerID;          // Class ID of the owner of this
                                           // interface.
   AEECLSID              dispClsID;        // Display Class ID
   boolean               bActivated;       // Is this display activated
   AEERect               rcCurrRegion;     // Current App Region
   AEERect               rcFullRegion;     // Current Full Screen Region

   //skc IWallpaper *          pWallpaper;       // Wallpaper for enable events
   uint32 *pWallpaper;//skc
   ModelListener         WPListener;       // Listener for wallpaper events

   ModelListener         LDListener;       // Listener for Logical Display

   DisplayCfg2GlobalDataType *pGlobal;      // Used in delete.
   IDisplayCfg2 *         pNext;            // Linked list data
   IDisplayCfg2 *         pPrev;            // Linked list data
   IDisplayCfg2Model      theModel;         // Model for this interface
   OverlayHelper_Listener *  pListeners;       // The listeners on the model
   boolean               bAnnunOn;         // Is the annunciator bar enabled
   boolean               bInSystemUpdate;  // Used to prevent reentrant updates
   boolean               bInSystemNotify;  // Used to prevent reentrant Notifies
   boolean               bInCleanup;
   ILogicalDisplayCfg *  pLogicalDisplayCfg;*/
};

/*==============================================================================
Wallpaper Helper Function Implementation
==============================================================================*/

#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { IBASE_Release((IBase*)(p)); (p) = NULL; }



/*
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
static int IDisplayCfg2_AddListener( IDisplayCfg2 *pMe, ModelListener *pListener )
{
	return EUNSUPPORTED;
}

static void DisplayCfg2_Delete(IDisplayCfg2 *pMe)
{
   FREE(pMe);
}

/*==============================================================================
IWallpaper Interface Implementation
==============================================================================*/
/*
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
*/
static uint32 DisplayCfg2_AddRef(IDisplayCfg2 *pMe)
{
   return ++pMe->dwRefs;
}

/*
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
*/
static uint32 DisplayCfg2_Release(IDisplayCfg2 *pMe)
{
   uint32 dwRefs = --pMe->dwRefs;
   if (dwRefs == 0)
   {
      DisplayCfg2_Delete(pMe);
   }
   return (dwRefs);
   
}

/*
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

Setting the classID to AEEIID_OVERLAY will cause the IOVERLAY interface for 
the current app context to be returned.

Setting the classID to AEEIID_MODEL will cause the IMODEL interface for this
interface to be returned.

Side Effects: 
None

See Also:
IQI_QueryInterface()

================================================================================
*/
   
static int DisplayCfg2_QueryInterface(IDisplayCfg2 *pif, AEECLSID id, void **ppo)
{
   switch (id)
   {
      case AEEIID_DISPLAYCFG2:
      case AEECLSID_QUERYINTERFACE:
         *ppo = (void*)pif;
         IQI_AddRef(*ppo);
         return SUCCESS;

      case AEECLSID_DISPLAYCFG1:
      case AEECLSID_DISPLAYCFG2:
      case AEECLSID_DISPLAYCFG3:
      case AEECLSID_DISPLAYCFG4:
         if(pif->classID == id)
         {
            *ppo = (void*)pif;
            IQI_AddRef(*ppo);
            return SUCCESS;
         }
         break;

      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 

/*
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
*/
static IOverlay * DisplayCfg2_GetPrimaryOverlay(IDisplayCfg2 * pMe)
{
	return NULL;
}

/*
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
None.

Side Effects: 
None

See Also:
None

================================================================================
*/
static int DisplayCfg2_Update(IDisplayCfg2 *pMe, AEERect * prc)
{
      return SUCCESS;
}


/*
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
None.

Side Effects: 
None

See Also:
None

================================================================================
*/
static int DisplayCfg2_GetMaxAppRegion(IDisplayCfg2 *pMe, boolean bAnnunOn,
                                      AEERect *pRec)
{

	return EUNSUPPORTED;
}

/*
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
None.

Side Effects: 
None

See Also:
None

================================================================================
*/
static int DisplayCfg2_SetAppConfig(IDisplayCfg2 *pMe, boolean bAnnunOn, 
                                   AEERect *pRec, IBitmap *pIBitmap)
{
	return EUNSUPPORTED;
}

/*
================================================================================

IDISPLAYCFG2_GetCurrAppRegion()

Description: 
This function retrieves the current region that this application is being
displayed on.

Prototype:

int IDISPLAYCFG2_GetCurrAppRegion(IDisplayCfg2 *pMe, AEERect *pRec)

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
None.

Side Effects: 
None

See Also:
None

================================================================================
*/
static int DisplayCfg2_GetCurrAppRegion(IDisplayCfg2 *pMe, AEERect *pRec, AEERect *pRecFull)
{
	return EUNSUPPORTED;
}

/*
================================================================================

IDISPLAYCFG2_GetAppRegion()

Description: 
   This function retrieves the current region that the specified application is
   being displayed on.

Prototype:

   int IDISPLAYCFG2_GetAppRegion(IDisplayCfg2 *pMe, AEECLSID clsID, AEERect *pRec)

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
   None.

Side Effects: 
   None

See Also:
   None

================================================================================
*/
static int DisplayCfg2_GetAppRegion(IDisplayCfg2 *pMe, AEECLSID clsID, 
                                   AEERect *pRec, AEERect *pRecFull)
{
	return EUNSUPPORTED;
}

/*
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
           interface.
bAdd    :  TRUE to register the overlay, FALSE to unregister it.
           ===/pre>

           Return Value:
SUCCESS          - Operation Successful
EFAILED          - Generic failure

Comments:  
None.

Side Effects: 
None

See Also:
None

================================================================================
*/
static int DisplayCfg2_RegisterOverlay(IDisplayCfg2 * pMe, IOverlay *pOverlay, 
                                      boolean bAdd)
{
	return EUNSUPPORTED;
}

/*
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

Side Effects: 
None

See Also:
IDISPLAYCFG2_GetParam()
DisplayCfg2ParamType

================================================================================
*/
static int DisplayCfg2_SetParam(IDisplayCfg2 *pMe, DisplayCfg2ParamType paramID, 
                               DisplayCfg2TypeUnion p1,
                               DisplayCfg2TypeUnion p2)
{
  if(DISPCFG_PARAM_ACTIVATE == paramID)
  {
    return SUCCESS;
  }
	return EUNSUPPORTED;
}

/*
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

Side Effects: 
None

See Also:
IDISPLAYCFG2_SetParam()
DisplayCfg2ParamType

================================================================================
*/
static int DisplayCfg2_GetParam(IDisplayCfg2 *pMe, DisplayCfg2ParamType paramID, 
                               DisplayCfg2TypeUnion * pP1, 
                               DisplayCfg2TypeUnion * pP2)
{
	return EUNSUPPORTED;
}


int DisplayCfg2_New(IShell *pShell, AEECLSID ClsId, void **ppObj)
{
   IDisplayCfg2 *pNew = NULL;

   static const VTBL(IDisplayCfg2) DisplayCfg2Funcs = 
   {
      DisplayCfg2_AddRef,
      DisplayCfg2_Release,
      DisplayCfg2_QueryInterface,
      DisplayCfg2_RegisterOverlay,
      DisplayCfg2_GetPrimaryOverlay,
      DisplayCfg2_Update,
      DisplayCfg2_GetMaxAppRegion,
      DisplayCfg2_SetAppConfig,
      DisplayCfg2_GetCurrAppRegion,
      DisplayCfg2_GetAppRegion,
      DisplayCfg2_SetParam,
      DisplayCfg2_GetParam,
      IDisplayCfg2_AddListener
   };

   
   // Make sure its us
   switch(ClsId)
   {
      case AEECLSID_DISPLAYCFG1:
      case AEECLSID_DISPLAYCFG2:
      case AEECLSID_DISPLAYCFG3:
      case AEECLSID_DISPLAYCFG4:
         break;
      default:
         return ECLASSNOTSUPPORT;
   }

   pNew = MALLOCREC(IDisplayCfg2);

   if(NULL == pNew)
   {
         return ENOMEMORY;
   }
   pNew->dwRefs = 1;
   pNew->pvt = &DisplayCfg2Funcs;
   *ppObj = pNew;
   return SUCCESS;
}


/*lint -restore*/
