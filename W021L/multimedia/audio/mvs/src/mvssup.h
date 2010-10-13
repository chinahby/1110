#ifndef MVSSUP_H
#define MVSSUP_H

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                 S U P P L E M E N T A R Y   D E F I N I T I O N S

DESCRIPTION
  This header file contains supplementary functions and data definitions for
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

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvssup.h#3 $ $DateTime: 2009/07/13 05:47:29 $ $Author: rmukul $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/09    rm     Adding support for 4GV_WB_VoIP.
05/14/09    sj     Added stub function mvssup_set_voc_timing() to resolve
                   compilation issues on SCMM.
02/26/07   kan     Added AMR-WB changes 
xx/xx/xx    sm     Added prototypes for get SCR and DTX functions.
02/03/06    sm     Added 4GV VOIP support
01/16/06    sm     Added FEATURE_QDJ_TIMEWARP
07/28/05    sm     G723 fixes.
07/22/05   ayu     Added AMR IF1 support.
07/05/05    sm     Added G.723.1 support.
04/28/05   ymc     Added 4GV support.
02/20/05    sm     Added G.711 vocoder support.
01/28/05   ymc     Added support for MVS_MODE_LINEAR_PCM. Removed MSMAUD deps
01/17/05   ymc     Fixed AMR IF2 featurization.  
           ymc     Updated Copyright.
11/17/04   ymc     Removed support for MSMAUD_VOC_WVRC and MSMAUD_VOC_SMV.
09/17/04   ymc     Added IS733 and IS127 support.
           ymc     Added interfaces to create and operate MVS frame queues.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
           ymc     Updated Copyright.
09/16/03   ymc     Added support for WCDMA AMR rate change.
06/29/03   ymc     Updated mvssup_wcdma_encode_amrif2 and
                   mvssup_wcdma_decode_amrif2 interfaces to return frame length.
04/24/03   ymc     Added mvssup_channel_class_type.
           ymc     Added channel class parameter to mvssup_wcdma_set_dl_channel.
           ymc     Removed mvssup_get_dl_amr_mode.
04/17/03    sm     Moved AMR structures to mvsamr.h
02/14/03   ymc     Added MVS_AMR_SPEECH_DEGRADED and MVS_AMR_ONSET support.
           ymc     Removed next_mode in mvs_amr_ul_frame_info_type.
           ymc     Removed amr_dl_mode in mvssup_amr_set_amr_mode.
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

#ifdef FEATURE_MVS

#include "mvsamr.h"
#if defined(FEATURE_WCDMA)
#error code not present
#endif  /* FEATURE_WCDMA */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Definitions of QCELP-13k and EVRC-8k voice frame information */
typedef enum {
  MVS_VOC_0_RATE,       /* blank frame   */
  MVS_VOC_8_RATE,       /* 1/8 rate      */
  MVS_VOC_4_RATE,       /* 1/4 rate      */
  MVS_VOC_2_RATE,       /* 1/2 rate      */
  MVS_VOC_1_RATE,       /* full rate     */
  MVS_VOC_ERASURE,      /* erasure frame */
  MVS_VOC_RATE_MAX,
  MVS_VOC_RATE_UNDEF = MVS_VOC_RATE_MAX
} mvs_voc_rate_type;

#if defined(FEATURE_WCDMA)
#error code not present
#endif  /* FEATURE_WCDMA */

/* Definitions of EFR and FR voice frame information */
/* GSM EFR and FR frame type definitions */
typedef enum {
  MVS_GSM_SID,          /* GSM FR or EFR : silence descriptor  */
  MVS_GSM_SPEECH_GOOD,  /* GSM FR or EFR : good speech frame   */
  MVS_GSM_BFI,          /* GSM FR or EFR : bad frame indicator */
  MVS_GSM_FRAME_MAX,
  MVS_GSM_FRAME_UNDEF = MVS_GSM_FRAME_MAX  /* undefined */
} mvs_gsm_frame_type;

/* GSM frame rate definitions */
typedef enum {
  MVS_GSM_RATE_EFR,     /* GSM EFR mode */
  MVS_GSM_RATE_FR,      /* GSM FR mode */
  MVS_GSM_RATE_MAX,
  MVS_GSM_RATE_UNDEF = MVS_GSM_RATE_MAX  /* undefined */
} mvs_gsm_rate_type;

