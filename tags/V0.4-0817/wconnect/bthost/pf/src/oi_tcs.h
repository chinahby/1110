#ifdef FEATURE_BT_EXTPF_CTP
#ifndef _OI_TCS_H
#define _OI_TCS_H
/**
 * @file  
 *
 * This file provides the public interface to the BLUEmagic 3.0 implementation of the
 * Telephony Control protocol Specification - Binary (TCS Binary). The interface
 * currently provides only the ability to set up a call. To use this interface,
 * applications must first register a list of callback functions using
 * OI_TCS_Register(). Only one application may be registered with the TCS module at
 * any time.
 * 
 * To initiate an outgoing call, the application invokes OI_TCS_EstablishCall().
 * 
 * An application is notified of an incoming call by an @ref OI_TCS_ESTABLISH_IND 
 * callback. To reject the call, the application calls OI_TCS_ClearCall(). The 
 * application can then make successive calls to OI_TCS_IncomingStatus(). To accept 
 * the call, the application must call OI_TCS_AcceptCall(). Once OI_TCS_IncomingStatus() 
 * has been called, the @ref OI_TCS_ESTABLISH_CFM callback function will be called when 
 * the call is complete.
 *
 * OI_TCS_ClearCall() can be used by either side to terminate an active call.
 * The @ref OI_TCS_RELEASE_IND callback will be called when the call has been
 * shut down, regardless of whether OI_TCS_ClearCall() was called.
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
#include "oi_tcs_consts.h"

/* control characters defined in section F.3.10 of the Bluetooth v1.1 specification */
#define OI_TCS_CHAR_DIALLING_PAUSE  0x05
#define OI_TCS_CHAR_REGISTER_RECALL 0x16

/* constants identifying the TCS role */
#define OI_TCS_ROLE_TERMINAL 1
#define OI_TCS_ROLE_GATEWAY  2

/** Call status codes: These status codes indicate the progression of the call
    and are used by the incoming side with OI_TCS_IncomingStatus() and presented 
    to the outgoing side in OI_TCS_OUTGOING_STATUS(). */
typedef enum {

    /** This value indicates that the calling party number is not yet complete. */
    OI_TCS_CALL_STATUS_OVERLAP_RECEIVING,

    /** This value indicates that the called party number information
     * appears to be complete. If an @ref OI_TCS_ESTABLISH_IND or @ref
     * OI_TCS_INFORMATION_IND callback has been received with a
     * sendingComplete value of TRUE, this status does not need to be
     * sent.
     */
    OI_TCS_CALL_STATUS_PROCEEDING,

    /** This value indicates that the user is being alerted. This status should
        be passed once the application has begun notification of the user. */
    OI_TCS_CALL_STATUS_ALERTING
    
} OI_TCS_CALL_STATUS;

/************************************************************

  Type definitions

*************************************************************/

/** This is the type for a telephone number. A structure of this type can represent
    a called party number (if the calling field is FALSE) or calling party number (if
    the calling field is TRUE). The presentation and screening fields are only present 
    in the calling number. */
typedef struct {

    /** the direction of the number - calling if TRUE, called if FALSE */
    OI_BOOL calling;

    /** the type of number */
    OI_TCS_NUMBER_TYPE type;

    /** the numbering plan */
    OI_TCS_NUMBER_PLAN plan;

    /** the presentation indicator - CALLING only */
    OI_TCS_PRESENTATION presentation;
    
    /** the screening indicator - CALLING only */
    OI_TCS_SCREENING screening;

    /** the digits in the number */
    OI_UINT8 *digits;
    
    /** the number of digits */
    OI_UINT8 numDigits;

} OI_TCS_NUMBER;


/** capabilities for a SCO link */
typedef struct {
    OI_TCS_BEARER_PACKET_TYPE packetType;
    OI_TCS_BEARER_CODING coding;
} OI_TCS_BEARER_SCO;

/** capabilities for an ACL link */
typedef struct {
    OI_L2CAP_FLOWSPEC *flowspec;
    OI_TCS_BEARER_UI_LAYER2 layer2;
    OI_TCS_BEARER_UI_LAYER3 layer3;
} OI_TCS_BEARER_ACL;

