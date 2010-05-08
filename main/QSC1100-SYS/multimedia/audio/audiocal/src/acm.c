/** 
\file ***************************************************************************
*
*                                    AUDIO   CALIBRATION   MANAGER
*
*DESCRIPTION
* This file implements functions required to parse/build the command buffer to 
* set/get CALUNITS to calibration data tables in voccal.c/sndcal.c
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
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audiocal/QSC1100/main/latest/src/acm.c#8 $ $DateTime: 2009/06/12 00:14:09 $ $Author: markande $
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
#include <stdlib.h>/* required for malloc() and free()*/
#include <string.h>/* required for memcpy, memset, and memmove*/

#include "acm.h"
#include "acmapi.h"
#include "msg.h"
#include "voc.h"

#ifdef FEATURE_AVS_DYNAMIC_CALIBRATION

/*
Defined in voccal.c
*/
extern enum {
  VOC_CAL_PCM_PATH_CAL_CDMA,
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)  
#error code not present
  #endif
  VOC_CAL_PCM_PATH_CAL_MAX
} voc_cal_pcm_path_cfg;
/*
Defined in voccal.c
*/
extern const voc_pcm_path_cal_type 
                    *voc_pcm_cal_aptr[VOC_CAL_PCM_PATH_CAL_MAX][VOC_CODEC_MAX];

/** ACM session*/
atp_phone_context_struct  acm_phone_context;
/*const char caldb_file_name[50] = {"audiocaldb.cdb"};*/
/**
* FUNCTION : acm_read_voc_filter_indices
*
* DESCRIPTION :
* Helper function to read three indices(pcm path,method and device) required to locate a 
* voice filter from calibration table voc_pcm_cal_aptr
*Each index is a uint32 value (4bytes).
*
* DEPENDENCIES :None
*
* RETURN VALUE :acm_error_code
* Returns indices_ptr with red values from req_buf_ptr.
* If req_buf_ptr doesn't contains enough buffer to read, 
* returns ACM_ERR_UNEXPECTED_BUF_SIZE
*
* SIDE EFFECTS :Increments request buffer pointer (req_buf_ptr) by 12bytes, as it read 
*three indices.
* Decrements request buffer length (req_buf_length) by 12bytes.
 */
