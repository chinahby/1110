#ifndef CUSTCM_H
#define CUSTCM_H
/*===========================================================================

DESCRIPTION
  Configuration for CM

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc60x0/build/cust/custcm.h#3 $ $DateTime: 2006/12/06 14:24:22 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/06   mp      Removing FEATURE_UNIFORM_SCAN_OOS
11/22/06   mp      Adding FEATURE_UNIFORM_SCAN_OOS
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/* -----------------------------------------------------------------------
** Call Manager
** ----------------------------------------------------------------------- */
#define FEATURE_CM

/* Temporary feature for CM API changes.
*/
#define FEATURE_NEWCM

/* Enable CM Powerup-hold-origination feature to hold origination
** right after powerup giving service a chance to appear.
*/
#define FEATURE_CM_PWRUP_HOLD_ORIG

/* Enable CM Preferred-service-hold-origination feature to hold origination after
** powerup or mode preference change giving preferred service a chance
** to appears once sufficient service is available.
*/
#define FEATURE_CM_PREF_SRV_HOLD_ORIG

/* Enable CM Silent redial feature to silently redial failed calls or to ride through
** limited periods of no service.
*/
#define FEATURE_CM_SILENT_REDIAL

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#endif /* CUSTCM_H */