/* Definitions of HR voice frame information */
typedef enum {
  MVS_HR_SID,           /* silence descriptor         */
  MVS_HR_SPEECH_GOOD,   /* good speech frame          */
  MVS_HR_SPEECH_BAD,    /* bad speech frame           */
  MVS_HR_INVALID_SID,   /* invalid silence descriptor */
  MVS_HR_FRAME_MAX,
  MVS_HR_FRAME_UNDEF = MVS_HR_FRAME_MAX  /* undefined */
} mvs_hr_frame_type;

/* SCR mode type and DTX mode type definitions */
/* SCR mode type definitions */
typedef enum {
  MVS_SCR_DISABLE,      /* Disable TX SCR operation */
  MVS_SCR_ENABLE        /* Enable TX SCR operation  */
} mvs_scr_mode_type;

/* GSM DTX mode type definitions */
typedef enum {
  MVS_DTX_OFF,          /* disable encoder DTX mode */
  MVS_DTX_ON            /* enable encoder DTX mode  */
} mvs_dtx_mode_type;

/* G.711 vocoder data format */
typedef enum {
  MVS_G711_MODE_MULAW,  /* G.711, mulaw format */
  MVS_G711_MODE_ALAW,   /* G.711, alaw format  */
  MVS_G711_MODE_MAX
} mvs_g711_mode_type;

/* G.723 vocoder data format */
typedef enum {
  MVS_G723_MODE_HIGH,   /* G.723.1, 6.3kbits    */
  MVS_G723_MODE_LOW,    /* G.723.1, 5.3kbits    */
  MVS_G723_MODE_SID,    /* G.723.1, SID frame   */
  MVS_G723_MODE_BLANK,  /* G.723.1, blank frame */
  MVS_G723_MODE_ERROR,  /* Error, no data recd  */
  MVS_G723_MODE_MAX
} mvs_g723_mode_type;


/* Definitions of MVS mode type */
typedef enum {
  MVS_MODE_NONE,
  MVS_MODE_IS733,       /* QCELP-13k                    */
  MVS_MODE_IS127,       /* EVRC-8k                      */
  MVS_MODE_4GV_NB,      /* 4GV narrow band              */
  MVS_MODE_4GV_WB,      /* 4GV wide band                */
#ifdef FEATURE_ACP
#error code not present
#endif
  MVS_MODE_AMR,         /* Adaptive multi-rate          */
  MVS_MODE_EFR,         /* Enhanced full rate           */
  MVS_MODE_FR,          /* Full rate                    */
  MVS_MODE_HR,          /* Half rate                    */
  MVS_MODE_LINEAR_PCM,  /* enable/disable PCM interface */
  MVS_MODE_G711,        /* G.711 vocoder                */
  MVS_MODE_G723,        /* G.723 vocoder                */
  MVS_MODE_AMR_WB,       /* AMR-WB vocoder               */
  MVS_MODE_MAX
} mvs_mode_type;


/* ==========================================================================
** Definitions of MVS frame information (frame rate and frame mode)
** ==========================================================================
*/

typedef struct {
 mvs_mode_type mode;
} mvs_frame_info_hdr_type;


/* QCELP-13k and EVRC-8k and 4GV frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_voc_rate_type rate;
} mvs_voc_tx_frame_info_type;

typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_voc_rate_type  rate;
  struct {
    boolean          enable;
    uint16           factor;
    boolean          phase_enable;
    uint16           run_length;
    uint16           phase_offset;
  } timewarp;
} mvs_voc_rx_frame_info_type;

typedef union {
  mvs_voc_tx_frame_info_type tx_info;
  mvs_voc_rx_frame_info_type rx_info;
} mvs_voc_frame_info_type;

/* AMR frame info type */
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_amr_frame_type frame;
  mvs_amr_mode_type mode;
} mvs_amr_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_amr_frame_type frame;
  mvs_amr_mode_type mode;
} mvs_amr_dl_frame_info_type;

typedef union {
  mvs_amr_ul_frame_info_type ul_info;
  mvs_amr_dl_frame_info_type dl_info;
} mvs_amr_frame_info_type;

/* GSM EFR and FR frame info type */
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_gsm_frame_type frame;
  mvs_gsm_rate_type rate;
} mvs_gsm_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_gsm_frame_type frame;
  boolean taf;                     /* time allignment flag */
} mvs_gsm_dl_frame_info_type;

