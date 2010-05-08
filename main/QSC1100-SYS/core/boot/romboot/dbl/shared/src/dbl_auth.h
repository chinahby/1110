#ifndef DBL_AUTH_H
#define DBL_AUTH_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_auth.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "dbl_target_accessor.h"
#include "boot_fsbl_if.h"
#include "dbl.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================
  Authentication information structure
===========================================================================*/
typedef struct dbl_auth_info_type dbl_auth_info_type;
struct dbl_auth_info_type
{
  mi_boot_image_header_type *image_header;
  /* Following interface will get initialized when FSBL gets initialized */
  boot_hash_if_type         *hash_if;
  boot_auth_if_type         *auth_if;
};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  DBL_AUTH_INIT

DESCRIPTION
  This function initializes the authentication code with important
  information about authentication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_auth_init
(
  dbl_shared_data_type *shared_data   /* Shared data         */ 
);

/*===========================================================================

FUNCTION  DBL_AUTH_UPDATE_HASH_AUTH_IF

DESCRIPTION
  This function updates hash and authentication interface routines.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_auth_update_hash_auth_if
(
  const dbl_shared_data_type  *shared_data,   /* Shared data         */ 
  boot_hash_if_type     *hash_if,       
  boot_auth_if_type     *auth_if 
);


/*===========================================================================

FUNCTION  DBL_AUTH_QUEUE_IMAGE_FOR_AUTH

DESCRIPTION
  This function "queues" an image for authentication at a later time in the
  normal cold boot process. The "queuing" is simply to save information
  about the image to be authenticated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_auth_queue_image_for_auth
(
  dbl_auth_info_type        *auth_ptr,
  mi_boot_image_header_type *image_header
);

/*===========================================================================

FUNCTION  DBL_AUTH_IMAGE

DESCRIPTION
  This function authenticates the image that has been "queued" for
  authentication. The image is authenticated only if authentication is
  enabled. If authentication functions fail, then this function does
  not return and goes to the error handler.

  The image contains the following in the order given:
    DBL code
    signature
    certificate chain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_auth_image
(
  dbl_shared_data_type *shared_data   /* Shared data         */ 
);

/*===========================================================================

FUNCTION  DBL_AUTH_IMAGE

DESCRIPTION
  This function calculates hash of the image that has been "queued" for
  authentication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void dbl_auth_calc_hash
(
  const dbl_shared_data_type *shared_data,   /* Shared data         */ 
  uint8                      *digest_ptr
);

#endif  /* DBL_AUTH_H */

