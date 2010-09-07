#ifndef AEEINETUTILS_H
#define AEEINETUTILS_H
/*===========================================================================

FILE: AEEINetUtils.h

SERVICES:
   BREW Network Utilities Interface

DESCRIPTION:
   Interface definitions, data structures, etc. for BREW Network Utilities Interface

PUBLIC CLASSES:
   INetUtils

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None.

   Copyright (c) 2006-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
    INTERFACE ID
===========================================================================*/

#define AEEIID_INetUtils   0x010329cf

/*===========================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEEIQI.h"

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

//
// for INetUtils_GetPDPInfo
//
#define PDP_INFO_NAME_SIZE 32
typedef struct AEEPDPInfo
{
   boolean bValid;
   AECHAR  szName[PDP_INFO_NAME_SIZE+1];
} AEEPDPInfo;

//********************************************************************************************************************************
//
// INetUtils Interface
//
//********************************************************************************************************************************
#define INHERIT_INetUtils(iname) \
   INHERIT_IQI(iname); \
   int (*GetDefaultNetwork)(iname* me, int16 sFamily, int* pnNetwork); \
   int (*GetPDPCount)(iname* me, uint16* pwCount); \
   int (*GetPDPInfo)(iname* me, uint16 wProfile, AEEPDPInfo* pInfo); \
   /* Extension Function to allow future enhancement to the interface without */  \
   /* breaking its binary compatibility.                                      */  \
   int (*ExtFunc)(iname* me, AEECLSID id, void *pBuf, int nBufSize)

// declare the actual interface
AEEINTERFACE_DEFINE(INetUtils);

// INetUtils methods
static __inline uint32 INetUtils_AddRef(INetUtils *me)
{
   return AEEGETPVTBL(me,INetUtils)->AddRef(me);
}

static __inline uint32 INetUtils_Release(INetUtils *me)
{
   return AEEGETPVTBL(me,INetUtils)->Release(me);
}

static __inline int INetUtils_QueryInterface(INetUtils *me, 
                                             AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,INetUtils)->QueryInterface(me, cls, ppo);
}

static __inline int INetUtils_GetDefaultNetwork(INetUtils *me, int16 sFamily, int* pnNetwork)
{
   return AEEGETPVTBL(me,INetUtils)->GetDefaultNetwork(me, sFamily, pnNetwork);
}

static __inline int INetUtils_GetPDPCount(INetUtils *me, uint16* pwCount)
{
   return AEEGETPVTBL(me,INetUtils)->GetPDPCount(me, pwCount);
}

static __inline int INetUtils_GetPDPInfo(INetUtils *me, uint16 wProfile, AEEPDPInfo* pInfo)
{
   return AEEGETPVTBL(me,INetUtils)->GetPDPInfo(me, wProfile, pInfo);
}

static __inline int INetUtils_ExtFunc(INetUtils *me, AEECLSID id, void *pBuf, int nBufSize)
{
   return AEEGETPVTBL(me,INetUtils)->ExtFunc(me, id, pBuf, nBufSize);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEPDPInfo

Description:
   This data type is used with INetUtils_GetPDPInfo().

Definition:
   #define PDP_INFO_NAME_SIZE 32

   typedef struct AEEPDPInfo
   {
      boolean bValid;
      AECHAR  szName[PDP_INFO_NAME_SIZE+1];
   } AEEPDPInfo;

Members:
   bValid: TRUE if the profile is valid, FALSE otherwise.
   szName: null terminated name of the profile.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   INetUtils_GetPDPInfo()


=======================================================================
INTERFACES DOCUMENTATION
=======================================================================

INetUtils Interface.

This interface provides network related services that
are not associated with a specific network instance.

NOTE: Your application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

INetUtils_GetDefaultNetwork() returns the current default network type (CDMA, 
UMTS, etc.) of the device.

INetUtils_GetPDPCount() and INetUtils_GetPDPInfo() return information regarding
the configured Packet Data Profiles on the device.

The following header file is required:~
   BREW 4.0 - AEEINetUtils.h
   BREW 3.1 - AEENetUtils.h

=============================================================================

INetUtils_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   INetUtils_Release()

=============================================================================

INetUtils_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   INetUtils_AddRef()

==============================================================================

INetUtils_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

=============================================================================

INetUtils_GetDefaultNetwork()

Description:
   This function returns the default data network.

Prototype:
   int INetUtils_GetDefaultNetwork(INetUtils *me, int16 sFamily, int* pnNetwork)

Parameters:
   me: the interface pointer
   sFamily: the requested address family to be supported by the default network
   pnNetwork: filled with the default data network (AEE_NETWORK_*)

Return Value:
   AEE_NET_SUCCESS: the default data network was filled in
   AEE_EFAILED: operation failed.

   Other error codes are also possible.

Comments:
   Most applications will not need to get the default network.

   The default network may be the CDMA Service Network, the configured default
   UMTS Packet Data Profile or other network depending on the device and/or
   service provider.

   The specification of address family is required since the system may support
   two default networks, one for IPv4 and another one for IPv6.
   This is especially important in UMTS where the default network is associated
   with a specific Packet Data Profile. In this case the system is configured 
   with a default profile for IPv4 and another default profile for IPv6. 

Version:
   Introduced BREW Client 3.1.5

See Also:
   Data networks

=============================================================================

INetUtils_GetPDPCount()

Description:
   This function returns the number of Packet Data Profiles
   available on the device.

Prototype:
   int INetUtils_GetPDPCount(INetUtils *me, uint16* pwCount)

Parameters:
   me: the interface pointer
   pwCount: filled in with the PDP count

Return Value:
   AEE_NET_SUCCESS: if the count was filled in

   Other error codes are also possible.

Comments:
   In an UMTS device/network, there may be several networks configured and/or 
   available. The count of available networks may be determined at runtime 
   using this function. This is useful for enumerating the networks via 
   INetUtils_GetPDPInfo().

   Note that the available profiles are numbered consecutively from one to this
   count, although they may not all be valid.

Version:
   Introduced BREW Client 3.1.5

See Also:
   INetUtils_GetPDPInfo()

=============================================================================

INetUtils_GetPDPInfo()

Description:
   This function returns the profile information for a given Packet Data 
   Profile number.

Prototype:
   int INetUtils_GetPDPInfo(INetUtils *me, uint16 wProfile, AEEPDPInfo* pInfo)

Parameters:
   me: the interface pointer
   wProfile: the profile number of interest
   pInfo   : filled in with the profile information

Return Value:
   AEE_NET_SUCCESS: the information was filled in
   AEE_NET_EINVAL: the PDP number is not valid
   AEE_EFAILED: operation failed.

   Other error codes are also possible.

Comments:
   In an UMTS device/network, there may be several networks configured and/or 
   available. The count of available networks may be determined at runtime 
   using INetUtils_GetPDPCount(). This is useful for enumerating the networks 
   via this function.

   Note that the available profiles are numbered consecutively from one to this 
   count, although they may not all be valid.

Version:
   Introduced BREW Client 3.1.5

See Also:
   INetUtils_GetPDPCount()

=====================================================================*/

#endif    // AEEINETUTILS_H
