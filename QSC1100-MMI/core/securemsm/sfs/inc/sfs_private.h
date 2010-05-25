#ifndef SFS_PRIVATE_H
#define SFS_PRIVATE_H
#include "customer.h"
#ifdef FEATURE_SEC_SFS
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      S E C U R I T Y    S E R V I C E S
 
                           SFS Private Data Utils Header

                            H E A D E R    F I L E


GENERAL DESCRIPTION
  Header file for private data utils

EXTERNAL FUNCTIONS


Copyright (c) 2004-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/inc/sfs_private.h#3 $ 
  $DateTime: 2009/03/22 12:33:50 $ $Author: shitalm $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/09   sm      updated api documentation and removed a redundant function
05/07/07   avm     removed FEATURE_SEC_SFS_SECURE_PARTITION featurization
12/05/06   df      converted to cpp, externed for c access
10/12/06   df      added sfs_priv_errno_enum_type
04/04/06   df      new hdr config api
03/22/06   df      numerous checks added
                   changes to support seed sector on secure partition
07/22/05   df      FEATURE_SEC_SFS
03/20/05   df      changed priv key size macro from 16 to 256
*/

#ifdef __cplusplus
extern "C"
{
#endif


/* External definitions */

/* sfs errnos - typicaly used by imei priv access functions */
typedef enum
{
  SFS_PRIV_SUCCESS            =  0,    /* Operation successful                  **/
  SFS_PRIV_FAILURE            =  1,    /* Op failed due to unknown err   **/
  SFS_PRIV_READONLY           =  2,    /* Op failed because item is read only **/
  SFS_PRIV_INACTIVE           =  3,    /* Op failed, item not provisioned **/
  SFS_PRIV_BAD_PARAM          =  4     /* Op failed, bad parameter **/
} sfs_priv_errno_enum_type;

/* sfs files that can be used to store private data */
typedef enum
{
  SFS_PRIV_FILE_1            =  0,
  SFS_PRIV_FILE_2            =  1,
  SFS_PRIV_FILE_3            =  2,
  SFS_PRIV_FILE_4            =  3,
  SFS_PRIV_FILE_5            =  4 
} sfs_priv_file_enum_type;

/* Internal definitions */
#define SFS_PRIV_KEY_SIZE 256

/* Function prototypes */
/*===========================================================================

FUNCTION SFS_PRIV_SIMLOC_WRITE()

DESCRIPTION
  Write private data to simloc file in SFS or Secure Flash Partition

  Callers should call sfs_oem_priv_data_loc_get() to see if they can go
  through this API

DEPENDENCIES
  None

RETURN VALUE
  number of bytes written
  0 if no bytes written or error

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_priv_simloc_write(uint8 *buf, int len);

/*===========================================================================

FUNCTION SFS_PRIV_SIMLOC_READ()

DESCRIPTION
  Reads simloc data
  buf - buffer to read data into
  len - number of bytes to read

DEPENDENCIES
  None

RETURN VALUE
  number of bytes read
  0 if no bytes read or error

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_priv_simloc_read(uint8 *buf, int len);


/*===========================================================================

FUNCTION SFS_PRIV_IMEI_WRITE()

DESCRIPTION
  Write private data to imei file in SFS or Secure Flash Partition

  Callers should call sfs_oem_priv_data_loc_get() to see if they can go
  through this API

DEPENDENCIES
  None

RETURN VALUE
sfs_priv_errno_enum_type :
SFS_PRIV_SUCCESS            =  0,    Operation successful                 
SFS_PRIV_FAILURE            =  1,    Op failed due to unknown err   
SFS_PRIV_READONLY           =  2,    Op failed because item is read only 
SFS_PRIV_INACTIVE           =  3,    Op failed, item not provisioned 
SFS_PRIV_BAD_PARAM          =  4     Op failed, bad parameters passed

SIDE EFFECTS
  Multiple writes may be disabled depending on OEM implementation. See
  secapi_oem_allow_imei_write() in sec_oem_utils.h.
===========================================================================*/

sfs_priv_errno_enum_type sfs_priv_imei_write(uint8 *buf, int len);


/*===========================================================================

FUNCTION SFS_PRIV_IMEI_READ()

DESCRIPTION
  reads imei data
  buf - buffer to read data into
  len - number of bytes to read

  Callers should call sfs_oem_priv_data_loc_get() to see if they can go
  through this API

DEPENDENCIES
  None

RETURN VALUE
sfs_priv_errno_enum_type :
SFS_PRIV_SUCCESS            =  0,    Operation successful                 
SFS_PRIV_FAILURE            =  1,    Op failed due to unknown err   
SFS_PRIV_READONLY           =  2,    Op failed because item is read only 
SFS_PRIV_INACTIVE           =  3,    Op failed, item not provisioned 
SFS_PRIV_BAD_PARAM          =  4     Op failed, bad parameters passed

SIDE EFFECTS
  None.
===========================================================================*/
sfs_priv_errno_enum_type sfs_priv_imei_read(uint8 *buf, int len);


/*===========================================================================

FUNCTION SFS_PRIV_FILE_WRITE()

DESCRIPTION
  Writes private data to the file specified by the file type in SFS or
  Secure Flash Partition

  Callers should call sfs_oem_priv_data_loc_get() to see if they can go
  through this API

DEPENDENCIES
  None

RETURN VALUE
  number of bytes written
  0 if no bytes written or error

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_priv_file_write(sfs_priv_file_enum_type file, uint8 *buf, int len);


/*===========================================================================

FUNCTION SFS_PRIV_FILE_READ()

DESCRIPTION
  reads private data from file specified in the file type
  file - file to read from
  buf - buffer to read data into
  len - number of bytes to read

  Callers should call sfs_oem_priv_data_loc_get() to see if they can go
  through this API

DEPENDENCIES
  None

RETURN VALUE
  number of bytes read or 
  0 if no bytes read or error

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_priv_file_read(sfs_priv_file_enum_type file, uint8 *buf, int len);

#ifdef __cplusplus
}
#endif

#endif  // FEATURE_SFS
#endif /* SFS_PRIVATE_H */
