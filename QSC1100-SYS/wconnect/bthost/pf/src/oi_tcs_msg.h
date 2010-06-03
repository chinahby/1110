#ifdef FEATURE_BT_EXTPF_CTP
#ifndef _TCS_MSG_H
#define _TCS_MSG_H
/**
 * @file  
 *
 * functions for sending TCS messages
 */

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_l2cap.h"
#include "oi_l2cap_qos.h"

#include "oi_tcs.h"
#include "oi_tcs_spec.h"
#include "oi_tcs_cc.h"

/** The value to be ORed with the actual clock offset. */
#define TCS_CLOCK_OFFSET_VALID   BIT15

/** The value to be used when no clock offset should be sent in a message. */
#define TCS_CLOCK_OFFSET_INVALID 0

/** The maximum size of a TCS packet. */
#define TCS_MAX_MESSAGE_SIZE 256

/** Receive data. */
void OI_TCS_Recv_Msg(TCS_CALL_INFO *call,
                     OI_UINT8 *buf,
                     OI_UINT16 bufLen);

/** 
 * Send ALERTING message.  This message is sent by the incoming side
 * to indicate that the called user alerting has been initiated.
 *
 * @param call The global state for the call
 * @param bearer (NULL if none)
 * @param progress (0 if none)
 * @param handle (O if none)
 * @param destCID (0 if none)
 */
OI_STATUS OI_Tcs_Send_Alerting(TCS_CALL_INFO *call,
                               OI_TCS_BEARER *bearer,
                               OI_TCS_PROGRESS progress,
                               SCO_HANDLE handle,
                               OI_L2CAP_CID destCID,
                               OI_UINT8 *companySpecific,
                               OI_UINT8 companySpecificSize);

/**
 * Send CALL_PROCEEDING message.
 *
 */
OI_STATUS OI_Tcs_Send_CallProceeding(TCS_CALL_INFO *call,
                                     OI_TCS_BEARER *bearer,
                                     OI_TCS_PROGRESS progress,
                                     SCO_HANDLE SCOHandle,
                                     OI_L2CAP_CID destCID,
                                     OI_UINT8 *companySpecific,
                                     OI_UINT8 companySpecificSize);

/**
 * This message is sent by the incoming side to indicate call acceptance by the
 * called user.
 * Direction: incoming to outgoing
 * 
 * @param call The global state for the call
 * @param bearer (NULL if none)
 * @param handle (0 if none)
 *
 */
OI_STATUS OI_Tcs_Send_Connect(TCS_CALL_INFO *call,
                              OI_TCS_BEARER *bearer,
                              SCO_HANDLE handle);

/**
 * CONNECT ACKNOWLEDGE
 * This message is sent by the outgoing side to acknowledge the receipt of a
 * CONNECT message.
 * Direction: outgoing to incoming
 */
OI_STATUS OI_Tcs_Send_ConnectAck(TCS_CALL_INFO *call,
                                 SCO_HANDLE handle,
                                 OI_L2CAP_CID destCID);

/**
 * DISCONNECT
 * This message is sent by either side as an invitation to terminate the call.
 * Direction: both
 *
 * @param call The global state for the call
 * @param cause
 * @param progress
 * @param handle
 * @param destCID
 */
OI_STATUS OI_Tcs_Send_Disconnect(TCS_CALL_INFO *call,
                                 OI_TCS_CAUSE cause,
                                 OI_TCS_PROGRESS progress,
                                 SCO_HANDLE handle,
                                 OI_L2CAP_CID destCID);

/**
 * INFORMATION
 * This message is sent by either side to provide additional information during call
 * establishment (in case of overlap sending).
 * Direction: both 
 */
OI_STATUS OI_Tcs_Send_Information(TCS_CALL_INFO *call,
                                  OI_BOOL sendingComplete,
                                  OI_CHAR keypadFacility,
                                  OI_TCS_NUMBER *callingNumber,
                                  OI_TCS_NUMBER *calledNumber,
                                  OI_TCS_AUDIO_CONTROL audioControl,
                                  OI_UINT8 *companySpecific,
                                  OI_UINT8 companySpecificSize,
                                  OI_UINT timerType);

