#ifndef CUSTMSUITE_H
#define CUSTMSUITE_H
/*===========================================================================

DESCRIPTION
  Configuration for Mobile Suite.

  Copyright (c) 2002,2007  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custmsuite.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/02   lz      Initial version.
===========================================================================*/

/*---------------------------------------------------------------------------
                         Wireless Messaging Services
---------------------------------------------------------------------------*/

/* Main feature flag
*/
//#define FEATURE_APP_MMS           /* Mobile Messaging Service Application */
#define FEATURE_APP_WAP           /* WAP Browser */
#define FEATURE_SMS_LISTENER      /* SMS Listener app */
/* Used by MobileSuite to determine if size_t is defined on the platform
 * it is ported to.
*/
#ifndef BUILD_JFLASH
#define _SIZE_T_DEFINED
#endif
#endif /* CUSTMSUITE_H */
