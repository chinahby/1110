/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

   S O C K E T   A P I   H E A D E R   F I L E
                   
DESCRIPTION

 The OEM Socket Header File. Contains declarations for functions.


        Copyright © 2000-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
===========================================================================*/
#ifndef OEMSOCK_H
#define OEMSOCK_H


#include "AEE_OEMComdef.h"
#include "AEEComdef.h"
#include "AEEError.h"
#include "AEENet.h"

#include "AEEBearerTechnology.h"
#include "AEENetworkTypes.h"
#include "AEEQoSSession.h"
#include "AEEPrimaryQoSSession.h"
#include "AEEQoSBundle.h"
#include "AEEMcastSession.h"
#include "AEESockPort.h"

#define AEE_NET_WRITE_EVENT  0x01     /* writeable socket */
#define AEE_NET_READ_EVENT   0x02     /* readable socket */
#define AEE_NET_CLOSE_EVENT  0x04     /* closeable socket */

#include "OEMSock_QoSSpec.h"

typedef uint32 oem_mcast_handle_type;

#if defined(__cplusplus)
extern "C" {
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern OEMCONTEXT OEMNet_Open
(
   int          nNetwork,
   int16        nFamily,
   AEECallback *pcbNetWaiter,
   AEECallback *pcbSocketWaiter,
   AEECallback *pcbQoSWaiter,
   AEECallback *pcbMcastWaiter,
   AEECallback *pcbQoSAwareUnawareWaiter,
   AEECallback *pcbBearerTechnologyChangeWaiter,
   AEECallback *pcbIPv6PrivAddrWaiter,
   AEECallback *pcbPrimaryQoSModifyResultWaiter,
   AEECallback *pcbPrimaryQoSModifyWaiter,
   AEECallback *pcbOutageWaiter,
   AEECallback *pcbQoSProfilesChangedWaiter,
   AEECallback *pcbIPv6PrefixUpdateWaiter,
   int16       *err
);

extern int16 OEMNet_Close
(
   OEMCONTEXT netd
);

extern void OEMNet_SetNetWaiter
(
   OEMCONTEXT   netd,
   AEECallback *pcbWaiter
);

extern void OEMNet_SetMTPDWaiter
(
   OEMCONTEXT   netd,
   AEECallback *pcbWaiter
);

extern void OEMNet_SetQoSWaiter
(
   OEMCONTEXT   netd,
   AEECallback *pcbWaiter
);

extern void OEMNet_SetMcastWaiter
(
   OEMCONTEXT   netd,
   AEECallback *pcbWaiter
);

extern void OEMNet_SetQoSAwareUnawareWaiter
(
   OEMCONTEXT   netd,
   AEECallback *pcbWaiter
);

extern void OEMNet_SetBearerTechnologyChangeWaiter
(
  OEMCONTEXT   netd,
  AEECallback *pcbWaiter
);

extern void OEMNet_SetIPv6PrivAddrWaiter
(
  OEMCONTEXT   netd,
  AEECallback *pcbWaiter
);

extern void OEMNet_SetIPv6PrefixUpdateWaiter
(
 OEMCONTEXT   netd,
 AEECallback *pcbWaiter
 );

extern void OEMNet_SetPrimaryQoSModifyResultWaiter
(
  OEMCONTEXT   netd,
  AEECallback *pcbWaiter
);

extern void OEMNet_SetPrimaryQoSModifyWaiter
(
  OEMCONTEXT   netd,
  AEECallback *pcbWaiter
);

extern void OEMNet_SetOutageWaiter
(
  OEMCONTEXT   netd,
  AEECallback *pcbWaiter
);

extern void OEMNet_SetQoSProfilesChangedWaiter
(
  OEMCONTEXT   netd,
  AEECallback *pcbWaiter
);

extern int16 OEMNet_RegisterToMTPD
(
   OEMCONTEXT   netd,
   AEECallback *pcbWaiter
);

extern int16 OEMNet_DeRegisterFromMTPD
(
   OEMCONTEXT   netd
);

extern void OEMNet_SetSocketWaiter
(
   OEMCONTEXT   netd,
   AEECallback *pcbWaiter
);

extern PFNNOTIFY OEMNet_InstallSocketEventHandler
(
   OEMCONTEXT netd,
   PFNNOTIFY
);

extern int16 OEMNet_PPPOpen
(
   OEMCONTEXT netd
);

extern int16 OEMNet_PPPClose
(
   OEMCONTEXT netd
);

extern NetState OEMNet_PPPState
( 
   OEMCONTEXT netd
);

extern int16 OEMNet_NameServers
(
   OEMCONTEXT netd,
   IPAddr    *ainaAddrs,
   int       *pnNumAddrs
);

extern int16 OEMNet_GetRLP3Cfg
(
   OEMCONTEXT  netd,
   int16       nOptName,
   AEERLP3Cfg *prlp3
);

extern int16 OEMNet_SetRLP3Cfg
(
   OEMCONTEXT        netd,
   int16             nOptName,
   const AEERLP3Cfg *prlp3
);

extern int16 OEMNet_SetPPPAuth
(
   OEMCONTEXT  netd,
   const char *pszAuth
);

extern int16 OEMNet_GetPPPAuth
(
   OEMCONTEXT netd,
   char      *pszAuth, 
   int       *pnLen
);

extern int16 OEMNet_SetDDTMPref
(
   OEMCONTEXT netd,
   boolean    bOn
);

extern int16 OEMNet_MyIPAddr
(
   OEMCONTEXT netd,
   IPAddr    *addr
);

extern int16 OEMNet_GetUrgent
(
   OEMCONTEXT    netd,
   AEEUDPUrgent *pUrgent
);

extern int16 OEMNet_GetNativeDescriptor
(
   OEMCONTEXT netd,
   uint32    *pdwDesc
);

extern int16 OEMNet_GetDefaultNetwork
(
   int16      sFamily,
   int       *pnNetwork   
);

extern int16 OEMNet_GetAppProfileId
(
    uint32 uAppType, 
    int16 *pnProfile
 );

extern int16 OEMNet_GetUMTSCount
(
   uint16    *pwCount
);

extern int16 OEMNet_GetUMTSInfo
(
   uint16       wNumber,
   AEEUMTSInfo* pInfo
);

extern int16 OEMNet_GetHWAddr
(
   OEMCONTEXT netd,
   byte       addr[],
   int*       pnSize
);

extern int16 OEMNet_SetDormancyTimeout
(
   OEMCONTEXT netd,
   uint8      ucSecs
);

extern int16 OEMNet_McastJoin
(
 OEMCONTEXT             netd,
 AEESockAddrStorage*    psaGroupAddr,
 oem_mcast_handle_type *pHandle
);

extern int16 OEMNet_McastJoinEx
(
 OEMCONTEXT            netd,
 AEESockAddrStorage*   apsaGroupAddresses[],
 oem_mcast_handle_type aHandles[],
 boolean               aSendReg[],
 int                   nNumOfRequests
);

extern int16 OEMNet_McastLeave
(
 OEMCONTEXT             netd,
 oem_mcast_handle_type  handle
);

extern int16 OEMNet_McastLeaveEx
(
 OEMCONTEXT             netd,
 oem_mcast_handle_type  aHandles[],
 int                    nNumOfRequests
);

extern int16 OEMNet_McastRegisterEx
(
 OEMCONTEXT             netd,
 oem_mcast_handle_type  aHandles[],
 int                    nNumOfRequests
);

int32 OEMNet_GetMcastEventInfo
(
   OEMCONTEXT             netd,
   oem_mcast_handle_type *pHandle,
   AEEMcastEvent         *pEvent,
   AEEMcastInfoCode      *pInfoCode,
   boolean               *bIsDeprecatedInfoCode
);

extern int16 OEMNet_GetLastNetDownReason
(
   OEMCONTEXT        netd,
   AEENetDownReason* pnNetDownReason
);

extern int16 OEMNet_RequestQoS
(
   OEMCONTEXT           netd,
   oem_qos_spec_type   *pQoS,
   oem_qos_handle_type *pHandle
);

extern int16 OEMNet_RequestQoSBundle
(
   OEMCONTEXT           netd,
   oem_qos_spec_type   **ppQoSSpecs,
   oem_qos_handle_type *pHandles,
   uint32 uNumQosSpecs,
   AEEQoSBundleRequestOpcode opcode
);

extern int16 OEMNet_ModifyQoS
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle,
   oem_qos_spec_type   *pQoS
);

extern int16 OEMNet_ModifyPrimaryQoS
(
   OEMCONTEXT                   netd,
   oem_primary_qos_spec_type   *pQoS,
   void                        *pHandle 
);

extern int16 OEMNet_ReleaseQoS
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle
);

extern int16 OEMNet_ReleaseQoSBundle
(
   OEMCONTEXT          netd,
   oem_qos_handle_type* pHandles,
   uint32 uNumHandles
);

extern int16 OEMNet_GetQoSFlowSpec
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle,
   oem_ip_flow_type    *pRxFlow,
   oem_ip_flow_type    *pTxFlow
);

extern int16 OEMNet_GetGrantedQoSFlowSpecDuringAnyState
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle,
   oem_ip_flow_type    *pRxFlow,
   oem_ip_flow_type    *pTxFlow
);

extern int16 OEMNet_GetPrimaryQoSGrantedFlowSpec
(
   OEMCONTEXT           netd,
   oem_ip_flow_type    *pRxFlow,
   oem_ip_flow_type    *pTxFlow
);

extern AEEQoSStatus OEMNet_GetQoSStatus
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle
);

extern int16 OEMNet_QoSGoActive
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle
);

extern int16 OEMNet_QoSDeactivate
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle
);

extern int16 OEMNet_QoSBundleGoActive
(
   OEMCONTEXT          netd,
   oem_qos_handle_type* pHandles,
   uint32 uNumHandles
);

extern int16 OEMNet_QoSBundleDeactivate
(
   OEMCONTEXT          netd,
   oem_qos_handle_type* pHandles,
   uint32 uNumHandles
);

extern int16 OEMNet_SetQoSDormancyTimeout
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle,
   uint8                ucSecs
);

extern int32 OEMNet_GetQoSEventInfo(
   OEMCONTEXT           netd,
   oem_qos_handle_type *pHandle,
   AEEQoSEvent         *pEvent,
   AEEQoSInfoCode      *pInfoCode
);

extern int32 OEMNet_GetPrimaryQoSModifyEventInfo(
   OEMCONTEXT           netd,
   void                 **pHandle,
   AEEPrimaryQoSEvent   *pEvent
);

extern int16 OEMNet_IsQoSAware(
   OEMCONTEXT netd, 
   boolean *pbIsQoSAware
);

extern int16 OEMNet_GetSupportedQoSProfiles(
   OEMCONTEXT netd,
   uint16 awQoSProfiles[],
   uint8 uNumProfiles,
   uint8* puNumProfilesRequired
);

extern int16 OEMNet_GetSipServerAddr(
   OEMCONTEXT netd, 
   IPAddr aSipServerAddresses[], 
   uint8  uNumAddresses,
   uint8* puNumAddressesRequired
);

extern int16 OEMNet_GetSipServerDomainNames(
   OEMCONTEXT netd, 
   AEENetDomainName aSipServerDomainNames[], 
   uint8  uNumNames,
   uint8* puNumNamesRequired
);

extern int16 OEMNet_GetHystActTimer(
   OEMCONTEXT netd, 
   int *pnHystActTimer
);

extern int16 OEMNet_SetHystActTimer(
   OEMCONTEXT netd, 
   int nHystActTimer
);

extern int16 OEMNet_GetBearerTechnology(
   OEMCONTEXT netd, 
   AEEBearerTechType *pAEEBearerTechType
);

extern int16 OEMNet_GetBearerTechnologyOpts(
   OEMCONTEXT netd, 
   IBearerTechnology *pBearerTechOpts
);

extern int16 OEMNet_GenerateIPv6PrivateAddr(
   OEMCONTEXT  netd, 
   boolean     bIsUnique,
   void*       pHandle
);

extern int16 OEMNet_GetIPv6PrivAddrInfo(
   OEMCONTEXT           netd,
   AEEIPv6PrivAddrInfo* pPrivAddrInfo,
   const void*          pHandle
);

