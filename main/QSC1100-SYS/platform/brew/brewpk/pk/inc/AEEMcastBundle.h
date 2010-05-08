#ifndef AEEMCASTBUNDLE_H
#define AEEMCASTBUNDLE_H
/*====================================================

FILE: AEEMcastBundle.h

SERVICES:  BREW Multicast Bundle Interface

GENERAL DESCRIPTION:
   Interface definitions, data structures, etc. 
   for BREW Mcast Bundle Interface.
   The headers are in beta form and may be changed in future releases.

        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE.h"  // for AEECallback
#include "AEEMcastSession2.h" 

// Error codes
#define AEE_IMCASTBUNDLE_E                           (AEEIID_MCASTBUNDLE<<5)
#define AEE_IMCASTBUNDLE_EMCAST_SESSION_INVALID_OPT  (AEE_IMCASTBUNDLE_E+0) // Invalid options in a Mcast Session

// Multicast Session Error
#define AEE_MCAST_SESSION_ERR_CODE_SUCCESS       (AEE_IMCASTBUNDLE_E+1)
#define AEE_MCAST_SESSION_ERR_CODE_BADPARAM      (AEE_IMCASTBUNDLE_E+2)
#define AEE_MCAST_SESSION_ERR_CODE_BADSTATE      (AEE_IMCASTBUNDLE_E+3)
#define AEE_MCAST_SESSION_ERR_CODE_SELECT_NET    (AEE_IMCASTBUNDLE_E+4)
#define AEE_MCAST_SESSION_ERR_CODE_NET_CONFLICT  (AEE_IMCASTBUNDLE_E+5)


typedef int32 AEEMcastSessionErrorCode;

#define AEE_MCAST_SESSION_ERR_INDEX_UNSPEC (-1)

typedef struct AEEMcastSessionError 
{
   AEEMcastSessionErrorCode nErrorCode;
   int32 nIndex;
} AEEMcastSessionError;

//-----------------------------------------------------------------
// Interface
//-----------------------------------------------------------------

// Interface ID
#define AEEIID_MCASTBUNDLE 0x0106d84e

typedef struct IMcastBundle IMcastBundle;

AEEINTERFACE(IMcastBundle)
{
   INHERIT_IQueryInterface(IMcastBundle);
   
   int   (*SelectNetworkEx)(IMcastBundle* me, int nNetwork, int16 nFamily);
   int   (*Join)(IMcastBundle* me, IMcastSession2* aMcastSessions[], 
                 void* apGroupAddresses[], boolean abRegSetupFlags[], int nSize);
   int   (*Leave)(IMcastBundle* me, IMcastSession2* aMcastSessions[], int nSize);
   int   (*Register)(IMcastBundle* me, IMcastSession2* aMcastSessions[], int nSize);
   int   (*AddRetryCB)(IMcastBundle* me, AEECallback* pcbRetry);
   int   (*GetError)(IMcastBundle* me, AEEMcastSessionError* pError);
   
   // Extension Function to allow future enhancement to the interface without 
   // breaking its binary compatibility.
   int   (*ExtFunc)(IMcastBundle* me, AEECLSID id, void *pBuf, int nBufSize);
};

//-----------------------------------------------------------------
// Methods
//-----------------------------------------------------------------

static __inline uint32 IMCASTBUNDLE_AddRef(IMcastBundle *me)
{
   return AEEGETPVTBL(me, IMcastBundle)->AddRef(me);
}

static __inline uint32 IMCASTBUNDLE_Release(IMcastBundle *me)
{
   return AEEGETPVTBL(me, IMcastBundle)->Release(me);
}

static __inline int IMCASTBUNDLE_QueryInterface(IMcastBundle *me, 
                                                AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me, IMcastBundle)->QueryInterface(me, cls, ppo);
}

static __inline int IMCASTBUNDLE_SelectNetworkEx(IMcastBundle *me, int nNetwork, int16 nFamily)
{
   return AEEGETPVTBL(me, IMcastBundle)->SelectNetworkEx(me, nNetwork, nFamily);
}

static __inline int IMCASTBUNDLE_Join(IMcastBundle* me, IMcastSession2* aMcastSessions[], 
                                      void* apGroupAddresses[], boolean abRegSetupFlags[], int nSize)
{
   return AEEGETPVTBL(me, IMcastBundle)->Join(me, aMcastSessions, apGroupAddresses, abRegSetupFlags, nSize);
}

static __inline int IMCASTBUNDLE_Leave(IMcastBundle* me, IMcastSession2* aMcastSessions[], int nSize)
{
   return AEEGETPVTBL(me, IMcastBundle)->Leave(me, aMcastSessions, nSize);
}

static __inline int IMCASTBUNDLE_Register(IMcastBundle* me, IMcastSession2* aMcastSessions[], int nSize)
{
   return AEEGETPVTBL(me, IMcastBundle)->Register(me, aMcastSessions, nSize);
}

static __inline int IMCASTBUNDLE_AddRetryCB(IMcastBundle* me, AEECallback* pcbRetry)
{
   return AEEGETPVTBL(me, IMcastBundle)->AddRetryCB(me, pcbRetry);
}

static __inline int IMCASTBUNDLE_GetError(IMcastBundle* me, AEEMcastSessionError* pError)
{
   return AEEGETPVTBL(me,IMcastBundle)->GetError(me, pError);
}

/*=====================================================================
DATA STRUCTURE DOCUMENTATION
======================================================================
AEEMcastSessionError

Description:
   AEEMcastSessionError can be used by the client to identify a Mcast Session 
   within a group of Mcast Sessions (Mcast Bundle), which is either invalid or 
   prevented from an operation to be performed successfully on the entire 
   Mcast Sessions group.

   When an operation on multiple Mcast Sessions (IMCASTBUNDLE_Join(), IMCASTBUNDLE_Leave(), 
   IMCASTBUNDLE_Register()) fails with an error value AEE_IMCASTBUNDLE_EMCAST_SESSION_INVALID_OPT, 
   the client can call IMCASTBUNDLE_GetError() to receive the AEEMcastSessionError. 
   The nIndex member of AEEMcastSessionError will indicate which Mcast Session in the 
   array caused the failure. The nErrorCode member of AEEMcastSessionError will reveal 
   the reason for the failure.

   If an AEEMcastSessionError member is not applicable, it is marked as "unspecified". 
   (nIndex = AEE_MCAST_SESSION_ERR_INDEX_UNSPEC)

Definition:
   typedef struct AEEMcastSessionError 
   {
      AEEMcastSessionErrorCode nErrorCode;
      int32 nIndex;
   } AEEMcastSessionError;

Members:
   nErrorCode: type of error
   nIndex:     index of the Mcast Session which caused the error within the 
               Mcast Sessions list

Comments:
   None

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTBUNDLE_GetError()
   AEEMcastSessionErrorCode 

=======================================================================
AEEMcastSessionErrorCode

Description:
   Type of error in an AEEMcastSessionError struct.

Definition:
   #define AEE_MCAST_SESSION_ERR_CODE_SUCCESS       0 
   #define AEE_MCAST_SESSION_ERR_CODE_BADPARAM      1
   #define AEE_MCAST_SESSION_ERR_CODE_BADSTATE      2
   #define AEE_MCAST_SESSION_ERR_CODE_SELECT_NET    3
   #define AEE_MCAST_SESSION_ERR_CODE_NET_CONFLICT  4 

typedef int32 AEEMcastSessionErrorCode;


Members:
   AEE_MCAST_SESSION_ERR_CODE_SUCCESS:   No error
   AEE_MCAST_SESSION_ERR_CODE_BADPARAM:  Invalid parameter. In this
      case the client can refer to the erroneous parameter in the array (using
      the nIndex member of AEEMcastSessionError).
   AEE_MCAST_SESSION_ERR_CODE_BADSTATE: The Mcast Session state is not suitable 
      for the requested operation.
   AEE_MCAST_SESSION_ERR_CODE_SELECT_NET: Error trying to select the Mcast 
      Session's network.
   AEE_MCAST_SESSION_ERR_CODE_NET_CONFLICT: Conflict between the Mcast Session's 
      network and the MCAST Bundle's selected network.

Comments:
   None

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   AEEMcastSessionError
   IMCASTSESSION_GetError()

=======================================================================
INTERFACES DOCUMENTATION
=======================================================================

Interface Name: IMcastBundle

   This interface provides API for operating on multiple IMcastSession2 objects, 
   which are all linked to the same data network.
   The IMcastBundle object can be used for a single bundle operation 
   (e.g IMCASTBUNDLE_Join, IMCASTBUNDLE_Leave, IMCASTBUNDLE_Register) but can also 
   be reused for various operations on different bundles of IMcastSession2 objects - 
   it is not limited to a specific bundle of IMcastSession2 objects.

   The number of requested Mcast Sessions in a bundle is limited. Typically more 
   than 25 sessions are not allowed.

   An IMcastSession2 object can be used with and without IMcastBundle intermittently. 
   For example, BREW application can request multiple Mcast Sessions using IMcastBundle
   and then release them directly (without IMcastBundle) one by one.
   It can also request multiple Mcast Sessions directly (without IMcastBundle) one by
   one and then release them using IMcastBundle. See Also: IMcastSession.

   For requesting multiple sessions, the client needs to:
   -  Create an array of IMcastSession2 objects.
   -  Register a listener callback for each Mcast Session object 
      (IMCASTSESSION2_AddListener()) for receiving Mcast events. 
   -  Create a new IMcastBundle object or use an already existing IMcastBundle object.
   -  Select the desired data network to be used for all Mcast sessions,
      by calling IMCASTBUNDLE_SelectNetworkEx().
   -  Call IMCASTBUNDLE_Join() with three arrays: one that containing the IMcastSession2 
      objects, one that contains pointers to the desired multicast group addresses and ports 
      for each Mcast Session, and an array of booleans that its role will be described in 
      the IMCASTBUNDLE_Join operation description. IMCASTBUNDLE_Join() performs asynchronous 
      operation and hence a return value of SUCCESS only means that the Mcast request have 
      been submitted. In the case where IMCASTBUNDLE_Join() returns AEE_NET_WOULDBLOCK, 
      use IMCASTBUNDLE_AddRetryCB() to register a callback which will be resumed 
      when the IMCASTBUNDLE_Join() operation should be retried.
   -  The final outcome is later indicated by a Mcast event for each requested Mcast.
      The Mcast events will be delivered separately for each Mcast Session through the
      listener callbacks that were previously registered.

   For releasing multiple sessions which are not needed anymore, the client needs to:
   -  Create a new IMcastBundle object or use an already existing IMcastBundle object.
   -  Call IMCASTBUNDLE_Leave() with a list of valid Mcast Sessions to be released. 
      IMCASTBUNDLE_Leave() performs asynchronous operation and hence a return value 
      of SUCCESS only means that the Mcast request has been submitted.
   -  The final outcome is later indicated by a Mcast event for each Mcast session.
      The Mcast events will be delivered separately for each Mcast Session through the
      listener callbacks that were previously registered.

   For registering multiple sessions that were previously joined without registration, the 
   client needs to:
   -  Create a new IMcastBundle object or use an already existing IMcastBundle object.
   -  Call IMCASTBUNDLE_Register() with a list of valid Mcast Sessions to be registered.
      IMCASTBUNDLE_Register() performs asynchronous operation and hence a return 
      value of SUCCESS only means that the Mcast request has been submitted.
   -  The final outcome is later indicated by a Mcast event for each requested Mcast.
      The Mcast events will be delivered separately for each Mcast Session through the
      listener callbacks that were previously registered.

   For retrieving the last error in an IMcastBundle operation, 
   use IMCASTBUNDLE_GetError().


   NOTE: Your application must have a privilege level of Network or All to be able
   to invoke the functions in this interface.

   The following header file is required:~
   AEEMcastBundle.h

=============================================================================

IMCASTBUNDLE_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   IMCASTBUNDLE_Release()

=============================================================================

IMCASTBUNDLE_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   IMCASTBUNDLE_AddRef()

==============================================================================

IMCASTBUNDLE_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

=======================================================================

IMCASTBUNDLE_SelectNetworkEx()

Description:
   This function selects a specific data network to be associated
   with the Mcast Bundle. 

   Each Mcast Session is associated to a data network. 
   IMCASTBUNDLE_SelectNetworkEx() will not change the Mcast Sessions' data network.

   Upon IMCASTBUNDLE_Join(), each Mcast Session within the bundle must either be 
   associated to a data network which is identical to the bundle's selected network, 
   or not specify a network at all. Otherwise the Join request will fail.

   When an IMcastBundle instance is created, no network is selected.
   If IMCASTBUNDLE_Join() is called before IMCASTBUNDLE_SelectNetworkEx(), 
   AEE_NETWORK_DEFAULT will be implicitly selected.

Prototype:
   int IMCASTBUNDLE_SelectNetworkEx(IMcastBundle *me, int nNetwork, int16 nFamily)

Parameters:
   me:         the interface pointer
   nNetwork:   data network type (AEE_NETWORK_*)
   nFamily:    address family (AEE_AF_*)

Return Value:
   SUCCESS: the data network was selected.
   AEE_NET_EINVAL: the network is not valid.
   EBADSTATE: the operation is currently not applicable.
   EFAILED: other failure.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTBUNDLE_Join()
   Data networks

=======================================================================

IMCASTBUNDLE_Join()

Description:
   This function issues request for establishing new multiple Mcast Sessions 
   linked to the same data network. The request is forwarded to lower layers 
   in order to establish the new Mcast links.
   
   The Mcast Bundle's network will be used as the data network for all the
   Mcast Sessions in the bundle. Each requested Mcast Session in the bundle, must 
   either specify a network which is identical to the bundle's network 
   or not specify a network at all. In any other case the operation will fail 
   and none of the Mcast Sessions will be established. 

   Upon a SUCCESS Join request, all Mcast Sessions within the bundle will be 
   associated to the same data network as the bundle.

   If a network was not yet selected for the IMcastBundle instance 
   (IMCASTBUNDLE_SelectNetworkEx()), AEE_NETWORK_DEFAULT will be implicitly 
   selected.

   Note that a return value of SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that all Mcast Sessions are 
   already available.
   After IMCASTBUNDLE_Join() returns SUCCESS, the client should listen (see 
   IMCASTSESSION2_AddListener()) for Mcast events, separately for each Mcast Session,
   which indicate the different stages of a success (AEE_MCAST2_STATUS_EV, 
   AEE_MCAST2_REGISTER_SUCCESS_EV).

   Also note the possibility of an AEE_NET_WOULDBLOCK return value. This occurs
   in cases where a network connection, such as a PPP link, must be established.
   In that case, IMCASTBUNDLE_Join() should be called again to obtain the final 
   result (error or success). For notification of when to call IMCASTBUNDLE_Join() 
   again, the client should register a callback through IMCASTBUNDLE_AddRetryCB(). 
   Note that when the client's callback is resumed, it is not guaranteed that 
   a subsequent call to IMCASTBUNDLE_Join() will complete, so the client must be 
   prepared to receive AEE_NET_WOULDBLOCK again.

   After calling IMCASTBUNDLE_Join(), the Mcast Bundle object can be re-used for
   other multiple session operations with any bundle of IMcastSession objects.

Prototype:
   int IMCASTBUNDLE_Join(IMcastBundle* me, IMcastSession2* aMcastSessions[], 
                         void* apGroupAddresses[], boolean abRegSetupFlags[], int nSize);

Parameters:
   me: the interface pointer
   aMcastSessions: array of pointers to requested Mcast sessions.
   apGroupAddresses: array of pointers to desired Multicast group addresses and ports for the 
                     new sessions to join (each element should be a pointer to an initialized 
                     AEESockAddrStorage or AEESockAddrInet structure).
   abRegSetupFlags: array of booleans (registration flags). The registration flag 
                    provides the application some level of control over the registration process. 
                    For example, in some scenarios if the register flag is set to TRUE the 
                    registration message shall *not* be sent to the network in conjunction with 
                    the Join request. The application can, in such scenario, explicitly initiate 
                    the registration process when desired. Please note that lower layers may initiate 
                    the registration process or disable it according to device and/or network 
                    configuration. In such case the registration flags as specified by the application 
                    may not take effect.
   nSize: the number of Mcast Sessions (which should be the same as the size of the 3 arrays).

Return Value:
   SUCCESS:
      the request was successfully issued. This doesn't indicate that the Mcast 
      Sessions are already available.

   AEE_NET_WOULDBLOCK:
      operation cannot be completed right now; use
      IMCASTBUNDLE_AddRetryCB() to try again later.

   EBADPARM: 
      uSize is not valid.

   AEE_IMCASTBUNDLE_EMCAST_SESSION_INVALID_OPT:
      at least one of the Mcast Session in the list is invalid or prevented the
      Join operation from ending successfully e.g. the Mcast Session state is 
      not suitable for this operation, the Mcast Session's network does not match
      the Mcast Bundle's network etc.
      Use IMCASTBUNDLE_GetError() to identify the index to the first Mcast Session 
      in the array which caused the failure and to reveal the error code 
      (See AEEMcastSessionError).

   ENOMEMORY: 
      out of memory.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTBUNDLE_AddRetryCB()
   IMCASTBUNDLE_Leave()
   IMCASTBUNDLE_Register()
   IMCASTBUNDLE_SelectNetworkEx()
   IMCASTBUNDLE_GetError()
   AEEMcastSessionErrorCode

=======================================================================

IMCASTBUNDLE_Leave()

Description:
   This function issues a request for closing multiple Mcast Sessions. 
   The request is forwarded to lower layers in order to release the Mcast links
   resources.

   All Mcast Sessions which are requested to be closed must be linked to the
   same network, otherwise the operation will fail and none of the Mcast Session
   instances will be released.

   In addition all Mcast Sessions which are requested to be closed must be 
   closeable, otherwise the operation will fail and none of the Mcast Session 
   instances will be released.

   Note that a return value of SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that all Mcast Sessions are 
   released.

   After IMCASTBUNDLE_Leave() returns SUCCESS, the client should listen (see 
   IMCASTSESSION2_AddListener()) for Mcast events, separately for each Mcast Session,  
   which indicate that Mcast was released (AEE_MCAST2_DEREGISTERED_EV).

   After calling IMCASTBUNDLE_Leave(), the Mcast Bundle object can be re-used for
   any other multiple session operations with any bundle of IMcastSession objects.

Prototype:
   int IMCASTBUNDLE_Leave(IMcastBundle* me, IMcastSession2* aMcastSessions[], int nSize);

Parameters:
   me: the interface pointer
   aMcastSessions: array of pointers to valid Mcast sessions to be released.
   nSize: the number of Mcast Sessions.

Return Value:
   SUCCESS: 
      the request was successfully issued. This doesn't indicate that all
      Mcast resources are released yet.

   EBADPARM:
      uSize is not valid.

   AEE_IMCASTBUNDLE_EMCAST_SESSION_INVALID_OPT: 
      at least one of the Mcast Session in the array 
      is invalid or prevented the Close operation from ending successfully 
      e.g. the McastSession state is not suitable for this operation, the 
      McastSession's network does not match the Mcast Bundle's network etc. 
      Use IMCASTBUNDLE_GetError() to identify the index to the first Mcast Session 
      in the list which caused the failure and to reveal the error code 
      (See AEEMcastSessionError).

   ENOMEMORY: 
      out of memory.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTBUNDLE_Join()
   IMCASTBUNDLE_GetError()

=======================================================================

IMCASTBUNDLE_Register()

Description:
   This function issues request for registering multiple Mcast Sessions 
   linked to the same data network, that were previously joined with the 
   appropriate abRegSetupFlags entry equals TRUE. The request is forwarded to 
   lower layers in order to establish the new Mcast links.

   The Mcast Bundle's network will be used as the data network for all the
   Mcast Sessions in the bundle. Each requested Mcast Session in the bundle, must 
   either specify a network which is identical to the bundle's network 
   or not specify a network at all. In any other case the operation will fail 
   and none of the Mcast Sessions will be established. 

   Upon a SUCCESS Join request, all Mcast Sessions within the bundle will be 
   associated to the same data network as the bundle.

   Note that a return value of SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that all Mcast Sessions are 
   already available.
   After IMCASTBUNDLE_Register() returns SUCCESS, the client should listen (see 
   IMCASTSESSION2_AddListener()) for AEE_MCAST2_STATUS_EV events, separately for each 
   Mcast Session, with info codes that indicate success or failure.

   After calling IMCASTBUNDLE_Register(), the Mcast Bundle object can be re-used for
   other multiple session operations with any bundle of IMcastSession objects.

Prototype:
   int IMCASTBUNDLE_Register(IMcastBundle* me, IMcastSession2* aMcastSessions[], int nSize); 

Parameters:
   me: the interface pointer
   aMcastSessions: array of pointers to requested Mcast sessions.
   nSize: the number of McastSessions.

Return Value:
   SUCCESS:
      the request was successfully issued. This doesn't indicate that the Mcast 
      Sessions are already available.

   EBADPARM: 
      uSize is not valid.

   AEE_IMCASTBUNDLE_EMCAST_SESSION_INVALID_OPT:
      at least one of the Mcast Session in the list is invalid or prevented the
      Register operation from ending successfully e.g. the McastSession state is 
      not suitable for this operation, the McastSession's network does not match
      the Mcast Bundle's network etc.
      Use IMCASTBUNDLE_GetError() to identify the index to the first Mcast Session 
      in the array which caused the failure and to reveal the error code 
      (See AEEMcastSessionError).

   ENOMEMORY: 
      out of memory.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTBUNDLE_Join()
   IMCASTBUNDLE_GetError()
   AEEMcastSessionErrorCode

=======================================================================

IMCASTBUNDLE_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   IMCASTBUNDLE_Join() operation should be retried.

   In the case where IMCASTBUNDLE_Join() returns AEE_NET_WOULDBLOCK, IMCASTBUNDLE_Join() 
   should be called again later to obtain the final result (error or success).
   For notification of when to call IMCASTBUNDLE_Join() again, 
   the client should register a callback through IMCASTBUNDLE_AddRetryCB(). 
   Note that when the client's callback is resumed, it is not guaranteed that 
   a subsequent call to IMCASTBUNDLE_Join() will complete, 
   so the client must be prepared to receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IMCASTBUNDLE_AddRetryCB(IMcastBundle* me, AEECallback* pcbRetry);

Parameters:
   me: the interface pointer
   pcbRetry: the client's callback.

Return Value:
   SUCCESS: 
      client's callback was stored successfully.

   EBADSTATE: 
      McastBundle state is not suitable for this operation.

   EBADPARM: 
      client's callback is invalid.

   Other error codes are also possible

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTBUNDLE_Join()

=======================================================================

IMCASTBUNDLE_GetError()

Description:
   This function returns an AEEMcastSessionError that can be used by clients in
   order to receive information on the reason for the failure.

   Clients should call this function after a Mcast Bundle operation IMCASTBUNDLE_Join(),
   IMCASTBUNDLE_Leave(), IMCASTBUNDLE_Register() returns AEE_IMCASTBUNDLE_EMCAST_SESSION_INVALID_OPT. 
   In this case the AEEMcastSessionError will indicate which Mcast Session in the 
   bundle caused the failure and what was the type of the error. The client can 
   refer to the appropriate Mcast Session in the Mcast Sessions list 
   (according to the AEEMcastSessionError index value) and call IMCASTSESSION_GetError() 
   to receive more information on the Mcast Specification error.

   If an AEEMcastSessionError member is not applicable, its index will be marked
   as AEE_MCAST_SESSION_ERR_INDEX_UNSPEC.


Prototype:
   int IMCASTSESSION_GetError(IMcastBundle* me, AEEMcastSessionError* pError);

Parameters:
   po: the interface pointer
   pError [out]: returns a Mcast Session error.

Return Value:
   SUCCESS: error was successfully retrieved into pError.
   EBADPARM: the provided pError parameter is invalid.

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   AEEMcastSessionError

=======================================================================*/
#endif    // AEEMCASTBUNDLE_H
