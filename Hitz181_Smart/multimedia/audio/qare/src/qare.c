/** 
***************************************************************************
*
*                                    Qualcomm Audio REpository
*
*DESCRIPTION
* This is a set of APIs can be used to save and retirve files from filesystem.
*
* Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*****************************************************************************
**/
/**
***************************************************************************
*
*                      EDIT HISTORY FOR FILE
*
*  This section contains comments describing changes made to this file.
*  Notice that changes are listed in reverse chronological order.
*   
*  $Header: //source/qcom/qct/multimedia/audio/qare/main/latest/src/qare.c#2 $ $DateTime: 2008/12/29 06:20:34 $ $Author: markande $
*
*when       who     what, where, why
*--------   ---     ----------------------------------------------------------
12/16/08    mar     Initial file version.

******************************************************************************
**/


/*
      --------------------
      |include files      |
      --------------------
*/
#include "qare.h"
#include "qarei.h"
#include <stdlib.h>             /* declarations for malloc and free */
#include <string.h>             /* Interface to string library*/
#include <stdio.h>              /* declarations for sprintf */
#include "AEEstd.h"             /*for std_strlprintf*/

#include "fs.h"


/**
* FUNCTION : qare_get_checksum
*
* DESCRIPTION : returns the 16 bit checksum value, for given buffer address
*               and its length
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the checksum value
*
*
*SIDE EFFECTS :None
 */
static qare_chksum_type qare_get_checksum
(
qare_ushort *buff,
qare_page_size_type len
)
{
    long sum = 0;  /* assume 32 bit long, 16 bit short */

    while(len > 1)
    {
        sum += *(buff++);
        if(sum & 0x80000000)   /* if high order bit set, fold */
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        len -= 2;
    }
    if(len)       /* take care of left over byte */
    {
        sum += (uint16) *((char *)buff);
    }
    while(sum>>16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return(qare_chksum_type)~sum;
}


/**
* FUNCTION : qare_store_fs
*
* DESCRIPTION : stores the file content into the qare_<file_id>.db file on the
*               file system,this function also stores the checksum for 
*               validity check while retrieval
*               
*               FILE FORMAT
*               |<--4 byte(CHECKSUM)-->|
*               |<--1 byte(VALID BYTE)-->|
*               |<--n byte data(DATA)-->| 
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_store_fs
(
qare_state_ctrl_type* p_qare_ctl,
qare_file_id_type file_id,
char* buf
)
{

    char filename[20];
    unsigned int size;
    qare_chksum_type chksum;
    qare_uchar    is_valid=QARE_FILE_PRESENT;

    fs_rsp_msg_type           fs_rsp;
    fs_open_xparms_type       open_parms;
    fs_handle_type            fs_handle;



    memset(filename, 0, sizeof(filename));
    std_strlprintf(filename,sizeof(filename),"qare_%d.qdb",file_id);

    open_parms.create.cleanup_option   = FS_OC_CLOSE;
    open_parms.create.buffering_option = FS_OB_PROHIBIT;
    open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

    fs_open(filename,FS_OA_CREATE,&open_parms,NULL,&fs_rsp);

    if(  (fs_rsp.open.handle == NULL) 
         && (fs_rsp.any.status == FS_FILE_ALREADY_EXISTS_S)
      )
    {
        fs_open(filename,FS_OA_READWRITE,&open_parms,NULL,&fs_rsp);
        if(fs_rsp.open.handle == NULL || fs_rsp.any.status != FS_OKAY_S)
        {
            return QARE_ERR_FILEOPEN_FAIL;
        }
    }
    else if(fs_rsp.open.handle == NULL || fs_rsp.any.status != FS_OKAY_S)
    {
        return QARE_ERR_FILEOPEN_FAIL;
    }
    /*initialize the values*/
    fs_handle=fs_rsp.open.handle;
    size=p_qare_ctl->page_available_inf[file_id].page_size;
    chksum=qare_get_checksum((qare_ushort *)buf,size);
    is_valid=1;

    /*write the cheksum*/
    fs_write(fs_handle,&chksum,sizeof(qare_chksum_type),NULL,&fs_rsp);
    if(fs_rsp.any.status != FS_OKAY_S)
    {
        fs_close(fs_handle, NULL, &fs_rsp);          
        return QARE_ERR_OUT_FILEWRITE_FAIL;
    }

    /*write the valid bit*/
    fs_write(fs_handle,&is_valid,sizeof(qare_uchar),NULL,&fs_rsp);
    if(fs_rsp.any.status != FS_OKAY_S)
    {
        fs_close(fs_handle, NULL, &fs_rsp);          
        return QARE_ERR_OUT_FILEWRITE_FAIL;
    }
    /*write the data*/
    fs_write(fs_handle, buf,size, NULL, &fs_rsp);
    if(fs_rsp.any.status != FS_OKAY_S)
    {
        fs_close(fs_handle, NULL, &fs_rsp);          
        return QARE_ERR_OUT_FILEWRITE_FAIL;
    }
    fs_close(fs_handle, NULL, &fs_rsp);

    return QARE_ERR_SUCCESS ;
}


