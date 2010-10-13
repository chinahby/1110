#ifndef IXHTTP_H
#define IXHTTP_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             H T T P   C L I E N T   A P P L I C A T I O N

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/inc/IxHTTP.h#4 $ $DateTime: 2008/04/07 14:26:25 $ $Author: sladha $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/05   om      Created Module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#ifdef __cplusplus
extern "C" {
#endif

  #include "ipc.h"
  #include "ipcsvc.h"

#ifdef __cplusplus
}
#endif


#include "IxStream.h"

#include "IxErrno.h"

/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/
#define IXHTTP_IPC_USR_DATA_FIELDS    1
#define IXHTTP_ATTR_LIST_SIZE         10
#define IXHTTP_MAX_NUM_CLIENTS        25
#define IXHTTP_MAX_DOTD_ADDR_SIZE     21

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================

INTERFACES DOCUMENTATION
=============================================================================
Interface Name: IxHTTP

Description:
This interface abstracts the HTTP protocol and allows users to:
(1) Set HTTP headers
(2) Get/Post an HTTP message
(3) Get the associated HTTP response

NOTE: 
-The interface implements only those elements of the HTTP protocol
needed to handle communication of OMA DRM 2.0 ROAP messages.

-The following attributes are currently supported: "Accept", "Content-Type",
 "DRM-Version".

-The server response status codes of 200 and 100 are understood by the engine.
All status codes within the 200-299 range are treated as successful. All other
status codes cause an error to be sent.

Required header file:
IxStream.h
IxErrno.h

See Also

=============================================================================*/
class IxHTTP
{
public:

