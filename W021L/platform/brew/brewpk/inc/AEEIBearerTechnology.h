#ifndef AEEIBEARERTECHNOLOGY_H
#define AEEIBEARERTECHNOLOGY_H
/*===========================================================================

FILE: AEEBearerTechnology.h

SERVICES: AEE Bearer Technology definitions

DESCRIPTION: Base level definitions, typedefs, etc. for AEE Bearer Technology

   Copyright (c) 2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

#include "AEEIQI.h"
#include "AEEIxOpts.h"

/*-------------------------------------------------------------------
   Defines and Type Declarations
-------------------------------------------------------------------*/

typedef xOpt AEEBearerTechnologyOpt;

//
// Network technology types
//
#define AEEBEARERTECHNOLOGY_CDMA 1
#define AEEBEARERTECHNOLOGY_UMTS 2
#define AEEBEARERTECHNOLOGY_WLAN 3
typedef uint32 AEEBEARERTECHNOLOGY_Network;

//
// CDMA sub-technologies.
//
#define AEEBEARERTECHNOLOGY_CDMA_1X              0x01
#define AEEBEARERTECHNOLOGY_CDMA_EVDO_REV0       0x02
#define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA       0x04
typedef uint32 AEEBEARERTECHNOLOGY_CDMATypeMask;

//
// CDMA service options.
//
#define AEEBEARERTECHNOLOGY_CDMA_1X_IS95         0x01
#define AEEBEARERTECHNOLOGY_CDMA_1X_IS2000       0x02
#define AEEBEARERTECHNOLOGY_CDMA_IS2000_REL_A    0x04
#define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_DPA   0x01
#define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_MFPA  0x02
#define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_EMPA  0x04
typedef uint32 AEEBEARERTECHNOLOGY_CDMAServiceOptionsMask;

//
// UMTS sub-technologies.
//
#define AEEBEARERTECHNOLOGY_UMTS_WCDMA         0x01
#define AEEBEARERTECHNOLOGY_UMTS_GPRS          0x02
#define AEEBEARERTECHNOLOGY_UMTS_HSDPA         0x04
#define AEEBEARERTECHNOLOGY_UMTS_HSUPA         0x08
#define AEEBEARERTECHNOLOGY_UMTS_EDGE          0x10
typedef uint32 AEEBEARERTECHNOLOGY_UMTSTypeMask;

//
// Defines to set various Bearer Technology Options
//

// For the following option IDs, pVals are 32-bit quantities, no need to copy
#define AEEBEARERTECHNOLOGYOPT_NETWORK              (XOPT_32BIT + 0)
#define AEEBEARERTECHNOLOGYOPT_CDMA_TYPE            (XOPT_32BIT + 1)
#define AEEBEARERTECHNOLOGYOPT_CDMA_SERVICE_OPTIONS (XOPT_32BIT + 2)
#define AEEBEARERTECHNOLOGYOPT_UMTS_TYPE            (XOPT_32BIT + 3)

#define AEEBEARERTECHNOLOGYOPT_END                  XOPT_END
#define AEEBEARERTECHNOLOGYOPT_ANY                  XOPT_ANY
#define AEEBEARERTECHNOLOGYOPT_COPYOPTS             XOPT_COPYOPTS
//**********************************************************************
//
// IBearerTechnology Interface
//
//**********************************************************************

#define AEEIID_IBearerTechnology    0x01046312

#define INHERIT_IBearerTechnology(iname) \
   INHERIT_IxOpts(iname); \
   int        (*ExtFunc)(iname* po, AEECLSID id, void *pBuf, int nBufSize)

AEEINTERFACE_DEFINE(IBearerTechnology);

////////////////////////////
// IBearerTechnology methods
////////////////////////////

static __inline uint32 IBearerTechnology_AddRef(IBearerTechnology *me)
{
   return AEEGETPVTBL(me,IBearerTechnology)->AddRef(me);
}

static __inline uint32 IBearerTechnology_Release(IBearerTechnology *me)
{
   return AEEGETPVTBL(me,IBearerTechnology)->Release(me);
}

static __inline int IBearerTechnology_QueryInterface(IBearerTechnology *me,
                                                     AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IBearerTechnology)->QueryInterface(me, cls, ppo);
}

static __inline int IBearerTechnology_AddOpt(IBearerTechnology *me,
                                             AEEBearerTechnologyOpt *abt)
{
   return AEEGETPVTBL(me,IBearerTechnology)->AddOpt(me, abt);
}

static __inline int IBearerTechnology_RemoveOpt(IBearerTechnology *me,
                                                int32 nOptId, int32 nIndex)
{
   return AEEGETPVTBL(me,IBearerTechnology)->RemoveOpt(me, nOptId, nIndex);
}

