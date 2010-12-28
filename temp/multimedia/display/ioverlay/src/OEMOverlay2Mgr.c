/*
================================================================================

FILE:  OEMOverlayMgr.c

SERVICES: Provides overlay manager related services.


================================================================================

Copyright © 1999-2005 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlay2Mgr.c#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
*/
/*lint -save -e611 -e740 -e545 -esym(751,_IOverlay)*/
#include "OEMFeatures.h"

#ifdef FEATURE_OVERLAY
#define FEATURE_USE_ENHANCED_OVERLAY

#include "IOverlay2.h"

#include "AEEStdLib.h"
#include "AEECriticalSection.h"
#include "AEE_OEM.h"
#include "OVERLAY2MGR.bid"
#include "AEEBase.h"
//#include "OEMOverlayHelper.h"
//#include "OEMOverlay_priv.h"

#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { IBASE_Release((IBase*)(p)); (p) = NULL; }

typedef struct _OverlayListNodeType2 OverlayListNodeType2;

//Overlay Manager
struct IOverlay2Mgr
{
   const AEEVTBL(IOverlay2Mgr) *pvt;// Function Table

   IShell*               pShell;       // Pointer to the shell
   uint32                dwRefs;       // Reference count for this object
   OverlayListNodeType2 * pCurrOverlayNode; //Current Overlay in Enum Operation
   OverlayEnumInfoType2   CurrEnumInfo; // Current Enumeration info
   boolean               bEnumInit;    // Has Enumeration been initialized
   AEECallback           CBOnClose;    // Callback for app close
   boolean               bCleanup;     // Should I cleanup

};

/*==============================================================================
Global list management
==============================================================================*/
struct _OverlayListNodeType2
{
   IOverlay2 *        pOverlay;
   struct _OverlayListNodeType2 * pNext;
   uint32            dwRefs;
};


/*==============================================================================
OverlayMgr Implementation
==============================================================================*/
/*==============================================================================
DESTRUCTOR
==============================================================================*/

static void Overlay2Mgr_Delete(IOverlay2Mgr *pMe)
{

   CALLBACK_Cancel(&(pMe->CBOnClose));

   if(pMe->bCleanup)
   {
      RELEASEIF(pMe->pShell);
   }

   FREE(pMe);
}

static void Overlay2Mgr_DeleteCB(void *po)
{
   IOverlay2Mgr * pMe = (IOverlay2Mgr *) po;
   pMe->bCleanup = FALSE;
   Overlay2Mgr_Delete(pMe);
}

/*==============================================================================
INTERFACE FUNCTIONS
==============================================================================*/

/*==============================================================================

IOVERLAYMGR_AddRef()

Description:

This function increments the reference count of the IOverlay2Mgr interface
object, allowing the interface to be shared by multiple callers.  The object is
freed when the reference count reaches 0 (zero).

Prototype:

uint32 IOVERLAYMGR_AddRef(IOverlay2Mgr *p);

Parameters:
===pre>
p:  Pointer to the IOverlay2Mgr interface object
===/pre>

Return Value:
Returns the incremented reference count for the IOverlay2Mgr interface object.

Comments:
None

Side Effects:
None

See Also:
IOVERLAYMGR_Release()

==============================================================================*/
static uint32 Overlay2Mgr_AddRef(IOverlay2Mgr *po)
{
   IOverlay2Mgr *pme = (IOverlay2Mgr*)po;
   return ++pme->dwRefs;
}

/*==============================================================================

IOVERLAYMGR_Release()

Description:

This function decrements the reference count of the IOverlay2Mgr interface
object.  The object is freed from memory and is no longer valid once its
reference count reaches 0 (zero).

Prototype:

uint32 IOVERLAYMGR_Release(IOverlay2Mgr *p);

Parameters:
===pre>
p:  Pointer to the IOverlay2Mgr interface object
===/pre>

Return Value:
Returns the decremented reference count for the IOverlay2Mgr interface object.
If the returned reference count is 0 (zero), the object has been freed and will
no longer be valid.

Comments:
None

Side Effects:
None

See Also:
IOVERLAYMGR_AddRef()

==============================================================================*/
static uint32 Overlay2Mgr_Release(IOverlay2Mgr *po)
{
   IOverlay2Mgr *pme = (IOverlay2Mgr*)po;
   uint32 dwRefs;

   dwRefs = --pme->dwRefs;
   if (dwRefs == 0)
      Overlay2Mgr_Delete(pme);
   return (dwRefs);
   

}