extern int16 OEMNet_DeletePrivAddrInfo(
   OEMCONTEXT  netd,
   void*       pHandle
);

extern int16 OEMNet_GetIPv6PrefixInfo(
   OEMCONTEXT           netd,
   INAddr6*             pAddr,
   AEEIPv6PrefixInfo*   pPrefixInfo
);

extern int16 OEMNet_GetOutageInfo(
   OEMCONTEXT        netd,
   AEEOutageInfo*    pOutageInfo
);

extern void OEMNet_CloseAllNets(void);

extern void OEMNet_CloseSiblingNets
(
   OEMCONTEXT netd
);

extern int16 OEMNet_GoNull
(
   OEMCONTEXT netd, 
   int nReason
);

extern int32 OEMNet_BcmcsDbUpdate
(
   IWebOpts* pBcmcsDbRecord,
   uint32    uUpdateMode
);

#if defined(AEE_SIMULATOR)
extern int16 OEMNet_SimulateQoSAwareUnawareEvent(
   OEMCONTEXT           netd
);

extern int16 OEMNet_ResetSimulatedQoSSystemType(
   OEMCONTEXT           netd
);

extern int16 OEMNet_SimulateBearerTechnologyChangeEvent(
   OEMCONTEXT           netd
);

extern int16 OEMNet_SimulateAsyncPrivIPv6AddrGeneration(
   OEMCONTEXT           netd
);
extern int16 OEMNet_SimulatePrivIPv6AddrFailureGeneration(
   OEMCONTEXT           netd
);

extern int16 OEMNet_SimulateIPv6PrefixUpdatedEvent(
   OEMCONTEXT           netd,
   INAddr6 *            pAddr
);

extern int16 OEMNet_SimulateIPv6PrefixDeprecatedEvent(
   OEMCONTEXT           netd,
   INAddr6 *            pAddr
);

extern int16 OEMNet_SimulateOutageEvent(
   OEMCONTEXT           netd
);

extern int16 OEMNet_SimulateQoSProfilesChangedEvent(
   OEMCONTEXT           netd
);
#endif // defined(AEE_SIMULATOR)

extern int16 OEMSocket_GetSockName
(
   OEMCONTEXT            sockd,         /* Socket descriptor */
   void                 *pAddr,
   int                  *pAddrLen
);

extern int16 OEMSocket_GetPeerName
(
   OEMCONTEXT            sockd,         /* Socket descriptor */
   void                 *pAddr,
   int                  *pAddrLen
);

extern OEMCONTEXT OEMSocket_Open
(
   OEMCONTEXT            netd,
   uint16                wFamily,
   AEESockType           type,          /* socket type */
   int                   protocol,      /* socket protocol */
   int16                *err
);

extern int16 OEMSocket_Connect
(
   OEMCONTEXT            sockd,         /* Socket descriptor */
   const void           *pAddr          /* destination address */
);

extern int16 OEMSocket_Bind
(
   OEMCONTEXT            sockd,         /* socket descriptor */
   const void           *pAddr          /* local address */
);

extern int16 OEMSocket_Close
(
   OEMCONTEXT            sockd          /* socket descriptor */
);

extern int32 OEMSocket_Write
(
   OEMCONTEXT            sockd,         /* socket descriptor */
   const byte           *buffer,        /* user buffer from which to copy data */
   uint32                nbytes,        /* number of bytes to be written to socket */
   int16                *err
);

extern int32 OEMSocket_Read
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  byte                 *buffer,        /* user buffer to which to copy data */
  uint32                nbytes,        /* number of bytes to be read from socket */
  int16                 *err
);

extern int32 OEMSocket_Writev
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  const SockIOBlock     iov[],         /* array of data buffers from which to copy data */
  uint16                iovcount,      /* number of array items */
  int16                 *err
);

extern int32 OEMSocket_Readv
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  SockIOBlock           iov[],         /* array of data buffers to copy data into */
  uint16                iovcount,      /* number of array items */
  int16                 *err
);

extern int32 OEMSocket_SendTo
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  const byte            *buffer,       /* user buffer from which to copy the data */
  uint32                nbytes,        /* number of bytes to be written */
  uint32                flags,         /* data transport options */
  const void           *pAddr,         /* destination address */
  int16                 *err
);

extern int32 OEMSocket_RecvFrom
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  byte                 *buffer,        /* user buffer from which to copy the data */
  uint32                nbytes,        /* number of bytes to be written */
  uint32                flags,         /* unused */
  void                 *pAddr,
  int                  *pAddrLen,
  int16                 *err
);

extern int16 OEMSocket_AsyncSelect
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int32                 mask           /* bitmask of events to set */
);

extern int32 OEMSocket_GetNextEvent
( 
  OEMCONTEXT           *sockd,        /* socket descriptor */
  int16                *err
);

extern int16 OEMSocket_Listen
(
  OEMCONTEXT            sockd,        /* socket descriptor */
  int16                 nMaxBacklog   /* maximum backlog of unaccepted sockets */
);

extern OEMCONTEXT OEMSocket_Accept
(
  OEMCONTEXT            sockd,         /* Listen socket descriptor */
  int16                *err
);

extern int16 OEMSocket_Shutdown
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int32                 how            /* how to shutdown (AEE_SHUTDOWN_*) */
);

extern int16 OEMSocket_GetNativeDescriptor
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  uint32               *pdwDesc
);

extern int16 OEMSocket_GetSDBAckInfo
(
  OEMCONTEXT sockd,                    /* socket descriptor */
  AEESDBAckInfo* pInfo
);

extern int16 OEMSocket_SetKeepAlive
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bKeepAlive
);

extern int16 OEMSocket_SetLinger
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bOn,           // linger on or off
  uint32                dwLinger       // linger time, in seconds
);

extern int16 OEMSocket_SetRcvBuf
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                   nRcvBuf
);

extern int16 OEMSocket_SetSndBuf
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                   nSndBuf
);

extern int16 OEMSocket_SetNoDelay
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bNoDelay
);

extern int16 OEMSocket_SetDelayedAck
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bDelayedAck
);

extern int16 OEMSocket_SetSACK
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bSACK
);

extern int16 OEMSocket_SetTimeStamp
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bTimeStamp
);

extern int16 OEMSocket_SetMembership
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bAdd,          // Add or Drop membership
  IPAddr               *pMCast         // multicast group to join/leave
);

int16 OEMSocket_SetIPToS
(  
   OEMCONTEXT sockd,                   /* socket descriptor */
   int nIPToS                          /* type of service   */
); 

extern int16 OEMSocket_SetIPv6TrafficClass
(
   OEMCONTEXT sockd,                   /* socket descriptor */
   int nIPv6TClass                     /* traffic class     */
); 

int16 OEMSocket_SetIPTTL
(  
   OEMCONTEXT sockd,                   /* socket descriptor */
   int nIPTTL                          /* Time To Live      */
); 

int16 OEMSocket_SetTCPMaxSegmentSize
(  
   OEMCONTEXT sockd,                   /* socket descriptor    */
   int nMaxSeg                         /* maximal segment size */
); 

extern int16 OEMSocket_SetReuseAddr
(
   OEMCONTEXT sockd,                   /* socket descriptor */
   boolean bReuseAddr
);

extern int16 OEMSocket_SetDisableFlowFwding
(
   OEMCONTEXT sockd,                   /* socket descriptor */
   boolean bDisableFlowFwding
); 

extern int16 OEMSocket_SetSDBAckCB
(
  OEMCONTEXT sockd,                    /* socket descriptor */
  AEECallback* pCB                     /* callback supplied by the user */
); 

extern int16 OEMSocket_GetKeepAlive
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbKeepAlive
);

extern int16 OEMSocket_GetLinger
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbOn,          // linger on or off
  uint32               *pdwLinger      // linger time, in seconds
);

extern int16 OEMSocket_GetRcvBuf
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                  *pnRcvBuf
);

extern int16 OEMSocket_GetSndBuf
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                  *pnSndBuf
);

extern int16 OEMSocket_GetNoDelay
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbNoDelay
);

extern int16 OEMSocket_GetDelayedAck
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbDelayedAck
);

extern int16 OEMSocket_GetSACK
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbSACK
);

extern int16 OEMSocket_GetTimeStamp
(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbTimeStamp
);

extern int16 OEMSocket_GetIPToS
(
   OEMCONTEXT sockd,                  /* socket descriptor */
   int*       pnIPToS                 /* type of service   */
);

extern int16 OEMSocket_GetIPv6TrafficClass
(
   OEMCONTEXT sockd,                /* socket descriptor */
   int*       pnIPv6TClass          /* traffic class     */
); 

extern int16 OEMSocket_GetIPTTL
(
   OEMCONTEXT sockd,                  /* socket descriptor */
   int*       pnIPTTL                 /* Time To Live      */
);

extern int16 OEMSocket_GetTCPMaxSegmentSize
(
   OEMCONTEXT sockd,                  /* socket descriptor    */
   int*       pnMaxSeg                /* maximal segment size */
 );

extern int16 OEMSocket_GetReuseAddr
(
   OEMCONTEXT sockd,                  /* socket descriptor */
   boolean*   bReuseAddr
);

extern int16 OEMSocket_GetDisableFlowFwding
(
   OEMCONTEXT sockd,                  /* socket descriptor*/
   boolean*   pbDisableFlowFwding
); 

extern int16 OEMSocket_GetSDBAckCB
(
  OEMCONTEXT         sockd,            /* socket descriptor */
  AEESDBAckCBType*   pAEESDBAckCBType  /* data associated with the socket option */
);

#if defined(__cplusplus)
}
#endif

/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
OEMNet Interface
=======================================================================

Function: OEMNet_Open()

Description:
   Opens up a network.  
  
Prototype:
   OEMCONTEXT OEMNet_Open
   (
      int          nNetwork,
      int16        nFamily,
      AEECallback *pcbNetWaiter,
      AEECallback *pcbSocketWaiter,
      AEECallback *pcbQoSWaiter,
      AEECallback *pcbMcastWaiter,
      AEECallback *pcbQoSAwareUnawareWaiter,
      AEECallback *pcbBearerTechnologyChangeWaiter,
      AEECallback *pcbIPv6PrivAddrWaiter,
      AEECallback *pcbPrimaryQoSModifyResultWaiter,
      AEECallback *pcbPrimaryQoSModifyWaiter,
      AEECallback *pcbOutageWaiter,
      AEECallback *pcbQoSProfilesChangedWaiter,
      AEECallback *pcbIPv6PrefixUpdateWaiter,
      int16       *err
   );
      
Parameters:
   int: network type (AEE_NETWORK_*)
   pcbNetWaiter: callback to be resumed when a network event occurs
   pcbSocketWaiter: callback to be resumed when a socket event occurs
   pcbQoSWaiter: callback to be resumed when a QoS event occurs
   pcbMcastWaiter: callback to be resumed when a multicast event occurs
   pcbQoSAwareUnawareWaiter: callback to be resumed when a QoS Aware/Unaware event occurs
   pcbBearerTechnologyChangeWaiter: callback to be resumed when a Bearer Technology Change event occurs
   pcbIPv6PrivAddrWaiter: callback to be resumed when an IPv6 Privacy Address event occurs
   pcbPrimaryQoSModifyResultWaiter: callback to be resumed when Primary QoS is modified
   pcbPrimaryQoSModifyWaiter: callback to be resumed when Primary QoS Modify 
                              Accepted/Rejected event occurs.
   pcbOutageWaiter: callback to be resumed when an Outage event occurs
   pcbQoSProfilesChangedWaiter: callback to be resumed when a Supported QoS Profiles Change event occurs
   pcbIPv6PrefixUpdateWaiter: callback to be resumed when an IPv6 Prefix Update event occurs
   err: error code (returned by operation) 

Return Value:
   On successful creation of a network, this function returns network 
   descriptor which is a signed value greater than or equal to 0.  On error, 
   returns AEE_NET_ERROR.  Error specifics are returned via the err parameter.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_Close()

Description:
   Closes the specified network.  All corresponding sockets must have
   been closed for the network, prior to closing.
  
Prototype:
   int16 OEMNet_Close( OEMCONTEXT netd );
      
