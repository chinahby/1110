#ifndef _AEE_BT_EXT_A2DP_H_
#define _AEE_BT_EXT_A2DP_H_

/*===========================================================================
FILE:      AEEBTExtA2DP.h

SERVICES:  BlueTooth A2DP interface for BREW

GENERAL DESCRIPTION: BREW interface to the A2DP layer

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTEXTA2DP

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

// BREW generated ID
#define AEECLSID_BLUETOOTH_A2DP    	0x0102e967

// event types
#define AEEBT_A2DP_EVT_CONNECTED    0
#define AEEBT_A2DP_EVT_CON_FAILED   1
#define AEEBT_A2DP_EVT_DISCONNECTED 2
#define AEEBT_A2DP_EVT_BITRATE      3
#define AEEBT_A2DP_EVT_START        4
#define AEEBT_A2DP_EVT_SUSPEND      5
#define AEEBT_A2DP_EVT_OPEN         6
#define AEEBT_A2DP_EVT_CLOSE        7

/*===========================================================================
 * ErrorCodes for A2DP App
 ===========================================================================*/
/* This error code is used when, there is 
 * no error */
#define AEEBT_A2DP_ERR_NONE                                0
/* This error code is used when no endpoints
 * are found */
#define AEEBT_A2DP_ERR_INCOMPATIABLE_DEVICES               1
/* This error code is used when endpoint
 * Discovery fails */
#define AEEBT_A2DP_ERR_ENDPOINT_DISCOVERY                  2
/* This error code is used when we fail to
 * start stream */
#define AEEBT_A2DP_ERR_FAILED_TO_START_STREAM              3
/* This error code is used when, we try to close
 * and open the stream, but the remote device
 * disconnects */
#define AEEBT_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM  4
/* This error code is used when Abort
 * fails */
#define AEEBT_A2DP_ERR_ABORT_FAILED                        5
/* This error code is used when, lower layer
 * fails to process a BT command */
#define AEEBT_A2DP_ERR_FAILED_TO_PROCESS_CMD               6
/* This error code is used when, Set config
 * fails */
#define AEEBT_A2DP_ERR_SET_CONFIG_FAILED                   7
/* This error code is used when, configuration
 * parameters doesn't match */
#define AEEBT_A2DP_ERR_CONFIG_PARAMS_NOT_AGREEABLE         8
/* This error code is used when, connection is
 *  rejected */
#define AEEBT_A2DP_ERR_CONNECT_REFUSED_NO_RESOURCES        9
/* This error code is used when, page 
 * fails */
#define AEEBT_A2DP_ERR_PAGE_FAILED                         10
/* This error code is used when, Authentication
 * fails */
#define AEEBT_A2DP_ERR_AUTHENTICATION_FAILED               11
/* This is not an error code, is used when
 * normal disconnect happens */
#define AEEBT_A2DP_ERR_NORMAL_DISCONNECT                   12
/* This error code is used when, there is a 
 * link lost at the lower layers */
#define AEEBT_A2DP_ERR_LINK_LOST                           13
/* This error code is used when, a wrong 
 * is issued to the Lower layer */
#define AEEBT_A2DP_ERR_WRONG_COMMAND                       14
/* This error code is used when, RM rejects a connection 
 * because of lack of Resources */
#define AEEBT_A2DP_ERR_RESOURCES_NOT_AVALIABLE             15
/* This error code is used when,
 * the Radio is Off */
#define AEEBT_A2DP_ERR_RESET_IN_PROGRESS                   16



/*===========================================================================
 * Bitrate values
 ===========================================================================*/
#define AEEBT_A2DP_BITRATE_LOWEST   0
#define AEEBT_A2DP_BITRATE_LOW      1
#define AEEBT_A2DP_BITRATE_MEDIUM   2
#define AEEBT_A2DP_BITRATE_HIGH     3
#define AEEBT_A2DP_BITRATE_HIGHEST  4
#define AEEBT_A2DP_BITRATE_CUSTOM   5

/*===========================================================================
 * Event type used in START and SUSPEND events.
 * LOCAL_INIT is used to indicate that the event is due to a command.
 * REMOTE_INIT is used for all other reasons, ie, due to signalling from
 * remote device, or a baseband timeout.
 ===========================================================================*/
