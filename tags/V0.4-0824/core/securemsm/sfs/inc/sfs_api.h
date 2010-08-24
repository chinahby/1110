#ifndef _SFS_API_H_
#define _SFS_API_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      S E C U R I T Y    S E R V I C E S
 
                          SFS API Header File

                  I N T E R N A L    H E A D E R    F I L E


GENERAL DESCRIPTION
  These are the external functions to access sfs.

  they include:
  sfs_open
  sfs_close
  sfs_read
  sfs_write
  sfs_mkdir
  sfs_rm
  sfs_init

EXTERNAL FUNCTIONS


Copyright (c) 2004 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/inc/sfs_api.h#4 $ 
  $DateTime: 2008/12/17 11:05:01 $ $Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/08   ejt     removed SFS_SYSTEM_INTERFACE from SFS files
04/04/07   df      adding MINIMAL_DUAL_PD featurization
03/03/07   df      new function sfs_ixfile_rm()
03/01/07   df      support for seek
01/09/06   df      local support for GetInfo
12/22/06   df      support for usfs
12/08/06   df      added cpp support
07/25/05   df      changed sfs_init prototype
07/22/05   df      added FEATURE_SEC_SFS
*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#ifdef FEATURE_SEC_SFS
#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "secerrno.h"

#if defined(SEC_PD)
/*===========================================================================

FUNCTION SFS_GETSIZE ()

DESCRIPTION
 non - published api, only available in the SEC_PD
   retrieved the file size of an open sfs file
DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS - the file size in stored in size
  E_FAILURE - an error occured while getting the file size

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sfs_getSize(int fd, uint32* size);
#endif

/*===========================================================================

FUNCTION SFS_IXFILE_OPEN ()

DESCRIPTION
  opens an sfs file -
  creates a subdirectory under the base path if necessary
  the subdirectory name is constructed based on the current base

  The base directory must exist. otherwise an NULL is returned

DEPENDENCIES
  None

RETURN VALUE
  return a valid file descriptor, or -1 if error

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_ixfile_open(const char* path, int flags);

/*===========================================================================

FUNCTION SFS_OPEN ()

DESCRIPTION
  encrypts the current seg; in place encryption

DEPENDENCIES
  None

RETURN VALUE
  return a valid file descriptor, or NULL, if file not opened

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_mkdir(const char* path);

/*===========================================================================

FUNCTION SFS_OPEN ()

DESCRIPTION
  encrypts the current seg; in place encryption

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_open(const char* path, int flags);

/*===========================================================================

FUNCTION SFS_CLOSE ()

DESCRIPTION
  closes an opened sfs file

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_close(int fd);

/*===========================================================================

FUNCTION SFS_READ ()

DESCRIPTION
  reads bytes from an encrypted sfs file

DEPENDENCIES
  None

RETURN VALUE
  returns the number of bytes read from sfs file

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_read(int fd, char *buf, int nbytes);

/*===========================================================================

FUNCTION SFS_WRITE ()

DESCRIPTION
  writes bytes to an encrypted sfs file

DEPENDENCIES
  None

RETURN VALUE
  returns the number of bytes written to sfs file

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_write(int fd, const char *buf, int nbytes);

/*===========================================================================

FUNCTION SFS_IXFILE_RM ()

DESCRIPTION
  removes an sfs file originated from IxFile interface

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_ixfile_rm(const char *path);

/*===========================================================================

FUNCTION SFS_rm ()

DESCRIPTION
  removes an sfs file

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_rm(const char *path);

/*===========================================================================

FUNCTION SFS_INIT()

DESCRIPTION
  Initializes sfs parameters

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS or E_FAILURE

SIDE EFFECTS
  None.
===========================================================================*/
void sfs_init(uint32 seg_size);


/*===========================================================================

FUNCTION SFS_SEEK ()

DESCRIPTION
  moves read location to point to n bytes from start, curr, or end of file

DEPENDENCIES
  None

RETURN VALUE
  returns the current location if success or -1 if error

SIDE EFFECTS
  None.
===========================================================================*/
int sfs_seek(int fd, int32 offset, int whence);

#ifdef __cplusplus
}
#endif
#endif //FEATURE_SEC_SFS
#endif //_SFS_API_H_