/**
* FUNCTION : qare_load_fs
*
* DESCRIPTION : Loads the file content from the qare_<file_id>.db file from 
*               the file system
*               this function also does a checksum validity check.
*               FILE FORMAT
*               |<--4 byte(CHECKSUM)-->|
*               |<--1 byte(VALID BYTE)-->|
*               |<--n byte data(DATA)-->|
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_load_fs
(
qare_state_ctrl_type* p_qare_ctl,
qare_file_id_type file_id,
char* buf
)
{
    char filename[20];
    unsigned int size;
    fs_rsp_msg_type           fs_rsp;
    fs_open_xparms_type       open_parms;
    fs_handle_type            fs_handle;
    qare_chksum_type chksum;
    qare_chksum_type chksum_cal;
    qare_uchar    is_valid=QARE_FILE_ABSENT;


    memset(filename, 0, sizeof(filename));
    std_strlprintf(filename,sizeof(filename),"qare_%d.qdb",file_id);



    open_parms.create.cleanup_option   = FS_OC_CLOSE;
    open_parms.create.buffering_option = FS_OB_PROHIBIT;
    open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

    fs_open(filename,FS_OA_READONLY,&open_parms,NULL,&fs_rsp);

    if(fs_rsp.any.status == FS_NONEXISTENT_FILE_S)
    {
        return QARE_ERR_FILE_NONEXIST;
    }


    if(fs_rsp.open.handle == NULL || fs_rsp.any.status != FS_OKAY_S)
    {
        return QARE_ERR_FILEOPEN_FAIL;
    }
    fs_handle=fs_rsp.open.handle;
    size=p_qare_ctl->page_available_inf[file_id].page_size;

    /*read the checksum*/
    fs_read(fs_handle, &chksum,sizeof(qare_chksum_type), NULL, &fs_rsp);
    if(fs_rsp.any.status != FS_OKAY_S)
    {
        fs_close(fs_handle, NULL, &fs_rsp);          
        return QARE_ERR_FAIL;
    }

    /*Read valid Bit*/
    fs_read(fs_handle, &is_valid,sizeof(qare_uchar), NULL, &fs_rsp);
    if(fs_rsp.any.status != FS_OKAY_S)
    {
        fs_close(fs_handle, NULL, &fs_rsp);          
        return QARE_ERR_FAIL;
    }

    /*Read the data*/
    fs_read(fs_handle, buf,size, NULL, &fs_rsp);
    if(fs_rsp.any.status != FS_OKAY_S)
    {
        fs_close(fs_handle, NULL, &fs_rsp);          
        return QARE_ERR_FAIL;
    }
    fs_close(fs_handle, NULL, &fs_rsp);
    /*cheksum chek*/
    chksum_cal=qare_get_checksum((qare_ushort *)buf,size);
    if( (chksum_cal!=chksum) || (is_valid!=QARE_FILE_PRESENT))
    {
        return QARE_ERR_CHKSUM_FAIL;
    }
    return QARE_ERR_SUCCESS ;
}