static __inline int IBearerTechnology_GetOpt(IBearerTechnology *me,  int32 nOptId,
                                             AEEBearerTechnologyOpt *pbt)
{
   return AEEGETPVTBL(me,IBearerTechnology)->GetOpt(me, nOptId, 0, pbt);
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEBEARERTECHNOLOGY_Network

Description:
  Specifies the network technology type.

Definition:

   #define AEEBEARERTECHNOLOGY_CDMA 1
   #define AEEBEARERTECHNOLOGY_UMTS 2
   #define AEEBEARERTECHNOLOGY_WLAN 3

   typedef uint32 AEEBEARERTECHNOLOGY_Network;

Members:
   AEEBEARERTECHNOLOGY_CDMA:
   CDMA network.

   AEEBEARERTECHNOLOGY_UMTS:
   UMTS network.

   AEEBEARERTECHNOLOGY_WLAN:
   WLAN network.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEBEARERTECHNOLOGY_CDMATypeMask

Description:
  CDMA sub-technologies. Masks are being used, since the phone
  may simultaneously support more than one technology.

Definition:

   #define AEEBEARERTECHNOLOGY_CDMA_1X              0x01
   #define AEEBEARERTECHNOLOGY_CDMA_EVDO_REV0       0x02
   #define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA       0x04

   typedef uint32 AEEBEARERTECHNOLOGY_CDMATypeMask;

Members:
   AEEBEARERTECHNOLOGY_CDMA_1X:         0x01
   AEEBEARERTECHNOLOGY_CDMA_EVDO_REV0:  0x02
   AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA:  0x04

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEBEARERTECHNOLOGY_CDMAServiceOptionsMask

Description:
  CDMA service options. Masks are being used, since the phone
  may simultaneously support more than one technology.

Definition:

   #define AEEBEARERTECHNOLOGY_CDMA_1X_IS95         0x01
   #define AEEBEARERTECHNOLOGY_CDMA_1X_IS2000       0x02
   #define AEEBEARERTECHNOLOGY_CDMA_IS2000_REL_A    0x04
   #define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_DPA   0x01
   #define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_MFPA  0x02
   #define AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_EMPA  0x04

   typedef uint32 AEEBEARERTECHNOLOGY_CDMAServiceOptionsMask;

Members:
   1x specific service options:
      AEEBEARERTECHNOLOGY_CDMA_1X_IS95:        0x01
      AEEBEARERTECHNOLOGY_CDMA_1X_IS2000:      0x02
      AEEBEARERTECHNOLOGY_CDMA_IS2000_REL_A:   0x04

   EVDO Rev A service options:
      AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_DPA:  0x01
      AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_MFPA: 0x02
      AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_EMPA: 0x04

Comments:
   DPA service option: Default Packet Application
   MFPA service option: MultiFlow Packet Application
   EMPA service option: Enhanced Multiflow Packet Application

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEBEARERTECHNOLOGY_UMTSTypeMask

Description:
  UMTS sub-technologies. Masks are being used, since the phone
  may simultaneously support more than one technology.

Definition:

   #define AEEBEARERTECHNOLOGY_UMTS_WCDMA         0x01
   #define AEEBEARERTECHNOLOGY_UMTS_GPRS          0x02
   #define AEEBEARERTECHNOLOGY_UMTS_HSDPA         0x04
   #define AEEBEARERTECHNOLOGY_UMTS_HSUPA         0x08
   #define AEEBEARERTECHNOLOGY_UMTS_EDGE          0x10

   typedef uint32 AEEBEARERTECHNOLOGY_UMTSTypeMask;

Members:
   AEEBEARERTECHNOLOGY_UMTS_WCDMA:   0x01
   AEEBEARERTECHNOLOGY_UMTS_GPRS:    0x02
   AEEBEARERTECHNOLOGY_UMTS_HSDPA:   0x04
   AEEBEARERTECHNOLOGY_UMTS_HSUPA:   0x08
   AEEBEARERTECHNOLOGY_UMTS_EDGE:    0x10

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEBearerTechnologyOpt

Description:
   Holds an (id, value) pair for AEEBearerTechnology.

Definition:
   typedef xOpt AEEBearerTechnologyOpt;

Members:
   int32  nId: option id, chosen from one of the Bearer Technology options
   void *pVal: option value, depends on nId

Comments:
   AEEBearerTechnologyOpt structures can hold various parameters of an
   AEEBearerTechnology.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEBEARERTECHNOLOGYOPT_

Description:
   The prefix AEEBEARERTECHNOLOGYOPT_ is used for Bearer Technology options.
   Bearer Technology options specify the various bearer technology
   attributes that can be set.

Definition:
   #define AEEBEARERTECHNOLOGYOPT_NETWORK              (XOPT_32BIT + 0)
   #define AEEBEARERTECHNOLOGYOPT_CDMA_TYPE            (XOPT_32BIT + 1)
   #define AEEBEARERTECHNOLOGYOPT_CDMA_SERVICE_OPTIONS (XOPT_32BIT + 2)
   #define AEEBEARERTECHNOLOGYOPT_UMTS_TYPE            (XOPT_32BIT + 3)

   #define AEEBEARERTECHNOLOGYOPT_END                  XOPT_END
   #define AEEBEARERTECHNOLOGYOPT_ANY                  XOPT_ANY
   #define AEEBEARERTECHNOLOGYOPT_COPYOPTS             XOPT_COPYOPTS

Members:
   AEEBEARERTECHNOLOGYOPT_NETWORK:
   Network type option (see AEEBEARERTECHNOLOGY_Network)

   AEEBEARERTECHNOLOGYOPT_CDMA_TYPE:
   CDMA type option (see AEEBEARERTECHNOLOGY_CDMATypeMask)

   AEEBEARERTECHNOLOGYOPT_CDMA_SERVICE_OPTIONS:
   CDMA Service Options option (see AEEBEARERTECHNOLOGY_CDMAServiceOptionsMask)

   AEEBEARERTECHNOLOGYOPT_UMTS_TYPE:
   UMTS type option (see AEEBEARERTECHNOLOGY_UMTSTypeMask)

Comments:
   1. Applications must not specify an option more then once.
      AddOpt() will fail for an option that already exists in the
      IBearerTechnology.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================*/

/*=====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IBearerTechnology

Description:
   The IBearerTechnology interface encapsulates the current Bearer Technology
   specification in which the various options are the technology's attributes.

=======================================================================

Function: IBearerTechnology_AddRef()

Description:
   This function is inherited from IxOPTS_AddRef().

=======================================================================

Function: IBearerTechnology_Release()

Description:
   This function is inherited from IxOPTS_Release().

=======================================================================

Function: IBearerTechnology_QueryInterface()

Description:
   This function is inherited from IxOPTS_QueryInterface().

=======================================================================

Function: IBearerTechnology_AddOpt()

Description:
  Append the list of AEEBearerTechnologyOpts to the current BearerTechnology option set

Prototype:

  int IBearerTechnology_AddOpt(IBearerTechnology *po, AEEBearerTechnologyOpt *apwoz)

Parameters:
  po: the interface pointer
  apwoz: array of AEEBearerTechnologyOpt structs, terminated by a AEEBearerTechnologyOpt with
         nId set to AEEBEARERTECHNOLOGYOPT_END

Return Value:
   AEE_SUCCESS: options were added successfully
   AEE_ENOMEMORY: could not allocate enough memory
   AEE_EBADITEM: incorrect item, for example returned when an option to be added has the same ID as
                 an existing option

Comments:
   IBearerTechnology holds copies of the structures in the apwoz array.
   Normally, the client does not need to use this function. It is provided for interface
   completeness only.

Side Effects:
   None.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None.

=======================================================================

Function: IBearerTechnology_RemoveOpt()

Description:
   Removes the AEEBearerTechnologyOpt matching nOptId

Prototype:

   int IBearerTechnology_RemoveOpt(IBearerTechnology *po, int32 nOptId)

Parameters:
   po:     the interface pointer
   nOptId: id of the option to remove from the options list

Return Value:
   AEE_SUCCESS: if the option was removed successfully
   AEE_EFAILED: if the specified AEEBearerTechnologyOpt couldn't be found

Comments:
   Normally, the client does not need to use this function. It is provided for interface
   completeness only.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================

Function: IBearerTechnology_GetOpt()

Description:
   Find an AEEBearerTechnologyOpt, in the list of AEEBearerTechnologyOpts in po, matching nOptId.
   pwo is filled in with a the matching AEEBearerTechnologyOpt contents.

Prototype:
   int IBearerTechnology_GetOpt(IBearerTechnology *po, int32 nOptId, AEEBearerTechnologyOpt *pwo)

Parameters:
   po :    the interface pointer
   nOptId: id of the option to find in the options list
   pwo:    where to stick the found AEEBearerTechnologyOpt

Return Value:
   AEE_SUCCESS: if the option was retrieved successfully
   AEE_EFAILED: if the specified AEEBearerTechnologyOpt couldn't be retrieved

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================*/

#endif  // AEEIBEARERTECHNOLOGY_H