/** A structure of this type is used to group together bearer capabilities requested for a call. 
*/
typedef struct {
    /** the type of link */
    OI_TCS_BEARER_LINK_TYPE type;

    /** the capability information */
    struct {
        /** the SCO capabilities */
        OI_TCS_BEARER_SCO sco;

        /** the ACL capabilities */
        OI_TCS_BEARER_ACL acl;
    } info;    

} OI_TCS_BEARER;


/** 
 * The application information used in a call request. This structure
 * is passed to @ref OI_TCS_EstablishCall on the outgoing side and 
 * is passed to @ref OI_TCS_ESTABLISH_IND on the incoming side.
 */
typedef struct {

    /** the class for the call */
    OI_TCS_CALL_CLASS callClass;

    /** TRUE if the call information is complete */
    OI_BOOL sendingComplete;

    /** the capability of the requested bearer channel (NULL to use the default) */
    OI_TCS_BEARER *bearer;

    /** the alerting signal */
    OI_TCS_SIGNAL signal;
    
    /** the calling party number */
    OI_TCS_NUMBER *callingNumber;

    /** the called party number */
    OI_TCS_NUMBER *calledNumber;

    /**
     * The company specific info (NULL if not present).  This should include
     * the two leading octets that identify the company.
     */
    OI_UINT8 *companySpecific;

    /** 
     * The total count of bytes in the company specific info.  This includes the
     * two leading octets that identify the company.
     */
    OI_UINT8 companySpecificSize;

} OI_TCS_CONNECT_INFO;

/************************************************************

  callbacks

*************************************************************/

/**
 * A callback function of this type indicates that a remote device is attempting
 * to connect. To accept the connection, call OI_TCS_AcceptConnection().
 *
 * @param addr  the address of the device attempting the connection
 */
typedef void (*OI_TCS_CONNECT_IND)(OI_BD_ADDR *addr);

/** 
 * A callback function of this type confirms the connection to another device.
 *
 * @param addr    the address of the remote device to which the connection was established
 * @param result  succss or failure of establishment
 */
typedef void (*OI_TCS_CONNECT_CFM)(OI_BD_ADDR *addr,
                                   OI_STATUS result);

/**
 * A callback function of this type confirms a disconnection from another device.
 *
 * @param addr    the address of the remote device that was disconnected
 */
typedef void (*OI_TCS_DISCONNECT_CFM)(OI_BD_ADDR *addr);

/**
 * A callback function of this type indicates that a remote device is attempting
 * to establish a call. If sendingComplete is TRUE, then the outgoing side has 
 * completed sending the called party number. OI_TCS_IncomingStatus() should be 
 * called to inform the remote side of the progress of the call, OI_TCS_AcceptCall() 
 * should be called to accept the call immediately, or OI_TCS_ClearCall() should be 
 * called to reject the call.
 * 
 * @param addr  the address of the remote device
 * @param info  the information for the call
 */
typedef void (*OI_TCS_ESTABLISH_IND)(OI_BD_ADDR *addr,
                                     OI_TCS_CONNECT_INFO *info);


/** 
 * A callback function of this type confirms establishment of an outgoing call. 
 *
 * @param addr    the address of the remote device to which the call was established
 * @param result  success or failure of establishment
 */
typedef void (*OI_TCS_ESTABLISH_CFM)(OI_BD_ADDR *addr,
                                     OI_STATUS result);

/**
 * A callback function of this type indicates information has been received. 
 * If sendingComplete is TRUE, then the outgoing side has completed sending 
 * the called party number.
 *
 * @param addr              the address of the device sending the information
 * @param sendingComplete   TRUE if the calling party number is complete
 * @param keypadFacility    OI_TCS_KEYPAD_FACILITY
 * @param callingNumber     the number of the calling party
 * @param calledNumber      the number of the called party
 * @param audioControl      audio control information
 * @param companySpecific       the company specific information element included with this info.  Note
 *                              that this includes the two company identifcation octets at the beginning.
 *                              This may be NULL.
 * @param companySpecificSize   the total number of bytes of the company specific information element.
 */
