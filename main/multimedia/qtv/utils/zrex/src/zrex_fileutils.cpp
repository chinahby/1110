/*==========================================================================

    zrex_FileUtils.cpp

  Description: This is the source file for the file system
    developed for the zrex player.

    Features of the zrex file system:
    1) file i/o status (counts numbers of calls)
    2) raises exceptions on file errors
    3) file read cacheing
    4) virtual file position.
    5) debug printout.

    Limitations:
    1) does not have file write support.  applications
      that need to simultaneously read and write to
      the same file should not use this!

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */


//#ifdef PV_OS_ZREX
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
//////////////////////////////////////////////////////////////////////
//
// Global exception definitions.
//
//////////////////////////////////////////////////////////////////////
#define COMMON_EXCEPTION_OUT_OF_MEMORY 100
#define COMMON_EXCEPTION_FILE_ERROR 101

#include "qcplayer_oscl_utils.h"
#include "zrex_fileutils.h"
#include "qcplayer.h"



//Note: you'll get infinite recursion at runtime if zrex_file.h was included
//at this point, since it redefines the file system calls (fopen, fseek, etc)
//to call their equivalents here, which in turn call the file system calls.
#ifdef _ZREX_FILE_H_
#error Include File Trouble in FileUtils.cpp!
#endif


////////////////////////////////////////////////////////////////////////////
//
// File I/O Utilities: File Cacheing and Virtual File Pos
//
////////////////////////////////////////////////////////////////////////////

//The file cache and the virtual file pos features increase the efficiency
//of MP4 file processing, in particular parsing the file and retrieving
//audio samples.  There's little or no effect on retrieving video samples.
//
//Note: we use the virtual file pos if and only if we are using the file
//cache.  Using either one alone will greatly increase the number
//of fseek calls.  Using them both together greatly reduces both
//the fseek and fread calls.  Why?
//1) Using the file cache alone reduces the number of fread calls,
//but adds an extra fseek for each cache fill and for each cache read.
//2) Using the virtual file pos alone gets rid of all the app-generated
//fseek calls, but adds an extra fseek per fread, so you actually end up
//with more fseek calls than before.
//3) Using them both together reduces the number of fread calls, and makes
//the number of fseek calls equal the number of fread calls.


//We need a cache and file pointer for each open file.
//There can only be 2 open files in our system.  Actually it's the
//same file, but with separate file pointers for audio and video.
//Notes on re-entrancy:  I use critical section protection when adding
//an entry to the array, but in general there's no contention trouble
//since the tasks are using different file pointers so different array entries.

//We need as many open files as layers in a clip so COMMON_MAX_LAYERS
//specifies the number of video layers and add one more for the audio layer.
static const int MAX_OPEN_FILES=COMMON_MAX_LAYERS+1;
static struct FileUtils::FilePointer Files[MAX_OPEN_FILES];
static rex_crit_sect_type FilesCS;
static bool bEnable;


FileUtils::FilePointer * FileUtils::FindFile(FILE * fp)
//
// file the file handle, or null if none.
{
  FileUtils::FilePointer *pFile=NULL;

  rex_enter_crit_sect(&FilesCS);
  for (int i=0;i<MAX_OPEN_FILES;i++)
  {
    if (Files[i].fp==fp)
    {
      pFile = &Files[i];
    }
  }
  rex_leave_crit_sect(&FilesCS);

  return pFile;
}

FileUtils::FilePointer * FileUtils::AddFile(FILE * fp)
//
//create a new file in the files array
{
  FilePointer * pFile = NULL;
  if (!FindFile(fp))
  {
    //do critical section protection when
    //adding a file, just in case 2 tasks are opening
    //files at the same time.
    rex_enter_crit_sect(&FilesCS);
    for (int i=0;i<MAX_OPEN_FILES;i++)
    {
      if (Files[i].fp==NULL)
      {
        Files[i].pFileCache=NULL;
        Files[i].pVirtualFilePos=NULL;
        //set fp last since since it enables a "findfile"
        Files[i].fp = fp;
        pFile=&Files[i];
        break;
      }
    }
    rex_leave_crit_sect(&FilesCS);
  }
  //should always be able to add a file unless there
  //are too many open.
  if (!pFile)
  {
//    ErrorMsg( LOG_LVL_ERROR, "ERROR: Can't add file to table",0,0,0);
  }
  return pFile;
}

void FileUtils::RemoveFile(FILE * fp)
{
  FilePointer * pFile = FindFile(fp);
  if (pFile)
  {
    if (pFile->pFileCache)
    {
      QTV_Free(pFile->pFileCache);
      pFile->pFileCache=NULL;
    }
    if (pFile->pVirtualFilePos)
    {
      QTV_Free(pFile->pVirtualFilePos);
      pFile->pVirtualFilePos = NULL;
    }
    //update the fp last to avoid data contention trouble.
    pFile->fp=NULL;
  }
}