/*==============================================================================

IOVERLAYMGR_QueryInterface()

Description: 
This function asks an object for another API contract from the object in
question.

Prototype:

int IOVERLAYMGR_QueryInterface(IOverlay2Mgr *p, AEECLSID c, void **d)

Parameters:
===pre>
p:  Pointer to the IOverlay2Mgr interface object.
c:  Requested class ID exposed by the object
d:  Pointer to the returned IOverlay2Mgr interface object.
===/pre>

Return Value:
SUCCESS          - Interface found
ENOMEMORY        - Insufficient memory
ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
The IOverlay2Mgr pointer in 'p' is set to the new interface (with refcount
positive), or NULL if the ClassID is not supported by the object.  'p' must
_not_ be set to NULL.

Side Effects: 
None

See Also:
IQI_QueryInterface()

==============================================================================*/
static int Overlay2Mgr_QueryInterface(IOverlay2Mgr *po, AEECLSID id,
                                     void **ppo)
{
   switch (id)
   {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_OVERLAY2MGR:
      case AEEIID_OVERLAY2MGR:
         *ppo = (void*)po;
         IQI_AddRef(*ppo);
         return SUCCESS;
      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 

/*==============================================================================

IOVERLAYMGR_CreateOverlay()

Description: 
This function creates an overlay based on the parameters specified.

Prototype:

int IOVERLAYMGR_CreateOverlay(IOverlay2Mgr *p, IBitmap * pBmp, AEECLSID display,
                              AEEPoint destPoint);

Parameters:
===pre>
p      :  Pointer to the IOverlay2Mgr interface object.
pBmp   :  Pointer to the IBitmap interface that will be displayed
display:  ClassID of the display that this overlay will be created on.
destPoint: Where on the display should this overlay be placed.
===/pre>

Return Value:
Pointer to an IOverlay2, if this value is NULL, than creation failed.

Comments:  
The Overlay that is returned will have some of its parameters set. See the
documentation for IOVERLAY_SetParam for more information.

Side Effects: 
None

See Also:
IOVERLAY_SetParam()

==============================================================================*/
IOverlay2 * Overlay2_Create(IShell * pShell);

static IOverlay2 * Overlay2Mgr_CreateOverlay(IOverlay2Mgr *pMe, IBitmap * pBmp,
                                           AEECLSID display, AEEPoint destPoint)
{
   IOverlay2 * pOverlay;
   int nErr = SUCCESS;
   OverlayTypeUnion2 p1,p2;
   p1.uint_data = 0;
   p2.uint_data = 0;

   pOverlay = Overlay2_Create(pMe->pShell);

   p1.clsid_data = display;
   nErr = IOVERLAY2_SetParamSuggested(pOverlay, OV2_PARM_DISPLAY, 
         p1, p2, FALSE);
   
   if(nErr == SUCCESS)
   {
      p1.ptr_data = (void *) pBmp;
      nErr = IOVERLAY2_SetParamSuggested(pOverlay, OV2_PARM_BITMAP, p1, p2, FALSE);
   }

   if(nErr == SUCCESS)
   {
      AEERect rect;
      rect.x  = destPoint.x;
      rect.y  = destPoint.y;
      rect.dx = 0;
      rect.dy = 0;
      p1.ptr_data = &rect;
      p2.uint_data = (OV2_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT|
                      OV2_FLAG_DEST_REGION_ALIGN_VERT_UPPER);
      nErr = IOVERLAY2_SetParamSuggested(pOverlay, OV2_PARM_DEST_REGION, 
                                        p1, p2, FALSE);
   }


   if((nErr != SUCCESS) && (pOverlay != NULL))
   {
      (void) IOVERLAY2_Release(pOverlay);
      pOverlay = NULL;
   }

   return pOverlay;

}

/*==============================================================================

IOVERLAYMGR_EnumInit()

Description: 
This function initializes the enumeration of the existing overlays.

Prototype:

int IOVERLAYMGR_EnumInit(IOverlay2Mgr *p, OverlayEnumInfoType2 * pEnumInfo); 

Parameters:
===pre>
p        :  Pointer to the IOverlay2Mgr interface object.
pEnumInfo:  Pointer to the OverlayEnumInfo structure that specifies the search
            criteria. 
            If this value is NULL all overlays will be returned.
===/pre>

Return Value:
SUCCESS          - Operation successful
ENOMEMORY        - Insufficient memory
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
None.

Side Effects: 
None

See Also:
IOVERLAYMGR_EnumNext()
==============================================================================*/
static int Overlay2Mgr_EnumInit(IOverlay2Mgr *p, OverlayEnumInfoType2 * pEnumInfo)
{
   return EUNSUPPORTED;
}

/*==============================================================================

IOVERLAYMGR_EnumNext()

Description: 
This function gets the next overlay in the set that was match by EnumInit.

Prototype:

IOverlay2 * IOVERLAYMGR_EnumNext(IOverlay2Mgr *p);

Parameters:
===pre>
p        :  Pointer to the IOverlay2Mgr interface object.
===/pre>

Return Value:
A pointer to the next IOverlay2 that was matched.

Comments:  
The returned IOverlay2 should be released when the caller of IOVERLAYMGR_EnumNext
is done with it.

Side Effects: 
None

See Also:
IOVERLAYMGR_EnumInit()

==============================================================================*/
static IOverlay2 * Overlay2Mgr_EnumNext(IOverlay2Mgr *p)
{
   return NULL;
}
/*==============================================================================

IOVERLAYMGR_GetCapabilities()

Description: 
This function retrieves the list of possible values for a parameter to
IOVERLAY_SetParam. These values are to give an idea of what the device supports.
An overlay may not be able to set all of these values at any given time. The
function IOVERLAY_GetCapabilities() will serve this function.

Prototype:

int IOVERLAYMGR_GetCapabilities(IOverlay2Mgr *p, OverlayParamType paramID,
                               uint32 * pSize, OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
p        :  Pointer to the IOverlay2Mgr interface object.
paramID  :  Parameter we are interested in.
pSize    :  Pointer to a variable the will return the size of the buffer needed.
pBuff    :  Pointer to a buffer to fill the possible values in. If this is NULL
            pSize will be set, but the buffer will not be used.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
The table in "comments section of OverlayParamType" documentation gives the list
of IOverlay2Mgr supported capability parameters.

Side Effects: 
None

See Also:
IOVERLAY_GetCapabilities()
IOVERLAY_SetParam()
IOVERLAY_GetParam()

==============================================================================*/
/*lint -save -esym(715, p)*/
static int Overlay2Mgr_GetCapabilities(IOverlay2Mgr *p, OverlayParamType2 paramID,
                                      uint32 * pSize,
                                      OverlayCapabilitiesType2 * pBuff)
{

   //return OverlayHelper_GetCapabilities(paramID, pSize, pBuff, NULL);
	return EUNSUPPORTED;
}
/*lint -restore*/

/*==============================================================================
CONSTRUCTOR
==============================================================================*/
int Overlay2Mgr_New(IShell *pShell, AEECLSID ClsId, void **ppObj)
{
   IOverlay2Mgr *pNew;
   int nErr = SUCCESS;
   static const VTBL(IOverlay2Mgr) overlayMgrFuncs = 
   {
      Overlay2Mgr_AddRef,
      Overlay2Mgr_Release,
      Overlay2Mgr_QueryInterface,
      Overlay2Mgr_CreateOverlay,
      Overlay2Mgr_EnumInit,
      Overlay2Mgr_EnumNext,
      Overlay2Mgr_GetCapabilities
   };

   // Make sure its us
   if (ClsId != AEECLSID_OVERLAY2MGR)
      return ECLASSNOTSUPPORT;

   // Get the object
   pNew = MALLOCREC(IOverlay2Mgr);
   if (!pNew)
   {
      DBGPRINTF_FATAL("Error MALLOC FAILED");
      return ENOMEMORY;
   }

   pNew->pvt = &overlayMgrFuncs;

   // Now initialize it
   pNew->pShell = pShell;
   ISHELL_AddRef(pNew->pShell);

   pNew->dwRefs = 1;


   pNew->bCleanup = TRUE;
   CALLBACK_Init(&(pNew->CBOnClose), Overlay2Mgr_DeleteCB, pNew);
   AEE_LinkSysObject(&(pNew->CBOnClose));

   *ppObj = pNew;
   return nErr;
}

#endif /* FEATURE_OVERLAY */

/*lint -restore*/
