#ifndef AEEGSM1xSIG_H
#define AEEGSM1xSIG_H
/*===========================================================================
FILE: AEEGSM1xSig.h

SERVICES: GSM1x Signaling interface.

GENERAL DESCRIPTION
   AEE GSM1x Signaling Interfaces (Base level definitions, typedefs, etc.)


        Copyright © 2001-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"


/*===========================================================================

                       CONSTANTS and ENUMs

===========================================================================*/

#define AEECLSID_IGSM1XSIG              0x01011cca
#define AEECLSID_IGSM1XSIGNOTIFIER      0x0101217d
                     
                     
// Notification Masks
//                   
#define NMASK_GSM1xSIG_PROTOCOL_TYPE         0x0001         // P_TYPE specified in value
                                                            // App will receive the specified
                                                            // P_TYPE Signaling or Reject msg in
                                                            // *AEEGSM1xSig_NotifyMessageType

// Max values for GSM1x Signaling payloads
//                                                                                                
                                        

#define GSM1xSIG_SIGNALING_MAX      222     // A GSM1x Msg with this length can
                                            // only be sent on Traffic channel.
                                            // L2 Fields          = 3 Bytes
                                            // DBM Hdr            = 4 Bytes 
                                            // SMS Type           = 1 Byte
                                            // Tele-Service TLV   = 4 Bytes
                                            // Addr TLV           = 5 Bytes
                                            // Bearer Data Hdr    = 2 Bytes
                                            // User sub-param Hdr = 2 Bytes
                                            // Protocol Type      = 1 Byte

#define GSM1xSIG_REJECT_MAX         GSM1xSIG_SIGNALING_MAX - 2


// Errors. We use the values from AEEError.h, but we have added the following

#define EGSM1x_INACTIVE   100


// Enum definitions
//                                                                    
typedef enum {

    GSM1xSIG_ACTIVE,
    GSM1xSIG_INACTIVE

} AEEGSM1xSig_Status;


// Signaling Protocol Types
// The following defines specify the possible values for protocol
// types.
//
#define GSM1xSIG_PTYPE_SIGNAL    0
#define GSM1xSIG_PTYPE_CONFIG    1
#define GSM1xSIG_PTYPE_RIL3      2
#define GSM1xSIG_PTYPE_EXT_RIL3  3


// Signaling types for Extended RIL3
//
#define GSM1xSIG_RIL3_EXT_GSM_TYPE      0    // RIL3 payload
#define GSM1xSIG_RIL3_EXT_REF_NUM_TYPE  1    // Authentication reference number



// Signaling types for RIL3 Reject message
//
#define GSM1xSIG_RIL3_REJECT_FORMAT          0    // Unsupported protocol type
#define GSM1xSIG_RIL3_REJECT_DISCRIMINATOR   1    // Unsupported discriminator
                                                  //  use with RIL3 & Ext RIL3 msg
#define GSM1xSIG_RIL3_REJECT_MSG_TYPE        2    // Unsupported message type
                                                  //  use with RIL3 & Ext RIL3 msg

typedef enum {

    GSM1xSIG_SIGNALING,
    GSM1xSIG_REJECT

} AEEGSM1xSig_NotifyMessageTypeEnum;




/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


// The following struct holds the received GSM1x Signaling Msg via 
// notification. Same struct is also used to send out msg to network 
// with the SendSignalingMessage method.
//
typedef struct {

    byte ProtocolRevision;       // This is a 4 bit value 
    byte ProtocolType;           // This is a 4 bit value
    byte ProtocolDataLen;
    byte ProtocolData[GSM1xSIG_SIGNALING_MAX];

} AEEGSM1xSig_SignalingMessageType;


// Struct used to send a Reject Msg to Network with the 
// SendSignalingReject method.
//
typedef struct {

    byte ProtocolTypeRejected;     // This is a 4 bit value
    byte RejectCauseLength;
    byte RejectCause[GSM1xSIG_REJECT_MAX];

} AEEGSM1xSig_RejectMessageType;

// This struct is used to send the payload along with the notify
// event for NMASK_GSM1x_PROTOCOL_TYPE.
//
typedef struct {
    
    int messageType;

    union {
        AEEGSM1xSig_SignalingMessageType signalMessage;
        AEEGSM1xSig_RejectMessageType    rejectMessage;
    } msg;

} AEEGSM1xSig_NotifyMessageType;



/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

typedef struct IGSM1xSigNotifier IGSM1xSigNotifier;

AEEINTERFACE(IGSM1xSigNotifier)
{
    INHERIT_INotifier(IGSM1xSigNotifier);
};


typedef struct IGSM1xSig IGSM1xSig;

