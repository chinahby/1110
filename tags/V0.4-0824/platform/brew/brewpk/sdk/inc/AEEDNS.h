#ifndef _AEEDNS_H_
#define _AEEDNS_H_

/*======================================================
FILE:      AEEDNS.h

SERVICES:  Generalized DNS queries.

GENERAL DESCRIPTION:

  IDNS provides a flexible interface into the DNS client, allowing the caller
  to construct compound queries and examine all sections of the response
  record.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IDNS

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEQueryInterface.h"
#include "AEENet.h"
#include "AEENetworkTypes.h"

/* new definition */
#include "../../inc/AEEIDNS.h"
#include "../../inc/AEEIDNSConfig.h"
#include "../../inc/AEEIDNSConfig2.h"

#include "../../inc/AEEDNSConfig.bid"

#define AEECLSID_DNSCONFIG AEECLSID_DNSConfig

#define IDNSCONFIG_AddRef              IDNSConfig_AddRef
#define IDNSCONFIG_Release             IDNSConfig_Release
#define IDNSCONFIG_QueryInterface      IDNSConfig_QueryInterface
#define IDNSCONFIG_SelectNetwork       IDNSConfig_SelectNetwork
#define IDNSCONFIG_SetServers          IDNSConfig_SetServers
#define IDNSCONFIG_GetServers          IDNSConfig_GetServers

#define IDNSCONFIG2_AddRef             IDNSConfig2_AddRef
#define IDNSCONFIG2_Release            IDNSConfig2_Release
#define IDNSCONFIG2_QueryInterface     IDNSConfig2_QueryInterface
#define IDNSCONFIG2_SelectNetworkEx    IDNSConfig2_SelectNetworkEx
#define IDNSCONFIG2_SetServers         IDNSConfig2_SetServers
#define IDNSCONFIG2_GetServers         IDNSConfig2_GetServers

#endif /* _AEEDNS_H_ */

