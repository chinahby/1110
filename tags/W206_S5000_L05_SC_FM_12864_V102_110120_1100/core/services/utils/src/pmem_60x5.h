#ifndef PMEM_60X5_H
#define PMEM_60X5_H
/* =========================================================================

DESCRIPTION
  Target specific memory heap and allocation definitions

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The pmem_module_init() needs to be called before any other calls
  are made to the pmem system.

Copyright (c) 2006 by QUALCOMM Incorporated.  All Rights Reserved.
============================================================================ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc6055/services/utils/pmem_60x5.h#1 $ $DateTime: 2007/01/03 15:06:14 $ $Author: sbin $

when       who     what, where, why
--------   ---     --------------------------------------------------------
===========================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

/*============================================================================
                          DATA DECLARATIONS
============================================================================*/

/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

/*============================================================================
*                         EXPORTED FUNCTIONS
============================================================================*/

/*===========================================================================

FUNCTION pmem_6k_init

DESCRIPTION
  This function performs initialization of the heap region.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_6k_init(void);

#endif /* PMEM_60X5_H */