#define AEEBT_A2DP_EVENT_LOCAL_INIT  0
#define AEEBT_A2DP_EVENT_REMOTE_INIT 1

/*===========================================================================
 * Control values for IBTEXTA2DP_Ctl
 ===========================================================================*/
/* Send AVRCP_SUSPEND when streaming ends
 * Default: enabled */
#define AEEBT_A2DP_CTL_SUSPEND_ON_END  0

/* Send AVRCP_CLOSE when streaming ends
 * Default: disabled */
#define AEEBT_A2DP_CTL_CLOSE_ON_END    1

/* Sets the delay between receiving a start
 * cfm and streaming data.
 * data1: timeout in ms.
 * Default: 500ms (must be bigger than 4) */
#define AEEBT_A2DP_CTL_START_DELAY     2

/* Honor the in_use flag of the remote 
 * endpoint.
 * Default: enabled */
#define AEEBT_A2DP_CTL_HONOR_IN_USE    3

/* Ignore the in_use flag of the remote
 * endpoint.
 * Default: disabled */
#define AEEBT_A2DP_CTL_IGNORE_IN_USE   4

/* Use AVRCP_RECONFIGURE to reconfigure streams
 * Default: disabled */
#define AEEBT_A2DP_CTL_RECFG_BY_RECFG  5

/* Use AVRCP_CLOSE to reconfigure streams
 * Default: enabled */
#define AEEBT_A2DP_CTL_RECFG_BY_CLOSE  6

/* Disconnect/reconnect to reconfigure streams
 * Default: disabled */
#define AEEBT_A2DP_CTL_RECFG_BY_REPAGE 7
  
/* Request AVS to increase bitrate. Disables
 * auto bitrate selection */
#define AEEBT_A2DP_CTL_INCR_BITRATE    8

/* Request to AVS to decrease bitrate. 
 * Disables auto bitrate selection */
#define AEEBT_A2DP_CTL_DECR_BITRATE    9

/* Sets the bitrate to a particular value.
 * Disables auto bitrate selection.
 * data1: bitrate in kbps */
#define AEEBT_A2DP_CTL_SET_BITRATE     10

/* Return to auto bitrate selection. This is
 * the default */
#define AEEBT_A2DP_CTL_AUTO_BITRATE    11

/* Sets the lowest bitrate that AUTO_BITRATE
 * will use. Enables auto bitrate selection.
 * data1: bt_pf_a2dp_bitrate_type, not CUSTOM
 * Default: AEEBT_A2DP_BITRATE_LOW */
#define AEEBT_A2DP_CTL_LOWEST_BITRATE  12


typedef struct _IBTExtA2DP IBTExtA2DP;

AEEINTERFACE(IBTExtA2DP)
{
   INHERIT_IQueryInterface(IBTExtA2DP);
   int (*SetDevice)(IBTExtA2DP* po, AEEBTBDAddr* pBDAddr);
   int (*Connect)(IBTExtA2DP* po, AEEBTBDAddr* pBDAddr);
   int (*Disconnect)(IBTExtA2DP* po);
   int (*Ctl)(IBTExtA2DP* po, uint8 ctl, uint32 data);
   int (*Start)(IBTExtA2DP* po);
   int (*Suspend)(IBTExtA2DP* po);
};

#define IBTEXTA2DP_AddRef(p)                              \
        AEEGETPVTBL((p),IBTExtA2DP)->AddRef((p))

#define IBTEXTA2DP_Release(p)                             \
        AEEGETPVTBL((p),IBTExtA2DP)->Release((p))

#define IBTEXTA2DP_QueryInterface(p,i,ppo)                \
        AEEGETPVTBL((p),IBTExtA2DP)->QueryInterface((p),(i),(ppo))

#define IBTEXTA2DP_SetDevice(p,pa)                        \
        AEEGETPVTBL((p),IBTExtA2DP)->SetDevice((p),(pa))

#define IBTEXTA2DP_Connect(p,pa)                          \
        AEEGETPVTBL((p),IBTExtA2DP)->Connect((p),(pa))

#define IBTEXTA2DP_Disconnect(p)                          \
        AEEGETPVTBL((p),IBTExtA2DP)->Disconnect((p))

