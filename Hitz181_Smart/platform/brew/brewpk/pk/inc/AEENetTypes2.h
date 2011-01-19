#ifndef AEENETTYPES2_H
#define AEENETTYPES2_H
/*=============================================================================

FILE:         AEENetTypes2.h

SERVICES:     None

DESCRIPTION:  Common network types and definitions

===============================================================================
Copyright (c) 2008 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEENetTypes.h"

//
// Data networks
//
#define AEE_NETWORK_UW_FMC               (0x8000)

/*=====================================================================
DATA STRUCTURE DOCUMENTATION
=======================================================================*/

/*=======================================================================

Data networks

Description:
   Definitions of the known data networks. These definitions are used to
   select the network in operations such as INetwork_SelectNetwork().

Definition:
   #define AEE_NETWORK_UW_FMC               (0x8000)


Members:
   AEE_NETWORK_UW_FMC:  A proprietary solution for access to CDMA packet data 
                        through WLAN connectivity.

Comments:
	None

See Also:
   INetwork_SelectNetwork()
   INetwork_GetDefaultNetwork()
=====================================================================================*/

#endif  // AEENETTYPES_H
