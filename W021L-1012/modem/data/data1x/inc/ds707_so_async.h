#ifndef DS707_SO_ASYNC_H
#define DS707_SO_ASYNC_H
/*===========================================================================

                         D S 7 0 7 _ A S Y N C _ S O
DESCRIPTION
  This file contains functions related to configuring service options for
  IS-707 circuit switched data calls (async data).

EXTERNALIZED FUNCTIONS
  ds707_so_async_recal
    Re-does the async SO states, based on current settings.

  ds707_so_async_init
    Any power-up initialization that needs to be performed for
    async data service options should be done here.

  ds707_so_async_get_orig
    Returns the SO to originate with.

  ds707_so_async_so_is_async
     Returns TRUE if passed in SO is an async SO; else returns FALSE.

 Copyright (c) 2002-2009 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_async.h_v   1.2   14 Oct 2002 09:48:18   ajithp  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_so_async.h#2 $ $DateTime: 2009/01/27 05:42:02 $ $Author: nsivakum $
  
when       who    what, where, why
--------   ---    ----------------------------------------------------------
01/23/09   sn     Featurized code under FEATURE_ASYNC_DATA_NOOP.
10/14/02   atp    Cleanup.
10/10/02   ak     Updated for JCDMA M51/1X by making so_list public and
                  moving #define of number async SO's into .h.
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
extern const ds707_so_type ds707_so_async_list[];

#define DS707_SO_ASYNC_NUM   4


/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_ASYNC_RECAL
              
DESCRIPTION   Re-does the async SO states, based on current settings.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_async_recal(void);

/*===========================================================================
FUNCTION      DS707_SO_ASYNC_INIT
              
DESCRIPTION   Wrapper function for register_async_so() and 
              set_async_so_state() functions, in order to simplify the
              external interface. Any power-up initialization that needs to
              be performed for async data service options should be done here.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_async_init(void);

/*===========================================================================
FUNCTION       DS707_SO_ASYNC_GET_ORIG

DESCRIPTION    Returns the SO to orig with.

DEPENDENCIES   NONE

RETURN VALUE   Service option to originate with.

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_so_async_get_orig
(
  ds707_rateset_type  rateset
);

/*===========================================================================
FUNCTION       DS707_SO_ASYNC_SO_IS_ASYNC

DESCRIPTION    Returns TRUE if passed in SO is an async SO.  Else returns
               FALSE.

DEPENDENCIES   NONE

RETURN VALUE   See DESCRIPTION

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_so_async_so_is_async
(
  ds707_so_type    so
);
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */
#endif /* DS707_SO_ASYNC_H   */

