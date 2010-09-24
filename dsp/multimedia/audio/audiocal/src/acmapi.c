
    /** 
\file ***************************************************************************
*
*                                    A C M    A P I
*
*DESCRIPTION
* This file implements functions required to get/set data into calibration data 
tables in.voccal.c/sndcal.c.
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
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audiocal/QSC1100/main/latest/src/acmapi.c#9 $ $DateTime: 2009/05/29 04:43:52 $ $Author: prashant $
*
*when         who     what, where, why<br>
*--------   ---     ----------------------------------------------------------<br>
*01/25/08   mas    created  file<br>
*
*******************************************************************************
*/

/*
      --------------------
      |include files     |
      --------------------
*/

#include <stdlib.h>  /* needed for malloc() and free()*/
#include <string.h>  /* needed for memcpy, memset, and memmove*/
#include "acmapi.h"
#include "voc.h"
#define MSG_MED(a,b,c,d)  
#ifdef FEATURE_AVS_DYNAMIC_CALIBRATION
    
acm_guid build_guid = {0xE42CCE6E,0x1D5B,0x45d5,{0x95,0x33,0xF9,0x42,0x89,0xAE,0xCE,0x7A}}; 

acm_guid calunitlist[CALUNIT_MAX]= { 

  /* Sound Calibration*/
  {0x48A19AC4,0xC9CA,0x4082,{0xB1,0x1A,0xE8,0x2F,0xB1,0x1A,0x2E,0x05}},
  /* Slow Talk*/
  {0xE0E83AC6,0xF78B,0x46df,{0x88,0x9E,0x93,0xD4,0xCE,0x9B,0xB9,0x30}},
  /* Gains*/
  {0x576932A5,0x6C68,0x41E1,{0xAE,0x52,0xA3,0x0D,0x19,0x7D,0xE5,0xD7}},
  /* FIR FILTER*/
  {0x44963455,0x68CD,0x44fb,{0x94,0x47,0xE8,0x0B,0x68,0xF6,0x38,0xF8}},
  /* IIR FILTER*/
  {0xCFC293E7,0xF395,0x46c8,{0xA4,0xFE,0x72,0x4C,0x33,0xEB,0xDB,0xA0}},
  /* AGC FILTER*/
  {0x6D74384B,0x2840,0x453a,{0xB3,0x25,0x4B,0x90,0xF6,0x4E,0x77,0xE6}},
  /* ECHO CANCELLER*/
  {0xB3FAB128,0xA41E,0x42b7,{0x80,0x30,0x63,0x0B,0x4C,0x4D,0x3A,0x1A}},
  /* RVE PARAMETERS*/
  {0x020A398A,0xCE1C,0x4768,{0xAA,0xDE,0xB3,0x8D,0x41,0xAD,0xE0,0xFE}},
  /* NEXTGEN EC*/
  {0x6E2AD721,0x31B0,0x4a4b,{0xA0,0x51,0xA1,0xB0,0x50,0x89,0x6F,0xD1}},
  /* ADRC QSYNTH FILTER*/
  {0xBC14883B,0x7967,0x4712,{0x90,0xF9,0xF6,0x6E,0xCB,0x57,0x9C,0xD2}},
  /* ADRC QTUNES FILTER*/
  {0x8A158E13,0x8993,0x4ca8,{0xB0,0x1B,0xF9,0x80,0x06,0xFF,0x17,0x89}},
  /* SIDE CHAIN TUNING FILTER*/
  {0x33556EFB,0x778D,0x4454,{0x98,0x6D,0xCE,0x94,0x31,0xF3,0xA9,0x29}},
  /* IIR PCM TUNING FILTER*/
  {0xBCAE88C4,0x6334,0x4b98,{0xA0,0x7D,0x7A,0x02,0x2E,0xAF,0xAF,0xAC}},
  /* FIR PCM QSYNTH FILTER*/
  {0x3C6FC7CA,0x0698,0x48d1,{0x86,0xE5,0xC9,0xF2,0x91,0xA5,0xB5,0x48}},
  /* FIR PCM QTUNES FILTER*/
  {0xA221E572,0x16A1,0x4a74,{0xB1,0x49,0xBA,0xD3,0x1A,0xF5,0x63,0x3C}},
  /* CRYSTAL SPEECH*/
  {0x2D18DC77,0x03BA,0x4699,{0x96,0xBC,0x62,0x06,0x68,0x2B,0x63,0xFD}}
}; 

     
#ifdef FEATURE_AUDFMT_IIR_FILTER
  /* Audio format IIR turning filter
  **
  ** The maximum filter is 4
  **This is defined in voccal.c. since it is not externized, redifined here.
  */
  #define VOC_CAL_AUDFMT_IIR_FILTER_NUM 4
