#ifndef AEEIDNSCONFIG2_H
#define AEEIDNSCONFIG2_H
/*=============================================================================

FILE:          AEEIDNSConfig2.h

SERVICES:      Configure a DNS

DESCRIPTION:   IDNSConfig2 provides generic methods to configure an IDNS.

===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEIQI.h"
#include "AEENetAddrTypes.h"

//=============================================================================
//   IDNSConfig2 Interface
//=============================================================================
#define AEEIID_IDNSConfig2      0x0102fd28

//=============================================================================
//   IDNSConfig2 methods
//=============================================================================
#define INHERIT_IDNSConfig2(iname)\
   INHERIT_IQI(iname);\
   int   (*SelectNetworkEx) (iname* pi, int nNetwork, uint16 nFamily);\
   int   (*SetServers)      (iname* pi, const union AEESockAddrStorage* pServers, uint16 wCount); \
   int   (*GetServers)      (iname* pi, union AEESockAddrStorage* pServers, uint16* pwCount)

AEEINTERFACE_DEFINE(IDNSConfig2);

//=============================================================================
//   IDNSConfig2 methods
//=============================================================================
static __inline uint32 IDNSConfig2_AddRef(IDNSConfig2 *me)
{
   return AEEGETPVTBL(me,IDNSConfig2)->AddRef(me);
}

static __inline uint32 IDNSConfig2_Release(IDNSConfig2 *me)
{
   return AEEGETPVTBL(me,IDNSConfig2)->Release(me);
}

static __inline int IDNSConfig2_QueryInterface(IDNSConfig2* me, AEECLSID id, void** ppo)
{
   return AEEGETPVTBL(me,IDNSConfig2)->QueryInterface(me, id, ppo);
}

static __inline int IDNSConfig2_SelectNetworkEx(IDNSConfig2* me, int nNetwork, uint16 nFamily)
{
   return AEEGETPVTBL(me,IDNSConfig2)->SelectNetworkEx(me, nNetwork, nFamily);
}

static __inline int IDNSConfig2_SetServers(IDNSConfig2* me, const AEESockAddrStorage* pServers, uint16 cntServers)
{
   return AEEGETPVTBL(me,IDNSConfig2)->SetServers(me, pServers, cntServers);
}

static __inline int IDNSConfig2_GetServers(IDNSConfig2* me, AEESockAddrStorage* pServers, uint16* pwCount)
{
   return AEEGETPVTBL(me,IDNSConfig2)->GetServers(me, pServers, pwCount);
}

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IDNSConfig2 Interface

Description:
   This Interface deprecates IDNSConfig and provides generic methods to configure
   an IDNS.  An instance of IDNSConfig2 may only be obtained via
   IDNS_QueryInterface(), not via IEnv_CreateInstance().

   Use IDNSConfig2_SelectNetworkEx() to select a network (specific network type
   and address family) other than the default.

   Use IDNSConfig2_SetServers() and IDNSConfig2_GetServers() to specify and to
   query DNS servers other than the default.

The following header file is required:~
   BREW 4.0 - AEEIDNS.h
   BREW 3.1 - AEEDNS.h

=============================================================================

IDNSConfig2_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IDNSConfig2_Release()

=============================================================================

IDNSConfig2_Release()

Description:
    This function is inherited from IQI_AddRef().

See Also:
   IDNSConfig2_AddRef()

=============================================================================

IDNSConfig2_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

=============================================================================

IDNSConfig2_SelectNetworkEx()

Description:
   This function selects a specific data network.

Prototype:
   int IDNSConfig2_SelectNetworkEx(IDNSConfig2* me, int nNetwork, uint16 nFamily)

Parameters:
   me: the interface pointer
   nNetwork: data network type (AEE_NETWORK_*)
   nFamily: address family (AEE_AF_*)

Return Value:
   AEE_NET_SUCCESS: the data network was selected
   AEE_NET_EINVAL: the network is not valid
   AEE_NET_EOPNOTSUPP: the network has already been selected.
   AEE_EFAILED: other error.

   Other error codes are also possible.

Comments:
   Most applications will not need to explicitly select the network.

   When an IDNS instance is created, no network is selected. If an IDNS method
   that requires a network is called before IDNSConfig2_SelectNetworkEx(),
   AEE_NETWORK_DEFAULT and AEE_AF_INET will be implicitly selected, and the
   decision of which network to actually use is deferred to lower layers.
   Decision will be made upon connection creation, depending on the device
   and/or service provider.

   Once a network has been selected, either explicitly via
   IDNSConfig2_SelectNetworkEx(), or implicitly as described above, the network
   may not be changed. To use a different network, a new IDNS instance is
   required.

Version:
   Introduced BREW Client 3.1.4

See Also:
   Data networks
   Socket Address Family

=============================================================================

IDNSConfig2_SetServers()

Description:
   This function is used to specify DNS server(s) to query.

Prototype:
   int IDNSConfig2_SetServers
   (
      IDNSConfig2* me,
      const union AEESockAddrStorage* pServers,
      uint16 wCount
   );

Parameters:
   me: the interface pointer
   pServers: pointer to an array of wCount server addresses
   wCount:   the number of servers in pServers

Return Value:
   AEE_NET_SUCCESS: if the servers were set
   AEE_EMEMPTR: if pServers is not a valid pointer

Comments:
   None.

Version:
   Introduced BREW Client 3.1.4

See Also:
   None.

=============================================================================

IDNSConfig2_GetServers()

Description:
   This function is used to retrieve the current DNS server list

Prototype:
   int IDNSConfig2_GetServers
   (
      IDNSConfig2* me,
      union AEESockAddrStorage* pServers,
      uint16* pwCount
   )

Parameters:
   me: the interface pointer
   pServers: pointer to an array of *pwCount server addresses
   pwCount:  on input, the max number of servers that may be written to
             pServers. on output, the actual number of servers that were
             written to pServers.

Return Value:
   AEE_NET_SUCCESS: if the servers were set
   AEE_EMEMPTR: if one of pServers or pwCount is not a valid pointer

Comments:
   None.

Version:
   Introduced BREW Client 3.1.4

See Also:
   None.

=====================================================================*/

#endif /* AEEIDNSCONFIG2_H */
