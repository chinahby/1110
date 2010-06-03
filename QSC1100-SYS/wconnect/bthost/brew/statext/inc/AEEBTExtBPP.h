#ifndef _AEE_BT_EXT_BPP_H_
#define _AEE_BT_EXT_BPP_H_

/*===========================================================================
FILE:      AEEBTExtBPP.h

SERVICES:  BlueTooth Basic Pring Profile Interface

GENERAL DESCRIPTION: BREW interface to BT BPP

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtBPP

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEFile.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_BPP     0x010220bb

// MIME media-type formats (Table42 of BPP spec)
#define AEEBT_BPP_DOC_TYPE_XHTML_PRINT      "application/vnd.pwg-xhtml-print+xml:1.0"
#define AEEBT_BPP_DOC_TYPE_BASIC_TEXT       "text/plain"
#define AEEBT_BPP_DOC_TYPE_VCARD            "text/x-vcard"
#define AEEBT_BPP_DOC_TYPE_VCALENDAR        "text/x-vcalendar"
#define AEEBT_BPP_DOC_TYPE_VMESSAGE         "text/x-vmessage"
#define AEEBT_BPP_DOC_TYPE_POSTSCRIPT_2     "application/PostScript:2"
#define AEEBT_BPP_DOC_TYPE_POSTSCRIPT_3     "application/PostScript:3"
#define AEEBT_BPP_DOC_TYPE_PCL5E            "application/vnd.hp-PCL:5E"
#define AEEBT_BPP_DOC_TYPE_PCL3C            "application/vnd.hp-PCL:3C"
#define AEEBT_BPP_DOC_TYPE_PDF              "application/PDF"
#define AEEBT_BPP_DOC_TYPE_JPEG             "image/jpeg"
#define AEEBT_BPP_DOC_TYPE_GIF89A           "image/gif:89A"        

// MIME document formats
enum
{
  AEEBT_BPP_DOC_FORMAT_XHTML_PRINT,
  AEEBT_BPP_DOC_FORMAT_BASIC_TEXT,
  AEEBT_BPP_DOC_FORMAT_VCARD,
  AEEBT_BPP_DOC_FORMAT_VCARD_3_0,
  AEEBT_BPP_DOC_FORMAT_VCALENDAR,
  AEEBT_BPP_DOC_FORMAT_VCALENDAR_2_0,
  AEEBT_BPP_DOC_FORMAT_VMESSAGE,
  AEEBT_BPP_DOC_FORMAT_POSTSCRIPT_2,
  AEEBT_BPP_DOC_FORMAT_POSTSCRIPT_3,
  AEEBT_BPP_DOC_FORMAT_PCL5E,
  AEEBT_BPP_DOC_FORMAT_PCL3C,
  AEEBT_BPP_DOC_FORMAT_PDF,
  AEEBT_BPP_DOC_FORMAT_JPEG,
  AEEBT_BPP_DOC_FORMAT_GIF89A
};

typedef uint16    AEEBTBPPdocType;

// event types returned via HandleEvent()
enum
{
  AEEBT_BPP_EVT_REG_DONE,       
  AEEBT_BPP_EVT_DEREG_DONE,   
  AEEBT_BPP_EVT_CONN_REQ, 
  AEEBT_BPP_EVT_AUTH_REQ,

  AEEBT_BPP_EVT_OP_ABORTED,
  AEEBT_BPP_EVT_SEND_FILE_DONE,
  AEEBT_BPP_EVT_SOAP_REQUEST_DONE,

  AEEBT_BPP_EVT_GET_REF_OBJ_REQ,

  AEEBT_BPP_EVT_CONNECTED, 
  AEEBT_BPP_EVT_CONN_FAILED, 
  AEEBT_BPP_EVT_DISCONNECTED, 
  AEEBT_BPP_EVT_SEC_DISCONNECTED,
  // This event indicates that the current soap request has been answered 
  // and further soap requests can be sent to continuously get the 
  // printing status of the job
  AEEBT_BPP_EVT_SOAP_REQ_DONE_CONTINUE
};

// error codes for AEEBT_BPP_EVT_*
// DEPRECATED - DO NOT ADD ERROR CODES TO THIS ENUMERATION!
enum
{
  // DEPRECATED
  AEEBT_BPP_ERR_NONE          = AEEBT_ERR_NONE,            // no worry
  AEEBT_BPP_ERR_BAD_PARAM     = AEEBT_ERR_BAD_PARAM,       // for any function
  AEEBT_BPP_ERR_RESOURCE      = AEEBT_ERR_RESOURCE,        // lack of resource (memory, ...)
  AEEBT_BPP_ERR_DB_FULL       = AEEBT_ERR_SD_DB_FULL,      // SDDB is full
  AEEBT_BPP_ERR_REC_EXISTS    = AEEBT_ERR_SD_REC_EXISTS,   // server record already in SDDB
  AEEBT_BPP_ERR_REC_NOT_FOUND = AEEBT_ERR_SD_REC_NOT_FOUND,// server record not found in SDDB
  AEEBT_BPP_ERR_FAILED        = AEEBT_ERR_UNKNOWN          // general failure
  // DEPRECATED
};

// BPP Authentication types
enum
{
  AEEBT_BPP_AUTH_NONE,
  AEEBT_BPP_AUTH_PASSWORD,
  AEEBT_BPP_AUTH_USERID_AND_PASSWORD
};
typedef uint8 AEEBTBPPAuthType;

typedef struct
{
  uint16           AEEBTBPPdataLen;
  byte*            AEEBTBPPdata;
} AEEBTBPPbyteSeqType;

typedef struct _IBTExtBPP IBTExtBPP;

AEEINTERFACE(IBTExtBPP)
{
  INHERIT_IQueryInterface(IBTExtBPP);

  int (*Register)                 (IBTExtBPP*          po,
                                   const char*         pSrvName);
  int (*Deregister)               (IBTExtBPP*          po);

  int (*Authenticate)             (IBTExtBPP*          po, 
                                   AEEBTBPPTargetType  target,
                                   const AECHAR*       pUserID, 
                                   const AECHAR*       pPassword);

  int (*AcceptConnection)         (IBTExtBPP*          po);
  int (*Connect)                  (IBTExtBPP*          po, 
                                   const AEEBTBDAddr*  pBDAddr, 
                                   uint8               uChannelNumber,
                                   AEEBTBPPTargetType  target);
  int (*Disconnect)               (IBTExtBPP*          po,
                                   AEEBTBPPTargetType  target);
  int (*Abort)                    (IBTExtBPP*          po);

  int (*SendFile)                 (IBTExtBPP*          po,
                                   AEEBTBPPdocType     documentType,
                                   const char*         pFileName,
                                   const AECHAR*       pDescriptor,
                                   uint32              jobID);
  int (*SOAPRequest)              (IBTExtBPP*          po,
                                   const char*         pSndrSOAPmsg,
                                   char*               pSOAPmsg,
                                   uint16              SOAPmsgMaxLen,
                                   AEEBTBPPTargetType  target);

  int (*GetRefObjResponse)        (IBTExtBPP*          po,
                                   const char*         pFileName,
                                   uint32              offset,
                                   int32               count);
};

#define IBTEXTBPP_AddRef(p)                               \
        AEEGETPVTBL((p),IBTExtBPP)->AddRef((p))

#define IBTEXTBPP_Release(p)                              \
        AEEGETPVTBL((p),IBTExtBPP)->Release((p))

#define IBTEXTBPP_QueryInterface(p,i,ppo)                 \
        AEEGETPVTBL((p),IBTExtBPP)->QueryInterface((p),(i),(ppo))

#define IBTEXTBPP_Register(p,pn)                        \
        AEEGETPVTBL((p),IBTExtBPP)->Register((p),(pn))
        
#define IBTEXTBPP_Deregister(p)                           \
        AEEGETPVTBL((p),IBTExtBPP)->Deregister((p))
        
#define IBTEXTBPP_Authenticate(p,t,pu,pp)                           \
        AEEGETPVTBL((p),IBTExtBPP)->Authenticate((p),(t),(pu),(pp))

#define IBTEXTBPP_AcceptConnection(p)                           \
        AEEGETPVTBL((p),IBTExtBPP)->AcceptConnection((p))
        
#define IBTEXTBPP_Connect(p,pa,c,t)                         \
        AEEGETPVTBL((p),IBTExtBPP)->Connect((p),(pa),(c),(t))

#define IBTEXTBPP_Disconnect(p,t)                           \
        AEEGETPVTBL((p),IBTExtBPP)->Disconnect((p),(t))
        
#define IBTEXTBPP_Abort(p)                                \
        AEEGETPVTBL((p),IBTExtBPP)->Abort((p))
        
#define IBTEXTBPP_SendFile(p,t,pd,pf,i)                    \
        AEEGETPVTBL((p),IBTExtBPP)->SendFile((p),(t),(pd),(pf),(i))
        
#define IBTEXTBPP_SOAPRequest(p,pss,pps,psm,t)         \
        AEEGETPVTBL((p),IBTExtBPP)->SOAPRequest((p),(pss),(pps),(psm),(t))
        
#define IBTEXTBPP_GetRefObjResponse(p,pf,o,c)         \
        AEEGETPVTBL((p),IBTExtBPP)->GetRefObjResponse((p),(pf),(o),(c))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtBPP

Description: BlueTooth Basic Printing Profile

  Consult the Bluetooth ISOD for description of BPP.
  
  The use of IBTExtBPP consists of sending commands and receiving events.  
  These events are delivered to IBTExtBPP clients as notifications, hence
  the clients must register for BPP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_BPP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_BPP_EVT_* events when the notification mask
  is NMASK_BT_BPP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                   pData->
  -----                                   ----------------------------
  AEEBT_BPP_EVT_CONN_REQ                  bdAddr - BD addr of client
  AEEBT_BPP_EVT_AUTH_REQ                  BppObject
  AEEBT_BPP_EVT_REG_DONE                  uError
  AEEBT_BPP_EVT_DEREG_DONE                uError

  AEEBT_BPP_EVT_OP_ABORTED                uError
  AEEBT_BPP_EVT_SEND_FILE_DONE            uError
  AEEBT_BPP_EVT_SOAP_REQUEST_DONE         uError


  AEEBT_BPP_EVT_CONNECTED                 bdAddr - BD addr of remote device
  AEEBT_BPP_EVT_CONN_FAILED               bdAddr
  AEEBT_BPP_EVT_DISCONNECTED              bdAddr

Interfaces:

  IBTEXTBPP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTBPP_AddRef()
    IBTEXTBPP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTBPP_QueryInterface()
    
=======================================================================

Function:  IBTEXTBPP_Register()

Description:
  Server application uses this command to initialize the BPP server and 
  registers it in the service discovery database (SDDB.)

Prototype:
  int IBTEXTBPP_Register(
                          IBTExtBPP*     po,
                          const char*    pSrvName);

Parameters:
  po                    - pointer to the IBTExtBPP object
  pSrvName              - service name to store in SDDB; must not be NULL

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_BPP_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_BPP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_BPP_ERR_NONE
    AEEBT_BPP_ERR_REC_EXISTS

=======================================================================

Function:  IBTEXTBPP_Deregister()

Description: 
  Terminates the Basic Printing Profile server and deregisters it 
  from the SDDB.

Prototype:
  int IBTEXTBPP_Deregister( IBTExtBPP* po );

Parameters:
  po                    - pointer to the IBTExtBPP object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_BPP_EVT_DEREG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_BPP_EVT_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_BPP_ERR_NONE
    AEEBT_BPP_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTBPP_Authenticate()

Description: 
  Sender calls this command to authenticate before establishing a connection
  with the Printer.

Prototype:
  int IBTEXTBPP_Authenticate( IBTExtBPP*          po,
                              AEEBTBPPTargetType  target,
                              const AEECHAR*      pUserID,
                              const AEECHAR*      pPassword );

Parameters:
  po                    - pointer to the IBTExtBPP object
  target                - whether DPS or STS channel
  pUserID               - pointer to userID string
  pPassword             - pointer to password string

Return value:
  SUCCESS   - the request to authenticate was queued and that 
              AEEBT_BPP_EVT_CONNNECTED event will be generated at a 
              later time, if the authentication was sucessful.  Else,
              AEEBT_BPP_EVT_CONN_FAILED event will be generated.
              
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_BPP_EVT_CONNECTED or 
  AEEBT_BPP_EVT_CONN_FAILED

=======================================================================

Function:  IBTEXTBPP_AcceptConnection()

Description: 
  The server application must issue this command to accept a 
  connection request from a client. 

Prototype:
  int IBTEXTBPP_AcceptConnection( IBTExtBPP* po );

Parameters:
  po                    - pointer to the IBTExtBPP object

Return value:
  SUCCESS   - the request to accept the connection request was queued
              and that AEEBT_BPP_EVT_CONNECTED event will be generated at a 
              later time (if the server accepted the connection.)
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_BPP_EVT_CONNECTED with the accompanying error code AEEBT_BPP_ERR_NONE.

=======================================================================

Function:  IBTEXTBPP_Connect()

Description:
  Called by client application to connect to remote BPP server.

Prototype:

  int IBTEXTBPP_Connect(
                          IBTExtBPP*         po,
                          const AEEBTBDAddr* pBDAddr, 
                          uint8              uChannelNumber,
                          AEEBTBPPTargetType target);

Parameters:

  po              - pointer to the IBTExtBPP object
  pBDAddr         - BD address of BPP server
  uChannelNumber  - the server channel number to use to establish connection
  target          - DPS, STS, PBR etc.

Return value:
  SUCCESS   - the request to connect to BPP server was queued and that
              AEEBT_BPP_EVT_CONNECTED or AEEBT_BPP_EVT_CONN_FAILED event will 
              be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Event:
  AEEBT_BPP_EVT_CONNECTED or
  AEEBT_BPP_EVT_CONN_FAILED

=======================================================================

Function:  IBTEXTBPP_Disconnect()

Description:
  Client can call this interface to terminate the OBEX connection
  with the server.

Prototype:
  int IBTEXTBPP_Disconnect( IBTExtBPP*         po,
                            AEEBTBPPTargetType target );

Parameters:
  po          - pointer to the IBTExtBPP object
  target      - Job or Status channel.

Return value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEEBT_BPP_EVT_DISCONNECTED event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_BPP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_BPP_ERR_NONE
    AEEBT_BPP_ERR_NO_CONN

=======================================================================

Function:  IBTEXTBPP_Abort()

Description:
  Client application can issue this command to abort the current operation.

Prototype:
  int IBTEXTBPP_Abort( IBTExtBPP* po );

Parameters:
  po              - pointer to the IBTExtBPP object

Return value:
  SUCCESS   - the current operation has been aborted.
  EIDLE     - no operation in progress
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTBPP_SendFile()

Description:
  Client issues this command to send a file for printing.  

Prototype:
  int IBTEXTBPP_SendFile( 
                          IBTExtBPP*       po,
                          AEEBTBPPdocType  documentType,
                          const char*      pFileName,
                          const AECHAR*    pDescriptor,
                          uint32           jobID);

Parameters:
  po              - pointer to the IBTExtBPP object
  documentType    - MIME Media Type version
  pFileName       - name of the file to be printed
  pDescription    - optional document type information.  For example,
                    when sending vCard data to the printer, the 
                    description field can contain layout information.
                    
                    Syntax:
                    <Parameter Name>Parameter Value</Parameter Name>
                    
                    Example:
                    <CardsPerPage>16</CardsPerPage>
                    
                    The printer will interpret the document type before
                    examining the contents of the description field.
  
                    Refer to sections 10.4.1 and 10.5.1 in the spec for 
                    more information.
                    
  jobID           - JobID (use 0 for Simple Push model)

Return value:
  SUCCESS   - the request to send a file to the BPP server has been queued
              and that AEEBT_BPP_EVT_SEND_FILE_DONE event will be generated 
              at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_BPP_EVT_SEND_FILE_DONE

=======================================================================

Function:  IBTEXTBPP_SOAPRequest()

Description:
  Client issues this command to send a SOAP message to the BPP server.
  The BPP server response SOAP message will be supplied in the user
  provided buffer.
  
  The sender SOAP message can be any of the following:
  
  GetPrinterAttributes: This SOAP action can be used to query printer
  status and capabilities
  
  CreateJob: This SOAP action may be used to submit the job attributes
  for the print job.  The allocated jobId will be returned in the 
  response SOAP message.
  
  SendDocument: This action is used to transfer the actual print content
  of a print job to the printer.  It shall be used in conjunction with a
  Createjob action.
  
  GetJobAttributes: This SOAP action may be used to query specific attributes
  and status of a specific job.
  
  CancelJob: This SOAP action may be used to cancel a job using the JobId.
  
  GetEvent: This SOAP action may be used by a sender to query status from
  the Printer over the Status Channel while a print job is being sent
  on the Job Channel.
  
  CreatePreciseJob: This SOAP action may be used by a sender as part of
  the enchanced layout features for the job-based usage model.
  
  GetMargins: This SOAP action may be used by a Sender as part of the
  enchanced layout features for the job-based usage model.
  
  The following is a sample SOAP message for CreateJob request.  For a
  more detailed information please consult BPP specification document.
  
  CONTENT-LENGTH: bytes in body
  CONTENT-LANGUAGE: en-US
  CONTENT-TYPE: text/xml; charset="utf-8"
  SOAPACTION: "urn:schemas-bluetooth-org:service:Printer:1#CreateJob"
  <s:Envelope
  xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"
  s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body>
  <u:CreateJob xmlns:u="urn:schemas-bluetooth-org:service:Printer:1">
  <JobName> MyJob</JobName>
  <JobOriginatingUserName>MyName</JobOriginatingUserName>
  <DocumentFormat>application/PostScript:3</DocumentFormat>
  <Copies>1</Copies>
  <Sides>one-sided</Sides>
  <NumberUp>1</NumberUp>
  <OrientationRequested>portrait</OrientationRequested>
  <MediaSize> iso_a4_210x297mm</MediaSize>
  <MediaType>cardstock</MediaType>
  <PrintQuality>normal</PrintQuality>
  <CancelOnLostLink>true</CancelOnLostLink>
  </u:CreateJob>
  </s:Body>
  </s:Envelope>
  
Prototype:
  int IBTEXTBPP_SOAPRequest( 
                          IBTExtBPP*         po,
                          const char*        pSndrSOAPmsg,
                          char*              pSOAPmsg,
                          uint16             SOAPmsgMaxLen,
                          AEEBTBPPTargetType target);

Parameters:

  po              - pointer to the IBTExtBPP object
  pSndrSOAPmsg    - Sender SOAP message string.  Must be null terminated.
  pSOAPmsg        - User provided buffer in which the SOAP message response
                    of BPP server is returned.  Will be null terminated.
  SOAPmsgMaxLen   - Size of the user provided buffer
  target          - Which channel to send the SOAP request.
                    DPS (job channel) or STS (status channel)

Return value:
  SUCCESS   - the request to send a SOAP request has been queued
              and that AEEBT_BPP_EVT_SOAP_REQUEST_DONE event will be generated 
              at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BPP_EVT_SOAP_REQUEST_DONE
  
=======================================================================

Function:  IBTEXTBPP_GetRefObjResponse()

Description:
  Response to GetReferenceObject request from the printer.
  
Prototype:
  int IBTEXTBPP_GetRefObjRequest( 
                          IBTExtBPP*     po,
                          const char*    pFileName,
                          uint32         offset,
                          int32          count);

Parameters:

  po              - pointer to the IBTExtBPP object
  pFileName       - reference object name
  offset          - byte offet into the print or image object.  The
                    first byte of the image is byte 0.  offset shall
                    be >= 0
  count           - the number of bytes to send.  If count = -1, the
                    reminder of the print or image (starting from offset) 
                    will be returned.  count shall be >= -1

Return value:
  SUCCESS   - the response message has been queued successfully
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BPP_EVT_GET_REF_OBJ_REQ
  
=======================================================================*/
#endif /* _AEE_BT_EXT_BPP_H_ */