FileUtils::FileCache *  FileUtils::FindFileCache(FILE *stream)
//
//find file cache for a stream.
{
  FilePointer * pFile = FindFile(stream);
  if (pFile)
  {
    return pFile->pFileCache;
  }
  return NULL;
}

FileUtils::VirtualFilePos *  FileUtils::FindVirtualFilePos(FILE *stream)
//
//find virtual file pos for a stream.
{
  FilePointer * pFile = FindFile(stream);
  if (pFile)
  {
    return pFile->pVirtualFilePos;
  }
  return NULL;
}

bool FileUtils::RefillFileCache(FILE *stream)
//
// Refill or adjust the file cache so we can do an fread from it.
// Return TRUE if success.
//
{
  FileCache * pFileCache = FindFileCache(stream);
  if (pFileCache)
  {
    //get current actual file pos.
    long nFpos = FileUtils::Ftell(stream);

    //If actual file pos is in the buffer, then simply adjust
    //the cache pointer to match.
    if (pFileCache->nFpos <= nFpos && nFpos < (pFileCache->nFpos+pFileCache->nSize))
    {
      pFileCache->nPos = (int)(nFpos - pFileCache->nFpos);

      //success.
      return true;
    }
    //Otherwise refill the cache from the current file pos
    else
    {
      pFileCache->nSize = FileUtils::VirtualFread(pFileCache->data,1,UTILS_FILECACHESIZE,stream);
      pFileCache->nFpos=nFpos;
      pFileCache->nPos=0;

      //restore the actual filepos, since filling the cache should
      //not update the actual filepos.
      FileUtils::Fseek(stream,nFpos,SEEK_SET);

      if (pFileCache->nSize>0)
      {
        //success.
        return true;
      }
    }
  }
  return false;
}

size_t FileUtils::CacheFread(void *buffer, size_t count,FILE *stream)
//
//  Do a simulated fread of single bytes from the file cache.
//
{
  size_t n=0;

  FileCache * pFileCache = FindFileCache(stream);
  if (pFileCache)
  {
    while (n<count)
    {
      //Get file data into cache.
      if (!RefillFileCache(stream))
      {
        break; //no more data available.
      }

      //See how many bytes to get.
      int nChunk = ZMIN((int)(count-n), pFileCache->nSize - pFileCache->nPos);
      if (nChunk <= 0)
      {
        break; //shouldn't happen.
      }

      //Read data from file cache
      memcpy((void *)((int)buffer+n), pFileCache->data+pFileCache->nPos, nChunk);
      pFileCache->nPos += nChunk;

      //Inc total read item count.
      n += nChunk;

      //Advance the actual file pos
      if (FileUtils::Fseek(stream,nChunk,SEEK_CUR)!=0)
      {
        break; //file error
      }
    }
  }

  // return item count
  return n;
}

size_t FileUtils::VirtualFread( void *buffer, size_t size, size_t count, FILE *stream )
//
// do fread with virtual file pos
{
  VirtualFilePos * pVirtualFilePos = FindVirtualFilePos(stream);

  //Set the actual file pos to the virtual file pos.
  if (pVirtualFilePos)
  {
    #ifdef FILE_IO_STATS
    FilePointer * pFile = FindFile(stream);
    if (pFile) pFile->nFseek++;
    #endif
    if (fseek(stream,pVirtualFilePos->nFpos,SEEK_SET)!=0)
    {
      return 0; //file error
    }
  }

  //Read from file.
  size_t n = fread(buffer,size,count,stream);
  #ifdef FILE_IO_STATS
  FilePointer * pFile = FindFile(stream);
  if (pFile) pFile->nFread++;
  if (pFile) pFile->nFreadBytes+=n;
  #endif

  //Update virtual filepos
  if (pVirtualFilePos)
  {
    pVirtualFilePos->nFpos += (n*size);
  }

  //Return item count.
  return n;
}

////////////////////////////////////////////////////////////////////////////
//
// File I/O Utilities: File Stats
//
////////////////////////////////////////////////////////////////////////////

