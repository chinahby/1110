#ifndef AEENETUTILS2_H
#define AEENETUTILS2_H
/*===========================================================================

FILE: AEENetUtils2.h

SERVICES:
   BREW Network Utilities 2 Interface

DESCRIPTION:
   Interface definitions, data structures, etc. for BREW Network Utilities
   Interface

PUBLIC CLASSES:
   INetUtils2

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None.

             Copyright © 2008 QUALCOMM Incorporated.
                     All Rights Reserved.
             Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
    INTERFACE ID
===========================================================================*/

#define AEEIID_INETUTILS2 	  0x0106efcb

/*===========================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEEQueryInterface.h"

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

//*****************************************************************************
//
// INetUtils2 Interface
//
//*****************************************************************************

typedef struct INetUtils2 INetUtils2;

AEEINTERFACE(INetUtils2)
{
   INHERIT_IQueryInterface(INetUtils2);

   int (*GetAppProfileId)(INetUtils2 *me, uint32 uAppType, int16 *pnProfile);

   // Extension Function to allow future enhancement to the interface without 
   // breaking its binary compatibility.
   int (*ExtFunc)(INetUtils2 *me, AEECLSID id, void *pBuf, int nBufSize);
};

// INetUtils2 methods
static __inline uint32 INETUTILS2_AddRef(INetUtils2 *me)
{
   return AEEGETPVTBL(me,INetUtils2)->AddRef(me);
}

static __inline uint32 INETUTILS2_Release(INetUtils2 *me)
{
   return AEEGETPVTBL(me,INetUtils2)->Release(me);
}

static __inline int INETUTILS2_QueryInterface(INetUtils2 *me, AEECLSID cls,
                                              void** ppo)
{
   return AEEGETPVTBL(me,INetUtils2)->QueryInterface(me, cls, ppo);
}

static __inline int INETUTILS2_GetAppProfileId(INetUtils2 *me, uint32 uAppType,
                                               int16 *pnProfile) 
{
   return AEEGETPVTBL(me,INetUtils2)->GetAppProfileId(me, uAppType,
                                                      pnProfile); 
}

/*
=======================================================================
INTERFACES DOCUMENTATION
=======================================================================

INetUtils2 Interface

This interface provides network related services that
are not associated with a specific network instance.

INETUTILS2_GetAppProfileId returns the profile_id for the application type
given.  

The following header file is required:
AEENetUtils2.h

=============================================================================

INETUTILS2_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   INETUTILS2_Release()

=============================================================================

INETUTILS2_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   INETUTILS2_AddRef()

==============================================================================

INETUTILS2_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

=============================================================================
INETUTILS2_GetAppProfileId()

Description:
   This function returns the profile id for the application type

Prototype:
   int INETUTILS2_GetAppProfileId(INetUtils2 *me, uint32 uAppType, 
                                  int16 *pnProfile) 

Parameters:
   me: the interface pointer
   nAppType: Application type
   pnProfile: Profile id returned.

Return Value:
   SUCCESS: the profile id was filled in
   EFAILED: operation failed.

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:

=============================================================================

=====================================================================*/

#endif    // AEENETUTILS2_H

