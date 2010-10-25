#ifndef AEEINETWORK_H
#define AEEINETWORK_H
/*=========================================================================

FILE: AEEINetwork.h

SERVICES:
   INetwork interface

DESCRIPTION:
   This interface provides mechanisms for explicitly set/get parameters
   of a BREW application data network as well as means for explicitly starting
   the network.

===========================================================================

   Copyright (c) 2006-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEEIQI.h"
#include "AEENetAddrTypes.h"
#include "AEENetTypes.h"
#include "AEECallback.h"
#include "AEEStdErr.h"
#include "AEEIBearerTechnology.h"

/*-------------------------------------------------------------------
      Defines and Type Declarations
-------------------------------------------------------------------*/

// Net events
#define NETWORK_NUM_OF_EVENT_TYPES  5           // This definition is obsolete 
                                                // and should be ignored

#define NETWORK_EVENT_STATE                   1  // Data network state change
#define NETWORK_EVENT_IP                      2  // IP address change
#define NETWORK_EVENT_QOS_AWARE_UNAWARE       3  // QoS Aware/Unaware events
#define NETWORK_EVENT_IDLE                    4  // Network is idle
#define NETWORK_EVENT_MTPD                    5  // MTPD events
#define NETWORK_EVENT_IPV6_PRIVACY_ADDR       6  // IPv6 Privacy Address events
#define NETWORK_EVENT_OUTAGE                  7  // Outage (mobile handoff) events
#define NETWORK_EVENT_QOS_PROFILES_CHANGED    8  // QoS profile change events
#define NETWORK_EVENT_IPV6_PREFIX_UPDATE      9  // IPv6 Prefix Update events
#define NETWORK_EVENT_BEARER_TECH_CHANGED    10  // Bearer Technology Change events
// IMPORTANT NOTE: Addition of any new events must be defined using a unique ID (newly
//                 generated classId), instead of sequential enumeration.
//                 This is to avoid conflicting definitions in different development
//                 branches (e.g. 3.x and 4.x).
//                 Also note that addition of a new network event id requires an 
//                 appropriate mapping in the implementation file. 
#define NETWORK_EVENT_EXTENDED_IP_CONFIG  0x0106e2a9                  // Extended IP Config events

// Net Status
#define AEE_NET_STATUS_INVALID_STATE   0
#define AEE_NET_STATUS_OPENING         1
#define AEE_NET_STATUS_OPEN            2
#define AEE_NET_STATUS_CLOSING         3
#define AEE_NET_STATUS_CLOSED          4
#define AEE_NET_STATUS_SLEEPING        5
#define AEE_NET_STATUS_ASLEEP          6
#define AEE_NET_STATUS_WAKING          7

typedef int AEENetStatus;

// INetwork Get/Set options
#define INETWORK_GET_SET_OPTIONS_BASE (1000)

#define INETWORK_OPT_DEF_RLP3         (1 + INETWORK_GET_SET_OPTIONS_BASE)  /* default RLP3 settings */
#define INETWORK_OPT_CUR_RLP3         (2 + INETWORK_GET_SET_OPTIONS_BASE)  /* current RLP3 settings */
#define INETWORK_OPT_NEG_RLP3         (3 + INETWORK_GET_SET_OPTIONS_BASE)  /* negotiated RLP3 settings */
#define INETWORK_OPT_DNS_SERVERS      (4 + INETWORK_GET_SET_OPTIONS_BASE)  /* generic address (IPv4/6) of system DNS servers */
#define INETWORK_OPT_PPP_AUTH         (5 + INETWORK_GET_SET_OPTIONS_BASE)  /* system PPP auth credentials */
#define INETWORK_OPT_UDP_URGENT       (6 + INETWORK_GET_SET_OPTIONS_BASE)  /* urgent sendto info */
#define INETWORK_OPT_DDTM_PREF        (7 + INETWORK_GET_SET_OPTIONS_BASE)  /* Dedicated data transmission mode (HDR only) */
#define INETWORK_OPT_DORMANCY_TIMEOUT (8 + INETWORK_GET_SET_OPTIONS_BASE)  /* IS-707 (cdma2000 1x) packet data idle time before dormancy */
#define INETWORK_OPT_HW_ADDR          (9 + INETWORK_GET_SET_OPTIONS_BASE)  /* hardware address */

// Unique IDs for ExtFunc() 
#define INETWORK_EXT_FUNC_IGNORE_TIMEOUT_VALS  0x01039421
#define INETWORK_EXT_FUNC_GET_SELECTED_NETWORK 0x010388da
#define INETWORK_EXT_FUNC_CLOSE 0x01046203
#define INETWORK_EXT_FUNC_REFRESH_DHCP_CONFIG_INFO  0x0106e2d2

// INetwork event notification function
typedef void (*PFNNETWORKEVENT)(void * pUser, int nEvent);

//
// for INetwork_GetSupportedQoSProfiles
//
typedef struct AEENetworkQoSProfiles
{
   uint16* awQoSProfiles;
   uint8   uNumProfiles;
   uint8*  puNumProfilesRequired;
} AEENetworkQoSProfiles;

#define NETWORK_EXTFUNC_GET_SUPPORTED_QOS_PROFILES 0x0103f7a4 

//
// for INetwork_GetSipServerAddr
//
typedef struct AEENetworkSipServerAddresses
{
   IPAddr* aSipServerAddresses;
   uint8   uNumAddresses;
   uint8*  puNumAddressesRequired;
} AEENetworkSipServerAddresses;

#define NETWORK_EXTFUNC_GET_SIP_SERVER_ADDRESSES 0x0104103b

//
// for INetwork_GetSipServerDomainNames
//
typedef struct AEENetworkSipServerDomainNames
{
   AEENetDomainName* aSipServerDomainNames;
   uint8   uNumNames;
   uint8*  puNumNamesRequired;
} AEENetworkSipServerDomainNames;
 
#define NETWORK_EXTFUNC_GET_SIP_SERVER_DOMAIN_NAMES 0x01041129 

//
// for INetwork_GetBearerTechnology
//
#define NETWORK_EXTFUNC_GET_BEARER_TECHNOLOGY       0x010422e4

//
// for INetwork_GetBearerTechnologyOpts
//
#define NETWORK_EXTFUNC_GET_BEARER_TECHNOLOGY_OPTS  0x01045a91

//
// for INetwork_GenerateIPv6PrivAddr
//
#define NETWORK_EXTFUNC_GENERATE_IPV6_PRIV_ADDR     0x01042cdd

#define NETWORK_EXTFUNC_GET_PRIV_IPV6_ADDR_INFO     0x01042cdf

//
// for INetwork_GetOutageInfo
//
#define NETWORK_EXTFUNC_GET_OUTAGE_INFO             0x01045a90

//
// for INetwork_GetIPv6PrefixInfo
//
typedef struct AEENetworkPrefixInfo
{  
   INAddr6           addr;
   AEEIPv6PrefixInfo prefixInfo;
} AEENetworkPrefixInfo;

#define NETWORK_EXTFUNC_GET_IPV6_PREFIX_INFO    0x010457b3


//********************************************************************************************************************************
//
// INetwork interface
//
//********************************************************************************************************************************

#define AEEIID_INetwork 0x01035f45

#if defined(AEEINTERFACE_CPLUSPLUS)