Parameters:
   netd: network descriptor

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   A return value of AEE_NET_SUCCESS implies that the given network 
   descriptor (netd) is not valid anymore.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetSocketWaiter()

Description:
   Registers new socket waiter callback, which waits for socket events,
   while replacing the previous waiter.
  
Prototype:
   void OEMNet_SetSocketWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );
      
Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a socket event occurs

Return Value:
   None

Comments:
   The installed socket event handler (see OEMNet_InstallSocketEventHandler)
   is not affected by calling this function.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetNetWaiter()

Description:
   Registers a new network waiter, which waits for network events,
   while replacing the previous waiter.
  
Prototype:
   void OEMNet_SetNetWaiter
   (
      OEMCONTEXT   netd,
      AEECallback* pcbWaiter
   );
      
Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a network event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetMTPDWaiter()

Description:
   Registers a new MT packet data waiter, which waits for MT packet data
   events, while replacing the previous waiter.
  
Prototype:
   void OEMNet_SetMTPDWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );
      
Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a MT packet data event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetQoSWaiter()

Description:
   Registers a new QoS event waiter, which waits for QoS events,
   while replacing the previous waiter.
  
Prototype:
   void OEMNet_SetQoSWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );
      
Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a QoS event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetMcastWaiter()

Description:
   Registers a new multicast event waiter, which waits for multicast events,
   while replacing the previous waiter.

Prototype:
   void OEMNet_SetMcastWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a multicast event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetBearerTechnologyChangeWaiter()

Description:
   Registers a new Bearer Technology Change event waiter, which waits for Bearer Technology Change events,
   while replacing the previous waiter.

Prototype:
   void OEMNet_SetBearerTechnologyChangeWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a Bearer Technology Change event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetIPv6PrivAddrWaiter()

Description:
   Registers a new IPv6 Privacy Address event waiter, which waits for IPv6 Privacy Address events,
   replacing the previous waiter.

Prototype:
   void OEMNet_SetIPv6PrivAddrWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when an IPv6 Privacy Address event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetIPv6PrefixUpdateWaiter()

Description:
   Registers a new IPv6 Prefix Update event waiter, which waits for IPv6 Prefix Updates events,
   replacing the previous waiter.

Prototype:
   void OEMNet_SetIPv6PrefixUpdateWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when an IPv6 Prefix Update event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================

Function: OEMNet_SetPrimaryQoSModifyResultWaiter

Description:
   Registers a Primary QoS Modify Result event waiter.

Prototype:
   void OEMNet_SetPrimaryQoSModifyResultWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a Primary QoS Modify Result event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetPrimaryQoSModifyWaiter

Description:
   Registers a Primary QoS Modify event waiter, which waits for Modify Accepted
   and Modify Rejected events.

Prototype:
   void OEMNet_SetPrimaryQoSModifyWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a Primary QoS Modify event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetOutageWaiter

Description:
   Registers a new Outage event waiter, which waits for Outage events,
   while replacing the previous waiter.

Prototype:
   void OEMNet_SetOutageWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when an Outage event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_SetQoSProfilesChangedWaiter

Description:
   Registers a new Supported QoS Profiles Change event waiter,
   which waits for such events, while replacing the previous waiter.

Prototype:
   void OEMNet_SetQoSProfilesChangedWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );

Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a Supported QoS Profiles Change
              event occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMNet_RegisterToMTPD()

Description:
   Sets the MT packet data waiter and then registers to MT packet data events.
  
Prototype:
   int16 OEMNet_RegisterToMTPD
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );
      
Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a MT packet data event occurs

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMNet_DeRegisterFromMTPD()

=======================================================================

Function: OEMNet_DeRegisterFromMTPD()

Description:
   Clears the MT packet data waiter and then de-registers from MT packet data
   events.
  
Prototype:
   int16 OEMNet_DeRegisterFromMTPD
   (
      OEMCONTEXT   netd
   );
      
Parameters:
   netd: network descriptor

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMNet_RegisterToMTPD()

=======================================================================

Function: OEMNet_SetQoSAwareUnawareWaiter()

Description:
   Registers a new QoS Awaew/Unaware event waiter, which waits for QoS 
   Aware/Unaware events, while replacing the previous waiter.
  
Prototype:
   void OEMNet_SetQoSAwareUnawareWaiter
   (
      OEMCONTEXT   netd,
      AEECallback *pcbWaiter
   );
      
Parameters:
   netd: network descriptor
   pcbWaiter: callback to be resumed when a QoS Aware/Unaware event occurs.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None
   
=======================================================================

Function: OEMNet_InstallSocketEventHandler()

Description:
   Installs a socket event handler that will be called whenever a
   socket event occurs.  If the socket handler returns TRUE, the event
   is assumed to be consumed, and no further processing will be done on
   the event.  If the socket handler does not consume the event, it
   should return FALSE.

Prototype:
   PFNNOTIFY OEMNet_InstallSocketEventHandler(OEMCONTEXT netd,
                                              PFNNOTIFY pfnSocketEventHandler);
      
Parameters:
   netd: network descriptor
   pfnSocketEventHandler - pointer to new socket event handler, or NULL
                           to deinstall an existing handler

Return Value:
   The previous socket event handler (may be NULL).

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_PPPOpen()

Description:
   Starts up the network subsystem and establishes a network connection
   to the internet.  After the connection is established, the OEM should
   resume the net waiter callback to indicate to the libraries that
   the connection is ready for use.
  
Prototype:
   int16 OEMNet_PPPOpen( OEMCONTEXT netd );
      
Parameters:
   netd: network descriptor

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   Initiates call origination and PPP negotiation.

See Also:
   None

========================================================================

Function: OEMNet_PPPClose()

Description:
   Initiates termination to bring down any network connections started with
   OEMNET_PPPOpen.  

   This function is asynchronous, and should resume the net waiter callback
   upon completion of the close operation.

Prototype:
   int16 OEMNet_PPPClose( OEMCONTEXT netd );

Parameters:
   netd: network descriptor

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   Initiates termination of PPP and any lower layer connections.

See Also:
   None

===========================================================================

Function: OEMNet_NameServers()

Description:
   This function allows the caller to discover the configured name
   server addresses. The addresses may come from PPP setup or from phone
   configuration, or both. If it's both, PPP setup addresses are listed
   first, followed by the phone configured addresses. This has the effect of the 
   phone configured addresses being treated as overrides by AEEDNS, etc.

Prototype:
   int16 OEMNet_NameServers(OEMCONTEXT netd, IPAddr *ainaAddrs, int *pnNumAddrs);

Parameters:
   netd: network descriptor
   ainaAddrs:  [in/out] a caller-allocated array of IPAddrs, filled with the 
               answer to the question: "which name servers?". If NULL,
               pnNumAddrs is filled with the number of addresses available.
   pnNumAddrs: [in/out] caller sets this to array size of ainaAddrs. Set to the
               number of available/filled addresses (depending on whether 
               ainaAddrs is null or not).

Return Value:
   SUCCESS:  if the addresses are found, filled
   EBADPARM: if pnNumAddrs is null

Comments:  
   pnNumAddrs may be set to 0 if there are no servers configured in 
   NVRam and there is no PPP setup available.

   the addresses returned in ainaAddrs must be in network byte-order.

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_GetRLP3Cfg()

Description:
        This function allows the caller to discover the configured RLP 
settings, if the OEM's network layer is implemented using RLP.

Prototype:
   int16 OEMNet_GetRLP3Cfg(OEMCONTEXT netd, int16 nOptName,AEERLP3Cfg *prlp3)

Parameters:
   netd: network descriptor
   int16 nOptName: one of 3 values:
              INET_OPT_DEF_RLP3 : retrieve default RLP3 settings
              INET_OPT_CUR_RLP3 : retrieve current RLP3 settings
              INET_OPT_NEG_RLP3 : retrieve negotiated RLP3 settings

   AEERLP3Cfg prlp3: [out] filled with relevant settings

Return Value:
   SUCCESS: if the settings were retrieved
   EUNSUPPORTED: if RLP isn't employed or this API is otherwise unsupported

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_SetRLP3Cfg()

Description:
        This function allows the caller to modify the configured RLP 
     settings, if the OEM's network layer is implemented using RLP.

Prototype:
   int16 OEMNet_SetRLP3Cfg(OEMCONTEXT netd, int16 nOptName,AEERLP3Cfg *prlp3)

Parameters:
   netd: network descriptor
   int16 nOptName: one of 2 values:
              INET_OPT_DEF_RLP3 : set default RLP3 settings
              INET_OPT_CUR_RLP3 : set current RLP3 settings

   AEERLP3Cfg prlp3: the new settings

Return Value:
   SUCCESS: if the settings were configured
   EUNSUPPORTED: if RLP isn't employed or this API is otherwise unsupported

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_SetPPPAuth()

Description:
        This function allows the caller to modify the configured PPP
   authentication settings, if this is relevant to the network implementation.

Prototype:
   int16 OEMNet_SetPPPAuth(OEMCONTEXT netd, const char *cpszAuth)


Parameters:
   netd: network descriptor
   const char *cpszAuth: the new credentials, in the form of 2 concatenated, null terminated strings, e.g.: "userid@somewhere.com\000password\000"

Return Value:
   SUCCESS: if the credentials were set
   EUNSUPPORTED: if PPP authentication cannot be configured

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_GetPPPAuth()

Description:
        This function allows the caller to retrieve the configured PPP
   authentication settings, if this is relevant to the network implementation.

Prototype:
   int16 OEMNet_GetPPPAuth(OEMCONTEXT netd, char *pszAuth, int *pnLen)


Parameters:
   netd: network descriptor
   char *pszAuth: [out] the buffer into which the credentials are to 
                  be copied, in the form of 2 concatenated, null 
                  terminated strings, e.g.: "userid@domain.com\000password\000"
   int *pnLen: [in/out] the size of pszAuth.  If pszAuth is NULL, pnLen is 
                ignored on input, and set to the number of bytes required to 
                hold authentication information on output.

Return Value:
   SUCCESS: if the credentials were retrieved
   EUNSUPPORTED: if PPP authentication cannot be configured

Comments:  
   None

Side Effects: 
   None

See Also:
   None

===========================================================================

Function: OEMNet_SetDDTMPref()

Description:
        This function allows the caller to modify the data dedicated
        transmission mode preference, if this is relevant to the
        network implementation.

Prototype:
   int16 OEMNet_SetDDTMPref(OEMCONTEXT netd, boolean bOn)


Parameters:
   netd: network descriptor
   boolean bOn: TRUE to turn DDTM on, FALSE otherwise

Return Value:
   SUCCESS: if the DDTM preference was set
   EUNSUPPORTED: if DDTM preference cannot be configured

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_PPPState()

Description:
   Returns a value indicating the status of the PPP connection.

   This function is synchronous, and should not callback any notification
   function.

Prototype:
   NetState OEMNet_PPPState
   (
      OEMCONTEXT netd
   );

Parameters:
   netd: network descriptor

Return Value:
   Returns an enumerated value of type NetState describing the network layer's
   current status.  These are:
      NET_PPP_OPENING - The network layer is being opened.  The AEE libraries
                        will defer work until NET_PPP_OPEN is returned.

      NET_PPP_OPEN - The network layer is open, and can make new sockets,
                     service existing sockets, or both.

      NET_PPP_CLOSING - The network layer is in the process of closing.
                        The AEE libraries will not try to create new sockets,
                        service existing sockets, or open the net library
                        while in this state.

      NET_PPP_CLOSED -  The network layer is closed. The AEE libraries will
                        not try to create or service sockets in this state,
                        but may open the library.
                        An OEM should close all open TCP sockets if the network 
                        closes.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_MyIPAddr()

Description:
   Returns the IP address of the active session.  NOTE: The IP address is
   in network byte order.

   This function is synchronous, and therefore should not callback any 
   notification function.
  
Prototype:
   int16 OEMSocket_MyIPAddr
   (
     OEMCONTEXT netd,
     IPAddr    *addr
   );

Parameters:
   netd: network descriptor
   addr: pointer to location where IP will be placed

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetUrgent()

