#ifndef OSBL_SHARED_SEG_H
#define OSBL_SHARED_SEG_H

/*===========================================================================

                 O S  Secondary  B o o t  L o a d e r
                      S h a r e d  S e g m e n t
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for OSBL
  shared segment support.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_shared_seg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/07   ly      Create
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "osbl.h"
#include "boot_comdef.h"
#include "boot_elf_loader_if.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_
#ifdef FEATURE_SHARED_SEGMENT
/*===========================================================================

FUNCTION  OSBL_INIT_SHARED_SEGMENT

DESCRIPTION
 This function will initialize the shared segment

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS

===========================================================================*/
extern void osbl_init_shared_segment( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION  OSBL_SAVE_SHARED_SEGMENT_INFO

DESCRIPTION
 This function call routines to save shared info before jumping to AMSS

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
extern void osbl_save_shared_segment_info
(
  bl_shared_data_type *bl_shared_data
);
#endif /* FEATURE_SHARED_SEGMENT */
#endif /* _ARM_ASM_ */

#endif  /* OSBL_SHARED_SEG_H */


