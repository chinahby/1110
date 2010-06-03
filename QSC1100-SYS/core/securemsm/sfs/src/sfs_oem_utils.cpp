/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      S E C U R I T Y    S E R V I C E S
 
                           SEC OEM Utilities

GENERAL DESCRIPTION
  Contains OEM utility for selecting SFS options that can be configured
  according to OEM requirements

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/src/sfs_oem_utils.cpp#5 $ 
  $DateTime: 2009/03/31 13:51:55 $ $Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/09   ejt     featurized include of efuse.h to MSM6xxxx targets
03/20/09   sm      created
*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

extern "C"
{
#include "sfs_oem_utils.h"
#include "msg.h"
#include "secerrno.h"
#include "assert.h"
#if !defined(IMAGE_APPS_PROC) && !defined(IMAGE_MODEM_PROC) && !defined(PLATFORM_LTK)
#include "efuse.h"
#endif

/*===========================================================================
*
* FUNCTION: sfs_oem_priv_data_loc_get ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
sfs_util_selected_priv_data_loc_type sfs_oem_priv_data_loc_get (void )
{
  sfs_util_selected_priv_data_loc_type priv_data_loc_type = E_PRIV_DATA_UNSUPPORTED;

#ifdef FEATURE_SEC_SFS
#ifdef FEATURE_SEC_SFS_SECURE_PARTITION

   /* If the Secure Partition feature is defined, the partition is present */
   /* and can be used                                                      */
  priv_data_loc_type = E_PRIV_DATA_SFP;

#endif //FEATURE_SEC_SFS_SECURE_PARTITION
#endif // FEATURE_SEC_SFS

MSG_MED("Private Data Location Type is: %d ",priv_data_loc_type,0,0);
return priv_data_loc_type;
}

/*===========================================================================
*
* FUNCTION: sfs_oem_allow_imei_write ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
sfs_util_imei_write_control_type sfs_oem_allow_imei_write(void)
{
	sfs_util_imei_write_control_type imei_write_control = E_IMEI_MULTIPLE_WRITE;

#ifndef PLATFORM_LTK
#if !defined(IMAGE_APPS_PROC) && !defined(IMAGE_MODEM_PROC)

	/* For MSM6xxx targets, check if the E-FUSE is blown */
    uint32 enable_bit = 0;
    efuse_err_enum_type efuse_result = EFUSE_NO_ERR;

    efuse_result = efuse_get_item(TRUSTED_BOOT, &enable_bit);
    if ( EFUSE_NO_ERR != efuse_result )
    {
      MSG_ERROR("SFS-IMEI write: EFUSE error! %d", efuse_result, 0, 0);
	}
    else if ( enable_bit )
    {
      MSG_ERROR("SFS-IMEI write: EFUSE blown", 0, 0, 0);    
      imei_write_control = E_IMEI_READONLY;
    }
#endif /* !defined(IMAGE_APPS_PROC) && !defined(IMAGE_MODEM_PROC) */
#endif /* PLATFORM_LTK */

	return imei_write_control;
}

} // extern "C"
