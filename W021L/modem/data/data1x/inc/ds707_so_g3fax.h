#ifndef DS707_SO_G3FAX_H
#define DS707_SO_G3FAX_H
/*===========================================================================

                          D S 7 0 7 _ G 3 F A X _ S O
GENERAL DESCRIPTION

 Copyright (c) 2002-2009 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_g3fax.h_v   1.2   14 Oct 2002 09:48:26   ajithp  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_so_g3fax.h#2 $ $DateTime: 2009/01/27 05:42:02 $ $Author: nsivakum $
  
when       who    what, where, why
--------   ---    ----------------------------------------------------------
01/23/09   sn     Featurized code under FEATURE_ASYNC_DATA_NOOP.
10/14/02   atp    Cleanup, added ds707_so_g3fax_so_is_g3fax().
10/10/02   ak     Updated for JCDMA M51/1X by making so_list public and
                  moving #define of number fax SO's into .h.
02/20/02   ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "ds707.h"

/*===========================================================================
                          TYPEDEFS and DECLARATIONS
===========================================================================*/
extern const ds707_so_type ds707_so_g3fax_list[];
#define DS707_SO_G3FAX_NUM   4


/*===========================================================================
                         EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_G3FAX_RECAL

DESCRIPTION   Called when the G3 FAX SO's need to be recalibrated for
              enabling/disabling with SNM, when some mobile setting has
              changed.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_g3fax_recal(void);

/*===========================================================================
FUNCTION      DS707_SO_G3FAX_INIT

DESCRIPTION   Wrapper function for register_g3fax_so() and set_g3fax_so
              state() functions, in order to simplify the external interface.
              Any power-up initialization that needs to be performed for
              Group 3 fax service options should be done here.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_g3fax_init(void);

/*===========================================================================
FUNCTION       DS707_SO_G3FAX_GET_ORIG

DESCRIPTION    Returns the SO to orig with.

DEPENDENCIES   NONE

RETURN VALUE   Service option to originate with.

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_so_g3fax_get_orig
(
  ds707_rateset_type  rateset
);

/*===========================================================================
FUNCTION       DS707_SO_G3FAX_SO_IS_G3FAX

DESCRIPTION    Returns TRUE if passed in SO is a g3fax SO.  Else returns
               FALSE.

DEPENDENCIES   NONE

RETURN VALUE   See DESCRIPTION

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_so_g3fax_so_is_g3fax
(
  ds707_so_type    so
);

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */
#endif /* DS707_SO_G3FAX_H   */

