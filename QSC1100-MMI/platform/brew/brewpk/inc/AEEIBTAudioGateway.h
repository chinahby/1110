#ifndef AEEIBTAUDIOGATEWAY_H
#define AEEIBTAUDIOGATEWAY_H
/*=============================================================================
FILE:         AEEIBTAudioGateway.h

SERVICES:     IBTAudioGateway Interfaces

DESCRIPTION:  IBTAudioGateway is the interface to the bi-directional 
              Audio Gateway of the underlying Bluetooth driver.
              
              This interface is reserved for applications with system privilege
              because its usage alters the state of phone.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBTDef.h"
#include "AEEISignal.h"

/*=============================================================================
 Interface ID
=============================================================================*/
#define AEEIID_IBTAudioGateway                   0x0105aa38

/*=============================================================================
 BTAG Operation types
=============================================================================*/
#define AEEBTAG_OP_NONE                0
#define AEEBTAG_OP_CONFIG              1
#define AEEBTAG_OP_ENABLE              2
#define AEEBTAG_OP_DISABLE             3
#define AEEBTAG_OP_CONNECT             4
#define AEEBTAG_OP_DISCONNECT          5
#define AEEBTAG_OP_CONNECT_AUDIO       6
#define AEEBTAG_OP_DISCONNECT_AUDIO    7

typedef uint32  BTAGOperationType;

/*=============================================================================
 BTAG Event types
=============================================================================*/
#define AEEBTAG_EV_NONE                0
#define AEEBTAG_EV_ENABLED             1
#define AEEBTAG_EV_DISABLED            2
#define AEEBTAG_EV_CONFIG_UPDATED      3
#define AEEBTAG_EV_CONNECTED           4
#define AEEBTAG_EV_DISCONNECTED        5
#define AEEBTAG_EV_AUDIO_CONNECTED     6
#define AEEBTAG_EV_AUDIO_DISCONNECTED  7
#define AEEBTAG_EV_BUTTON_PRESS        8
#define AEEBTAG_EV_VR_ACT_RCVD         9

typedef uint32  BTAGEventType;

/*=============================================================================
 BTAG States
=============================================================================*/
#define AEEBTAG_STATE_DISABLED         1
#define AEEBTAG_STATE_ENABLED          2
#define AEEBTAG_STATE_CONNECTED        3
#define AEEBTAG_STATE_AUDIO_CONNECTED  4

typedef uint32  BTAGState;

/*=============================================================================
 BTAG HS Button Function selection
=============================================================================*/
#define AEEBTAG_HSBUTTON_IGNORE        1
#define AEEBTAG_HSBUTTON_DIAL          2
#define AEEBTAG_HSBUTTON_REDIAL        3
#define AEEBTAG_HSBUTTON_VR            4
#define AEEBTAG_HSBUTTON_DEFER         5

typedef uint32  BTAGButtonFunction;

/*=============================================================================
 BTAG HS config flags
=============================================================================*/
#define AEEBTAG_CFG_PRIVATE_B             0x0001
#define AEEBTAG_CFG_SEND_KEYBEEP_B        0x0002
#define AEEBTAG_CFG_NO_AUTO_RECONN_B      0x0004
#define AEEBTAG_CFG_VIDEOPHONE_ENABLED_B  0x0008
// more flag bits to be defined

typedef uint32  BTAGConfigFlags;

/*=============================================================================
 Structures
=============================================================================*/
typedef struct
{
   BTConnectionState  dConnState;
   BTConnectionState  dAudioConnState;
   BDAddress          remoteAddr;
   BTServiceClassUUID dServiceUUID;
   uint32             uServerChannel;
}  BTAGConnection;

typedef struct
{
   BTAGState          dState;
   BTAGOperationType  dPendingOperation;
   int                nNumConnections;
   int                nMaxNumConnSupported;
}  BTAGStatus;

