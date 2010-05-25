#ifndef OS_ABSTRACT_MAIN_H
#define OS_ABSTRACT_MAIN_H

/*==============================================================================

GENERAL DESCRIPTION
  High-Speed USB stack and REX OS abstraction layer main module.

EXTERNALIZED FUNCTIONS

  Externalized to outside of the OS abstraction layer:
    hsu_os_init
      Initializes the OS abstraction layer.

    hsu_os_uninit
      Un-initializes the OS abstraction layer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/os_abstract/rex/hsu_os_abstract_main.h#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
05/10/06  ds      Initial version

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES

==============================================================================*/

/* HSU subsystem include files */
#include <hsu_common.h>

/* High-Speed USB stack include files. */
#include <jtypes.h>

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================

                    EXTERNALIZED FUNCTION DECLARATIONS

==============================================================================*/

/* ==========================================================================
FUNCTION HSU_OS_INIT

DESCRIPTION
   Initializes the REX OS abstraction layer.

DEPENDENCIES
   None.

RETURN VALUE
   If the initialization is successful, returns zero. Otherwise, returns 
   a non-zero error code.

SIDE EFFECTS
   None.
============================================================================= */
jresult_t hsu_os_init(void);


/* ==========================================================================
FUNCTION HSU_OS_UNINIT

DESCRIPTION
   Un-initializes the REX OS abstraction layer.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
============================================================================= */
void hsu_os_uninit(void);

#endif /* OS_ABSTRACT_MAIN_H */
