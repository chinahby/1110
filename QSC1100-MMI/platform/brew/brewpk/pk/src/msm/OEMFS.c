/*=============================================================================

FILE: OEMFS.c

SERVICES: OEM Functions for Embedded File System

GENERAL DESCRIPTION:
A sample implementation of the OEMFS functions declared in
OEMFS.h that are used in the implementation of AEEFile.
  
PUBLIC CLASSES AND STATIC FUNCTIONS:

   OEMFS_Open
   OEMFS_Close
   OEMFS_Read
   OEMFS_Write
   OEMFS_WriteEx
   OEMFS_GetAttributes
   OEMFS_GetOpenFileAttributes
   OEMFS_Seek
   OEMFS_SeekEx
   OEMFS_Tell
   OEMFS_Truncate
   OEMFs_TruncateEx
   OEMFS_Remove
   OEMFS_Rename
   OEMFS_Mkdir
   OEMFS_Rmdir
   OEMFS_Test
   OEMFS_EnumStart
   OEMFS_EnumNext
   OEMFS_EnumEnd
   OEMFS_GetFreeSpaceEx

INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include <stdlib.h>
#include "fs.h"
#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
#      ifdef FEATURE_EFS2
#         include "fs_diag.h"
#      else
#         include "fsdiag.h"
#      endif
#endif // FEATURE_DIAG_FS_ACCESS_VALIDATION
#include "AEE_OEMComdef.h"
#include "AEE_OEMDispatch.h"
#include "OEMFS.h"
#include "OEMFSPath_priv.h"
#include "AEEComdef.h"
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEBufBound.h"
#include "fs_public.h"
#include "fs_errno.h"

#define POSIX_JANUARY_6_1980 315964800
#define POSIX_TIME_TOPHONE(t) ((t)-POSIX_JANUARY_6_1980)

/* =============================================================================

   FS TRACKER FEATURE DESCRIPTION

   The FS Tracker is a very simple wrapper around some of the the file-
   manipulation functions implemented in this file: OEMFS_Open() and 
   OEMFS_Close().  When this feature is compiled in, it causes a stack trace
   to be generated each time a file is opened and freed when that file is 
   closed.  If at any point we run out of file handles, the FS tracker will:
      a) close all open files
      b) write a file under /err/open_files.bt on EFS that contains the stack
         traces and files names for all open files.

   The idea is that if we get into a situation where we run out of file handles,
   the heap tracker can help us identify whether we ran into this situation 
   because we simply do not have enough file handles, or because there is a 
   file-handle leak of some sort (i.e., we open files and forget to close them).

   DEPENDENCIES:

   We assume the function HT_ARM926EJS_UNWIND_STACK() is implemented.  This 
   function generated in-target stack traces.

   NOTES:  

   The /err/open_files.bt file needs to be interpreted by the tool addr2sym, 
   which is also used to interpret heap-tracker log files.

   CAVEATS:

   This feature tracks only files that are opened through OEMFS.c.  There are
   many files which are not opened through the interface, but instead directly
   by using the EFS APIs efs_open() and efs_close().  Most notably, files
   corresponding to NV items are opened directly through efs_open() by AMSS
   software.  The FS tracker is useful for detecting file-handle leaks caused
   by BREW, because BREW always goes through OEMFS_Open() and OEMFS_Close().

   ========================================================================== */
#ifdef FEATURE_OEMFS_TRACKER
#include "assert.h"
#include "OEMHeap.h"
#include "OEMOS.h"
boolean oemfs_tracker_enabled = FALSE;
static void generate_file_trace(OEMINSTANCE fh, const char *name);
static void free_file_trace(OEMINSTANCE );
static void close_all_open_files(void);
static void dump_file_traces(void);
#endif//FEATURE_OEMFS_TRACKER


#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
#define OEM_NUM_WATCH_DIRS 2
#endif

#define FARF_TRACE  0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }


/*********************************************************************
**  PRIVATE DATA STRUCTURES
*********************************************************************/

typedef struct
{
   char                    szFile[FS_PATH_MAX];
   int                     nErr;
   int                     bDirs:1;
   fs_enum_iterator_type   iterator;
   EFSDIR                 *pDir;
   fs_enum_data_type       data;
   char                    szDir[1];
} OEMFSEnumCx;

typedef struct
{
   AEEOpenFileMode         nMode;
   int                     fmode;
} ModeMap;

/**********************************************************************
**  LOCAL FUNCTION DECLARATIONS
**********************************************************************/
static char    OEMFS_MapAttrib(fs_attribute_mask_type fattrib);
static int     OEMFS_MapErrno(void);
static char    FSToBrewAttrib(uint16 dwAttrib);
#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
static boolean OEMFS_ChkRmtAccess(char *pszName, uint32 dwMode);
#endif

/***********************************************************************
**  FILE-SCOPED VARIABLES
***********************************************************************/

// file access restriction callback
static PFNCHKRMTACCESS  gpfnChkRmtAccess = 0;

/*==================================================================
Function: OEMFS_NativePath

Description: Return device specific path.
==================================================================*/
static const char *OEMFS_NativePath(const char *cpszIn)
{
   static  char apszNativePathBuffers[8][FS_PATH_MAX];
   static  int  nPathBufIdx = 0;
   char   *pszPath = apszNativePathBuffers[nPathBufIdx];
   int     nLen = ARRAY_SIZE(apszNativePathBuffers[nPathBufIdx]);
   
   if (SUCCESS != OEMFS_GetNativePath(cpszIn,pszPath,&nLen)) {
      FARF(TRACE,("OEMFS_NativePath(\"%s\")=>0",cpszIn));
      return 0;
   }

   nPathBufIdx++;
   if (nPathBufIdx >= ARRAY_SIZE(apszNativePathBuffers)) {
      nPathBufIdx = 0;
   }

   FARF(TRACE,("OEMFS_NativePath(\"%s\")=>\"%s\"",cpszIn,pszPath));

   return pszPath;
}

/*==================================================================
Function: OEMFS_MapHandle

Description: Maps file handle between BREW and EFS.
==================================================================*/
static fs_handle_type OEMFS_MapHandle(uint32 dwFileHandle, fs_access_option_type *pao)
{
   /* we hide open flags in the upper word of the dword we return to BREW */
   if ((fs_access_option_type *)0 != pao) {
      *pao = (fs_access_option_type) ((dwFileHandle>>16)&(FS_OA_APPEND|FS_OA_READONLY));
   }
   return (fs_handle_type)((dwFileHandle)&0x0ffff);
}

