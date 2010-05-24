#ifndef _AEE_BT_EXT_PBAP_H_
#define _AEE_BT_EXT_PBAP_H_

/*===========================================================================
FILE:      AEEBTExtPBAP.h

SERVICES:  BlueTooth Phone Book Access Profile Interface

GENERAL DESCRIPTION: BREW interface to BT PBAP

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtPBAP

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEFile.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_PBAP         0x0103c65c

// event types returned via HandleEvent()
enum
{
  /* for servers */
  AEEBT_PBAP_EVT_SRV_REG_DONE,       // registration process done
  AEEBT_PBAP_EVT_SRV_DEREG_DONE,     // deregistration process done
  AEEBT_PBAP_EVT_CONN_REQ,           // a client wants to connect

  AEEBT_PBAP_EVT_PULL_PHONE_BOOK_REQ,
  AEEBT_PBAP_EVT_SET_PHONE_BOOK_REQ,
  AEEBT_PBAP_EVT_PULL_VCARD_LISTING_REQ,
  AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_REQ,

  /* for clients */
  AEEBT_PBAP_EVT_CLI_REG_DONE,       // registration process done
  AEEBT_PBAP_EVT_CLI_DEREG_DONE,     // deregistration process done
  AEEBT_PBAP_EVT_OP_ABORTED,         // current operation aborted

  /* common to servers and clients */
  AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE,
  AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE,
  AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE,
  AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE,

  AEEBT_PBAP_EVT_CONNECTED, 
  AEEBT_PBAP_EVT_CONN_FAILED, 
  AEEBT_PBAP_EVT_DISCONNECTED,
  AEEBT_PBAP_EVT_AUTH_REQ
};

typedef struct _IBTExtPBAP IBTExtPBAP;

AEEINTERFACE(IBTExtPBAP)
{
  INHERIT_IQueryInterface(IBTExtPBAP);

  /* for server */
  int (*RegisterServer)           (IBTExtPBAP*              po,
                                   const char*              pSrvName,
                                   AEEBTPBAPRepositories    supRepositories,
                                   AEEBTPBAPAuthentication  auth);

  int (*DeregisterServer)         (IBTExtPBAP*              po);
  int (*AcceptConnection)         (IBTExtPBAP*              po,
                                   boolean                  bAccept);

  /* for client and server */
  int (*Authenticate)             (IBTExtPBAP*              po,
                                   const AECHAR*            pwUserID, 
                                   const AECHAR*            pwPIN);

  /* for client */
  int (*Connect)                  (IBTExtPBAP*              po, 
                                   const AEEBTBDAddr*       pBDAddr, 
                                   uint8                    uChannelNumber,
                                   uint8                    bauth);
  int (*Disconnect)               (IBTExtPBAP*              po);

  int (*RegisterClient)           (IBTExtPBAP*              po,
                                   const char*              pCliName);
  int (*DeregisterClient)         (IBTExtPBAP*              po);

  int (*Abort)                    (IBTExtPBAP*              po);

  int (*PullPhoneBook)            (IBTExtPBAP*              po,
                                   const AECHAR*            pwObjName,
                                   const AEEBTPBAPAttribMask*  pAttribMask,
                                   const AEEBTPBAPFormat*   pFormat,
                                   const uint16*            pMaxListCount,
                                   const uint16*            pListStartOffset);
  int (*SetPhoneBook)             (IBTExtPBAP*              po,
                                   const AECHAR*            pwFolder,
                                   AEEBTPBAPSetPathFlag     path);
  int (*PullvCardListing)         (IBTExtPBAP*              po,
                                   const AECHAR*            pwFolder,
                                   const AEEBTPBAPOrder*    pSortOrder,
                                   const AECHAR*            pwSearchValue,
                                   const AEEBTPBAPSearchAttrib* pSearchAttrib,
                                   const uint16*            pMaxListCount,
                                   const uint16*            pListStartOffset);
  int (*PullvCardEntry)           (IBTExtPBAP*              po,
                                   const AECHAR*            pwName,
                                   const AEEBTPBAPAttribMask* pAttribMask,
                                   const AEEBTPBAPFormat*   pFormat);

  /* for server */
  int (*PullPhoneBookResponse)    (IBTExtPBAP*              po,
                                   const AECHAR*            pwFileName,
                                   const uint16*            pPhoneBookSize,
                                   const uint8*             pNewMissedCalls,
                                   AEEBTError               error);
  int (*SetPhoneBookResponse)     (IBTExtPBAP*              po,
                                   AEEBTError               error);
  int (*PullvCardListingResponse) (IBTExtPBAP*              po,
                                   const AECHAR*            pwFileName,
                                   const uint16*            pPhoneBookSize,
                                   const uint8*             pNewMissedCalls,
                                   AEEBTError               error);
  int (*PullvCardEntryResponse)   (IBTExtPBAP*              po,
                                   const AECHAR*            pwFileName,
                                   AEEBTError               error);
}; 