struct INetwork : public IQI 
{
   virtual int CDECL SelectNetworkEx (int nNetwork, int16 nFamily) = 0;
   virtual int CDECL OnEvent (int nEvent, PFNNETWORKEVENT pfn, void *pUser, boolean bRegister) = 0;
   virtual int CDECL NetStatus (AEENetStatus *pns, AEENetStats *ps) = 0;
   virtual int CDECL Start () = 0;
   virtual int CDECL AddRetryCB (AEECallback* pcbRetry) = 0;
   virtual int CDECL GetMyIPAddrs (struct IPAddr aipaAddr[], int* pNumAddr) = 0;
   virtual int CDECL SetLinger (uint16 wSecs, uint16* pwldSecs) = 0;
   virtual int CDECL SetDormancyTimeout (uint8 ucSecs) = 0;
   virtual int CDECL SetOpt (int nOptName, void *pOptVal, int nOptSize) = 0;
   virtual int CDECL GetOpt (int nOptName, void *pOptVal, int *pnOptSize) = 0;
   virtual int CDECL GetLastNetDownReason (AEENetDownReason *pnReason) = 0;
   virtual int CDECL IsQoSAware (boolean* pbIsQoSAware) = 0;
   virtual int CDECL ExtFunc (AEECLSID id, void *pBuf, int nBufSize) = 0;
   int CDECL GetSipServerAddr(IPAddr    aSipServerAddresses[],
                              uint8     uNumAddresses,
                              uint8*    puNumAddressesRequired) 
   { 
      AEENetworkSipServerAddresses sipAddresses;

      sipAddresses.aSipServerAddresses = aSipServerAddresses;
      sipAddresses.uNumAddresses = uNumAddresses;
      sipAddresses.puNumAddressesRequired = puNumAddressesRequired;
      return ExtFunc(INETWORK_EXT_FUNC_GET_SIP_SERVER_ADDRESSES,
                     &sipAddresses,
                     sizeof(sipAddresses)); 
   }
   int CDECL GetSipServerDomainNames(AEENetDomainName  aSipServerDomainNames[],
                               uint8             uNumNames,
                               uint8*            puNumNamesRequired)
   {
      AEENetworkSipServerDomainNames sipDomainNames;

      sipDomainNames.aSipServerDomainNames = aSipServerDomainNames;
      sipDomainNames.uNumNames = uNumNames;
      sipDomainNames.puNumNamesRequired = puNumNamesRequired;   
      return ExtFunc(INETWORK_EXT_FUNC_GET_SIP_SERVER_DOMAIN_NAMES,
                     &sipDomainNames,
                     sizeof(sipDomainNames));
   }
   int CDECL GetSelectedNetwork(int* pnNetwork)
   {
      return ExtFunc(INETWORK_EXT_FUNC_GET_SELECTED_NETWORK, pnNetwork, 0);
   }
   void IgnoreTimeoutVals(boolean bIgnore)
   {
      ExtFunc(INETWORK_EXT_FUNC_IGNORE_TIMEOUT_VALS, (void*)&bIgnore, 0);
   }
   int CDECL Close(void)
   {
      return ExtFunc(INETWORK_EXT_FUNC_CLOSE, NULL, 0);
   }
   int CDECL RefreshDHCPConfigInfo(void)
   {
      return ExtFunc(INETWORK_EXT_FUNC_REFRESH_DHCP_CONFIG_INFO, NULL, 0);
   }
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_INetwork(iname) \
   INHERIT_IQI(iname); \
   int        (*SelectNetworkEx)(iname* po, int nNetwork, int16 nFamily); \
   int        (*OnEvent)(iname* po, int nEvent, PFNNETWORKEVENT pfn, void *pUser, boolean bRegister); \
   int        (*NetStatus)(iname* po, AEENetStatus *pns, AEENetStats *ps); \
   int        (*Start)(iname *po); \
   int        (*AddRetryCB)(iname *po, AEECallback* pcbRetry); \
   int        (*GetMyIPAddrs)(iname* po, struct IPAddr aipaAddr[], int* pNumAddr);\
   int        (*SetLinger)(iname* po, uint16 wSecs, uint16* pwldSecs); \
   int        (*SetDormancyTimeout)(iname* po, uint8 ucSecs); \
   int        (*SetOpt)(iname* po, int nOptName, void *pOptVal, int nOptSize); \
   int        (*GetOpt)(iname* po, int nOptName, void *pOptVal, int *pnOptSize); \
   int        (*GetLastNetDownReason)(iname* po, AEENetDownReason *pnReason); \
   int        (*IsQoSAware)(iname* po, boolean* pbIsQoSAware); \
                                                                                  \
   /* Extension Function to allow future enhancement to the interface without */  \
   /* breaking its binary compatibility.                                      */  \
   int        (*ExtFunc)(iname* po, AEECLSID id, void *pBuf, int nBufSize)

// declare the actual interface
AEEINTERFACE_DEFINE(INetwork);

//////////////////////
// INetwork methods
//////////////////////

static __inline uint32 INetwork_AddRef(INetwork *me)
{
   return AEEGETPVTBL(me,INetwork)->AddRef(me);
}

static __inline uint32 INetwork_Release(INetwork *me)
{
   return AEEGETPVTBL(me,INetwork)->Release(me);
}

static __inline int INetwork_QueryInterface(INetwork *me,
                                            AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,INetwork)->QueryInterface(me, cls, ppo);
}

static __inline int INetwork_SelectNetworkEx(INetwork *me, int nNetwork, int16 nFamily)
{
   return AEEGETPVTBL(me,INetwork)->SelectNetworkEx(me, nNetwork, nFamily);
}

static __inline int INetwork_OnEvent(INetwork *me, int nEvent, PFNNETWORKEVENT pfn, void *pUser, boolean bRegister)
{
   return AEEGETPVTBL(me,INetwork)->OnEvent(me, nEvent, pfn, pUser, bRegister);
}

static __inline int INetwork_NetStatus(INetwork *me, AEENetStatus *pns, AEENetStats *ps)
{
   return AEEGETPVTBL(me,INetwork)->NetStatus(me, pns, ps);
}

static __inline int INetwork_Start(INetwork *me)
{
   return AEEGETPVTBL(me,INetwork)->Start(me);
}

static __inline int INetwork_AddRetryCB(INetwork *me, AEECallback* pcbRetry)
{
   return AEEGETPVTBL(me,INetwork)->AddRetryCB(me, pcbRetry);
}

static __inline int INetwork_GetMyIPAddrs(INetwork *me, struct IPAddr aipaAddr[], int* pNumAddr)
{
   return AEEGETPVTBL(me,INetwork)->GetMyIPAddrs(me, aipaAddr, pNumAddr);
}

static __inline int INetwork_SetLinger(INetwork *me, uint16 wSecs, uint16* pwOldSecs)
{
   return AEEGETPVTBL(me,INetwork)->SetLinger(me, wSecs, pwOldSecs);
}

static __inline int INetwork_SetDormancyTimeout(INetwork *me, uint8 ucSecs)
{
   return AEEGETPVTBL(me,INetwork)->SetDormancyTimeout(me, ucSecs);
}

static __inline int INetwork_SetOpt(INetwork *me, int nOptName, void *pOptVal, int nOptSize)
{
   return AEEGETPVTBL(me,INetwork)->SetOpt(me, nOptName, pOptVal, nOptSize);
}

static __inline int INetwork_GetOpt(INetwork *me, int nOptName, void *pOptVal, int *pnOptSize)
{
   return AEEGETPVTBL(me,INetwork)->GetOpt(me, nOptName, pOptVal, pnOptSize);
}

static __inline int INetwork_GetLastNetDownReason(INetwork *me, AEENetDownReason *pnReason)
{
   return AEEGETPVTBL(me,INetwork)->GetLastNetDownReason(me, pnReason);
}

static __inline int INetwork_IsQoSAware(INetwork *me, boolean* pbIsQoSAware)
{
   return AEEGETPVTBL(me,INetwork)->IsQoSAware(me, pbIsQoSAware);
}

static __inline int INetwork_ExtFunc(INetwork *me, AEECLSID id, void *pBuf, int nBufSize)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me, id, pBuf, nBufSize);
}

static __inline int INetwork_GetSelectedNetwork(INetwork *me, int* pnNetwork)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me, INETWORK_EXT_FUNC_GET_SELECTED_NETWORK, pnNetwork, 0);
}

