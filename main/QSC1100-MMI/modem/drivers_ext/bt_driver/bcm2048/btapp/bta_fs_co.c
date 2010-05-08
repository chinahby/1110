/*****************************************************************************
**
**  Name:           bta_fs_co.c
**
**  Description:    This file contains the file system call-out
**                  function implementation for Insight.
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/


#include "gki.h"
#include "bta_fs_co.h"
#include "bta_fs_ci.h"
//#include "btapp_flash.h"
#ifndef FLASH_AVAILABLE
#define FLASH_AVAILABLE TRUE
#endif
#define FEATURE_USE_OEM_FILEMGR /*for support vm848*/
//#if FLASH_AVAILABLE == TRUE
/* platform specific include file for FS */
//#include "fs.h"
//#include "msg.h"
//#endif
#ifdef FEATURE_USE_OEM_FILEMGR
#include "bt_ui_ht_common.h"

#ifdef FEATURE_SUPPORT_VC0848
#include "../../../vc0848/vc0848.h"
#endif

typedef enum _bt_enum_type
{
    BT_ENUM_FILES_INIT = 0x00,                           /* Enumerate files only */
    BT_ENUM_DIRECTORIES,                      /* Enumerate directories only */
    BT_ENUM_FILES
}bt_enum_type;
static IFileMgr* pifmgr = NULL;
static bt_enum_type enum_type = BT_ENUM_DIRECTORIES;