#define IBTEXTPBAP_AddRef(p)                               \
        AEEGETPVTBL((p),IBTExtPBAP)->AddRef((p))

#define IBTEXTPBAP_Release(p)                              \
        AEEGETPVTBL((p),IBTExtPBAP)->Release((p))

#define IBTEXTPBAP_QueryInterface(p,i,ppo)                 \
        AEEGETPVTBL((p),IBTExtPBAP)->QueryInterface((p),(i),(ppo))

#define IBTEXTPBAP_RegisterServer(p,pn,r,a)                        \
        AEEGETPVTBL((p),IBTExtPBAP)->RegisterServer((p),(pn),(r),(a))

#define IBTEXTPBAP_DeregisterServer(p)                           \
        AEEGETPVTBL((p),IBTExtPBAP)->DeregisterServer((p))

#define IBTEXTPBAP_AcceptConnection(p,a)            \
        AEEGETPVTBL((p),IBTExtPBAP)->AcceptConnection((p),(a))
        
#define IBTEXTPBAP_Authenticate(p,pu,pp)            \
        AEEGETPVTBL((p),IBTExtPBAP)->Authenticate((p),(pu),(pp))

#define IBTEXTPBAP_Connect(p,pa,c,a)                         \
        AEEGETPVTBL((p),IBTExtPBAP)->Connect((p),(pa),(c),(a))

#define IBTEXTPBAP_Disconnect(p)                           \
        AEEGETPVTBL((p),IBTExtPBAP)->Disconnect((p))
        
#define IBTEXTPBAP_RegisterClient(p,pn)                        \
        AEEGETPVTBL((p),IBTExtPBAP)->RegisterClient((p),(pn))

#define IBTEXTPBAP_DeregisterClient(p)                           \
        AEEGETPVTBL((p),IBTExtPBAP)->DeregisterClient((p))
  
#define IBTEXTPBAP_Abort(p)                                \
        AEEGETPVTBL((p),IBTExtPBAP)->Abort((p))
    
#define IBTEXTPBAP_PullPhoneBook(p,n,a,f,pc,po)                  \
        AEEGETPVTBL((p),IBTExtPBAP)->PullPhoneBook((p),(n),(a),(f),(pc),(po))
       
#define IBTEXTPBAP_SetPhoneBook(p,f,pa)                      \
        AEEGETPVTBL((p),IBTExtPBAP)->SetPhoneBook((p),(f),(pa))
        
#define IBTEXTPBAP_PullvCardListing(p,f,o,v,pa,pc,po)         \
        AEEGETPVTBL((p),IBTExtPBAP)->PullvCardListing((p),(f),(o),(v),(pa),(pc),(po))
        
#define IBTEXTPBAP_PullvCardEntry(p,pn,a,f)                      \
        AEEGETPVTBL((p),IBTExtPBAP)->PullvCardEntry((p),(pn),(a),(f))
        
#define IBTEXTPBAP_PullPhoneBookResponse(p,pf,ps,pc,e)                  \
        AEEGETPVTBL((p),IBTExtPBAP)->PullPhoneBookResponse((p),(pf),(ps),(pc),(e))
        
#define IBTEXTPBAP_SetPhoneBookResponse(p,e)                  \
        AEEGETPVTBL((p),IBTExtPBAP)->SetPhoneBookResponse((p),(e))
        
#define IBTEXTPBAP_PullvCardListingResponse(p,pf,ps,pc,e)                  \
        AEEGETPVTBL((p),IBTExtPBAP)->PullvCardListingResponse((p),(pf),(ps),(pc),(e))
        