static __inline void INetwork_IgnoreTimeoutVals(INetwork *me, boolean bIgnore)
{
   AEEGETPVTBL(me,INetwork)->ExtFunc(me, INETWORK_EXT_FUNC_IGNORE_TIMEOUT_VALS, (void*)&bIgnore, 0);
}

static __inline int INetwork_Close(INetwork *me)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me, INETWORK_EXT_FUNC_CLOSE, NULL, 0);
}

static __inline int INetwork_GetSupportedQoSProfiles(INetwork *me, 
                                                     uint16 awQoSProfiles[], 
                                                     uint8 uNumProfiles,
                                                     uint8* puNumProfilesRequired)
{
   AEENetworkQoSProfiles qosProfiles;

   qosProfiles.awQoSProfiles = awQoSProfiles;
   qosProfiles.uNumProfiles = uNumProfiles;   
   qosProfiles.puNumProfilesRequired = puNumProfilesRequired;   
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me, 
                                            NETWORK_EXTFUNC_GET_SUPPORTED_QOS_PROFILES,
                                            &qosProfiles,
                                            sizeof(qosProfiles));
}

static __inline int INetwork_GetSipServerAddr(INetwork *me, 
                                              IPAddr aSipServerAddresses[], 
                                              uint8  uNumAddresses,
                                              uint8* puNumAddressesRequired)
{
   AEENetworkSipServerAddresses sipAddresses;

   sipAddresses.aSipServerAddresses = aSipServerAddresses;
   sipAddresses.uNumAddresses = uNumAddresses;
   sipAddresses.puNumAddressesRequired = puNumAddressesRequired;
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me,
                                            NETWORK_EXTFUNC_GET_SIP_SERVER_ADDRESSES,
                                            &sipAddresses,
                                            sizeof(sipAddresses));
}

static __inline int INetwork_GetSipServerDomainNames(INetwork*         me, 
                                                     AEENetDomainName  aSipServerDomainNames[], 
                                                     uint8             uNumNames,
                                                     uint8*            puNumNamesRequired)
{
   AEENetworkSipServerDomainNames sipDomainNames;

   sipDomainNames.aSipServerDomainNames = aSipServerDomainNames;
   sipDomainNames.uNumNames = uNumNames;
   sipDomainNames.puNumNamesRequired = puNumNamesRequired;   
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me,
                                            NETWORK_EXTFUNC_GET_SIP_SERVER_DOMAIN_NAMES,
                                            &sipDomainNames,
                                            sizeof(sipDomainNames));
}


static __inline int INetwork_GetBearerTechnology(INetwork*          me,
                                                 AEEBearerTechType* pAEEBearerTech)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me, 
                                            NETWORK_EXTFUNC_GET_BEARER_TECHNOLOGY,
                                            pAEEBearerTech,
                                            sizeof((*pAEEBearerTech)));
}

static __inline int INetwork_GetBearerTechnologyOpts(INetwork*            me,
                                                     IBearerTechnology**  ppBearerTechOpts)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me, 
                                             NETWORK_EXTFUNC_GET_BEARER_TECHNOLOGY_OPTS,
                                             ppBearerTechOpts,
                                             sizeof(*ppBearerTechOpts));
}

static __inline int INetwork_GenerateIPv6PrivAddr(INetwork* me,
                                                  boolean   bIsUnique)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me, 
                                            NETWORK_EXTFUNC_GENERATE_IPV6_PRIV_ADDR,
                                            &bIsUnique,
                                            sizeof(bIsUnique));
}

static __inline int INetwork_GetIPv6PrivAddrInfo(INetwork*            me,
                                                 AEEIPv6PrivAddrInfo* pPrivAddrInfo)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me,
                                            NETWORK_EXTFUNC_GET_PRIV_IPV6_ADDR_INFO,
                                            pPrivAddrInfo,
                                            sizeof(*pPrivAddrInfo));
}

static __inline int INetwork_GetOutageInfo(INetwork*      me,
                                           AEEOutageInfo* pOutageInfo)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me,
                                            NETWORK_EXTFUNC_GET_OUTAGE_INFO,
                                            pOutageInfo,
                                            sizeof(*pOutageInfo));
}

static __inline int INetwork_GetIPv6PrefixInfo(INetwork*           me,
                                               INAddr6*            pAddr,
                                               AEEIPv6PrefixInfo*  pPrefixInfo)
{
   int nRet;
   AEENetworkPrefixInfo AEEPrefixInfo;

   if ((NULL == pAddr) || (NULL == pPrefixInfo)) {
      return AEE_EBADPARM;
   }

   // copy the input parameter
   AEEPrefixInfo.addr = *pAddr;
   nRet = AEEGETPVTBL(me,INetwork)->ExtFunc(me,
                                            NETWORK_EXTFUNC_GET_IPV6_PREFIX_INFO,
                                            &AEEPrefixInfo,
                                            sizeof(AEEPrefixInfo));

   // copy the output parameter to the user's allocated space
   *pPrefixInfo = AEEPrefixInfo.prefixInfo;
   return nRet;
}

static __inline int INetwork_RefreshDHCPConfigInfo(INetwork* me)
{
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me,
      INETWORK_EXT_FUNC_REFRESH_DHCP_CONFIG_INFO,
      NULL,
      0);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEENetStatus

Description:
   This data type is an enumeration of the possible network statuses.

Definition:
   #define AEE_NET_STATUS_INVALID_STATE   0
   #define AEE_NET_STATUS_OPENING         1
   #define AEE_NET_STATUS_OPEN            2
   #define AEE_NET_STATUS_CLOSING         3
   #define AEE_NET_STATUS_CLOSED          4
   #define AEE_NET_STATUS_SLEEPING        5
   #define AEE_NET_STATUS_ASLEEP          6
   #define AEE_NET_STATUS_WAKING          7

   typedef int AEENetStatus;

Members:
   AEE_NET_STATUS_INVALID_STATE  : Network is in invalid state.
   AEE_NET_STATUS_OPENING        : Network connection is being established.
   AEE_NET_STATUS_OPEN           : Network connection is active.
   AEE_NET_STATUS_CLOSING        : Network connection is closing.
   AEE_NET_STATUS_CLOSED         : Network connection is inactive.
   AEE_NET_STATUS_SLEEPING       : Network connection is active, but related
                                   network resources (e.g. CDMA traffic channel)
                                   are being released.
   AEE_NET_STATUS_ASLEEP         : Network connection is active, but related
                                   network resources (e.g. CDMA traffic channel)
                                   have been released (dormant state).
   AEE_NET_STATUS_WAKING         : Network connection is active, but related
                                   network resources (e.g. CDMA traffic channel)
                                   are being re-acquired.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   INetwork_NetStatus()

=============================================================================

INETWORK_EXT_FUNC_GET_SELECTED_NETWORK

Description:
   Unique ID which is used as an input parameter to ExtFunc() for extending the 
   INetwork interface to support the function INetwork_GetSelectedNetwork().

Definition:
   #define INETWORK_EXT_FUNC_GET_SELECTED_NETWORK 0x010388da

Version:
   Introduced BREW Client 4.0

See Also:
   INetwork_GetSelectedNetwork()

=============================================================================

INETWORK_EXT_FUNC_IGNORE_TIMEOUT_VALS  

Description:
   Unique ID which is used as an input parameter to ExtFunc() for extending the 
   INetwork interface to support the function INetwork_IgnoreTimeoutVals().

Definition:
   #define INETWORK_EXT_FUNC_IGNORE_TIMEOUT_VALS 0x01039421

Version:
   Introduced BREW Client 4.0

See Also:
   INetwork_IgnoreTimeoutVals()

=============================================================================
INETWORK_EXT_FUNC_CLOSE

Description:
   Unique ID which is used as an input parameter to ExtFunc() for extending the 
   INetwork interface to support the function INetwork_Close().

