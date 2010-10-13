#ifndef QARE_H
#define QARE_H
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
*  $Header: //source/qcom/qct/multimedia/audio/qare/main/latest/src/qare.h#1 $ $DateTime: 2008/12/17 09:45:31 $ $Author: markande $
*
*when       who     what, where, why
*--------   ---     ----------------------------------------------------------
12/16/08    mar     Initial file version.

******************************************************************************
**/

typedef unsigned int qare_uint;
typedef unsigned short qare_ushort;
typedef unsigned char qare_uchar;

typedef int qare_page_owner_type;
typedef int qare_location_type;
typedef int qare_frequency_type;
typedef unsigned short qare_chksum_type;
typedef qare_uint qare_page_size_type;


#define QARE_MAXIMUM_NUM_CACHE  4
#define QARE_PAGE_OWNER_NONE (qare_page_owner_type)-1
#define QARE_LOC_NONE (qare_location_type)-1

/**
each emumerated value in this enum represents the file type supoorted by qare.
new support can be added by addind the extra enum here, and*
passing the right*sizes while qare_init call**
**/
typedef enum 
{
  QARE_FILEID_ECNS_HANDSET =0,
  QARE_FILEID_ECNS_HEADSET,
  QARE_FILEID_ECNS_HANDSFREE,
  QARE_FILEID_ECNS_SPEAKER,
  QARE_FILEID_ECNS_BT,
  QARE_FILEID_FLUENCE_HANDSET,
  QARE_FILEID_FLUENCE_SPEAKER,
  QARE_FILEID_MAX,
  QARE_FILEID_INVALID
}  qare_file_id_type;

/**
cache page availibility cab be present or absent
**/
typedef enum 
{
  QARE_FILE_ABSENT,
  QARE_FILE_PRESENT
}  qare_file_validity_type;

/**
page availibility information
**/
typedef struct qare_available_file_struct
{
  qare_location_type page_available;
  qare_uint page_size;
}  qare_available_file_type;

/**
page information, allocated address and owner of the page
**/
typedef struct qare_page_info_struct
{
  qare_frequency_type frq;
  qare_page_owner_type page_owner;
  void* data_buf;
}  qare_page_info_type;

/**
queue control information, perpage info, memory adderess, owner and usedpage count
**/
typedef struct qare_cache_queue_ctrl_struct
{
  qare_page_size_type used_page_count;
  qare_page_info_type pages[QARE_MAXIMUM_NUM_CACHE];
}  qare_cache_queue_ctrl_type;

/*
bool type used in the qare module
*/
typedef enum
{
  QARE_BOOL_NO=0,
  QARE_BOOL_YES=1
}  qare_ctrl_boolean_type;

/*
every state of qare is maintained by this control info.
*/
typedef struct qare_state_ctrl_struct
{
  qare_uint ctrl_init;
  qare_available_file_type page_available_inf[QARE_FILEID_MAX];
  qare_page_size_type page_start[QARE_FILEID_MAX];
  qare_cache_queue_ctrl_type cache_queue;
}  qare_state_ctrl_type;


/*******************************
  EXPOSED DATA STRUCTURES
********************************/


/*
Error return values
*/
typedef enum
{  
  QARE_ERR_SUCCESS=0,
  QARE_ERR_FAIL,            /*failed*/
  QARE_ERR_CTL_UNINIT,      /*CTL info is not initialized*/
  QARE_ERR_INVALID_FILEID,  /*file_id is invalid*/
  QARE_ERR_NULL_POINTER,    /*pointer refering to the NULL value*/
  QARE_ERR_CURRUPTEDCTL,    /*CTL info is corrupted*/
  QARE_ERR_PAGE_NOT_FREE,   /*page is not free, this error value is for internal purpose*/
  QARE_ERR_OUT_OF_MEMORY,   /*can not allocate memory*/
  QARE_ERR_FILEOPEN_FAIL,   /*can not open the files*/
  QARE_ERR_OUT_FILEWRITE_FAIL, /*writing to file failed*/
  QARE_ERR_CHKSUM_FAIL,     /*chksum is not verified*/
  QARE_ERR_FILE_NONEXIST    /*file does not exists*/
}  qare_error_code_type;


/**
* FUNCTION : qare_init
*
* DESCRIPTION : This is exposed fuction and should be called, so that system
*               will initialize itself p_qare_ctl[IN OUT]: ctl
*               info will be maintained by caller, and will be
*               passed for initializationsame value should be
*               passes in subsequent calls of read and write on
*               QARE p_qare_file_sizes[IN]:  pass an array of
*               size QARE_FILEID_MAX, containing sizes for each
*               of the file_id as in qare_file_id_type enum
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
  qare_state_ctrl_type* p_qare_ctrl,
  qare_uint* p_qare_file_sizes
);

/**
* FUNCTION : qare_write_data
*
* DESCRIPTION : function finds out the page which is least frequently used
*            p_qare_ctl [IN]:this is the control info maintained
*                               by caller, same value which was
*                               returend at the time of
*                               qare_init call file_id [IN]:type
*                               of file you want to
*                               retrieve,should be one of the
*                               value from qare_file_id_type
*                               enum membuf [IN]:memory buffer
*                               from where value will be read.
*                               buffer should be pointing to a
*                               memory of size equal to the size
*                               specified the time of qare_init
*                               for this file_id
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
  qare_state_ctrl_type* p_qare_ctrl,
  qare_file_id_type file_id,
  char* mem_buf
);


/**
* FUNCTION : qare_read_data
*
* DESCRIPTION : Exposed function which will should be called to
*            read the datafrom repository p_qare_ctl [IN]:thisis
*            the control info maintained by caller, same value
*            which was returend at the time of qare_init call
*            file_id [IN]:	type of file you want to
*            retrieve,should be one of the value from
*            qare_file_id_type enum membuf [OUT]:memory buffer
*            where value will be read. buffer should be pointing
*            to a memory of size equal to the size specified the
*            time of qare_init for this file_id
*
* DEPENDENCIES :None
*
* RETURN VALUE :returns the error code on fail, QARE_ERR_SUCCESS on success
*
*
*SIDE EFFECTS :None
 */
qare_error_code_type qare_read_data
(
  qare_state_ctrl_type* p_qare_ctrl,
  qare_file_id_type file_id,
  char* mem_buf
);


#endif /*QARE_H*/