#define IBTEXTPBAP_PullvCardEntryResponse(p,pf,e)                  \
        AEEGETPVTBL((p),IBTExtPBAP)->PullvCardEntryResponse((p),(pf),(e))


/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtPBAP

Description: BlueTooth Phone Book Access Profile

  Consult the Bluetooth ISOD for description of PBAP.
  
  The use of IBTExtPBAP consists of sending commands and receiving events.  
  These events are delivered to IBTExtPBAP clients as notifications, hence
  the clients must register for PBAP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_PBAP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_PBAP_EVT_* events when the notification mask
  is NMASK_BT_PBAP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                                  pData->
  -----                                                  -------------
  
  // Server Events
  AEEBT_PBAP_EVT_CONN_REQ                                 bdAddr 
  AEEBT_PBAP_EVT_SRV_REG_DONE                             uError
  AEEBT_PBAP_EVT_SRV_DEREG_DONE                           uError
  
  AEEBT_PBAP_EVT_PULL_PHONE_BOOK_REQ                      PBAPObject
  AEEBT_PBAP_EVT_SET_PHONE_BOOK_REQ                       PBAPObject
  AEEBT_PBAP_EVT_PULL_VCARD_LISTING_REQ                   PBAPObject
  AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_REQ                     PBAPObject
  
  // Client Events
  AEEBT_PBAP_EVT_OP_ABORTED                               uError

  AEEBT_PBAP_EVT_CLI_REG_DONE                             uError
  AEEBT_PBAP_EVT_CLI_DEREG_DONE                           uError
  
  AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE                     uError
  AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE                      uError
  AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE                  uError
  AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE                    uError
  
  // Common Events
  AEEBT_PBAP_EVT_CONNECTED                                bdAddr
  AEEBT_PBAP_EVT_CONN_FAILED                              bdAddr
  AEEBT_PBAP_EVT_DISCONNECTED                             bdAddr
  AEEBT_PBAP_EVT_AUTH_REQ                                 PBAPObject

Interfaces:

  IBTEXTPBAP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTPBAP_AddRef()
    IBTEXTPBAP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTPBAP_QueryInterface()
    
=======================================================================

Function:  IBTEXTPBAP_RegisterServer()

Description:
  Server application uses this command to initialize the PBAP server and 
  registers it in the service discovery database (SDDB.) as PSE

Prototype:
  int IBTEXTPBAP_Register(
                          IBTExtPBAP*              po,
                          const char*              pSrvName,
                          AEEBTPBAPRepositories    supRepositories,
                          AEEBTPBAPAuthentication  auth);

Parameters:
  po                    - pointer to the IBTExtPBAP object
  pSrvName              - service name to store in SDDB; must not be NULL
  supRepositories       - supported repositories (Local and/or SIM)
  auth                  - Whether to enable authentication (user-id + PIN,
                          PIN only or None)

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_PBAP_EVT_SRV_REG_DONE event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_PBAP_EVT_SRV_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_PBAP_ERR_NONE
    AEEBT_PBAP_ERR_REC_EXISTS

=======================================================================

Function:  IBTEXTPBAP_DeregisterServer()

Description: 
  Terminates the PBAP server and deregisters it from the SDDB.

Prototype:
  int IBTEXTPBAP_Deregister( IBTExtPBAP* po );

Parameters:
  po                    - pointer to the IBTExtPBAP object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_PBAP_EVT_SRV_DEREG_DONE event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_PBAP_EVT_SRV_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_PBAP_ERR_NONE
    AEEBT_PBAP_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTPBAP_AcceptConnection()

Description: 
  PSE application must issue this command to accept a connection request 
  from a PCE. 

Prototype:
  int IBTEXTPBAP_AcceptConnection( IBTExtPBAP* po,
                                   boolean     bAccept );

Parameters:
  po                    - pointer to the IBTExtPBAP object
  bAccept               - TRUE to accept, FALSE to reject 

Return value:
  SUCCESS   - the request to accept the connection request was queued
              and that AEEBT_PBAP_EVT_CONNECTED event will be generated 
              at a later time (if the server accepted the connection.)
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_PBAP_EVT_CONNECTED with the accompanying error code AEEBT_ERR_NONE.
  