/**
 * PROGRESS
 * This message is sent by the incoming side to indicate the progress of a call in
 * the event of interworking or by either side in the call with the provision of
 * optional in-band information/patterns.
 * Direction: incoming to outgoing
 */
OI_STATUS OI_Tcs_Send_Progress(TCS_CALL_INFO *call,
                               OI_TCS_PROGRESS progress,
                               SCO_HANDLE handle,
                               OI_L2CAP_CID destCID);

/**
 * Send a RELEASE message.
 *
 * This message is used to indicate that the device sending the
 * message had disconnected the channel (if any) and intends to release
 * the channel, and that receiving device should release the channel
 * after sending RELEASE COMPLETE. 
 */
OI_STATUS OI_Tcs_Send_Release(TCS_CALL_INFO *call,
                              OI_TCS_CAUSE cause,
                              SCO_HANDLE handle);

/**
 * Send a RELEASE COMPLETE message.
 *
 * This message is used to indicate that the device sending the message has
 * released the channel (if any), and that the channel is available for re-use.
 */
OI_STATUS OI_Tcs_Send_ReleaseComplete(TCS_CALL_INFO *call,
                                      OI_TCS_CAUSE cause,
                                      SCO_HANDLE handle);

/** 
 * Send a SETUP message.
 *
 * @param call The global state for the call
 * @param callClass  The type of call (REQUIRED).
 * @param sendingComplete  TRUE if the called party number has been completed (REQUIRED).
 * @param bearerCapability  The capabilities of the bearer link (NULL to use default).
 * @param signal  An alerting signal or tone. (0 to not include)
 * @param callingNumber  The number of the calling party. (NULL to not include)
 * @param calledNumber  The number of the called party. (NULL to not include)
 */
OI_STATUS OI_Tcs_Send_Setup(TCS_CALL_INFO *call,
                            OI_TCS_CALL_CLASS callClass,
                            OI_BOOL sendingComplete,
                            OI_TCS_BEARER *bearerCapability,
                            OI_TCS_SIGNAL signal,
                            OI_TCS_NUMBER *callingNumber,
                            OI_TCS_NUMBER *calledNumber,
                            OI_UINT8 *companySpecific,
                            OI_UINT8 companySpecificSize);

/**
 * Send a SETUP ACKNOWLEDGE message.
 *
 * This message is sent by the incoming side to indicate that call establishment
 * has been initiated, but additional information may be required.
 */
OI_STATUS OI_Tcs_Send_SetupAck(TCS_CALL_INFO *call,
                               OI_TCS_BEARER *bearerCapability,
                               OI_TCS_PROGRESS progress,
                               SCO_HANDLE handle,
                               OI_L2CAP_CID destCID,
                               OI_UINT8 *companySpecific,
                               OI_UINT8 companySpecificSize);

/**
 * Send START DTMF.
 *
 * This message contains the digit the other side should reconvert back into a
 * DTMF tone, which is then applied towards the remote user.
 *
 * @param call The global state for the call
 * @param facility  (REQUIRED)
 */
OI_STATUS OI_Tcs_Send_StartDTMF(TCS_CALL_INFO *call,
                                OI_CHAR facility);

/** 
 * Send START DTMF ACKNOWLEDGE
 * 
 * This message is sent to indicate the successful initiation of the action required
 * by the Start DTMF message.
 *
 * @param call The global state for the call
 * @param facility (REQUIRED)
 */
OI_STATUS OI_Tcs_Send_StartDTMFAck(TCS_CALL_INFO *call,
                                   OI_CHAR facility);

/**
 * Start DTMF Reject
 *
 * This message is sent to indicate that the other side cannot accept the Start
 * DTMF message.
 *
 * @param call The global state for the call
 * @param cause (OPTIONAL - 0 for none)
 */
OI_STATUS OI_Tcs_Send_StartDTMFReject(TCS_CALL_INFO *call,
                                      OI_TCS_CAUSE cause);

/**
 * Stop DTMF
 *
 * This message is used to stop the DTMF tone sent towards the remote user.
 */