/*****************************************************************************
**  Function Declarations
*****************************************************************************/
//#if FLASH_AVAILABLE == TRUE
//static fs_enum_iterator_type bta_fs_enum;
//#endif
/*******************************************************************************
**
** Function         bta_fs_convert_status
**
** Description      This function converts the return status of an MSM FS
**                  function to a BTA FS status value.
**
** Returns          BTA FS status value.
**
*******************************************************************************/
#if 0
#if FLASH_AVAILABLE == TRUE
static tBTA_FS_CO_STATUS bta_fs_convert_status(fs_status_type status)
{
    tBTA_FS_CO_STATUS co_status;
    
    switch (status) {
    case FS_OKAY_S:
        co_status = BTA_FS_CO_OK;
        break;

    case FS_ILLEGAL_OPERATION_S:
        co_status = BTA_FS_CO_EACCES;
        break;

    case FS_DIR_NOT_EMPTY_S:
        co_status = BTA_FS_CO_ENOTEMPTY;
        break;

    case FS_EOF_S:
        co_status = BTA_FS_CO_EOF;
        break;

    case FS_ENUM_DONE_S:
        co_status = BTA_FS_CO_EODIR;
        break;

    default:
        co_status = BTA_FS_CO_FAIL;
        break;
    }
    return co_status;
}
#endif
#endif
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
void bta_fs_co_open(const char *p_path, int oflags, UINT32 size, UINT16 evt,  UINT8 app_id)
{
    tBTA_FS_CO_STATUS  status = BTA_FS_CO_OK;
    UINT32  file_size = 0;
    IFile *pif = NULL;
    FileInfo pInfo;

    pifmgr = bt_ui_get_FileMgr();
    if(NULL == pifmgr)
    {
        BCM_MSG_MED("co_open fail",0,0,0);
        bta_fs_ci_open(0, BTA_FS_CO_FAIL, 0, evt);
        return;
    }
    /* if read only */
    if (BTA_FS_O_MODE_MASK(oflags) == BTA_FS_O_RDONLY) 
    {
        pif = IFILEMGR_OpenFile(pifmgr, p_path, _OFM_READ);
        BCM_MSG_MED("co_open read only %x",pif,0,0);
    } 
    else 
    {
        /* try create first */
        if(SUCCESS == IFILEMGR_Test(pifmgr, p_path) )
        {
#if 0
            char strNewName[MAX_FILE_NAME] = {'\0'};
            char *pStr = NULL;
            pStr = strrchr(p_path, '.');
            if(pStr)
            {
               memcpy(strNewName, p_path, pStr - p_path);
               strcat(strNewName,"_1");
               strcat(strNewName,pStr);
               pif = IFILEMGR_OpenFile(pifmgr, strNewName, _OFM_CREATE);
            }
            else
            {
               pif = IFILEMGR_OpenFile(pifmgr, p_path, _OFM_CREATE/*_OFM_READWRITE*/); 
            }
#else
            int err;
            err = IFILEMGR_Remove(pifmgr,p_path);
            if(SUCCESS != err)
            {
                pif = IFILEMGR_OpenFile(pifmgr, p_path, _OFM_READWRITE);
                BCM_MSG_MED("co_open readwrite %x",pif,0,0);
            }
            else
            {
                pif = IFILEMGR_OpenFile(pifmgr, p_path, _OFM_CREATE);
                BCM_MSG_MED("co_open create %x",pif,0,0);
            }
#endif
            //BCM_MSG_MED("co_open readwrite %x",pif,0,0);
        }
        else
        {
            pif = IFILEMGR_OpenFile(pifmgr, p_path, _OFM_CREATE);
            BCM_MSG_MED("co_open create %x",pif,0,0);
        }
    }

    if( pif == NULL )
    {
        BCM_MSG_MED("[CO] bta_fs_co_open: BTA_FS_CO_FAIL",0,0,0);
        status = BTA_FS_CO_FAIL;
    }
    else
    {
        if (IFILEMGR_GetInfo(pifmgr,p_path, &pInfo) == SUCCESS)
        {
            file_size = pInfo.dwSize;
        }

        if (strncmp(p_path, AEEFS_CARD0_DIR,10) == 0)
        {
            BCM_MSG_MED("bluetooth_used_sd",0,0,0);
            Appscommon_bluetooth_used_sd(1);
        }
    }

    BCM_MSG_MED("[CO] bta_fs_co_open: handle:%x size:%x app id:%x",pif, file_size,app_id);
    //APPL_TRACE_DEBUG1("file= %s", p_path);

    bta_fs_ci_open((int)pif, status, file_size, evt);
}

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
tBTA_FS_CO_STATUS bta_fs_co_close(int fd, UINT8 app_id)
{
    BCM_MSG_MED("[CO] bta_fs_co_close: handle:%x, app id:%d", fd, app_id,0);
    IFILE_Release((IFile *)fd);
    fd = 0;
    if(1 == Appscommon_bluetooth_used_sd(-1))
    {
#ifdef FEATURE_SUPPORT_VC0848
        vc_union_type vc_data;
        vc_data.dev_run.curr_dev = VC_DEV_SD;
        VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data);
#endif
        Appscommon_bluetooth_used_sd(0);
    }
    return BTA_FS_CO_OK;
}

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
void bta_fs_co_read(int fd, UINT8 *p_buf, UINT16 nbytes, UINT16 evt, UINT8 app_id)
{
    tBTA_FS_CO_STATUS  status = BTA_FS_CO_OK;
    INT32   num_read;
    //BOOLEAN is_eof = FALSE;

    IFile *pif = (IFile *)fd;
    num_read = IFILE_Read(pif, (void *)p_buf, nbytes);
    BCM_MSG_MED("btapp_flash_read()handle:%x size:%x filesize:%x", pif,num_read, nbytes);
    
    //if ((read_rsp.status == FS_OKAY_S)&& (read_rsp.count < (dword) nbytes)) 
    //{
    //    read_rsp.status = FS_EOF_S;
    //    is_eof = TRUE;
    //}
    //num_read = read_rsp.count;
#if 0    
    if(num_read != 0)
    {
        is_eof = TRUE;
    }
#endif

    if(  num_read < nbytes )
    {
       int nRet;
       nRet = IFILEMGR_GetLastError(pifmgr);
       BCM_MSG_MED("IFILEMGR_GetLastError, nRet:0x%04x", nRet, 0, 0);
       if(nRet == SUCCESS)
        {
            BCM_MSG_MED("[CO] bta_fs_co_read: BTA_FS_CO_EOF",0,0,0);
            status = BTA_FS_CO_EOF;
        }
        else
        {
            BCM_MSG_MED("[CO] bta_fs_co_read: BTA_FS_CO_FAIL",0,0,0);
            status = BTA_FS_CO_FAIL;
        }
    }
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
    if(bt_ui_cb.b_stop_op_file)
    {
        status = BTA_FS_CO_FAIL;
    }
#endif
    BCM_MSG_MED("[CO] bta_fs_co_read: handle:%x app_id:%d state %d", fd,  app_id,status);        
    bta_fs_ci_read(fd, (UINT16)num_read, status, evt);
}

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
void bta_fs_co_write(int fd, const UINT8 *p_buf, INT16 nbytes, UINT16 evt,  UINT8 app_id)
{
    tBTA_FS_CO_STATUS  status = BTA_FS_CO_OK;
    UINT32 written_bytes;

    IFile *pif = (IFile *)fd;
    written_bytes = IFILE_Write(pif, (void *)p_buf, nbytes);
    
    BCM_MSG_MED("btapp_flash_write() handle:%x written_bytes:%x nbytes:%x",  fd,written_bytes, nbytes);
    
    if( written_bytes != nbytes )
    {
        BCM_MSG_MED("[CO] bta_fs_co_write: BTA_FS_CO_FAIL",0,0,0);
        status = BTA_FS_CO_FAIL;
#ifdef FEATURE_SUPPORT_VC0848
        {
            vc_union_type vc_data;
            vc_data.dev_run.curr_dev = VC_DEV_SD;
            VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data);
        }
#endif /* FEATURE_SUPPORT_VC0848 */    
    }
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
    if(bt_ui_cb.b_stop_op_file)
    {
        status = BTA_FS_CO_FAIL;
    }