#endif
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
acm_calunit_enum acmapi_get_calunit_from_guid
(
  acm_guid *guid
)
{
  /*initialize calunit*/
  acm_calunit_enum calunit = CALUNIT_UNKNOWN;
  int i=0;
  if ( guid == NULL )
  {
    return calunit;
  }
  /*try to match with calunit list we have*/
  for(i=0;i<CALUNIT_MAX;i++)
  {
    if( memcmp((void *)&calunitlist[i],(void *)guid,ACM_GUID_SIZE) == 0)
    {
      /*found. break here.*/
      calunit = (acm_calunit_enum) i;
      break;
    }
  }
  return calunit;
}

/*=============================================================
 FUNCTION acmapi_check_build_guid

 DESCRIPTION : Helper function to compares a build guid with the given guid. 

 DEPENDENCIES: None 

 RETURN VALUE: acm_error_code
ACM_ERR_NONE if success
 ACM_ERR_CALFILE_MISMATCH if build guid doesn't matches
 
 SIDE EFFECTS: None
 ============================================================*/
acm_error_code acmapi_check_build_guid
(
  acm_guid *guid
)
{
  if ( guid == NULL )
  {
    return ACM_ERR_UNKNOWN;
  }
  /*Compare with build guid. */
  if( memcmp((void *)&build_guid,(void *)guid,ACM_GUID_SIZE) == 0 )
  {
    return ACM_ERR_NONE;
  }
  else
  {
    return ACM_ERR_CALFILE_MISMATCH;
  }
}


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
acm_error_code acmapi_get_feature_info
(
  byte *req_buf_ptr,
  uint32* req_buf_length,
  byte **resp_buf_ptr,
  uint32 *resp_buf_length
)
{
  int i=0;
  /*  
    * This is count of various FEATURES used in audio calibration database.
    * List is taken manually from audio calibration file, which is inturn taken 
    * from voccal.c/h and sndcal.c/h
    */
  int feature_count = 60;
  byte *feature_buf_ptr;
  acm_guid guid;
  acm_error_code retval = ACM_ERR_UNKNOWN;
  acm_cmd_struct_header cmd_header;
  memcpy((void *)&guid,req_buf_ptr,ACM_GUID_SIZE);
  req_buf_ptr +=ACM_GUID_SIZE;
  *req_buf_length -= ACM_GUID_SIZE;
  retval = acmapi_check_build_guid(&guid);
  if ( retval != ACM_ERR_NONE )
  {
    return retval;
  }
  /*Calculate command buffer size. one byte will be used to represent if a 
     feature is enabled/disbled*/
  *resp_buf_length = sizeof(byte)*feature_count + sizeof(acm_cmd_struct_header);
  /*allocate memory and set header information*/
  feature_buf_ptr = (byte *)malloc(*resp_buf_length);
  cmd_header.cmd_buf_length = sizeof(byte)*feature_count;
  cmd_header.cmd_id = CMD_SET_FEATURE_INFO;
  memcpy(feature_buf_ptr,(void *)&cmd_header,sizeof(acm_cmd_struct_header));
  /*calculate offset*/
  i = sizeof(acm_cmd_struct_header); 
    
#ifdef FEATURE_ANALOG_HFK
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDFMT_FIR_FILTER
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDFMT_IIR_FILTER
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDFMT_STF
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDIO_AGC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDIO_FORMAT
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AVS_I2SSBC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AVS_NEXTGEN_EC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_BT
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_BT_AG
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_BT_HS
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_CLICK
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_EXTERNAL_SADC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_GSM
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_IIR_FILTER
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_INTERNAL_SADC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_MIDI_OUT
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_MM_REC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_QAUDIOFX
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_QSYNTH_ADRC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_QSYNTH_COMMON
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_QTUNES_ADRC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_QTUNES_COMMON
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_SBC_CODEC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_SBC_DSP_CODEC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_SPEAKER_PHONE
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_TTY
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_TTY_UMTS
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_USB_CARKIT
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_WCDMA
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_ADIE_CODEC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_EXTERNAL_SDAC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_INTERNAL_SDAC
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_6550_FFA_AUDIO_CAL
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_6280_FFA_AUDIO_CAL
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_6500_FFA_AUDIO_CAL
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_QSC60X0_AUDIO_CAL
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_UMTS_FFA
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_SURF_5200
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_FFA
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_TTP
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef T_FFA
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDIO_CAL_MED_HIGH_FFA
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef T_MSM5200
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef T_MSM5105
    feature_buf_ptr[i++] = TRUE;
#else
#error code not present
#endif 
    
#ifdef FEATURE_AUDIO_CAL_HIGH
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AUDIO_CAL_MED_HIGH
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_6260_FFA_AUDIO_CAL
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_QSC60X5_AUDIO_CAL
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_6800_FFA_AUDIO_CAL
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_6800_SURF_AUDIO_CAL
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_IIR_FILTER_5S
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_QSC6270_AUDIO_CAL
#error code not present
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
#ifdef MSMAUD_QSC1110_AUDIO_CAL
    feature_buf_ptr[i++] = TRUE;
#else
    feature_buf_ptr[i++] = FALSE;
#endif 
    
       *resp_buf_ptr = feature_buf_ptr;
    return ACM_ERR_NONE;
} 
    
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
uint32 acmapi_get_tx_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_pcm_filter_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)pcm_filter_ptr, sizeof(qdsp_cmd_pcm_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_filter_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_pcm_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(pcm_filter_ptr, *buf_ptr, sizeof(qdsp_cmd_pcm_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_filter_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_pcm_filter_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_rx_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_pcm_filter_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)pcm_filter_ptr, sizeof(qdsp_cmd_pcm_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_filter_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_pcm_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(pcm_filter_ptr, *buf_ptr, sizeof(qdsp_cmd_pcm_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_filter_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_pcm_filter_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_fir_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_tx_filter_size();
  req_buf_length += acmapi_get_rx_filter_size();
  return req_buf_length;
} 

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
uint32 acmapi_get_tx_iir_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)pcm_iir_filter_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_pcm_iir_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(pcm_iir_filter_ptr, *buf_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_rx_iir_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)pcm_iir_filter_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_pcm_iir_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(pcm_iir_filter_ptr, *buf_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_tx_iir_filter5_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)pcm_iir_filter_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_pcm_iir_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(pcm_iir_filter_ptr, *buf_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_rx_iir_filter5_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)pcm_iir_filter_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_pcm_iir_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(pcm_iir_filter_ptr, *buf_ptr, sizeof(qdsp_cmd_pcm_iir_filter_type));
  *buf_ptr += sizeof(qdsp_cmd_pcm_iir_filter_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_pcm_iir_filter_type);
  return ACM_ERR_NONE;
} 
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
uint32 acmapi_get_iir_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_tx_iir_filter_size();
  req_buf_length += acmapi_get_rx_iir_filter_size();
#ifdef FEATURE_IIR_FILTER_5S
  req_buf_length += acmapi_get_tx_iir_filter5_size();
#endif /* FEATURE_IIR_FILTER_5S */
#ifdef FEATURE_IIR_FILTER_5S
  req_buf_length += acmapi_get_rx_iir_filter5_size();
#endif /* FEATURE_IIR_FILTER_5S */
  return req_buf_length;
} 
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
uint32 acmapi_get_agc_param_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_agc_param_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)agc_param_ptr, sizeof(qdsp_cmd_agc_param_type));
  *buf_ptr += sizeof(qdsp_cmd_agc_param_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_agc_param_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(agc_param_ptr, *buf_ptr, sizeof(qdsp_cmd_agc_param_type));
  *buf_ptr += sizeof(qdsp_cmd_agc_param_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_agc_param_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_agc_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_agc_param_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_ec_params_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_ec_params_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)ec_params_ptr, sizeof(qdsp_cmd_ec_params_type));
  *buf_ptr += sizeof(qdsp_cmd_ec_params_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_ec_params_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(ec_params_ptr, *buf_ptr, sizeof(qdsp_cmd_ec_params_type));
  *buf_ptr += sizeof(qdsp_cmd_ec_params_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_ec_params_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_echo_canceller_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_ec_params_size();
  return req_buf_length;
} 

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
uint32 acmapi_get_rve_param_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_rve_param_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)rve_param_ptr, sizeof(qdsp_cmd_rve_param_type));
  *buf_ptr += sizeof(qdsp_cmd_rve_param_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_rve_param_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(rve_param_ptr, *buf_ptr, sizeof(qdsp_cmd_rve_param_type));
  *buf_ptr += sizeof(qdsp_cmd_rve_param_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_rve_param_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_dual_rve_param_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_dual_rve_param_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)dual_rve_param_ptr, sizeof(qdsp_cmd_dual_rve_param_type));
  *buf_ptr += sizeof(qdsp_cmd_dual_rve_param_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_dual_rve_param_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(dual_rve_param_ptr, *buf_ptr, sizeof(qdsp_cmd_dual_rve_param_type));
  *buf_ptr += sizeof(qdsp_cmd_dual_rve_param_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_dual_rve_param_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_rve_parameters_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_rve_param_size();
  req_buf_length += acmapi_get_dual_rve_param_size();
  return req_buf_length;
} 

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
uint32 acmapi_get_nextgen_ec_parms_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_nextgen_ec_parms_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)nextgen_ec_parms_ptr, sizeof(qdsp_cmd_nextgen_ec_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_nextgen_ec_parms_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_nextgen_ec_parms_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(nextgen_ec_parms_ptr, *buf_ptr, sizeof(qdsp_cmd_nextgen_ec_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_nextgen_ec_parms_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_nextgen_ec_parms_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_nextgen_ec_wb_parms_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_nextgen_ec_parms_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)nextgen_ec_parms_ptr, sizeof(qdsp_cmd_nextgen_ec_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_nextgen_ec_parms_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_nextgen_ec_parms_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(nextgen_ec_parms_ptr, *buf_ptr, sizeof(qdsp_cmd_nextgen_ec_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_nextgen_ec_parms_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_nextgen_ec_parms_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_nextgen_ec_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_nextgen_ec_parms_size();
  req_buf_length += acmapi_get_nextgen_ec_wb_parms_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_qsynth_adrc_ctl_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)audfmt_adrc_ctl_ptr, sizeof(qdsp_cmd_audfmt_adrc_ctl_type));
  *buf_ptr += sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_audfmt_adrc_ctl_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(audfmt_adrc_ctl_ptr, *buf_ptr, sizeof(qdsp_cmd_audfmt_adrc_ctl_type));
  *buf_ptr += sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_adrc_qsynth_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_qsynth_adrc_ctl_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_qtunes_adrc_ctl_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)audfmt_adrc_ctl_ptr, sizeof(qdsp_cmd_audfmt_adrc_ctl_type));
  *buf_ptr += sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_audfmt_adrc_ctl_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(audfmt_adrc_ctl_ptr, *buf_ptr, sizeof(qdsp_cmd_audfmt_adrc_ctl_type));
  *buf_ptr += sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_audfmt_adrc_ctl_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_adrc_qtunes_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_qtunes_adrc_ctl_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_audfmt_sidechain_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(boolean);
  req_buf_length += sizeof(boolean);  /* add padding byte */
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16) * 48; 
  return req_buf_length;
} 

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
)
{ 

  uint32 coeff_size = 0;
  memcpy(*buf_ptr, (void *)&audfmt_sidechain_filter_ptr->enable, sizeof(boolean));
  *buf_ptr += sizeof(boolean) + sizeof(boolean);
  memcpy(*buf_ptr, (void *)&audfmt_sidechain_filter_ptr->mainChainGain, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  memcpy(*buf_ptr, (void *)&audfmt_sidechain_filter_ptr->sideChainGain, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  memcpy(*buf_ptr, (void *)&audfmt_sidechain_filter_ptr->bands, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  coeff_size = sizeof(uint16) * 48; 
  memcpy(*buf_ptr, (void *)audfmt_sidechain_filter_ptr->coeff, coeff_size);
  *buf_ptr += coeff_size;
  return ACM_ERR_NONE;
} 

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
)
{ 

  uint32 coeff_size = 0;
  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_audfmt_sidechain_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }

  memcpy((void *)&audfmt_sidechain_filter_ptr->enable, *buf_ptr, sizeof(boolean));
  *buf_ptr += sizeof(boolean) + sizeof(boolean);
  *remaining_buf_length_ptr -= sizeof(boolean) + sizeof(boolean);
  memcpy((void *)&audfmt_sidechain_filter_ptr->mainChainGain, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  memcpy((void *)&audfmt_sidechain_filter_ptr->sideChainGain, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  memcpy((void *)&audfmt_sidechain_filter_ptr->bands, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  coeff_size = sizeof(uint16) * 48; 
  memcpy((void *)audfmt_sidechain_filter_ptr->coeff, *buf_ptr, coeff_size);
  *buf_ptr += coeff_size;
  *remaining_buf_length_ptr -= coeff_size;
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_side_chain_tuning_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_audfmt_sidechain_filter_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_audfmt_iir_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(boolean);
  req_buf_length += sizeof(boolean);  /* add padding byte */
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16) * 48; 
  return req_buf_length;
} 

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
)
{ 

  uint32 coeff_size = 0;
  memcpy(*buf_ptr, (void *)&audfmt_iir_filter_ptr->enable, sizeof(boolean));
  *buf_ptr += sizeof(boolean) + sizeof(boolean);
  memcpy(*buf_ptr, (void *)&audfmt_iir_filter_ptr->bands, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  coeff_size = sizeof(uint16) * 48; 
  memcpy(*buf_ptr, (void *)audfmt_iir_filter_ptr->coeff, coeff_size);
  *buf_ptr += coeff_size;
  return ACM_ERR_NONE;
} 

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
)
{ 

  uint32 coeff_size = 0;
  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_audfmt_iir_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }

  memcpy((void *)&audfmt_iir_filter_ptr->enable, *buf_ptr, sizeof(boolean));
  *buf_ptr += sizeof(boolean) + sizeof(boolean);
  *remaining_buf_length_ptr -= sizeof(boolean) + sizeof(boolean);
  memcpy((void *)&audfmt_iir_filter_ptr->bands, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  coeff_size = sizeof(uint16) * 48; 
  memcpy((void *)audfmt_iir_filter_ptr->coeff, *buf_ptr, coeff_size);
  *buf_ptr += coeff_size;
  *remaining_buf_length_ptr -= coeff_size;
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_iir_pcm_tuning_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_audfmt_iir_filter_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_qsynth_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16) * 128; 
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16);
  return req_buf_length;
} 

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
)
{ 

  uint32 coeff_size = 0;
  memcpy(*buf_ptr, (void *)&audfmt_pcm_filter_ptr->flag, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  coeff_size = sizeof(uint16) * 128; 
  memcpy(*buf_ptr, (void *)audfmt_pcm_filter_ptr->coeff, coeff_size);
  *buf_ptr += coeff_size;
  memcpy(*buf_ptr, (void *)&audfmt_pcm_filter_ptr->length, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  memcpy(*buf_ptr, (void *)&audfmt_pcm_filter_ptr->factor, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  return ACM_ERR_NONE;
} 

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
)
{ 

  uint32 coeff_size = 0;
  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_audfmt_pcm_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }

  memcpy((void *)&audfmt_pcm_filter_ptr->flag, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  coeff_size = sizeof(uint16) * 128; 
  memcpy((void *)audfmt_pcm_filter_ptr->coeff, *buf_ptr, coeff_size);
  *buf_ptr += coeff_size;
  *remaining_buf_length_ptr -= coeff_size;
  memcpy((void *)&audfmt_pcm_filter_ptr->length, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  memcpy((void *)&audfmt_pcm_filter_ptr->factor, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_fir_pcm_qsynth_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_qsynth_filter_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_qtunes_filter_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16) * 128; 
  req_buf_length += sizeof(uint16);
  req_buf_length += sizeof(uint16);
  return req_buf_length;
} 

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
)
{ 

  uint32 coeff_size = 0;
  memcpy(*buf_ptr, (void *)&audfmt_pcm_filter_ptr->flag, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  coeff_size = sizeof(uint16) * 128; 
  memcpy(*buf_ptr, (void *)audfmt_pcm_filter_ptr->coeff, coeff_size);
  *buf_ptr += coeff_size;
  memcpy(*buf_ptr, (void *)&audfmt_pcm_filter_ptr->length, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  memcpy(*buf_ptr, (void *)&audfmt_pcm_filter_ptr->factor, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  return ACM_ERR_NONE;
} 

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
)
{ 

  uint32 coeff_size = 0;
  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_audfmt_pcm_filter_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }

  memcpy((void *)&audfmt_pcm_filter_ptr->flag, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  coeff_size = sizeof(uint16) * 128; 
  memcpy((void *)audfmt_pcm_filter_ptr->coeff, *buf_ptr, coeff_size);
  *buf_ptr += coeff_size;
  *remaining_buf_length_ptr -= coeff_size;
  memcpy((void *)&audfmt_pcm_filter_ptr->length, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  memcpy((void *)&audfmt_pcm_filter_ptr->factor, *buf_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -= sizeof(uint16);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_fir_pcm_qtunes_filter_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_qtunes_filter_size();
  return req_buf_length;
} 
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
uint32 acmapi_get_cs_parms_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_crystal_speech_parms_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)crystal_speech_parms_ptr, sizeof(qdsp_cmd_crystal_speech_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_crystal_speech_parms_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_crystal_speech_parms_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(crystal_speech_parms_ptr, *buf_ptr, sizeof(qdsp_cmd_crystal_speech_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_crystal_speech_parms_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_crystal_speech_parms_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_cs_wb_parms_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(qdsp_cmd_crystal_speech_parms_type);
  return req_buf_length;
} 

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
)
{ 
  memcpy(*buf_ptr, (void *)crystal_speech_parms_ptr, sizeof(qdsp_cmd_crystal_speech_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_crystal_speech_parms_type);
  return ACM_ERR_NONE;
} 

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
)
{ 

  if( *remaining_buf_length_ptr < sizeof(qdsp_cmd_crystal_speech_parms_type))
  {
    return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  memcpy(crystal_speech_parms_ptr, *buf_ptr, sizeof(qdsp_cmd_crystal_speech_parms_type));
  *buf_ptr += sizeof(qdsp_cmd_crystal_speech_parms_type);
  *remaining_buf_length_ptr -= sizeof(qdsp_cmd_crystal_speech_parms_type);
  return ACM_ERR_NONE;
} 

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
uint32 acmapi_get_crystal_speech_calunit_size(void)
{ 

  uint32 req_buf_length = 0 ;
  req_buf_length += acmapi_get_cs_parms_size();
  req_buf_length += acmapi_get_cs_wb_parms_size();
  return req_buf_length;
} 
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
acm_error_code acmapi_get_volume_levels_calunit_size
(
  audio_filter_indices_struct *audio_filter_indices_ptr,
  uint32 *volume_level_size
)
{
  snd_cal_control_type *audio_config_ptr;
  snd_cal_return_type retVal;
  snd_gen_cal_type *gen_cal_ptr;

  *volume_level_size = 0 ;
  retVal = snd_cal_get_audio_control((snd_device_type)audio_filter_indices_ptr->snd_device,
                                        (snd_method_type)audio_filter_indices_ptr->snd_method,&audio_config_ptr);

  if ( retVal == SND_CAL_FAILED || audio_config_ptr == NULL)
  {
    MSG_MED("acmapi_get_volume_levels_calunit_size: snd_cal_get_audio_control failed", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  /*default size for all snd_gen_type s*/
  *volume_level_size += sizeof(uint32); /*device_vol*/
  *volume_level_size += sizeof(uint32); /*generator*/
  *volume_level_size += sizeof(uint32); /*generator*/
  *volume_level_size += sizeof(uint16); /*num_levels*/
  *volume_level_size += sizeof(uint16); /*num_scale*/
  gen_cal_ptr = audio_config_ptr->gen_cal;
  if( gen_cal_ptr == NULL )
  {
    MSG_MED("acmapi_get_volume_levels_calunit_size: gen_cal is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  if ( gen_cal_ptr->generator == SND_GEN_VOC 
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)  
  || gen_cal_ptr->generator == SND_GEN_MIDI
#endif  
  )
  {
    *volume_level_size += sizeof(uint32); /*pcm_path*/
    *volume_level_size += sizeof(uint16); /*warmup_time_ms*/
    *volume_level_size += sizeof(uint16); /*pad_mask + one byte padding*/
    /*level data*/
    *volume_level_size += sizeof(snd_gen_level_voc_type) * (gen_cal_ptr->num_levels + 1) ;
  }
  else if( gen_cal_ptr->generator == SND_GEN_RING)
  {
    /*level data*/
    *volume_level_size += sizeof(uint16) * (gen_cal_ptr->num_levels + 1) ;
  }
  return ACM_ERR_NONE;
}

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
acm_error_code acmapi_get_volume_levels_data
(
  snd_cal_control_type *audio_config_ptr,
  byte **snd_cal_calunit_resp_buf_ptr
)
{
  snd_gen_cal_type *gen_cal = NULL;
  snd_gen_voc_cal_type *voc_gen_cal_ptr = NULL;
  snd_gen_ring_cal_type *ring_gen_cal_ptr = NULL;

  uint8 level_idx = 0 ;

  *(*snd_cal_calunit_resp_buf_ptr) =  (uint32) audio_config_ptr->device_vol;
  *snd_cal_calunit_resp_buf_ptr += sizeof(uint32) ;

  *(*snd_cal_calunit_resp_buf_ptr) = (uint32) audio_config_ptr->generator;
  *snd_cal_calunit_resp_buf_ptr += sizeof(uint32) ;

  gen_cal = audio_config_ptr->gen_cal;
  if( gen_cal == NULL )
  {
    MSG_MED("acmapi_get_volume_levels_data: gen_cal is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }

  *(*snd_cal_calunit_resp_buf_ptr)  = (uint32) gen_cal->generator;
  *snd_cal_calunit_resp_buf_ptr += sizeof(uint32) ;

  *(*snd_cal_calunit_resp_buf_ptr)  = gen_cal->num_levels;
  *snd_cal_calunit_resp_buf_ptr += sizeof(uint16) ; 

  *(*snd_cal_calunit_resp_buf_ptr)  = gen_cal->num_scale;
  *snd_cal_calunit_resp_buf_ptr += sizeof(uint16) ;

  if ( gen_cal->generator == SND_GEN_VOC
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)  
  || gen_cal->generator == SND_GEN_MIDI
#endif  
)
  {
     voc_gen_cal_ptr = (snd_gen_voc_cal_type *)audio_config_ptr->gen_cal;
    if ( voc_gen_cal_ptr->level_data == NULL )
    {
      MSG_MED("acmapi_get_volume_levels_data: level_data is NULL", 0,0,0);
      return ACM_ERR_UNKNOWN;
    }
    *(*snd_cal_calunit_resp_buf_ptr) = (uint32)voc_gen_cal_ptr->pcm_path;
    *snd_cal_calunit_resp_buf_ptr += sizeof(uint32);
    *(*snd_cal_calunit_resp_buf_ptr) = voc_gen_cal_ptr->warmup_time_ms;
    *snd_cal_calunit_resp_buf_ptr += sizeof(uint16);
    *(*snd_cal_calunit_resp_buf_ptr) = voc_gen_cal_ptr->pad_mask;
    *snd_cal_calunit_resp_buf_ptr += sizeof(uint16);/*sizeof(pad_mask) + one byte padding*/

    for(level_idx=0; level_idx <= voc_gen_cal_ptr->num_levels; level_idx++)
    {
      memcpy(*snd_cal_calunit_resp_buf_ptr,
                      &voc_gen_cal_ptr->level_data[level_idx], sizeof(snd_gen_level_voc_type));
      *snd_cal_calunit_resp_buf_ptr += sizeof(snd_gen_level_voc_type);
    }
  }  
  else if( gen_cal->generator == SND_GEN_RING)
  {
    ring_gen_cal_ptr = (snd_gen_ring_cal_type *)audio_config_ptr->gen_cal;
    if ( ring_gen_cal_ptr->level_data == NULL )
    {
      MSG_MED("acmapi_get_volume_levels_data: level_data is NULL", 0,0,0);
      return ACM_ERR_UNKNOWN;
    }
    for(level_idx=0; level_idx <= ring_gen_cal_ptr->num_levels; level_idx++)
    {      
      *(*snd_cal_calunit_resp_buf_ptr) = ring_gen_cal_ptr->level_data[level_idx];
      *snd_cal_calunit_resp_buf_ptr += sizeof(uint16);
    }
  }
  return ACM_ERR_NONE;
}

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
acm_error_code acmapi_set_volume_levels_data
(
  snd_cal_control_type *audio_config_ptr,
  byte **buf_ptr,
  uint32 *remaining_buf_length_ptr
)
{
  snd_gen_cal_type *gen_cal = NULL;
  snd_gen_voc_cal_type *voc_gen_cal_ptr = NULL;
  snd_gen_ring_cal_type *ring_gen_cal_ptr = NULL;

  uint8 level_idx = 0 ;

  /*We need to copy level data only. All other variables cannot be modified*/
  /*Skip buffer till level data*/
  
  /*audio_config_ptr->device_vol*/
  *buf_ptr += sizeof(uint32);
  *remaining_buf_length_ptr -= sizeof(uint32);

  /*audio_config_ptr->generator*/
  *buf_ptr += sizeof(uint32);   
  *remaining_buf_length_ptr -= sizeof(uint32);

  gen_cal = audio_config_ptr->gen_cal;
  if( gen_cal == NULL )
  {
    MSG_MED("acmapi_set_volume_levels_data: gen_cal is NULL", 0,0,0);
    return ACM_ERR_UNKNOWN;
  }
  
  /*audio_config_ptr->gen_cal->generator*/
  *buf_ptr += sizeof(uint32);    
  *remaining_buf_length_ptr -= sizeof(uint32);

  /*audio_config_ptr->gen_cal->num_levels*/
  *buf_ptr += sizeof(uint16); 
  *remaining_buf_length_ptr -= sizeof(uint16);

  /*audio_config_ptr->gen_cal->num_scale*/
  *buf_ptr += sizeof(uint16); 
  *remaining_buf_length_ptr -= sizeof(uint16);

  if ( gen_cal->generator == SND_GEN_VOC 
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)  
  || gen_cal->generator == SND_GEN_MIDI
#endif  
  )
  {
    voc_gen_cal_ptr = (snd_gen_voc_cal_type *)audio_config_ptr->gen_cal;
    if ( voc_gen_cal_ptr->level_data == NULL )
    {
      MSG_MED("acmapi_set_volume_levels_data: level_data is NULL", 0,0,0);
      return ACM_ERR_UNKNOWN;
    }
    /*voc_gen_cal_ptr->pcm_path*/
    *buf_ptr += sizeof(uint32);
    *remaining_buf_length_ptr -= sizeof(uint32);
    /*voc_gen_cal_ptr->warmup_time_ms=(uint16)*(*buf_ptr);*/
    *buf_ptr += sizeof(uint16);
    *remaining_buf_length_ptr -= sizeof(uint16);
    /*voc_gen_cal_ptr->pad_mask = (uint8)*(*buf_ptr);*/
    *buf_ptr += sizeof(uint16); /*sizeof(pad_mask) + one byte padding;    */
    *remaining_buf_length_ptr -= sizeof(uint16); /*sizeof(pad_mask) + one byte padding;*/
      for(level_idx=0; level_idx <= voc_gen_cal_ptr->num_levels; level_idx++)
      {
        memcpy(&voc_gen_cal_ptr->level_data[level_idx],*buf_ptr,sizeof(snd_gen_level_voc_type));
        *buf_ptr += sizeof(snd_gen_level_voc_type);
        *remaining_buf_length_ptr -= sizeof(snd_gen_level_voc_type);
      }
  }
  else if( gen_cal->generator == SND_GEN_RING)
  {
    ring_gen_cal_ptr = (snd_gen_ring_cal_type *)audio_config_ptr->gen_cal;
    if ( ring_gen_cal_ptr->level_data == NULL )
    {
      MSG_MED("acmapi_set_volume_levels_data: level_data is NULL", 0,0,0);
      return ACM_ERR_UNKNOWN;
    }
      for(level_idx=0; level_idx <= ring_gen_cal_ptr->num_levels; level_idx++)
      {
        ring_gen_cal_ptr->level_data[level_idx] = *(*buf_ptr);
        *buf_ptr += sizeof(uint16);
        *remaining_buf_length_ptr -= sizeof(uint16);
      }
  }
  return ACM_ERR_NONE;
}

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
uint32  acmapi_get_gains_calunit_size(void)
{
  uint32 req_buf_length = 0 ;
  req_buf_length += sizeof(uint16); /* TX Voice Volume*/
  req_buf_length += sizeof(uint16); /* TX DTMF gain*/
  req_buf_length += sizeof(uint16); /* CODEC TX gain*/
  req_buf_length += sizeof(uint16); /* CODEC RX gain*/
  req_buf_length += sizeof(uint16); /* CODEC ST gain*/
  MSG_MED("acmapi: returning gains CalUnit Size as %d", req_buf_length,0,0);
  return req_buf_length;
}

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
acm_error_code  acmapi_get_codec_st_gain
(
  uint16 *codec_st_gain_ptr,
  byte **buf_ptr
)
{
  /*copy CODEC RX gaindata*/
  memcpy(*buf_ptr, codec_st_gain_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  return ACM_ERR_NONE;
}

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
acm_error_code  acmapi_set_codec_st_gain
(
uint16 *codec_st_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
)
{
  /*Check we have enough buffer*/
  if( *remaining_buf_length_ptr < sizeof(uint16))
  {
     return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  /*copy CODEC ST gain data*/
  memcpy(codec_st_gain_ptr, *buf_ptr,sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -=sizeof(uint16);
  return ACM_ERR_NONE;
}
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
acm_error_code  acmapi_get_codec_rx_gain
(
  uint16 *codec_rx_gain_ptr,
  byte **buf_ptr
)
{
  /*copy CODEC RX gaindata*/
  memcpy(*buf_ptr, codec_rx_gain_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  return ACM_ERR_NONE;
}

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
)
{
  /*Check we have enough buffer*/
  if( *remaining_buf_length_ptr < sizeof(uint16))
  {
     return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  /*copy CODEC RX gain data*/
  memcpy(codec_rx_gain_ptr, *buf_ptr,sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -=sizeof(uint16);
  return ACM_ERR_NONE;
}
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
acm_error_code  acmapi_get_codec_tx_gain
(
  uint16 *codec_tx_gain_ptr,
  byte **buf_ptr
)
{
  /*copy CODEC TX gaindata*/
  memcpy(*buf_ptr, codec_tx_gain_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  return ACM_ERR_NONE;
}

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
acm_error_code  acmapi_set_codec_tx_gain
(
uint16 *codec_tx_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
)
{
  /*Check we have enough buffer*/
  if( *remaining_buf_length_ptr < sizeof(uint16))
  {
     return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  /*copy CODEC TX gain data*/
  memcpy(codec_tx_gain_ptr, *buf_ptr,sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -=sizeof(uint16);
  return ACM_ERR_NONE;
}
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
acm_error_code  acmapi_get_tx_dtmf_gain
(
  uint16 *tx_dtmf_gain_ptr,
  byte **buf_ptr
)
{
  /*copy TX DTMF gain data*/
  memcpy(*buf_ptr, tx_dtmf_gain_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  return ACM_ERR_NONE;
}

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
acm_error_code  acmapi_set_tx_dtmf_gain
(
uint16 *tx_dtmf_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
)
{
  /*Check we have enough buffer*/
  if( *remaining_buf_length_ptr < sizeof(uint16))
  {
     return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  /*copy TX DTMF gain data*/
  memcpy(tx_dtmf_gain_ptr, *buf_ptr,sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -=sizeof(uint16);
  return ACM_ERR_NONE;
}
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
acm_error_code  acmapi_get_tx_gain
(
  uint16 *tx_gain_ptr,
  byte **buf_ptr
)
{
  /*copy TX Voice Volume data*/
  memcpy(*buf_ptr, tx_gain_ptr, sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  return ACM_ERR_NONE;
}

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
acm_error_code  acmapi_set_tx_gain
(
uint16 *tx_gain_ptr,
byte **buf_ptr,
uint32 *remaining_buf_length_ptr
)
{
  /*Check we have enough buffer*/
  if( *remaining_buf_length_ptr < sizeof(uint16))
  {
     return ACM_ERR_UNEXPECTED_BUF_SIZE;
  }
  /*copy TX Voice Volume data*/
  memcpy(tx_gain_ptr, *buf_ptr,sizeof(uint16));
  *buf_ptr += sizeof(uint16);
  *remaining_buf_length_ptr -=sizeof(uint16);
  return ACM_ERR_NONE;
}

#endif /*FEATURE_AVS_DYNAMIC_CALIBRATION*/
  
