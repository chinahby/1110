#ifndef IXHTTPSVC_H
#define IXHTTPSVC_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             H T T P   C L I E N T   A P P L I C A T I O N

           I P C   S E R V E R   I M P L E M E N T A T I O N

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/ixhttp/src/IxHTTPSvc.h#6 $ 
  $DateTime: 2008/04/07 14:26:25 $ 
  $Author: sladha $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/07   gr      Changed the value of IXHTTP_IPC_SIG; the old value was
                   colliding with a value used internally by L4.
06/06/06   jay/ssm Support reading multiple TCP segs & DRM integration fixes    
03/15/06   rv      Fixes for compiling on linux
12/29/05   om      Created Module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "arm.h"
#include "qw.h"
#include "rex.h"
#include "err.h"
#include "task.h"
#include "diag.h"
#include "nv.h"

#include "AEEContentHandlerTypes.h"

#include "IxErrno.h"

#include "IxHTTP.h"

//#define IXHTTP_WINSOC

#ifdef IXHTTP_WINSOC
  #include "IxHTTPXPortWinSock.h"
#else
  #include "IxHTTPXportSocket.h"
#endif
#include "IxHTTPPtcl.h"
#include "IxHTTPPtclV11.h"

#define NO_WAIT 1


/*===========================================================================

           DEFINITIONS AND CONSTANTS FOR HTTP TEST SERVER

===========================================================================*/

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
#define IXHTTPSVC_WAIT_FUNC            IxHTTPSvcWaitFunc
#define IXHTTPSVC_TCB_PTR              (&ix_httpsvc_tcb)

#define IXHTTPSVC_START_SIG      0x8000 /* Reserved for TASK_START_SIG     */
#define IXHTTPSVC_STOP_SIG       0x4000 /* Reserved for TASK_STOP_SIG      */
#define IXHTTPSVC_OFFLINE_SIG    0x2000 /* Reserved for TASK_OFFLINE_SIG   */

/* The following sigs are all okay, and can be allocated freely
 */
#define IXHTTPSVC_RPT_TIMER_SIG            0x0001
#define IXHTTPSVC_IPC_SIG                  0x0100
//#define IXHTTPSVC_EVENT_SIG              0x0004
#define IXHTTP_TASK_PRI           TASK_PRIORITY(BT_PRI_ORDER)
#define IXHTTP_STACK_SIZ          (8192/sizeof(rex_stack_word_type))
#define IXHTTP_GUARD_TIMEOUT               180000

#ifdef __cplusplus
extern "C" {
#endif
  
	#ifdef PLATFORM_LTK	
#error code not present
	#endif		
  
  extern rex_sigs_type IxHTTPSvcWaitFunc( rex_sigs_type wait_mask );
  
  extern void IxHTTPSvcMainFunc( dword dummy );
  
  extern void IxHTTPSvcProcessSigs( rex_sigs_type sigs );

  extern void ixhttp_timer_cb( IxHTTPSvc* svc);
#ifdef __cplusplus
}
#endif

/*===========================================================================

                 API DEFINITION

===========================================================================*/
class IxHTTPSvc : public IxHTTP
{
public:
  typedef enum
  {
    ADDRESS_INVALID = 0,
    ADDRESS_IP = 1,
    ADDRESS_RESERVED = 0xffffffffU
  } AddressType;

  IxHTTPSvc(uint32 clientId, uint32 clientPtr, uint32 clientThread);
  virtual ~IxHTTPSvc();

  /* Method to specifiy header attributes */
  virtual IxErrnoType AddAttribute( const char* attr, const char* value );
  virtual IxErrnoType GetAttribute( const char* attr, char **value );
  virtual const char**      GetSupportedAttributes( void );

  /* GET method and response */
  virtual IxErrnoType Get( char*  urlPtr, 
                        uint32 urlLen );
  virtual IxErrnoType Get( IxStream& urlStream );

  void      GetNotifier( IxErrnoType myErrno );
  virtual void      GetNotifier( IxErrnoType myErrno,
                                 IxStream*   rspStream,
                                 uint32      rspStatusCode);

  /* POST method and response */
  virtual IxErrnoType Post( char*  urlPtr, 
                            uint32 urlLen,
                            char*  bodyPtr,
                            uint32 bodyLen );

  virtual IxErrnoType Post( IxStream& urlStream,
                            IxStream& bodyStream );