Definition:
   #define INETWORK_EXT_FUNC_CLOSE 0x01046203

Version:
   Introduced BREW Client 4.0.1

See Also:
   INetwork_Close()

=============================================================================
PFNNETWORKEVENT

Description:
   This is the prototype for a Network event callback function. It is
   called when networking events occur after the application has registered for
   events using INetwork_OnEvent().

Definition:
  typedef void (*PFNNETWORKEVENT)(void * pUser, int nEvent);

Members:
   pUser : User-specific data passed to INetwork_OnEvent upon registration for
           the event specified by nEvent.
   nEvent: The type of event (NETWORK_EVENT_*) that triggered this callback.

Comments:
   NETWORK_EVENT_IP: The application can call INetwork_GetMyIPAddrs to get
                     the new IP address.~
   NETWORK_EVENT_STATE:The application can call INetwork_NetStatus to query the
                       new status of the data network connection for the network.~
   NETWORK_EVENT_QOS_AWARE_UNAWARE: The application can call INetwork_IsQoSAware
                     to query the new QoS network system type (Aware/Unaware).
   NETWORK_EVENT_IDLE: The network connection is open and idle - no open sockets exist.
   NETWORK_EVENT_MTPD: Mobile Terminated Packet Data (MTPD) notifications are intended
                       for clients that need to be notified about situations
                       where a network service (PUSH service) should be contacted.
                       When receiving the MTPD event, the client needs to continue 
                       the packet data session initialization by using sockets
                       for data communications.

                       For example, when a client receives the MTPD event, the client may:
                       - Create an ISockPort instance.
                       - Call ISockPort_SelectNetwork() with the same network type
                         selected by the INetwork instance.
                       - Call ISockPort_OpenEx() to open the socket.
                       - Connect to a specific network server by calling ISockPort_Connect().

                       Supported Networks:
                        Currently only the UMTS network.

   NETWORK_EVENT_IPV6_PRIVACY_ADDR: The application can call INetwork_GetIPv6PrivAddrInfo
                     to query the IPv6 private address information.
   NETWORK_EVENT_OUTAGE: The application can call INetwork_GetOutageInfo to query the
                     outage information.
   NETWORK_EVENT_QOS_PROFILES_CHANGED: The application can call INetwork_GetSupportedQoSProfiles
                     to query the QoS profiles that are now supported.
   NETWORK_EVENT_IPV6_PREFIX_UPDATE: The application can call INetwork_GetIPv6PrefixUpdateEvtInfo
                     to query the IPv6 Prefix Update event information.
   NETWORK_EVENT_BEARER_TECH_CHANGED: The application can call INetwork_GetBearerTechnology
                     to query the new bearer technology.

Version:
   Introduced BREW Client 3.1.5~
   NETWORK_EVENT_IDLE introduced in Brew Client 4.0~
   NETWORK_EVENT_MTPD introduced in Brew Client 4.0~
   NETWORK_EVENT_IPV6_PRIVACY_ADDR Introduced BREW Client 3.1.5 SP01~
   NETWORK_EVENT_OUTAGE Introduced BREW Client 3.1.5 SP01~
   NETWORK_EVENT_QOS_PROFILES_CHANGED Introduced BREW Client 3.1.5 SP01~
   NETWORK_EVENT_IPV6_PREFIX_UPDATE Introduced BREW Client 3.1.5 SP01~
   NETWORK_EVENT_BEARER_TECH_CHANGED introduced in Brew Client 3.1.5 SP01~

See Also:
   INetwork_OnEvent()

=======================================================================

AEERLP3Cfg

Description:
   The network layer of some devices may use RLP3 under PPP, which might be 
   able to be configured. This data type represents RLP3 configuration.
   Pass this struct as pOptVal to INetwork_GetOpt()/INetwork_SetOpt()

Definition:
   typedef struct AEERLP3Cfg
   {
      byte ucFwdNakRounds;
      byte aucFwdNaksPerRound[3];
      byte ucRevNakRounds;
      byte aucRevNaksPerRound[3];
   } AEERLP3Cfg;

Members:
   ucFwdNakRounds:      number forward NAK rounds (3 max)
   aucFwdNaksPerRound:  NAKs per round, forward
   ucRevNakRounds:      number reverse NAK rounds (3 max)
   aucRevNaksPerRound:  NAKs per round, reverse

Comments:
   - The Maximum NAK count can be 3 in any round. If it is greater than 3, 
     SetOpt returns AEE_NET_EBADOPTVAL
   - If NakRounds is less than 3, the RLP layer ignores the values in the 
     NaksPerRound that correspond to those extra rounds.~
     Example:
     if ucFwdNaksPerRound is set to 2, aucFwdNaksPerRound[2] is ignored
   - To set CUR RLP (current) option, the data connection must be active.
   - if NEG RLP values cannot be modified, SetOpt returns EBADPARM.

Version:
   Introduced BREW Client 2.0

See Also:
   INetwork_GetOpt()
   INetwork_SetOpt()

=======================================================================

AEEUDPUrgent

Description:
   The physical layer of some devices may support various modes of data 
   communication. This data type provides information about whether lower-latency 
   communication is possible when data network is asleep instead of waiting for complete
   wakeup in order to send a user datagram.
   Pass this struct as pOptVal to INetwork_GetOpt().

Definition:
   typedef struct AEEUDPUrgent
   {
      boolean bUrgentSupported;
      uint16  nUrgentLimit;
   } AEEUDPUrgent;

Members:
   bUrgentSupported: TRUE if data may be deliverable while data network is asleep,
                     FALSE if not
   nUrgentLimit: Maximum number of bytes of user data which may be sent per 
                 packet in this mode

Comments:
   The flag and limit are only advisory. It may be that data will end up 
   blocking for data network wakeup anyway even if bUrgentSupported is TRUE. Similarly, 
   the actual supported urgent payload limit may be smaller than offered, and 
   can be context and environment dependent.

Version:
   Introduced BREW Client 2.1

See Also:
   INetwork_GetOpt()

=======================================================================

AEENetStats

Description:
   This data type is used to deliver network connection information such as 
   data rate, active time, bytes sent, and other items.

Definition:
   typedef struct
   {
      uint32   dwOpenTime;
      uint32   dwActiveTime;
      uint32   dwBytes;
      uint32   dwRate;
      uint32   dwTotalOpenTime;
      uint32   dwTotalActiveTime;
      uint32   dwTotalBytes;
      uint32   dwTotalRate;
   } AEENetStats;

Members:
   dwOpenTime:
     Time in seconds since the network connection was established.
   dwActiveTime :
     Time in seconds that the network connection was actually active (with 
     open sockets).
   dwBytes:
     Total bytes sent on current network connection.
   dwRate:
     Rate of transfer on current network connection (bytes per second, equals 
     to dwBytes/dwActiveTime).
   dwTotalOpenTime:
     Time in seconds for all open network connections.
   dwTotalActiveTime:
     Time in seconds for all active network connections.
   dwTotalBytes:
     Total bytes sent (all connections).
   dwTotalRate:
     Total rate (all connections, equals to dwTotalBytes/dwTotalActiveTime).

Comments:
   None

Version:
  Introduced BREW Client 1.0

See Also:
   INetwork_NetStatus()

=======================================================================

INTERFACES DOCUMENTATION

=======================================================================

INetwork Interface

This interface provides API to control a BREW app's data network.

NOTE: The application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

INetwork_SelectNetworkEx() selects the network type (CDMA, UMTS, etc.) and the
address family (IPv4, IPv6) for the INetwork object.

INetwork_Start() and INetwork_AddRetryCB() are used for explicitly starting
(opening) a network connection. These APIs constitute together the means for
controlling "Network Start Transaction". Usually application will not need to
use these functions but rather will directly use a network resource such as
socket which will implicitly start the network.

INetwork_GetMyIPAddrs() returns the device’s own IP address(es).