OI_STATUS OI_Tcs_Send_StopDTMF(TCS_CALL_INFO *call);


/**
 * Stop DTMF Acknowledge
 *
 * @param call The global state for the call
 * @param facility (REQUIRED)
 */
OI_STATUS OI_Tcs_Send_StopDTMFAck(TCS_CALL_INFO *call,
                                  OI_CHAR facility);

// Group Management messages

/**
 * ACCESS RIGHTS REQUEST
 *
 * This message is sent by the initiating side to obtain access rights.
 */
OI_STATUS OI_Tcs_Send_AccessRightsRequest(TCS_CALL_INFO *call);


/**
 * ACCESS RIGHTS ACCEPT
 *
 * This message is sent by the responding side to indicate granting of access
 * rights.
 */
OI_STATUS OI_Tcs_Send_AccessRightsAccept(TCS_CALL_INFO *call);


/**
 * ACCESS RIGHTS REJECT
 * 
 * This message is sent by the responding side to indicate denial of access rights.
 */
OI_STATUS OI_Tcs_Send_AccessRightsReject(TCS_CALL_INFO *call);


/**
 * INFO SUGGEST
 *
 * This message is sent by the WUG master to indicate that a change has
 * occurred in the WUG configuration.
 * Direction: WUG master to WUG member
 *
 * @param call The global state for the call
 * @param config  The configuration information. (REQUIRED)
 */
OI_STATUS OI_Tcs_Send_InfoSuggest(TCS_CALL_INFO *call,
                                  TCS_CONFIG_DATA *config);

/**
 * INFO ACCEPT
 * 
 * This message is sent by the WUG member to indicate the acceptance of the
 * updated WUG configuration.
 */
OI_STATUS OI_Tcs_Send_InfoAccept(TCS_CALL_INFO *call);


/**
 * LISTEN REQUEST
 *
 * This message is sent by a WUG member to indicate to the WUG master the
 * request for a Fast inter-member access to the indicated WUG member.
 * Direction: WUG member to WUG master
 *
 * @param call The global state for the call
 * @param calledNumber  The called number. (REQUIRED)
 */
OI_STATUS OI_Tcs_Send_ListenRequest(TCS_CALL_INFO *call,
                                    OI_TCS_NUMBER *calledNumber);

/**
 * LISTEN SUGGEST
 * This message is sent by a WUG master to indicate to the WUG member the
 * request for a Fast inter-member access.
 * Message Type: LISTEN SUGGEST
 * Direction: WUG master to WUG member
 */
OI_STATUS OI_Tcs_Send_ListenSuggest(TCS_CALL_INFO *call);


/** 
 * LISTEN ACCEPT
 * This message is sent to indicate the acceptance of the previous request for a
 * Fast inter-member access.
 * Direction: both
 *
 * @param call The global state for the call
 * @param clockOffset Bits 16-2 of the Bluetooth clock offset (0 for none)
 */
OI_STATUS OI_Tcs_Send_ListenAccept(TCS_CALL_INFO *call,
                                   TCS_CLOCK_OFFSET clockOffset);

/**
 * LISTEN REJECT
 * This message is sent to indicate the rejection of the previous request for a Fast
 * inter-member access.
 * Message Type: LISTEN REJECT
 * Direction: both
 *
 * @param call The global state for the call
 * @param cause (O for none)
 */
OI_STATUS OI_Tcs_Send_ListenReject(TCS_CALL_INFO *call,
                                   OI_TCS_CAUSE cause);

/**
 * This message is sent by either side to provide additional
 * information in a connectionless manner.  Message Type: CL INFO
 * Direction: both
 *
 * @param call The global state for the call
 * @param audioControl (NULL for none)
 */
OI_STATUS OI_Tcs_Send_CLInfo(TCS_CALL_INFO *call, 
                             OI_TCS_AUDIO_CONTROL audioControl,
                             OI_UINT8 *companySpecific,
                             OI_UINT8 companySpecificSize);

#endif /* _TCS_MSG_H */

/**@}*/
#endif /* FEATURE_BT_EXTPF_CTP */
