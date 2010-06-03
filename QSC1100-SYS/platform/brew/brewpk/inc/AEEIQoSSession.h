#ifndef AEEIQOSSESSION_H
#define AEEIQOSSESSION_H
/*======================================================
FILE:  AEEIQoSSession.h

SERVICES:
   BREW QoS Session Interface

GENERAL DESCRIPTION:
   Interface definitions, data structures, etc. for BREW Quality Of Service
   Session Interface

   Copyright (c) 2006-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEEIQI.h"
#include "AEEIQoSSpec.h"
#include "AEEIQoSFlow.h"
#include "AEEIModel.h"
#include "AEECallback.h"

#define AEEIID_IQoSSession    0x0103bce1

#define AEE_QOS_AVAILABLE_EV              0
#define AEE_QOS_AVAILABLE_MODIFIED_EV     1
#define AEE_QOS_UNAVAILABLE_EV            2
#define AEE_QOS_AVAILABLE_DEACTIVATED_EV  3
#define AEE_QOS_MODIFY_ACCEPTED_EV        4
#define AEE_QOS_MODIFY_REJECTED_EV        5
#define AEE_QOS_INFO_CODE_UPDATED_EV      6

typedef int AEEQoSEvent;


#define AEE_QOS_STATUS_ACTIVE             0
#define AEE_QOS_STATUS_INACTIVE           1
#define AEE_QOS_STATUS_GOING_ACTIVE       2
#define AEE_QOS_STATUS_GOING_INACTIVE     3
#define AEE_QOS_STATUS_NOT_AVAILABLE      4

typedef int AEEQoSStatus;


#define AEE_QOS_INFO_CODE_NOT_SPECIFIED 0
#define AEE_QOS_INFO_CODE_NOT_SUPPORTED 1
#define AEE_QOS_INFO_CODE_NOT_AVAILABLE 2
#define AEE_QOS_INFO_CODE_NOT_GUARANTEED 3
#define AEE_QOS_INFO_CODE_INVALID_PARAMS 4
#define AEE_QOS_INFO_CODE_INTERNAL_ERROR 5
#define AEE_QOS_INFO_CODE_INSUFFICIENT_NET_RESOURCES 6
#define AEE_QOS_INFO_CODE_INSUFFICIENT_LOCAL_RESOURCES 7
#define AEE_QOS_INFO_CODE_QOS_AWARE_SYSTEM   8
#define AEE_QOS_INFO_CODE_QOS_UNAWARE_SYSTEM 9
#define AEE_QOS_INFO_CODE_QOS_REJECTED_OPERATION  10
#define AEE_QOS_INFO_CODE_QOS_TIMED_OUT_OPERATION 11
#define AEE_QOS_INFO_CODE_QOS_WILL_GRANT_WHEN_QOS_RESUMED 12

typedef int AEEQoSInfoCode;

//
// for IQoSSession_SelectNetworkEx
//
typedef struct AEEQosSesssionSelectNetworkEx
{
   int nNetwork;
   int16 sFamily;
} AEEQosSesssionSelectNetworkEx;

#define QOSSESSION_EXTFUNC_SELECT_NETWORK_EX 1 // dwVal = pointer to AEEQosSesssionSelectNetworkEx
#define QOSSESSION_EXTFUNC_DEACTIVATE        0x0103db77  // dwVal = NULL

//
// for IQoSSession_ModifyEx
//
typedef struct AEEQosSesssionModifyEx
{
   IQoSSpec* piRequestedQoSSpec;
   AEEQoSModifyMask uQoSModifyMask;
} AEEQosSesssionModifyEx;

#define QOSSESSION_EXTFUNC_MODIFY_EX         0x0103f845  // dwVal = pointer to AEEQosSesssionModifyEx

//
// for IQoSSession_GetGrantedFlowSpecDuringAnyState
//
typedef struct AEEQoSSessionGetSpecDuringAnyState
{
   IQoSFlow** ppRxFlowSpec;
   IQoSFlow** ppTxFlowSpec;
} AEEQoSSessionGetSpecDuringAnyState;

#define QOSSESSION_EXTFUNC_GET_SPEC_ANY_STATE_EX   0x01044579  // dwVal = pointer to AEEQoSSessionGetSpecDuringAnyState


/*
   Quality Of Service Session Interface
*/

#if defined(AEEINTERFACE_CPLUSPLUS)