INetwork_NetStatus() returns the current status (opening, open, closing, or
closed) of the device’s network connection and some statistics on
the current performance of the network subsystem.

INetwork_SetLinger() sets the linger time of the network connection. This is
the amount of time that the BREW AEE waits to terminate the network connection
after it is not needed anymore (the device’s last socket is closed, etc.). The
default value is 30 seconds.

INetwork_SetDormancyTimeout() sets the dormancy timeout of the network
connection. This is the amount of inactivity time that the device waits to bring
the network connection into dormant state . The default value is 30 seconds.

INetwork_SetOpt() and INetwork_GetOpt() are used for setting/getting certain
network options.

INetwork_OnEvent() is used to register the application for receiving network
events. This function is also used to cancel the registration.

INetwork_GetLastNetDownReason() returns the last network down reason.
Typically called by the application when the data network is CLOSED.

INetwork_IsQoSAware() returns whether or not the current system is
CDMA/WLAN QoS Aware system.

INetwork_GetSupportedQoSProfiles() returns the current 
QoS Profiles supported by the network

INetwork_GetSipServerAddr() returns the SIP (Session Initiation Protocol)
server IP addresses for this network.

INetwork_GetSipServerDomainNames() returns the SIP (Session Initiation Protocol)
server Domain Names for this network. 

INetwork_GetBearerTechnology() returns the bearer technology type for this network.

INetwork_GetBearerTechnologyOpts() returns the bearer technology options 
for this network.

INetwork_GenerateIPv6PrivAddr() requests to generate a private IPv6 address.

INetwork_GetIPv6PrivAddrInfo() returns the information regarding
the IPv6 private address.

INetwork_GetOutageInfo() returns the information associated with the
latest outage event associated with this data network.

INetwork_GetIPv6PrefixInfo() returns the information regarding an IPv6 Prefix
corresponding to a given address.

The following header file is required:~
   BREW 4.0 - AEEINetwork.h
   BREW 3.1 and prior - AEENetwork.h

=============================================================================

INetwork_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   INetwork_Release()

=============================================================================

INetwork_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   INetwork_AddRef()

==============================================================================

INetwork_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

=============================================================================

INetwork_SelectNetworkEx()

Description:
   This function selects a specific data network.

Prototype:
   int INetwork_SelectNetworkEx(INetwork *me, int nNetwork, int16 nFamily)

Parameters:
   me: the interface pointer
   nNetwork: data network type (AEE_NETWORK_*)
   nFamily: address family (AEE_AF_*)

Return Value:
   AEE_NET_SUCCESS: the data network was selected
   AEE_NET_EINVAL: the specified network (or network-family combination) is not valid
   AEE_NET_EOPNOTSUPP: the network has already been selected.
   AEE_EFAILED: other error.

   Other error codes are also possible.

Comments:
   When an INetwork instance is created, no network is selected.
   If an INetwork method that requires a network is called before
   INetwork_SelectNetworkEx(), AEE_NETWORK_DEFAULT and AEE_AF_INET will be
   implicitly selected, and the decision of which network to actually use is
   deferred to lower layers. Decision will be made upon connection creation,
   depending on the device and/or service provider.

   Once a network has been selected, either explicitly via
   INetwork_SelectNetworkEx(), or implicitly as described above, the network
   may not be changed. To use a different network, a new INetwork instance is
   required.

Version:
   Introduced BREW Client 3.1.4

See Also:
   Data networks

=============================================================================

INetwork_OnEvent()

Description:
    This function allows the caller to register to receive notifications of
    network status changes and events.
    A single callback function can be registered for each event type.
    The registration is in effect until overridden by a new registration for
    the same event type or canceled by a call to this function for the same
    event type with bRegister set to FALSE.

Prototype:
    int INetwork_OnEvent(INetwork *me, int nEvent, PFNNETWORKEVENT pfn, void *pUser, boolean bRegister)

Parameters:
    me:     The interface pointer.
    nEvent: Event to register for (NETWORK_EVENT_*).
    pfn:    User callback to be called when an event of type nEvent occurs.
    pUser:  User-specific data. This data shall be passed back to the application
            in the callback function.
    bRegister: TRUE to set up the registration. FALSE to cancel existing
               registration.

Return Value:
    SUCCESS: if registered
    EBADPARM: if nEvent is invalid or pfn is NULL
    ENOMEMORY: if allocation failure
    EALREADY: if already registered

    Other error codes are also possible

Comments:
    None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   PFNNETWORKEVENT

=============================================================================

INetwork_NetStatus()

Description:
   This function returns the current network status.
   If the AEENetStats pointer is valid, the buffer is filled with the current
   network connection information such as data rate, active time, bytes sent,
   and other items. The caller can view the performance of the current session
   and all sessions since the last time the device was reset.

Prototype:
   int INetwork_NetStatus(INetwork *me, AEENetStatus *pns, AEENetStats *ps)

Parameters:
   me: the interface pointer
   pns: returns the current network status. May be NULL.
   ps:  returns the network connection statistics. May be NULL.

Return Value:
   AEE_SUCCESS: network status was retrieved successfully.
   AEE_EFAILED: failed to retrieve network status.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.4

See Also:
   None

=============================================================================

INetwork_Start()

Description:
   This function is used to explicitly bring up a network connection.
   Usually applications will not need to use this function since network bringup
   shall be carried out implicitly when using a network resource such as
   socket.

   A transaction of bringing up the network connection is an asynchronous
   operation and so an AEE_NET_WOULDBLOCK may be returned. In that case,
   INetwork_Start() should be called again to obtain the final result (error
   or success). For notification of when to call INetwork_Start() again,
   the client should register a callback through INetwork_AddRetryCB(). Note
   that when the client's callback is resumed, it is not guaranteed that
   a subsequent call to INetwork_Start() will complete, so the client must
   be prepared to receive AEE_NET_WOULDBLOCK again.

   The client application must not initiate more than one "Network bring up"
   transactions simultaneously.
   If the client application requires more than one "Network bring up"
   transactions simultaneously it may use several INetwork instances, one
   per transaction.

Prototype:
   int INetwork_Start(INetwork *me)

Parameters:
   me: the interface pointer

Return Value:
   SUCCESS: network connection successfully brought up.
   AEE_NET_WOULDBLOCK: operation cannot be completed right now; use
                       INetwork_AddRetryCB() to try again later.
   ENOMEMORY: out of memory.
   AEE_NET_ENETNONET: network is not available.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.4

See Also:
   INetwork_AddRetryCB()

=============================================================================

INetwork_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   INetwork_Start() operation should be retried. It is used in conjunction
   with INetwork_Start() to control a "Network Bring up" transaction.

Prototype:
   int INetwork_AddRetryCB(INetwork *me, AEECallback* pcbRetry)

Parameters:
   me: the interface pointer
   pcbRetry: the client's callback.

Return Value:
   AEE_SUCCESS: client's callback was stored successfully.
   AEE_EBADPARM: pcbRetry is NULL.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.4

See Also:
   INetwork_Start()

=============================================================================

INetwork_GetMyIPAddrs()

Description:
   This function returns the IP addresses of the local host or device, in
   network byte order.

Prototype:
   int INetwork_GetMyIPAddrs(INetwork *me, struct IPAddr aipaAddr[], int* pNumAddr)

Parameters:
   me: the interface pointer
   aipaAddr: Caller allocated IPAddr array in which the IP addresses of the local host
             or device are returned.
             If aipaAddr is set to NULL, when the function returns,
             *pNumAddr will hold the actual number of addresses configured in the device.
   pNumAddr: [in] : The size (number of IPAddr entries) of aipaAddr.
                    If 0, The function returns in this parameter the number
                    of IP addresses currently configured on the device.
             Output: Number of IPAddr actually returned in aipaAddr.
                     If the user called the function with aipaAddr set to NULL,
                     *pNumAddr will hold the actual number of addresses configured in the device.