/*==================================================================
Function: OEMFS_Open

Description: Opens a file in the EFS.
==================================================================*/
int OEMFS_Open(const char *cpszName, AEEOpenFileMode nMode, OEMINSTANCE *ppFileHandle)
{
   int                           fmode;      
   int                           nFileHandle = -1;

   static const ModeMap amm[] =
      {
         {_OFM_READ,        O_RDONLY},
         {_OFM_READWRITE,   O_RDWR},
         {_OFM_CREATE,      (O_CREAT | O_EXCL | O_RDWR)},
         {_OFM_APPEND,      (O_APPEND | O_RDWR)},
         {(AEEOpenFileMode)0, (int)0}
      };
   const ModeMap *pmm;

   FARF(TRACE,("OEMFS_Open(\"%s\",%d)", cpszName, nMode));

   fmode = O_RDONLY;
   efs_errno = 0;

   for (pmm = amm; 0 != pmm->nMode; pmm++) {
      if (pmm->nMode == nMode) {
         fmode = pmm->fmode;
         break;
      }
   }

   // map the filename and call the native open...
   cpszName = OEMFS_NativePath(cpszName);

   if ((char *)0 != cpszName) {
      nFileHandle = efs_open(cpszName, fmode, S_IREAD|S_IWRITE);
   } else {
      efs_errno = ENAMETOOLONG;
   }

   if (-1 != nFileHandle) {
       uint32 dwFileHandle = nFileHandle;

      /* hide append in the upper word of the file handle */
      if (_OFM_APPEND == nMode) {
         dwFileHandle |= (FS_OA_APPEND<<16);
      } else if (_OFM_READ == nMode) {
         dwFileHandle |= (FS_OA_READONLY<<16);
      } else {
         // Keep handle non-zero.
         dwFileHandle |= 0x80000000; 
      }

      *ppFileHandle = (OEMINSTANCE)dwFileHandle;
#ifdef FEATURE_OEMFS_TRACKER
      if (oemfs_tracker_enabled)
      {
        generate_file_trace(*ppFileHandle, cpszName);
      }
#endif//FEATURE_OEMFS_TRACKER
   }
#ifdef FEATURE_OEMFS_TRACKER
   else if (oemfs_tracker_enabled)
   {
     if (efs_errno == ENOSPC)
     {
       /* We are out of file handles.  The first thing we will do is walk over
          the list of open files and close each one of them.  We are going to 
          throw a fatal error afterwards.
       */
       close_all_open_files();
       dump_file_traces();
       ASSERT(!"Out of file handles!");
     }
   }
#endif//FEATURE_OEMFS_TRACKER

   FARF(TRACE,("OEMFS_Open() fd: %d=>%d", *ppFileHandle, 
             OEMFS_MapErrno()));

   // Translate results...
   return OEMFS_MapErrno();
}


/*==================================================================
Function: OEMFS_Close

Description: Closes a previously opened file in the EFS.
==================================================================*/
int OEMFS_Close(OEMINSTANCE handle)
{
   fs_handle_type  hf = OEMFS_MapHandle((uint32)handle, 0);
   
   if (0 == efs_close (hf)) {
      efs_errno = 0;
   }

#ifdef FEATURE_OEMFS_TRACKER
   if (oemfs_tracker_enabled)
   {
     free_file_trace(handle);
   }
#endif//FEATURE_OEMFS_TRACKER
   
   FARF(TRACE,("OEMFS_Close(%d)=>%d", handle,
               OEMFS_MapErrno()));

   return OEMFS_MapErrno();
}

/*==================================================================
Function: OEMFS_Read

Description: Reads bytes from a file in the EFS.
==================================================================*/
int32 OEMFS_Read(OEMINSTANCE handle, void *buffer, dword nBytes, int *pnErr)
{
   fs_handle_type  hf;
   dword nReadBytes = 0;
   int32 nRet = 0;
   unsigned char * pcBuf = buffer;

   hf = OEMFS_MapHandle((uint32)handle, 0);

   FARF(TRACE,("OEMFS_Read(%d,%p,%d)",(uint32)handle,buffer,nBytes));
   
   // EFS2 reads are limited to 8k, so repeat as needed
   do {
      nRet = efs_read(hf, (void*)(pcBuf + nReadBytes), nBytes - nReadBytes);
      if (-1 == nRet) {
         goto done;
      }
      nReadBytes += nRet;

   } while ((nRet > 0) && (nBytes > nReadBytes));


   FARF(TRACE,("OEMFS_Read()=>%d", nReadBytes));
   return nReadBytes;

done:

   *pnErr = OEMFS_MapErrno();
   FARF(TRACE,("OEMFS_Read() err: %d=>%d", *pnErr, -1));

   return -1;
}

/*==================================================================
Function: OEMFS_Write

Description: Writes bytes to a file in the EFS.
==================================================================*/
int32 OEMFS_Write(OEMINSTANCE handle, const void *buffer,
                  uint32 nBytes, int *pnErr)
{
   fs_handle_type        hf;
   uint32                nCount = 0;
   int32                 nRet = 0;
   const unsigned char  *pcBuf = buffer;

   FARF(TRACE,("OEMFS_Write(%d,%p,%d)",(uint32)handle,buffer,nBytes));

   hf = OEMFS_MapHandle((uint32)handle,0);

   // EFS2 writes are limited to 8k, so repeat as needed
   do {
      nRet = efs_write(hf, (void *)(pcBuf + nCount), nBytes - nCount);
      if (-1 == nRet) {
         goto done;
      }

      nCount += nRet;

   } while ((nRet > 0) && (nBytes > nCount));

   FARF(TRACE,("OEMFS_Write()=>%d", nCount));
   return nCount;

done:

   *pnErr = OEMFS_MapErrno();
   FARF(TRACE,("OEMFS_Write() err: %d=>%d", *pnErr, -1));

   return -1;
}

/*==================================================================
Function: OEMFS_WriteEx

Description: Writes bytes to a file in the EFS.
==================================================================*/
int32 OEMFS_WriteEx(OEMINSTANCE handle, const void *buffer,
                    uint32 nBytes, const char* cpszFilename, int *pnErr)
{
   return OEMFS_Write(handle, buffer, nBytes, pnErr);
}


