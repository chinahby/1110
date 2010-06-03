#ifndef OI_CTP_H
#define OI_CTP_H

/**
 * @file
 *
 * This file defines the the interface for the Cordless Telephony Profile (CTP).
 * The remainder of the functions for CTP are found in oi_tcs.h.
 */

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#include "oi_tcs.h"
#include "oi_sdpdb.h"

/* Network type codes for use with OI_CTP_InitGateway(). */
#define OI_CTP_NETWORK_PSTN 1
#define OI_CTP_NETWORK_ISDN 2
#define OI_CTP_NETWORK_GSM  3
#define OI_CTP_NETWORK_CDMA 4
#define OI_CTP_NETWORK_CELLULAR 5
#define OI_CTP_NETWORK_PACKET_SWITCHED 6
#define OI_CTP_NETWORK_OTHER 7

/** The maximum number of digits allowed in the calling and called number information elements */
#define OI_CTP_MAX_NUM_DIGITS 24

/**
 * Initialize a cordless gateway.
 * 
 * @param  strings      the name to advertise in the service record
 * @param  networkType  the type of network to which the the gateway is connected
 * @param  callbacks    the callbacks to use
 */
OI_STATUS OI_CTP_GW_Register(const OI_SDP_STRINGS *strings,
                             OI_UINT8 networkType,
                             const OI_TCS_CALLBACKS *callbacks);

/**
 * Terminate a cordless gateway.
 */
OI_STATUS OI_CTP_GW_Deregister(void);

/**
 * Initialize a cordless terminal.
 * 
 * @param strings       the name to advertise in the service record
 * @param  callbacks    the callbacks to use
 */
OI_STATUS OI_CTP_TL_Register(const OI_SDP_STRINGS *strings,
                             const OI_TCS_CALLBACKS *callbacks);

/**
 * Initialize a cordless terminal in the Intercom role only
 * 
 * @param strings       the name to advertise in the service record
 * @param  callbacks    the callbacks to use
 */
OI_STATUS OI_CTP_TL_RegisterIntercom(const OI_SDP_STRINGS *strings,
                                     const OI_TCS_CALLBACKS *callbacks);

/**
 * Terminate a cordless terminal.
 */
OI_STATUS OI_CTP_TL_Deregister(void);

/**
 * Establish a connection to a cordless service on the specified device.  This
 * function should only be called by a terminal initiating a connection to a 
 * gateway, or a gateway re-establishing a connection when the gateway does not
 * support park mode.  If a terminal is connecting to another terminal, @ref
 * OI_CTP_ConnectIntercom should be used instead.
 *
 * @param addr  The address of the connected device to establish the call.
 */
OI_STATUS OI_CTP_Connect(OI_BD_ADDR *addr);

/**
 * Establish a connection to an intercom service on the specified device.
 *
 * @param addr  The address of the connected device to establish the call.
 */
OI_STATUS OI_CTP_ConnectIntercom(OI_BD_ADDR *addr);

/**
 * Establish a call.  This function can be used to establish a call with a specific device or, 
 * in the case of the gateway, to offer the call to all attached terminals.  This will send a
 * SETUP message with the bearer type and numbers specified in the arguments, sending complete 
 * set to true, and no signal information element.  The raw OI_TCS_EstablishCall may be used
 * to override these defaults.
 *
 * @param addr           The address of the device to connect to, or NULL to offer the call to all 
 *                       attached terminals if the local device is operating in gateway role
 * @param linkType       The type of link to use for the call data (OI_TCS_BEARER_LINK_TYPE_SCO or OI_TCS_BEARER_LINK_TYPE_NONE).
 *                       SCO provides the default parameters of HV1 packets and CVSD coding.
 * @param callingNumber  The calling party number.
 * @param calledNumber   The called party number
 */
OI_STATUS OI_CTP_EstablishCall( OI_BD_ADDR *addr,
                                OI_TCS_BEARER_LINK_TYPE linkType,
                                OI_TCS_NUMBER *callingNumber,
                                OI_TCS_NUMBER *calledNumber);

/* The remainder of the functions for CTP are identical to those provided by TCS. The following macros are provided for convenience. */

#define OI_CTP_AcceptConnection OI_TCS_AcceptConnection
#define OI_CTP_Disconnect OI_TCS_Disconnect
#define OI_CTP_ClearCall OI_TCS_ClearCall
#define OI_CTP_Info OI_TCS_Info
#define OI_CTP_IncomingStatus OI_TCS_IncomingStatus
#define OI_CTP_AcceptCall OI_TCS_AcceptCall
#define OI_CTP_StartDTMF OI_TCS_StartDTMF
#define OI_CTP_StartDTMFAck OI_TCS_StartDTMFAck
#define OI_CTP_StartDTMFReject OI_TCS_StartDTMFReject
#define OI_CTP_StopDTMF OI_TCS_StopDTMF
#define OI_CTP_StopDTMFAck OI_TCS_StopDTMFAck
#define OI_CTP_RegisterRecall OI_TCS_RegisterRecall


/**@}*/

#endif