#define IBTEXTA2DP_Ctl(p,c,d)                             \
        AEEGETPVTBL((p),IBTExtA2DP)->Ctl((p),(c),(d))

#define IBTEXTA2DP_Start(p)                          \
        AEEGETPVTBL((p),IBTExtA2DP)->Start((p))

#define IBTEXTA2DP_Suspend(p)                          \
        AEEGETPVTBL((p),IBTExtA2DP)->Suspend((p))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtA2DP

Description: BlueTooth Advanced Audio Distribution Profile

  This interface is for the A2DP Source role only.

  The use of IBTExtA2DP consists of sending commands and receiving events.
  These events are delivered to IBTExtA2DP clients as notifications, hence
  the clients must register for A2DP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_A2DP.  See AEEBTExt.h for
  more information on IBTExtNotifier.

  If a command returns SUCCESS, then an event is guaranteed to be generated
  in response at a later time.

  Once an A2DP class is created, it will cause the Bluetooth device to be
  connectable. That is, it will scan for connections. This may cause shorter
  standby times, and consume battery power.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_A2DP_EVT_* events when the notification mask
  is NMASK_BT_A2DP.  The pData field of the AEENotify is a pointer to
  NotificationData.

  Event                                      pData->
  -----                                      ----------------------------
  AEEBT_A2DP_EVT_CONNECTED                   bdAddr
  AEEBT_A2DP_EVT_CON_FAILED                  uError
  AEEBT_A2DP_EVT_DISCONNECTED                uError

Interfaces:

  IBTEXTA2DP provides several additional interfaces; please see the
  corresponding header file for complete documentation.

  IBASE                        AEE.h
    IBTEXTA2DP_AddRef()
    IBTEXTA2DP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTA2DP_QueryInterface()

=======================================================================

Function: IBTEXTA2DP_Connect()

Description: Connects to an A2DP sink device.

Prototype:

  int IBTEXTA2DP_Connect( IBTExtA2DP*       po,
                          AEEBTAddr*        pBDAddr );

Parameters:

  po            - Pointer to the IBTExtA2DP object
  pBDAddr       - Address of the A2DP sink

Return value:

  SUCCESS indicates that the configuration request was queued and that
  AEEBT_A2DP_EVT_CONNECTED or _CON_FAILED event will be generated at a
  later time. All other values indicate failure, and will not generate an event.

  After a device is connected, the Bluetooth driver will NOT direct audio to be
  played over the A2DP device. The app must perform this operation.

  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtA2DP object
    ENOMEMORY - out of memory

Event:

  AEEBT_A2DP_EVT_CONNECTED indicates a successful connection has been established
  with the specfied bluetooth device.

  AEEBT_A2DP_EVT_CONN_FALIED with accompanying error code.

=======================================================================

Function: IBTEXTA2DP_Disconnect()

Description: Disconnects an A2DP sink device.

Prototype:

  int IBTEXTA2DP_Disconnect( IBTExtA2DP*       po );

Parameters:

  po            - Pointer to the IBTExtA2DP object

Return value:

  SUCCESS indicates that the configuration request was queued and that an
  AEEBT_A2DP_EVT_DISCONNECTED event will be generated at a later time.
  All other values indicate failure, and will not generate an event.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtA2DP object
    ENOMEMORY - out of memory

Event:

  AEEBT_A2DP_EVT_DISCONNECTED indicates a successful connection has been
  disconnected.

=======================================================================

Function: IBTEXTA2DP_Ctl()

Description: Used for fine grained control of A2DP, mostly useful for
             testing and interop with broken devices.

Prototype:

  int IBTEXTA2DP_Ctl( IBTExtA2DP*       po,
                      uint8             ctl,
                      uint32            data );