/*==================================================================
Function: OEMFS_GetAttributes

Description: Gets attributes of a file in the EFS.
==================================================================*/
int OEMFS_GetAttributes(const char *cpszName,
                        char       *pattrib,
                        uint32     *pdwCreationDate,
                        uint32     *pdwSize)
{
   struct fs_stat  fsStat;
   int             nRet = -1;
   char            cAttrib;
   uint32          dwCreationDate, dwSize;
   
   if ((char *)0 == pattrib) {
      pattrib = &cAttrib;
   }
   if ((uint32 *)0 == pdwCreationDate) {
      pdwCreationDate = &dwCreationDate;
   }
   if ((uint32 *)0 == pdwSize) {
      pdwSize = &dwSize;
   }
   
   FARF(TRACE,("OEMFS_GetAttributes(\"%s\")",cpszName));
   efs_errno = 0;

   cpszName = OEMFS_NativePath(cpszName);
   
   if ((char *)0 == cpszName) {
      FARF(TRACE,("OEMFS_GetAttributes()=>%d",EBADFILENAME));
      return EBADFILENAME;
   }
   
   /* check to see if is directory */
   nRet = efs_stat(cpszName, &fsStat);
   
   if(-1 == nRet) {
      FARF(TRACE,("OEMFS_GetAttributes()=>%d",
                  OEMFS_MapErrno()));
      return OEMFS_MapErrno();
   }
   
   /* "" is a directory... */   
   if ((1 == S_ISDIR(fsStat.st_mode)) || ('\0' == *cpszName)) {
      if ((char *)0 != pattrib) {
         *pattrib = AEE_FA_DIR;
      }
      if ((uint32 *)0 != pdwCreationDate) {
         *pdwCreationDate = POSIX_TIME_TOPHONE(fsStat.st_ctime);
      }      
      if ((uint32 *)0 != pdwSize) {
         *pdwSize = 0;
      }
      FARF(TRACE,("OEMFS_GetAttributes() attrib: AEE_FA_DIR=>0"));
      return SUCCESS;
   }

   /* go ahead and do file stuff */   
   if (0 == nRet) {   
      *pattrib = FSToBrewAttrib(fsStat.st_mode);
      *pdwCreationDate = POSIX_TIME_TOPHONE(fsStat.st_ctime);
      *pdwSize = fsStat.st_size;
   }

   FARF(TRACE,("OEMFS_GetAttributes() attrib: %d",
               *pattrib));
   FARF(TRACE,("OEMFS_GetAttributes() date: %d",
               *pdwCreationDate));
   FARF(TRACE,("OEMFS_GetAttributes() size: %d",
               *pdwSize));
   FARF(TRACE,("OEMFS_GetAttributes()=>%d",
               OEMFS_MapErrno()));

   return OEMFS_MapErrno();
}


/*==================================================================
Function: OEMFS_GetOpenFileAttributes

Description: Gets attributes of an open file in the EFS.
==================================================================*/
int OEMFS_GetOpenFileAttributes(OEMINSTANCE handle,
                                char        *pattrib,
                                uint32      *pdwCreationDate,
                                uint32      *pdwSize)
{
   struct fs_stat fsStat;
   int nRet = -1;
   fs_handle_type  hf;
   char       cAttrib;
   uint32     dwCreationDate, dwSize;

   efs_errno = 0;   
   if ((char *)0 == pattrib) {
      pattrib = &cAttrib;
   }
   if ((uint32 *)0 == pdwCreationDate) {
      pdwCreationDate = &dwCreationDate;
   }
   if ((uint32 *)0 == pdwSize) {
      pdwSize = &dwSize;
   }

   hf = OEMFS_MapHandle((uint32)handle,0);

   FARF(TRACE,("OEMFS_GetOpenFileAttributes(%d)",(uint32)handle));

   nRet = efs_fstat(hf, &fsStat);

   if (-1 == nRet) {
      FARF(TRACE,("OEMFS_GetAttributes()=>%d",
                  OEMFS_MapErrno()));
      return OEMFS_MapErrno();
   } else {   
      *pattrib = FSToBrewAttrib(fsStat.st_mode);
      *pdwCreationDate = POSIX_TIME_TOPHONE(fsStat.st_ctime);
      *pdwSize = fsStat.st_size;
   }

   FARF(TRACE,("OEMFS_GetAttributes() attrib: %d",
               *pattrib));
   FARF(TRACE,("OEMFS_GetAttributes() date: %d",
               *pdwCreationDate));
   FARF(TRACE,("OEMFS_GetAttributes() size: %d",
               *pdwSize));
   FARF(TRACE,("OEMFS_GetAttributes()=>%d",
               OEMFS_MapErrno()));

   return OEMFS_MapErrno();
}

/*==================================================================
Function: OEMFS_Seek

Description: Moves the seek pointer to the specified location in 
a file in the EFS.
==================================================================*/
int32 OEMFS_Seek(OEMINSTANCE handle, AEEFileSeekType sType,
                 int32 nOffset, int *pnErr)
{
   fs_handle_type          hf;
   int32                   nFileSize = 0;
   int32                   nTell;
   struct fs_stat          fsStat;

   // initialize pnErr
   *pnErr = SUCCESS;

   FARF(TRACE,("OEMFS_Seek(%d,%d,%d)", (uint32)handle, sType, nOffset));

   // save where we're at....
   nTell = OEMFS_Tell(handle, pnErr);
   if (-1 == nTell) {
      FARF(TRACE,("OEMFS_Seek() err: %d=>%d", *pnErr, -1));
      return -1;
   }

   // Get file size
   {
      hf = OEMFS_MapHandle((uint32)handle,0);

      if (-1 == efs_fstat(hf, &fsStat)) {
         *pnErr = OEMFS_MapErrno();
         FARF(TRACE,("OEMFS_Seek() err: %d=>%d", *pnErr, -1));
         return -1;
      }
      nFileSize =  fsStat.st_size;
   }

   //
   // IMPORTANT
   //
   // DMSS does not support a negative offset parameter.  This means
   // we must internally convert the offset to the appropriate value
   // from the start of the file.
   //
   switch (sType) {
      case _SEEK_END:
         nOffset += nFileSize;
         break;

      case _SEEK_CURRENT: 
         // 0 seek from current is treated as a "tell"...
         if (!nOffset) {
            FARF(TRACE,("OEMFS_Seek()=>%d", nTell));
            return nTell;
         }

         nOffset += nTell;
         break;

      case _SEEK_START:
         break;

      default:
         *pnErr = EBADPARM;
         return -1;
   } 

   // Any negative seek now is an error ...
   if (nOffset < 0) {
      *pnErr = EBADSEEKPOS;
      FARF(TRACE,("OEMFS_Seek() err: %d=>%d", *pnErr, -1));
      return -1;
   }

   if (-1 != efs_lseek(hf, nOffset, SEEK_SET)) {

      // if position is greater than file size, do a truncate, 
      // only if file is not readonly.
      fs_access_option_type   fmode;      
      OEMFS_MapHandle((uint32)handle, &fmode);
      if(!(FS_OA_READONLY == fmode)) {
        if ((uint32)nOffset > fsStat.st_size) {
            if (0 != efs_ftruncate (hf, nOffset)) {
               *pnErr = OEMFS_MapErrno();
               return -1;
            }            
         }
      }
      *pnErr = SUCCESS;
      FARF(TRACE,("OEMFS_Seek()=>%d", nOffset));
      return nOffset;
   }

   *pnErr = OEMFS_MapErrno();
   FARF(TRACE,("OEMFS_Seek() err: %d=>%d", *pnErr, -1));

   return -1;
}

