#ifndef OEMDEVICEINFOEX_H
#define OEMDEVICEINFOEX_H

/*=============================================================================

FILE: OEMDeviceInfoEx.h

SERVICES: 

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006-2008 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //source/qcom/qct/platform/brew/brewpk/rel/3.1.5sp/pk/inc/OEMDeviceInfoEx.h#6 $
  $DateTime: 2009/03/18 02:28:34 $
  $Author: daggarwa $
  $Change: 865673 $

=============================================================================*/


/*===========================================================================

                    INCLUDE FILES

===========================================================================*/
#include "AEEShell.h"
#include "AEEDeviceNotifier.h"
#include "AEEDeviceInfoEx.h"
#include "OEMConfig.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================

                    FUNCTION DECLARATIONS

===========================================================================*/

static __inline int OEM_SetDeviceInfoFlipOpen(boolean flipOpen)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_FLIP_OPEN,
                              (void *) &flipOpen,
                              sizeof(boolean));
}

static __inline int OEM_SetDeviceInfoOrientation(AEEScrOrientation orient)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_SCR_ORIENTATION,
                              (void *) &orient,
                              sizeof(AEEScrOrientation));
}

static __inline int OEM_SetDeviceInfoSpeakerOn(boolean speakerOn)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_SPEAKER_ON,
                              (void *) &speakerOn,
                              sizeof(boolean));
}

static __inline int OEM_SetDeviceInfoHeadphoneOn(boolean headphoneOn)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_HEADPHONE_ON,
                              (void *) &headphoneOn,
                              sizeof(boolean));
}

static __inline int OEM_SetDeviceInfoStereoHeadphoneOn(boolean headphoneOn)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_STEREO_HEADPHONE_ON,
                              (void *) &headphoneOn,
                              sizeof(boolean));
}

static __inline int OEM_SetDeviceInfoUSBHFKOn(boolean hfkOn)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_USB_HFK_ON,
                              (void *) &hfkOn,
                              sizeof(boolean));
}

static __inline int OEM_SetDeviceInfoHFKOn(boolean hfkOn)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_HFK_ON,
                              (void *) &hfkOn,
                              sizeof(boolean));
}

static __inline int OEM_SetDeviceInfoSDACOn(boolean sdacOn)
{
   return OEM_SetDeviceInfoEx(AEE_DEVICESTATE_SDAC_ON,
                              (void *) &sdacOn,
                              sizeof(boolean));
}

#endif /* OEMDEVICEINFOEX_H */
