#ifndef _AEEDEVICENOTIFIER_H_
#define _AEEDEVICENOTIFIER_H_
/*======================================================
FILE:      aeedevicenotifier.h

SERVICES:  Device Level Notifier.

GENERAL DESCRIPTION:

   Device-level notification for such as flip and keyguard events.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IDeviceNotifier

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 1999-2005,2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEComdef.h"
#include "AEEInterface.h"
#include "AEEClassIDs.h"

// Standard Device Notifier Masks
// 
#define NMASK_DEVICENOTIFIER_FLIP         0x00000001
#define NMASK_DEVICENOTIFIER_KEYGUARD     0x00000002
#define NMASK_DEVICENOTIFIER_HEADSET      0x00000008
#define NMASK_DEVICENOTIFIER_SCR_ROTATE   0x00000010
#define NMASK_DEVICENOTIFIER_SET_CONFIG   0x00000020
#define NMASK_DEVICENOTIFIER_TIMEACQUIRED 0x00000040
#define NMASK_DEVICENOTIFIER_MMC          0x00000080

#define NMASK_DEVICENOTIFIER_ALL          (NMASK_DEVICENOTIFIER_FLIP | NMASK_DEVICENOTIFIER_KEYGUARD | \
                                           NMASK_DEVICENOTIFIER_HEADSET | NMASK_DEVICENOTIFIER_SCR_ROTATE |\
                                           NMASK_DEVICENOTIFIER_SET_CONFIG | NMASK_DEVICENOTIFIER_TIMEACQUIRED |\
                                           NMASK_DEVICENOTIFIER_MMC)

// Device Notification Context Sensitive Data
//
typedef struct _AEEDeviceNotify
{
   uint16      wParam;
   uint32      dwParam;
} AEEDeviceNotify;


// Screen Orientation
//
typedef uint16 AEEScrOrientation;
#define AEE_SO_PORTRAIT       0
#define AEE_SO_LANDSCAPE      1

// MMC insert/remove actions.
#define AEE_INSERT_MMC 0
#define AEE_REMOVE_MMC 1

typedef uint32 AEEFlipInfo;
// AEEFlipInfo 
//  +--------+--------+--------+--------+
//  | x-axis | y-axis | z-axis |   ID   |
//  +--------+--------+--------+--------+
//   LBS                             MSB
//
//  x-axis/y-axis/z-axis field values all express either linear or angular offset values. 
//  Linear offset is expressed either as an absolute value, in OEM-defined linear units, 
//  (or) as an integral percentage of the total linear movement range of the flip. 
//  The only API functions that express linear offset as an absolute value are 
//   IFLIP_GetMinPosition() and IFLIP_GetMaxPosition().
//  Angular offset is in terms of 2 degree increments.
//  For both offset values, 0 values for the x-axis, y-axis and z-axis normally represents 
//  closed position of the flip.
//  ID is the number uniquely identifying a flip for multiple flip devices (starting from 0).
//


// Utility Macros
#define DEVNOTIFY_MAKE(x,y,z,i)  (((uint32)(x)<<24) | ((uint32)(y)<<16) | ((uint32)(z)<<8) | ((uint32)(i)))
#define DEVNOTIFY_GET_X(d)       ((d)>>24 & 0xff)
#define DEVNOTIFY_GET_Y(d)       ((d)>>16 & 0xff)
#define DEVNOTIFY_GET_Z(d)       ((d)>>8  & 0xff)
#define DEVNOTIFY_GET_ID(d)      ((d)     & 0xff)


/*=====================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

Device Notifier Masks

Description:
    This is a 32-bit integer used as mask to register for notification
    events. The following show the mask and the associated event.
===pre>
	Mask						Event
	----------------------------------------	-------------------------
	NMASK_DEVICENOTIFIER_FLIP		         EVT_FLIP
 	NMASK_DEVICENOTIFIER_KEYGUARD		         EVT_KEYGUARD
	NMASK_DEVICENOTIFIER_HEADSET	                 EVT_HEADSET
	NMASK_DEVICENOTIFIER_SCR_ROTATE	                 EVT_SCR_ROTATE
===/pre>	
Definition:
   uint32

Members:
   NMASK_DEVICENOTIFIER_FLIP: 
   
	Flip event notification. Applications can register
	for this notification using ISHELL_RegisterNotify.
	Context sensitive data in AEENotify of EVT_NOTIFY 
	(pData::AEENotify) specifies AEEDeviceNotify structure.

	wParam  : See EVT_FLIP documentation for possible values
        dwParam : See EVT_FLIP documentation for possible values

   NMASK_DEVICENOTIFIER_KEYGUARD: 
   
	Key-guard event notification. Applications can register
	for this notification using ISHELL_RegisterNotify.
	Context sensitive data in AEENotify of EVT_NOTIFY 
	(pData::AEENotify) specifies AEEDeviceNotify structure.

	wParam  : See EVT_KEYGUARD documentation for possible values
        dwParam : See EVT_KEYGUARD documentation for possible values

   NMASK_DEVICENOTIFIER_HEADSET: 
   
	Headset event notification. Applications can register
	for this notification using ISHELL_RegisterNotify.
	Context sensitive data in AEENotify of EVT_NOTIFY 
	(pData::AEENotify) specifies AEEDeviceNotify structure.

	wParam  : See EVT_HEADSET documentation for possible values
        dwParam : See EVT_HEADSET documentation for possible values

   NMASK_DEVICENOTIFIER_SCR_ROTATE: 
   
	Screen rotation notification. Applications can register
	for this notification using ISHELL_RegisterNotify.
	Context sensitive data in AEENotify of EVT_NOTIFY 
	(pData::AEENotify) specifies AEEDeviceNotify structure.

	wParam  : See EVT_SRC_ROTATE documentation for possible values
        dwParam : See EVT_SRC_ROTATE documentation for possible values

   NMASK_DEVICENOTIFIER_SET_CONFIG: 
   
	Set config notification. Applications can register
	for this notification using ISHELL_RegisterNotify.
        context sensitive data in AEENotify of EVT_NOTIFY 
	(pData::AEENotify) specifies the ID of the configuration 
	item updated using ICONFIG_SetItem().

	wParam  : Not Used
	dwParam : Specifies the ID of the configuration item updated using ICONFIG_SetItem()

   NMASK_DEVICENOTIFIER_TIMEACQUIRED: 
   
	The phone now has valid time.  If the
	phone already has valid time when BREW starts up 
	then this event will never be emitted by the model 
	-- see the ISTIMEVALID() macro.
   
   NMASK_DEVICENOTIFIER_MMC: 
   
	MMC Card insert/remove notification. Context sensitive data 
	in AEENotify of EVT_NOTIFY (pData::AEENotify) specifies
	AEEDeviceNotify structure.

	wParam:  Specifies whether MMC Card is inserted (AEE_INSERT_MMC) or removed (AEE_REMOVE_MMC).
	dwParam: Specifies the MMC for which this notification is being sent (0 for AEEFS_CARD0_DIR)

Comments:
    None

See Also:
    AEEDeviceNotify

=======================================================================

AEEDeviceNotify

Description:
   This is the structure passed in as the context sensitive data in AEENotify
   of EVT_NOTIFY (pData::AEENotify) for the notifications triggered by:
   - NMASK_DEVICENOTIFIER_FLIP
   - NMASK_DEVICENOTIFIER_KEYGUARD
   - NMASK_DEVICENOTIFIER_HEADSET
   - NMAKE_DEVICENOTIFIER_SCR_ROTATE
   - NMASK_DEVICENOTIFIER_SET_CONFIG
   - NMASK_DEVICENOTIFIER_MMC

Definition:
   struct _AEEDeviceNotify
   {
      uint16      wParam;
      uint32      dwParam;
   } AEEDeviceNotify;

Members:
   wParam: Will takes different values based on NMASK_DEVICENOTIFIER_X. See the
           corresponding NMASK_DEVICENOTIFIER_X for exact values. 

   dwParam: Will takes different values based on NMASK_DEVICENOTIFIER_X. See the
            corresponding NMASK_DEVICENOTIFIER_X for exact values.

Comments:
   None

See Also:
   EVT_NOTIFY, EVT_FLIP, EVT_KEYGUARD,  EVT_HEADSET, EVT_SCR_ROTATE
  
=======================================================================

AEEScrOrientation

Description:
   This data type is used to represent screen orientation.
   This data type can have the following values:
      - AEE_SO_PORTRAIT       0
      - AEE_SO_LANDSCAPE      1

   This is the data type passed in as
      1) The context sensitive data in AEENotify of EVT_NOTIFY (pData::AEENotify) 
      for the notifications triggered by NMAKE_DEVICENOTIFIER_SCR_ROTATE, and 
      2) As wParam, upon the event EVT_SCR_ROTATE.

Definition:
   typedef uint16 AEEScrOrientation;
   #define AEE_SO_PORTRAIT       0
   #define AEE_SO_LANDSCAPE      1

Members:
    None.

Comments:
   None

See Also:
   EVT_SCR_ROTATE, NMAKE_DEVICENOTIFIER_SCR_ROTATE

=======================================================================

AEEFlipInfo

Description:
   This is a 32-bit unsigned integer used for dwParam of EVT_FLIP and EVT_KEYGUARD, 
   and in IFlip interface functions.
===pre>

     +--------+--------+--------+--------+
     | x-axis | y-axis | z-axis |   ID   |
     +--------+--------+--------+--------+
      LSB                             MSB
===/pre>   
   ID is the number uniquely identifying a flip for multiple flip devices (starting from 0).

   x-axis/y-axis/z-axis values are expressed as one of the following cases:
   ~
   Case 1: OEM-defined linear units (absolute value).  
   
   Case 2: As a percentage of the total linear range (percentage value).
   
   Case 3: In angular units, each unit representing 2 degrees.
*

   Use the flipPositionType member of AEEFlipInfoEx to determine if the x-axis, y-axis, z-axis
   fields represent linear values (case 1, case 2) (or) angles (case 3).
    - If flipPositionType is AEEFLIP_POSTYPE_LINEAR, then the fields represent linear values (case 1, case 2).
    - If flipPositionType is AEEFLIP_POSTYPE_ANGLE, then the fields represent angular values (case 3).
          
   Further differentiation between case 1 and case 2:
       Only IFLIP_GetFlipMinPosition and IFLIP_GetFlipMaxPosition return an absolute value for 
       x-axis, y-axis, z-axis in AEEFlipInfo (case 1). The units used for the absolute values are OEM 
       and device specific. 
         
       All other IFlip interface functions, and dwParam of EVT_FLIP and EVT_KEYGUARD, for a linear 
       representation, express the field values as a percentage of the linear movement range. (case 2)
         
       For example, if a flip has 
       1) flipPositionType = AEEFLIP_POSTYPE_LINEAR in AEEFlipInfoEx, and
       2) IFlip_GetFlipMinPosition returns AEEFlipInfo with an x-axis value of 0 units, and
       3) IFlipGetMaxPosition returns AEEFlipInfo with an x-axis value if 200 units, and
       4) On EVT_FLIP, AEEFlipInfo has an x-axis value of 50 (percentage), 

       then the actual x-axis value of the flip is calculated as follows:
       = (Current x-axis percentage / 100) * (Max x-axis value  - Min x-axis value ) 
       = (50 / 100) * (200-0) = 100 units. 

   The figures x-axis flip, y-axis flip, z-axis flip and y-slide flip illustrate flip movement and 
   their relation to AEEFlipInfo.

   You can use the following macros to construct this value or get components from this value:
   - DEVNOTIFY_MAKE: Given the x, y, z and ID, it'll construct the AEEFlipInfo value.
   - DEVNOTIFY_GET_X: Given an AEEFlipInfo value, it will get the x component value.
   - DEVNOTIFY_GET_Y: Given an AEEFlipInfo value, it will get the y component value.
   - DEVNOITFY_GET_Z: Given an AEEFlipInfo value, it will get the z component value.
   - DEVNOTIFY_GET_ID: Given an AEEFlipInfo value, it will get the ID component value.

Members:
   None

Comments:
   None

See Also:
   EVT_FLIP,
   EVT_KEYGUARD,
   AEEFlipInfoEx,
   IFlip

=====================================================================*/

#endif /* _AEEDEVICENOTIFIER_H_ */