/*==================================================================
Function: OEMFS_SeekEx

Description: Moves the seek pointer to the specified location in 
              a file in the EFS.
==================================================================*/
int32 OEMFS_SeekEx(OEMINSTANCE pFileHandle, AEEFileSeekType sType, 
                   int32 nOffset, const char* cpszFilename, int *pnErr)
{
   return OEMFS_Seek(pFileHandle, sType, nOffset, pnErr);
}


/*==================================================================
Function: OEMFS_Tell

Description: Returns the current position of the seek pointer in
             a file in the EFS.
==================================================================*/
int32 OEMFS_Tell(OEMINSTANCE handle, int *pnErr)
{
   fs_handle_type  hf;
   fs_off_t curPos = -1;

   hf = OEMFS_MapHandle((uint32)handle, 0);
   
   FARF(TRACE,("OEMFS_Tell(%d)", (uint32)handle));

   curPos = efs_lseek (hf, 0, SEEK_CUR);

   if (-1 == curPos) {
      *pnErr = OEMFS_MapErrno();
      FARF(TRACE,("OEMFS_Tell() err: %d=>%d", *pnErr, -1));
      return -1;
   }

   FARF(TRACE,("OEMFS_Tell()=>%d", curPos));
   return curPos;
}

/*==================================================================
Function: OEMFS_Truncate

Description: Truncates a file in the EFS to the specified size.
==================================================================*/
int OEMFS_Truncate(OEMINSTANCE handle, uint32 nPos)
{
   int     nErr, nRet;
   struct fs_stat fsStat;
   fs_handle_type hf;

   FARF(TRACE,("OEMFS_Truncate(%d, %d)", (uint32)handle, nPos));

   efs_errno = 0;
   // do a stat. if tuncate_pos > file size return error.
   hf = OEMFS_MapHandle((uint32)handle,0);

   nRet = efs_fstat(hf, &fsStat);

   if (-1 == nRet) {
      nErr = OEMFS_MapErrno();
      FARF(TRACE,("OEMFS_Truncate()=>%d", nErr));
      return nErr;
   } 

   if (nPos > fsStat.st_size) {
      FARF(TRACE,("OEMFS_Truncate()=>%d", EBADPARM));
      return EBADPARM;
   }

   efs_ftruncate (hf, nPos);
   nErr = OEMFS_MapErrno();

   if (SUCCESS == nErr) {
      OEMFS_Seek(handle,_SEEK_END,0,&nErr);
   }

   FARF(TRACE,("OEMFS_Truncate()=>%d", nErr));
   return nErr;
}

/*==================================================================
Function: OEMFS_TruncateEx

Description: Truncates a file in the EFS to the specified size.
==================================================================*/
int OEMFS_TruncateEx(OEMINSTANCE handle, const char* cpszFilename, 
                     uint32 nPos)
{
   return OEMFS_Truncate(handle, nPos);
}

/*==================================================================
Function: OEMFS_Remove

Description: Removes a file from the EFS.
==================================================================*/
int OEMFS_Remove(const char *cpszFile)
{
   FARF(TRACE,("OEMFS_Remove(%s, %d)", cpszFile));
   efs_errno = 0;

   cpszFile = OEMFS_NativePath(cpszFile);

   if ((char *)0 != cpszFile) {
      efs_unlink(cpszFile);
   } else {
      efs_errno = ENAMETOOLONG;
   }

   FARF(TRACE,("OEMFS_Remove()=>%d", OEMFS_MapErrno()));

   return OEMFS_MapErrno(); 
}


/*==================================================================
Function: OEMFS_Rename

Description: Renames a file in the EFS to the specified new name.
==================================================================*/
int OEMFS_Rename(const char *cpszOldName,const char *cpszNewName)
{
   FARF(TRACE,("OEMFS_Rename(%s,%s)", cpszOldName, cpszNewName));
   {
      char   cAttrib;
      uint32 dwSize;
      int    nErr;
      
      efs_errno = 0;
      nErr = OEMFS_GetAttributes(cpszOldName,&cAttrib,0,&dwSize);
      
      if (SUCCESS != nErr) {
         FARF(TRACE,("OEMFS_Rename()=>%d", nErr));
         return nErr;
      }
      
      if (cAttrib & AEE_FA_DIR) {
         FARF(TRACE,("OEMFS_Rename()=>%d", EINVALIDOPERATION));
         return EINVALIDOPERATION;
      }
   }
   
   cpszNewName = OEMFS_NativePath(cpszNewName);
   cpszOldName = OEMFS_NativePath(cpszOldName);

   if ((char *)0 != cpszNewName && (char *)0 != cpszOldName) {
      efs_rename (cpszOldName, cpszNewName);
      FARF(TRACE,("OEMFS_Rename()=>%d", OEMFS_MapErrno()));
      return OEMFS_MapErrno();
   } else {
      FARF(TRACE,("OEMFS_Rename()=>%d", EBADFILENAME));
      return EBADFILENAME;
   }
}

/*==================================================================
Function: OEMFS_Mkdir

Description: Creates a direcory at the specified location in the EFS.
==================================================================*/
int OEMFS_Mkdir(const char *cpszDir)
{
   FARF(TRACE,("OEMFS_Mkdir(%s)", cpszDir));

   cpszDir = OEMFS_NativePath(cpszDir);
   
   if ((char *)0 == cpszDir) {
      FARF(TRACE,("OEMFS_Mkdir()=>%d", EBADFILENAME));
      return EBADFILENAME;
   }
   efs_errno = 0;
   if (OEMFS_IsAutoCreate(cpszDir)) {
      int nDirLen, nLen;
      nDirLen = STRLEN(cpszDir); 

      do {
         nLen = nDirLen;

         // try to make whole thing
         efs_mkdir (cpszDir, 0);
         
         // parent doesn't exist, try to make all parents
         while (ENOTEMPTY == efs_errno) {
            
            char *pszSlash = MEMRCHR(cpszDir,'/',nLen); 

            // if no previous slash, return error unmolested
            if ((char *)0 == pszSlash) {
               break;
            }
            
            // trim last slash...
            // this is ok because OEMFS_NativePath() returns a 
            // global I can modify...
            *pszSlash = 0;

            // shorten len for next loop iteration
            nLen = STRLEN(cpszDir);
            
            // make parent
            efs_mkdir (cpszDir, 0);
            
            // restore slash
            *pszSlash = '/';
         }
         
         // while we successfully made a parent...
      } while ( (0 == efs_errno) && nDirLen != nLen);

   } else {
      efs_mkdir (cpszDir, 0);
   }

   FARF(TRACE,("OEMFS_Mkdir()=>%d", OEMFS_MapErrno()));

   return OEMFS_MapErrno();
}

