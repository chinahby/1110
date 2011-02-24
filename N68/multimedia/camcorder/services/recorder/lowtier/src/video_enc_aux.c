/*===========================================================================

             V I D E O    E N C O D E R    I N T E R F A C E

DESCRIPTION
  This file contains implementation which depends on other modules.
  This code isn't shipped as a library and hence will be recompiled
  whenver qdsp or other layers change.

REFERENCES
  Video Encoder ISOD: XX-XXXXX-00

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/lowtier/main/latest/src/video_enc_aux.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
31/10/07  sanal   Merging change from mainline
                   sanal   Changes to remove linker errors when 
                           FEATURE_VIDEO_ENCODE_DEBUG is disabled 
08/10/06   Gopi    Removed funciton mp4e_block_write32 for 6260 target
04/10/06   Nagesh  Made code Lint error free
11/16/05   drl     Always use timetick_get_ms in mp4e_get_time_ms regardless
                   of target.
10/17/05   gm      Automated script to modify the MSG macro's
10/03/05   cla     Modified video recording profiling code to support debugging 
                   using QXDM.
08/16/05   dw      Updated video recording profiling and event logging system
07/25/05   dw      Sync with csim - changes to include new header file
05/25/05   drl     Replaced include of custmp4.h with customer.h
02/22/05   wyh     Raw Encoder API changes.
02/15/05   dw      Added logic analyzer hooks with timelog under VIDEO_TIMETEST
02/03/05   dw      Added code for camcorder time analysis under VIDEO_TIMETEST 
01/20/05   pp/jk   Moved scalable search to video_enc_common.c. 
                   Put mp4e_block_write32 back. Cleaned up the timer.
01/19/05   wh      Temporary API. Don't handshake with video_eng_frame
01/13/05   qcam    Scalable search.
01/11/05   jk      Made Lint free. Added mp4e_get_time_ms().
12/29/04   wyh     Heap tracking support through videodrv_malloc();
11/29/04   drl     Changed transcoder_mode to OFFLINE in support of N-Frame
                   delay
11/29/04   wyh     mp4e_codec_update() will only update the dsp in the
                   VIDEO_ENCODING_STARTED state.
09/07/04   drl     Removed mp4e_block_write32. 
04/21/04   sj      Added state check to not touch qdsp memory when in
                   VIDEO_NOT_INIT state.
11/24/03   sj      Added support for Intra Refresh
09/16/03   sj      Added support for different Zero Block Detection for Luma
                   and Chroma.
08/04/03   sj      Added support for requestIntraFrame from dsp for
                   pause/resume
07/15/03   sa      Removed HWIO calls and replaced with
                   clk_regime_enable(CLK_RGM_MOT_EST_M)
06/29/03   sj      Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef T_ARM
#include "customer.h"
#endif

#ifdef FEATURE_VIDEO_ENCODE

#include <stdio.h>
#include <string.h>
#ifndef T_ARM
#include "qdsp_sim.h"
#endif

#include "camcorder_msg.h" 
#include "video_enc_aux.h"
#include "video_enc.h"
#ifdef T_ARM
#include "qdspext.h"
#endif

#if T_ARM
#ifdef T_MSM6500
  #include "timetick.h"         /* Clock services                          */
#else
#error code not present
#endif
#endif

#ifdef VIDEO_TIMETEST
#include "assert.h"
#endif /* VIDEO_TIMETEST */

extern video_enc_state_type video_driver_state;


#ifdef FEATURE_CAMCORDER_MALLOC
  camcorderutil_malloc_context_type *videodrv_malloc_context = NULL;
#endif

/*===========================================================================

                           MACRO DEFINITIONS

===========================================================================*/
#ifdef VIDEO_TIMETEST
#define MAX_TIMELOG_INSTANCE 10000 /* Approx 1.5 mins of diagnostic data */
#endif /*VIDEO_TIMETEST */


