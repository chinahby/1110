#ifndef AEENETWORKTYPES_H
#define AEENETWORKTYPES_H
/*======================================================
FILE:  AEENetworkTypes.h

SERVICES:  BREW common network types

GENERAL DESCRIPTION:
	Common data structures, enums, etc. for the various networking interfaces.

   Copyright (c) 2005-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=====================================================*/

/* older includes must stay */
#include "AEEComdef.h"

/* new definition */
#include "../../inc/AEENetTypes.h"
#include "../../inc/AEENetAddrTypes.h"
#include "../../inc/AEESocketTypes.h"

#define UMTS_INFO_NAME_SIZE 32

//
// AEEDNSResult holds the result from INETMGR_GetHostByName
//
// AEEDNSMAXADDRS applies only to AEEDNSResult, meaning only to
// INETMGR_GetHostByName, meaning only for IPv4 results.
#define AEEDNSMAXADDRS  4

typedef struct
{
   int      nResult;
   INAddr   addrs[AEEDNSMAXADDRS];
} AEEDNSResult;

/*=====================================================================
DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEDNSResult

Description:
This data type holds the result of an INETMGR_GetHostByName() operation.

Definition:
#define AEEDNSMAXADDRS  4

typedef struct
{
   int      nResult;
   INAddr   addrs[AEEDNSMAXADDRS];
} AEEDNSResult;

Members:
nResult: From 1 to AEEDNSMAXADDRS that is the number of addresses 
         successfully retrieved. Any other value is a BREW error code.
addrs:   array of IPv4 addresses.

Comments:
         None

Version:
         Introduced BREW Client 2.0

See Also:
         INAddr
            INETMGR_GetHostByName()

=======================================================================
*/

#endif      // AEENETWORKTYPES_H
