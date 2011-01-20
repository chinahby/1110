#ifndef AEENETWORK_H
#define AEENETWORK_H
/*=========================================================================

FILE: AEENetwork.h

SERVICES:
   INetwork interface

DESCRIPTION:
   This interface provides mechanisms for explicitly set/get parameters
   of a BREW application data network as well as means for explicitly starting
   the network.

===========================================================================

        Copyright © 2005-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

===========================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEE.h"
#include "AEENetworkTypes.h"
#include "AEEBearerTechnology.h"
#include "../../inc/AEEINetwork.h"

#define AEEIID_NETWORK AEEIID_INetwork

#define NETWORK_EVENT_PPP NETWORK_EVENT_STATE

#define INETWORK_AddRef                   INetwork_AddRef
#define INETWORK_Release                  INetwork_Release
#define INETWORK_QueryInterface           INetwork_QueryInterface
#define INETWORK_SelectNetworkEx          INetwork_SelectNetworkEx
#define INETWORK_OnEvent                  INetwork_OnEvent
#define INETWORK_NetStatus                INetwork_NetStatus
#define INETWORK_Start                    INetwork_Start
#define INETWORK_AddRetryCB               INetwork_AddRetryCB
#define INETWORK_GetMyIPAddrs             INetwork_GetMyIPAddrs
#define INETWORK_SetLinger                INetwork_SetLinger
#define INETWORK_SetDormancyTimeout       INetwork_SetDormancyTimeout
#define INETWORK_SetOpt                   INetwork_SetOpt
#define INETWORK_GetOpt                   INetwork_GetOpt
#define INETWORK_GetLastNetDownReason     INetwork_GetLastNetDownReason
#define INETWORK_IsQoSAware               INetwork_IsQoSAware
#define INETWORK_GetSelectedNetwork       INetwork_GetSelectedNetwork
#define INETWORK_IgnoreTimeoutVals        INetwork_IgnoreTimeoutVals
#define INETWORK_Close                    INetwork_Close
#define INETWORK_ExtFunc                  INetwork_ExtFunc
#define INETWORK_GetSupportedQoSProfiles  INetwork_GetSupportedQoSProfiles
#define INETWORK_GetSipServerAddr         INetwork_GetSipServerAddr
#define INETWORK_GetSipServerDomainNames  INetwork_GetSipServerDomainNames
#define INETWORK_GetBearerTechnology      INetwork_GetBearerTechnology
#define INETWORK_GetBearerTechnologyOpts  INetwork_GetBearerTechnologyOpts
#define INETWORK_GenerateIPv6PrivAddr     INetwork_GenerateIPv6PrivAddr
#define INETWORK_GetIPv6PrivAddrInfo      INetwork_GetIPv6PrivAddrInfo
#define INETWORK_GetOutageInfo            INetwork_GetOutageInfo
#define INETWORK_GetIPv6PrefixInfo        INetwork_GetIPv6PrefixInfo

#endif /* AEENETWORK_H */
