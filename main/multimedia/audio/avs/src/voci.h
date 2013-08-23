#ifndef VOCI_H
#define VOCI_H
/*===========================================================================

                   Q D S P 2   V O C O D E R   D R I V E R
                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions internal to the VOC task.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

Copyright (c) 1999 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/voci.h#8 $ $DateTime: 2009/10/22 23:49:33 $ $Author: kmodak $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/23/09   knm     Fixed compilation issue with SCMM specific featurization.
05/05/09   knm     Added APIs to set & check headphone connection status.
03/10/09   sj      Added slowtalk feature support
11/10/08    sk     Modified the code to resolve the compilation issue for
                   check_interlock().The prototype has defined under
                   FEATURE_VOC_DTMF_DETECT earlier.
04/11/08   skr     Changes for fixing CR 142714 and CR 141860
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
01/07/08    az     Included the in call midi and the new in call architecture.
11/14/07    hs     Added QEnsemble support
08/28/07   ymc     Added PCM attribute.
06/08/07    sg     Added voc_bt_a2dp_link_not_disabled() indicate is A2DP is
                   is some stage of active use.
05/10/07    sk     Fixed complier warnings. 
05/07/07    sp     Added code so that SBC encoder is reneabled if the DSP
                   state changes to from QTUNES -> QTUNES_INIT ->...-> QTUNES.
                   Fix for CR 117660.
02/26/07   kan     Added AMR-WB changes 
31/10/06  sp/anb   Serialized the commands sent to BT, Fixes 97271
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
07/17/06    hz     Moved voc_get_desired_config to voc.h, since 
                   mvs needs to use it to check incall.
06/15/06   sk/sp   Fixed CR 95512.Added voc_sbc_disable() declaration. 
02/17/06    pl     Modified VOC_ADEC_CMD_RETRY_SIG to 0x80000
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/19/06    sp     Fixed a bug that could cause A2DP not to be disconnected
                   when MIDI playback ends.
09/23/05    sp     Fixed a featurization error.
09/18/05    sp     Added support for DSP based SBC encoder and BT config retry 
                   mechanism.
09/18/05    pl     Added support for audio decoder command retry mechanism.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/16/05    hz     Added support for Vocoder image+ADPCM+VFE.
04/18/05    sm     Added GSM/WCDMA TTY support.
04/12/05    sg     Additional state for BT configuration.
03/02/05    aw     Added support for Qconcert.
01/25/05    st     Removed FEATURE_BREW_3_0_VOC_SIG.
01/07/05   act     Added support for FEATURE_VOC_NV_CAL.
12/03/04    sm     Added FEATURE_VOC_DTMF_DETECT. Brew 3.0 support.
10/14/04    sg     Added support for ADEC flush retry timer.
08/19/04    sg     Added VOC_CONFIG_STANDBY to voc_cmd_type.
06/08/04    aw     Added VOC_EQ_RETRY_TIMER_SIG for equalizer.
06/01/04    aw     Added voc_restore_comfort_noise function prototype.
05/04/04    sm     Added VOC_PMIC_SPKR_DELAY_SIG.
03/04/04    sg     Removed unused API.
01/26/04    sg     Added voc_sleep() API.
10/10/03    aw     Added MSMAUD_VOC_AMR_COMMON featurization.
08/21/03   ymc     Added voc_get_bt_ag_state.
05/13/03   ymc     Added GSM half rate support.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    sg     Added support for vocoder PCM interfaces.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
02/19/03    st     Added VOC_UNLOCK_GRAPH and VOC_UNLOCK_GRAPH_AUDIO.  
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
11/18/02    st     Featurized GSM/WCDMA specific code; added
                   voc_to_qdsp_gsm_frate() function prototype.
11/06/02    sm     Added QCP AMR support.
09/09/02   ymc     Added FEATURE_MVS.
09/06/02    st     Added voc_bt_ag_state_enum_type; modified voc_check_bt_ag()
                   function prototype.
08/23/02    sg     Modified to use FEATURE_EXTERNAL_SDAC.
08/22/02    sg     Added support for TTY activity status indication.
                   (Merged from MSM_AVS.01.05.18.01.02).
08/13/02   sg/st   Added support for MP4 and AAC.
07/11/02    yl     Added the new field in voc_rec_frame_packet_type.
03/21/02    st     Modified SDAC timer code to reuse for internal codec in 
                   support for MSMAUD_CODEC_CLOCK_CTRL feature.
03/12/02    st     Added VOC_UNLOCK_VOC_NOT_RESET to voc_unlock_type.
12/12/01    aw     Use msmaud.h to eliminate targetization.
09/03/01    st     Added support for recording a vocoder frame.
06/18/01  jkl/ym   Added FEATURE_ACP.
06/05/01    sm     Added prototype for voc_tty_cmd().
04/11/01   jkl     Added voc_retry_codec_gain().
03/04/01    st     Added voc_check_bt_ag() function prototype.
02/21/01    vl     Added MSM5100 support.
12/04/00    st     Added QTUNES support.
11/06/00    st     Renamed VOC_STATE_RETRY_SIG to VOC_STATE_GP_SIG.
10/17/00   ymc     Temporarily put MSMHW_INTERNAL_STEREO_DAC definition here
                   until msmhw.h file is provided in PPG archive.
08/28/00    ro     Uses QDSP Services. Removed voc_offline, voc_online support.
                   Removed VOC_STATE_CLOCK_SIG.
08/16/00   jkl     Added unlock types for DSP interlock.
06/11/00   spf     Updated references.
06/07/00    st     Added voc_enable_codec_func() and voc_disable_codec_func()
                   function prototypes.
05/16/00    st     Added voc_set_codec_retry_timer()  and 
                   voc_retry_codec_select() function prototypes; added 
                   VOC_CODEC_RETRY_TIMER_SIG; added stereo DAC support.
05/12/00   spf     Add TTY support: Add VOC_SET_TTY_MODE to voc_cmd_type,
                   voc_tty_mode_packet_type, add tty_mode to voc_packet_type.
01/25/00    ro     Added support for voc_vr_set_mode
                   Removed check_state command support.
01/21/00   spf     Added VOC_STATE_CLOCK_SIG.
10/26/99   jkl     Added VOC_STATE_RETRY_SIG. 
10/07/99   spf     Removed VOC_STATE_RETRY_SIG.
09/23/99   spf     Added VOC_STATE_RETRY_SIG.
09/16/99   spf     Removed FEATURE_QDSP2.
09/10/99   spf     voc_online, voc_offline support.
07/15/99   spf     DFM support.
06/17/99   spf     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"   /* Customer specific definitions                   */
#include "target.h"     /* Target specific definitions                     */
#include "msmaud.h"     /* Target specific audio features                  */
#include "voc.h"        /* Vocoder Interface Definitions                   */
#include "qdspcmd.h"    /* QDSP Interface Definitions                      */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "queue.h"      /* Definitions for queue services                  */

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