/**
* FUNCTION : qare_init_start_address
*
* DESCRIPTION : Initializes the start address for each of the file_id.
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_init_start_address
(
    qare_state_ctrl_type* p_qare_ctl,
    qare_uint* p_qare_file_sizes
)
{
    unsigned int p_start=0;
    int i=0;
    for(i=0;i<QARE_FILEID_MAX;++i)
    {
        p_qare_ctl->page_start[i]=p_start;
        p_start+=p_qare_file_sizes[i];
    }
    return QARE_ERR_SUCCESS;
}


/**
* FUNCTION : qare_init_cache_queue_ctl
*
* DESCRIPTION : queue pages are initialled with NULL pointer initially,
*               pageowner is assigned to NONE 
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_init_cache_queue_ctl
(
    qare_state_ctrl_type* p_qare_ctl
)
{
    int i=0;
    p_qare_ctl->cache_queue.used_page_count=0;
    for(i=0;i<QARE_MAXIMUM_NUM_CACHE;++i)
    {

        p_qare_ctl->cache_queue.pages[i].data_buf=NULL;
        p_qare_ctl->cache_queue.pages[i].frq=0;
        p_qare_ctl->cache_queue.pages[i].page_owner=QARE_PAGE_OWNER_NONE;       
    }
    return QARE_ERR_SUCCESS;
}

/**
* FUNCTION : qare_init_available_list
*
* DESCRIPTION : for each file_id availibiliity of cache page is
*               initialized as not available
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_init_available_list
(
    qare_state_ctrl_type* p_qare_ctl ,
    qare_uint* p_qare_file_sizes
)
{
    int i=0;
    for(i=0;i<QARE_FILEID_MAX;++i)
    {

        p_qare_ctl->page_available_inf[i].page_available=QARE_LOC_NONE;
        p_qare_ctl->page_available_inf[i].page_size=p_qare_file_sizes[i];
    }
    return QARE_ERR_SUCCESS;
}

/**
* FUNCTION : qare_init
*
* DESCRIPTION : This is exposed fuction and should be called, so that system 
*               will initialize itself
*               p_qare_ctl[IN OUT]: ctl info will be maintained by caller, and
*                                   will be passed for initialization
*                                   same value should be passes in subsequent
*                                   calls of read and write on QARE
*                                   p_qare_file_sizes[IN]:  pass an array of 
*                                   size QARE_FILEID_MAX, containing sizes 
*                                   for each of the file_id as in
*                                   qare_file_id_type enum
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
qare_error_code_type qare_init
(
    qare_state_ctrl_type* p_qare_ctl ,
    qare_uint* p_qare_file_sizes
)
{

    qare_init_cache_queue_ctl(p_qare_ctl);
    qare_init_start_address(p_qare_ctl,p_qare_file_sizes);
    qare_init_available_list(p_qare_ctl,p_qare_file_sizes);
    p_qare_ctl->ctrl_init=QARE_BOOL_YES;

    return QARE_ERR_SUCCESS;

}

/**
* FUNCTION : qare_free_page
*
* DESCRIPTION : function will free the page, deallocate the memory
*               and modify availibillity list
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_free_page
(
    qare_state_ctrl_type* p_qare_ctl,
    qare_page_info_type* p_cur_page
)
{
    if( (NULL==p_cur_page) || (NULL==p_qare_ctl))
    {
        return QARE_ERR_NULL_POINTER;
    }
    if(QARE_BOOL_NO==p_qare_ctl->ctrl_init)
    {
        return QARE_ERR_CTL_UNINIT;
    }

    if(p_cur_page->page_owner!=QARE_PAGE_OWNER_NONE)
    {
        if((p_cur_page->page_owner>QARE_FILEID_MAX)
           || (p_cur_page->page_owner<0)
           )
        {
            return QARE_ERR_CURRUPTEDCTL;
        }
        /*page availibillity list has to be updated*/
        p_qare_ctl->page_available_inf[p_cur_page->page_owner].page_available=
            QARE_LOC_NONE;

        free(p_cur_page->data_buf);
    }
    /*initialize the rest of the field*/
    p_cur_page->data_buf=NULL;
    p_cur_page->page_owner=QARE_PAGE_OWNER_NONE;
    p_cur_page->frq=0;
    return QARE_ERR_SUCCESS;
}

