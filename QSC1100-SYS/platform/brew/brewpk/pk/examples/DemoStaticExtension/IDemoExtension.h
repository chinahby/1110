#ifndef IDEMOEXTENSION_H
#define IDEMOEXTENSION_H
/*=============================================================================

FILE:         IDemoExtension.h

SERVICES:     IDemoExtension

DESCRIPTION:  Base level definitions, typedefs, etc. for IDemoExtension
              interface

===============================================================================
        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEDisp.h"
#include "AEEQueryInterface.h"

// !!! THIS MUST BE CHANGED !!!
// Every interface must have a unique interface ID generated with the BREW
// Class ID Generator.  This must be changed whenever the interface changes.
#define AEEIID_DEMOEXTENSION   0x00000000


#define INHERIT_IDemoExtension(iname) \
   INHERIT_IQueryInterface(iname); \
	int (*DrawHello)(iname *p, AEEPoint ptXYOffset)

// declare the actual interface
AEEINTERFACE_DEFINE(IDemoExtension);

static __inline uint32 IDEMOEXTENSION_AddRef(IDemoExtension *me)
{
   return AEEGETPVTBL(me,IDemoExtension)->AddRef(me);
}

static __inline uint32 IDEMOEXTENSION_Release(IDemoExtension *me)
{
   return AEEGETPVTBL(me,IDemoExtension)->Release(me);
}

static __inline int IDEMOEXTENSION_QueryInterface(IDemoExtension *me, AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(me,IDemoExtension)->QueryInterface(me, cls, ppo);
}

static __inline int IDEMOEXTENSION_DrawHello(IDemoExtension *me, AEEPoint ptXYOffset)
{
   return AEEGETPVTBL(me,IDemoExtension)->DrawHello(me, ptXYOffset);
}


/*=============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IDemoExtension Interface

Description:
    A sample interface.

===============================================================================

IDEMOEXTENSION_AddRef()

Description:
    This function is inherited from IQI_AddRef().

See Also:
    IFOO_Release()

===============================================================================

IDEMOEXTENSION_Release()

Description:
    This function is inherited from IQI_Release().

See Also:
    IFOO_AddRef()

===============================================================================

IDEMOEXTENSION_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

===============================================================================

IDEMOEXTENSION_DrawHello()

Description:

Prototype:
   int IDEMOEXTENSION_DrawHello(IDemoExtension *pif, AEEPoint ptXYOffset);

Parameters:
   pif:        Pointer to IDemoExtension interface
   ptXYOffset: Location where text will be drawn

Return Value:
	SUCCESS for successful execution
   EFAILED for error

Comments:
   Caller must update the display.

Side Effects:
   None.

=============================================================================*/

#endif /* #ifndef IDEMOEXTENSION_H */
