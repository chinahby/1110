#ifndef BOOT_READ_PAGES_H
#define BOOT_READ_PAGES_H

/*===========================================================================
                         boot_read_pages.h

DESCRIPTION
  This header file gives the defines  for the  functions to read pages
  from NAND.

Copyright(c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

 
/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800b/drivers/boot/boot_read_pages.h#1 $ $DateTime: 2006/07/20 13:49:43 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/04   ktr     Created.

===========================================================================*/

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS

=============================================================================*/


/*===========================================================================

FUNCTION boot_read_partial_pages

DESCRIPTION
  Reads all paged segments from BIB. If the start address of segment in ram is not 4k aligned
  copies the segment from start to the next 4k boundary.If the end of the segment
  is not 4k aligned copies the segment from last 4k boundary in segment to end.

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

int boot_read_partial_pages(void);


/*===========================================================================

FUNCTION boot_read_pages

DESCRIPTION
  Given a virtual address at which page fault occured this function identifies
  the SIZE(4k) block in ram containing the address, and copies it from NAND to ram.

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

int boot_read_pages
(
	uint32 virtual_address,
	uint32 size
);


#endif /* BOOT_READ_PAGES_H */