  /* Method definitions */
  IxHTTP(){};
  virtual ~IxHTTP(){};

/*=============================================================================

Function: AddAttribute()

Description:
   This function is used to specify header attributes.

Dependencies:
   None

Prototype:
   IxErrnoType AddAttribute
   (
      const char        *attr,
      const char        *value
   ) 

Parameters:
   attr           [IN]     Attribute string
   value          [IN]     Attribute value

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful - attribute not supported
   E_INVALID_ARG           Argument strings are too large

Comments:
   Usage:
   myHTTP.AddAttribute( "Accept", "application/something-special" );

Side Effects:
   None

Version:

See Also:
  GetSupportedAttributes()

=============================================================================*/
  virtual IxErrnoType AddAttribute( const char* attr, 
                                    const char* value ) = 0;

/*=============================================================================

Function: SetAttribute()

Description:
   This function is used to specify header attributes. It adds the attributes
   if has not been added previously, otherwise replaces the existing attribute
   with the new one.

Dependencies:
   None

Prototype:
   IxErrnoType SetAttribute
   (
      const char        *attr,
      const char        *value
   ) 

Parameters:
   attr           [IN]     Attribute string
   value          [IN]     Attribute value

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful - attribute not supported
   E_INVALID_ARG           Argument strings are too large

Comments:
   Usage:
   myHTTP.SetAttribute( "Accept", "application/something-new" );

Side Effects:
   None

Version:

See Also:
  GetSupportedAttributes()

=============================================================================*/
//  virtual IxErrnoType SetAttribute( const char* attr, 
//                                    const char* value ) = 0;

/*=============================================================================

Function: GetAttribute()

Description:
   This function retrieves the header attributes received.

Dependencies:
   None

Prototype:
   char* GetAttribute
   (
      const char        *attr
      char             **value;
   ) 

Parameters:
   attr           [IN]     Attribute string
   value          [OUT]    Pointer containing attribute value

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful - attribute not supported
   E_INVALID_ARG           Argument strings are too large


Comments:
   Usage:
   myHTTP.GetAttribute( "WWW-Authenticate", &attrVal );

Side Effects:
   None

Version:

See Also:
  GetSupportedAttributes()

=============================================================================*/
  virtual IxErrnoType GetAttribute( const char* attr,
                                    char **value ) = 0;

/*=============================================================================

Function: GetSupportedAttributes()

Description:
   This function gets a list of attribute names which are understood by the
   IxHTTP engine.

Dependencies:
   None

Prototype:
   char** GetSupportedAttributes
   (
   ) 

Parameters:
   None

Return Value:
   char**                  Array of strings containing supported
                           attributes

Comments:   

Side Effects:
  None

Version:

See Also:

=============================================================================*/
  virtual const char** GetSupportedAttributes( void ) = 0;

/*=============================================================================

Function: Get()

Description:
   This function sends a GET request to the HTTP server.

Dependencies:
   None

Prototype:
   IxErrnoType Get
   (
      char*             *urlPtr,
      uint32             urlLen
   ) 

Parameters:
   urlPtr         [IN]     The URL string
   urlLen         [IN]     Length of the URL string

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful - ipc message creation
                           failed

Comments:   
   The server response is delivered as aruments to the notifier callback.

   The notifier callback is also called when there are errors in
   sending/processing the message.

Side Effects:
   None

Version:

See Also:
   GetNotifier()

=============================================================================*/
  virtual IxErrnoType Get( char*  urlPtr, 
                                uint32 urlLen ) = 0;

/*=============================================================================

Function: Get()

Description:
   This function sends a GET request to the HTTP server.

Dependencies:
   None

Prototype:
   IxErrnoType Get
   (
      IxStream          &urlStream
   ) 

Parameters:
   urlStream      [IN]     IxStream containing URL

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful - ipc message creation
                           failed

Comments: 
   The default behaviour is a no-op and returns a SUCCESS. This
   function can be implemented to parse the stream and call
   Get() with the URL string.

Side Effects:
   None

Version:

See Also:
   GetNotifier()


=============================================================================*/
  virtual IxErrnoType Get( IxStream& urlStream ) = 0;

/*=============================================================================

Function: GetNotifier()

Description:
   This function will be called upon completion of the GET request.

Dependencies:
   None

Prototype:
   IxErrnoType GetNotifier
   (
      IxErrnoType        myErrno,
      IxStream          *rspStream,
      uint32             rspStatusCode
   ) 

Parameters:
   myErrno        [IN]     E_SUCCESS - rspStream contains a valid response
                           E_FAILURE - the server sent a response status that
                           is not supported or there were problems during
                           sending/receiving the message
                           
   rspStream      [IN]     IxStream containing the body of the server
                           response

   rspStatusCode  [IN]     The status code in the HTTP response

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful

Comments:

Side Effects:
   None

Version:

See Also:
   Get()

=============================================================================*/
  virtual void      GetNotifier( IxErrnoType myErrno,
                                 IxStream*   rspStream,
                                 uint32      rspStatusCode) = 0;


/*=============================================================================

Function: Post()

Description:
   This function sends a POST request to the HTTP server.

Dependencies:
   None

Prototype:
   IxErrnoType Get
   (
      char              *urlPtr,
      uint32             urlLen,
      char              *bodyPtr,
      uint32             bodyLen
   ) 

Parameters:
   urlPtr         [IN]     The URL string
   urlLen         [IN]     Length of the URL string
   bodyPtr        [IN]     The string containing the body of the POST message
   bodyLen        [IN]     The length of the message body

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful - ipc message creation
                           failed

Comments:   
   The server response is delivered as aruments to the notifier callback.

   The notifier callback is also called when there are errors in
   sending/processing the message.

Side Effects:
   None

Version:

See Also:
   PostNotifier()

=============================================================================*/
   virtual IxErrnoType Post( char*  urlPtr, 
                            uint32 urlLen,
                            char*  bodyPtr,
                            uint32 bodyLen ) = 0;

/*=============================================================================

Function: Post()

Description:
   This function sends a POST request to the HTTP server.

Dependencies:
   None

Prototype:
   IxErrnoType Get
   (
      IxStream          &urlStream
      IxStream          &bodyStream
   ) 

Parameters:
   urlStream      [IN]     IxStream containing URL
   bodyStream     [IN]     IxStream containing the body of the POST message

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful - ipc message creation
                           failed

Comments: 
   The default behaviour is a no-op and returns a SUCCESS. This function can
   be implemented to parse the streams and call Post() with the URL string and
   body.


Side Effects:
   None

Version:

See Also:
   PostNotifier()

=============================================================================*/
  virtual IxErrnoType Post( IxStream& urlStream,
                            IxStream& bodyStream) = 0;

/*=============================================================================

Function: PostNotifier()

Description:
   This function will be called upon completion of the GET request.

Dependencies:
   None

Prototype:
   IxErrnoType GetNotifier
   (
      IxErrnoType        myErrno,
      IxStream          *rspStream,
      uint32             rspStatusCode
   ) 

Parameters:
   myErrno        [IN]     E_SUCCESS - rspStream contains a valid response
                           E_FAILURE - the server sent a response status that
                           is not supported or there were problems during
                           sending/receiving the message
                           
   rspStream      [IN]     IxStream containing the body of the server
                           response

   rspStatusCode  [IN]     The status code in the HTTP response

Return Value:
   E_SUCCESS               successful
   E_FAILURE               unsuccessful

Comments:

Side Effects:
   None

Version:

See Also:
   Post()

=============================================================================*/
  virtual void      PostNotifier( IxErrnoType myErrno,
                                  IxStream*   rspStream,
                                  uint32      rspStatusCode) = 0;
};
 