Return Value:
   AEE_NET_SUCCESS: Current IP address(es) returned in aipaAddr.
   AEE_NET_ENETNONET: network is not available.
   AEE_NET_ENETDOWN: Network subsystem is not available.
   AEE_NET_EBADPARM: *pNumAddr is negative.

   Other error codes are also possible.

Comments:
   Most devices return a maximum of one IP address.

Version:
   Introduced BREW Client 3.1.4

See Also:
   None

=============================================================================

INetwork_SetLinger()

Description:
   This function sets the linger time for the network connection to the value
   specified by wSecs, and returns the previous value in pwOldSecs.
   When the connection is created, the default linger time is set to 30 seconds.

Prototype:
   int INetwork_SetLinger(INetwork *me, uint16 wSecs, uint16* pwOldSecs)

Parameters:
   me: the interface pointer
   wSecs: new linger time in seconds.
   pwOldSecs: returns the previous linger time in seconds. May be NULL.

Return Value:
   AEE_SUCCESS: linger time was set successfully.
   AEE_EFAILED: operation failed.

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   None

=============================================================================

INetwork_SetDormancyTimeout()

Description:
   This function sets the IS-707 (cdma2000 1x) packet data dormancy timeout for
   the network connection to the value specified by ucSecs.
   When the connection is created, the default dormancy timeout is set to 30
   seconds.

   If no network traffic occurs during a period of time equals to the dormancy
   timeout then the mobile should perform mobile initiated dormancy.

Prototype:
   int INetwork_SetDormancyTimeout(INetwork *me, uint8 ucSecs)

Parameters:
   me: the interface pointer
   ucSecs: new dormancy timeout in seconds.

Return Value:
   AEE_NET_SUCCESS: dormancy timeout was set successfully.
   AEE_NET_EOPNOTSUPP: operation is not supported (i.e. not a cdma2000 1x network)

   Other error codes are also possible.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   None

=============================================================================

INetwork_SetOpt()

Description:
   This function provides the ability to configure certain network options. It
   is similar in paradigm to ISockPort_IOCtl(), but deals more with network-level
   items to be configured.

Prototype:
   int INetwork_SetOpt(INetwork *me, int nOptName, void *pOptVal, int nOptSize)

Parameters:
   me: the interface pointer
   nOptName: option to set
   pOptVal:  option-specific data to set
   nOptSize: size of the option-specific data pointed to by pOptVal

Return Value:
   AEE_SUCCESS: option was set successfully.
   AEE_EPRIVLEVEL: if access to the option is prohibited by applet privilege
   AEE_EUNSUPPORTED: if nOptName is unknown
   AEE_EBADPARM: if nOptSize is set to a size that does not correspond to the correct
             option data

   Other error codes are also possible, depending on the option to set.

Comments:
  Currently supported option ids are:
~
    RLP options: these options are highly device dependent, and as such may not
be able to be configured, available, or well-behaved. They are protected by the
system privilege level~
~
   INETWORK_OPT_DEF_RLP3~
    Default RLP settings, pOptVal must be an AEERLP3Cfg~

   INETWORK_OPT_CUR_RLP3~
    Current RLP settings, pOptVal must be an AEERLP3Cfg~

   INETWORK_OPT_PPP_AUTH~
    Set the system's configured PPP authentication credentials (protected by PL_SYSTEM).

pOptVal's type is char *, and the format of the inputted string must be: "userid\000passwd\000".~
*

    DDTM option: this option is highly device dependent, and as such may not
be able to be configured, available, or well-behaved. It is protected by the
system privilege level.~
~
   INETWORK_OPT_DDTM_PREF~
    Set the data dedicated transmission mode preference, pOptVal is a pointer to boolean
*

Version:
   Introduced BREW Client 3.1.4

See Also:
   None.

=============================================================================

INetwork_GetOpt()

Description:
   This function provides the ability to query certain network options. It is
   similar in paradigm to ISockPort_IOCtl(), but deals more with network-level
   configurations.

Prototype:
   int INetwork_GetOpt(INetwork *me, int nOptName, void *pOptVal, int *pnOptSize)

Parameters:
   me: the interface pointer
   nOptName: option to get
   pOptVal:  filled with option-specific data. May be NULL.
   pnOptSize: on input, if pOptVal is not NULL, specifies the size of the
             memory block pointed to by pOptVal. if pOptVal is NULL, it is
             ignored.
             on output, holds the size of the filled option-specific data
             pointed to by pOptVal.

Return Value:
   AEE_SUCCESS: option was retrieved successfully.
   AEE_EPRIVLEVEL: if access to the option is prohibited by applet privilege
   AEE_EUNSUPPORTED: if nOptName is unknown
   AEE_EBADPARM: if pOptVal is not NULL and pnOptSize is set to something smaller
                 than necessary to hold the option data.

   Other error codes are also possible, depending on the option to get.

Comments:
  Currently supported option ids are:

    -    INETWORK_OPT_DEF_RLP3: default RLP settings, pOptVal must be an AEERLP3Cfg
    -    INETWORK_OPT_CUR_RLP3: current RLP settings, pOptVal must be an AEERLP3Cfg
    -    INETWORK_OPT_NEG_RLP3: negotiated RLP settings, pOptVal must be an AEERLP3Cfg

~
   RLP options: these options are highly device dependent, and as such may not be able
to be configured, available, or well-behaved. They are protected by the system privilege
level.  Note that the Negotiated RLP settings are read-only, and that the Negotiated
number of forward/reverse NAK rounds may not be exceeded by the Default nor Current RLP
settings.  Note also that getting the Current and Negotiated RLP settings only make sense
during a data call.
*

    -    INETWORK_OPT_PPP_AUTH: retrieve the system's configured PPP authentication
credentials (protected by PL_SYSTEM). pOptVal's type is char *, and the format of
the returned string is: "userid\0passwd\0". If the passed-in buffer isn't large
enough, the data is truncated, and not null-terminated. pnOptSize is changed only
when pOptVal is NULL.

    -    INETWORK_OPT_UDP_URGENT: obtain info about out-of-band UDP sendto flag. 
pOptVal must be a pointer to AEEUDPUrgent struct. bUrgentSupported indicates whether 
urgent sendto may succeed. bUrgentLimit indicates largest payload which might succeed.
Note that applications must list NET_URGENT as an external dependency in their MIF
in order to utilize this option.

    -    INETWORK_OPT_DNS_SERVERS:
returns generic addresses (IPv4/IPv6) of the system's configured DNS
servers. pOptVal must be an array of IPAddr. pnOptSize on input indicates
the number of bytes allocated and pointed to by pOptVal. On output,
pnOptSize is set to the number of bytes filled in pOptVal.

    -    INETWORK_OPT_HW_ADDR: obtain the hardware address, pOptVal is a
pointer to an array of bytes.  The availability and format of
a hardware address is very device specific.  For example, on
a WLAN network, the hardware address is a six-byte MAC
address.  On a CDMA network, there is no hardware address.

Version:
   Introduced BREW Client 3.1.4

See Also:
   None.

=============================================================================

INetwork_GetLastNetDownReason()

Description:
   This function returns the last network down reason.
   Typically called by the application when the data network is CLOSED.

Prototype:
   int INetwork_GetLastNetDownReason(INetwork *me, AEENetDownReason *pnReason)

Parameters:
   me:         the interface pointer
   pnReason:   [out] - pointer to the last network down reason. On AEE_NET_SUCCESS
               will be filled with the reason value (pre-allocated by caller).

Return Value:
   AEE_NET_SUCCESS: Success. pnNetDownReason filled with last network down reason.
   AEE_NET_EFAULT:  Lower layer returned an unexpected network down reason.

   Other AEE designated error codes might be returned.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEENetDownReason

=============================================================================

INetwork_IsQoSAware()