typedef union {
  mvs_gsm_ul_frame_info_type ul_info;
  mvs_gsm_dl_frame_info_type dl_info;
} mvs_gsm_frame_info_type;

/* HR frame info type */
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_hr_frame_type frame;
  boolean enc_err_flag;
} mvs_hr_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_hr_frame_type frame;
  boolean taf;
  boolean bfi;
  boolean ufi;
} mvs_hr_dl_frame_info_type;

typedef union {
  mvs_hr_ul_frame_info_type ul_info;
  mvs_hr_dl_frame_info_type dl_info;
} mvs_hr_frame_info_type;

/* G.711 frame info structure */
/* Uplink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g711_mode_type  g711_mode;
} mvs_g711_ul_frame_info_type;

/* Downlink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g711_mode_type  g711_mode;
} mvs_g711_dl_frame_info_type;

typedef union {
  mvs_g711_ul_frame_info_type  ul_info;
  mvs_g711_dl_frame_info_type  dl_info;
} mvs_g711_frame_info_type;

/* G.723 frame info structure */
/* Uplink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g723_mode_type  g723_mode;
} mvs_g723_ul_frame_info_type;

/* Downlink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g723_mode_type  g723_mode;
} mvs_g723_dl_frame_info_type;

typedef union {
  mvs_g723_ul_frame_info_type  ul_info;
  mvs_g723_dl_frame_info_type  dl_info;
} mvs_g723_frame_info_type;

/* PCM frame info type */
#ifdef FEATURE_VOC_PCM_INTERFACE 
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  void *info;            /* unused for now */
} mvs_pcm_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  void *info;            /* unused for now */
} mvs_pcm_dl_frame_info_type;

typedef union {
  mvs_pcm_ul_frame_info_type ul_info;
  mvs_pcm_dl_frame_info_type dl_info;
} mvs_pcm_frame_info_type;
#endif  /* FEATURE_VOC_PCM_INTERFACE */

typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_amr_frame_type_index_type frame_type_index;
  boolean fqi;   /* Frame Quality Indicator: TRUE: good frame, FALSE: bad frame */
} mvs_amr_if1_ul_frame_info_type;

typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_amr_frame_type_index_type frame_type_index;
  boolean fqi;   /* Frame Quality Indicator: TRUE: good frame, FALSE: bad frame */
} mvs_amr_if1_dl_frame_info_type;

typedef union {
  mvs_amr_if1_ul_frame_info_type ul_info;
  mvs_amr_if1_dl_frame_info_type dl_info;
} mvs_amr_if1_frame_info_type;

/* Generic MVS frame info type definitions */
typedef union {
  mvs_frame_info_hdr_type   hdr;
  mvs_voc_frame_info_type   voc_rate;
  mvs_amr_frame_info_type   amr_rate;
  mvs_gsm_frame_info_type   gsm_rate;
  mvs_hr_frame_info_type    hr_rate;
  mvs_g711_frame_info_type  g711_rate;
  mvs_g723_frame_info_type  g723_rate;
#ifdef FEATURE_VOC_PCM_INTERFACE
  mvs_pcm_frame_info_type   pcm_rate;
#endif  /* FEATURE_VOC_PCM_INTERFACE */
  mvs_amr_if1_frame_info_type amr_if1_info;
} mvs_frame_info_type;


/* Define MVS general voice frame queue interface */
#define MVS_VOC_PKT_SIZE 34              /* 34 bytes per vocoder packet */
#define MVS_PCM_PKT_SIZE 320             /* 320 bytes per PCM packet */
#define MVS_VOC_G711_PKT_SIZE 160        /* 160 bytes per G711 packet */
#define MVS_VOC_G723_PKT_SIZE 24         /* 24  bytes per G723 packet */
#define MVS_VOC_AWB_PKT_SIZE 60          /* 60 bytes per AMR-WB packet */

typedef struct {
  uint8 frame_data[MVS_PCM_PKT_SIZE];    /* Should be largest of all lb pkts */
  mvs_frame_info_type frame_info;
  uint16 frame_len;                      /* no. of bytes of frame_data */
} mvs_frame_type;

typedef struct {
  mvs_frame_type *frame_q;
  int q_size;
  int frame_cnt;
  int curr_frame;
  int miss_cnt;
  int lost_cnt;
} mvs_frame_q_type;
  