typedef void (*OI_TCS_INFORMATION_IND)(OI_BD_ADDR *addr,
                                       OI_BOOL sendingComplete,
                                       OI_CHAR keypadFacility,
                                       OI_TCS_NUMBER *callingNumber,
                                       OI_TCS_NUMBER *calledNumber,
                                       OI_TCS_AUDIO_CONTROL audioControl,
                                       OI_UINT8 *companySpecific,
                                       OI_UINT8 companySpecificSize);
/**
 * A callback function of this type indicates that a DTMF tone should be started.
 * @ref OI_TCS_StartDTMFAck or @ref OI_TCS_StartDTMFReject must be called in response 
 * to this function.  
 * This may be set to NULL in the @ref OI_TCS_CALLBACKS structure.  In that case,
 * @ref OI_TCS_DTMF_STOP must also be NULL.  Setting those to NULL is a valid 
 * configuration for a CTP TL, which only issues requests and does not receive them.
 *
 * @param addr  the address of the device generating the tone
 * @param tone  the DTMF tone to be generated
 */
typedef void (*OI_TCS_DTMF_START)(OI_BD_ADDR *addr,
                                  OI_CHAR tone);

/**
 * A callback function of this type indicates the acknowledgement of a previous 
 * DTMF start request.
 * This may be set to NULL in the @ref OI_TCS_CALLBACKS structure.  In that case
 * both @ref OI_TCS_DTMF_START_REJECT and @ref OI_TCS_DTMF_STOP_ACK should also
 * be set to NULL.  This is a valid configuration for a CTP GW, which only responds
 * to requests and does not issue them.
 *
 * @param addr      the address of the device responding
 * @param tone      the DTMF tone requested
 */
typedef void (*OI_TCS_DTMF_START_ACK)(OI_BD_ADDR *addr,
                                      OI_CHAR tone);

/**
 * A callback function of this type indicates the rejection of a previous 
 * DTMF start request.
 * This may be set to NULL in the @ref OI_TCS_CALLBACKS structure.  In that case
 * both @ref OI_TCS_DTMF_START_ACK and @ref OI_TCS_DTMF_STOP_ACK should also
 * be set to NULL.  This is a valid configuration for a CTP GW, which only responds
 * to requests and does not issue them.
 *
 * @param addr      the address of the device responding
 * @param cause     further information about rejection
 */
typedef void (*OI_TCS_DTMF_START_REJECT)(OI_BD_ADDR *addr,
                                         OI_TCS_CAUSE cause);

/**
 * A callback function of this type indicates that a DTMF tone should be stopped.
 * This may be set to NULL in the @ref OI_TCS_CALLBACKS structure.  In that case,
 * @ref OI_TCS_DTMF_START must also be NULL.  Setting those to NULL is a valid 
 * configuration for a CTP TL, which only issues requests and does not receive them.
 *
 * @param addr  the address of the device stopping the tone
 */
typedef void (*OI_TCS_DTMF_STOP)(OI_BD_ADDR *addr);

/**
 * A callback function of this type indicates that a DTMF tone has stopped.
 * This may be set to NULL in the @ref OI_TCS_CALLBACKS structure.  In that case
 * both @ref OI_TCS_DTMF_START_ACK and @ref OI_TCS_DTMF_START_REJECT should also
 * be set to NULL.  This is a valid configuration for a CTP GW, which only responds
 * to requests and does not issue them.
 *
 * @param addr  the address of the device stopping the tone
 * @param tone  the DTMF tone that is stopped
 */
typedef void (*OI_TCS_DTMF_STOP_ACK)(OI_BD_ADDR *addr,
                                     OI_CHAR tone);

/**
 * A callback function of this type indicates that the call has been released.
 *
 * @param cause  the cause of the release
 */
typedef void (*OI_TCS_RELEASE_IND)(OI_BD_ADDR *addr,
                                   OI_TCS_CAUSE cause);

