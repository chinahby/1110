#ifndef __AEEKEYSMAPPING_H
#define __AEEKEYSMAPPING_H

/*=============================================================================
        Copyright 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===============================================================================

DESCRIPTION: Header for the IKeysMapping interface

=============================================================================*/

#include "AEEQueryInterface.h"
#include "AEEInterface.h"

#define AEEIID_KEYSMAPPING 0x0105bb55


#define INHERIT_IKeysMapping( iname ) \
   INHERIT_IQueryInterface( iname ); \
   int (*GetMapping)(iname*, uint16 keyCode, uint32 modifiers, AECHAR *mapping)


AEEINTERFACE_DEFINE( IKeysMapping );

static __inline uint32 IKEYSMAPPING_AddRef(IKeysMapping *me)
{
   return AEEGETPVTBL(me,IKeysMapping)->AddRef(me);
}

static __inline uint32 IKEYSMAPPING_Release(IKeysMapping *me)
{
    return AEEGETPVTBL(me,IKeysMapping)->Release(me);
}

static __inline int IKEYSMAPPING_QueryInterface(IKeysMapping *me, AEECLSID clsid, void **ppo)
{
    return AEEGETPVTBL(me,IKeysMapping)->QueryInterface(me,clsid,ppo); 
}

static __inline int IKEYSMAPPING_GetMapping(IKeysMapping *me, uint16 avk, uint32 mods, AECHAR *pMapping)
{
   return AEEGETPVTBL(me,IKeysMapping)->GetMapping(me,avk,mods,pMapping);
}



/*
======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

IKeysMapping Interface

Description:
  This interface allows an application to query the meaning of a key 
  combination on extended keypads


The following header file is required:

AEEKeysMapping.h

=============================================================================

IKEYSMAPPING_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 


See Also:
    IKEYSMAPPING_Release()    

=======================================================================

IKEYSMAPPING_Release()

Description:
    This function is inherited from IBASE_Release()

See Also:
    IKEYSMAPPING_AddRef()

=======================================================================

IKEYSMAPPING_QueryInterface()

Description: 
	This function is inherited from IQI_QueryInterface()

See Also: 
    None

=======================================================================

IKEYSMAPPING_GetMapping()

Description: 
	This function returns the mapping of the an AVK code and 
	modifier combination to the intended character

Prototype:
   int IKEYSMAPPING_GetMapping(IKeysMapping *pIKeysMapping, uint16 avk, uint32 modifiers, AECHAR *mapping)

Parameters:

  pIKeysMapping: IKeysMapping interface object
  avk:           The key virtual code for which mapping is desired
  modifiers:     A bit mask of modifiers for which the mapping is desired (such as KB_LSHIFT) 
                 See AEEVCodes.h for a complete list of modifiers
  mapping [out]: On return, is set to the unicode value of the mapping for avk + modifiers
  
Return Value:
   AEE_SUCCESS  - Mapping found and successfully copied into "*mapping"
   EBADPARM     - if mapping is NULL
   EFAILED      - if avk + modifiers combination does not have a defined mapping

See Also:
   None

=======================================================================

*/



#endif /*__AEEKEYSMAPPING_H*/
