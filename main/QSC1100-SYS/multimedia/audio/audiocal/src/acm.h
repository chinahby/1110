#ifndef ACM_H
#define ACM_H

/** 
\file ***************************************************************************
*
*                                   A C M    H E A D E R    F I L E
*
*DESCRIPTION
* This header file contains all the definitions that are required to other 
* modules to interface with audio calibration manager
*
*Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*******************************************************************************
*/
/* <EJECT> */
/**
\file ***************************************************************************
*
*                      EDIT HISTORY FOR FILE
*
*  This section contains comments describing changes made to this file.
*  Notice that changes are listed in reverse chronological order.
*   
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audiocal/QSC1100/main/latest/src/acm.h#2 $ $DateTime: 2009/05/29 04:18:27 $ $Author: markande $
*
*when         who     what, where, why<br>
*--------   ---     ----------------------------------------------------------<br>
*01/25/08   mas    created  file<br>
*
*******************************************************************************
*/

/*
      --------------------
      |include files                |
      --------------------
 */
#include "comdef.h"
#include "audtp.h"
#include "sndcal.h"
#include "voccal.h"
#include "voccodec.h"
#include "qdspcmd.h"
#include "Sfs_api.h"

/*
      ----------------------------
      | Macros , type defs  and enums |
      ----------------------------
 */
#define ACM_GUID_SIZE 16

#define ACM_SFS_CREAT 0100
#define ACM_SFS_RDONLY 00
#define ACM_SFS_WRONLY 01

/**
*enumeration of ACM command codes
*/
typedef enum acm_cmd_code_enum_tag
{
  CMD_RESULT = 0,/**< Result of a request */
  CMD_SET_STRUCT,/**< Send a CALUNIT to other side*/
  CMD_GET_STRUCT,/**< Request a CALUNIT from other side*/
  CMD_GET_FEATURE_INFO,/**< Request information about Features enabled */
  CMD_SET_FEATURE_INFO,/**< Send information about Features enabled */
  CMD_SET_CALFILE_TO_SFS/**< Set a CALFIL to SFS */
}acm_cmd_code_enum;

/**
*enumeration of ACM supported CALUNITs
*/
typedef enum acm_calunit_enum_tag
{  
  CALUNIT_VOLUME_LEVELS=0,/**< Volume levels*/
  CALUNIT_SLOW_TALK,	/**< SlowTalk*/
  CALUNIT_GAINS, /**< Gains */
  CALUNIT_FIR, /**< FIR Filter*/
  CALUNIT_IIR, /**< IIR Filter*/
  CALUNIT_AUDIO_AGC,/**< Audio AGC*/
  CALUNIT_ECHO_CANCELLER,/**< Echo canceller*/
  CALUNIT_RVE_PARAMS,/**< Echo canceller*/
  CALUNIT_NEXTGEN_EC, /**< Nextgen Echo Canceller*/
  CALUNIT_QSYNTH_ADRC,/**< QSynth ADRC */
  CALUNIT_QTUNES_ADRC,/**< QTunes ADRC*/
  CALUNIT_SIDECHAIN_TUNNING_FILTER,/**< Side chain tuning filter*/
  CALUNIT_IIR_PCM_TUNNING_FILTER, /**< Audfmt IIR filter */
  CALUNIT_QSYNTH_AUDFMT_FIR_FILTER, /**< QSynth Audfmt FIR filter*/
  CALUNIT_QTUNES_AUDFMT_FIR_FILTER, /**< QTunes Audfmt FIR filter*/  
  CALUNIT_AVS_CRYSTAL_SPEECH,/**< Crystal speech*/
  CALUNIT_MAX,/**< CALUNIT count*/
  CALUNIT_UNKNOWN=CALUNIT_MAX
}acm_calunit_enum;