Description:
   This function is supported only for CDMA and WLAN networks.
   It queries whether the mobile is currently on a system which supports
   QoS (Aware System) or on a system which does not support QoS (Unaware system).

Prototype:
   int INetwork_IsQoSAware(INetwork *me, boolean* pbIsQoSAware)

Parameters:
   me:           The interface pointer
   pbIsQoSAware: [out] - Upon SUCCESS return, this parameter is set to
                         TRUE if the mobile is on a QoS Aware system,
                         and is set to FALSE if the mobile is on a
                         QoS Unaware system,

Return Value:
   AEE_NET_SUCCESS: Success. The mobile is on a CDMA/WLAN network.
   pbIsQoSAware indicates whether the mobile is on a QoS Aware (TRUE)
   or QoS Unaware (FALSE) system.

   AEE_NET_EOPNOTSUPP: The mobile is on network which does not support the
   QoS Aware/Unaware notification mechanism.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   Other AEE designated error codes might be returned.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None.

=============================================================================

INetwork_GetSelectedNetwork()

Description:
   This function attempts to return the selected network type.

Prototype:
   int INetwork_GetSelectedNetwork(INetwork *me, int* pnNetwork)

Parameters:
   me:        The interface pointer
   pnNetwork: [out] - Upon SUCCESS return, this parameter will
                      contain the selected network type.

Return Value:
   AEE_NET_SUCCESS: There is a network selected, pnNetwork will
                    contain the selected network type.

   AEE_NET_EOPNOTSUPP: There is no network selected yet.

Comments:
   None.

Version:
   Introduced BREW Client 4.0

See Also:
   None.

=============================================================================

INetwork_IgnoreTimeoutVals()

Description:
   This function specifies whether or not the INetwork instance's linger and 
   dormancy timeouts should be ignored by the underlying data network.

   The underlying data network's linger and dormancy timeouts are based on the 
   timeouts of the INetworks which are linked to it.
   By Default, an INetwork instance's timeouts impact the underlying data 
   network's timeouts.
   If an INetwork instance does not wish to impact the underlying data 
   network's timeouts, then INetwork_IgnoreTimeoutVals() should be called using 
   the "ignore" flag set to TRUE. This will assure that the INetwork instance's 
   timeouts will be ignored. Similarly, the INetwork's timeouts can be set back 
   to be considered by the underlying data network, by calling this function 
   using the "ignore" flag set to FALSE.

Prototype:
   void INetwork_IgnoreTimeoutVals(INetwork *me, boolean bIgnore)

Parameters:
   me:        The interface pointer
   bIgnore:   TRUE  - set the network timeouts to be ignored by the underlying 
              data network.
              FALSE - set the network timeouts to be considered by the underlying 
              data network.

Return Value:
   None.

Comments:
   None.

Version:
   Introduced BREW Client 4.0

See Also:
   None.

=============================================================================

INetwork_GetSupportedQoSProfiles()

Description:
   This function returns the current QoS Profiles supported by the network. 
        
Prototype:
   int INetwork_GetSupportedQoSProfiles(INetwork *me, 
                                        uint16 awQoSProfiles[], 
                                        uint8 uNumProfiles,
                                        uint8* puNumProfilesRequired)
Parameters:
   me:            The interface pointer
   awQoSProfiles: [out] - Caller allocated array in which the supported QoS 
                  Profile IDs are returned
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
   
Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   None.
============================================================================

INetwork_Close()

Description:
   This function closes the data network associated with the INetwork object.

Prototype:
   int INetwork_Close(INetwork *me)

Parameters:
   me: The interface pointer

Return Value:
   AEE_NET_SUCCESS: Success. data network was successfully closed.

   Other AEE designated error codes might be returned.

Comments:
   None.

Version:
   Introduced BREW Client 4.0.1

See Also:
   INETWORK_EXT_FUNC_CLOSE

=============================================================================

INetwork_GetSipServerAddr()
    
Description:
   This function returns the SIP (Session Initiation Protocol) server IP
   addresses for this network. 
         
Prototype:
  int INetwork_GetSipServerAddr(INetwork *me, 
                                IPAddr aSipServerAddresses[], 
                                uint8  uNumAddresses,
                                uint8* puNumAddressesRequired)
        
Parameters:
   me:                     The interface pointer
   aSipServerAddresses:    [out] - Caller allocated array in which the SIP server
                           addresses are returned.
   uNumAddresses:          [in] - The size of aSipServerAddresses (number of IPAddr 
                           items it can hold).
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
   The max supported number of SIP server IP addresses is 20.
   
Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   INetwork_GetSipServerDomainNames
                    
=============================================================================

INetwork_GetSipServerDomainNames()
    
Description:
   This function returns the SIP (Session Initiation Protocol) server 
   Domain Names for this network. 
         
Prototype:
  int INetwork_GetSipServerDomainNames(INetwork*         me, 
                                       AEENetDomainName  aSipServerDomainNames[], 
                                       uint8             uNumNames,
                                       uint8*            puNumNamesRequired)
        
Parameters:
   me:            The interface pointer
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
   The max supported number of SIP server Domain Names is 20.
   
Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   AEENetDomainName
   INetwork_GetSipServerAddr

=============================================================================

INetwork_GetBearerTechnology()
    
Description:
   This function returns the bearer technology type for this network. 
         
Prototype:
  static int INetwork_GetBearerTechnology(INetwork*          me,
                                          AEEBearerTechType* pAEEBearerTech)
        
Parameters:
   me:              The interface pointer
   pAEEBearerTech:  [out] - Caller allocated struct, in which the bearer technology
                            information is returned.
  
Return Value:
   AEE_NET_SUCCESS: Success. pAEEBearerTech holds the bearer technology information.
   
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   Other AEE designated error codes might be returned.
              
Comments:
   This function is deprecated and is provided for backward compatibility only.
   Please use INETWORK_GetBearerTechnologyOpts() instead.

   Note that WLAN data network is not supported.

Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   AEEBearerTechType

=============================================================================

INetwork_GetBearerTechnologyOpts()
    
Description:
   This function returns the bearer technology options for this network. 

   The client is responsible for releasing the returned Bearer Technology
   Options object when it is not needed anymore.

Prototype:
   static int INetwork_GetBearerTechnologyOpts(INetwork*            me,
                                               IBearerTechnology**  ppBearerTechOpts)

Parameters:
   me:                [in]  - The interface pointer
   ppBearerTechOpts:  [out] - IBearerTechnology object, in which the bearer technology
                              information is returned.
  
Return Value:
   AEE_NET_SUCCESS: Success. pAEEBearerTech holds the bearer technology information.
   
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   Other AEE designated error codes might be returned.
              
Comments:
   Note that current targets typically do not support this operation for a WLAN
   data network.
   
Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   IBearerTechnology

=============================================================================

INetwork_GenerateIPv6PrivAddr()
    
