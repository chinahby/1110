#ifndef AEEIQOSBUNDLE_H
#define AEEIQOSBUNDLE_H
/*=============================================================================

FILE:          AEEIQoSBundle.h

SERVICES:      BREW QoS Bundle Interfaces.

DESCRIPTION:   Interface definitions, data structures, etc. for BREW 
               QOS Bundle Interface.

===============================================================================
   Copyright (c) 2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "AEEIQoSList.h"
#include "AEECallback.h"

/*
   IQoSBundle interface
*/
#define AEEIID_IQoSBundle   0x01037ad4

#define AEE_QOS_BUNDLE_REQUEST_OP   1
#define AEE_QOS_BUNDLE_CONFIGURE_OP 2

typedef int AEEQoSBundleRequestOpcode;

/*
/ for IQOSBundle_OpenEx
*/
typedef struct AEEQoSBundleOpenEx
{
   IQoSList* pIQoSSessions;
   IQoSList* pIQoSSpecs;
   AEEQoSBundleRequestOpcode opcode;
} AEEQoSBundleOpenEx;

#define QOSBUNDLE_EXTFUNC_OPEN_EX 0x0103e86f  // pBuf  = pointer to AEEQoSBundleOpenEx


/*
   QoS Session Error
*/

#define AEE_QOS_SESSION_ERR_CODE_SUCCESS       0 /* no error */
#define AEE_QOS_SESSION_ERR_CODE_INVAL_SPEC    1
#define AEE_QOS_SESSION_ERR_CODE_INVAL_SESSION 2
#define AEE_QOS_SESSION_ERR_CODE_BADSTATE      3
#define AEE_QOS_SESSION_ERR_CODE_SELECT_NET    4
#define AEE_QOS_SESSION_ERR_CODE_NET_CONFLICT  5 /* conflicting with another option */


typedef int32 AEEQoSSessionErrorCode;

#define AEE_QOS_SESSION_ERR_INDEX_UNSPEC (-1)

typedef struct AEEQoSSessionError 
{
   AEEQoSSessionErrorCode nErrorCode;
   int32 nIndex;
} AEEQoSSessionError;


/*
   Quality Of Service Bundle Interface
*/
#define INHERIT_IQoSBundle(iname)   \
   INHERIT_IQI(iname);  \
   int   (*SelectNetworkEx)(iname* po, int nNetwork, int16 nFamily); \
   int   (*Open)(iname* po, IQoSList* pIQoSSessions, IQoSList* pIQoSSpecs); \
   int   (*GoActive)(iname* po, IQoSList* pIQoSSessions);    \
   int   (*Close)(iname* po, IQoSList* pIQoSSessions);       \
   int   (*Deactivate)(iname* po, IQoSList* pIQoSSessions);  \
   int   (*AddRetryCB)(iname* po, AEECallback *pcbRetry);    \
   int   (*GetError)(iname* po, AEEQoSSessionError* pError); \
   /* Extension Function to allow future enhancement to the interface without */    \
   /* breaking its binary compatibility.                                      */    \
   int   (*ExtFunc)(iname* po, AEECLSID id, void *pBuf, int nBufSize)

AEEINTERFACE_DEFINE(IQoSBundle);

/*
   IQoSBundle methods
 */
static __inline int IQoSBundle_AddRef(IQoSBundle *me)
{
   return AEEGETPVTBL(me,IQoSBundle)->AddRef(me);
}

static __inline int IQoSBundle_Release(IQoSBundle *me)
{
   return AEEGETPVTBL(me,IQoSBundle)->Release(me);
}

static __inline int IQoSBundle_QueryInterface(IQoSBundle *me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IQoSBundle)->QueryInterface(me, cls, ppo);
}

static __inline int IQoSBundle_SelectNetworkEx(IQoSBundle *me, int nNetwork, int16 nFamily)
{
   return AEEGETPVTBL(me,IQoSBundle)->SelectNetworkEx(me, nNetwork, nFamily);
}

static __inline int IQoSBundle_Open(IQoSBundle* me, IQoSList* pIQoSSessions, IQoSList* pIQoSSpecs)
{
   return AEEGETPVTBL(me,IQoSBundle)->Open(me, pIQoSSessions, pIQoSSpecs);
}

