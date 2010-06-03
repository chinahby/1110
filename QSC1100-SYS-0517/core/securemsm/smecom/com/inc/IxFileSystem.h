#ifndef __IXFILESYSTEM_H__
#define __IXFILESYSTEM_H__

/***********************************************************************
 * IxFileSystem.h
 *
 * Public API for IxFile file systems.
 * Copyright( C ) 2007, Qualcomm, Inc.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when          who     what, where, why
--------      ---     ------------------------------------------------------
2007-11-01    jee     Initial release

===========================================================================*/

#include "IxFileSystemTypes.h"

#ifdef __cplusplus

class IxFileSystem 
{
public:

    virtual ~IxFileSystem() {}

    /**
    * open
    *
    * Open a file in the filesystem. 
    *
    * @retval
    *    -1 on error,( file-descriptor ) >= 0 on success.
    **********************************************************************/
    virtual int Open( const char *path, int oflag, ... ) = 0;

    /**
    * Close the file descriptor.  The descriptor will no longer refer to any
    * file, and may be returned by subsequent calls to open.
    *
    * @param filedes
    *    The file-descriptor obtained earlier via open(  ) function.
    *
    * @retval
    *    Normal result is 0, error is -1.
    **********************************************************************/
    virtual int Close( int filedes ) = 0;

    /**
    * Attempt to read 'nbytes' of data from the file associated with the
    * specified file descriptor.
    *
    * @param filedes
    *    The file-descriptor obtained earlier via open(  ) function.
    *
    * @param buf
    *    The buffer where the read bytes from the file will be stored..
    *
    * @param nbyte
    *    The number of bytes to read from the file.
    *
    * @retval
    *    Returns the number of bytes successfully read, or -1 for an error.
    *    Zero is a valid result, and generally indicates that the end of file has
    *    been reached.
    *    It is permitted for read to return fewer bytes than were requested,
    *    even if the data is available in the file.
    **********************************************************************/
    virtual signed long int Read( int filedes, void *buf,
      unsigned long int nbyte ) = 0;

    /**
    * Attempt to write 'nbytes' of data to the file associated with the
    * specified file descriptor.
    *
    * @param filedes
    *    The file-descriptor obtained earlier via open(  ) function.
    *
    * @param buf
    *    The buffer that needs to get written onto the file.
    *
    * @param nbyte
    *    The number of bytes to write to the file.
    *
    * @retval
    *    Returns the number of bytes successfully written or -1 for an error.\n
    *    Zero generally indicates that the filesystem is full( writing again
    *    should result in an ENOSPC error ).\n
    *    It is permitted for write to write fewer bytes than were requested,
    *    even if space is available.
    **********************************************************************/
    virtual signed long int Write( int filedes, const void *buf,
      unsigned long int nbyte ) = 0;

    /**
    * Check a user's permissions for a file.  Checks if the file would be
    * possible to be accessed with the given mode.
    *
    **********************************************************************/
    virtual int Access( const char *path, int amode ) = 0;

    /**
    * Create a new directory.
    *
    * @param path
    *    The file path.
    *
    * @param mode
    *    'mode' specifies the permission bits of the new directory.
    *
    * @retval
    *    Returns 0 for success, or -1 for an error.
    **********************************************************************/
    virtual int MkDir( const char *path, unsigned short mode ) = 0;

    /**
    *
    * fstat(  ) is used to get information about an opened file handle.
    *
    * lstat(  ) and stat(  ) are very similar, in that they both return
    * information about a given pathname.  They differ in how they treat the
    * last component if it is a symlink.  'stat(  )' will "dereference" this
    * symlink, returning the information about what the symlink points to.  It
    * gives the same perspective that a call to 'open(  )' or
    * 'opendir(  )' would give of the symlink.  These are the correct
    * semantics for most applications wanting to find out about files.
    *
    * The lstat(  ) call on the other hand gives the information about any
    * symlinks.  This call should be used by any type of application that
    * needs to be aware of symlinks.  The most common is a type of file
    * browser that wishes to give visibility into the real file types,
    * including the presence of symlinks.
    *
    */

    /**
    * Return additional information about the named file/directory.
    *
    * @param path
    *    The file path.
    *
    * @param buf
    *    On success, the fields of 'buf' are filled in.
    *
    * @retval
    *    A -1 result indicates an error and 0 is success.
    *
    * @note
    *    stat follows symlinks. Please see lstat(  ) if do not want to
    *    to follow the symlinks.
    **********************************************************************/
    virtual int Stat( const char *path, IxFileStatType *buf ) = 0;