/* Definitions of MVS packet status type */
typedef enum {
  MVS_PKT_STATUS_NORMAL,
  MVS_PKT_STATUS_FAST,    /* packets were supplied too fast */
  MVS_PKT_STATUS_SLOW     /* packets were supplied too slowly */
} mvs_pkt_status_type;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

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
extern void mvssup_frame_q_init(
  mvs_frame_q_type *frame_q_ptr,
  mvs_frame_type *frames_buf,
  int size
);

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
extern void mvssup_frame_q_empty(
  mvs_frame_q_type *frame_q_ptr
);

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
extern boolean mvssup_frame_check_q_full(
  mvs_frame_q_type *frame_q_ptr
);

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
extern boolean mvssup_frame_check_q_empty(
  mvs_frame_q_type *frame_q_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_q_put

DESCRIPTION
  This function will put a frame into the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_pkt_status_type mvssup_frame_q_put(
  mvs_frame_q_type *frame_q_ptr,
  uint8 *frame_data,
  mvs_frame_info_type *frame_info,
  uint16 frame_len
);
  
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
extern mvs_pkt_status_type mvssup_frame_q_get(
  mvs_frame_q_type *frame_q_ptr,
  uint8 *frame_data,
  mvs_frame_info_type *frame_info,
  uint16 *frame_len
);

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
extern int mvssup_umts_encode_amrif1(
  uint8               *vocoder_packet,
  mvs_frame_info_type *frame_info,
  uint8               *if1_frame,
  mvs_frame_info_type *if1_frame_info
);

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
extern int mvssup_umts_decode_amrif1(
  uint8               *if1_frame,
  mvs_frame_info_type *if1_frame_info,
  uint8               *vocoder_packet,
  mvs_frame_info_type *frame_info
);

#endif /* FEATURE_AMR_IF1 */

/* <EJECT> */
#if defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_WCDMA */

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
extern int mvssup_wcdma_encode_amrif2(
  uint8                *vocoder_packet,
  mvs_frame_info_type  *frame_info,
  uint8                *if2_frame
);

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
extern int mvssup_wcdma_decode_amrif2(
  uint8 *if2_frame,
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);

#endif  /* FEATURE_AMR_IF2 */

/* <EJECT> */
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
extern void mvssup_amr_set_awb_mode(
  mvs_amr_mode_type amr_ul_mode
);

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
extern mvs_amr_mode_type mvssup_get_ul_awb_mode(void);


/* <EJECT> */
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
extern void mvssup_amr_set_amr_mode(
  mvs_amr_mode_type amr_ul_mode
);

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
extern mvs_amr_mode_type mvssup_get_ul_amr_mode(void);


/* <EJECT> */
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
extern void mvssup_amr_set_scr_mode(
  mvs_scr_mode_type scr_mode
);

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
extern mvs_scr_mode_type mvssup_amr_get_scr_mode(void);

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
extern void mvssup_gsm_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
);

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
extern mvs_dtx_mode_type mvssup_gsm_get_dtx_mode(void);

/* <EJECT> */

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
extern mvs_voc_rate_type mvssup_get_voc_max_rate(void);

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
extern mvs_voc_rate_type mvssup_get_voc_min_rate(void);

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
extern void mvssup_voc_set_frame_rate(
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
);

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
extern void mvssup_voc_tx_rate_limit(
  word rate_factor
);

/* <EJECT> */
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
extern mvs_g711_mode_type mvssup_get_g711_mode( void );

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
extern void mvssup_set_g711_mode(
  mvs_g711_mode_type mvs_g711_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_get_g723_mode

DESCRIPTION
  This function gets the current G.723 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_g723_mode_type mvssup_get_g723_mode( void );

/*===========================================================================

FUNCTION mvssup_set_g723_mode

DESCRIPTION
  This function sets the G.723 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_set_g723_mode(
  mvs_g723_mode_type mvs_g723_mode
);

#endif  /* FEATURE_MVS */

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
extern void mvssup_set_timewarp(
  boolean enable,
  uint16  factor
);

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
extern void mvssup_set_4gv_wb_timewarp(
  boolean  enable,
  uint16   run_length,
  uint16   phase_offset
);
#endif /* MSMAUD_VOC_4GV_WB_VOIP */

#endif /* FEATURE_QDJ_TIMEWARP */

extern void mvssup_set_voc_timing (void);

#endif  /* MVSSUP_H */

