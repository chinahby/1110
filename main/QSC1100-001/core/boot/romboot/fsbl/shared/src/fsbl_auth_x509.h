#ifndef FSBL_AUTH_X509_H
#define FSBL_AUTH_X509_H

/*===========================================================================

                  F A I L  S A F E  B o o t  L o a d e r
              X . 5 0 9  A u t h e n t i c a t i o n  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the FSBL X.509 authentication module, and declarations for
  the public functions in the module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_auth_x509.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_auth_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define FSBL_AUTH_X509_NAME "fsbl_auth_x509"
#define FSBL_AUTH_X509_VERSION 1


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  FSBL_AUTH_x509_INIT

DESCRIPTION
  Function to initialize an instance of the FSBL authentication module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boot_auth_if_type * fsbl_auth_x509_init( void );

#endif /* FSBL_AUTH_X509_H */

