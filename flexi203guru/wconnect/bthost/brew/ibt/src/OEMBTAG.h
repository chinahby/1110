#ifndef OEMBTAG_H
#define OEMBTAG_H
/*=============================================================================
FILE:         OEMBTAG.h

SERVICES:     IBTAudioGateway Interfaces

DESCRIPTION:  The OEM implementation of IBTAudioGateway

===============================================================================
        Copyright (c) 2007 - 2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTAG.h#2 $ 
$DateTime: 2009/09/02 22:49:46 $
$Author: phuongn $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2009-09-02   pn  Added support for remote volume control.

=============================================================================*/
#include "AEEShell.h"
#include "AEEIBTAudioGateway.h"

/*============================================================================
  Macros and other definitions
============================================================================*/
// Max num connections is currently 1.  
// Change this when AG driver supports multiple connections.
#define OEMBTAG_MAX_CONNECTIONS             1

//----------------------------------------------------------------------------
#define OEMBTAG_RING_ALERT_PERIOD_MS     2000 // 2 seconds between rings

//----------------------------------------------------------------------------
// AT command related definitions
#define OEMBTAG_AT_CMD_MAX_LEN            200
#define OEMBTAG_AT_CMD_MAX_PARAMS          10

//----------------------------------------------------------------------------
// Event related definitions
#define OEMBTAG_EV_NONE                     0
#define OEMBTAG_EV_ENABLED                  1
#define OEMBTAG_EV_DISABLED                 2
#define OEMBTAG_EV_CONNECT_FAILED           3
#define OEMBTAG_EV_CONNECTED                4
#define OEMBTAG_EV_DISCONNECTED             5
#define OEMBTAG_EV_AUDIO_CONNECT_FAILED     6
#define OEMBTAG_EV_AUDIO_CONNECTED          7
#define OEMBTAG_EV_AUDIO_DISCONNECTED       8
#define OEMBTAG_EV_BUTTON_PRESS             9
#define OEMBTAG_EV_SPKR_GAIN               10
#define OEMBTAG_EV_MIC_GAIN                11
#define OEMBTAG_EV_SLC_UP                  12
#define OEMBTAG_EV_SLC_DOWN                13
#define OEMBTAG_EV_DEV_PICKUP              14
#define OEMBTAG_EV_DEV_HANGUP              15
#define OEMBTAG_EV_DEV_DIAL                16
#define OEMBTAG_EV_DEV_MEM_DIAL            17
#define OEMBTAG_EV_DEV_REDIAL              18
#define OEMBTAG_EV_VR_ACT                  19
#define OEMBTAG_EV_VR_DEACT                20
#define OEMBTAG_EV_SND_TYPE                21
#define OEMBTAG_EV_A2DP_CONNECTED          22
#define OEMBTAG_EV_A2DP_DISCONNECTED       23
#define OEMBTAG_EV_A2DP_START              24
#define OEMBTAG_EV_A2DP_SUSPEND            25
#define OEMBTAG_EV_OP_CMPLT_SUCCESS        26
#define OEMBTAG_EV_OP_CMPLT_FAILED         27
#define OEMBTAG_EV_PREPPING              0xFF
typedef uint32 OEMBTAGEventType;

//----------------------------------------------------------------------------
#define OEMBTAG_SND_TYPE_NONE               0   // snd playback stopping
#define OEMBTAG_SND_TYPE_BT_SCO             1   // SCO supported sound
#define OEMBTAG_SND_TYPE_BT_OTHER           2   // other BT supported sound
#define OEMBTAG_SND_TYPE_BT                 3   // SCO and other BT sound
#define OEMBTAG_SND_TYPE_OTHER_START        4   // non-BT support sound
#define OEMBTAG_SND_TYPE_OTHER_STOP         5   // 
#define OEMBTAG_SND_TYPE_DTMF_START         6   // key press sound
#define OEMBTAG_SND_TYPE_DTMF_STOP          7   // 
#define OEMBTAG_SND_TYPE_DTMF_RING_START    8   // DTMF ring sound
#define OEMBTAG_SND_TYPE_DTMF_RING_STOP     9   // 
typedef uint32 OEMBTAGSoundType;
/*============================================================================
  Structures
============================================================================*/
typedef struct _OEMAG OEMAG;

typedef struct _OEMAGEvent OEMAGEvent;
struct _OEMAGEvent
{
   OEMAGEvent*          pNext;
   OEMBTAGEventType     type;
   BTAGOperationType    op;
   BDAddress            bdAddr;
   BTServiceClassUUID   svcClsID;
   OEMBTAGSoundType     soundType;
   uint8                u8Buf[100]; // 100 should be plenty
};

/*============================================================================
  OEM APIs exported to the AEE layer   
============================================================================*/
extern void OEMBTAG_Release (OEMAG* pMe);
extern int  OEMBTAG_New (IShell* piShell, OEMAG** ppo);
extern int  OEMBTAG_EventCbRegister (OEMAG* pMe, ISignal* piSignal);
extern void OEMBTAG_EventCbDeregister (OEMAG* pMe);
extern int  OEMBTAG_GetNextEvent (OEMAG* pMe, OEMAGEvent* pEvent);
extern int  OEMBTAG_ATCmdParserInit (OEMAG* pMe, ISignal* piSignal);
extern void OEMBTAG_ATCmdParserCleanup (OEMAG* pMe);
extern int  OEMBTAG_RetrieveATCmd (OEMAG* pMe, 
                                   char* pszATCmdBuf, 
                                   uint16* puNumChars);
extern int  OEMBTAG_AudioHandlerInit (OEMAG* pMe);
extern void OEMBTAG_AudioHandlerCleanup (OEMAG* pMe);
extern void OEMBTAG_SendRingAlert (OEMAG* pMe);
extern void OEMBTAG_StopRingAlert (OEMAG* pMe);
extern void OEMBTAG_SetInband (OEMAG* pMe, boolean bInbandOn);
extern int  OEMBTAG_SendResponse (OEMAG* pMe, const char* pszRsp);
extern void OEMBTAG_SuspendA2DP (OEMAG* pMe);
extern void OEMBTAG_ResumeA2DP (OEMAG* pMe);
extern int  OEMBTAG_Enable (OEMAG* pMe);
extern int  OEMBTAG_Disable (OEMAG* pMe);
extern int  OEMBTAG_Connect (OEMAG* pMe, const BDAddress* pAddr);
extern int  OEMBTAG_Disconnect (OEMAG* pMe, const BDAddress* pAddr);
extern int  OEMBTAG_ConnectAudio (OEMAG* pMe, const BDAddress* pAddr);
extern int  OEMBTAG_DisconnectAudio (OEMAG* pMe, const BDAddress* pAddr);
extern int  OEMBTAG_ReportVRState (OEMAG* pMe, boolean bVRActive);
extern void OEMBTAG_ApplySpkrGain (OEMAG* pMe, uint8 gain);
extern int  OEMBTAG_SetMicGain (OEMAG* pMe, uint8 gain);
extern boolean OEMBTAG_CheckAutoAnswer (OEMAG* pMe);

#endif /* OEMBTAG_H */
