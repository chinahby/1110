#ifndef AEEPRIMARYQOSSESSION_H
#define AEEPRIMARYQOSSESSION_H
/*======================================================
FILE:  AEEPrimaryQoSSession.h

SERVICES: BREW Primary QoS Session Interface

GENERAL DESCRIPTION:
	Interface definitions, data structures, etc. for BREW Primary 
   QOS Session Interface

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEQoSList.h"
#include "AEEQoSFlow.h"
#include "AEEQoSSpec.h"
#include "AEE.h"
#include "AEEModel.h"

/*
   Primary QoS Modification Mask
*/
#define AEE_PRIMARY_QOS_MODIFY_MASK_NONE           AEE_QOS_MODIFY_MASK_NONE
#define AEE_PRIMARY_QOS_MODIFY_MASK_RX_FLOW        AEE_QOS_MODIFY_MASK_RX_FLOW
#define AEE_PRIMARY_QOS_MODIFY_MASK_TX_FLOW        AEE_QOS_MODIFY_MASK_TX_FLOW
#define AEE_PRIMARY_QOS_MODIFY_MASK_ALL            AEE_QOS_MODIFY_MASK_RX_FLOW|AEE_QOS_MODIFY_MASK_TX_FLOW

typedef uint16 AEEPrimaryQoSModifyMask;

/*
   Primary QoS Events
*/

#define AEE_PRIMARY_QOS_MODIFY_ACCEPTED_EV        0
#define AEE_PRIMARY_QOS_MODIFY_REJECTED_EV        1
#define AEE_PRIMARY_QOS_MODIFIED_EV               2

typedef int AEEPrimaryQoSEvent;

/*
   Primary Quality Of Service Session Interface
*/

#define AEEIID_PRIMARYQOSSESSION 0x0104564e

#define INHERIT_IPrimaryQoSSession(iname) \
   INHERIT_IModel(iname);  \
   int      (*SelectNetwork)(iname *po, int nNetwork, int16 sFamily);   \
   int      (*AddRetryCB)(iname *po, AEECallback* pcbRetry);   \
   int      (*Modify)(iname *po, IQoSList* piRequestedRxQoSFlow, IQoSList* piRequestedTxQoSFlow, AEEPrimaryQoSModifyMask uPrimaryQoSModifyMask); \
   int      (*GetGrantedFlowSpec)(iname *po, IQoSFlow** ppRxFlowSpec, IQoSFlow** ppTxFlowSpec); \
   int      (*GetError)(iname *po, AEEQoSError* pqError);   \
   /* Extension Function to allow future enhancement to the interface without */ \
   /* breaking its binary compatibility.                                      */ \
   int      (*ExtFunc)(iname* po, AEECLSID id, void *pBuf, int nBufSize)

AEEINTERFACE_DEFINE(IPrimaryQoSSession);

///////////////////////////////
// IPrimaryQoSSession methods
///////////////////////////////
static __inline uint32 IPRIMARYQOSSESSION_AddRef(IPrimaryQoSSession *me)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->AddRef(me);
}

static __inline uint32 IPRIMARYQOSSESSION_Release(IPrimaryQoSSession *me)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->Release(me);
}

static __inline int IPRIMARYQOSSESSION_QueryInterface(IPrimaryQoSSession *me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->QueryInterface(me, cls, ppo);
}
                      
static __inline int IPRIMARYQOSSESSION_AddListener(IPrimaryQoSSession *me, ModelListener *pml)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->AddListener(me, pml);
}

static __inline int IPRIMARYQOSSESSION_SelectNetwork(IPrimaryQoSSession *me, int nNetwork, int16 sFamily)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->SelectNetwork(me, nNetwork, sFamily);
}

static __inline int IPRIMARYQOSSESSION_AddRetryCB(IPrimaryQoSSession *me, AEECallback* pcbRetry)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->AddRetryCB(me, pcbRetry);
}

static __inline int IPRIMARYQOSSESSION_Modify(IPrimaryQoSSession *me, 
                                              IQoSList* piRequestedRxQoSFlow, 
                                              IQoSList* piRequestedTxQoSFlow, 
                                              AEEPrimaryQoSModifyMask uPrimaryQoSModifyMask)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->Modify(me, 
                                                     piRequestedRxQoSFlow, 
                                                     piRequestedTxQoSFlow, 
                                                     uPrimaryQoSModifyMask);
}

