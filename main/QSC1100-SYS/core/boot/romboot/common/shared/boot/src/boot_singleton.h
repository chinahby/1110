#ifndef BOOT_SINGLETON_H
#define BOOT_SINGLETON_H

/*===========================================================================

                          B o o t  L o a d e r
                           S i n g l e t o n
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definitions for boot singletons.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_singleton.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_error_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/****************************************************************************

extern void prefix_register_singleton( prefix_type *singleton );

extern prefix_type *prefix_get_singleton( void );

extern boolean prefix_singleton_exists( void );

extern void prefix_lock_singleton( void );

****************************************************************************/


#define BOOT_SINGLETON_DECLARE(prefix) \
extern void prefix##_register_singleton( prefix##_type *singleton ); \
extern prefix##_type *prefix##_get_singleton( void ); \
extern boolean prefix##_singleton_exists( void ); \
extern void prefix##_lock_singleton( void )


/****************************************************************************

static prefix_type *prefix_singleton = NULL;
static boolean is_locked = FALSE;

void prefix_register_singleton( prefix_type *singleton )
{
  BL_VERIFY( ( !is_locked && singleton != NULL ), BL_ERR_NULL_PTR );

  perfix_singleton = singleton;
}

prefix_type *prefix_get_singleton( void )
{
  BL_VERIFY( prefix_singleton != NULL, BL_ERR_NULL_PTR );

  return prefix_singleton;
}

boolean prefix_singleton_exists( void )
{
  return ( prefix_singleton != NULL );
}

void prefix_lock_singleton( void )
{
  BL_VERIFY( prefix_singleton != NULL, BL_ERR_NULL_PTR );

  is_locked = TRUE;
}

****************************************************************************/

#define BOOT_SINGLETON_DEFINE(prefix) \
static prefix##_type *prefix##_singleton = NULL; \
static boolean prefix##_singleton_is_locked = FALSE; \
void prefix##_register_singleton( prefix##_type *singleton ) \
{ \
  BL_VERIFY( ( !prefix##_singleton_is_locked && singleton != NULL ), \
             BL_ERR_NULL_PTR ); \
  prefix##_singleton = singleton; \
} \
prefix##_type *prefix##_get_singleton( void ) \
{ \
  BL_VERIFY( prefix##_singleton != NULL, BL_ERR_NULL_PTR ); \
  return prefix##_singleton; \
} \
boolean prefix##_singleton_exists( void ) \
{ \
  return ( prefix##_singleton != NULL ); \
} \
void prefix##_lock_singleton( void ) \
{ \
  BL_VERIFY( prefix##_singleton != NULL, BL_ERR_NULL_PTR ); \
  prefix##_singleton_is_locked = TRUE; \
} \
extern prefix##_type *prefix##_get_singleton( void )


#endif /* BOOT_SINGLETON_H */

