/*
================================================================================

  FILE:  AEEPhysicalDisplayOverlay.h
  
  SERVICES: Provides the Physical Display Overlay interface

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/AEEPhysicalDisplayOverlay.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
================================================================================
*/
/*lint -save -e611 -e740 -e550*/
#ifndef __AEEPHYSICALDISPLAYOVERLAY_H__
#define __AEEPHYSICALDISPLAYOVERLAY_H__

#include "AEE.h"
#include "AEEModel.h"
#include "AEEStdLib.h"
#include "AEEBitmap.h"
#include "AEEMMOverlay.h"
#include "AEEPhysicalDisplay.h"

////////////////////////////////////////////////////////////////////////////////
// PhysicalDisplay Interface ID

#define AEEIID_PHYSICALDISPLAYOVERLAY       0x0103a2b1	

////////////////////////////////////////////////////////////////////////////////
// PhysicalDisplay data structures

typedef struct PhysicalDisplayAsyncUpdateCompleteType
{
   uint32      dwUnique;
   int         nErr;
   IMMOverlay *pOverlay;
   AEECLSID    clsPhysicalDisplay;
} PhysicalDisplayAsyncUpdateCompleteType;

//Model Events
#define EVT_MDL_PDC_ASYNC_UPDATE_COMPLETE       (EVT_MDL_DEV_START + 0x1009)

////////////////////////////////////////////////////////////////////////////////
// IPhysicalDisplayOverlay interface


#define INHERIT_IPhysicalDisplayOverlay(iname) \
   INHERIT_IQueryInterface(iname); \
   int  (*OverlayRegister)   (iname *p, IMMOverlay *pOverlay, uint32 * pdwOverlayID); \
   int  (*OverlayDeregister) (iname *p, uint32 dwOverlayID); \
   int  (*OverlayEnable)     (iname *p, uint32 dwOverlayID, boolean bEnable); \
   int  (*OverlayUpdate)     (iname *p, uint32 dwOverlayID, const AEERect *pRec, \
                                        uint32    dwUpdateID, boolean bAsync); \
   int  (*OverlayClear)      (iname *p, uint32 dwOverlayID); \
   int  (*ValidateBitmap)    (iname *p, IBitmap *pBitmap)


AEEINTERFACE_DEFINE(IPhysicalDisplayOverlay);

static __inline int IPHYSICALDISPLAYOVERLAY_AddRef(IPhysicalDisplayOverlay * pMe)
{
   if(pMe == NULL)
   {
      return EBADPARM;
   }

   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->AddRef(pMe);
}

static __inline int IPHYSICALDISPLAYOVERLAY_Release(IPhysicalDisplayOverlay * pMe)
{
   if(pMe == NULL)
   {
      return EBADPARM;
   }

   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->Release(pMe);
}

static __inline int IPHYSICALDISPLAYOVERLAY_QueryInterface(IPhysicalDisplayOverlay * pMe,
                                               AEECLSID clsid, void ** ppo)
{
   if(pMe == NULL)
   {
      return EBADPARM;
   }

   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->QueryInterface(pMe, clsid, 
                                                                 ppo);
}

static __inline int IPHYSICALDISPLAYOVERLAY_OverlayRegister(IPhysicalDisplayOverlay * pMe,
                                                     IMMOverlay *pOverlay,
                                                     uint32 * pdwOverlayID)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->OverlayRegister(pMe, pOverlay, 
                                                                 pdwOverlayID);
}

static __inline int IPHYSICALDISPLAYOVERLAY_OverlayDeregister(IPhysicalDisplayOverlay * pMe,
                                                       uint32    dwOverlayID)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->OverlayDeregister(pMe, dwOverlayID);
}

static __inline int IPHYSICALDISPLAYOVERLAY_OverlayEnable(IPhysicalDisplayOverlay * pMe,
                                                   uint32 dwOverlayID,
                                                   boolean bEnable)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->OverlayEnable(pMe, dwOverlayID, bEnable);
}

static __inline int IPHYSICALDISPLAYOVERLAY_OverlayUpdate(IPhysicalDisplayOverlay * pMe,
                                            uint32    dwOverlayID, 
                                            const AEERect * pRec)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->OverlayUpdate(pMe, 
                                                  dwOverlayID, pRec, 0, FALSE);
}

static __inline int IPHYSICALDISPLAYOVERLAY_OverlayUpdateAsync(IPhysicalDisplayOverlay * pMe,
                                            uint32    dwOverlayID, 
                                            const AEERect * pRec,
                                            uint32    dwUpdateID)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->OverlayUpdate(pMe, 
                                          dwOverlayID, pRec, dwUpdateID, TRUE);
}