Description:
   Determines whether urgent sendto option is supported and the payload
   limit, if any.

   This function is synchronous, and therefore should not callback any 
   notification function.
  
Prototype:
   int16 OEMNet_GetUrgent
   (
     OEMCONTEXT    netd,
     AEEUDPUrgent *pUrgent
   );

Parameters:
   netd: network descriptor
   pUrgent: pointer to AEEUDPUrgent struct

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_GetNativeDescriptor()

Description:
   Returns the native network descriptor from the underlying implementation.

Prototype:
   int16 OEMNet_NativeDescriptor
   (
      OEMCONTEXT netd,
      uint32    *pdwDesc
   );

Parameters:
   netd: network descriptor
   pdwDesc: pointer to dword, which will be filled in with the native descriptor

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_GetDefaultNetwork()

Description:
   Returns the default network (AEE_NETWORK_*)

Prototype:
   int16 OEMNet_GetDefaultNetwork
   (
      int16  sFamily,
      int    *pnDefault
   );

Parameters:
   sFamily:   Address family for which the default address is
              requested.
   pnNetwork: pointer to int, which will be filled in with the
                    default network (AEE_NETWORK_*)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_GetAppProfileId()

Description:
   Returns the profile id for given application type. 

Prototype:
   int16 OEMNet_GetAppProfileId
   (
      uint32 uAppType, 
      int16 *pnProfile
   );

Parameters:
   uAppType:  Application Type for which the profile is asked.
   pnProfile: pointer to int16, which will be filled in with the
              profile id for application type.

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMNet_GetUMTSCount()

Description:
   Returns the number of PDP contexts.

Prototype:
   int16 OEMNet_GetUMTSCount
   (
      uint16 *pwCount
   );

Parameters:
   pwCount: pointer to word, which will be filled in with the
                   number of PDP contexts

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
Function: OEMNet_GetUMTSInfo()

Description:
   Returns the requested PDP information.

Prototype:
   int16 OEMNet_GetUMTSInfo
   (
      uint16       wNumber,
      AEEUMTSInfo *pInfo
   );

Parameters:
   wNumber: the PDP of interest
   pInfo  : filled in with profile info

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetHWAddr()

Description:
   Obtains the hardware address.  The availability and format of
   a hardware address is very device specific.  For example, on
   a WLAN network, the hardware address is a six-byte MAC
   address.  On a CDMA network, there is no hardware address.

   This function is synchronous, and therefore should not callback any 
   notification function.
  
Prototype:
   int16 OEMNet_GetHWAddr
   (
     OEMCONTEXT netd,
     byte       addr[],
     int*       pnSize
   );

Parameters:
   netd: network descriptor
   addr: byte array to - where to place hw address
   pnSize: [in/out] - size of input array / size of hw address

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
Function: OEMNet_SetDormancyTimeout()

Description:
  Set the packet data dormancy timeout value (idle timeout after which mobile
  should enter dormancy).
  May be unsupported by certain networks.

Prototype:
   int16 OEMNet_SetDormancyTimeout
   (
      OEMCONTEXT netd,
      uint8      ucSecs
   );

Parameters:
   netd:   network descriptor
   ucSecs: timeout value in seconds

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

   If the network doesn't support this operation, EUNSUPPORTED is returned.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetLastNetDownReason()

Description:
   This function returns the network down reason for the 
   last network interface that the application was bound to.
  
Prototype:
   int16 OEMNet_GetLastNetDownReason
   (
      OEMCONTEXT        netd,
      AEENetDownReason* pnNetDownReason
   );

Parameters:
   netd: network descriptor
   pnNetDownReason: [out] - pointer to the last network down reason. 
   On AEE_NET_SUCCESS will be filled with the reason value (pre-allocated by caller).

Return Value:
   AEE_NET_SUCCESS: Success. pnNetDownReason filled with last network down reason.  
   AEE_NET_EFAULT: Lower layer returned an unexpected network down reason.
   
   Other AEE designated error codes might be returned.

Comments:
   None

Side Effects:
   None

See Also:
   AEENetDownReason

========================================================================
Function: OEMNet_RequestQoS()

Description:
   Requests Quality of Service

Prototype:
   int16 OEMNet_RequestQoS
   (
      OEMCONTEXT           netd,
      oem_qos_spec_type   *pQoS,
      oem_qos_handle_type *pHandle
   );

Parameters:
   netd: network descriptor
   pQoS: pointer to a valid input and output QoS specification.
         the QoS specification will be read and its error mask fields
         will be updated as needed to indicate erroneous parameters.
         (pre-allocated by caller)
   pHandle: filled in with a new QoS handle that identifies the QoS 
            instance that handles the QoS request
            (pre-allocated by caller)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
Function: OEMNet_ModifyQoS()

Description:
   Modifies Quality of Service

Prototype:
   int16 OEMNet_ModifyQoS
   (
      OEMCONTEXT           netd,
      oem_qos_handle_type  handle,
      oem_qos_spec_type   *pQoS
   );

Parameters:
   netd: network descriptor
   handle: handle to the granted QoS instance
   pQoS:  pointer to a valid input and output QoS specification.
          the QoS specification will be read and its error mask fields
          will be updated as needed to indicate erroneous parameters.
          (pre-allocated by caller)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
Function: OEMNet_ModifyPrimaryQoS()

Description:
   Modifies Primary QoS

Prototype:
   int16 OEMNet_ModifyQoS
   (
      OEMCONTEXT                   netd,
      oem_primary_qos_spec_type   *pQoS,
      void                        *handle 
   );

Parameters:
   netd:    network descriptor
   pQoS:    pointer to a valid Primary QoS specification.
            the Primary QoS specification will be read and its error mask will
            be updated if needed in order to indicate erroneous parameters.
            (pre-allocated by caller)
   handle:  handle to the Primary QoS instance

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================
Function: OEMNet_RequestQoSBundle()

Description:
   Requests Quality of Service for multiple QoS Sessions

Prototype:
   int16 OEMNet_RequestQoSBundle
   (
      OEMCONTEXT           netd,
      oem_qos_spec_type   **ppQoSSpecs,
      oem_qos_handle_type *pHandles,
      uint32 uNumQosSpecs,
      AEEQoSBundleRequestOpcode opcode
   );

Parameters:
   netd:         network descriptor
   ppQoSSpecs:   pointer to an array of valid input and output QoS 
                 specifications. The QoS specifications will be read and their 
                 error mask fields will be updated as needed to indicate 
                 erroneous parameters. (pre-allocated by caller)
   pHandles:     filled in with new QoS handles that identify the requested 
                 QoS instances (pre-allocated by caller)
   uNumQosSpecs: number of requested QoS specifications. uNumQosSpecs must not
                 be larger than the number of allocated handles for pHandles.
   opcode:       whether the requested QoS sessions are to be granted immediately
                 or stored and granted at a later time when required.

Return Value:
   AEE_NET_SUCCESS:    The request was successfully issued.
   EBADPARM:           Invalid parameters.
   AEE_NET_EBADOPTLEN: Number of QoSs exceeded MAX_UINT8
   AEE_NET_ENOMEM:     No memory.
   AEE_NET_EBADOPTVAL: Invalid option value for QoS Spec.
   AEE_NET_EBADOPTLEN: Invalid option length for QoS Spec.
   AEE_NET_EOPNOTSUPP: The operation is not supported.
   
   Other AEE designated error codes might be returned.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_ReleaseQoS()

Description:
   Releases a granted Quality of Service, identified by its handle value

Prototype:
   int16 OEMNet_ReleaseQoS
   (
      OEMCONTEXT           netd,
      oem_qos_handle_type  handle
   );

Parameters:
   netd: network descriptor
   handle: handle to the granted QoS instance
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_ReleaseQoSBundle()

Description:
   Releases multiple QoS sessions, identified by their handle values.

Prototype:
   extern int16 OEMNet_ReleaseQoSBundle
   (
      OEMCONTEXT          netd,
      oem_qos_handle_type* pHandles,
      uint32 uNumHandles
   );


Parameters:
   netd:        network descriptor
   pHandles:    handles to the QoS sessions to be released
   uNumHandles: number of handles

Return Value:
   AEE_NET_SUCCESS:    The request was successfully issued.
   AEE_NET_EBADOPTLEN: Number of handles exceeded MAX_UINT8
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.
  
Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetQoSFlowSpec()

Description:
   Retrieve the flow specification of a granted Quality of Service instance,
   identified by its handle value 

Prototype:
   int16 OEMNet_GetQoSFlowSpec
   (
      OEMCONTEXT           netd,
      oem_qos_handle_type  handle,
      oem_ip_flow_type    *pRxFlow,
      oem_ip_flow_type    *pTxFlow
   )

Parameters:
   netd: network descriptor
   handle: handle to the granted QoS instance
   pRxFlow: pointer to the QoS Flow in the Rx direction, which will be
            filled with the QoS' Rx flow spec (pre-allocated by caller)
   pTxFlow: pointer to the QoS Flow in the Tx direction, which will be
            filled with the QoS' Tx flow spec (pre-allocated by caller)
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetGrantedQoSFlowSpecDuringAnyState()

Description:
   Retrieve the flow specification of a granted Quality of Service instance,
   identified by its handle value. This API is supported even when the QoS
   is suspended.

Prototype:
   int16 OEMNet_GetGrantedQoSFlowSpecDuringAnyState
   (
      OEMCONTEXT           netd,
      oem_qos_handle_type  handle,
      oem_ip_flow_type    *pRxFlow,
      oem_ip_flow_type    *pTxFlow,
   );

Parameters:
   netd:          network descriptor
   handle:        handle to the granted QoS instance
   pRxFlow:       pointer to the QoS Flow in the Rx direction, which will be
                  filled with the QoS' Rx flow spec (pre-allocated by caller)
   pTxFlow:       pointer to the QoS Flow in the Tx direction, which will be
                  filled with the QoS' Tx flow spec (pre-allocated by caller)
 
Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetPrimaryQoSGrantedFlowSpec()

Description:
   Retrieve the flow specification of a Primary QoS instance.

Prototype:
   int16 OEMNet_GetPrimaryQoSGrantedFlowSpec
   (
      OEMCONTEXT           netd,
      oem_ip_flow_type    *pRxFlow,
      oem_ip_flow_type    *pTxFlow
   );

Parameters:
   netd: network descriptor
   pRxFlow: pointer to the Primary QoS Flow in the Rx direction, which will be
            filled with the Primary QoS' Rx flow spec (pre-allocated by caller)
   pTxFlow: pointer to the Primary QoS Flow in the Tx direction, which will be
            filled with the QoS' Tx flow spec (pre-allocated by caller)
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetQoSStatus()

Description:
   Returns the status of a QoS instance

Prototype:
   uint16 OEMNet_GetQoSStatus
   (
      OEMCONTEXT          netd,
      oem_qos_handle_type handle
   );

Parameters:
   netd: network descriptor
   handle: handle to the QoS instance
 
Return Value:
   Returns an AEEQoSStatus value (see AEEQoSSession.h)

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_QoSGoActive()

Description:
   Tries to make a QoS instance go active

Prototype:
   int16 OEMNet_QoSGoActive
   (
      OEMCONTEXT          netd,
      oem_qos_handle_type handle
   );

Parameters:
   netd: network descriptor
   handle: handle to the QoS instance
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_QoSBundleGoActive()

Description:
   Tries to activate multiple QoS instances

Prototype:
   extern int16 OEMNet_QoSBundleGoActive
   (
      OEMCONTEXT          netd,
      oem_qos_handle_type* pHandles,
      uint32 uNumHandles
   );

Parameters:
   netd:     network descriptor
   pHandles: handles to the QoS instances to be activated
   uNumHandles:  number of handles.

Return Value:
   AEE_NET_SUCCESS:    The request was successfully issued.
   EBADPARM:           Invalid parameter.
   AEE_NET_EBADOPTLEN: Number of handles exceeded MAX_UINT8
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.
  
Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_QoSDeactivate()

Description:
   Tries to suspend a QoS Session

Prototype:
   extern int16 OEMNet_QoSDeactivate
   (
      OEMCONTEXT  netd,
      oem_qos_handle_type  handle
   );

Parameters:
   netd:        network descriptor
   handle:      handle to the QoS Session instances to be suspended