AEEINTERFACE(IGSM1xSig)
{
    INHERIT_IQueryInterface(IGSM1xSig);
                
    AEEGSM1xSig_Status (*GetStatus)(IGSM1xSig *po);

    int  (*SendSignalingMessage)(IGSM1xSig *po, 
                                 const AEEGSM1xSig_SignalingMessageType* pMsg);
    int  (*SendSignalingReject) (IGSM1xSig *po, 
                                 const AEEGSM1xSig_RejectMessageType* pMsg);
};


//-------------------------
// IGSM1xSigNotifier API's
//-------------------------
#define IGSM1xSigNotifier_AddRef(p)               AEEGETPVTBL(p,IGSM1xSigNotifier)->AddRef(p)
#define IGSM1xSigNotifier_Release(p)              AEEGETPVTBL(p,IGSM1xSigNotifier)->Release(p)
#define IGSM1xSigNotifier_SetMask(p, pdwM)        AEEGETPVTBL(p,IGSM1xSigNotifier)->SetMask(p, pdwM)

//---------------------
// IGSM1xSig API's
//---------------------
#define IGSM1xSig_AddRef(p)                       AEEGETPVTBL(p,IGSM1xSig)->AddRef(p)
#define IGSM1xSig_Release(p)                      AEEGETPVTBL(p,IGSM1xSig)->Release(p)

// QueryInterface
#define IGSM1xSig_QueryInterface(p,i,p2)          AEEGETPVTBL(p,IGSM1xSig)->QueryInterface(p,i,p2)

// GSM1xSig Status APIs 
#define IGSM1xSig_GetStatus(p)                    AEEGETPVTBL(p,IGSM1xSig)->GetStatus(p)

// GSM1xSig Signaling APIs
#define IGSM1xSig_SendSignalingMessage(p,pMsg)    AEEGETPVTBL(p,IGSM1xSig)->SendSignalingMessage(p,pMsg)
#define IGSM1xSig_SendSignalingReject(p,pMsg)     AEEGETPVTBL(p,IGSM1xSig)->SendSignalingReject(p,pMsg)


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEEGSM1xSig_Status

Description:

AEEGSM1xSig_Status is sent as dwParam member of the EVT_NOTIFY event when
app register for NMASK_GSM1xSIG_STATUS_CHANGE notification. This enum is 
also returned when Apps call IGSM1xSig_GetStatus().

typedef enum {

        GSM1xSIG_ACTIVE,
        GSM1xSIG_INACTIVE

} AEEGSM1xSig_Status;


Members:
 
   GSM1xSIG_ACTIVE   - GSM1x is enabled on the phone
   GSM1xSIG_INACTIVE - GSM1x is disabled on the phone
   
Comments:
None

See Also:
None

=======================================================================

AEEGSM1xSig_NotifyMessageType

Description:

A pointer to  AEEGSM1xSig_NotifyMessageType struct is sent as dwParam member 
of the EVT_NOTIFY event when app register for NMASK_GSM1xSIG_PROTOCOL_TYPE 
notification. 

typedef struct {
    
    AEEGSM1xSig_NotifyMessageTypeEnum messageType;

    union {
        AEEGSM1xSig_SignalingMessageType *signalMessage;
        AEEGSM1xSig_RejecMessageType     *rejectMessage;
    } msg;

} AEEGSM1xSig_NotifyMessageType;


Members:
  
   messageType       - Indicates the type of message viz. Signaling or Reject

   msg.signalMesage  - Contains the signaling payload.
   msg.rejectMessage - Contains the reject payload.
   
Comments:
None

See Also:
None


=======================================================================

AEEGSM1xSig_SignalingMessageType
   
Description: 

AEEGSM1xSig_SignalingMessageType is used to send out a GSM1x Signaling 
Message using IGSM1xSig_SendSignalingMessage()

Definition:

typedef struct {

        byte ProtocolRevision;   // This is a 4 bit value 
        byte ProtocolType;       // This is a 4 bit value
        byte ProtocolDataLen;
        byte ProtocolData[GSM1xSIG_SIGNALING_MAX];

} AEEGSM1xSig_SignalingMessageType;

Members:

   ProtocolRevision   - 4 bit field specifiying the Protocol Revision of GSM1x Sig Msg
   ProtocolType       - 4 bit field specifying the Protocol Type of GSM1x Sig Msg
   ProtocolDataLength - Length of the protocl type specific field length
   ProtocolData       - Array holding the Protocol Type Data
   
Comments:
None

See Also:
None

=======================================================================
   
AEEGSM1xSig_RejectMessageType

Description:
                                                 
AEEGSM1xSig_RejectMessageType is used to send out a GSM1x Signaling Reject
message to the network using IGSM1xSig_SendSignalingReject(). 

Definition:

typedef struct {

        byte ProtocolTypeRejected;   // This is a 4 bit value 
        byte RejectCauseLength;
        byte RejectCause[GSM1xSIG_REJECT_MAX];

} AEEGSM1xSig_RejectMessageType;

