#ifndef _SFS_INDEX_H_
#define _SFS_INDEX_H_
#include "sfs.h"
#ifdef FEATURE_SEC_SFS
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
                            SFS Index Header File

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file is the internal header file for sfs index manipulation.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2004 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCVPath: $
  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/src/sfs_index.h#3 $ 
  $DateTime: 2008/12/17 11:05:01 $ $Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/08   ejt     removed SFS_SYSTEM_INTERFACE from SFS files
12/22/06   df      support for usfs
07/22/05   df      added FEATURE_SEC_SFS

===========================================================================*/

/*===========================================================================

FUNCTION SFS_CONSTRUCT_MIX_PNAME

DESCRIPTION
  contruct the mangled ix path/name using curr path info, and deposit it
  in path pointer supplied.
 
DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_construct_mix_pname(sfs_handle_type *fh, char *path);

/*===========================================================================

FUNCTION SFS_GET_INDEX_INFO

DESCRIPTION
  opens index file. searches the index file for file params.

  things to keep straight:
    fh    is the file handle we are trying to get info for
    ix_fh is the file handle for the index file itself.
 
DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_get_index_info(sfs_handle_type *fh);

/*===========================================================================

FUNCTION SFS_UPDATE_INDEX_INFO

DESCRIPTION
  if current entry does not exist, appends entry.
  if current entry exists, and it has changed, then write corresponding
    segment back out.
  if current entry is to be deleted, null it out.
  if there are enough nulls in the segment, do a squeeze.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sfs_update_index_info(sfs_handle_type *fh);
#endif // FEATURE_SEC_SFS
#endif //_SFS_INDEX_H_
