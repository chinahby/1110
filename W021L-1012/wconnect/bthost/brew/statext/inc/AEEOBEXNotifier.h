
#ifndef _AEEOBEXNOTIFIER_H_
#define _AEEOBEXNOTIFIER_H_

/*===========================================================================

FILE:      AEEOBEXNotifier.h

SERVICES:  AEEOBEXNotifier

DESCRIPTION: This is the header file for OBEXNotifier BREW Extension

PUBLIC CLASSES:  Not Applicable

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/inc/AEEOBEXNotifier.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $
                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/13/04   sp      initial checkin. unit tested for IOBEXServer API
3/24/04    sp      corrected featurization so that header file is not featurized
=============================================================================*/
#include "AEE.h"
#include "AEEInterface.h"
#include "AEE_OEM.h"

#include "AEEOBEX.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AEEOBEX_APP_ID_NULL           0xFFFF /* The NULL/invalid app */

/*========================================================================
  Notifications
======================================================================= */

/* BREW generated ID  */
#define AEECLSID_OBEX_NOTIFIER      0x0102d07a
// Notification Masks... 32 of them one for supported application
//This obviously limits the number of applications that can use IOBEX to 16. But the advantage of this
//approach is that each application can be sure that it will get a EVT_NOTIFY only when there is an
//event that needs its attention. We wouldnt need this mechanism if BREW provided a way to ensure
//that EVT_NOTIFY is send only to a specific applet

#define NMASK_OBEX_GOEP 0x1
#define NMASK_OBEX_OPP  0x2
#define NMASK_OBEX_FTP  0x4


#define AEEOBEX_ENMASK (ERROR_USER + 1)


typedef struct _IOBEXNotifier IOBEXNotifier;

AEEINTERFACE(IOBEXNotifier)
{
    INHERIT_INotifier(IOBEXNotifier);
    int (*DoNotify)(IOBEXNotifier* po, AEEOBEXNotificationType* pNotification,
                    ACONTEXT* pac);
};



#define IOBEXNotifier_DoNotify(p,pn,pac)         \
        AEEGETPVTBL((p),IOBEXNotifier)->DoNotify((p),(pn),(pac))

#define IOBEXNotifier_Release(p)                 \
        AEEGETPVTBL((p),IOBEXNotifier)->Release((p))



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================*/


/*=====================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name:  IOBEXNotifier

Description: OBEX Notifier

  IOBEXNotifier allows applications to register for notifications
  of events occurring in Bluetooth driver.

  Users can register for this notification via ISHELL_RegisterNotify().

  The defined notification masks are:
  NMASK_OBEX       notification of OBEX events

  For example, to register to receive all OBEX events:

  ISHELL_RegisterNotify( pShell, myClsID, AEECLSID_OBEX_NOTIFIER,
                         NMASK_OBEX | 0xFFFF0000 );

  When handling EVT_NOTIFY, dwMask field of the AEENotify contains the
  notification mask and the event.  The mask can be extracted by
  GET_NOTIFIER_MASK( dwMask ), and the event by GET_NOTIFIER_VAL( dwMask ).
  The pData field of the AEENotify is a pointer to NotificationData.

======================================================================= */

#ifdef __cplusplus
}
#endif


#endif //_AEEOBEXNOTIFIER_H_