#endif

    BCM_MSG_MED("[CO] bta_fs_co_write: handle:%d num_written:%d state %d", fd, written_bytes,status);

    bta_fs_ci_write(fd, status, evt);
}


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
void bta_fs_co_seek(int fd, INT32 offset, INT16 origin, UINT8 app_id)
{
#if FLASH_AVAILABLE == TRUE 

    FileSeekType seek_org;
    IFile *pif = (IFile *)fd;
    uint32 err = 0;
    switch (origin) {
    case BTA_FS_SEEK_SET:
        seek_org = _SEEK_START;
        break;
    case BTA_FS_SEEK_END:
        seek_org = _SEEK_END;
        break;
    case BTA_FS_SEEK_CUR:
    default:
        seek_org = _SEEK_CURRENT;
        break;
    }
    err = IFILE_Seek(pif, seek_org, offset);
    BCM_MSG_MED("co_seek status:%x fd:%x", err,  fd,0);

#else
    BCM_MSG_MED("[CO] bta_fs_co_seek: NOT IMPLEMENTED!!!",0,0,0);
#endif

}

/*******************************************************************************
**
** Function         bta_fs_co_access
**
** Description      This function is called to check the existence of
**                  a file or directory, and return whether or not it is a
**                  directory or length of the file.
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
tBTA_FS_CO_STATUS bta_fs_co_access(const char *p_path, int mode, BOOLEAN *p_is_dir,
                                   UINT8 app_id)
{
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
    int err = 0;
    pifmgr = bt_ui_get_FileMgr();
    if(NULL == pifmgr)
    {
        BCM_MSG_MED("co_access fail",0,0,0);
        *p_is_dir = FALSE;
        return BTA_FS_CO_FAIL;
    }

    err = IFILEMGR_Test (pifmgr,p_path);
    if(SUCCESS == err)
    {
        FileInfo pInfo;
        if (IFILEMGR_GetInfo(pifmgr,p_path, &pInfo) == SUCCESS)
        {
            if(pInfo.attrib & AEE_FA_DIR)
            {
                *p_is_dir = TRUE;
            }
            else
            {
                *p_is_dir = FALSE;
            }
            BCM_MSG_MED("co_access sucess:mode:%x %d %d",mode,*p_is_dir,pInfo.attrib);
        }
        return BTA_FS_CO_OK;
    }
    else
    {
        BCM_MSG_MED("co_access status:%x mode:%x %d",err,mode,*p_is_dir);
        return BTA_FS_CO_FAIL;
    }
#else
   *p_is_dir = FALSE;
    APPL_TRACE_WARNING0("[CO] bta_fs_co_access: NOT IMPLEMENTED!!!");
#endif
    
    return status;
}

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
tBTA_FS_CO_STATUS bta_fs_co_mkdir(const char *p_path, UINT8 app_id)
{
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
    int err = 0;
    pifmgr = bt_ui_get_FileMgr();
    if(NULL == pifmgr)
    {
        BCM_MSG_MED("co_mkdir fail",0,0,0);
        return BTA_FS_CO_FAIL;
    }

    err = IFILEMGR_MkDir(pifmgr,p_path);

    if(SUCCESS != err)
    {
        status = BTA_FS_CO_FAIL;    
        BCM_MSG_MED("[CO] bta_fs_co_mkdir: error %x",err,0,0);
    }
#else
    APPL_TRACE_WARNING0("[CO] bta_fs_co_mkdir: NOT IMPLEMENTED!!!");
#endif
    return (status);
}


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
tBTA_FS_CO_STATUS bta_fs_co_rmdir(const char *p_path, UINT8 app_id)
{
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
    int err = 0;
    pifmgr = bt_ui_get_FileMgr();
    if(NULL == pifmgr)
    {
        BCM_MSG_MED("co_rmdir fail",0,0,0);
        return BTA_FS_CO_FAIL;
    }

    err = IFILEMGR_RmDir(pifmgr,p_path);

    if(SUCCESS != err)
    {
        status = BTA_FS_CO_FAIL;
        BCM_MSG_MED("bta_fs_co_rmdir fail %d",err,0,0);
    }
#else
    APPL_TRACE_WARNING0("[CO] bta_fs_co_rmdir: NOT IMPLEMENTED!!!");
#endif                
    return (status);
}

/*******************************************************************************
**
** Function         bta_fs_co_unlink
**
** Description      This function is called to remove a file whose name
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
tBTA_FS_CO_STATUS bta_fs_co_unlink(const char *p_path, UINT8 app_id)
{
#if FLASH_AVAILABLE == TRUE
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;
    int err = 0;
    pifmgr = bt_ui_get_FileMgr();
    if(NULL == pifmgr)
    {
        BCM_MSG_MED("co_unlink fail",0,0,0);
        return BTA_FS_CO_FAIL;
    }

    err = IFILEMGR_Remove(pifmgr,p_path);

    if(SUCCESS != err)
    {
        status = BTA_FS_CO_FAIL;
        BCM_MSG_MED("bta_fs_co_unlink fail %d",err,0,0);
    }
    return (status);
#endif
}

/*******************************************************************************
**
** Function         bta_fs_co_getdirentry
**
** Description      This function is called to get a directory entry for the
**                  specified p_path.  The first/next directory should be filled
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
void bta_fs_co_getdirentry(const char *p_path, BOOLEAN first_item,
                           tBTA_FS_DIRENTRY *p_entry, UINT16 evt, UINT8 app_id)
{
    tBTA_FS_CO_STATUS    co_status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
    int err = 0;
    FileInfo File_Info;
    //FileInfo File_Info_ex;
    BCM_MSG_MED("getdirentry first:%d %d", first_item,enum_type,0);
    pifmgr = bt_ui_get_FileMgr();
    if(NULL == pifmgr)
    {
        BCM_MSG_MED("co_getdirentry fail",0,0,0);
        bta_fs_ci_direntry(BTA_FS_CO_FAIL, evt);
        return ;
    }
    /* if first_item call fs_enum_init, start enum with directories */
    if (first_item)
    {
        err = IFILEMGR_EnumInit(pifmgr,p_path,TRUE);
        enum_type = BT_ENUM_DIRECTORIES;
        BCM_MSG_MED("getdirentry first:%d %d", first_item,err,0);
        if(SUCCESS == err)
        {
            /* return fake dir entry for '.' to work around empty directory problem */
            p_entry->filesize = 0;
            p_entry->crtime[0] = '\0';
            strcpy(p_entry->p_name, ".");
            p_entry->mode = BTA_FS_A_DIR;
        }
        else
        {
            co_status = BTA_FS_CO_FAIL;
        }
        bta_fs_ci_direntry(co_status, evt);
        return;
    }

    if(enum_type == BT_ENUM_FILES_INIT)
    {
        enum_type = BT_ENUM_FILES;
        if(SUCCESS != IFILEMGR_EnumInit(pifmgr,p_path,FALSE))
        {
            co_status = BTA_FS_CO_FAIL;
            BCM_MSG_MED("Enum file init fail",0,0,0);
            bta_fs_ci_direntry(co_status, evt);
            return;
        }
    }

    if(IFILEMGR_EnumNext(pifmgr,&File_Info))
    {
        char * name = NULL;
        char *pp = NULL;
        //IFILEMGR_GetInfo(pifmgr, File_Info.szName, &File_Info_ex);
        p_entry->filesize = File_Info.dwSize;
        p_entry->crtime[0] = '\0';
        name = File_Info.szName;
        pp = strchr(name,'/');
        while(NULL != pp)
        {
            name = pp+1;
            pp = strchr(name,'/');
        }
        BCM_MSG_MED("direntry File_Info %x %x %x", File_Info.attrib,name[0],name[1]);
        if(NULL != name)
        {
            strcpy(p_entry->p_name, name);
        }
        p_entry->mode = 0;
        if(File_Info.attrib & AEE_FA_READONLY)
        {
            //BCM_MSG_MED("READONLY",0,0,0);
            p_entry->mode |= BTA_FS_A_RDONLY;
        }

        if(File_Info.attrib & AEE_FA_DIR)
        {
            //BCM_MSG_MED("DIR1",0,0,0);
            p_entry->mode |= AEE_FA_DIR;
        }

        if(enum_type ==  BT_ENUM_DIRECTORIES)
        {
            //BCM_MSG_MED("DIR2",0,0,0);
            p_entry->mode |= AEE_FA_DIR;
        }
        bta_fs_ci_direntry(BTA_FS_CO_OK, evt);
        return;
    }
    /*if enum file fail*/
    if(enum_type ==  BT_ENUM_DIRECTORIES)
    {
        enum_type = BT_ENUM_FILES_INIT;
        BCM_MSG_MED("Enum direntry finish,enum file",0,0,0);
        bta_fs_co_getdirentry(p_path, FALSE, p_entry, evt, app_id);
        return;
    }
    else
    {
        /*send event to bta,enum file end*/
        BCM_MSG_MED("Enum file finish.send event to bta",0,0,0);
        co_status = BTA_FS_CO_EODIR;
        bta_fs_ci_direntry(co_status, evt);
    }