/**
 * A callback function of this type provides the status of an outgoing call.
 * When the call is complete, OI_TCS_ESTABLISH_CFM() will be called.
 *
 * @param addr    address of the remote device
 * @param status  the status of the call (OI_TCS_CALL_STATUS)
 * @param companySpecific       the company specific information element included with this status.  Note
 *                              that this includes the two company identifcation octets at the beginning.
 *                              This may be NULL.
 * @param companySpecificSize   the total number of bytes of the company specific information element.
 */
typedef void (*OI_TCS_OUTGOING_STATUS)(OI_BD_ADDR *addr,
                                       OI_TCS_CALL_STATUS status,
                                       OI_UINT8 *companySpecific,
                                       OI_UINT8 companySpecificSize);

/**
 * A callback function of this type indicates that data was received on the 
 * bearer channel established with the specified device. This function will
 * only be called once a call is active. Data delivered over both SCO
 * and ACL connections will be delivered on this connection if these data 
 * can be received over HCI. If the data cannot be received over HCI (as with 
 * many baseband implementations of SCO) this callback will not be called.
 *
 * @param addr     address of the connected device
 * @param data     the received data
 * @param dataLen  the number of bytes received
 */
typedef void (*OI_TCS_CALL_DATA_IND)(OI_BD_ADDR *addr,
                                     OI_BYTE *data,
                                     OI_UINT16 dataLen);

/**
 * A callback function of this type indicates additonal information has been 
 * received in a connectionless manner.  This may be NULL.
 *
 * @param audioControl      audio control information
 * @param companySpecific       the company specific information element included with this info.  Note
 *                              that this includes the two company identifcation octets at the beginning.
 *                              This may be NULL.
 * @param companySpecificSize   the total number of bytes of the company specific information element.
 */
typedef void (*OI_TCS_CL_INFO_IND)(OI_BD_ADDR *addr,
                                   OI_TCS_AUDIO_CONTROL audioControl,
                                   OI_UINT8 *companySpecific,
                                   OI_UINT8 companySpecificSize);

/** callback functions registered by the application */
typedef struct {
    OI_TCS_CONNECT_IND connectInd;
    OI_TCS_CONNECT_CFM connectCfm;
    OI_TCS_DISCONNECT_CFM disconnectCfm;
    OI_TCS_ESTABLISH_CFM establishCfm;
    OI_TCS_ESTABLISH_IND establishInd;
    OI_TCS_INFORMATION_IND infoInd;
    OI_TCS_RELEASE_IND releaseInd;
    OI_TCS_DTMF_START startDTMF;
    OI_TCS_DTMF_START_ACK startDTMFAck;
    OI_TCS_DTMF_START_REJECT startDTMFReject;
    OI_TCS_DTMF_STOP stopDTMF;
    OI_TCS_DTMF_STOP_ACK stopDTMFAck;
    OI_TCS_OUTGOING_STATUS outgoingStatusCB;
    OI_TCS_CALL_DATA_IND callDataInd;
    OI_TCS_CL_INFO_IND clInfoInd;
} OI_TCS_CALLBACKS;

/************************************************************

  general functions 

*************************************************************/

/** 
 * This function registers the TCS protocol module.
 * 
 * @param role         the TCS role (OI_TCS_ROLE_TERMINAL or OI_TCS_ROLE_GATEWAY)
 * @param intercomOnly TRUE for Intercom Profile functionality only
 * @param callbacks    callback functions to register
 */
OI_STATUS OI_TCS_Register(OI_UINT8 role,
                          OI_BOOL intercomOnly,
                          const OI_TCS_CALLBACKS *callbacks);

/**
 * Terminate the TCS registration.
 */
OI_STATUS OI_TCS_Deregister(void);

/**
 * Connect to a TCS service. The connection must complete before further
 * operations can be performed.
 * 
 * @param addr   the Bluetooth device address of the device to which to connect
 * @param psm    L2CAP protocol/service multiplexor (PSM) for the service
 */
OI_STATUS OI_TCS_Connect(OI_BD_ADDR *addr,
                         OI_L2CAP_PSM psm);

/**
 * Accept or reject an incoming connection.
 *
 * @param addr    the address of the device that is attempting to connect
 * @param accept  TRUE to accept the connection, FALSE to reject it
 */
OI_STATUS OI_TCS_AcceptConnection(OI_BD_ADDR *addr,
                                  OI_BOOL accept);

