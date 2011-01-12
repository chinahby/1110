#ifndef OSBL_AUTH_H
#define OSBL_AUTH_H

/*===========================================================================

                O S  S e c o n d a r y   B o o t   L o a d e r
              X . 5 0 9  A u t h e n t i c a t i o n  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the OSBL X.509 authentication module, and declarations for
  the public functions in the module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_auth.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/07   ly      Create
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "secboot_types.h"
#include "boot_auth_if.h"
#include "osbl.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define OSBL_AUTH_NAME "osbl_auth"
#define OSBL_AUTH_VERSION 1

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  OSBL_AUTH_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize an instance of the OSBL authentication module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_auth_init_and_register( void );


#endif /* OSBL_AUTH_H */