/* <EJECT> */
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* --------------------------------------------------------------------------
** Vocoder Task Signals
** ------------------------------------------------------------------------*/
#ifdef FEATURE_AUDFMT_QCONCERT
#define VOC_QCONCERT_RETRY_TIMER_SIG 0x1000   /* QConcert control retry timer */
#endif /* FEATURE_AUDFMT_QCONCERT */

#ifdef FEATURE_GASYNTH_INTERFACE
#define VOC_ADEC_CMD_RETRY_SIG       0x80000  /* ADEC command retry timer     */
#endif /* FEATURE_GASYNTH_INTERFACE */

#ifdef FEATURE_VOC_NV_CAL
#define VOC_NV_SIG                   0x20000  /* NV wait signal               */
#endif /* FEATURE_VOC_NV_CAL */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

/* DO NOT USE THESE SIGNALS -- They are reserved as common signals for all
** tasks

#define  TASK_START_SIG           0x8000
#define  TASK_STOP_SIG            0x4000
#define  TASK_OFFLINE_SIG         0x2000

*/
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
#define VOC_BT_CONFIG_RETRY_TIMER_SIG   0x40000
#endif

#ifdef FEATURE_VOC_DTMF_DETECT
#define VOC_DTMF_DETECT_DELAY_SIG 0x0800   /* DTMF detect delay signal */
extern rex_timer_type voc_dtmf_detect_delay_timer;
#endif /* FEATURE_VOC_DTMF_DETECT */

#ifdef FEATURE_QTUNES_COMMON
#define VOC_FLUSH_RETRY_TIMER_SIG 0x0400   /* Decoder flush retry timer     */
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_AUDFMT_EQUALIZER
#define VOC_EQ_RETRY_TIMER_SIG    0x0200   /* Equalizer control retry timer */
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_MVS
#define VOC_MVS_RESERVED_SIG      0x0100   /* Reserved in mvsi.h     */
#endif /* FEATURE_MVS */

#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
#define VOC_CLK_CTRL_TIMER_SIG    0x0080   /* Clock control timer    */
#endif

#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
#define VOC_PMIC_SPKR_DELAY_SIG   0x0040   /* PMIC speaker timer     */
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */

#define VOC_CODEC_RETRY_TIMER_SIG 0x0020   /* Command retry timer    */
#define VOC_STATE_GP_SIG          0x0010   /* General purpose signal */
#define VOC_SAT_DCC_SIG           0x0008   /* SAT DCC update signal  */
#define VOC_CHECK_STATE_SIG       0x0004   /* Check State Signal     */
#define VOC_CMD_Q_SIG             0x0002   /* Command Queue Signal   */
#define VOC_RPT_TIMER_SIG         0x0001   /* Watchdog report timer  */

/* General purpose timer */
extern rex_timer_type voc_state_gp_timer;

/* Absolute value macro  */
#define VOCABS(x) (((x) < 0) ? (-(x)) : (x))

