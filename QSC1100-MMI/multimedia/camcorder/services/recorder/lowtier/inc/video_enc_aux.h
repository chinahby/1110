#ifndef VIDEO_ENCAUX_H
#define VIDEO_ENCAUX_H
/*===========================================================================

    V I D E O    E N C O D E R    H E A D E R    F I L E

DESCRIPTION
  This header file contains the prototypes for the auxiliary functions 
  for the video encoder driver.

REFERENCES
  Video Encoder ISOD: XX-XXXXX-00

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/lowtier/main/latest/inc/video_enc_aux.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
31/10/07  sanal   Merging change from mainline
                   sanal   Changes to remove linker errors when 
                           FEATURE_VIDEO_ENCODE_DEBUG is disabled 
08/10/06   Gopi    Removed funciton mp4e_block_write32 for 6260 target
10/03/05   cla     Modified video recording profiling code to support debugging 
                   using QXDM.
07/25/05   dw      Sync with csim - changes to include new header file
05/25/05   drl     Replaced include of custmp4.h with customer.h
02/15/05   dw      Added logic analyzer hooks with timelog under VIDEO_TIMETEST
02/03/05   dw      Added code for camcorder time analysis under VIDEO_TIMETEST 
01/20/05   pp/jk   Moved scalable search to video_enc_common.h. 
                   Put mp4e_block_write32 back.
01/13/05   qcam    Scalable search.
01/11/05   jk      Added mp4e_get_time_ms().
12/29/04   wyh     Heap tracking support through videodrv_malloc();
09/07/04   drl     Removed mp4e_block_write32. 
03/12/04   ali     Changes for offline encoder
11/24/03   sj      Added support for Intra Refresh
09/16/03   sj      Added support for different Zero Block Detection for Luma
                   and Chroma.
08/04/03   sj      Added QVAR_requestIntraFrame for pause/resume
06/29/03   sj      Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef T_ARM
#include "customer.h"
#endif

#include "comdef.h"
#include "video_enc_dsp_if.h"

#ifdef T_ARM
#include "camcorder_util.h"
#else
#include "qdsp_sim.h"
#endif

#ifdef FEATURE_VIDEO_ENCODE
extern VideoEncDSP_EncoderSelectionCommandType  codec_selection_cmd;
extern VideoEncodeDSP_EncoderConfigurationType  codec_config_cmd;
extern VideoEncodeDSP_EncoderStartType          codec_start_cmd;
extern VideoEncodeDSP_VideoEncodeUpdateType     codec_update_cmd;


#ifdef FEATURE_CAMCORDER_MALLOC
  extern camcorderutil_malloc_context_type *videodrv_malloc_context;
  #define videodrv_malloc(x) \
     camcorderutil_malloc(videodrv_malloc_context,x, \
                          __func__,__LINE__)
  #define videodrv_free(x)\
  { static msg_const_type m;\
    camcorderutil_free(videodrv_malloc_context,\
                      __LINE__,__func__,&m,x); \
  }
  #define videodrv_realloc(x,y)\
    camcorderutil_realloc(videodrv_malloc_context,\
                        __LINE__,__func__,x,y)
#else
  #define videodrv_malloc(x)    malloc(x)
  #define videodrv_free(x)      free(x)
  #define videodrv_realloc(x,y) realloc(x,y)
#endif

/* Define VIDEO_TIMETEST to enable timelogging and hardware ME profiling
** of the entire video encoding system on a per frame basis (MP4/H.263). 
** The array that holds timelogs is declared global (mp4e_timelog) and its
** contents can be viewed after recording has been completed. These contents
** are flushed upon the start of each record. 
**
** Logic analyzer hooks are also activated with VIDEO_TIMETEST and these
** hooks are located before the time/events are logged. 
*/
//#define VIDEO_TIMETEST
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
#define MP4E_DEBUG_CLEAR_TIMELOG()       mp4e_debug_clear_timelog()
#define MP4E_DEBUG_RECORD_TIMELOG(a,b,c) mp4e_debug_record_timelog(a,b,c)
#define MP4E_DEBUG_PRINT_TIMELOG(a)      mp4e_debug_print_timelog(a)
#else//FEATURE_VIDEO_ENCODE_DEBUG
#define MP4E_DEBUG_CLEAR_TIMELOG()
#define MP4E_DEBUG_RECORD_TIMELOG(a,b,c)
#define MP4E_DEBUG_PRINT_TIMELOG(a)
#endif//FEATURE_VIDEO_ENCODE_DEBUG

#ifdef FEATURE_VIDEO_ENCODE_DEBUG
typedef enum
{
    MP4E_TIMELOG_FRAME_ARM = 1,
    MP4E_TIMELOG_FRAME_ARM_INC,
    MP4E_TIMELOG_FRAME_DSP,
	MP4E_TIMELOG_FRAME_DSP_INC
}mp4e_timelog_frame_counter_type;




