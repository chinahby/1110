#ifndef CUSTOMOEMCONFIGITEMS_H
#define CUSTOMOEMCONFIGITEMS_H
/*=============================================================================

FILE: CustomOEMConfigItems.h

SERVICES:  Non-standard IConfig items 

GENERAL DESCRIPTION:
   This file defines the IConfig items that are used by the UI that are 
   implemented at the OEM layer and are not currently supported by the BREW SDK.

   The caller must not assume that these items are actually supported by the 
   OEM layer.  If the return value from ICONFIG_GetItem() is not SUCCESS, the 
   caller should take appropriate action, such as using default values instead. 
   
   NOTE THAT THIS FILE MUST BE MAINTAINED WITH IDENTICAL CONTENT TO THE
   VERSION IN THE uiOne HDK TREE.
 
PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004-2007 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/inc/CustomOEMConfigItems.h#8 $
  $DateTime: 2008/12/30 02:21:57 $
  $Author: krishnac $
  $Change: 812537 $

=============================================================================*/


#include "AEEConfig.h"

#define CUSTOMOEMCONFIG_BASE 0x9000


////
// Default Backlight setting.
//
// Type: byte, 0: never on, 1-254: secs before going off, 255: always on
// 
#define CFGI_BACK_LIGHT         (CUSTOMOEMCONFIG_BASE + 0)


////
#ifndef FEATURE_BMP
// Service alert (enable/disable)
//
// Type: byte, Valid values defined by ALERT_xxx
#define CFGI_SVC_ALERT         (CUSTOMOEMCONFIG_BASE + 1)
#endif

////
// Ext Pwr Backlight (on/off)
//
// Type: byte, Valid values defined by EXTPWRBKLT_xxx
#define CFGI_EXTPWR_BK_LIGHT        (CUSTOMOEMCONFIG_BASE + 2)

#ifndef FEATURE_BMP
////
// Voice privacy (standard/enhanced)
//
// Type: byte, Valid values defined by VOICEPRIVACY_xxx
#define CFGI_VOICEPRIVACY     (CUSTOMOEMCONFIG_BASE + 3)

#define VOICEPRIVACY_STD      0
#define VOICEPRIVACY_ENH      1


////
// RTRE Configuration (CDMA R-UIM only)
// 
// Type: byte, Valid values defined by RTRE_CONFIG_xxx
#define CFGI_RTRE_CONFIG       (CUSTOMOEMCONFIG_BASE + 4)

#define RTRE_CONFIG_RUIMONLY             0
#define RTRE_CONFIG_NVONLY               1
#define RTRE_CONFIG_RUIMORDROPBACK       2
#define RTRE_CONFIG_SIMACCESS            3
#define RTRE_CONFIG_DISABLED             255
#endif

/////
// UnitTest Activation status
//
// Type:  boolean.  TRUE if UnitTest is currently active
#define CFGI_UNITTEST_ACTIVE   (CUSTOMOEMCONFIG_BASE + 5)

#ifndef FEATURE_BMP
////
// Radio Technology supported by the phone
//
// Type: byte, Valid values defined by RADIO_TECH_xxx
#define CFGI_RADIO_TECH        (CUSTOMOEMCONFIG_BASE + 6)

#define RADIO_TECH_CDMA                  1
#define RADIO_TECH_GSM                   2
#define RADIO_TECH_WCDMA                 4
#endif

////
// Contrast level
//
// Type: byte.  Valid values are defined by CONTRAST_LVL_xxx
#define CFGI_CONTRAST_LVL     (CUSTOMOEMCONFIG_BASE + 7)
 
#define CONTRAST_LVL_MIN      0
#define CONTRAST_LVL_MAX      7

#ifndef FEATURE_BMP
////
// Factory Test Mode
//
// Type: boolean.  TRUE if the device is in factory test mode
// Read-only
#define CFGI_FACTORY_TEST_MODE         (CUSTOMOEMCONFIG_BASE + 8)

////
// Firmware Version
//
// Type: AECHAR string Read-only
#define CFGI_FIRMWARE_VERSION  (CUSTOMOEMCONFIG_BASE + 9)

#define FIRMWAREVERSION_MAX_LEN  32

//Auto Answer
//
//Type: Boolean. True if auto answer is on.
//Corresponds to NV_AUTO_ANSWER_I
#define CFGI_AUTO_ANSWER  (CUSTOMOEMCONFIG_BASE + 10)


//TTY mod

//Type: byte. 0 - Full, 1- Talk, 2 - Hear, 3 - Off
#define CFGI_TTY  (CUSTOMOEMCONFIG_BASE + 11)

#define TTY_FULL 0
#define TTY_TALK 1
#define TTY_HEAR 2
#define TTY_OFF  3
#endif

//Leap Seconds since start of System Time
//
//Type: uint8
//Corresponds to NV_DB_LP_SEC_I
#define CFGI_LP_SEC  (CUSTOMOEMCONFIG_BASE + 12)


//Offset local time from System Time, i.e. timezone offset
//
//Type: uint8
//Corresponds to NV_DB_LTM_OFF_I 
#define CFGI_LTM_OFF  (CUSTOMOEMCONFIG_BASE + 13)

//Daylight savings time indicator
//
//Type: boolean
//Corresponds to NV_DB_DAYLT_I  
#define CFGI_DAYLT  (CUSTOMOEMCONFIG_BASE + 14)


////
// If this value is TRUE, the messaging daemons will not run.  This item 
// can be used to prevent the messaging daemons from conflicting with 
// another messaging client on the device.
//
// Type:  boolean
//
// NOTE: This item is is shared with CustomConfigItems.h, as it must be 
// accessed by the OEM layer.  However it is still implemented in 
// the CustomConfigItems module.
#ifndef CFGI_MSG_DISABLE_DAEMONS
#define CFGI_MSG_DISABLE_DAEMONS (0xA000 + 63)
#else
#if CFGI_MSG_DISABLE_DAEMONS != (0xA000 + 63)
#error CFGI_MSG_DISABLE_DAEMONS value incorrect
#endif 
#endif



#endif /* CUSTOMOEMCONFIGITEMS_H */