/* --------------------------------------------------------------------------
** Vocoder Task Commands
** ------------------------------------------------------------------------*/

/* Voc command type
*/
typedef enum 
{                        /* (parameter) Brief description                    */
  VOC_ACQUIRE,           /* Acquire (activate) the vocoder                   */
  VOC_CONFIG,            /* Configure the vocoder service option             */
  VOC_CONFIG_STANDBY,    /* Configure the service option, and stay idle      */
#ifdef FEATURE_PHONE_VR
  VOC_SET_VR_MODE,       /* Set the voice recognition mode                   */
#endif  
#ifdef FEATURE_VOICE_RECORD
  VOC_RECORD_FRAME,      /* One recorded vocoder frame                       */
#endif
#ifdef FEATURE_TTY
  VOC_SET_TTY_MODE,      /* Set the TTY mode                                 */
#ifdef FEATURE_TTY_ACTIVITY_STATUS
  VOC_TTY_ACTIVITY_STATUS,/* Update TTY activity status                      */
#endif
#endif  
  VOC_RELEASE,           /* Release (deactivate) the vocoder                 */
#ifdef FEATURE_VOC_PCM_INTERFACE
  VOC_REGISTER_PCM_CLIENT, /* Register a PCM client                          */
#endif /* FEATURE_VOC_PCM_INTERFACE */
#ifdef FEATURE_VOC_DTMF_DETECT
  VOC_DTMF_DETECTED,     /* Detected DTMF                                    */
#endif /* FEATURE_VOC_DTMF_DETECT */
  VOC_INVALID            /* (Leave Last) Invalid Command - Causes Error      */
} voc_cmd_type;

/* Voc interlock type
*/
typedef enum 
{                          /* (parameter) Brief description                  */
  VOC_UNLOCK_NOT_RESET,    /* IDLE, DFM, or VOCODER state                    */
  VOC_UNLOCK_VOC_NOT_RESET,/* Not reset with vocoder compatible image        */
  VOC_UNLOCK_MODEM,        /* DFM or VOCODER state                           */
  VOC_UNLOCK_VOCODER       /* VOCODER state                                  */
#ifdef FEATURE_VOC_ADPCM
  ,VOC_UNLOCK_VOCODER_ADPCM/* VOCODER ADPCM state                            */
#endif /* FEATURE_VOC_ADPCM */
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_QSYNTH_COMMON
  ,VOC_UNLOCK_QSYNTH       /* QSYNTH state                                   */
  ,VOC_UNLOCK_QSYNTH_STD   /* Check whether Stand-alone SYNTH available      */   
#endif  
#ifdef FEATURE_QTUNES_COMMON
  ,VOC_UNLOCK_QTUNES       /* QTUNES state                                   */
#endif  
#ifdef FEATURE_GRAPH_AUDIO
  ,VOC_UNLOCK_GRAPH        /* GRAPH_IDLE or GRAPH_AUDIO states               */
  ,VOC_UNLOCK_GRAPH_AUDIO  /* GRAPH_AUDIO state                              */
#endif
#ifdef FEATURE_AVS_INCALL_ADEC_PB
  ,VOC_UNLOCK_VOCODER_ADEC  /* VOCODER+ADEC state                            */
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  ,VOC_UNLOCK_MIXER_ADEC    /*                                               */
#endif /* FEATURE_AVS_INCALL_IMXER_ADEC_PB */
} voc_unlock_type;

/* Header for commands
*/
typedef struct 
{
  q_link_type         link;             /* Command queue link                */
  voc_cmd_type        cmd;              /* VOC command to perform            */

} voc_hdr_type;

/* Voc Acquire Packet
*/  
typedef struct 
{
  voc_hdr_type        hdr;              /* Command Header                    */
  voc_acquire_type    key;              /* The entity that is acquiring      */
  voc_state_cb_type   cb_ptr;           /* Function to call on state changes */

} voc_acquire_packet_type;

/* Config Packet
*/  
typedef struct 
{
  voc_hdr_type        hdr;              /* Command Header                    */
  voc_acquire_type    key;              /* The entity that is configuring    */
  voc_capability_type option;           /* The service option to set         */

} voc_config_packet_type;

#ifdef FEATURE_PHONE_VR
/* Set VR Mode Packet
*/  
typedef struct 
{
  voc_hdr_type      hdr;                /* Command Header                    */
  voc_acquire_type  key;                /* The entity setting the VR mode    */
  voc_vr_mode_type  vr_mode;            /* The voice recognition mode        */

} voc_vr_mode_packet_type;
#endif  

#ifdef FEATURE_VOICE_RECORD
/* Recorded frame command packet
*/
typedef struct
{
  voc_hdr_type hdr;            /* Command Header                          */
  voc_rec_type dir;            /* Path (rx/tx) the frame is recorded from */
  uint16       frame_rate;     /* Rate of the frame                       */
  uint32       auto_stop_ms;   /* Silence time in ms to stop the VM       */
  uint8        *packet;        /* Frame data                              */
} voc_rec_frame_packet_type;
#endif /* FEATURE_VOICE_RECORD */

