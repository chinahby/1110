#ifndef CUSTSFS_H
#define CUSTSFS_H
/*===========================================================================

              C U S T O M I Z A T I O N   O F   S F S

DESCRIPTION
  Configuration for secure msm sfs

  Copyright (c) 2005 - 2009  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/securemsm/baselines/qsc1100/main/latest/build/cust/custsfs.h#4 $
$DateTime: 2009/03/16 16:33:28 $ 
$Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/09   ejt     removed FEATURE_SEC_SFS_JTAG_EFUSE
07/27/07   avm     undefined FEATURE_SEC_SFS_MSM_DEV_KEY for QSC6055 build
04/02/07   avm     defined FEATURE_SEC_USFS and undefined FEATURE_SEC_SFS_JTAG_EFUSE for 6800
04/02/07   avm     undefined FEATURE_SEC_USFS
02/08/07   avm     defined FEATURE_SEC_USFS
22/01/07   avm     Changed FEATURE_NONSEC_NOR to BOOT_MODE_NOR to check for NOR builds
10/09/06   df      Added FEATURE_SEC_SFS_JTAG_EFUSE
05/11/06   df      6800 65nm (no svd image)
04/05/06   df      6800 support secure partitions
04/04/06   df      6800 support for standalone mode
11/13/05   df      changes for secure flash partition
07/22/05   df      Created file.

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                          Security Services
---------------------------------------------------------------------------*/
/* Security Services SFS
*/
#ifdef FEATURE_SEC_SFS

#define FEATURE_SEC_USFS

/* for 6275 and others with secure msm key */
#define FEATURE_SEC_SFS_MSM_DEV_KEY

#ifndef FEATURE_SEC_SFS_MSM_DEV_KEY
  #ifndef FEATURE_SEC_SFS_SECURE_PARTITION
    /* for 6250 efs implementation define this: */
    #define FEATURE_SEC_SFS_NV_ITEM
  #endif
#endif

#endif  /* FEATURE_SEC_SFS */
#endif  /* CUSTSFS */