/*===========================================================================

                        GLOBAL LOCAL VARIABLES

===========================================================================*/
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
/* Frame counters to match profiling data between ARM/DSP */
LOCAL uint32 mp4e_timelog_frame_DSP;
LOCAL uint32 mp4e_timelog_frame_ARM;

#ifdef VIDEO_TIMETEST
LOCAL mp4e_diagnostic_type mp4e_timelog[MAX_TIMELOG_INSTANCE];
LOCAL uint32               mp4e_timelog_idx;

/* Outport Address for logic analyzer */
uint32 mp4e_timetest_out_addr;

/* Specific event time trackers in view of parallel processing */
LOCAL mp4e_diagnostic_time_type mp4e_timetrack;


LOCAL uint32 mp4e_timetest_map[] =
{
  /* MP4E_TIMELOG_INIT_STATE */
  MP4E_TIMETEST_LOGIC_ANALYZER_START_TRIGGER,

  /* MP4E_TIMELOG_ARM2DSP_INITIALIZE_CMD */                  
  MP4E_TIMETEST_ARM2DSP_INITIALIZE_CMD,

   /* MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SELECTION */          
  MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_SELECTION,

   /* MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_CONFIGURATION */      
  MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_CONFIGURATION,

  /* MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_START */              
  MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_START,
  
  /* MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_FEATURE_UPDATE */     
  MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_FEATURE_UPDATE,

  /* MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SLEEP */    
  MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_SLEEP,

  /* MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_IDLE */               
  MP4E_TIMETEST_ARM2DSP_VIDEO_ENCODE_IDLE,
  
  /* MP4E_TIMELOG_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK */
  MP4E_TIMETEST_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK,
  
  /* MP4E_TIMELOG_DSP2ARM_DOWNLOAD_DONE */                   
  MP4E_TIMETEST_DSP2ARM_DOWNLOAD_DONE,
  
  /* MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SELECTION_DONE */     
  MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_SELECTION_DONE,
  
  /* MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_CONFIG_DONE */        
  MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_CONFIG_DONE,
  
  /* MP4E_TIMELOG_DSP2ARM_VIDEO_FRAME_DONE */                
  MP4E_TIMETEST_DSP2ARM_VIDEO_FRAME_DONE,
  
  /* MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_IDLE_DONE */          
  MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_IDLE_DONE,
  
  /* MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_FEATURE_UPDATE_DONE */
  MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_FEATURE_UPDATE_DONE,
  
  /* MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SLEEP_DONE */         
  MP4E_TIMETEST_DSP2ARM_VIDEO_ENCODE_SLEEP_DONE,
  
  /* MP4E_TIMELOG_DSP2ARM_OUTPUT2_END_OF_FRAME */            
  MP4E_TIMETEST_DSP2ARM_OUTPUT2_END_OF_FRAME,
  
  /* MP4E_TIMELOG_ARM_START_LOSSLESS_ENCODE */
  MP4E_TIMETEST_ARM_START_LOSSLESS_ENCODE,
  
  /* MP4E_TIMELOG_ARM_LOSSLESS_ENCODE_DONE */
  MP4E_TIMETEST_ARM_LOSSLESS_ENCODE_DONE,
  
  /* MP4E_TIMELOG_ARM_SPECIAL_EFFECT_START */
  MP4E_TIMETEST_ARM_SPECIAL_EFFECT_START, 

  /* MP4E_TIMELOG_ARM_SPECIAL_EFFECT_DONE */
  MP4E_TIMETEST_ARM_SPECIAL_EFFECT_DONE, 

  /* MP4E_TIMELOG_ARM_RC_DROP_FRAME */
  MP4E_TIMETEST_ARM_RC_DROP_FRAME, 

  /* MP4E_TIMELOG_ARM_SAD */
  MP4E_TIMETEST_ARM_SAD
};

#else /*VIDEO_TIMETEST */

LOCAL mp4e_diagnostic_frame_type mp4e_timemsg;