/*=============================================================================
 Interface Definition
=============================================================================*/
#define INHERIT_IBTAudioGateway(iname)  \
   INHERIT_IQI(iname);        \
   int (*OnOpResult)         (iname*                 po,            \
                              ISignal*               piSignal);     \
   int (*GetOpResult)        (iname*                 po,            \
                              BTAGOperationType*     pdOpID,        \
                              BTResult*              pdResult);     \
   int (*OnEventNotify)      (iname*                 po,            \
                              ISignal*               piSignal);     \
   int (*GetEvent)           (iname*                 po,            \
                              BTAGEventType*         pdEvent);      \
   int (*GetConnectionList)  (iname*                 po,            \
                              BTAGConnection*        pConnList,     \
                              int                    nNumEntries,   \
                              int*                   pnReqEntries); \
   int (*GetStatus)          (iname*                 po,            \
                              BTAGStatus*            pStatus);      \
   int (*Config)             (iname*                 po,            \
                              BTAGConfigFlags        dCfgFlags,     \
                              BTAGButtonFunction     dButtonAction, \
                              BTSecurityLevel        dSecurity,     \
                              const char*            pszDialString, \
                              const BDAddress*       pAddrList,     \
                              int                    nNumAddr);     \
   int (*ConfigEx)           (iname*                 po,            \
                              const char*            pszConfigStr); \
   int (*Enable)             (iname*                 po);           \
   int (*Disable)            (iname*                 po);           \
   int (*Connect)            (iname*                 po,            \
                              const BDAddress*       pAddr);        \
   int (*Disconnect)         (iname*                 po,            \
                              const BDAddress*       pAddr);        \
   int (*ConnectAudio)       (iname*                 po,            \
                              const BDAddress*       pAddr);        \
   int (*DisconnectAudio)    (iname*                 po,            \
                              const BDAddress*       pAddr);        \
   int (*ReportVRState)      (iname*                 po,            \
                              boolean                bVRActive)


AEEINTERFACE_DEFINE(IBTAudioGateway);


static __inline uint32 IBTAudioGateway_AddRef (IBTAudioGateway* p)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->AddRef((p));
}

static __inline uint32 IBTAudioGateway_Release (IBTAudioGateway* p)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->Release((p));
}

static __inline int IBTAudioGateway_QueryInterface (IBTAudioGateway* p,
                                          AEECLSID clsid,
                                          void **ppo)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->QueryInterface((p),(clsid),(ppo));
}

static __inline int IBTAudioGateway_OnOpResult (IBTAudioGateway* p, 
                                                ISignal* piSignal)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->OnOpResult((p),(piSignal));
}

static __inline int IBTAudioGateway_GetOpResult (IBTAudioGateway* p, 
                                                 BTAGOperationType* pdOpID,
                                                 BTResult* pdR)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->GetOpResult((p),(pdOpID),(pdR));
}

static __inline int IBTAudioGateway_OnEventNotify (IBTAudioGateway* p, 
                                                   ISignal* piSignal)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->OnEventNotify((p),(piSignal));
}

static __inline int IBTAudioGateway_GetEvent (IBTAudioGateway* p, 
                                              BTAGEventType* pdEvent)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->GetEvent((p),(pdEvent));
}

static __inline int IBTAudioGateway_GetConnectionList (IBTAudioGateway* p, 
                                                       BTAGConnection* pC,
                                                       int n,
                                                       int* pn)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->GetConnectionList((p),(pC),(n),(pn));
}

static __inline int IBTAudioGateway_GetStatus (IBTAudioGateway* p, 
                                               BTAGStatus* pStatus)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->GetStatus((p),(pStatus));
}

static __inline int IBTAudioGateway_Config (IBTAudioGateway* p, 
                                            BTAGConfigFlags dCfgFlags,
                                            BTAGButtonFunction dB,
                                            BTSecurityLevel dS,
                                            const char* pszDS,
                                            const BDAddress* pAddr,
                                            int nNumAddr)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->Config((p),(dCfgFlags),(dB),(dS),
                                                   (pszDS),(pAddr),(nNumAddr));
}

static __inline int IBTAudioGateway_ConfigEx (IBTAudioGateway* p, 
                                              const char* pszBuf)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->ConfigEx((p),(pszBuf));
}

static __inline int IBTAudioGateway_Enable (IBTAudioGateway* p)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->Enable((p));
}

static __inline int IBTAudioGateway_Disable (IBTAudioGateway* p)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->Disable((p));
}

