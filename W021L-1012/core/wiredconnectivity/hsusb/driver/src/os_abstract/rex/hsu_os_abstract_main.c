/*==============================================================================

                  H S U _ O S _ A B S T R A C T _ M A I N . C


GENERAL DESCRIPTION
  High Speed USB stack REX OS abstraction layer main module.

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

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/os_abstract/rex/hsu_os_abstract_main.c#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
05/10/06  ds      Initial version

==============================================================================*/

/*==============================================================================

                      LOCAL LINT FLAGS

==============================================================================*/

/*lint +libh(port_sync.h)   Ignore lint errors in port_sync.h */


/*==============================================================================

                      INCLUDE FILES

==============================================================================*/

/* HSU subsystem include files */
#include <hsu_common.h>

/* USB stack include files. */
#include <jtypes.h>
#include <jerrno.h>
#include <port_sync.h>

/* OS abstraction layer internal include files */
#include "hsu_os_abstract_main.h"
#include "hsu_os_abstract_sync.h"
#include "hsu_os_abstract_bus.h"

#ifdef FEATURE_HSU_TEST
#error code not present
#endif

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

typedef struct
{
  boolean  os_abstract_is_initialized;
} os_main_file_static_vars_type;


/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Externalized Variables
==============================================================================*/

juint8_t jcurr_ctx;


/*==============================================================================
  File Static Variables
==============================================================================*/

static os_main_file_static_vars_type hsu_os_abstract_main = 
{
  FALSE  /* os_abstract_is_initialized */
};

static os_main_file_static_vars_type* const 
  file_static_vars_ptr = &hsu_os_abstract_main;


/*==============================================================================

                    FUNCTION DEFINITIONS

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
jresult_t hsu_os_init(void)
{
  jresult_t status;

  HSU_MSG_HIGH("hsu_os_init: Entry.",
               0, 0, 0);

  /* Initializing twice without an un-initialiazing in between
  ** (using os_uninit) is an error.
  */
  if (file_static_vars_ptr->os_abstract_is_initialized)
  {
    HSU_MSG_ERROR("hsu_os_init: Initializing twice consecutively is forbidden.",
                  0, 0, 0);

    return JEPERM;
  }

  #ifdef FEATURE_HSU_TEST
#error code not present
  #endif  /* FEATURE_HSU_TEST */

  jcurr_ctx = NORMAL_CONTEXT;

  for (;;)
  {
    status = hsu_os_sync_init();
    if (status != HSU_JSUCCESS)
    {
      break;
    }

    status = hsu_os_bus_init();
    if (status != HSU_JSUCCESS)
    {
      break;
    }

    break;
  }

  if (status != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR("hsu_os_init: Failed. status: %d", status, 0, 0);
    hsu_os_uninit();
  }
  else
  {
    file_static_vars_ptr->os_abstract_is_initialized = TRUE;
    HSU_MSG_HIGH("hsu_os_init: Exit.", 0, 0, 0);
  }

  return status;

} /* hsu_os_init */


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
void hsu_os_uninit(void)
{
  hsu_os_sync_uninit();
  hsu_os_bus_uninit();

  file_static_vars_ptr->os_abstract_is_initialized = FALSE;

  HSU_MSG_HIGH("hsu_os_uninit: Un-initialized High-Speed USB OS AL module.",
               0, 0, 0);

} /* hsu_os_uninit */


/* ==========================================================================
FUNCTION OS_PORT_INIT

DESCRIPTION
  Performs any required porting layer initialization.
  This function is called from the High-Speed USB stack's main 
  initialization function (jstart_stack()).

DEPENDENCIES
  None.  

RETURN VALUE
  Returns 0 on success, otherwise returns an error code.

SIDE EFFECTS
  None.
============================================================================= */
jresult_t os_port_init(void)
{
  return hsu_os_init();
} /* os_port_init */


/* ==========================================================================
FUNCTION OS_PORT_UNINIT

DESCRIPTION
  Performs any required porting layer un-initialization.
  This function is called from the High-Speed USB stack's main 
  un-initialization function (jstop_stack()).

DEPENDENCIES
  None.  

RETURN VALUE
  None.

SIDE EFFECTS
  None.
============================================================================= */
void os_port_uninit(void)
{
  hsu_os_uninit();
} /* os_port_uninit */
