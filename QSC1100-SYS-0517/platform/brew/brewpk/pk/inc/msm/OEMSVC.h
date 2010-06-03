#ifndef _OEMSVC_H_
#define _OEMSVC_H_

/*======================================================
FILE:  OEMSVC.h

SERVICES:  OEM supervisor mode functions

GENERAL DESCRIPTION:

This file contains prototypes and definitions for 
access to Supervisor mode services that OEMs need
to provide in order to support BREW..

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "disp.h"
#include "voc.h"
#include "OEMConfig.h"
#include "OEMNotify.h"
#include "AEEShell.h"

/*==================================================================
Function: OEM_SVCGetConfig

Description: Retrieves configuration information from the OEM

Prototype:
   int  OEM_SVCGetConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int     OEM_SVCGetConfig(AEEConfigItem i, void * pBuff, int nSize);

/*==================================================================
Function: OEM_SVCSetConfig

Description: Sets configuration information from the OEM

Prototype:
   int  OEM_SVCSetConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int     OEM_SVCSetConfig(AEEConfigItem i, void * pBuff, int nSize);

/*==============================================================
Function: OEM_SVCIsVocCapable

Description: Is the given type supported by the vocoder

==============================================================*/
boolean OEM_SVCIsVocCapable(voc_capability_type vocCap);

/*==================================================================
Function: OEM_SimpleBeep

Description:
   This function plays a standard OEM beep tone and vibration given
the duration, and returns TRUE if successful.

Prototype:
   boolean OEM_SimpleBeep(BeepType nBeepType, boolean bLoud);

Parameters:
   nBeepType: beep type which can be one of the following:
      - BEEP_OFF: stop playback of the current beep or vibration
      - BEEP_ALERT: alert beep tone
      - BEEP_REMINDER: reminder beep tone
      - BEEP_MSG: message beep tone
      - BEEP_ERROR: error beep tone
      - BEEP_VIBRATE_ALERT: alert vibration
      - BEEP_VIBRATE_REMIND: reminder vibration
   bLoud: boolean flag that sets the playback volume high or low

Return Value:
   TRUE if successfuly played or stopped the tone or vibration;
   FALSE otherwise/unsupported.

Comments:
   None

Side Effects:
   None

See Also:
   None
==================================================================*/
boolean OEM_SVCSimpleBeep(BeepType nBeepType, boolean bLoud);

boolean OEM_SVCUiIsInCall(void);
void    OEM_SVCUiAddEvent(OEMNotifyEvent evt);

/*==============================================================
Function: OEM_SVCdisp_get_info

Description: Gets Display info. from DMSS

==============================================================*/
int     OEM_SVCdisp_get_info(disp_info_type *pInfo);

#ifdef FEATURE_SECONDARY_DISPLAY
/*==============================================================
Function: OEM_SVCdisp_get_info2

Description: Gets Display info. from DMSS for secondary display

==============================================================*/
int     OEM_SVCdisp_get_info2(disp_info_type *pInfo);
#endif

#endif //_OEMSVC_H_
