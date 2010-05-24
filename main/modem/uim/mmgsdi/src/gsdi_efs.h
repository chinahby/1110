#ifndef GSDI_EFS_H
#define GSDI_EFS_H
/*===========================================================================


            G S D I   T A S K   A N D  E F S  I N T E R F A C E 

                                 H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_efs.h#2 $$DateTime: 2009/01/16 06:47:27 $ $Author: nbandwar $

when       who     what, where, why
-------------------------------------------------------------------------------
01/16/09   nb      Replaced EFS1 API calls with EFS2 APIs
03/08/06   jar     Merged in Branched Lint fixes
05/11/05   sst     Lint fixes
05/25/04   jar     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "fs.h"
#include "fs_public.h"
#include "gsdi_exp.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
#include "gsdi_perso_shim.h"
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#define GSDI_SIMSECURE_MAX_FILE_SIZE         604
#define GSDI_SIMSECURE_PARTITION_STATE_SIZE    4
#define GSDI_SIMSECURE_UINT8_STATE_OFFSET   0x00
#define GSDI_SIMSECURE_READ_PAGE_SIZE       0x200


/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  GSDI_SIM_PARTITION_NOT_INIT_S     = 0x00,
  GSDI_SIM_PARTITION_READ_WRITE_S,
  GSDI_SIM_PARTITION_READ_ONLY_S,
  GSDI_SIM_PARTITION_NOT_INIT_FF_S  = 0xFF
}gsdi_efs_sim_partition_enum_state;

/* ------------------------------------------------------------------------*/
/*                   G S D I   E F S   I N T E R F A C E                   */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/

typedef enum
{
  SIMSECURE_SUCCESS              =     0,
  SIMSECURE_NAND_DEVICE_FAIL     =    -1,  // Device initialization failed
  SIMSECURE_PARTITION_FAIL       =    -2   // Corrupt partition
}simsecure_status_t;

/*===========================================================================
FUNCTION GSDI_EFS_READ_FILE

DESCRIPTION
  Generic Function called by GSDI to read a file from EFS.  This function 
  will valide the input parameters provided and return the Data requested.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:  File was read
                   GSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_efs_read_file(
  const char *   file_handle,
  int32 *        returned_data_len_p,
  uint8 *        data_buffer_p
);

/*===========================================================================
FUNCTION GSDI_EFS_MKDIR

DESCRIPTION
  Generic Function called by GSDI to test whether a directory or a file 
  already exists within the specified file structure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:  File exists
                   GSDI_ERROR:    Error occurred 

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_efs_mkdir(
  const char *   handle
);

/*===========================================================================
FUNCTION GSDI_EFS_WRITE_FILE

DESCRIPTION
  Generic Function called by GSDI to write a file to EFS.  This function 
  will valide the input parameters provided and return a status when 
  completed.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:  File was written
                   GSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_efs_write_file(
  const char *   file_handle,
  int32          num_bytes,
  uint8 *        data_buffer_p
);

/*===========================================================================
FUNCTION GSDI_EFS_NAME_TEST

DESCRIPTION
  Generic Function called by GSDI to test whether a directory or a file 
  already exists within the specified file structure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:  File exists
                   GSDI_ERROR:    Error occurred 

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_efs_name_test(
  const char *   handle
);

/*===========================================================================
FUNCTION GSDI_EFS_CREATE_FILE

DESCRIPTION
  Generic Function called by GSDI to create a file in EFS.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:  File was created
                   GSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_efs_create_file(
  const char *   file_handle
);

/*===========================================================================
FUNCTION GSDI_FS_GET_FILE_SIZE

DESCRIPTION
  Generic Function called by GSDI to retrieve the size of a file in FS.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:   File size was retrieved
                   GSDI_NOT_FOUND: File name or directory is not existent in 
                                   FS.
                   GSDI_ERROR:     Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_fs_get_file_size(
  const char *   file_handle,
  int32 *        returned_data_len_p
);

/*===========================================================================

FUNCTION      gsdi_simsecure_read_from_flash


DESCRIPTION   This function reads 'len' number of bytes from the first good
              block of the SIM_SECURE partition.

DEPENDENCIES  The SIM_SECURE parition should have been initialized using
              simsecure_flash_init() before calling this function.

RETURN VALUE  Length of bytes read from the flash. 0 if error.

SIDE EFFECTS  None.

===========================================================================*/
gsdi_returns_T gsdi_simsecure_read_from_flash(
  uint8 * buf_p,
  int32   len,
  int32 * len_read_p
);


/*===========================================================================

FUNCTION      gsdi_simsecure_write_to_flash


DESCRIPTION   This function writes the data passed on the first page of the
              first good block of the SIM_SECURE partition.

DEPENDENCIES  The SIM_SECURE parition should have been initialized using
              simsecure_flash_init() before calling this function.

RETURN VALUE  Length of bytes written to the flash. 0 if error.

SIDE EFFECTS  None.

===========================================================================*/
gsdi_returns_T gsdi_simsecure_write_to_flash(
  uint8 * buf_p,
  int32   len,
  int32 * len_written_p
);

/*===========================================================================

FUNCTION      gsdi_efs_simsecure_flash_init


DESCRIPTION   This function initializes the SIM_SECURE partition: 1. opening
              the partition, 2. creating the good block table.

DEPENDENCIES  None.

RETURN VALUE  Status of the operation.

SIDE EFFECTS  None.

===========================================================================*/
gsdi_returns_T gsdi_efs_simsecure_flash_init(
  boolean * sim_partition_not_set_p
);

/* ============================================================================
FUNCTION:  GSDI_SIMSECURE_GET_FILE_SIZE

DESCRIPTION:
  Function called retrieve the file size of the SIM Lock Data Maintained in
  the SIMSECURE Partition of the Flash Device.

DEPENDENCIES:
  Function is called by the GSDI Personalization Engine Only.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
gsdi_returns_T gsdi_simsecure_get_file_size(
  int32 * file_size_p
);

#endif /* GSDI_EFS_H */

