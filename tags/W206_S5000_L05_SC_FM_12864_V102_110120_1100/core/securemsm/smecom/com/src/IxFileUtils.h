#ifndef IXFILEUTILS_H
#define IXFILEUTILS_H
/*============================================================================
IxFileUtils

ABSTRACT
   Utility functions used by the IxFile and IxFileMgr implementation.

DESCRIPTION
   Utility functions used by the IxFile and IxFileMgr implementation.
   

Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/IxFileUtils.h#3 $

=========================================================================== */
#include "comdef.h"
#include "IxFile.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*==============================================================
FUNCTION:  IxFileIsSFSDrive

DESCRIPTION:
   Returns TRUE if the input path matches the system SFS drive or
   the user SFS drive.
==============================================================*/
boolean IxFileIsSFSDrive (const char *path);

/*==============================================================
FUNCTION:  IxFileIsSysSFSDrive

DESCRIPTION:
   Returns TRUE if the input path matches the system SFS drive.
==============================================================*/
boolean IxFileIsSysSFSDrive (const char *path);

/*==============================================================
FUNCTION:  IxFileGetFilePath

DESCRIPTION:
   Given a path, potentially with a drive string in front, returns
   a mapped path with the drive string replaced by the appropriate
   path prefix. If the input path has no drive string in front or
   if no matching path prefix can be found for the drive string,
   the path returned is identical to the path passed in.
==============================================================*/
IxErrnoType IxFileGetFilePath (char *newPath, const char *oldPath);


#ifdef __cplusplus
}
#endif

#endif /* IXFILEUTILS_H */
