/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                                 G S D I  T A S K

                                      A N D

          F I L E  S Y S T E M  I N T E R F A C E  F U N C T I O N S


GENERAL DESCRIPTION

  This source files contains the GSDI to EFS / FS Interface Functions used
  by the GSDI Task.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_efs.c#3 $$ $DateTime: 2009/01/22 05:15:47 $ $Author: nbandwar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/09   nb      Added check for file existence before removing 
01/16/09   nb      Replaced EFS1 API calls with EFS2 APIs
11/12/08   kk      Removed code defined under 
                   FEATURE_MMGSDI_PERSONALIZATION_NON_QC
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
03/08/06   jar     Merged in Branched SIM Lock.
11/09/05   sun     Fixed lint errors
05/13/05   sun     Removed file size limits on read/write
05/11/05   sst     Lint fixes
11/15/04   jar     Changed to use GSDI MSG ERROR DEBUG MACROS
06/20/04 jar/tml   Cleanup by removing variables not used.
                   Fixes for Write Operations
05/18/04   jar     Initial Revision

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "customer.h"
#include "string.h"
#include "err.h"
#include "gsdi_efs.h"
#include "gsdi.h"
#include "tmc.h"
#include "fs_errno.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define DIR_PERMISSION       0x777 /* Read Write Execute Permission 
                                      for the created directory */


/* ------------------------------------------------------------------------*/
/*                   G S D I   E F S   I N T E R F A C E                   */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/
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
  const char   * file_handle_ptr,
  int32        * returned_data_len_ptr
)
{
  struct fs_stat         file_stat = {0};

  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     2.  returned_data_len_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MSG_ERROR("NULL File Handle",0,0,0);
    return GSDI_ERROR;
  }
  if ( returned_data_len_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Returned Data Len Pointer",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Call the FS Function to ge the File Size
     Only return a size if FS_OKAY_S is returned. Otherwise, set the length
     to 0.
     ------------------------------------------------------------------------*/
  if(efs_stat(file_handle_ptr, &file_stat) == 0)
  {
    *returned_data_len_ptr = uint32toint32(file_stat.st_size);
    return GSDI_SUCCESS;
  }
  else
  {
    MSG_ERROR("EFS File or Dir Not Found ", 0, 0, 0);
    *returned_data_len_ptr = 0;
    return GSDI_ERROR;
  }
} /* gsdi_fs_get_file_size */


/*===========================================================================
FUNCTION GSDI_EFS_READ_FILE

DESCRIPTION
  Generic Function called by GSDI to read a file from EFS.  This function
  will validate the input parameters provided and return the Data requested.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:  File was read
                   GSDI_ERROR:    Failure occurred (Need to check EF Failure)
                                  or the Input parameters were not valid.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_efs_read_file(
  const char *   file_handle_ptr,
  int32      *   returned_data_len_ptr,
  uint8      *   data_buffer_ptr
)
{
  fs_size_t               file_size = 0;
  fs_open_xparms_type     open_parms;
  struct fs_stat          file_stat = {0};
  int                     file_descriptor = 0;  

  memset(&open_parms, 0x00, sizeof( fs_open_xparms_type));

  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     2.  returned_data_len_ptr can not be NULL
     3.  data_buffer_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL File Handle",0,0,0);
    return GSDI_ERROR;
  }
  if ( returned_data_len_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Returned Data Len Pointer",0,0,0);
    return GSDI_ERROR;
  }
  if ( data_buffer_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Data Buffer Pointer",0,0,0);
    return GSDI_ERROR;
  }
  
  if(efs_stat(file_handle_ptr, &file_stat) != 0)
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_stat procedure failed: %x", file_stat.st_size, 0, 0);
    return GSDI_ERROR;  
  }

  file_size = file_stat.st_size;
  /* --------------------------------------------------------------------------
     Open the File in EFS for Read Only
     -------------------------------------------------------------------------*/
  open_parms.create.cleanup_option   = FS_OC_CLOSE;
  open_parms.create.buffering_option = FS_OB_PROHIBIT;

  file_descriptor = efs_open(file_handle_ptr, O_RDONLY);
  
  if(file_descriptor < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_open Procedure Failed ", 0, 0, 0);
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Read the file into buffer provided
     ----------------------------------------------------------------------*/
  if(efs_read(file_descriptor, data_buffer_ptr, file_size) 
              != (fs_ssize_t)file_size)
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_read procedure failed ", 0, 0, 0);
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Data has been successfully copied in data_buffer_ptr
     Need to assign the returned_data_len_ptr value and close this
     file
     ----------------------------------------------------------------------*/
  *returned_data_len_ptr = (int32)file_size;

  if(efs_close(file_descriptor) != 0)
  {
    MSG_HIGH("efs_close procedure failed 0x%x", file_descriptor, 0, 0);  
  }

  return GSDI_SUCCESS;
}  /* gsdi_efs_read_file */