#endif /*VIDEO_TIMETEST */
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
VideoEncDSP_EncoderSelectionCommandType * mp4e_codec_selection(void) {
  if (video_driver_state != VIDEO_NOT_INIT)
  {
    /* 
    ** The Driver has been initialized
    */
    return &codec_selection_cmd;
  }
  return NULL;
}

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
VideoEncodeDSP_EncoderConfigurationType * mp4e_codec_config(void) {
  if (video_driver_state != VIDEO_NOT_INIT)
  {
    /* 
    ** The Driver has been initialized
    */

    return &codec_config_cmd;
  }
  return NULL;
}

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
VideoEncodeDSP_EncoderStartType * mp4e_codec_start(void)
{

  if (video_driver_state != VIDEO_NOT_INIT)
  {
    /* 
    ** The Driver has been initialized
    */

    codec_start_cmd.transcoderMode = (uint16) VIDEO_ENC_DSP_OFFLINE_ENCODING;

    return &codec_start_cmd;
  }
  return NULL;
}

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
void mp4e_codec_update(VideoEncodeDSP_VideoEncodeUpdateType *p_codec_update_cmd)
{
  CAMQDSP_ReturnCodeType  error;

  if (video_driver_state == VIDEO_ENCODING_STARTED )
  {

    if ((error = VideoEncDSP_VideoEncUpdate(p_codec_update_cmd)) != 
        CAMQDSP_SUCCESS)
    {
      CAMCORDER_MSG_ERROR("mp4e_codec_update failure! error = %d", error,0,0);
    }
  }
  return;
}

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
uint32 mp4e_get_time_ms(void)
{
#ifdef T_ARM
  return timetick_get_ms();
#else
  return 0;
#endif
}