static __inline int IBTAudioGateway_Connect (IBTAudioGateway* p, 
                                             const BDAddress* pA)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->Connect((p),(pA));
}

static __inline int IBTAudioGateway_Disconnect (IBTAudioGateway* p, 
                                                const BDAddress* pA)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->Disconnect((p),(pA));
}

static __inline int IBTAudioGateway_ConnectAudio (IBTAudioGateway* p, 
                                                  const BDAddress* pA)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->ConnectAudio((p),(pA));
}

static __inline int IBTAudioGateway_DisconnectAudio (IBTAudioGateway* p, 
                                                     const BDAddress* pA)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->DisconnectAudio((p),(pA));
}

static __inline int IBTAudioGateway_ReportVRState (IBTAudioGateway* p, 
                                                   boolean bVRActive)
{
   return AEEGETPVTBL((p),IBTAudioGateway)->ReportVRState((p),(bVRActive));
}

/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTAGOperationType

Description:
   When an operation completes, the registered OnOpResult signal would be 
   invoked and associated operation code would be returned via 
   IBTAudioGateway_GetOpResult() along with result of the operation.
   
Definition:
   #define AEEBTAG_OP_NONE                0
   #define AEEBTAG_OP_CONFIG              1
   #define AEEBTAG_OP_ENABLE              2
   #define AEEBTAG_OP_DISABLE             3
   #define AEEBTAG_OP_CONNECT             4
   #define AEEBTAG_OP_DISCONNECT          5
   #define AEEBTAG_OP_CONNECT_AUDIO       6
   #define AEEBTAG_OP_DISCONNECT_AUDIO    7
   typedef uint32  BTAGOperationType;

===============================================================================
BTAGEventType

Description:
   Possible events that can occur on AG object, retrieved via 
   IBTAudioGateway_GetEvent().
   
Definition:
   #define AEEBTAG_EV_ENABLED             1
   #define AEEBTAG_EV_DISABLED            2
   #define AEEBTAG_EV_CONFIG_UPDATED      3
   #define AEEBTAG_EV_CONNECTED           4
   #define AEEBTAG_EV_DISCONNECTED        5
   #define AEEBTAG_EV_AUDIO_CONNECTED     6
   #define AEEBTAG_EV_AUDIO_DISCONNECTED  7
   #define AEEBTAG_EV_BUTTON_PRESS        8
   #define AEEBTAG_EV_VR_ACT_RCVD         9
   typedef uint32  BTAGEventType;

===============================================================================
BTAGState

Description:
   This data type specifies the possible states in which AG object can be.
   
Definition:
   #define AEEBTAG_STATE_DISABLED         1
   #define AEEBTAG_STATE_ENABLED          2
   #define AEEBTAG_STATE_CONNECTED        3
   #define AEEBTAG_STATE_AUDIO_CONNECTED  4
   typedef uint32  BTAGState;

===============================================================================
BTAGButtonFunction

Description:
   Operating in Headset profile, a headset would send a Button Press to AG when
   user presses the only button, or the Answer button (typically green in color)
   on the headset.  It is up to AG to interpret the Button Press and take
   appropriate action.  BTAGButtonFunction specifies the possible actions that
   can be taken by AG when it receives Button Press from headset.  Application
   can specify the action it wishes AG to take via IBTAudioGateway_Config().
   
Definition:
   #define AEEBTAG_HSBUTTON_IGNORE        1
   #define AEEBTAG_HSBUTTON_DIAL          2
   #define AEEBTAG_HSBUTTON_REDIAL        3
   #define AEEBTAG_HSBUTTON_VR            4
   #define AEEBTAG_HSBUTTON_DEFER         5
   typedef uint32  BTAGButtonFunction;

Members:
   AEEBTAG_HSBUTTON_IGNORE  : ignore Button Press; do nothing
   AEEBTAG_HSBUTTON_DIAL    : dial the pre-set number
   AEEBTAG_HSBUTTON_REDIAL  : dial the last dialed number
   AEEBTAG_HSBUTTON_VR      : activate Voice Recognition engine
   AEEBTAG_HSBUTTON_DEFER   : defer to app; app will handle this button press