Return Value:
   AEE_NET_SUCCESS:    The request was successfully issued.
   AEE_NET_EINVAL:     Invalid QOS handle.
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.

Comments:
   None

Side Effects:
   None

See Also:
   None
  
========================================================================

Function: OEMNet_QoSBundleDeactivate()

Description:
   Tries to activate multiple QoS instances

Prototype:
   extern int16 OEMNet_QoSBundleDeactivate
   (
      OEMCONTEXT          netd,
      oem_qos_handle_type* pHandles,
      uint32 uNumHandles
   );

Parameters:
   netd:        network descriptor
   pHandles:    handles to the QoS instances to be suspended
   uNumHandles: number of handles.

Return Value:
   AEE_NET_SUCCESS:    The request was successfully issued.
   EBADPARM:           Invalid parameter.
   AEE_NET_EBADOPTLEN: Number of handles exceeded MAX_UINT8
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_SetQoSDormancyTimeout()

Description:
   Set a QoS inactivity timeout value (idle timeout after which the QoS
   should enter dormancy)

Prototype:
   int16 OEMNet_SetQoSDormancyTimeout
   (
      OEMCONTEXT          netd,
      oem_qos_handle_type handle,
      uint8               ucSecs
   );

Parameters:
   netd: network descriptor
   handle: handle to the QoS instance
   ucSecs: timeout value in seconds
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetQoSEventInfo()

Description:
   Retrieve QoS event information.

Prototype:
   int32 OEMNet_GetQoSEventInfo(
      OEMCONTEXT           netd,
      oem_qos_handle_type *pHandle,
      AEEQoSEvent         *pEvent,
      uint32              *pInfoCode
   );

Parameters:
   netd: network descriptor
   pHandle: filled in with the QoS event's handle identification 
            (pre-allocated by caller)
   pEvent: filled in with the QoS event that occurred
           (pre-allocated by caller)
   pInfoCode: filled in with the QoS event's information code
              (pre-allocated by caller)
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   Should be called after AEE receives a notification that QoS events  
   have occurred, in order to retrieve the data of these QoS events.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetPrimaryQoSModifyEventInfo()

Description:
   Retrieve Primary QoS event information.

Prototype:
   int32 OEMNet_GetPrimaryQoSModifyEventInfo
   (
      OEMCONTEXT           netd,
      void                 **pHandle,
      AEEPrimaryQoSEvent   *pEvent
   );

Parameters:
   netd: network descriptor
   pHandle: filled in with the Primary QoS instance handle 
            (pre-allocated by caller)
   pEvent:  filled in with the Primary QoS event that occurred
            (pre-allocated by caller)
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   Should be called after AEE receives a notification that Primary QoS events
   have occurred, in order to retrieve the data of these Primary QoS events.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_CloseAllNets()

Description:
   Initiates asynchronous closing of all existing network connections.

Prototype:
   void OEMNet_CloseAllNets(void);

Parameters:
   None.
  
Return Value:
   None.

Comments:
   None.

Side Effects:
   Initiates termination of all existing network connections and any lower 
   layer connections.

See Also:
   OEMNet_CloseSiblingNets()

========================================================================

Function: OEMNet_CloseSiblingNets()

Description:
   Initiates asynchronous closing of all existing network connections which
   uses the same network interface as netd.

Prototype:
   void OEMNet_CloseSiblingNets
   (
      OEMCONTEXT netd
   );

Parameters:
   netd: network descriptor
 
Return Value:
   None.

Comments:
   None.

Side Effects:
   Initiates termination of all relevant network connections and any lower 
   layer connections.

See Also:
   OEMNet_CloseAllNets()

========================================================================

Function: OEMNet_McastJoin()

Description:
   Requests a multicast session

Prototype:
   int16 OEMNet_McastJoin
   (
      OEMCONTEXT             netd,
      AEESockAddrStorage    *psaGroupAddr,
      oem_mcast_handle_type *pHandle
   );

Parameters:
   netd: network descriptor
   psaGroupAddr:  pointer to an address of multicast group to join
   pHandle: filled in with a handle that identifies the multicast flow 
            instance that handles the multicast join request
            (pre-allocated by caller)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_McastJoinEx()

Description:
   Requests a bundle of multicast sessions

Prototype:
   extern int16 OEMNet_McastJoinEx
   (
      OEMCONTEXT            netd,
      AEESockAddrStorage*   apsaGroupAddresses[],
      oem_mcast_handle_type aHandles[],
      boolean               aSendReg[],
      int                   nNumOfRequests
   );

Parameters:
   netd: network descriptor
   apsaGroupAddr:  array of pointers to addresses of multicast groups to join
   aHandles: filled in with handles that identify the multicast flows 
             instances that handle the multicast join requests
             (pre-allocated by caller)
   aSendReg: array of booleans to indicate for each session if we want it to 
             be registered now or not
   nNumOfRequests: number of sessions

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_McastLeave()

Description:
   Leaves a multicast session, identified by its handle value

Prototype:
   int16 OEMNet_McastLeave
   (
      OEMCONTEXT           netd,
      oem_mcast_handle_type  handle
   );

Parameters:
   netd: network descriptor
   handle: handle to the OEM multicast session
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_McastLeaveEx()

Description:
   Leaves a bundle of multicast sessions, identified by their handle values

Prototype:
   int16 OEMNet_McastLeaveEx
   (
      OEMCONTEXT             netd,
      oem_mcast_handle_type *aHandles,
      int                    nNumOfRequests
   );

Parameters:
   netd: network descriptor
   aHandles: array of handles to the OEM multicast sessions
   nNumOfRequests: number of sessions

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_McastRegisterEx()

Description:
   Registers a bundle of multicast sessions, identified by their handle values

Prototype:
   int16 OEMNet_McastRegisterEx
   (
      OEMCONTEXT             netd,
      oem_mcast_handle_type *aHandles,
      int                    nNumOfRequests
   );

Parameters:
   netd: network descriptor
   aHandles: array of handles to the OEM multicast sessions
   nNumOfRequests: number of sessions

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetMcastEventInfo()

Description:
   Retrieve multicast event information.

Prototype:
   int32 OEMNet_GetMcastEventInfo
   (
      OEMCONTEXT             netd,
      oem_mcast_handle_type *pHandle,
      AEEMcastEvent         *pEvent,
      AEEMcastInfoCode      *pInfoCode,
      boolean               *bIsDeprecatedInfoCode
   );

Parameters:
   netd: network descriptor
   pHandle: filled in with the multicast event's handle identification 
            (pre-allocated by caller)
   pEvent: filled in with the multicast event that occurred
           (pre-allocated by caller)
   pInfoCode: filled in with the multicast event's information code
              (pre-allocated by caller)
   pbIsDeprecatedInfoCode: filled in with TRUE if the event's info code is a deprecated one,
                           and with FALSE otherwise
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   Should be called after AEE receives a notification that multicast events  
   have occurred, in order to retrieve the data of these multicast events.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_BcmcsDbUpdate()

Description:
   Update the BCMCS DB of flow mapping information, as requested 
   through IBCMCSDB_Update. See IBCMCSDB_Update() for more details

Prototype:
   int32 OEMNet_BcmcsDbUpdate
   (
      IWebOpts* pBcmcsDbRecord,
      uint32    uUpdateMode
   );

Parameters:
   pBcmcsDbRecord: The record used for updating the BCMCS DB.
   uUpdateMode: Mode of update.
 
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_IsQoSAware()

Description:
   This function is supported only for CDMA network.
   It queries whether the mobile is currently on a system that supports 
   QoS (Aware System) or on a system which does not support QoS (Unaware system).

Prototype:
   int16 OEMNet_IsQoSAware(
      OEMCONTEXT netd, 
      boolean *pbIsQoSAware
   );

Parameters:
   netd: network descriptor
   pbIsQoSAware :[out] Upon SUCCESS return, this parameter will be set to TRUE 
   if the mobile is on a QoS enabled system, and it will be set
   to FALSE otherwise.   

Return Value:
   AEE_NET_SUCCESS: Success. The mobile is on a CDMA network. 
   pbIsQoSAware indicates whether the mobile is on a QoS Aware (TRUE) or 
   QoS Unaware (FALSE) system.

   AEE_NET_EOPNOTSUPP: The mobile is on a UMTS network. 
   pbIsQoSAware is invalid.
  
   AEE_NET_ENETNONET: Network subsystem unavailable.
   
   Other AEE designated error codes might be returned.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetSupportedQoSProfiles()

Description:
   This function returns the current QoS Profiles supported by the network. 

Prototype:
   int16 OEMNet_GetSupportedQoSProfiles(
      OEMCONTEXT netd,
      uint16 awQoSProfiles[],
      uint8 uNumProfiles,
      uint8* puNumProfilesRequired
   );
Parameters:
   netd: network descriptor
   awQoSProfiles: [out] - Caller allocated array in which the supported 
                  QoS Profile IDs are returned
   uNumProfiles:  [in] - The number of entries in awQoSProfiles (number of QoS 
                  Profile IDs it can hold) 
   puNumProfilesRequired: [out] - The number of entries required to receive all 
                  the QoS Profile IDs. If set to NULL, this value will not be filled.


   
Return Value:
   AEE_NET_SUCCESS: Success. puNumProfilesRequired holds the number of 
   QoS Profiles currently supported by the network. awQoSProfiles contains the 
   QoS Profiles information. If its size does not fit the number of supported 
   profiles only the first n profile IDs that can fit into the provided size 
   are returned. The caller may call the API again with an appropriately larger 
   awQoSProfiles allocated array to get all supported Qos profile IDs

   AEE_NET_EOPNOTSUPP: The operation is not supported.
  
   AEE_NET_ENETNONET: Network subsystem unavailable.
    
   AEE_NET_EQOSUNAWARE: The current network is QoS Unaware System 
   (does not support QoS)
      
   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None

See Also:
   None

========================================================================  

Function: OEMNet_GetSipServerAddr()

Description:
   This function returns the SIP (Session Initiation Protocol) server IP
   addresses for this network. 

Prototype:
   int16 OEMNet_GetSipServerAddr(
      OEMCONTEXT netd, 
      IPAddr aSipServerAddresses[], 
      uint8  uNumAddresses,
      uint8* puNumAddressesRequired
   );

Parameters:
   netd:                   network descriptor
   aSipServerAddresses:    [out] - Caller allocated array in which the SIP server
                           addresses are returned
   uNumAddresses:          [in] - The size of aSipServerAddresses (number of IPAddr 
                           items it can hold) 
   puNumAddressesRequired: [out] - The size (number of entries) of aSipServerAddresses
                           required to receive all SIP server addresses.
                           If set to NULL, this value will not be filled.            
   
Return Value:
   AEE_NET_SUCCESS: Success. aSipServerAddresses holds SIP server IP addresses.
   puNumAddressesRequired holds the total number of existing SIP server addresses.
   If aSipServerAddresses is too small to contain all existing SIP server addresses
   it is filled with the maximum number of addresses possible.
   
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None

See Also:
   None

========================================================================
  
Function: OEMNet_GetSipServerDomainNames()

Description:
   This function returns the SIP (Session Initiation Protocol) server 
   Domain Names for this network. 

Prototype:
   int16 OEMNet_GetSipServerDomainNames(
      OEMCONTEXT netd, 
      AEENetDomainName aSipServerDomainNames[], 
      uint8  uNumNames,
      uint8* puNumNamesRequired
   );

Parameters:
   netd:                   network descriptor
   aSipServerDomainNames:  [out] - Caller allocated array in which the SIP server
                           domain names are returned.
   uNumNames:              [in] - The size of aSipServerDomainNames (number of  
                           AEENetDomainName items it can hold).
   puNumNamesRequired:     [out] - The size (number of entries) of aSipServerDomainNames
                           required to receive all SIP server Domain Names.
                           If set to NULL, this value will not be filled.            
   
Return Value:
   AEE_NET_SUCCESS: Success. aSipServerDomainNames holds SIP server Domain Names.
   puNumNamesRequired holds the total number of existing SIP server Domain Names.
   If aSipServerDomainNames is too small to contain all existing SIP server Domain Names
   it is filled with the maximum number of Domain Names possible.
   
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetHystActTimer()

