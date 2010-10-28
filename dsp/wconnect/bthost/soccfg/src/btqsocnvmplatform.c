/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   B T S   N V M   P L A T F O R M   S P E C I F I C   C O D E 

GENERAL DESCRIPTION
  This file implements Bluetooth Platform specific initialization code

Copyright (c) 2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: 
  $DateTime: 
  $Author: 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-02-06   sa  Cleanup & Added Support for FTM_BT feature for WM targets
  2008-09-16   sk  Initial version.


===============================================================================*/

#include "btqsocnvmplatform.h"
#include "btqsocnvm.h"
#include <ctype.h>
#include <stdlib.h>

#ifdef FEATURE_FTM_BT_MODEM_DUALPROC
#error code not present
#elif defined(UNDER_CE)
#include <windows.h>
#else /* UNDER_CE */
#include "btmsg.h"
#endif /* UNDER_CE */


/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/


/*===========================================================================
                                 Globals
===========================================================================*/

/*==========================================================================

  FUNCTION       btqsocnvmplatform_log_error

  DESCRIPTION    Logs error.

  DEPENDENCIES   None.

  PARAMETERS     btqsocnvm_parser_err_type

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
#ifdef FEATURE_FTM_BT_MODEM_DUALPROC
#error code not present
#elif defined(UNDER_CE)
void btqsocnvmplatform_log_error(btqsocnvm_parser_err_type parser_err)
{
  RETAILMSG(1,(TEXT("NVM File Error %d"), parser_err));
}

#else /* UNDER_CE */
void btqsocnvmplatform_log_error(btqsocnvm_parser_err_type parser_err)
{
  BT_MSG_HIGH( "NVM File Error %d", parser_err, 0, 0 );
}
#endif /* UNDER_CE */