acm_error_code acm_read_voc_filter_indices
(
  byte **req_buf_ptr,/**< request buffer pointer*/
  uint32 *req_buf_length,/**< request buffer length*/
  voc_filter_indices_struct *indices_ptr/**<pointer to filter indices structure*/
)
{
  if( req_buf_ptr == NULL || *req_buf_ptr == NULL )
  {
    MSG_MED("acm_read_voc_filter_indices: req_buf_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  if( req_buf_length == NULL )
  {
    MSG_MED("acm_read_voc_filter_indices: req_buf_length is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
/**check we have enough buffer to read indices*/
  if( *req_buf_length < sizeof(voc_filter_indices_struct))
  {
    MSG_MED("acm_read_voc_filter_indices: Buffer size is less than voc_filter_indices_struct size"
                      , 0,0,0);
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }

/**Copy indices to structure from buffer*/
  memcpy(indices_ptr, *req_buf_ptr, sizeof(voc_filter_indices_struct));

/**Increment buffer pointer*/
  *req_buf_ptr += sizeof(voc_filter_indices_struct);

/** Decrement remaining buffer length*/
  *req_buf_length -= sizeof(voc_filter_indices_struct);

  if ( indices_ptr->snd_device < SND_DEVICE_HANDSET || 
        indices_ptr->snd_device >= SND_DEVICE_MAX
      )
  {
      MSG_MED("acm_read_voc_filter_indices: Invalid snd_device_type value", 0,0,0);
      return ACM_ERR_INVALID_INDEX;
  }

  if ( indices_ptr->snd_method < SND_METHOD_VOICE|| 
        indices_ptr->snd_method >= SND_METHOD_MAX
      )
  {
      MSG_MED("acm_read_voc_filter_indices: Invalid snd_method_type value", 0,0,0);
      return ACM_ERR_INVALID_INDEX;
  }

  if ( indices_ptr->voc_cal_pcm_path_cfg  < VOC_CAL_PCM_PATH_CAL_CDMA || 
        indices_ptr->voc_cal_pcm_path_cfg >= VOC_CAL_PCM_PATH_CAL_MAX
      )
  {
      MSG_MED("acm_read_voc_filter_indices: Invalid voc_cal_pcm_path_cfg value", 0,0,0);
      return ACM_ERR_INVALID_INDEX;
  }
  
  return ACM_ERR_NONE;
}

/**
* FUNCTION : acm_read_audio_filter_indices
*
* DESCRIPTION :
* Helper function to read two indices(method and device) required to locate a 
* soundcalibration data.in calibration table audio_config
*Each index is a uint32 value (4bytes).
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
* Returns indices_ptr with red values from req_buf_ptr
*If req_buf_ptr doesn't contains enough buffer to read, 
* returns ACM_ERR_UNEXPECTED_BUF_SIZE
*
* SIDE EFFECTS :Increments request buffer pointer (req_buf_ptr) by 8bytes, as it read 
*two indices.
* Decrements request buffer length (req_buf_length) by 8bytes.
 */
acm_error_code acm_read_audio_filter_indices
(
  byte **req_buf_ptr,/**< request buffer pointer*/
  uint32 *req_buf_length,/**< request buffer length*/
  audio_filter_indices_struct *indices_ptr/**<pointer to filter indices structure*/
)
{
  if( req_buf_ptr == NULL || *req_buf_ptr == NULL )
  {
    MSG_MED("acm_read_audio_filter_indices: req_buf_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  if( req_buf_length == NULL )
  {
    MSG_MED("acm_read_audio_filter_indices: req_buf_length is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /**check we have enough buffer to read indices*/
  if( *req_buf_length < sizeof(audio_filter_indices_struct))
  {
    MSG_MED("acm_read_voc_filter_indices: Buffer size is less than voc_filter_indices_struct size"
                    , 0,0,0);
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }

  /**Copy indices to structure from buffer*/
  memcpy(indices_ptr, *req_buf_ptr, sizeof(audio_filter_indices_struct));
  /**Increment buffer pointer*/
  *req_buf_ptr += sizeof(audio_filter_indices_struct);
  /**decrement remaining buffer length*/
  *req_buf_length -= sizeof(audio_filter_indices_struct);

  if ( indices_ptr->snd_device < SND_DEVICE_HANDSET || 
        indices_ptr->snd_device >= SND_DEVICE_MAX
      )
  {
      MSG_MED("acm_read_voc_filter_indices: Invalid snd_device_type value", 0,0,0);
      return ACM_ERR_INVALID_INDEX;
  }

  if ( indices_ptr->snd_method < SND_METHOD_VOICE|| 
        indices_ptr->snd_method >= SND_METHOD_MAX
      )
  {
      MSG_MED("acm_read_voc_filter_indices: Invalid snd_method_type value", 0,0,0);
      return ACM_ERR_INVALID_INDEX;
  }
   return ACM_ERR_NONE;
}
/**
* FUNCTION : acm_read_one_index
*
* DESCRIPTION :
* Helper function to read one index
*Each index is a uint32 value (4bytes).
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
* Returns indices_ptr with red values from req_buf_ptr
*If req_buf_ptr doesn't contains enough buffer to read, 
* returns ACM_ERR_UNEXPECTED_BUF_SIZE
*
* SIDE EFFECTS :Increments request buffer pointer (req_buf_ptr) by 4bytes, as it read 
*one index.
* Decrements request buffer length (req_buf_length) by 4bytes.
 */
acm_error_code acm_read_one_index
(
  byte **req_buf_ptr,/**< request buffer pointer*/
  uint32 *req_buf_length_ptr,/**< request buffer length*/
  uint32*index_ptr/**<pointer to index to be returned*/
)
{
  if ( *req_buf_ptr == NULL )
  {
    MSG_MED("acm_read_one_index: req_buf_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  if(*req_buf_length_ptr < sizeof(uint32) )
  {
    MSG_MED("acm_read_one_index: req_buf_ptr size is less than uint32 size ", 0,0,0);
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(index_ptr , *req_buf_ptr,sizeof(uint32));
  *req_buf_ptr += sizeof(uint32) ;
  *req_buf_length_ptr -= sizeof(uint32) ;
  return ACM_ERR_NONE ;
}

/**
* FUNCTION : acm_get_pcm_cal_ptr
*
* DESCRIPTION :
* Helper function to get voc_pcm_path_cal_type,ie calibration values for a PCM 
* path, based on the three indices(pcm path,method and device) passed to it, from 
* calibration table voc_pcm_cal_aptr.
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
* 
* SIDE EFFECTS :
* returns indices_ptr with red values from req_buf_ptr.
* Increments request buffer pointer (req_buf_ptr) by 12bytes, as it read 
*three indices.
* Decrements request buffer length (req_buf_length) by 12bytes.
 */
acm_error_code acm_get_pcm_cal_ptr
(
  /** indices to locate required 
    *calibration data for a PCM path
    */
  voc_filter_indices_struct *indices_ptr,
  /** Calibration data for requested indices 
    *  combination
    */
  voc_pcm_path_cal_type **pcm_cal_ptr
)
{
  snd_cal_control_type *audio_config = NULL;
  snd_cal_return_type retVal = SND_CAL_FAILED;
  snd_gen_voc_cal_type *voc_gen_cal=NULL;
  voc_codec_type pcm_path = VOC_CODEC_DEFAULT;

  if( indices_ptr == NULL )
  {
    MSG_MED("acm_get_pcm_cal_ptr: indices_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  if( pcm_cal_ptr == NULL )
  {
    MSG_MED("acm_get_pcm_cal_ptr: **pcm_cal_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /**
  * Get Calibration type for device and method
  */
  retVal = snd_cal_get_audio_control((snd_device_type)indices_ptr->snd_device,
                                                    (snd_method_type)indices_ptr->snd_method,&audio_config);

  if ( retVal == SND_CAL_FAILED || audio_config == NULL)
  {
    MSG_MED("acm_get_pcm_cal_ptr: snd_cal_get_audio_control failed", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /**
  * Get PCM path
  */
  voc_gen_cal = (snd_gen_voc_cal_type *)audio_config->gen_cal;
  if ( voc_gen_cal == NULL )
  {
    MSG_MED("acm_get_pcm_cal_ptr: voc_gen_cal is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  if ( voc_gen_cal->generator == SND_GEN_VOC 
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)  
  || voc_gen_cal->generator == SND_GEN_MIDI
#endif  
        
      )
  {
    pcm_path = voc_gen_cal->pcm_path;
    /**
    * Get calibration values for pcm_path
    */
    *pcm_cal_ptr = 
          (voc_pcm_path_cal_type *)voc_pcm_cal_aptr[indices_ptr->voc_cal_pcm_path_cfg][pcm_path];
  }

  if ( *pcm_cal_ptr == NULL )
  {
    MSG_MED("acm_get_pcm_cal_ptr: *pcm_cal_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  return ACM_ERR_NONE;
}

/**
* FUNCTION : acm_get_error_struct
*
* DESCRIPTION :
* Helper function to create a command buffer which wrppes the passed result and 
* error code. This command buffer is used to inform errors to client
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
* 
* SIDE EFFECTS : None
* 
*/
void acm_get_error_struct
(
/** command buffer with result and error code */
  byte **result_struct_ptr,
  /** command buffer length */
  uint32 *resp_buf_length,
  /** result value to be wrapped in command buffer*/
  boolean result,
  /** error code to be wrapped in command buffer*/
  uint16 error_code
)
{
  acm_cmd_result_struct *tmp_result_struct_ptr;
  /**Allocate memory*/
  tmp_result_struct_ptr = (acm_cmd_result_struct *)malloc(sizeof(acm_cmd_result_struct));
  if ( tmp_result_struct_ptr == NULL )
  {
    *resp_buf_length = 0 ;
    *result_struct_ptr = NULL;
    return;
  }
  /**set header info*/
  tmp_result_struct_ptr->cmd_header.cmd_id = CMD_RESULT;
  tmp_result_struct_ptr->cmd_header.cmd_buf_length = 
                      sizeof(acm_cmd_result_struct) - sizeof(acm_cmd_struct_header);
  tmp_result_struct_ptr->result_flag = result;
  tmp_result_struct_ptr->error_code = error_code;

  *result_struct_ptr = (byte *)tmp_result_struct_ptr;
  *resp_buf_length = sizeof(acm_cmd_result_struct);
}

#ifdef FEATURE_IIR_FILTER

/**
* FUNCTION : acm_set_iir_calunit
*
* DESCRIPTION :
* Function to set IIR CALUNIT members 
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
*    ACM_ERR_NONE if success
*    ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* SIDE EFFECTS :None
* 
*/
acm_error_code acm_set_iir_calunit
(
  /**pointer to the buffer containing IIR calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data from where we need to get IIR Calunit 
    *data 
    */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;

  /**
  *copy TX PCM filter coefficients
  */
  error = acmapi_set_tx_iir_filter(&pcm_cal_ptr->tx_iir_filter, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

   MSG_MED("acm_set_iir_calunit: Set TX coefficients ", 0,0,0);

  /**
  *copy RX PCM filter coefficients
  */
  error = acmapi_set_rx_iir_filter(&pcm_cal_ptr->rx_iir_filter, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("acm_set_iir_calunit: Set RX coefficients ", 0,0,0);

  #ifdef FEATURE_IIR_FILTER_5S
    /** 
    *copy 5stage TX PCM filter coefficients 
    */
    error = acmapi_set_tx_iir_filter(&pcm_cal_ptr->tx_iir_filter5, &req_buf_ptr, req_buf_length_ptr);
    if ( error != ACM_ERR_NONE )
    {
      return error;
    }
    MSG_MED("acm_set_iir_calunit: 5stage TX PCM filter coefficients", 0,0,0);
    /** 
    *copy 5stage RX PCM filter coefficients 
    */
    error = acmapi_set_rx_iir_filter(&pcm_cal_ptr->rx_iir_filter5, &req_buf_ptr, req_buf_length_ptr);
    if ( error != ACM_ERR_NONE )
    {
      return error;
    }
    MSG_MED("acm_set_iir_calunit: 5stage RX PCM filter coefficients", 0,0,0);
  #endif
  return ACM_ERR_NONE;
}

/**
* FUNCTION : acm_get_iir_calunit
*
* DESCRIPTION :
* returns an IIR filter calunit instance based on the indices: method, device 
* and pcm path 
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
*    ACM_ERR_NONE if success
*    ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* SIDE EFFECTS :None
* 
*/
acm_error_code acm_get_iir_calunit
(
  /** pointer to the buffer into which we get IIR calunit*/
  byte *iir_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length,
  /**pointer to audiocalibration data from where we need to get IIR Calunit data */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error_code = ACM_ERR_NONE;

  /**
  *copy TX PCM filter coefficients
  */
  error_code = acmapi_get_tx_iir_filter(&pcm_cal_ptr->tx_iir_filter, &iir_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("acm_get_iir_calunit: got tx_iir_filter", 0,0,0);

  /**
  *copy RX PCM filter coefficients
  */
  error_code = acmapi_get_rx_iir_filter(&pcm_cal_ptr->rx_iir_filter, &iir_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("acm_get_iir_calunit: got rx_iir_filter", 0,0,0);

  #ifdef FEATURE_IIR_FILTER_5S
    /**
    *copy 5 stage TX PCM filter coefficients
    */
    error_code = acmapi_get_tx_iir_filter(&pcm_cal_ptr->tx_iir_filter5, &iir_calunit_resp_buf_ptr);
    if ( error_code != ACM_ERR_NONE )
    {
      return error_code;
    }
    MSG_MED("acm_get_iir_calunit: got tx_iir_filter5", 0,0,0);

    /**
    *copy 5 stage RX PCM filter coefficients
    */
    error_code = acmapi_get_rx_iir_filter(&pcm_cal_ptr->rx_iir_filter5, &iir_calunit_resp_buf_ptr);
    if ( error_code != ACM_ERR_NONE )
    {
      return error_code;
    }
    MSG_MED("acm_get_iir_calunit: got tx_iir_filter5", 0,0,0);
  #endif
  return ACM_ERR_NONE;
}
#endif /*FEATURE_IIR_FILTER*/

#ifdef FEATURE_AVS_NEXTGEN_EC

/**
* FUNCTION acm_set_nextgen_ec_calunit
*
* DESCRIPTION : sets an NextGen_Ec calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: None
*/
acm_error_code acm_set_nextgen_ec_calunit
(
  /**pointer to the buffer containing NextGen EC calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data from where we need to get IIR Calunit data */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error = ACM_ERR_NONE;
  qdsp_cmd_nextgen_ec_parms_type *nextgen_ec_parms_ptr = NULL ;
  if(pcm_cal_ptr->nextgen_ec_parms == NULL )
  {
    MSG_MED("acm_set_nextgen_ec_calunit: nextgen_ec_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
    error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if ( ec_mode_idx >= VOC_EC_DEFAULT )
  {
	  MSG_MED("%s: %d Invalid Index", __FILE__,__LINE__,0);
	  return ACM_ERR_INVALID_INDEX;
  }
  nextgen_ec_parms_ptr = (qdsp_cmd_nextgen_ec_parms_type *)pcm_cal_ptr->nextgen_ec_parms[ec_mode_idx];
  if ( nextgen_ec_parms_ptr == NULL )
  {
    MSG_MED("acm_set_nextgen_ec_calunit: nextgen_ec_parms_ptr is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*copy nextgen_ec data*/
  error = acmapi_set_nextgen_ec_parms(nextgen_ec_parms_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("acm_set_nextgen_ec_calunit: set nextgen_ec data ", 0,0,0);
  
  if(pcm_cal_ptr->nextgen_ec_wb_parms == NULL )
  {
    MSG_MED("acm_set_nextgen_ec_calunit: nextgen_ec_wb_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  nextgen_ec_parms_ptr = (qdsp_cmd_nextgen_ec_parms_type *)pcm_cal_ptr->nextgen_ec_wb_parms[ec_mode_idx];
  if ( nextgen_ec_parms_ptr == NULL )
  {
    MSG_MED("acm_set_nextgen_ec_calunit: nextgen_ec_wb_parms_ptr is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  
  /*copy nextgen_ec_wb data   */
  error = acmapi_set_nextgen_ec_wb_parms(nextgen_ec_parms_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("acm_set_nextgen_ec_calunit: set nextgen_ec_wb data", 0,0,0);
  return ACM_ERR_NONE;
}
/**
* FUNCTION acm_get_nextgen_ec_calunit
*
* DESCRIPTION : returns a NextGen EC calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
 */
acm_error_code acm_get_nextgen_ec_calunit
(
  /** pointer to the buffer into which we get Nextgen EC calunit*/
  byte *nextgen_ec_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error_code = ACM_ERR_NONE;
  qdsp_cmd_nextgen_ec_parms_type *nextgen_ec_parms_ptr = NULL ;
  
  if(pcm_cal_ptr->nextgen_ec_parms == NULL )
  {
    MSG_MED("acm_get_nextgen_ec_calunit: nextgen_ec_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if ( ec_mode_idx >= VOC_EC_DEFAULT )
  {
	  MSG_MED("%s: %d Invalid Index", __FILE__,__LINE__,0);
	  return ACM_ERR_INVALID_INDEX;
  }
  memcpy(nextgen_ec_calunit_resp_buf_ptr, &ec_mode_idx, sizeof(uint32) );
  nextgen_ec_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  nextgen_ec_parms_ptr = (qdsp_cmd_nextgen_ec_parms_type *)pcm_cal_ptr->nextgen_ec_parms[ec_mode_idx];
  if ( nextgen_ec_parms_ptr == NULL )
  {
    MSG_MED("acm_get_nextgen_ec_calunit: nextgen_ec_parms_ptr[ec_mode_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*copy nextgen_ec data*/
  error_code = acmapi_get_nextgen_ec_parms(nextgen_ec_parms_ptr, &nextgen_ec_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }

  MSG_MED("acm_get_nextgen_ec_calunit: got nextgen_ec data ", 0,0,0);
  
  if(pcm_cal_ptr->nextgen_ec_wb_parms == NULL )
  {
    MSG_MED("acm_get_nextgen_ec_calunit: nextgen_ec_wb_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  nextgen_ec_parms_ptr = (qdsp_cmd_nextgen_ec_parms_type *)pcm_cal_ptr->nextgen_ec_wb_parms[ec_mode_idx];
  if ( nextgen_ec_parms_ptr == NULL )
  {
    MSG_MED("acm_get_nextgen_ec_calunit: nextgen_ec_wb_parms[ec_mode_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*copy nextgen_ec wide band data*/
  error_code = acmapi_get_nextgen_ec_wb_parms(nextgen_ec_parms_ptr, &nextgen_ec_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }

  MSG_MED("acm_get_nextgen_ec_calunit: got nextgen_ec wide band data", 0,0,0);
  return ACM_ERR_NONE;
}
#endif /*FEATURE_AVS_NEXTGEN_EC*/

#ifdef FEATURE_AUDFMT_IIR_FILTER
/**
* FUNCTION acm_set_audfmt_iir_calunit
*
* DESCRIPTION : sets an audfmt_iir_filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_audfmt_iir_calunit
(
  /**pointer to the buffer containing audfmt IIR calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 voc_sample_rate_idx = VOC_SAMPLE_RATE_NONE;
  acm_error_code error = ACM_ERR_NONE;
  qdsp_cmd_audfmt_iir_filter_type *audfmt_iir_filter_ptr = NULL ;
  
  if(pcm_cal_ptr->audfmt_iir_filter == NULL )
  {
    MSG_MED("acm_set_audfmt_iir_calunit: audfmt_iir_filter is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_set_audfmt_iir_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	 return ACM_ERR_INVALID_INDEX;
  }
  audfmt_iir_filter_ptr = (qdsp_cmd_audfmt_iir_filter_type *)pcm_cal_ptr->audfmt_iir_filter[voc_sample_rate_idx];
  if ( audfmt_iir_filter_ptr == NULL )
  {
    MSG_MED("acm_set_audfmt_iir_calunit: audfmt_iir_filter_ptr is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*copy audfmt iir filter data for given sampling rate*/
  error = acmapi_set_audfmt_iir_filter (audfmt_iir_filter_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("ACM: set audfmt_iir_filter data ", 0,0,0);
  return ACM_ERR_NONE;
}

/*
* FUNCTION acm_get_audfmt_iir_calunit
*
* DESCRIPTION : returns a audfmt_iir_filtercalunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
 */
acm_error_code acm_get_audfmt_iir_calunit
(
  /** pointer to the buffer into which we get audfmt_iir calunit*/
  byte *audfmt_iir_filter_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 voc_sample_rate_idx = 0 ;
  acm_error_code error_code = ACM_ERR_NONE;
  qdsp_cmd_audfmt_iir_filter_type *audfmt_iir_filter_ptr = NULL ;
    
  if(pcm_cal_ptr->audfmt_iir_filter == NULL )
  {
    MSG_MED("acm_get_audfmt_iir_calunit: audfmt_iir_filter is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
   if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_get_audfmt_iir_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  memcpy(audfmt_iir_filter_calunit_resp_buf_ptr, &voc_sample_rate_idx, sizeof(uint32) );
  audfmt_iir_filter_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  audfmt_iir_filter_ptr = (qdsp_cmd_audfmt_iir_filter_type *)pcm_cal_ptr->audfmt_iir_filter[voc_sample_rate_idx];
  if ( audfmt_iir_filter_ptr == NULL )
  {
    MSG_MED("acm_set_audfmt_iir_calunit: audfmt_iir_filter_ptr[voc_sample_rate_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*copy nextgen_ec data*/
  error_code = acmapi_get_audfmt_iir_filter(audfmt_iir_filter_ptr,
                                                            &audfmt_iir_filter_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }

  MSG_MED("ACM: got audfmt_iir_filter data ", 0,0,0);    
  return ACM_ERR_NONE;
}
#endif /*FEATURE_AUDFMT_IIR_FILTER*/

/*
* FUNCTION acm_set_fir_calunit
*
* DESCRIPTION : sets an FIR filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_fir_calunit
(
  /**pointer to the buffer containing FIR calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;

  /*copy tx path data   */
  error = acmapi_set_tx_filter(&pcm_cal_ptr->tx_filter, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("ACM: set tx_filter ", 0,0,0);

  /*copy rx path data   */
  error = acmapi_set_rx_filter(&pcm_cal_ptr->rx_filter, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("ACM: set tx_filter", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_fir_calunit
*
* DESCRIPTION : returns an FIR filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_fir_calunit
(
  /** pointer to the buffer into which we get FIR calunit instance*/
  byte *fir_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{  
  acm_error_code error_code = ACM_ERR_NONE;
  /*copy tx path data*/
  error_code = acmapi_get_tx_filter(&pcm_cal_ptr->tx_filter, &fir_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("ACM: got tx_filter", 0,0,0);
  /*copy rx path data*/
  error_code = acmapi_get_rx_filter(&pcm_cal_ptr->rx_filter, &fir_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("ACM: got rx_filter", 0,0,0);
  return ACM_ERR_NONE;
}

#ifdef FEATURE_AUDFMT_FIR_FILTER
  #ifdef FEATURE_QSYNTH_COMMON
/**
* FUNCTION acm_set_audfmt_qsynth_fir_calunit
*
* DESCRIPTION : sets an Audfmt Qsynth FIR filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_audfmt_qsynth_fir_calunit
(
  /**pointer to the buffer containing audfmt FIR calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;
  if ( pcm_cal_ptr->qsynth_filter == NULL )
  {
    MSG_MED("acm_set_audfmt_fir_calunit: qsynth_filter is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*QSYNTH PCM filter config */
  error = acmapi_set_qsynth_filter((qdsp_cmd_audfmt_pcm_filter_type*)&pcm_cal_ptr->qsynth_filter[0], &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  #ifdef MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
    /*QSYNTH PCM filter config */
    error = acmapi_set_qsynth_filter((qdsp_cmd_audfmt_pcm_filter_type*)&pcm_cal_ptr->qsynth_filter[1], &req_buf_ptr, req_buf_length_ptr);
    if ( error != ACM_ERR_NONE )
    {
      return error;
    }
  #endif
  MSG_MED("ACM: set audfmt_fir filter data ", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_audfmt_qsynth_fir_calunit
*
* DESCRIPTION : returns an Qsynth FIR filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_audfmt_qsynth_fir_calunit
(
  /** pointer to the buffer into which we get FIR calunit instance*/
  byte *audfmt_fir_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{  
  acm_error_code error_code = ACM_ERR_NONE;
  if ( pcm_cal_ptr->qsynth_filter == NULL )
  {
    MSG_MED("acm_get_audfmt_fir_calunit: qsynth_filter is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*copy filter data*/
  error_code = acmapi_get_qsynth_filter((qdsp_cmd_audfmt_pcm_filter_type*)&pcm_cal_ptr->qsynth_filter[0], &audfmt_fir_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  #ifdef MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
    /*copy filter data*/
    error_code = acmapi_get_qsynth_filter((qdsp_cmd_audfmt_pcm_filter_type*)&pcm_cal_ptr->qsynth_filter[1], &audfmt_fir_calunit_resp_buf_ptr);
    if ( error_code != ACM_ERR_NONE )
    {
      return error_code;
    }
  #endif
  MSG_MED("ACM: got audfmt_fir_filter data", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION : acmapi_get_audfmt_qsynth_fir_calunit_size
*
* DESCRIPTION : calculates audfmt qsynth FIR calunit size
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
uint32  acmapi_get_audfmt_qsynth_fir_calunit_size(void)
{
  uint32 req_buf_length = 0 ;
  /**
  *Calculate audfmt fir size 
  */
  req_buf_length = acmapi_get_fir_pcm_qsynth_filter_calunit_size();
  #ifdef MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
    req_buf_length  += acmapi_get_fir_pcm_qsynth_filter_calunit_size() ;
  #endif
  MSG_MED("ACMI: returning AudFMT FIR CalUnit Size as %d", req_buf_length,0,0);
  return req_buf_length;
}
#endif /*FEATURE_QSYNTH_COMMON*/
  #ifdef FEATURE_QTUNES_COMMON
/**
* FUNCTION acm_set_audfmt_qtunes_fir_calunit
*
* DESCRIPTION : sets an Audfmt QTunes FIR filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_audfmt_qtunes_fir_calunit
(
  /**pointer to the buffer containing audfmt FIR calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;
  uint32 voc_sample_rate_idx = VOC_SAMPLE_RATE_NONE;
  qdsp_cmd_audfmt_pcm_filter_type *audfmt_pcm_filter_ptr = NULL;
  if ( pcm_cal_ptr->qtunes_filter == NULL )
  {
    MSG_MED("acm_set_audfmt_fir_calunit: qtunes_filter is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_set_audfmt_qtunes_fir_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  audfmt_pcm_filter_ptr = (qdsp_cmd_audfmt_pcm_filter_type *)pcm_cal_ptr->qtunes_filter[voc_sample_rate_idx];
  if ( audfmt_pcm_filter_ptr == NULL )
  {
    MSG_MED("acm_set_audfmt_fir_calunit: audfmt_pcm_filter_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*QSYNTH PCM filter config */
  error = acmapi_set_qtunes_filter(audfmt_pcm_filter_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("ACM: set audfmt_fir filter data ", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_audfmt_qtunes_fir_calunit
*
* DESCRIPTION : returns an qtunes FIR filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_audfmt_qtunes_fir_calunit
(
  /** pointer to the buffer into which we get FIR calunit instance*/
  byte *audfmt_fir_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{  
  acm_error_code error_code = ACM_ERR_NONE;
  uint32 voc_sample_rate_idx = VOC_SAMPLE_RATE_NONE;
  qdsp_cmd_audfmt_pcm_filter_type *audfmt_pcm_filter_ptr = NULL;
    
  if ( pcm_cal_ptr->qtunes_filter == NULL )
  {
    MSG_MED("acm_get_audfmt_fir_calunit: qsynth_filter is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_set_audfmt_qtunes_fir_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  memcpy(audfmt_fir_calunit_resp_buf_ptr, &voc_sample_rate_idx, sizeof(uint32) );
  audfmt_fir_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  audfmt_pcm_filter_ptr = (qdsp_cmd_audfmt_pcm_filter_type *)pcm_cal_ptr->qtunes_filter[voc_sample_rate_idx];
  if ( audfmt_pcm_filter_ptr == NULL )
  {
    MSG_MED("acm_get_audfmt_qtunes_fir_calunit: audfmt_pcm_filter_ptr[voc_sample_rate_index] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*copy filter data*/
  error_code = acmapi_get_qtunes_filter(audfmt_pcm_filter_ptr, &audfmt_fir_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }

  MSG_MED("ACM: got audfmt_fir_filter data", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION : acmapi_get_audfmt_qtunes_fir_calunit_size
*
* DESCRIPTION : calculates audfmt FIR calunit size
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
uint32  acmapi_get_audfmt_qtunes_fir_calunit_size(void)
{
  uint32 req_buf_length = 0 ;
  /**
  *Calculate audfmt fir size 
  */
  req_buf_length = acmapi_get_fir_pcm_qtunes_filter_calunit_size();
  MSG_MED("ACMI: returning AudFMT QTunes FIR CalUnit Size as %d", req_buf_length,0,0);
  return req_buf_length;
}
#endif /*FEATURE_QTUNES_COMMON*/
#endif /*FEATURE_AUDFMT_FIR_FILTER*/

/**
* FUNCTION acm_set_ec_calunit
*
* DESCRIPTION : sets an Echo Canceller filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_ec_calunit
(
  /**pointer to the buffer containing audfmt Echocanceller calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error = ACM_ERR_NONE;
  qdsp_cmd_ec_params_type *ec_params_ptr = NULL;
  if(pcm_cal_ptr->ec_params == NULL )
  {
    MSG_MED("acm_set_ec_calunit: ec_params is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if (ec_mode_idx >= VOC_EC_DEFAULT ) 
  {
    MSG_MED("acm_set_ec_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  ec_params_ptr = (qdsp_cmd_ec_params_type *)pcm_cal_ptr->ec_params[ec_mode_idx];
  if ( ec_params_ptr == NULL )
  {
    MSG_MED("acm_set_ec_calunit: ec_params_ptr[ec_mode_idx]l is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*set ec data */
  error = acmapi_set_ec_params(ec_params_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("ACM: set EC data ", 0,0,0);
  return ACM_ERR_NONE;
}

/*
* FUNCTION acm_get_ec_calunit
*
* DESCRIPTION : returns an Echo Canceller filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: None
*/
acm_error_code acm_get_ec_calunit
(
  /** pointer to the buffer into which we get Echocanceller calunit instance*/
  byte *ec_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error_code = ACM_ERR_NONE;
  qdsp_cmd_ec_params_type *ec_params_ptr = NULL;
  if(pcm_cal_ptr->ec_params == NULL )
  {
    MSG_MED("acm_get_ec_calunit: ec_params is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if (ec_mode_idx >= VOC_EC_DEFAULT ) 
  {
    MSG_MED("acm_get_ec_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  memcpy(ec_calunit_resp_buf_ptr, &ec_mode_idx, sizeof(uint32) );
  ec_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  ec_params_ptr = (qdsp_cmd_ec_params_type *)pcm_cal_ptr->ec_params[ec_mode_idx];
  if ( ec_params_ptr == NULL )
  {
    MSG_MED("acm_get_ec_calunit: ec_params[ec_mode_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*get ec data*/
  error_code = acmapi_get_ec_params(ec_params_ptr, &ec_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }

  MSG_MED("ACM: got EC data", 0,0,0);
  return ACM_ERR_NONE;
}

#ifdef FEATURE_AUDIO_AGC
/**
* FUNCTION acm_set_audio_agc_calunit
*
* DESCRIPTION : sets an audio_agc filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
 */
acm_error_code acm_set_audio_agc_calunit
(
  /**pointer to the buffer containing audfmt Echocanceller calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;
  /*set ec data */
  error = acmapi_set_agc_param(&pcm_cal_ptr->agc_param, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("ACM: set audio_agc data ", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_audio_agc_calunit
*
* DESCRIPTION : returns an audio_agc filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_audio_agc_calunit
(
  /** pointer to the buffer into which we get audio agc calunit instance*/
  byte *audio_agc_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error_code = ACM_ERR_NONE;
  /*get audio agc data*/
  error_code = acmapi_get_agc_param(&pcm_cal_ptr->agc_param, &audio_agc_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("ACM: got audio agc data", 0,0,0);
  return ACM_ERR_NONE;
}
#endif /*#ifdef FEATURE_AUDIO_AGC*/

#ifdef FEATURE_QSYNTH_ADRC
/**
* FUNCTION acm_set_qsynth_adrc_calunit
*
* DESCRIPTION : sets an qsynth_adrc filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_qsynth_adrc_calunit
(
  /**pointer to the buffer containing qsynth adrc calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;
  uint32 voc_sample_rate_idx = 0 ;
  qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr = NULL;
  if (pcm_cal_ptr->qsynth_adrc_ctl == NULL )
  {
    MSG_MED("acm_set_qsynth_adrc_calunit: qsynth_adrc_ctl is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error != ACM_ERR_NONE )
  {
   	return error;
  }
  if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_set_qsynth_adrc_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  
  audfmt_adrc_ctl_ptr = (qdsp_cmd_audfmt_adrc_ctl_type *)pcm_cal_ptr->qsynth_adrc_ctl[voc_sample_rate_idx];
  if ( audfmt_adrc_ctl_ptr == NULL )
  {
    MSG_MED("acm_set_qsynth_adrc_calunit: qsynth_adrc_ct[voc_sample_rate_idx]l is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
    /*set qsynth adrc data */
  error = acmapi_set_qsynth_adrc_ctl(audfmt_adrc_ctl_ptr, &req_buf_ptr, req_buf_length_ptr);
    if ( error != ACM_ERR_NONE )
    {
    return error;
    }
   
  MSG_MED("ACM: set qsynth adrc data ", 0,0,0);
    return ACM_ERR_NONE;
}

/*=============================================================
 FUNCTION acm_get_qsynth_adrc_calunit

 DESCRIPTION : returns an qsynth_adrc filter calunit instance based on the indices. 

 DEPENDENCIES: None 

 RETURN VALUE: acm_error_code

 ACM_ERR_NONE if success
 ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
 
 SIDE EFFECTS: None
 ============================================================*/
acm_error_code acm_get_qsynth_adrc_calunit
(
  /** pointer to the buffer into which we get qsynth adrc calunit instance*/
  byte *qsynth_adrc_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{  
    acm_error_code error_code = ACM_ERR_NONE;
	uint32 voc_sample_rate_idx = 0 ;
	qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr = NULL;
  if (pcm_cal_ptr->qsynth_adrc_ctl == NULL )
  {
    MSG_MED("acm_set_qsynth_adrc_calunit: qsynth_adrc_ctl is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_get_qsynth_adrc_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  memcpy(qsynth_adrc_calunit_resp_buf_ptr, &voc_sample_rate_idx, sizeof(uint32) );
  qsynth_adrc_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  audfmt_adrc_ctl_ptr = (qdsp_cmd_audfmt_adrc_ctl_type *)pcm_cal_ptr->qsynth_adrc_ctl[voc_sample_rate_idx];
  if ( audfmt_adrc_ctl_ptr == NULL )
  {
    MSG_MED("acm_get_qsynth_adrc_calunit: audfmt_adrc_ctl_ptr[voc_sample_rate_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*get qsynth_adrc data*/

  error_code = acmapi_get_qsynth_adrc_ctl(audfmt_adrc_ctl_ptr, &qsynth_adrc_calunit_resp_buf_ptr);
    if ( error_code != ACM_ERR_NONE )
    {
      return error_code;
    }
  
  MSG_MED("ACM: got qsynth adrc data", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION : acmapi_get_qsynth_adrc_calunit_size
*
* DESCRIPTION : calculates QSynth ADRC calunit size
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
uint32  acmapi_get_qsynth_adrc_calunit_size(void)
{
    uint32 req_buf_length = 0 ;
    /* qsynth_adrc */
    req_buf_length = acmapi_get_adrc_qsynth_filter_calunit_size() ;
    #ifdef MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
    req_buf_length += acmapi_get_adrc_qsynth_filter_calunit_size() ;
    #endif/*MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT*/
    MSG_MED("ACMI: returning qsynth_adrc CalUnit Size as %d", req_buf_length,0,0);
    return req_buf_length;
}

#endif /*FEATURE_QSYNTH_ADRC*/

#ifdef FEATURE_QTUNES_ADRC
/**
* FUNCTION acm_set_qtunes_adrc_calunit
*
* DESCRIPTION : sets an qtunes_adrc filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_qtunes_adrc_calunit
(
  /**pointer to the buffer containing qsynth adrc calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;
  uint32 voc_sample_rate_idx = 0 ;
  qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr = NULL;
  if ( pcm_cal_ptr->qtunes_adrc_ctl == NULL )
  {
    MSG_MED("acm_set_qtunes_adrc_calunit: qtunes_adrc_ctl is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_set_qtunes_adrc_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  
  audfmt_adrc_ctl_ptr = (qdsp_cmd_audfmt_adrc_ctl_type *)pcm_cal_ptr->qtunes_adrc_ctl[voc_sample_rate_idx];
  if ( audfmt_adrc_ctl_ptr == NULL )
  {
    MSG_MED("acm_set_qtunes_adrc_calunit: qtunes_adrc_ct[voc_sample_rate_idx]l is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*set qtunes adrc data */
  error = acmapi_set_qtunes_adrc_ctl(audfmt_adrc_ctl_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("ACM: set qtunes adrc data ", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_qtunes_adrc_calunit
*
* DESCRIPTION : returns an qtunes_adrc filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_qtunes_adrc_calunit
(
  /** pointer to the buffer into which we get qtunes adrc calunit instance*/
  byte *qtunes_adrc_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error_code = ACM_ERR_NONE;
  uint32 voc_sample_rate_idx = 0 ;
  qdsp_cmd_audfmt_adrc_ctl_type *audfmt_adrc_ctl_ptr = NULL;
    if ( pcm_cal_ptr->qtunes_adrc_ctl == NULL )
  {
    MSG_MED("acm_set_qtunes_adrc_calunit: qtunes_adrc_ctl is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &voc_sample_rate_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if (voc_sample_rate_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_get_qtunes_adrc_calunit: Invalid voc_sample_rate_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  memcpy(qtunes_adrc_calunit_resp_buf_ptr, &voc_sample_rate_idx, sizeof(uint32) );
  qtunes_adrc_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  audfmt_adrc_ctl_ptr = (qdsp_cmd_audfmt_adrc_ctl_type *)pcm_cal_ptr->qtunes_adrc_ctl[voc_sample_rate_idx];
  if ( audfmt_adrc_ctl_ptr == NULL )
  {
    MSG_MED("acm_get_qtunes_adrc_calunit: audfmt_adrc_ctl_ptr[voc_sample_rate_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  
  /*get qtunes_adrc data*/

  error_code = acmapi_get_qtunes_adrc_ctl(audfmt_adrc_ctl_ptr, &qtunes_adrc_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  
  MSG_MED("ACM: got qtunes adrc data", 0,0,0);
  return ACM_ERR_NONE;
}
#endif /*FEATURE_QTUNES_ADRC*/

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/**
* FUNCTION acm_set_crystal_speech_calunit
*
* DESCRIPTION : sets an crystal_speech calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_crystal_speech_calunit
(
  /**pointer to the buffer containing crystal speech calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error = ACM_ERR_NONE;
  qdsp_cmd_crystal_speech_parms_type *crystal_speech_parms_ptr = NULL;
  if ( pcm_cal_ptr->cs_parms == NULL )
  {
    MSG_MED("acm_set_crystal_speech_calunit: cs_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if (ec_mode_idx >= VOC_EC_DEFAULT ) 
  {
    MSG_MED("acm_set_crystal_speech_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  crystal_speech_parms_ptr = (qdsp_cmd_crystal_speech_parms_type *)pcm_cal_ptr->cs_parms[ec_mode_idx];
  if ( crystal_speech_parms_ptr == NULL )
  {
    MSG_MED("acm_set_crystal_speech_calunit: cs_parms[ec_mode_idx]l is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*copy crystal speech data*/
  error = acmapi_set_cs_parms(crystal_speech_parms_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("ACM: set crystal_speech data ", 0,0,0);
  if ( pcm_cal_ptr->cs_wb_parms == NULL )
  {
    MSG_MED("acm_set_crystal_speech_calunit: cs_wb_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  crystal_speech_parms_ptr = (qdsp_cmd_crystal_speech_parms_type *)pcm_cal_ptr->cs_wb_parms[ec_mode_idx];
  if ( crystal_speech_parms_ptr == NULL )
  {
    MSG_MED("acm_set_crystal_speech_calunit: cs_wb_parms[ec_mode_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*/copy crystal speech data for wideband*/
  error = acmapi_set_cs_wb_parms(crystal_speech_parms_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("ACM: set crystal_speech_wb data", 0,0,0);  
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_crystal_speech_calunit
*
* DESCRIPTION : returns a crystal_speech calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_crystal_speech_calunit
(
  /** pointer to the buffer into which we get crystal_speech calunit instance*/
  byte *cs_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error_code = ACM_ERR_NONE;
  qdsp_cmd_crystal_speech_parms_type *crystal_speech_parms_ptr = NULL;
  if ( pcm_cal_ptr->cs_parms == NULL )
  {
    MSG_MED("acm_get_crystal_speech_calunit: cs_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if (ec_mode_idx >= VOC_EC_DEFAULT ) 
  {
    MSG_MED("acm_get_crystal_speech_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  memcpy(cs_calunit_resp_buf_ptr, &ec_mode_idx, sizeof(uint32) );
  cs_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  crystal_speech_parms_ptr = (qdsp_cmd_crystal_speech_parms_type *)pcm_cal_ptr->cs_parms[ec_mode_idx];
  if ( crystal_speech_parms_ptr == NULL )
  {
    MSG_MED("acm_get_crystal_speech_calunit: cs_parms[ec_mode_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*copy crystal_speech data*/
  error_code = acmapi_get_cs_parms(crystal_speech_parms_ptr, &cs_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("ACM: got crystal_speech data ", 0,0,0);


  if ( pcm_cal_ptr->cs_wb_parms == NULL )
  {
    MSG_MED("acm_get_crystal_speech_calunit: cs_wb_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  crystal_speech_parms_ptr = (qdsp_cmd_crystal_speech_parms_type *)pcm_cal_ptr->cs_wb_parms[ec_mode_idx];
  if ( crystal_speech_parms_ptr == NULL )
  {
    MSG_MED("acm_get_crystal_speech_calunit: cs_wb_parms[ec_mode_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*copy crystal_speech wide band data*/
  error_code = acmapi_get_cs_wb_parms(crystal_speech_parms_ptr, &cs_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("ACM: got crystal_speech wide band data", 0,0,0);	
  return ACM_ERR_NONE;
}
#endif /*FEATURE_AVS_CRYSTAL_SPEECH*/

/**
* FUNCTION acm_set_rve_parameters_calunit
*
* DESCRIPTION : sets an crystal_speech calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_rve_parameters_calunit
(
  /**pointer to the buffer containing rve param instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error = ACM_ERR_NONE;
  qdsp_cmd_dual_rve_param_type* dual_rve_param_ptr=NULL;
  qdsp_cmd_rve_param_type* rve_param_ptr=NULL;
  /*########READ INDEX##########*/
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if (ec_mode_idx >= VOC_EC_DEFAULT ) 
  {
    MSG_MED("acm_set_crystal_speech_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  /*########READ RVE PARAM##########*/
  if ( pcm_cal_ptr->rve_param == NULL )
  {
    MSG_MED("acm_set_crystal_speech_calunit: cs_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  
  
  rve_param_ptr = (qdsp_cmd_rve_param_type*)pcm_cal_ptr->rve_param[ec_mode_idx];
  if ( rve_param_ptr != NULL )
  {
	  /*copy crystal speech data*/
	  error = acmapi_set_rve_param(rve_param_ptr, &req_buf_ptr, req_buf_length_ptr);
	  if ( error != ACM_ERR_NONE )
	  {
		return error;
	  }
	   MSG_MED("ACM: set rve param data ", 0,0,0);
  }

  /*########DUAL RVE PARAM##########*/
  if ( pcm_cal_ptr->dual_rve_param == NULL )
  {
    MSG_MED("acm_set_rve_parameters_calunit: dual_rve_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  dual_rve_param_ptr = (qdsp_cmd_dual_rve_param_type*)pcm_cal_ptr->dual_rve_param[ec_mode_idx];
  if ( dual_rve_param_ptr != NULL )
  {
      error = acmapi_set_dual_rve_param(dual_rve_param_ptr, &req_buf_ptr, req_buf_length_ptr);
	  if ( error != ACM_ERR_NONE )
	  {
		return error;
	  }
	  MSG_MED("ACM: set dual rve param data", 0,0,0);  
  }
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_rve_parameters_calunit
*
* DESCRIPTION : returns a rve_parameters calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_rve_parameters_calunit
(
  /** pointer to the buffer into which we get crystal_speech calunit instance*/
  byte *rve_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error_code = ACM_ERR_NONE;
  
  qdsp_cmd_dual_rve_param_type* dual_rve_param_ptr=NULL;
  qdsp_cmd_rve_param_type* rve_param_ptr=NULL;
  
  
  if ( pcm_cal_ptr->rve_param == NULL )
  {
    MSG_MED("acm_get_rve_parameters_calunit: cs_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if (ec_mode_idx >= VOC_EC_DEFAULT ) 
  {
    MSG_MED("acm_get_rve_parameters_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  
  /*Write forth index to respose buffer*/
  memcpy(rve_calunit_resp_buf_ptr, &ec_mode_idx, sizeof(uint32) );
  rve_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  rve_param_ptr = (qdsp_cmd_rve_param_type*)pcm_cal_ptr->rve_param[ec_mode_idx];
  if (rve_param_ptr != NULL )
  {
	  /*copy crystal_speech data*/
	  error_code = acmapi_get_rve_param(rve_param_ptr, &rve_calunit_resp_buf_ptr);
	  if ( error_code != ACM_ERR_NONE )
	  {
		return error_code;
	  }
	  MSG_MED("ACM: got crystal_speech data ", 0,0,0);
  }

  if ( pcm_cal_ptr->dual_rve_param == NULL )
  {
    MSG_MED("acm_get_rve_parameters_calunit: cs_wb_parms is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  dual_rve_param_ptr = (qdsp_cmd_dual_rve_param_type *)pcm_cal_ptr->dual_rve_param[ec_mode_idx];
  if ( dual_rve_param_ptr != NULL )
  {
    /*copy crystal_speech data*/
	  error_code = acmapi_get_dual_rve_param(dual_rve_param_ptr, &rve_calunit_resp_buf_ptr);
	  if ( error_code != ACM_ERR_NONE )
	  {
		return error_code;
	  }
	  MSG_MED("ACM: got dual_rve_param_ptr data ", 0,0,0);
  }
  return ACM_ERR_NONE;
}

#ifdef FEATURE_AUDFMT_STF
/**
* FUNCTION acm_set_audfmt_stf_calunit
*
* DESCRIPTION : sets an audfmt_stf filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_audfmt_stf_calunit
(
  /**pointer to the buffer containing audfmt side chain tuning filter calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error = ACM_ERR_NONE;
  qdsp_cmd_audfmt_sidechain_filter_type *audfmt_sidechain_filter_ptr = NULL;
  if( pcm_cal_ptr->audfmt_sidechain_filter == NULL )
  {
    MSG_MED("acm_set_audfmt_stf_calunit: audfmt_sidechain_filter is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error = acm_read_one_index(&req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  if (ec_mode_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_set_audfmt_stf_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  audfmt_sidechain_filter_ptr = (qdsp_cmd_audfmt_sidechain_filter_type *)pcm_cal_ptr->audfmt_sidechain_filter[ec_mode_idx];
  if ( audfmt_sidechain_filter_ptr == NULL )
  {
    MSG_MED("acm_set_audfmt_stf_calunit: audfmt_sidechain_filter[ec_mode_idx]l is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*set audfmt_sidechain_filter data */
  error = acmapi_set_audfmt_sidechain_filter(audfmt_sidechain_filter_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }

  MSG_MED("ACM: set audfmt sidechain tuning filter data ", 0,0,0);
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_audfmt_stf_calunit
*
* DESCRIPTION : returns an audfmt_stf filter calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_audfmt_stf_calunit
(
  /** pointer to the buffer into which we get audfmt_stf calunit instance*/
  byte *audfmt_stf_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data instance */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  uint32 ec_mode_idx = 0 ;
  acm_error_code error_code = ACM_ERR_NONE;
  qdsp_cmd_audfmt_sidechain_filter_type *audfmt_sidechain_filter_ptr = NULL;
  if( pcm_cal_ptr->audfmt_sidechain_filter == NULL )
  {
    MSG_MED("acm_get_audfmt_stf_calunit: audfmt_sidechain_filter is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  error_code = acm_read_one_index(req_buf_ptr, req_buf_length_ptr, &ec_mode_idx);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  if (ec_mode_idx >= VOC_SAMPLE_RATE_MAX ) 
  {
    MSG_MED("acm_set_audfmt_stf_calunit: Invalid ec_mode_idx value", 0,0,0);
	return ACM_ERR_INVALID_INDEX;
  }
  memcpy(audfmt_stf_calunit_resp_buf_ptr, &ec_mode_idx, sizeof(uint32) );
  audfmt_stf_calunit_resp_buf_ptr += sizeof( uint32 ) ;
  
  audfmt_sidechain_filter_ptr = (qdsp_cmd_audfmt_sidechain_filter_type *)pcm_cal_ptr->audfmt_sidechain_filter[ec_mode_idx];
  if ( audfmt_sidechain_filter_ptr == NULL )
  {
    MSG_MED("acm_get_audfmt_stf_calunit: audfmt_sidechain_filter_ptr[ec_mode_idx] is NULL ", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*get audfmt_stf data*/
  error_code = acmapi_get_audfmt_sidechain_filter(audfmt_sidechain_filter_ptr, &audfmt_stf_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }

  MSG_MED("ACM: got audfmt sidechain tuning filter data", 0,0,0);
  return ACM_ERR_NONE;
}
#endif /*FEATURE_AUDFMT_STF*/

/**
* FUNCTION acm_get_slow_talk_calunit
*
* DESCRIPTION : returns an slow talk calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_slow_talk_calunit
(
  byte *req_buf_ptr,
  uint32 *req_buf_length,
  byte *resp_buf_ptr
)
{
	audio_filter_indices_struct indices;
	acm_error_code error_code = ACM_ERR_NONE;
	
	/*skip the index values*/
	error_code = acm_read_audio_filter_indices(&req_buf_ptr,req_buf_length,&indices);
	if ( error_code != ACM_ERR_NONE )
	{
		return error_code;
	}
	memcpy(resp_buf_ptr, (void *)&indices, sizeof(audio_filter_indices_struct));
	resp_buf_ptr += sizeof(audio_filter_indices_struct);
	memcpy(resp_buf_ptr,&voc_cal_slowtalk_configs,sizeof(qdsp_cmd_slowtalk_parms_type));
	resp_buf_ptr += sizeof(qdsp_cmd_slowtalk_parms_type);
	return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_sound_calibration_calunit
*
* DESCRIPTION : returns an sound calibration calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_get_sound_calibration_calunit
(
  byte *req_buf_ptr,
  uint32 *req_buf_length,
  byte *snd_cal_calunit_resp_buf_ptr
)
{
  snd_cal_control_type *audio_config_ptr;
  snd_cal_return_type retVal;
  audio_filter_indices_struct indices;
  acm_error_code error_code = ACM_ERR_NONE;
  /*read indices from request buffer*/
  error_code = acm_read_audio_filter_indices(&req_buf_ptr,req_buf_length,&indices);
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }

  /*copy the indices in to response buffer */
  memcpy(snd_cal_calunit_resp_buf_ptr, (void *)&indices, sizeof(audio_filter_indices_struct));
  /*increment the response buffer pointer*/
  snd_cal_calunit_resp_buf_ptr += sizeof(audio_filter_indices_struct);
/*Get calibration data instance */
  retVal = snd_cal_get_audio_control((snd_device_type)indices.snd_device,
                                (snd_method_type)indices.snd_method,&audio_config_ptr);

  if ( retVal == SND_CAL_FAILED || audio_config_ptr == NULL)
  {
    MSG_MED("acm_get_sound_calibration_calunit: snd_cal_get_audio_control failed", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /* copy volume levels */
  error_code =  acmapi_get_volume_levels_data(audio_config_ptr, &snd_cal_calunit_resp_buf_ptr);
  if ( error_code != ACM_ERR_NONE )
  {
    MSG_MED("acm_get_sound_calibration_calunit: could not get volume level data", 0,0,0);
    return error_code;
  }

  MSG_MED("ACM: got volume levels data", 0,0,0);
  return ACM_ERR_NONE;

}
/**
* FUNCTION acm_set_slow_talk_calunit
*
* DESCRIPTION : sets an slow talk calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: None
*/
acm_error_code acm_set_slow_talk_calunit
(
  byte *req_buf_ptr,
  uint32 *req_buf_length_ptr
)
{
  audio_filter_indices_struct audio_filter_indices;
  acm_error_code error = ACM_ERR_NONE;
  error = acm_read_audio_filter_indices(&req_buf_ptr, req_buf_length_ptr, &audio_filter_indices);
  if ( error != ACM_ERR_NONE )
  {
   	  return error;
  }
  if( *req_buf_length_ptr < sizeof(qdsp_cmd_slowtalk_parms_type))
  {
     MSG_MED("acm_set_slow_talk_calunit: Buffer size is less than qdsp_cmd_slowtalk_parms_type size",0,0,0);
     return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(&voc_cal_slowtalk_configs,req_buf_ptr,sizeof(qdsp_cmd_slowtalk_parms_type));
  (*req_buf_length_ptr)-= sizeof(qdsp_cmd_slowtalk_parms_type);
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_set_sound_calibration_calunit
*
* DESCRIPTION : sets an sound calibration calunit instance based on the indices. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
*
* ACM_ERR_NONE if success
* ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
*
* SIDE EFFECTS: None
*/
acm_error_code acm_set_sound_calibration_calunit
(
  byte *req_buf_ptr,
  uint32 *req_buf_length_ptr
)
{
  audio_filter_indices_struct audio_filter_indices;
  
  snd_cal_control_type *audio_config_ptr;
  snd_cal_return_type retVal;
  acm_error_code error = ACM_ERR_NONE;

  error = acm_read_audio_filter_indices(&req_buf_ptr, req_buf_length_ptr, &audio_filter_indices);
  if ( error != ACM_ERR_NONE )
  {
   	return error;
  }
  
  retVal = snd_cal_get_audio_control((snd_device_type)audio_filter_indices.snd_device
                                      ,(snd_method_type)audio_filter_indices.snd_method,&audio_config_ptr);

  if ( retVal == SND_CAL_FAILED || audio_config_ptr == NULL)
  {
    MSG_MED("acm_set_sound_calibration_calunit: snd_cal_get_audio_control failed", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*set volume levels*/
  error = acmapi_set_volume_levels_data(audio_config_ptr, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("ACM: set volume levels data ", 0,0,0);
  
  return ACM_ERR_NONE;
}

/**
* FUNCTION : acm_set_gains_calunit
*
* DESCRIPTION :
* Function to set Gains CALUNIT members 
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
*    ACM_ERR_NONE if success
*    ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* SIDE EFFECTS :None
* 
*/
acm_error_code acm_set_gains_calunit
(
  /**pointer to the buffer containing gains calunit instance*/
  byte *req_buf_ptr,
  /**buffer length*/
  uint32 *req_buf_length_ptr,
  /**pointer to audiocalibration data from where we need to get gains Calunit 
    *data 
    */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
  acm_error_code error = ACM_ERR_NONE;

  /**
  *copy TX Voice Volume
  */
  error = acmapi_set_tx_gain(&pcm_cal_ptr->tx_gain, &req_buf_ptr, req_buf_length_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("acm_set_gains_calunit: Set TX Voice Volume ", 0,0,0);

  /**
  *copy TX DTMF gain
  */
  error = acmapi_set_tx_dtmf_gain(&pcm_cal_ptr->dtmf_tx_gain,
                                                         &req_buf_ptr, req_buf_length_ptr
                                                        );
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("acm_set_gains_calunit: Set DTMF TX gain ", 0,0,0);

  /**
  *copy CODEC TX gain
  */
  error = acmapi_set_codec_tx_gain(&pcm_cal_ptr->codec_tx_gain,
                                                         &req_buf_ptr, req_buf_length_ptr
                                                        );
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("acm_set_gains_calunit: Set CODEC TX gain ", 0,0,0);

  /**
  *copy CODEC RX gain
  */
  error = acmapi_set_codec_rx_gain(&pcm_cal_ptr->codec_rx_gain,
                                                         &req_buf_ptr, req_buf_length_ptr
                                                        );
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("acm_set_gains_calunit: Set CODEC RX gain ", 0,0,0);

    /**
  *copy CODEC ST gain
  */
  error = acmapi_set_codec_st_gain(&pcm_cal_ptr->codec_st_gain,
                                                         &req_buf_ptr, req_buf_length_ptr
                                                        );
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  MSG_MED("acm_set_gains_calunit: Set CODEC ST gain ", 0,0,0);
  return ACM_ERR_NONE;
}
/**
* FUNCTION : acm_get_gains_calunit
*
* DESCRIPTION :
* returns a gain calunit instance based on the indices: method, device 
* and pcm path 
*
* DEPENDENCIES :None
*
* RETURN VALUE : acm_error_code
*    ACM_ERR_NONE if success
*    ACM_ERR_UNEXPECTED_BUF_SIZE if buffer size doesnot matches with expected structure size
* SIDE EFFECTS :None
* 
*/
acm_error_code acm_get_gains_calunit
(
  /** pointer to the buffer into which we get gains calunit*/
  byte *gains_calunit_resp_buf_ptr,
  /** pointer to command buffer */
  byte **req_buf_ptr,
  /**length of the command buffer*/
  uint32 *req_buf_length,
  /**pointer to audiocalibration data from where we need to get gains Calunit data */
  voc_pcm_path_cal_type *pcm_cal_ptr
)
{
 acm_error_code error_code = ACM_ERR_NONE;
//  uint16                        tx_gain;         /* TX Voice Volume           */
//  uint16                        dtmf_tx_gain;    /* TX DTMF gain              */
//  uint16                        codec_tx_gain;   /* CODEC TX gain             */
//  uint16                        codec_rx_gain;   /* CODEC RX gain             */
//  uint16                        codec_st_gain;   /* CODEC ST gain             */
  /**
  *copy TX Voice Volume
  */
  error_code = acmapi_get_tx_gain(&pcm_cal_ptr->tx_gain, 
                                                          &gains_calunit_resp_buf_ptr
                                                        );
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("acm_get_gains_calunit: got tx_gain_data", 0,0,0);

  /**
  *copy TX DTMF gain
  */
  error_code = acmapi_get_tx_dtmf_gain(&pcm_cal_ptr->dtmf_tx_gain,
                                                                  &gains_calunit_resp_buf_ptr
                                                                  );
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("acm_get_gains_calunit: got tx_dtmf_gain_data", 0,0,0);

  /**
  *copy CODEC TX gain
  */
  error_code = acmapi_get_codec_tx_gain(&pcm_cal_ptr->codec_tx_gain,
                                                                    &gains_calunit_resp_buf_ptr
                                                                    );
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("acm_get_gains_calunit: got codec_tx_gain_data", 0,0,0);

  /**
  *copy CODEC RX gain
  */
  error_code = acmapi_get_codec_rx_gain(&pcm_cal_ptr->codec_rx_gain,
                                                                    &gains_calunit_resp_buf_ptr
                                                                    );
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("acm_get_gains_calunit: got codec_rx_gain_data", 0,0,0);

  /**
  *copy CODEC ST gain
  */
  error_code = acmapi_get_codec_st_gain(&pcm_cal_ptr->codec_st_gain,
                                                                    &gains_calunit_resp_buf_ptr
                                                                    );
  if ( error_code != ACM_ERR_NONE )
  {
    return error_code;
  }
  MSG_MED("acm_get_gains_calunit: got codec_st_gain_data", 0,0,0);
  
  return ACM_ERR_NONE;
}

/**
* FUNCTION acm_get_struct
*
* DESCRIPTION : finds to which structure data belongs to based on GUID and calls appropriate method. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_NONE if success
* CALUNIT_UNKNOWN if no matching guid found (from supported list).
*
* SIDE EFFECTS: None
*/
acm_error_code acm_get_struct
(
  byte *req_buf_ptr,
  uint32 *req_buf_length,
  byte **resp_buf_ptr,
  uint32 *resp_buf_length
)
{
  acm_calunit_enum calunit = CALUNIT_UNKNOWN;
  acm_guid guid;
  acm_cmd_struct_header header;
  byte *tmp_resp_buf_ptr = NULL;
  acm_get_calunit_size_ptr_type get_calunit_size_ptr = NULL ;
  acm_get_calunit_ptr_type  get_calunit_ptr = NULL;
  boolean filter_is_voc_type =  TRUE;  
  audio_filter_indices_struct audio_filter_indices;
  acm_error_code error_code = ACM_ERR_NONE;
  voc_pcm_path_cal_type *pcm_cal_ptr = NULL ;
  voc_filter_indices_struct voc_filter_indices;
  int index_count = 0 ;
  if( req_buf_ptr == NULL )
  {
    MSG_MED("acm_get_struct: req_buf_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
if ( *req_buf_length < ACM_GUID_SIZE)
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  if ( resp_buf_length == NULL )
  {
    MSG_MED("acm_get_struct: resp_buf_length is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  /*read CALUNIT GUID from buffer*/
  memcpy((void *)&guid,req_buf_ptr,ACM_GUID_SIZE);
  /*Get CALUNIT type*/
  calunit = acmapi_get_calunit_from_guid(&guid);
  req_buf_ptr +=ACM_GUID_SIZE;
  *req_buf_length -= ACM_GUID_SIZE;

  *resp_buf_length = 0;
  switch(calunit)
  {
    #ifdef FEATURE_IIR_FILTER
      case CALUNIT_IIR:
        MSG_MED("ACM: CMD_GET_STRUCT received for IIR", 0,0,0);
        /*assign IIR filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_iir_filter_calunit_size;
        /*assign function pointer to get IIR calunit*/
        get_calunit_ptr = acm_get_iir_calunit;
        index_count = 3 ;
      break;
    #endif
    #ifdef FEATURE_AVS_NEXTGEN_EC
      case CALUNIT_NEXTGEN_EC:
        MSG_MED("ACM: CMD_GET_STRUCT received for NEXTGEN_EC", 0,0,0);
        /*assign NextGen EC calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_nextgen_ec_calunit_size;
        /*assign function pointer to get NextGen EC calunit*/
        get_calunit_ptr = acm_get_nextgen_ec_calunit;
        index_count = 4 ;
      break;
    #endif
    #ifdef FEATURE_AUDFMT_IIR_FILTER
      case CALUNIT_IIR_PCM_TUNNING_FILTER:
        MSG_MED("ACM: CMD_GET_STRUCT received for AUDFMT_IIR_FILTER", 0,0,0);
          /*assign Audfmt IIR filter calunit size function pointer*/
          get_calunit_size_ptr = acmapi_get_iir_pcm_tuning_filter_calunit_size;
          /*assign function pointer to get Audfmt IIR calunit*/
          get_calunit_ptr = acm_get_audfmt_iir_calunit;
          index_count = 4 ;
      break;
    #endif
    case CALUNIT_FIR:
    MSG_MED("ACM: CMD_GET_STRUCT received for FIR", 0,0,0);
    /*assign FIR filter calunit size function pointer*/
    get_calunit_size_ptr = acmapi_get_fir_filter_calunit_size;
    /*assign function pointer to get FIR calunit*/
    get_calunit_ptr = acm_get_fir_calunit;
    index_count = 3 ;
    break;
    #ifdef FEATURE_AUDFMT_FIR_FILTER
    #ifdef FEATURE_QSYNTH_COMMON
      case CALUNIT_QSYNTH_AUDFMT_FIR_FILTER:
        MSG_MED("ACM: CMD_GET_STRUCT received for AUDFMT FIR", 0,0,0);
        /*assign Audfmt FIR filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_audfmt_qsynth_fir_calunit_size;
        /*assign function pointer to get FIR calunit*/
        get_calunit_ptr = acm_get_audfmt_qsynth_fir_calunit;
        index_count = 3 ;
      break;
    #endif/*FEATURE_QSYNTH_COMMON*/
    #ifdef FEATURE_QTUNES_COMMON
      case CALUNIT_QTUNES_AUDFMT_FIR_FILTER:
        MSG_MED("ACM: CMD_GET_STRUCT received for AUDFMT FIR", 0,0,0);
        /*assign Audfmt FIR filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_audfmt_qtunes_fir_calunit_size;
        /*assign function pointer to get FIR calunit*/
        get_calunit_ptr = acm_get_audfmt_qtunes_fir_calunit;
        index_count = 4 ;
      break;
    #endif/*FEATURE_QTUNES_COMMON*/
    #endif /*FEATURE_AUDFMT_FIR_FILTER*/
    case CALUNIT_ECHO_CANCELLER:
      MSG_MED("ACM: CMD_GET_STRUCT received for ECHO_CANCELLER", 0,0,0);
      /*assign ECHO_CANCELLER filter calunit size function pointer*/
      get_calunit_size_ptr = acmapi_get_echo_canceller_calunit_size;
      /*assign function pointer to get ECHO_CANCELLER calunit*/
      get_calunit_ptr = acm_get_ec_calunit;
      index_count = 4 ;
    break;
	case CALUNIT_RVE_PARAMS:
        MSG_MED("ACM: CMD_GET_STRUCT received for CALUNIT_RVE_PARAMS", 0,0,0);
        /*assign EAudio AGC filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_rve_parameters_calunit_size;
        /*assign function pointer to get ECHO_CANCELLER calunit*/
        get_calunit_ptr = acm_get_rve_parameters_calunit;
        index_count = 4 ;
    break;
    #ifdef FEATURE_AUDIO_AGC
      case CALUNIT_AUDIO_AGC:
        MSG_MED("ACM: CMD_GET_STRUCT received for AUDIO_AGC", 0,0,0);
        /*assign EAudio AGC filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_agc_filter_calunit_size;
        /*assign function pointer to get ECHO_CANCELLER calunit*/
        get_calunit_ptr = acm_get_audio_agc_calunit;
        index_count = 4 ;
      break;
    #endif
    #ifdef FEATURE_QSYNTH_ADRC
      case CALUNIT_QSYNTH_ADRC:
        MSG_MED("ACM: CMD_GET_STRUCT received for QSynth ADRC", 0,0,0);
        /*assign QSynth ADRC filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_qsynth_adrc_calunit_size;
        /*assign function pointer to get QSynth ADRC calunit*/
        get_calunit_ptr = acm_get_qsynth_adrc_calunit;
        index_count = 4 ;
      break;
    #endif/*FEATURE_QSYNTH_ADRC*/
    #ifdef FEATURE_QTUNES_ADRC
      case CALUNIT_QTUNES_ADRC:
        MSG_MED("ACM: CMD_GET_STRUCT received for QTunes ADRC", 0,0,0);
        /*assign QTunes ADRC filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_adrc_qtunes_filter_calunit_size;
        /*assign function pointer to get QTunes ADRC calunit*/
        get_calunit_ptr = acm_get_qtunes_adrc_calunit;
        index_count = 4 ;
      break;
    #endif/*FEATURE_QTUNES_ADRC*/
    #ifdef FEATURE_AVS_CRYSTAL_SPEECH
      case CALUNIT_AVS_CRYSTAL_SPEECH:
        MSG_MED("ACM: CMD_GET_STRUCT received for Crystal Speech", 0,0,0);
        /*assign Crystal Speech filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_crystal_speech_calunit_size;
        /*assign function pointer to get Crystal Speech calunit*/
        get_calunit_ptr = acm_get_crystal_speech_calunit;
        index_count = 4 ;
        break;
    #endif/*FEATURE_AVS_CRYSTAL_SPEECH*/
    #ifdef FEATURE_AUDFMT_STF
      case CALUNIT_SIDECHAIN_TUNNING_FILTER:
        MSG_MED("ACM: CMD_GET_STRUCT received for Sidechain Tuning Filter", 0,0,0);
        /*assign Sidechain Tuning Filter calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_side_chain_tuning_filter_calunit_size;
        /*assign function pointer to get Sidechain Tuning Filter calunit*/
        get_calunit_ptr = acm_get_audfmt_stf_calunit;
        index_count = 4 ;
        break;
    #endif /* FEATURE_AUDFMT_STF*/
    case CALUNIT_VOLUME_LEVELS:
      MSG_MED("ACM: CMD_GET_STRUCT received for Sound Calibration volume levels", 0,0,0);
      /*CALUNIT is not voc filter*/
      filter_is_voc_type = FALSE;
      index_count = 2 ;
    break;
	case CALUNIT_SLOW_TALK:
      MSG_MED("ACM: CMD_GET_STRUCT received for Slow Talk volume levels", 0,0,0);
      /*CALUNIT is not voc filter*/
      filter_is_voc_type = FALSE;
      index_count = 2 ;
    break;
    case CALUNIT_GAINS:
        MSG_MED("ACM: CMD_GET_STRUCT received for Gains", 0,0,0);
        /*assign gains calunit size function pointer*/
        get_calunit_size_ptr = acmapi_get_gains_calunit_size;
        /*assign function pointer to get Gains calunit*/
        get_calunit_ptr = acm_get_gains_calunit;
        index_count = 4 ;
      break;
    case CALUNIT_UNKNOWN:
      MSG_MED("ACM: CMD_GET_STRUCT received for Unknown Filter", 0,0,0);
      return ACM_ERR_UNKNOWN_CALUNIT;
  }

   /**
      * response buffer size = command buffer header size +
                                          CALUNIT size +
                                          indices size +
                                          GUId size
      */
  *resp_buf_length = 0 ;
  if ( filter_is_voc_type == TRUE )
  {
     /*get calunit size*/
     *resp_buf_length += (*get_calunit_size_ptr)();
     /*indices size*/
     *resp_buf_length += sizeof(voc_filter_indices_struct);
     if ( index_count > 3 )
    {
      *resp_buf_length += ( index_count-3 ) * sizeof(uint32);
    }
  }
  else
  {
    /*read indices*/
    if( *req_buf_length < sizeof(audio_filter_indices_struct))
    {
      return ACM_ERR_UNEXPECTED_BUF_SIZE;
    }
    memcpy(&audio_filter_indices, req_buf_ptr, sizeof(audio_filter_indices_struct));
	switch(calunit)
	{
		case CALUNIT_SLOW_TALK:
				 *resp_buf_length=sizeof(audio_filter_indices_struct)+sizeof(qdsp_cmd_slowtalk_parms_type);
			break;
		case CALUNIT_VOLUME_LEVELS:
				/**
				  * Calculate calunit size for sound filter
				  * to calculate volume level size we need to consider level data size. 
				  * Level data size varies based on generater
				  */
				error_code = acmapi_get_volume_levels_calunit_size(&audio_filter_indices , resp_buf_length);
				if ( error_code != ACM_ERR_NONE)
				{
				  return error_code;
				}
				/*indices size*/
			   *resp_buf_length += sizeof(audio_filter_indices_struct); 
		break;
	}
	
  }
  /*acm command buffer header size*/
  *resp_buf_length += sizeof(acm_cmd_struct_header);
  /*GUID size*/
  *resp_buf_length += ACM_GUID_SIZE;

  /*allocate memory*/
  *resp_buf_ptr = (byte *)malloc(*resp_buf_length);
  if ( resp_buf_ptr == NULL )
  {
    return ACM_ERR_UNKNOWN;
  }
  tmp_resp_buf_ptr = *resp_buf_ptr;
  /*prepare command buffer header*/
  header.cmd_id = CMD_SET_STRUCT;
  header.cmd_buf_length = *resp_buf_length - sizeof(acm_cmd_struct_header);
  /*copy header*/
  memcpy(tmp_resp_buf_ptr,(void *)&header,sizeof(acm_cmd_struct_header));
   tmp_resp_buf_ptr += sizeof(acm_cmd_struct_header);
  /*copy Calunit guid*/
  memcpy(tmp_resp_buf_ptr,(void *)&guid,ACM_GUID_SIZE);
  tmp_resp_buf_ptr += ACM_GUID_SIZE;
  if ( filter_is_voc_type == TRUE )
  {
    /*read indices from request buffer*/
    error_code = acm_read_voc_filter_indices(&req_buf_ptr,req_buf_length,&voc_filter_indices);
    if ( error_code != ACM_ERR_NONE )
    {
      return error_code;
    }
    /*copy the indices in to response buffer*/
    memcpy(tmp_resp_buf_ptr, (void *)&voc_filter_indices, sizeof(voc_filter_indices_struct));
    /*increment the response buffer pointer*/
    tmp_resp_buf_ptr += sizeof(voc_filter_indices_struct);
    /*get pcm_cal_ptr*/
     error_code = acm_get_pcm_cal_ptr(&voc_filter_indices, &pcm_cal_ptr);	
    if ( error_code != ACM_ERR_NONE )
    {
      return error_code;
    }
    return (*get_calunit_ptr)(tmp_resp_buf_ptr,&req_buf_ptr, req_buf_length, pcm_cal_ptr); 
  }
  else
  {
    /*get sound calibration calunit*/
	switch(calunit)
	{
		case CALUNIT_SLOW_TALK:
			return acm_get_slow_talk_calunit(req_buf_ptr, req_buf_length, tmp_resp_buf_ptr); 
		default:
			return acm_get_sound_calibration_calunit(req_buf_ptr, req_buf_length, tmp_resp_buf_ptr); 
	}
	
  }
  return ACM_ERR_UNKNOWN;
}

/**
* FUNCTION acm_set_struct
*
* DESCRIPTION : finds to which structure data belongs to based on GUID and calls appropriate method. 
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_NONE if success
* CALUNIT_UNKNOWN if no matching guid found (from supported list).
* 
* SIDE EFFECTS: None
 */
acm_error_code acm_set_struct
(
  byte *req_buf_ptr,
  uint32* req_buf_length,
  byte **resp_buf_ptr, 
  uint32 *resp_buf_length
)
{
  acm_calunit_enum calunit = CALUNIT_UNKNOWN;
  acm_guid guid;
  acm_set_calunit_ptr_type set_calunit_func_ptr = NULL;
  voc_filter_indices_struct voc_filter_indices;   
  voc_pcm_path_cal_type *pcm_cal_ptr = NULL ;
  acm_error_code error = ACM_ERR_NONE;

  if( req_buf_ptr == NULL )
  {
    MSG_MED("acm_set_struct: req_buf_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  
  if ( *req_buf_length < ACM_GUID_SIZE)
  {
    MSG_MED("ACM: Invalid buffer size in CMD_SET_STRUCT command buffer ", 0,0,0);
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  /*read calunit from buffer*/
  memcpy((void *)&guid,req_buf_ptr,ACM_GUID_SIZE);
  /*get calunit type*/
  calunit = acmapi_get_calunit_from_guid(&guid);
  req_buf_ptr +=ACM_GUID_SIZE;
  *req_buf_length -= ACM_GUID_SIZE;  
  
  switch(calunit)
  {
    #ifdef FEATURE_IIR_FILTER
      case CALUNIT_IIR:
        MSG_MED("ACM: CMD_SET_STRUCT received for IIR", 0,0,0);
        /*assign function pointer to set IIR calunit*/
        set_calunit_func_ptr = acm_set_iir_calunit;
      break;
    #endif/*FEATURE_IIR_FILTER*/
    #ifdef FEATURE_AVS_NEXTGEN_EC
    case CALUNIT_NEXTGEN_EC:
      MSG_MED("ACM: CMD_SET_STRUCT received for NEXTGEN_EC", 0,0,0);
      /*assign function pointer to set Nextgen EC calunit*/
      set_calunit_func_ptr = acm_set_nextgen_ec_calunit;
      break;
    #endif /*FEATURE_AVS_NEXTGEN_EC*/
    #ifdef FEATURE_AUDFMT_IIR_FILTER
      case CALUNIT_IIR_PCM_TUNNING_FILTER:
        MSG_MED("ACM: CMD_SET_STRUCT received for AUDFMT_IIR_FILTER", 0,0,0);
        /*assign function pointer to set audfmt IIR calunit*/
        set_calunit_func_ptr = acm_set_audfmt_iir_calunit;
      break;
    #endif/*FEATURE_AUDFMT_IIR_FILTER*/
    case CALUNIT_FIR:
      MSG_MED("ACM: CMD_SET_STRUCT received for FIR", 0,0,0);
      /*assign function pointer to set FIR calunit*/
      set_calunit_func_ptr = acm_set_fir_calunit;
    break;

    #ifdef FEATURE_AUDFMT_FIR_FILTER
    #ifdef FEATURE_QSYNTH_COMMON
      case CALUNIT_QSYNTH_AUDFMT_FIR_FILTER:
        MSG_MED("ACM: CMD_SET_STRUCT received for QSynth FIR", 0,0,0);
        /*assign function pointer to set audfmt FIR calunit*/
        set_calunit_func_ptr = acm_set_audfmt_qsynth_fir_calunit;
      break;
    #endif/*FEATURE_QSYNTH_COMMON*/
    #ifdef FEATURE_QTUNES_COMMON
      case CALUNIT_QTUNES_AUDFMT_FIR_FILTER:
        MSG_MED("ACM: CMD_SET_STRUCT received for QTunes FIR", 0,0,0);
        /*assign function pointer to set audfmt FIR calunit*/
        set_calunit_func_ptr = acm_set_audfmt_qtunes_fir_calunit;
      break;
    #endif/*FEATURE_QTUNES_COMMON*/
    #endif/*FEATURE_AUDFMT_FIR_FILTER*/
    case CALUNIT_ECHO_CANCELLER:
      MSG_MED("ACM: CMD_SET_STRUCT received for EC", 0,0,0);
      /*assign function pointer to set Echo Canceller calunit*/
      set_calunit_func_ptr = acm_set_ec_calunit;
    break;
	case CALUNIT_RVE_PARAMS:
      MSG_MED("ACM: CMD_SET_STRUCT received for EC", 0,0,0);
      /*assign function pointer to set Echo Canceller calunit*/
      set_calunit_func_ptr = acm_set_rve_parameters_calunit;
    break;
    #ifdef FEATURE_AUDIO_AGC
      case CALUNIT_AUDIO_AGC:
        MSG_MED("ACM: CMD_SET_STRUCT received forAudio AGC", 0,0,0);
        /*assign function pointer to set audio AGC calunit*/
        set_calunit_func_ptr = acm_set_audio_agc_calunit;
      break;
    #endif /*FEATURE_AUDIO_AGC*/
    #ifdef FEATURE_QSYNTH_ADRC
    case CALUNIT_QSYNTH_ADRC:
      MSG_MED("ACM: CMD_SET_STRUCT received for QSynth ADRC", 0,0,0);
      /*assign function pointer to set QSynth ADRC calunit*/
      set_calunit_func_ptr = acm_set_qsynth_adrc_calunit;
      break;
    #endif/*FEATURE_QSYNTH_ADRC*/
    #ifdef FEATURE_QTUNES_ADRC
    case CALUNIT_QTUNES_ADRC:
      MSG_MED("ACM: CMD_SET_STRUCT received for QTunes ADRC", 0,0,0);
      /*assign function pointer to set Qtunes ADRC calunit*/
      set_calunit_func_ptr = acm_set_qtunes_adrc_calunit;
      break;
    #endif/*FEATURE_QTUNES_ADRC*/
    #ifdef FEATURE_AVS_CRYSTAL_SPEECH
      case CALUNIT_AVS_CRYSTAL_SPEECH:
        MSG_MED("ACM: CMD_SET_STRUCT received for Crystal Speech", 0,0,0);
        /*assign function pointer to set crystal speech calunit*/
        set_calunit_func_ptr = acm_set_crystal_speech_calunit;
      break;
    #endif/*FEATURE_AVS_CRYSTAL_SPEECH*/
    #ifdef FEATURE_AUDFMT_STF
      case CALUNIT_SIDECHAIN_TUNNING_FILTER:
        MSG_MED("ACM: CMD_SET_STRUCT received for Sidechain Tuning Filter", 0,0,0);
        /*assign function pointer to set sidechain tuning filter calunit*/
        set_calunit_func_ptr = acm_set_audfmt_stf_calunit;
      break;
    #endif /* FEATURE_AUDFMT_STF*/
    case CALUNIT_GAINS:
      MSG_MED("ACM: CMD_SET_STRUCT received for Gains", 0,0,0);
      /*assign function pointer to set Gains calunit*/
      set_calunit_func_ptr = acm_set_gains_calunit;
    break;
    case CALUNIT_VOLUME_LEVELS:
      MSG_MED("ACM: CMD_SET_STRUCT received for Sound calibration volume levels Filter", 0,0,0);
      /*assign function pointer to set voluen levels*/
      return acm_set_sound_calibration_calunit(req_buf_ptr,req_buf_length);
    break;
	case CALUNIT_SLOW_TALK:
      MSG_MED("ACM: CMD_SET_STRUCT received for slow talk levels Filter", 0,0,0);
      /*assign function pointer to set voluen levels*/
      return acm_set_slow_talk_calunit(req_buf_ptr,req_buf_length);
    break;
    case CALUNIT_UNKNOWN:
      MSG_MED("ACM: CMD_SET_STRUCT received for Unknown Filter ", 0,0,0);
      return ACM_ERR_UNKNOWN_CALUNIT;
  } 
  /*if function pointer is NULL return error*/
  if(set_calunit_func_ptr == NULL )
  {    
    MSG_MED("acm_set_struct: set_calunit_func_ptr is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  
  /*read voice filter indices from buffer*/
  error = acm_read_voc_filter_indices(&req_buf_ptr,req_buf_length, &voc_filter_indices);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  /*get pcm_path_cal pointer*/
  error = acm_get_pcm_cal_ptr(&voc_filter_indices, &pcm_cal_ptr);
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  /*set calunit*/
  error = (*set_calunit_func_ptr)(req_buf_ptr,req_buf_length,pcm_cal_ptr);
  return error;
}

/**
* FUNCTION acm_set_calfile_to_sfs
*
* DESCRIPTION : Write a calibration file to SFS file system.
*
* DEPENDENCIES: None 
*
* RETURN VALUE: acm_error_code
* ACM_ERR_NONE if success
* 
* SIDE EFFECTS: None
*/
acm_error_code acm_set_calfile_to_sfs
(
  byte *req_buf_ptr,
  uint32* req_buf_length,
  byte **resp_buf_ptr,
  uint32 *resp_buf_length
)
{/*
  int fd = NULL ;
  int bytes_written = 0 ;
  acm_error_code error = ACM_ERR_NONE;
  acm_guid guid;
  memcpy((void *)&guid,req_buf_ptr,ACM_GUID_SIZE);
  req_buf_ptr +=ACM_GUID_SIZE;
  *req_buf_length -= ACM_GUID_SIZE;
  error = acmapi_check_build_guid(&guid);  
  if ( error != ACM_ERR_NONE )
  {
    return error;
  }
  sfs_rm(caldb_file_name);
  fd = sfs_open(caldb_file_name,ACM_SFS_CREAT | ACM_SFS_WRONLY);
  bytes_written = sfs_write(fd, (const char *)req_buf_ptr,*req_buf_length);
  MSG_MED("acm_set_calfile_to_sfs: No. of bytes written: %d", bytes_written,0,0);
  sfs_close(fd);
  return error;
  */
  return ACM_ERR_NONE;
}

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
 void acm_receive_req_buffer
(
  /**pointer to request buffer*/
  byte *req_buf_ptr,
  /** request buffer length */
  uint32 req_buf_length,
  /**pointer to response buffer*/
  byte **resp_buf_ptr,
  /**length of the response buffer*/
  uint32 *resp_buf_length
)
{
  uint32 tmp_req_buffer_len =0;
  byte *tmp_req_buf_ptr = req_buf_ptr;
  acm_error_code retval = ACM_ERR_UNKNOWN;
  acm_cmd_struct_header header ;

  if( req_buf_ptr == NULL )
  {
    MSG_MED("receive_req_buffer: req_buf_ptr is NULL", 0,0,0);
    retval = ACM_ERR_UNKNOWN;
  }
  else if( resp_buf_ptr == NULL )
  {
    MSG_MED("receive_req_buffer: resp_buf_ptr is NULL", 0,0,0);
    retval = ACM_ERR_UNKNOWN;
  }
  else
  {
    *resp_buf_ptr = NULL;
    if ( req_buf_length > sizeof(acm_cmd_struct_header) )
    {
      MSG_MED("ACM: Received command buffer", 0,0,0);
      memcpy((void *)&header,tmp_req_buf_ptr,sizeof(acm_cmd_struct_header));

      /*Increment buffer pointer by header length*/
      tmp_req_buf_ptr += sizeof(acm_cmd_struct_header);
      tmp_req_buffer_len = req_buf_length - sizeof(acm_cmd_struct_header);
    
      /*switch to ACM Command*/
      switch(header.cmd_id)
      {
      case CMD_GET_STRUCT: /*Requesting a Calunit data*/
        retval = acm_get_struct(tmp_req_buf_ptr,&tmp_req_buffer_len,resp_buf_ptr,resp_buf_length);
      break;
      case CMD_SET_STRUCT: /*trying to set a Calunit data*/
        retval = acm_set_struct(tmp_req_buf_ptr,&tmp_req_buffer_len,resp_buf_ptr,resp_buf_length);
        /*Incase of Set struct, if we set data successfuly we need return 
        Command Result as success*/
        if ( retval == ACM_ERR_NONE )
          acm_get_error_struct(resp_buf_ptr, resp_buf_length,TRUE, ACM_ERR_NONE);
        break;
      case CMD_GET_FEATURE_INFO:
        retval = acmapi_get_feature_info(tmp_req_buf_ptr,&tmp_req_buffer_len,resp_buf_ptr,resp_buf_length);
      break;
      case CMD_SET_CALFILE_TO_SFS:
        retval = acm_set_calfile_to_sfs(tmp_req_buf_ptr,&tmp_req_buffer_len,resp_buf_ptr,resp_buf_length);
      break;
      default:
        MSG_MED("ACM: Unknown Command Code in command buffer", 0,0,0);
        retval = ACM_ERR_INVALID_CMD_ID;
      }
    }
    else
    {
      MSG_MED("ACM: Invalid command buffer size", 0,0,0);
      retval = ACM_ERR_UNEXPECTED_BUF_SIZE ;
    }
  }
  /*CLOCKWORKS FIX*/
  if(resp_buf_ptr!=NULL)/* This is just to fix clockworks error)*/
  {
	  /*If we have any errors send error code in response buffer*/
	  if ( retval != ACM_ERR_NONE)
	  {
		if ( *resp_buf_ptr != NULL )
		{
		  free(*resp_buf_ptr);
		  *resp_buf_ptr = NULL;
		}
		MSG_MED("ACM: Sending error response buffer. Error code%d", retval,0,0); 
		acm_get_error_struct(resp_buf_ptr, resp_buf_length,FALSE, retval);
	  }
  }
}

#endif /*FEATURE_AVS_DYNAMIC_CALIBRATION*/

