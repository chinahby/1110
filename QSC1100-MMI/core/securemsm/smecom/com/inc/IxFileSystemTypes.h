/***********************************************************************
 * IxFileSystemTypes.h
 *
 * Copyright (C) 2007, Qualcomm, Inc.
 *
 * Type definitions for IxFile file systems.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxFileSystemTypes.h#1 $ $DateTime: 2007/12/06 15:30:46 $ $Author: jmiera $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2007-11-01    jee     initial release

===========================================================================*/

#ifndef __IXFS_TYPES_H__
#define __IXFS_TYPES_H__

#define IXFILE_NAME_MAX                512
           

typedef struct
{
   int  fileType;
   char name[IXFILE_NAME_MAX+1];
} IxFileDirEntT;


/*
** Various meaningful bits in the mode parameter passed to the OpenFile method.
** Some commonly used values of the mode parameter are:
**
** o IxFile_OFM_READ : Open file for reading. Fail if file does not exist.
** o IxFile_OFM_READWRITE : Open file for reading and writing. Fail if
**     file does not exist.
** o IxFile_OFM_READWRITE | IxFile_OFM_CREATE : Open file for reading and
**     writing. Create file if it does not exist.
** o IxFile_OFM_CREATE | IxFile_OFM_TRUNCATE | IxFile_OFM_READWRITE :
**     Open file for reading and writing. Truncate file if it already exists.
**     Create file if it does not exist.
** o IxFile_OFM_APPEND : Open file for reading and writing, with all writes
**     occurring after a seek to the end. Fail if file does not exist.
** o IxFile_OFM_APPEND | IxFile_OFM_CREATE : Open file for reading and writing,
**     with all writes occurring after a seek to the end. Create file if
**     it does not exist.
*/
typedef enum {
   IxFile_OFM_READ      = 0x0001,
   IxFile_OFM_READWRITE = 0x0002,
   IxFile_OFM_CREATE    = 0x0004,
   IxFile_OFM_APPEND    = 0x0008,
   IxFile_OFM_TRUNCATE  = 0x0010,
   IxFile_OFM_BUFFERED  = 0x0020,
   IxFile_OFM_RAM_FILE  = 0x0040
}IxFileOFMModeType;

typedef unsigned int IxFileOpenFileMode;

#define IXFILE_FA_NORMAL      (0x0)
#define IXFILE_FA_HIDDEN      (0x1)
#define IXFILE_FA_DIR         (0x2)
#define IXFILE_FA_READONLY    (0x4)
#define IXFILE_FA_SYSTEM      (0x8)
#define IXFILE_FA_CONST       (0x10)               // File is in ROM
#define IXFILE_FA_FIXED       (0x20)               // File cannot be overridden

/*
** The following values have been set to match those used by efs2. They don't
** need to be, although using these values is convenient.
*/
#define IXFILE_S_IFCHR  0020000        /* Character device */
#define IXFILE_S_IFDIR  0040000        /* Directory */
#define IXFILE_S_IFBLK  0060000        /* Block device */
#define IXFILE_S_IFREG  0100000        /* Regular file */
#define IXFILE_S_IFLNK  0120000        /* Symlink */
#define IXFILE_S_IFITM  0160000        /* Item File */
#define IXFILE_S_IFMT   0170000        /* Mask of all values */

/*
** Macros to check for file type.
*/
#define IXFILE_S_ISCHR(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFCHR)
#define IXFILE_S_ISDIR(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFDIR)
#define IXFILE_S_ISBLK(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFBLK)
#define IXFILE_S_ISREG(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFREG)
#define IXFILE_S_ISLNK(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFLNK)
#define IXFILE_S_ISITM(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFITM)


typedef struct
{
   unsigned long creationDate;
   unsigned long size;
   int           fileType;
} IxFileStatType;

typedef struct
{
   unsigned long totalBytes;
   unsigned long freeBytes;
   unsigned long usedBytes;
   unsigned long fsID;
   unsigned long maxFileNameLen;
} IxFileStatFSType;

#endif /* __IXFS_TYPES_H__ */
