#ifndef AEEOBEXCOMDEFS_H
#define AEEOBEXCOMDEFS_H
/*=============================================================================
FILE:         AEEOBEXComDef.h

SERVICES:     IOBEX common definitions

DESCRIPTION:  This file contains the data structures and
              data types used by the IOBEX server and client interface.
              OBEX Constants and definitions in the file refer to those
              defined in IrDA OBEX spec v1.3 .

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


#include "AEEStdDef.h"


/*=============================================================================
   Definitions, Macros
==============================================================================*/

// Maximum Uid length defined by the IrDA OBEX Spec v1.3 Sec 3.5.2
#define AEEOBEX_MAX_UID_LEN             20

// MIME types used by OBEX 
#define AEEOBEX_FOLDER_LISTING_TYPE         "x-obex/folder-listing" 
#define AEEOBEX_ICALENDAR_TYPE              "text/calendar"         
#define AEEOBEX_VCALENDAR_TYPE              "text/x-vcalendar"      
#define AEEOBEX_VCARD_TYPE                  "text/x-vcard"          
#define AEEOBEX_VNOTE_TYPE                  "text/x-vnote"          
#define AEEOBEX_VMESSAGE_TYPE               "text/x-vmsg"           
#define AEEOBEX_UPF_TYPE                    "image/x-UPF"           
#define AEEOBEX_JPEG_TYPE                   "image/jpeg"            
#define AEEOBEX_TEXT_TYPE                   "text/plain"            
 

/*=============================================================================
 OBEX Transport types
=============================================================================*/
#define AEEOBEX_TRANS_BT_RFCOMM   0x0105d583


// A data type to refer to types of transport
typedef AEEUID  AEEOBEXTransport;


/*=============================================================================
 OBEX Event types 
=============================================================================*/
// Common events for client and server session
#define AEEOBEX_EVT_AUTH_CHALLENGE       0x01
#define AEEOBEX_EVT_AUTH_RESPONSE        0x02
#define AEEOBEX_EVT_AUTH_COMPL           0x03
#define AEEOBEX_EVT_ERROR                0x04

// Events for Server 
#define AEEOBEX_EVT_SRV_REG_CMPL         0x10
#define AEEOBEX_EVT_SRV_DEREG_CMPL       0x11
#define AEEOBEX_EVT_SRV_NEW_CLI          0x12

// Events for server session
#define AEEOBEX_EVT_SRV_CONN_REQ         0x20
#define AEEOBEX_EVT_SRV_PUT_REQ          0x21
#define AEEOBEX_EVT_SRV_GET_REQ          0x22
#define AEEOBEX_EVT_SRV_SETPATH_REQ      0x23
#define AEEOBEX_EVT_SRV_DISC_REQ         0x24
#define AEEOBEX_EVT_SRV_CONN_COMPL       0x26
#define AEEOBEX_EVT_SRV_ABORTED          0x27

// Events for Clients
#define AEEOBEX_EVT_CLI_CONN_RESP        0x40
#define AEEOBEX_EVT_CLI_GET_RESP         0x41
#define AEEOBEX_EVT_CLI_PUT_RESP         0x42
#define AEEOBEX_EVT_CLI_SETPATH_RESP     0x43
#define AEEOBEX_EVT_CLI_DISC_RESP        0x44
#define AEEOBEX_EVT_CLI_ABORT_COMPL      0x45


// A data type to refer to types of OBEX events
typedef uint32    AEEOBEXEventType;

/*=============================================================================
 OBEX Command types 
=============================================================================*/
#define AEEOBEX_REQ_CONNECT                      0x80
#define AEEOBEX_REQ_PUT                          0x02
#define AEEOBEX_REQ_GET                          0x03
#define AEEOBEX_REQ_SETPATH                      0x85
#define AEEOBEX_REQ_DISCONNECT                   0x81
#define AEEOBEX_REQ_ABORT                        0xFF

// A data type to refer to command type
typedef uint32 AEEOBEXRequestType;

/*==============================================================================
 Character set for Realm string defined in IrDA OBEX v1.3 specification; Sec 3.5.1.3
===============================================================================*/
#define AEEOBEX_CHAR_ASCII                      0x00
#define AEEOBEX_CHAR_ISO_8859_1                 0x01
#define AEEOBEX_CHAR_ISO_8859_2                 0x02
#define AEEOBEX_CHAR_ISO_8859_3                 0x03
#define AEEOBEX_CHAR_ISO_8859_4                 0x04
#define AEEOBEX_CHAR_ISO_8859_5                 0x05
#define AEEOBEX_CHAR_ISO_8859_6                 0x06
#define AEEOBEX_CHAR_ISO_8859_7                 0x07
#define AEEOBEX_CHAR_ISO_8859_8                 0x08
#define AEEOBEX_CHAR_UNICODE                    0xFF


