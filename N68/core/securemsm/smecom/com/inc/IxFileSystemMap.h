#ifndef IXFILESYSTEMMAP_H
#define IXFILESYSTEMMAP_H
/*============================================================================
IxFileSystemMap

ABSTRACT
   Maps path prefixes to filesystems.

DESCRIPTION
   Maps path prefixes to filesystems.


Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxFileSystemMap.h#2 $

=========================================================================== */
#include "IxErrno.h"
#include "IxFileSystem.h"

class IxFileSystemMap {

#define IXFILE_MAX_NUM_FS 8

  /*------------------------------------------------------------------------
                               PUBLIC METHODS
  ------------------------------------------------------------------------*/

public:

  typedef struct
  {
    const char   *pPath;
    IxFileSystem *pFS;
  } MapT;

  /***********************************************************************
   * MapFS
   *
   * Maps a path prefix to a filesystem. The IxFileSystem object specified
   * is registered as the handler for the path prefix passed in.
   *
   * Returns E_SUCCESS on success and a different error code on failure.
   **********************************************************************/
   static IxErrnoType MapFS (IxFileSystem *pFS, const char *pPathPrefix);

  /***********************************************************************
   * UnmapFS
   *
   * Deletes the association between a path prefix and a filesystem.
   *
   * Returns E_SUCCESS if the specified IxFileSystem object is currently
   * registered as the handler for the specified path prefix and it is
   * successfully unmapped. Returns E_NO_ENTRY if the specified
   * filesystem is not the handler. Returns E_FAILURE on other errors.
   **********************************************************************/
   static IxErrnoType UnmapFS (IxFileSystem *pFS);

private:

  /*------------------------------------------------------------------------
                               PRIVATE METHODS
  ------------------------------------------------------------------------*/

  /***********************************************************************
   * GetFS
   *
   * Returns a pointer to the filesystem that handles the specified path
   * prefix. If there is more than one filesystem registered for the
   * path prefix, the first one found is returned.
   *
   * Returns E_SUCCESS on success and a different error code on failure.
   **********************************************************************/
   static IxErrnoType GetFS (IxFileSystem **ppFS, const char *pPathPrefix);

  /*------------------------------------------------------------------------
                               PRIVATE DATA
  ------------------------------------------------------------------------*/

  static MapT fsMap[IXFILE_MAX_NUM_FS];

};

#endif /* IXFILESYSTEMMAP_H */