/**
* FUNCTION : qare_allocate_page
*
* DESCRIPTION : function will allocate the page, allocate the memory 
*               and modify availibillity list
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_allocate_page
(
    qare_state_ctrl_type* p_qare_ctl,
    qare_file_id_type file_id,
    qare_page_info_type* p_cur_page
)
{
    if(NULL==p_qare_ctl)
    {
        return QARE_ERR_NULL_POINTER;
    }
    if(QARE_BOOL_NO==p_qare_ctl->ctrl_init)
    {
        return QARE_ERR_CTL_UNINIT;
    }
    if(file_id>=QARE_FILEID_MAX)
    {
        return QARE_ERR_INVALID_FILEID;
    }
    if(NULL!=p_cur_page->data_buf)
    {
        return QARE_ERR_PAGE_NOT_FREE;
    }
    p_cur_page->data_buf=
        malloc(p_qare_ctl->page_available_inf[file_id].page_size);
    p_cur_page->page_owner=file_id;
    if(NULL==p_cur_page->data_buf)
    {
        return QARE_ERR_OUT_OF_MEMORY;
    }
    if(p_qare_ctl->cache_queue.used_page_count<QARE_MAXIMUM_NUM_CACHE)
    {
        (p_qare_ctl->cache_queue.used_page_count)++;
    }
    return QARE_ERR_SUCCESS;
}

/**
* FUNCTION : qare_get_lfu_point
*
* DESCRIPTION : function finds out the page which is least frequently used
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the lfu index
*
*
*SIDE EFFECTS :None
 */
static qare_location_type qare_get_lfu_point
(
    qare_state_ctrl_type* p_qare_ctl
)
{
    int i;
    if(p_qare_ctl->cache_queue.used_page_count<QARE_MAXIMUM_NUM_CACHE)
    {
        /*We have free page, no need to replace anything*/
        return p_qare_ctl->cache_queue.used_page_count;
    }
    else
    {
        qare_location_type lfu_index=0;
        qare_frequency_type min_frq=
            p_qare_ctl->cache_queue.pages[lfu_index].frq;

        /*search(linearly) n find the LFU point.
        this is the slowest part of the alorithm, 
        I hate this, was having no other option*/
        for(i=0;i<QARE_MAXIMUM_NUM_CACHE;++i)
        {
            if(p_qare_ctl->cache_queue.pages[i].frq<min_frq)
            {
                lfu_index=i;
                min_frq=p_qare_ctl->cache_queue.pages[lfu_index].frq;
            }
        }
        return lfu_index;
    }
}

/**
* FUNCTION : qare_update_freq
*
* DESCRIPTION : function updates the frequency counter on page access.
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns void
*
*
*SIDE EFFECTS :None
 */
static void qare_update_freq
(
    qare_state_ctrl_type* p_qare_ctl,
    qare_file_id_type file_id
)
{
    qare_location_type page_loc;
    if(file_id>=QARE_FILEID_MAX)
    {
        return;
    }
    page_loc=p_qare_ctl->page_available_inf[file_id].page_available;
    ++(p_qare_ctl->cache_queue.pages[page_loc].frq);
}

/**
* FUNCTION : qare_write_data
*
* DESCRIPTION : function finds out the page which is least frequently used
*               p_qare_ctl [IN]:this is the control info maintained by caller,
*               same value which was returend at the time of qare_init call
*               file_id    [IN]:type of file you want to retrieve,
*                               should be one of the
*               value from qare_file_id_type enum
*               membuf [IN]: memory buffer from where value will be read. 
*                            buffer should be pointing to a memory of 
*                            size equal to the size 
*               specified the time of qare_init for this file_id
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
qare_error_code_type qare_write_data
(
    qare_state_ctrl_type* p_qare_ctl,
    qare_file_id_type file_id,char* mem_buf
)
{
    qare_location_type free_point;
    qare_location_type avail_point;
    qare_page_info_type* p_cur_page;
    char* storage_buf;
    qare_error_code_type error_code=QARE_ERR_SUCCESS;

    if(NULL==mem_buf || NULL==p_qare_ctl)
    {
        return QARE_ERR_NULL_POINTER;
    }
    if(QARE_BOOL_NO==p_qare_ctl->ctrl_init)
    {
        return QARE_ERR_CTL_UNINIT;
    }
    if(file_id>=QARE_FILEID_MAX)
    {
        return QARE_ERR_INVALID_FILEID;
    }

    /*Allocate the page if not avaialable*/
    if(p_qare_ctl->page_available_inf[file_id].page_available ==QARE_LOC_NONE)
    {
        free_point=qare_get_lfu_point(p_qare_ctl);
        p_cur_page=&(p_qare_ctl->cache_queue.pages[free_point]);
        if(
           (error_code=qare_free_page(p_qare_ctl,p_cur_page))
           !=QARE_ERR_SUCCESS
           )
        {
            return error_code;
        }
        if((error_code=qare_allocate_page(p_qare_ctl,file_id,p_cur_page))
           !=QARE_ERR_SUCCESS
           )
        {
            return error_code;
        }
        p_qare_ctl->page_available_inf[file_id].page_available=free_point;

    }
    qare_update_freq(p_qare_ctl,file_id);
    /*Get the page*/
    avail_point=p_qare_ctl->page_available_inf[file_id].page_available;
    p_cur_page=&(p_qare_ctl->cache_queue.pages[avail_point]);
    storage_buf=p_cur_page->data_buf;
    memcpy(storage_buf,mem_buf,
           p_qare_ctl->page_available_inf[file_id].page_size);
    if(
       (error_code=qare_store_fs(p_qare_ctl,file_id,storage_buf))
       !=QARE_ERR_SUCCESS
       )
    {
        return error_code;
    }
    return QARE_ERR_SUCCESS;
}