    /**
    * Return additional information about an opened file.  Otherwise similar
    * to stat.
    *
    * @param fildes
    *    The file-descriptor of an open file obtained from open(  ) function.
    *
    * @param buf
    *    On success, the fields of 'buf' are filled in.
    *
    * @retval
    *    A -1 result indicates an error and 0 is success.
    *
    * @note
    *    fstat follows symlinks.
    ***********************************************************************/
    virtual int FStat( int fildes, IxFileStatType *buf ) = 0;

    /**
    * Return additional information.  If the final component of the path is
    * not a symlink, this is identical to stat.  If the final component is
    * a symlink, stat returns information about the file pointed to by the
    * symlink.  lstat, instead, returns information about the symlink
    * itself.
    *
    * @param path
    *    The file path.
    *
    * @param buf
    *    On success, the fields of 'buf' are filled in.
    *
    * @retval
    *    A -1 result indicates an error and 0 is success.
    *
    * @note
    *    lstat does NOT follow symlinks. Please see ::stat(  ) if you want
    *    to follow the symlinks as well.
    **********************************************************************/
    virtual int LStat( const char *path, IxFileStatType *buf ) = 0;

    /**
    * Obtain information about an entire filesystem.
    *
    * @param path
    *    path should be a pathname of a valid entity of the filesystem( the
    *    root directory works ).
    *
    * @param buf
    *    On success the buf will be filled.
    *
    * @retval
    *    Returns 0, and fills in 'buf' on success, otherwise returns -1.
    **********************************************************************/
    virtual int StatVfs( const char *path, IxFileStatFSType *buf ) = 0;

    /**
    * Obtatin information about an entire filesystem using a file descriptor
    * for a handle. Otherwise similar to statvfs.
    *
    * @param fildes
    *    The file-descriptor of an open file obtained from open(  ) function.
    *
    * @param buf
    *    On success the buf will be filled.
    *
    * @retval
    *    Returns 0, and fills in 'buf' on success, otherwise returns -1.
    ***********************************************************************/
    virtual int FStatVfs( int fildes, IxFileStatFSType *buf ) = 0;


    /**
    * lseek
    *
    * Change the file offset for the opened file descriptor.
    *
    * @param filedes
    *    The file-descriptor of an open file obtained from open(  ) function.
    *
    * @param offset
    *    The new offset of the file.
    *
    * @param whence
    *    'whence' indicates how the new offset is computed:
    *      IxFile_SEEK_SET - Set to 'offset'.
    *      IxFile_SEEK_CUR - Set to 'offset' + current position.
    *      IxFile_SEEK_END - Set to 'offset' + file size.
    *
    * @retval
    *    Upon success, lseek returns the resulting offset, as bytes from the
    *    beginning of the file.  Otherwise( off_t ) -1 is returned.
    **********************************************************************/
    virtual signed long int LSeek( int filedes, signed long int offset,
      int whence ) = 0;


    /**
    * Truncate a file to a specified length.  The file will be extended, or
    * shrunk to the newly specified size.
    *
    * @param path
    *    The path of the file whose length has to be changed.
    *
    * @param length
    *    The new size of the file.
    *
    * @retval
    *    Returns 0 on success, or -1 for error.
    **********************************************************************/
    virtual signed long int Truncate( const char *path,
      signed long int length ) = 0;

    /**
    * Truncate an open file.  Similar to truncate.  The file must be
    * opened for writing.
    *
    * @param fildes
    *    The file-descriptor of an open file.
    *
    * @param length
    *    The new size of the file.
    *
    * @retval
    *    Returns 0 on success, or -1 for error.
    **********************************************************************/
    virtual signed long int FTruncate( int fildes, signed long int length ) = 0;

    /**
    * Delete a name in the filesystem.  If this is the only reference to this
    * file, the file itself will also be deleted.  unlink can be used on
    * all filesystem objects, except for directories.  Use RmDir for
    * directories.
    *
    * @param path
    *    The file that needs to be deleted.
    *
    * @retval
    *    Returns 0 for success, or -1 for error.
    **********************************************************************/
    virtual int Unlink( const char *path ) = 0;