#ifdef FEATURE_TTY
/* Set TTY Mode Packet
*/  
typedef struct 
{
  voc_hdr_type      hdr;                /* Command Header                    */
  voc_tty_mode_type tty_mode;           /* The TTY mode                      */

} voc_tty_mode_packet_type;

#ifdef FEATURE_TTY_ACTIVITY_STATUS
/* TTY Activity Status Packet
*/  
typedef struct 
{
  voc_hdr_type      hdr;            /* Command Header                        */
  boolean           encoder_status; /* TRUE if encoder is active, else FALSE */
  boolean           decoder_status; /* TRUE if decoder is active, else FALSE */

} voc_tty_activity_packet_type;
#endif /* FEATURE_TTY_ACTIVITY_STATUS */
#endif /* FEATURE_TTY */

/* Release Packet
*/
typedef struct 
{
  voc_hdr_type        hdr;              /* Command Header                    */
  voc_acquire_type    key;              /* The entity that is releasing      */

} voc_release_packet_type;

#ifdef FEATURE_VOC_PCM_INTERFACE
/* Register PCM client
*/
typedef struct
{
  voc_hdr_type           hdr;                   /* Command Header            */
  voc_pcm_interface_type interface;             /* PCM interface to register */
  voc_pcm_interface_attrib_type *pcm_attrib;

  union {
    void* func;
    voc_pcm_client_output_fn_type output_func;  /* Output data callback func */
    voc_pcm_client_input_fn_type  input_func;   /* Input data callback func  */
  } client_func;
  
} voc_register_pcm_client_type;
#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_VOC_DTMF_DETECT
/* Detected a DTMF */
typedef struct {
  voc_hdr_type                   hdr;           /* Command Header            */
  boolean                        rx_detected;   /* detected RX? ow TX        */
  voc_dtmf_detected_status_type  dtmf;          /* key detected              */
} voc_dtmf_detected_type;
#endif /* FEATURE_VOC_DTMF_DETECT */

/* Voc command packet union
*/
typedef union 
{
  voc_hdr_type               hdr;       /* Command Header                    */
  voc_acquire_packet_type    acquire;   /* Acquire the vocoder               */
  voc_config_packet_type     config;    /* Configure the vocoder             */
#ifdef FEATURE_PHONE_VR
  voc_vr_mode_packet_type    vr_mode;   /* Set the voice recognition mode    */
#endif  
#ifdef FEATURE_VOICE_RECORD
  voc_rec_frame_packet_type  rec_frame; /* Recorded frame data               */
#endif
#ifdef FEATURE_TTY
  voc_tty_mode_packet_type   tty_mode;  /* Set the TTY mode                  */

#ifdef FEATURE_TTY_ACTIVITY_STATUS
  voc_tty_activity_packet_type tty_status; /* TTY Activity status            */
#endif
#endif /* FEATURE_TTY */
  voc_release_packet_type    release;   /* Release the vocoder               */

#ifdef FEATURE_VOC_PCM_INTERFACE
  voc_register_pcm_client_type pcm_client; /* Register PCM data client       */
#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_VOC_DTMF_DETECT
  voc_dtmf_detected_type     dtmf_detected;/* return detected DTMF           */
#endif /* FEATURE_VOC_DTMF_DETECT */
} voc_packet_type;

#ifdef FEATURE_TTY_ACTIVITY_STATUS
/* Callback pointer to indicate TTY status */
extern voc_tty_status_fn_type voc_tty_status_func;
#endif

#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
/* Enum type to indicate which clock source to control
*/
typedef enum {
  VOC_CLK_CTRL_INTERNAL_CODEC = 0x0001,       /* Internal codec clock ctrl */
  VOC_CLK_CTRL_STEREO_DAC     = 0x0002        /* Stereo DAC clock ctrl     */
} voc_clk_ctrl_enum_type;
#endif /* FEATURE_EXTERNAL_SDAC || MSMAUD_CODEC_CLOCK_CTRL */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

typedef enum {
  VOC_BT_RETRY_TIMER_NONE,
  VOC_BT_RETRY_TIMER_CODEC_CONFIG, 
#ifdef FEATURE_BT_AG
  VOC_BT_RETRY_TIMER_AG_ENABLE,
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  VOC_BT_RETRY_TIMER_A2DP_ENABLE,
#endif
#ifdef FEATURE_SBC_DSP_CODEC
  VOC_BT_RETRY_TIMER_SBC_CONFIG,
#endif
  VOC_BT_RETRY_TIMER_MAX
}voc_bt_config_retry_timer_type;