/**
 * Disconnect a connected device.
 *
 * @param addr    the address of the remote device that was disconnected
 */
OI_STATUS OI_TCS_Disconnect(OI_BD_ADDR *addr);

/**
 * This function clears (terminates) the current call and is also used
 * to reject an incoming call.
 *
 * @param addr   the Bluetooth device address associated with the call to clear
 * @param cause  the reason for terminating the call
 */
OI_STATUS OI_TCS_ClearCall(OI_BD_ADDR *addr,
                           OI_TCS_CAUSE cause);


/************************************************************

  outgoing calls

*************************************************************/

/**
 * This function initiates an outgoing call. If addr is NULL, this
 * function initiates an outgoing call in a multi-terminal
 * configuration, so the setup request is broadcast to all attached
 * terminals; this call can be answered by any terminal, so there is
 * no device address specified. 
 * 
 * The result of the establishment will be reported in the 
 * @ref OI_TCS_ESTABLISH_CFM callback. This function is only applicable to 
 * devices configured as gateways.
 *
 * @param addr device address of the remote device to call (NULL to broadcast)
 * @param info TCS connection information
 */
OI_STATUS OI_TCS_EstablishCall(OI_BD_ADDR *addr,
                               OI_TCS_CONNECT_INFO *info);

/** 
 * This function sends further information required for establishing a call. 
 * This function should only be called after calling OI_TCS_EstablishCall().
 *
 * @param addr              device address of the remote device
 * @param sendingComplete   TRUE if the calling party number is complete
 * @param keypadFacility    a keypad character to send
 * @param callingNumber     the number of the calling party
 * @param calledNumber      the number of the called party
 * @param audioControl      audio control information (OI_TCS_AUDIO_CONTROL_NONE 
 *                          if no control action)
 * @param companySpecific       NULL if none.  If used this must include the Company Identification
 *                              element, therefore @ref companySpecificSize must be greater than or 
 *                              equal to two.
 * @param companySpecificSize   The total number of bytes located at @ref companySpecific that should
 *                              be sent.
 */
OI_STATUS OI_TCS_Info(OI_BD_ADDR *addr,
                      OI_BOOL sendingComplete,
                      OI_CHAR keypadFacility,
                      OI_TCS_NUMBER *callingNumber,
                      OI_TCS_NUMBER *calledNumber,
                      OI_TCS_AUDIO_CONTROL audioControl,
                      OI_UINT8 *companySpecific,
                      OI_UINT8 companySpecificSize);

/************************************************************

  incoming calls

*************************************************************/

/** 
 * This function notifies the TCS layer of the progress of a call.  
 * This function may be called repeatedly after the @ref OI_TCS_ESTABLISH_IND 
 * callback has been called and before calling OI_TCS_AcceptCall(). Any of the 
 * OI_TCS_CALL_STATUS codes can be passed to this function. Not all of the status
 * values must be sent in successive calls, but the values must be passed in 
 * ascending order. For example, the OI_TCS_CALL_STATUS_OVERLAP_RECEIVING status 
 * should not be passed after the OI_TCS_CALL_STATUS_PROCEEDING status has been
 * passed.
 *
 * @param addr    device address of the remote device
 * @param status  the current call status
 * @param bearer  NULL if none, only used if this is the first response
 * @param companySpecific       NULL if none.  If used this must include the Company Identification
 *                              element, therefore @ref companySpecificSize must be greater than or 
 *                              equal to two.
 * @param companySpecificSize   The total number of bytes located at @ref companySpecific that should
 *                              be sent.
 */
OI_STATUS OI_TCS_IncomingStatus(OI_BD_ADDR *addr,
                                OI_TCS_CALL_STATUS status,
                                OI_TCS_BEARER *bearer,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificLen);

/**
 * Accept a connection.
 *
 * @param addr      address of the remote device
 * @param bearer    NULL if none, only used if this is the first response
 */
OI_STATUS OI_TCS_AcceptCall(OI_BD_ADDR *addr, 
                            OI_TCS_BEARER *bearer);

/************************************************************

  supplementary services

*************************************************************/