=======================================================================

Function:  IBTEXTPBAP_Authenticate()

Description:
  PCE and PSE application can use this command to authenticate
  the connection being brought up.
  
  Client and Server application should issue this command in response to 
  event AEEBT_PBAP_EVT_AUTH_REQ.

Prototype:
  int IBTEXTPBAP_Authenticate(
                          IBTExtPBAP*             po,
                          const AECHAR*           pwUserID,
                          const AECHAR*           pwPIN);

Parameters:

  po              - pointer to the IBTExtPBAP object
  pwUserID        - pointer to User-id string
  pwPIN           - pointer to PIN string

Return value:
  SUCCESS   - the request to authenticate was queued.  On the client, 
              AEEBT_PBAP_EVT_CONNECTED or AEEBT_PBAP_EVT_CONN_FAILED event 
              will be generated at a later time.  On server, the event 
              AEEBT_PBAP_EVT_CONN_REQ will be generated if authentication
              is successful
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  For server, if the authentication passes, the event AEEBT_PBAP_EVT_CONN_REQ 
  will be generated again with bUnauthorized set to FALSE.  Server can then
  accept or reject the connection request.
  
  For client, depending on whether server accepts the connection request, 
  either the event AEEBT_PBAP_EVT_CONNECTED or AEEBT_PBAP_EVT_CONN_FAILED 
  will be generated.

=======================================================================

Function:  IBTEXTPBAP_Connect()

Description:
  Called by client application to connect to remote PBAP server.

Prototype:

  int IBTEXTPBAP_Connect(
                          IBTExtPBAP*             po,
                          const AEEBTBDAddr*     pBDAddr, 
                          uint8                  uChannelNumber);

Parameters:

  po              - pointer to the IBTExtPBAP object
  pBDAddr         - BD address of PBAP server
  uChannelNumber  - the server channel number to use to establish connection.
                    If 0, the stack will do SDP query to obtain SCN.

Return value:
  SUCCESS   - the request to connect to PBAP server was queued and that
              AEEBT_PBAP_EVT_CONNECTED or AEEBT_PBAP_EVT_CONN_FAILED event
              will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Event:
  AEEBT_PBAP_EVT_CONNECTED or
  AEEBT_PBAP_EVT_CONN_FAILED

=======================================================================

Function:  IBTEXTPBAP_Disconnect()

Description:
  Client can call this interface to terminate the PBAP session
  with the server.

Prototype:
  int IBTEXTPBAP_Disconnect( IBTExtPBAP* po );

Parameters:
  po          - pointer to the IBTExtPBAP object

Return value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEEBT_PBAP_EVT_DISCONNECTED event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_PBAP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_PBAP_ERR_NONE
    AEEBT_PBAP_ERR_NO_CONN
    
=======================================================================

Function:  IBTEXTPBAP_RegisterClient()

Description:
  Client application uses this command to register in the service 
  discovery database (SDDB.) as PCE.

Prototype:
  int IBTEXTPBAP_RegisterClient(
                          IBTExtPBAP*              po,
                          const char*              pCliName);

Parameters:
  po                    - pointer to the IBTExtPBAP object
  pSrvName              - service name to store in SDDB; must not be NULL

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_PBAP_EVT_CLI_REG_DONE event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_PBAP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_PBAP_ERR_NONE
    AEEBT_PBAP_ERR_REC_EXISTS

=======================================================================

Function:  IBTEXTPBAP_DeregisterClient()

Description: 
  De-registers PCE from the SDDB.

Prototype:
  int IBTEXTPBAP_DeregisterClient( IBTExtPBAP* po );

Parameters:
  po                    - pointer to the IBTExtPBAP object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_PBAP_EVT_CLI_DEREG_DONE event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_PBAP_EVT_CLI_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_PBAP_ERR_NONE
    AEEBT_PBAP_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTPBAP_Abort()

Description:
  Client application can issue this command to abort the current operation.

Prototype:
  int IBTEXTPBAP_Abort( IBTExtPBAP* po );

Parameters:
  po              - pointer to the IBTExtPBAP object

Return value:
  SUCCESS   - the current operation has been aborted.
  EIDLE     - no operation in progress
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTPBAP_PullPhoneBook()