/* <EJECT> */
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
#ifdef VIDEO_TIMETEST
/* <EJECT> */
/*===========================================================================

FUNCTION mp4e_debug_record_timetest

DESCRIPTION
  This function logs the input parameters of time ticks, events and hardware
  motion estimation disgnostics into a global array of structs so that the 
  array contents can be processed after runtime.
  
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
  None
  
===========================================================================*/
static void mp4e_debug_record_timetest(mp4e_diagnostic_event_type    event,
                               void                          *profile_info,
                               uint32                        framenum)
{
  if (mp4e_timetest_map[event] && mp4e_timetest_out_addr)
  {
    outpw(mp4e_timetest_out_addr, mp4e_timetest_map[event]);
    outpw(mp4e_timetest_out_addr, 0x8000);
  }
  /*  
  ** Collect timing/cycles data up to TIMELOG_MAX_INSTANCE limit
  ** Incoming data after limit reached is ignored
  */
  if(mp4e_timelog_idx < MAX_TIMELOG_INSTANCE)
  {
    register const uint32 timelog_current = mp4e_get_time_ms();

    mp4e_timelog[mp4e_timelog_idx].time   = timelog_current;
    mp4e_timelog[mp4e_timelog_idx].event  = event;

    switch(event)
    {
      /************************** DSP BE ENCODING ****************************/
      /* Log current time when DSP starts encoding 1 frame */
      case MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_START:
        ASSERT(mp4e_timetrack.start_time_DSP == 0);
        mp4e_timetrack.start_time_DSP = timelog_current;
        break;

      /* Log time taken for DSP to encode 1 frame */
      case MP4E_TIMELOG_DSP2ARM_VIDEO_FRAME_DONE:
        mp4e_timelog[mp4e_timelog_idx].time_diff = timelog_current - 
                                                   mp4e_timetrack.start_time_DSP;
        mp4e_timetrack.start_time_DSP = 0;
        break;


      /*************************** ARM ENCODING ******************************/
      /* Log current time when ARM starts encoding 1 frame */
      case MP4E_TIMELOG_ARM_START_LOSSLESS_ENCODE:
        ASSERT(mp4e_timetrack.start_time_ARM == 0);
        mp4e_timetrack.start_time_ARM = timelog_current;
        break;

      /* Log time taken for ARM to encode 1 frame */
      case MP4E_TIMELOG_ARM_LOSSLESS_ENCODE_DONE:
        mp4e_timelog[mp4e_timelog_idx].time_diff = timelog_current - 
                                                   mp4e_timetrack.start_time_ARM;
        mp4e_timetrack.start_time_ARM = 0;
        break;


      /******************* ARM ACKING/RECEIVING VFE OUTPUT2 *******************/
      /* Log time between ARM handshake and VFE Ack2 */
      case MP4E_TIMELOG_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK:
        mp4e_timelog[mp4e_timelog_idx].time_diff = timelog_current - 
                                                   mp4e_timetrack.output2_time;                                    
        break;

      /* Log time for every output2 received */
      case MP4E_TIMELOG_DSP2ARM_OUTPUT2_END_OF_FRAME:
        mp4e_timelog[mp4e_timelog_idx].time_diff = (mp4e_timetrack.output2_time
                                                    == 0) ? 0 : 
                                                    timelog_current - 
                                                    mp4e_timetrack.output2_time;
        
        mp4e_timetrack.output2_time = timelog_current;
        break;


      /******************** ARM/DSP INTERACTION OVERHEAD *********************/
      /* Logging times for sequential events that can be grouped together */
      case MP4E_TIMELOG_ARM2DSP_INITIALIZE_CMD:
      case MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SELECTION:
      case MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_CONFIGURATION:
        ASSERT(mp4e_timetrack.event_time == 0);
        mp4e_timetrack.event_time = timelog_current;
        break;

      case MP4E_TIMELOG_DSP2ARM_DOWNLOAD_DONE:
      case MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SELECTION_DONE:     
      case MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_CONFIG_DONE:
        mp4e_timelog[mp4e_timelog_idx].time_diff = timelog_current - 
                                                   mp4e_timetrack.event_time;
        mp4e_timetrack.event_time = 0;
        break;


      /************************* DON'T CARE EVENTS ***************************/
      case MP4E_TIMELOG_INIT_STATE:
      case MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_FEATURE_UPDATE:
      case MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_FEATURE_UPDATE_DONE:
      case MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_IDLE:
      case MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_IDLE_DONE:
      case MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SLEEP:
      case MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SLEEP_DONE:
      default:
        break;
    }

    /*
    ** Only collect HW ME diagnostics if pointer is not NULL
    ** Pointer valid only when called from mp4_encode_frame()
    **/
    if(profile_info)
    {
      VideoEncDSP_ProfileStatusType *p_info = 
                     (VideoEncDSP_ProfileStatusType *)profile_info;
      mp4e_timelog[mp4e_timelog_idx].framePktNum     = 
                                     p_info->framePktNum;
      mp4e_timelog[mp4e_timelog_idx].avgFetchCycles  = 
                                     p_info->mbAvgFetchCycles;
      mp4e_timelog[mp4e_timelog_idx].avgSearchCycles = 
                                     p_info->mbAvgSearchCycles;
      mp4e_timelog[mp4e_timelog_idx].avgNumMbSearch  = 
                                     p_info->mbAvgNumMbSearch;
      mp4e_timelog[mp4e_timelog_idx].avgNumBlkSearch = 
                                     p_info->mbAvgNumBlkSearch;
    }

    /* Log the number of frames DSP or ARM has encoded so that we can
    ** match the timelogs in the event of ARM/DSP parallel processing
    ** and when N changes in N-frames buffer */
    if(framenum)
    {
      mp4e_timelog[mp4e_timelog_idx].frame_no = framenum;
    }
    
    mp4e_timelog_idx++;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mp4e_debug_clear _timetest

DESCRIPTION
  This function logs the input parameters of time ticks, events and hardware
  motion estimation diagnostics into a global array so that array contents 
  can be processed after runtime.
  
DEPENDENCIES
  Global local array of structs must be declared before calling this function
  to initialize each element.

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None
       
RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
static void mp4e_debug_clear_timetest(void)
{
  /* 
  ** Initialize all elements before start of every recording
  */
  memset (mp4e_timelog,0,MAX_TIMELOG_INSTANCE*sizeof(mp4e_diagnostic_type)); 
  mp4e_timelog[0].time  = mp4e_get_time_ms();
  mp4e_timelog[0].event = MP4E_TIMELOG_INIT_STATE;
  mp4e_timelog_idx = 1; 

  /* Outport Address for logiz analyzer + Trigger signal */
  mp4e_timetest_out_addr = 0x30000000;
  outpw(mp4e_timetest_out_addr, MP4E_TIMETEST_LOGIC_ANALYZER_START_TRIGGER);
  outpw(mp4e_timetest_out_addr, 0x8000);

  /* Initialize frame and time counters */
  memset (&mp4e_timetrack,0,sizeof(mp4e_diagnostic_time_type));
  mp4e_timelog_frame_DSP = 0;
  mp4e_timelog_frame_ARM = 0;  
}

#else /* VIDEO_TIMETEST */

/*===========================================================================

FUNCTION mp4e_debug_record_timemsg

DESCRIPTION
  This function logs the input parameters of time ticks, events and hardware
  motion estimation disgnostics into a data structure. 
  
DEPENDENCIES
  None
 
RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
static void mp4e_debug_record_timemsg(mp4e_diagnostic_event_type    event,
                               void                          *info,
                               uint32                        framenum)
{
  switch(event)
  {
    case MP4E_TIMELOG_DSP2ARM_OUTPUT2_END_OF_FRAME:
      memset(&mp4e_timemsg, 0, sizeof(mp4e_diagnostic_frame_type));
      break; 

    case MP4E_TIMELOG_ARM_START_LOSSLESS_ENCODE:
      /*
      ** Only collect HW ME diagnostics if pointer is not NULL
      ** Pointer valid only when called from mp4_encode_frame()
      **/
      if ( info )
      {
        VideoEncDSP_ProfileStatusType *p_info = 
              (VideoEncDSP_ProfileStatusType *)info;
        mp4e_timemsg.framePktNum = p_info->framePktNum;
        mp4e_timemsg.avgFetchCycles = p_info->mbAvgFetchCycles;
        mp4e_timemsg.avgSearchCycles = p_info->mbAvgSearchCycles;
        mp4e_timemsg.avgNumMbSearch = p_info->mbAvgNumMbSearch;
        mp4e_timemsg.avgNumBlkSearch = p_info->mbAvgNumBlkSearch;
      }
      break;

    case MP4E_TIMELOG_ARM_SAD:
      if (info)
      {
        mp4e_timemsg.sad = *((uint32*)info);
      }
      break; 

    case MP4E_TIMELOG_ARM_SPECIAL_EFFECT_START:
      if (mp4e_timemsg.time[event])
      { /* In the case we have multiple special effects going on,
        ** we don't want to overwrite the first occurrence of special effect 
        ** start time.
        */
        return;
      }
      break;

    default:
      break;
  }
  mp4e_timemsg.time[event] = mp4e_get_time_ms();
}/*lint !e715 " framenum used by other function also which is reffered by a common Macro "*/