/**
*enumeration of ACM error codes
*/
typedef enum acm_error_code_tag
{  
  ACM_ERR_NONE = 0,/**< Success. No error*/
  ACM_ERR_CALFILE_MISMATCH,/**< Invalid build*/
  ACM_ERR_INVALID_CMD_ID,/**< Invalid command Id */
  ACM_ERR_UNKNOWN_CALUNIT,/**< We are not supporting the CALUNIT*/
  ACM_ERR_UNEXPECTED_BUF_SIZE,/**< Invalid buffer size received from PC*/
  ACM_ERR_INVALID_INDEX,
  ACM_ERR_UNKNOWN /**< Unknown error.*/
}acm_error_code;
/**
* structure definition for GUID
*/
typedef PACKED struct acm_guid_tag
{
  uint32 x;
  uint16 s1;
  uint16 s2;
  byte  c[8];
} acm_guid;

/**
* structure definition for command buffer header 
*/
typedef PACKED struct acm_cmd_struct_header_tag
{
  uint16 cmd_id; /**< a value from acm_cmd_code_enum*/
  uint32 cmd_buf_length;/**< buffer length excluding header size*/
}acm_cmd_struct_header;

/**
* structure definition for command buffer 
*/
typedef PACKED struct acm_cmd_struct_tag
{
  acm_cmd_struct_header header;/**< header info*/
  byte *cmd_buffer;/**< pointer to command buffer*/
}acm_cmd_struct;

/**
* structure definition for command result (CMD_RESULT)
* We can use this structure to return error codes while processing a buffer
*/
typedef PACKED struct acm_cmd_result_struct_tag
{
  acm_cmd_struct_header cmd_header;/**< command buffer header*/
  boolean result_flag; /**< result flag indicating sucess/fail.1=Sucess and 
  0=fail */
  uint16 error_code;/**< error code from acm_error_code enum*/
} acm_cmd_result_struct;

/**
* structure definition to hold the indices for a voc filter
*/
typedef  struct voc_filter_indices_struct_tag
{  
  int32 voc_cal_pcm_path_cfg;/**< PCM Path*/
  int32 snd_device;/**< Device type*/
  int32 snd_method;/**< Method type*/
 } voc_filter_indices_struct;

/**
* structure definition to hold the indices for a audio filter
*/
typedef  struct audio_filter_indices_struct_tag
{
  int32 snd_device;/**< Device type*/
  int32 snd_method;/**< Method type*/
 } audio_filter_indices_struct;
/**
* Typedef for a function which sets calunit instance to calibration table for a 
* given combination of indices
* Each filter will have a function of this type to set its calunit.
*/
typedef acm_error_code (*acm_set_calunit_ptr_type)
(
  /** pointer to command buffer which conatains a calunit instance*/
  byte *req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length,
  /**pointer to a calibration data instance for given combination of indices (method, 
  device, pcm path) calibration table*/
  voc_pcm_path_cal_type *pcm_cal_ptr
);
/**
* Typedef for a function which gets calunit instance from calibration table for a 
* given combination of indices.
* Each filter will have a function of this type to set its calunit.
*/
typedef acm_error_code (*acm_get_calunit_ptr_type)
(
  /** pointer to buffer into which calunit instance to get*/
  byte *calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length,
    /**pointer to a calibration data instance for given combination of indices (method, 
  device, pcm path) calibration table*/
  voc_pcm_path_cal_type *pcm_cal_ptr
);
typedef uint32 (*acm_get_calunit_size_ptr_type)(void);

/**
externised instance of ACM session
*/
extern atp_phone_context_struct acm_phone_context;

/**
* FUNCTION : receive_req_buffer
*
* DESCRIPTION :
* Receive a request buffer process and return a response buffer 
* This function is a callback function set in acm_phone_context, which is called 
* by protocol layer, when it completly receives a request buffer
*
* DEPENDENCIES : Buffer must be in ACM understandable format
*
* RETURN VALUE : response buffer in resp_buf_ptr and response length in resp_buf_length
*
* SIDE EFFECTS : Creates memory for response buffer and return in resp_buf_ptr.
* It is upto the caller to deallocate the memory
 */
extern void acm_receive_req_buffer
(
  /**pointer to request buffer*/
  byte *req_buf_ptr,
  /** request buffer length */
  uint32 req_buf_length,
  /**pointer to response buffer*/
  byte **resp_buf_ptr,
  /**length of the response buffer*/
  uint32 *resp_buf_length
);

extern qdsp_cmd_slowtalk_parms_type voc_cal_slowtalk_configs;

#endif /*ACM_H*/