typedef enum {
  VOC_BT_STATE_DISABLED,                  /* BT A2DP/AG disabled              */
#ifdef FEATURE_BT_AG
  VOC_BT_STATE_AG_SCO_LINK_PENDING,       /* BT AG sco link enable pending    */
  VOC_BT_STATE_AG_CODEC_CONFIG_PENDING,   /* BT AG Codec config pending       */
  VOC_BT_STATE_AG_ENABLE_PENDING,         /*  DSP is not in proper state      */
  VOC_BT_STATE_AG_ENABLED,                /* BT AG enabled                    */
  VOC_BT_STATE_AG_DISABLE_PENDING,        /* BT AG sco link disable pending    */ 
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  VOC_BT_STATE_A2DP_LINK_PENDING,         /* BT A2DP link enable pending      */
  VOC_BT_STATE_A2DP_CODEC_CONFIG_PENDING, /* BT A2DP codec config pending     */
  VOC_BT_STATE_A2DP_ENABLE_PENDING,      /*  DSP is not in proper state       */
  VOC_BT_STATE_A2DP_ENABLED,              /* BT A2DP enabled                  */
  VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING,   /* DSP has not yet accepted the cmd */
  VOC_BT_STATE_A2DP_DISABLE_PENDING,      /* BT A2DP link disable pending    */ 
#endif /* FEATURE_SBC_CODEC */
  VOC_BT_STATE_INVALID
} voc_bt_state_enum_type;

/* This enum is used to indicate the state of the Bluetooth AG or
   A2DP connection.
*/
typedef enum {
  VOC_BT_MODE_NONE,
  VOC_BT_MODE_AG,
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  VOC_BT_MODE_A2DP
#endif /* FEATURE_SBC_CODEC */
}voc_bt_mode_type;

/* This enum is used to indicate the action to take in regards to the
** Bluetooth Audio gateway/A2DP.
*/
typedef enum {
  VOC_BT_OP_FORCE_DISABLE,     /* Disable AG or A2DP whichever is current */
  VOC_BT_OP_NORMAL,
#ifdef FEATURE_BT_AG
  VOC_BT_OP_AG_FORCE_DISABLE,  /* Force disable of audio gateway */
  VOC_BT_OP_AG_NORMAL,         /* Perform normal operation       */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
   VOC_BT_OP_A2DP_FORCE_DISABLE,  /* Force disable of A2DP */
  VOC_BT_OP_A2DP_NORMAL,         /* Perform normal operation       */
#endif /* FEATURE_SBC_CODEC */
  VOC_BT_OP_INVALID
} voc_bt_op_type;
#endif

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

/* This enum is used to indicate different frequency plan
   available in ICODEC
*/
typedef enum {
  VOC_ICODEC_FREQ_PLAN_NONE,
  VOC_ICODEC_FREQ_PLAN_8KHZ       = 8000,
  VOC_ICODEC_FREQ_PLAN_11P025KHZ  = 11025,
  VOC_ICODEC_FREQ_PLAN_12KHZ      = 12000,
  VOC_ICODEC_FREQ_PLAN_16KHZ      = 16000,
  VOC_ICODEC_FREQ_PLAN_22P050KHZ  = 22050,
  VOC_ICODEC_FREQ_PLAN_24KHZ      = 24000,
  VOC_ICODEC_FREQ_PLAN_32KHZ      = 32000,
  VOC_ICODEC_FREQ_PLAN_44KHZ      = 44000,
  VOC_ICODEC_FREQ_PLAN_44P1KHZ    = 44100,
  VOC_ICODEC_FREQ_PLAN_48KHZ      = 48000,
  VOC_ICODEC_FREQ_PLAN_MAX
}voc_icodec_freq_plan_enum_type;

/* This enum is used to indicate different OSR Rate
   available in ICODEC
*/
typedef enum {
  VOC_ICODEC_OSR_RATE_64    = 64,
  VOC_ICODEC_OSR_RATE_128   = 128,
  VOC_ICODEC_OSR_RATE_256   = 256
}voc_icodec_osr_rate_enum_type;

/* <EJECT> */
/*===========================================================================

        I N T E R N A L   V O C O D E R   T A S K   F U N C T I O N S
  
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_cmd

DESCRIPTION
  Queue a command for processing by the Vocoder (VOC) Task.

DEPENDENCIES
  This routine must not be called until the VOC Task has been started.

RETURN VALUE
  None

SIDE EFFECTS
  voc_cmd_q

===========================================================================*/
extern void voc_cmd ( voc_packet_type *voc_cmd_ptr );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_pkt

DESCRIPTION
  Get a VOC Command Packet.  Use by callers of voc_cmd().
  
DEPENDENCIES
  voc_task_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available voc_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  voc_free_q (and the packet fetched off voc_free_q)

===========================================================================*/
extern voc_packet_type *voc_get_pkt (void);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_cmd_done

DESCRIPTION
  Dispose of a command which is done being processed.

DEPENDENCIES
  done_q_ptr

RETURN VALUE
  None

SIDE EFFECTS
  Can signal the task which is associated with the command.

===========================================================================*/
extern void voc_cmd_done( voc_packet_type *voc_cmd_ptr );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_process_cmd