/*===========================================================================

FUNCTION mp4e_debug_clear_timemsg

DESCRIPTION
  This function initialize the data structure that stores video encode 
  debugging info such as time ticks, events and hardware motion estimation.
  
DEPENDENCIES
  None

RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
static void mp4e_debug_clear_timemsg(void)
{
  mp4e_timelog_frame_DSP = 0;
  mp4e_timelog_frame_ARM = 0;  
}

/*===========================================================================

FUNCTION mp4e_debug_print_timemsg

DESCRIPTION
  This function prints out messages on QXDM log with the info contained in the 
  data structure that stores video encode debugging info such as time ticks, 
  events and hardware motion estimation.
  
DEPENDENCIES
  None

RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
static void mp4e_debug_print_timemsg(mp4e_diagnostic_format_type format)
{
  switch(format)
  {
    case MP4E_FORMAT_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK:
      {
        uint32 isr_to_ack_time = 0;
        uint32 arm_encode_time = 0;
        uint32 dsp_be_time = 0;
        uint32 special_effect_time = 0;

        isr_to_ack_time = 
          mp4e_timemsg.time[MP4E_TIMELOG_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK]
          - mp4e_timemsg.time[MP4E_TIMELOG_DSP2ARM_OUTPUT2_END_OF_FRAME];

        arm_encode_time =
          mp4e_timemsg.time[MP4E_TIMELOG_ARM_LOSSLESS_ENCODE_DONE]
          - mp4e_timemsg.time[MP4E_TIMELOG_ARM_START_LOSSLESS_ENCODE];

        dsp_be_time = 
          mp4e_timemsg.time[MP4E_TIMELOG_DSP2ARM_VIDEO_FRAME_DONE]
          - mp4e_timemsg.time[MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_START];

        special_effect_time =
          mp4e_timemsg.time[MP4E_TIMELOG_ARM_SPECIAL_EFFECT_DONE]
          - mp4e_timemsg.time[MP4E_TIMELOG_ARM_SPECIAL_EFFECT_START];

        CAMCORDER_MSG_HIGH("VFE_ACK isr->ack=%dms arm_enc=%dms dsp_be=%dms",
          isr_to_ack_time, arm_encode_time, dsp_be_time);
        CAMCORDER_MSG_HIGH("VFE_ACK sad=%d special_effect=%dms",mp4e_timemsg.sad,
          special_effect_time, 0);

        
        if ( mp4e_timemsg.time[MP4E_TIMELOG_ARM_RC_DROP_FRAME] ) 
        {
          CAMCORDER_MSG_HIGH("Frame dropped by RC",0,0,0);
        }
      }
      break;
  }
}
#endif /* VIDEO_TIMETEST */

