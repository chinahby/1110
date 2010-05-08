/*======================================================
FILE:  AEE_OEMSound.h

SERVICES:  AEE OEM sound interface

GENERAL DESCRIPTION:

Sound related definitions for the AEE/OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEM_SOUND_H)
#define AEE_OEM_SOUND_H  1

#include "AEESoundPlayer.h"

#if defined(__cplusplus)
extern "C" {
#endif

//
// ISound internal callbacks. OEMSound must call these functions to send status 
//
extern void AEESound_StatusCB( const void * pClientData, AEESoundStatus eStatus );
extern void AEESound_VolumeCB( const void * pClientData, AEESoundStatus eStatus, uint16 wVolume );
extern void AEESound_LevelCB( const void * pClientData, AEESoundStatus eStatus, uint16 wLevel );

// ISoundPlayer internal callbacks. OEMSoundPlayer must call these functions
// to send status to ISoundPlayer interface
extern void AEESoundPlayer_PlayCB(AEESoundPlayerStatus eSPStatus, const void * pUser, AEESoundPlayerCmdData * pData);
extern void AEESoundPlayer_StatusCB(AEESoundPlayerStatus eSPStatus, const void * pUser);
extern void AEESoundPlayer_TimeCB(AEESoundPlayerStatus eSPStatus, const void * pUser, uint32 dwTimeMS);

#if defined(__cplusplus)
}
#endif

/*==================================================================
==================================================================*/
#endif // !defined(AEE_OEM_SOUND_H)
