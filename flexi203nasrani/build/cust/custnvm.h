#ifndef CUSTNVM_H
#define CUSTNVM_H
/*===========================================================================

DESCRIPTION
  Configuration for NVM.

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custnvm.h#1 $ $DateTime: 2008/03/06 14:38:01 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/02   jct     Created
===========================================================================*/

/* NV task will auto-detect the EEPROM size and automatically
** scale the roaming list size
*/
#undef  FEATURE_NV_AUTOSIZE

/* Phone's NV memory will support one NAM of 625 bytes, and the one-time
   subsidy lock item
*/
#define  FEATURE_NV_ONE_NAM_RL_LARGE

/* This feature configures NV to have two small roaming lists for a two NAM phone,
** as opposed to having one large roaming list for a one NAM phone.
*/
#undef  FEATURE_NV_TWO_NAMS_RL_SMALL

/* Define to enable the use of a trimode rf calibration file to derive
** NV items.  Also triggers the NV section to support this feature.
*/
#define FEATURE_TRIMODE_ITEMS

/* Enable reading of BTF value from NV
*/
#define FEATURE_ENC_BTF_IN_NV

#if defined FEATURE_BUILD_CDMA_TRIMODE || \
    defined FEATURE_BUILD_CDMA_PCS
      /* Define to enable the use of a trimode rf calibration file to derive
      ** NV items.  Also triggers the NV section to support this feature.
      */
      #define FEATURE_TRIMODE_ITEMS
#endif

#endif /* CUSTNVM_H */

