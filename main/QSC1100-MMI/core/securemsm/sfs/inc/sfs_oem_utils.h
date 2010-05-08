#ifndef SFS_OEM_UTILS_H
#define SFS_OEM_UTILS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      S E C U R I T Y    S E R V I C E S
 
                        SFS OEM Utils Header file

GENERAL DESCRIPTION
  Contains OEM utility for selecting SFS options that can be configured
  according to OEM requirements

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/inc/sfs_oem_utils.h#4 $ 
  $DateTime: 2009/03/25 13:40:16 $ $Author: shitalm $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/09   sm      created
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "secerrno.h"

/* This enum identifies the location to be used to store certain private data */
typedef enum
{
  E_PRIV_DATA_SFS          = 0,   /* Use SFS to store the private data */
  E_PRIV_DATA_SFP          = 1,   /* Use Secure Flash Partition (SFP) to store the private data */
  E_PRIV_DATA_UNSUPPORTED  = 2    /* Don't use SFP or SFS to store the private data */
}sfs_util_selected_priv_data_loc_type;

/* This enum identifies the write controls for the IMEI */
typedef enum
{
  E_IMEI_READONLY               =  0,    /* IMEI is readonly and cannot be written to */
  E_IMEI_ONETIME_WRITE          =  1,    /* IMEI can be written only if it has not already been written before */
  E_IMEI_MULTIPLE_WRITE         =  2     /* IMEI can be written multiple times        */
}sfs_util_imei_write_control_type;

/*===========================================================================

FUNCTION SFS_OEM_PRIV_DATA_LOC_GET ()

DESCRIPTION
  Allows the OEM to control where the private data configured through 
  SFS_PRIVATE API should be stored.

  OEM should use E_PRIV_DATA_SFP or E_PRIV_DATA_SFS only if the target
  specific prerequisites for SFS i.e Secure Boot and blowing of fuses
  for MSM Dev Key are met OR OEM has configured OEM_DEV_KEY

  OEM should note that using SFS will not prevent data from being
  deleted if the file system is erased

DEPENDENCIES
  None

RETURN VALUE
  E_PRIV_DATA_SFS for Secure File System.

  E_PRIV_DATA_SFP for Secure Flash Partition.

  E_PRIV_DATA_EFS for EFS

SIDE EFFECTS

===========================================================================*/
sfs_util_selected_priv_data_loc_type sfs_oem_priv_data_loc_get (void );

/*===========================================================================
*
* FUNCTION: sfs_oem_allow_imei_write ()
*
* DESCRIPTION:
*
*   Allows the OEM to control whether the IMEI can be written to and if so,
*   how many times it can be written.
*
*   This function is called by the SFS / Secure Flash Partition API when the
*   IMEI is being provisioned
*
*   The default implementation is as follows:
*   |========================================================================|
*   |Features supported  | FUSE Controlled |  No Fuse Control                |
*   |====================|=================|=================================| 
*   |                    |                 | One time write | Multiple write |
*   |                    |                 |================|================|
*   |Secure File System  |       NO        |        NO      |      YES       |
*   |(IMEI is written    |                 |                |                | 
*	|in encrypted form in|                 |                |                | 
*	|the file system)    |                 |                |                | 
*   |====================|=================|=================================|
*   |Secure Flash        | YES for MSM6xxx |        NO      |      YES       |
*   |Partition           | Do not write    |                |                | 
*   |(IMEI is written in | IMEI if TRUSTED |                |                | 
*   |encrypted form to   | BOOT fuse is    |                |                | 
*   |the flash partition)| blown           |                |                | 
*   |                    | See note blow   |                |                | 
*   |========================================================================|
*
*   The OEM can customize the entire function, i.e whether FUSE control should
*   be used and if so, which fuse to use. If no FUSE control is used, whether
*   the IMEI can be written to multiple times or only once etc.
*
*   
* RETURN VALUE:
*
*   E_IMEI_READONLY to denote the IMEI cannot be written to.
*   E_IMEI_ONETIME_WRITE to denote the IMEI can be written ONLY IF it has not
*   already been provisioned.
*   E_IMEI_MULTIPLE_WRITE to denote IMEI can be written to.
*
===========================================================================*/
sfs_util_imei_write_control_type sfs_oem_allow_imei_write(void);

#ifdef __cplusplus
}
#endif
#endif /* SFS_OEM_UTILS_H */

