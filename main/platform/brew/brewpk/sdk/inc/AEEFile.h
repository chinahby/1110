#ifndef AEEFILE_H
#define AEEFILE_H
/*======================================================
FILE:  AEEFILE.h

SERVICES:  AEE FileMgr/File Interface

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEE FileMgr/File

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEE.h"

typedef struct _IFile         IFile;

//********************************************************************************************************************************
//
// IFileMgr Interface
//
//********************************************************************************************************************************

enum {
   _OFM_READ      = 0x0001,
   _OFM_READWRITE = 0x0002,
   _OFM_CREATE    = 0x0004,
   _OFM_APPEND    = 0x0008
};
#ifdef WIN32
typedef unsigned AEEOpenFileMode;
#else
typedef int8 AEEOpenFileMode;
#endif

enum {
   _SEEK_START,
   _SEEK_END,
   _SEEK_CURRENT
};
#ifdef WIN32
typedef unsigned AEEFileSeekType;
#else
typedef int8 AEEFileSeekType;
#endif

//Deprecate this enum: not all compilers will be happy doing math ops
//on the values.  Use the #defined values below.
enum {
   _FA_NORMAL     = 0,
   _FA_HIDDEN     = 0x0001,
   _FA_DIR        = 0x0002,
   _FA_READONLY   = 0x0004,
   _FA_SYSTEM     = 0x0008,
   _FA_CONST      = 0x0010,
   _FA_FIXED      = 0x0020
};
#ifdef WIN32
typedef unsigned AEEFileAttrib;
#else
typedef int8 AEEFileAttrib;
#endif

#define AEE_FA_NORMAL      (0x0)
#define _FA_NORMAL         AEE_FA_NORMAL

#define AEE_FA_HIDDEN      (0x1)
#define _FA_HIDDEN         AEE_FA_HIDDEN

#define AEE_FA_DIR         (0x2)
#define _FA_DIR            AEE_FA_DIR

#define AEE_FA_READONLY    (0x4)
#define _FA_READONLY       AEE_FA_READONLY

#define AEE_FA_SYSTEM      (0x8)
#define _FA_SYSTEM         AEE_FA_SYSTEM

#define AEE_FA_CONST       (0x10)               // File is in ROM
#define _FA_CONST          AEE_FA_CONST

#define AEE_FA_FIXED       (0x20)               // File cannot be overridden
#define _FA_FIXED          AEE_FA_FIXED

#define AEE_MAX_FILE_NAME   64

typedef struct _FileInfo
{
   char        attrib;
   uint32      dwCreationDate;
   uint32      dwSize;
   char        szName[AEE_MAX_FILE_NAME];
} AEEFileInfo;


//
//  "fs:/" namespace macros
//
#define AEEFS_ROOT_DIR     "fs:/"
#define AEEFS_HOME_DIR     "fs:/~/"
#define AEEFS_SYS_DIR      "fs:/sys/"
#define AEEFS_MOD_DIR      "fs:/mod/"
#define AEEFS_MIF_DIR      "fs:/mif/"
#define AEEFS_SHARED_DIR   "fs:/shared/"
#define AEEFS_ADDRESS_DIR  "fs:/address/"
#define AEEFS_RINGERS_DIR  "fs:/ringers/"
#define AEEFS_CARD0_DIR    "fs:/card0/"
//#define AEEFS_MCF_PICTURE_DIR    "fs:/mod/10888/"

//
//
// File Permissions, to be passed to IFILEMGR_CheckPathAccess()
//
// AEEFP_READ - Permission to read file or enumerate contents of directory
// AEEFP_WRITE - Permission to write to file or write/delete/rename files in directory
// AEEFP_CREATE - Permission to create file or directory
// AEEFP_DELETE - Permission to delete file or directory
//

#define AEEFP_NONE              (0)
#define AEEFP_READ              (0x0001)
#define AEEFP_WRITE             (0x0002)
#define AEEFP_CREATE            (0x0004)
#define AEEFP_DELETE            (0x0008)

//
// Extended file information structure.  Note the following:
//
// nStructSize - Must be initialized with the proper structure size. This allows
// for future fields to be added without damage to the client.
//
// pszName - ASCIIZ name to be filled with the file name of max len nMaxName
// nMaxName - Maximum size of name field filled.
//
// pszDescription - Wide string description of max size nMaxDescription.
// nMaxDescription - Size in bytes of description.
//
// pClasses - List of AEECLSIDs that own/use this file.
// nMaxClasses - Size in bytes of pClasses
//
typedef struct _FileInfoEx
{
   int         nStructSize;
   char        attrib;
   uint32      dwCreationDate;
   uint32      dwSize;
   char  *     pszFile;
   int         nMaxFile;
   AECHAR *    pszDescription;
   int         nDescriptionSize;
   AEECLSID *  pClasses;
   int         nClassesSize;
} AEEFileInfoEx;


//Structure to maintain FileUse Information
typedef struct _AEEFileUseInfo
{
   uint16            wMaxFiles;     //Maximum number if Files in EFS this Module is allowed to create
   uint16            wFilesUsed;    //Number of Files currenty used by this Module
   uint32            dwMaxSpace;    //Maximum EFS Space this module is allowed to consume
   uint32            dwSpaceUsed;   //Total space currently used by this module so far
}AEEFileUseInfo;


// Keep these around for old time's sake

typedef AEEFileAttrib      FileAttrib;
typedef AEEOpenFileMode    OpenFileMode;
typedef AEEFileSeekType    FileSeekType;
typedef AEEFileInfo        FileInfo;
#define MAX_FILE_NAME      AEE_MAX_FILE_NAME

#define SCS_DEFAULT        (-1)
#define SCS_BEST           (-2)
#define SCS_MIN            (-3)
#define SCS_MAX            (-4)

// IFILE_Map protections...

#define AEE_FMAP_PROT_EXEC      (0x01) // Allow execution
#define AEE_FMAP_PROT_READ      (0x02) // Allow read
#define AEE_FMAP_PROT_WRITE     (0x04) // Allow write
#define AEE_FMAP_PROT_NONE      (0x08)

// IFILE_Map flags...

#define AEE_FMAP_FIXED           (0x01)   // Only input void * supported otherwise fails...
#define AEE_FMAP_SHARED          (0x02)   // Share mapping with all other processes...
#define AEE_FMAP_PRIVATE         (0x04)   // Private mapping

#define INHERIT_IFile(iname) \
   INHERIT_IAStream(iname); \
   uint32   (*Write)(iname * pIFile, PACKED const void * pBuffer, uint32 dwCount); \
   int      (*GetInfo)(iname * pIFile, FileInfo * pInfo); \
   int32    (*Seek)(iname * pIFile, FileSeekType seek, int32 position); \
   int      (*Truncate)(iname * pIFile, uint32 truncate_pos); \
   int      (*GetInfoEx)(iname * po,AEEFileInfoEx * pi); \
   int32    (*SetCacheSize)(iname * po,int nSize); \
   void *   (*Map)(iname * po, void * pStart, uint32 dwSize, int protections,int flags, uint32 dwOffset)

QINTERFACE(IFile)
{
   INHERIT_IFile(IFile);
};

// Standard IAStream Methods

#define IFILE_AddRef(p)             GET_PVTBL(p,IFile)->AddRef(p)
#define IFILE_Release(p)            GET_PVTBL(p,IFile)->Release(p)
#define IFILE_Readable(p, pfn,pu)   GET_PVTBL(p,IFile)->Readable(p,pfn,pu)
#define IFILE_Read(p, b, c)         GET_PVTBL(p,IFile)->Read(p, b, c)
#define IFILE_Cancel(p,pfn,pu)      GET_PVTBL(p,IFile)->Cancel(p,pfn,pu)

// IFile Methods

#define IFILE_Write(p, b, c)           GET_PVTBL(p,IFile)->Write(p, b, c)
#define IFILE_GetInfo(p, pi)           GET_PVTBL(p,IFile)->GetInfo(p, pi)
#define IFILE_Seek(p, o, po)           GET_PVTBL(p,IFile)->Seek(p, o, po)
#define IFILE_Truncate(p, t)           GET_PVTBL(p,IFile)->Truncate(p, t)
#define IFILE_GetInfoEx(p,pi)          GET_PVTBL(p,IFile)->GetInfoEx(p,pi)
#define IFILE_SetCacheSize(p,n)        GET_PVTBL(p,IFile)->SetCacheSize(p,n)
#define IFILE_Map(p,ps,dws,pr,fl,dw)   GET_PVTBL(p,IFile)->Map(p,ps,dws,pr,fl,dw)

#define INHERIT_IFileMgr(iname) \
   INHERIT_IBase(iname); \
   IFile *     (*OpenFile)(iname * piname, const char *pszFile, OpenFileMode mode); \
   int         (*GetInfo)(iname * piname, const char *pszName, FileInfo * pInfo); \
   int         (*Remove)(iname * piname, const char *pszName); \
   int         (*MkDir)(iname * piname, const char *pszDir); \
   int         (*RmDir)(iname * piname, const char *pszDir); \
   int         (*Test)(iname * piname, const char *pszName); \
   uint32      (*GetFreeSpace)(iname * piname,uint32 * pdwTotal); \
   int         (*GetLastError)(iname * piname); \
   int         (*EnumInit)(iname * piname, const char *pszDir, boolean bDirs); \
   boolean     (*EnumNext)(iname * piname, FileInfo * pInfo); \
   int         (*Rename)(iname * pfm,const char * pszSrc, const char * pszDest); \
   boolean     (*EnumNextEx)(iname * pfm,AEEFileInfoEx * pInfo); \
   int         (*SetDescription)(iname * pfm, const char * pszName,AECHAR * pszDesc); \
   int         (*GetInfoEx)(iname * pfm,const char * pszName,AEEFileInfoEx * pi); \
   int         (*Use)(iname * pfm,const char * pszName,boolean bUse); \
   int         (*GetFileUseInfo)(IFileMgr *pfm, AEEFileUseInfo *pfu); \
   int         (*ResolvePath)(IFileMgr *pfm, const char *cpszIn, char *pszOut, int *pnOutLen);\
   int         (*CheckPathAccess)(IFileMgr *pfm, const char *cpszIn, uint32 dwDesiredRights, uint32 *pdwActualRights); \
   int         (*GetFreeSpaceEx)(iname * piname, const char *cpszPath, uint32 * pdwTotal, uint32 *pdwFree)


QINTERFACE(IFileMgr)
{
   INHERIT_IFileMgr(IFileMgr);
};

#define IFILEMGR_AddRef(p)                GET_PVTBL(p,IFileMgr)->AddRef(p)
#define IFILEMGR_Release(p)               GET_PVTBL(p,IFileMgr)->Release(p)
#define IFILEMGR_OpenFile(p, psz, m)      GET_PVTBL(p,IFileMgr)->OpenFile(p, psz, m)
#define IFILEMGR_GetInfo(p, psz, pi)      GET_PVTBL(p,IFileMgr)->GetInfo(p, psz, pi)
#define IFILEMGR_Remove(p, psz)           GET_PVTBL(p,IFileMgr)->Remove(p, psz)
#define IFILEMGR_MkDir(p, psz)            GET_PVTBL(p,IFileMgr)->MkDir(p, psz)
#define IFILEMGR_RmDir(p, psz)            GET_PVTBL(p,IFileMgr)->RmDir(p, psz)
#define IFILEMGR_Test(p,psz)              GET_PVTBL(p,IFileMgr)->Test(p, psz)
#define IFILEMGR_GetFreeSpace(p,pdt)      GET_PVTBL(p,IFileMgr)->GetFreeSpace(p,pdt)
#define IFILEMGR_GetFreeSpaceEx(p,psz,pdt,pdt2) GET_PVTBL(p,IFileMgr)->GetFreeSpaceEx(p,psz,pdt,pdt2)
#define IFILEMGR_GetLastError(p)          GET_PVTBL(p,IFileMgr)->GetLastError(p)
#define IFILEMGR_EnumInit(p, psz,b)       GET_PVTBL(p,IFileMgr)->EnumInit(p, psz,b)
#define IFILEMGR_EnumNext(p,pi)           GET_PVTBL(p,IFileMgr)->EnumNext(p, pi)
#define IFILEMGR_Rename(p,ps,pd)          GET_PVTBL(p,IFileMgr)->Rename(p,ps,pd)
#define IFILEMGR_EnumNextEx(p,pi)         GET_PVTBL(p,IFileMgr)->EnumNextEx(p,pi)
#define IFILEMGR_SetDescription(p,ps,psz) GET_PVTBL(p,IFileMgr)->SetDescription(p,ps,psz)
#define IFILEMGR_GetInfoEx(p,psz,pi)      GET_PVTBL(p,IFileMgr)->GetInfoEx(p,psz,pi)
#define IFILEMGR_Use(p,psz)               GET_PVTBL(p,IFileMgr)->Use(p,psz,TRUE)
#define IFILEMGR_UnUse(p,psz)             GET_PVTBL(p,IFileMgr)->Use(p,psz,FALSE)
#define IFILEMGR_GetFileUseInfo(p,pfu)    GET_PVTBL(p,IFileMgr)->GetFileUseInfo(p,pfu)
#define IFILEMGR_ResolvePath(p,i,o,ol)    GET_PVTBL(p,IFileMgr)->ResolvePath(p,i,o,ol)
#define IFILEMGR_CheckPathAccess(p,i,d,a) GET_PVTBL(p,IFileMgr)->CheckPathAccess(p,i,d,a)

#endif    // AEEFILE_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

OpenFileMode

Description:
OpenFileMode is used to specify the file opening mode when opening a file using the
IFILEMGR_OpenFile() .

Definition:
typedef enum {
   _OFM_READ=0x0001,
   _OFM_READWRITE=0x0002,
   _OFM_CREATE=0x0004,
   _OFM_APPEND=0x0008,
   _OFM_NO_BUFFER=0x8000
} OpenFileMode;

Members:

_OFM_READ : Open file in read-only mode
_OFM_READWRITE : Open file in read/write mode
_OFM_CREATE : Open file in create mode
_OFM_APPEND : Open file in append mode
_OFM_NO_BUFFER : Disable data buffering


Comments:
Option _ OFM_NO_BUFFER doesn't do anything on the BREW Simulator.
_OFM_READWRITE and _OFM_APPEND will not create a file.
_OFM_CREATE will only create a file if it did not exist prior to the
IFILEMGR_OpenFile call.

See Also:
None

=======================================================================

FileAttrib

Description:
FileAttrib specifies the type of a file.

Definition:
typedef enum {
   _FA_NORMAL=0,
   _FA_HIDDEN=0x0001,
   _FA_DIR=0x0002,
   _FA_READONLY=0x0004,
   _FA_SYSTEM=0x0008
} FileAttrib;

Members:

_FA_NORMAL : File is normal file
_FA_HIDDEN : File is a hidden file (reserved)
_FA_DIR : File is directory (reserved)
_FA_READONLY : File is read only file
_FA_SYSTEM : File is system file


Comments:
None

See Also:
None

=============================================================================

FileInfo

Description:
FileInfo is used to contain information associated with a file.

Definition:
typedef struct _FileInfo
{
   FileAttrib attrib;
   uint32 dwCreationDate;
   uint32 dwSize;
   char szName[MAX_FILE_NAME];
} FileInfo;

Members:

attrib : File attributes specified by FileAttrib .
dwCreationDate : File creation date. Elapsed time in seconds since January
                 6, 1980 00:00:00 GMT.
dwSize : File size
szName : File name

Comments:
None

See Also:
FileAttrib

=============================================================================

FileSeekType

Description:
FileSeekType is used to specify the origin of a seek operation when setting the current
file pointer position using the IFILE_Seek() .

Definition:
typedef enum {
   _SEEK_START,
   _SEEK_END,
   _SEEK_CURRENT
} FileSeekType;

Members:
_SEEK_START : Start seek from the start of the file
_SEEK_END : Start seek from the end of the file
_SEEK_CURRENT : Start seek from current file pointer position

Comments:
None

See Also:
None

=============================================================================

AEEFileInfoEx

Description:
AEEFileInfoEx is used to contain extended information associated with a file.

Definition:
typedef struct _FileInfoEx
{
   int         nStructSize;
   char        attrib;
   uint32      dwCreationDate;
   uint32      dwSize;
   char  *     pszFile;
   int         nMaxFile;
   AECHAR *    pszDescription;
   int         nDescriptionSize;
   AEECLSID *  pClasses;
   int         nClassesSize;
} AEEFileInfoEx;

Members:

nStructSize : Size of the structure
attrib      : File attributes specified by FileAttrib
dwCreationDate : File creation date
dwSize      : File size
pszFile     : ASCIIZ name to be filled with the file name of max length nMaxName
nMaxFile    : Maximum size of name field filled
pszDescription : Wide string description of the file with max size of nMaxDescription.
nDescriptionSize: Size in bytes of description
pClasses    : List of AEECLSIDs that own/use this file
nClassesSize : Size in bytes of pClasses list

Comments:
    All parameters need to be initialized. The nMaxFile, nDescriptionSize, and nClassesSize
must specify the size in bytes that the pointers, nMaxFile, pszDescription and pClasses
can hold.
    The pointers to the buffers are provided by the users of this structure and
then are filled by the functions which use it.

See Also:
FileInfo

=============================================================================

AEEFileUseInfo

Description:
AEEFileUseInfo is used to contain the file usage information.

Definition:
typedef struct _AEEFileUseInfo
{
   uint16            wMaxFiles;
   uint16            wFilesUsed;
   uint32            dwMaxSpace;
   uint32            dwSpaceUsed;
} AEEFileUseInfo;

Members:

    wMaxFiles  :  This value specifies the max number of files a module is allowed
to have in its module directory. It includes mod, sig, bar and any other data files and subsequent file creations in the module's directory.  Valid range: 7 - 65535
    wFilesUsed  :  Number of files currently used by this Module
    dwMaxSpace  :  This value specifies the maximum number of bytes a module is
allowed to consume with files in its module directory. It includes mod, sig, bar and any other data files and any subsequent file space addition in the module's directory. Valid range: 20480 - 4294967295.
    dwSpaceUsed  :  Total space currently used by this module so far

Comments:
None

See Also:
None

=======================================================================
  INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IFileMgr

Description:

The IFileMgr Interface functions are used to create, remove, and rename
files/directories. It also provides the tools to obtain information
about files and directories.

In version 3.1, the "fs:/" naming convention was introduced, allowing
uniform access to files across module directories and case-sensitive
file names.

Path names that do not begin with "fs:/" are handled according to the
following rules, which applied to all file names prior to BREW 3.1:

- All ASCII upper-case characters are converted to lower-case.

- Backslashes ("\") are converted to forward slashes ("/").

- The applet's module directory is treated as the root directory for
the lookup.  No ".." links or other names can be used to refer
to files outside the module directory, except for the following
special names: "shared", "ringers", and "addrbk".  These names refer
to special system directories, and can be used by apps that have the
appropriate privileges to share files with other apps or device
software.

Note that the above processing rules discard case information and can
corrupt text strings that are encoded using the Shift_JIS character
encoding.  Developers who want to avoid this should use the "fs:/" naming
convention, even when accessing files within the current applet's module
directory.  The prefix "fs:/~/" can be used to access files relative to
the current module directory in a case-sensitive and Shift_JIS-safe
manner.

Within the "fs:/" namespace, the following paths have special meanings:
===pre>
  "fs:/" (AEEFS_ROOT_DIR): the root of the BREW file namespace.  Prefacing
             a path with this string tells BREW to interpret the filename
             according to the new namespace rules.

  "fs:/~/" (AEEFS_HOME_DIR): this is a shortcut that denotes the
             currently running application's directory.  It is mapped
             to "fs:/mod/<modid>" where <modid> is the module ID of
             the currently running application.

  "fs:/sys/" (AEEFS_SYS_DIR): this is the location of BREW's "system"
             files.  Access to this directory is restricted,  See
             IFILEMGR_CheckPathAccess() for more information.

  "fs:/mod/" (AEEFS_MOD_DIR): the location of all the module directories

  "fs:/mif/" (AEEFS_MIF_DIR): the location of all of BREW's MIF files.

  "fs:/shared/" (AEEFS_SHARED_DIR): the new name for "Shared".  This
             name can be used to share files with other applications
             using case-sensitive names

  "fs:/address/" (AEEFS_ADDRESS_DIR): the location of the address book
             directory.  This directory is only present when the
             address book for the device is implemented using the default
             BREW address book implementation.  It is mapped mostly for
             backwards compatibility with the old BREW file
             namespace AEE_ADDRBOOK_DIR ("addrbk") in AEE.h

  "fs:/ringers/" (AEEFS_RINGERS_DIR): the location of the device's ringer
             directory.  An application must have PL_RINGER_WRITE (Write
             Access to Ringer Directory) to access files in this
             directory.

  "fs:/card0/" (AEEFS_CARD0_DIR): New for BREW 3.1, if a device supports
             a removable media card format, this path maps to the first
             instance of such a device.  An application must list
             AEECLSID_CARD0_ACCESS in its MIF in the "Dependencies"
             section to access files in this directory.

  "fs:/~<clsid>/": New for BREW 3.1, an application may access files
             in another application's directory.  <clsid> must be a
             valid class id of an application or extension.  These
             paths map to "fs:/mod/<modid>" where <modid> is the
             module id of the module that exports <clsid>.  The target
             module must export this path via a module ACL.  See
             IFILEMGR_CheckPathAccess() for more information.
===/pre>

See IFILEMGR_ResolvePath() for more information about other path
support.

An application must have a privilege level of PL_FILE (File) or
PL_SYSTEM (All) to be able to create files and directories in the
application's module directory.  An application must have a
privilege level of PL_SHARED_WRITE (Shared) or PL_SYSTEM (All) to
create files and directories in the shared application directory.
PL_SHARED_WRITE implies PL_FILE for the shared directory only.
Similar rules apply to AEECLSID_CARD0, PL_ADDRBOOK, and
PL_RINGER_WRITE.

The functions IFILEMGR_EnumInit() and IFILEMGR_EnumNext() are used to
enumerate all the directories or files in a given directory (you can
also enumerate all the files with a given file extension). You first
call IFILEMGR_EnumInit() to specify the directory in which you want to
enumerate files and directories. Each subsequent call to
IFILEMGR_EnumNext() provides information about one of the requested
directories or files, such as its name, size, creation date and
attributes. IFILEMGR_EnumNext() returns FALSE when all the specified
files or directories have been enumerated. IFILEMGR_GetInfo() returns
the same information for a particular file or directory specified by
name (you can obtain this information for an open file by calling
IFILE_GetInfo() and providing its IFile instance pointer as input).

IFILEMGR_GetFreeSpace() returns the number of free bytes available in the
device's file system.

IFILEMGR_GetLastError() returns the error code of the error (if any)
that was most recently detected by an IFileMgr function. This error
code can be used to obtain more specific information about why a
function failed to perform a requested task. IFILEMGR_Test() checks
whether a specified file or directory exists.

IFILEMGR_MkDir() creates a new directory, specified by its name and
path relative to the directory of the applet that is making the call.

IFILEMGR_RmDir() removes a directory (you must first remove all the
files and directories in the directory to be removed).

IFILEMGR_OpenFile() is used to create a new file, or to open an
existing file for reading and/or writing. The file is specified by its
name and its path relative to the applet's
directory. IFILEMGR_OpenFile() returns an IFile instance pointer for
the opened file. This pointer is provided as input to functions in the
IFile Interface that are used to read and write the file's
contents. You close an open file by calling IFILE_Release() with this
pointer as its parameter.

IFILEMGR_Rename() is used to rename a file and IFILEMGR_Remove()
removes a file.  A file must be closed before it can be renamed or
removed. If the file has been opened more than once, each open must be
matched by a close before the rename or remove can succeed.
===H2>
To use the functions in the IFileMgr Interface:
===/H2>
 
1. Call ISHELL_CreateInstance() to create an instance of the IFileMgr Interface.
2. Call the functions listed above to obtain information about the files and
directories that are present in your application's directory, or to determine
the amount of free space available in the file system.
3. Call IFILEMGR_MkDir() and IFILEMGR_RmDir() to create and remove directories,
and call IFILEMGR_OpenFile(), IFILEMGR_Rename(), and IFILEMGR_Remove()
to create, rename and remove individual files.
===H2>
To read and/or modify the contents of a file:
===/H2>
 
1. Call IFILEMGR_OpenFile() to open the file for reading or writing.
2. Call functions in the IFile Interface to access the file's contents.
3. Call IFILE_Release() to close the file when you have completed accessing it.
4. Call IFILEMGR_Release() when you no longer need the IFileMgr Interface instance.

The following header file is required:~
   AEEFile.h

=======================================================================

IFILEMGR_AddRef()

Description:
    This function increments the reference count of the IFileMgr Interface
object

Prototype:
    uint32 IFILEMGR_AddRef(IFileMgr * pIFileMgr)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object

Return Value:
    Returns the updated reference count.

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_Release()

=======================================================================

IFILEMGR_EnumInit()

Description:
    This function initializes the IFileMgr Interface object for interactively
using the IFILEMGR_EnumNext() operation on the files or directories in the
specified directory.

Prototype:
    int IFILEMGR_EnumInit(IFileMgr * pIFileMgr, const char * pszDir, boolean bDirs)

Parameters:
    pIFileMgr :  Pointer to the IFileMgr Interface object
    pszDir : NULL terminated string containing the root directory name.
             "" is a valid file name and refers to the application's ROOT directory.
    bDirs : Enumeration type that takes the following values -
            TRUE - enumerate directories only
            FALSE - enumerate files only

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_EnumNext()

=======================================================================

IFILEMGR_EnumNext()

Description:
    This function returns file information for the next file/directory
within the specified root directory of the IFileMgr Interface object.

Prototype:
    boolean IFILEMGR_EnumNext(IFileMgr * pIFileMgr, FileInfo * pInfo)

Parameters:
    pIFileMgr : [in] :  Pointer to the IFileMgr Interface object
    pInfo : [out] : placeholder for file information

Return Value:
    TRUE : If successful
    FALSE : If unsuccessful

Comments:
    The file information this function retrieves contains the file
attributes, file creation date and file size.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_EnumInit()
    FileInfo

=======================================================================

IFILEMGR_GetFreeSpace()

Description:
    This function gets the number of free bytes currently available on
file system. If the passed parameter pdwTotal is non-NULL, this function
initializes pdwTotal with the total room in the file system.

Prototype:
    uint32 IFileMgr_GetFreeSpace(IFileMgr * pIFileMgr, uint32 * pdwTotal)

Parameters:
    pIFileMgr : [in] : Pointer to the IFileMgr Interface object
    pdwTotal : [out] : Placeholder for the total room in the file system.

Return Value:

Number of bytes of file system space currently available on the main
filesystem on the device. Never returns an error code. To distinguish 
the case when free space is really zero from a case where the call 
IFILEMGR_GetFreeSpace(IFileMgr * pIFileMgr, uint32 * pdwTotal) failed
- check the value returned in pdwTotal. If both *pdwTotal and return
value are zero it means the call failed. Else there should be a valid, 
non zero value in *pdwTotal.


Comments:
If the space available on the Simulator is larger than the maximum 32-bit
integer value, the maximum value of a 32-bit integer is returned(0xFFFFFFFF).

Side Effects:
None

Version:
   Introduced BREW Client 1.0

See Also:
IFILEMGR_GetFreeSpaceEx()

=======================================================================

IFILEMGR_GetFreeSpaceEx()

Description:
    This function gets the number of free bytes currently available on
file system. If the passed parameters pdwTotal and pdwFree are non-NULL, 
this function initializes pdwTotal with the total room in the file system
and pdwFree with the free space in the file system. Parameter cpszPath
indicates the file system (main flash, peripheral cards, etc) for which
the file space details are required. If the cpszPath points to the main
file system on the device, it is identical to IFILEMGR_GetFreeSpace().
If the cpszPath points to a peripheral card (say MMC), (using fs:/card0/),
the returned parameters are related to the file system on that card.

Prototype:
    int IFileMgr_GetFreeSpaceEx(IFileMgr * pIFileMgr, const char * cpszPath,
                              uint32 * pdwTotal, uint32 * pdwFree)

Parameters:
    pIFileMgr : [in] : Pointer to the IFileMgr Interface object
    cpszPath : [in] : Pointer a path string that specifies the file system 
                      whose space details are required. 
    pdwTotal : [out] : Placeholder for the total room in the file system.
    pdwFree : [out] : Placeholder for the available space in the file system.

Return Value:
    SUCCESS  : if successful
    ENOMEDIA or EUNSUPPORTED : if MMC card is not present (or in the case 
       of simulator, the MMC card is not provisioned).

Comments:
If the space available on the Simulator is larger than the maximum 32-bit
integer value, the maximum value of a 32-bit integer is returned(0xFFFFFFFF).
If cpszPath points to the regular file system (not MMC card) the function 
always returns SUCCESS, however pdwFree is 0 in case of an error.

Side Effects:
None

Version:
   Introduced BREW Client 3.1.3

See Also:
IFILEMGR_GetFreeSpace()

=======================================================================

IFILEMGR_GetInfo()

Description:
    This function gets the information on a file identified by a file name.
The file information this function retrieves contains the file attributes,
file creation date, and file size.

Prototype:
    int IFILEMGR_GetInfo(IFileMgr * pIFileMgr, const char * pszName, FileInfo * pInfo)

Parameters:
    pIFileMgr : [in] : Pointer to the IFileMgr Interface object
    pszName : [in] : Null terminated string containing the file name.
                     "" is a valid file name and refers to the application's
                     ROOT directory.
    pInfo : [out] : placeholder for file information

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_GetInfo(), FileInfo

=======================================================================

IFILEMGR_GetLastError()
Description:
    This function would typically be called when a file operation performed
by the applet has failed and the applet needs to know about the reason for
the failure.

Prototype:
    int IFILEMGR_GetLastError(IFileMgr * pIFileMgr)

Parameters:

    pIFileMgr : Pointer to the IFileMgr Interface object

Return Value:
    SUCCESS : If last file operation was successful~
    If unsuccessful, one of the following errors return:~
~
    EFILEEXISTS : File exists~
    EFILENOEXISTS : File does not exist~
    EDIRNOTEMPTY : Directory not empty~
    EBADFILENAME : Bad file name~
    EBADSEEKPOS : Bad seek position~
    EFILEEOF : End of file~
    EFSFULL : File system full~
    EFILEOPEN : File already open~
    EBADPARM : Invalid parameter~
*
Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IFILEMGR_MkDir()

Description:
    This function is used to create a directory, specified by a directory
name.

Prototype:
    int IFILEMGR_MkDir(IFileMgr * pIFileMgr, const char * pszDir)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object
    pszDir : NULL terminated string containing a directory name

Return Value:
    SUCCESS : If the file is successfully created or if file already exists
    EFAILED : If unsuccessful. To get detailed error code call 
              IFILEMGR_GetLastError(). An example of detailed error code is
              EFSFULL i.e. Either there is no space in the file system, or the 
              module is trying to exceed the maximum number of files/dirs
              allowed to create by the module.

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_RmDir()

=======================================================================

IFILEMGR_OpenFile()

Description:
    This function is used to open a file in specified mode.

Prototype:
    IFile * IFILEMGR_OpenFile(IFileMgr * pIFileMgr, const char * pszFile, OpenFile mode)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object
    pszFile : Null terminated string containing the file name
    mode : File open mode
           The file open mode takes the following values:
    _OFM_READ : Open in read mode
    _OFM_READWRITE : Open in read/write mode
    _OFM_CREATE : Create
    _OFM_APPEND : Open for appending

Return Value:
    Pointer : To the IFile Interface object of the file opened If successful,
    NULL : If the function call fails

Comments:
    If NULL pointer is returned, use IFILEMGR_GetLastError() to get error details.
To close a file that has been opened using the IFILEMGR_OpenFile(), release the
IFile Interface pointer using the IFILE_Release(). If pszFile contains the path
where the file exists (or needs to be created), the file is opened (or created) if
the caller has the privileges to open/create files in that directory, and the
directory exists. Directories can be created using the IFILEMGR_MkDir(). Files are
created with read/write access.

    If IFILEMGR_OpenFile() is called with _OFM_READWRITE or _OFM_APPEND, NULL is
returned if the file does not exist.  An open mode of _OFM_CREATE will return an
error if the file already exists; call IFILEMGR_Remove() to remove a file, then
create it with IFILEMGR_OpenFile() with the _OFM_CREATE mode.

   If IFILEMGR_GetLastError() returns EFSFULL, then either there is no space in the
file system or the module is trying to exceed the maximum number of files/dirs allowed
to create by the module.

Side Effects:
    If the file is opened in the _OFM_APPEND mode the file pointer is
moved to the end of the file.

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_MkDir()

=======================================================================

IFILEMGR_Release()

Description:
    This function decrements the reference count for the IFileMgr Interface
object and does appropriate cleanup if the reference count reaches 0 (zero).

Prototype:
    uint32 IFILEMGR_Release(IFileMgr * pIFileMgr)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object

Return Value:
    Returns the updated reference count.

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_AddRef()

=======================================================================

IFILEMGR_Remove()

Description:
    This function is used to remove a file identified by a given filename.

Prototype:
    int IFILEMGR_Remove(IFileMgr * pIFileMgr, const char * pszName)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object
    pszName : Null terminated string containing the name of the file to to
              be removed

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    A file needs to be closed prior to it being removed. Use IFILE_Release()
to close a file.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IFILEMGR_Rename()

Description:
    This function renames the file from the source name to the destination name.

Prototype:
    int IFILEMGR_Rename(IFileMgr * pIFileMgr, const char * pszSrc, const char * pszDest)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object
    pszSrc : Source file to be renamed
    pszDest : Destination file

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    If EFAILED is returned, use IFILEMGR_GetLastError() to get the error details.

   Renaming directories is not supported; this function only works for files.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_EnumNext()

=======================================================================
IFILEMGR_RmDir()

Description:
    This function is used to remove a directory identified by a given
directory name.

Prototype:
    int IFILEMGR_RmDir(IFileMgr * pIFileMgr, const char * pszDir)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object
    pszDir : NULL terminated string containing a directory name

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    If there are files or directories elements beneath the directory to
be removed, they should be removed prior to calling this function, or this
function call fails and the directory is not removed.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_MkDir()

=======================================================================

IFILEMGR_Test()

Description:
    This function tests if the specified file/directory exists.
    For directories only directories on a real filesystem are 
    considered, directories in the path for a added const file are 
    not considered.

Prototype:
    int IFILEMGR_Test(IFileMgr * pIFileMgr, const char * pszName)

Parameters:
    pIFileMgr : Pointer to the IFileMgr Interface object
    pszName : NULL terminated string containing file/directory name.
              "" is a valid file name and refers to the application's
              ROOT directory.

Return Value:
    SUCCESS : If specified file/directory exists
    EFAILED : If otherwise

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IFILEMGR_EnumNextEx()

Description:
    This function returns extended file information for the next file/directory
within the specified root directory of the IFileMgr Interface object.

Prototype:
    boolean IFILEMGR_EnumNextEx(IFileMgr * pIFileMgr, AEEFileInfoEx * pInfo)

Parameters:
    pIFileMgr : [in] :  Pointer to the IFileMgr Interface object
    pInfo : [out] : placeholder for extended file information

Return Value:
    TRUE : If successful
    FALSE : If unsuccessful

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IFILEMGR_EnumInit()
   AEEFileInfoEx

=======================================================================

IFILEMGR_SetDescription()

Description:
    This function sets the file description that is part of the 
    extended file information. This API verifies that the application
    has the WRITE privilege to the file path name passed to this API. 
    It then updates an internal database (which itself is a file) to 
    associate provided description text with the provided file name. 
    The operation is not provided by the device filesystem (and 
    hence no call for this specific operation will show up below the
    BREW layer). It will not verify if the file is present or not on
    the file system, the application can check file presense if it 
    wants.

Prototype:
   int IFILEMGR_SetDescription(IFileMgr * pfm, const char * pszName, AECHAR * pszDesc);

Parameters:
    pfm        : [in] : Pointer to the IFileMgr Interface object
    pszName    : [in] : NULL terminated string containing a file name
    pszDesc    : [in] : Wide character string containing the description

Return Value:
    SUCCESS: If successful
    EFAILED : If unsuccessful
    ENOMEMORY: Insufficient memory

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IFILEMGR_EnumInit()
   FileInfo

=======================================================================

IFILEMGR_GetInfoEx()

Description:
    This function gets the extended information of a file identified by a file name.
The extended file information is contained in AEEFileInfoEx structure.

Prototype:
   int IFILEMGR_GetInfoEx(IFileMgr * pfm, const char * pszName, AEEFileInfoEx * pi);

Parameters:
   pfm        : [in] : Pointer to the IFileMgr Interface object
   pszName    : [in] : NULL terminated string containing a file name
   pi         : [out]: Placeholder for extended file information

Return Value:
   SUCCESS: If successful
   EFAILED : If unsuccessful
   ENOMEMORY: Insufficient memory

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   AEEFileInfoEx
   IFILEMGR_GetInfoEx()

=======================================================================

IFILEMGR_Use()

Description:
    This function adds class ID of the calling app to the list of classes using
    the specified file. The specified file will not be deleted until the last
    app indicates that the file is no longer used or IFILEMGR_Remove() is 
    called.

Prototype:
   int IFILEMGR_Use(IFileMgr * pfm,const char * pszName);

Parameters:
    pIFileMgr  : [in] : Pointer to the IFileMgr Interface object
    pszName    : [in] : NULL terminated string containing a file name

Return Value:
    SUCCESS: If successful
    EFAILED : If unsuccessful
    ENOMEMORY: Insufficient memory

Comments:
   Currently, files in shared directory are tracked. This API applies to files
   in the shared directory only. If an application calls IFILEMGR_Use() on a 
   file not in the shared area, it is not guaranteed that the file will be 
   present in the filesystem till the application is interested in the file.


Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IFILEMGR_UnUse()
   IFILEMGR_GetInfoEx()
   AEEFileInfoEx

=======================================================================

IFILEMGR_UnUse()

Description:
    This function removes the class ID of the calling app from the list of classes using
    the specified file. The specified file will be deleted when the last
    app indicates that the file is no longer used or IFILEMGR_Remove() is called.

Prototype:
   int IFILEMGR_Use(IFileMgr * pfm,const char * pszName);

Parameters:
    pIFileMgr  : [in] : Pointer to the IFileMgr Interface object
    pszName    : [in] : NULL terminated string containing a file name

Return Value:
    SUCCESS: If successful
    EFAILED : If unsuccessful
    ENOMEMORY: Insufficient memory

Comments:
   Currently, files in shared directory are tracked. This API applies to files
   in the shared directory only.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IFILEMGR_Use()
   IFILEMGR_GetInfoEx()
   AEEFileInfoEx

=======================================================================

IFILEMGR_GetFileUseInfo()

Description:
    This function gets the file usage infomation for the module

Prototype:
   int IFILEMGR_GetFileUseInfo(IFileMgr * pfm, AEEFileUseInfo * pfu);

Parameters:
    pfm  : [in] : Pointer to the IFileMgr Interface object
    pfu  : [out]: Placeholder for file usage information

Return Value:
    SUCCESS: If successful
    EFAILED : If unsuccessful
    ENOMEMORY: Insufficient memory

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   AEEFileUseInfo

=======================================================================

IFILEMGR_ResolvePath()

Description:
   This function resolves a filesystem path to the BREW canonical form.

Prototype:
   int IFILEMGR_ResolvePath(IFileMgr * pfm, const char *cpszIn,
                            char *pszOut, int *pnOutLen)

Parameters:
    pfm    : Pointer to the IFileMgr Interface object
    cpszIn : the path to be resolved
    pszOut : [out] the output buffer, holds the canonical path, may
              be NULL
   pnOutLen: [in/out] On entry, this provides the maximum number of
              bytes to write into pszOut[].  On exit, this holds the
              number of bytes written to pszOut[].  When pszOut is NULL,
              this input value is ignored and the value on exit will
              be the number of bytes required to hold the resolved path.

Return Value:
    SUCCESS: If successful
    EBUFFERTOOSMALL: if pszOut is non-NULL, but *pnOutLen cannot hold
                    the resolved path
    EBADFILENAME: if the path cannot be resolved or is NULL

Comments:

   Starting with BREW 3.1, the canonical BREW file namespace begins
   with the string "fs:/" and allows addressing of the entire BREW
   filesystem.  Previously, dynamic applications were restricted to
   only naming (and therefore accessing) files and directories under
   their module directory, with the exception of the "shared"
   directory, "ringers" directory and possibly the "addrbk" directory.

   With BREW 3.1, dynamic applications may name anything, but the
   same access restrictions as before, as well as some new access
   restrictions may apply.

   This function maps old names (those that don't begin with "fs:/")
   to new names, and resolves the BREW file namespace shortcuts
   "fs:/~/..." and "fs:/~<clsid>/" to canonical "fs:/" paths.
   Additionally, the directory navigation entities "." and ".." are
   now supported and resolved by this function.

   This function is used by all the other IFILEMGR APIs to resolve
   pathname parameters before accessing the filesystem, so anything
   resolveable by this function is resolveable by, say
   IFILEMGR_OpenFile().

   The output string (if pszOut is non-NULL) is guaranteed to be
   null-terminated.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IFILEMGR_CheckPathAccess()

=======================================================================

IFILEMGR_CheckPathAccess()

Description:
   This function checks whether the caller has certain rights
     on the input path.

Prototype:
   int IFILEMGR_CheckPathAccess(IFileMgr * pfm, const char *cpszIn,
                                uint32 dwDesiredRights,
                                uint32 *pdwActualRights)

Parameters:
   pfm             : Pointer to the IFileMgr Interface object
   cpszIn          : the path to be checked
   dwDesiredRights : the desired rights, a bitwise-or of the following:
~
      AEEFP_READ - access to read-path is desired~
      AEEFP_WRITE - access to write-path is desired ~
      AEEFP_CREATE - access to create-path is desired~
      AEEFP_DELETE - access to delete-path is desired~
*
   pdwActualRights : [out] filled with the actual rights,
                     ignored if NULL

Return Value:
    SUCCESS: If successful
    EPRIVLEVEL: if access is denied
    EBADFILENAME: if the path cannot be resolved or checked ~
                (for example, if access to a non-existent module's
                 directory is desired)

Comments:

   Starting with BREW 3.1, the canonical BREW file namespace begins
   with the string "fs:/" and allows addressing of the entire BREW
   filesystem.  Previously, dynamic applications were restricted to
   only naming (and therefore accessing) files and directories under
   their module directory, with the exception of the "shared"
   directory, "ringers" directory and possibly the "addrbk" directory.

   With BREW 3.1, dynamic applications may name anything, but the
   same access restrictions as before as well as some new access
   restrictions may apply.  First and foremost, a module's permissions
   must include PL_FILE to be able to modify the BREW filesystem.

   Modules may "publish" or export files in their home directories.
   As of BREW 3.1, BREW looks for the RESTYPE_BINARY resource
   IDB_MIF_FS_ACLS ("ACLs" in MIF editor) in each module's MIF to resolve
   which files are published with what permissions.

   In this resource, BREW expects to find a doubly-NULL-terminated
   string with the following syntax:
===pre>

       ACLs          = (ACL '\0')+ '\0'
       ACL           = assignperms ":" path
       path          = <single-byte string>
       assignperms   = assignperm [ ";" assignperms ]
       assignperm    = groups "=" exactperms "/" subtreeperms
       groups        = group [ "," group ]
       group         = <hex number>
       exactperms    = perms
       subtreeperms  = perms
       perms         = [+-] [r] [w]

===/pre>
   For example, the following ACL:
===pre>
     0x0=rw/rw:/
===/pre>

   Allows full access to everybody to the module's home directory.
   The ACL has one assignperms field, which has one group: 0 (the
   special group 0 is reserved to mean "all groups").  The exactperms
   "rw" actually grants delete(!) access to the entry in question.

   Let's consider another example:
===pre>
     0x0=r/rw:/
===/pre>

   This one differs from the one above in that exactperms is without
   "w".  This ACL says: anybody can read my homedir (enumerate it),
   and anybody can read, write, create or delete anything under it.
   This is probably the best "public" directory ACL.

   Here are BREW's FS ACLs (which are considered relative to "fs:/"):
===pre>

       const char *cpszzFSACL =
         "0x00000000 =  r /    :/"             "\0" // all groups to read fs:/, but nothing under fs:/
         "0x00000000 =  r /  r :/mif"          "\0" // all groups to read fs:/mif
         "0x00000000 =  r /    :/mod"          "\0" // all groups to read fs:/mod, but not under fs:/mod

         "0x00000020 = rw / rw :/sys/download" "\0" // PL_DOWNLOAD to read/write fs:/sys/download

         "0x00000000 =  r /  r ;"                   // all groups to read fs:/shared
         "0x00000040 =  - / +w :/shared"       "\0" // PL_SHARED_WRITE to write under fs:/shared

         "0x00000100 =  r / rw :/ringers"      "\0" // PL_RINGER_WRITE to write under fs:/ringers

         "0x00000200 =  r / rw :/address"      "\0" // PL_ADDRBOOK read fs:/address, all access under fs:/addresss

         "0x0101414E =  r / rw :/card0"        "\0" // CARD0_ACCESS read fs:/card0, all access under fs:/card0
         ;
===/pre>

   Notice that left of the colon, whitespace is ignored.  Empty
   permissions sections means assigment to "no permission".  A
   permission section of just "-" means no modification to
   permissions.

   What if you want to grant everybody read and write to a particular
   subdirectory, but you want to make a particular part of that
   directory "super-secret-choco-fudgy-batcave" (full access private
   to just your "group")?  You need a separate ACL for each subtree...
   So:
===pre>
     0x0=r/rw:/public
     0x0=/;0x00C110C0=rw/rw:/public/choco
===/pre>

   The first rule says: "make public public", very much like the
   above.  The second rule, however, overrides the first rule (the
   rules are considered in order).  The second rule denies everybody
   access except apps in the group "00C110C0", which are granted full
   access to /public/choco (including deletion of /public/choco
   itself).

   Since the ACLs are considered in order, they also include the nice
   feature of +-.  We can use this to re-express our choco example:
===pre>
     0x0=r/rw:/public
     0x0=-r/-rw;0x00C110C0=+rw/+rw:/public/choco
===/pre>

   The second rule is written as a modification to the first rule:
   deny access to everybody and add write access for 00C110C0.

   NOTES:

- Paths must start with a "/" to be considered by the ACL parsing
      code.  Without it, no match with any BREW path will succeed.

- A '+' and '-' are only legal as the first non-white character of
      the perms fields.

- Names need not be directories, though when they're not, the
      subtreeperms field is irrelevant.

- An application becomes part of a group by listing the group in
      its "Imported Classes".  Every application is also in its own
      group (the group equal to the application's classid).  See
      ISHELL_CheckPrivLevel() for more information.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IFILEMGR_ResolvePath(), ISHELL_CheckPrivLevel()


======================================================================
  INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IFile

Description:
    The IFile Interface functions allow you to read and modify the contents
of files created with the IFileMgr Interface . To obtain an instance of the
IFile Interface for a file, you call IFILEMGR_OpenFile() for that file. You
then use the IFile Interface pointer returned by IFILEMGR_OpenFile() to
access that file with the operations described below. When you have completed
access to the file, you call IFILE_Release() to close it.

CAUTION: Your application must have a privilege level of File or All to be
able to modify files with IFILE_Write() or IFILE_Truncate() . Your application
must have a privilege level of Shared or All to invoke these functions on
files in the shared application directory.

The function IFILE_GetInfo() returns information about an open file, including
its name, size and creation timestamp (if the file is not open, you can obtain
the same information by calling IFILEMGR_GetInfo() with the name of the file
as input).

IFILE_Read() reads a specified number of bytes from the file and copies them
into a character buffer in your application. IFILE_Write() writes a number of
bytes to the file from a character buffer that contains the data to be written.
All read and write operations are with respect to the current file cursor,
which determines which byte of the file is to be read or written next. Initially,
the cursor points to the first byte of the file (unless it was opened for
appending, in which case the cursor points to a position just past the last byte
of the file). Each file read and write sets the cursor to the position just past
the last byte read or written.

You can use IFILE_Seek() to set the cursor to a particular position in the file
and to obtain the current value of the cursor, which allows you to read from
and write to any position in the file.

The function IFILE_Truncate() allows you to reduce the size of a file, which
discards the contents of the truncated portion at the end of the file.
===H2>
To use functions in the IFile Interface:
===H2>
 

1. Call ISHELL_CreateInstance() if necessary to obtain an instance of the IFileMgr
Interface.
2. If the file you wish to access does not exist yet,
- Call IFILEMGR_OpenFile() to create the file, supplying the file's name as input.
Upon creation, the file is open for reading and writing.
-Otherwise, call IFILEMGR_OpenFile() to open the file for reading, reading and
writing, or appending, based on the type of access you need.

3. Using the IFile pointer obtained in step 2, call IFILE_Seek() if necessary to
position the file cursor, and then call IFILE_Read() and IFILE_Write() to read
 from and write to the file as needed.
4. Call IFILE_GetInfo() if you need to obtain information about the file, and call
IFILE_Truncate() to truncate it.
5. Call IFILE_Release() to close the file when you have completed accessing it.

The following header file is required:~
   AEEFile.h

=======================================================================

IFILE_AddRef()

Description:
    This function increments the reference count of the IFile Interface
object. This allows the object to be shared by multiple callers. The object
is freed when the reference count reaches 0 (zero).

Prototype:
    uint32 IFILE_AddRef(IFile * pIFile)

Parameters:
    pIFile : Pointer to the IFile Interface object

Return Value:
    Incremented reference count for the object.

Comments:
    A valid object returns a positive reference count.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILE_Release()

=======================================================================

IFILE_Cancel()

Description:
    This function cancels the callback registered with this IFile interface
object with IFILE_Readable() .

Prototype:
    void IFILE_Cancel(IFile * pIFile, PFNNOTIFY pfn, void * pUser)

Parameters:
    pIFile : Pointer to IFile interface object for which the registered
             callback function needs to be cancelled.
    pfn : Address of the callback function.
    pUser : User defiend data that will be passed to the callback function
            when it is invoked.

Return Value:
    None

Comments:
    This function doesn't care for the values of pfn and pUser parameters.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILE_Readable()

=======================================================================

IFILE_GetInfo()

Description:
    This function gets the file creation date, file size, filename, and
file attributes of the file pointed to by the IFile Interface object.

Prototype:
    int IFILE_GetInfo(IFile * pIFile, FileInfo * pInfo)

Parameters:
    pIFile : [in] : Pointer to IFile Interface object
    pInfo : [out] : placeholder for file information

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    The file information is returned in FileInfo structure provided by pInfo.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_GetInfo()
    FileInfo

=======================================================================

IFILE_Read()

Description:
    This function reads a specified number of bytes from an open file. The
read operation is blocking.

Prototype:
    int32 IFILE_Read(IFile * pIFile, void * pBuffer, uint32 dwCount)

Parameters:
    pIFile : [in] : Pointer to IFile Interface object
    pBuffer : [out] : Buffer into which the file data is read
    dwCount : [in] : Number of bytes to be read

Return Value:
    Number of bytes read : If successful~
    0 (zero) : If unsuccessful

Comments:
    To read data from a file, the file needs to be open. See IFILEMGR_OpenFile()
function of the IFileMgr Interface for more details on opening a file. The bytes
are read, starting from the location of the file pointer in the IFile Interface
object. The file pointer within a file can be relocated using the IFILE_Seek()
function.

Side Effects:
    When the read operation is over, the file pointer of the IFile Interface object
points to the end of the block of bytes that were read.

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_OpenFile(),
    IFILE_Write(),
    IFILE_Seek()

=======================================================================

IFILE_Readable()

Description:
    This function is used for registering a callback function which tries
to read from the file at a later time.

Prototype:
    void IFILE_Readable(IFile * pIFile, PFNNOTIFY pfn, void * pUser)

Parameters:
    pIFile : Pointer to IFile interface object for which the callback
             function needs to be registered.
    pfn : Address of the callback function.
    pUser : User defiend data that will be passed to the callback function
            when it is invoked.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILE_Cancel()

=======================================================================

IFILE_Release()

Description:
    This function decrements the reference count of the IFile Interface
object. If the reference count reaches 0 (zero), the file associated with
the IFile Interface object closes.

Prototype:
    uint32 IFILE_Release(IFile * pIFile)

Parameters:
    pIFile : Pointer to the IFile Interface object

Return Value:
    The updated reference count.

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILE_AddRef()
    IFILEMGR_OpenFile()

=======================================================================

IFILE_Seek()

Description:
    This function moves the file pointer of the IFile Interface object a
given distance from a specified origin.

Prototype:
    uint32 IFILE_Seek(IFile * pIFile, FileSeekType seekType, int32 moveDistance)

Parameters:
    pIFile : Pointer to IFile Interface object
    seekType : [_SEEK_CURRENT | _SEEK_START | _SEEK_END]
    moveDistance : Distance to move

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

    In the special case when the seek type is _SEEK_CURRENT and the move distance
    is 0 (zero), the position of the current file pointer is returned.

Comments:
    The specified origin ( FileSeekType ) can take three values:
~
    _SEEK_CURRENT~
    _SEEK_START~
    _SEEK_END~
*
    IFILE_Seek() does NOT guarantee space reservation on the filesystem when seek is done 
    for a position beyond the current file EOF. This is because the device filesystem which 
    actually does the seek operation on the file may not physically increase the file size 
    when the seek pointer is set beyond EOF. The meta-data is updated to reflect the new file 
    size which results from of seek operation but the device free space for filesystem does 
    not change unless the file is actually written to beyond EOF.

    Another side effect of underlying device filesystem is that if the device filesystem 
    actually increases the file size on seeks beyond EOF, then a large value of seek offset 
    may be prone to dog timeouts as is large values for file writes. If the device filesystem 
    does not increase physical file size then large values of seeks are fine and the call to 
    IFILE_Seek()returns immediately updating the meta data for the file (it may be possible to 
    increase file size to more than device filesystem space in this case, but remember this is 
    just in meta-data and not the actual file size on the device)

    The move distance can be specified in positive or negative directions (values).
    For example, negative direction is used when seeking from the end of the file
    (_SEEK_END). If the seek type is _SEEK_CURRENT and move distance is 0 (zero),
    this acts as a tell operation and returns the current position of the file pointer.
    Otherwise, the file pointer is moved by the given distance from the specified origin. If the
    file was opened in the _OFM_READ mode, this operation succeeds if the move
    distance from the specified origin is within the boundaries of the file.
    See IFILEMGR_OpenFile() function description to find out more on the file open modes.
    If the file was opened in the _OFM_READWRITE mode and the move distance points
    to the location before the beginning of the file, this operation fails. If the move
    distance from the specified origin is past the end of the file, the file size
    is extended to the point of the move distance (from the seek origin).

For example,~
    a file is created in READWRITE mode, and the file size if 40 bytes. The
    IFILE_Seek() function is used with the seek type of _SEEK_START, and a move distance
    of 120. This causes the file size to increase to 120 bytes, and the file pointer
    points to the end of the file.

Side Effects:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_OpenFile()

=======================================================================

IFILE_Truncate()

Description:
    This function truncates the file specified by the IFile Interface object
 to position specified by the truncate_pos parameter.

Prototype:
    int IFILE_Truncate(IFile * pIFile, uint32 truncate_pos)

Parameters:
    pIFile : Pointer to IFile Interface object
    truncate_pos : Truncate position

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    To truncate a file, the file needs to be open for writing (via
    _OFM_APPEND, _OFM_CREATE, or _OFM_READWRITE)

Side Effects:
    After the file is truncated, the file pointer is moved to the end
    position of the file.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================

IFILE_GetInfoEx()

Description:
    This function gets the extended information of the file identified.
    The extended file information is contained in AEEFileInfoEx structure.

Prototype:
   int IFILE_GetInfoEx(IFile * pIFile, AEEFileInfoEx * pi);

Parameters:
   pIFile : Pointer to IFile Interface object
   pi     : [out]: Placeholder for extended file information

Return Value:
   SUCCESS: If successful
   EFAILED : If unsuccessful
   ENOMEMORY: Insufficient memory

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   AEEFileInfoEx
   IFILEMGR_GetInfoEx()

=======================================================================

IFILE_SetCacheSize()

Description:
    This function sets the size for file-caching operations.  This allows
callers to selectively buffer file access to improve performance.

Prototype:
    int32 IFILE_SetCacheSize(IFile * pIFile, int32 nSize)

Parameters:
    pIFile : Pointer to IFile Interface object
    nSize:  Size of cache.
~
   SCS_DEFAULT - Use default file cache size specified by OEM~
   SCS_MIN - Minimum cache size set by OEM~
   SCS_MAX - Maximum cache size set by OEM~
   SCS_BEST - Use file size if between min and max specified by OEM~
*

Return Value:
    Size of the cache.  If the input size == 0, the cache is freed.

Side Effects:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=======================================================================

IFILE_Write()

Description:
    This function writes the specified number of bytes to an open file.
The file must have been open by the IFileMgr when creating the file object.
The operation is blocking.

Prototype:
    uint32 IFILE_Write(IFile * pIFile, PACKED const void * pBuffer, uint32 dwCount)

Parameters:
    pIFile : [in] : Pointer to IFile Interface object
    pBuffer : [in] : Buffer from which the data is written
    dwCount : [in] : Number of bytes to be written

Return Value:
    Number of bytes written : If successful~
    0 (zero) : If unsuccessful

Comments:
    To write data to a file, the file needs to be open in the read/write mode.
IFILEMGR_OpenFile() function of the IFileMgr Interface for more details on opening
a file. The bytes are written, starting from the location of the file pointer in
the IFile Interface object. The file pointer within a file can be relocated using
the IFILE_Seek() function.

Side Effects:
    When the write operation is over, the file pointer of the IFile Interface object
points to the end of the block of bytes that were written.


Version:
   Introduced BREW Client 1.0

See Also:
    IFILEMGR_OpenFile(),
    IFILE_Read()


=====================================================================

IFILE_Map()

Description:
    This function maps a file specified (with the offset) into memory.
	Memory mapping is supported for const files only and the memory address 
	for mapping the file cannot be specified by the user.

Prototype:
    void* IFILE_Map(IFile* pIFile, void* pStart, uint32 dwSize, 
	                int protections, int flags, uint32 dwOffset)
    
Parameters:
    pIFile       : [in] : Pointer to IFile Interface object
	pStart       : [in] : Should be NULL (zero)
	dwSize       : [in] : Size of the mapping in bytes. dwSize should not be greater 
	                      than the file size remaining from dwOffset.
	protections  : [in] : Determines whether read, write, execute, or some combination
	                      of accesses are permitted to the data being mapped.
						  Write access (AEE_FMAP_PROT_WRITE) is not permitted in current 
						  implementation.
	flags        : [in] : Provides information about the handling of the mapped data.
	                      Only shared flag (AEE_FMAP_SHARED) is supported currently.
	dwOffset     : [in] : Offset of bytes from start of the file to be mapped.

Return Value:
    If Successful  : memory address for the mapping requested.
    If unsuccessful: NULL (zero) 

Comments:
    To map file to memory, the file needs to be a const file. Refer
    IFILEMGR_OpenFile() function of the IFileMgr Interface for more details on opening
    a file. The mapping does not support AEE_FMAP_PROT_WRITE protection. 
    Only the AEE_FMAP_SHARED flag is supported. 
    The pStart parameter should be NULL (zero).

Side Effects:
    None.

Version:
   Introduced BREW Client 3.1

See Also:
    IFILEMGR_OpenFile()


=====================================================================
*/
