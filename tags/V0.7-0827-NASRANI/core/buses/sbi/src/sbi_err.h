#ifndef SBII_ERR_H
#define SBII_ERR_H

/*===========================================================================

              Serial Bus Interface Error handler H E A D E R   F I L E

DESCRIPTION
  This module contains all the public constants, function declarations, macro 
  definitions and register declarations for the SBI modules.

Copyright (c) 1998-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/sbi.h_v   1.4   07 Jun 2002 12:54:54   ehandojo  $
  $Header: //depot/asic/msmshared/drivers/sbi/SSBI/sbi_err.h#1 $ $DateTime: 2005/03/28 12:17:20 $ $Author: ogunsaju $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/03   jcw     initial version

===========================================================================*/
#include "comdef.h"

/*===========================================================================

FUNCTION sbi_handle_runtime_error

DESCRIPTION
  This function handles runtime errors within the library.  It is external
  to the library so it can be customized as desired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void sbi_handle_runtime_error
(
    const char * err_string,
    uint32 parm0,
    uint32 parm1,
    uint32 parm2
);

/*===========================================================================

FUNCTION sbi_handle_runtime_warning

DESCRIPTION
  This function handles runtime errors within the library.  It is external
  to the library so it can be customized as desired. If this is called, then
  something unexpected has occurred in the library.  The driver will recover
  although some transactions may be lost.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void sbi_handle_runtime_warning
(
    const char * err_string,
    uint32 parm0,
    uint32 parm1,
    uint32 parm2
);

#endif  /* SBII_ERR_H */