DESCRIPTION
  This procedure processes commands received by voc_cmd().  See voci.h for
  a description of the vocoder packets.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void voc_process_cmd( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_task_init

DESCRIPTION
  This procedure initializes the VOC task.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  voc_tcb

===========================================================================*/
extern void voc_task_init (void);

#ifdef FEATURE_VOC_NV_CAL
/* <EJECT> */
/*===========================================================================

FUNCTION voc_nv_wait

DESCRIPTION
  Wait on NV actions.  Handle signals as they come in just in case NV
  takes a while.  Only time critical watchdog signals are acknowledged.

DEPENDENCIES
  There is a command sent to NV, and this task will be signalled.  There
  are no other outstanding NV requests from this task.

RETURN VALUE
  None

SIDE EFFECT
  Some signals are delayed till we're done with NV

===========================================================================*/
extern void voc_nv_wait (void);
#endif /* FEATURE_VOC_NV_CAL */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_DFM_SAT_DCC_CB

DESCRIPTION
  This callback function is registered with the QDSP command interface
  layer when voc_init_analog() is called.  Once registered, this callback
  will be invoked whenever a SAT DCC interrupt occurs.  If SAT DCC update
  notification is enabled, this callback signal the vocoder task to process
  the SAT DCC update.

DEPENDENCIES
  voc_init_analog() must be called for this callback to be registered.

RETURN VALUE
  voc_scc_type - the SAT DCC

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_dfm_sat_dcc_cb( uint16 qdsp_sat_dcc );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_codec_retry_timer

DESCRIPTION
  This function sets and/or clears the codec config retry timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_set_codec_retry_timer( boolean set_timer );

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE
/*==========================================================================

FUNCTION voc_set_adec_cmd_retry_timer

DESCRIPTION
  This function sets or clear timer to retry writing adec command
  to DSP 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_set_adec_cmd_retry_timer( boolean set_timer );

/* <EJECT> */
/*==========================================================================

FUNCTION voc_qsynth_adec_cmd_retry

DESCRIPTION
  This function attempts to write adec command to DSP again because the adec 
  command was queued up due to DSP in lock state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_qsynth_adec_cmd_retry( void );
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_retry_codec_select

DESCRIPTION
  This function configures to the current codec.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_retry_codec_select ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_retry_codec_gain

DESCRIPTION
  This function configures to the current codec gain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_retry_codec_gain ( void );

/* <EJECT> */
#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
/*===========================================================================

FUNCTION voc_check_codec_config

DESCRIPTION
  This function verifies if the codec config command has been executed; if
  so, it will disable the indicated clocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean voc_check_codec_config ( uint32 clk_mask );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_clk_ctrl_timer

DESCRIPTION
  This function sets and/or clears the clock disable timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_set_clk_ctrl_timer(
  boolean                set_timer,
  voc_clk_ctrl_enum_type mask
);
#endif /* FEATURE_EXTERNAL_SDAC || MSMAUD_CODEC_CLOCK_CTRL */
/* <EJECT> */
/*===========================================================================

FUNCTION voc_enable_codec_func

DESCRIPTION
  Enables codec with registered codec function of current path.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_enable_codec_func (void);
/* <EJECT> */
/*===========================================================================

FUNCTION voc_disable_codec_func

DESCRIPTION
  Disables all codecs with registered codec functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void voc_disable_codec_func (
  boolean disable_current_path
    /* TRUE: disables all paths; FALSE: disables all but current path */
);

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_bt_state

DESCRIPTION
  This function get the status of the vocoder bluetooth AG/A2DP.

DEPENDENCIES
  None.

RETURN VALUE
  BT connection state as listed in voc_bt_state_enum_type
SIDE EFFECTS
  None.

===========================================================================*/
extern voc_bt_state_enum_type voc_get_bt_state( void );
/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_bt_mode

DESCRIPTION
  This function get the BT mode that we are in. 

DEPENDENCIES
  None.

