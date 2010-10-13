#ifndef _AEEAFLAGS_H_
#define _AEEAFLAGS_H_

/*======================================================
FILE:  AEEAFlags.h

SERVICES:  Include file event type definitions

GENERAL DESCRIPTION:
   Defines BREW Applet Flags.

Copyright © 1999,2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

//
// Applet Flags
//

#define AEE_AFLAG_HIDDEN          0x0001      // Applet is hidden
#define AEE_AFLAG_CFG             0x0002      // Applet has a CFG menu
#define AEE_AFLAG_SYSTEM_CFG      0x0004      // Applet has a hidden CFG menu
#define AEE_AFLAG_TOOL            0x0008      // Applet is a tool
#define AEE_AFLAG_GAME            0x0010      // Applet is a game
#define AEE_AFLAG_PIM             0x0020      // Applet is a PIM
#define AEE_AFLAG_WEB             0x0040      // Applet is a Web Applet
#define AEE_AFLAG_PHONE           0x0080      // Applet is a Phone Applet (requires PL_SYSTEM)
#define AEE_AFLAG_SCREENSAVER     0x0100      // Applet is a Screen Saver
#define AEE_AFLAG_RUNNING         0x0200      // Applet is top-visible or Suspended
#define AEE_AFLAG_POPUP           0x1000      // Applet is popup - no display clear
#define AEE_AFLAG_STATIC          0x4000      // System use only - Static application
#define AEE_AFLAG_BADSHUTDOWN     0x8000      // System use only - Applet failed to shutdown properly

#endif