Members:

   ProtocolTypeRejected - Protocol Type that is rejected
   RejectCauseLength    - Length of the Reject Cause fields
   RejectCause          - Holds the Reject Cause information
   
Comments:
None

See Also:
None

=======================================================================

=====================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IGSM1xSig

Description: 
   IGSM1xSig inteface enables GSM1x capability on the mobile. It provides
   a simple interface to the GSM1x Signaling layer. It also provides a 
   method to check the status of the GSM1x capability.
   
It provides the following services:

- Send GSM1x Signaling Msg to network
- Notification upon receiving GSM1x Signaling Msg from Network
- Check status of GSM1x. 

The IGSM1xSig interface is obtained via the ISHELL_CreateInstance 
mechanism.

To send event notifications, IGSM1xSig uses a helper class 
IGSM1xSigNotifier. Methods for IGSM1xSigNotifier should not be called 
directly by BREW applets. Brew applet should specify the class id for 
IGSM1xSigNotifier and the NMASK in its MIF file.

Only one type of event notification is provided:
   - NMASK_GSM1xSIG_PROTOCOL_TYPE
     This event is send whenever a GSM1x signaling message is received. 
     While registering applet should specify the value indicating the 
     protocol type applet is interested in. An applet will only receive 
     signaling messages for the protocol types it has registered for. 
     A pointer to AEEGSM1xSig_NotifyMessageType struct is sent as dwParam
     member of the EVT_NOTIFY event. 
     In general if an applet is interested in messages for protocol type
     "x", it should specify a NMASK of (NMASK_GSM1xSIG_PROTOCOL_TYPE | x).
     Possible values for protocol type are specified in enum type 
     AEEGSM1xSig_ProtocolTypes.

See Also:
   None.

======================================================================= 

Function: IGSM1xSig_GetStatus()

Description:
   This method is used to retrieve the current GSM1x status on the 
   phone. This method lets the caller know if GSM1x capability is
   enabled or disabled on the mobile.
   
Prototype:

   AEEGSM1xSig_Status IGSM1xSig_GetStatus(IGSM1x * po);
   
Parameters:
   po: Pointer to the IGSM1xSig object
   
Return Value:
   GSM1xSIG_ACTIVE   - GSM1x capability enabled on the mobile
   GSM1xSIG_INACTIVE - GSM1x capability disabled on the mobile
   
Comments:
   None.
   
Side Effects:
   None.
   
See Also:
   IGSM1xControl
======================================================================= 

Function: IGSM1xSig_SendSignalingMessage()

Description:
   This method is used to send a GSM1x signaling message (except for
   GSM1x Authentication Req/Rsp Message) to the network.
   
Prototype:

   int IGSM1xSig_SendSignalingMessage(IGSM1xSig *po, AEEGSM1xSig_SignalingMessageType *pMsg);
   
Parameters:
   po: Pointer to the IGSM1xSig object
   pMsg: Pointer to the AEEGSM1xSig_SignalingMessageType struct containing the
         GSM1x signaling msg details 
                                                                                           
Return Value:
   SUCCESS         - GSM1x Signaling Message queued up.
   EBADPARM        - Value in pMsg is invalid (ex. attempting to send Auth msg)
   EFAILED         - General Failure in sending out the signaling message
   EITEMBUSY       - No more buffers left for sending messages.
   EGSM1x_INACTIVE - Phone is not in IGSM1x mode
   EBADCLASS:      - IPhone is not initialized.

Comments:  
   This method doesn't guarantee deilvery of the message to the network, 
   it only ensures that message was sent from the mobile. It is up to the
   calling routines to use other mechanism (ex. explicit ack) to detemine 
   if the network entity received the message

Side Effects: 
   None

See Also: 
   None
======================================================================= 

Function:    IGSM1xSig_SendSignalingReject()

Description:
   This method is used to send a GSM1x signaling reject message to
   the network.
   
Prototype:

   int IGSM1xSig_SendSignalingReject(IGSM1xSig *po, AEEGSM1xSig_RejectMessageType *pMsg);
   
Parameters:
   po: Pointer to the IGSM1xSig object
   pMsg: Pointer to the AEEGSM1xSig_RejectMessageType struct containing the
         GSM1x signaling reject msg
                 
Return Value:                 
   SUCCESS         - GSM1x Reject Message queued up.
   EBADPARM        - Value in pMsg is invalid (ex. attempting to send Auth msg)
   EFAILED         - General Failure in sending out the signaling message
   EITEMBUSY       - No more buffers left for sending messages.
   EGSM1x_INACTIVE - Phone is not in IGSM1x mode
   EBADCLASS:      - IPhone is not initialized.

Comments:  
   This method doesn't guarantee deilvery of reject message to the network.
   
Side Effects:
   None
   
See Also
   None
=======================================================================*/


#endif    // AEEGSM1xSIG_H      