RETURN VALUE

  VOC_BT_MODE_NONE
  VOC_BT_MODE_AG
  VOC_BT_MODE_A2DP

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_bt_mode_type voc_get_bt_mode( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_bt

DESCRIPTION
  This is a wrapper function for voc_check_bt_ag and voc_check_bt_a2dp. This 
  function will call the appropriate check function depending on the current
  BT state. This will also do some error check to see if BT_AG or BT_A2DP
  could be enabled depending on the current codec.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_check_bt (
  voc_bt_op_type operation
);
/* <EJECT> */
/*===========================================================================

FUNCTION voc_retry_bt_config

DESCRIPTION
  This function retries the BT configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_retry_bt_config ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_bt_config_retry_timer

DESCRIPTION
  This function sets and/or clears the BT config retry timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_set_bt_config_retry_timer ( 
  voc_bt_config_retry_timer_type timer
);
#endif /* defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) */

#ifdef FEATURE_BT_AG
/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_bt_ag

DESCRIPTION
  Enables/disables Bluetooth Audio gateway. This is the ONLY place that
  the vocoder is ever configured to the BT codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_check_bt_ag (
  voc_bt_op_type operation
);

#endif /* FEATURE_BT_AG */

/* <EJECT> */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/*===========================================================================

FUNCTION voc_check_bt_a2dp

DESCRIPTION
  Enables/disables Bluetooth A2DP. This is the ONLY place that
  the vocoder is ever configured to the BT SBC codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean voc_check_bt_a2dp (
  voc_bt_op_type operation
);

/* <EJECT> */                                                                                                     
/*===========================================================================

FUNCTION voc_sbc_disable

DESCRIPTION
  This function disables the DSP based SBC encoder
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_sbc_disable(void);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_bt_a2dp_link_not_disabled

DESCRIPTION
  Checks is the A2DP link is not disabled

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if not disabled; FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
voc_bt_state_enum_type voc_bt_a2dp_link_not_disabled( void );
#endif /* (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC) */

#ifdef FEATURE_SBC_DSP_CODEC
/* <EJECT> */                                                                                                     
/*===========================================================================

FUNCTION voc_sbc_reenable

DESCRIPTION
  This function re-enables the DSP based SBC encoder if the DSP is in proper
  state to accept this command.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_sbc_reenable(void);

#endif /* FEATURE_SBC_DSP_CODEC */

/* <EJECT> */
#ifdef FEATURE_VOICE_RECORD
/*===========================================================================

FUNCTION VOC_REC_FRAME_PROCESSING

DESCRIPTION
  This function accepts the recorded frame and saves it in a buffer.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_rec_frame_processing (
  voc_rec_type dir,
  uint16       frame_rate,
  uint8        *frame
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REC_FRAME

DESCRIPTION
  This function takes the recorded frame and issues a command to have it
  processed.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_rec_frame (
  qdsp_cmd_rec_dir_enum_type dir,
  uint16                     frame_rate,
  uint16                     packet_length,
  uint16                     *frame
);
#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
#ifdef FEATURE_TTY

#ifdef FEATURE_TTY_ACTIVITY_STATUS
/*===========================================================================

FUNCTION voc_tty_activity_status

DESCRIPTION
  Updates the TTY activity status flags.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_tty_activity_status (
  boolean encoder_status, /* TRUE if encoder is active, FALSE otherwise */
  boolean decoder_status  /* TRUE if decoder is active, FALSE otherwise */
);
#endif /* FEATURE_TTY_ACTIVITY_STATUS */

/*===========================================================================

FUNCTION VOC_TTY_CMD

DESCRIPTION
  This function sets the TTY mode.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S -

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_tty_cmd ( void );

#endif /* FEATURE_TTY */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_WB_COMMON
/*===========================================================================

FUNCTION voc_to_qdsp_amr_wb_ftype

DESCRIPTION
  This function can convert voc_amr_wb_frame_type into
  qdsp_cmd_amr_wb_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_amr_wb_fid_ptr: holding converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_to_qdsp_amr_wb_ftype
(
  voc_amr_wb_frame_type voc_amr_wb_frame_id,
  qdsp_cmd_amr_wb_frame_type *qdsp_amr_wb_fid_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_amr_wb_ftype

DESCRIPTION
  This function can convert qdsp_cmd_amr_wb_frame_type into
  voc_amr_wb_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_amr_wb_fid_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type qdsp_to_voc_amr_wb_ftype
(
  qdsp_cmd_amr_wb_frame_type qdsp_amr_wb_frame_id,
  voc_amr_wb_frame_type *voc_amr_wb_fid_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_to_qdsp_amr_wb_frate

DESCRIPTION
  This function can convert voc_amr_wb_frame_rate_type into
qdsp_cmd_amr_wb_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_amr_wb_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_to_qdsp_amr_wb_frate
(
  voc_amr_wb_frame_rate_type voc_amr_wb_frame_rate,
  qdsp_cmd_amr_wb_frame_rate_type *qdsp_amr_wb_frate_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_amr_wb_frate

DESCRIPTION
  This function can convert qdsp_cmd_amr_wb_frame_rate_type into
voc_amr_wb_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_amr_wb_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type qdsp_to_voc_amr_wb_frate
(
  qdsp_cmd_amr_wb_frame_rate_type qdsp_amr_wb_frame_rate,
  voc_amr_wb_frame_rate_type *voc_amr_wb_frate_ptr
);

#endif  /* MSMAUD_VOC_AMR_WB_COMMON */


