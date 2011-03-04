#ifndef _AEE_BT_EXT_AG_H_
#define _AEE_BT_EXT_AG_H_

/*===========================================================================
FILE:      AEEBTExtAG.h

SERVICES:  BlueTooth Audio Gateway

GENERAL DESCRIPTION: BREW interface to the BT audio gateway

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtAG

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


// BREW generated ID
#define AEECLSID_BLUETOOTH_AG    	0x01013424


// event types returned via HandleEvent()
enum
{
  AEEBT_AG_EVT_CONFIGURED,                // response to Config()
  AEEBT_AG_EVT_ENABLED,                   // response to Enable()
  AEEBT_AG_EVT_CONNECTED,                 // response to Connect()
  AEEBT_AG_EVT_CONNECT_FAILED,            // response to Connect()
  AEEBT_AG_EVT_DISCONNECTED,              // response to Disconnect()
  AEEBT_AG_EVT_RUNG,                      // response to Ring()
  AEEBT_AG_EVT_VR_CAPABLE_SET,            // response to SetVRCapable()
  AEEBT_AG_EVT_SPKR_GAIN_SET,             // response to SetSpkrGain()
  AEEBT_AG_EVT_MIC_GAIN_SET,              // response to SetMicGain()
  AEEBT_AG_EVT_AUDIO_CONNECTED,           // may be due to ConnectAudio()
  AEEBT_AG_EVT_AUDIO_CONNECT_FAILED,      // may be due to ConnectAudio()
  AEEBT_AG_EVT_AUDIO_DISCONNECTED,        // may be due to DisconnectAudio()
  AEEBT_AG_EVT_DISABLED,                  // may be due to Disable()

  // the following events are not associated with any commands
  AEEBT_AG_EVT_BUTTON_PRESSED,            // audio device's button pressed
  AEEBT_AG_EVT_SPKR_GAIN_REPORT,          // audio device's spkr gain report
  AEEBT_AG_EVT_MIC_GAIN_REPORT,           // audio device's mic gain report

  // the following events are applicable to HF only
  AEEBT_AG_EVT_SLC_UP,                    // Service Level Connection up
  AEEBT_AG_EVT_SLC_DOWN,                  // Service Level Connection down
  AEEBT_AG_EVT_DEV_PICKUP,                // HF answered incoming call
  AEEBT_AG_EVT_DEV_HANGUP,                // HF hung up call
  AEEBT_AG_EVT_VR_ON,                     // HF turned VR on
  AEEBT_AG_EVT_VR_OFF,                    // HF turned VR off
  AEEBT_AG_EVT_DEV_DIAL,                  // HF dialed a number
  AEEBT_AG_EVT_DEV_MEM_DIAL,              // HF dialed a number in memory
  AEEBT_AG_EVT_DEV_REDIAL,                // HF redialed last number
  AEEBT_AG_EVT_DEV_SEND_DTMF,             // HF sent DTMF tone

  AEEBT_AG_EVT_VR_STATE_UPDATED,          // may be due to UpdateVRState()

  // the following events are External I/O specific
  AEEBT_AG_EVT_EXTERNAL_IO_ENABLED,       // External IO successfully enabled
  AEEBT_AG_EVT_EXTERNAL_IO_DISABLED,      // External IO successfully enabled

  AEEBT_AG_EVT_INBAND_RING_CAPABLE_SET,   // response to SetInbandRingCapable()
  AEEBT_AG_EVT_BATTERY_CHARGE_IND_SET,    // response to SetBatteryChargeIndicator()
  AEEBT_AG_EVT_RSSI_SET,                  // response to SetRSSI()
  AEEBT_AG_EVT_SOUND_CHANGING             // AVS about to do something with audio  
};

// error codes for AEEBT_AG_EVT_*
enum
{
  AEEBT_AG_ERR_NONE,
  AEEBT_AG_ERR_BAD_PARAMS,
  AEEBT_AG_ERR_CONFIGURE_FAILED,
  AEEBT_AG_ERR_ALREADY_ENABLED,
  AEEBT_AG_ERR_NOT_ENABLED,
  AEEBT_AG_ERR_NO_SIO_STREAM,
  AEEBT_AG_ERR_ENABLE_FAILED,
  AEEBT_AG_ERR_CONNECT_FAILED,
  AEEBT_AG_ERR_DISCONNECT_FAILED,
  AEEBT_AG_ERR_RING_FAILED,
  AEEBT_AG_ERR_SET_SPKR_GAIN_FAILED,
  AEEBT_AG_ERR_SET_MIC_GAIN_FAILED,
  AEEBT_AG_ERR_CONNECT_AUDIO_FAILED,
  AEEBT_AG_ERR_DISCONNECT_AUDIO_FAILED,
  AEEBT_AG_ERR_NO_AUDIO,
  AEEBT_AG_ERR_ALREADY_DISABLED,
  AEEBT_AG_ERR_DISABLE_FAILED,
  AEEBT_AG_ERR_FEATURE_NOT_SUPPORTED,
  AEEBT_AG_ERR_FAILED
};

// idle modes for Config()
enum
{
  AEEBT_AG_IDLEMODE_PASSIVE,  // listen for connection requests
  AEEBT_AG_IDLEMODE_ACTIVE    // initiate connection to remote device
};
typedef uint8     AEEBTIdleMode;


// audio device types for Enable()
enum
{
  AEEBT_AG_AUDIO_DEVICE_HEADSET,
  AEEBT_AG_AUDIO_DEVICE_HANDSFREE
};
typedef uint8     AEEBTAudioDevice;

// RSSI levels
#define AEEBT_AG_RSSI_LEVEL_0  0
#define AEEBT_AG_RSSI_LEVEL_1  1
#define AEEBT_AG_RSSI_LEVEL_2  2
#define AEEBT_AG_RSSI_LEVEL_3  3
#define AEEBT_AG_RSSI_LEVEL_4  4
#define AEEBT_AG_RSSI_LEVEL_5  5
typedef uint8     AEEBTRSSI;

// Battery Charge levels
#define AEEBT_AG_BATTCHG_LEVEL_0  0
#define AEEBT_AG_BATTCHG_LEVEL_1  1
#define AEEBT_AG_BATTCHG_LEVEL_2  2
#define AEEBT_AG_BATTCHG_LEVEL_3  3
#define AEEBT_AG_BATTCHG_LEVEL_4  4
#define AEEBT_AG_BATTCHG_LEVEL_5  5
typedef uint8     AEEBTBatteryChargeLevel;

typedef struct _IBTExtAG IBTExtAG;

AEEINTERFACE(IBTExtAG)
{
   INHERIT_IQueryInterface(IBTExtAG);
   int   (*Config)(IBTExtAG* po, AEEBTIdleMode, uint16 wIdleTimeout);
   int   (*Enable)(IBTExtAG* po, const AEEBTBDAddr* pAddr, 
                   AEEBTAudioDevice dev);
   int   (*Connect)(IBTExtAG* po, const AEEBTBDAddr* pAddr, 
                    AEEBTAudioDevice dev);
   int   (*Ring)(IBTExtAG* po, uint16 uMaxRings, uint16 uPeriod);
   int   (*SetVRCapable)(IBTExtAG* po, boolean vr_capable);
   int   (*UpdateVRState)(IBTExtAG* po, boolean bVROn);
   int   (*SetSpkrGain)(IBTExtAG* po, uint8 nGain);
   int   (*SetMicGain)(IBTExtAG* po, uint8 nGain);
   int   (*ConnectAudio)(IBTExtAG* po);
   int   (*DisconnectAudio)(IBTExtAG* po);
   int   (*Disconnect)(IBTExtAG* po);
   int   (*Disable)(IBTExtAG* po);

   int   (*GetDialedString)(IBTExtAG* po, AECHAR* pString, 
                            uint8* pNumChars, boolean bIsMemLocation);
   int   (*EnableExternalIO)(IBTExtAG* po, AEECallback* pCb);
   int   (*DisableExternalIO)(IBTExtAG* po);
   int   (*RetrieveCommand)(IBTExtAG* po, uint8* pszCommand, 
                            uint8* uNumBytes);
   int   (*SendResponse)(IBTExtAG* po, const uint8* pszResponse,
                         uint8 uNumBytes);
   int   (*SetInbandRingCapable)(IBTExtAG* po, boolean bInbandCapable);
   int   (*SetBatteryChargeIndicator)(IBTExtAG* po, 
                                      AEEBTBatteryChargeLevel currentLevel);
   int   (*SetRSSI)(IBTExtAG* po, AEEBTRSSI currentRSSI);
};


#define IBTEXTAG_AddRef(p)                  \
        AEEGETPVTBL((p),IBTExtAG)->AddRef((p))
        
#define IBTEXTAG_Release(p)                 \
        AEEGETPVTBL((p),IBTExtAG)->Release((p))
        
#define IBTEXTAG_QueryInterface(p,i,ppo)    \
        AEEGETPVTBL((p),IBTExtAG)->QueryInterface((p),(i),(ppo))
        
#define IBTEXTAG_Config(p,m,t)              \
        AEEGETPVTBL((p),IBTExtAG)->Config((p),(m),(t))
        
#define IBTEXTAG_Enable(p,a,d)              \
        AEEGETPVTBL((p),IBTExtAG)->Enable((p),(a),(d))
        
#define IBTEXTAG_Connect(p,a,d)             \
        AEEGETPVTBL((p),IBTExtAG)->Connect((p),(a),(d))
        
#define IBTEXTAG_Ring(p,rc,rp)              \
        AEEGETPVTBL((p),IBTExtAG)->Ring((p),(rc),(rp))
        
#define IBTEXTAG_SetVRCapable(p,b)          \
        AEEGETPVTBL((p),IBTExtAG)->SetVRCapable((p),(b))
        
#define IBTEXTAG_UpdateVRState(p,b)         \
        AEEGETPVTBL((p),IBTExtAG)->UpdateVRState((p),(b))

#define IBTEXTAG_SetSpkrGain(p,g)           \
        AEEGETPVTBL((p),IBTExtAG)->SetSpkrGain((p),(g))
        
#define IBTEXTAG_SetMicGain(p,g)            \
        AEEGETPVTBL((p),IBTExtAG)->SetMicGain((p),(g))
        
#define IBTEXTAG_ConnectAudio(p)            \
        AEEGETPVTBL((p),IBTExtAG)->ConnectAudio((p))
        
#define IBTEXTAG_DisconnectAudio(p)         \
        AEEGETPVTBL((p),IBTExtAG)->DisconnectAudio((p))
        
#define IBTEXTAG_Disconnect(p)              \
        AEEGETPVTBL((p),IBTExtAG)->Disconnect((p))
        
#define IBTEXTAG_Disable(p)                 \
        AEEGETPVTBL((p),IBTExtAG)->Disable((p))
        
#define IBTEXTAG_GetDialedString(p,ps,pn,b) \
        AEEGETPVTBL((p),IBTExtAG)->GetDialedString((p),(ps),(pn),(b))
        
#define IBTEXTAG_EnableExternalIO(p,pcb)    \
        AEEGETPVTBL((p),IBTExtAG)->EnableExternalIO((p),(pcb))
        
#define IBTEXTAG_DisableExternalIO(p)       \
        AEEGETPVTBL((p),IBTExtAG)->DisableExternalIO((p))
        
#define IBTEXTAG_RetrieveCommand(p,pb,pc)   \
        AEEGETPVTBL((p),IBTExtAG)->RetrieveCommand((p),(pb),(pc))
        
#define IBTEXTAG_SendResponse(p,pb,c)       \
        AEEGETPVTBL((p),IBTExtAG)->SendResponse((p),(pb),(c))
        
#define IBTEXTAG_SetInbandRingCapable(p,b)  \
        AEEGETPVTBL((p),IBTExtAG)->SetInbandRingCapable((p),(b))
        
#define IBTEXTAG_SetBatteryChargeIndicator(p,l)  \
        AEEGETPVTBL((p),IBTExtAG)->SetBatteryChargeIndicator((p),(l))
        
#define IBTEXTAG_SetRSSI(p,l)  \
        AEEGETPVTBL((p),IBTExtAG)->SetRSSI((p),(l))
        
/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================
AEEBTIdleMode

Description:

  AEEBTIdleMode specifies the operation of AG when it's idle (that is
  when there's no audio activity.)  This is used in configuration of AG.

Definition:

  enum
  {
    AEEBT_AG_IDLEMODE_PASSIVE,  // listen for connection requests
    AEEBT_AG_IDLEMODE_ACTIVE    // initiate connection to remote device
  };

=======================================================================
AEEBTAudioDevice

Description:

  AEEBTAudioDevice specifies whether AG should conform to the Bluetooth
  Headset profile or Hands-free profile when in connection with the device
  to which this is associated.

Definition:

  enum
  {
    AEEBT_AG_AUDIO_DEVICE_HEADSET,
    AEEBT_AG_AUDIO_DEVICE_HANDSFREE
  };

=======================================================================


=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtAG

Description: BlueTooth Audio Gateway

  The use of IBTExtAG consists of sending commands and receiving events.
  These events are delivered to IBTExtAG clients as notifications, hence
  the clients must register for AG notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_AG.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated by the BT audio device, 
  typically in response to a user action, such as pressing the send button 
  or adjusting the speaker volume.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_AG_EVT_* events when the notification mask
  is NMASK_BT_AG.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                           pData->
  -----                           ----------------------------
  AEEBT_AG_EVT_CONNECTED          bdAddr - BD address of remote device
  AEEBT_AG_EVT_DISCONNECTED       bdAddr - BD address of remote device
  AEEBT_AG_EVT_CONNECT_FAILED     uError - reason for failing
  AEEBT_AG_EVT_SPKR_GAIN_REPORT   uVolumeGain - speaker gain in remote device
  AEEBT_AG_EVT_MIC_GAIN_REPORT    uVolumeGain - mic gain in remote device
  AEEBT_AG_EVT_DEV_DIAL           * Note 1
  AEEBT_AG_EVT_DEV_MEM_DIAL       * Note 1
  AEEBT_AG_EVT_DEV_SEND_DTMF      uDTMFChar - DTMF character (UTF8)

Notes:

  1. Client must call IBTExtAG_GetDialedString() when receiving either
     AEEBT_AG_EVT_DEV_DIAL or AEEBT_AG_EVT_DEV_MEM_DIAL.
  
Interfaces:

  IBTEXTAG provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTAG_AddRef()
    IBTEXTAG_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTAG_QueryInterface()

=======================================================================

Function:  IBTEXTAG_Config()

Description: Configures the BT audio gateway.  This command may be issued
             before or after enabling the AG.

Prototype:

   int IBTEXTAG_Config( IBTExtAG* po, AEEBTIdleMode mode, uint16 wIdleTimeout );

Parameters:

   po           - pointer to the IBTExtAG object
   mode         - Describes the behaviour when AG is in the Idle mode.
                  The Idle mode is entered when no audio connection exists
                  and when no data is sent to or from the remote audio
                  device for wIdleTimeout milliseconds.
   wIdleTimeout - the amount of time in seconds of data inactivity
                  after which AG will enter the Idle mode

Return value:

   SUCCESS indicates that the configure request was queued and that a
   AEEBT_AG_EVT_CONFIGURED event will be sent to the registered event
   handler at a later time.  All other values indicate failure, and
   will not generate an event.  In this context, the following error
   codes have special meaning:

   EBADPARM  - unrecognized nIdleMode
   ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_CONFIGURED can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE             - no error

=======================================================================

Function:  IBTEXTAG_Enable()

Description: Enables the BT audio gateway to operate according to the
   Headset (HS) or Hands-Free (HF) Profile.  AG will only communicate
   with one audio device as specified by pAddr.  That audio device must
   support the enabled profile (HS or HF).  AG will register the HS or
   HF service with SDP.  

Prototype:

   int IBTEXTAG_Enable( IBTExtAG* po, const AEEBTAddr* pAddr, 
                        AEEBTAudioDevice dev );

Parameters:

   po           - pointer to the IBTExtAG object
   pAddr        - pointer to the BT device address of the BT Audio Device;
                  if NULL, AG will go into passive mode waiting for connection
                  requests from any devices
   dev          - Describes whether the remote audio device is a Headset
                  or Hands-Free device.  This will determine the mode AG
                  with operate in: Headset Profile or Hands-Free Profile.
                  if pAddr is NULL, this is ignored

Return value:

   SUCCESS indicates that the enable request was queued and that a
   AEEBT_AG_EVT_ENABLED event will be generated at a later time.  
   All other values indicate failure, and will not generate an event.
   In this context, the following error codes have special meaning:

   EBADPARM  - unrecognized nAudioDevice
   ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_ENABLED can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE             - no error
  AEEBT_AG_ERR_ALREADY_ENABLED  - AG was already enabled
  AEEBT_AG_ERR_NO_SIO_STREAM    - not possible to make or receive BT connection

=======================================================================

Function:  IBTEXTAG_Connect()

Description: Initiates the connection to the remove device specified by
             pAddr.

Prototype:

   int IBTEXTAG_Connect( IBTExtAG* po, const AEEBTAddr* pAddr, 
                         AEEBTAudioDevice dev );

Parameters:

   po           - pointer to the IBTExtAG object
   pAddr        - pointer to the BT device address of the remote device
   dev          - Describes whether the remote audio device is a Headset
                  or Hands-Free device.  This will determine the mode AG
                  with operate in: Headset Profile or Hands-Free Profile.

Return value:

   SUCCESS indicates that the request to connecto to remote device was queued 
   and that a AEEBT_AG_EVT_CONNECTED or AEEBT_AG_EVT_CONNEC_FAILTED event 
   will be generated at a later time.  All other values indicate failure, 
   and will not generate an event.  In this context, the following error 
   codes have special meaning:

   EBADPARM  - pAddr is NULL or invalid dev
   ENOMEMORY - out of memory

Events:

  AEEBT_AG_EVT_CONNECTED will be accompanied by the BD address of the 
  remote device.
  
  AEEBT_AG_EVT_CONNECT_FAILED will be accompanied by one of the following 
  error codes:
  
  AEEBT_AG_ERR_NOT_ENABLED      - AG must be enabled first
  AEEBT_AG_ERR_NO_SIO_STREAM    - not possible to make or receive BT connection
  AEEBT_AG_ERR_CONNECT_FAILED   - failed to connect, reason not specified

=======================================================================

Function:  IBTEXTAG_Ring()

Description: Sends ring signals to the remote audio device.
             This command does not send an in-band audio ring.  

Prototype:

  int IBTEXTAG_Ring( IBTExtAG* po, uint16 uMaxRings, uint16 uPeriod );

Parameters:

  po          - pointer to the IBTExtAG object
  uMaxRings   - number of RINGs to send to remote device
  uPeriod     - time between RINGs, in multiple of 0.25 seconds

Return value:

  SUCCESS indicates that the ring request was queued and that a
  AEEBT_AG_EVT_RUNG event will be generated at a later time.  All 
  other values indicate failure, and will not generate an event.  
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Events:

  AEEBT_AG_EVT_RUNG can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE             - no error
  AEEBT_AG_ERR_NOT_ENABLED      - AG must be enabled first
  AEEBT_AG_ERR_NO_SIO_STREAM    - not possible to make BT connection
  AEEBT_AG_ERR_RING_FAILED      - failed to ring device, reason not specified

=======================================================================

Function:  IBTEXTAG_SetVRCapable()

Description: Enables/disable VR capability for AG.  This can be done before
             or after AG is enabled.

Prototype:

  int IBTEXTAG_SetVRCapable( IBTExtAG* po, boolean vr_capable );

Parameters:

  po         - pointer to the IBTExtAG object
  vr_capable - TRUE to enable VR capability, FALL to disable it

Return value:

  SUCCESS indicates that the request to change VR capability was queued and 
  that a AEEBT_AG_EVT_VR_CAPABLE_SET event will be generated at a later 
  time.  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_VR_CAPABLE_SET can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE                  - no error
  AEEBT_AG_ERR_FEATURE_NOT_SUPPORTED - VR feature not supported

=======================================================================

Function:  IBTEXTAG_UpdateVRState()

Description: Reports VR state to AG.

Prototype:

  int IBTEXTAG_UpdateVRState( IBTExtAG* po, boolean bVROn );

Parameters:

  po      - pointer to the IBTExtAG object
  bVROn   - TRUE if phone is in VR, FALSE otherwise

Return value:

  SUCCESS indicates that the request to report VR state to AG was queued and 
  that a AEEBT_AG_EVT_VR_STATE_UPDATED event will be generated at a later 
  time.  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_VR_STATE_UPDATED can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE                  - no error

=======================================================================

Function:  IBTEXTAG_SetSpkrGain()

Description: Sends a speaker gain level to the BT audio device.  If AG
             is not currently connected to the BT audio device, this gain 
             will be sent to the BT audio device the next time a connection 
             is made.

Prototype:

  int IBTEXTAG_SetSpkrGain( IBTExtAG* po, uint8 nGain );

Parameters:

  po    - pointer to the IBTExtAG object
  nGain - value to be sent in the speaker gain command to the BT audio
          device.  The permissable values are 0 through 15.

Return value:

  SUCCESS indicates that the speaker gain request was queued and that a
  AEEBT_AG_EVT_SPKR_GAIN_SET event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:
  AEEBT_AG_EVT_SPKR_GAIN_SET can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE             - no error
  AEEBT_AG_ERR_NOT_ENABLED      - AG must be enabled first
  AEEBT_AG_ERR_BAD_PARAMS       - gain value out of range

=======================================================================

Function:  IBTEXTAG_SetMicGain()

Description: Sends a microphone gain level to the BT audio device.  If AG
   is not currently connected to the BT audio device, this gain will
   be sent to the BT audio device the next time a connection is made.

Prototype:

  int IBTEXTAG_SetMicGain( IBTExtAG* po, uint8 nGain);

Parameters:

  po    - pointer to the IBTExtAG object
  nGain - value to be sent in the microphone gain command to the BT audio
          device.  The permissable values are 0 through 15.

Return value:

  SUCCESS indicates that the microphone gain request was queued and that
  an AEEBT_AG_EVT_MIC_GAIN_SET event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_MIC_GAIN_SET can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE             - no error
  AEEBT_AG_ERR_NOT_ENABLED      - AG must be enabled first
  AEEBT_AG_ERR_BAD_PARAMS       - gain value out of range

=======================================================================

Function:  IBTEXTAG_ConnectAudio()

Description: Sets up an audio link to the BT audio device.

Prototype:

  int IBTEXTAG_ConnectAudio( IBTExtAG* po );

Parameters:

  po       - pointer to the IBTExtAG object

Return value:

  SUCCESS indicates that the audio connection request was queued and that
  an AEEBT_AG_EVT_AUDIO_CONNECTED event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_AUDIO_CONNECTED can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE                 - no error
  AEEBT_AG_ERR_NOT_ENABLED          - AG must be enabled first
  AEEBT_AG_ERR_NO_SIO_STREAM        - not possible to make BT connection
  AEEBT_AG_ERR_CONNECT_AUDIO_FAILED - no reason specified

=======================================================================

Function:  IBTEXTAG_DisconnectAudio()

Description: Tears down an audio link with the BT audio device.

Prototype:

  int IBTEXTAG_DisconnectAudio( IBTExtAG* po );

Parameters:

  po - pointer to the IBTExtAG object

Return value:

  SUCCESS indicates that the audio disconnect request was queued and that
  an AEEBT_AG_EVT_AUDIO_DISCONNECTED event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:
  AEEBT_AG_EVT_AUDIO_DISCONNECTED can be accompanied by any of the following
  reason codes:
  
  AEEBT_REASON_LOCAL_HOST_TERMINATED_CONNECTION
  AEEBT_REASON_USER_ENDED_CONNECTION

=======================================================================

Function:  IBTEXTAG_Disconnect()

Description: Tears down the link between the local device and the remote device.

Prototype:

  int IBTEXTAG_Disconnect( IBTExtAG* po );

Parameters:

  po - pointer to the IBTExtAG object

Return value:

  SUCCESS indicates that the disconnect request was queued and that a
  AEEBT_AG_EVT_DISCONNECTED event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_DISCONNECTED can be accompanied by either a pointer to the 
  BD address of the remote device, or any of the following error codes:
  
  AEEBT_AG_ERR_DISCONNECT_FAILED  - no reason specified

=======================================================================

Function:  IBTEXTAG_Disable()

Description: Disables the AG capability within the BT driver.  After this
             command is successfully processed, the BT driver tears down 
             all links with the BT audio device.

Prototype:

  int IBTEXTAG_Disable( IBTExtAG* po );

Parameters:

  po - pointer to the IBTExtAG object

Return value:

  SUCCESS indicates that the disable request was queued and that a
  AEEBT_AG_EVT_DISABLED event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:
  AEEBT_AG_EVT_DISABLED can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE          - no error
  AEEBT_AG_ERR_NOT_ENABLED   - AG was not enabled

=======================================================================

Function:  IBTEXTAG_GetDialedString()

Description: Retrieves the dialed string sent by car-kit.

Prototype:

  int IBTEXTAG_GetDialedString( IBTExtAG* po, AECHAR* pString, 
                                uint8* pNumChars, boolean bIsMemLocation );

Parameters:

  po             - pointer to the IBTExtAG object
  pString        - pointer to destination of dialed string
  pNumChars      - [in]  max number of bytes pString can hold
                   [out] number of bytes stored in pString
  bIsMemLocation - [in] TRUE if pString holds phone book entry, FALSE if it
                   holds digits to be dialed

Return value:

  SUCCESS indicates that the pString holds the dialed string and pNumChars holds
  number of bytes in pString.
  All other values indicate failure.
  No events will be generated at a later time.
  In this context, the following error codes have special meaning:
  
  EBADPARM - invalid pointers passed in

Event:
  None

=======================================================================

Function:  IBTEXTAG_EnableExternalIO()

Description: Requests BT driver to pass unrecogized AT commands to app.  
             Once this is done, BT driver will invoke the callback to forward 
             AT commands it does not process.  App can use 
             IBTEXTAG_RetrieveCommand() to retrieve the forwarded AT command 
             and IBTEXTAG_SendResponse() to send result/response back to 
             remote audio device.

Prototype:

  int IBTEXTAG_EnableExternalIO( IBTExtAG* po, AEECallback* pCb );

Parameters:

  po      - pointer to the IBTExtAG object
  pCb     - [in] pointer to callback to be invoked when AT commands are 
            being forwarded to app from BT driver

Return value:

  SUCCESS indicates that the request to configure External I/O has been queued
  and AEEBT_AG_EVT_EXTERNAL_IO_ENABLED will be generated at a later time.
  All other values indicate failure and no events will be generated.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory
  EBADPARM  - invalid pointers passed in

Event:
  AEEBT_AG_EVT_EXTERNAL_IO_ENABLED with error codes:
    AEEBT_AG_ERR_NONE          - no error

=======================================================================

Function:  IBTEXTAG_DisableExternalIO()

Description: Requests BT driver to stop passing unrecogized AT commands to app.  

Prototype:

  int IBTEXTAG_DisableExternalIO( IBTExtAG* po );

Parameters:

  po      - pointer to the IBTExtAG object

Return value:

  SUCCESS indicates that the request to disable External I/O has been queued
  and AEEBT_AG_EVT_EXTERNAL_IO_DISABLED will be generated at a later time.
  All other values indicate failure and no events will be generated.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:
  AEEBT_AG_EVT_EXTERNAL_IO_DISABLED with error codes:
    AEEBT_AG_ERR_NONE          - no error

=======================================================================

Function:  IBTEXTAG_RetrieveCommand()

Description: Retrieves command from remote audio device forwarded by BT driver.
             This should be called after callback function passed into 
             IBTEXTAG_EnableExternalIO() is invoked.
             This should only be used while External I/O is enabled.

Prototype:

  int IBTEXTAG_RetrieveCommand( IBTExtAG* po, uint8* pszCommand,
                                uint8* puNumBytes );

Parameters:

  po          - pointer to the IBTExtAG object
  pszCommand  - [out] AT command received from remote device
  puNumBytes  - [in] number of bytes pszCommand can hold
                [out] number of bytes copied to pszCommand

Return value:

  SUCCESS indicates that AT command has been copied to pszCommand and 
  puNumBytes contains the length of the AT command.  If pszCommand buffer
  is full, there might be more data left to be copied.  This function 
  should be called again until puNumBytes contains a count less than the 
  size of pszCommand.
  
  All other values indicate failure.  No events will be generated.
  In this context, the following error codes have special meaning:
  
  EBADPARM  - invalid pointers passed in
  EBADSTATE - External I/O is not enabled

Event:
  None
  
=======================================================================

Function:  IBTEXTAG_SendResponse()

Description: Sends response to remote audio device.  
             This should only be used while External I/O is enabled.

Prototype:

  int IBTEXTAG_SendResponse( IBTExtAG* po, const uint8* pszResponse,
                             uint8 uNumBytes );

Parameters:

  po          - pointer to the IBTExtAG object
  pszResponse - [in] response to be sent to car-kit
  uNumBytes   - [in] number of bytes to send

Return value:

  SUCCESS indicates that BT driver has copied the response to remote audio 
  device.  All other values indicate failure.  No events will be generated.
  In this context, the following error codes have special meaning:

  EBADPARM  - invalid pointers passed in
  EBADSTATE - External I/O is not enabled

Event:
  None

=======================================================================

Function:  IBTEXTAG_SetInbandRingCapable()

Description: Enables/disable Inband Ring capability for AG.  This can be done 
             before or after AG is enabled.

Prototype:

  int IBTEXTAG_SetInbandRingCapable( IBTExtAG* po, boolean bCapable );

Parameters:

  po         - pointer to the IBTExtAG object
  bCapable   - TRUE to enable Inband Ring capability, FALL to disable it

Return value:

  SUCCESS indicates that the request to change Inband Ring capability was queued
  and that a AEEBT_AG_EVT_INBAND_RING_CAPABLE_SET event will be generated at a
  later time.  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_INBAND_RING_CAPABLE_SET can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE                  - no error

=======================================================================

Function:  IBTEXTAG_SetBatteryChargeIndicator()

Description: Sets new level of Battery Charge.  This can be done before or 
             after AG is enabled.

Prototype:

  int IBTEXTAG_SetBatteryChargeIndicator( 
        IBTExtAG* po, AEEBTBatteryChargeLevel currentLevel );

Parameters:

  po           - pointer to the IBTExtAG object
  currentLevel - the current Battery Charge level

Return value:

  SUCCESS indicates that the request to set Battery Charge Indicator was queued
  and that a AEEBT_AG_EVT_BATTERY_CHARGE_IND_SET event will be generated at a
  later time.  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_BATTERY_CHARGE_IND_SET can be accompanied by any of the following
  error codes:
  
  AEEBT_AG_ERR_NONE                  - no error

=======================================================================

Function:  IBTEXTAG_SetRSSI()

Description: Sets new RSSI level.  This can be done before or after AG is 
             enabled.

Prototype:

  int IBTEXTAG_SetRSSI( IBTExtAG* po, AEEBTRSSI currentLevel );

Parameters:

  po           - pointer to the IBTExtAG object
  currentLevel - the current RSSI level

Return value:

  SUCCESS indicates that the request to set Battery Charge Indicator was queued
  and that a AEEBT_AG_EVT_RSSI_SET event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.
  In this context, the following error codes have special meaning:
  
  ENOMEMORY - out of memory

Event:

  AEEBT_AG_EVT_RSSI_SET can be accompanied by any of the following error codes:
  
  AEEBT_AG_ERR_NONE                  - no error

=======================================================================*/
#endif /* _AEE_BT_EXT_AG_H_ */