// A data type to refer to character set for realm string
typedef uint32  AEEOBEXCharSet;

/*==============================================================================
 OBEX result codes 
===============================================================================*/

#define AEEOBEX_STAT_RSP_BASE                          0x0000
#define AEEOBEX_STAT_BASE                              0x0100  

// Refer Sec 3.2.1 of IrDa OBEX specification.
#define AEEOBEX_STAT_SUCCESS                     (AEEOBEX_STAT_RSP_BASE + 0x00)
#define AEEOBEX_STAT_METHOD_NOT_ALLOWED          (AEEOBEX_STAT_RSP_BASE + 0x01)
#define AEEOBEX_STAT_CONTINUE                    (AEEOBEX_STAT_RSP_BASE + 0x02)
#define AEEOBEX_STAT_LENGTH_REQUIRED             (AEEOBEX_STAT_RSP_BASE + 0x03)
#define AEEOBEX_STAT_SERVICE_UNAVAILABLE         (AEEOBEX_STAT_RSP_BASE + 0x04)
#define AEEOBEX_STAT_VALUE_NOT_ACCEPTABLE        (AEEOBEX_STAT_RSP_BASE + 0x05)
#define AEEOBEX_STAT_NOT_MODIFIED                (AEEOBEX_STAT_RSP_BASE + 0x06)
#define AEEOBEX_STAT_GONE                        (AEEOBEX_STAT_RSP_BASE + 0x07)
#define AEEOBEX_STAT_REQ_TIMEOUT                 (AEEOBEX_STAT_RSP_BASE + 0x08)
#define AEEOBEX_STAT_UNAUTHORIZED                (AEEOBEX_STAT_RSP_BASE + 0x09)
#define AEEOBEX_STAT_NOT_IMPLEMENTED             (AEEOBEX_STAT_RSP_BASE + 0x0A)
#define AEEOBEX_STAT_DATABASE_FULL               (AEEOBEX_STAT_RSP_BASE + 0x0B)
#define AEEOBEX_STAT_DATABASE_LOCKED             (AEEOBEX_STAT_RSP_BASE + 0x0C)
#define AEEOBEX_STAT_INTERNAL_SERVER_ERROR       (AEEOBEX_STAT_RSP_BASE + 0x0D)
#define AEEOBEX_STAT_UNSUPPORTED_MEDIA_TYPE      (AEEOBEX_STAT_RSP_BASE + 0x0E)
#define AEEOBEX_STAT_PARTIAL_CONTENT             (AEEOBEX_STAT_RSP_BASE + 0x0F)
#define AEEOBEX_STAT_FORBIDDEN                   (AEEOBEX_STAT_RSP_BASE + 0x10)