#else
    APPL_TRACE_WARNING0("[CO] bta_fs_co_getdirentry: NOT IMPLEMENTED!!!");
#endif
}

#else /*FEATURE_SUPPORT_VC0848*/
#if FLASH_AVAILABLE == TRUE
/* platform specific include file for FS */
#include "fs.h"
#include "msg.h"
#endif

typedef UINT8 CHP_FS_FILE_PARA_T;

//#include "bt_fs_api.h"

static const fs_open_create_type fs_open_create = {
	FS_OC_DISCARD,				/* discard on power failure */
	FS_OB_ALLOW,				/* allow buffering */
	FS_FA_UNRESTRICTED,			/* unrestricted access */
	0							/* base address (unused) */
};
/*****************************************************************************
**  Function Declarations
*****************************************************************************/
#if FLASH_AVAILABLE == TRUE
static fs_enum_iterator_type bta_fs_enum;
#endif
/*******************************************************************************
**
** Function         bta_fs_convert_status
**
** Description      This function converts the return status of an MSM FS
**                  function to a BTA FS status value.
**
** Returns          BTA FS status value.
**
*******************************************************************************/
#if FLASH_AVAILABLE == TRUE
static tBTA_FS_CO_STATUS bta_fs_convert_status(fs_status_type status)
{
	tBTA_FS_CO_STATUS co_status;
	
	switch (status) {
	case FS_OKAY_S:
		co_status = BTA_FS_CO_OK;
		break;

	case FS_ILLEGAL_OPERATION_S:
		co_status = BTA_FS_CO_EACCES;
		break;

	case FS_DIR_NOT_EMPTY_S:
		co_status = BTA_FS_CO_ENOTEMPTY;
		break;

	case FS_EOF_S:
		co_status = BTA_FS_CO_EOF;
		break;

	case FS_ENUM_DONE_S:
		co_status = BTA_FS_CO_EODIR;
		break;

	default:
		co_status = BTA_FS_CO_FAIL;
		break;
	}
	return co_status;
}
#endif

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
void bta_fs_co_open(const char *p_path, int oflags, UINT32 size, UINT16 evt,  UINT8 app_id)
{
     fs_open_rsp_type open_rsp;
     fs_rsp_msg_type file_size_rsp_msg;
	  
    tBTA_FS_CO_STATUS  status = BTA_FS_CO_OK;
    UINT32  file_size = 0;
    int fd;

#if 0
	/* if read only */
	if (BTA_FS_O_MODE_MASK(oflags) == BTA_FS_O_RDONLY) {
		APPL_TRACE_DEBUG0("co_open read only");
		btapi_fs_open(p_path, FS_OA_READONLY,
				(fs_open_xparms_type *) & fs_open_create, NULL,
				(fs_rsp_msg_type *) & open_rsp,app_id);
	} else {
		/* try create first */
		APPL_TRACE_DEBUG0("co_open create");
		btapi_fs_open(p_path, FS_OA_CREATE,
				(fs_open_xparms_type *) & fs_open_create, NULL,
				(fs_rsp_msg_type *) & open_rsp,app_id);

		/* if file already exists try read/write */
		if (open_rsp.status == FS_FILE_ALREADY_EXISTS_S) {
			APPL_TRACE_DEBUG0("co_open readwrite");
			btapi_fs_open(p_path, FS_OA_READWRITE,
					(fs_open_xparms_type *) & fs_open_create, NULL,
					(fs_rsp_msg_type *) & open_rsp,app_id);
		}
	}
#else
	/* if read only */
	if (BTA_FS_O_MODE_MASK(oflags) == BTA_FS_O_RDONLY) {
		MSG_ERROR("co_open read only",0,0,0);
		fs_open(p_path, FS_OA_READONLY,
				(fs_open_xparms_type *) & fs_open_create, NULL,
				(fs_rsp_msg_type *) & open_rsp);
	} else {
		/* try create first */
		MSG_ERROR("co_open create",0,0,0);
		fs_open(p_path, FS_OA_CREATE,
				(fs_open_xparms_type *) & fs_open_create, NULL,
				(fs_rsp_msg_type *) & open_rsp);

		/* if file already exists try read/write */
		if (open_rsp.status == FS_FILE_ALREADY_EXISTS_S) {
			MSG_ERROR("co_open readwrite",0,0,0);
			fs_open(p_path, FS_OA_READWRITE,
					(fs_open_xparms_type *) & fs_open_create, NULL,
					(fs_rsp_msg_type *) & open_rsp);
		}
	}
#endif
    if( open_rsp.status != FS_OKAY_S )
        fd = 0;
    else
        fd = open_rsp.handle; 

    if( fd == 0 )
    {
        MSG_ERROR("[CO] bta_fs_co_open: BTA_FS_CO_FAIL",0,0,0);
        status = BTA_FS_CO_FAIL;
    }
#if 0
    btapi_fs_file_size(p_path, 0, &file_size_rsp_msg);
#else
    fs_file_size(p_path, 0, &file_size_rsp_msg);
#endif
    if (file_size_rsp_msg.file_size.status == FS_OKAY_S) 
         file_size = file_size_rsp_msg.file_size.size;



    MSG_ERROR("[CO] bta_fs_co_open: handle:%d app id:%d",fd, app_id,0);
    //APPL_TRACE_DEBUG1("file= %s", p_path);

    bta_fs_ci_open(fd, status, file_size, evt);
}

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
tBTA_FS_CO_STATUS bta_fs_co_close(int fd, UINT8 app_id)
{
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;
    fs_rsp_msg_type close_msg;    
    
    MSG_ERROR("[CO] bta_fs_co_close: handle:%d, app id:%d", fd, app_id,0);

#if 0
    btapi_fs_close(fd,app_id);
#else
    fs_close(fd,NULL, &close_msg);
#endif
    return (status);
}

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
void bta_fs_co_read(int fd, UINT8 *p_buf, UINT16 nbytes, UINT16 evt, UINT8 app_id)
{
    tBTA_FS_CO_STATUS  status = BTA_FS_CO_OK;
    INT32   num_read;
    BOOLEAN is_eof = FALSE;

	fs_read_rsp_type read_rsp;
#if 0
	btapi_fs_read((word) fd, p_buf, (dword) nbytes, NULL, (fs_rsp_msg_type *) & read_rsp);
#else
	fs_read((word) fd, p_buf, (dword) nbytes, NULL, (fs_rsp_msg_type *) & read_rsp);
#endif
	MSG_ERROR("btapp_flash_read() status:%x fd:%x bytes:%d", read_rsp.status,  fd, read_rsp.count);

	if ((read_rsp.status == FS_OKAY_S)
		&& (read_rsp.count < (dword) nbytes)) {
		read_rsp.status = FS_EOF_S;

              is_eof = TRUE;
	}

      num_read = read_rsp.count;
    
    if(  num_read < nbytes )
        {
           if(is_eof)
            {
                MSG_ERROR("[CO] bta_fs_co_read: BTA_FS_CO_EOF",0,0,0);
                status = BTA_FS_CO_EOF;
            }
            else
            {
                MSG_ERROR("[CO] bta_fs_co_read: BTA_FS_CO_FAIL",0,0,0);
                status = BTA_FS_CO_FAIL;
            }
        }

    MSG_ERROR("[CO] bta_fs_co_read: handle:%d app_id:%d", fd,  app_id,0);        

    bta_fs_ci_read(fd, (UINT16)num_read, status, evt);
}

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
void bta_fs_co_write(int fd, const UINT8 *p_buf, INT16 nbytes, UINT16 evt,
                     UINT8 app_id)
{
    tBTA_FS_CO_STATUS  status = BTA_FS_CO_OK;
    UINT32 written_bytes;

    fs_write_rsp_type write_rsp;
#if 0
    btapi_fs_write((word) fd, (void *) p_buf, (dword) nbytes, NULL, (fs_rsp_msg_type *) & write_rsp);
#else
    fs_write((word) fd, (void *) p_buf, (dword) nbytes, NULL, (fs_rsp_msg_type *) & write_rsp);
#endif
    MSG_ERROR("btapp_flash_write()  status:%x fd:%x bytes:%d",  write_rsp.status, fd, write_rsp.count);
	
    written_bytes = write_rsp.count;
    if( written_bytes != nbytes )
        {
            MSG_ERROR("[CO] bta_fs_co_write: BTA_FS_CO_FAIL",0,0,0);
            status = BTA_FS_CO_FAIL;
        }
    MSG_ERROR("[CO] bta_fs_co_write: handle:%d num_written:%d", fd, written_bytes,0);

    bta_fs_ci_write(fd, status, evt);
}


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
void bta_fs_co_seek(int fd, INT32 offset, INT16 origin, UINT8 app_id)
{
#if FLASH_AVAILABLE == TRUE 

	fs_seek_origin_type seek_org;
	fs_rsp_msg_type seek_resp;

	switch (origin) {
	case BTA_FS_SEEK_SET:
		seek_org = FS_SEEK_SET;
		break;
	case BTA_FS_SEEK_END:
		seek_org = FS_SEEK_EOF;
		break;
	case BTA_FS_SEEK_CUR:
	default:
		seek_org = FS_SEEK_CURRENT;
		break;
	}
#if 0
	btapi_fs_seek((word) fd, seek_org, (dword) offset, NULL, &seek_resp);
#else
	fs_seek((word) fd, seek_org, (dword) offset, NULL, &seek_resp);
#endif
	MSG_ERROR("co_seek status:%x fd:%x", seek_resp.seek.status,  fd,0);

#else
    MSG_ERROR("[CO] bta_fs_co_seek: NOT IMPLEMENTED!!!",0,0,0);
#endif

}

