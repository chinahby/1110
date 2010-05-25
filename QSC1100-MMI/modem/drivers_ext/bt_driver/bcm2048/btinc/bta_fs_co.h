/*****************************************************************************
**
**  Name:           bta_fs_co.h
**
**  Description:    This is the interface file for the synchronization
**                  server call-out functions.
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_FS_CO_H
#define BTA_FS_CO_H

#include "bta_api.h"
#include "goep_fs.h"

/*****************************************************************************
**  Constants and Data Types
*****************************************************************************/
/**************************
**  Common Definitions
***************************/

/* Status codes returned by call-out functions, or in call-in functions as status */
#define BTA_FS_CO_OK            GOEP_OK
#define BTA_FS_CO_FAIL          GOEP_FAIL   /* Used to pass all other errors */
#define BTA_FS_CO_EACCES        GOEP_EACCES
#define BTA_FS_CO_ENOTEMPTY     GOEP_ENOTEMPTY
#define BTA_FS_CO_EOF           GOEP_EOF
#define BTA_FS_CO_EODIR         GOEP_EODIR
#define BTA_FS_CO_ENOSPACE      GOEP_ENOSPACE/* Returned in bta_fs_ci_open if no room */

typedef UINT16 tBTA_FS_CO_STATUS;

/* Flags passed to the open function (bta_fs_co_open)
**      Values are OR'd together. (First 3 are
**      mutually exclusive.
*/
#define BTA_FS_O_RDONLY         GOEP_O_RDONLY
#define BTA_FS_O_WRONLY         GOEP_O_WRONLY
#define BTA_FS_O_RDWR           GOEP_O_RDWR  

#define BTA_FS_O_CREAT          GOEP_O_CREAT 
#define BTA_FS_O_EXCL           GOEP_O_EXCL  
#define BTA_FS_O_TRUNC          GOEP_O_TRUNC 

#define BTA_FS_O_MODE_MASK(x)      (((UINT16)(x)) & 0x0003)

/* Origin for the bta_fs_co_seek function  */
#define BTA_FS_SEEK_SET         GOEP_SEEK_SET
#define BTA_FS_SEEK_CUR         GOEP_SEEK_CUR
#define BTA_FS_SEEK_END         GOEP_SEEK_END

/* mode field in bta_fs_co_access callout */
#define BTA_FS_ACC_EXIST        GOEP_ACC_EXIST
#define BTA_FS_ACC_READ         GOEP_ACC_READ
#define BTA_FS_ACC_RDWR         GOEP_ACC_RDWR

#define BTA_FS_LEN_UNKNOWN      GOEP_LEN_UNKNOWN
#define BTA_FS_INVALID_FD       GOEP_INVALID_FD
#define BTA_FS_INVALID_APP_ID   (0xFF)  /* this app_id is reserved */

/* mode field in tBTA_FS_DIRENTRY (OR'd together) */
#define BTA_FS_A_RDONLY         GOEP_A_RDONLY
#define BTA_FS_A_DIR            GOEP_A_DIR      /* Entry is a sub directory */

#define BTA_FS_CTIME_LEN        GOEP_CTIME_LEN  /* Creation time "yyyymmddTHHMMSSZ" */

/* Return structure type for a directory entry */
typedef struct
{
    UINT32  refdata;            /* holder for OS specific data used to get next entry */
    UINT32  filesize;
    char    crtime[BTA_FS_CTIME_LEN]; /* "yyyymmddTHHMMSSZ", or "" if none */
    char    *p_name;            /* Contains the addr of memory to copy name into */
    UINT8   mode;               /* BTA_FS_A_RDONLY and/or BTA_FS_A_DIR */
} tBTA_FS_DIRENTRY;


