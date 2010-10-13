/** 
***************************************************************************
*
*                                    Qualcomm Audio REpository
*
*DESCRIPTION
* This is a set of APIs can be used to save and retirve files from filesystem.
*
* Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*******************************************************************************
**/
/**
***************************************************************************
*
*                      EDIT HISTORY FOR FILE
*
*  This section contains comments describing changes made to this file.
*  Notice that changes are listed in reverse chronological order.
*   
*  $Header: //source/qcom/qct/multimedia/audio/qare/main/latest/src/qarei.h#1 $ $DateTime: 2008/12/17 09:45:31 $ $Author: markande $
*
*when       who     what, where, why
*--------   ---     ----------------------------------------------------------
12/16/08    mar     Initial file version.

******************************************************************************
**/

#ifndef QAREI_H
#define QAREI_H


/**
* FUNCTION : qare_get_checksum
*
* DESCRIPTION : returns the 16 bit checksum value, for given bufferaddress and its length
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
);


/**
* FUNCTION : qare_store_fs
*
* DESCRIPTION : stores the file content into the qare_<file_id>.db file on the file system
*				this function also stores the checksum for validity check while retrieval
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
);


/**
* FUNCTION : qare_load_fs
*
* DESCRIPTION : Loads the file content from the qare_<file_id>.db file from the
*               file system.
*               this function also does a checksum validity check.
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
 );



/**
* FUNCTION : qare_init_start_address
*
* DESCRIPTION : Initializes the start address for each of the file_id. 
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_init_start_address(qare_state_ctrl_type* p_qare_ctl,qare_uint* p_qare_file_sizes);


/**
* FUNCTION : qare_init_cache_queue_ctl
*
* DESCRIPTION : queue pages are initialled with NULL pointer initially,pageowner is assigned to NONE 
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_init_cache_queue_ctl(qare_state_ctrl_type* p_qare_ctl);



/**
* FUNCTION : qare_init_available_list
*
* DESCRIPTION : for each file_id availibiliity of cache page is initialized as not available
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_init_available_list(qare_state_ctrl_type* p_qare_ctl ,qare_uint* p_qare_file_sizes);




/**
* FUNCTION : qare_free_page
*
* DESCRIPTION : function will free the page, deallocate the memory and modify availibillity list
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_free_page(qare_state_ctrl_type* p_qare_ctl,qare_page_info_type* p_cur_page);



/**
* FUNCTION : qare_allocate_page
*
* DESCRIPTION : function will allocate the page, allocate the memory and modify availibillity list
*
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
static qare_error_code_type qare_allocate_page(qare_state_ctrl_type* p_qare_ctl,qare_file_id_type file_id,qare_page_info_type* p_cur_page);


/**
* FUNCTION : qare_get_lfu_point
*
* DESCRIPTION : function finds out the page which is least frequently used
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the lfu index
*
*
*SIDE EFFECTS :None
 */
static qare_location_type qare_get_lfu_point(qare_state_ctrl_type* p_qare_ctl);

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
static void qare_update_freq(qare_state_ctrl_type* p_qare_ctl,qare_file_id_type file_id);

#endif /*QARE_H*/

          /************ END OF FILE *************/

