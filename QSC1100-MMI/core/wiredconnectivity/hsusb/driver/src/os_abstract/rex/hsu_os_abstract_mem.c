/*==============================================================================

                 H S U _ O S _ A B S T R A C T _ M E M . C


GENERAL DESCRIPTION
  High-Speed USB OS abstraction layer memoty management API implementation.

EXTERNALIZED FUNCTIONS

  Externalized to outside of the OS abstraction layer:
    os_cache_flush
    os_cache_invalidate

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/06   ds      Initial version

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

/* AMSS include files. */
#ifndef T_REXNT
#include <cache_mmu.h>
#endif  /* T_REXNT */

/* High Speed USB subsystem include files. */
#include <jtypes.h>
#include <hsu_common.h>

/* OS abstraction layer include files. */
#include <port_mem.h>

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Externalized Variables
==============================================================================*/

/*==============================================================================
  File Static Variables
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================

  FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION OS_CACHE_FLUSH

DESCRIPTION
  Flush a memory region from the data cache to the system memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_cache_flush(void *addr, juint32_t size)
{
  HSU_USE_PARAM(addr);
  HSU_USE_PARAM(size);

#if 0 
  /* TODO  */
  #ifndef T_REXNT
  dcache_flush_region(addr, size);
  #else   /* T_REXNT */
#error code not present
  #endif  /* T_REXNT */
#endif /* if 0 */
}

/*===========================================================================
FUNCTION OS_CACHE_INVALIDATE

DESCRIPTION
  Invalidate a memory region from the data cache, so that further references
  to this memory region will produce an access to the system memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_cache_invalidate(void *addr, juint32_t size)
{
  HSU_USE_PARAM(addr);
  HSU_USE_PARAM(size);

#if 0 
  #ifndef T_REXNT
  dcache_inval_region(addr, size);
  #else   /* T_REXNT */
#error code not present
  #endif  /* T_REXNT */
#endif /* if 0 */
}