/*******************************************************************************
**
** Function         bta_fs_co_access
**
** Description      This function is called to check the existence of
**                  a file or directory, and return whether or not it is a
**                  directory or length of the file.
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
tBTA_FS_CO_STATUS bta_fs_co_access(const char *p_path, int mode, BOOLEAN *p_is_dir,
                                   UINT8 app_id)
{
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
    
    fs_nametest_rsp_type nametest_rsp;

    //APPL_TRACE_DEBUG1("co_access %s", p_path);

    /* try for directory */
#if 0
    btapi_fs_nametest(p_path, FS_TEST_DIR, NULL, (fs_rsp_msg_type *) & nametest_rsp);
#else
    fs_nametest(p_path, FS_TEST_DIR, NULL, (fs_rsp_msg_type *) & nametest_rsp);
#endif
    MSG_ERROR("co_access status:%x found:%d mode:%x",
					  nametest_rsp.status, nametest_rsp.name_found, mode);

    if (nametest_rsp.status == FS_OKAY_S && nametest_rsp.name_found) {
		*p_is_dir = TRUE;
		return BTA_FS_CO_OK;
    }

    /* try for file first */
#if 0
    btapi_fs_nametest(p_path, FS_TEST_FILE, NULL,	(fs_rsp_msg_type *) & nametest_rsp);
