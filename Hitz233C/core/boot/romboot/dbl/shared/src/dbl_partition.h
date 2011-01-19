#ifndef DBL_PARTITION_H
#define DBL_PARTITION_H

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

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_partition.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "dbl_target_accessor.h"
#include "miparti.h"
#include "dbl.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION  DBL_PARTITION_TABLE_SUPPORTED

DESCRIPTION
  Flash specific "find_partition_table" function that finds partition table
  in the given flash only if the the partition table is supported.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_partition_table_supported
(
  dbl_shared_data_type     *shared_data
);

/*===========================================================================

FUNCTION  DBL_PARTITION_LOAD_ENTRY

DESCRIPTION
  This function loads partition entry for partition table.

DEPENDENCIES
  This function will update the source field in image header.

RETURN VALUE
  None.
SIDE EFFECTS
  If failed, we will jump to dbl_error_handler to handle errors.

===========================================================================*/
extern void dbl_partition_load_entry
(
  flash_partable_t                     parti_tbl_ptr,    /* partition table         */
  dbl_flash_partition_image_info_type *shared_parti_ptr, /* image header pointer    */
  char                                *name              /* id to locate the entry 
                                                            of partition table      */
);

#endif  /* DBL_PARTITION_H */