Description:
   This function returns the Hysteresis Activation Timer for this network. 

Prototype:
   int16 OEMNet_GetHystActTimer(
      OEMCONTEXT netd, 
      int *pnHystActTimer
   );

Parameters:
   netd:            network descriptor
   pnHystActTimer:  [out] - Pointer to integer in which the Hysteresis Activation 
                    Timer value is returned.

Return Value:
   AEE_NET_SUCCESS: Success. pnHystActTimer holds the Hysteresis Activation Timer value.

   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_SetHystActTimer()

Description:
   This function sets the Hysteresis Activation Timer for this network. 

Prototype:
   int16 OEMNet_GetHystActTimer(
      OEMCONTEXT netd, 
      int nHystActTimer
   );

Parameters:
   netd:           network descriptor
   nHystActTimer:  the Hysteresis Activation Timer value.

Return Value:
   AEE_NET_SUCCESS: Success.

   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetBearerTechnology()

Description:
   This function returns the bearer technology type 
   for this network. 

Prototype:
   int16 OEMNet_GetBearerTechnology(
      OEMCONTEXT netd, 
      AEEBearerTechType *pAEEBearerTechType
   );

Parameters:
   netd:                      network descriptor
   pAEEBearerTechType:        [out] - Caller allocated struct in which the bearer technology
                              information is returned.

Return Value:
   AEE_NET_SUCCESS: Success. pAEEBearerTechType holds the bearer technology type.
   
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GetBearerTechnologyOpts()

Description:
   This function returns the bearer technology options
   for this network. 

Prototype:
   int16 OEMNet_GetBearerTechnologyOpts(
      OEMCONTEXT netd, 
      IBearerTechnology *pBearerTechOpts
   );

Parameters:
   netd:                   network descriptor
   pBearerTechOpts:        [out] - Caller allocated IBearerTechnology
                           instance pointer in which the bearer technology
                           options are returned.

Return Value:
   AEE_NET_SUCCESS: Success. pBearerTechOpts holds the bearer technology options.
   
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMNet_GenerateIPv6PrivateAddr()

Description:
   This function generates an IPv6 private address, and registers
   OEMNet_IPv6PrivAddrEvtCB as a callback for receiving events regarding
   the generated address. The address may be generated synchronously, 
   or the user will have to wait for an event indicating that the address
   was generated.

Prototype:
   int16 OEMNet_GenerateIPv6PrivateAddr(OEMCONTEXT netd, 
                                        boolean    bIsUnique,
                                        void*      pHandle
                                       );

Parameters:
   netd:       network descriptor
   bIsUnique:  is the requested address unique or shared
   pHandle:    pointer to request handle.

Return Value:
   AEE_NET_SUCCESS: Success. The address was generated.
                    pOEMPrivAddrGenParams holds the address.

   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   AEE_NET_EWOULDBLOCK: Address will be generated asynchronously.

   Other AEE designated error codes might be returned.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMNet_IPv6PrivAddrEvtCB()
   OEMNet_GetIPv6PrivAddrInfo()
   OEMNet_DeletePrivAddrInfo()

========================================================================

Function: OEMNet_IPv6PrivAddrEvtCB()

Description:
   This is a callback for receiving IPv6 Privacy Address events.
   It is being registered by OEMNet_GenerateIPv6PrivateAddr(), when the
   user requests to generate a new private IPv6 address.
   This function copies the relevant data it into the information
   record corresponding this request. It then calls OEMNet_ProcessIPv6PrivAddrEvent().

Prototype:
   void OEMNet_IPv6PrivAddrEvtCB(dss_iface_ioctl_event_enum_type        event,
                                 dss_iface_ioctl_event_info_union_type  event_info,
                                 void                                   *user_data,
                                 sint15                                 dss_nethandle,
                                 dss_iface_id_type                      iface_id
                                );

Parameters:
   event:               the event type. Currently, the supported
                        types are: DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV,
                                   DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV,
                                   DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV
   event_info:          holds a struct of the type dss_iface_ioctl_priv_addr_info_type.  
   user_data:           data supplied by the user in the call to OEMNet_GenerateIPv6PrivateAddr.
                        This data is used by the OEM to find the corresponding Address-information record.
   dss_nethandle:       handle to the OEMNet object.
   iface_id:            interface ID.

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMNet_GenerateIPv6PrivateAddr()
   OEMNet_GetIPv6PrivAddrInfo()
   OEMNet_DeletePrivAddrInfo()

========================================================================

Function: OEMNet_GetIPv6PrivAddrInfo()

Description:
   Retrieve IPv6 Privacy Address information.

Prototype:
   int16 OEMNet_GetIPv6PrivAddrInfo(
      OEMCONTEXT            netd,
      AEEIPv6PrivAddrInfo*  pPrivAddrInfo,
      void*                 pHandle
   );

Parameters:
   netd:          network descriptor
   pPrivAddrInfo: filled in with the IPv6 private address's information
                  (pre-allocated by caller)
   pHandle:       filled in with the IPv6 Privacy Address's handle identification 
                  (pre-allocated by caller)
 
Return Value:
   On success, returns AEE_NET_SUCCESS.
   Returns AEE_NET_ERROR if the info record was not found.
   Returns EBADPARM if one of the parameters is not valid.

Comments:
   Should be called in order to retrieve the information regarding the 
   private address.

Side Effects:
   None

See Also:
   AEEIPv6PrivAddrInfo
   OEMNet_GenerateIPv6PrivateAddr()
   OEMNet_DeletePrivAddrInfo()

========================================================================

Function: OEMNet_DeletePrivAddrInfo()

Description:
   Delete IPv6 private address information.

Prototype:
   extern int16 OEMNet_DeletePrivAddrInfo(
      OEMCONTEXT netd,
      void* pHandle
   );

Parameters:
   netd: network descriptor
   pHandle: Handle to the information record to be deleted.
   
Return Value:
   On success, returns AEE_NET_SUCCESS. If the info record corresponding 
   to the handle is not found - returns AEE_NET_ERROR.

Comments:
   Should be called when an INetwork object is being released, to release its
   corresponding private address information.

Side Effects:
   None

See Also:
   AEEIPv6PrivAddrInfo
   OEMNet_GenerateIPv6PrivateAddr()
   OEMNet_GetIPv6PrivAddrInfo()

========================================================================

Function: OEMNet_GetIPv6PrefixInfo()

Description:
   Retrieve the information regarding an IPv6 Prefix
   corresponding to a given address.

Prototype:
   extern int16 OEMNet_GetIPv6PrefixInfo(
      OEMCONTEXT          netd,
      INAddr6*            pAddr,
      AEEIPv6PrefixInfo*  pPrefixInfo
   );

Parameters:
   netd: network descriptor
   pAddr: [in] - The IPv6 address used by the application.
   pPrefixInfo: [out] Struct holding the information about the
                      prefix associated with pAddr. The application
                      should allocate this struct.

Return Value:
   On success, returns AEE_NET_SUCCESS.
   If a prefix corresponding to the given address 
   does not exist (either it never existed or was removed),
   returns AEE_NET_SUCCESS, but the prefix state will be 
   AEE_IPV6_PREFIX_NOT_EXIST, and the prefix length will be 0.
   If the prefix state received from the AMSS is illegal,
   returns AEE_NET_ERROR.

Comments:
   None.

Side Effects:
   None

See Also:
   AEEIPv6PrefixInfo
========================================================================

Function: OEMNet_GetOutageInfo()

Description:
   Retrieve Outage information.

Prototype:
   int16 OEMNet_GetOutageInfo(
      OEMCONTEXT     netd,
      AEEOutageInfo* pOutageInfo
   );

Parameters:
   netd:           network descriptor
   pOutageInfo:    filled in with the outage information
                   (pre-allocated by caller)

Return Value:
   On success, returns AEE_NET_SUCCESS.
   Returns EBADPARM if one of the parameters is not valid.

Comments:
   None.

Side Effects:
   None

See Also:
   AEEOutageInfo

========================================================================
INTERFACES   DOCUMENTATION
=======================================================================
Interface Name: OEMSocket 
=======================================================================

Function: OEMSocket_GetSockName()

Description:
   Returns the local IP address of a socket.  NOTE: the address will be in 
   network byte order.

   This function is synchronous, and therefore should not call any notification
   functions.
  
Prototype:
   int16 OEMSocket_GetSockName
   (
      OEMCONTEXT sockd,
      void      *pAddr,
      int       *pAddrLen
   );

Parameters:
   sockd: [in] socket descriptor
   pAddr: [out] pointer to an opaque structure (see AEESockAddrStorage) 
          allocated by the client to be filled with the IP address.
   pAddrLen: [in/out] indicates the size of the allocated memory pointed by 
             pAddr. On return, indicates the size of the IP address that was 
             filled into pAddr.

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   AEESockAddrStorage

========================================================================

Function: OEMSocket_GetPeerName()

Description:
   Returns the IP address of a connected peer.  NOTE: the address will be in 
   network byte order.

   This function is synchronous, and therefore should not call any notification
   functions.
  
Prototype:
   int16 OEMSocket_GetPeerName
   (
      OEMCONTEXT sockd,
      void      *pAddr,
      int       *pAddrLen
   );

Parameters:
   sockd:  [in] socket descriptor
   pAddr:  [out] pointer to an opaque structure (see AEESockAddrStorage) 
           allocated by the client to be filled with the IP address.
   pAddrLen: [in/out] indicates the size of the allocated memory pointed by 
             pAddr. On return, indicates the size of the IP address that was 
             filled into pAddr.

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   AEESockAddrStorage

==========================================================================

Function: OEMSocket_Open()

Description:
   Create a TCP or UDP socket and related data structures, and return a 
   reference to that socket.

   Supported Types:
   The OEM must support SOCK_STREAM and SOCK_DGRAM data types.
   The OEM must support IPPROTO_TCP (TCP) and IPPROTO_UDP (UDP) data types.

   Note this function must be called to obtain a valid socket descriptor, for
   use with all other socket-related functions.  Thus, before any socket
   functions can be used (e.g. I/O, asynchronous notification, etc.), this
   call must have successfully returned a valid socket descriptor.

   This function is synchronous, and therefore should not callback any 
   notification function.

Prototype:
   OEMCONTEXT OEMSocket_Open
   (
      OEMCONTEXT netd,
      uint16     wFamily,
      NetSocket  type,
      int        protocol,
      int16     *err
   );

Parameters:
   netd: network descriptor
   wFamily: address family of the requested socket
   type: socket type (see above)
   protocol: socket protocol (see above)
   err: error code (returned by operation) 

Return Value:
   On successful creation of a socket, this function returns socket  
   descriptor which is a signed value greater than or equal to 0.  On error, 
   returns AEE_NET_ERROR.  Error specifics are returned via the err parameter.
 
Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMSocket_Connect()

Description:
   For TCP, attempts to establish the TCP connection.  Upon successful
   connection, should resume the socket waiter callback.
   OEMs implementations must not support connected UDP sockets, and must
   return an error if OEMSocket_Connect is called with a UDP socket.

   This function is asynchronous, and should resume the socket waiter
   callback if the connection attempt is completed or aborted, and the
   original error value was AEE_NET_SUCCESS or AEE_NET_EWOULDBLOCK.

Prototype:
   int16 OEMSocket_Connect
   (
      OEMCONTEXT  sockd,                                       
      const void *pAddr
   );

Parameters:
   sockd: socket descriptor
   pAddr: an opaque structure (see AEESockAddrStorage) containing the 
          address in network byte order to connect to.

Return Value:
   Returns AEE_NET_SUCCESS if arguments are valid, and the connection process
   could be started.  Thus, a return value of AEE_NET_SUCCESS does not indicate
   that the socket could be connected.  If an error can be detected immediately,
   returns one of the AEE designated error codes indicating reason for failure.

Comments:
   Must have called OEMSocket_Open() prior to use.

Side Effects:
   Starts connection process for a socket.  May automatically call bind() on
   that socket.

See Also:
   AEESockAddrStorage

===========================================================================

Function: OEMSocket_Bind()

