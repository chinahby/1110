#ifndef TMCNV_H
#define TMCNV_H

/*===========================================================================

         T A S K   M A I N   C O N T R O L L E R   N V   I N T E R F A C E

                          H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  the Task Main controller NV interface.

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/02   jqi     Initial Release

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "rex.h"                                        /* REX header file */
#include "nv.h"                                          /* NV header file */
#include "comdef.h"               /* Definition for basic types and macros */

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION TMCNV_INIT                                     EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes Task Main Controller NV-Interface

DEPENDENCIES
  Should be called before any other operations.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tmcnv_init(
  rex_sigs_type             nv_sig,
  rex_sigs_type             dog_sig,
  void                      (*dog_rpt_func_ptr) (void)  
);
/*===========================================================================

FUNCTION TMCNV_READ                                     EXTERNALIZED FUNCTION

DESCRIPTION
  Read a item from nv.

DEPENDENCIES
  tmcnv_init has been called before this function is used.

RETURN VALUE
  nv_stat_enum_type

SIDE EFFECTS
  The calling function is stopped until nv read operation is finised.

===========================================================================*/

nv_stat_enum_type tmcnv_read(
  nv_items_enum_type        nv_item,
  nv_item_type              *data_ptr  
);
/*===========================================================================

FUNCTION TMCNV_WRITE                                    EXTERNALIZED FUNCTION

DESCRIPTION
  Write a item to nv.

DEPENDENCIES
  tmcnv_init has been called before this function is used.

RETURN VALUE
  nv_stat_enum_type

SIDE EFFECTS
  The calling function is stopped until nv write operation is finised.

===========================================================================*/

nv_stat_enum_type tmcnv_write(
  nv_items_enum_type        nv_item,
  nv_item_type              *data_ptr    
);

#endif /* TMCNV_H */