/*==================================================================
Function: OEMFS_Rmdir

Description: Removes the specified directory from the EFS.
==================================================================*/
int OEMFS_Rmdir(const char *cpszDir)
{
   int nErr = SUCCESS;
   FARF(TRACE,("OEMFS_Rmdir(%s)", cpszDir));

   {
      boolean bDirs;

      for (nErr = SUCCESS, bDirs = FALSE; 
           SUCCESS == nErr && bDirs <= TRUE; bDirs++) {
         OEMINSTANCE handle;
         
         // Need to insure that the directory is empty...
         nErr = OEMFS_EnumStart(cpszDir, bDirs, &handle);

         if (SUCCESS == nErr) {
            // See if there are any sub-dirs or files, don't care what
            //  the name is

            if (SUCCESS == OEMFS_EnumNext(handle,0,0)) {
               nErr = EDIRNOTEMPTY;
            }
            
            OEMFS_EnumStop(handle);
         }
      }

      if (SUCCESS != nErr) {
         FARF(TRACE,("OEMFS_Rmdir()=>%d", nErr));
         return nErr;
      }
   }

   cpszDir = OEMFS_NativePath(cpszDir);

   if ((char *)0 != cpszDir) {

      nErr = efs_rmdir(cpszDir);

      if (-1 == nErr){
         FARF(TRACE,("OEMFS_Rmdir()=>%d", OEMFS_MapErrno()));
         nErr = OEMFS_MapErrno();
      }
   } else {
      FARF(TRACE,("OEMFS_Rmdir()=>%d", EBADFILENAME));
      nErr = EBADFILENAME;
   }

   return nErr;
}

/*==================================================================
Function: OEMFS_Test

Description: Tests if the specified name is a file or a directory.
==================================================================*/
int OEMFS_Test(const char * cpszName)
{
   struct fs_stat fsStat;

   FARF(TRACE,("OEMFS_Test(%s)", cpszName));
      
   efs_errno = 0;
   cpszName = OEMFS_NativePath(cpszName);
   
   if ((char *)0 != cpszName) {
      efs_stat(cpszName, &fsStat);

      if ( (0 != efs_errno) &&
         '\0' != *cpszName) {      /* "" is a directory, always exists */
         efs_errno = ENOENT;
      }
   } else {
      efs_errno = ENAMETOOLONG;
   }

   FARF(TRACE,("OEMFS_Test()=>%d", OEMFS_MapErrno()));

   return OEMFS_MapErrno();
}

/*==================================================================

 Calls the FS layer to do the next forward enumeration.

==================================================================*/
static int OEMFS_EnumCxNext(OEMFSEnumCx * pe)
{
   int               nErr;
   struct fs_dirent *pName;
   struct fs_stat   fsStat;
   int               nRet = -1;
   char             *pszEnd = NULL;

   efs_errno = 0;
   // Perform in a loop in case we encounter "." or ".."
   for (;;) {
      int nNameLen = FS_PATH_MAX;

      pName = efs_readdir(pe->pDir);
      nErr = OEMFS_MapErrno(); 

      if ((0 == pName) || (0 == pName->d_name)) {
         if (SUCCESS != nErr) {
            return (pe->nErr = nErr);
         }
         return (pe->nErr = EFILENOEXISTS);
      }

      pszEnd = BASENAME(pName->d_name);
      
      MAKEPATH(pe->szFile, pszEnd,
               pe->data.fullname, &nNameLen);   

      if (STRCMP(pszEnd,".") && STRCMP(pszEnd,"..")) {
         break;
      }
   }

   while (1) {
      int nNameLength = FS_PATH_MAX;

      nRet = efs_stat(pe->data.fullname, &fsStat);
   
      if (-1 == nRet) {
         nErr = OEMFS_MapErrno();       
         return (pe->nErr = nErr);
      }

      if (pe->bDirs) {      
         /* "" is a directory... */   
         if (1 == S_ISDIR(fsStat.st_mode)) {
            break;
         }
      }
      else {      
         if (1 == S_ISREG(fsStat.st_mode)) {
            break;
         }
      }

      pName = efs_readdir(pe->pDir);
      nErr = OEMFS_MapErrno();       

      if ((0 == pName) || (0 == pName->d_name)) {
         if (SUCCESS != nErr) {
            return (pe->nErr = nErr);
         }
         return (pe->nErr = EFILENOEXISTS);
      }

      pszEnd = BASENAME(pName->d_name);
     
      MAKEPATH(pe->szFile, pszEnd,
               pe->data.fullname, &nNameLength);
   }

   if (0 == nRet) {   
      if ((fsStat.st_mode & S_IFMT) &&
            (fsStat.st_mode & S_IREAD)&&
          (fsStat.st_mode & S_IWRITE)) {
         pe->data.attributes = FS_FA_UNRESTRICTED;
      } else if ((fsStat.st_mode & S_IFMT) &&
            (fsStat.st_mode & S_IREAD)) {
         pe->data.attributes = FS_FA_READONLY;
      } else {
         pe->data.attributes = FS_FA_UNRESTRICTED;
      }

      pe->data.creation_date = POSIX_TIME_TOPHONE(fsStat.st_ctime);
      pe->data.physical_size = pe->data.logical_size = fsStat.st_size;
   }

   return (pe->nErr = SUCCESS);

}

