

#ifndef ACMAPI_H
#define ACMAPI_H
/** 
\file ***************************************************************************
*
*                                   A C M A P I    H E A D E R    F I L E
*
*DESCRIPTION: This header file contains all the definitions that are required to
* to interface with audio calibration data tables in voccal.c/sndcal.c.
*
* Going forward, Source Generator tool is going to create this file from 
* audio calibration file (XML file) during build process.
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
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audiocal/QSC1100/main/latest/src/acmapi.h#5 $ $DateTime: 2009/05/29 04:18:27 $ $Author: markande $
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
#include "qdspcmd.h"
#include "acm.h"

/**
* FUNCTION acmapi_get_feature_info
*
* DESCRIPTION : returns a byte array indicating TRUE if feature is enabled and FALSE if 
* feature is disabled. 
*
* This function should be generated automatically from audio cal xml file in future
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_NONE if success
* ACM_ERR_CALFILE_MISMATCH if build guid doesn't matches
*
* SIDE EFFECTS: None
*/
extern acm_error_code acmapi_get_feature_info
(
   byte *req_buf_ptr,
   uint32* req_buf_length,
   byte **resp_buf_ptr,
   uint32 *resp_buf_length
);

/**
* FUNCTION acmapi_get_calunit_from_guid
*
* DESCRIPTION : finds matching Calunit from map for paased guid
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_calunit_enum
* if matching guid found in map enum elelment if found 
* else CALUNIT_UNKNOWN
*
* SIDE EFFECTS: None
*/
extern acm_calunit_enum acmapi_get_calunit_from_guid
(
   acm_guid *guid
);

    
/**
* FUNCTION : acmapi_get_ezHearTargetExpRatioIF_size
*
* DESCRIPTION : calculate ezHearTargetExpRatioIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearTargetExpRatioIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearTargetExpRatioIF 
*
* DESCRIPTION : copies ezHearTargetExpRatioIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearTargetExpRatioIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearTargetExpRatioIF 
*
* DESCRIPTION : copies ezHearTargetExpRatioIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearTargetExpRatioIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearVocoderNBorWBIF_size
*
* DESCRIPTION : calculate ezHearVocoderNBorWBIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearVocoderNBorWBIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearVocoderNBorWBIF 
*
* DESCRIPTION : copies ezHearVocoderNBorWBIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearVocoderNBorWBIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearVocoderNBorWBIF 
*
* DESCRIPTION : copies ezHearVocoderNBorWBIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearVocoderNBorWBIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearMaxLocalExpCompDiffIF_size
*
* DESCRIPTION : calculate ezHearMaxLocalExpCompDiffIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearMaxLocalExpCompDiffIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearMaxLocalExpCompDiffIF 
*
* DESCRIPTION : copies ezHearMaxLocalExpCompDiffIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearMaxLocalExpCompDiffIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearMaxLocalExpCompDiffIF 
*
* DESCRIPTION : copies ezHearMaxLocalExpCompDiffIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearMaxLocalExpCompDiffIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearMinSilenceDurationIF_size
*
* DESCRIPTION : calculate ezHearMinSilenceDurationIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearMinSilenceDurationIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearMinSilenceDurationIF 
*
* DESCRIPTION : copies ezHearMinSilenceDurationIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearMinSilenceDurationIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearMinSilenceDurationIF 
*
* DESCRIPTION : copies ezHearMinSilenceDurationIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearMinSilenceDurationIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearProcessDelayIF_size
*
* DESCRIPTION : calculate ezHearProcessDelayIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearProcessDelayIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearProcessDelayIF 
*
* DESCRIPTION : copies ezHearProcessDelayIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearProcessDelayIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearProcessDelayIF 
*
* DESCRIPTION : copies ezHearProcessDelayIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearProcessDelayIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearMaxGenSamplesExpansionIF_size
*
* DESCRIPTION : calculate ezHearMaxGenSamplesExpansionIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearMaxGenSamplesExpansionIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearMaxGenSamplesExpansionIF 
*
* DESCRIPTION : copies ezHearMaxGenSamplesExpansionIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearMaxGenSamplesExpansionIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearMaxGenSamplesExpansionIF 
*
* DESCRIPTION : copies ezHearMaxGenSamplesExpansionIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearMaxGenSamplesExpansionIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadThreshIF_size
*
* DESCRIPTION : calculate ezHearAvadThreshIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadThreshIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadThreshIF 
*
* DESCRIPTION : copies ezHearAvadThreshIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadThreshIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadThreshIF 
*
* DESCRIPTION : copies ezHearAvadThreshIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadThreshIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadThresh2IF_size
*
* DESCRIPTION : calculate ezHearAvadThresh2IF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadThresh2IF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadThresh2IF 
*
* DESCRIPTION : copies ezHearAvadThresh2IF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadThresh2IF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadThresh2IF 
*
* DESCRIPTION : copies ezHearAvadThresh2IF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadThresh2IF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadPwrScaleIF_size
*
* DESCRIPTION : calculate ezHearAvadPwrScaleIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadPwrScaleIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadPwrScaleIF 
*
* DESCRIPTION : copies ezHearAvadPwrScaleIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadPwrScaleIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadPwrScaleIF 
*
* DESCRIPTION : copies ezHearAvadPwrScaleIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadPwrScaleIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadHangoverMaxIF_size
*
* DESCRIPTION : calculate ezHearAvadHangoverMaxIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadHangoverMaxIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadHangoverMaxIF 
*
* DESCRIPTION : copies ezHearAvadHangoverMaxIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadHangoverMaxIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadHangoverMaxIF 
*
* DESCRIPTION : copies ezHearAvadHangoverMaxIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadHangoverMaxIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadAlphaSNRIF_size
*
* DESCRIPTION : calculate ezHearAvadAlphaSNRIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadAlphaSNRIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadAlphaSNRIF 
*
* DESCRIPTION : copies ezHearAvadAlphaSNRIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadAlphaSNRIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadAlphaSNRIF 
*
* DESCRIPTION : copies ezHearAvadAlphaSNRIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadAlphaSNRIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadAvarScaleIF_size
*
* DESCRIPTION : calculate ezHearAvadAvarScaleIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadAvarScaleIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadAvarScaleIF 
*
* DESCRIPTION : copies ezHearAvadAvarScaleIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadAvarScaleIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadAvarScaleIF 
*
* DESCRIPTION : copies ezHearAvadAvarScaleIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadAvarScaleIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadSnrDiffMaxIF_size
*
* DESCRIPTION : calculate ezHearAvadSnrDiffMaxIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadSnrDiffMaxIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadSnrDiffMaxIF 
*
* DESCRIPTION : copies ezHearAvadSnrDiffMaxIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadSnrDiffMaxIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadSnrDiffMaxIF 
*
* DESCRIPTION : copies ezHearAvadSnrDiffMaxIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadSnrDiffMaxIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadSnrDiffMinIF_size
*
* DESCRIPTION : calculate ezHearAvadSnrDiffMinIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadSnrDiffMinIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadSnrDiffMinIF 
*
* DESCRIPTION : copies ezHearAvadSnrDiffMinIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadSnrDiffMinIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadSnrDiffMinIF 
*
* DESCRIPTION : copies ezHearAvadSnrDiffMinIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadSnrDiffMinIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadInitLengthIF_size
*
* DESCRIPTION : calculate ezHearAvadInitLengthIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadInitLengthIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadInitLengthIF 
*
* DESCRIPTION : copies ezHearAvadInitLengthIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadInitLengthIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadInitLengthIF 
*
* DESCRIPTION : copies ezHearAvadInitLengthIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadInitLengthIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadMaxValIF_size
*
* DESCRIPTION : calculate ezHearAvadMaxValIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadMaxValIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadMaxValIF 
*
* DESCRIPTION : copies ezHearAvadMaxValIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadMaxValIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadMaxValIF 
*
* DESCRIPTION : copies ezHearAvadMaxValIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadMaxValIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadInitBoundIF_size
*
* DESCRIPTION : calculate ezHearAvadInitBoundIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadInitBoundIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadInitBoundIF 
*
* DESCRIPTION : copies ezHearAvadInitBoundIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadInitBoundIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadInitBoundIF 
*
* DESCRIPTION : copies ezHearAvadInitBoundIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadInitBoundIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearAvadResetBoundIF_size
*
* DESCRIPTION : calculate ezHearAvadResetBoundIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearAvadResetBoundIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearAvadResetBoundIF 
*
* DESCRIPTION : copies ezHearAvadResetBoundIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearAvadResetBoundIF
( 
  uint16 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearAvadResetBoundIF 
*
* DESCRIPTION : copies ezHearAvadResetBoundIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearAvadResetBoundIF
( 
  uint16 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearCirBufSizeIF_size
*
* DESCRIPTION : calculate ezHearCirBufSizeIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearCirBufSizeIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearCirBufSizeIF 
*
* DESCRIPTION : copies ezHearCirBufSizeIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearCirBufSizeIF
( 
  uint32 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearCirBufSizeIF 
*
* DESCRIPTION : copies ezHearCirBufSizeIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearCirBufSizeIF
( 
  uint32 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearMinimuSimilarityIF_size
*
* DESCRIPTION : calculate ezHearMinimuSimilarityIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearMinimuSimilarityIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearMinimuSimilarityIF 
*
* DESCRIPTION : copies ezHearMinimuSimilarityIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearMinimuSimilarityIF
( 
  uint32 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearMinimuSimilarityIF 
*
* DESCRIPTION : copies ezHearMinimuSimilarityIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearMinimuSimilarityIF
( 
  uint32 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearMinPastSimilarityIF_size
*
* DESCRIPTION : calculate ezHearMinPastSimilarityIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearMinPastSimilarityIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearMinPastSimilarityIF 
*
* DESCRIPTION : copies ezHearMinPastSimilarityIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearMinPastSimilarityIF
( 
  uint32 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearMinPastSimilarityIF 
*
* DESCRIPTION : copies ezHearMinPastSimilarityIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearMinPastSimilarityIF
( 
  uint32 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_ezHearMinForceSimilarityIF_size
*
* DESCRIPTION : calculate ezHearMinForceSimilarityIF size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ezHearMinForceSimilarityIF_size(void);

/**
* FUNCTION : acmapi_get_ezHearMinForceSimilarityIF 
*
* DESCRIPTION : copies ezHearMinForceSimilarityIF data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ezHearMinForceSimilarityIF
( 
  uint32 *_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ezHearMinForceSimilarityIF 
*
* DESCRIPTION : copies ezHearMinForceSimilarityIF data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ezHearMinForceSimilarityIF
( 
  uint32 *_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_slow_talk_size 
*
* DESCRIPTION : calculate Slow Talk calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_slow_talk_calunit_size(void);

/**
* FUNCTION : acmapi_get_tx_filter_size
*
* DESCRIPTION : calculate tx_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_tx_filter_size(void);

/**
* FUNCTION : acmapi_get_tx_filter 
*
* DESCRIPTION : copies tx_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_tx_filter
( 
  qdsp_cmd_pcm_filter_type *pcm_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_tx_filter 
*
* DESCRIPTION : copies tx_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_tx_filter
( 
  qdsp_cmd_pcm_filter_type *pcm_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_rx_filter_size
*
* DESCRIPTION : calculate rx_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_rx_filter_size(void);

/**
* FUNCTION : acmapi_get_rx_filter 
*
* DESCRIPTION : copies rx_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_rx_filter
( 
  qdsp_cmd_pcm_filter_type *pcm_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_rx_filter 
*
* DESCRIPTION : copies rx_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_rx_filter
( 
  qdsp_cmd_pcm_filter_type *pcm_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_fir_filter_size 
*
* DESCRIPTION : calculate FIR FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_fir_filter_calunit_size(void);

#ifdef FEATURE_IIR_FILTER

/**
* FUNCTION : acmapi_get_tx_iir_filter_size
*
* DESCRIPTION : calculate tx_iir_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_tx_iir_filter_size(void);

/**
* FUNCTION : acmapi_get_tx_iir_filter 
*
* DESCRIPTION : copies tx_iir_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_tx_iir_filter
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_tx_iir_filter 
*
* DESCRIPTION : copies tx_iir_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_tx_iir_filter
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_rx_iir_filter_size
*
* DESCRIPTION : calculate rx_iir_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_rx_iir_filter_size(void);

/**
* FUNCTION : acmapi_get_rx_iir_filter 
*
* DESCRIPTION : copies rx_iir_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_rx_iir_filter
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_rx_iir_filter 
*
* DESCRIPTION : copies rx_iir_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_rx_iir_filter
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

#ifdef FEATURE_IIR_FILTER_5S

/**
* FUNCTION : acmapi_get_tx_iir_filter5_size
*
* DESCRIPTION : calculate tx_iir_filter5 size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_tx_iir_filter5_size(void);

/**
* FUNCTION : acmapi_get_tx_iir_filter5 
*
* DESCRIPTION : copies tx_iir_filter5 data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_tx_iir_filter5
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_tx_iir_filter5 
*
* DESCRIPTION : copies tx_iir_filter5 data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_tx_iir_filter5
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_rx_iir_filter5_size
*
* DESCRIPTION : calculate rx_iir_filter5 size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_rx_iir_filter5_size(void);

/**
* FUNCTION : acmapi_get_rx_iir_filter5 
*
* DESCRIPTION : copies rx_iir_filter5 data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_rx_iir_filter5
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_rx_iir_filter5 
*
* DESCRIPTION : copies rx_iir_filter5 data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_rx_iir_filter5
( 
  qdsp_cmd_pcm_iir_filter_type *pcm_iir_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);
#endif/* FEATURE_IIR_FILTER_5S */