Description:
   For all client sockets, attaches a local address to the socket.  If the 
   call is not explicitly issued, the socket will implicitly bind in calls to 
   OEMSocket_Connect() or OEMSocket_SendTo().  Note that this function does 
   not support binding a local IP address, but rather ONLY a local port number.
   The local IP address is assigned automatically by the sockets library.
   The function must receive (as a parameter) a valid socket descriptor, 
   implying a previous successful call to OEMSocket_Open().

   OEMs are not required to provide bind() functionality to TCP sockets.

   This function is synchronous, and therefore should not callback any
   notification functions.

Prototype:
   int16 OEMSocket_Bind
   (
      OEMCONTEXT  sockd,                                       
      const void *pAddr
   );

Parameters:
   sockd: socket descriptor
   pAddr: an opaque structure (see AEESockAddrStorage) containing the 
          address in network byte order to bind to.

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   AEESockAddrStorage

========================================================================

Function: OEMSocket_Close()

Description:
   Non-blocking close of a socket.  Performs all necessary clean-up of data 
   structures and frees the socket for re-use.  For TCP initiates the active 
   close for connection termination.  After the TCP connection is complete,
   the socket resources may optionally not be freed.  In this case, this
   function should return AEE_NET_ERROR and set err to AEE_NET_EWOULDBLOCK.
   The aee libraries will receive notification via OEMSocket_AsyncSelect, 
   and call OEMSocket_Close again to free the resources.

   This function can be synchronous (returning anything other than *err
   set to AEE_NET_EWOULDBLOCK), or asynchronous as described above.  For
   asynchronous incantations, the socket waiter callback should be resumed
   if and only if the AEE_NET_CLOSE_EVENT was registered with
   OEMSocket_AsyncSelect.

Prototype:
   int16 OEMSocket_Close
   (
      OEMCONTEXT sockd
   );

