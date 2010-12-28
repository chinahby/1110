#ifndef PROGRESSIVE_BOOT_BACKGROUND_LOAD_H
#define PROGRESSIVE_BOOT_BACKGROUND_LOAD_H

/*===========================================================================

DESCRIPTION
  This file contains the definitions and functions for L4 Progressive Boot
  Background loading in AMSS

Copyright(c) 2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

 
/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/06   tk     Created.

===========================================================================*/

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
 
/*===========================================================================
                      FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================

FUNCTION PROGRESSIVE_BOOT_BACKGROUND_LOAD_L4

DESCRIPTION
  This function Background loads Paged Segments in the Sleep task.

DEPENDENCIES
  None

RETURN VALUE
  None.

===========================================================================*/

void progressive_boot_background_load_l4(void);


#endif /* PROGRESSIVE_BOOT_BACKGROUND_LOAD_H */


