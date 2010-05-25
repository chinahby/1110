#ifndef AEEIDEBUGSET_H
#define AEEIDEBUGSET_H

#include "AEEQueryInterface.h"

#define INHERIT_IDebugSet(iname) \
   INHERIT_IQueryInterface(iname); \
   int (*GetItem)(iname *, int, AECHAR **); \
   void (*OnItem)(iname *, int)

AEEINTERFACE_DEFINE(IDebugSet);

// The interface for a debug extension
#define AEEIID_DEBUGSET             0x010376cc


static __inline uint32 IDEBUGSET_AddRef(IDebugSet * pme)
{
   return AEEGETPVTBL(pme, IDebugSet)->AddRef(pme);
}

static __inline uint32 IDEBUGSET_Release(IDebugSet * pme)
{
   return AEEGETPVTBL(pme, IDebugSet)->Release(pme);
}

static __inline uint32 IDEBUGSET_QueryInterface(IDebugSet * pme, 
                                                AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(pme, IDebugSet)->QueryInterface(pme, cls, ppo);
}
   
static __inline int IDEBUGSET_GetItem(IDebugSet * pme, int nItemID, 
                                      AECHAR ** ppwsz)
{
   return AEEGETPVTBL(pme, IDebugSet)->GetItem(pme, nItemID, ppwsz);
}

static __inline void IDEBUGSET_OnItem(IDebugSet * pme, int nItemID)
{
   AEEGETPVTBL(pme, IDebugSet)->OnItem(pme, nItemID);
}

/*=====================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IDebugSet Interface

Description:
   Provides an interface to debugging information of AEE subsystems.

See Also:
   None

========================================================================

IDEBUGSET_AddRef()

Description:
    This function is inherited from IQI_AddRef().

See Also:
    IDEBUGSET_Release()

=======================================================================

IDEBUGSET_Release()

Description:
    This function is inherited from IQI_Release().

See Also:
    IDEBUGSET_AddRef()

=======================================================================

IDEBUGSET_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

See Also:
    None
=====================================================================*/
#endif /* AEEIDEBUGSET_H */