void FileUtils::ShowFileStats()
{
#ifdef FILE_IO_STATS
  FilePointer * pFile;
  for (int i=0;i<MAX_OPEN_FILES;i++)
  {
    pFile=&Files[i];

    if (pFile)
    {
      dprintf( LOG_LVL_MSG, "File I/O Stats[%d]:",i,0,0);
      dprintf( LOG_LVL_MSG, "   nFseek:      %d",pFile->nFseek,0,0);
      dprintf( LOG_LVL_MSG, "   nFtell:      %d",pFile->nFtell,0,0);
      dprintf( LOG_LVL_MSG, "   nFgetpos:    %d",pFile->nFgetpos,0,0);
      dprintf( LOG_LVL_MSG, "   nFsetpos:    %d",pFile->nFsetpos,0,0);
      dprintf( LOG_LVL_MSG, "   nFread:      %d",pFile->nFread,0,0);
      dprintf( LOG_LVL_MSG, "   nFreadBytes: %d",pFile->nFreadBytes,0,0);
      if (pFile->nFread>0)
        dprintf( LOG_LVL_MSG, "   nFreadAvg:   %d.%03d",
        whole((float)pFile->nFreadBytes/(float)pFile->nFread),
        decimal((float)pFile->nFreadBytes/(float)pFile->nFread),0);
      dprintf( LOG_LVL_MSG, "   nFilesize:   %d",pFile->nFilesize,0,0);
    }
  }
#endif
}

////////////////////////////////////////////////////////////////////////////
//
// File I/O Utilities: Application level routines.
//
////////////////////////////////////////////////////////////////////////////

FILE * FileUtils::Fopen( const char *filename, const char *mode )
{
  if (!bEnable)
  {
    return fopen(filename, mode);
  }

  FILE * fp = fopen(filename,mode);

  if (fp)
  {
    //init file stuff
    FilePointer * pFile = FindFile(fp);
    if (pFile) RemoveFile(fp);
    pFile = AddFile(fp);
    if (pFile)
    {
      #ifdef FILE_IO_STATS
      //reset file stats.
      pFile->nFread=pFile->nFseek=pFile->nFsetpos=pFile->nFgetpos=pFile->nFtell=0;
      pFile->nFread=0;
      pFile->nFreadBytes=0;
      pFile->nFilesize=0;
      #endif
      //init file cache
      if (UTILS_FILECACHESIZE > 1)
      {
        pFile->pFileCache = (FileCache *)QTV_Malloc(sizeof(FileCache));
        if (pFile->pFileCache)
        {
          pFile->pFileCache->nSize=0;
          pFile->pFileCache->nPos=0;
          pFile->pFileCache->nFpos=0;
        }
      }
      //init virtual file pos.
      if (UTILS_FILECACHESIZE > 1)
      {
        pFile->pVirtualFilePos = (VirtualFilePos *)QTV_Malloc(sizeof(VirtualFilePos));
        if (pFile->pVirtualFilePos)
        {
          //Get file pos limits for this file.
          int s = 0;
          //seek to end.
          if (s==0) s = fseek(fp,0,SEEK_END);
          if (s==0)
          {
            pFile->pVirtualFilePos->nFposEnd = ftell(fp);
            s = (pFile->pVirtualFilePos->nFposEnd != (-1)) ? 0 : (-1);
          }
          #ifdef FILE_IO_STATS
          pFile->nFilesize = ftell(fp);
          #endif
          //seek back to start
          if (s==0) s = fseek(fp,0,SEEK_SET);
          if (s==0)
          {
            pFile->pVirtualFilePos->nFposSet = ftell(fp);
            s = pFile->pVirtualFilePos->nFposSet != (-1) ? 0 : (-1);
          }
          //set current pos to start pos.
          pFile->pVirtualFilePos->nFpos = pFile->pVirtualFilePos->nFposSet;
          //if there were any file errors, then invalidate the virtual filepos.
          if (s!=0)
          {
            QTV_Free(pFile->pVirtualFilePos);
            pFile->pVirtualFilePos=NULL;
          }
        }
      }
    }
  }
  return fp;
}

int FileUtils::Fclose( FILE *stream )
{
  if (!bEnable)
  {
    return fclose(stream);
  }

  //free file info.
  FilePointer * pFile = FindFile(stream);
  if (pFile)
  {
    RemoveFile(stream);
  }

/* ------
Commented out, since this code goes to all targets as common and 
below REX API is not currently ported to all active targets and baselines.
--------- */
  //rex_del_crit_sect(&FilesCS);
  return fclose(stream);
}

int FileUtils::Fseek( FILE *stream, long offset, int origin )
{
  if (!bEnable)
  {
    return fseek(stream,offset,origin);
  }

  int s;

  //Do a virtual seek if possible.
  VirtualFilePos * pVirtualFilePos = FindVirtualFilePos(stream);
  if (pVirtualFilePos)
  {
    switch (origin)
    {
    case SEEK_SET:
      pVirtualFilePos->nFpos = pVirtualFilePos->nFposSet + offset;
      s = 0;
      break;
    case SEEK_CUR:
      pVirtualFilePos->nFpos = pVirtualFilePos->nFpos + offset;
      s = 0;
      break;
    case SEEK_END:
      pVirtualFilePos->nFpos = pVirtualFilePos->nFposEnd + offset;
      s = 0;
      break;
    default:
      s = (-1);
      break;
    }
    //seeking past the end of file is OK, but seeking past the
    //beginning is an error.
    if (s==0)
    {
      if (pVirtualFilePos->nFpos < pVirtualFilePos->nFposSet)
      {
        s = (-1);
      }
    }
  }
  //Pure filesystem fseek.
  else
  {
    #ifdef FILE_IO_STATS
    FilePointer * pFile = FindFile(stream);
    if (pFile) pFile->nFseek++;
    #endif
    s = fseek(stream,offset,origin);
  }
  //Error check.
  if (s!=0)
  {
    ZUtils::ThrowException(COMMON_EXCEPTION_FILE_ERROR,1);
  }
  return s;
}