/**
* FUNCTION : qare_read_data
*
* DESCRIPTION : Exposed function which will should be called to read the data 
*               from repository
*               p_qare_ctl [IN]:this is the control info maintained by caller, 
*                               same value which was returend at the 
*                               time of qare_init call
*               file_id[IN]:    type of file you want to retrieve,
*                               should be one of the 
*                               value from qare_file_id_type enum
*               membuf [OUT]:   memory buffer where value will be read. 
*                               buffer should be pointing 
*                               to a memory of size equal to the size 
*                               specified at the time of qare_init 
*                               for this file_id
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
* SIDE EFFECTS :None
 */
qare_error_code_type qare_read_data
(
    qare_state_ctrl_type* p_qare_ctl,
    qare_file_id_type file_id,
    char* mem_buf
)
{
    qare_location_type free_point;
    qare_location_type avail_point;
    qare_page_info_type* p_cur_page;
    char* storage_buf;
    qare_error_code_type error_code=QARE_ERR_SUCCESS;

    if(NULL==mem_buf || NULL==p_qare_ctl)
    {
        return QARE_ERR_NULL_POINTER;
    }
    if(QARE_BOOL_NO==p_qare_ctl->ctrl_init)
    {
        return QARE_ERR_CTL_UNINIT;
    }
    if(file_id>=QARE_FILEID_MAX)
    {
        return QARE_ERR_INVALID_FILEID;
    }

    /*Allocate the page if not avaialable*/
    if(p_qare_ctl->page_available_inf[file_id].page_available==QARE_LOC_NONE)
    {
        free_point=qare_get_lfu_point(p_qare_ctl);
        p_cur_page=&(p_qare_ctl->cache_queue.pages[free_point]);
        if(
           (error_code=qare_free_page(p_qare_ctl,p_cur_page))
           !=QARE_ERR_SUCCESS
           )
        {
            return error_code;
        }
        if((error_code=qare_allocate_page(p_qare_ctl,file_id,p_cur_page))
           !=QARE_ERR_SUCCESS
           )
        {
            return error_code;
        }
        p_qare_ctl->page_available_inf[file_id].page_available=free_point;


        /*read the data from file buffer*/
        avail_point=p_qare_ctl->page_available_inf[file_id].page_available;
        p_cur_page=&(p_qare_ctl->cache_queue.pages[avail_point]);
        storage_buf=p_cur_page->data_buf;

        /*read the content from the filesystem*/
        if(
           (error_code=qare_load_fs(p_qare_ctl,file_id,storage_buf))
           !=QARE_ERR_SUCCESS
           )
        {
            return error_code;
        }
    }
    qare_update_freq(p_qare_ctl,file_id);
    /*Get the page*/
    avail_point=p_qare_ctl->page_available_inf[file_id].page_available;
    p_cur_page=&(p_qare_ctl->cache_queue.pages[avail_point]);
    storage_buf=p_cur_page->data_buf;
    memcpy(mem_buf,
           storage_buf,
           p_qare_ctl->page_available_inf[file_id].page_size
           );

    return QARE_ERR_SUCCESS;
}


/************ END OF FILE *************/