/**
* FUNCTION : acmapi_get_iir_filter_size 
*
* DESCRIPTION : calculate IIR FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_iir_filter_calunit_size(void);
#endif/* FEATURE_IIR_FILTER */

#ifdef FEATURE_AUDIO_AGC

/**
* FUNCTION : acmapi_get_agc_param_size
*
* DESCRIPTION : calculate agc_param size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_agc_param_size(void);

/**
* FUNCTION : acmapi_get_agc_param 
*
* DESCRIPTION : copies agc_param data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_agc_param
( 
  qdsp_cmd_agc_param_type *agc_param_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_agc_param 
*
* DESCRIPTION : copies agc_param data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_agc_param
( 
  qdsp_cmd_agc_param_type *agc_param_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_agc_filter_size 
*
* DESCRIPTION : calculate AGC FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_agc_filter_calunit_size(void);
#endif/* FEATURE_AUDIO_AGC */

/**
* FUNCTION : acmapi_get_ec_params_size
*
* DESCRIPTION : calculate ec_params size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_ec_params_size(void);

/**
* FUNCTION : acmapi_get_ec_params 
*
* DESCRIPTION : copies ec_params data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_ec_params
( 
  qdsp_cmd_ec_params_type *ec_params_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_ec_params 
*
* DESCRIPTION : copies ec_params data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_ec_params
( 
  qdsp_cmd_ec_params_type *ec_params_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_echo_canceller_size 
*
* DESCRIPTION : calculate ECHO CANCELLER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_echo_canceller_calunit_size(void);

/**
* FUNCTION : acmapi_get_rve_param_size
*
* DESCRIPTION : calculate rve_param size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_rve_param_size(void);

/**
* FUNCTION : acmapi_get_rve_param 
*
* DESCRIPTION : copies rve_param data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_rve_param
( 
  qdsp_cmd_rve_param_type *rve_param_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_rve_param 
*
* DESCRIPTION : copies rve_param data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_rve_param
( 
  qdsp_cmd_rve_param_type *rve_param_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_dual_rve_param_size
*
* DESCRIPTION : calculate dual_rve_param size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_dual_rve_param_size(void);

/**
* FUNCTION : acmapi_get_dual_rve_param 
*
* DESCRIPTION : copies dual_rve_param data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_dual_rve_param
( 
  qdsp_cmd_dual_rve_param_type *dual_rve_param_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_dual_rve_param 
*
* DESCRIPTION : copies dual_rve_param data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_dual_rve_param
( 
  qdsp_cmd_dual_rve_param_type *dual_rve_param_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_rve_parameters_size 
*
* DESCRIPTION : calculate RVE PARAMETERS calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_rve_parameters_calunit_size(void);

#ifdef FEATURE_AVS_NEXTGEN_EC

/**
* FUNCTION : acmapi_get_nextgen_ec_parms_size
*
* DESCRIPTION : calculate nextgen_ec_parms size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_nextgen_ec_parms_size(void);

/**
* FUNCTION : acmapi_get_nextgen_ec_parms 
*
* DESCRIPTION : copies nextgen_ec_parms data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_nextgen_ec_parms
( 
  qdsp_cmd_nextgen_ec_parms_type *nextgen_ec_parms_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_nextgen_ec_parms 
*
* DESCRIPTION : copies nextgen_ec_parms data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_nextgen_ec_parms
( 
  qdsp_cmd_nextgen_ec_parms_type *nextgen_ec_parms_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_nextgen_ec_wb_parms_size
*
* DESCRIPTION : calculate nextgen_ec_wb_parms size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_nextgen_ec_wb_parms_size(void);

/**
* FUNCTION : acmapi_get_nextgen_ec_wb_parms 
*
* DESCRIPTION : copies nextgen_ec_wb_parms data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_nextgen_ec_wb_parms
( 
  qdsp_cmd_nextgen_ec_parms_type *nextgen_ec_parms_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_nextgen_ec_wb_parms 
*
* DESCRIPTION : copies nextgen_ec_wb_parms data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_nextgen_ec_wb_parms
( 
  qdsp_cmd_nextgen_ec_parms_type *nextgen_ec_parms_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_nextgen_ec_size 
*
* DESCRIPTION : calculate NEXTGEN EC calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_nextgen_ec_calunit_size(void);
#endif/* FEATURE_AVS_NEXTGEN_EC */