int FileUtils::Fgetpos( FILE *stream, fpos_t *pos )
{
  if (!bEnable)
  {
    return fgetpos(stream,pos);
  }

  int s;

  //Get the virtual file pos if available.
  VirtualFilePos * pVirtualFilePos = FindVirtualFilePos(stream);
  if (pVirtualFilePos)
  {
#ifdef __CC_ARM
    pos->__lo = pVirtualFilePos->nFpos;
#else
    *pos = pVirtualFilePos->nFpos;
#endif
    s = 0; //success
  }
  //Otherwise get actual file pos
  else
  {
    #ifdef FILE_IO_STATS
    FilePointer * pFile = FindFile(stream);
    if (pFile) pFile->nFgetpos++;
    #endif
    s = fgetpos(stream,pos);
  }
  //Error check.
  if (s!=0)
  {
    ZUtils::ThrowException(COMMON_EXCEPTION_FILE_ERROR,2);
  }
  return s;
}

int FileUtils::Fsetpos( FILE *stream, const fpos_t *pos )
{
  if (!bEnable)
  {
    return fsetpos(stream,pos);
  }

  int s;

  //Set virtual file pos if available
  VirtualFilePos * pVirtualFilePos = FindVirtualFilePos(stream);
  if (pVirtualFilePos)
  {
#ifdef __CC_ARM
    pVirtualFilePos->nFpos = (long)pos->__lo;
#else
    pVirtualFilePos->nFpos = (long)*pos;
#endif
    s = 0;
  }
  //Otherwise set actual file pos
  else
  {
    #ifdef FILE_IO_STATS
    FilePointer * pFile = FindFile(stream);
    if (pFile) pFile->nFsetpos++;
    #endif
    s = fsetpos(stream,pos);
  }
  //Error handling.
  if (s!=0)
  {
    ZUtils::ThrowException(COMMON_EXCEPTION_FILE_ERROR,3);
  }
  return s;
}

long FileUtils::Ftell( FILE *stream )
{
  if (!bEnable)
  {
    return ftell(stream);
  }

  long s;

  //Get virtual file pos if available
  VirtualFilePos * pVirtualFilePos = FindVirtualFilePos(stream);
  if (pVirtualFilePos)
  {
    s = pVirtualFilePos->nFpos;
  }
  //Otherwise get real file pos.
  else
  {
    s = ftell(stream);
  }
  //Error handling.
  if (s==(-1))
  {
    ZUtils::ThrowException(COMMON_EXCEPTION_FILE_ERROR,4);
  }
  return s;
}

size_t FileUtils::Fread( void *buffer, size_t size, size_t count, FILE *stream )
{
  if (!bEnable)
  {
    return fread(buffer, size, count, stream);
  }

  size_t n;

  //Use the cache when reading small counts of single bytes.
  FileCache * pFileCache = FindFileCache(stream);
  //note: compiler bug on the target, if I put the count compare last it compares
  //to zero instead of the cachesize!
  if ((count < UTILS_FILECACHELIMIT) && (size==1) && (pFileCache != NULL))
  {
    n = CacheFread(buffer,count,stream);
  }
  // Otherwise read directly from the file
  else
  {
    //printf("V.Fread sz %d count %d ftell %d",size,count,(int)FileUtils::Ftell(stream));
    n = VirtualFread(buffer,size,count,stream);
  }
  //Return item count
  return n;
}

//////////////////////////////////////////////////////////////////////
//
// Enable/disable file utils
//
//////////////////////////////////////////////////////////////////////

//Note: the application must not enable or disable while any file is
//open.

void FileUtils::EnableReadCache()
{
  bEnable=true;
}

void FileUtils::DisableReadCache()
{
  bEnable=false;
}

//////////////////////////////////////////////////////////////////////
//
// Common Init Routine
//
//////////////////////////////////////////////////////////////////////

void FileUtils::Init()
{
  //init critical section objects
  rex_init_crit_sect(&FilesCS);

  //set files array to empty
  for (int i=0;i<MAX_OPEN_FILES;i++)
  {
    Files[i].fp=NULL;
  }

  //default is cache enabled.
  bEnable=true;
}

#endif //PV_OS_ZREX
