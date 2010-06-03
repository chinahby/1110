#ifndef  AEEIBITMAPDEV_H
#define  AEEIBITMAPDEV_H
/*=============================================================================

FILE:          AEEIBitmapDev.h

SERVICES:      IBitmapDev interface

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2001-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEIQI.h"
#include "AEECallback.h"

#define AEEIID_IBitmapDev 0x01012719

#define INHERIT_IBitmapDev(iname) \
   INHERIT_IQI(iname); \
   int      (*Update)               (iname *po); \
   boolean  (*IsEnabled)            (iname *po); \
   int      (*NotifyEnable)         (iname *po, AEECallback *pcb)

AEEINTERFACE_DEFINE(IBitmapDev);

static __inline uint32
IBitmapDev_AddRef(IBitmapDev *po)
{
   return AEEGETPVTBL(po,IBitmapDev)->AddRef(po);
}

static __inline uint32
IBitmapDev_Release(IBitmapDev *po)
{
   return AEEGETPVTBL(po,IBitmapDev)->Release(po);
}

static __inline int
IBitmapDev_QueryInterface(IBitmapDev *po, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(po,IBitmapDev)->QueryInterface(po, cls, ppo);
}

static __inline int
IBitmapDev_Update(IBitmapDev *po)
{
   return AEEGETPVTBL(po,IBitmapDev)->Update(po);
}

static __inline boolean
IBitmapDev_IsEnabled(IBitmapDev *po)
{
   return AEEGETPVTBL(po,IBitmapDev)->IsEnabled(po);
}

static __inline int
IBitmapDev_NotifyEnable(IBitmapDev *po, AEECallback *pcb)
{
   return AEEGETPVTBL(po,IBitmapDev)->NotifyEnable(po, pcb);
}

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IBitmapDev Interface

Description:
   This interface is used for various functions relevent to device bitmaps.  It
   is only implemented for device bitmaps and is obtained by calling a bitmap's
   QueryInterface method with an interface ID of AEEIID_IBitmapDev.

===============================================================================

Function: IBitmapDev_AddRef()

This function is inherited from IQI_AddRef().

===============================================================================

Function: IBitmapDev_Release()

This function is inherited from IQI_Release().

===============================================================================

Function: IBitmapDev_QueryInterface()

This function is inherited from IQI_QueryInterface().

===============================================================================

Function: IBitmapDev_Update()

Description:
   This function copies the bitmap to its associated display synchronously.
   If dirty rectangle optimization is implemented, only areas of the display
   marked as dirty will be copied.

Prototype:
   int IBitmapDev_Update(IBitmapDev *po);

Parameters:
   po       [in]: Pointer to IBitmapDev interface.

Return Value:
   SUCCESS: if functiona executed correctly.~
   Other error codes may be returned depending on the implementation.

Comments:
   This function has no effect if the bitmap is disabled.

Side Effects:
   None

See Also:
   IBITMAP_Invalidate()

===============================================================================

Function: IBitmapDev_IsEnabled()

Description:
   This function is used to query the enabled state of the bitmap.

Prototype:
   boolean IBitmapDev_IsEnabled(IBitmapDev *po);

Parameters:
   po       [in]: Pointer to IBitmapDev interface.

Return Value:
   TRUE: if this bitmap is enabled.~
   FALSE: otherwise.

Comments:
   While the bitmap is disabled, no drawing operations are allowed.  IBitmap
   drawing functions will have no effect.  If an application has direct access
   to the bitmap's pixel buffer through IDIB, it is important that the app
   refrain from modifying the pixel buffer while the bitmap is disabled.

Side Effects:
   None

See Also:
   IBitmapDev_NotifyEnable()

===============================================================================

Function: IBitmapDev_NotifyEnable()

Description:
   This function registers for a notification of a change to the bitmap's
   enabled state.  The callback will be triggered when the state of the bitmap
   changes either from enabled to disabled or disabled to enabled.

Prototype:
   int IBitmapDev_NotifyEnable(IBitmapDev *po, AEECallback *pcb);

Parameters:
   po       [in]: Pointer to IBitmapDev interface.
   pcb      [in]: Pointer to callback structure.

Return Value:
   SUCCESS: if function executed correctly.
   EBADPARM: if pcb is NULL.~
   Other error codes may be returned depending on the implementation.

Comments:
   It is desirable to reregister for this notification in the callback function,
   so as not to miss any state changes.

   This callback is also triggered when this bitmap goes away.

Side Effects:
   None

See Also:
   IBitmapDev_IsEnabled()

===============================================================================
*/

#endif /* AEEIBITMAPDEV_H */