#ifdef FEATURE_QSYNTH_ADRC

/**
* FUNCTION : acmapi_get_qsynth_adrc_ctl_size
*
* DESCRIPTION : calculate qsynth_adrc_ctl size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_qsynth_adrc_ctl_size(void);

/**
* FUNCTION : acmapi_get_qsynth_adrc_ctl 
*
* DESCRIPTION : copies qsynth_adrc_ctl data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_qsynth_adrc_ctl
( 
  qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_qsynth_adrc_ctl 
*
* DESCRIPTION : copies qsynth_adrc_ctl data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_qsynth_adrc_ctl
( 
  qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_adrc_qsynth_filter_size 
*
* DESCRIPTION : calculate ADRC QSYNTH FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_adrc_qsynth_filter_calunit_size(void);
#endif/* FEATURE_QSYNTH_ADRC */

#ifdef FEATURE_QTUNES_ADRC

/**
* FUNCTION : acmapi_get_qtunes_adrc_ctl_size
*
* DESCRIPTION : calculate qtunes_adrc_ctl size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_qtunes_adrc_ctl_size(void);

/**
* FUNCTION : acmapi_get_qtunes_adrc_ctl 
*
* DESCRIPTION : copies qtunes_adrc_ctl data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_qtunes_adrc_ctl
( 
  qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_qtunes_adrc_ctl 
*
* DESCRIPTION : copies qtunes_adrc_ctl data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_qtunes_adrc_ctl
( 
  qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_adrc_qtunes_filter_size 
*
* DESCRIPTION : calculate ADRC QTUNES FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_adrc_qtunes_filter_calunit_size(void);
#endif/* FEATURE_QTUNES_ADRC */