// Result codes that are sent by the underlying OBEX implementation
#define AEEOBEX_STAT_COMMAND_ERROR                     (AEEOBEX_STAT_BASE + 0x01)
#define AEEOBEX_STAT_CONNECTION_TIMEOUT                (AEEOBEX_STAT_BASE + 0x02)
#define AEEOBEX_STAT_CONNECT_FAILED                    (AEEOBEX_STAT_BASE + 0x03)
#define AEEOBEX_STAT_DISCONNECT_FAILED                 (AEEOBEX_STAT_BASE + 0x04)
#define AEEOBEX_STAT_ERROR                             (AEEOBEX_STAT_BASE + 0x05)
#define AEEOBEX_STAT_INCOMPLETE_PACKET                 (AEEOBEX_STAT_BASE + 0x06)
#define AEEOBEX_STAT_NOT_CONNECTED                     (AEEOBEX_STAT_BASE + 0x07)
#define AEEOBEX_STAT_NO_MORE_CONNECTIONS               (AEEOBEX_STAT_BASE + 0x08)
#define AEEOBEX_STAT_OPERATION_IN_PROGRESS             (AEEOBEX_STAT_BASE + 0x09)
#define AEEOBEX_STAT_PUT_RESPONSE_ERROR                (AEEOBEX_STAT_BASE + 0x0A)
#define AEEOBEX_STAT_GET_RESPONSE_ERROR                (AEEOBEX_STAT_BASE + 0x0B)
#define AEEOBEX_STAT_ERROR_REQUIRED_HEADER_NOT_FOUND   (AEEOBEX_STAT_BASE + 0x0C)
#define AEEOBEX_STAT_TOO_MANY_HEADER_BYTES             (AEEOBEX_STAT_BASE + 0x0D)
#define AEEOBEX_STAT_UNKNOWN_COMMAND                   (AEEOBEX_STAT_BASE + 0x0E)
#define AEEOBEX_STAT_UNSUPPORTED_VERSION               (AEEOBEX_STAT_BASE + 0x0F)
#define AEEOBEX_STAT_CLIENT_ABORTED_COMMAND            (AEEOBEX_STAT_BASE + 0x10)
#define AEEOBEX_STAT_BAD_PACKET                        (AEEOBEX_STAT_BASE + 0x11)
#define AEEOBEX_STAT_BAD_REQUEST                       (AEEOBEX_STAT_BASE + 0x12)
#define AEEOBEX_STAT_OBJECT_OVERFLOW                   (AEEOBEX_STAT_BASE + 0x13)
#define AEEOBEX_STAT_NOT_FOUND                         (AEEOBEX_STAT_BASE + 0x14)
#define AEEOBEX_STAT_ACCESS_DENIED                     (AEEOBEX_STAT_BASE + 0x15)
#define AEEOBEX_STAT_PACKET_OVERFLOW                   (AEEOBEX_STAT_BASE + 0x16)
#define AEEOBEX_STAT_NO_SUCH_FOLDER                    (AEEOBEX_STAT_BASE + 0x17)
#define AEEOBEX_STAT_NAME_REQUIRED                     (AEEOBEX_STAT_BASE + 0x18)
#define AEEOBEX_STAT_PASSWORD_TOO_LONG                 (AEEOBEX_STAT_BASE + 0x19)
#define AEEOBEX_STAT_PRECONDITION_FAILED               (AEEOBEX_STAT_BASE + 0x1A)
#define AEEOBEX_STAT_INVALID_AUTH_DIGEST               (AEEOBEX_STAT_BASE + 0x1B)
#define AEEOBEX_STAT_INVALID_OPERATION                 (AEEOBEX_STAT_BASE + 0x1C)
#define AEEOBEX_STAT_SRV_INCOMPLETE_GET                (AEEOBEX_STAT_BASE + 0x1D)
#define AEEOBEX_STAT_FOLDER_BROWSING_NOT_ALLOWED       (AEEOBEX_STAT_BASE + 0x1E)
#define AEEOBEX_STAT_SERVER_FORCED_DISCONNECT          (AEEOBEX_STAT_BASE + 0x1F)
#define AEEOBEX_STAT_CONNECTION_NOT_FOUND              (AEEOBEX_STAT_BASE + 0x20)
#define AEEOBEX_STAT_AUTH_FAILED                       (AEEOBEX_STAT_BASE + 0x21)
#define AEEOBEX_STAT_AUTH_NOT_HANDLED                  (AEEOBEX_STAT_BASE + 0x22)
#define AEEOBEX_STAT_NO_RESOURCE                       (AEEOBEX_STAT_BASE + 0x23)


// A data type to refer to status sent in OBEX response 
typedef uint32 AEEOBEXCmdStatus;

/*=============================================================================
  Parameters to be read from the interface
============================================================================*/
#define AEEOBEXPARAM_BT_RFCOMM_SCN          0x0105d584
#define AEEOBEXPARAM_BT_BDA                 0x0105d585
#define AEEOBEXPARAM_MAX_PACKET_SIZE        0x0105d586


// data to refer to the parameter type
typedef AEEUID AEEOBEXParamType;

/*=============================================================================
  Bit map of OBEX Flags
============================================================================*/
#define AEEOBEX_FLAG_BACKUP          0x01
#define AEEOBEX_FLAG_DONT_CREATE_DIR    0x02


// data to refer to the flags
typedef uint32 AEEOBEXFlags;


/*=============================================================================
  OBEX Packet
============================================================================*/
#define AEEOBEX_PACKET_FINAL          0x01
#define AEEOBEX_PACKET_INTERMEDIATE   0x02


// data to refer to the packets
typedef uint32 AEEOBEXPacket;


