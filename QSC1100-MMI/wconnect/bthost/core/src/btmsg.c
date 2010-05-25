/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    M S G    M O D U L E

GENERAL DESCRIPTION
  This contains the Bluetooth Message Logging module

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 by QUALCOMM Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btmsg.c_v   1.3   03 Apr 2002 13:58:28   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btmsg.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btmsg.c_v  $
 * 
 *    Rev 1.4   27 Jun 2006           dsn
 * Changes for SOC support
 *
 *    Rev 1.3   03 Apr 2002 13:58:28   propach
 * Made use of BT_SWDEV_BNEP switch.
 * 
 *    Rev 1.2   08 Mar 2002 14:41:42   amitj
 * Changes to debug BNEP code
 * 
 *    Rev 1.1   16 Mar 2001 20:55:42   ryuen
 * Added a new entry to bt_msg_mode to support BA
 * 
 *    Rev 1.0   29 Jan 2001 17:08:34   propach
 * Initial revision.
 * 
 *    Rev 1.4   07 Dec 2000 09:57:50   waynelee
 * Removed compile warnings
 * 
 *    Rev 1.3   05 Sep 2000 16:45:54   waynelee
 * Added online, offline support.  Added debug MSGs.  Added Robustness.
 * 
 *    Rev 1.2   30 Aug 2000 09:42:12   waynelee
 * Made changes for new BT_MSG macros
 * 
 *    Rev 1.1   29 Aug 2000 08:25:36   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.0   25 Aug 2000 23:42:32   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT

#undef MSG_FILE     /* undef MSG_FILE to prevent compile warning for not
                    ** using msg_file in btmsg.h -> msg.h
                    */
#include "btmsg.h"

#define BT_MSG_LAYER  BT_MSG_GN   /* necessary for btmsg.h */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Message Level for each Bluetooth module */
uint8 bt_msg_mode[ BT_MSG_LAST ] =
{
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_GN */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_BB */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_LP */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_SE */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_LM */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_HC */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_RM */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_SC */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_L2 */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_SD */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_RC */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_SP */

#ifdef FEATURE_BT_AG
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_AG */
#else
  ( 0 ),
#endif /* FEATURE_BT_AG */

#ifdef FEATURE_BT_HS
#error code not present
#else
  ( 0 ),
#endif /* FEATURE_BT_HS */

  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_NA */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B ),  /* BT_MSG_AP */
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B )   /* BT_MSG_BA */

#ifdef FEATURE_BT_SOC
  ,
  ( BT_MSG_DEBUG_B | BT_MSG_SIG_B | BT_MSG_API_B )  /* BT_MSG_BNEP */
#endif /* FEATURE_BT_SOC */
};


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Static Functions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#endif /* FEATURE_BT */