/*==================================================================
Function: OEFS_EnumStart

Description: File enumeration initializer.
==================================================================*/
int OEMFS_EnumStart(const char *cpszDir, char isDir, OEMINSTANCE *ppcxt)
{
   OEMFSEnumCx *pe;
   int nErr = SUCCESS;

   FARF(TRACE,("OEMFS_EnumStart(\"%s\")", cpszDir));

   //
   // Allocate the context that will carry us through this enumeration.
   //
   pe = (OEMFSEnumCx *)MALLOC(sizeof(OEMFSEnumCx) + STRLEN(cpszDir));
   
   if ((OEMFSEnumCx *)0 == pe) {
      FARF(TRACE,("OEMFS_EnumStart()=>%d",ENOMEMORY));
      return ENOMEMORY;
   }

   // hang onto original name
   STRLCPY(pe->szDir, cpszDir, STRLEN(cpszDir)+1); // +1 in nSize includes NULL

   // Translate the enumeration type
   pe->bDirs = isDir;

   /* map name, call EFS */
   cpszDir = OEMFS_NativePath(cpszDir);

   if ((char *)0 != cpszDir) {
      
      pe->pDir = efs_opendir(cpszDir);
      if (NULL == pe->pDir) {
         nErr = OEMFS_MapErrno();
      }

   } else {
      nErr = EBADFILENAME;
   }
   
   /* Check the return code */
   if (SUCCESS == nErr) {

      STRLCPY(pe->szFile, cpszDir, sizeof(pe->szFile));
      
      // Handle the first enumeration.  This is done to prevent us from
      // encountering errors in cases where the user removes the file/dir
      // of the current entry they got from EnumNext
      nErr = OEMFS_EnumCxNext(pe);

      if (EFILENOEXISTS == nErr) {
         nErr = SUCCESS;
      }

      if (SUCCESS != nErr) {
	      OEMFS_EnumStop(pe);
	      pe = NULL;
      }
   }
   else {	   
      // No files here; get out...
      FREE(pe);
      pe = NULL;
   }


   *ppcxt = (OEMINSTANCE)pe;

   FARF(TRACE,("OEMFS_EnumStart() pcxt: %d=>%d",*ppcxt,nErr));

   return nErr;
}

/*==================================================================
Function: OEMFS_EnumNext

Description: Enumerates through the files in the location specified
             by OEMFS_EnumInit.
==================================================================*/
int OEMFS_EnumNext(OEMINSTANCE pcxt, OEMFSEnum *pFSEnum, int *pnLen)
{
   OEMFSEnumCx *pe = (OEMFSEnumCx *)pcxt;

   FARF(TRACE,("OEMFS_EnumNext(%d,%p,%d)",
               pcxt,pFSEnum,
               pnLen?*pnLen:696969696));
   
   // at end or error?
   if (SUCCESS != pe->nErr) {
      FARF(TRACE,("OEMFS_EnumNext()=>%d",pe->nErr));
      return pe->nErr;
   }
   
   // want info
   if ((int *)0 != pnLen) {

      int nNameLen;

      // want size, DO NOT ADVANCE ENUMERATOR
      if ((OEMFSEnum *)0 == pFSEnum) {
         MAKEPATH(pe->szDir, BASENAME(pe->data.fullname),
                  0, pnLen);
         *pnLen += (sizeof(OEMFSEnum) - 1);
         FARF(TRACE,("OEMFS_EnumNext() nLen: %d=>%d",*pnLen,SUCCESS));
         return SUCCESS;
      }

      // space for name includes the char szName[1] at end of OEMFSEnum
      nNameLen = *pnLen - sizeof(OEMFSEnum) + 1;

      if (nNameLen < 0) {
         FARF(TRACE,("OEMFS_EnumNext()=>%d",EBUFFERTOOSMALL));
         return EBUFFERTOOSMALL;
      }
      
      // Move the fields from the last successful enumeration to the 
      //  client area. 
      //
      pFSEnum->attrib         = OEMFS_MapAttrib(pe->data.attributes);
      pFSEnum->dwCreationDate = pe->data.creation_date;
      pFSEnum->dwSize         = pe->data.logical_size;
      
      // If we are enumerating directories no need to check using fs_nametest
      if (pe->bDirs) {
         pFSEnum->attrib |= AEE_FA_DIR;
      }
      
      MAKEPATH(pe->szDir, BASENAME(pe->data.fullname),
               pFSEnum->szName, &nNameLen);
      
      FARF(TRACE,("OEMFS_EnumNext() name: %s",
                  pFSEnum->szName));
      FARF(TRACE,("OEMFS_EnumNext() attrib: %d",
                  pFSEnum->attrib));
      FARF(TRACE,("OEMFS_EnumNext() date: %d",
                  pFSEnum->dwCreationDate));
      FARF(TRACE,("OEMFS_EnumNext() size: %d",
                  pFSEnum->dwSize));
   }

   // advance enumerator
   OEMFS_EnumCxNext(pe);

   FARF(TRACE,("OEMFS_EnumNext()=>%d",SUCCESS));

   return SUCCESS;
}

/*==================================================================
Function: OEMFS_EnumStop

Description: Indicates that we can free resources related to enumeration
==================================================================*/
int OEMFS_EnumStop(OEMINSTANCE pcxt)
{
   OEMFSEnumCx *pe = (OEMFSEnumCx *)pcxt;
   EFSDIR* pDir = (EFSDIR*)pe->pDir;

   FARF(TRACE,("OEMFS_EnumStop(%d)"));

   efs_errno = 0;

   if (pDir){
      if (0 == efs_closedir(pDir)) { 
         FREE(pe);
      }
   } 
   
   FARF(TRACE,("OEMFS_EnumStop()=>%d",
               OEMFS_MapErrno()));

   return OEMFS_MapErrno();
}

/*==================================================================
Function: OEMFS_GetFreeSpaceEx

Description: Returns total and free space already used on the file
             system pointed by szPath.
==================================================================*/
int OEMFS_GetFreeSpaceEx(const char *szPath, uint32 *pdwTotal, uint32 *pdwFree)
{
   struct fs_statvfs info;
   uint64 qwTotal;
   uint64 qwFree;
   int nRet = -1;

   FARF(TRACE,("OEMFS_GetFreeSpaceEx()"));

   // map the filename and call the native statfs...
   szPath = OEMFS_NativePath(szPath);

   info.f_bsize = 0;
   info.f_blocks = 0;
   info.f_bavail = 0;

   if ((char *)0 != szPath) {
      nRet = efs_statvfs ( szPath, &info );
   } else {
      return EBADFILENAME;
   }

   if ( nRet == 0 ) {
      
      // check if total space > 4GB. Cap values to 4GB if so.
      // this assumes that total space can fit in uint64. Not a bad assumption.
      qwTotal = ((uint64)info.f_bsize) * ((uint64)info.f_blocks);

      if(pdwTotal) {
         if (qwTotal > (uint64)MAX_UINT32) {
            *pdwTotal = MAX_UINT32;
         }
         else {
            *pdwTotal = info.f_bsize * info.f_blocks;
         }
      }
      
      if (pdwFree) { 
         // check if free space > 4GB. Cap value to 4GB if so.
         {
            // this assumes that free space can fit in uint64. Not a bad assumption.
            qwFree = ((uint64)info.f_bsize) *  ((uint64)info.f_bavail);
            if (qwFree > (uint64)MAX_UINT32) {
               *pdwFree =  MAX_UINT32;
            }
            else {
               *pdwFree =  info.f_bsize * info.f_bavail;
            }
         }
      }
      return SUCCESS;
   } else {
      return OEMFS_MapErrno();
   }
}