static __inline int IPRIMARYQOSSESSION_GetGrantedFlowSpec(IPrimaryQoSSession *me, 
                                                   IQoSFlow** ppRxFlowSpec, 
                                                   IQoSFlow** ppTxFlowSpec)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->GetGrantedFlowSpec(me, ppRxFlowSpec, ppTxFlowSpec);
}

static __inline int IPRIMARYQOSSESSION_GetError(IPrimaryQoSSession *me, AEEQoSError* pqError)
{
   return AEEGETPVTBL(me,IPrimaryQoSSession)->GetError(me, pqError);
}

/*=============================================================================
  DATA STRUCTURE DOCUMENTATION
===============================================================================

AEEPrimaryQoSModifyMask

Description:
   This data type is a bitmask which is used as a parameter to 
   IPRIMARYQOSSESSION_Modify() in order to specify the combination of 
   Primary QoS Specification blocks to be modified.

Definition:
   #define AEE_PRIMARY_QOS_MODIFY_MASK_NONE           AEE_QOS_MODIFY_MASK_NONE
   #define AEE_PRIMARY_QOS_MODIFY_MASK_RX_FLOW        AEE_QOS_MODIFY_MASK_RX_FLOW
   #define AEE_PRIMARY_QOS_MODIFY_MASK_TX_FLOW        AEE_QOS_MODIFY_MASK_TX_FLOW
   #define AEE_PRIMARY_QOS_MODIFY_MASK_ALL            AEE_QOS_MODIFY_MASK_RX_FLOW|AEE_QOS_MODIFY_MASK_TX_FLOW

   typedef uint16 AEEPrimaryQoSModifyMask;

Members: 
   AEE_PRIMARY_QOS_MODIFY_MASK_NONE:      None
   AEE_PRIMARY_QOS_MODIFY_MASK_RX_FLOW:   Modify Rx Flow block
   AEE_PRIMARY_QOS_MODIFY_MASK_TX_FLOW:   Modify Tx Flow block
   AEE_PRIMARY_QOS_MODIFY_MASK_ALL:       All above modification options

Comments:
   None

See Also:
  IPRIMARYQOSSESSION_Modify()

===============================================================================

AEEPrimaryQoSEvent

Description:
   This data type is used for Primary QoS events

Definition:
   #define AEE_PRIMARY_QOS_MODIFY_ACCEPTED_EV        0
   #define AEE_PRIMARY_QOS_MODIFY_REJECTED_EV        1
   #define AEE_PRIMARY_QOS_MODIFIED_EV               2

   typedef int AEEPrimaryQoSEvent;

Members: 
   AEE_PRIMARY_QOS_MODIFY_ACCEPTED_EV:
      This event is generated in response to an IPRIMARYQOSSESSION_Modify() 
      request that has previously returned SUCCESS, and indicates that the 
      network can grant the Primary QoS specified in that operation.

      In response to this event, it is recommended that the 
      applications should query the currently Primary QoS Specification using 
      IPRIMARYQOSSESSION_GetGrantedFlowSpec().     
  
   AEE_PRIMARY_QOS_MODIFY_REJECTED_EV:   
      This event is generated in response to an IPRIMARYQOSSESSION_Modify() 
      request that has previously returned SUCCESS, and indicates that the 
      network cannot grant the Primary QoS specified in that operation.

   AEE_PRIMARY_QOS_MODIFIED_EV:   
      This event is generated to indicate that the Primary QoS Specification
      was modified. Modification might have been triggered by either
      the application (via IPRIMARYQOSSESSION_Modify()) or the network.

      In response to this event, it is recommended that the 
      applications should query the currently Primary QoS Specification using 
      IPRIMARYQOSSESSION_GetGrantedFlowSpec().

Comments:
   In order to receive any of the above Primary QoS events, application must 
   explicitly register to them via IPRIMARYQOSSESSION_AddListener().

See Also:
   IPRIMARYQOSSESSION_AddListener()
   IPRIMARYQOSSESSION_Modify()
   IPRIMARYQOSSESSION_GetGrantedFlowSpec()

===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

Interface Name: IPrimaryQoSSession

This interface provides an API to perform operations on a Primary QoS Session.
Primary QoS Session is used to guarantee default quality parameters for all 
incoming and outgoing IP packet streams.
 
A Primary QoS is composed of IQoSFlow objects in Rx (receive) direction 
and in Tx (transmit) direction. Each Flow specifies the default quality 
parameters in the corresponding direction.

In order to use a Primary QoS Session, a client should:
- Create a new IPrimaryQoSSession object.
- Select the desired data network by calling IPRIMARYQOSSESSION_SelectNetwork().
- Register a listener callback for receiving Primary QoS events by using 
IPRIMARYQOSSESSION_AddListener().

In order to modify Primary QoS Flows in Rx and/or Tx direction, client should
call IPRIMARYQOSSESSION_Modify() and listen to AEE_PRIMARY_QOS_MODIFY_ACCEPTED_EV
and AEE_PRIMARY_QOS_MODIFY_REJECTED_EV.
In case of a AEE_NET_EQOS_SPEC_INVALID_OPT error, client should call
IPRIMARYQOSSESSION_GetError() to retrieve the last error.

In order to track modification of Primary QoS specification, user should listen
to AEE_PRIMARY_QOS_MODIFIED_EV.

In order to retrieve the granted Flow Spec of the Primary QoS, client should 
call IPRIMARYQOSSESSION_GetGrantedFlowSpec().

The following header file is required:
AEEPrimaryQoSSession.h
  
===============================================================================

IPRIMARYQOSSESSION_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   IPRIMARYQOSSESSION_Release()

===============================================================================

IPRIMARYQOSSESSION_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   IPRIMARYQOSSESSION_AddRef()

===============================================================================

IPRIMARYQOSSESSION_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

===============================================================================

IPRIMARYQOSSESSION_AddListener()

Description:
   This function is inherited from IMODEL_AddListener().
   It is used to register to AEEPrimaryQoSEvent events.
   
   If a network was not yet selected for the IPrimaryQoSSession instance, 
   AEE_NETWORK_DEFAULT & AEE_AF_INET will be implicitly selected.

   The ModelEvent object received at the listener callback contains:
   - pModel:  pointer to the originating object. Use IQI to query it for 
              IPrimaryQoSSession interface
   - evCode:  the Primary QoS event that occurred (AEEPrimaryQoSEvent).
   - dwParam: not used
 
See Also:
   AEEPrimaryQoSEvent

===============================================================================

IPRIMARYQOSSESSION_SelectNetwork()

Description:
   This function selects a specific data network to which this 
   Primary QoS Session instance would be linked to.
   
   Specifications for the desired data network include address family.
   
Prototype:   
   int IPRIMARYQOSSESSION_SelectNetwork(IPrimaryQoSSession *po, int nNetwork, int16 sFamily);

Parameters:
   po:         the interface pointer
   nNetwork:   the network for this Primary QoS Session instance to be linked to.
               One of AEE_NETWORK_* constants.
   sFamily:    address family (AEE_AF_*)

Return Value:
   SUCCESS: 
      the data network was selected.
   
   AEE_NET_EINVAL: 
      the network is not valid.
   
   EBADPARM: 
      invalid input parameter
   
   Other error codes are also possible.

Comments:
   Application can re-select a different data network to which this Primary QoS 
   Session instance would be linked to. In this case all registered callbacks 
   (via IPRIMARYQOSSESSION_AddListener()) would be canceled.

Version:
   Introduced BREW Client <3.1.5> SP01

See Also:
   Data networks

===============================================================================

IPRIMARYQOSSESSION_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   IPRIMARYQOSSESSION_Modify() operation should be retried.

   In the case where IPRIMARYQOSSESSION_Modify() returns AEE_NET_WOULDBLOCK, 
   IPRIMARYQOSSESSION_Modify() should be called again later to obtain the final 
   result (error or success).
   For notification of when to call IPRIMARYQOSSESSION_Modify() again, 
   the client should register a callback via IPRIMARYQOSSESSION_AddRetryCB(). 
   Note that when the client's callback is resumed, it is not guaranteed that 
   a subsequent call to IPRIMARYQOSSESSION_Modify() will complete, so the client 
   must be prepared to receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IPRIMARYQOSSESSION_AddRetryCB(IPrimaryQoSSession *po, AEECallback* pcbRetry);

Parameters:
   po:       the interface pointer
   pcbRetry: the client's callback.

Return Value:
   SUCCESS: 
      client's callback was stored successfully.
   
   EBADSTATE: 
      PrimaryQoSSession state is not suitable for this operation 
      (function call did not follow an IPRIMARYQOSSESSION_Modify() call which 
      returned AEE_NET_WOULDBLOCK)
   
Comments:
   None.

Version:
   Introduced BREW Client <3.1.5> SP01

See Also:
   IPRIMARYQOSSESSION_Modify()

===============================================================================

IPRIMARYQOSSESSION_Modify()

Description:
   This function issues a request to modify Primary QoS's Flow specification 
   in either Rx direction, Tx direction or both directions.

   Modifying a Primary QoS requires AEEPRIVID_PRIMARYQOSSESSION privileges.

   If a network was not yet selected for the IPrimaryQoSSession instance, 
   AEE_NETWORK_DEFAULT & AEE_AF_INET will be implicitly selected.

   Note that a return value of SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that the Primary QoS is already 
   modified - the network may accept or reject this operation.
   After IPRIMARYQOSSESSION_Modify() returns SUCCESS, the client should listen 
   (see IIPRIMARYQOSSESSION_AddListener()) for Primary QoS events: 
   AEE_PRIMARY_QOS_MODIFY_ACCEPTED_EV and AEE_PRIMARY_QOS_MODIFY_REJECTED_EV.
   
   In case the Primary QoS modification request was accepted, all Primary QoS 
   objects which are associated with this Primary QoS and which registered to 
   Primary QoS events will be notified by AEE_PRIMARY_QOS_MODIFIED_EV event 
   (including the modifier instance).

   Note the possibility of an AEE_NET_WOULDBLOCK return value. This occurs
   in cases where a network connection, such as a PPP link, must be established.
   In that case, IPRIMARYQOSSESSION_Modify() returns AEE_NET_WOULDBLOCK, 
   and should be called again to obtain the final result (error or success).
   
   For notification of when to call IPRIMARYQOSSESSION_Modify() again, 
   the client should register a callback through  IPRIMARYQOSSESSION_AddRetryCB(). 
   Note that when the client's callback is resumed, it is not guaranteed that a 
   subsequent call to IPRIMARYQOSSESSION_Modify() will complete, so the client 
   must be prepared to receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IPRIMARYQOSSESSION_Modify(IPrimaryQoSSession *po, 
                                 IQoSList* piRequestedRxQoSFlow, 
                                 IQoSList* piRequestedTxQoSFlow, 
                                 AEEPrimaryQoSModifyMask uPrimaryQoSModifyMask); 

Parameters:
   po:                    the interface pointer
   piRequestedRxQoSFlow:  the Rx Flow Specification list used for modifying the 
                          Primary QoS instance
   piRequestedTxQoSFlow:  the Tx Flow Specification list used for modifying the 
                          Primary QoS instance
   uPrimaryQoSModifyMask: bit mask which indicates the Primary QoS specification 
                          blocks to be modified (see AEEPrimaryQoSModifyMask).
                          This mask should not be empty.

Return Value:
   SUCCESS:
      the request was successfully issued. This doesn't indicate that Primary 
      QoS was already modified.

   EBADPARM: 
      illegal modify mask.

   AEE_NET_EQOS_SPEC_INVALID_OPT:
      the desired Primary QoS specification contains invalid options. Use 
      IPRIMARYQOSSESSION_GetError() to identify which option was invalid.

   EPRIVLEVEL: application does not have privileges to modify Primary QoS
   
   EUNSUPPORTED: operation not supported.

   Other error codes are also possible.

Comments:
   *  The Rx and Tx Flow spec lists compose the Primary QoS Session instance 
      for modification. The two lists of flow specs contains IQoSFlow objects 
      and specify the treatment required for an IP flow for the Rx direction 
      and for the Tx direction respectively.
      The order of IQoSFlow objects in these lists is significant. The first 
      object is the requested / most suitable flow for the client while the 
      last object is the minimum required flow by the client. 
      All other objects are auxiliary flows in decreasing order of desirability 
      in between the requested flow and the minimum required flow.  
      If only one object exists in the list,it is handled as the requested flow.
      In any case where more than a single object exists in the list, in order to
      specify that no minimum flow is required, the last object of the list must 
      be set to an empty flow. (an empty flow is a flow with no QoS options. 
      see AEEQoSFlow.h).
      Rx/Tx Flow lists cannot be empty.
   *  Modification mask should not be empty
   *  To modify Primary QoS Specification in Rx direction only, application 
      should provide AEE_QOS_MODIFY_MASK_RX_FLOW mask and an non empty Flow list 
      in Rx direction (Tx Flow list will be ignored).
   *  To modify Primary QoS Specification in Tx direction only, application 
      should provide AEE_QOS_MODIFY_MASK_TX_FLOW mask and an non empty Flow list 
      in Tx direction (Rx Flow list will be ignored).
   *  To modify Primary QoS Specification in Rx direction and Tx direction , 
      application should provide AEE_QOS_MODIFY_MASK_RX_FLOW|AEE_QOS_MODIFY_MASK_TX_FLOW 
      mask (AEE_QOS_MODIFY_MASK_ALL) and non empty Flow lists in Rx and Tx directions.

Version:
   Introduced BREW Client <3.1.5> SP01

See Also:
   IPRIMARYQOSSESSION_GetError()
   IPRIMARYQOSSESSION_AddListener()
   AEEPrimaryQoSEvent
   AEEPrimaryQoSModifyMask
   IQoSList
   IQoSFlow

===============================================================================

IPRIMARYQOSSESSION_GetGrantedFlowSpec()

Description:
   This function returns the granted Rx and Tx QoS Flow specifications
   for the Primary QoS.

   This function should be called only when the Primary QoS Session's associated 
   network is up.

   If a network was not yet selected for the IPrimaryQoSSession instance, 
   AEE_NETWORK_DEFAULT & AEE_AF_INET will be implicitly selected.
   
   The client is responsible for releasing the returned Rx and Tx Flow Spec
   objects when they are not needed anymore.

Prototype:
   int IPRIMARYQOSSESSION_GetGrantedFlowSpec(IPrimaryQoSSession *po, 
                                             IQoSFlow** ppRxFlowSpec, 
                                             IQoSFlow** ppTxFlowSpec);

Parameters:
   po:           the interface pointer
   ppRxFlowSpec [out]: returns the granted Rx Flow Spec of the Primary QoS. 
                 Can be NULL to indicate that the caller is not interested in 
                 the Rx Flow Spec.
   ppTxFlowSpec [out]: returns the granted Tx Flow Spec of the Primary QoS. 
                 Can be NULL to indicate that the caller is not interested in 
                 the Tx Flow Spec.

Return Value:
   SUCCESS: 
      current Primary QoS's Rx and Tx Flow Specs were successfully retrieved.
   
   EBADSTATE: 
      PrimaryQoSSession state is not suitable for this operation 
      (associated network is not up).

   EUNSUPPORTED: operation not supported.

   Other error codes are also possible.

Comments:
   If both ppRxFlowSpec and ppTxFlowSpec are NULL, the function returns 
   immediately with SUCCESS.

Version:
   Introduced BREW Client <3.1.5> SP01

See Also:
   None.

===============================================================================

IPRIMARYQOSSESSION_GetError()

Description:
   This function returns an AEEQoSError that can be used by clients in order
   to retrieve error information.

   Clients should call this function after an IPRIMARYQOSSESSION_Modify() call
   IQOSSESSION_Open() or IQOSSESSION_ModifyEx() returns 
   which returned AEE_NET_EQOS_SPEC_INVALID_OPT.

Prototype:
   int IPRIMARYQOSSESSION_GetError(IPrimaryQoSSession *po, AEEQoSError* pqError);

Parameters:
   po:            the interface pointer
   pqError [out]: returns the QoS error.

Return Value:
   SUCCESS: 
      error was successfully retrieved into pqError.
   
Comments:
   None.

Version:
   Introduced BREW Client <3.1.5> SP01

See Also:
   AEEQoSError.

=============================================================================*/

#endif // AEEPRIMARYQOSSESSION_H