static __inline int IPHYSICALDISPLAYOVERLAY_OverlayClear(IPhysicalDisplayOverlay * pMe,
                                                  uint32    dwOverlayID)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->OverlayClear(pMe, dwOverlayID);
}

static __inline int IPHYSICALDISPLAYOVERLAY_ValidateBitmap(IPhysicalDisplayOverlay * pMe, 
                                                    IBitmap *pBitmap)
{
   return (int) AEEGETPVTBL((pMe), IPhysicalDisplayOverlay)->ValidateBitmap(pMe, pBitmap);
}

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================
PhysicalDisplayAsyncUpdateCompleteType

Description:
   A pointer to this data type will be used as the dwParam of 
   EVT_MDL_PDC_ASYNC_UPDATE_COMPLETE.


Definition:
typedef struct PhysicalDisplayAsyncUpdateCompleteType
{
   uint32    dwUnique;
   int       nErr;
   IMMOverlay *pOverlay;
   AEECLSID  clsPhysicalDisplay;
} PhysicalDisplayAsyncUpdateCompleteType;



Members:
===pre>
dwUnique           :  The unique update ID that returned from the  
                      IMMOVERLAY_UpdateAsync function.
nErr               :  The status of the update operation.
pOverlay           :  Pointer to the IMMOverlay who initiatied the update
clsPhysicalDisplay : The classID of the physical display whose updated completed
===/pre>

Comments:  
   None.

See Also:
   IPHYSICALDISPLAYOVERLAY_OverlayUpdateAsync()
   IPHYSICALDISPLAYOVERLAY_AddListener()

================================================================================
 INTERFACE DOCUMENTATION
================================================================================

IPhysicalDisplayOverlay Interface

Description:
   IPhysicalDisplay is the interface for controlling overlays on a 
   physical display

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

IPHYSICALDISPLAYOVERLAY_OverlayRegister()

Description: 
   This function registers the IMMOverlay with the physical display

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayRegister(IPhysicalDisplayOverlay * pMe,
                                       IMMOverlay *pOverlay, 
                                       uint32 * pdwNewOverlayID)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplayOverlay interface object.
   pOverlay        : Pointer to the IMMOverlay to register
   pdwOverlayID    : Pointer to a uint32 that will contain the overlay ID
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

IPHYSICALDISPLAYOVERLAY_OverlayDeregister()

Description: 
   This function deregisters the specified overlay on this physical display.

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayDeregister(IPhysicalDisplayOverlay * pMe,
                                          uint32    dwOverlayID)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplayOverlay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
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

IPHYSICALDISPLAYOVERLAY_OverlayEnable()

Description: 
   This function enables or disables the specified overlay on for physical 
   display.

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayEnable(IPhysicalDisplayOverlay * pMe,
                               uint32 dwOverlayID,
                               boolean bEnable)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplayOverlay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
   bEnable         : TRUE to enable the overlay, FALSE to disable it
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

IPHYSICALDISPLAYOVERLAY_OverlayUpdate()

Description: 
   This function updates the overlay on the physical display

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayUpdate(IPhysicalDisplayOverlay * pMe,
                                      uint32    dwOverlayID, 
                                      AEERect * pRec)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplayOverlay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
   pRec            : Pointer to the rectangle to update
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

IPHYSICALDISPLAYOVERLAY_OverlayClear()

Description: 
   This function clears the area that this overlay takes on the physical display

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayClear(IPhysicalDisplayOverlay * pMe,
                                      uint32    dwOverlayID)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplayOverlay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
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
IPHYSICALDISPLAYOVERLAY_Invoke()

Description: 
   This function causes the specified method to be invoked. This is not intended 
   to be used from application code, the user should use the wrapper function
   for the method they are trying to invoke.

Prototype:

   int IPHYSICALDISPLAYOVERLAY_Invoke(IPhysicalDisplayOverlay * pMe, int nMethod, 
                                              int nIn, int nOut, 
                                              int nhIn, int nhOut,
                                              const uint32 va[]);

Parameters:
===pre>
   pMe    : Pointer to the IPhysicalDisplayOverlay interface object.
   nMethod: the method to invoke on the remote object
   nIn    : the number of input buffers 
   nOut   : the number of output buffers
   nhIn   : the number of input handles
   nhOut  : the number of output handles
   va     : the arguments to the remote method
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None

================================================================================

IPHYSICALDISPLAYOVERLAY_ValidateBitmap()

Description: 
   This function checks to see if the bitmap is supported on the physical display.

Prototype:

   int IPHYSICALDISPLAYOVERLAY_ValidateBitmap(IPhysicalDisplayOverlay * pMe,
                                       IBitmap *pBitmap)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplayOverlay interface object.
   pBitmap         : Pointer to the IBitmap that will be checked
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
*/

/*lint -restore*/
#endif //__AEEPHYSICALDISPLAYOVERLAY_H__
