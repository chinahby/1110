/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                 S U P P L E M E N T A R Y   D E F I N I T I O N S

DESCRIPTION
  This file contains supplementary functions and data definitions for
multimode voice services.

REFERENCES
  None.

Copyright (c) 2002 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvssup.c#9 $ $DateTime: 2009/08/20 21:51:09 $ $Author: c_lpanch $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/09   lp      CMI changes-fixed compilation error
08/17/09   lp      CMI changes
07/13/09   rm      Adding support for 4GV_WB_VoIP.
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
12/12/08   rm      Fix for CR 165984 , amr-wb WCDMA call problem
09/12/08   rm  	   Fixed klokwork errors
08/12/08   sj      Fixed klokwork errors
03/16/07   ymc     Fixed wrong words count during swapping bytes.
03/15/07   kan     Added 4GV-WB Code
02/26/07   kan     Added AMR-WB changes
11/28/06   suk     Removed compiler warning.
05/19/06   ayu     Cleaned up critical lint errors.
05/05/06    sm     Added mvssup_amr_get_scr_mode() & mvssup_gsm_get_dtx_mode().
03/18/06    ay     Cleaned up lint errors.
02/03/06    sm     Added 4GV VOIP support
01/16/06    sm     Added FEATURE_QDJ_TIMEWARP
07/28/05    sm     G723 fixes.
07/26/05   ymc     Fixed packet length calculation in IF1 and IF2 de-framing
                   functions.
07/22/05   ayu     Added AMR IF1 support.
07/05/05    sm     Added G.723.1 support.
04/28/05   ymc     Added 4GV support.
           ymc     Cleaned up lint errors.
02/20/05    sm     Added G.711 vocoder support.
01/17/05   ymc     Fixed AMR IF2 featurization.
           ymc     Updated Copyright.
11/17/04   ymc     Removed support for MSMAUD_VOC_WVRC and MSMAUD_VOC_SMV.
09/17/04   ymc     Added IS733 and IS127 support.
           ymc     Added interfaces to operate MVS frame queues.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
01/27/04   ymc     Lower message level to medium level for new uplink AMR rate
                   setting message.
           ymc     Updated Copyright.
           ymc     Fixed amrsup to/from mvs amr frame and mode conversion.
09/16/03   ymc     Added support for WCDMA AMR rate change.
06/29/03   ymc     Updated mvssup_wcdma_encode_amrif2 and
                   mvssup_wcdma_decode_amrif2 interfaces to return frame length.
05/20/03   ymc     Added packet swapping for WCDMA AMR packet processing.
04/24/03   ymc     Added channel class parameter to mvssup_wcdma_set_dl_channel.
           ymc     Removed mvs_dl_amr_mode and associated interface function.
04/17/03    sm     Added amrsup support.
            sm     Refeaturized code.
03/03/03   ymc     Cleaned up all lint errors and warnings.
02/14/03   ymc     Removed next amr mode in AMR uplink packet exchange.
           ymc     Removed dl amr mode setting in mvssup_amr_set_amr_mode.
01/24/03   ymc     Updated copyright.
01/14/03    sm     Added FEATURE_AMR_IF2.
12/12/02   ymc     Added GSM half rate support.
12/04/02    sm     Added support for AMR IF2.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "msmaud.h"

#ifdef FEATURE_MVS

#include "mvssup.h"
#include "mvsdsm.h"
#if (defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)) \
    && defined(FEATURE_WCDMA)
#error code not present
#endif /* (MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB) && FEATURE_WCDMA */
#include "msg.h"

/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)
LOCAL mvs_amr_mode_type mvs_ul_amr_mode = MVS_AMR_MODE_MAX;

#ifdef FEATURE_WCDMA
#error code not present
#endif  /* FEATURE_WCDMA */

#endif  /* MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB */

#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) \
    || defined(MSMAUD_VOC_AMR_WB)
LOCAL mvs_scr_mode_type mvs_scr_mode = MVS_SCR_DISABLE;
LOCAL mvs_dtx_mode_type mvs_dtx_mode = MVS_DTX_OFF;
#endif  /* MSMAUD_VOC_AMR || MSMAUD_VOC_HR || MSMAUD_VOC_EFR || MSMAUD_VOC_FR
           || MSMAUD_VOC_AMR_WB */

#if defined(MSMAUD_VOC_IS733) || defined(MSMAUD_VOC_IS127) \
    || defined(MSMAUD_VOC_4GV) || defined(MSMAUD_VOC_4GV_WB)
LOCAL mvs_voc_rate_type mvs_voc_max_rate = MVS_VOC_1_RATE;
LOCAL mvs_voc_rate_type mvs_voc_min_rate = MVS_VOC_8_RATE;
#endif  /* MSMAUD_VOC_IS733 || MSMAUD_VOC_IS127 || MSMAUD_VOC_4GV
                                                || MSMAUD_VOC_4GV_WB*/

#ifdef MSMAUD_VOC_G711
/* Default initial G.711 mode to u-law */
LOCAL mvs_g711_mode_type mvs_ul_g711_mode = MVS_G711_MODE_MULAW;
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/* Default initial G.723 mode to high quality */
LOCAL mvs_g723_mode_type mvs_ul_g723_mode = MVS_G723_MODE_HIGH;
#endif /* MSMAUD_VOC_G723 */

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