Description:
  Client issues this command to pull a phone book object from the Server.

Prototype:
  int IBTEXTPBAP_PullPhoneBook( 
                          IBTExtPBAP*                po,
                          const AECHAR*              objName,
                          const AEEBTPBAPAttribMask* pAttribMask,
                          const AEEBTPBAPFormat*     pFormat,
                          const uint16*              pMaxListCount,
                          const uint16*              pListStartOffset);

Parameters:
  po              - pointer to the IBTExtPBAP object
  objName         - phone book object name including the path 
                    (e.g "telecom/pb.vcf")
  pAttribMask     - requested vCard attributes mask.  If NULL, default value
                    (as specified in the spec.) is used.
  pFormat         - vCard 2.1 or 3.0.  If NULL, defaults to vCard 2.1 format.
  pMaxListCount   - If not NULL, denotes the max number of entries that PSE
                    should return in the phone book object.  
                    If unspecified, it defaults to 65535
  pListStartOffset- If not NULL, indicates the offset of the first entry
                    of phone book object compared to the first entry of 
                    the phone book object that would be returned if
                    MaxListCount parameter was not specified.
                    If unspecified, defaults to 0

Return value:
  SUCCESS   - the request to pull the phone book object has been queued
              and that AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE
  
=======================================================================

Function:  IBTEXTPBAP_SetPhoneBook()

Description:
  Client issues this command to set the current folder in the virtual
  folder architecture.

Prototype:
  int IBTEXTPBAP_SetPhoneBook( 
                          IBTExtPBAP*          po,
                          const AECHAR*        pwFolder,
                          AEEBTPBAPSetPathFlag path);

Parameters:
  po              - pointer to the IBTExtPBAP object
  pwFolder        - folder name (e.g "pb").  Note that OBEX SetPath
                    command allows only for setting the current folder
                    to the root, parent or a child folder.  For example,
                    to set the current phonebook folder to "pb" from 
                    the root, it is necessary to call this interface 
                    twice; first to change to "telecom" followed by 
                    change into "pb".
  path            - Whether to move to root, child or parent folder

Return value:
  SUCCESS   - the request to set the phone book folder has been queued
              and that AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE
  
=======================================================================

Function:  IBTEXTPBAP_PullvCardListing()

Description:
  Client issues this command to retrieve the PSE's phonebook-listing
  object.

Prototype:
  int IBTEXTPBAP_PullvCardListing( 
                          IBTExtPBAP*                  po,
                          const AECHAR*                pwFolder,
                          const AEEBTPBAPOrder*        pSortOrder,
                          const AEECHAR*               pwSearchValue,
                          const AEEBTPBAPSearchAttrib* pSearchAttrib,
                          const uint16*                pMaxListCount,
                          const uint16*                pListStartOffset);

Parameters:
  po              - pointer to the IBTExtPBAP object
  pwFolder        - folder name (e.g "pb").  PullvCardListing() uses
                    relative paths
  pSortOrder      - what sorting order to use (Alphabetical, Indexed or 
                    Phonetical).  If MULL, defaults to Indexed.
  pwSearchValue   - search string value.  Can be NULL, if no filtering
                    is required.
  pSearchAttrib   - search attribute (Name, Number or Sound).  If NULL,
                    defaults to Name.
  pMaxListCount   - If not NULL, denotes the max number of entries that PSE
                    should return in <x-bt/vcard-listing> listing object.  
                    If unspecified, it defaults to 65535
  pListStartOffset- If not NULL, indicates the offset of the first entry
                    of <x-bt/vcard-listing> listing object compared to the
                    first entry of the vcard-listing object that would be
                    returned if MaxListCount parameter was not specified.
                    If unspecified, defaults to 0

Return value:
  SUCCESS   - the request to pull phonebook-listing object has been queued
              and that AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE
  
=======================================================================

Function:  IBTEXTPBAP_PullvCardEntry()

Description:
  Client issues this command to retrieve a specific vCard from PSE.

Prototype:
  int IBTEXTPBAP_PullvCardEntry( 
                          IBTExtPBAP*                 po,
                          const AECHAR*               pwName,
                          const AEEBTPBAPAttribMask*  pAttribMask,
                          const AEEBTPBAPFormat*      pFormat);