/**
 * Transmit a signal to start a DTMF tone to the remote device. This
 * function can only be used when a call has been established.
 *
 * @param addr   device address of the remote device
 * @param tone   the character on the keypad representing the tone to be generated 
 *               (Value must be <= 0x7f.)
 *
 * @return OI_STATUS_INVALID_STATE if there is not a call present in the active state
 */
OI_STATUS OI_TCS_StartDTMF(OI_BD_ADDR *addr,
                           OI_CHAR tone);

/**
 * Transmit a signal to acknowledge the start of a DTMF tone. This
 * function can only be used when a call has been established, and should
 * be called when the OI_TCS_DTMF_START indicator is issued.
 *
 * @param addr   device address of the remote device
 * @param tone   the character on the keypad representing the tone to be generated 
 *               (Value must be <= 0x7f.)
 *
 * @return OI_STATUS_INVALID_STATE if there is not a call present in the active state
 */
OI_STATUS OI_TCS_StartDTMFAck(OI_BD_ADDR *addr,
                              OI_CHAR tone);

/**
 * Transmit a signal to reject the start of a DTMF tone. This
 * function can only be used when a call has been established, and should
 * be called when the OI_TCS_DTMF_START indicator is issued.
 *
 * @param addr   device address of the remote device
 * @param cause  the cause for rejecting the start request.  This should 
 *               normally be OI_TCS_CAUSE_FACILITY_REJECTED.
 *
 * @return OI_STATUS_INVALID_STATE if there is not a call present in the active state
 */
OI_STATUS OI_TCS_StartDTMFReject(OI_BD_ADDR *addr,
                                 OI_TCS_CAUSE cause);

/**
 * Transmit a signal to stop a DTMF tone to the remote device. This function 
 * can only be used when a call has been established. This function should be 
 * called only after OI_TCS_StartDTMF() has been called.
 *
 * @param addr   device address of the remote device
 *
 * @return OI_STATUS_INVALID_STATE if there is not a call present in the active state
 */
OI_STATUS OI_TCS_StopDTMF(OI_BD_ADDR *addr);

/**
 * Transmit a signal to acknowledge the end of a DTMF tone to the remote device. 
 * This function can only be used when a call has been established, and should be 
 * called when the OI_TCS_DTMF_STOP indicator has been issued.
 *
 * @param addr   device address of the remote device
 * @param tone   the character on the keypad representing the tone to be generated 
 *               (Value must be <= 0x7f.)
 *
 * @return OI_STATUS_INVALID_STATE if there is not a call present in the active state
 */
OI_STATUS OI_TCS_StopDTMFAck(OI_BD_ADDR *addr,
                             OI_CHAR tone);

/**
 * Seize a register for input. This function sends an OI_TCS_Info message
 * with a keypad facility of 0x16. Further digits are sent using the
 * OI_TCS_StartDTMF function.
 *
 * @param addr  (OI_BD_ADDR*) a pointer to the address of the remote device
 */
#define OI_TCS_RegisterRecall(addr) OI_TCS_Info(addr, FALSE, OI_TCS_CHAR_REGISTER_RECALL, NULL, NULL, OI_TCS_AUDIO_CONTROL_NONE, NULL, 0)

/************************************************************

  connectionless TCS

*************************************************************/

/** 
 * This function sends signalling information without establishing a call.
 *
 * @param addr              device address of the remote device (NULL to broadcast).
 * @param audioControl      audio control information (OI_TCS_AUDIO_CONTROL_NONE 
 *                          if no control action)
 * @param companySpecific       the company specific information element included with this info.  Note
 *                              that this includes the two company identifcation octets at the beginning.
 *                              This may be NULL.
 * @param companySpecificSize   the total number of bytes of the company specific information element.
 */
OI_STATUS OI_TCS_CLInfo(OI_BD_ADDR *addr,
                        OI_TCS_AUDIO_CONTROL audioControl,
                        OI_UINT8 *companySpecific,
                        OI_UINT8 companySpecificSize);

/**@}*/

#endif /* _OI_TCS_H */
#endif /* FEATURE_BT_EXTPF_CTP */