static __inline int IQoSBundle_OpenEx(IQoSBundle* me, IQoSList* pIQoSSessions, 
                                      IQoSList* pIQoSSpecs, AEEQoSBundleRequestOpcode opcode)
{
   AEEQoSBundleOpenEx qosBundleOpenExInfo;

   qosBundleOpenExInfo.pIQoSSessions = pIQoSSessions;
   qosBundleOpenExInfo.pIQoSSpecs    = pIQoSSpecs;
   qosBundleOpenExInfo.opcode        = opcode;

   return AEEGETPVTBL(me,IQoSBundle)->ExtFunc(me, QOSBUNDLE_EXTFUNC_OPEN_EX, 
                                              (void*)&qosBundleOpenExInfo, sizeof(AEEQoSBundleOpenEx));
}

static __inline int IQoSBundle_GoActive(IQoSBundle *me, IQoSList* pIQoSSessionList)
{
   return AEEGETPVTBL(me,IQoSBundle)->GoActive(me, pIQoSSessionList);
}

static __inline int IQoSBundle_Close(IQoSBundle* me, IQoSList* pIQoSSessionList)
{
   return AEEGETPVTBL(me,IQoSBundle)->Close(me, pIQoSSessionList);
}

static __inline int IQoSBundle_Deactivate(IQoSBundle* me, IQoSList* pIQoSSessionList)
{
   return AEEGETPVTBL(me,IQoSBundle)->Deactivate(me, pIQoSSessionList);
}

static __inline int IQoSBundle_AddRetryCB(IQoSBundle* me, AEECallback *pcbRetry)
{
   return AEEGETPVTBL(me,IQoSBundle)->AddRetryCB(me, pcbRetry);
}

