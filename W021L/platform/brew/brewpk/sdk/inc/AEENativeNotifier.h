#ifndef AEENATIVENOTIFIER_H
#define AEENATIVENOTIFIER_H
/*======================================================
FILE:  AEENativeNotifier.h

SERVICES: Native Notifier include file.

GENERAL DESCRIPTION:
   This file contians the Native Notifier definitions.

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "NativeNotifier.bid"

// Native Notifier masks - Idle / Non-Idle Notifications
#define NOTIFY_MASK_NATIVE_IDLE_EVENT          0x00000001
#define NOTIFY_MASK_NATIVE_NONIDLE_EVENT       0x00000002

#define NMASK_NATIVENOTIFIER_IDLE              NOTIFY_MASK_NATIVE_IDLE_EVENT
#define NMASK_NATIVENOTIFIER_NONIDLE           NOTIFY_MASK_NATIVE_NONIDLE_EVENT

#define NMASK_NATIVENOTIFIER_ALL               (NMASK_NATIVENOTIFIER_IDLE | \
                                                NMASK_NATIVENOTIFIER_NONIDLE)

/*=====================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

Native Notifier Masks

Description:
    This is a 32-bit integer used to identify the notification from 
    native events. These events correspond to transitions to native Idle
    or Non-Idle states

Definition:
   uint32

Members:
   NMASK_NATIVENOTIFIER_IDLE: Idle event notification
   NMASK_NATIVENOTIFIER_NONIDLE: Non-Idle event notification

Comments:
    None

See Also:
    None

=======================================================================*/

#endif // AEENATIVENOTIFIER_H