INTERNAL FUNCTION MVSSUP_SWAP

DESCRIPTION
  This function swaps bytes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NONE

===========================================================================*/
void mvssup_swap(
  const uint8 *src_ptr, /* source buffer                           */
  uint8 *dst_ptr,       /* destination buffer                      */
  int   num_words       /* number of words to swap                 */
)
{
  uint8 tbyte;

  num_words++;
  while(--num_words) {
    tbyte      = *src_ptr++;
    *dst_ptr++ = *src_ptr++;
    *dst_ptr++ = tbyte;
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_q_init

DESCRIPTION
  This function will initialize MVS voice frame queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_frame_q_init(
  mvs_frame_q_type *frame_q_ptr,
  mvs_frame_type *frames_buf,
  int size
)
{
  if ((frame_q_ptr==NULL) || (frames_buf==NULL) || (size<=0))
  {
    MSG_ERROR("MVS frame q init failed %d", size, 0, 0);
    return;
  }


  frame_q_ptr->frame_q = frames_buf;
  frame_q_ptr->q_size = size;
  frame_q_ptr->frame_cnt = 0;
  frame_q_ptr->curr_frame = 0;
  frame_q_ptr->miss_cnt = 0;
  frame_q_ptr->lost_cnt = 0;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_q_empty

DESCRIPTION
  This function will emptify MVS voice frame queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_frame_q_empty(
  mvs_frame_q_type *frame_q_ptr
)
{
  if (frame_q_ptr!=NULL)
  {
    frame_q_ptr->frame_cnt = 0;
    frame_q_ptr->curr_frame = 0;
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_check_q_full

DESCRIPTION
  This function will check if MVS voice frame queue is full or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mvssup_frame_check_q_full(
  /*lint -esym(818,frame_q_ptr) Tell Lint that frame_q_ptr will stay
   * non-const */
  mvs_frame_q_type *frame_q_ptr
)
{
  if ((frame_q_ptr==NULL) || (frame_q_ptr->frame_q==NULL)
      || (frame_q_ptr->q_size <= 0))
  {
    return TRUE;
  }

  if (frame_q_ptr->frame_cnt >= frame_q_ptr->q_size)
  {
    return TRUE;
  }

  return FALSE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_check_q_empty

DESCRIPTION
  This function will check if MVS voice frame queue is empty or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mvssup_frame_check_q_empty(
  /*lint -esym(818,frame_q_ptr) Tell Lint that frame_q_ptr will stay
   * non-const */
  mvs_frame_q_type *frame_q_ptr
)
{
  if ((frame_q_ptr==NULL) || (frame_q_ptr->frame_q==NULL)
      || (frame_q_ptr->q_size <= 0))
  {
    return TRUE;
  }

  if (frame_q_ptr->frame_cnt <= 0)
  {
    return TRUE;
  }

  return FALSE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_q_put

DESCRIPTION
  This function will put a frame into the queue.

DEPENDENCIES
  mvssup_frame_q_init must be called first to initialize the queue.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mvs_pkt_status_type mvssup_frame_q_put(
  mvs_frame_q_type *frame_q_ptr,
  /*lint -esym(818,frame_data) Tell Lint that frame_data will stay non-const */
  uint8 *frame_data,
  /*lint -esym(818,frame_info) Tell Lint that frame_info will stay non-const */
  mvs_frame_info_type *frame_info,
  uint16 frame_len
)
{
  mvs_pkt_status_type status = MVS_PKT_STATUS_NORMAL;
  int tail;


  if ((frame_q_ptr==NULL) || (frame_data==NULL) || (frame_info==NULL))
  {
    return MVS_PKT_STATUS_SLOW;    /* slow status indicates program error */
  }

  if ((frame_q_ptr->frame_q == NULL) || (frame_q_ptr->q_size <= 0))
  {
    MSG_ERROR("MVS frame queue needs to be initialised.", 0, 0, 0);
    return MVS_PKT_STATUS_SLOW;    /* slow status indicates program error */
  }

  if (frame_q_ptr->frame_cnt > frame_q_ptr->q_size)
  {
    MSG_FATAL("Unexpected MVS frame queue error %d %d !!", \
              frame_q_ptr->frame_cnt, frame_q_ptr->q_size, 0);
    return MVS_PKT_STATUS_SLOW;
  }


  if (frame_q_ptr->frame_cnt == frame_q_ptr->q_size)
  {
    frame_q_ptr->frame_cnt--;

    if (frame_q_ptr->frame_cnt == 0)
    {
      frame_q_ptr->curr_frame = 0;
    }
    else
    {
      frame_q_ptr->curr_frame = (frame_q_ptr->curr_frame + 1)
                                % (frame_q_ptr->q_size);
    }

    frame_q_ptr->lost_cnt++;

    MSG_HIGH("MVS frame queue overflow %x %d %d", \
             (uint32)(frame_q_ptr->frame_q), frame_q_ptr->frame_cnt, \
             frame_q_ptr->lost_cnt);

    status = MVS_PKT_STATUS_FAST;
  }


  tail = (frame_q_ptr->curr_frame + frame_q_ptr->frame_cnt)
         % (frame_q_ptr->q_size);

  memcpy((frame_q_ptr->frame_q + tail)->frame_data, frame_data, frame_len);
  (frame_q_ptr->frame_q + tail)->frame_info = *frame_info;
  (frame_q_ptr->frame_q + tail)->frame_len = frame_len;

  frame_q_ptr->frame_cnt++;

  return status;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_q_get

DESCRIPTION
  This function will get a frame from the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mvs_pkt_status_type mvssup_frame_q_get(
  mvs_frame_q_type *frame_q_ptr,
  uint8 *frame_data,
  mvs_frame_info_type *frame_info,
  uint16 *frame_len
)
{
  if ((frame_q_ptr==NULL) || (frame_data==NULL) || (frame_info==NULL))
  {
    return MVS_PKT_STATUS_FAST;   /* fast status indicates program error */
  }

  if ((frame_q_ptr->frame_q == NULL) || (frame_q_ptr->q_size <= 0))
  {
    MSG_ERROR("MVS frame queue needs to be initialised.", 0, 0, 0);
    return MVS_PKT_STATUS_FAST;   /* fast status indicates program error */
  }

  if (frame_q_ptr->frame_cnt < 0)
  {
    MSG_FATAL("Unexpected MVS frame queue error %d %d !!", \
              frame_q_ptr->frame_cnt, frame_q_ptr->q_size, 0);
    return MVS_PKT_STATUS_FAST;
  }


  if (frame_q_ptr->frame_cnt == 0)
  {
    frame_q_ptr->miss_cnt++;

    MSG_HIGH("MVS frame queue underflow %x %d %d", \
             (uint32)(frame_q_ptr->frame_q), frame_q_ptr->frame_cnt, \
             frame_q_ptr->miss_cnt);

    return MVS_PKT_STATUS_SLOW;
  }


  memcpy(frame_data,
         (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_data,
         (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_len);
  *frame_info = (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_info;
  if (frame_len != NULL)
  {
    *frame_len = (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_len;
  }

  frame_q_ptr->frame_cnt--;

  if (frame_q_ptr->frame_cnt == 0)
  {
    frame_q_ptr->curr_frame = 0;
  }
  else
  {
    frame_q_ptr->curr_frame = (frame_q_ptr->curr_frame + 1)
                              % (frame_q_ptr->q_size);
  }

  return MVS_PKT_STATUS_NORMAL;
}

#ifdef FEATURE_AMR_IF1
/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_umts_encode_amrif1

DESCRIPTION
  This function processes a vocoder packet and builds an AMR IF1 packet.

PARAMETERS
  [in/xxx] vocoder_packet : the pointer to the vocoder packet to encode.
  [in/xxx] frame_info     : the pointer to an amr_rate uplink info that
                            describes the vocoder_packet.
  [xx/out] if1_frame      : the pointer to the encoded AMR IF1 frame.
  [xx/out] if1_frame_info : the pointer to an amr_if1_info uplink info that
                            describes the if1_frame.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of encoded frame.
  if1_frame : IF1-encoded frame.
  if1_frame_info : holds frame information of IF1-encoded frame.

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_umts_encode_amrif1(
  uint8               *vocoder_packet,
  mvs_frame_info_type *frame_info,
  uint8               *if1_frame,
  mvs_frame_info_type *if1_frame_info
)
{
  mvs_amr_frame_type          frame_type_in;
  mvs_amr_mode_type           frame_rate_in;
  amrsup_if1_frame_info_type  frame_info_out;
  int                         bytes_encoded;


  /* abort if any in/out pointer is NULL */
  if ((vocoder_packet == NULL) || (frame_info == NULL) ||
      (if1_frame == NULL) || (if1_frame_info == NULL)) {
    MSG_ERROR("Invalid NULL parameters.", 0, 0, 0);
    return 0;
  }

  if ((frame_info->hdr.mode != MVS_MODE_AMR)
      && (frame_info->hdr.mode != MVS_MODE_AMR_WB))
  {
    MSG_ERROR("Invalid MVS mode %d for AMR IF1 encoding.", \
              frame_info->hdr.mode, 0, 0);
    return 0;
  }

  /* abort if the requested TX_TYPE is invalid */
  if (MVS_AMR_FRAME_TYPE_MAX <= frame_info->amr_rate.ul_info.frame) {
    MSG_ERROR("Invalid Frame Type (TX_TYPE) value.", 0, 0, 0);
    return 0;
  }

  /* abort if the requested Mode Indication is invalid */
  if (MVS_AMR_MODE_MAX <= frame_info->amr_rate.ul_info.mode) {
    MSG_ERROR("Invalid Frame Rate (Mode Indication) value.", 0, 0, 0);
    return 0;
  }

  /* check the consistency between the channel setup and the current VOC AMR
   * mode
   */
  if (mvs_ul_amr_mode != frame_info->amr_rate.ul_info.mode) {
    MSG_ERROR("Inconsistent values: channel mode %d  voc mode %d", \
              mvs_ul_amr_mode, frame_info->amr_rate.ul_info.mode, 0);
  }

  /* high-low byte swapping */
  if (frame_info->hdr.mode == MVS_MODE_AMR_WB)
  {
    mvssup_swap(vocoder_packet, vocoder_packet,
                (amrsup_frame_len(AMRSUP_SPEECH_GOOD, AMRSUP_MODE_2385)+1)>>1);
  }
  else
  {
    mvssup_swap(vocoder_packet, vocoder_packet,
                (amrsup_frame_len(AMRSUP_SPEECH_GOOD, AMRSUP_MODE_1220)+1)>>1);
  }

  /* encode the voice packet into the AMR IF1 Class A, B, & C format */
  frame_type_in = MVS_AMR_TO_AMRSUP_FRAME(frame_info->amr_rate.ul_info.frame);
  frame_rate_in = MVS_AMR_TO_AMRSUP_MODE(frame_info->amr_rate.ul_info.mode);

  bytes_encoded =
    amrsup_if1_framing(vocoder_packet,
                       frame_type_in,
                       frame_rate_in,
                       if1_frame,
                       &frame_info_out);

  if (frame_info_out.amr_type == AMRSUP_CODEC_AMR_NB)
  {
  if1_frame_info->amr_if1_info.ul_info.frame_type_index =
    AMRSUP_TO_MVS_AMR_FRAME_TYPE_INDEX(frame_info_out.frame_type_index);

    if1_frame_info->hdr.mode = MVS_MODE_AMR;
  }
#if defined(MSMAUD_VOC_AMR_WB_COMMON)
  else
  {
    if1_frame_info->amr_if1_info.ul_info.frame_type_index =
      AMRSUP_TO_MVS_AWB_FRAME_TYPE_INDEX(frame_info_out.frame_type_index);

    if1_frame_info->hdr.mode = MVS_MODE_AMR_WB;
  }
#endif /* defined(MSMAUD_VOC_AMR_WB_COMMON)*/

  if1_frame_info->amr_if1_info.ul_info.fqi = frame_info_out.fqi;

  return bytes_encoded;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_umts_decode_amrif1

DESCRIPTION
  This function processes an AMR IF1 packet and builds a vocoder packet.

PARAMETERS
  [in/xxx] if1_frame      : the pointer to the AMR IF1 frame to decode.
  [in/xxx] if1_frame_info : the pointer to an amr_if1_info downlink info that
                            describes the if1_frame.
  [xx/out] vocoder_packet : the pointer to the decoded vocoder packet.
  [xx/out] frame_info     : the pointer to an amr_rate downlink info that
                            describes the vocoder_packet.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of decoded frame.
  vocoder_packet : decoded packet
  frame_info : holds frame information of decoded frame

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_umts_decode_amrif1(
  uint8               *if1_frame,
  mvs_frame_info_type *if1_frame_info,
  uint8               *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
  amrsup_if1_frame_info_type  frame_info_in;
  amrsup_frame_type           frame_type_out;
  amrsup_mode_type            frame_rate_out;
  int                         bytes_decoded;


  /* abort if any in/out pointer is NULL */
  if ((if1_frame == NULL) || (if1_frame_info == NULL) ||
      (vocoder_packet == NULL) || (frame_info == NULL)) {
    MSG_ERROR("Invalid NULL parameters.", 0, 0, 0);
    return 0;
  }

  /* abort if the requested TX_TYPE is invalid */
  if (MVS_AMR_FRAME_TYPE_INDEX_MAX <=
      if1_frame_info->amr_if1_info.dl_info.frame_type_index) {
    MSG_ERROR("Invalid Frame Type Index value.", 0, 0, 0);
    return 0;
  }

  /* decode the AMR IF1 Class A, B, & C format into the voice packet */
  if (if1_frame_info->hdr.mode == MVS_MODE_AMR)
  {
    frame_info_in.amr_type = AMRSUP_CODEC_AMR_NB;

  frame_info_in.frame_type_index =
    MVS_AMR_TO_AMRSUP_FRAME_TYPE_INDEX( \
      if1_frame_info->amr_if1_info.dl_info.frame_type_index);

    frame_info->hdr.mode = MVS_MODE_AMR;
  }
#if defined(MSMAUD_VOC_AMR_WB_COMMON)
  else if (if1_frame_info->hdr.mode == MVS_MODE_AMR_WB)
  {
    frame_info_in.amr_type = AMRSUP_CODEC_AMR_WB;

    frame_info_in.frame_type_index =
      MVS_AWB_TO_AMRSUP_FRAME_TYPE_INDEX( \
        if1_frame_info->amr_if1_info.dl_info.frame_type_index);

    frame_info->hdr.mode = MVS_MODE_AMR_WB;
  }
#endif /*#defined(MSMAUD_VOC_AMR_WB_COMMON)*/
  else
  {
    MSG_ERROR("Invalid MVS mode %d for AMR IF1 decoding.", \
              if1_frame_info->hdr.mode, 0, 0);
    return 0;
  }

  frame_info_in.fqi = if1_frame_info->amr_if1_info.dl_info.fqi;

  bytes_decoded =
    amrsup_if1_de_framing(vocoder_packet,
                          &frame_type_out,
                          &frame_rate_out,
                          if1_frame,
                          &frame_info_in);

  frame_info->amr_rate.dl_info.frame = AMRSUP_TO_MVS_AMR_FRAME(frame_type_out);

  if( frame_rate_out >= AMRSUP_MODE_MAX )
  {
       MSG_ERROR("Index = %d, Out of Range for AMRSUP_TO_MVS_AMR_MODE. ", frame_rate_out, 0, 0);
       return 0;
  }
  else
  {
      frame_info->amr_rate.dl_info.mode = AMRSUP_TO_MVS_AMR_MODE(frame_rate_out);
  }

  /* low-high byte swapping */
  if (frame_info->hdr.mode == MVS_MODE_AMR_WB)
  {
    mvssup_swap(vocoder_packet, vocoder_packet,
                (amrsup_frame_len(AMRSUP_SPEECH_GOOD, AMRSUP_MODE_2385)+1)>>1);
  }
  else
  {
    mvssup_swap(vocoder_packet, vocoder_packet,
                (amrsup_frame_len(AMRSUP_SPEECH_GOOD, AMRSUP_MODE_1220)+1)>>1);
  }


  /* vocoder_packet in little-endian format, length must be even. */
  if (bytes_decoded >= 0) {
    bytes_decoded = (int) (((unsigned int) bytes_decoded+1) >> 1) << 1;
  }

  return bytes_decoded;
}

#endif /* FEATURE_AMR_IF1 */

/* <EJECT> */
#if (defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)) \
    && defined(FEATURE_WCDMA)
#error code not present
#endif /* (MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB) && FEATURE_WCDMA */

#ifdef FEATURE_AMR_IF2
/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_wcdma_encode_amrif2

DESCRIPTION
  This function processes a vocoder packet and builds an AMR IF2 packet.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of encoded frame.

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_wcdma_encode_amrif2(
  uint8                *vocoder_packet,
  /*lint -esym(818,frame_info) Tell Lint that frame_info will stay non-const */
  mvs_frame_info_type  *frame_info,
  uint8                *if2_frame
)
{
  amrsup_frame_type amrsup_frame;
  amrsup_mode_type amrsup_mode;

  MSG_MED("mvssup_wcdma_encode_amrif2()", 0, 0, 0);

  if ((vocoder_packet==NULL) || (frame_info==NULL))
  {
    MSG_ERROR("Invalid parameters.", 0, 0, 0);
    return 0;
  }

  /* check VOC AMR mode and channel setup consistency */
  if (frame_info->amr_rate.ul_info.mode != mvs_ul_amr_mode)
  {
    MSG_ERROR("Warning: channel mode %d  voc mode %d",  \
              mvs_ul_amr_mode, frame_info->amr_rate.ul_info.mode, 0);
  }

  if(frame_info->amr_rate.ul_info.frame >= MVS_AMR_FRAME_TYPE_MAX)
  {
	MSG_ERROR("Invalid AMR_Frame : %d",frame_info->amr_rate.ul_info.frame,0,0);
	return 0;
  }
  if(frame_info->amr_rate.ul_info.mode >= MVS_AMR_MODE_MAX)
  {
	MSG_ERROR("Invalid AMR_Mode : %d",frame_info->amr_rate.ul_info.mode,0,0);
	return 0;
  }
  /* high and low bytes swapping */
  mvssup_swap(vocoder_packet, vocoder_packet, (AMRSUP_IF2_FRAME_BYTES+1) / 2);

  /* Encode AMR A, B, & C data into IF2 format */
  amrsup_frame = MVS_AMR_TO_AMRSUP_FRAME(frame_info->amr_rate.ul_info.frame);
  amrsup_mode = MVS_AMR_TO_AMRSUP_MODE(frame_info->amr_rate.ul_info.mode);

  return amrsup_if2_framing(vocoder_packet,
                            amrsup_frame,
                            amrsup_mode,
                            if2_frame);
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_wcdma_decode_amrif2

DESCRIPTION
  This function processes an AMR IF2 packet and builds a vocoder packet.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of decoded frame.

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_wcdma_decode_amrif2(
  uint8 *if2_frame,
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
  int frame_len=0;
  amrsup_frame_type amrsup_frame;
  amrsup_mode_type amrsup_mode;

  MSG_MED("mvssup_wcdma_decode_amrif2()", 0, 0, 0);

  /* Assembling the downlink speech into original voice packet */
  frame_len = amrsup_if2_de_framing(vocoder_packet,
                                    &amrsup_frame,
                                    &amrsup_mode,
                                    if2_frame);

  if(amrsup_frame >= AMRSUP_FRAME_TYPE_MAX)
  {
	MSG_ERROR("Invalid AMR_Frame : %d",amrsup_frame,0,0);
	return 0;
  }

  if(amrsup_mode >= AMRSUP_MODE_MAX)
  {
	MSG_ERROR("Invalid AMR_Mode : %d",amrsup_mode,0,0);
	return 0;
  }

  frame_info->amr_rate.dl_info.frame = AMRSUP_TO_MVS_AMR_FRAME(amrsup_frame);
  frame_info->amr_rate.dl_info.mode = AMRSUP_TO_MVS_AMR_MODE(amrsup_mode);

  /* high and low bytes swapping */
  mvssup_swap(vocoder_packet, vocoder_packet, (AMRSUP_IF2_FRAME_BYTES+1) / 2);

  /* vocoder_packet in little-endian format, length must be even. */
  if (frame_len >= 0) {
    frame_len = (int) (((unsigned int) frame_len+1) >> 1) << 1;
  }

  return frame_len;
}
#endif /* FEATURE_AMR_IF2 */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_WB
/*===========================================================================

FUNCTION mvssup_amr_set_awb_mode

DESCRIPTION
  This function sets the AMR-WB transfer mode of an AMR-WB vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvssup_amr_set_awb_mode(
  mvs_amr_mode_type amr_ul_mode
)
{
  voc_amr_wb_frame_rate_type voc_amr_wb_mode = VOC_AMR_WB_RATE_MAX;

  if ((amr_ul_mode<MVS_AMR_MODE_0660) || (amr_ul_mode>MVS_AMR_MODE_MAX))
  {
    MSG_ERROR("Invalid uplink AMR-WB mode setting %d", amr_ul_mode, 0, 0);
    return;
  }

  mvs_ul_amr_mode = amr_ul_mode;

  switch (mvs_ul_amr_mode)
  {
    case MVS_AMR_MODE_0660 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_0660;
      break;

    case MVS_AMR_MODE_0885 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_0885;
      break;

    case MVS_AMR_MODE_1265 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_1265;
      break;

    case MVS_AMR_MODE_1425 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_1425;
      break;

    case MVS_AMR_MODE_1585 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_1585;
      break;

    case MVS_AMR_MODE_1825 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_1825;
      break;

    case MVS_AMR_MODE_1985 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_1985;
      break;

    case MVS_AMR_MODE_2305 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_2305;
      break;

    case MVS_AMR_MODE_2385 :
      voc_amr_wb_mode = VOC_AMR_WB_RATE_2385;
      break;

    default :
      MSG_ERROR("VOC UL AMR-WB mode %d setting failed.", voc_amr_wb_mode, 0, 0);
      return;
  }

  if (voc_init_amr_wb_tx_rate(voc_amr_wb_mode) != VOC_DONE_S)
  {
    MSG_ERROR("VOC UL AMR-WB mode %d setting failed.", voc_amr_wb_mode, 0, 0);
    return;
  }

  MSG_MED("New uplink AMR-WB mode: %d", mvs_ul_amr_mode, 0, 0);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_get_ul_awb_mode

DESCRIPTION
  This function returns currently configured uplink AMR-WB mode.

DEPENDENCIES
  None

RETURN VALUE
  mvs_ul_amr_mode

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
mvs_amr_mode_type mvssup_get_ul_awb_mode(void)
{
  return mvs_ul_amr_mode;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#endif  /* MSMAUD_VOC_AMR_WB */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR
/*===========================================================================

FUNCTION mvssup_amr_set_amr_mode

DESCRIPTION
  This function sets the AMR transfer mode of an AMR vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvssup_amr_set_amr_mode(
  mvs_amr_mode_type amr_ul_mode
)
{
  voc_gsm_frame_rate_type voc_amr_mode = VOC_GSM_RATE_MAX;

  MSG_MED("mvssup_amr_set_amr_mode(): amr_ul_mode %d", amr_ul_mode, 0, 0);

  if ((amr_ul_mode<MVS_AMR_MODE_0475) || (amr_ul_mode>MVS_AMR_MODE_MAX))
  {
    MSG_ERROR("Invalid uplink AMR mode setting %d", amr_ul_mode, 0, 0);
    return;
  }

  mvs_ul_amr_mode = amr_ul_mode;

  switch (mvs_ul_amr_mode)
  {
    case MVS_AMR_MODE_0475 :
      voc_amr_mode = VOC_AMR_RATE_475;
      break;

    case MVS_AMR_MODE_0515 :
      voc_amr_mode = VOC_AMR_RATE_515;
      break;

    case MVS_AMR_MODE_0590 :
      voc_amr_mode = VOC_AMR_RATE_590;
      break;

    case MVS_AMR_MODE_0670 :
      voc_amr_mode = VOC_AMR_RATE_670;
      break;

    case MVS_AMR_MODE_0740 :
      voc_amr_mode = VOC_AMR_RATE_740;
      break;

    case MVS_AMR_MODE_0795 :
      voc_amr_mode = VOC_AMR_RATE_795;
      break;

    case MVS_AMR_MODE_1020 :
      voc_amr_mode = VOC_AMR_RATE_1020;
      break;

    case MVS_AMR_MODE_1220 :
      voc_amr_mode = VOC_AMR_RATE_1220;
      break;

    default :
      MSG_ERROR("VOC UL AMR mode %d setting failed.", voc_amr_mode, 0, 0);
      return;
  }

  if (voc_init_amr_tx_rate(voc_amr_mode) != VOC_DONE_S)
  {
    MSG_ERROR("VOC UL AMR mode %d setting failed.", voc_amr_mode, 0, 0);
    return;
  }

  MSG_MED("New uplink AMR mode: %d", mvs_ul_amr_mode, 0, 0);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_get_ul_amr_mode

DESCRIPTION
  This function returns currently configured uplink AMR mode.

DEPENDENCIES
  None

RETURN VALUE
  mvs_ul_amr_mode

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
mvs_amr_mode_type mvssup_get_ul_amr_mode(void)
{
  MSG_MED("mvssup_get_ul_amr_mode(): mvs_ul_amr_mode %d", mvs_ul_amr_mode, 0, 0);

  return mvs_ul_amr_mode;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#endif  /* MSMAUD_VOC_AMR */

/* <EJECT> */
#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) \
    || defined(MSMAUD_VOC_AMR_WB)
/*===========================================================================

FUNCTION mvssup_amr_set_scr_mode

DESCRIPTION
  This function sets the SCR mode of the AMR vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_amr_set_scr_mode(mvs_scr_mode_type scr_mode)
{
  voc_gsm_dtx_mode_type dtx_mode;

  MSG_MED("mvssup_amr_set_scr_mode(): scr_mode %d", scr_mode, 0, 0);

  if ((scr_mode!=MVS_SCR_DISABLE) && (scr_mode!=MVS_SCR_ENABLE))
  {
    MSG_ERROR("Invalid MVS SCR mode setting %d.", scr_mode, 0, 0);
    return;
  }

  dtx_mode = (scr_mode==MVS_SCR_ENABLE)? VOC_GSM_DTX_ON : VOC_GSM_DTX_OFF;
  if (voc_set_gsm_enc_dtx(dtx_mode) != VOC_DONE_S)
  {
    MSG_ERROR("MVS SCR mode %d setup failed.", scr_mode, 0, 0);
    return;
  }

  mvs_scr_mode = scr_mode;
  MSG_HIGH("Set MVS SCR mode to %d", mvs_scr_mode, 0, 0);
}

/*===========================================================================

FUNCTION mvssup_amr_get_scr_mode

DESCRIPTION
  This function gets the SCR mode of the AMR vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mvs_scr_mode_type mvssup_amr_get_scr_mode(void)
{
  return(mvs_scr_mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_gsm_set_dtx_mode

DESCRIPTION
  This function sets the DTX mode of a GSM vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvssup_gsm_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
)
{
  voc_gsm_dtx_mode_type voc_dtx_mode;

  MSG_MED("mvssup_gsm_set_dtx_mode(): dtx_mode %d", dtx_mode, 0, 0);

  if ((dtx_mode!=MVS_DTX_OFF) && (dtx_mode!=MVS_DTX_ON))
  {
    MSG_ERROR("Invalid MVS DTX mode setting %d", dtx_mode, 0, 0);
    return;
  }

  voc_dtx_mode = (dtx_mode==MVS_DTX_ON)? VOC_GSM_DTX_ON : VOC_GSM_DTX_OFF;
  if (voc_set_gsm_enc_dtx(voc_dtx_mode) != VOC_DONE_S)
  {
    MSG_ERROR("MVS DTX mode %d setup failed.", dtx_mode, 0, 0);
    return;
  }

  mvs_dtx_mode = dtx_mode;
  MSG_HIGH("Set MVS DTX mode to %d", mvs_dtx_mode, 0, 0);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION mvssup_gsm_get_dtx_mode

DESCRIPTION
  This function gets the DTX mode of a GSM vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mvs_dtx_mode_type mvssup_gsm_get_dtx_mode(void)
{
  return(mvs_dtx_mode);
}

#endif  /* MSMAUD_VOC_AMR || MSMAUD_VOC_HR || MSMAUD_VOC_EFR || MSMAUD_VOC_FR
           || MSMAUD_VOC_AMR_WB */
/* <EJECT> */

#if defined(MSMAUD_VOC_IS127) || defined(MSMAUD_VOC_IS733) \
    || defined(MSMAUD_VOC_4GV) || defined(MSMAUD_VOC_4GV_WB)
/*===========================================================================

FUNCTION mvssup_get_voc_max_rate

DESCRIPTION
  This function retrieves the current maximum vocoder frame rate setting.

DEPENDENCIES
  None

RETURN VALUE
  mvs_voc_max_rate

SIDE EFFECTS
  None

===========================================================================*/
mvs_voc_rate_type mvssup_get_voc_max_rate(void)
{
  return mvs_voc_max_rate;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_get_voc_min_rate

DESCRIPTION
  This function retrieves the current minimum vocoder frame rate setting.

DEPENDENCIES
  None

RETURN VALUE
  mvs_voc_min_rate

SIDE EFFECTS
  None

===========================================================================*/
mvs_voc_rate_type mvssup_get_voc_min_rate(void)
{
  return mvs_voc_min_rate;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_voc_set_frame_rate

DESCRIPTION
  This function sets the max. and min. transfer rates for IS733, IS127 modes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_voc_set_frame_rate(
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
)
{
  MSG_MED("mvssup_voc_set_frame_rate(): max_rate %d, min_rate %d", \
	  			max_rate, min_rate, 0);

  if ((max_rate > MVS_VOC_1_RATE) || (min_rate > MVS_VOC_1_RATE))
  {
    MSG_ERROR("Illegal frame rates max %d min %d.", max_rate, min_rate, 0);
    max_rate = MVS_VOC_8_RATE;
    min_rate = MVS_VOC_8_RATE;
  }

  mvs_voc_max_rate = max_rate;
  mvs_voc_min_rate = min_rate;

#ifdef FEATURE_VOC_PACKET_INTERFACE
  if (voc_set_frame_rate_cmd((voc_rate_type)max_rate, (voc_rate_type)min_rate)
      != VOC_DONE_S)
#endif
  {
    MSG_ERROR("Error in setting rates max %d min %d", max_rate, min_rate, 0);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_voc_tx_rate_limit

DESCRIPTION
  This function limits the vocoder transmit rate to ( 2s+1 ) / ( 2s+2 ).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_voc_tx_rate_limit(
  word rate_factor
)
{
  (void)voc_tx_rate_limit(rate_factor);
}

#endif  /* MSMAUD_VOC_IS127 || MSMAUD_VOC_IS733 || MSMAUD_VOC_4GV
                                                || MSMAUD_VOC_4GV_WB*/

/* <EJECT> */
#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION mvssup_get_g711_mode

DESCRIPTION
  This function gets the current G.711 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
mvs_g711_mode_type mvssup_get_g711_mode( void )
{
  return(mvs_ul_g711_mode);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION mvssup_set_g711_mode

DESCRIPTION
  This function sets the G.711 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvssup_set_g711_mode(
  mvs_g711_mode_type mvs_g711_mode
)
{
  voc_g711_mode_type voc_g711_mode = VOC_G711_MODE_MAX;

  /* Check input parameter */
  if((mvs_g711_mode < MVS_G711_MODE_MULAW) ||
     (mvs_g711_mode >= MVS_G711_MODE_MAX)) {
    MSG_ERROR("Invalid uplink G711 mode setting %d", mvs_g711_mode, 0, 0);
    return;
  }

  /* Convert MVS param to VOC param */
  switch (mvs_g711_mode) {
    case MVS_G711_MODE_MULAW:
      voc_g711_mode = VOC_G711_MODE_MULAW;
      break;

    case MVS_G711_MODE_ALAW:
      voc_g711_mode = VOC_G711_MODE_ALAW;
      break;

    case MVS_G711_MODE_MAX:
    default :
      MSG_ERROR("Invalid VOC UL G711 mode %d.", mvs_g711_mode, 0, 0);
      return;
  }

  /* store new G711 mode */
  mvs_ul_g711_mode = mvs_g711_mode;

  /* set new G711 mode */
  if (voc_set_g711_mode(voc_g711_mode) != VOC_DONE_S) {
    MSG_ERROR("VOC UL G711 mode %d setting failed.", voc_g711_mode, 0, 0);
    return;
  }

  MSG_MED("New G711 mode: %d", mvs_g711_mode, 0, 0);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* MSMAUD_VOC_G711 */

/* <EJECT> */
#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION mvssup_get_g723_mode

DESCRIPTION
  This function gets the current G.723.1 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mvs_g723_mode_type mvssup_get_g723_mode( void )
{
  return(mvs_ul_g723_mode);
}

/*===========================================================================

FUNCTION mvssup_set_g723_mode

DESCRIPTION
  This function sets the G.723.1 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_set_g723_mode(
  mvs_g723_mode_type mvs_g723_mode
)
{
  voc_gsm_frame_rate_type voc_g723_mode = VOC_G723_MODE_MAX;

  /* Check input parameter */
  if((mvs_g723_mode < MVS_G723_MODE_HIGH) ||
     (mvs_g723_mode >= MVS_G723_MODE_MAX)) {
    MSG_ERROR("Invalid uplink G723 mode setting %d", mvs_g723_mode, 0, 0);
    return;
  }

  /* Convert MVS param to VOC param */
  switch (mvs_g723_mode) {
    case MVS_G723_MODE_HIGH:
      voc_g723_mode = VOC_G723_MODE_HIGH;
      break;

    case MVS_G723_MODE_LOW:
      voc_g723_mode = VOC_G723_MODE_LOW;
      break;

    case MVS_G723_MODE_SID:
    case MVS_G723_MODE_BLANK:
    case MVS_G723_MODE_MAX:
    default :
      MSG_ERROR("Invalid VOC UL G723 mode %d.", mvs_g723_mode, 0, 0);
      return;
  }

  /* store new G723 mode */
  mvs_ul_g723_mode = mvs_g723_mode;

  /* set new G723 mode */
  if (voc_set_g723_mode(voc_g723_mode) != VOC_DONE_S) {
    MSG_ERROR("VOC UL G723 mode %d setting failed.", voc_g723_mode, 0, 0);
    return;
  }

  MSG_MED("New G723 mode: %d", mvs_g723_mode, 0, 0);
}
#endif /* MSMAUD_VOC_G723 */

#ifdef FEATURE_QDJ_TIMEWARP
/*===========================================================================

FUNCTION mvssup_set_timewarp

DESCRIPTION
  This function sets the timewarp factor for the current frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_set_timewarp(
  boolean enable,
  uint16  factor
) {
  voc_set_timewarp(enable, factor);
}

#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */

#ifdef MSMAUD_VOC_4GV_WB_VOIP
/*===========================================================================

FUNCTION mvssup_set_4gv_wb_timewarp

DESCRIPTION
  This function sets the phase factors for the current frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_set_4gv_wb_timewarp(
  boolean  enable,
  uint16   run_length,
  uint16   phase_offset
) {
  voc_set_4gv_wb_timewarp(enable, run_length, phase_offset);
}
#endif /* MSMAUD_VOC_4GV_WB_VOIP */
#endif /* FEATURE_QDJ_TIMEWARP */

void mvssup_set_voc_timing (void)
{
}

#endif  /* FEATURE_MVS */