/*==================================================================
Function: OEMFS_RegRmtAccessChk

Description: Registers the remote file access directories to watch.
==================================================================*/
void OEMFS_RegRmtAccessChk(const char **pszDirList, uint32 nListElements, PFNCHKRMTACCESS pfn )
{
#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
   static const char *acpszDirNameList[OEM_NUM_WATCH_DIRS];
#endif
   gpfnChkRmtAccess = pfn;
   
#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION

   // We are unable to validate directories 
   // that will not translate into AEE file paths.
   // So only watch BREW directories.
   // For the DMSS implmentation we only 
   // really need to watch these directories
   // since the directory watch is inclusive.
   acpszDirNameList[0] = OEMFS_BREW_DIR;
   acpszDirNameList[1] = OEMFS_RINGERS_DIR;
   
   // call DMSS register access
   diag_fs_register_access_check(acpszDirNameList, OEM_NUM_WATCH_DIRS, (diag_fs_access_check_fptr_type)OEMFS_ChkRmtAccess);
#endif
}

#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
/*==================================================================
Function: OEMFS_ChkRmtAccess

Description: Called when a file access request is made.  Must call
             the AEE callback passed to OEMFS_RegRmtAccessChk and 
             return wether or not the file access is allowed.
==================================================================*/
static boolean OEMFS_ChkRmtAccess(char *szName, uint32 mode)
{
   OEMFSRmtAccessMode faMode;
   
   if (NULL != gpfnChkRmtAccess) {
   
      switch (mode) {
#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
         case DIAG_FS_CHECK_READ:
            faMode = FRAM_READ;
            break;
         case DIAG_FS_CHECK_MKDIR:
         case DIAG_FS_CHECK_SET_ATTRIB:
         case DIAG_FS_CHECK_WRITE:
         case DIAG_FS_CHECK_REMOTE_LINK:
            faMode = FRAM_WRITE;
            break;
         case DIAG_FS_CHECK_GET_ATTRIB:
         case DIAG_FS_CHECK_DISP_DIRS:
         case DIAG_FS_CHECK_DISP_FILES:
         case DIAG_FS_CHECK_ITERATE:
            faMode = FRAM_LIST;
            break;
         case DIAG_FS_CHECK_RMDIR:
         case DIAG_FS_CHECK_DELETE:
            faMode = FRAM_DELETE;
            break;
#ifdef FEATURE_EFS2
         case DIAG_FS_CHECK_OPEN:
         case DIAG_FS_CHECK_CLOSE:
         case DIAG_FS_CHECK_READLINK:
            faMode = FRAM_LIST;
            break;
         case DIAG_FS_CHECK_SYMLINK:
         case DIAG_FS_CHECK_RENAME:
         case DIAG_FS_CHECK_CHMOD:
            faMode = FRAM_WRITE;
            break;
         case DIAG_FS_CHECK_OPENDIR:
         case DIAG_FS_CHECK_READDIR:
         case DIAG_FS_CHECK_CLOSEDIR:
         case DIAG_FS_CHECK_STAT:
         case DIAG_FS_CHECK_LSTAT:
         case DIAG_FS_CHECK_FSTAT:
         case DIAG_FS_CHECK_STATFS:
         case DIAG_FS_CHECK_ACCESS:
            faMode = FRAM_LIST;
            break;
         case DIAG_FS_CHECK_UNLINK:
            faMode = FRAM_DELETE;
            break;         
#endif // FEATURE_EFS2
#endif // FEATURE_DIAG_FS_ACCESS_VALIDATION
         default:
            // unknown mode, deny access
            DBGPRINTF("Unknown mode, Denied");
            return FALSE;
      }

      {
         static char szFSName[FS_PATH_MAX];
         int         nLen = ARRAY_SIZE(szFSName);

         if (SUCCESS != OEMFS_GetBREWPath(szName, szFSName, &nLen)) {
            DBGPRINTF("Unable to translate file name, Denied");
            return FALSE;
         }

         return gpfnChkRmtAccess(szFSName, faMode);
      }
   }
   // we have not registered any directories we must allow full 
   // access for now.  Directories are registered in AEE_Init and 
   // will happen before the serial port is available for remote 
   // file access requests
   DBGPRINTF("Callback pointer not valid, access allowed until BREW initializes");
   return TRUE;
}
#endif

/*==================================================================


  LOCAL ROUTINES


==================================================================*/

/*===========================================================================
Function: 
   OEMFS_MapAttrib

Description:
   Maps fs_attribute_mask_type file attributes removed from EFS to FileAttrib

Parameter(s):
   fattrib [in]: EFS file attribute

Comment: 

fs_attribute_mask_type is defnied as

typedef enum {  
  FS_FA_UNRESTRICTED =0x00FF, // No file access restrictions             
  FS_FA_READONLY     =0x00BF, // File cannot be opened for update access
  FS_FA_PERMANENT    =0x007F, // File cannot be removed nor truncated     
  FS_FA_SYS_PERMANENT=0x0037, // File persists across system reformats
  FS_FA_REMOTE       =0x001F  // Remote file (resides outside the space
} fs_attribute_mask_type;

Because of this only _FA_NORMAL, _FA_READONLY and _FA_SYSTEM file attributes
are supported rendering _FA_DIR and _FA_HIDDEN values of FileAttrib unused.
===========================================================================*/
static char OEMFS_MapAttrib(fs_attribute_mask_type fattrib)
{
   // Read only files...

   if (fattrib == FS_FA_READONLY) {
      return AEE_FA_READONLY;
   }

   // System files...

   if ((fattrib == FS_FA_PERMANENT)     ||
       (fattrib == FS_FA_SYS_PERMANENT) ||
       (fattrib == FS_FA_REMOTE)) {
      return AEE_FA_SYSTEM;
   }

   return AEE_FA_NORMAL;
}

#ifdef FEATURE_OEMFS_TRACKER

typedef struct oemfs_trace_t oemfs_trace_t;

#define OEMFS_TRACE_DEPTH (100)

struct oemfs_trace_t
{
  oemfs_trace_t *prev;