#endif /* FEATURE_VIDEO_ENCODE_DEBUG */


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
void mp4e_debug_clear_timelog()
{
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
#ifdef VIDEO_TIMETEST
    mp4e_debug_clear_timetest();
#else//VIDEO_TIMETEST
    mp4e_debug_clear_timemsg();
#endif//VIDEO_TIMETEST
#endif// FEATURE_VIDEO_ENCODE_DEBUG
}
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
                               uint32                        framenum)
{
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
    if(framenum == MP4E_TIMELOG_FRAME_ARM)
	  framenum = mp4e_timelog_frame_ARM;
	else if(framenum == MP4E_TIMELOG_FRAME_ARM_INC)
	  framenum = ++mp4e_timelog_frame_ARM;
    else if(framenum == MP4E_TIMELOG_FRAME_DSP)
	  framenum = mp4e_timelog_frame_DSP;
	else if(framenum == MP4E_TIMELOG_FRAME_DSP_INC)
	  framenum = ++mp4e_timelog_frame_DSP;
#ifdef VIDEO_TIMETEST
    mp4e_debug_record_timetest((mp4e_diagnostic_event_type)event,
							    profile_info,framenum);
#else//VIDEO_TIMETEST
    mp4e_debug_record_timemsg((mp4e_diagnostic_event_type)event,
							   profile_info,framenum);
#endif//VIDEO_TIMETEST
#endif// FEATURE_VIDEO_ENCODE_DEBUG
}
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
void mp4e_debug_print_timelog( uint32 format)
{
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
#ifndef VIDEO_TIMETEST
    mp4e_debug_print_timemsg((mp4e_diagnostic_format_type) format);
#endif //VIDEO_TIMETEST
#endif //FEATURE_VIDEO_ENCODE_DEBUG
}

#endif /* FEATURE_VIDEO_ENCODE */