/*
** List of events of interest while logging time, including
** - ARM to DSP commands
** - CAMQDSP interrupts to ARM
** - Lossless Encoder start/handshake
** - ACKs to VFE for next output2
*/
typedef enum
{
  MP4E_TIMELOG_INIT_STATE,

  MP4E_TIMELOG_ARM2DSP_INITIALIZE_CMD,                  
  MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SELECTION,          
  MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_CONFIGURATION,      
  MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_START,              
  MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_FEATURE_UPDATE,     
  MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SLEEP,              
  MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_IDLE,               
  
  MP4E_TIMELOG_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK,
  
  MP4E_TIMELOG_DSP2ARM_DOWNLOAD_DONE,                   
  MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SELECTION_DONE,     
  MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_CONFIG_DONE,        
  MP4E_TIMELOG_DSP2ARM_VIDEO_FRAME_DONE,                
  MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_IDLE_DONE,          
  MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_FEATURE_UPDATE_DONE,
  MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SLEEP_DONE,         
  MP4E_TIMELOG_DSP2ARM_OUTPUT2_END_OF_FRAME,            
  
  MP4E_TIMELOG_ARM_START_LOSSLESS_ENCODE,
  MP4E_TIMELOG_ARM_LOSSLESS_ENCODE_DONE,
  MP4E_TIMELOG_ARM_SPECIAL_EFFECT_START,
  MP4E_TIMELOG_ARM_SPECIAL_EFFECT_DONE,
  MP4E_TIMELOG_ARM_RC_DROP_FRAME,
  MP4E_TIMELOG_ARM_SAD,
  MP4E_TIMELOG_TOTAL_EVENTS
} mp4e_diagnostic_event_type;
  
#ifdef VIDEO_TIMETEST

/* Outport Address for logic analyzer profiling */
extern uint32 mp4e_timetest_out_addr;

/* The following macros for logic analyzer hooks during system profiling
** These hooks will be placed in the same locations as the timelog system
*/
#define MP4E_TIMETEST_LOGIC_ANALYZER_START_TRIGGER             0xFF00

#define MP4E_TIMETEST_ARM2DSP_INITIALIZE_CMD                   0x0001
#define MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_SELECTION           0x0002
#define MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_CONFIGURATION       0x0003
#define MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_START               0x0004
#define MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_FEATURE_UPDATE      0x0005
#define MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_SLEEP               0x0006
#define MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_IDLE                0x0007
#define MP4E_TIMETEST_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK         0x0008

#define MP4E_TIMETEST_VFE2ARM_OUTPUT2_FRAME                    0x0009
#define MP4E_TIMETEST_DSP2ARM_DOWNLOAD_DONE                    0x0010

#define MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_SELECTION_DONE      0x0011
#define MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_CONFIG_DONE         0x0012
#define MP4E_TIMETEST_DSP2ARM_VIDEO_FRAME_DONE                 0x0013
#define MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_IDLE_DONE           0x0014
#define MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_FEATURE_UPDATE_DONE 0x0015
#define MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_SLEEP_DONE          0x0016
#define MP4E_TIMETEST_DSP2ARM_OUTPUT2_END_OF_FRAME             0x0017

#define MP4E_TIMETEST_ARM_START_LOSSLESS_ENCODE                0x0018
#define MP4E_TIMETEST_ARM_LOSSLESS_ENCODE_DONE                 0x0019
#define MP4E_TIMETEST_ARM_MPEG4_ENCODE_START                   0x0020
#define MP4E_TIMETEST_ARM_MPEG4_ENCODE_END                     0x0021
#define MP4E_TIMETEST_ARM_H263_ENCODE_START                    0x0022
#define MP4E_TIMETEST_ARM_H263_ENCODE_END                      0x0023
#define MP4E_TIMETEST_ARM_SPECIAL_EFFECT_START                 0x0024 
#define MP4E_TIMETEST_ARM_SPECIAL_EFFECT_DONE                  0x0025
#define MP4E_TIMETEST_ARM_RC_DROP_FRAME                        0x0026
#define MP4E_TIMETEST_ARM_SAD                                  0x0027

/*
** Struct holding timelog and HW ME diagnostics data. 
** Current implementation creates array of mp4e_diagnostic_type
** members. Array size is set at TIMELOG_MAX_INSTANCE.
*/
typedef struct {  
  uint32                     frame_no;
  uint32                     time;
  int32                      time_diff;
  mp4e_diagnostic_event_type event;
  uint16                     framePktNum;
  uint16                     avgFetchCycles;
  uint16                     avgSearchCycles;
  uint16                     avgNumMbSearch;
  uint16                     avgNumBlkSearch;
} mp4e_diagnostic_type;