===============================================================================
BTAGConfigFlags

Description:
   Each bit specifies a boolean setting.
   
Definition:
   #define AEEBTAG_CFG_PRIVATE_B             0x0001
   #define AEEBTAG_CFG_SEND_KEYBEEP_B        0x0002
   #define AEEBTAG_CFG_NO_AUTO_RECONN_B      0x0004
   #define AEEBTAG_CFG_VIDEOPHONE_ENABLED_B  0x0008
   typedef uint32  BTAGConfigFlags;

Members:
   AEEBTAG_CFG_PRIVATE_B            : call audio gets routed to HS only if call
                                        is answered/originated by HS;
   AEEBTAG_CFG_SEND_KEYBEEP_B       : keybeep tones should be sent to HS; this
                                        implies that audio connection would get
                                        brought up when key is pressed
   AEEBTAG_CFG_NO_AUTO_RECONN_B     : should not automatically re-connect to HS;
                                        app must explicitly initiate connection
   AEEBTAG_CFG_VIDEOPHONE_ENABLED_B : make video call instead of voice call

===============================================================================
BTAGConnection

Description:
   Specifies information for each connection between AG and an audio device.

Definition:
   typedef struct
   {
      BTConnectionState   dConnState;
      BTConnectionState   dAudioConnState;
      BDAddress           remoteAddr;
      BTServiceClassUUID  dServiceUUID;
      uint32              uServerChannel;
   }  BTAGConnection;

Members:
   dConnState       : AEEBT_CONN_STATE_xxx
   dAudioConnState  : AEEBT_CONN_STATE_xxx; valid only if connState==CONNECTED
   remoteAddr       : remote end of the connection
   dServiceUUID     : Headset? or Hands-free?
   uServerChannel   : server channel number

===============================================================================
BTAGStatus

Description:
   Specifies current status of AG object.

Definition:
   typedef struct
   {
      BTAGState          dState;
      BTAGOperationType  dPendingOperation;
      int                nNumConnections;
      int                nMaxNumConnSupported;
   }  BTAGStatus;

Members:
   dState               : current state of AG object
   dPendingOp           : AEEEBTAG_OP_xxx
   nNumConnections      : 0 or more
   nMaxNumConnSupported : number of concurrent audio devices supported by
                            the underlying AG subsystem

=============================================================================*/