/*****************************************************************************
**  Function Declarations
*****************************************************************************/
/**************************
**  Common Functions
***************************/
/*******************************************************************************
**
** Function         bta_fs_co_open
**
** Description      This function is executed by BTA when a file is opened.
**                  The phone uses this function to open 
**                  a file for reading or writing.
**
** Parameters       p_path  - Fully qualified path and file name.
**                  oflags  - permissions and mode (see constants above)
**                  size    - size of file to put (0 if unavailable or not applicable)
**                  evt     - event that must be passed into the call-in function.
**                  app_id  - application ID specified in the enable functions.
**                            It can be used to identify which profile is the caller
**                            of the call-out function.
**
** Returns          void
**                  
**                  Note: Upon completion of the request, a file descriptor (int),
**                        if successful, and an error code (tBTA_FS_CO_STATUS)
**                        are returned in the call-in function, bta_fs_ci_open().
**
*******************************************************************************/
BTA_API extern void bta_fs_co_open(const char *p_path, int oflags, UINT32 size,
                           UINT16 evt, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_close
**
** Description      This function is called by BTA when a connection to a
**                  client is closed.
**
** Parameters       fd      - file descriptor of file to close.
**                  app_id  - application ID specified in the enable functions.
**                            It can be used to identify which profile is the caller
**                            of the call-out function.
**
** Returns          (tBTA_FS_CO_STATUS) status of the call.
**                      [BTA_FS_CO_OK if successful],
**                      [BTA_FS_CO_FAIL if failed  ]
**
*******************************************************************************/
BTA_API extern tBTA_FS_CO_STATUS bta_fs_co_close(int fd, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_read
**
** Description      This function is called by BTA to read in data from the
**                  previously opened file on the phone.
**
** Parameters       fd      - file descriptor of file to read from.
**                  p_buf   - buffer to read the data into.
**                  nbytes  - number of bytes to read into the buffer.
**                  evt     - event that must be passed into the call-in function.
**                  app_id  - application ID specified in the enable functions.
**                            It can be used to identify which profile is the caller
**                            of the call-out function.
**
** Returns          void
**                  
**                  Note: Upon completion of the request, bta_fs_ci_read() is
**                        called with the buffer of data, along with the number
**                        of bytes read into the buffer, and a status.  The
**                        call-in function should only be called when ALL requested
**                        bytes have been read, the end of file has been detected,
**                        or an error has occurred.
**
*******************************************************************************/
BTA_API extern void bta_fs_co_read(int fd, UINT8 *p_buf, UINT16 nbytes, UINT16 evt,
                           UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_write
**
** Description      This function is called by io to send file data to the
**                  phone.
**
** Parameters       fd      - file descriptor of file to write to.
**                  p_buf   - buffer to read the data from.
**                  nbytes  - number of bytes to write out to the file.
**                  evt     - event that must be passed into the call-in function.
**                  app_id  - application ID specified in the enable functions.
**                            It can be used to identify which profile is the caller
**                            of the call-out function.
**
** Returns          void
**                  
**                  Note: Upon completion of the request, bta_fs_ci_write() is
**                        called with the file descriptor and the status.  The
**                        call-in function should only be called when ALL requested
**                        bytes have been written, or an error has been detected,
**
*******************************************************************************/
BTA_API extern void bta_fs_co_write(int fd, const UINT8 *p_buf, INT16 nbytes, UINT16 evt,
                            UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_seek
**
** Description      This function is called by io to move the file pointer
**                  of a previously opened file to the specified location for 
**                  the next read or write operation.
**
** Parameters       fd      - file descriptor of file.
**                  offset  - Number of bytes from origin.
**                  origin  - Initial position: BTA_FS_SEEK_SET, BTA_FS_SEEK_CUR,
**                            or BTA_FS_SEEK_END.
**
** Returns          void
**                  
*******************************************************************************/
BTA_API extern void bta_fs_co_seek (int fd, INT32 offset, INT16 origin, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_access
**
** Description      This function is called to check the existence of a file or
**                  directory.
**
** Parameters       p_path   - (input) file or directory to access (fully qualified path).
**                  mode     - (input) [BTA_FS_ACC_EXIST, BTA_FS_ACC_READ, or BTA_FS_ACC_RDWR]
**                  p_is_dir - (output) returns TRUE if p_path specifies a directory.
**                  app_id   - (input) application ID specified in the enable functions.
**                                     It can be used to identify which profile is the caller
**                                     of the call-out function.
**
** Returns          (tBTA_FS_CO_STATUS) status of the call.
**                   [BTA_FS_CO_OK if it exists]
**                   [BTA_FS_CO_EACCES if permissions are wrong]
**                   [BTA_FS_CO_FAIL if it does not exist]
**
*******************************************************************************/
BTA_API extern tBTA_FS_CO_STATUS bta_fs_co_access(const char *p_path, int mode,
                                          BOOLEAN *p_is_dir, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_mkdir
**
** Description      This function is called to create a directory with
**                  the pathname given by path. The pathname is a null terminated
**                  string. All components of the path must already exist.
**
** Parameters       p_path   - (input) name of directory to create (fully qualified path).
**                  app_id   - (input) application ID specified in the enable functions.
**                                     It can be used to identify which profile is the caller
**                                     of the call-out function.
**
** Returns          (tBTA_FS_CO_STATUS) status of the call.
**                  [BTA_FS_CO_OK if successful]
**                  [BTA_FS_CO_FAIL if unsuccessful]
**
*******************************************************************************/
BTA_API extern tBTA_FS_CO_STATUS bta_fs_co_mkdir(const char *p_path, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_rmdir
**
** Description      This function is called to remove a directory whose
**                  name is given by path. The directory must be empty.
**
** Parameters       p_path   - (input) name of directory to remove (fully qualified path).
**                  app_id   - (input) application ID specified in the enable functions.
**                                     It can be used to identify which profile is the caller
**                                     of the call-out function.
**
** Returns          (tBTA_FS_CO_STATUS) status of the call.
**                      [BTA_FS_CO_OK if successful]
**                      [BTA_FS_CO_EACCES if read-only]
**                      [BTA_FS_CO_ENOTEMPTY if directory is not empty]
**                      [BTA_FS_CO_FAIL otherwise]
**
*******************************************************************************/
BTA_API extern tBTA_FS_CO_STATUS bta_fs_co_rmdir(const char *p_path, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_unlink
**
** Description      This function is called by to remove a file whose name
**                  is given by p_path.
**
** Parameters       p_path   - (input) name of file to remove (fully qualified path).
**                  app_id   - (input) application ID specified in the enable functions.
**                                     It can be used to identify which profile is the caller
**                                     of the call-out function.
**
** Returns          (tBTA_FS_CO_STATUS) status of the call.
**                      [BTA_FS_CO_OK if successful]
**                      [BTA_FS_CO_EACCES if read-only]
**                      [BTA_FS_CO_FAIL otherwise]
**
*******************************************************************************/
BTA_API extern tBTA_FS_CO_STATUS bta_fs_co_unlink(const char *p_path, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_fs_co_getdirentry
**
** Description      This function is called to retrieve a directory entry for the
**                  specified path.  The first/next directory should be filled
**                  into the location specified by p_entry.
**
** Parameters       p_path      - directory to search (Fully qualified path)
**                  first_item  - TRUE if first search, FALSE if next search
**                                      (p_cur contains previous)
**                  p_entry (input/output) - Points to last entry data (valid when
**                                           first_item is FALSE)
**                  evt     - event that must be passed into the call-in function.
**                  app_id  - application ID specified in the enable functions.
**                            It can be used to identify which profile is the caller
**                            of the call-out function.
**
** Returns          void
**                  
**                  Note: Upon completion of the request, the status is passed
**                        in the bta_fs_ci_direntry() call-in function.
**                        BTA_FS_CO_OK is returned when p_entry is valid,
**                        BTA_FS_CO_EODIR is returned when no more entries [finished]
**                        BTA_FS_CO_FAIL is returned if an error occurred
**
*******************************************************************************/
BTA_API extern void bta_fs_co_getdirentry(const char *p_path, BOOLEAN first_item,
                                   tBTA_FS_DIRENTRY *p_entry, UINT16 evt,
                                   UINT8 app_id);

#endif /* BTA_FS_CO_H */
