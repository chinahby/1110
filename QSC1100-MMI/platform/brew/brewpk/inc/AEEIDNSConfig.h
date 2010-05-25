#ifndef AEEIDNSCONFIG_H
#define AEEIDNSCONFIG_H
/*=============================================================================

FILE:          AEEIDNSConfig.h

SERVICES:      Configure a DNS

DESCRIPTION:   IDNSConfig provides generic methods to configure an IDNS.
               
===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
            
=============================================================================*/

#include "AEEIQI.h"
#include "AEENetAddrTypes.h"

//=============================================================================
//   IDNSConfig Interface
//=============================================================================
#define AEEIID_IDNSConfig      0x0101c8af

#define INHERIT_IDNSConfig(iname)\
   INHERIT_IQI(iname);\
   int   (*SelectNetwork) (iname* pi, int nNetwork);\
   int   (*SetServers)    (iname* pi, const INSockAddr* pServers, uint16 wCount); \
   int   (*GetServers)    (iname* pi, INSockAddr* pServers, uint16* pwCount)

AEEINTERFACE_DEFINE(IDNSConfig);

//=============================================================================
//   IDNSConfig methods
//=============================================================================
static __inline uint32 IDNSConfig_AddRef(IDNSConfig *me)
{
   return AEEGETPVTBL(me,IDNSConfig)->AddRef(me);
}

static __inline uint32 IDNSConfig_Release(IDNSConfig *me)
{
   return AEEGETPVTBL(me,IDNSConfig)->Release(me);
}

static __inline int IDNSConfig_QueryInterface(IDNSConfig* me, AEECLSID id, void** ppo)
{
   return AEEGETPVTBL(me,IDNSConfig)->QueryInterface(me, id, ppo);
}

static __inline int IDNSConfig_SelectNetwork(IDNSConfig* me, int nNetwork)
{
   return AEEGETPVTBL(me,IDNSConfig)->SelectNetwork(me, nNetwork);
}

static __inline int IDNSConfig_SetServers(IDNSConfig* me, const INSockAddr* pServers, uint16 cntServers)
{
   return AEEGETPVTBL(me,IDNSConfig)->SetServers(me, pServers, cntServers);
}

static __inline int IDNSConfig_GetServers(IDNSConfig* me, INSockAddr* pServers, uint16* pwCount)
{
   return AEEGETPVTBL(me,IDNSConfig)->GetServers(me, pServers, pwCount);
}

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IDNSConfig Interface

Description:
   The IDNSConfig Interface provides methods to configure an IDNS.  An
   instance of IDNSConfig may only be obtained via IDNS_QueryInterface();
   not via IEnv_CreateInstance().

   Use IDNSConfig_SelectNetwork() to select an AEE_NETWORK_* other than
   the default.

   Use IDNSConfig_SetServers() to specify DNS servers other than the
   default.

The following header file is required:~
   BREW 4.0 - AEEIDNS.h
   BREW 3.1 - AEEDNS.h

=============================================================================

IDNSConfig_AddRef()

Description:
    This function is inherited from IQI_AddRef().

=============================================================================

IDNSConfig_Release()

Description:
    This function is inherited from IQI_AddRef().

=============================================================================

IDNSConfig_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

=============================================================================

IDNSConfig_SelectNetwork()

Description:
    Selects a specific data network.

Prototype:
    int IDNSConfig_SelectNetwork
    (
      IDNSConfig *pIDNSConfig,
      int nNetwork
    );

Parameters:
    pIDNSConfig :  [in] pointer to the IDNSConfig Interface of interest
    nNetwork    :  [in] AEE_NETWORK_*

Return Value:
    AEE_NET_SUCCESS: if the data network was selected.
    AEE_NET_EINVAL: if the network is not valid.
    AEE_NET_EOPNOTSUPP: if the network has already been selected.
    AEE_EFAILED: otherwise

Comments:
    Most applications will not need to explicitly select the network.

    When an IDNS instance is created, no network is selected.  If
    an IDNS method that requires a network is called before
    IDNSConfig_SelectNetwork(), AEE_NETWORK_DEFAULT will be implicitly
    selected.

    AEE_NETWORK_DEFAULT corresponds to either CDMA Service Network
    -or- the configured default UMTS Packet Data Profile, depending on
    the device and/or service provider.

    Once a network has been selected, either explicitly via
    IDNSConfig_SelectNetwork(), or implicitly as described above, the
    network may not be changed.  To use a different network, a new
    IDNS instance is required.

Version:
   Introduced BREW Client 3.1.0

See Also:
   None

=============================================================================

IDNSConfig_SetServers()

Description:
    Specify DNS server(s) to query.

Prototype:
    int IDNSConfig_SetServers
    (
      IDNSConfig *pIDNSConfig,
      const INSockAddr* pServers,
      uint16 wCount
    );

Parameters:
    pIDNSConfig : [in] pointer to the IDNSConfig Interface of interest
    pServers    : [in] pointer to an array of wCount server addresses
    wCount      : [in] the number of servers in pServers

Return Value:
    AEE_NET_SUCCESS: if the servers were set
    AEE_EMEMPTR: if pServers is not a valid pointer
    AEE_EFAILED: otherwise

Comments:
    Most applications will not need to explicitly set the servers.
    When an IDNS instance is created, no servers are set.  If an IDNS
    method that requires servers is called before IDNSConfig_SetServers(),
    the default servers will be implicitly set.

    The implementation may only remember the first few servers.  Two
    servers are better than one, but three is only marginally better
    than two.

Version:
   Introduced BREW Client 3.1.0

See Also:
   INSockAddr

=============================================================================

IDNSConfig_GetServers()

Description:
    Retrieve the current DNS server list

Prototype:
    int IDNSConfig_GetServers
    (
      IDNSConfig *pIDNSConfig,
      INSockAddr* pServers,
      uint16*     pwCount
    );

Parameters:
    pIDNSConfig : [in] pointer to the IDNSConfig Interface of interest
    pServers    : [out] pointer to an array of *pwCount server addresses
    pwCount     : [in/out] on input, the max number of servers to may
                  be written to pServers; on output, the actual number
                  of servers that were written to pServers

Return Value:
    AEE_SUCCESS: if the server address was filled in
    AEE_EMEMPTR: if pServer is not a valid pointer
    AEE_EFAILED: otherwise

Comments:
    When a good response is received from a DNS server, that server is
    moved to the front of the server list, and will be the first used
    for the next query.  The order of the servers returned by this
    method will reflect this preferred order.

Version:
   Introduced BREW Client 3.1.0

See Also:
   INSockAddr

=====================================================================*/

#endif /* AEEIDNSCONFIG_H */
