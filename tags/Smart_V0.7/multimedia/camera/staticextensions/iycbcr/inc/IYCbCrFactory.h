#ifndef  IYCBCRFACTORY_H
#define  IYCBCRFACTORY_H

/*===============================================================================
FILE:          IYCBCRFactory.h

SERVICES:      Factory interface for instantiating IYCbCr classes

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2005 - 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEComdef.h"
#include "AEEShell.h"

#include "IYCbCr.h"

#include "YCBCRFactory.BID"

typedef struct IYCBCRFactory IYCBCRFactory;

AEEINTERFACE(IYCBCRFactory)
{
  INHERIT_IQueryInterface(IYCBCRFactory);
  int (*Create)(IYCBCRFactory *pMe, uint32 nScheme, uint32 cx, uint32 cy,
      IYCbCr **ppo);
};

/* Access macros for IYCBCRFactory */

#define  IYCBCRFactory_AddRef(p)   AEEGETPVTBL(p,IYCBCRFactory)->AddRef(p)
#define  IYCBCRFactory_Release(p)  AEEGETPVTBL(p,IYCBCRFactory)->Release(p)
#define  IYCBCRFactory_QueryInterface(p,i,p2) \
         AEEGETPVTBL(p,IYCBCRFactory)->QueryInterface((p),(i),(p2))
#define  IYCBCRFactory_Create(p, i, x, y, pp) \
         AEEGETPVTBL(p,IYCBCRFactory)->Create((p),(i),(x),(y),pp)

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IYCBCRFactory Interface

Description:
  This interface is the implementations of IYCbCrFactory class that can be
  used to create IYCbCr object.

  An application typically obtains an IYCBCRFactory pointer by calling
  IYCBCRFACTORY_QueryInterface() with the class ID AEECLSID_QUERYINTERFACE
  or AEECLSID_YCBCR. On success, an IYCBCRFactory pointer is returned,
  which must be released when the caller has finished using it.

See Also:
  IYCbCr.h

===============================================================================

IYCBCRFACTORY_AddRef()

Description:
  This function is inherited from IQI_AddRef(). It adds reference to
  the existing IYCBCRFactory object.

See Also:
  IYCBCRFACTORY_Release()

===============================================================================

IYCBCRFACTORY_Release()

Description:
  This function is inherited from IQI_Release(). It releases the IYCBCRFactory
  object and its members.

See Also:
  IYCBCRFACTORY_AddRef()

===============================================================================

IYCBCRFACTORY_QueryInterface()

Description:
  This function is inherited from IQI_QueryInterface(). An application typically
  obtains an IYCBCRFactory pointer by calling IYCBCRFACTORY_QueryInterface()
  with the class ID AEECLSID_QUERYINTERFACE or AEECLSID_YCBCR.

See Also:
  IYCBCRFACTORY_AddRef()

===============================================================================

IYCBCRFACTORY_Create

Description:
  Initiate the IYCbCr object information in the IYCBCRFactory object.

See Also:
  IYCbCr.h

===============================================================================
*/

#endif /* IYCBCRFACTORY_H */