Parameters:

  po            - Pointer to the IBTExtA2DP object
  ctl           - Control value. One of:
             * Send AVRCP_SUSPEND when streaming ends
             * Default: enabled *
            #define AEEBT_A2DP_CTL_SUSPEND_ON_END  0
            
             * Send AVRCP_CLOSE when streaming ends
             * Default: disabled *
            #define AEEBT_A2DP_CTL_CLOSE_ON_END    1
            
             * Sets the delay between receiving a start
             * cfm and streaming data.
             * data1: timeout in ms.
             * Default: 500ms (must be bigger than 4) *
            #define AEEBT_A2DP_CTL_START_DELAY     2
            
             * Honor the in_use flag of the remote 
             * endpoint.
             * Default: enabled *
            #define AEEBT_A2DP_CTL_HONOR_IN_USE    3
            
             * Ignore the in_use flag of the remote
             * endpoint.
             * Default: disabled *
            #define AEEBT_A2DP_CTL_IGNORE_IN_USE   4
            
             * Use AVRCP_RECONFIGURE to reconfigure streams
             * Default: disabled *
            #define AEEBT_A2DP_CTL_RECFG_BY_RECFG  5
            
             * Use AVRCP_CLOSE to reconfigure streams
             * Default: enabled *
            #define AEEBT_A2DP_CTL_RECFG_BY_CLOSE  6
            
             * Disconnect/reconnect to reconfigure streams
             * Default: disabled *
            #define AEEBT_A2DP_CTL_RECFG_BY_REPAGE 7
              
             * Request AVS to increase bitrate. Disables
             * auto bitrate selection *
            #define AEEBT_A2DP_CTL_INCR_BITRATE    8
            
             * Request to AVS to decrease bitrate. 
             * Disables auto bitrate selection *
            #define AEEBT_A2DP_CTL_DECR_BITRATE    9
            
             * Sets the bitrate to a particular value.
             * Disables auto bitrate selection.
             * data1: bitrate in kbps *
            #define AEEBT_A2DP_CTL_SET_BITRATE     10
            
             * Return to auto bitrate selection. This is
             * the default *
            #define AEEBT_A2DP_CTL_AUTO_BITRATE    11
            
             * Sets the lowest bitrate that AUTO_BITRATE
             * will use. Enables auto bitrate selection.
             * data1: bt_pf_a2dp_bitrate_type, not CUSTOM
             * Default: AEEBT_A2DP_BITRATE_LOW *
            #define AEEBT_A2DP_CTL_LOWEST_BITRATE  12
   
    data          - Data which will be interpreted differently based on the
                  ctl value.

Return value:

  SUCCESS indicates that the configuration request was queued.
  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtA2DP object
    ENOMEMORY - out of memory

Event:

  None.
  AEEBT_A2DP_BITRATE if the bitrate has changed as a result of this
  call.

=======================================================================

Function: IBTEXTA2DP_Start()

Description: Normally, this function should never be called. It will
             configure the remote headset using the previous SBC
             parameters, and send a AVDTP Start command.
             If there are no previous SBC parameters, this will probably
             result in disconnecting from the headset.
             SBC parameters will be set by the sound subsystem, and will
             be valid after receiving a START event with the data value
             of AEEBT_A2DP_EVENT_LOCAL_INIT, and before disconnecting
             from the device.

Prototype:

  int IBTEXTA2DP_Start( IBTExtA2DP*       po )

Parameters:

  po            - Pointer to the IBTExtA2DP object

Return value:

  SUCCESS indicates that the Start request was queued.
  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtA2DP object
    ENOMEMORY - out of memory

Event:

  AEEBT_A2DP_EVT_START once streaming has started.

=======================================================================

Function: IBTEXTA2DP_Suspend()

Description: Normally, this function should never be called. It will
             suspend streaming to the remote device, sending either an
             AVDTP Suspend or AVDTP Close command to the remote (depending
             on values set in IBTEXTA2DP_Ctl()).
             If audio is still being played to the A2DP sink, this audio
             will be dropped. The app should insure that the seleted
             sound device is no longer A2DP to avoid dropped audio.

Prototype:

  int IBTEXTA2DP_Suspend( IBTExtA2DP*       po )

Parameters:

  po            - Pointer to the IBTExtA2DP object

Return value:

  SUCCESS indicates that the configuration request was queued.
  All other values indicate failure.
  In this context, the following errorcodes have special meaning:

    EFAILED   - invalid pointer to the IBTExtA2DP object
    ENOMEMORY - out of memory

Event:

  AEEBT_A2DP_EVT_SUSPEND when the stream has been suspended.
  No event is generated if the stream is not in the AVDTP Streaming state.

  =======================================================================*/
#endif /* _AEE_BT_EXT_A2DP_H_ */