/*===========================================================================
FUNCTION GSDI_EFS_CREATE_FILE

DESCRIPTION
  Generic Function called by GSDI to create a file in EFS.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:  File was created
                   GSDI_ERROR:    Failure occurred (Need to check EF Failure)
                                  or the input params were not valid.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_efs_create_file(
  const char *   file_handle_ptr
)
{
  fs_open_xparms_type     open_parms;
  int                     file_descriptor = 0;

  memset(&open_parms, 0x00, sizeof( fs_open_xparms_type));

  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL File Handle",0,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     To create a file in EFS, you must use the efs_open Method.
     Provide the Parameter:  O_CREAT to create the file.
     ------------------------------------------------------------------------*/
  open_parms.create.cleanup_option   = FS_OC_CLOSE;
  open_parms.create.buffering_option = FS_OB_PROHIBIT;

  file_descriptor = efs_open(file_handle_ptr, O_CREAT);
  if(file_descriptor < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_open Procedure Failed ", 0, 0, 0);
    return GSDI_ERROR;  
  }
  /* ------------------------------------------------------------------------
     Need to close the file used in the open.
     If the close fails, we'll assume it is still usable for now.
     ----------------------------------------------------------------------*/
  if(efs_close(file_descriptor) != 0)
  {
    /* We will flag the Error but will not fail the entire
    ** Procedures since the handle could not be closed
    */
    MSG_HIGH("efs_close procedure failed 0x%x", file_descriptor, 0, 0);
  }
  
  return GSDI_SUCCESS;
} /* gsdi_efs_create_file */

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
  const char *   file_handle_ptr
)
{
  struct fs_stat          file_stat = {0};
  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR ("NULL Handle Provided",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     To test the existence of the file, we must use the fs_name_test()
     function provided.
     ------------------------------------------------------------------------*/

  /* --------------------------------------------------------------------------
     Criteria for success is if the file is found
     ------------------------------------------------------------------------*/
  if(efs_stat(file_handle_ptr, &file_stat) == 0)
  {
    MSG_HIGH("efs_stat procedure success", 0, 0, 0);
    return GSDI_SUCCESS;  
  }
  else
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_stat procedure failed ", 0, 0, 0);
    return GSDI_ERROR;
  }
} /* gsdi_efs_name_test*/


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
  const char *   file_handle_ptr
)
{
  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR ("NULL Handle Provided",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     To create the directory...we'll use the efs_mkdir() function.
     ------------------------------------------------------------------------*/
  if(efs_mkdir(file_handle_ptr, DIR_PERMISSION) < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_mkdir procedure failed", 0, 0, 0);
    return GSDI_ERROR; 
  }

  return GSDI_SUCCESS;
} /* gsdi_efs_mkdir */


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
  const char *   file_handle_ptr,
  int32          num_bytes,
  uint8 *        data_buffer_ptr
)
{
  gsdi_returns_T          gsdi_status     = GSDI_SUCCESS;
  int                     file_descriptor = 0;
  uint32                  bytes_to_write  = int32touint32(num_bytes);
  struct fs_stat          file_stat       = {0};
  fs_open_xparms_type     open_parms;

  memset(&open_parms, 0x00, sizeof( fs_open_xparms_type));

  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL File Handle",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Remove the file ... Number of Retries
     ------------------------------------------------------------------------*/
  if(efs_stat(file_handle_ptr, &file_stat) == 0)
  {
    if(efs_unlink(file_handle_ptr) < 0)
    {
      MMGSDI_DEBUG_MSG_ERROR("Failure to remove file ", 0, 0, 0);
      return GSDI_ERROR;  
    }
  }
  else if (efs_errno != ENOENT) 
  {
    /* Test failed due to reason other than "File Does not Exists"*/
    MMGSDI_DEBUG_MSG_ERROR("Failure To get file stats ", 0, 0, 0);
    return GSDI_ERROR;  
  }
  
  /* --------------------------------------------------------------------------
     Open the File in EFS for Create as a New File
     -------------------------------------------------------------------------*/
  open_parms.create.cleanup_option   = FS_OC_CLOSE;
  open_parms.create.buffering_option = FS_OB_PROHIBIT;

  file_descriptor = efs_open(file_handle_ptr, O_CREAT | O_TRUNC | O_WRONLY);
  if(file_descriptor < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_open Procedure Failed ", 0, 0, 0);
    return GSDI_ERROR;  
  }

  /* --------------------------------------------------------------------------
    Write the Data to the File
    -------------------------------------------------------------------------*/
  if(efs_write(file_descriptor, data_buffer_ptr, (fs_size_t)bytes_to_write) 
               != (fs_ssize_t)bytes_to_write)
  {
    MMGSDI_DEBUG_MSG_ERROR("efs_write failed ", 0, 0, 0);
    gsdi_status = GSDI_ERROR;
  }
  
  /* ------------------------------------------------------------------------
     Data has been successfully written to FS.  Ok to close up the file
     ----------------------------------------------------------------------*/
  if(efs_close(file_descriptor) < 0)
  {
    MSG_HIGH("efs_close procedure failed 0x%x", file_descriptor, 0, 0);
  }

  return gsdi_status;
}  /* gsdi_efs_write_file */


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
  int32  len,
  int32 * len_written_p
)
{
  MMGSDI_RETURN_IF_NULL(len_written_p);

  *len_written_p = 0;
  return GSDI_NOT_SUPPORTED;
} /* gsdi_simsecure_write_to_flash */


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
)
{
  MMGSDI_RETURN_IF_NULL(len_read_p);

  *len_read_p = 0;
  return GSDI_NOT_SUPPORTED;
} /* gsdi_simsecure_read_from_flash */

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
)
{
  MMGSDI_RETURN_IF_NULL(file_size_p);
  
  *file_size_p = 0;
  return GSDI_NOT_SUPPORTED;
} /* gsdi_simsecure_get_file_size */


/*===========================================================================

FUNCTION      GSDI_EFS_SIMSECURE_FLASH_INIT


DESCRIPTION   This function initializes the SIM_SECURE partition: 1. opening
              the partition, 2. creating the good block table.

DEPENDENCIES  None.

RETURN VALUE  Status of the operation.

SIDE EFFECTS  None.

===========================================================================*/
gsdi_returns_T gsdi_efs_simsecure_flash_init(
  boolean * sim_partition_set_p
)
{
  return GSDI_NOT_SUPPORTED;
}