Parameters:
  po              - pointer to the IBTExtPBAP object
  pName           - name of vCard object to pull (e.g, 1.vcf)
  pAttribMask     - request vCard attributes.  If NULL, default (as specified
                    in the spec) is used.
  pFormat         - requested format (vCard 2.1 or 3.0). If NULL, defaults
                    to vCard 2.1

Return value:
  SUCCESS   - the request to pull phonebook-listing object has been queued
              and that AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE
  
=======================================================================

Function:  IBTEXTPBAP_PullPhoneBookResponse()

Description:
  Issued by PBAP Server in response to request to pull a phone book from
  the Client as given by AEEBT_PBAP_EVT_PULL_PHONE_BOOK_REQ event.

Prototype:
  int IBTEXTPBAP_PullPhoneBookResponse( 
                          IBTExtPBAP*          po,
                          const AECHAR*        pwFileName,
                          const uint16*        pPhoneBookSize,
                          const uint8*         pNewMissedCalls,
                          AEEBTError           error);

Parameters:
  po              - pointer to the IBTExtPBAP object
  pwFileName      - pointer to file containing vCard data.  NULL if PCE 
                    requested only phone book size (i.e, MaxListCount in
                    the request is 0).
  pPhoneBookSize  - size of phone book (i.e, number of non-null entries)
                    sent in response when the value of MaxListCount in 
                    the request is 0.  Set to NULL, if the request is for
                    downloading phone book contents.
  pNewMissedCalls - number of missed calls that has not been checked yet
                    by the user when PBAP session started.  NULL if
                    requested phone book object is not mch.
  error           - status indicating success or failure (if the request
                    cannot be fulfilled).

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_PULL_PHONE_BOOK_REQ
  
=======================================================================

Function:  IBTEXTPBAP_SetPhoneBookResponse()

Description:
  Issued by PBAP Server in response to set phonebook request from
  the Client as given by AEEBT_PBAP_EVT_SET_PHONE_BOOK_REQ event.

Prototype:
  int IBTEXTPBAP_SetPhoneBookResponse( 
                          IBTExtPBAP*          po,
                          AEEBTError           status);

Parameters:
  po              - pointer to the IBTExtPBAP object
  status          - status indicating success or failure of the operation

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_SET_PHONE_BOOK_REQ
  
=======================================================================

Function:  IBTEXTPBAP_PullvCardListingResponse()

Description:
  Issued by PBAP Server in response to request from the Client to
  pull phonebook-listing object as given by 
  AEEBT_PBAP_EVT_PULL_VCARD_LISTING_REQ event.

Prototype:
  int IBTEXTPBAP_PullvCardListingResponse( 
                          IBTExtPBAP*          po,
                          const AECHAR*        pwFileName,
                          const uint16*        pPhoneBookSize,
                          const uint8*         pNewMissedCalls,
                          AEEBTError           error);

Parameters:
  po              - pointer to the IBTExtPBAP object
  pwFileName      - pointer to file containing phonebook-listing object in XML
                    format.  NULL if client requested only size of phone book
                    to be returned (i.e, MaxListCount in the request was 0)
  pPhoneBookSize  - number of indexes that are non-null in the phone book 
                    of interest.  Return NULL if MaxListCount in the
                    request is not 0.
  pNewMissedCalls - number of missed calls that has not been checked yet by
                    the user when the PBAP session started.  Return NULL
                    if the phone book object is not mch.
  error           - status indicating success or failure if the request
                    cannot be fulfilled.

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_PULL_VCARD_LISTING_REQ
  
=======================================================================

Function:  IBTEXTPBAP_PullvCardEntryResponse()

Description:
  Issued by PBAP Server in response to request from the Client to
  pull a specific phone book entry as given by 
  AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_REQ event.

Prototype:
  int IBTEXTPBAP_PullvCardEntryResponse( 
                          IBTExtPBAP*          po,
                          const AECHAR*        pwFileName,
                          AEEBTError           error);

Parameters:
  po              - pointer to the IBTExtPBAP object
  pwFileName      - pointer to file containing phone book entry data 
                    in vCard format
  error           - status indicating success or failure if the request
                    cannot be fulfilled.

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_REQ


=======================================================================*/

#endif /* _AEE_BT_EXT_PBAP_H_ */