struct IQoSSession : public IModel
{
   virtual int CDECL SelectNetwork (int nNetwork) = 0;
   virtual int CDECL Open (IQoSSpec* piRequestedQoSSpec) = 0;
   virtual int CDECL Modify (IQoSSpec* piRequestedQoSSpec) = 0;
   virtual int CDECL Close (void) = 0;
   virtual int CDECL AddRetryCB (AEECallback* pcbRetry) = 0;
   virtual int CDECL GetFlowSpec (IQoSFlow** ppRxFlowSpec, IQoSFlow** ppTxFlowSpec) = 0;
   virtual AEEQoSStatus CDECL GetStatus (void) = 0;
   virtual int CDECL GoActive (void) = 0;
   virtual int CDECL SetDormancyTimeout (uint8 ucSecs) = 0;
   virtual int CDECL GetError (AEEQoSError* pError) = 0;
   virtual int CDECL ExtFunc (int nOption, uint32 dwVal) = 0;
   int CDECL SelectNetworkEx(int nNetwork, int16 sFamily)
   {
      AEEQosSesssionSelectNetworkEx qssnInfo;

      qssnInfo.nNetwork = nNetwork;
      qssnInfo.sFamily = sFamily;

      return ExtFunc(QOSSESSION_EXTFUNC_SELECT_NETWORK_EX, (uint32)&qssnInfo);
   }
   int Deactivate()
   {
      return ExtFunc(QOSSESSION_EXTFUNC_DEACTIVATE, 0);
   }
   int CDECL ModifyEx(IQoSSpec* piRequestedQoSSpec,
                            AEEQoSModifyMask uQoSModifyMask)
   {
      AEEQosSesssionModifyEx qsmInfo;

      qsmInfo.piRequestedQoSSpec = piRequestedQoSSpec;
      qsmInfo.uQoSModifyMask     = uQoSModifyMask;

      return ExtFunc(QOSSESSION_EXTFUNC_MODIFY_EX, (uint32)&qsmInfo);
   }
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

// Define IQoSSession interface methods
#define INHERIT_IQoSSession(iname) \
   INHERIT_IModel(iname); \
   int      (*SelectNetwork)(iname *po, int nNetwork); \
   int      (*Open)(iname *po, IQoSSpec* piRequestedQoSSpec); \
   int      (*Modify)(iname *po, IQoSSpec* piRequestedQoSSpec); \
   int      (*Close)(iname *po); \
   int      (*AddRetryCB)(iname *po, AEECallback* pcbRetry); \
   int      (*GetFlowSpec)(iname *po, IQoSFlow** ppRxFlowSpec, IQoSFlow** ppTxFlowSpec); \
   AEEQoSStatus (*GetStatus)(iname *po); \
   int      (*GoActive)(iname *po); \
   int      (*SetDormancyTimeout)(iname *po, uint8 ucSecs); \
   int      (*GetError)(iname *po, AEEQoSError* pError); \
   /* Extension Function to allow future enhancement to the interface without
      breaking its binary compatibility. */ \
   int      (*ExtFunc)(iname* po, int nOption, uint32 dwVal)

// declare the actual interface
AEEINTERFACE_DEFINE(IQoSSession);

static __inline uint32 IQoSSession_AddRef(IQoSSession *me)
{
   return AEEGETPVTBL(me,IQoSSession)->AddRef(me);
}

static __inline uint32 IQoSSession_Release(IQoSSession *me)
{
   return AEEGETPVTBL(me,IQoSSession)->Release(me);
}

static __inline int IQoSSession_QueryInterface(IQoSSession *me, AEECLSID id, void **ppo)
{
   return AEEGETPVTBL(me,IQoSSession)->QueryInterface(me, id, ppo);
}

static __inline int IQoSSession_AddListener(IQoSSession *me, ModelListener *pml)
{
   return AEEGETPVTBL(me,IQoSSession)->AddListener(me, pml);
}

static __inline void IQoSSession_Notify(IQoSSession *me, ModelEvent *pme)
{
   AEEGETPVTBL(me,IQoSSession)->Notify(me, pme);
}

static __inline int IQoSSession_SelectNetwork(IQoSSession *me, int nNetwork)
{
   return AEEGETPVTBL(me,IQoSSession)->SelectNetwork(me, nNetwork);
}

static __inline int IQoSSession_Open(IQoSSession *me, IQoSSpec* piRequestedQoSSpec)
{
   return AEEGETPVTBL(me,IQoSSession)->Open(me, piRequestedQoSSpec);
}

static __inline int IQoSSession_Modify(IQoSSession *me, IQoSSpec* piRequestedQoSSpec)
{
   return AEEGETPVTBL(me,IQoSSession)->Modify(me, piRequestedQoSSpec);
}

static __inline int IQoSSession_Close(IQoSSession *me)
{
   return AEEGETPVTBL(me,IQoSSession)->Close(me);
}

static __inline int IQoSSession_AddRetryCB(IQoSSession *me, AEECallback* pcbRetry)
{
   return AEEGETPVTBL(me,IQoSSession)->AddRetryCB(me, pcbRetry);
}

static __inline int IQoSSession_GetFlowSpec(IQoSSession *me, IQoSFlow** ppRxFlowSpec, IQoSFlow** ppTxFlowSpec)
{
   return AEEGETPVTBL(me,IQoSSession)->GetFlowSpec(me, ppRxFlowSpec, ppTxFlowSpec);
}

static __inline AEEQoSStatus IQoSSession_GetStatus(IQoSSession *me)
{
   return AEEGETPVTBL(me,IQoSSession)->GetStatus(me);
}

static __inline int IQoSSession_GoActive(IQoSSession *me)
{
   return AEEGETPVTBL(me,IQoSSession)->GoActive(me);
}

static __inline int IQoSSession_SetDormancyTimeout(IQoSSession *me, uint8 ucSecs)
{
   return AEEGETPVTBL(me,IQoSSession)->SetDormancyTimeout(me, ucSecs);
}

static __inline int IQoSSession_GetError(IQoSSession *me, AEEQoSError* pError)
{
   return AEEGETPVTBL(me,IQoSSession)->GetError(me, pError);
}

static __inline int IQoSSession_SelectNetworkEx(IQoSSession *po, int nNetwork, int16 sFamily)
{
   AEEQosSesssionSelectNetworkEx qssnInfo;

   qssnInfo.nNetwork = nNetwork;
   qssnInfo.sFamily = sFamily;

   return AEEGETPVTBL(po,IQoSSession)->ExtFunc(po, QOSSESSION_EXTFUNC_SELECT_NETWORK_EX, (uint32)&qssnInfo);
}

static __inline int IQoSSession_Deactivate(IQoSSession *me)
{
   return AEEGETPVTBL(me,IQoSSession)->ExtFunc(me, QOSSESSION_EXTFUNC_DEACTIVATE, 0);
}

static __inline int IQoSSession_ModifyEx(IQoSSession *me,
                                        IQoSSpec* piRequestedQoSSpec,
                                        AEEQoSModifyMask uQoSModifyMask)
{
   AEEQosSesssionModifyEx qsmInfo;

   qsmInfo.piRequestedQoSSpec = piRequestedQoSSpec;
   qsmInfo.uQoSModifyMask     = uQoSModifyMask;

   return AEEGETPVTBL(me,IQoSSession)->ExtFunc(me, QOSSESSION_EXTFUNC_MODIFY_EX, (uint32)&qsmInfo);
}

static __inline int IQoSSession_GetGrantedFlowSpecDuringAnyState(IQoSSession* po,
                                                                 IQoSFlow**   ppRxFlowSpec,
                                                                 IQoSFlow**   ppTxFlowSpec)
{
   AEEQoSSessionGetSpecDuringAnyState qsGetSpecDuringAnyState;

   qsGetSpecDuringAnyState.ppRxFlowSpec = ppRxFlowSpec;
   qsGetSpecDuringAnyState.ppTxFlowSpec = ppTxFlowSpec;

   return AEEGETPVTBL(po,IQoSSession)->ExtFunc(po, QOSSESSION_EXTFUNC_GET_SPEC_ANY_STATE_EX, (uint32)&qsGetSpecDuringAnyState);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================
AEEQoSEvent

Description:
   This data type is used for QoS events.

Definition:
   #define AEE_QOS_AVAILABLE_EV              0
   #define AEE_QOS_AVAILABLE_MODIFIED_EV     1
   #define AEE_QOS_UNAVAILABLE_EV            2
   #define AEE_QOS_AVAILABLE_DEACTIVATED_EV  3
   #define AEE_QOS_MODIFY_ACCEPTED_EV        4
   #define AEE_QOS_MODIFY_REJECTED_EV        5
   #define AEE_QOS_INFO_CODE_UPDATED_EV      6

   typedef int AEEQoSEvent;

Members:
   AEE_QOS_AVAILABLE_EV:~
      This event is no longer received. AEE_QOS_AVAILABLE_MODIFIED_EV will be
      received instead.

      This is a change from the previously documented semantics where the event
      was received in response to IQoSSession_Open() and IQoSSession_ModifyEx()
      operations that returned AEE_SUCCESS, to indicate that the most desirable
      IQoSFlow is now available.

   AEE_QOS_AVAILABLE_MODIFIED_EV:~
      This event is received under the following conditions:

      *  In response to IQoSSession_Open() and IQoSSession_GoActive()
         operations that returned AEE_SUCCESS, to indicate that an IQoSFlow is now
         available, whether it is the same or not as the most desirable IQoSFlow.

         This is a change from the previously documented semantics where the event
         was received in response to IQoSSession_Open() operations that returned
         AEE_SUCCESS, to indicate that an IQoSFlow is now available, but the available
         IQoSFlow is not the same as the most desirable IQoSFlow.

      *  In response to IQoSSession_Deactivate() operation that returned
         AEE_SUCCESS, but the request failed and the QoS could not be deactivated.

      *  This event is also generated when the network forcibly modifies a
         previously granted QoS which, for instance, can happen during a network
         handoff.

      In response to this event, in either case, it is recommended that the
      applications should query the currently applied QoS using
      IQoSSession_GetFlowSpec() and take an appropriate action if the granted
      QoS is not sufficient for the proper functioning of the application.
      The application may choose to release the QoS instance and report an
      error to the user or retry original QoS request using IQoSSession_ModifyEx()
      after waiting for some time although it may still not get the requested
      QoS if the network does not have sufficient resources to fulfill it.
      If the QoS has been modified by the network, further information about
      the reason of QoS modification may be provided by dwParam in the
      ModelEvent parameter to the listener callback function.

   AEE_QOS_UNAVAILABLE_EV:~
      This event indicates that a requested QoS could not be assigned or an
      assigned QoS is not available anymore. The event is generated in the
      following cases:

      *  In response to successful completion of an IQoSSession_Close()
         operation that returned AEE_SUCCESS.

      *  In response to IQoSSession_Open(), IQoSSession_ModifyEx() or
         IQoSSession_GoActive() operations that returned AEE_SUCCESS,
         but the QoS Session could not be granted and its state is not
         maintained by the network.

      *  The event is also generated when the network has forcibly revoked
         previously allocated QoS. This could happen due to various reasons like
         network running out of resources, handoff to an older network system
         which does not support QoS etc. For each occurrence, specific reason due
         to which the QoS is not available may be indicated by dwParam in the
         ModelEvent parameter to the listener callback function.

      The event implies that the IQoSSession object is not valid anymore and
      also that the link providing the QoS has been freed up.
      Note that the application is still able to exchange data over the network
      although no specific QoS can be guaranteed.

   AEE_QOS_AVAILABLE_DEACTIVATED_EV:~
      This event indicates that an assigned QoS is deactivated and all the
      resources are released. The event is generated in the following cases:

      *  After using IQoSSession_SetDormancyTimeout() to set an inactivity timeout.

      *  In response to successful completion of an IQoSSession_Deactivate()
         operation that returned AEE_SUCCESS.

      *  In response to IQoSSession_GoActive() operation that returned AEE_SUCCESS,
         but the QoS Session could not be granted but its state is maintained
         by the network.

      *  The event is also generated when the network deactivates the QOS, for
         example, due to inactivity on the link. For each occurrence, specific
         reason due to which the QoS is deactivated may be indicated by dwParam in
         the ModelEvent parameter to the listener callback function.

      The event implies that the IQoSSession object and the link providing the
      QoS are still valid and configured with QoS parameters but the resource
      reservations do not exist anymore.

      Application can try to re-activate the IQoSSession object by using
      IQoSSession_GoActive() or by sending data on the QoS link.

      Applications can query the currently deactivated QoS using
      IQoSSession_GetGrantedFlowSpecDuringAnyState(). The spec returned
      can be identical to the spec granted earlier, or different than
      previously granted spec.

  AEE_QOS_MODIFY_ACCEPTED_EV:~
      This event is generated in response to a IQoSSession_ModifyEx() operation
      that returned AEE_SUCCESS, and indicates that the network can grant the QoS
      Spec specified in that operation.

      This event does not change the QoS Sessions's status.
      If QoS Session's status is changed because of a IQoSSession_ModifyEx()
      operation, another event is posted to indicate its new status.

      Since the network may or may not grant a most desirable QoS, it is
      recommended that the applications should query the currently applied QoS
      using IQoSSession_GetFlowSpec() or IQoSSession_GetGrantedFlowSpecDuringAnyState(),
      and take an appropriate action if the granted QoS is not sufficient
      for the proper functioning of the application.
      The application may choose to release the QoS instance and report an error
      to the user and retry the original QoS request using IQoSSession_ModifyEx()
      after waiting for a period of time, although it may still not get the
      requested QoS if the network does not have sufficient resources to
      fulfill it.

  AEE_QOS_MODIFY_REJECTED_EV:~
      This event is generated in response to a IQoSSession_ModifyEx() operation
      that returned AEE_SUCCESS, and indicates that the network can not grant QoS
      Spec specified in that operation.

      This event does not change the QoS Sessions's status.
      If QoS Session's status is changed because of a IQoSSession_ModifyEx()
      operation, another event is posted to indicate its new status.

      The QoS Session requested by the application prior to the
      IQoSSession_ModifyEx() operation will remain unchanged.

   AEE_QOS_INFO_CODE_UPDATED_EV:~
      This event is generated when new information related to a QoS instance
      is available. The AEEQoSInfoCode passed with this event contains the
      updated information.

Comments:
   The generation of AEE_QOS_AVAILABLE_MODIFIED_EV event instead of
   AEE_QOS_AVAILABLE_EV event should not cause backward compatibility issues.

   Since network can modify the granted QoS at any time, "Older" applications
   should have handle both AEE_QOS_AVAILABLE_EV event and
   AEE_QOS_AVAILABLE_MODIFIED_EV event, and will not be affected by the new
   behavior.

   An application which handled the AEE_QOS_AVAILABLE_EV event but did not
   handle the AEE_QOS_AVAILABLE_MODIFIED_EV will need to be fixed.

Version:
   Introduced BREW Client 3.1

See Also:
  IQoSSession_AddListener()
  IQoSSession_Open()
  IQoSSession_ModifyEx()
  IQoSSession_GoActive()
  IQoSSession_Deactivate()
  IQoSSession_SetDormancyTimeout()
  IQoSSpec
  AEEQoSInfoCode

======================================================================

AEEQoSStatus

Description:
   This data type represents a QoS link status.

Definition:
   #define AEE_QOS_STATUS_ACTIVE             0
   #define AEE_QOS_STATUS_INACTIVE           1
   #define AEE_QOS_STATUS_GOING_ACTIVE       2
   #define AEE_QOS_STATUS_GOING_INACTIVE     3
   #define AEE_QOS_STATUS_NOT_AVAILABLE      4

   typedef int AEEQoSStatus;

Members:
   AEE_QOS_STATUS_ACTIVE: QoS link is active.
   AEE_QOS_STATUS_INACTIVE: QoS link is dormant.
   AEE_QOS_STATUS_GOING_ACTIVE: QoS link is going out of dormant.
   AEE_QOS_STATUS_GOING_INACTIVE: QoS link is going into dormant.
   AEE_QOS_STATUS_NOT_AVAILABLE: couldn't retrieve the status of the QoS link.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_GetStatus()

======================================================================

AEEQoSInfoCode

Description:
   This data type is used to deliver additional information regarding the cause
   of a QoS event. It is delivered as dwParam in the ModelEvent parameter to
   a listener callback function.

Definition:
   #define AEE_QOS_INFO_CODE_NOT_SPECIFIED 0
   #define AEE_QOS_INFO_CODE_NOT_SUPPORTED 1
   #define AEE_QOS_INFO_CODE_NOT_AVAILABLE 2
   #define AEE_QOS_INFO_CODE_NOT_GUARANTEED 3
   #define AEE_QOS_INFO_CODE_INVALID_PARAMS 4
   #define AEE_QOS_INFO_CODE_INTERNAL_ERROR 5
   #define AEE_QOS_INFO_CODE_INSUFFICIENT_NET_RESOURCES 6
   #define AEE_QOS_INFO_CODE_INSUFFICIENT_LOCAL_RESOURCES 7
   #define AEE_QOS_INFO_CODE_QOS_AWARE_SYSTEM   8
   #define AEE_QOS_INFO_CODE_QOS_UNAWARE_SYSTEM 9
   #define AEE_QOS_INFO_CODE_QOS_REJECTED_OPERATION  10
   #define AEE_QOS_INFO_CODE_QOS_TIMED_OUT_OPERATION 11
   #define AEE_QOS_INFO_CODE_QOS_WILL_GRANT_WHEN_QOS_RESUMED 12

   typedef int AEEQoSInfoCode;

Members:
   AEE_QOS_INFO_CODE_NOT_SPECIFIED: Not Specified.
   AEE_QOS_INFO_CODE_NOT_SUPPORTED: Not Supported.
   AEE_QOS_INFO_CODE_NOT_AVAILABLE: Not Available.
   AEE_QOS_INFO_CODE_NOT_GUARANTEED: Not Guaranteed.
   AEE_QOS_INFO_CODE_INVALID_PARAMS: Invalid Parameters.
   AEE_QOS_INFO_CODE_INTERNAL_ERROR: Internal Error.
   AEE_QOS_INFO_CODE_INSUFFICIENT_NET_RESOURCES: Insufficient Net Resources.
   AEE_QOS_INFO_CODE_INSUFFICIENT_LOCAL_RESOURCES: Insufficient Local Resources.
   AEE_QOS_INFO_CODE_QOS_AWARE_SYSTEM:   Moved to a QoS available system
   AEE_QOS_INFO_CODE_QOS_UNAWARE_SYSTEM: Moved to a QoS unavailable system
   AEE_QOS_INFO_CODE_QOS_REJECTED_OPERATION: Rejected operation. e.g. when a QoS
      Request is made and the operation is rejected by the Network.
   AEE_QOS_INFO_CODE_QOS_TIMED_OUT_OPERATION: An operation timed out.
      e.g. when a QoS Request is made and no response is given by the Network.
   AEE_QOS_INFO_CODE_QOS_WILL_GRANT_WHEN_QOS_RESUMED: This info code is currently not
      being used.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   ModelEvent
   IQoSSession_AddListener()

======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IQoSSession

This interface represents a QoS Session (secondary PDP context in UMTS).
QoS Session allows an incoming and outgoing IP packet stream to be treated
according to a desired quality parameters to guarantee specialized behavior.
The desired QoS is given by an IQoSSpec object which can contain QoS parameters
for both directions - receive and transmit. The IQoSSpec specify both the
relevant IP packet stream (filter spec) and also the desired quality parameters
(flow spec). QoS Session applies the requested flow characteristics to the
traffic identified by the filter spec.

NOTE: Your application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

In order to use a QoS Session, a client should:
- Create a new IQoSSession object.
- Select the desired data network by calling IQoSSession_SelectNetwork().
- Register a listener callback for receiving QoS events by using IQoSSession_AddListener().
- Call IQoSSession_Open() with the desired IQoSSpec. IQoSSession_Open() performs
  asynchronous operation and hence a return value of AEE_SUCCESS only means that
  the QOS request has been submitted. In the case where IQoSSession_Open()
  returns AEE_NET_WOULDBLOCK, use IQoSSession_AddRetryCB() to register a
  callback which will be resumed when the IQoSSession_Open() operation should
  be retried.
- Wait for QoS event which indicates the status of the requested QoS. The QoS
  event will be delivered through the listener callback that was previously
  registered. Receipt of a successful QoS event (e.g. AEE_QOS_AVAILABLE_MODIFIED_EV)
  means that the QoS Session is established and the client can start sending or
  receiving data using the granted QoS.
- When the QoS Session is not needed anymore, call IQoSSession_Close().
  IQoSSession_Close() performs asynchronous operation and hence a return value of
  AEE_SUCCESS only means that the request has been submitted.
- Wait for QoS event which indicates that the session was closed. The QoS
  event will be delivered through the listener callback that was previously
  registered. Receipt of the AEE_QOS_UNAVAILABLE_EV QoS event means that the
  QoS Session is closed and its resources are released.

For modifying QoS parameters for an established QoS Session, use
IQoSSession_ModifyEx().

For retrieving the granted flow spec of a currently activated QoSSession,
use IQoSSession_GetFlowSpec() or IQoSSession_GetGrantedFlowSpecDuringAnyState().

For retrieving the flow spec of a QoSSession that is currently
not activated, use IQoSSession_GetGrantedFlowSpecDuringAnyState().
Note that the returned spec may or may not be identical to the spec
granted when the session was previously activated. See documentation
of IQoSSession_GetGrantedFlowSpecDuringAnyState().

For retrieving the status of the QoS Session, use IQoSSession_GetStatus().

For controlling dormancy of the QoS Session, use IQoSSession_GoActive() and
IQoSSession_SetDormancyTimeout().

For suspending a granted QoS Session, use IQoSSession_Deactivate().

For retrieving the last error in a requested IQoSSpec, use
IQoSSession_GetError().

The following header file is required:~
   BREW 4.0 - AEEIQoSSession.h
   BREW 3.1 - AEEQoSSession.h

=============================================================================

IQoSSession_AddRef()

Description:
   This function is inherited from IModel_AddRef().

See Also:
   IQoSSession_Release()

=============================================================================

IQoSSession_Release()

Description:
   This function is inherited from IModel_Release().

See Also:
   IQoSSession_AddRef()

==============================================================================

IQoSSession_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

==============================================================================

IQoSSession_AddListener()

Description:
   This function is inherited from IModel_AddListener().

   The ModelEvent object received at the listener callback contains:
   - pModel: pointer to the originating object. Use IQI to query it for
             IQoSSession interface
   - evCode: the QoS event that occurred (AEEQoSEvent).
   - dwParam: additional information regarding the cause of the QoS event
              (AEEQoSInfoCode)

See Also:
   None

=======================================================================

IQoSSession_SelectNetwork()

Description:
   This function selects a specific data network for this QoS Session.

   When an IQoSSession instance is created, no network is selected. If
   IQoSSession_Open() is called before IQoSSession_SelectNetwork(),
   AEE_NETWORK_DEFAULT will be implicitly selected.

   After the QoS Session is opened by calling IQoSSession_Open(), the network
   may not be changed. However, IQoSSession instance is re-usable, meaning that
   the client may call IQoSSession_SelectNetwork() again after a QoS event which
   indicates that the QoS was released (AEE_QOS_UNAVAILABLE_EV) is received.

Prototype:
   int IQoSSession_SelectNetwork(IQoSSession *po, int nNetwork);

Parameters:
   po: the interface pointer
   nNetwork: the network for this QoS Session. one of AEE_NETWORK_* constants.

Return Value:
   AEE_SUCCESS:
      the data network was selected.

   AEE_NET_EINVAL:
      the network is not valid.

   AEE_EBADSTATE:
      the network has already been selected.

   AEE_EFAILED:
      other failure.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_SelectNetworkEx()
   IQoSSession_Open()
   Data networks

=======================================================================

IQoSSession_SelectNetworkEx()

Description:
   This function selects a specific data network for this QoS Session.
   Specifications for the desired data network include address family.

Prototype:
   int IQoSSession_SelectNetworkEx(IQoSSession *po, int nNetwork, int16 sFamily);

Parameters:
   po: the interface pointer
   nNetwork: the network for this QoS Session. one of AEE_NETWORK_* constants.
   sFamily: address family (AEE_AF_*)

Return Value:
   AEE_SUCCESS:
      the data network was selected.

   AEE_NET_EINVAL:
      the network is not valid.

   AEE_EBADSTATE:
      the network has already been selected.

   AEE_EBADPARM:
      invalid input parameter

   AEE_EFAILED:
      other failure.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IQoSSession_SelectNetwork()
   Data networks

=======================================================================

IQoSSession_Open()

Description:
   This function issues a request for establishing a new QoS Session. The
   request is forwarded to lower layers in order to establish a new QoS link.

   If a network was not yet selected for the IQoSSession instance,
   AEE_NETWORK_DEFAULT will be implicitly selected.

   Note that a return value of AEE_SUCCESS indicates that the request was
   successfully issued but it doesn't indicate that QoS is already available.
   After IQoSSession_Open() returns AEE_SUCCESS, the client should listen (see
   IQoSSession_AddListener()) for QoS events and wait for a QoS event which
   indicates success (AEE_QOS_AVAILABLE_EV or AEE_QOS_AVAILABLE_MODIFIED_EV).
   Receiving one of these events indicates that QoS was granted and the client
   can start sending or receiving data using the granted QoS.

   After IQoSSession_Open() returns AEE_SUCCESS, IQoSSession_Open() shall not be
   called again.
   However, IQoSSession instance is re-usable, meaning that the client may call
   IQoSSession_Open() again after a QoS event which indicates that the QoS was
   released (AEE_QOS_UNAVAILABLE_EV) is received.

   Also note the possibility of an AEE_NET_WOULDBLOCK return value. This occurs
   in cases where a network connection, such as a PPP link, must be established.
   In that case, IQoSSession_Open() returns AEE_NET_WOULDBLOCK, and should be
   called again to obtain the final result (error or success).

   For notification of when to call IQoSSession_Open() again, the client should
   register a callback through IQoSSession_AddRetryCB(). Note that when the
   client's callback is resumed, it is not guaranteed that a subsequent call
   to IQoSSession_Open() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IQoSSession_Open(IQoSSession *po, IQoSSpec* piRequestedQoSSpec);

Parameters:
   po: the interface pointer
   piRequestedQoSSpec: the desired QoS specification for the new session.

Return Value:
   AEE_SUCCESS:
      the request was successfully issued. This doesn't indicate that QoS is
      already available.

   AEE_NET_WOULDBLOCK:
      operation cannot be completed right now; try again later.

   AEE_EBADPARM:
      the desired QoS specification is not valid (e.g. piRequestedQoSSpec is
      NULL)

   AEE_NET_EQOS_SPEC_INVALID_OPT:
      the desired QoS specification contains invalid options. Use
      IQoSSession_GetError() to identify which option was invalid.

   AEE_NET_EQOSUNAWARE:
      the current network is QoS Unaware System (does not support QoS).
      This is relevant for CDMA and WLAN networks.

   AEE_EBADSTATE: QoSSession state is not suitable for this operation.

   AEE_ENOMEMORY: out of memory.

   AEE_NET_ENETNONET: network is not available.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_AddRetryCB()
   IQoSSession_Close()
   IQoSSession_AddListener()
   IQoSSession_SelectNetwork()
   IQoSSession_GetError()
   IQoSSpec

=======================================================================

IQoSSession_Modify()

Description:
   Deprecated

See Also:
   IQoSSession_ModifyEx()

=======================================================================

IQoSSession_ModifyEx()

Description:
   This function issues the following modification requests:
   * Modify an existing QoS Session either in Rx, Tx or both directions.
   * Add a new QoS in a specific direction.
   * Remove a granted QoS in a specific direction.
   The modification requests can be performed on Rx blocks, Tx blocks or both.

   Note that a return value of AEE_NET_SUCCESS indicates that the request was
   successfully issued but it doesn't indicate that QoS is already modified -
   the network may accept or reject this operation
   After IQoSSession_ModifyEx() returns AEE_NET_SUCCESS, the client should listen (see
   IQoSSession_AddListener()) for QoS events: AEE_QOS_MODIFY_ACCEPTED_EV and
   AEE_QOS_MODIFY_REJECTED_EV.

   In case the IQoSSession_ModifyEx() request changed the QoS Session's status,
   another event is posted (in addition to
   AEE_QOS_MODIFY_ACCEPTED_EV/AEE_QOS_MODIFY_REJECTED_EV event) to indicate
   the new status.

   IQoSSession_ModifyEx() can be used only after a QoS Session has already been
   opened successfully.

Prototype:
   int IQoSSession_ModifyEx(IQoSSession *po,
                            IQoSSpec* piRequestedQoSSpec,
                            AEEQoSModifyMask uQoSModifyMask);

Parameters:
   po: the interface pointer
   piRequestedQoSSpec: the QoS specification used for modifying the
                       QoS instance
   uQoSModifyMask: bit mask which indicates the QoS specification blocks
                   to be modified

Return Value:
   AEE_NET_SUCCESS:
      the request was successfully issued. This doesn't indicate that QoS was
      already modified.

   AEE_EBADPARM:
      illegal modify mask.

   AEE_NET_EQOS_SPEC_INVALID_OPT:
      the desired QoS specification contains invalid options. Use
      IQoSSession_GetError() to identify which option was invalid.

   AEE_NET_EQOSUNAWARE :
      the current network is QoS Unaware System (does not support QoS).
      This is relevant for CDMA and WLAN networks.

   AEE_EBADSTATE: QoSSession state is not suitable for this operation.

   AEE_EUNSUPPORTED: operation not supported.

   Other error codes are also possible.

Comments:
   The following table summarizes the modification options:
   ===pre>
  ------------------------------------------------------------------------------------
  |     OPERATION     |      MASK      |  QOS   |  QOS   |         COMMENT           |
  |                   |                |  FLOW  | FILTER |                           |
  ------------------------------------------------------------------------------------
  | Modify Tx Flow for|                |        |        | Can appear with TX_FILTER |
  | a granted QoS in  |    TX_FLOW     |   +    |        | and other Rx modification |
  | this direction    |                |        |        | masks                     |
  ------------------------------------------------------------------------------------
  | Modify Tx Filter  |                |        |        | Can appear with TX_FLOW   |
  | fot a granted QoS |   TX_FILTER    |        |    +   | and other Rx modification |
  | in this direction |                |        |        | maks                      |
  ------------------------------------------------------------------------------------
  | Delete Tx Spec    |                |        |        | Cannot appear with any    |
  | of a granted QoS  | TX_SPEC_DELETE |        |        | other modify mask in      |
  | in this direction |                |        |        | Tx direction.             |
  |                   |                |        |        | Can appear with other Rx  |
  |                   |                |        |        | modification masks        |
  ------------------------------------------------------------------------------------
  |                   |                |        |        | Cannot appear with any    |
  | Add new Tx Spec   |  TX_SPEC_ADD   |   +    |    +   | other modify mask in      |
  |                   |                |        |        | Tx direction.             |
  |                   |                |        |        | Can appear with other Rx  |
  |                   |                |        |        | modification masks        |
  ------------------------------------------------------------------------------------
  | Modify Rx Flow for|                |        |        | Can appear with RX_FILTER |
  | a granted QoS in  |    RX_FLOW     |   +    |        | and other Tx modification |
  | this direction    |                |        |        | masks                     |
  ------------------------------------------------------------------------------------
  | Modify Rx Filter  |                |        |        | Can appear with RX_FLOW   |
  | fot a granted QoS |   RX_FILTER    |        |    +   | and other Tx modification |
  | in this direction |                |        |        | masks                     |
  ------------------------------------------------------------------------------------
  | Delete Rx Spec    |                |        |        | Cannot appear with any    |
  | of a granted QoS  | RX_SPEC_DELETE |        |        | other modify mask in      |
  | in this direction |                |        |        | Rx direction.             |
  |                   |                |        |        | Can appear with other Tx  |
  |                   |                |        |        | modification masks        |
  ------------------------------------------------------------------------------------
  |                   |                |        |        | Cannot appear with any    |
  |  Add new Rx Spec  |  RX_SPEC_ADD   |   +    |    +   | other modify mask in      |
  |                   |                |        |        | Rx direction.             |
  |                   |                |        |        | Can appear with other Tx  |
  |                   |                |        |        | modification masks        |
  ------------------------------------------------------------------------------------
  ===/pre>

  - To modify or add a block, a new QoS specification must fully specify this block.
  - QoS Specification blocks which are not specified in the above table are ignored.
  - To delete a QoS Specification in a specific direction user can provide NULL
    as the Requested QoS Spec.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   IQoSSession_Open()
   IQoSSession_GetError()
   IQoSSession_AddListener()
   IQoSSpec
   AEEQoSModifyMask

=======================================================================

IQoSSession_Close()

Description:
   This function issues a request for closing a QoS Session. The request is
   forwarded to lower layers in order to release the QoS link resources.

   Note that a return value of AEE_SUCCESS indicates that the request was
   successfully issued but it doesn't indicate that all QoS resources are
   released.

   After IQoSSession_Close() returns AEE_SUCCESS, the client can listen (see
   IQoSSession_AddListener()) for QoS events and wait for a QoS event which
   indicates that QoS was released (AEE_QOS_UNAVAILABLE_EV). Only this event
   indicates that all QoS resources are released.

   IQoSSession_Close() can be called only after the QoSSession was previously
   opened with a call to IQoSSession_Open(). IQoSSession_Close() can not be
   called twice, i.e. after the QoSSession was previously closed with a call to
   IQoSSession_Close().

Prototype:
   int IQoSSession_Close(IQoSSession *po);

Parameters:
   po: the interface pointer

Return Value:
   AEE_SUCCESS: the request was successfully issued. This doesn't indicate that all
                QoS resources are released yet.

   AEE_EBADSTATE: QoSSession state is not suitable for this operation.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_Open()
   IQoSSession_AddListener()

=======================================================================

IQoSSession_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   IQoSSession_Open() operation should be retried.

   In the case where IQoSSession_Open() returns AEE_NET_WOULDBLOCK,
   IQoSSession_Open() should be called again later to obtain the final result
   (error or success).
   For notification of when to call IQoSSession_Open() again, the client should
   register a callback through IQoSSession_AddRetryCB(). Note that when the
   client's callback is resumed, it is not guaranteed that a subsequent call
   to IQoSSession_Open() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IQoSSession_AddRetryCB(IQoSSession *po, AEECallback* pcbRetry);

Parameters:
   po: the interface pointer
   pcbRetry: the client's callback.

Return Value:
   AEE_SUCCESS:
      client's callback was stored successfully.

   AEE_EBADSTATE:
      QoSSession state is not suitable for this operation.

  AEE_EBADPARM:
      pcbRetry is NULL.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_Open()

=======================================================================

IQoSSession_GetFlowSpec()

Description:
   This function returns the current Rx and Tx Flow Specs.

   This function should be called only after QoS was granted. i.e. after
   receiving a QoS event which indicates success (AEE_QOS_AVAILABLE_EV or
   AEE_QOS_AVAILABLE_MODIFIED_EV).

   The client is responsible for releasing the returned Rx and Tx Flow Spec
   objects when they are not needed anymore.

Prototype:
   int IQoSSession_GetFlowSpec(IQoSSession *po, IQoSFlow** ppRxFlowSpec, IQoSFlow** ppTxFlowSpec);

Parameters:
   po: the interface pointer
   ppRxFlowSpec: returns the current Rx Flow Spec. Can be NULL to indicate that
                 the caller is not interested in the Rx Flow Spec.
                 The output parameter *ppRxFlowSpec will be set to NULL if there
                 is no flow spec in the Rx direction.

   ppTxFlowSpec: returns the current Tx Flow Spec. Can be NULL to indicate that
                 the caller is not interested in the Tx Flow Spec.
                 The output parameter *ppTxFlowSpec will be set to NULL if there
                 is no flow spec in the Tx direction.

Return Value:
   AEE_SUCCESS:
      current Rx and Tx Flow Specs were successfully retrieved.

   AEE_EBADSTATE:
      QoSSession state is not suitable for this operation.

   Other error codes are also possible.

Comments:
   If both ppRxFlowSpec and ppTxFlowSpec are NULL, the function returns
   immediately with AEE_SUCCESS.
   The output parameters *ppTxFlowSpec and *ppRxFlowSpec can be set to NULL,
   for example in the case where QoS is deactivated, both of them will be set
   to NULL, note that in such case the return value will be AEE_NET_SUCCESS.
   To obtain the QoS spec while QoS is deactivated,
   please see IQoSSession_GetGrantedFlowSpecDuringAnyState()

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_GetGrantedFlowSpecDuringAnyState()

=======================================================================

IQoSSession_GetStatus()

Description:
   This function returns the status of the QoS link.

   This function should be called only after QoS was granted. i.e. after
   receiving a QoS event which indicates success (AEE_QOS_AVAILABLE_EV,
   AEE_QOS_AVAILABLE_MODIFIED_EV, AEE_QOS_AVAILABLE_DEACTIVATED).

Prototype:
   AEEQoSStatus IQoSSession_GetStatus(IQoSSession *po);

Parameters:
   po: the interface pointer

Return Value:
   The status of the QoS link.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   AEEQoSStatus

=======================================================================

IQoSSession_GoActive()

Description:
   This function is used in order to activate the QoS link (from dormant mode).
   Once the application received the AEE_QOS_AVAILABLE_EV or
   AEE_QOS_AVAILABLE_MODIFIED_EV events, it is again able to transmit data over
   the specific QoS instance.

   The request to activate a QoS Session instance can be performed only on
   a QoS Session instance which was previously opened by IQoSSession_Open()
   and was not yet closed.

   Note that a return value of AEE_SUCCESS indicates that the request was
   successfully issued but it doesn't indicate that QoS is already active.
   After IQoSSession_GoActive() returns AEE_SUCCESS, the client should listen (see
   IQoSSession_AddListener()) for QoS events and wait for a QoS event which
   indicates activation of the QoS link (AEE_QOS_AVAILABLE_EV or AEE_QOS_AVAILABLE_MODIFIED_EV).
   Receiving one of these events indicates that QoS is now active.

   This function should be called only after QoS was granted. i.e. after
   receiving a QoS event which indicates success (AEE_QOS_AVAILABLE_EV or
   AEE_QOS_AVAILABLE_MODIFIED_EV).

   This function should not be called when the QoS link is active.

Prototype:
   int IQoSSession_GoActive(IQoSSession *po);

Parameters:
   po: pointer the IQoSSession instance to be activated.

Return Value:
   AEE_SUCCESS:
      the request was successfully issued. This doesn't indicate that the QoS
      Session is already activated.

   AEE_EBADSTATE:
      QoSSession state is not suitable for this operation.

   AEE_NET_EINVAL:
      an invalid QoS Session pointer is passed.

   AEE_NET_EQOSUNAWARE:
      the current network is QoS Unaware System (does not support QoS).
      This is relevant for CDMA and WLAN networks.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_SetDormancyTimeout()
   IQoSSession_Deactivate()

=======================================================================

IQoSSession_Deactivate()

Description:
   This function is used in order to suspend a granted QoS Session.
   Once the application received the AEE_QOS_AVAILABLE_DEACTIVATE_EV event,
   and until the QoS is reactivated, the application is no longer able to
   send data using this QoS instance. Any attempts to transmit data on a
   deactivated QoS Session instance will result in activating the instance but
   in the mean time data will be routed to the default flow bearing no QoS
   guarantees.

   The request to deactivate a QoS Session instance can be performed only on
   a QoS Session instance which was previously opened by IQoSSession_Open()
   and was not yet closed.

   The QoS Sessions which is requested to be suspended must be
   in a state which enables suspension, otherwise the operation will fail.

   Note that a return value of AEE_SUCCESS indicates that the request was
   successfully issued but it doesn't indicate that QoS is already active.
   After IQoSSession_Deactivate() returns AEE_SUCCESS, the client should listen (see
   IQoSSession_AddListener()) for QoS events and wait for a QoS event which
   indicates suspension of the QoS link (AEE_QOS_AVAILABLE_DEACTIVATE_EV).
   Receiving this event indicates that QoS is now deactivated.

Prototype:
   int IQoSSession_Deactivate(IQoSSession* po);

Parameters:
   po: pointer the QoS Session interface to be deactivated.

Return Value:
   AEE_SUCCESS:
      the request was successfully issued. This doesn't indicate that the QoS
      Session is already suspended.

   AEE_EBADSTATE:
      QoSSession state is not suitable for this operation.

   AEE_NET_EQOSUNAWARE:
      the current network is QoS Unaware System (does not support QoS).
      This is relevant for CDMA and WLAN networks.


   AEE_NET_EOPNOTSUPP:
      operation is not supported.

   Other error codes are also possible.


Comments:
   IQoSSession_Deactivate() is not supported on UMTS networks.

Version:
   Introduced BREW Client <4.0.1>

See Also:
   IQoSSession_GoActive()


=======================================================================

IQoSSession_SetDormancyTimeout()

Description:
   This function sets the IS-707 (cdma2000 1x) packet data dormancy timeout for
   the QoS link.
   If no network traffic on the QoS link occurs during a period of time equals
   to the dormancy timeout then the mobile should perform mobile initiated
   dormancy for the QoS link.

   When the QoS link goes dormant, an AEE_QOS_AVAILABLE_DEACTIVATED_EV event is
   sent to registered clients.

   This function should be called only after QoS was granted. i.e. after
   receiving a QoS event which indicates success (AEE_QOS_AVAILABLE_EV or
   AEE_QOS_AVAILABLE_MODIFIED_EV).

Prototype:
   int IQoSSession_SetDormancyTimeout(IQoSSession *po, uint8 ucSecs);

Parameters:
   po: the interface pointer
   ucSecs: dormant timeout in seconds. 0 means no timeout (no mobile initiated
           dormancy should occur)

Return Value:
   AEE_SUCCESS:
      the timeout was set.

   AEE_EUNSUPPORTED:
      operation not supported.

   AEE_NET_EOPNOTSUPP:
      network not suitable for this operation (i.e. not a
      cdma2000 1x network).

   AEE_EFAILED:
      other error.

   AEE_EBADSTATE:
      QoSSession state is not suitable for this operation.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSession_GoActive()

=======================================================================

IQoSSession_GetError()

Description:
   This function returns an AEEQoSError that can be used by clients in order
   to identify which error occurred, and whether it is related to a specific
   option in the requested QoS Spec.

   Clients should call this function after a QoS request operation (either
   IQoSSession_Open() or IQoSSession_ModifyEx()) returns
   AEE_NET_EQOS_SPEC_INVALID_OPT.

Prototype:
   int IQoSSession_GetError(IQoSSession *po, AEEQoSError* pError);

Parameters:
   po: the interface pointer
   pError [out]: returns a QoS error.

Return Value:
   AEE_SUCCESS:
      error was successfully retrieved into pError.

   AEE_EBADPARM:
      pError is null.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   AEEQoSError.

=======================================================================

IQoSSession_GetGrantedFlowSpecDuringAnyState()

Description:
   This function returns the current Rx and Tx Flow Specs.
   It extends the functionality of IQoSSession_GetFlowSpec():
   Once QoS is granted, this function can be called as long as the QoS
   is still available, and return a spec even if is currently deactivated
   (as opposed to IQoSSession_GetFlowSpec(), which will return NULL in this case).

   * The function can be called if the QoS is in one of the following states:
     - AEE_QOS_STATUS_ACTIVE: will return the currently granted spec.
     - AEE_QOS_STATUS_INACTIVE, AEE_QOS_STATUS_GOING_ACTIVE and AEE_QOS_STATUS_GOING_INACTIVE:
      will return the spec that was granted when the QOS was active, as long as
      this information is available in the system. Otherwise the returned spec
      may not be identical to the spec actually granted when the QOS was active,
      but may still contain partial information of that spec.
   * It should not be called if the status of the QoS is AEE_QOS_STATUS_NOT_AVAILABLE.

   * The user can call IQoSSession_GetStatus() before or after getting the spec,
     to query the current state of the QoS.

   The client is responsible for releasing the returned Rx and Tx Flow Spec
   objects when they are not needed anymore.

Prototype:
   int IQoSSession_GetGrantedFlowSpecDuringAnyState(IQoSSession* po,
                                                    IQoSFlow**   ppRxFlowSpec,
                                                    IQoSFlow**   ppTxFlowSpec);
Parameters:
   po: the interface pointer
   ppRxFlowSpec: returns the current Rx Flow Spec. Can be NULL to indicate that
                 the caller is not interested in the Rx Flow Spec.
                 The output parameter *ppRxFlowSpec will be set to NULL if there
                 is no flow spec in the Rx direction.

   ppTxFlowSpec: returns the current Tx Flow Spec. Can be NULL to indicate that
                 the caller is not interested in the Tx Flow Spec.
                 The output parameter *ppTxFlowSpec will be set to NULL if there
                 is no flow spec in the Tx direction.

Return Value:
   AEE_SUCCESS:
      current Rx and Tx Flow Specs were successfully retrieved.

   AEE_EBADSTATE:
      QoSSession state is not suitable for this operation.

   Other error codes are also possible.

Comments:
   If both ppRxFlowSpec and ppTxFlowSpec are NULL, the function returns
   immediately with AEE_NET_SUCCESS.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   IQoSSession_GetFlowSpec()
=======================================================================*/

#endif      // AEEIQOSSESSION_H