#ifdef FEATURE_AUDFMT_STF

/**
* FUNCTION : acmapi_get_audfmt_sidechain_filter_size
*
* DESCRIPTION : calculate audfmt_sidechain_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_audfmt_sidechain_filter_size(void);

/**
* FUNCTION : acmapi_get_audfmt_sidechain_filter 
*
* DESCRIPTION : copies audfmt_sidechain_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_audfmt_sidechain_filter
( 
  qdsp_cmd_audfmt_sidechain_filter_type *audfmt_sidechain_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_audfmt_sidechain_filter 
*
* DESCRIPTION : copies audfmt_sidechain_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_audfmt_sidechain_filter
( 
  qdsp_cmd_audfmt_sidechain_filter_type *audfmt_sidechain_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_side_chain_tuning_filter_size 
*
* DESCRIPTION : calculate SIDE CHAIN TUNING FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_side_chain_tuning_filter_calunit_size(void);
#endif/* FEATURE_AUDFMT_STF */

#ifdef FEATURE_AUDFMT_IIR_FILTER

/**
* FUNCTION : acmapi_get_audfmt_iir_filter_size
*
* DESCRIPTION : calculate audfmt_iir_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_audfmt_iir_filter_size(void);

/**
* FUNCTION : acmapi_get_audfmt_iir_filter 
*
* DESCRIPTION : copies audfmt_iir_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_audfmt_iir_filter
( 
  qdsp_cmd_audfmt_iir_filter_type *audfmt_iir_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_audfmt_iir_filter 
*
* DESCRIPTION : copies audfmt_iir_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_audfmt_iir_filter
( 
  qdsp_cmd_audfmt_iir_filter_type *audfmt_iir_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_iir_pcm_tuning_filter_size 
*
* DESCRIPTION : calculate IIR PCM TUNING FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_iir_pcm_tuning_filter_calunit_size(void);
#endif/* FEATURE_AUDFMT_IIR_FILTER */