Description:
   This function requests to generate a private IPv6 address.
   This API is part of functionality based on RFC 3041 - 
   "Privacy Extensions for Stateless Address Autoconfiguration in IPv6".

   Note that each INetwork instance can hold a single private address at a given moment.
   Each call to INetwork_GenerateIPv6PrivAddr() overwrites any information regarding the
   address associated with a previous INetwork_GenerateIPv6PrivAddr() request.
   Namely, the previous address will not be handled anymore, and no events regarding
   it will be forwarded to the application.
   The state of the address is always associated with the newest generation request.

   * Shared vs. Unique Address:
      The user can generate two types of private addresses: unique or shared.
      - With a shared address, it is possible that several applications will receive
        the same address upon generate() request. This might even be true for 
        applications that have selected different network types.
      - With a unique address, each generate() request will produce a different private address.

   * Address Generation Sequence:
      An application wishing to generate a private IPv6 address, should do the following:
      - Call INetwork_OnEvent(pMe->pINetwork, NETWORK_EVENT_IPV6_PRIVACY_ADDR, pfn, pUser, TRUE),
        to register for IPv6 Private Addresses events.
        
      - Call INetwork_GenerateIPv6PrivAddr() to generate the address.
         The address may be synchronously or asynchronously generated.

      - Check the return value of the function:
      a. If the function returned AEE_NET_SUCCESS, a valid address was generated. The application
         should call INetwork_GetIPv6PrivAddrInfo() to retrieve the newly generated address.
         Address state would now be VALID.
      b. If the function returned AEE_NET_EWOULDBLOCK: the address was not synchronously generated.
         The application should wait for a NETWORK_EVENT_IPV6_PRIVACY_ADDR event and then call
         INetwork_GetIPv6PrivAddrInfo() to get the information regarding the address.
         Until the address is generated, its state would be GEN_PENDING.
      c. If the function returned another error code, address generation failed. 
         Address state would now be GEN_FAILED.
      
      - As soon as the address is in valid state, the application should bind a socket to it.
        If no socket binds to the address within a certain time, the address will be deleted.

   * Address Deprecation:
      Private address may become deprecated. On deprecation, the application should cease using
      sockets bound to the deprecated address as soon as possible. If the application can not close
      the sockets immediately, it is allowed to continue using them for a while. However, as soon as
      possible, the application should close the sockets and generate a new address.
      Binding a new socket to the address after it became deprecated will fail.
      A deprecated address will be deleted when no socket is still bound to it.

   * Address Deletion:
      A private address will be deleted in the following cases:
      - No socket is being bound to the address within a certain time from its generation.
      - The address becomes deprecated and no socket is bound to it anymore.
      - The network goes down or an IPv6 prefix expiration occurs.

   * IPv6 Private Address States:
      A private IPv6 address may be in one of the following states:

      - AEE_IPV6_PRIV_ADDR_STATE_VALID: the address is valid, and sockets
        can be bound to it.

      - AEE_IPV6_PRIV_ADDR_STATE_DEPRECATED: the address has been deprecated.
        The application should close the sockets bound to the address, but can
        technically still keep using them. However, the application will fail
        to bind new sockets to the deprecated address. Once no sockets are
        bound to the deprecated address, it will be deleted.

      - AEE_IPV6_PRIV_ADDR_STATE_DELETED: the address was deleted.
        Any attempt to bind to a deleted address will fail.

      - AEE_IPV6_PRIV_ADDR_STATE_GEN_PENDING: An application has requested to generate
        an address, but the generation was not yet completed. When generation successfully completes,
        the application will receive a NETWORK_EVENT_IPV6_PRIVACY_ADDR event, and the state
        of the address will be AEE_IPV6_PRIV_ADDR_STATE_VALID.

      - AEE_IPV6_PRIV_ADDR_STATE_GEN_FAILED: Address generation was failed. 

   * Consecutive generation requests:
      Since each INetwork instance can hold a single private address at a given moment,
      generation request performed while the previous address is still valid, will overwrite
      any information regarding the address associated with a previous generate() request.
      Namely, the previous address will not be handled anymore, and no events regarding it will
      be forwarded to the application.
      The state of the address is always associated with the newest generation request.
   
Prototype:
  static int INetwork_GenerateIPv6PrivAddr(INetwork* me,
                                           boolean   bIsUnique)
        
Parameters:
   me:         The interface pointer
   bIsUnique:  TRUE if the user wishes to generate a unique address,
               FALSE if the user wishes to generate a shared address.

Return Value:
   AEE_NET_SUCCESS: Success. An IPv6 private address was successfully generated.
   
   AEE_NET_EOPNOTSUPP: The operation is not supported.

   AEE_NET_ENETNONET: Network subsystem unavailable.

   AEE_NET_EWOULDBLOCK: The address was not synchronously generated,
                        the current state of the address is
                        AEE_IPV6_PRIV_ADDR_STATE_GEN_PENDING.
                        The user should wait for an event and then get
                        the information regarding the address.

   EALREADY: An INetwork_GenerateIPv6PrivAddr() request is already in process.
             This error code will be returned if the user calls 
             INetwork_GenerateIPv6PrivAddr() while the state of the address 
             is AEE_IPV6_PRIV_ADDR_STATE_GEN_PENDING.

   Other AEE designated error codes might be returned.
              
Comments:
   None.
   
Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   INetwork_GetIPv6PrivAddrInfo

   =============================================================================

INetwork_GetIPv6PrivAddrInfo()
    
Description:
   This function returns the information associated with the
   private IPv6 address generated using this INetwork interface.
   The user can use this function to get the address information
   after calling INetwork_GenerateIPv6PrivAddr(), or after receiving
   an NETWORK_EVENT_IPV6_PRIVACY_ADDR event.
         
Prototype:
  static int INetwork_GetIPv6PrivAddrInfo(INetwork*            me,
                                          AEEIPv6PrivAddrInfo* pPrivAddrInfo)
        
Parameters:
   me:             The interface pointer.
   pPrivAddrInfo:  [out] - Caller allocated struct, in which the address info
                           is returned.
  

Return Value:
   AEE_NET_SUCCESS: Success. pAddrInfo holds the address info.
   
   AEE_NET_ERROR: No private address was ever generated using this INetwork object.

   Other AEE designated error codes might be returned.
              
Comments:
   None.
   
Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   INetwork_GenerateIPv6PrivAddr
   AEEIPv6PrivAddrInfo

=============================================================================

INetwork_GetOutageInfo()
    
Description:
   This function returns the information associated with the
   latest outage event associated with this data network.
   The user can use this function to get the event information
   after receiving a NETWORK_EVENT_OUTAGE event.
         
Prototype:
   static int INetwork_GetOutageInfo(INetwork*      me,
                                     AEEOutageInfo* pOutageInfo)       
Parameters:
   me:                 The interface pointer.
   pOutageInfo:        [out] - Caller allocated struct, in which the event info
                       is returned.

Return Value:
   AEE_NET_SUCCESS: Success. pOutageInfo holds the outage info.
   
   AEE_NET_ERROR: No event ever occurred, so there is no valid event info.

   Other AEE designated error codes might be returned.
              
Comments:
   None.
   
Version:
   Introduced BREW Client 3.1.5 SP01
                  
See Also:
   AEEOutageInfo

=============================================================================

INetwork_GetIPv6PrefixInfo()

Description:
   This function returns the information regarding an IPv6 Prefix
   corresponding to a given address.
         
Prototype:
   static int INetwork_GetIPv6PrefixInfo(INetwork*           me,
                                         INAddr6*            pAddr,
                                         AEEIPv6PrefixInfo*  pPrefixInfo
                                        )
 Parameters:
   me:            The interface pointer.
   pAddr:         [in] - The IPv6 address of interest.
   pPrefixInfo:   [out] - The information regarding the prefix used
                          to generate the address pointed by pAddr.
                          This struct should be pre-allocated by caller.

Return Value:
   AEE_NET_SUCCESS: Success. pPrefixInfo holds the prefix information.
   Note that in case the prefix corresponding to the address does not
   exist (either was removed or the user supplied an invalid address),
   the function will return AEE_NET_SUCCESS, but the prefix state
   will be AEE_IPV6_PREFIX_NOT_EXIST and the prefix length will be 0.
   
   Other AEE designated error codes might be returned.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEIPv6PrefixInfo

=============================================================================

INetwork_RefreshDHCPConfigInfo()

Description:
   This function asks the network to refresh the DHCP config info.
   a DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EV is sent when the operation
   is complete.

Prototype:
   int INetwork_RefreshDHCPConfigInfo(INetwork* me)

Parameters:
   me:  The interface pointer

Return Value:
   AEE_NET_SUCCESS: Success. The request was received successfully.

   Other AEE designated error codes might be returned.

Comments:
   None  

Version:
   Introduced BREW Client 4.0.2

See Also:
   None.

=====================================================================*/

#endif /* AEEINETWORK_H */