    /**
    * Open a directory for iteration.  The pointer returned is an opaque
    * handle that can be used to iterate through the entries of the directory.
    * The entries should be read with readdir, and closedir called at
    * the end.
    *
    * @param dirname
    *    The base path under which to iterate.
    *
    * @retval
    *    Returns NULL on error, otherwise an opaque pointer.
    *
    **********************************************************************/
    virtual unsigned long int OpenDir( const char *dirname ) = 0;

    /**
    * Read the next entry from the opened directory.
    *
    * @param driHandle
    *    The iterator handle obtained earlier from the ::opendir(  ) function.
    * @param dirEnt
    *    Pointer to IxFileDirEntT structure to fill in.
    *
    * @retval
    *    Returns 0 on success, or -1 on error.
    *    On success, returns a pointer to a struct IxFileDirEntT, which contains
    *    a field d_name which is tne name of this directory component.  Returns
    *    a NULL pointer on error or end of file.
    **********************************************************************/
    virtual int ReadDir( unsigned long int dirHandle, IxFileDirEntT *dirEnt ) = 0;

    /**
    * closedir
    *
    * Close a directory iterator.
    *
    * @param dirp
    *    The iterator handle obtained earlier from the ::opendir(  ) function.
    *
    * @retval
    *    Returns 0 on success or -1 for an error.
    **********************************************************************/
    virtual int CloseDir( unsigned long int dirhandle ) = 0;

    /**
    * Renames a file.  Currently, only regular files can be renamed.
    * 'oldpath' and 'newpath' do not have to be in the same directory( but
    * must be on the same filesystem device ).
    *
    * @retval
    *    Returns 0 on success, or -1 on error.
    *
    **********************************************************************/
    virtual int Rename( const char *oldpath, const char *newpath ) = 0;

    /**
    * Remove a directory.  Only empty directories can be removed.
    *
    * @param path
    *    The file the needs to be removed.
    *
    * @retval
    *    Returns 0 on success or -1 on error.
    **********************************************************************/
    virtual int RmDir( const char *path ) = 0;

    /**
    * ChMod
    **********************************************************************/
    virtual int ChMod( const char *path, unsigned short mode ) = 0;

    /**
    * Create a symlink containing the string 'oldpath'.  Future references to
    * 'newpath' act as if 'oldpath' were substituted for the name component of
    * the path.  If 'oldpath' is a relative path, it is interpreted relative
    * to the directory 'newpath' resides in.
    */
    virtual int SymLink( const char *oldpath, const char *newpath ) = 0;

    /**
    * Read the value of a symlink.  Up to 'bufsiz' bytes of data will be
    * copied into 'buf'.  Note that the result is not null terminated.
    * Returns the number of bytes placed into 'buf', or '-1' on error.
    */
    virtual int ReadLink( const char *path, char *buf, unsigned long int bufsiz ) = 0;

    /**
    * Initialize
    *
    * Initialize the filesystem and mount the root filesystem.
    *
    * Returns 0 on success, otherwise returns an error code( does not set
    * errno ).
    **********************************************************************/
    virtual int Initialize( void ) = 0;

    /**
    * PowerDown
    *
    * Place the filesystem in a state where it is safe to remove power.
    **********************************************************************/
    virtual int PowerDown( void ) = 0;

    /**
    * Mount a filesystem at a given directory.  Unlike Posix, the name given
    * is a parsed string describing the source of the filesystem.
    *
    * The [special] argument should be an integer index for the given
    * filesystem.  The [fstype] argument should be the name of the filesystem
    * type( such as "romfs" ).
    *
    * At this time, flags and "data" are zero.
    *
    * Returns 0 for success.
    */
    virtual int Mount( const char *special, const char *dir,
        const char *fstype, int flags, const void *data ) = 0;

    /**
    * Unmounts a previously mounted filesystem. 
    */
    virtual int Umount( const char *target ) = 0;

    /**
    * Relocates where an existing mountpoint is in the filesystem.
    */
    virtual int Remount( const char *oldtarget, const char *newtarget ) = 0;

    /**
    * Reset the filesystem and stop the system.
    */
    virtual void Reset( void ) = 0;

    /**
    * Change the Owner ID or Group ID of the specified file or directory.
    *
    */
    virtual int ChOwn( const char *path, int uid_val, int gid_val ) = 0;

    virtual int GetLastError( void ) = 0;
};

#endif /* __cplusplus */

#endif /* End of __IXFILESYSTEM_H__ */
