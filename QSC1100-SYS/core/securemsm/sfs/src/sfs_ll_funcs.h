#ifndef _SFS_LL_FUNCS_H_
#define _SFS_LL_FUNCS_H_

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      S E C U R I T Y    S E R V I C E S
 
                             SFS Low Level Funcs

                  I N T E R N A L    H E A D E R    F I L E


GENERAL DESCRIPTION
   Header file for sfs low level functions

EXTERNAL FUNCTIONS


Copyright (c) 2005-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/src/sfs_ll_funcs.h#5 $ 
  $DateTime: 2009/03/22 12:20:09 $ $Author: shitalm $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/09   sm      removed FEATURE_SEC_SFS_SECURE_PARTITION and kept functionality
12/17/08   ejt     removed SFS_SYSTEM_INTERFACE from SFS files
03/29/07   df      changes to remove efs calls from sec pd
12/22/06   df      support for usfs
12/08/06   df      cpp support - removed some c header file includes
03/22/06   df      numerous checks added
                   changes to support seed sector on secure partition
01/08/06   df      changes to support silent backup
11/12/05   df      changes for secure flash partition
07/22/05   df      added FEATURE_SEC_SFS
*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sfs.h"
#ifdef FEATURE_SEC_SFS
 
/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

                    LOCAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION SFS_LL_TRUNC ()

DESCRIPTION
  uses the low level file trucate function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_ll_trunc (const char* path, int32 length);


/*===========================================================================

FUNCTION SFS_LL_TRUNC ()

DESCRIPTION
  uses the low level file open function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

sfs_segfh_type sfs_ll_open (sfs_handle_type *fh, int32 flags);


/*===========================================================================

FUNCTION SFS_LL_CLOSE ()

DESCRIPTION
  uses the low level file close function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_ll_close(sfs_handle_type *fh);


/*===========================================================================

FUNCTION SFS_LL_READ ()

DESCRIPTION
  uses the low level file read function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_ll_read(sfs_handle_type *fh, char *buf, int32 nbytes);


/*===========================================================================

FUNCTION SFS_LL_WRITE ()

DESCRIPTION
  uses the low level file write function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_ll_write(sfs_handle_type *fh, const char *buf, int32 nbytes);


/*===========================================================================

FUNCTION SFS_LL_EXISTS()

DESCRIPTION
  uses the low level file stat function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

boolean sfs_ll_exists(const char* path);


/*===========================================================================

FUNCTION SFS_LL_ISDIR

DESCRIPTION
  verifies that a directory exists

DEPENDENCIES
  None

RETURN VALUE
   TRUE or FALSE

SIDE EFFECTS
  None.
===========================================================================*/

boolean sfs_ll_isdir(const char* path);

/*===========================================================================

FUNCTION SFS_LL_RM()

DESCRIPTION
  uses the low level file delete function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_ll_rm(const char* path);

/*===========================================================================

FUNCTION SFS_LL_RM()

DESCRIPTION
  uses the low level file delete function

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_ll_mkdir(const char* path);


/*===========================================================================

FUNCTION SFS_LL_ALLOC_SIZE

DESCRIPTION
  gets the proper allocation size based on segment size

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

uint32 sfs_ll_alloc_size(sfs_handle_type *fh);

#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================

FUNCTION SFS_LL_FLASH_READ_K

DESCRIPTION
  uses the low level file read

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_ll_flash_read_k(sfs_handle_type *fh, uint8 *buf);

/*===========================================================================

FUNCTION SFS_FLASH_ISREADY

DESCRIPTION
checks to see if flash secure partition has been initialize with key sector

DEPENDENCIES
None

RETURN VALUE
E_SUCCESS or E_FAILURE

SIDE EFFECTS
None.
===========================================================================*/
boolean sfs_flash_isready(void);
#ifdef __cplusplus
}
#endif
#endif // FEATURE_SEC_SFS
#endif // _SFS_LL_FUNCS_H_