/*=============================================================================
 Data associated with a authentication response from a remote device
=============================================================================*/
typedef struct
{
   int                nUidLen;
   uint8              aUserId[AEEOBEX_MAX_UID_LEN];
}  AEEOBEXAuthResp;


/*============================================================================
DATA STRUCTURE DOCUMENTATION
==============================================================================


AEEOBEXTransport

Definition
   typedef uint32  AEEOBEXTransport;

Description: Different transport types supported

   AEEOBEX_TRANS_BT_RFCOMM   : Bluetooth transport

Comments :

   Transport parameters are sent to the implementation as a null terminated
   string containing name-value pairs separated by a delimiter.The format
   of the string is "<name1>=<value1>;<name2>=<value2>".
   The list of names and value types for bluetooth RFCOMM transport
   are 
   ======================================================================
   Name               Data type              Sample String format
   ======================================================================
   bd_address         BDAddress              "bd_address=9abc650000a0"
   server_channel     uint32( hex format)    "server_channel=10"
   bt_security        BTSecurityLevel        "bt_security=1"
   service_name       string                 "service_name=ftp server"
   uuid16             uint16( hex format)    "uuid16=0010"
   uuid32             uint32( hex format)    "uuid32=001000A0"
   uuid128            16 byte( hex format)   "uuid128=12AF51A9030C4B2937407F8C9ECB238A"

   Examples of transport string for client
   1."bd_address=9abc650000a0;server_channel=10"
   A client is attempting to connect to a server whose BD 
   address is 0x9abc650000a0 and RFCOMM server channel is 0x10.

   2."bd_address=9abc650000a0;server_channel=0;uuid16=008"
   A client is attempting to connect to a server whose BD 
   address is 0x9abc650000a0 and whose SDP uuid matches 0x008.It
   is the responsibility of the implementation to query the server
   and find the correct server channel number.

   Examples of transport string for server
   1."bt_security=1"
   OBEX server is registerd with RFCOMM, however the implementation 
   does not register with SDP.
   2."service_name=ftp server;bt_security=1;uuid16=008"
   OBEX server is registerd with RFCOMM, and the implementation registers the
   server with SDP.


   The transport parameters are explained below

   bd_address     : Not valid for servers
                    Mandatory for clients that are using Bluetooth transport
                    This is the device address of the server
   server_channel : Not valid for servers.The implementation will
                    asign a server channel number that is available.

                    For clients this is the server channel on which the RFCOMM
                    RFCOMM connection is to be established.
                    If server channel is not specified,the implementation will 
                    attempt to connect to the service whose UUID is specified.
   bt_security    : Security for the connection
   uuid16         : specifies a 16 bit UUID
   uuid32         : specifies a 32 bit UUID
   uuid128        : specifies a 128 bit UUID
                    For servers, if the UUID is specified, the implementation
                    will register the service with SDP.If UUID is not specified
                    SDP registration is to be done by the application
                    For clients, the implementation will attempt to connect to a 
                    service with this UUID if the server channel is not specified.
   service_name   : Valid for servers only.This is the service name that is added
                    to the SDP record.While sending the service name the delimitor
                    is ';' and the null terminator should not be included.

==============================================================================
AEEOBEXCmdStatus

Definition
   typedef uint32 AEEOBEXCmdStatus;

Description:
   OBEX result codes.

   Following result codes map to result codes defined in
   Sec 3.2.1 of IrDa OBEX specification v1.3

   AEEOBEX_STAT_SUCCESS                  
   AEEOBEX_STAT_METHOD_NOT_ALLOWED       
   AEEOBEX_STAT_CONTINUE                    
   AEEOBEX_STAT_LENGTH_REQUIRED             
   AEEOBEX_STAT_SERVICE_UNAVAILABLE         
   AEEOBEX_STAT_VALUE_NOT_ACCEPTABLE        
   AEEOBEX_STAT_NOT_MODIFIED                
   AEEOBEX_STAT_GONE                        
   AEEOBEX_STAT_REQ_TIMEOUT                 
   AEEOBEX_STAT_UNAUTHORIZED                
   AEEOBEX_STAT_NOT_IMPLEMENTED             
   AEEOBEX_STAT_DATABASE_FULL               
   AEEOBEX_STAT_DATABASE_LOCKED             
   AEEOBEX_STAT_INTERNAL_SERVER_ERROR       
   AEEOBEX_STAT_UNSUPPORTED_MEDIA_TYPE      
   AEEOBEX_STAT_PARTIAL_CONTENT             
   AEEOBEX_STAT_FORBIDDEN                   

   Following result codes are sent by the underlying OBEX implementation

   AEEOBEX_STAT_COMMAND_ERROR                   
   AEEOBEX_STAT_CONNECTION_TIMEOUT         
   AEEOBEX_STAT_CONNECT_FAILED             
   AEEOBEX_STAT_DISCONNECT_FAILED          
   AEEOBEX_STAT_ERROR                           
   AEEOBEX_STAT_INCOMPLETE_PACKET               
   AEEOBEX_STAT_NOT_CONNECTED                   
   AEEOBEX_STAT_NO_MORE_CONNECTIONS             
   AEEOBEX_STAT_OPERATION_IN_PROGRESS           
   AEEOBEX_STAT_PUT_RESPONSE_ERROR              
   AEEOBEX_STAT_GET_RESPONSE_ERROR              
   AEEOBEX_STAT_ERROR_REQUIRED_HEADER_NOT_FOUND 
   AEEOBEX_STAT_TOO_MANY_HEADER_BYTES           
   AEEOBEX_STAT_UNKNOWN_COMMAND                 
   AEEOBEX_STAT_UNSUPPORTED_VERSION             
   AEEOBEX_STAT_CLIENT_ABORTED_COMMAND          
   AEEOBEX_STAT_BAD_PACKET                      
   AEEOBEX_STAT_BAD_REQUEST                     
   AEEOBEX_STAT_OBJECT_OVERFLOW                 
   AEEOBEX_STAT_NOT_FOUND                       
   AEEOBEX_STAT_ACCESS_DENIED                   
   AEEOBEX_STAT_PACKET_OVERFLOW                 
   AEEOBEX_STAT_NO_SUCH_FOLDER                  
   AEEOBEX_STAT_NAME_REQUIRED                   
   AEEOBEX_STAT_PASSWORD_TOO_LONG               
   AEEOBEX_STAT_PRECONDITION_FAILED             
   AEEOBEX_STAT_INVALID_AUTH_DIGEST             
   AEEOBEX_STAT_INVALID_OPERATION               
   AEEOBEX_STAT_SRV_INCOMPLETE_GET              
   AEEOBEX_STAT_FOLDER_BROWSING_NOT_ALLOWED     
   AEEOBEX_STAT_SERVER_FORCED_DISCONNECT        
   AEEOBEX_STAT_CONNECTION_NOT_FOUND            
   AEEOBEX_STAT_AUTH_FAILED                       
   AEEOBEX_STAT_AUTH_NOT_HANDLED                       
   AEEOBEX_STAT_NO_RESOURCE                       



=============================================================================
Definition
   typedef uint32  AEEOBEXCharSet;

Description:
   A data type to refer to types of character set for realm string.

   AEEOBEX_CHAR_ASCII                 : ASCII
   AEEOBEX_CHAR_ISO-8859-1            : ISO-8859-1 
   AEEOBEX_CHAR_ISO-8859-2            : ISO-8859-2
   AEEOBEX_CHAR_ISO-8859-3            : ISO-8859-3
   AEEOBEX_CHAR_ISO-8859-4            : ISO-8859-4
   AEEOBEX_CHAR_ISO-8859-5            : ISO-8859-5
   AEEOBEX_CHAR_ISO-8859-6            : ISO-8859-6
   AEEOBEX_CHAR_ISO-8859-7            : ISO-8859-7 
   AEEOBEX_CHAR_ISO-8859-8            : ISO-8859-8
   AEEOBEX_CHAR_UNICODE               : Unicode (usc2)

=============================================================================
Definition
   typedef uint32  AEEOBEXEventType;

Description:
   A data type to refer to types of OBEX events



   AEEOBEX_EVT_AUTH_CHALLENGE        : Sent when the authentication challenge 
                                       header is received.The application
                                       should respond by calling 
                                       xxx_SetUidPassword()
   AEEOBEX_EVT_AUTH_RESPONSE         : Sent when the authentication response  
                                       header is received.Data 
                                       corresponding to this
                                       event is AEEOBEXAuthResp.The application
                                       should respond by calling 
                                       xxx_SetUidPassword()
   AEEOBEX_EVT_AUTH_COMPL             :Sent when the authentication procedure
                                       is complete. Data is AEEOBEXCmdStatus.
                                       In case of the server, the application
                                       should respond by calling
                                       IOBEXSrvSession_AcceptConnection().
   AEEOBEX_EVT_ERROR                  :sent to indicate error in 
                                       transport connection. 
   AEEOBEX_EVT_SRV_REG_CMPL           :sent to the server when registration
                                       is complete.Result is returned in
                                       AEEOBEXCmdStatus
   AEEOBEX_EVT_SRV_DEREG_CMPL         :sent to the server when de registration
                                       is complete.Result is returned in
                                       AEEOBEXCmdStatus
   AEEOBEX_EVT_SRV_NEW_CLI            :sent to the server when a new client
                                       attempts to communicate with it. Data is 
                                       session object.The app can then service 
                                       requests from the client using this object
   AEEOBEX_EVT_SRV_CONN_REQ           :Sent to the server when the client tries 
                                       to connect to it.The server can respond by calling
                                       IOBEXSrvSession_AcceptConnection() or 
                                       IOBEXSrvSession_Authenticate().
   AEEOBEX_EVT_SRV_PUT_REQ            :Sent to the server when it receives a PUT
                                       request.
   AEEOBEX_EVT_SRV_GET_REQ            :Sent to the server when it receives a GET
                                       request.
   AEEOBEX_EVT_SRV_SETPATH_REQ        :Sent to the server when it receives a SETPATH
                                       request.
   AEEOBEX_EVT_SRV_DISC_REQ           :Sent to the server when it receives a 
                                       DISCONNECT request.
   AEEOBEX_EVT_SRV_CONN_COMPL         :Sent to the server indicating result of the 
                                       connection procedure 
   AEEOBEX_EVT_SRV_ABORTED            :Sent to the server when the client tries to 
                                       abort a multi packet operation
   AEEOBEX_EVT_CLI_CONN_RESP          :Sent to the client when the connection
                                       procedure is complete.
   AEEOBEX_EVT_CLI_GET_RESP           :Sent to the client when server responds 
                                       GET request.
   AEEOBEX_EVT_CLI_PUT_RESP           :Sent to the client when server responds to
                                       PUT request.
   AEEOBEX_EVT_CLI_SETPATH_RESP       :Sent to the client when server responds to
                                       SET PATH request.
   AEEOBEX_EVT_CLI_DISC_RESP          :Sent to the client when it has successfully
                                       disconnected from the server.
   AEEOBEX_EVT_CLI_ABORT_COMPL        :Sent to the client when the current operation
                                       was aborted.




=============================================================================
AEEOBEXParamType

Definition
   typedef uint32 AEEOBEXParamType;

Description:
   Parameters to be read from the interface

   AEEOBEXPARAM_BT_RFCOMM_SCN     : Rfcomm server channel for the connection
                                    a uint32 value
   AEEOBEXPARAM_BT_BDA            : BD address of the remote device,
                                    a six byte array.
   AEEOBEXPARAM_MAX_PACKET_SIZE   : Maximum packet size that can be sent to 
                                    the remote. uint32 value

=============================================================================
AEEOBEXFlags

Definition
   typedef uint32 AEEOBEXFlags;

Description:
   OBEX parameters to be read from the interface

   AEEOBEX_FLAG_BACKUP     : change directory up one level
   AEEOBEX_FLAG_CREATE_DIR : Create directory if dosent already exist
   

=============================================================================
AEEOBEXPacket

Definition
   typedef uint32 AEEOBEXPacket;

Description:
   OBEX parameters to be read from the interface

   AEEOBEX_PACKET_FINAL        : Final packet describing a request
   AEEOBEX_PACKET_INTERMEDIATE : Intermediate packet

=============================================================================
AEEOBEXAuthResp

Description
   Data corresponding to an authentication response received.This information
   is sent along with the AEEOBEX_EVT_AUTH_RESPONSE event.
   The remote has responded with a user ID after the app initiated
   authentication.The application should respond by calling 
   IOBEXSrvSession_SetUidPassword() if it is a server or IOBEXClient_SetUidPassword() 
   if it is a client. The response should include the password for the
   UID that is present in this struct.

Definition

typedef struct
{
   int                nUidLen;
   uint8              aUserId[AEEOBEX_MAX_UID_LEN];
}  AEEOBEXAuthResp;

Members
   nUidLen          : user Id length
   aUserId          : user Id


===============================================================================*/
#endif /* AEEOBEXCOMDEFS_H */