/*
** Struct holding time tracking variables to link timelogs 
** between subsequent occurence of the same events.
*/
typedef struct {
  uint32 start_time_DSP;
  uint32 start_time_ARM;
  uint32 output2_time;
  uint32 event_time;
} mp4e_diagnostic_time_type;

#else /* VIDEO_TIME_TEST */
/*
** Struct holding timelog and HW ME diagnostics data. 
*/
typedef struct {  
  uint32                     time[MP4E_TIMELOG_TOTAL_EVENTS];
  uint32                     sad;
  uint16                     framePktNum;
  uint16                     avgFetchCycles;
  uint16                     avgSearchCycles;
  uint16                     avgNumMbSearch;
  uint16                     avgNumBlkSearch;
} mp4e_diagnostic_frame_type;

typedef enum {  
  MP4E_FORMAT_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK
} mp4e_diagnostic_format_type;

#endif /* VIDEO_TIME_TEST */

#endif /* FEATURE_VIDEO_ENCODE_DEBUG */


/* <EJECT> */
/*===========================================================================

FUNCTION mp4e_codec_selection

DESCRIPTION
  This function is used to inform the caller (camera services) of the payload
  for the Codec Selection Command to the DSP.

  The Video Encode Driver should have been initialized.

DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  pointer to VideoEncDSP_EncoderSelectionCommandType

===========================================================================*/
extern VideoEncDSP_EncoderSelectionCommandType * mp4e_codec_selection(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mp4e_codec_config

DESCRIPTION
  This function is used to inform the caller (camera services) of the payload
  for the Codec Config Command to the DSP.
  
  The Video Encode Driver should have been initialized.

DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  Pointer to VideoEncodeDSP_EncoderConfigurationType

===========================================================================*/
extern VideoEncodeDSP_EncoderConfigurationType * mp4e_codec_config(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mp4e_codec_start

DESCRIPTION
  This function is used to inform the caller (camera services) of the payload
  for the Codec Start Command to the DSP.
  
  The Video Encode Driver should have been initialized.

DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  pointer to VideoEncodeDSP_EncoderStartType

===========================================================================*/
extern VideoEncodeDSP_EncoderStartType * mp4e_codec_start(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mp4e_codec_update

DESCRIPTION
  This function is used to inform the caller (camera services) of the payload
  for the Codec Update Command to the DSP.
  
  The Video Encode Driver should have been initialized.

DEPENDENCIES
  None

ARGUMENTS IN
  VideoEncodeDSP_EncoderStartType* start_ptr

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

===========================================================================*/
void mp4e_codec_update(VideoEncodeDSP_VideoEncodeUpdateType *p_codec_update_cmd);

/* <EJECT> */
/*===========================================================================

FUNCTION mp4e_get_time_ms

DESCRIPTION
  This function gets the time, in milliseconds, from clock services.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  int32
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  None
  
===========================================================================*/
extern uint32 mp4e_get_time_ms(void);

/* <EJECT> */
 
/*===========================================================================

FUNCTION mp4e_debug_clear_timelog
DESCRIPTION
  This function calls mp4e_debug_record_timetest or mp4e_debug_record_timemsg
  if FEATURE_VIDEO_ENCODE_DEBUG.
  
DEPENDENCIES
  None
 
ARGUMENTS IN
  None
ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS

 
===========================================================================*/
void mp4e_debug_clear_timelog(void);

/*===========================================================================

FUNCTION mp4e_debug_record_timelog

DESCRIPTION
  This function calls mp4e_debug_record_timetest or mp4e_debug_record_timemsg
  if FEATURE_VIDEO_ENCODE_DEBUG.
 
DEPENDENCIES
  None
 
ARGUMENTS IN
  mp4e_timelog_event_type       event
  VideoEncDSP_ProfileStatusType *profile_info

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  function mp4e_debug_record_timelog is alive even if 
  FEATURE_VIDEO_ENCODE_DEBUG is disabled
  
===========================================================================*/
void mp4e_debug_record_timelog(uint32                       event, 
                               void                          *profile_info,
                               uint32                        framenum);

/*===========================================================================

FUNCTION mp4e_debug_print_timelog

DESCRIPTION
  This function calls mp4e_debug_print_timemsg if FEATURE_VIDEO_ENCODE_DEBUG
  is enabled.
  
DEPENDENCIES
  None

RETURN VALUE
  None
   
SIDE EFFECTS
  function mp4e_debug_print_timelog is alive even if 
  FEATURE_VIDEO_ENCODE_DEBUG is disabled
  
===========================================================================*/
void mp4e_debug_print_timelog( uint32 format);

#endif /* FEATURE_VIDEO_ENCODE */
#endif /* VIDEO_ENCAUX_H */
