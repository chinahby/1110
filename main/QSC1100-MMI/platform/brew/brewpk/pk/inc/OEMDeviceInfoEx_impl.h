#ifndef OEMDEVICEINFOEX_IMPL_H
#define OEMDEVICEINFOEX_IMPL_H

/*=============================================================================

FILE: OEMDeviceInfoEx_impl.h

SERVICES: This file is to be included only by the source file that implements
   OEM_GetDeviceInfoEx()

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006-2008 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //source/qcom/qct/platform/brew/brewpk/rel/3.1.5sp/pk/inc/OEMDeviceInfoEx_impl.h#6 $
  $DateTime: 2009/03/18 02:28:34 $
  $Author: daggarwa $
  $Change: 865673 $

=============================================================================*/


/*===========================================================================

                    INCLUDE FILES

===========================================================================*/
#include "OEMDeviceInfoEx.h"
#include "AEE_OEMDispatch.h"
#include "OEMConfig.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/



/*===========================================================================

                    STATIC DATA

===========================================================================*/

// TRUE if the headset is connected
static boolean sHeadphoneOn       = FALSE;
// TRUE if the stereo headset is connected
static boolean sStereoHeadphoneOn = FALSE;
// TRUE if the speaker is connected
static boolean sSpeakerOn         = FALSE;
// TRUE if the USB hands free kit is connected
static boolean sUSBHFKOn          = FALSE;
// TRUE if the hands free kit is connected
static boolean sHFKOn             = FALSE;
// TRUE if the Stereo DAC is on
static boolean sSDACOn            = FALSE;

// TRUE if the flip is open
#ifdef AEE_SIMULATOR
static boolean sFlipOpen = TRUE;
#else
static boolean sFlipOpen = FALSE;
#endif

// Screen orientation
static AEEScrOrientation sScreenOrientation = AEE_SO_PORTRAIT;




/*===========================================================================

                    FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION: OEM_SetDeviceInfoEx

DESCRIPTION:  Used by the OEM layer to indicate that a device configuration
              item has changed.

PARAMETERS:
    nItem  [in]:  The item that has changed
   *pBuff  [in]:  New value
    nSize  [in]:  Size of the new value

RETURN VALUE:
   int:  BREW result code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEM_SetDeviceInfoEx(AEEDeviceItem nItem, void *pBuff, int nSize)
{
   switch (nItem) {
   case AEE_DEVICESTATE_HEADPHONE_ON:
      if ( (nSize != (int)sizeof(boolean)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sHeadphoneOn = *((boolean *)pBuff);
      break;

   case AEE_DEVICESTATE_STEREO_HEADPHONE_ON:
      if ( (nSize != (int)sizeof(boolean)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sStereoHeadphoneOn = *((boolean *)pBuff);
      break;

   case AEE_DEVICESTATE_SPEAKER_ON:
      if ( (nSize != (int)sizeof(boolean)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sSpeakerOn = *((boolean *)pBuff);
      break;

   case AEE_DEVICESTATE_USB_HFK_ON:
      if ( (nSize != (int)sizeof(boolean)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sUSBHFKOn = *((boolean *)pBuff);
      break;

   case AEE_DEVICESTATE_HFK_ON:
      if ( (nSize != (int)sizeof(boolean)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sHFKOn = *((boolean *)pBuff);
      break;

   case AEE_DEVICESTATE_SDAC_ON:
      if ( (nSize != (int)sizeof(boolean)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sSDACOn = *((boolean *)pBuff);
      break;

   case AEE_DEVICESTATE_SCR_ORIENTATION:
      if ( (nSize != (int)sizeof(AEEScrOrientation)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sScreenOrientation = *((AEEScrOrientation *)pBuff);
      break;
   
   case AEE_DEVICESTATE_FLIP_OPEN:
      if ( (nSize != (int)sizeof(boolean)) || (NULL == pBuff) ) {
         return EBADPARM;
      }
      sFlipOpen = *((boolean *)pBuff);
      break;

   default:
      return EUNSUPPORTED;
   }

   // Notify that the device info has changed
   (void) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
   return SUCCESS;
}


/*=============================================================================
FUNCTION: OEM_GetDeviceInfoEx_Extra

DESCRIPTION:  Helper function to be called from OEM_GetDeviceInfoEx to 
              support the device info items implemented by this file

PARAMETERS:
    nItem      [in]:  The item to read
   *pBuff     [out]:
   *pnSize [in/out]:

RETURN VALUE: 
   int:  BREW result code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEM_GetDeviceInfoEx_Extra(AEEDeviceItem nItem, void *pBuff, int *pnSize)
{
   if (NULL == pnSize) {
      return EBADPARM;
   }

   switch (nItem){
   case AEE_DEVICESTATE_HEADPHONE_ON:
      *pnSize = (int) sizeof(boolean);
      if (pBuff != NULL) {
         *((boolean *) pBuff) = sHeadphoneOn;
      }
      return SUCCESS;

   case AEE_DEVICESTATE_STEREO_HEADPHONE_ON:
      *pnSize = (int) sizeof(boolean);
      if (pBuff != NULL) {
         *((boolean *) pBuff) = sStereoHeadphoneOn;
      }
      return SUCCESS;

   case AEE_DEVICESTATE_SPEAKER_ON:
      *pnSize = (int) sizeof(boolean);
      if (pBuff != NULL) {
         *((boolean *) pBuff) = sSpeakerOn;
      }
      return SUCCESS;      

   case AEE_DEVICESTATE_USB_HFK_ON:
      *pnSize = (int) sizeof(boolean);
      if (pBuff != NULL) {
         *((boolean *) pBuff) = sUSBHFKOn;
      }
      return SUCCESS;

   case AEE_DEVICESTATE_HFK_ON:
      *pnSize = (int) sizeof(boolean);
      if (pBuff != NULL) {
         *((boolean *) pBuff) = sHFKOn;
      }
      return SUCCESS;

   case AEE_DEVICESTATE_SDAC_ON:
      *pnSize = (int) sizeof(boolean);
      if (pBuff != NULL) {
         *((boolean *) pBuff) = sSDACOn;
      }
      return SUCCESS;

   case AEE_DEVICESTATE_SCR_ORIENTATION:
      *pnSize = (int)sizeof(AEEScrOrientation);
      if (pBuff != NULL) {
         *((AEEScrOrientation *)pBuff) = sScreenOrientation;
      }
      return SUCCESS;

   case AEE_DEVICESTATE_FLIP_OPEN:
      *pnSize = (int) sizeof(boolean);
      if (pBuff != NULL) {
         *((boolean *) pBuff) = sFlipOpen;
      }
      return SUCCESS;
   
   case AEE_DEVICEITEM_MANNER_MODE:
      {
         uint32 mannerMode = 0;
         int result = SUCCESS;


         if ((pBuff == NULL) || (*pnSize < sizeof(mannerMode))) {
            *pnSize = sizeof(mannerMode);
         }
         else {
            result = OEM_GetConfig(CFGI_MANNER_MODE, &mannerMode, sizeof(mannerMode));

            if (SUCCESS == result) {
               *((uint32*) pBuff) = mannerMode;
            }
         }

         return result;
      }
      break;

   default:
      break;

   }

   return EUNSUPPORTED;
}


#endif /* OEMDEVICEINFOEX_IMPL_H */