/* <EJECT> */
#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR)
/*===========================================================================

FUNCTION voc_to_qdsp_gsm_ftype

DESCRIPTION
  This function can convert voc_gsm_frame_type into qdsp_cmd_gsm_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_gsm_fid_ptr: holding converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_to_qdsp_gsm_ftype
(
  voc_gsm_frame_type voc_gsm_frame_id,
  qdsp_cmd_gsm_frame_type *qdsp_gsm_fid_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_to_qdsp_gsm_frate

DESCRIPTION
  This function can convert voc_gsm_frame_rate_type into
qdsp_cmd_gsm_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_gsm_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_to_qdsp_gsm_frate(
  voc_gsm_frame_rate_type       voc_gsm_frame_rate,
  qdsp_cmd_gsm_frame_rate_type  *qdsp_gsm_frate_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_gsm_ftype

DESCRIPTION
  This function can convert qdsp_cmd_gsm_frame_type into voc_gsm_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_gsm_fid_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type qdsp_to_voc_gsm_ftype
(
  qdsp_cmd_gsm_frame_type qdsp_gsm_frame_id,
  voc_gsm_frame_type *voc_gsm_fid_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_gsm_frate

DESCRIPTION
  This function can convert qdsp_cmd_gsm_frame_rate_type into
voc_gsm_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_gsm_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type qdsp_to_voc_gsm_frate
(
  qdsp_cmd_gsm_frame_rate_type qdsp_gsm_frame_rate,
  voc_gsm_frame_rate_type *voc_gsm_frate_ptr
);
#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR || 
          MSMAUD_VOC_FR || MSMAUD_VOC_HR */

/* <EJECT> */
#if defined(FEATURE_PHONE_VR) || defined(FEATURE_VOICE_RECORD)
/*===========================================================================

FUNCTION VOC_RESTORE_COMFORT_NOISE

DESCRIPTION
  This function restore the comfort noise value after voice recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_restore_comfort_noise( void );
#endif /* FEATURE_PHONE_VR || FEATURE_VOICE_RECORD */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION VOC_UPDATE_EQUALIZER

DESCRIPTION
  This function will update the QSYNTH/QTUNES equalizer parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_update_equalizer( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_equalizer_retry_timer

DESCRIPTION
  This function sets and/or clears the eqaulzier retry timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_equalizer_retry_timer
(
  boolean enable
);
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION voc_update_qconcert_config

DESCRIPTION
  This function will update the QSYNTH/QTUNES qconcert parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void voc_update_qconcert_config( void );

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_qconcert_retry_timer

DESCRIPTION
  This function sets and/or clears the qconcert retry timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_qconcert_retry_timer
(
  boolean enable
);
#endif /* FEATURE_AUDFMT_QCONCERT */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_audfmt_pp_config

DESCRIPTION
  This function setup the pro-processig blocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
void voc_set_audfmt_pp_config(void);

/* <EJECT> */
/*==========================================================================

FUNCTION voc_set_audfmt_pp_cal

DESCRIPTION
  This function setup the pro-processig calibration blocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
void voc_set_audfmt_pp_cal(void);

#ifdef FEATURE_QTUNES_COMMON
/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_adec_flush_retry_timer

DESCRIPTION
  This function sets and/or clears the adec flush retry timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_set_adec_flush_retry_timer( boolean set_timer );
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_VOC_DTMF_DETECT

/* Callbacks registered through voc_dtmf_detect(). */
extern voc_dtmf_detect_cb_type voc_rx_detect_cb;
extern voc_dtmf_detect_cb_type voc_tx_detect_cb;

/*===========================================================================

FUNCTION VOC_DTMF_DETECT_ENABLE

DESCRIPTION
  This function sends command to enable/disable DTMF detection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_dtmf_detect_enable (void);

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_DTMF_DETECT_DELAY_CB

DESCRIPTION
  This function is called after a delay time to ensure the same keys are not
    re-detected continuously.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_dtmf_detect_delay_cb (void);
/* <EJECT> */
/*===========================================================================

FUNCTION VOC_DTMF_DETECTED

DESCRIPTION
  Use this function to respond to a client that is detecting DTMFs
    (in task context).

DEPENDENCIES
  The client must acquire the Vocoder before using this interface.

RETURN VALUE
  VOC_DONE_S       if successful.
  VOC_CMD_FAILED_S if the client's input parameters were incorrect.

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_dtmf_detected(
  boolean                        rx_detected,
  voc_dtmf_detected_status_type  dtmf
);
#endif /* FEATURE_VOC_DTMF_DETECT */

/*===========================================================================

FUNCTION check_interlock

DESCRIPTION
  Check whether the DSP is in correct state to accept commands.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - DSP is in the proper state.
  FALSE - DSP is not in the proper state.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean check_interlock (voc_unlock_type check);

/*===========================================================================

FUNCTION voc_update_slowtalk_config

DESCRIPTION
  This function update the qconcert parameters value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
void voc_update_slowtalk_config( void );

#ifdef FEATURE_ULC_WARMUP_DELAY
/*===========================================================================

FUNCTION voc_get_headphone_delay

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern timetick_type voc_get_headphone_delay ( 
  void
);

/*===========================================================================

FUNCTION voc_set_headphone_status

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern void voc_set_headphone_status (
  voc_codec_type  pcm_path 
);

#endif /* FEATURE_ULC_WARMUP_DELAY */

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

#endif /* VOCI_H */