#ifdef FEATURE_AUDFMT_FIR_FILTER

#ifdef FEATURE_QSYNTH_COMMON

/**
* FUNCTION : acmapi_get_qsynth_filter_size
*
* DESCRIPTION : calculate qsynth_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_qsynth_filter_size(void);

/**
* FUNCTION : acmapi_get_qsynth_filter 
*
* DESCRIPTION : copies qsynth_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_qsynth_filter
( 
  qdsp_cmd_audfmt_pcm_filter_type *audfmt_pcm_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_qsynth_filter 
*
* DESCRIPTION : copies qsynth_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_qsynth_filter
( 
  qdsp_cmd_audfmt_pcm_filter_type *audfmt_pcm_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_fir_pcm_qsynth_filter_size 
*
* DESCRIPTION : calculate FIR PCM QSYNTH FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_fir_pcm_qsynth_filter_calunit_size(void);
#endif/* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON

/**
* FUNCTION : acmapi_get_qtunes_filter_size
*
* DESCRIPTION : calculate qtunes_filter size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_qtunes_filter_size(void);

/**
* FUNCTION : acmapi_get_qtunes_filter 
*
* DESCRIPTION : copies qtunes_filter data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_qtunes_filter
( 
  qdsp_cmd_audfmt_pcm_filter_type *audfmt_pcm_filter_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_qtunes_filter 
*
* DESCRIPTION : copies qtunes_filter data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_qtunes_filter
( 
  qdsp_cmd_audfmt_pcm_filter_type *audfmt_pcm_filter_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_fir_pcm_qtunes_filter_size 
*
* DESCRIPTION : calculate FIR PCM QTUNES FILTER calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_fir_pcm_qtunes_filter_calunit_size(void);
#endif/* FEATURE_QTUNES_COMMON */
#endif/* FEATURE_AUDFMT_FIR_FILTER */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH

