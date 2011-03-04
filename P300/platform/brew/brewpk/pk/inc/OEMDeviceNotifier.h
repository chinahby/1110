#ifndef _OEMDEVICENOTIFIER_H_
#define _OEMDEVICENOTIFIER_H_
/*======================================================
FILE:      oemdevicenotifier.h

SERVICES:  Device Level Notifier.

GENERAL DESCRIPTION:

   Device-level notification for such as flip and keyguard events.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IDeviceNotifier

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEComdef.h"
#include "AEEInterface.h"
#include "AEEDeviceNotifier.h"

typedef struct _IDeviceNotifier IDeviceNotifier;

AEEINTERFACE(IDeviceNotifier)
{
   INHERIT_INotifier(IDeviceNotifier);
};

#define IDEVICENOTIFIER_AddRef(o)         AEEGETPVTBL((o),IDeviceNotifier)->AddRef((o))
#define IDEVICENOTIFIER_Release(o)        AEEGETPVTBL((o),IDeviceNotifier)->Release((o))
#define IDEVICENOTIFIER_SetMask(o,m)      AEEGETPVTBL((o),IDeviceNotifier)->SetMask((o),(m))


#define AEE_SEND_FLIP_EVT(p) {\
   (void) AEE_Event(EVT_FLIP, (p)->wParam, (p)->dwParam); \
   (void) AEE_Notify(AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_FLIP, p);}

#define AEE_SEND_KEYGUARD_EVT(p) {\
   (void) AEE_Event(EVT_KEYGUARD, (p)->wParam, (p)->dwParam); \
   (void) AEE_Notify(AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_KEYGUARD, p);}

#define AEE_SEND_HEADSET_EVT(p) {\
   (void) AEE_Event(EVT_HEADSET, (p)->wParam, 0); \
   (void) AEE_Notify(AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_HEADSET, p);}

#define AEE_SEND_SCRROTATE_EVT(p) {\
   (void) AEE_Event(EVT_SCR_ROTATE, (p)->wParam, (p)->dwParam); \
   (void) AEE_Notify(AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_SCR_ROTATE, p);}

#define AEE_SEND_MMC_NOTIFY(p) {\
   (void) AEE_Notify(AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_MMC, p);}

#define AEE_SEND_FLIP_EVT_EX(p,b) {\
   b = AEE_Event(EVT_FLIP, (p)->wParam, (p)->dwParam); \
   (void) AEE_Notify(AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_FLIP, p);}

#define AEE_SEND_KEYGUARD_EVT_EX(p,b) {\
   b = AEE_Event(EVT_KEYGUARD, (p)->wParam, (p)->dwParam); \
   (void) AEE_Notify(AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_KEYGUARD, p);}

/*=====================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================
 
=======================================================================
   INTERFACES DOCUMENTATION
=======================================================================

IDeviceNotifier Interface

Description:
   This is the interface for device-level notifications for EVT_FLIP, EVT_KEYGUARD,
   EVT_HEADSET and EVT_SCR_ROTATE. This interface derives from INotifier.  

See Also:
   INotifier Interface

======================================================================= 
IDeviceNotifier_AddRef()
This function is inherited from IBASE_AddRef().
======================================================================= 
IDeviceNotifier_Release()
This function is inherited from IBASE_AddRef().
======================================================================= 
IDeviceNotifier_SetMask()
This function is inhereited from INOTIFIER_SetMask()
======================================================================= 

AEE_SEND_FLIP_EVT()

Description:
   Sends EVT_FLIP to the active BREW app and notifies all the registered
   application.

Prototype:
   void AEE_SEND_FLIP_EVT(AEEDeviceNotify * p);

Parameters:
   p [in]: Pointer to a AEEDeviceNotify variable where AEEDeviceNotify::wParam
           and AEEDeviceNotify::dwParam are set to wParam and dwParam of EVT_FLIP.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   EVT_FLIP   

======================================================================= 

AEE_SEND_KEYGUARD_EVT()

Description:
   Sends EVT_KEYGUARD to the active BREW app and notifies all the registered
   application.

Prototype:
   void AEE_SEND_KEYGAURD_EVT(AEEDeviceNotify * p);

Parameters:
   p [in]: Pointer to a AEEDeviceNotify variable where AEEDeviceNotify::wParam
           and AEEDeviceNotify::dwParam are set to wParam and dwParam of EVT_KEYGUARD.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   EVT_KEYGUARD 

======================================================================= 

AEE_SEND_HEADSET_EVT()

Description:
   Sends EVT_HEADSET to the active BREW app and notifies all the registered
   application.

Prototype:
   void AEE_SEND_HEADSET_EVT(AEEDeviceNotify * p);

Parameters:
   p [in]: Pointer to a AEEDeviceNotify variable where AEEDeviceNotify::wParam
           and AEEDeviceNotify::dwParam are set to wParam and dwParam of EVT_HEADSET.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   EVT_HEADSET

======================================================================= 

AEE_SEND_SCRROTATE_EVT()

Description:
   Sends EVT_SCR_ROTATE to the active BREW app and notifies all the registered
   application.

Prototype:
   void AEE_SEND_SCRROTATE_EVT(AEEDeviceNotify * p);

Parameters:
   p [in]: Pointer to a AEEDeviceNotify variable where AEEDeviceNotify::wParam
           and AEEDeviceNotify::dwParam are set to wParam and dwParam of EVT_SCR_ROTATE.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   EVT_SCR_ROTATE 
======================================================================= 

AEE_SEND_MMC_NOTIFY()

Description:
   Notifies all the registered application (EVT_NOTIFY event) with the
   MMC insert/remove action.

Prototype:
   void AEE_SEND_MMC_NOTIFY(AEEDeviceNotify * p);

Parameters:
   p [in]: Pointer to a AEEDeviceNotify variable where AEEDeviceNotify::wParam
   is set to AEE_INSERT_MMC or AEE_REMOVE_MMC for MMC card insertion or removal
   as the case may be, and AEEDeviceNotify::dwParam is set to indicate the MMC 
   which has been inserted or removed. E.g 0 for AEEFS_CARD0_DIR.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=====================================================================
AEE_SEND_FLIP_EVT_EX()

Description:
Sends EVT_FLIP to the active BREW app and notifies all the registered
application.
Populates the  boolean variable b with the return value from AEE_Event.

Prototype:
void AEE_SEND_FLIP_EVT_EX(AEEDeviceNotify * p, boolean b );

Parameters:
p [in]: Pointer to a AEEDeviceNotify variable where AEEDeviceNotify::wParam
and AEEDeviceNotify::dwParam are set to wParam and dwParam of EVT_FLIP.
b [out]: boolean variable that is populated with the return value from 
AEE_Event

Return Value:
None

Comments:
Sample code snippet:
AEEDeviceNotify AeeDevNotifyStruct;
boolean bHandled;
<code to set AeeDevNotifyStruct members, etc here>
AEE_SEND_FLIP_EVT_EX(&AeeDevNotifyStruct, bHandled);

Side Effects:
None

See Also:
EVT_FLIP   
AEE_Event
AEE_Notify

======================================================================= 

AEE_SEND_KEYGUARD_EVT_EX()

Description:
Sends EVT_KEYGUARD to the active BREW app and notifies all the registered
application.
Populates the  boolean variable b with the return value from AEE_Event.

Prototype:
void AEE_SEND_KEYGAURD_EVT_EX(AEEDeviceNotify * p, boolean b);

Parameters:
p [in]: Pointer to a AEEDeviceNotify variable where AEEDeviceNotify::wParam
and AEEDeviceNotify::dwParam are set to wParam and dwParam of EVT_KEYGUARD.
b [out]: boolean variable that is populated with the return value from 
AEE_Event

Return Value:
None

Comments:
Sample code snippet:
AEEDeviceNotify AeeDevNotifyStruct;
boolean bHandled;
<code to set AeeDevNotifyStruct members, etc here>
AEE_SEND_KEYGUARD_EVT_EX(&AeeDevNotifyStruct, bHandled);

Side Effects:
None

See Also:
EVT_KEYGUARD
AEE_Event
AEE_Notify

=======================================================================*/
#endif /* _OEMDEVICENOTIFIER_H_ */

