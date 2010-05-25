#ifndef _OEMBTEXTAG_H_
#define _OEMBTEXTAG_H_
/*===========================================================================

FILE:      OEMBTExtAG.h

SERVICES:  BlueTooth Audio Gateway QCT Extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth audio gateway

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtAG.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtAG_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance of
   an IBTExtAG.
   There can be only one AG interface.

Parameters:
   IBTExtAG* pParent: Pointer to the parent IBTExtAG structure.

Return Value:  SUCCESS or a BREW error code.

Componts:      None

Side Effects:  Allocates APP memory for 

===========================================================================*/
int OEMBTExtAG_Init( IBTExtAG *pParent ); 

// IBase (see AEE.h)
int OEMBTExtAG_Release( IBTExtAG *pParent ); 

// IBTExtAG_Config (see AEEBTExtAG.h)
int OEMBTExtAG_Config( IBTExtAG *pParent, 
			     AEEBTIdleMode mode, 
			     uint16 wIdleTimeout );

// IBTExtAG_Enable (see AEEBTExtAG.h)
int OEMBTExtAG_Enable( IBTExtAG *pParent, 
			     const AEEBTBDAddr* pAddr, 
                       AEEBTAudioDevice audiodevice );

// IBTExtAG_Connect (see AEEBTExtAG.h)
int OEMBTExtAG_Connect( IBTExtAG *pParent, 
                        const AEEBTBDAddr *pAddr, 
                        AEEBTAudioDevice dev );

// IBTExtAG_Ring (see AEEBTExtAG.h)
int OEMBTExtAG_Ring( IBTExtAG *pParent,
                     uint16 uMaxRings,
                     uint16 uPeriod );

// IBTExtAG_SetVRCapable (see AEEBTExtAG.h)
int OEMBTExtAG_SetVRCapable( IBTExtAG *pParent,
                             boolean   vr_capable );

// IBTEXTAG_UpdateVRState (see AEEBTExtAG.h)
int OEMBTExtAG_UpdateVRState( IBTExtAG *pParent,
                              boolean bVROn );

// IBTExtAG_SetSpkrGain (see AEEBTExtAG.h)
int OEMBTExtAG_SetSpkrGain( IBTExtAG *pParent,
                            uint8 nGain );

// IBTExtAG_SetMicGain (see AEEBTExtAG.h)
int OEMBTExtAG_SetMicGain( IBTExtAG *pParent,
                           uint8 nGain );

// IBTExtAG_ConnectAudio (see AEEBTExtAG.h)
int OEMBTExtAG_ConnectAudio( IBTExtAG *pParent );

// IBTExtAG_DisconnectAudio (see AEEBTExtAG.h)
int OEMBTExtAG_DisconnectAudio( IBTExtAG *pParent );

// IBTExtAG_Disconnect (see AEEBTExtAG.h)
int OEMBTExtAG_Disconnect( IBTExtAG *pParent );

// IBTExtAG_Disable (see AEEBTExtAG.h)
int OEMBTExtAG_Disable( IBTExtAG *pParent );

// IBTEXTAG_GetDialedString (see AEEBTExtAG.h)
int OEMBTExtAG_GetDialedString( IBTExtAG *pParent, AECHAR *pString,
                                uint8 *pNumChars, boolean bIsMemLocation );

// IBTEXTAG_EnableExternalIO (see AEEBTExtAG.h)
int OEMBTExtAG_EnableExternalIO( IBTExtAG* pParent, AEECallback* pCb );

// IBTEXTAG_DisableExternalIO (see AEEBTExtAG.h)
int OEMBTExtAG_DisableExternalIO( IBTExtAG* pParent );

// IBTEXTAG_RetrieveCommand (see AEEBTExtAG.h)
int OEMBTExtAG_RetrieveCommand( IBTExtAG* pParent, 
                                uint8*    pszCommand,
                                uint8*    puNumBytes );

// IBTEXTAG_SendResponse (see AEEBTExtAG.h)
int OEMBTExtAG_SendResponse( IBTExtAG*    pParent, 
                             const uint8* pszResponse,
                             uint8        uNumBytes );

// IBTExtAG_SetInbandRingCapable (see AEEBTExtAG.h)
int OEMBTExtAG_SetInbandRingCapable( IBTExtAG* pParent, 
                                     boolean   bCapable );

// IBTExtAG_SetBatteryChargeIndicator (see AEEBTExtAG.h)
int OEMBTExtAG_SetBatteryChargeIndicator( IBTExtAG* pParent, 
                                          AEEBTBatteryChargeLevel curLevel);

// IBTExtAG_SetRSSI (see AEEBTExtAG.h)
int OEMBTExtAG_SetRSSI( IBTExtAG* pParent, AEEBTRSSI curLevel );

#endif //  _OEMBTEXTAG_H_
