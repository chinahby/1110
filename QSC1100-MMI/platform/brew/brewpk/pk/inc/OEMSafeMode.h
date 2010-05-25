/*======================================================
FILE:  OEMSafemode.h

SERVICES:   OEM safemode interface

GENERAL DESCRIPTION:

Safemode related definitions for the OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(OEM_SAFEMODE_H)
#define OEM_SAFEMODE_H  1

#include "AEEShell.h"

#if defined(__cplusplus)
extern "C" {
#endif

//    The AEESafeMode mechanism is used by BREW to prevent faulty BREW
//    applications from repeatedly crashing the device. The mechanism
//    puts the device in probation mode on restart after the first crash,
//    starts a timer (configurable by CFGI_SAFEMODE_TIMER), and restores
//    run level to Normal when the timer expires. If the device crashes
//    again before the timer expires, the device is put into safe mode on
//    restart, and the timer is started again, and all creation of BREW
//    applications is supressed till the device returns to normal
//    mode. Normal mode may be restored by recalling or upgrading the
//    faulty application. The diagram below describes the transitions
//    between Normal, Probation, and Safe modes.
//  
//    Normal ---crash-----> Probation -----crash----->Safe
//       ^                     |                         |
//       |                     |                         |
//       |_time/recall/upgrade_|                         |
//       |                                               |
//       |________________time/recall/upgrade____________| 
//
//
//   Before creating a new application, BREW checks the current run level
//   of the device using AEESafeMode_GetRunLevel, and refuses the
//   creation if the current run level is Safe and
//   OEM_IsClsOKInSafeMode() returns FALSE. During exit, BREW calls
//   AEESafeMode_Reset to allow the device to restart in its preferred
//   mode (usually NORMAL).


#define AEESAFEMODE_RUNLEVEL_SAFE       0
#define AEESAFEMODE_RUNLEVEL_PROBATION  1
#define AEESAFEMODE_RUNLEVEL_NORMAL     2
#define AEESAFEMODE_RUNLEVEL_MAX        3

extern boolean OEM_IsClsOKInSafeMode(uint32 clsid);

#if defined(__cplusplus)
}
#endif

/*=======================================================================
  INTERFACES   DOCUMENTATION
=============================================================
AEESafeMode Interface


Description:

   The AEESafeMode mechanism is used by BREW to prevent faulty BREW
applications from repeatedly crashing the device. The mechanism
puts the device in probation mode on restart after the first crash,
starts a timer (configurable by CFGI_SAFEMODE_TIMER), and restores
run level to Normal when the timer expires. If the device crashes
again before the timer expires, the device is put into safe mode on
restart, and the timer is started again, and all creation of BREW
applications is supressed till the device returns to normal
mode. Normal mode may be restored by recalling or upgrading the
faulty application. The diagram below describes the transitions
between Normal, Probation, and Safe modes.
 ===pre>
Normal ---crash-----> Probation -----crash----->Safe
   ^                     |                         |
   |                     |                         |
   |_time/recall/upgrade_|                         |
   |                                               |
   |________________time/recall/upgrade____________| 
===/pre>
 
  Before creating a new application, BREW checks the current run level
  of the device using AEESafeMode_GetRunLevel, and refuses the
  creation if the current run level is Safe and
  OEM_IsClsOKInSafeMode() returns FALSE. During exit, BREW calls
  AEESafeMode_Reset to allow the device to restart in its preferred
  mode (usually NORMAL).
==================================================================*/
#endif // !defined(OEM_SAFEMODE_H)
