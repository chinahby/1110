#ifndef  AEEIANNUNCIATORCONTROL_H
#define  AEEIANNUNCIATORCONTROL_H
/*==============================================================================
FILE:          AEEIAnnunciatorControl.h

SERVICES:      IAnnunciator Control interface

DESCRIPTION:   

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

/*------------------------------------------------------------------------------
      Include Files
------------------------------------------------------------------------------*/

#include "AEEIQI.h"
#include "AEERect.h"

/*------------------------------------------------------------------------------
      Type Declarations
------------------------------------------------------------------------------*/

#define AEEIID_IAnnunciatorControl       0x010628c9

//******************************************************************************
//
// IAnnunciatorControl Interface
//
//******************************************************************************
#ifdef CUST_EDITION	
#define INHERIT_IAnnunciatorControl(iname) \
   INHERIT_IQI(iname); \
   int      (*GetRegion)            (iname *po, AEERect *pRegion); \
   void     (*Enable)               (iname *po, boolean bEnable); \
   void     (*EnableEx)             (iname *po, boolean bEnable, boolean bForceRearaw)
#else
#define INHERIT_IAnnunciatorControl(iname) \
   INHERIT_IQI(iname); \
   int      (*GetRegion)            (iname *po, AEERect *pRegion); \
   void     (*Enable)               (iname *po, boolean bEnable)
#endif
AEEINTERFACE_DEFINE(IAnnunciatorControl);

static __inline uint32 IAnnunciatorControl_AddRef(IAnnunciatorControl *po)
{
   return AEEGETPVTBL(po,IAnnunciatorControl)->AddRef(po);
}

static __inline uint32 IAnnunciatorControl_Release(IAnnunciatorControl *po)
{
   return AEEGETPVTBL(po,IAnnunciatorControl)->Release(po);
}

static __inline int IAnnunciatorControl_QueryInterface(IAnnunciatorControl *po, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(po,IAnnunciatorControl)->QueryInterface(po, cls, ppo);
}

static __inline int IAnnunciatorControl_GetRegion(IAnnunciatorControl *po, AEERect *pRegion)
{
   return AEEGETPVTBL(po,IAnnunciatorControl)->GetRegion(po, pRegion);
}

static __inline void IAnnunciatorControl_Enable(IAnnunciatorControl *po, boolean bEnable)
{
   AEEGETPVTBL(po,IAnnunciatorControl)->Enable(po, bEnable);
}

#ifdef CUST_EDITION	
static __inline void IAnnunciatorControl_EnableEx(IAnnunciatorControl *po, boolean bEnable, boolean bForceRearaw)
{
   AEEGETPVTBL(po,IAnnunciatorControl)->EnableEx(po, bEnable, bForceRearaw);
}
#endif /*CUST_EDITION*/

/*==============================================================================
   INTERFACE DOCUMENTATION
================================================================================

Interface Name: IAnnunciatorControl

Description:
   This interface is used by OEM AppFrame to get the size of the annunciator
   region for each display and functions to enable and disabling the 
   annunciator region.

================================================================================

Function: IAnnunciatorControl_AddRef()

This function is inherited from IQI_AddRef().

================================================================================

Function: IAnnunciatorControl_Release()

This function is inherited from IQI_Release().

================================================================================

Function: IAnnunciatorControl_QueryInterface()

This function is inherited from IQI_QueryInterface().

================================================================================
Function: IAnnunciatorControl_GetRegion()

Description:
   This function is called by BREW during display initialization for an 
   application.  This should return the location and size of the annunciator
   region.

Prototype:
   int IAnnunciatorControl_GetRegion(IAnnunciatorControl *po, AEERect *pRegion)

Parameters:
   po             [in]: Pointer to the IAnnunciatorControl interface.
   pRegion        [out]: Pointer to an AEERect structure that will contain the
                         size and location of the annunciator region. 

Return Value:
   SUCCESS if the initialization completed
   EBADPARM if the pRect pointer is NULL
   EUNSUPPORTED if the annunciator for this display is not supported.
   EFAILED for miscellaneous errors
   Other errors codes may be returned as appropriate.

Comments:  
   None

Side Effects: 
   None

See Also:
   None

================================================================================
Function: IAnnunciatorControl_Enable()

Description:
   This function is used to enable and disable the annunciator region for a 
   given display.

Prototype:
   void IAnnunciatorControl_Enable(IAnnunciatorControl *po, boolean bEnable)

Parameters:
   po             [in]: Pointer to the IAnnunciatorControl interface.
   bEnable        [in]: TRUE is the annunciator should be enable, otherwise
                        FALSE.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   None

==============================================================================*/

#endif /* AEEIANNUNCIATORCONTROL_H */

