#ifndef BOOT_FSBL_IF_H
#define BOOT_FSBL_IF_H

/*===========================================================================

                  F a i l  S a f e   B o o t  L o a d e r
                     M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the FSBL.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_fsbl_if.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/31/07   anb     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#include "boot_auth_if.h"
#include "boot_hash_if.h"
#include "boot_dload_if.h"
#include "boot_fsbl_config_if.h"
#include "boot_headers.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#ifndef _ARM_ASM_

typedef struct boot_fsbl_if_type
{
  /*=========================================================================

  FUNCTION  FSBL_HASH_SHA1_INIT

  DESCRIPTION
    Function to initialize an instance of the FSBL hash module.  The flash_if
    parameter is optional and is used to connect the flash copy callback routine 
    with the flash access interface, to support incrementally hashing each block
    of data from flash as it is loaded.

  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the public hash interface.

  SIDE EFFECTS
    None

  ===========================================================================*/
  boot_hash_if_type* (*hash_sha1_init)
  (
    void
  );

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
  boot_auth_if_type* (*auth_x509_init)
  (
    void
  );

  /*=========================================================================

  FUNCTION  FSBL_DLOAD_INIT

  DESCRIPTION
    Function to initialize an instance of the FSBL download module.

  DEPENDENCIES
    None
  
  RETURN VALUE
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  boot_dload_if_type* (*dload_init)
  (
    void
  );

  /*=========================================================================

  FUNCTION  FSBL_CONFIG_INIT

  DESCRIPTION
    Function to initialize an instance of the FSBL configuration module.  

  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the public hash interface.

  SIDE EFFECTS
    None
  ===========================================================================*/
  boot_fsbl_config_if_type* (*config_init)
  (
    void
  );

  /*=========================================================================

  FUNCTION  FSBL_GET_MEMORY_RANGE

  DESCRIPTION
    Function to retreive the memory address range occupied by the FSBL.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void (*get_memory_range)
  (
    void   **base_addr,  /* Location to store the FSBL base address */
    uint32  *size        /* Location to store the FSBL memory size */
  );

/*=========================================================================

 FUNCTION  FSBL_ERROR_HANDLER_INIT

  DESCRIPTION
    Function to initialize the error handler to be used by FSBL

  DEPENDENCIES
    None
  
  RETURN VALUE
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void (*error_handler_init)
  (
    boot_error_if_type *err_if
  );

} boot_fsbl_if_type;


/*------------------------------------------------------------
**
**  FSBL Interface to DBL & OSBL
**
** ---------------------------------------------------------*/
typedef struct boot_fsbl_func_tbl
{
  /*=========================================================================

  FUNCTION INIT

  DESCRIPTION
    This function will initialize FSBL an update the pointer

  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE 
    FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*init)
  (
    boot_fsbl_if_type *fsbl_if,      /* Pointer to the public interface */
    boot_pbl_shared_data_type *pbl_shared_data
  );

} boot_fsbl_func_tbl;

#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#endif /* BOOT_FSBL_IF_H */