#else
    fs_nametest(p_path, FS_TEST_FILE, NULL,	(fs_rsp_msg_type *) & nametest_rsp);
#endif
    MSG_ERROR("co_access status:%x found:%d mode:%x",
					  nametest_rsp.status, nametest_rsp.name_found, mode);

    if (nametest_rsp.status == FS_OKAY_S && nametest_rsp.name_found) {
		*p_is_dir = FALSE;
		return BTA_FS_CO_OK;
    }

    /* nametest failed for both file and dir */
    return BTA_FS_CO_FAIL;
#else
   *p_is_dir = FALSE;
    APPL_TRACE_WARNING0("[CO] bta_fs_co_access: NOT IMPLEMENTED!!!");
#endif
    
    return status;
}

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
tBTA_FS_CO_STATUS bta_fs_co_mkdir(const char *p_path, UINT8 app_id)
{
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
    
    fs_rsp_msg_type mkdir_msg;

    fs_mkdir( p_path, NULL, &mkdir_msg );

    if( mkdir_msg.mkdir.status != FS_OKAY_S && mkdir_msg.mkdir.status != FS_DIR_ALREADY_EXISTS_S )
    {
        status = BTA_FS_CO_FAIL;    
        MSG_ERROR("[CO] bta_fs_co_mkdir: error",0,0,0);
    }
#else
    APPL_TRACE_WARNING0("[CO] bta_fs_co_mkdir: NOT IMPLEMENTED!!!");
#endif
    return (status);
}


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
tBTA_FS_CO_STATUS bta_fs_co_rmdir(const char *p_path, UINT8 app_id)
{
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
        
    fs_rsp_msg_type rmdir_msg;

    fs_rmdir( p_path, NULL, &rmdir_msg );
    switch( rmdir_msg.rmdir.status )
        {
        case FS_NONEXISTENT_DIR_S:
        case FS_OKAY_S: 
            status = BTA_FS_CO_OK;
            break;
        case FS_DIR_NOT_EMPTY_S:
            status = BTA_FS_CO_ENOTEMPTY;
            break;
        default:
                status = BTA_FS_CO_FAIL;
        }
#else
    APPL_TRACE_WARNING0("[CO] bta_fs_co_rmdir: NOT IMPLEMENTED!!!");
#endif                
    return (status);
}