Parameters:
   sockd: socket descriptor

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.  It should be noted
   that if the socket cannot be closed right away, the OEM may return an
   AEE_NET_EWOULDBLOCK error, indication that it should be called at a later
   time (through an indication via the OEMSocket_AsyncSelect() function.

Comments:
   None

Side Effects:
   Initiates active close for TCP connections.

See Also:
   OEMSocket_AsyncSelect()

========================================================================

Function: OEMSocket_Write()

Description:
   Sends specified number of bytes in the buffer over the TCP transport.
   If a socket is connected, but it is not possible to send or buffer
   data, the function should return AEE_NET_ERROR and set *err to be
   AEE_NET_WOULDBLOCK.

Prototype:
   int32 OEMSocket_Write
   (
      OEMCONTEXT  sockd,                                       
      const byte *buffer,
      uint32      bytes,
      int16      *err
   );

Parameters:
   sockd: socket descriptor
   buffer: user buffer from which to copy data
   nbytes: number of bytes to be written to socket
   err: error code (returned by operation) 

Return Value:
   On success, returns the number of bytes written, which could be less than
   the number of bytes specified.  On error (including AEE_NET_EWOULDBLOCK), 
   returns AEE_NET_ERROR. 

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMSocket_Read()

Description:
   Reads specified number of bytes into buffer from the TCP transport.
   If the socket is connected, but there is no data to read, the function
   should return AEE_NET_ERROR and set *err to AEE_NET_EWOULDBLOCK.

Prototype:
   int32 OEMSocket_Read
   (
      OEMCONTEXT sockd,                                       
      byte      *buffer,
      uint32     nbytes,
      int16     *err
   );

Parameters:
   sockd: socket descriptor
   buffer: user buffer to which to copy data
   nbytes: number of bytes to be read from socket
   err: error code (returned by operation) 

Return Value:
   On success, returns the number of bytes read, which could be less than
   the number of bytes specified.  On error, returns AEE_NET_ERROR, including
   when *err == AEE_NET_EWOULDBLOCK.

   NOTE: A return of 0, indicates that an End-of-File condition has occurred.

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMSocket_Writev()

Description:
   Provides the gather write variant of OEMSocket_Write(), which 
   allows the application to write from non-contiguous buffers.    Sends 
   specified number of bytes in the buffer over the TCP transport.

Prototype:
   int32 OEMSocket_Writev
   (
      OEMCONTEXT        sockd,                                       
      const SockIOBlock iov[],     
      uint16            iovcount,
      int16            *err
   );

Parameters:
   sockd: socket descriptor
   iov: array of data buffers from which to copy data
   iovcount: number of array items
   err: error code (returned by operation) 

Return Value:
   On success, returns the number of bytes written, which could be less
   than the number of bytes specified.  On error, returns AEE_NET_ERROR.

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMSocket_Readv()

Description:
   Provides the scatter read variant of OEMSocket_Read(), which 
   allows the application to read into non-contiguous buffers.    Reads
   specified number of bytes into the buffer from the TCP transport.

Prototype:
   int32 OEMSocket_Readv
   (
      OEMCONTEXT  sockd,                                       
      SockIOBlock iov[],           
      uint16      iovcount,                                   
      int16      *err
   );

Parameters:
   sockd: socket descriptor
   iov: array of data buffers to copy data into
   iovcount: number of array items
   err: error code (returned by operation) 

Return Value:
   On success, returns the number of bytes read, which could be less than
   the number of bytes specified.  On error, returns AEE_NET_ERROR.

   NOTE: A return of 0, indicates that an End-of-File condition has occurred.

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEMSocket_SendTo()

Description:
   Sends 'nbytes' bytes in the buffer over the UDP transport.

Prototype:
   int32 OEMSocket_SendTo
   (
      OEMCONTEXT  sockd,                                       
      const byte *buffer,               
      uint32      nbytes,                          
      uint32      flags,
      const void *pAddr,
      int16      *err
   );

Parameters:
   sockd: socket descriptor
   buffer: user buffer from which to copy the data
   nbytes: number of bytes to be written
   flags: send flags
   pAddr: an opaque structure (see AEESockAddrStorage) containing the 
          address in network byte order to send to.
   err: error code (returned by operation) 

Return Value:
   On success, returns the number of bytes to be written, which could be
   less than the number of bytes specified.  On error, returns AEE_NET_ERROR.

Comments:
   Currently supported flags are documented in AEENet.h.

   For cdma2000, the URGENT flag corresponds to use of Short Data Burst (SDB)
   over the reversed enhanced access channel (R-EACH). The WAKEUP flag requests
   traffic channel origination immediately after the SDB attempt.  This is
   necessary because the cdma2000 standard currently requires origination to
   take priority over any other access attempt, and thus the short data burst
   would either be prematurely aborted or undesirably delayed.

Side Effects:
   None

See Also:
   AEESockAddrStorage

==========================================================================

Function: OEMSocket_RecvFrom()

Description:
   Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
   address structure with values from who sent the data.

Prototype:
   int32 OEMSocket_RecvFrom
   (
      OEMCONTEXT sockd,                                       
      byte      *buffer,               
      uint32     nbytes,
      void      *pAddr,
      int       *pAddrLen,
      int16     *err
   );

Parameters:
   sockd: socket descriptor
   buffer: user buffer from which to copy the data
   nbytes: number of bytes to be written
   pAddr: pointer to an opaque structure (see AEESockAddrStorage) allocated by
          the client to be filled with the IP address of the sender.
   pAddrLen: caller indicates the size of the allocated memory pointed by 
             pAddr. On return, indicates the size of the IP address that was 
             filled into pAddr.
   err: error code (returned by operation) 

Return Value:
   On success, returns the number of bytes read, which could be less than the
   number of bytes specified.  On error, returns AEE_NET_ERROR.

Comments:
   None

Side Effects:
   None

See Also:
   AEESockAddrStorage

==========================================================================

Function: OEMSocket_AsyncSelect()

Description:
   Enables the events to be notified about through the asynchronous 
   notification mechanism.  Application specifies a bitmask of events that it
   is interested in, for which it will receive asynchronous notification via
   its application callback function.  This function also performs a real-time
   check to determine if any of the events have already occurred, and if so
   resume either the Net waiter callback or the socket waiter callback
   according to the event.

   Events OEMs need to support are:
~
      AEE_NET_READ_EVENT: Socket is now available to read, or connect~
      AEE_NET_WRITE_EVENT: Socket is now available for writing~
      AEE_NET_CLOSE_EVENT: Socket is being closed~
*

Prototype:
   int16 OEMSocket_AsyncSelect
   (
      OEMCONTEXT sockd,                                       
      int32      interest_mask
   );

Parameters:
   sockd: socket descriptor
   mask: bitmask of events to set (see above)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   Sets the relevant event mask in the socket control block.  Will also 
   notify the application via the callback function.

See Also:
   None

===========================================================================

Function: OEMSocket_GetNextEvent()

Description:
   This function performs a real-time check to determine if any of the events 
   of interest specified with the mask in OEMSocket_AsyncSelect() have 
   occurred.  It also clears any bits in the event mask that have occurred.
   The application must re-enable these events through a subsequent call to 
   OEMSocket_AsyncSelect().  The application passes in a pointer to a single 
   socket descriptor to determine if any events have occurred for that socket.

Prototype:
   int32 OEMSocket_GetNextEvent
   ( 
      OEMCONTEXT *sockd, 
      int16      *err
   );

Parameters:
   sockd: socket descriptor
   err: error code (returned by operation) 

Return Value:
   Returns an event mask of the events that were asserted.  A value of zero
   indicates that no events have occurred.

   On error, returns AEE_NET_ERROR.
   
Comments:
   None

Side Effects:
   Clears the bits in the socket control block event mask, corresponding to 
   the events that have occurred.

See Also:
   OEMSocket_AsyncSelect()

===========================================================================

Function: OEMSocket_Listen()

Description:
   Performs a passive open for connections, such that incoming connections 
   may be subsequently accepted.

   The socket must be a TCP socket that has been bound to a local port. The
   backlog parameter indicates the maximum length for the queue of pending
   connections.  If backlog is larger than the system maximum, it will be
   silently reduced to the system maximum. 

Prototype:
   int16 OEMSocket_Listen
   (
      OEMCONTEXT sockd,
      int16      backlog
   );

Parameters:
   sockd: socket descriptor
   backlog: maximum number of pending connections

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_Accept()

===========================================================================

Function: OEMSocket_Accept()

Description:
   The accept function is used on listening sockets to respond when
   AEE_NET_READ_EVENT is asserted.  The first backlog queued connection is
   removed from the queue, and bound to a new socket (as if
   you called OEMSocket_Open).  The newly created socket is in the
   connected state.  The listening socket is unaffected and the queue
   size is maintained (i.e. there is no need to call listen again.)

Prototype:
   OEMCONTEXT OEMSocket_Accept
   (
      OEMCONTEXT sockd,
      int16     *err
   );

Parameters:
   sockd: listening socket descriptor
   err: error code (returned by operation) 

Return Value:
   On successful creation of a socket, this function returns socket file 
   descriptor which is OEM defined.  
   On error, returns AEE_NET_ERROR.  Error specifics are returned via the err parameter.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_Listen

===========================================================================

Function: OEMSocket_Shutdown()

Description:
   Causes all or part of a full-duplex connection to be terminated gracefully.

   If how is AEE_SHUTDOWN_RD, no more reads will be allowed.  If how is AEE_SHUTDOWN_WR,
   no more writes will be allowed (AKA half-close).  If how is AEE_SHUTDOWN_RDWR,
   both read and write will be disallowed.

Prototype:
   int16 OEMSocket_Shutdown
   (
      OEMCONTEXT sockd,
      int32      how
   );

Parameters:
   sockd: socket descriptor
   how: dictates which portion(s) of the connection to shutdown

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes (including AEE_NET_WOULDBLOCK) indicating
   reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetNativeDescriptor()

Description:
   Returns the native socket descriptor from the underlying implementation.

Prototype:
   int16 OEMSocket_GetNativeDescriptor
   (
      OEMCONTEXT sockd,
      uint32    *pdwDesc
   );

Parameters:
   sockd: socket descriptor of interest
   pdwDesc: pointer to dword, which will be filled in with the native descriptor

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetSDBAckInfo()

Description:
   Retrieves the latest SDB-Ack information received for the socket.

Prototype:
   int16 OEMSocket_GetSDBAckInfo
   (
      OEMCONTEXT sockd,
      AEESDBAckInfo* pInfo
   );

Parameters:
   sockd: socket descriptor of interest
   pInfo: pointer to a struct, which will be filled in with the 
          last SDB-Ack information. The struct should be allocated
          by the user.

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_SetSDBAckCB
   OEMSocket_GetSDBAckCB

===========================================================================

Function: OEMSocket_SetKeepAlive()

Description:
   Sets whether or not to periodically test if connection still alive.

Prototype:
   int16 OEMSocket_SetKeepAlive
   (
      OEMCONTEXT sockd,
      boolean    bKeepAlive
   );

Parameters:
   sockd: socket descriptor of interest
   bKeepAlive: whether or not to periodically test if connection still alive

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetLinger()

Description:
   Sets how to long to linger on close if data to send.

Prototype:
   int16 OEMSocket_SetLinger
   (
      OEMCONTEXT sockd,
      boolean    bOn,          // linger on or off
      uint32     dwLinger      // linger time, in seconds
   );

Parameters:
   sockd: socket descriptor of interest
   bOn: linger on or off
   dwLinger: linger time, in seconds

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetRcvBuf()

Description:
   Sets the receive buffer size.

Prototype:
   int16 OEMSocket_SetRcvBuf
   (
      OEMCONTEXT sockd,
      int        nRcvBuf
   );

Parameters:
   sockd: socket descriptor of interest
   nRcvBuf: the desired receive buffer size

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetSndBuf()

Description:
   Sets the send buffer size.

Prototype:
   int16 OEMSocket_SetSndBuf
   (
      OEMCONTEXT sockd,
      int        nSndBuf
   );

Parameters:
   sockd: socket descriptor of interest
   nSndBuf: the desired send buffer size

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetNoDelay()

Description:
   Disables the Nagle algorithm.

Prototype:
   int16 OEMSocket_SetNoDelay
   (
      OEMCONTEXT sockd,
      boolean    bNodelay
   );

Parameters:
   sockd: socket descriptor of interest
   bNoDelay: whether or not to disable the Nagle algorithm

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetDelayedAck()

Description:
   Sets the state of delayed ack.

Prototype:
   int16 OEMSocket_SetDelayedAck
   (
      OEMCONTEXT sockd,
      boolean    bDelayedAck
   );

Parameters:
   sockd: socket descriptor of interest
   bDelayedAck: whether or not to enable delayed ack

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetSACK()

Description:
   Enable SACK.

Prototype:
   int16 OEMSocket_SetSACK
   (
      OEMCONTEXT sockd,
      boolean    bSACK
   );

Parameters:
   sockd: socket descriptor of interest
   bSACK: whether or not to Enable SACK

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetTimeStamp()

Description:
   Enable TCP time stamp option.

Prototype:
   int16 OEMSocket_SetTimeStamp
   (
      OEMCONTEXT sockd,
      boolean    bTimeStamp
   );

Parameters:
   sockd: socket descriptor of interest
   bTimeStamp: whether or not to Enable TCP time stamp option

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetMembership()

Description:
   Join/leave a multicast group

Prototype:
   int16 OEMSocket_SetMembership
   (
   OEMCONTEXT sockd,
   boolean    bAdd,
   IPAddr    *pMCast
   );


Parameters:
   sockd: socket descriptor of interest
   bAdd:  whether to add or drop membership
   pMCast: the multicast group to join/leave
    
Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetIPToS()

Description:
   Sets the socket's IP Type of Service.


Prototype:
   int16 OEMSocket_SetIPToS
   (  
      OEMCONTEXT sockd,
      int nIPToS
   );

Parameters:
   sockd: socket descriptor of interest
   nIPToS: IP Type of Service

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetIPv6TrafficClass()

Description:
   Set the socket's Traffic Class for outgoing IPv6 datagrams.


Prototype:
   int16 OEMSocket_SetIPv6TrafficClass
   (  
      OEMCONTEXT sockd,
      int nIPv6TClass
   );

Parameters:
   sockd: socket descriptor of interest
   nIPv6TClass: traffic class

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetIPTTL()

Description:
   Sets the socket's Time To Live for outgoing IP packets.

Prototype:
   int16 OEMSocket_SetIPTTL
   (  
      OEMCONTEXT sockd,
      int nIPTTL
   );

Parameters:
   sockd: socket descriptor of interest
   nIPTTL: Time To Live

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetTCPMaxSegmentSize()

Description:
   Sets the socket's maximum segment size for outgoing TCP packets.

Prototype:
   int16 OEMSocket_SetTCPMaxSegmentSize
   (  
      OEMCONTEXT sockd,
      int nMaxSeg
   );

Parameters:
   sockd: socket descriptor of interest
   nMaxSeg: maximum segment size

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_SetReuseAddr()

Description:
   Sets whether or not to allow sockets to reuse a pair of address and port 
   number that is already bound by another socket.    

Prototype:
   int16 OEMSocket_SetReuseAddr
   (
      OEMCONTEXT sockd,
      boolean bReuseAddr
   );

Parameters:
   sockd: socket descriptor of interest
   bReuseAddr: whether or not to allow sockets to reuse a pair of address and 
   port number that is already bound by another socket.    

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_GetReuseAddr

===========================================================================

Function: OEMSocket_SetDisableFlowFwding()

Description:
   Sets whether or not to disable routing data on best effort flow when QoS is 
   not available.

Prototype:
   int16 OEMSocket_SetDisableFlowFwding
   (
      OEMCONTEXT sockd,
      boolean bDisableFlowFwding
   );

Parameters:
   sockd: socket descriptor of interest
   bDisableFlowFwding: whether or not to disable routing data on best effort 
   flow when QoS is not available

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_GetDisableFlowFwding

===========================================================================

Function: OEMSocket_SetSDBAckCB()

Description:
   Sets a callback to be invoked when an SDB or DOS Ack is received.

Prototype:
   int16 OEMSocket_SetSDBAckCB
   (
      OEMCONTEXT sockd,
      AEECallback* pCB
   ); 

Parameters:
   sockd: socket descriptor of interest
   pCb: a callback to be invoked when the SDB or DOS Ack is received.

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_GetSDBAckCB

===========================================================================

Function: OEMSocket_GetKeepAlive()

Description:
   Gets whether or not a connection is periodically tested if it is still alive.

Prototype:
   int16 OEMSocket_GetKeepAlive
   (
      OEMCONTEXT sockd,
      boolean   *pbKeepAlive
   );

Parameters:
   sockd: socket descriptor of interest
   pbKeepAlive: whether or not a connection is periodically tested if it is still alive (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetLinger()

Description:
   Gets how to long to linger on close if data to send.

Prototype:
   int16 OEMSocket_GetLinger
   (
      OEMCONTEXT sockd,
      boolean   *pbOn,          // linger on or off
      uint32    *pdwLinger      // linger time, in seconds
   );

Parameters:
   sockd: socket descriptor of interest
   *pbOn: linger on or off (returned)
   *pdwLinger: linger time, in seconds (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetRcvBuf()

Description:
   Gets the receive buffer size.

Prototype:
   int16 OEMSocket_GetRcvBuf
   (
      OEMCONTEXT sockd,
      int       *pnRcvBuf
   );

Parameters:
   sockd: socket descriptor of interest
   pnRcvBuf: the desired receive buffer size (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetSndBuf()

Description:
   Gets the send buffer size.

Prototype:
   int16 OEMSocket_GetSndBuf
   (
      OEMCONTEXT sockd,
      int       *pnSndBuf
   );

Parameters:
   sockd: socket descriptor of interest
   pnSndBuf: the desired send buffer size (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetNoDelay()

Description:
   Gets the state of the Nagle algorithm.

Prototype:
   int16 OEMSocket_GetSndBuf
   (
      OEMCONTEXT sockd,
      boolean   *pbNodelay
   );

Parameters:
   sockd: socket descriptor of interest
   pbNoDelay: whether or not the Nagle algorithm is disabled (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetDelayedAck()

Description:
   Gets the state of delayed ack.

Prototype:
   int16 OEMSocket_GetDelayedAck
   (
      OEMCONTEXT sockd,
      boolean   *pbDelayedAck
   );

Parameters:
   sockd: socket descriptor of interest
   pbDelayedAck: whether or not delayed ack is enabled (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetSACK()

Description:
   Gets the state of SACK.

Prototype:
   int16 OEMSocket_GetSACK
   (
      OEMCONTEXT sockd,
      boolean   *pbSACK
   );

Parameters:
   sockd: socket descriptor of interest
   pbSACK: whether or not SACK is enabled (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetTimeStamp()

Description:
   Gets the state of TCP time stamp option.

Prototype:
   int16 OEMSocket_GetTimeStamp
   (
      OEMCONTEXT sockd,
      boolean   *pbTimeStamp
   );

Parameters:
   sockd: socket descriptor of interest
   pbTimeStamp: whether or not TCP time stamp option is enabled (returned)

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetIPToS()

Description:
   Gets the socket's Type of Service.

Prototype:
   int16 OEMSocket_GetIPToS
   (
      OEMCONTEXT sockd,
      int*       pnIPToS
   );

Parameters:
   sockd: socket descriptor of interest
   pnIPToS: pointer to the Type of Service value

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetIPv6TrafficClass()

Description:
   Gets the socket's Traffic Class for outgoing IPv6 datagrams.

Prototype:
   int16 OEMSocket_GetIPv6TrafficClass
   (
      OEMCONTEXT sockd,
      int*       pnIPv6TClass
   );

Parameters:
   sockd: socket descriptor of interest
   pnIPv6TClass: pointer to the traffic class value

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetIPTTL()

Description:
   Gets the socket's Time To Live for outgoing IP packets.
   
Prototype:
   int16 OEMSocket_GetIPTTL
   (
      OEMCONTEXT sockd,
      int*       pnIPTTL
   );

Parameters:
   sockd: socket descriptor of interest
   pnIPTTL: pointer to the Time To Live value

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetTCPMaxSegmentSize()

Description:
   Gets the socket's maximum segment size for outgoing TCP packets.

Prototype:
   int16 OEMSocket_GetTCPMaxSegmentSize
   (
      OEMCONTEXT sockd,
      int*       pnMaxSeg
   );

Parameters:
   sockd: socket descriptor of interest
   pnMaxSeg: pointer to the maximum segment size value

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================

Function: OEMSocket_GetReuseAddr()

Description:
   Returns whether or not to allow sockets to reuse a pair of address and port 
   number that is already bound by another socket.       

Prototype:
   int16 OEMSocket_GetReuseAddr
   (
      OEMCONTEXT sockd,
      boolean*   bReuseAddr
   );

Parameters:
   sockd: socket descriptor of interest
   bReuseAddr: whether or not to allow sockets to reuse a pair of address and 
   port number that is already bound by another socket.    

Return Value:
   On success, returns AEE_NET_SUCCESS. On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_SetReuseAddr

===========================================================================

Function: OEMSocket_GetDisableFlowFwding()

Description:   
   Returns whether or not the routing data on best effort flow is disabled 
   when QoS is not available.

Prototype:
   int16 OEMSocket_GetDisableFlowFwding
   (
      OEMCONTEXT sockd,
      boolean*   pbDisableFlowFwding
   );

Parameters:
   sockd: socket descriptor of interest
   pbDisableFlowFwding: whether or not the routing data on best effort flow is 
   disabled when QoS is not available.

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_SetDisableFlowFwding

======================================================================

Function: OEMSocket_GetSDBAckCB()

Description:   
   Returns the data associated with AEE_SO_SDB_ACK_CB socket option.
   Namely, the associated callback function and user data.
  
Prototype:
   int16 OEMSocket_GetSDBAckCB
   (
      OEMCONTEXT         sockd,
      AEESDBAckCBType*   pAEESDBAckCBType
   );

Parameters:
   sockd: socket descriptor of interest
   pAEESDBAckCBType: data associated to the socket option.

Return Value:
   On success, returns AEE_NET_SUCCESS.  On error, returns one of the AEE
   designated error codes indicating reason for failure.

Comments:
   None

Side Effects:
   None

See Also:
   OEMSocket_SetSDBAckCB

====================================================================== */

#endif /* OEMSOCK_H */