  void      PostNotifier( IxErrnoType myErrno );
  virtual void      PostNotifier( IxErrnoType myErrno,
                                  IxStream*   rspStream,
                                  uint32      rspStatusCode);

  /* Methods used for incoming IPC messages */
  void ProcessGet( char*  urlPtr, 
                   uint32 urlLen,
                   uint32 attrListAddr);
  void ProcessPost( char*  urlPtr, 
                    uint32 urlLen,
                    char*  bodyPtr, 
                    uint32 bodyLen,
                    uint32 attrListAddr);
  void ProcessGetAttribute( char*  attrName);

  void              ProcessRequest();
  void              XportCleanup( void );

  uint32            GetClientId(void) {return __clientId;};
  void              SetAsyncErrno(IxErrnoType Errno) {__asyncErrno = Errno;};
  void              Close( void );
  void              timeOut ( void );

  /* Set the Gateway for all future HTTP connections */
  static void SetGateway( GatewayAddrType* pGateway );

private:
  typedef enum
  {
    INIT_STATE,
    XPORT_OPEN_STATE,
    XPORT_SEND_STATE,
    XPORT_RECEIVE_STATE,
    XPORT_CLOSING_STATE,
    XPORT_CLOSED_STATE,
    FAILED_STATE,
    RESERVED_STATE = 0xffffffffU
  } StateType;

  IxErrnoType  GetXport();
  AddressType       CheckAddress( const char* urlPtr );
  uint32            ParseAddress( char** urlRef );
   
  uint32                  __clientId;
  uint32                  __clientPtr;
  uint32                  __clientThread;
  IxErrnoType             __asyncErrno;
  StateType               __state;
  IxHTTPPtcl::MethodType  __httpMethod;
  char*                   __urlPtr;
  uint32                  __serverLen;
  char*                   __urlPath;
  char*                   __redirectURLPtr;
  IxHTTPPtcl::MethodType  __redirectHTTPMethod;
  char*                   __bodyPtr;
  uint32                  __bodyLen;
  IxStream*               __rspStream;
  char*                   __mimePtr;
  uint32                  __rspStatusCode;  

#ifdef IXHTTP_WINSOC
  IxHTTPXportWinSock*     __xport;
#else
  IxHTTPXportSocket*      __xport;
#endif

  IxHTTPPtcl::AttrType    __attrList[IXHTTP_ATTR_LIST_SIZE];
  IxHTTPPtcl::AttrType    __receivedAttrList[IXHTTP_ATTR_LIST_SIZE];
  boolean                 __hostSet;
  rex_timer_type*         __timeOutTimer;
  boolean                 __isMultipart;
  char*                   __multipartDivider;
  char*                   __partialPart;
  uint32                  __partialPartLen;
  boolean                 __isChunked;
  IxStream*               __chunkHolder;
  boolean                 __bMoreData;
  uint32                  __contentLen;
  uint32                  __contentLenRxd;
  uint32                  __chunkSize;
  uint32                  __rxdChunkSize;
  IxStream*               __dataCollector;
  boolean                 __bEventInProgress;
  boolean                 __bMimeSent;
  uint32                  __eventCounter;
  uint32                  __partLen;
  uint32                  __partCounter;
  char*                   __partialHdr;
  uint32                  __partialHdrLen;
  static char*            __pGateway;

public:
  boolean                 __delete;
  boolean                 __resend; 
  boolean                 __isTimerActive;
  uint32                  __timeOut; 
  uint32                  __uniqueId; 


private:
  /* Private utility methods */
  IxErrnoType   ParseHeader( char** rspRef, uint32* rspLen );
  void          CopyChunks( char *rspPtr, uint32 rspLen );
  void          CollectData( char *rspPtr, uint32 rspLen  );
  void          ProcessCompleteResponse( char* rspPtr, uint32 rspLen );
  void          ProcessProgressiveDownload( char* dataPtr, uint32 dataLen );
  void          ProcessMultipartContent( char* dataPtr, uint32 dataLen );
  void          SendProgressiveDownloadEvent( HttpEvtStatusEType  status,
                                              char* dataPtr, 
                                              uint32 dataLen );
  boolean       IsMimeTypeAccepted( const char* mimeType );
  void          ResetState();
  IxErrnoType   ReprocessRequest();
};



#endif /* IXHTTPSVC_H */

