#ifndef _BTDEBUG_H
#define _BTDEBUG_H

#ifdef FEATURE_BT

/*===========================================================================

              B L U E T O O T H    D E B U G    H E A D E R

DESCRIPTION
  Contains common debug macros for the bluetooth drivers.

Copyright (c) 2000      by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btdebug.h_v   1.1   14 Jun 2002 14:38:12   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btdebug.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btdebug.h_v  $
 * 
 *    Rev 1.1   14 Jun 2002 14:38:12   waynelee
 * Changes BT's ASSERT_DEBUG to not fatal error.
 * 
 *    Rev 1.0   29 Jan 2001 17:06:12   propach
 * Initial revision.
 * 
 *    Rev 1.2   21 Dec 2000 17:38:44   waynelee
 * Removed T_CORE_EMUL
 * 
 *    Rev 1.1   19 Jul 2000 17:25:10   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:19:04   ryuen
 * Initial revision.

===========================================================================*/

#undef ASSERT_DEBUG_ON

#include "assert.h"

#ifdef ASSERT_DEBUG_ON
#error code not present
#else
  #define ASSERT_DEBUG( __cond__ )                          \
    {                                                       \
      if ( !( __cond__ ) )                                  \
      {                                                     \
        BT_ERR( "Assertion " #__cond__ " failed", 0, 0, 0 );  \
      }                                                     \
    }
#endif

#endif /* FEATURE_BT */

#endif /* _BTDEBUG_H */

