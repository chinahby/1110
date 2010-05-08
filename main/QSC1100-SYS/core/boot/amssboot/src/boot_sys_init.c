/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  B O O T  S Y S T E M  I N I T I A L I Z A T I O N

GENERAL DESCRIPTION
  The system initialization related to iguana.

EXTERNALIZED FUNCTIONS
  __lib_init()

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/qsc6055/drivers/boot/boot_sys_init.c#1 $

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <l4/types.h>
#include <iguana/tls.h>
#include <iguana/thread.h>
#include <iguana/object.h>
#include <qmalloc/qmalloc.h>

/* add to environment */
#define MAX_ELF_SEGMENTS  1000
#define CLIST_MEMORY_SIZE (2 * sizeof(cap_t) * MAX_ELF_SEGMENTS)
#define IGUANA_GETENV_NAMING_SERVER (0x100)
struct clist;

extern void __naming_init(objref_t naming_server);
extern void __cap_init(struct clist *clist);

void __lib_init(uintptr_t *buf);

static void *iguana_obj_env;

/*============================================================================

  FUNCTION __LIB_INIT 

  DESCRIPTION
      Initialise all the iguana libraries for AMSS.

  FORMAL PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS

============================================================================*/
void __lib_init(uintptr_t *buf)
{
    char *heap_base = NULL;
    size_t heap_size = 0;

    iguana_obj_env = buf;

    __lib_iguana_init(iguana_obj_env);

    heap_base = (char *)iguana_getenv(IGUANA_GETENV_HEAP_BASE);
    heap_size = (size_t)iguana_getenv(IGUANA_GETENV_HEAP_SIZE);

    q_malloc_init_dynamic(heap_base, heap_base + heap_size - 1);
    __cap_init((struct clist *)iguana_getenv(IGUANA_GETENV_CLIST_BASE));

    q_malloc_morecore_init();

    __naming_init(iguana_getenv(IGUANA_GETENV_NAMING_SERVER));
    __tls_init_malloc();

} /* lib_init */

