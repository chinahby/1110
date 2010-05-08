/*
================================================================================

FILE:  OEMOverlayMgr.c

SERVICES: Provides overlay manager related services.


================================================================================

Copyright © 1999-2005 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlayMgr.c#3 $
$Author: jbrasen $
$DateTime: 2009/02/11 14:50:55 $

================================================================================
*/
#include "OEMFeatures.h"

#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#include "AEEStdLib.h"
#include "AEECriticalSection.h"
#include "AEE_OEM.h"
#include "AEECLSID_MMOVERLAYMGR.BID"
#include "AEEBase.h"
#include "OEMOverlayHelper.h"
#include "OEMOverlay_priv.h"

#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { (void)IBASE_Release((void*)(p)); (p) = NULL; }

//Overlay Manager
struct IMMOverlayMgr
{
   const AEEVTBL(IMMOverlayMgr) *pvt;// Function Table

   IShell*               pShell;       // Pointer to the shell
   uint32                dwRefs;       // Reference count for this object
   AEECallback           CBOnClose;    // Callback for app close
   boolean               bCleanup;     // Should I cleanup

};

/*==============================================================================
OverlayMgr Implementation
==============================================================================*/
/*==============================================================================
DESTRUCTOR
==============================================================================*/

static void OverlayMgr_Delete(IMMOverlayMgr *pMe)
{
   CALLBACK_Cancel(&(pMe->CBOnClose));

   if(pMe->bCleanup)
   {
      RELEASEIF(pMe->pShell);
   }

   FREE(pMe);
}

static void OverlayMgr_DeleteCB(void *po)
{
   IMMOverlayMgr * pMe = (IMMOverlayMgr *) po;
   pMe->bCleanup = FALSE;
   OverlayMgr_Delete(pMe);
}

/*==============================================================================
INTERFACE FUNCTIONS
==============================================================================*/

/*==============================================================================

IMMOVERLAYMGR_AddRef()

Description:

This function increments the reference count of the IMMOverlayMgr interface
object, allowing the interface to be shared by multiple callers.  The object is
freed when the reference count reaches 0 (zero).

Prototype:

uint32 IMMOVERLAYMGR_AddRef(IMMOverlayMgr *p);

Parameters:
===pre>
p:  Pointer to the IMMOverlayMgr interface object
===/pre>

Return Value:
Returns the incremented reference count for the IMMOverlayMgr interface object.

Comments:
None

Side Effects:
None

See Also:
IMMOVERLAYMGR_Release()

==============================================================================*/
static uint32 OverlayMgr_AddRef(IMMOverlayMgr *po)
{
   IMMOverlayMgr *pme = (IMMOverlayMgr*)po;
   return ++pme->dwRefs;
}

/*==============================================================================

IMMOVERLAYMGR_Release()

Description:

This function decrements the reference count of the IMMOverlayMgr interface
object.  The object is freed from memory and is no longer valid once its
reference count reaches 0 (zero).

Prototype:

uint32 IMMOVERLAYMGR_Release(IMMOverlayMgr *p);

Parameters:
===pre>
p:  Pointer to the IMMOverlayMgr interface object
===/pre>

Return Value:
Returns the decremented reference count for the IMMOverlayMgr interface object.
If the returned reference count is 0 (zero), the object has been freed and will
no longer be valid.

Comments:
None

Side Effects:
None

See Also:
IMMOVERLAYMGR_AddRef()

==============================================================================*/
static uint32 OverlayMgr_Release(IMMOverlayMgr *po)
{
   IMMOverlayMgr *pme = (IMMOverlayMgr*)po;
   uint32 dwRefs;

   dwRefs = --pme->dwRefs;
   if (dwRefs == 0)
      OverlayMgr_Delete(pme);
   return (dwRefs);
}