/**
* FUNCTION : acmapi_get_cs_parms_size
*
* DESCRIPTION : calculate cs_parms size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_cs_parms_size(void);

/**
* FUNCTION : acmapi_get_cs_parms 
*
* DESCRIPTION : copies cs_parms data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_cs_parms
( 
  qdsp_cmd_crystal_speech_parms_type *crystal_speech_parms_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_cs_parms 
*
* DESCRIPTION : copies cs_parms data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_cs_parms
( 
  qdsp_cmd_crystal_speech_parms_type *crystal_speech_parms_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_cs_wb_parms_size
*
* DESCRIPTION : calculate cs_wb_parms size.
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the filter
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_cs_wb_parms_size(void);

/**
* FUNCTION : acmapi_get_cs_wb_parms 
*
* DESCRIPTION : copies cs_wb_parms data from filter structure to buffer 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
acm_error_code  acmapi_get_cs_wb_parms
( 
  qdsp_cmd_crystal_speech_parms_type *crystal_speech_parms_ptr, 
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_cs_wb_parms 
*
* DESCRIPTION : copies cs_wb_parms data from buffer to filter structure 
*
* DEPENDENCIES: None
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes wrote to buffer
*/
acm_error_code  acmapi_set_cs_wb_parms
( 
  qdsp_cmd_crystal_speech_parms_type *crystal_speech_parms_ptr, 
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_crystal_speech_size 
*
* DESCRIPTION : calculate CRYSTAL SPEECH calunit size. 
*
* DEPENDENCIES: None
*
* RETURN VALUE: uint32 size - size of the calunit
*
* SIDE EFFECTS: None
*/
uint32 acmapi_get_crystal_speech_calunit_size(void);
#endif/* FEATURE_AVS_CRYSTAL_SPEECH */


/**
* FUNCTION acmapi_get_volume_levels_calunit_size
*
* DESCRIPTION : calculates volume levels calunit size. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: None
*/
extern acm_error_code acmapi_get_volume_levels_calunit_size
(
  audio_filter_indices_struct *audio_filter_indices_ptr,
  uint32 *volume_level_size
);

/**
* FUNCTION : acmapi_get_volume_levels_data
*
* DESCRIPTION : copies volume levels  data from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read into buffer
*/
extern acm_error_code acmapi_get_volume_levels_data
(
  snd_cal_control_type *audio_config_ptr,
  byte **snd_cal_calunit_resp_buf_ptr
);
/**
* FUNCTION : acmapi_set_volume_levels_data
*
* DESCRIPTION : copies volume levels data from filter structure to buffer
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read from buffer
* decrements the remaining buffer length by number of bytes read from buffer
*/
extern acm_error_code acmapi_set_volume_levels_data
(
  snd_cal_control_type *audio_config_ptr,
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_gains_calunit_size
*
* DESCRIPTION : calculates gains calunit size
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: None
*/
extern uint32  acmapi_get_gains_calunit_size(void);

/**
* FUNCTION : acmapi_get_codec_st_gain
*
* DESCRIPTION : copies CODEC ST gain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read into buffer
*/
extern acm_error_code  acmapi_get_codec_st_gain
(
  uint16 *codec_st_gain_ptr,
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_codec_st_gain
*
* DESCRIPTION : copies CODEC STgain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read
* decrements remaining buffer length by numbr of bytes read
*/
extern acm_error_code  acmapi_set_codec_st_gain
(
uint16 *codec_st_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_codec_rx_gain
*
* DESCRIPTION : copies CODEC RX gain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read into buffer
*/
extern acm_error_code  acmapi_get_codec_rx_gain
(
  uint16 *codec_rx_gain_ptr,
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_codec_rx_gain
*
* DESCRIPTION : copies CODEC RX gain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read
* decrements remaining buffer length by numbr of bytes read
*/
acm_error_code  acmapi_set_codec_rx_gain
(
uint16 *codec_rx_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
);
/**
* FUNCTION : acmapi_get_codec_tx_gain
*
* DESCRIPTION : copies CODEC TX gain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read into buffer
*/
extern acm_error_code  acmapi_get_codec_tx_gain
(
  uint16 *codec_tx_gain_ptr,
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_codec_tx_gain
*
* DESCRIPTION : copies CODEC TX gain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read
* decrements remaining buffer length by numbr of bytes read
*/
extern acm_error_code  acmapi_set_codec_tx_gain
(
uint16 *codec_tx_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_tx_dtmf_gain
*
* DESCRIPTION : copies TX DTMF gain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read into buffer
*/
extern acm_error_code  acmapi_get_tx_dtmf_gain
(
  uint16 *tx_dtmf_gain_ptr,
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_tx_dtmf_gain
*
* DESCRIPTION : copies TX DTMF gain from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read
* decrements remaining buffer length by numbr of bytes read
*/
extern acm_error_code  acmapi_set_tx_dtmf_gain
(
uint16 *tx_dtmf_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
);

/**
* FUNCTION : acmapi_get_tx_gain
*
* DESCRIPTION : copies TX Voice Volume from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read into buffer
*/
extern acm_error_code  acmapi_get_tx_gain
(
  uint16 *tx_gain_ptr,
  byte **buf_ptr
);

/**
* FUNCTION : acmapi_set_tx_gain
*
* DESCRIPTION : copies TX Voice Volume from buffer to structure
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_UNKNOWN
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: Modifies calibration table
* increments the buffer pointer by number of bytes read
* decrements remaining buffer length by numbr of bytes read
*/
extern acm_error_code  acmapi_set_tx_gain
(
uint16 *tx_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
);

#endif /*ACMAPI_H*/
    
    