/*******************************************************************************
**
** Function         bta_fs_co_unlink
**
** Description      This function is called to remove a file whose name
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
tBTA_FS_CO_STATUS bta_fs_co_unlink(const char *p_path, UINT8 app_id)
{
#if FLASH_AVAILABLE == TRUE
    tBTA_FS_CO_STATUS status = BTA_FS_CO_OK;

    fs_rsp_msg_type  rmfile_rsp_msg;

    fs_remove(p_path, NULL, &rmfile_rsp_msg);
    if(rmfile_rsp_msg.rmfile.status != FS_OKAY_S )
        MSG_ERROR("btapp_flash_delete FAILED",0,0,0);
    
    return (status);
#endif

}

/*******************************************************************************
**
** Function         bta_fs_co_getdirentry
**
** Description      This function is called to get a directory entry for the
**                  specified p_path.  The first/next directory should be filled
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
void bta_fs_co_getdirentry(const char *p_path, BOOLEAN first_item,
                           tBTA_FS_DIRENTRY *p_entry, UINT16 evt, UINT8 app_id)
{
    tBTA_FS_CO_STATUS    co_status = BTA_FS_CO_OK;

#if FLASH_AVAILABLE == TRUE
    fs_enum_init_rsp_type enum_rsp;
    fs_enum_data_type enum_data;
    fs_enum_type enum_kind;
    fs_status_type status;

    MSG_ERROR("getdirentry first:%d", first_item,0,0);
    //APPL_TRACE_DEBUG1("  path:%s", p_path);

    /* if first_item call fs_enum_init, start enum with directories */
    if (first_item) 
    {
        fs_enum_init(p_path, FS_ENUM_DIRECTORIES, &bta_fs_enum, NULL,
                         (fs_rsp_msg_type *) & enum_rsp);
        MSG_ERROR("fs_enum_init status:%x", enum_rsp.status,0,0);

        if (enum_rsp.status == FS_OKAY_S) 
        {
                /* return fake dir entry for '.' to work around empty directory problem */
                p_entry->filesize = 0;
                p_entry->crtime[0] = '\0';
                strcpy(p_entry->p_name, ".");
                p_entry->mode = BTA_FS_A_DIR;
        }

        bta_fs_ci_direntry(bta_fs_convert_status(enum_rsp.status), evt);
        return;

    }

    /* call fs_enum_next */
    fs_enum_next(&enum_data, &bta_fs_enum, NULL,
                 (fs_rsp_msg_type *) & enum_rsp);
    MSG_ERROR("fs_enum_next status:%x full:%d dir:%d",
                      enum_rsp.status, enum_data.fullname_length,
                      enum_data.dirname_length);

    
    co_status = bta_fs_convert_status(enum_rsp.status);

    if ((status = enum_rsp.status) == FS_OKAY_S) 
    {
        p_entry->filesize = enum_data.logical_size;
        p_entry->crtime[0] = '\0';

        /* Name returned by enum_next includes full path; for files we can use the 
           ** convenient dirname_length and fullname_length to extract file name.
           ** For directories they don't help is out; have to do it ourselves.
         */
        if (bta_fs_enum.enum_kind == FS_ENUM_FILES) 
        {
            memcpy(p_entry->p_name,
                   &enum_data.fullname[enum_data.dirname_length],
                   (enum_data.fullname_length - enum_data.dirname_length));
            p_entry->p_name[enum_data.fullname_length -
                            enum_data.dirname_length] = '\0';
        } 
        else 
        {
            UINT16 dirname_length = strlen(p_path) + 1;
            memcpy(p_entry->p_name, &enum_data.fullname[dirname_length],
                   (enum_data.fullname_length - dirname_length));
            p_entry->p_name[enum_data.fullname_length - dirname_length] =
                '\0';
        }
        p_entry->mode = 0;
        p_entry->mode |=
            (enum_data.attributes == FS_FA_READONLY) ? BTA_FS_A_RDONLY : 0;
        p_entry->mode |=
            (bta_fs_enum.enum_kind ==
             FS_ENUM_DIRECTORIES) ? BTA_FS_A_DIR : 0;
    } 
    else if (enum_rsp.status == FS_ENUM_DONE_S)/*1c*/
    {
        enum_kind = bta_fs_enum.enum_kind;

        fs_enum_finish(&bta_fs_enum, NULL, (fs_rsp_msg_type *) & enum_rsp);

        MSG_ERROR("1. fs_enum_finish status:%x kind:%d",
                          enum_rsp.status, enum_kind,0);

        /* if we just did directories, now it's time to do files */
        if (enum_kind == FS_ENUM_DIRECTORIES) 
        {
            fs_enum_init(p_path, FS_ENUM_FILES, &bta_fs_enum, NULL,
                         (fs_rsp_msg_type *) & enum_rsp);
            MSG_ERROR("fs_enum_init status:%x", enum_rsp.status,0,0);

            /* call getdirentry recursively to kick it off */
            bta_fs_co_getdirentry(p_path, FALSE, p_entry, evt, app_id);
            return;
        }
        
    }
    else if (enum_rsp.status == FS_BAD_FILE_NAME_S) 
    {
        enum_kind = bta_fs_enum.enum_kind;

        MSG_ERROR("1. fs_enum FS_BAD_FILE_NAME_S status:%x kind:%d",
                          enum_rsp.status, enum_kind,0);
              /* Maybe a restricted file or Folder, call getdirentry recursively to kick it off again*/
        bta_fs_co_getdirentry(p_path, FALSE, p_entry, evt, app_id);
        return;
    }
    else 
    {
        /* any other error response; we're done */
        fs_enum_finish(&bta_fs_enum, NULL, (fs_rsp_msg_type *) & enum_rsp);
        MSG_ERROR("2. fs_enum_finish status:%x kind:%d",
                          enum_rsp.status, bta_fs_enum.enum_kind,0);
    }

#else
    APPL_TRACE_WARNING0("[CO] bta_fs_co_getdirentry: NOT IMPLEMENTED!!!");
#endif
    

    bta_fs_ci_direntry(co_status, evt);
}
#endif