/*==============================================================================

IMMOVERLAYMGR_QueryInterface()

Description: 
This function asks an object for another API contract from the object in
question.

Prototype:

int IMMOVERLAYMGR_QueryInterface(IMMOverlayMgr *p, AEECLSID c, void **d)

Parameters:
===pre>
p:  Pointer to the IMMOverlayMgr interface object.
c:  Requested class ID exposed by the object
d:  Pointer to the returned IMMOverlayMgr interface object.
===/pre>

Return Value:
SUCCESS          - Interface found
ENOMEMORY        - Insufficient memory
ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
The IMMOverlayMgr pointer in 'p' is set to the new interface (with refcount
positive), or NULL if the ClassID is not supported by the object.  'p' must
_not_ be set to NULL.

Side Effects: 
None

See Also:
IQI_QueryInterface()

==============================================================================*/
static int OverlayMgr_QueryInterface(IMMOverlayMgr *po, AEECLSID id,
                                     void **ppo)
{
   switch (id)
   {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_MMOVERLAYMGR:
      case AEEIID_MMOVERLAYMGR:
         *ppo = (void*)po;
         (void)IQI_AddRef(*ppo);
         return SUCCESS;
      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 

/*==============================================================================

IMMOVERLAYMGR_CreateOverlay()

Description: 
   This function creates an overlay based on the parameters specified.

Prototype:

   int IMMOVERLAYMGR_CreateOverlay(IMMOverlayMgr * pMe, 
                                   IBitmap * pBmp,
                                   AEECLSID display,
                                   AEEPoint destPoint,
                                   IMMOverlay **ppOverlay);

Parameters:
===pre>
   p      :  Pointer to the IMMOverlayMgr interface object.
   pBmp   :  Pointer to the IBitmap interface that will be displayed
   display:  ClassID of the display that this overlay will be created on.
   destPoint: Where on the display should this overlay be placed.
   ppOverlay: Pointer to an IMMOverlay pointer that will be initalized in this
              call
===/pre>

Return Value:
   SUCCESS          - Operation successful
   ENOMEMORY        - Insufficient memory
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The Overlay that is returned will have some of its parameters set. See the
   documentation for IMMOVERLAY_SetParam for more information.

Side Effects: 
   None

See Also:
   IMMOVERLAY_SetParam()

==============================================================================*/
/*lint -save -e818 pMe can not be const */
static int OverlayMgr_CreateOverlay(IMMOverlayMgr * pMe, 
                                    IBitmap * pBmp,
                                    AEECLSID display,
                                    AEEPoint destPoint,
                                    IMMOverlay **ppOverlay)
{
   IMMOverlay * pOverlay;
   int nErr = SUCCESS;

   pOverlay = Overlay_Create(pMe->pShell);
   if(pOverlay == NULL)
   {
      return AEE_EFAILED;
   }

   nErr = IMMOVERLAY_SetParam(pOverlay, OV_PARM_DISPLAY, display, 0);
   
   if(nErr == SUCCESS)
   {
      nErr = IMMOVERLAY_SetParam(pOverlay, OV_PARM_BITMAP, (uint32)pBmp, 0);
   }

   if(nErr == SUCCESS)
   {
      AEERect rect;
      rect.x  = destPoint.x;
      rect.y  = destPoint.y;
      rect.dx = 0;
      rect.dy = 0;
      nErr = IMMOVERLAY_SetParam(pOverlay, OV_PARM_DEST_REGION, 
                                 (uint32)&rect, (OV_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT|
                                                 OV_FLAG_DEST_REGION_ALIGN_VERT_UPPER));
   }

   if(SUCCESS != nErr)
   {
      (void) IMMOVERLAY_Release(pOverlay);
      pOverlay = NULL;
   }

   *ppOverlay = pOverlay;
   return nErr;
}
/*lint -restore */
/*==============================================================================

IMMOVERLAYMGR_GetCapabilities()

Description: 
This function retrieves the list of possible values for a parameter to
IMMOVERLAY_SetParam. These values are to give an idea of what the device supports.
An overlay may not be able to set all of these values at any given time. The
function IMMOVERLAY_GetCapabilities() will serve this function.

Prototype:

int IMMOVERLAYMGR_GetCapabilities(IMMOverlayMgr *pMe, 
                                  OverlayParamType paramID,
                                  int32 * pSize,
                                  OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
p        :  Pointer to the IMMOverlayMgr interface object.
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
of IMMOverlayMgr supported capability parameters.

Side Effects: 
None

See Also:
IMMOVERLAY_GetCapabilities()
IMMOVERLAY_SetParam()
IMMOVERLAY_GetParam()

==============================================================================*/
/*lint -save -e818 pMe can not be const */
static int OverlayMgr_GetCapabilities(IMMOverlayMgr *pMe, 
                                      OverlayParamType paramID, int32 * pSize,
                                      OverlayCapabilitiesType * pBuff)
{
   (void)pMe;
   return OverlayHelper_GetCapabilities(paramID, pSize, pBuff, NULL);
}
/*lint -restore */
/*==============================================================================
CONSTRUCTOR
==============================================================================*/
int OverlayMgr_New(IShell *pShell, AEECLSID ClsId, void **ppObj)
{
   IMMOverlayMgr *pNew;
   int nErr = SUCCESS;
   static const VTBL(IMMOverlayMgr) overlayMgrFuncs = 
   {
      OverlayMgr_AddRef,
      OverlayMgr_Release,
      OverlayMgr_QueryInterface,
      OverlayMgr_CreateOverlay,
      OverlayMgr_GetCapabilities
   };

   // Make sure its us
   if (ClsId != AEECLSID_MMOVERLAYMGR)
      return ECLASSNOTSUPPORT;

   // Get the object
   pNew = MALLOCREC(IMMOverlayMgr);
   if (!pNew)
   {
      DBGPRINTF_FATAL("Error MALLOC FAILED");
      return ENOMEMORY;
   }

   pNew->pvt = &overlayMgrFuncs;

   // Now initialize it
   pNew->pShell = pShell;
   (void)ISHELL_AddRef(pNew->pShell);

   pNew->dwRefs = 1;

   pNew->bCleanup = TRUE;
   CALLBACK_Init(&(pNew->CBOnClose), OverlayMgr_DeleteCB, pNew);
   AEE_LinkSysObject(&(pNew->CBOnClose));

   *ppObj = pNew;
   return nErr;
}

#endif /* FEATURE_OVERLAY */