static __inline int IQoSBundle_GetError(IQoSBundle* me, AEEQoSSessionError* pError)
{
   return AEEGETPVTBL(me,IQoSBundle)->GetError(me, pError);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================
AEEQoSSessionError

Description:
   AEEQoSSessionError can be used by the client to identify a QoS Session 
   within a group of QoS Sessions (QoS Bundle), which is either invalid or 
   prevented from an operation to be performed successfully on the entire 
   QoS Sessions group.

   When an operation on multiple QoS Sessions (IQoSBundle_Open(), IQoSBundle_OpenEx(),
   IQoSBundle_Close(), IQoSBundle_GoActive(), IQoSBundle_Deactivate()) fails
   with an error value AEE_NET_EQOS_SESSION_INVALID_OPT, the client can call
   IQoSBundle_GetError() to receive the AEEQoSSessionError. The nIndex member of
   AEEQoSSessionError will indicate which QoS Session in the list caused the 
   failure. The eErrorCode member of AEEQoSSessionError will reveal the reason
   for the failure.

   If an AEEQoSSessionError member is not applicable, it is marked as “unspecified”. 
   (nIndex = AEE_QOS_SESSION_ERR_INDEX_UNSPEC)
  
Definition:
   typedef struct AEEQoSSessionError 
   {
      AEEQoSSessionErrorCode eErrorCode;
      int32 nIndex;
   } AEEQoSSessionError;

Members:
   eErrorCode: type of error
   nIndex:     index of the QoS Session which caused the error within the 
               QoS Sessions list

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSBundle_GetError()
   AEEQoSSessionErrorCode 
              
=======================================================================
AEEQoSSessionErrorCode

Description:
   Type of error in an AEEQoSSessionError struct.

Definition:
   #define AEE_QOS_SESSION_ERR_CODE_SUCCESS       0 
   #define AEE_QOS_SESSION_ERR_CODE_INVAL_SPEC    1
   #define AEE_QOS_SESSION_ERR_CODE_INVAL_SESSION 2
   #define AEE_QOS_SESSION_ERR_CODE_BADSTATE      3
   #define AEE_QOS_SESSION_ERR_CODE_SELECT_NET    4
   #define AEE_QOS_SESSION_ERR_CODE_NET_CONFLICT  5 

   typedef int32 AEEQoSSessionErrorCode;
  
  
Members:
   AEE_QOS_SESSION_ERR_CODE_SUCCESS:     No error
   AEE_QOS_SESSION_ERR_CODE_INVAL_SPEC:  Invalid QoS Specification. In this
      case the client can refer to the erroneous QoS Session in the list (using
      the nIndex member of AEEQoSSessionError) and call IQoSSession_GetError()
      to receive more information on the QoS Specification error.
   AEE_QOS_SESSION_ERR_CODE_INVAL_SESSION: Unspecified error.
   AEE_QOS_SESSION_ERR_CODE_BADSTATE:      The QoS Session state is not suitable 
      for the requested operation.
   AEE_QOS_SESSION_ERR_CODE_SELECT_NET:   Error trying to select the QoS 
      Session's network.
   AEE_QOS_SESSION_ERR_CODE_NET_CONFLICT: Conflict between the QoS Session's 
      network and the QoS Bundle's selected network.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEEQoSSessionError
   IQoSSession_GetError()

=======================================================================
AEEQoSBundleRequestOpcode

Description:
   AEEQoSBundleRequestOpcode is an input parameter to IQoSBundle_OpenEx().
   It indicates whether the requested QoS sessions are to be granted immediately
   or stored and granted at a later time when required.

Definition:
   #define AEE_QOS_BUNDLE_REQUEST_OP   1
   #define AEE_QOS_BUNDLE_CONFIGURE_OP 2

   typedef int AEEQoSBundleRequestOpcode;
    
Members:
   AEE_QOS_BUNDLE_REQUEST_OP:    Store the QoS sessions and grant them immediately.
   AEE_QOS_BUNDLE_CONFIGURE_OP:  Store the QoS sessions and grant them at a 
                                 later time by calling IQoSBundle_GoActive().
Comments:
   None

Version:
   Introduced BREW Client 4.0.1

See Also:   
   IQoSBundle_OpenEx()
   IQoSBundle_GoActive()

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IQoSBundle

This interface provides API for operating on multiple IQoSSession objects, 
which are all linked to the same data network.
The IQoSBundle object can be used for a single bundle operation 
(e.g IQoSBundle_Open,IQoSBundle_Close) but can also be reused for various 
operations on different bundles of IQoSSession objects - it is not limited to a 
specific bundle of IQoSSeesion objects.

An IQoSSession object can be used with and without IQoSBundle intermittently. 
For example, BREW application can request multiple QoS Sessions using IQoSBundle
and then release them directly (without IQoSBundle) one by one.
It can also request multiple QoS Sessions directly (without IQoSBundle) one by
one and then release them using IQoSBundle. See Also: IQoSSeesion, IQoSList.

For requesting multiple sessions, the client needs to:
-  Create a list (IQoSList) of IQoSSession objects.
-  Register a listener callback for each QoS Session object 
   (IQoSSession_AddListener()) for receiving QoS events. 
-  Create a new IQoSBundle object or use an already existing IQoSBundle object.
-  Select the desired data network to be used for all QoS links,
   by calling IQoSBundle_SelectNetworkEx().
-  Call IQoSBundle_Open() with a list containing the IQoSSession objects and 
   with appropriate list of desired IQoSSpec objects for each QoS Session.
   IQoSBundle_Open() performs asynchronous operation and hence a return 
   value of AEE_SUCCESS only means that the QoS request have been submitted. 
   In the case where IQoSBundle_Open() returns AEE_NET_WOULDBLOCK, 
   use IQoSBundle_AddRetryCB() to register a callback which will be resumed 
   when the IQoSBundle_Open() operation should be retried.
-  The final outcome is later indicated by a QoS event for each requested QoS.
   The QoS events will be delivered separately for each QoS Session through the
   listener callbacks that were previously registered.
   
For releasing multiple sessions which are not needed anymore, the client needs to:
-  Create a new IQoSBundle object or use an already existing IQoSBundle object.
-  Call IQoSBundle_Close() with a list of valid QoS Sessions to be released. 
   IQoSBundle_Close() performs asynchronous operation and hence a return value 
   of AEE_SUCCESS only means that the QoS request has been submitted.
-  The final outcome is later indicated by a QoS event for each requested QoS.
   The QoS events will be delivered separately for each QoS Session through the
   listener callbacks that were previously registered.
   
For suspending multiple sessions, the client needs to:
-  Create a new IQoSBundle object or use an already existing IQoSBundle object.
-  Call IQoSBundle_Deactivate() with a list of valid QoS Sessions to be suspended.
   IQoSBundle_Deactivate() performs asynchronous operation and hence a return 
   value of AEE_SUCCESS only means that the QoS request has been submitted.
-  The final outcome is later indicated by a QoS event for each requested QoS.
   The QoS events will be delivered separately for each QoS Session through the
   listener callbacks that were previously registered.

For resuming multiple sessions, the client needs to:
-  Create a new IQoSBundle object or use an already existing IQoSBundle object.
-  Call IQoSBundle_GoActive() with a list of valid QoS Sessions to be resumed.
   IQoSBundle_GoActive() performs asynchronous operation and hence a return 
   value of AEE_SUCCESS only means that the QoS request has been submitted.
-  The final outcome is later indicated by a QoS event for each requested QoS.
   The QoS events will be delivered separately for each QoS Session through the
   listener callbacks that were previously registered.
    
For retrieving the last error in an IQoSBundle operation, 
use IQoSBundle_GetError().


NOTE: Your application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

The following header file is required:~
   BREW 4.0 - AEEIQoSBundle.h
   BREW 3.1 - AEEQoSBundle.h

=============================================================================

IQoSBundle_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IQoSBundle_Release()

=============================================================================

IQoSBundle_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IQoSBundle_AddRef()

==============================================================================

IQoSBundle_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

=======================================================================

IQoSBundle_SelectNetworkEx()

Description:
   This function selects a specific data network to be associated
   with the QoS Bundle. 

   Each QoS Session is associated to a data network. 
   IQoSBundle_SelectNetworkEx() will not change the QoS Sessions' data network.

   Upon IQoSBundle_Open()/IQoSBundle_OpenEx(), each QoS Session within the bundle must either
   be associated to a data network which is identical to the bundle's selected 
   network, or not specify a network at all. Otherwise the Open request will fail.
   
   When an IQoSBundle instance is created, no network is selected. If 
   IQoSBundle_Open()/IQoSBundle_OpenEx() is called before IQoSBundle_SelectNetworkEx(), 
   AEE_NETWORK_DEFAULT will be implicitly selected.

   The IQoSBundle instance is re-usable, therefor the client may call
   IQoSBundle_SelectNetworkEx() multiple times on the same bundle instance. 

Prototype:
   int IQoSBundle_SelectNetworkEx(IQoSBundle* po, int nNetwork, int16 nFamily);

Parameters:
   po:         the interface pointer
   nNetwork:   data network type (AEE_NETWORK_*)
   nFamily:    address family (AEE_AF_*)

Return Value:
   AEE_SUCCESS: the data network was selected.
   AEE_NET_EINVAL: the network is not valid.
   AEE_EBADSTATE: the operation is currently not applicable.
   AEE_EFAILED: other failure.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSBundle_Open()
   IQoSBundle_OpenEx()
   Data networks

=======================================================================

IQoSBundle_Open()

Description:
   This function issues request for establishing new multiple QoS Sessions 
   linked to the same data network and expects the QoS Sessions to be granted 
   immediately. The request is forwarded to lower layers in 
   order to establish the new QoS links.

   The number of requested QoS Sessions in a bundle is limited and cannot
   exceed MAX_UINT8.

   The QoS Bundle's network will be used as the data network for all the
   QoS Sessions in the bundle. Each requested QoS Session in the bundle, must 
   either specify a network which is identical to the bundle's network 
   or not specify a network at all. In any other case the operation will fail 
   and none of the QoS Sessions will be established. 
   
   Upon an AEE_SUCCESS Open request, all QoS Sessions within the bundle will be 
   associated to the same data network as the bundle.
   
   If a network was not yet selected for the IQoSBundle instance 
   (IQoSBundle_SelectNetworkEx()), AEE_NETWORK_DEFAULT will be implicitly 
   selected.
  
   Note that a return value of AEE_SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that all QoS Sessions are 
   already available.
   After IQoSBundle_Open() returns AEE_SUCCESS, the client should listen (see 
   IQoSSession_AddListener()) for QoS events, separately for each QoS Session,
   which indicate success (AEE_QOS_AVAILABLE_MODIFIED_EV).

   Also note the possibility of an AEE_NET_WOULDBLOCK return value. This occurs
   in cases where a network connection, such as a PPP link, must be established.
   In that case, IQoSBundle_Open() should be called again to obtain the final 
   result (error or success). For notification of when to call IQoSBundle_Open() 
   again, the client should register a callback thru IQoSBundle_AddRetryCB(). 
   Note that when the client's callback is resumed, it is not guaranteed that 
   a subsequent call to IQoSBundle_Open()/IQoSBundle_OpenEx() will complete,
   so the client must be prepared to receive AEE_NET_WOULDBLOCK again.
   
   After calling IQoSBundle_Open(), the QoS Bundle object can be re-used for
   other multiple session operations with any bundle of IQoSSession objects.
  
Prototype:
   int IQoSBundle_Open(IQoSBundle* po, IQoSList* pIQoSSessions, IQoSList* pIQoSSpecs);

Parameters:
   po: the interface pointer
   pIQoSSessions: list of requested QoS sessions.
   pIQoSSpecs: list of desired QoS specification for the new sessions.

Return Value:
   AEE_SUCCESS:
      the request was successfully issued. This doesn't indicate that the QoS 
      Sessions are already available.

   AEE_NET_WOULDBLOCK:
      operation cannot be completed right now; use
      IQoSBundle_AddRetryCB() to try again later.

   AEE_EBADPARM: 
      the QoS Session list or QoS Spec list are invalid,
      or are not of the same length.

   AEE_NET_EQOS_SESSION_INVALID_OPT:
      at least one of the QoS Session in the list is invalid or prevented the
      Open operation from ending successfully e.g. the QoSSession state is 
      not suitable for this operation, the QoSSession's network does not match
      the QoS Bundle's network, the QoS Specification is invalid etc.
      Use IQoSBundle_GetError() to identify the index to the first QoS Session 
      in the list which caused the failure and to reveal the error code 
      (See AEEQoSSessionError).

   AEE_NET_EQOSUNAWARE :
      the current network is CDMA QoS Unaware System (does not support QoS).

   AEE_ENOMEMORY: 
      out of memory.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSBundle_OpenEx()
   IQoSBundle_AddRetryCB()
   IQoSBundle_Close()
   IQoSBundle_SelectNetworkEx()
   IQoSBundle_GetError()
   IQoSSession_GetError()
   AEEQoSSessionErrorCode

=======================================================================
  
IQoSBundle_OpenEx()

Description:
   This function issues request for establishing new multiple QoS Sessions 
   linked to the same data network. The request is forwarded to lower layers in 
   order to establish the new QoS links.

   The application can specify whether the QoS Sessions are to be granted 
   immediately (AEE_QOS_BUNDLE_REQUEST_OP) or stored and granted at a 
   later time by calling IQoSBundle_GoActive() (AEE_QOS_BUNDLE_CONFIGURE_OP).

   The number of requested QoS Sessions in a bundle is limited and cannot
   exceed MAX_UINT8.

   The QoS Bundle's network will be used as the data network for all the
   QoS Sessions in the bundle. Each requested QoS Session in the bundle, must 
   either specify a network which is identical to the bundle's network 
   or not specify a network at all. In any other case the operation will fail 
   and none of the QoS Sessions will be established. 
   
   Upon an AEE_SUCCESS Open request, all QoS Sessions within the bundle will be 
   associated to the same data network as the bundle.
   
   If a network was not yet selected for the IQoSBundle instance 
   (IQoSBundle_SelectNetworkEx()), AEE_NETWORK_DEFAULT will be implicitly 
   selected.
  
   Note that a return value of AEE_SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that all QoS Sessions are 
   already available.
   After IQoSBundle_OpenEx() returns AEE_SUCCESS, the client should listen (see 
   IQoSSession_AddListener()) for QoS events, separately for each QoS Session,
   which indicate success:
   * For AEE_QOS_BUNDLE_REQUEST_OP the application should expect to receive 
     AEE_QOS_AVAILABLE_MODIFIED_EV.
   * For AEE_QOS_BUNDLE_CONFIGURE_OP the application should expect to receive 
     either AEE_QOS_AVAILABLE_DEACTIVATED_EV or AEE_QOS_AVAILABLE_MODIFIED_EV. 
     The latter event is received if the network grants QoS before application 
     requests the network to grant QoS using IQoSBundle_GoActive().

   Also note the possibility of an AEE_NET_WOULDBLOCK return value. This occurs
   in cases where a network connection, such as a PPP link, must be established.
   In that case, IQoSBundle_Open() should be called again to obtain the final 
   result (error or success). For notification of when to call IQoSBundle_Open() 
   again, the client should register a callback through IQoSBundle_AddRetryCB(). 
   Note that when the client's callback is resumed, it is not guaranteed that 
   a subsequent call to IQoSBundle_Open()/IQoSBundle_OpenEx() will complete, 
   so the client must be prepared to receive AEE_NET_WOULDBLOCK again.
   
   After calling IQoSBundle_OpenEx(), the QoS Bundle object can be re-used for
   other multiple session operations with any bundle of IQoSSession objects.
  
Prototype:
   int IQoSBundle_OpenEx(IQoSBundle* me, IQoSList* pIQoSSessions, 
                         IQoSList* pIQoSSpecs, AEEQoSBundleRequestOpcode opcode);

Parameters:
   me:            the interface pointer
   pIQoSSessions: list of requested QoS sessions.
   pIQoSSpecs:    list of desired QoS specification for the new sessions.
   opcode:        whether the requested QoS sessions are to be granted immediately
                  or stored and granted at a later time when required.

Return Value:
   AEE_SUCCESS:
      the request was successfully issued. This doesn't indicate that the QoS 
      Sessions are already available.

   AEE_NET_WOULDBLOCK:
      operation cannot be completed right now; use
      IQoSBundle_AddRetryCB() to try again later.

   AEE_EBADPARM: 
      either the opcode, QoS Session list or QoS Spec list are invalid      
      or the QoS Session list and QoS Spec list are not of the same length

   AEE_NET_EQOS_SESSION_INVALID_OPT:
      at least one of the QoS Session in the list is invalid or prevented the
      Open operation from ending successfully e.g. the QoSSession state is 
      not suitable for this operation, the QoSSession's network does not match
      the QoS Bundle's network, the QoS Specification is invalid etc.
      Use IQoSBundle_GetError() to identify the index to the first QoS Session 
      in the list which caused the failure and to reveal the error code 
      (See AEEQoSSessionError).

   AEE_NET_EQOSUNAWARE :
      the current network is CDMA QoS Unaware System (does not support QoS).

   AEE_ENOMEMORY: 
      out of memory.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client <4.0.1>

See Also:
   IQoSBundle_Open()
   IQoSBundle_AddRetryCB()
   IQoSBundle_Close()
   IQoSBundle_SelectNetworkEx()
   IQoSBundle_GoActive()
   IQoSBundle_GetError()
   IQoSSession_GetError()
   AEEQoSSessionErrorCode
   AEEQoSBundleRequestOpcode

=======================================================================

IQoSBundle_Close()

Description:
   This function issues a request for closing multiple QoS Sessions. 
   The request is forwarded to lower layers in order to release the QoS links
   resources.

   The number of requested QoS Sessions in a bundle is limited and cannot
   exceed MAX_UINT8.
  
   All QoS Sessions which are requested to be closed must be linked to the
   same network, otherwise the operation will fail and none of the QoS Session
   instances will be released.
  
   In addition all QoS Sessions which are requested to be closed must be 
   closeable, otherwise the operation will fail and none of the QoS Session 
   instances will be released.

   Note that a return value of AEE_SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that all QoS Sessions are 
   released.

   After IQoSBundle_Close() returns AEE_SUCCESS, the client should listen (see 
   IQoSSession_AddListener()) for QoS events, separately for each QoS Session,  
   which indicate that QoS was released (AEE_QOS_UNAVAILABLE_EV).

   After calling IQoSBundle_Close(), the QoS Bundle object can be re-used for
   any other multiple session operations with any bundle of IQoSSession objects.
  
Prototype:
   int IQoSBundle_Close(IQoSBundle* po, IQoSList* pIQoSSessions);

Parameters:
   po: the interface pointer
   pIQoSSessions: list of valid QoS sessions to be released.

Return Value:
   AEE_NET_SUCCESS: 
      the request was successfully issued. This doesn't indicate that all
      QoS resources are released yet.

   AEE_EBADPARM:
      the QoS Session list is invalid.
          
   AEE_NET_EQOS_NET_CONFLICT: 
      conflict between the QoS Session's networks.

   AEE_NET_EQOS_SESSION_INVALID_OPT: 
      at least one of the QoS Session in the list 
      is invalid or prevented the Close operation from ending successfully 
      e.g. the QoSSession state is not suitable for this operation, 
      Use IQoSBundle_GetError() to identify the index to the first QoS Session 
      in the list which caused the failure and to reveal the error code 
      (See AEEQoSSessionError).

   AEE_EBADSTATE: 
      QoSSession state is not suitable for this operation.

   AEE_ENOMEMORY: 
      out of memory.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSBundle_Open()
   IQoSBundle_OpenEx()
   IQoSBundle_GetError()

=======================================================================

IQoSBundle_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   IQoSBundle_Open()/IQoSBundle_OpenEx() operation should be retried.

   In the case where IQoSBundle_Open()/IQoSBundle_OpenEx() returns
   AEE_NET_WOULDBLOCK, IQoSBundle_Open() should be called again later to obtain
   the final result (error or success).
   For notification of when to call IQoSBundle_Open()/IQoSBundle_OpenEx() again, 
   the client should register a callback through IQoSBundle_AddRetryCB().
   Note that when the client's callback is resumed, it is not guaranteed that
   a subsequent call to IQoSBundle_Open() will complete,
   so the client must be prepared to receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IQoSBundle_AddRetryCB(IQoSBundle *po, AEECallback* pcbRetry);

Parameters:
   po: the interface pointer
   pcbRetry: the client's callback.

Return Value:
   AEE_SUCCESS: 
      client's callback was stored successfully.

   AEE_EBADSTATE: 
      QoSBundle state is not suitable for this operation.

   AEE_EBADPARM: 
      client's callback is invalid.

  Other error codes are also possible

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSBundle_Open()
   IQoSBundle_OpenEx()

=======================================================================

IQoSBundle_GoActive()

Description:
   This function is used in order to activate multiple QoS links.

   The number of requested QoS Sessions in a bundle is limited and cannot
   exceed MAX_UINT8.
  
   All QoS Sessions which are requested to be activated must be linked to the
   same network, otherwise the operation will fail and none of the QoS Session
   instances will be activated.
  
   In addition all QoS Sessions which are requested to be activated must be 
   in a state which enables activation, otherwise the operation will fail and 
   none of the QoS Session instances will be activated.

   Note that a return value of AEE_SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that the QoS Sessions are 
   already active.
   After IQoSBundle_GoActive() returns AEE_SUCCESS, the client should listen (see 
   IQoSSession_AddListener()) for QoS events, separately for each QoS Session,  
   which indicate that QoS was activated (AEE_QOS_AVAILABLE_MODIFIED_EV).

   After calling IQoSBundle_GoActive(), the QoS Bundle object can be re-used for
   any other multiple session operations with any bundle of IQoSSession objects.

Prototype:
   int IQoSBundle_GoActive(IQoSBundle* po, IQoSList* pIQoSSessions);

Parameters:
   po: the interface pointer.
   pIQoSSessions: list of valid QoS sessions to be activated.

Return Value:
   AEE_SUCCESS:
      the request was successfully issued. This doesn't indicate that the QoS
      Sessions are already available.

   AEE_EBADPARM:
      the QoS Session list is invalid.
  
   AEE_NET_EQOS_NET_CONFLICT: 
      conflict between the QoS Session's networks.
    
   AEE_NET_EQOS_SESSION_INVALID_OPT: 
      at least one of the QoS Session in the list is invalid or prevented the 
      Activate operation from ending successfully e.g. the QoSSession state is 
      not suitable for this operation. Use IQoSBundle_GetError() to identify 
      the index to the first QoS Session in the list which caused the failure 
      and to reveal the error code (See AEEQoSSessionError).

   AEE_NET_EQOSUNAWARE :
      the current network is CDMA QoS Unaware System (does not support QoS).
      
   AEE_ENOMEMORY: 
     out of memory.
  
   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSBundle_OpenEx()
   IQoSBundle_GetError()
   AEEQoSSessionError

=======================================================================

IQoSBundle_Deactivate()

Description:
   This function is used in order to suspend multiple QoS links.

   The number of requested QoS Sessions in a bundle is limited and cannot
   exceed MAX_UINT8.
  
   All QoS Sessions which are requested to be suspended must be linked to the
   same network, otherwise the operation will fail and none of the QoS Session
   instances will be suspended.

   In addition all QoS Sessions which are requested to be suspended must be 
   in a state which enables suspension, otherwise the operation will fail 
   and none of the QoS Session instances will be suspended.

   Note that a return value of AEE_SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that the QoS Sessions are 
   already suspended.
   After IQoSBundle_Deactivate() returns AEE_SUCCESS, the client should listen (see 
   IQoSSession_AddListener()) for QoS events, separately for each QoS Session,
   which indicate that QoS was suspended 
   (DSS_IFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV).

   After calling IQoSBundle_Deactivate(), the QoS Bundle object can be re-used for
   any other multiple session operations with any bundle of IQoSSession objects.

Prototype:
   int IQoSBundle_Deactivate(IQoSBundle* po, IQoSList* pIQoSSessions);

Parameters:
   po: the interface pointer.
   pIQoSSessions: list of valid QoS sessions to be suspended.

Return Value:
   AEE_SUCCESS:
      the request was successfully issued. This doesn't indicate that the QoS
      Sessions are already suspended.

   AEE_EBADPARM:
      the QoS Session list is invalid.
  
   AEE_NET_EQOS_NET_CONFLICT: 
      conflict between the QoS Session's networks.
    
   AEE_NET_EQOS_SESSION_INVALID_OPT: 
      at least one of the QoS Session in the list is invalid or prevented the 
      Deactivate operation from ending successfully e.g. the QoSSession state is 
      not suitable for this operation, Use IQoSBundle_GetError() to identify 
      the index to the first QoS Session in the list which caused the failure 
      and to reveal the error code (See AEEQoSSessionError).
      
   AEE_NET_EQOSUNAWARE :
      the current network is CDMA QoS Unaware System (does not support QoS).

   EAEE_NOMEMORY: 
      out of memory.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSBundle_GetError()
   AEEQoSSessionError

=======================================================================

IQoSBundle_GetError()

Description:
   This function returns an AEEQoSSessionError that can be used by clients in
   order to receive information on the reason for the failure.
  
   Clients should call this function after a QoS Bundle operation (IQoSBundle_Open(),
   IQoSBundle_OpenEx(), IQoSBundle_Close(), IQoSBundle_GoActive(),
   IQoSBundle_Deactivate()) returns AEE_NET_EQOS_SESSION_INVALID_OPT.
   In this case the AEEQoSSessionError will indicate which QoS Session in the
   bundle caused the failure and what was the type of the error. In case the 
   returned error code is AEE_QOS_SESSION_ERR_CODE_INVAL_SPEC the 
   client can refer to the appropriate QoS Session in the QoS Sessions list
   (according to the AEEQoSSessionError index value) and call IQoSSession_GetError()
   to receive more information on the QoS Specification error.

   If an AEEQoSSessionError member is not applicable, its index will be marked
   as AEE_QOS_SESSION_ERR_INDEX_UNSPEC.
  
    
Prototype:
   int IQoSSession_GetError(IQoSBundle* po, AEEQoSSessionError* pError);
      
Parameters:
   po: the interface pointer
   pError [out]: returns a QoS Session error.
        
Return Value:
   AEE_SUCCESS: error was successfully retrieved into pError.
   AEE_EBADPARM: the provided pError parameter is invalid.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEEQoSSessionError.

=======================================================================*/
#endif // AEEIQOSBUNDLE_H