/*=============================================================================
INTERFACE DOCUMENTATION
===============================================================================
IBTAudioGateway Interface

Description:
   This interface provides methods to control the Audio Gateway of the 
   underlying Bluetooth driver, enabling connectivity between phone and 
   Bluetooth devices supporting Headset and/or Handsfree profiles.
   Users of this interface must have system privilege.

===H2>
   Usage example:
===/H2>

   ===== To create an IBTAudioGateway object, see instruction in 
         class header file

   ===== To configure AG:
   pMe->psAGDevList = MALLOC (AEEBTAG_MAX_DEVICES * sizeof(BDAddress));
   // fill list with address of Headsets/Hands-frees
   pMe->dAGSecurity = AEEBT_SEC_xxx;
   pMe->dAGButtonAct = AEEBT_HSBUTTON_DIAL;
   // set pszDialString with user-selected number; only needed if 
   // pMe->dAGButtonAct is set to AEEBT_HSBUTTON_DIAL
   pMe->dCfgFlags = AEEBTAG_CFG_PRIVATE_B | AEEBTAG_CFG_CALL_AUTO_CONN_B |
                    AEEBTAG_CFG_VIDEOPHONE_ENABLED_B;
   IBTAudioGateway_Config (pMe->piBTAG, pMe->dCfgFlags, pMe->dAGButtonAct, 
                           pMe->dAGSecurity, pszDialString, pMe->psAGDevList, 
                           nNumDevs);


   ===== To register signal
   // first create the signals; for example, use ISignalCBFactory_CreateSignal()
   // then register the signals:
   IBTAudioGateway_OnOpResult (pMe->piBTAG, pMe->piAGOpResultSignal);
   IBTAudioGateway_OnEventNotify (pMe->piBTAG, pMe->piAGEventSignal);

   
   ===== To enable AG:
   IBTAudioGateway_Enable (pMe->piBTAG);


   ===== To retrieve and handle events when OpFailure signal is triggered:
   if (IBTAudioGateway_GetOpResult (pMe->piBTAG, &dOp, &dResult) == AEE_SUCCESS)
   {
      if (dResult != AEEBT_RESULT_SUCCESS)
         // handle errors
   }
   ISignalCtl_Enable (pMe->piAGOpResultSignalCtl); // re-enable signal


   ===== To retrieve and handle events when Event signal is triggered:
   while (IBTAudioGateway_GetEvent (pMe->piBTAG, &dEvent) != AEE_ENOMORE)
   {
      switch (dEvent)
      {
         case AEEBTAG_EV_ENABLED:
         ...
         case AEEBTAG_EV_VR_ACT_RCVD:
      }
   }
   ISignalCtl_Enable (pMe->piAGEventSignalCtl); // re-enable signal


   ===== To retrieve current AG status:
   if ((result = IBTAudioGateway_GetStatus (pMe->piBTAG, pMe->pAGStatus)) == AEE_SUCCESS)
   {
      // how many concurrent connections possible?
      if ((NULL == pMe->pAGConns) && (pMe->pAGStatus->nMaxNumConnSupported > 0))
      {
         pMe->pAGConns = MALLOC (pMe->pAGStatus->nMaxNumConnSupported * 
                                 sizeof(BTAGConnection));
      )
      // perhaps update annunciator with new state
      UpdateAnnun (pMe->pAGStatus->dState);
      ...
   }
   

   ===== To retrieve connection info when DIS/CONNECTED event is received:
   int nNumEntries = 0;
   IBTAudioGateway_GetConnectionList (pMe->piBTAG, pMe->psAGConns, 
                                      pMe->pAGStatus->nMaxNumConnSupported, 
                                      &nNumEntries));


   ===== To connect/disconnect:
   IBTAudioGateway_Connect (pMe->piBTAG, &addrOfMyFavoriteDevice);
   IBTAudioGateway_Disconnect (pMe->piBTAG, &addrOfMyFavoriteDevice);


   ===== To connect/disconnect audio:
   IBTAudioGateway_ConnectAudio (pMe->piBTAG, &addrOfMyFavoriteDevice);
   IBTAudioGateway_DisconnectAudio (pMe->piBTAG, &addrOfMyFavoriteDevice);


   ===== To report VR engine state:
   IBTAudioGateway_ReportVRState (pMe->piBTAG, bVRActive);


   ===== To disable AG:
   IBTAudioGateway_Disable (pMe->piBTAG);


   ===== To release an IBTAudioGateway object:
   IBTAudioGateway_Release (pMe->piBTAG);

===============================================================================
IBTAudioGateway_AddRef

Description:
   Inherited from IQI_AddRef().

See Also:
   IBTAudioGateway_Release()
   
===============================================================================
IBTAudioGateway_Release

Description:
   Inherited from IQI_Release().

See Also:
   IBTAudioGateway_AddRef()
   
===============================================================================
IBTAudioGateway_QueryInterface

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBTAudioGateway-derived class

Prototype:
   int IBTAudioGateway_QueryInterface (IBTAudioGateway*  po, 
                                       AEECLSID          clsReq, 
                                       void**            ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   If ppo is back a NULL pointer, the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================
IBTAudioGateway_OnOpResult()
   
Description:
   This method a signal to be triggered when the current operation completes.

Prototype:
   int IBTAudioGateway_OnOpResult (IBTAudioGateway*   po, 
                                   ISignal*           piSignal);

Parameters:
   po [i]              : the object
   piSignal [i]        : signal to be triggered when operation is done

Return values:
   AEE_SUCCESS         : signal has been scheduled
   AEE_EBADPARM        : input pointers are invalid

See Also:
   IBTAudioGateway_GetOpResult()

===============================================================================
IBTAudioGateway_GetOpResult()
   
Description:
   This method retrieves information on the just completed operation.
   
   Application should call this function after the signal registered via
   IBTAudioGateway_OnOpResult() is triggered.

   
Prototype:
   int IBTAudioGateway_GetOpResult (IBTAudioGateway*    po, 
                                    BTAGOperationType*  pdOpID
                                    BTResult*           pdResult);

Parameters:
   po [i]              : the object
   pdOpID [o]          : ID of operation
   pdResult [o]        : result of operation; should be a falure of some sort

Return values:
   AEE_SUCCESS         : operation result successfully retrieved
   AEE_EBADPARM        : input parameters are invalid
   AEE_ENOMORE         : no data to retrieve

See Also:
   IBTAudioGateway_OnOpResult()

===============================================================================
IBTAudioGateway_OnEventNotify()
   
Description:
   This method registers a signal to be triggered when there is an event
   that occurred on the AG object.  Application should use 
   IBTAudioGateway_GetEvent() to retrieve the event information.
   
Prototype:
   int IBTAudioGateway_OnEventNotify (IBTAudioGateway*  po, 
                                      ISignal*          piSignal);

Parameters:
   po [i]              : the object
   piSignal [i]        : signal to be triggered when an event occurs

Return values:
   AEE_SUCCESS         : signal has been scheduled
   AEE_EBADPARM        : input pointers are invalid

See Also:
   IBTAudioGateway_GetEvent()
   
===============================================================================
IBTAudioGateway_GetEvent()

Description:
   This method retrieves AG events.  This API should be called when the signal
   registered via IBTAudioGateway_OnEventNotify() is triggered.

Prototype:
   int IBTAudioGateway_GetEvent (IBTAudioGateway*  po, 
                                 BTAGEventType*    pdEvent);

Parameters:
   po [i]              : the object
   pdEvent [o]         : destination for the event code

Return values:
   AEE_SUCCESS         : event info successfully retrieved
   AEE_EBADPARM        : input pointers are invalid

See Also:
   IBTAudioGateway_OnEventNotify()
   
===============================================================================
IBTAudioGateway_GetConnectionList()
   
Description:
   This method retrieves current list of AG connections.  This API can be called
   any time.

Prototype:
   int IBTAudioGateway_GetConnectionList (IBTAudioGateway* po, 
                                          BTAGConnection*  pConnList,
                                          int              nNumEntries,
                                          int*             pnNumConns);

Parameters:
   po [i]              : the object
   pConnList [o]       : destination for info on all existing AG connections
   nNumEntries [i]     : max number of entries the list can hold
   pnNumConns [o]      : number of existing connections, no more than the max
                           concurrent connections supported by underlying AG

Return values:
   AEE_SUCCESS         : connection list successfully retrieved
   AEE_EBADPARM        : input pointers are invalid
   AEE_EBUFFERTOOSMALL : pnNumConns will contain the required size

See Also:
   BTAGStatus

===============================================================================
IBTAudioGateway_GetStatus()
   
Description:
   This method retrieves current status of AG object.  This API can be called
   any time.

   AG status could be changed due to an operation initiated by application,
   by an internal event (such as an incoming call), or by remote device.
   
Prototype:
   int IBTAudioGateway_GetStatus (IBTAudioGateway*  po, 
                                  BTAGStatus*       pStatus);

Parameters:
   po [i]              : the object
   pStatus [o]         : destination for AG status

Return values:
   AEE_SUCCESS         : AG status successfully retrieved
   AEE_EBADPARM        : input pointers are invalid

See Also:
   IBTAudioGateway_Config()
   IBTAudioGateway_Enable()
   IBTAudioGateway_Disable()

===============================================================================
IBTAudioGateway_Config()
   
Description:
   This method configures the Audio Gateway subsystem.  It can be used before
   or after the Audio Gateway is enabled.

   If AG is configured with a list of BD addresses, AG will automatically 
   initiate connection to these devices when there's an incoming call, unless
   dCfgFlags has the AEEBTAG_CFG_NO_AUTO_RECONN_B set, or when user makes an
   outgoing call from the phone and AEEBTAG_CFG_PRIVATE_B of dCfgFlags is not
   set.  If connection attempt to one device fails, AG will try the next one 
   on the list.  This process stops with the first successful connection.

   AG will accept connection request from any device, provided they pass the 
   security check.  To prevent connection requests coming from devices not
   on BD address list from getting accepted automatically, the phone should
   have a resident Bluetooth application that can use IBTMgr_OnConnRequest() to 
   register for connection request notification, and IBTMgr_AuthorizeConnReq() 
   to either accept or reject the request.

   New configuration takes effect immediately regardless of presence of a
   connection.  Depending on the underlying Bluetoooth driver, increased level
   of security may bring down existing connections.  Also, configuration data 
   is not expected to be retained by the interface after the object is 
   released, or across power cycles.

Prototype:
   int IBTAudioGateway_Config (IBTAudioGateway*   po, 
                               BTAGConfigFlags    dCfgFlags,
                               BTAGButtonFunction dButtonAction,
                               BTSecurityLevel    dSecurity,
                               const char*        pszDialString,
                               const BDAddress*   pAddrList,
                               int                nNumAddr);

Parameters:
   po [i]              : the object
   dCfgFlags [i]       : bitmask for config flags
   dButtonAction [i]   : how to interpret Button Press
   dSecurity [i]       : level of security to enforce
   pszDialString [i]   : pre-set dial string; only required if 
                           dButtonAction == AEEBTAG_HSBUTTON_DIAL
   pAddrList [i]       : list of devices to which to connect
   nNumAddr [i]        : number of valid entries on psAddrList; only the first
                           AEEBTAG_MAX_DEVICES entries will be saved

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : invalid config values
   AEE_EALREADY        : the previous Config operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTMgr
   BTAGConfigFlags
   BTAGButtonFunction
   BTSecurityLevel

===============================================================================
IBTAudioGateway_ConfigEx()
   
Description:
   This method is an alternative to IBTAudioGateway_Config().

   Like IBTAudioGateway_Config(), this method configures the Audio Gateway 
   subsystem and can be used before or after the Audio Gateway is enabled.
   Unlike IBTAudioGateway_Config(), this method takes config parameters in 
   string format, and can also be used to update selective config values 
   application wishes to change.

   The paramters are expected in name=value and separated by semicolon.
   Only specified config items are updated.  Config values for unspecified 
   items remain unchanged.

   Name        Value
   -----------------
   flags     : any combination of (private, keybeep, call_connect, vp)
                 separated by comma, or "none" if none is set
   button    : one of (ignore, dial, redial, vr, or defer)
   sec       : one of (none, auth, or auth+encrypt)
   dialstr   : null-terminated string representing the number to dial,
                 enclosed in single quotes
   bdaddr    : 12-digit hex value string, separated by comma

   Examples param string:
   "button=redial;sec=none;numdev=2;bdaddr=9abc56781122,9abc56784455;", or

   "flags=private,call_connect;button=ignore;sec=auth+encrypt;"
   "bdaddr=9abc56781122;", or

   "flags=private,vp;button=dial;sec=auth+encrypt;dialstr='555-1212';"

Prototype:
   int IBTAudioGateway_ConfigEx (IBTAudioGateway*  po, 
                                 const char*       pszParam);

Parameters:
   po [i]              : the object
   pszParam [i]        : config params in string format

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : invalid config values, or missing required config items
   AEE_EALREADY        : the previous Config operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTAudioGateway_Config()

===============================================================================
IBTAudioGateway_Enable()
   
Description:
   This method activates the Audio Gateway subsystem.  As a result of
   this operation, the local Bluetooth device becomes connectable.  Headsets
   and Hands-free devices can now connect to AG provided they pass security 
   check.

Prototype:
   int IBTAudioGateway_Enable (IBTAudioGateway* po);

Parameters:
   po [i]              : the object

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : input pointers are invalid
   AEE_EBADSTATE       : AG is not in DISABLED state
   AEE_EALREADY        : the previous Enable operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTAudioGateway_Disable()
   
===============================================================================
IBTAudioGateway_Disable()
   
Description:
   This method deactivate the Audio Gateway subsystem.  Existing connection(s)
   between phone and audio devices will be brought down as a result, and AG
   will become unconnectable.  Configuration data, however, will remain intact.

Prototype:
   int IBTAudioGateway_Disable (IBTAudioGateway*  po);

Parameters:
   po [i]              : the object

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : input pointers are invalid
   AEE_EBADSTATE       : AG is already in DISABLED state
   AEE_EALREADY        : the previous Disable operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTAudioGateway_Enable()
   
===============================================================================
IBTAudioGateway_Connect()
   
Description:
   This method initiates connection to a given audio device.

   Application may initiate connection to any device, on or off the BD address
   list given in IBTAudioGateway_Config().

   Depending on the underlying AG subsystem, local device may or may not be
   able to maintain connection with more than one audio device concurrently.
   Via IBTAudioGateway_GetStatus(), application can determine the maximum number of
   concurrent connections support by the underlying AG subsystem.

Prototype:
   int IBTAudioGateway_Connect (IBTAudioGateway* po,
                                const BDAddress* pAddr );

Parameters:
   po [i]              : the object
   pAddr [i]           : address of audio device to which to connect

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : input pointers are invalid
   AEE_EBADSTATE       : Audio Gateway is in DISABLED state
   AEE_EALREADY        : the previous Connect operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTAudioGateway_Enable()
   IBTAudioGateway_Disconnect()
   
===============================================================================
IBTAudioGateway_Disconnect()
   
Description:
   This method initiates discconnection from a given audio device.

Prototype:
   int IBTAudioGateway_Disconnect (IBTAudioGateway* po,
                                   const BDAddress* pAddr );

Parameters:
   po [i]              : the object
   pAddr [i]           : address of audio device from which to disconnect

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : input pointers are invalid
   AEE_EBADSTATE       : AG is not in CONNECTED or AUDIO_CONNECTED state
   AEE_EALREADY        : the previous Disconnect operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTAudioGateway_Enable()
   IBTAudioGateway_Connect()
   
===============================================================================
IBTAudioGateway_ConnectAudio()
   
Description:
   This method initiates audio connection to a given remote device.

   Application may initiate audio connection to any device, on or off the 
   BD address list given in IBTAudioGateway_Config().

   Depending on the underlying AG subsystem, local device may or may not be 
   able to maintain audio connection with more than one audio device 
   concurrently.

Prototype:
   int IBTAudioGateway_ConnectAudio (IBTAudioGateway* po,
                                     const BDAddress* pAddr );

Parameters:
   po [i]              : the object
   pAddr [i]           : address of device to which to set up audio connection

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : input pointers are invalid
   AEE_EBADSTATE       : AG is not in CONNECTED state
   AEE_EALREADY        : the previous ConnectAudio operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTAudioGateway_Enable()
   IBTAudioGateway_DisconnectAudio()
   
===============================================================================
IBTAudioGateway_DisconnectAudio()
   
Description:
   This method initiates audio discconnection from a given audio device.

Prototype:
   int IBTAudioGateway_DisconnectAudio (IBTAudioGateway* po,
                                        const BDAddress* pAddr );

Parameters:
   po [i]              : the object
   pAddr [i]           : address of audio device from which to disconnect
                           the audio link

Return values:
   AEE_SUCCESS         : operation has been scheduled; signals that
                           may get triggered are those registered via
                           OnOpResult() and OnEventNotify()
   AEE_EBADPARM        : input pointers are invalid
   AEE_EBADSTATE       : AG is not in AUDIO_CONNECTED state
   AEE_EALREADY        : the previous DisconnectAudio operation hasn't completed
   AEE_EITEMBUSY       : another operation is in progress

See Also:
   IBTAudioGateway_Enable()
   IBTAudioGateway_ConnectAudio()
   
===============================================================================
IBTAudioGateway_ReportVRState()
   
Description:
   This method informs the AG object of current state of the VR engine.

Prototype:
   int IBTAudioGateway_ReportVRState (IBTAudioGateway*  po,
                                      boolean           bVRActive);

Parameters:
   po [i]              : the object
   bVRActive [i]       : TRUE if VR engine got activated; FALSE de-activated

Return values:
   AEE_SUCCESS         : VR state successfully reported
   AEE_EFAILED         : Failed to report VR state 

==========================================================================*/

#endif // AEEIBTAUDIOGATEWAY_H