  OEMINSTANCE handle; // lookup key

  char filename[FS_PATH_MAX];

  boolean partial;
  signed long depth;
  unsigned long timestamp_ms;
  void* trace[OEMFS_TRACE_DEPTH];
};

static oemfs_trace_t *trace_list;

static void enqueue_trace(oemfs_trace_t *trace)
{
  trace->prev = trace_list;
  trace_list = trace;
}

static oemfs_trace_t * lookup_and_dequeue_trace(OEMINSTANCE fh)
{
  if (trace_list)
  {
    oemfs_trace_t *trace = trace_list;
    oemfs_trace_t *prev = NULL;

    while (trace)
    {
      if (trace->handle == fh)
      {
        break;
      }
      prev = trace;
      trace = trace->prev;
    }

    if (trace)
    {
      if (prev)
      {
        prev->prev = trace->prev;
      }
      else
      {
        trace_list = trace->prev;
      }
      trace->prev = NULL; // not strictly necessary
      return trace;
    }
  }

  return NULL;
}

static void generate_file_trace(OEMINSTANCE fh, const char *name)
{
  oemfs_trace_t *trace = (oemfs_trace_t *)sys_malloc(sizeof(oemfs_trace_t));
  if (trace)
  {
    /* Generate the trace */

    extern signed long HT_ARM926EJS_UNWIND_STACK(void **bt_buffer, unsigned long bt_size);
    trace->depth = HT_ARM926EJS_UNWIND_STACK(trace->trace, OEMFS_TRACE_DEPTH*sizeof(void *));
    trace->partial = FALSE;
    if (trace->depth < 0)
    {
      trace->depth *= -1;
      trace->partial = TRUE;
    }
    trace->timestamp_ms = OEMOS_GetUpTime();

    /* Copy the filename and the file handle into the structure */

    STRLCPY(trace->filename, name, FS_PATH_MAX);
    trace->handle = fh;

    enqueue_trace(trace);
  }
}

static void free_file_trace(OEMINSTANCE fh)
{
  oemfs_trace_t *trace;
  trace = lookup_and_dequeue_trace(fh);
  if (trace)
  {
    sys_free(trace);
  }
}

static void close_all_open_files()
{
  oemfs_trace_t *trav = trace_list;
  fs_handle_type fh;
  fs_rsp_msg_type resp;
  while (trav)
  {
    fh = OEMFS_MapHandle((uint32)trav->handle,0);
    efs_close(fh);
    trav = trav->prev;
  }
}

static void dump_file_traces()
{
  /* We borrow this function from the heap. */
  extern void print_trace_buffer(const char *fname, 
                                 const char *desc,
                                 void **buffer,
                                 unsigned long buffer_length,
                                 boolean partial,
                                 unsigned long timestamp_ms);

  oemfs_trace_t *trav = trace_list, *old = NULL;

  while (trav)
  {
    print_trace_buffer("/err/open_files.bt", 
                       trav->filename,
                       trav->trace,
                       trav->depth,
                       trav->partial,
                       trav->timestamp_ms);

    old = trav;
    trav = trav->prev;
    sys_free(old);
  }
  trace_list = NULL;
}

#endif//FEATURE_OEMFS_TRACKER


// EFS1 API based OEMFS_StatVFS. If the EFS2 api has a bug use
// this function to get total and free space information about
// the filesystem.
/*==================================================================
Function: OEMFS_StatVFS

Description: Returns total and free space already used on the file
system pointed by szPath.
==================================================================*/
int OEMFS_StatVFS(const char *cpszPath, uint64 *pqwTotal, uint64 *pqwFree)
{
   struct fs_statvfs buf;
   uint64 qwTotal;

   FARF(TRACE,("OEMFS_StatVFS()"));

   // map the filename and call the native statfs...
   cpszPath = OEMFS_NativePath(cpszPath);

   if ((char *)0 == cpszPath) {
      return EBADFILENAME;
   }
   
   buf.f_bsize = 0;
   buf.f_blocks = 0;
   buf.f_bavail = 0;
   
   if(-1 == efs_statvfs (cpszPath, &buf)) {
      return OEMFS_MapErrno();
   }

   qwTotal = ((uint64)buf.f_bsize) * ((uint64)buf.f_blocks);
   if (pqwTotal) {
      *pqwTotal = qwTotal;
   }
   if (pqwFree) {
      *pqwFree =  ((uint64)buf.f_bsize) * ((uint64)buf.f_bavail);
   }

   return SUCCESS;
}

/*==================================================================
Translates EFS2 efs_errno into AEE error
==================================================================*/
int OEMFS_MapErrno(void)
{
   static const struct
   {
      int            to;
      int            from;
   } aem[] = 
   {
      {SUCCESS,           0},
      {EPRIVLEVEL,        EPERM},
      {EFILENOEXISTS,     ENOENT},       
      {EFILEEXISTS,       EEXIST},
      {EFSFULL,           ENOSPC},
      {EBADSEEKPOS,       ESPIPE},
      {EBADFILENAME,      ENAMETOOLONG},
      {EDIRNOTEMPTY,      ENOTEMPTY},
      {EFILEOPEN,         EBUSY},
      {EFILEEXISTS,       EISDIR},
      {EPRIVLEVEL,        EACCES},
#if defined(ETXTBSY)
      {EITEMBUSY,     ETXTBSY},
#endif
#if defined(ENOMEM)            // 6100/5150 does not have ENOMEM?
      {ENOMEMORY,         ENOMEM}, 
#endif // defined (ENOMEM)
      {EBADPARM,          EBADF},
      {EOUTOFNODES,       EMFILE},
      {EINVALIDOPERATION, EINVAL},
#if defined(ENOCARD)	 
      {ENOMEDIA,          ENOCARD},
#endif	 
#if defined(EEOF)
      {EFILEEOF,          EEOF},
#endif
   };

   int from = efs_errno;
   int i;

   for (i = 0; i < ARRAY_SIZE(aem); i++ ) {
      if (aem[i].from == from) {
         return aem[i].to;
      }
   }

   return EFAILED;
}

static char FSToBrewAttrib(uint16 dwAttrib)
{
   char attrib = AEE_FA_NORMAL;

   if ((dwAttrib & S_IFMT) &&
         (dwAttrib & S_IREAD) &&
         (dwAttrib & S_IWRITE)) {
      attrib = AEE_FA_NORMAL;
   } else if ((dwAttrib & S_IFMT) && (dwAttrib & S_IREAD)) {
      attrib = AEE_FA_READONLY;
   } else {
      attrib = AEE_FA_NORMAL;
   }

   return attrib;
}