/***********************************************************************/
/********************* END PUBLIC INTERFACE ****************************/
/***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define IXHTTP_MSG_BASE                   IPCSVC_IXHTTP_BASE_MSG_ID

  /* GET request */
#define IxHTTPGetMsg_id                   (IXHTTP_MSG_BASE + 0x0001)
#define IxHTTPGetNotifierMsg_id           (IXHTTP_MSG_BASE + 0x0002)
#define IxHTTPPostMsg_id                  (IXHTTP_MSG_BASE + 0x0003)
#define IxHTTPPostNotifierMsg_id          (IXHTTP_MSG_BASE + 0x0004)
#define IxHTTPXportEventMsg_id            (IXHTTP_MSG_BASE + 0x0005)
#define IxHTTPIntraXportEventMsg_id       (IXHTTP_MSG_BASE + 0x0006)
#define IxHTTPNewMsg_id                   (IXHTTP_MSG_BASE + 0x0007)
//#define IxHTTPXportFailedMsg_id           (IXHTTP_MSG_BASE + 0x0008)
#define IxHTTPIntraXportSocketCbMsg_id    (IXHTTP_MSG_BASE + 0x0009)
#define IxHTTPShutdownSvrObjctMsg_id      (IXHTTP_MSG_BASE + 0x000e)
#define IxHTTPDeleteSvrObjctMsg_id        (IXHTTP_MSG_BASE + 0x000f)
#define IxHTTPGetAttributeMsg_id          (IXHTTP_MSG_BASE + 0x0010)
#define IxHTTPAttributeGotMsg_id          (IXHTTP_MSG_BASE + 0x0011)

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           clientPtr;
  } IxHTTPNewMsg;

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           uniqueId;
    uint32           attrListAddr;
    uint32           urlLen;
    char*            urlPtr;
    uint32           timeOut;
  } IxHTTPGetMsg;
  
  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           clientPtr;
    IxErrnoType      httpErrno;
    uint32           rspStream;
    uint32           rspStatusCode;
  } IxHTTPGetNotifierMsg;
  
  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           uniqueId;
    uint32           attrListAddr;
    uint32           urlLen;
    uint32           bodyLen;
    char*            urlPtr;
    char*            bodyPtr;
    uint32           timeOut;
  } IxHTTPPostMsg;
  
  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           clientPtr;
    IxErrnoType      httpErrno;
    uint32           rspStream;
    uint32           rspStatusCode;
  } IxHTTPPostNotifierMsg;

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           svc;
    uint32           err;
  } IxHTTPXportEventMsg;

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    //uint32           clientId;
    uint32           xport;
  } IxHTTPIntraXportEventMsg;
/*
  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           svc;
  } IxHTTPXportFailedMsg;
*/

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           xport;
  } IxHTTPIntraXportSocketCbMsg;

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    uint32           svc;
  } IxHTTPDeleteSvrObjctMsg; 
  
  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
  } IxHTTPShutdownSvrObjctMsg;

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    char*            attrName;
  } IxHTTPGetAttributeMsg;

  typedef struct
  {
    ipc_msg_id_type  msg_id;
    uint32           clientId;
    IxErrnoType      httpErrno;
    char*            attrValue;
  } IxHTTPAttributeGotMsg;

#ifdef __cplusplus
}
#endif
#endif /* IXHTTP_H */
