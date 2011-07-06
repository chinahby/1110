/*==========================================================================
  
               zrex_FileUtils.h	
												
	Description: This is the include file for the file system
		developed for the zrex player.

    Features of the zrex file system:
	1) file i/o status (counts numbers of calls)
	2) raises exceptions on file errors
	3) file cacheing
	4) virtual file position.
	5) debug printout.
								
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved;
and Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */


//#ifdef PV_OS_ZREX
#if 1

#ifndef _ZREX_FILEUTILS_H_
#define _ZREX_FILEUTILS_H_

#include <stdio.h>

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"


//////////////////////////////////////////////////////////////////////
//
// Engineering build options...
//
//////////////////////////////////////////////////////////////////////

#ifndef QC_RELEASE_BUILD
	#ifdef T_WINNT
#error code not present
	#else
		#undef FILE_IO_STATS
	#endif
#endif

//////////////////////////////////////////////////////////////////////
//
// Parameters
//
//////////////////////////////////////////////////////////////////////


//This is the number of bytes read by the zrex file system.
//It always reads in blocks of 512 no matter how many bytes
//you request.  Don't change this, since it reflects the hw
//characteristic.
#define UTILS_FILEREADBLOCKSIZE 512

//Set file cache size here...
//To disable cache, set size = 1.
//When using the cache, set it to a multiple of the file read block size.
#define UTILS_FILECACHESIZE (1*UTILS_FILEREADBLOCKSIZE)

//Set file cache limit here...
//We only use the cache when reading fewer than this number of bytes.
//Note: due to the zrex file block reading, this should be a multiple of
//the fileblock size.  Just looking at the stats on a typical mpeg-4, a 
//smaller cache limit is better (64 is good).
#define UTILS_FILECACHELIMIT UTILS_FILEREADBLOCKSIZE

//////////////////////////////////////////////////////////////////////
//
// FileUtils class
//
//////////////////////////////////////////////////////////////////////

class FileUtils 
{

public:

	//(This is only used internally but must be public for the ARM compiler)
	struct FileCache
	{
		//file data cache
		unsigned char data[UTILS_FILECACHESIZE]; 
		//number of file bytes in cache
		int nSize; 
		//offset of current byte in cache
		int nPos; 
		//equivalent fpos of first byte in cache
		long nFpos; 
	};

	//(This is only used internally but must be public for the ARM compiler)
	struct VirtualFilePos
	{
		//virtal file pos
		long nFpos, nFposSet, nFposEnd;
	};

	//(This is only used internally but must be public for the ARM compiler)
	struct FilePointer
	{
		//actual file pointer
		FILE * fp;
		//cache for this file
		FileCache * pFileCache;
		//virtual file pos for this file.
		VirtualFilePos * pVirtualFilePos;
#ifdef FILE_IO_STATS
		int nFread;		
		//I/O stats for this file.
		int nFseek;
		int nFtell;		
		int nFgetpos;		
		int nFsetpos;		
		int nFreadBytes;	//total bytes read from file
		long nFilesize; //actual file size
#endif
	};

	//Init routine for this module.
	static void Init();

	//Enable/disable file cache.
	static void EnableReadCache();
	static void DisableReadCache();

	//File I/O utils
	static int Fseek( FILE *stream, long offset, int origin );
	static int Fgetpos( FILE *stream, fpos_t *pos );
	static int Fsetpos( FILE *stream, const fpos_t *pos );
	static long Ftell( FILE *stream );
	static FILE * Fopen( const char *filename, const char *mode );
	static int Fclose( FILE *stream );
	static size_t Fread( void *buffer, size_t size, size_t count, FILE *stream ) ;
	static void ShowFileStats();


private:


	static FilePointer * FindFile(FILE * fp);
	static FilePointer * AddFile(FILE * fp);
	static void RemoveFile(FILE * fp);
	static bool RefillFileCache(FILE *stream);
	static size_t CacheFread(void *buffer, size_t count,FILE *stream);
	static FileCache *  FindFileCache(FILE *stream);
	static VirtualFilePos *  FindVirtualFilePos(FILE *stream);
	static size_t VirtualFread( void *buffer, size_t size, size_t count, FILE *stream ) ;

};	

#endif

#endif //PV_OS_ZREX
