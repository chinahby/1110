/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Sound Hardware Routines (2nd/3rd Generation)")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         S O U N D   T E S T  R O U T I N E S

GENERAL DESCRIPTION
  These routines exercise the functionality and operability characteristics
  of the Sound Server.

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  snd_test_code
    Optionally compiled Hardware Test routine.

Copyright(c) 1999 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndtest.c#17 $ $DateTime: 2009/05/22 09:25:30 $ $Author: nrchella $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/09   nr	   Replaced SND_DEVICE_HANDSET with SND_DEVICE_CURRENT for
				   MVS loopback
03/10/09    sj      Added slowtalk feature support
12/26/08    rm     Added RVE API changes
12/23/08    vm     Added test code for AUXPCM testing support
11/17/08   knm     Moved the aux_line_in flags to snd_test_block
10/22/08    dp     FM over BT AG changes
10/17/08    vm     added idlemetring support for qtv image
09/11/08    kk     Fixed compiler errors for thinUI build SMBRLT for 6801.
08/07/08    sj     Fixed the compiler errors thrown by RVCT2.2 compiler
07/09/08    sk     Fixed Featurzation to avoid compilation issue when
                   FEATURE_AVS_CRYSTAL_SPEECH is not enabled and
                   SND_TEST_CODEC_OVERRIDE is defined.
06/20/08   knm     Added test code for AuxPGA Line In path configuration
06/17/08    sj     Fixed the compiler errors thrown by RVCT2.2 compiler
04/15/08    sj     Added sndtest code support for overriding any codec type
                   with other codec. One use case is overriding
                   on_chip_0_codec with dual_mic_codec
01/08/08    kk     Fixed compilation issues in snd_set_qensemble_params().
11/22/07   vsud    Removed banned-api sprintf and replaced with std_strlprintf.
11/20/07   vss     Featurized code to do adie ssbi write/read test.
11/14/07    hs     Added QEnsemble support
10/26/07    hs     Added EVW file format support
10/29/07    sr     Fixed featurization issue of snd_test_enable_flag
10/29/07    sr     Created snd_test_enable_flag , default value = false
08/27/07   ymc     Forced PCM data buffer to be 160 samples only as
                   sndtest PCM interface is not supporting other PCM format yet.
04/20/07    at     Added EVB file format recording test support.
03/15/07    kan    Added 4GV-WB Code
02/26/07   kan     Added AMR-WB changes
09/05/06    ay     Added AMR-WB+ test support.
                   Updated copyright text.
04/03/06    ak     Added support to test sidetone control.
03/16/06   anb     Modified to support sbc re-configuration given the encoded params
                   CR 78581,87334.Added audiosbcenc_data_type variable to test the
				   same.
01/30/06    sp     Added support for I2S SBC interface.
09/15/05    sk     Added test code for FEATURE_AVS_WB_PCM_RX_INTERFACE
08/29/05    sp     Made changes so that SBC paramters could be changed for
                   testing purposes.
08/10/05    sm     Fixed another FEATURE_AAC definition.
07/22/05    sm     Fixed FEATURE_AAC definition.
07/05/05    sm     Added G.723.1 support.
05/20/05   hz      Added support for AAC recording test.
04/28/05   ymc     Added 4GV NB vocoder support.
03/18/05   act     Corrected problem with GPIO control when the GPIO register
                   was GPIO_REG_0.
03/18/05   act     Added direct-to-register ADIE control.
02/22/05    sm     Added G.711 vocoder support.
02/15/05   act     Added GPIO control and Tx AGC control.
12/03/04    sm     Added FEATURE_AUDIO_CALIBRATION
11/19/04    sp     Added support for Arm Post Processing.
10/14/04   ymc     Enhanced MVS loopback test functionalities and included
                   support for IS127 and IS733 modes.
01/30/04    sg     Moved sndtest.h inclusion so that SND_TEST_CODE needs to be
                   defined only in sndtest.h, to use the test code.
01/17/04   ymc     Included "err.h" to fix compilation error for
                   FEATURE_VOC_PCM_INTERFACE_TEST.
           ymc     Updated Copyright.
12/08/03    sg     Modified to support changes in the vocoder PCM interface.
05/13/03   ymc     Added GSM half rate support.  Added d2e_loop.
           ymc     Removed WVS support.
           ymc     Updated copyright.
04/17/03    sm     Removed extra parm in mvssup_amr_set_amr_mode().
            sg     Added test code for vocoder PCM interfaces.
10/17/02   ymc     Changed VOC_ACQ_MVS into VOC_ACQ_TEST.
           ymc     Moved AMR mode and DTX mode settings out of MVS callback fn.
09/18/02   ymc     Added MVS test code.
09/06/02    sg     Added TTY activity status test code.
01/15/02   ymc     Deleted VOC_API_VERSION.  VOC_API_VERSION_1 is no longer
                   supported.
           ymc     Updated copyright.
10/08/01    st     Removed voice playback test functions.
07/24/01   ymc     Added snd_test_wvs_enable.
                   Replaced snd_test_block Row 9 hfk test functions
                   with WVS loop test implementation; wvs_loop and wvs_enable.
                   Included wvs.h.
                   Conditionally include DFM support under FEATURE_ACP.
                   Added support for loopback tests.
07/14/00   spf     Added #include <string.h>.
11/15/99    st     Removed const qualifier from voice prompt header.
10/27/99   spf     Updated to use new VOC_API_VERSION definitions.
10/26/99   jkl     Added PCM Pad value to write full range.
10/14/99   jkl     Added tests to dfm, audio loop back and TX rate limit.
10/04/99   jkl     Added test to voc_put_pcmpad().
09/23/99   jkl     Replaced VOC_CAP_ISXXXX for VOC_CAP_IS733
09/09/99    st     Included AGC tests
08/24/99    st     Added casting to get rid of compiler errors.
08/17/99   jkl     Created from sndhwg2.c.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* In case test code is not compiled in */
#include "target.h"             /* Target specific definitions          */
#include "sndtest.h"            /* Sound Test definitions and prototype */

#ifdef SND_TEST_CODE

#include <string.h>             /* Interface to string library          */
#include "qdspcmd.h"
#include "voc.h"
#ifdef SND_AVBAP_SUPPORTED
#error code not present
#endif
#ifdef FEATURE_MVS
#include "mvs.h"
#endif  /* FEATURE_MVS */

#if defined(FEATURE_VOC_PCM_INTERFACE) && \
    defined(FEATURE_VOC_PCM_INTERFACE_TEST)
#include <stdio.h>
#include "fs.h"
#include "ts.h"
#include "vocstate.h"
#endif /* FEATURE_VOC_PCM_INTERFACE && FEATURE_VOC_PCM_INTERFACE_TEST */

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(SND_TEST_CODEC_OVERRIDE)
#include <stdio.h>
#include "fs.h"
#endif /* FEATURE_ARM_POST_PROCESSING || SND_TEST_CODEC_OVERRIDE */

#include "err.h"

#include "cmx.h"

#if defined(FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST) && \
    defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)      && \
    defined(FEATURE_EFS)
#include "fs.h"                 /* File system typedefs and prototypes */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST &&
          FEATURE_AMR_WBPLUS_AUDIO_DEC      &&
          FEATURE_EFS                       */

#ifdef MSMAUD_ADIE_SSBI_IO_TEST
#include "adie.h"
#include "adie_sbi.h"
#endif /* MSMAUD_ADIE_SSBI_IO_TEST */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_MVS
/***************************************************************************/
/* Declarations to test MVS interfaces                                     */
/***************************************************************************/

/* Definitions used to control Multimode Voice Services */
typedef enum {
  SND_TEST_MVS_FAILED,
  SND_TEST_MVS_OFF,
  SND_TEST_MVS_ACQUIRE,
  SND_TEST_MVS_MODE_SELECT,
  SND_TEST_MVS_MODE_READY,
  SND_TEST_MVS_ON
} snd_test_mvs_status_type;

snd_test_mvs_status_type snd_test_mvs_status = SND_TEST_MVS_OFF;

#define SND_TEST_MVS_FRAME_Q_SIZE 100   /* 2 sec buffer */
mvs_frame_type snd_test_mvs_frame_buf[SND_TEST_MVS_FRAME_Q_SIZE];
mvs_frame_q_type snd_test_mvs_frame_q;
int snd_test_mvs_delay;

mvs_mode_type snd_test_mvs_mode = MVS_MODE_NONE;

#ifdef MSMAUD_VOC_AMR
mvs_amr_mode_type snd_test_amr_mode = MVS_AMR_MODE_1220;
#endif

#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR)
mvs_dtx_mode_type  snd_test_dtx_mode = MVS_DTX_ON;
mvs_scr_mode_type  snd_test_scr_mode = MVS_SCR_ENABLE;
#endif

#if defined(MSMAUD_VOC_IS127) || defined(MSMAUD_VOC_IS733) \
    || defined(MSMAUD_VOC_4GV)  || defined(MSMAUD_VOC_4GV_WB)
mvs_voc_rate_type snd_test_max_voc_rate = MVS_VOC_1_RATE;
mvs_voc_rate_type snd_test_min_voc_rate = MVS_VOC_8_RATE;
#endif  /* MSMAUD_VOC_IS127 || MSMAUD_VOC_IS733 || MSMAUD_VOC_4GV
|| MSMAUD_VOC_4GV_WB */

#ifdef MSMAUD_VOC_G711
mvs_g711_mode_type snd_test_g711_mode = MVS_G711_MODE_MULAW;
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
mvs_g723_mode_type snd_test_g723_mode = MVS_G723_MODE_HIGH;
#endif /* MSMAUD_VOC_G723 */

#endif  /* FEATURE_MVS */

boolean snd_test_enable_flag = FALSE;

#if defined(FEATURE_VOC_PCM_INTERFACE) && \
    defined(FEATURE_VOC_PCM_INTERFACE_TEST)
/***************************************************************************/
/* Declarations for testing Vocoder PCM interface                              */
/***************************************************************************/

#define SND_TEST_PCM_INPUT_FILE_NAME_LENGTH 30

/* List of PCM files which can be used as PCM input */
char snd_test_pcm_input_files[][SND_TEST_PCM_INPUT_FILE_NAME_LENGTH] = {
  "test",
};

#define SND_TEST_PCM_NUM_BUFFERS 200 /* 4 seconds worth */
#define SND_TEST_PCM_BUF_HWM 100     /* High Water mark - to request more
                                        data/store accumulated data */
/* sndtest PCM interface only supports NB mono mic at present */
#define SND_TEST_PCM_DATA_FRAME_LEN 160

#define SND_TEST_PCM_BLOCK_SIZE  (SND_TEST_PCM_BUF_HWM * \
                                  SND_TEST_PCM_DATA_FRAME_LEN*2)

uint32 snd_test_rxi_force_miss = 0;
uint32 snd_test_txi_force_miss = 0;

/* Data structure for processing each PCM channel */
typedef struct {
  fs_handle_type fs_handle;
  boolean        busy;
  uint16         buffer[SND_TEST_PCM_NUM_BUFFERS * SND_TEST_PCM_DATA_FRAME_LEN];
  uint32         rd_index;
  uint32         wr_index;
  uint32         num_bufs_used;
} snd_test_pcm_channel_type;

/* Master data structure for all channels */
struct {
  uint32                    bitmask;  /* Indicates which PCM Ints are ON */
  snd_test_pcm_channel_type channel[VOC_PCM_INTERFACE_MAX];
} snd_test_pcm_control;


/* Bitmask of output channels that are being logged */
LOCAL uint32 snd_test_pcm_log_data = 0;
/* Bitmask of input channels for which files are being read */
LOCAL uint32 snd_test_pcm_read_file = 0;

/* Output channel that is being looped back */
LOCAL voc_pcm_interface_type snd_test_pcm_loopback_src = VOC_PCM_INTERFACE_MAX;
/* Input channel that is being looped back */
LOCAL voc_pcm_interface_type snd_test_pcm_loopback_dest = VOC_PCM_INTERFACE_MAX;

#endif /* FEATURE_VOC_PCM_INTERFACE && FEATURE_VOC_PCM_INTERFACE_TEST */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#ifdef FEATURE_AAC
cmx_handle_type        aac_handle;
#ifdef FEATURE_MM_REC
cmx_mm_rec_param_type  aac_param;
#endif /* FEATURE_MM_REC */
cmx_codec_spec_type    adif_spec;

char* aac_file[CMX_AF_AAC_CHANNEL_DUAL+1]
                   [CMX_AF_SAMPLE_RATE_48000+1]
                   [80] =
{
  {
    {{"none"}},
    {{"none"}},
    {{"none"}},
    {{"none"}},
    {{"none"}},
    {{"none"}},
    {{"none"}},
    {{"none"}},
    {{"none"}},
    {{"none"}}
  },

  {
    {{"none"}},
    {{"brew/mod/mediaplayer/media/aac_m_8k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_11P025k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_12k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_16k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_22P050k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_24k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_32k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_44P10k.aac"}},
    {{"brew/mod/mediaplayer/media/aac_m_48k.aac"}}
  },

  {
    {{"none"}},
	{{"brew/mod/mediaplayer/media/aac_s_8k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_11P025k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_12k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_16k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_22P050k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_24k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_32k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_44P10k.aac"}},
	{{"brew/mod/mediaplayer/media/aac_s_48k.aac"}}
  }
};
#endif /* FEATURE_AAC */

#ifdef FEATURE_AUDFMT_EVB
cmx_handle_type evb_handle;
cmx_qcp_rec_para_type cmx_qcp_rec_para;
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
cmx_handle_type evw_handle;
cmx_qcp_rec_para_type cmx_qcp_rec_para_evw;
#endif /* FEATURE_AUDFMT_EVB */

snd_test_block_type snd_test_block;

#ifdef  FEATURE_AVS_WB_PCM_RX_INTERFACE
void snd_test_rx_pcm_cb(snd_pp_buffer_type snd_test_pp_buffer);
snd_pp_buffer_type        snd_test_pp_buffer;
uint16                    snd_test_pp_test = 0xff;
#endif /* FEATURE_AVS_WB_PCM_RX_INTERFACE*/

#ifdef FEATURE_ARM_POST_PROCESSING

void snd_test_pp_cb(snd_pp_buffer_type snd_test_pp_buffer);
snd_pp_buffer_type        snd_test_pp_buffer;
uint16                    snd_test_pp_test = 0xff;
uint16                    snd_test_pp_file_out = 0xff;
uint16                    snd_test_pp_file_shift = 0;
fs_handle_type            snd_test_pp_fs_handle = NULL;
fs_rsp_msg_type           snd_test_pp_snd_fs_rsp;
fs_open_xparms_type       snd_test_pp_open_parms;
char                      snd_test_pp_file_name[20];
extern void mmu_invalidate_data_cache_lines( uint32* start_ptr ,
                                            uint32 num_bytes );
#endif /*FEATURE _ARM_POST_PROCESSING*/

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
snd_sbc_config_params_type  snd_test_sbc_config;
audiosbcenc_data_type       snd_test_audiosbcenc_config;
boolean                     snd_test_sbc_config_reqd = FALSE;

#define  SND_TEST_ENC_OCTET0   (AUDIOSBCENC_CHANNEL_MODE_JOINT_STEREO | \
								AUDIOSBCENC_SAMPLING_FREQUENCY_48000)

#define  SND_TEST_ENC_OCTET1   (AUDIOSBCENC_SUBBANDS_8 | \
								AUDIOSBCENC_BLOCK_LENGTH_4 | \
								AUDIOSBCENC_ALLOCATION_METHOD_LOUDNESS)


#endif

#ifdef FEATURE_SBC_CODEC
uint8 snd_test_sbc_blocks         = 0xff;
uint8 snd_test_sbc_mode           = 0xff;
uint8 snd_test_sbc_alloc          = 0xff;
uint8 snd_test_sbc_bitrate_hint   = 0xff;
uint8 snd_test_sbc_bands          = 0xff;

uint32 snd_test_sbc_bitrate       = 384000;
#endif


#ifdef FEATURE_AVS_I2SSBC
snd_sample_rate_type snd_test_i2ssbc_sample_rate = SND_SAMPLE_RATE_48000;
#endif /* FEATURE_AVS_I2SSBC */

#if defined(FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST) && \
    defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
uint8 snd_test_adec_buffer[2000000];
uint32 snd_test_adec_len = 0;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST && FEATURE_AMR_WBPLUS_AUDIO_DEC */

/***************************************************************************/
/* A predefined array that can be use to test Sound compact Tone Requests. */
/***************************************************************************/

#define SND_CANNED_TIME   1000

static snd_compact_tone_type snd_canned[] = {
  {SND_1             , {SND_CANNED_TIME} },
  {SND_2             , {SND_CANNED_TIME} },
  {SND_3             , {SND_CANNED_TIME} },
  {SND_4             , {SND_CANNED_TIME} },
  {SND_5             , {SND_CANNED_TIME} },
  {SND_6             , {SND_CANNED_TIME} },
  {SND_7             , {SND_CANNED_TIME} },
  {SND_8             , {SND_CANNED_TIME} },
  {SND_9             , {SND_CANNED_TIME} },
  {SND_POUND         , {SND_CANNED_TIME} },
  {SND_STAR          , {SND_CANNED_TIME} },
  {SND_CTRL          , {SND_CANNED_TIME} },
  {SND_2ND           , {SND_CANNED_TIME} },
  {SND_WARN          , {SND_CANNED_TIME} },
  {SND_ERR           , {SND_CANNED_TIME} },
  {SND_TIME          , {SND_CANNED_TIME} },
  {SND_RING_A        , {SND_CANNED_TIME} },
  {SND_RING_B        , {SND_CANNED_TIME} },
  {SND_RING_C        , {SND_CANNED_TIME} },
  {SND_RBACK         , {SND_CANNED_TIME} },
  {SND_BUSY          , {SND_CANNED_TIME} },
  {SND_INTERCEPT_A   , {SND_CANNED_TIME} },
  {SND_INTERCEPT_B   , {SND_CANNED_TIME} },
  {SND_REORDER_TONE  , {SND_CANNED_TIME} },
  {SND_PWRUP         , {SND_CANNED_TIME} },
  {SND_OFF_HOOK_TONE , {SND_CANNED_TIME} },
  {SND_CALL_WT_TONE  , {SND_CANNED_TIME} },
  {SND_DIAL_TONE_TONE, {SND_CANNED_TIME} },
  {SND_ANSWER_TONE   , {SND_CANNED_TIME} },
  {SND_HIGH_PITCH_A  , {SND_CANNED_TIME} },
  {SND_HIGH_PITCH_B  , {SND_CANNED_TIME} },
  {SND_MED_PITCH_A   , {SND_CANNED_TIME} },
  {SND_MED_PITCH_B   , {SND_CANNED_TIME} },
  {SND_LOW_PITCH_B   , {SND_CANNED_TIME} },
  {SND_LOW_PITCH_B   , {SND_CANNED_TIME} },
  {SND_TEST_ON       , {SND_CANNED_TIME} },
  {SND_MSG_WAITING   , {SND_CANNED_TIME} },
  {SND_PIP_TONE_TONE , {SND_CANNED_TIME} },
  {SND_SPC_DT_INDIA  , {SND_CANNED_TIME} },
  {SND_SIGNAL_INDIA  , {SND_CANNED_TIME} },
  {SND_DT_TONE_INDIA , {SND_CANNED_TIME} },
  {SND_DT_TONE_BRAZIL, {SND_CANNED_TIME} },
  {SND_DT_DTACO_TONE , {SND_CANNED_TIME} },
  {SND_STOP          , {SND_CANNED_TIME} }
};

const snd_compact_sound_type compact_snd_canned = {
  SND_COMPACT_SOUND, snd_canned
};
/* Flexible sound used by snd_sound_start */
static snd_flexible_tone_type snd_flexible[] = {
  {SND_FREQ         , {SND_CANNED_TIME} ,  100,    200},
  {SND_FREQ         , {SND_CANNED_TIME} ,  200,    300},
  {SND_FREQ         , {SND_CANNED_TIME} ,  300,    400},
  {SND_FREQ         , {SND_CANNED_TIME} ,  400,    500},
  {SND_FREQ         , {SND_CANNED_TIME} ,  500,    600},
  {SND_FREQ         , {SND_CANNED_TIME} ,  600,    700},
  {SND_FREQ         , {SND_CANNED_TIME} ,  700,    800},
  {SND_FREQ         , {SND_CANNED_TIME} ,  800,    900},
  {SND_FREQ         , {SND_CANNED_TIME} ,  900,   1000},
  {SND_FREQ         , {SND_CANNED_TIME} , 1000,   1100},
  {SND_FREQ         , {SND_CANNED_TIME} , 1100,   1200},
  {SND_FREQ         , {SND_CANNED_TIME} , 1200,   1300},
  {SND_FREQ         , {SND_CANNED_TIME} , 1300,   1400},
  {SND_FREQ         , {SND_CANNED_TIME} , 1400,   1500},
  {SND_FREQ         , {SND_CANNED_TIME} , 1500,   1600},
  {SND_FREQ         , {SND_CANNED_TIME} , 1600,   1700},
  {SND_FREQ         , {SND_CANNED_TIME} , 1700,   1800},
  {SND_STOP         , {SND_CANNED_TIME} ,    0,      0}
};

const snd_flexible_sound_type flexible_snd_flexible = {
  SND_FLEXIBLE_SOUND, snd_flexible
};

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

#ifdef FEATURE_AVS_I2SSBC
/*==========================================================================

FUNCTION SND_TEST_I2SSBC_CB_FUNC

DESCRIPTION
  Callback to receive I2S SBC status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_i2ssbc_cb_func(snd_i2ssbc_status_type status)
{
  MSG_HIGH("Received I2S SBC status %d", status, 0,0);
}
#endif /* FEATURE_AVS_I2SSBC */
/* <EJECT> */
#ifdef FEATURE_TTY_ACTIVITY_STATUS
/*==========================================================================

FUNCTION SND_TEST_TTY_ACTIVITY_STATUS_CB

DESCRIPTION
  Callback to receive TTY Activity status information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_tty_activity_status_cb (
  boolean enc,
  boolean dec
)
{
  MSG_ERROR("TTY Enc=%d, Dec=%d", (int)enc, (int)dec, 0);
}
#endif

/* <EJECT> */
#ifdef FEATURE_MVS
/*==========================================================================

FUNCTION SND_TEST_MVS_UL_CB

DESCRIPTION
  MVS uplink packet processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_mvs_ul_cb(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info,
  uint16 packet_length,
  mvs_pkt_status_type *status
)
{
  if ((vocoder_packet==NULL) || (frame_info==NULL))
  {
    MSG_ERROR("packet pointer error", 0, 0, 0);
    if (status!=NULL)
    {
      *status = MVS_PKT_STATUS_SLOW;
    }

    return;
  }


  *status = mvssup_frame_q_put(&snd_test_mvs_frame_q, vocoder_packet,
                               frame_info, packet_length);

  if (mvssup_frame_check_q_full(&snd_test_mvs_frame_q) != TRUE)
  {
    *status = MVS_PKT_STATUS_SLOW;   /* ask for more packets */
  }
}


/* <EJECT> */
/*==========================================================================

FUNCTION SND_TEST_MVS_DL_CB

DESCRIPTION
  MVS downlink packet processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_mvs_dl_cb(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info,
  mvs_pkt_status_type *status
)
{
  uint16 snd_test_frame_len;

  if(snd_test_mvs_delay > 0) {
    snd_test_mvs_delay--;
    *status = MVS_PKT_STATUS_SLOW;
    return;
  }

  if (mvssup_frame_check_q_empty(&snd_test_mvs_frame_q) == TRUE)
  {
    *status = MVS_PKT_STATUS_SLOW;
  }
  else
  {
    *status = mvssup_frame_q_get(&snd_test_mvs_frame_q, vocoder_packet,
                                 frame_info, &snd_test_frame_len);
  }


  if (*status==MVS_PKT_STATUS_NORMAL)
  {
    switch (snd_test_mvs_mode)
    {
#ifdef MSMAUD_VOC_EFR
#error code not present
#endif

#ifdef MSMAUD_VOC_FR
#error code not present
#endif

#ifdef MSMAUD_VOC_HR
      case MVS_MODE_HR :
        frame_info->hr_rate.dl_info.taf = FALSE;
        frame_info->hr_rate.dl_info.bfi = FALSE;
        frame_info->hr_rate.dl_info.ufi = FALSE;
        break;
#endif

      default :
        break;          /* uplink and downlink packet formats are the same. */
    }
  }
}


/* <EJECT> */
/*==========================================================================

FUNCTION SND_TEST_MVS_CB

DESCRIPTION
  Function handling MVS event callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_mvs_cb(mvs_event_type *event)
{
  if (event==NULL)
  {
    MSG_ERROR("NULL event pointer.", 0, 0, 0);
    return;
  }

  switch(event->hdr.event) {
    case MVS_EV_COMMAND:
      if(event->cmd.cmd_status == MVS_CMD_SUCCESS) {

        if(snd_test_mvs_status == SND_TEST_MVS_ACQUIRE) {

          MSG_LOW("Acquired MVS",0,0,0);
          snd_test_mvs_status = SND_TEST_MVS_MODE_SELECT;
          snd_test_mvs_delay = SND_TEST_MVS_FRAME_Q_SIZE;
          mvs_enable(MVS_CLIENT_TEST, snd_test_mvs_mode,
                     snd_test_mvs_ul_cb, snd_test_mvs_dl_cb,
                     MVS_PKT_CONTEXT_VOC);

        } else if(snd_test_mvs_status == SND_TEST_MVS_MODE_SELECT) {

          MSG_LOW("MVS mode command accepted",0,0,0);

        } else if(snd_test_mvs_status == SND_TEST_MVS_ON) {
        } else if(snd_test_mvs_status == SND_TEST_MVS_OFF) {

          MSG_LOW("Disabled MVS",0,0,0);

        }
      } else if(event->cmd.cmd_status == MVS_CMD_FAILURE) {
        MSG_ERROR("MVS command failed",0,0,0);
      } else if(event->cmd.cmd_status == MVS_CMD_BUSY) {
        MSG_ERROR("MVS busy, try later",0,0,0);
        snd_test_mvs_status = SND_TEST_MVS_OFF;
      }
      break;

    case MVS_EV_MODE:
      if(snd_test_mvs_status == SND_TEST_MVS_MODE_SELECT) {
        if(event->mode.mode_status == MVS_MODE_INIT) {
          MSG_LOW("Mode initializing",0,0,0);
        } else if(event->mode.mode_status == MVS_MODE_READY) {

          MSG_LOW("Selected MVS mode %d",snd_test_mvs_mode,0,0);
          snd_test_mvs_status = SND_TEST_MVS_ON;

          snd_set_device(SND_DEVICE_CURRENT,
                         SND_MUTE_UNMUTED,
                         SND_MUTE_UNMUTED,
                         NULL,
                         NULL);

          snd_set_volume(SND_DEVICE_CURRENT,
                         SND_METHOD_VOICE,
                         4,
                         NULL,
                         NULL);

        } else if(event->mode.mode_status == MVS_MODE_NOT_AVAIL) {
          MSG_ERROR("Mode not available",event->mode.mode,0,0);
          snd_test_mvs_status = SND_TEST_MVS_OFF;

        }
      } else {
        MSG_ERROR("Received mode in status %d",snd_test_mvs_status,0,0);
      }
      break;

    default:
      MSG_ERROR("Unknown event %d",event->hdr.event,0,0);
      break;
  }
}

/* <EJECT> */
/*==========================================================================

FUNCTION SND_TEST_MVS_ENABLE

DESCRIPTION
  This function is used to activate or de-activate MVS for testing
purposes.  When activated, handset device will be used.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_mvs_enable(boolean enable)
{
  static boolean enabled_flag = FALSE;


  if (enable==TRUE)
  {
    mvssup_frame_q_init(&snd_test_mvs_frame_q, snd_test_mvs_frame_buf,
                        SND_TEST_MVS_FRAME_Q_SIZE);

    if (enabled_flag == FALSE)
    {
      snd_test_mvs_status = SND_TEST_MVS_ACQUIRE;
      mvs_acquire(MVS_CLIENT_TEST, snd_test_mvs_cb);
    }
    else
    {
      snd_test_mvs_status = SND_TEST_MVS_MODE_SELECT;
      mvs_enable(MVS_CLIENT_TEST, snd_test_mvs_mode,
                 snd_test_mvs_ul_cb, snd_test_mvs_dl_cb,
                 MVS_PKT_CONTEXT_VOC);
    }

    enabled_flag = TRUE;
  }
  else
  {
    mvs_release(MVS_CLIENT_TEST);

    snd_set_device(SND_DEVICE_CURRENT,
                   SND_MUTE_MUTED,
                   SND_MUTE_MUTED,
                   NULL,
                   NULL);

    enabled_flag = FALSE;
  }
}
#endif  /* FEATURE_MVS */

/* <EJECT> */
#if defined(FEATURE_VOC_PCM_INTERFACE) && \
    defined(FEATURE_VOC_PCM_INTERFACE_TEST)
/*==========================================================================

FUNCTION snd_test_pcm_output_loopback_cb

DESCRIPTION
  Callback which will be invoked by vocoder for PCM Output interfaces in
  loopback mode. The received data is buffered in the destination
  channel's buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_pcm_output_loopback_cb(
  const uint16 *pcm_data /* PCM output */
)
{
  static boolean warned = FALSE;
  snd_test_pcm_channel_type *channel =
                    &snd_test_pcm_control.channel[snd_test_pcm_loopback_dest];

  /* Buffer only 2 seconds worth of data */
  if (channel->num_bufs_used >= SND_TEST_PCM_BUF_HWM && warned == FALSE)
  {
    warned = TRUE;
    MSG_ERROR("PCM ran out of output buffers", 0,0,0);
  }
  else
  {
    uint32 index = channel->wr_index + channel->num_bufs_used;
    if (index >= SND_TEST_PCM_BUF_HWM)
    {
      index -= SND_TEST_PCM_BUF_HWM;
    }
    index *= SND_TEST_PCM_DATA_FRAME_LEN;

    memcpy(&channel->buffer[index], pcm_data, SND_TEST_PCM_DATA_FRAME_LEN*2);
    channel->num_bufs_used++;

    warned = FALSE;
  }
}

/*==========================================================================

FUNCTION snd_test_pcm_rx_input_loopback_cb

DESCRIPTION
  Callback which will be invoked by vocoder for PCM Input interfaces in
  loopback mode. The data buffered from the output channel is looped back to
  this input channel.

DEPENDENCIES
  None

RETURN VALUE
  VOC_PCM_DATA_STATUS_AVAILABLE if PCM data is available.

SIDE EFFECTS
  None

===========================================================================*/
voc_pcm_data_status_type snd_test_pcm_rx_input_loopback_cb(
  uint16  *pcm_data
)
{
  static boolean warned = FALSE;
  voc_pcm_data_status_type status = VOC_PCM_DATA_STATUS_UNAVAILABLE;
  snd_test_pcm_channel_type *channel =
                    &snd_test_pcm_control.channel[snd_test_pcm_loopback_dest];

  if (snd_test_rxi_force_miss)
  {
    snd_test_rxi_force_miss--;
  }
  else if (channel->num_bufs_used)
  {
    uint32 index = channel->wr_index * SND_TEST_PCM_DATA_FRAME_LEN;
    memcpy(pcm_data, &channel->buffer[index], SND_TEST_PCM_DATA_FRAME_LEN*2);

    channel->num_bufs_used--;
    channel->wr_index++;
    if (channel->wr_index >= SND_TEST_PCM_BUF_HWM)
    {
      channel->wr_index = 0;
    }
    status = VOC_PCM_DATA_STATUS_AVAILABLE;
    warned = FALSE;
  }
  else if (warned == FALSE)
  {
    MSG_ERROR("PCM ran out of input buffers", 0,0,0);
    warned = TRUE;
  }
  return status;
}

/*==========================================================================

FUNCTION snd_test_enable_pcm_interface_loopback

DESCRIPTION
  Uses the specified PCM channels to create a PCM data loop with a 2 second
  delay.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_enable_pcm_interface_loopback(
  voc_pcm_interface_type src_int,
  voc_pcm_interface_type dest_int
)
{
  /* Always use the Destination's channel */
  snd_test_pcm_channel_type *channel = &snd_test_pcm_control.channel[dest_int];

  if (!snd_test_pcm_control.bitmask)
  {
    snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED,
                   NULL, NULL);
    voc_acquire(VOC_ACQ_TEST, NULL);
  }
  snd_test_pcm_control.bitmask |= (1 << dest_int);
  snd_test_pcm_control.bitmask |= (1 << src_int);

  channel->busy = FALSE;
  channel->rd_index = 0;
  channel->wr_index = 0;
  channel->num_bufs_used = SND_TEST_PCM_BUF_HWM - 1;

  memset(channel->buffer, 0,
         SND_TEST_PCM_NUM_BUFFERS*SND_TEST_PCM_DATA_FRAME_LEN*2);
  if (voc_register_pcm_output_client(src_int, snd_test_pcm_output_loopback_cb)
      != VOC_DONE_S)
  {
    ERR_FATAL("Register failed", 0,0,0);
  }

  if (voc_register_pcm_input_client(dest_int,
                                    snd_test_pcm_rx_input_loopback_cb)
      != VOC_DONE_S)
  {
    ERR_FATAL("Register failed", 0,0,0);
  }

  snd_test_pcm_loopback_src = src_int;
  snd_test_pcm_loopback_dest = dest_int;
}

/*==========================================================================

FUNCTION snd_test_disable_pcm_interface_loopback

DESCRIPTION
  Disables the active PCM data loop.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_disable_pcm_interface_loopback(void)
{
  uint16 voc_acq_mask;

  if (snd_test_pcm_control.bitmask)
  {
    snd_test_rxi_force_miss = 0;
    MSG_ERROR("RX PCM input missed=%d", qdsp_read(QDSP_rxPcmWrMiss), 0,0);
    qdsp_write(QDSP_rxPcmWrMiss, 0);
    if (snd_test_pcm_loopback_src == VOC_PCM_INTERFACE_TX_OUTPUT)
    {
      MSG_ERROR("TX PCM Output missed=%d", qdsp_read(QDSP_txPcmRdMiss), 0,0);
      qdsp_write(QDSP_txPcmRdMiss, 0);
    }
    else if (snd_test_pcm_loopback_src == VOC_PCM_INTERFACE_RX_OUTPUT)
    {
      MSG_ERROR("RX PCM Output missed=%d", qdsp_read(QDSP_rxPcmRdMiss), 0,0);
      qdsp_write(QDSP_rxPcmRdMiss, 0);
    }

    voc_register_pcm_input_client(snd_test_pcm_loopback_dest, NULL);
    voc_register_pcm_output_client(snd_test_pcm_loopback_src, NULL);

    snd_test_pcm_control.bitmask &= ~(1 << snd_test_pcm_loopback_dest);
    snd_test_pcm_control.bitmask &= ~(1 << snd_test_pcm_loopback_src);

    voc_release(VOC_ACQ_TEST);

    /* If ACQ_TEST was the last client, mute the vocoder */
    voc_acq_mask = voc_state_get_state_data()->acquire_mask;
    voc_acq_mask &= ~(1 << VOC_ACQ_TEST);
    voc_acq_mask &= ~(1 << VOC_ACQ_SND);
    if (!voc_acq_mask)
    {
      snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_MUTED, SND_MUTE_MUTED,
                     NULL, NULL);
    }
  }
  else
  {
    MSG_ERROR("Loopback already disabled", 0,0,0);
  }

  snd_test_pcm_loopback_src = VOC_PCM_INTERFACE_MAX;
  snd_test_pcm_loopback_dest = VOC_PCM_INTERFACE_MAX;
}

/*==========================================================================

FUNCTION snd_test_disable_pcm_input

DESCRIPTION
  Disables the specified Input PCM interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_disable_pcm_input(
  voc_pcm_interface_type interface /* Interface to disable */
)
{
  snd_test_pcm_channel_type *channel;
  uint16                    voc_acq_mask;

  if (snd_test_pcm_control.bitmask & (1 << interface))
  {
    if (interface == VOC_PCM_INTERFACE_RX_INPUT) {
      snd_test_rxi_force_miss = 0;
      MSG_ERROR("Disabling RX PCM Input", 0,0,0);
      MSG_ERROR("RX PCM input missed=%d", qdsp_read(QDSP_rxPcmWrMiss), 0,0);
      qdsp_write(QDSP_rxPcmWrMiss, 0);
    }

    if (interface == VOC_PCM_INTERFACE_TX_INPUT) {
      snd_test_txi_force_miss = 0;
      MSG_ERROR("Disabling TX PCM Input", 0,0,0);
      MSG_ERROR("TX PCM input missed=%d", qdsp_read(QDSP_txPcmWrMiss), 0,0);
      qdsp_write(QDSP_txPcmWrMiss, 0);
    }

    snd_test_pcm_control.bitmask &= ~(1 << interface);
    voc_register_pcm_input_client(interface, NULL);

    if (!snd_test_pcm_control.bitmask)
    {
      voc_release(VOC_ACQ_TEST);

      /* If ACQ_TEST was the last client, mute the vocoder */
      voc_acq_mask = voc_state_get_state_data()->acquire_mask;
      voc_acq_mask &= ~(1 << VOC_ACQ_TEST);
      voc_acq_mask &= ~(1 << VOC_ACQ_SND);
      if (!voc_acq_mask)
      {
        snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_MUTED, SND_MUTE_MUTED,
                       NULL, NULL);
      }
    }

    channel = &snd_test_pcm_control.channel[interface];
    if (channel->fs_handle != NULL)
    {
      fs_rsp_msg_type     snd_fs_rsp;
      fs_close(channel->fs_handle, NULL, &snd_fs_rsp);
      channel->fs_handle = NULL;
      snd_test_pcm_log_data &= ~(1 << interface);
    }
  }
  else
  {
    MSG_ERROR("No RX PCM Input to disable", 0,0,0);
  }
}

/*==========================================================================

FUNCTION snd_test_pcm_input_cb

DESCRIPTION
  Callback which will be invoked by the vocoder to get data for the PCM
  input channel (which uses data read from a file).

DEPENDENCIES
  None

RETURN VALUE
  VOC_PCM_DATA_STATUS_AVAILABLE if PCM data is available.

SIDE EFFECTS
  None

===========================================================================*/
voc_pcm_data_status_type snd_test_pcm_input_cb(
  voc_pcm_interface_type interface,
  uint16  *pcm_data
)
{
  static boolean warned = FALSE;
  voc_pcm_data_status_type status = VOC_PCM_DATA_STATUS_UNAVAILABLE;

  snd_test_pcm_channel_type *channel = &snd_test_pcm_control.channel[interface];

  if (snd_test_rxi_force_miss)
  {
    snd_test_rxi_force_miss--;
  }
  else if ( !(channel->busy == TRUE && channel->rd_index == channel->wr_index) )
  {
    uint32 index = (channel->rd_index + channel->num_bufs_used) *
                    SND_TEST_PCM_DATA_FRAME_LEN;

    memcpy(pcm_data, &channel->buffer[index], SND_TEST_PCM_DATA_FRAME_LEN*2);

    channel->num_bufs_used++;
    if (channel->num_bufs_used >= SND_TEST_PCM_BUF_HWM)
    {
      channel->num_bufs_used = 0;
      channel->rd_index += SND_TEST_PCM_BUF_HWM;
      if (channel->rd_index >= SND_TEST_PCM_NUM_BUFFERS)
      {
        channel->rd_index = 0;
      }

      /* Done with this file - disable the PCM interface */
      if (channel->fs_handle == NULL)
      {
        channel->busy = TRUE;
      }
      else
      {
        snd_test_pcm_read_file |= (1 << interface);
      }
    }
    warned = FALSE;
    status = VOC_PCM_DATA_STATUS_AVAILABLE;
  }
  else if (status == VOC_PCM_DATA_STATUS_UNAVAILABLE && warned == FALSE)
  {
    warned = TRUE;
    MSG_ERROR("Data underflow for RxPCM input", 0,0,0);
  }
  return status;
}

/*==========================================================================

FUNCTION snd_test_rx_input_cb

DESCRIPTION
  Callback which will be invoked by the vocoder to get data for the PCM
  input channel (which uses data read from a file).

DEPENDENCIES
  None

RETURN VALUE
  VOC_PCM_DATA_STATUS_AVAILABLE if PCM data is available.

SIDE EFFECTS
  None

===========================================================================*/
voc_pcm_data_status_type snd_test_rx_input_cb(
  uint16 *data
)
{
  return snd_test_pcm_input_cb(VOC_PCM_INTERFACE_RX_INPUT, data);
}

/*==========================================================================

FUNCTION snd_test_tx_input_cb

DESCRIPTION
  Callback which will be invoked by the vocoder to get data for the PCM
  input channel (which uses data read from a file).

DEPENDENCIES
  None

RETURN VALUE
  VOC_PCM_DATA_STATUS_AVAILABLE if PCM data is available.

SIDE EFFECTS
  None

===========================================================================*/
voc_pcm_data_status_type snd_test_tx_input_cb(
  uint16 *data
)
{
  return snd_test_pcm_input_cb(VOC_PCM_INTERFACE_TX_INPUT, data);
}

/*==========================================================================

FUNCTION snd_test_read_pcm_file

DESCRIPTION
  Reads data from the file system and stores it in buffers, which can be
  used by the PCM input callback function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_read_pcm_file()
{
  uint32                    index;
  snd_test_pcm_channel_type *channel;
  fs_rsp_msg_type           snd_fs_rsp;
  voc_pcm_interface_type    interface = VOC_PCM_INTERFACE_RX_INPUT;

  if (snd_test_pcm_read_file)
  {
    for (interface = 0; interface < VOC_PCM_INTERFACE_MAX; interface++)
    {
      if (snd_test_pcm_read_file & (1 << interface))
      {
        channel = &snd_test_pcm_control.channel[interface];

        if (channel->busy == FALSE && channel->fs_handle != NULL)
        {
          index = channel->wr_index * SND_TEST_PCM_DATA_FRAME_LEN;

          channel->busy = TRUE;
          fs_read(channel->fs_handle, &channel->buffer[index],
                  SND_TEST_PCM_BLOCK_SIZE, NULL, &snd_fs_rsp);

          if (snd_fs_rsp.any.status != FS_OKAY_S)
          {
            MSG_FATAL("fs_read failed", 0, 0, 0);
            fs_close(channel->fs_handle, NULL, &snd_fs_rsp);
            channel->fs_handle = NULL;
            snd_test_disable_pcm_input(interface);
          }
          else
          {
            /* check if we hit the EOF */
            if (snd_fs_rsp.read.count < SND_TEST_PCM_BLOCK_SIZE)
            {
              /* Repeat the file from the beginning */
              fs_seek(channel->fs_handle, FS_SEEK_SET, 0, NULL, &snd_fs_rsp);
              channel->busy = FALSE;
              continue;
            }
            INTLOCK();
            channel->wr_index += SND_TEST_PCM_BUF_HWM;
            if (channel->wr_index >= SND_TEST_PCM_NUM_BUFFERS)
            {
              channel->wr_index = 0;
            }
            channel->busy = FALSE;
            snd_test_pcm_read_file &= ~(1 << interface);
            INTFREE();
          }
        }
      }
    }
  }
}

/*==========================================================================

FUNCTION snd_test_enable_pcm_file_input

DESCRIPTION
  Enables the specified PCM Input interface, and opens the specified file to
  be fed as input to the PCM interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_enable_pcm_file_input(
  voc_pcm_interface_type interface, /* PCM Output Interface to enabled     */
  uint8                  file_index /* Index into snd_test_pcm_input_files */
)
{
  snd_test_pcm_channel_type *channel;
  fs_rsp_msg_type           snd_fs_rsp;
  voc_pcm_client_input_fn_type callback_func;

  switch (interface)
  {
    case VOC_PCM_INTERFACE_RX_INPUT:
      MSG_ERROR("Enabling RX PCM Input", 0,0,0);
      callback_func = snd_test_rx_input_cb;
      break;

    case VOC_PCM_INTERFACE_TX_INPUT:
      MSG_ERROR("Enabling TX PCM Input", 0,0,0);
      callback_func = snd_test_tx_input_cb;
      break;

    default:
      MSG_FATAL("Can only support PCM input!", 0, 0, 0);
      return;
  }

  channel = &snd_test_pcm_control.channel[interface];
  /* Cleanup - close any previously opened file */
  if (channel->fs_handle != NULL)
  {
    fs_close(channel->fs_handle, NULL, &snd_fs_rsp);
    channel->fs_handle = NULL;
  }

  fs_open(&snd_test_pcm_input_files[file_index][0], FS_OA_READONLY, NULL,
          NULL, &snd_fs_rsp);

  if (snd_fs_rsp.open.handle != NULL && snd_fs_rsp.any.status == FS_OKAY_S)
  {
    channel->fs_handle = snd_fs_rsp.open.handle;
  }
  else
  {
    MSG_FATAL("fs_open failed", 0, 0, 0);
    return;
  }

  if (!snd_test_pcm_control.bitmask)
  {
    snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED,
                   NULL, NULL);
    voc_acquire(VOC_ACQ_TEST, NULL);
  }
  snd_test_pcm_control.bitmask |= (1 << interface);

  channel->busy = FALSE;
  channel->rd_index = 0;
  channel->wr_index = 0;
  channel->num_bufs_used = 0;

  /* Pre-fill the buffer with data */
  snd_test_pcm_read_file |= (1 << interface);
  snd_test_read_pcm_file();
  snd_test_pcm_read_file |= (1 << interface);
  snd_test_read_pcm_file();

  if (voc_register_pcm_input_client(interface, callback_func) != VOC_DONE_S)
  {
    ERR_FATAL("Register failed", 0,0,0);
  }
}

/*==========================================================================

FUNCTION snd_test_pcm_output_null_cb

DESCRIPTION
  Callback for PCM output. Just dumps the data instead of logging it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_pcm_output_null_cb(
  const uint16  *pcm_data
)
{
}

/*==========================================================================

FUNCTION snd_test_pcm_output_null

DESCRIPTION
  Enables the specified PCM output interface. The data received in the callback
  is dumped.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_pcm_output_null(
  voc_pcm_interface_type interface
)
{
  snd_test_pcm_channel_type *channel = &snd_test_pcm_control.channel[interface];

  if (!snd_test_pcm_control.bitmask)
  {
    snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED,
                   NULL, NULL);
    voc_acquire(VOC_ACQ_TEST, NULL);
  }
  snd_test_pcm_control.bitmask |= (1 << interface);

  channel->busy = TRUE;
  channel->rd_index = 0;
  channel->wr_index = 0;
  channel->num_bufs_used = 0;

  if (voc_register_pcm_output_client(interface, snd_test_pcm_output_null_cb)
      != VOC_DONE_S)
  {
    ERR_FATAL("Register failed", 0,0,0);
  }
}

/*==========================================================================

FUNCTION snd_test_process_pcm_output

DESCRIPTION
  Stores the PCM output data received from the vocoder in a buffer, for use
  in loopback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_process_pcm_output(
  voc_pcm_interface_type interface,
  const uint16           *pcm_data
)
{
  static boolean warned = FALSE;
  snd_test_pcm_channel_type *channel = &snd_test_pcm_control.channel[interface];
  uint32 index = 0;

  if (snd_test_pcm_loopback_src == interface)
  {
    snd_test_pcm_output_loopback_cb(pcm_data);
  }

  if (channel->busy == TRUE && channel->rd_index == channel->wr_index)
  {
    MSG_FATAL("PCM ran out of buffers for %d", (int)interface, 0, 0);
    warned = TRUE;
  }
  else
  {
    index = (channel->wr_index + channel->num_bufs_used) *
            SND_TEST_PCM_DATA_FRAME_LEN;
    memcpy(&channel->buffer[index], pcm_data, SND_TEST_PCM_DATA_FRAME_LEN*2);
    channel->num_bufs_used++;

    if (channel->num_bufs_used >= SND_TEST_PCM_BUF_HWM)
    {
      channel->wr_index += SND_TEST_PCM_BUF_HWM;
      channel->num_bufs_used = 0;
      if (channel->wr_index >= SND_TEST_PCM_NUM_BUFFERS)
      {
        /* Wrap around to the beginning of the circular buffer */
        channel->wr_index = 0;
      }

      /* Indicate that there is data available for logging */
      snd_test_pcm_log_data |= (1 << interface);
    }
    warned = FALSE;
  }
}

/*==========================================================================

FUNCTION snd_test_tx_output_cb

DESCRIPTION
  Callback to get TX PCM data from the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_tx_output_cb(
  const uint16  *pcm_data
)
{
  snd_test_process_pcm_output(VOC_PCM_INTERFACE_TX_OUTPUT, pcm_data);
}

/*==========================================================================

FUNCTION snd_test_rx_output_cb

DESCRIPTION
  Callback to get RX PCM data from the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_rx_output_cb(
  const uint16  *pcm_data
)
{
  snd_test_process_pcm_output(VOC_PCM_INTERFACE_RX_OUTPUT, pcm_data);
}

/* Prototype */
void snd_test_disable_pcm_output( voc_pcm_interface_type interface );

/*==========================================================================

FUNCTION snd_test_log_pcm

DESCRIPTION
  Logs the PCM output data to EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_log_pcm(void)
{
  voc_pcm_interface_type    interface;
  uint32                    index;
  fs_rsp_msg_type           snd_fs_rsp;
  snd_test_pcm_channel_type *channel;

  if (snd_test_pcm_log_data)
  {
    /* Log the data for each interface that has a full buffer block */
    for (interface = 0; interface < VOC_PCM_INTERFACE_MAX; interface++)
    {
      channel = &snd_test_pcm_control.channel[interface];
      if ((snd_test_pcm_log_data & (1 << interface)) && channel->busy == FALSE)
      {
        index = channel->rd_index;
        index *= SND_TEST_PCM_DATA_FRAME_LEN;
        channel->busy = TRUE;

        fs_write(channel->fs_handle, &channel->buffer[index],
                 SND_TEST_PCM_BLOCK_SIZE, NULL, &snd_fs_rsp);

        if (snd_fs_rsp.any.status != FS_OKAY_S)
        {
          MSG_FATAL("fs_write failed", 0, 0, 0);
          fs_close(channel->fs_handle, NULL, &snd_fs_rsp);
          channel->fs_handle = NULL;
          snd_test_disable_pcm_output(interface);
        }
        else
        {
          INTLOCK();
          channel->rd_index += SND_TEST_PCM_BUF_HWM;
          if (channel->rd_index >= SND_TEST_PCM_NUM_BUFFERS)
          {
            channel->rd_index = 0;
          }

          snd_test_pcm_log_data &= ~(1 << interface);

          channel->busy = FALSE;
          INTFREE();
        }
      }
    }
  }
}

/*==========================================================================

FUNCTION snd_test_enable_and_log_pcm_output

DESCRIPTION
  Enables PCM output for the specified interface; logs the output to file in
  EFS. The files are named "pcmRxOutputXXX" or "pcmTxOutputXXX" where XXX
  corresponds to the timestamp at the time of creation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_enable_and_log_pcm_output(
  voc_pcm_interface_type interface
)
{
  snd_test_pcm_channel_type *channel;
  fs_rsp_msg_type           snd_fs_rsp;
  fs_open_xparms_type       open_parms;
  char                      file_name[20];

  switch (interface)
  {
    case VOC_PCM_INTERFACE_RX_OUTPUT:
      MSG_ERROR("Enabling RX PCM Output", 0,0,0);
      break;

    case VOC_PCM_INTERFACE_TX_OUTPUT:
      MSG_ERROR("Enabling TX PCM Output", 0,0,0);
      break;

    default:
      ERR_FATAL("Can only log PCM output!", 0, 0, 0);
  }

  channel = &snd_test_pcm_control.channel[interface];
  /* Cleanup - close any previously opened file */
  if (channel->fs_handle != NULL)
  {
    fs_close(channel->fs_handle, NULL, &snd_fs_rsp);
    channel->fs_handle = NULL;
  }

  open_parms.create.cleanup_option   = FS_OC_CLOSE;
  open_parms.create.buffering_option = FS_OB_PROHIBIT;
  open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

  memset(file_name, '\0', sizeof(file_name));
  std_strlprintf(file_name,sizeof(file_name), "%s%ld",
                 ((interface == VOC_PCM_INTERFACE_RX_OUTPUT)?"pcmRxOutput": "pcmTxOutput"),
                  clk_read_secs());

  /* MSG_ERROR doesn't take %s format */
  MSG_ERROR("Writing to PCM file ...", 0,0,0);

  fs_open(file_name, FS_OA_CREATE, &open_parms, NULL, &snd_fs_rsp);
  if (snd_fs_rsp.open.handle != NULL && snd_fs_rsp.any.status == FS_OKAY_S)
  {
    channel->fs_handle = snd_fs_rsp.open.handle;
  }
  else
  {
    MSG_FATAL("fs_open failed", 0, 0, 0);
    return;
  }

  if (!snd_test_pcm_control.bitmask)
  {
    snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED,
                   NULL, NULL);
    voc_acquire(VOC_ACQ_TEST, NULL);
  }
  snd_test_pcm_control.bitmask |= (1 << interface);

  memset(channel->buffer, 0,
         SND_TEST_PCM_NUM_BUFFERS*SND_TEST_PCM_DATA_FRAME_LEN*2);
  channel->busy = FALSE;
  channel->rd_index = 0;
  channel->wr_index = 0;
  channel->num_bufs_used = 0;
  snd_test_pcm_log_data &= ~(1 << interface);

  if (voc_register_pcm_output_client(interface,
        (interface == VOC_PCM_INTERFACE_RX_OUTPUT)?
           snd_test_rx_output_cb: snd_test_tx_output_cb) != VOC_DONE_S)
  {
    ERR_FATAL("Register failed", 0,0,0);
  }
}

/*==========================================================================

FUNCTION snd_test_disable_pcm_output

DESCRIPTION
  Disables PCM data logging for the specified interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_test_disable_pcm_output(
  voc_pcm_interface_type interface
)
{
  snd_test_pcm_channel_type *channel;
  uint16                    voc_acq_mask;

  if (snd_test_pcm_control.bitmask & (1 << interface))
  {
    switch (interface)
    {
      case VOC_PCM_INTERFACE_RX_OUTPUT:
        MSG_ERROR("RX PCM Output missed=%d", qdsp_read(QDSP_rxPcmRdMiss), 0,0);
        MSG_ERROR("Disabling RX PCM Output", 0,0,0);
        qdsp_write(QDSP_rxPcmRdMiss, 0);
        break;

      case VOC_PCM_INTERFACE_TX_OUTPUT:
        MSG_ERROR("TX PCM Output missed=%d", qdsp_read(QDSP_txPcmRdMiss), 0,0);
        MSG_ERROR("Disabling TX PCM Output", 0,0,0);
        qdsp_write(QDSP_txPcmRdMiss, 0);
        break;

      default:
        ERR_FATAL("Can only disable PCM output!", 0, 0, 0);
    }

    if (snd_test_pcm_loopback_src == interface)
    {
      if (voc_register_pcm_output_client(interface,
                   snd_test_pcm_output_loopback_cb) != VOC_DONE_S)
      {
        ERR_FATAL("Register failed", 0,0,0);
      }
    }
    else
    {
      snd_test_pcm_control.bitmask &= ~(1 << interface);
      voc_register_pcm_output_client(interface, NULL);

      if (!snd_test_pcm_control.bitmask)
      {
        voc_release(VOC_ACQ_TEST);

        /* If ACQ_TEST was the last client, mute the vocoder */
        voc_acq_mask = voc_state_get_state_data()->acquire_mask;
        voc_acq_mask &= ~(1 << VOC_ACQ_TEST);
        voc_acq_mask &= ~(1 << VOC_ACQ_SND);
        if (!voc_acq_mask)
        {
          snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_MUTED, SND_MUTE_MUTED,
                         NULL, NULL);
        }
      }
    }

    channel = &snd_test_pcm_control.channel[interface];
    if (channel->fs_handle != NULL)
    {
      fs_rsp_msg_type     snd_fs_rsp;
      if (channel->wr_index != channel->rd_index)
      {
        /* Save any partial buffers */
        snd_test_pcm_log_data |= (1 << interface);
        snd_test_log_pcm();
        if (channel->num_bufs_used)
        {
          fs_write(channel->fs_handle,
               &channel->buffer[channel->rd_index*SND_TEST_PCM_DATA_FRAME_LEN],
               SND_TEST_PCM_DATA_FRAME_LEN*2*channel->num_bufs_used, NULL,
               &snd_fs_rsp);
        }
      }
      fs_close(channel->fs_handle, NULL, &snd_fs_rsp);
      channel->fs_handle = NULL;
      snd_test_pcm_log_data &= ~(1 << interface);
    }
  }
  else
  {
    MSG_ERROR("No PCM output to disable", 0,0,0);
  }
}
#endif /* FEATURE_VOC_PCM_INTERFACE && FEATURE_VOC_PCM_INTERFACE_TEST */

/* <EJECT> */
/*==========================================================================

FUNCTION SND_TEST_CALLBACK

DESCRIPTION
  This function performs the call back called from sound server indicating
  status and results from the requested command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For test nothing happens

===========================================================================*/
LOCAL void snd_test_callback( void *client_data, snd_status_type status )
{
 if ( status == SND_PLAY_DONE );
}

/* <EJECT> */
#if defined(FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST) && \
    defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
/*==========================================================================

FUNCTION SND_TEST_CALLBACK

DESCRIPTION
  This function performs the call back called from sound server indicating
  status and results from the requested command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For test nothing happens

===========================================================================*/
void snd_test_av_sync_cb (
  qword timestamp,        /* timestamp for the callback in ms               */
  qword num_of_samples,   /* sample/frame counter. Use only lower 48 bits   */
  qword num_of_bytes,     /* bytes/frame counter. Use only for window media */
  const void *client_data /* av sync client data                            */
)
{
  MSG_ERROR("av_sync_cb: timestamp=%d, num_samples=%d, num_bytes=%d",
            qw_lo(timestamp), qw_lo(num_of_samples), qw_lo(num_of_bytes));
}
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST && FEATURE_AMR_WBPLUS_AUDIO_DEC */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_test_code

DESCRIPTION
  This function provides hardware test code when compiled into a non-production
  version of the system.  This function is hardware dependent.

DEPENDENCIES
  If SND_TEST_CODE is defined, then this routine watches for value poked into
  memory from the Diagnostic Monitor or In-Circuit Emulator and actives the
  hardware as indicated by the flags.

RETURN VALUE
  None

SIDE EFFECTS
  Values in snd_test_block are clear after they are executed.

===========================================================================*/

void snd_test_code
(
  word *change_mask
    /* Pointer to "main loop" Change Flags */
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  static snd_op_type needed_stop_cmd = SND_TONE_STOP;

#ifdef SND_PARA_SUPPORT
  snd_sound_type *temp_combo_ptr;
#endif

#ifdef SND_AVBAP_SUPPORTED
#error code not present
#endif

#ifdef FEATURE_AVS_I2SSBC
  snd_i2ssbc_config_params_type  i2ssbc_config;
#endif /* FEATURE_AVS_I2SSBC */

#ifdef SND_TEST_CODEC_OVERRIDE
    fs_rsp_msg_type rsp;
    fs_handle_type  filehandle;
    const char      filename[] = "apps/dualmic.txt";
#endif // SND_TEST_CODEC_OVERRIDE

  /* If this is the first time, initialize the test block. Set all      */
  /* commands to 0xff so they won't be executed. Set all parameters to  */
  /* default values. After initialization, if a command value is not    */
  /* equal to 0xFF, it will be executed.                                */
  /* The commands are sent by the DM or in-circuit emulator modifying   */
  /* the memory location.                                               */

    /* Initialising the variables for checking idlemetering.txt */
	fs_rsp_msg_type	returnstatus;
	fs_handle_type     idlemeterfilehandle;
	const char idlefilename[] =  "apps/idlemetering.txt";

/*Initialising the variables for, checking the file system,
 to set the variable corresponding to the pcm interface mode  */
#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif


  if ( !snd_test_block.init_flag ) {
    MSG_HIGH("snd_test_code() Compiled and Running",0,0,0);
    (void) memset(&snd_test_block, ~0, sizeof(snd_test_block));
    /* Set sound command parameters to legal values */
    snd_test_block.omute           = SND_MUTE_UNMUTED;
    snd_test_block.imute           = SND_MUTE_UNMUTED;
    snd_test_block.priority        = SND_PRIO_LOW;
    snd_test_block.vol_volume      = 2;
    snd_test_block.vol_dev         = SND_DEVICE_HANDSET;
    snd_test_block.vol_method      = SND_METHOD_VOICE;
    snd_test_block.smp_tone_vol    = 2;
    snd_test_block.smp_sound_vol   = 2;
    snd_test_block.method          = SND_METHOD_KEY_BEEP;
    snd_test_block.dtmf_path       = SND_APATH_LOCAL;
    snd_test_block.duration        = 1000;
    snd_test_block.st_ctl          = 0xFF;
    memcpy(snd_test_block.voc_pac_version,PACKET_VERSION_1,9);

#ifdef SND_TEST_CODEC_OVERRIDE
	snd_test_block.codec_override_flag = 0xff;

    fs_open( filename, FS_OA_READONLY, NULL, NULL, &rsp );
    if ( FS_OKAY_S == rsp.open.status ){
        filehandle = rsp.open.handle;
        fs_close(filehandle, NULL, &rsp );
	/* override the handset codec  with the dual mic handset codec */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  #if defined(T_FFA)
#error code not present
  #elif defined(FEATURE_EXTERNAL_SADC)
	voc_set_codec_override(VOC_CODEC_ON_CHIP_0, VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET, TRUE);
  #endif
#endif //FEATURE_AVS_CRYSTAL_SPEECH
    }
    else{
	/* no file is present */
	voc_set_codec_override(VOC_CODEC_ON_CHIP_0, VOC_CODEC_ON_CHIP_0, FALSE);
    }
#endif // SND_TEST_CODEC_OVERRIDE


  /*check whether idlemetering.txt is present or not */
  fs_open( idlefilename, FS_OA_READONLY, NULL, NULL, &returnstatus );
  if ( FS_OKAY_S == returnstatus.open.status ){
    idlemeterfilehandle = returnstatus.open.handle;
    fs_close(idlemeterfilehandle, NULL, &returnstatus );

    /* Initialising idlemetering.. */
    voc_set_idle_metering_enable(TRUE);
  }
  else{
    /* idlemetering.txt file is not present */
    voc_set_idle_metering_enable(FALSE);
  }

#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

#ifdef FEATURE_AAC_REC
    aac_param.mm_rec_link                = CMX_MM_REC_LINK_REVERSE;
    aac_param.mm_rec_param.mm_rec_format = CMX_MM_FORMAT_AAC;

    aac_param.mm_rec_param.cmx_mm_rec_aac_para.aac_file_format =
        CMX_AF_AAC_DATA_FORMAT_ADTS;

    aac_param.mm_rec_param.cmx_mm_rec_aac_para.sample_rate     =
        CMX_AF_SAMPLE_RATE_8000;

    aac_param.mm_rec_param.cmx_mm_rec_aac_para.sync_cb_func    = NULL;

    aac_param.mm_rec_param.cmx_mm_rec_aac_para.chan            =
        CMX_AF_AAC_CHANNEL_MONO;

    aac_param.mm_rec_param.cmx_mm_rec_aac_para.bit_per_spl     = 0x1C00;
#endif /* FEATURE_AAC_REC */
#ifdef FEATURE_SBC_CODEC
    snd_test_sbc_config.alloc     = SND_SBC_ALLOC_SNR;
    snd_test_sbc_config.bands     = SND_SBC_SUBBANDS_8;
    snd_test_sbc_config.blocks    = SND_SBC_BLOCKS_8;
    snd_test_sbc_config.mode      = SND_SBC_MODE_JT_STEREO;
#endif
  }

  if ( !snd_test_enable_flag ) {
	  return;
  }

#ifdef FEATURE_AVS_WB_PCM_RX_INTERFACE
  if (snd_test_pp_test == 2)
  {
    snd_test_pp_test = 0xff;
    snd_pp_ctl(snd_test_rx_pcm_cb, NULL, NULL);
  }
#endif /* FEATURE_AVS_WB_PCM_RX_INTERFACE */

#ifdef FEATURE_ARM_POST_PROCESSING

  if (snd_test_pp_test == 0)
  {
    snd_test_pp_test = 0xff;
    snd_pp_ctl(NULL, NULL, NULL);
  }
  else if (snd_test_pp_test == 1)
  {
    snd_test_pp_test = 0xff;
    snd_pp_ctl(snd_test_pp_cb, NULL, NULL);
  }

  if (snd_test_pp_file_out == 0)
  {
    snd_test_pp_file_out = 0xff;
    if (snd_test_pp_fs_handle != NULL)
    {
      fs_close(snd_test_pp_fs_handle, NULL, &snd_test_pp_snd_fs_rsp);
    }
  }
  else if (snd_test_pp_file_out != 0xff)
  {
    snd_test_pp_file_out = 0xff;
    /* This is the first frame so open the file */
    if (snd_test_pp_fs_handle != NULL)
    {
      fs_close(snd_test_pp_fs_handle, NULL, &snd_test_pp_snd_fs_rsp);
    }

    snd_test_pp_open_parms.create.cleanup_option   = FS_OC_CLOSE;
    snd_test_pp_open_parms.create.buffering_option = FS_OB_PROHIBIT;
    snd_test_pp_open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

    memset(snd_test_pp_file_name, '\0', sizeof(snd_test_pp_file_name));
    std_strlprintf(snd_test_pp_file_name,sizeof(snd_test_pp_file_name),
                   "%s%ld","PP_PCM_FILE", clk_read_secs());

    fs_open(snd_test_pp_file_name, FS_OA_CREATE, &snd_test_pp_open_parms, NULL, &snd_test_pp_snd_fs_rsp);
    if (snd_test_pp_snd_fs_rsp.open.handle != NULL &&
                                          snd_test_pp_snd_fs_rsp.any.status == FS_OKAY_S)
    {
      snd_test_pp_fs_handle = snd_test_pp_snd_fs_rsp.open.handle;
      fs_close(snd_test_pp_fs_handle, NULL, NULL);
      fs_open(snd_test_pp_file_name, FS_OA_APPEND, &snd_test_pp_open_parms, NULL, &snd_test_pp_snd_fs_rsp);
      if (snd_test_pp_snd_fs_rsp.open.handle != NULL &&
                                          snd_test_pp_snd_fs_rsp.any.status == FS_OKAY_S)
      {
        snd_test_pp_fs_handle = snd_test_pp_snd_fs_rsp.open.handle;
      }
      else
      {
        snd_test_pp_fs_handle = NULL;
      }
    }
  }
#endif /* FEATURE_ARM_POST_PROCESSING*/

#ifdef FEATURE_SBC_CODEC
  if (snd_test_sbc_alloc != 0xff)
  {
    snd_test_sbc_config.alloc = snd_test_sbc_alloc;
    snd_test_sbc_config_reqd  = TRUE;
    snd_test_sbc_alloc        = 0xff;
  }
  if (snd_test_sbc_mode != 0xff)
  {
    snd_test_sbc_config.mode  = snd_test_sbc_mode;
    snd_test_sbc_config_reqd  = TRUE;
    snd_test_sbc_mode         = 0xff;
  }
  if (snd_test_sbc_blocks != 0xff)
  {
    snd_test_sbc_config.blocks = snd_test_sbc_blocks;
    snd_test_sbc_config_reqd   = TRUE;
    snd_test_sbc_blocks        = 0xff;
  }
  if (snd_test_sbc_bands != 0xff)
  {
    snd_test_sbc_config.bands  = snd_test_sbc_bands;
    snd_test_sbc_config_reqd   = TRUE;
    snd_test_sbc_bands         = 0xff;
  }

  if (snd_test_sbc_config_reqd)
  {
     snd_test_audiosbcenc_config.octet0.channel_mode= SND_TEST_ENC_OCTET0;
     snd_test_audiosbcenc_config.octet1.subbands= SND_TEST_ENC_OCTET1;

	 /*Call to change the current SBC configuration params*/
     snd_sbc_config(&snd_test_audiosbcenc_config, NULL, NULL);
     snd_test_sbc_config_reqd = FALSE;
  }


  if (snd_test_sbc_bitrate_hint != 0xff)
  {
    snd_sbc_set_bitrate(snd_test_sbc_bitrate_hint,
                        snd_test_sbc_bitrate, NULL, NULL);
    snd_test_sbc_bitrate_hint = 0xff;
  }
#endif /* FEATURE_SBC_CODEC */
#ifdef FEATURE_AVS_I2SSBC
  if ( snd_test_block.i2ssbc != 0xff ) {
    if (snd_test_block.i2ssbc == 0) {
      snd_i2ssbc_disable(NULL, NULL);
    } else {
      i2ssbc_config.sample_rate = snd_test_i2ssbc_sample_rate;
      i2ssbc_config.cb_func     = snd_test_i2ssbc_cb_func;

      snd_i2ssbc_enable( &i2ssbc_config, NULL, NULL);
    }
    snd_test_block.i2ssbc = 0xff;
  }
#endif /* FEATURE_AVS_I2SSBC */

#ifdef FEATURE_ACP
#error code not present
#endif
  /* config */
  if ( snd_test_block.config_evrc != 0xff ) {
    (void) voc_config(VOC_ACQ_SND,VOC_CAP_IS127);
    snd_test_block.config_evrc = 0xff;
  }
  /* config */
  if ( snd_test_block.config_13k != 0xff ) {
    (void) voc_config(VOC_ACQ_SND,VOC_CAP_IS733);
    snd_test_block.config_13k = 0xff;
  }
  /* Loop back Tests
  */
  /* packet internal*/
  if ( snd_test_block.pkt_int_loop != 0xff ) {
    voc_pkt_int_loop_cmd (snd_test_block.pkt_int_loop);
    snd_test_block.pkt_int_loop = 0xff;
  }
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  /* audio */
  if ( snd_test_block.audio_loop != 0xff ) {
    voc_audio_loop_cmd (snd_test_block.audio_loop);
    snd_test_block.audio_loop = 0xff;
  }
  /* up pkt */
  if ( snd_test_block.micro_loop != 0xff ) {
#ifdef FEATURE_UP_LOOPBACK_TEST
    voc_micro_loop_cmd(snd_test_block.micro_loop);
#endif
    snd_test_block.micro_loop = 0xff;
  }
  /* decoder to encoder loop */
  if ( snd_test_block.d2e_loop != 0xff ) {
    voc_d2e_loop_cmd(snd_test_block.d2e_loop);
    snd_test_block.d2e_loop = 0xff;
  }
  /* tx rate limit factor */
  if ( snd_test_block.tx_rate != 0xff ) {
    (void) voc_tx_rate_limit((word) snd_test_block.tx_rate);
    snd_test_block.tx_rate = 0xff;
  }
  /* packet */
  if ( snd_test_block.pkt_loop != 0xff ) {
    voc_pkt_loop_cmd (snd_test_block.pkt_loop);
    snd_test_block.pkt_loop = 0xff;
  }
  /* pcm */
  if ( snd_test_block.pcm_loop != 0xff ) {
     voc_pcm_loop_cmd (snd_test_block.pcm_loop);
     snd_test_block.pcm_loop = 0xff;
  }
  /* pcm pad */
  if ( snd_test_block.pcm_pad != 0xff ) {
    voc_put_pcmpad (snd_test_block.pcm_pad_val);
    snd_test_block.pcm_pad = 0xff;
  }
  /* Set the device */
  if ( snd_test_block.device != 0xff ) {
    snd_set_device((snd_device_type) snd_test_block.device,
                   (snd_mute_control_type) snd_test_block.imute,
                   (snd_mute_control_type) snd_test_block.omute,
                    NULL, NULL);
    snd_test_block.device = 0xff;
  }

#ifdef FEATURE_ACP
#error code not present
#endif

  /* Mute or Unmute FM or CDMA voice Receive */
  if ( snd_test_block.rx_mute != 0xff ) {
    snd_rx_mute_ctl( SND_MUTE_SNDV, snd_test_block.rx_mute );
    snd_test_block.rx_mute = 0xff;
  }

#ifdef SND_HFK
  /* Mute or Unute the car stereo (first set path to ext) */
  if ( snd_test_block.stereo_mute != 0xff ) {
    snd_stereo_mute_ctl(snd_test_block.stereo_mute, NULL, NULL);
    snd_test_block.stereo_mute = 0xff;
  }
#endif /* SND_HFK */

  /* Filters */
  if ( snd_test_block.filters != 0xff ) {
    snd_filters((snd_filters_type) (snd_test_block.filters ?
                 SND_FILTERS_DATA : SND_FILTERS_VOICE), NULL, NULL) ;
    snd_test_block.filters = 0xff;
  }

  /* Stop Tone/Sound Id/Sound */
  if ( snd_test_block.stop_last != 0xff) {
    switch (needed_stop_cmd) {
      case SND_TONE_STOP:
        snd_tone_stop(NULL, NULL);
        break;
      case SND_SOUND_STOP:
      default:
        snd_sound_stop((snd_priority_type)snd_test_block.priority, NULL, NULL);
        break;
    }
      snd_test_block.stop_last = 0xff;
  }

  /* Play a Tone */
  if ( snd_test_block.tone != 0xff ) {
    snd_tone_start(SND_DEVICE_CURRENT,
                   (snd_method_type) snd_test_block.method,
                   (snd_tone_type)(snd_test_block.tone + SND_FIRST_TONE +1),
                   snd_test_block.duration,
                   (snd_apath_type) snd_test_block.dtmf_path,
                   NULL, NULL);

      needed_stop_cmd       = SND_TONE_STOP;
      snd_test_block.tone = 0xff;
  }
  /* Play a sample Tone with volume determined by smp_tone_vol */
  if ( snd_test_block.smp_tone != 0xff ) {
    snd_sample_tone_start(SND_DEVICE_CURRENT,
                 (snd_method_type) snd_test_block.method,
                 (snd_tone_type)(snd_test_block.smp_tone + SND_FIRST_TONE + 1),
                 snd_test_block.duration,
                 (word) snd_test_block.smp_tone_vol,
                 NULL, NULL);
      needed_stop_cmd       = SND_TONE_STOP;
      snd_test_block.smp_tone = 0xff;
  }

 /* Play a Tone with specified pair of freq */
  if ( snd_test_block.freq != 0xff ) {
    snd_freq_tone_start(SND_DEVICE_CURRENT,
                      (snd_method_type) snd_test_block.method,
                      snd_test_block.freq_high,
                      snd_test_block.freq_low,
                      snd_test_block.duration,
                      (snd_apath_type) snd_test_block.dtmf_path,
                      NULL, NULL);

      needed_stop_cmd       = SND_TONE_STOP;
      snd_test_block.freq = 0xff;
  }

  /* Play a Sound from list of tones */
  if (snd_test_block.sound != 0xff) {
    snd_sound_start ( SND_DEVICE_CURRENT,
                      (snd_method_type) snd_test_block.method,
                      (snd_sound_type *)&compact_snd_canned,
                      (snd_priority_type)snd_test_block.priority,
                      (snd_apath_type) snd_test_block.dtmf_path,
                      NULL, NULL);
      needed_stop_cmd             = SND_SOUND_STOP;
      snd_test_block.sound = 0xff;
  }

  /* Play a flexible Sound from a list of tones */
  if (snd_test_block.sound_flex != 0xff) {
    snd_sound_start ( SND_DEVICE_CURRENT,
                      (snd_method_type) snd_test_block.method,
                      (snd_sound_type *)&flexible_snd_flexible,
                      (snd_priority_type)snd_test_block.priority,
                      (snd_apath_type) snd_test_block.dtmf_path,
                      NULL, NULL);
      needed_stop_cmd             = SND_SOUND_STOP;
      snd_test_block.sound_flex = 0xff;
  }

  /* Play a sample Sound with determined volume by smp_sound_vol */
  if (snd_test_block.smp_sound != 0xff) {
    snd_sample_sound_start ( SND_DEVICE_CURRENT,
                      (snd_method_type) snd_test_block.method,
                      (snd_sound_type *)&compact_snd_canned,
                      (word) snd_test_block.smp_sound_vol,
                      10,
                      NULL, NULL);
      needed_stop_cmd             = SND_SOUND_STOP;
      snd_test_block.smp_sound = 0xff;
  }

  /* Play a Sound from the database */
  if (snd_test_block.snd_id != 0xff) {
    snd_sound_id_start( SND_DEVICE_CURRENT,
                      (snd_method_type)      snd_test_block.method,
                      (snd_sound_id_type)    snd_test_block.snd_id,
                      (snd_priority_type)    snd_test_block.priority,
                      (snd_apath_type)       snd_test_block.dtmf_path,
                      (snd_cb_func_ptr_type) snd_test_callback,
                      NULL);

      needed_stop_cmd             = SND_SOUND_STOP;
      snd_test_block.snd_id = 0xff;
  }
  /* Play a sample Sound from the database with smp_sound_vol volume */
  if (snd_test_block.smp_snd_id != 0xff) {
    snd_sample_sound_id_start( SND_DEVICE_CURRENT,
                      (snd_method_type)      snd_test_block.method,
                      (snd_sound_id_type)    snd_test_block.smp_snd_id,
                      (uint16)               snd_test_block.smp_sound_vol,
                      (uint16)               0,
                      (snd_cb_func_ptr_type) snd_test_callback,
                      NULL);

      needed_stop_cmd             = SND_SOUND_STOP;
      snd_test_block.smp_snd_id = 0xff;
  }
  /* Set the Volume */
  if ( snd_test_block.set_vol != 0xff ) {
    snd_set_volume( (snd_device_type) snd_test_block.vol_dev,
                    (snd_method_type) snd_test_block.vol_method,
                    (word) snd_test_block.vol_volume, NULL, NULL);
    snd_test_block.set_vol = 0xff;
  }


#ifdef FEATURE_MVS

  /* Configure MVS loopback type */
  if ( snd_test_block.mvs_loop != 0xff ) {
    mvs_set_loopback((mvs_loopback_type)(snd_test_block.mvs_loop));
    snd_test_block.mvs_loop = 0xff;
  }

  /* Activate or de-activate MVS */
  if ( snd_test_block.mvs_enable != 0xff ) {
    snd_test_mvs_enable((boolean)(snd_test_block.mvs_enable));
    snd_test_block.mvs_enable = 0xff;
  }

  /* Select MVS mode to be tested */
  if ( snd_test_block.mvs_mode != 0xff ) {
    snd_test_mvs_mode = (mvs_mode_type)(snd_test_block.mvs_mode & 0x0F);

    switch (snd_test_mvs_mode)
    {
#if defined(MSMAUD_VOC_IS127) || defined(MSMAUD_VOC_IS733) \
    || defined(MSMAUD_VOC_4GV)
#ifdef MSMAUD_VOC_IS127
      case MVS_MODE_IS127 :
#endif
#ifdef MSMAUD_VOC_IS733
      case MVS_MODE_IS733 :
#endif
#ifdef MSMAUD_VOC_4GV
      case MVS_MODE_4GV_NB :
      case MVS_MODE_4GV_WB :
#endif
        if ((snd_test_block.mvs_mode & 0xF0) == 0xF0)
        {
          mvssup_voc_set_frame_rate(snd_test_max_voc_rate,
                                    snd_test_min_voc_rate);
        }
        else
        {
          if ((snd_test_block.mvs_mode & 0x80) == 0)
          {
            snd_test_min_voc_rate
            = (mvs_voc_rate_type)((snd_test_block.mvs_mode & 0x70) >> 4);
          }
          else
          {
            snd_test_max_voc_rate
            = (mvs_voc_rate_type)((snd_test_block.mvs_mode & 0x70) >> 4);
          }
        }

        break;
#endif  /* MSMAUD_VOC_IS127 || MSMAUD_VOC_IS733 || MSMAUD_VOC_4GV */

#ifdef MSMAUD_VOC_AMR_WB
      case MVS_MODE_AMR_WB :
        snd_test_amr_mode
          = (mvs_amr_mode_type)((snd_test_block.mvs_mode & 0x70) >> 4);
        mvssup_amr_set_awb_mode(snd_test_amr_mode + MVS_AMR_MODE_0660);

        snd_test_scr_mode
          = (mvs_scr_mode_type)((snd_test_block.mvs_mode & 0x80) >> 7);
        mvssup_amr_set_scr_mode(snd_test_scr_mode);

        break;
#endif  /* MSMAUD_VOC_AMR_WB */

#ifdef MSMAUD_VOC_AMR
      case MVS_MODE_AMR :
        snd_test_amr_mode
          = (mvs_amr_mode_type)((snd_test_block.mvs_mode & 0x70) >> 4);
        mvssup_amr_set_amr_mode(snd_test_amr_mode);

        snd_test_scr_mode
          = (mvs_scr_mode_type)((snd_test_block.mvs_mode & 0x80) >> 7);
        mvssup_amr_set_scr_mode(snd_test_scr_mode);

        break;
#endif  /* MSMAUD_VOC_AMR */

#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR)
#ifdef MSMAUD_VOC_EFR
#error code not present
#endif
#ifdef MSMAUD_VOC_FR
#error code not present
#endif
#ifdef MSMAUD_VOC_HR
      case MVS_MODE_HR :
#endif
        snd_test_dtx_mode
          = (mvs_dtx_mode_type)((snd_test_block.mvs_mode & 0x80) >> 7);
        mvssup_gsm_set_dtx_mode(snd_test_dtx_mode);

        break;
#endif  /* MSMAUD_VOC_EFR || MSMAUD_VOC_FR || MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
      case MVS_MODE_G711:
        /* High bit of snd_test_block.mvs_mode is the G.711 mode */
        snd_test_g711_mode =
            (mvs_g711_mode_type)((snd_test_block.mvs_mode & 0x80) >> 7);
        mvssup_set_g711_mode(snd_test_g711_mode);
        break;
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
      case MVS_MODE_G723:
        /* High bit of snd_test_block.mvs_mode is the G.723 mode.
        ** 0=HIGH, 1=LOW
        */
        snd_test_g723_mode =
            (mvs_g723_mode_type)((snd_test_block.mvs_mode & 0x80) >> 7);
        mvssup_set_g723_mode(snd_test_g723_mode);
        break;
#endif /* MSMAUD_VOC_G723 */

      default :
        break;
    }

    snd_test_block.mvs_mode = 0xff;
  }

#endif  /* FEATURE_MVS */

  /* Test function wrapper */
  if ( snd_test_block.row93 != 0xff ) {
#ifndef MSMAUD_ADIE_SSBI_IO_TEST
    if (snd_test_block.row93 == 0) {
#ifdef FEATURE_AUDIO_AGC
    } else if (snd_test_block.row93 == 1) {
      snd_agc_ctl(TRUE,NULL,NULL);
    } else if (snd_test_block.row93 == 2) {
      snd_agc_ctl(FALSE,NULL,NULL);
    } else if (snd_test_block.row93 == 3) {
      snd_avc_ctl(TRUE,NULL,NULL);
    } else if (snd_test_block.row93 == 4) {
      snd_avc_ctl(FALSE,NULL,NULL);
    } else if (snd_test_block.row93 == 5) {
      (void)voc_tx_agc_cmd(TRUE);
    } else if (snd_test_block.row93 == 6) {
      (void)voc_tx_agc_cmd(FALSE);
#endif
    }
#endif /* MSMAUD_ADIE_SSBI_IO_TEST */

#ifdef MSMAUD_ADIE_SSBI_IO_TEST
    switch (snd_test_block.row93)
    {
      case 1: adie_codec_read(snd_test_block.row103, 0xff, &(snd_test_block.row104));
      break;

      case 2: adie_codec_write(snd_test_block.row103, 0xff, snd_test_block.row104);
      break;
    }
#endif /* MSMAUD_ADIE_SSBI_IO_TEST */
    snd_test_block.row93 = 0xff;
  }



  if(snd_test_block.smartsnd != 0xff)
  {
	  switch(snd_test_block.smartsnd)
	  {
		case 0:
			snd_smartsnd_ctl(SND_SMARTSND_NONE,NULL,NULL);
			break;
		case 1:
			snd_smartsnd_ctl(SND_SMARTSND_AGC_ENABLED_AIG_ENABLED,NULL,NULL);
			break;
		case 2:
			snd_smartsnd_ctl(SND_SMARTSND_AGC_ENABLED_AIG_DISABLED,NULL,NULL);
			break;
		case 3:
			snd_smartsnd_ctl(SND_SMARTSND_AVC_AGC_AIG_ENABLED,NULL,NULL);
			break;
		case 4:
			snd_smartsnd_ctl(SND_SMARTSND_AVC_AGC_ENABLED_AIG_DISABLED,NULL,NULL);
			break;
		case 5:
			snd_smartsnd_ctl(SND_SMARTSND_AVC_ENABLED_AGC_AIG_DISABLED,NULL,NULL);
			break;
		case 6:
			snd_smartsnd_ctl(SND_SMARTSND_RVE_AGC_AIG_ENABLED,NULL,NULL);
			break;
		case 7:
			snd_smartsnd_ctl(SND_SMARTSND_RVE_AGC_ENABLED_AIG_DISABLED,NULL,NULL);
			break;
		case 8:
			snd_smartsnd_ctl(SND_SMARTSND_RVE_ENABLED_AGC_AIG_DISABLED,NULL,NULL);
			break;
	  }
	  snd_test_block.smartsnd = 0xff;
  }


#ifdef FEATURE_TTY_ACTIVITY_STATUS
   /* Register callback to receive TTY Activty status reports */
   if ( snd_test_block.tty_activity != 0xff ) {
     voc_register_tty_status_client(snd_test_tty_activity_status_cb);
     snd_test_block.tty_activity = 0xff;
   }
#endif /* FEATURE_TTY_ACTIVITY_STATUS */

#ifndef MSMAUD_ADIE_SSBI_IO_TEST
#ifdef SND_HFK_PLUS
#error code not present
#endif
#endif /* MSMAUD_ADIE_SSBI_IO_TEST */

#ifdef SND_EXTCOM
#error code not present
#endif  /* End SND_EXTCOM */

#ifdef SND_AVBAP_SUPPORTED
#error code not present
#endif /* End SND_AVBAP_SUPPORTED */

#if defined(FEATURE_VOC_PCM_INTERFACE) && \
    defined(FEATURE_VOC_PCM_INTERFACE_TEST)

  /* Control RX PCM Input interface:
   * 0 to disable;
   * non-zero file index to enable.
   */
  if ( snd_test_block.pcm_rx_inp != 0xff ) {

    if (snd_test_block.pcm_rx_inp) {
      uint8 max_file_index = sizeof(snd_test_pcm_input_files) /
                                    SND_TEST_PCM_INPUT_FILE_NAME_LENGTH;

      if (snd_test_block.pcm_rx_inp > max_file_index) {
        MSG_FATAL("Ignoring invalid test config", 0,0,0);
      }
      else {
        snd_test_enable_pcm_file_input(VOC_PCM_INTERFACE_RX_INPUT,
                                       snd_test_block.pcm_rx_inp - 1);
      }
    }
    else {
      snd_test_disable_pcm_input(VOC_PCM_INTERFACE_RX_INPUT);
    }
    snd_test_block.pcm_rx_inp = 0xff;
  }

  /* Control TX PCM Input interface:
   * 0 to disable;
   * non-zero file index to enable.
   */
  if ( snd_test_block.pcm_tx_inp != 0xff ) {

    if (snd_test_block.pcm_tx_inp) {
      uint8 max_file_index = sizeof(snd_test_pcm_input_files) /
                                    SND_TEST_PCM_INPUT_FILE_NAME_LENGTH;

      if (snd_test_block.pcm_tx_inp > max_file_index) {
        MSG_FATAL("Ignoring invalid test config", 0,0,0);
      }
      else {
        snd_test_enable_pcm_file_input(VOC_PCM_INTERFACE_TX_INPUT,
                                       snd_test_block.pcm_tx_inp - 1);
      }
    }
    else {
      snd_test_disable_pcm_input(VOC_PCM_INTERFACE_TX_INPUT);
    }
    snd_test_block.pcm_tx_inp = 0xff;
  }

  /* Control RX PCM Output interface:
   * 0 to disable;
   * 1 to log to a file;
   * 2 to register a callback which does nothing (log to NULL device).
   */
  if ( snd_test_block.pcm_rx_out != 0xff ) {

    if (snd_test_block.pcm_rx_out == 1) {
      snd_test_enable_and_log_pcm_output(VOC_PCM_INTERFACE_RX_OUTPUT);
    }
    else if (snd_test_block.pcm_rx_out == 2) {
      snd_test_pcm_output_null(VOC_PCM_INTERFACE_RX_OUTPUT);
    }
    else {
      snd_test_disable_pcm_output(VOC_PCM_INTERFACE_RX_OUTPUT);
    }
    snd_test_block.pcm_rx_out = 0xff;
  }

  /* Control TX PCM Output interface:
   * 0 to disable;
   * 1 to log to a file;
   * 2 to register a callback which does nothing (log to NULL device)
   */
  if ( snd_test_block.pcm_tx_out != 0xff ) {

    if (snd_test_block.pcm_tx_out == 1) {
      snd_test_enable_and_log_pcm_output(VOC_PCM_INTERFACE_TX_OUTPUT);
    }
    else if (snd_test_block.pcm_tx_out == 2) {
      snd_test_pcm_output_null(VOC_PCM_INTERFACE_TX_OUTPUT);
    }
    else {
      snd_test_disable_pcm_output(VOC_PCM_INTERFACE_TX_OUTPUT);
    }
    snd_test_block.pcm_tx_out = 0xff;
  }

  if (snd_test_pcm_control.bitmask) {
    /* Log and pending buffers for output channels */
    snd_test_log_pcm();

    /* Read PCM file for input channel */
    snd_test_read_pcm_file();
  }

  /* Loopback PCM output to PCM input interfaces:
   * 0 to disable;
   * 1 to loop RX PCM Output to RX PCM Input;
   * 2 to loop TX PCM Output to RX PCM Input;
   * 3 to Release vocoder (and not disable any PCM interfaces) - useful
   *   for transitioning to SLEEP.
   */
  if ( snd_test_block.pcm_int_loop != 0xff ) {
    switch (snd_test_block.pcm_int_loop) {
      case 0: /* Disable PCM Interface loopback */
        MSG_ERROR("Disabling PCM loopback", 0,0,0);
        snd_test_disable_pcm_interface_loopback();
        break;

      case 1: /* Loop RX PCM Output to RX PCM Input */
        if (snd_test_pcm_loopback_src != VOC_PCM_INTERFACE_MAX) {
          /* Disable any existing loop */
          snd_test_disable_pcm_interface_loopback();
        }
        MSG_ERROR("Looping RX PCM output to RX input", 0,0,0);
        snd_test_enable_pcm_interface_loopback(VOC_PCM_INTERFACE_RX_OUTPUT,
                                               VOC_PCM_INTERFACE_RX_INPUT);
        break;

      case 2: /* Loop TX PCM Output to RX PCM Input */
        if (snd_test_pcm_loopback_src != VOC_PCM_INTERFACE_MAX) {
          /* Disable any existing loop */
          snd_test_disable_pcm_interface_loopback();
        }
        MSG_ERROR("Looping TX PCM output to RX input", 0,0,0);
        snd_test_enable_pcm_interface_loopback(VOC_PCM_INTERFACE_TX_OUTPUT,
                                               VOC_PCM_INTERFACE_RX_INPUT);
        break;

      case 3: /* Loop RX PCM Output to TX PCM Input */
        if (snd_test_pcm_loopback_src != VOC_PCM_INTERFACE_MAX) {
          /* Disable any existing loop */
          snd_test_disable_pcm_interface_loopback();
        }
        MSG_ERROR("Looping RX PCM output to TX input", 0,0,0);
        snd_test_enable_pcm_interface_loopback(VOC_PCM_INTERFACE_RX_OUTPUT,
                                               VOC_PCM_INTERFACE_TX_INPUT);
        break;

      case 4: /* Loop TX PCM Output to TX PCM Input */
        if (snd_test_pcm_loopback_src != VOC_PCM_INTERFACE_MAX) {
          /* Disable any existing loop */
          snd_test_disable_pcm_interface_loopback();
        }
        MSG_ERROR("Looping TX PCM output to TX input", 0,0,0);
        snd_test_enable_pcm_interface_loopback(VOC_PCM_INTERFACE_TX_OUTPUT,
                                               VOC_PCM_INTERFACE_TX_INPUT);
        break;

      case 5:
        MSG_ERROR("Releasing vocoder", 0,0,0);
        voc_release(VOC_ACQ_TEST);
        snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_MUTED, SND_MUTE_MUTED,
                       NULL, NULL);
        break;

      default:
        MSG_ERROR("Invalid PCM Int. loopback mode",0,0,0);
        break;
    }
    snd_test_block.pcm_int_loop = 0xff;
  }
#endif /* FEATURE_VOC_PCM_INTERFACE && FEATURE_VOC_PCM_INTERFACE_TEST */

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

#ifdef FEATURE_AAC_REC
  if (snd_test_block.aac_rec != 0xFF) {

    if (snd_test_block.aac_rec == 1) {
      snd_test_block.aac_rec = 0xFF;

      aac_handle.efs.source_type = CMX_SOURCE_EFS;

      memcpy(aac_handle.efs.filename,
             aac_file[aac_param.mm_rec_param.cmx_mm_rec_aac_para.chan]
                     [aac_param.mm_rec_param.cmx_mm_rec_aac_para.sample_rate],
             sizeof(aac_handle.efs.filename));

      cmx_mm_record(&aac_handle,
                    CMX_MM_REC_OPERATION_NORMAL,
                    &aac_param,
                    NULL,
                    NULL);

    } else if (snd_test_block.aac_rec == 0) {

      snd_test_block.aac_rec = 0xFF;

      cmx_mm_record_stop(CMX_MM_REC_LINK_REVERSE, NULL, NULL);
    }
  }
#endif /* FEATURE_AAC_REC */

#ifdef FEATURE_AAC
  if (snd_test_block.adif_test != 0xFF) {

    snd_test_block.adif_test = 0xFF;

    aac_handle.efs.source_type = CMX_SOURCE_EFS;

    memcpy(aac_handle.efs.filename,
           "brew/mod/mediaplayer/media/L1_8000.aac",
           sizeof("brew/mod/mediaplayer/media/L1_8000.aac"));

    adif_spec.aac_codec.codec_type.file_type = CMX_AF_FILE_AAC;

    adif_spec.aac_codec.format       = CMX_AF_AAC_DATA_FORMAT_ADIF;
    adif_spec.aac_codec.audio_object = CMX_AF_MP4_AUDIO_OBJECT_AAC_LC;
    adif_spec.aac_codec.sample_rate  = CMX_AF_SAMPLE_RATE_8000;
    adif_spec.aac_codec.num_channels = CMX_AF_AAC_CHANNEL_MONO;
    adif_spec.aac_codec.bit_rate     = 0x9c40;

    adif_spec.aac_codec.ep_config                            = 0;
    adif_spec.aac_codec.aac_section_data_resilience_flag     = 0;
    adif_spec.aac_codec.aac_scalefactor_data_resilience_flag = 0;
    adif_spec.aac_codec.aac_spectral_data_resilience_flag    = 0;

    cmx_audfmt_play_codec(&aac_handle,
                          &adif_spec,
                          NULL,
                          NULL);

  }
#endif /* FEATURE_AAC */

#ifdef FEATURE_AUDFMT_EVB
  if ( snd_test_block.evb_rec_op != 0xFF )
  {
    switch ( snd_test_block.evb_rec_op )
    {
      /*----------------------------------------------------------*/
      /* START REVERSE LINK RECORDING */
      /*----------------------------------------------------------*/
      case 1:
        /* Setup EFS handle */
        evb_handle.efs.source_type = CMX_SOURCE_EFS;
        memcpy(evb_handle.efs.filename,
               "brew/mod/mediaplayer/media/evb_test_reverse.qcp",
               sizeof("brew/mod/mediaplayer/media/evb_test_reverse.qcp"));

        /* Setup rec_param based on requested format */
        if ( snd_test_block.evb_rec_format == CMX_EVB_FORMAT_VAR_FULL_4GV_NB )
        {
          cmx_qcp_rec_para.format = snd_test_block.evb_rec_format;
          cmx_qcp_rec_para.report_ms = 0;
          cmx_qcp_rec_para.auto_stop_ms = 0;
        }
        else
        {
          MSG_ERROR("EVB Record Test: invalid evb_rec_format value.", 0, 0, 0);
          break;
        }

        /* Start recording */
        cmx_qcp_record_reverse(&evb_handle,
                               &cmx_qcp_rec_para,
                               NULL,
                               NULL);
        break;

      /*----------------------------------------------------------*/
      /* START FORWARD LINK RECORDING */
      /*----------------------------------------------------------*/
      case 2:
        /* Setup EFS handle */
        evb_handle.efs.source_type = CMX_SOURCE_EFS;
        memcpy(evb_handle.efs.filename,
               "brew/mod/mediaplayer/media/evb_test_forward.qcp",
               sizeof("brew/mod/mediaplayer/media/evb_test_forward.qcp"));

        /* Setup rec_param based on requested format */
        if ( snd_test_block.evb_rec_format == CMX_EVB_FORMAT_VAR_FULL_4GV_NB )
        {
          cmx_qcp_rec_para.format = snd_test_block.evb_rec_format;
          cmx_qcp_rec_para.report_ms = 0;
          cmx_qcp_rec_para.auto_stop_ms = 0;
        }
        else
        {
          MSG_ERROR("EVB Record Test: invalid evb_rec_format value.", 0, 0, 0);
          break;
        }

        /* Start recording */
        cmx_qcp_record_forward(&evb_handle,
                               &cmx_qcp_rec_para,
                               NULL,
                               NULL);
        break;

      /*----------------------------------------------------------*/
      /* PAUSE RECORDING */
      /*----------------------------------------------------------*/
      case 3:
        cmx_qcp_record_pause(NULL, NULL);
        break;

      /*----------------------------------------------------------*/
      /* RESUME RECORDING */
      /*----------------------------------------------------------*/
      case 4:
        cmx_qcp_record_resume(NULL, NULL);
        break;

      /*----------------------------------------------------------*/
      /* STOP RECORDING */
      /*----------------------------------------------------------*/
      case 5:
        cmx_qcp_record_stop(NULL, NULL);
        break;

      default:
        MSG_ERROR("EVB Record Test: invalid evb_rec value.", 0, 0, 0);

    }

    snd_test_block.evb_rec_op = 0xFF;
    snd_test_block.evb_rec_format = 0xFF;
  }
#endif /* FEATURE_AUDFMT_EVB */




#ifdef FEATURE_AUDFMT_EVW
  if ( snd_test_block.evw_rec_op != 0xFF )
  {
    switch ( snd_test_block.evw_rec_op )
    {
      /*----------------------------------------------------------*/
      /* START REVERSE LINK RECORDING */
      /*----------------------------------------------------------*/
      case 1:
        /* Setup EFS handle */
        evw_handle.efs.source_type = CMX_SOURCE_EFS;
        memcpy(evw_handle.efs.filename,
               "brew/mod/mediaplayer/media/evw_test_reverse.qcp",
               sizeof("brew/mod/mediaplayer/media/evw_test_reverse.qcp"));

        /* Setup rec_param based on requested format */
        if ( snd_test_block.evw_rec_format == CMX_EVW_FORMAT_VAR_FULL_4GV_WB )
        {
          cmx_qcp_rec_para_evw.format = snd_test_block.evw_rec_format;
          cmx_qcp_rec_para_evw.report_ms = 0;
          cmx_qcp_rec_para_evw.auto_stop_ms = 0;
        }
        else
        {
          MSG_ERROR("EVW Record Test: invalid evw_rec_format value.", 0, 0, 0);
          break;
        }

        /* Start recording */
        cmx_qcp_record_reverse(&evw_handle,
                               &cmx_qcp_rec_para_evw,
                               NULL,
                               NULL);
        break;

      /*----------------------------------------------------------*/
      /* START FORWARD LINK RECORDING */
      /*----------------------------------------------------------*/
      case 2:
        /* Setup EFS handle */
        evw_handle.efs.source_type = CMX_SOURCE_EFS;
        memcpy(evw_handle.efs.filename,
               "brew/mod/mediaplayer/media/evw_test_forward.qcp",
               sizeof("brew/mod/mediaplayer/media/evw_test_forward.qcp"));

        /* Setup rec_param based on requested format */
        if ( snd_test_block.evw_rec_format == CMX_EVW_FORMAT_VAR_FULL_4GV_WB )
        {
          cmx_qcp_rec_para_evw.format = snd_test_block.evw_rec_format;
          cmx_qcp_rec_para_evw.report_ms = 0;
          cmx_qcp_rec_para_evw.auto_stop_ms = 0;
        }
        else
        {
          MSG_ERROR("EVW Record Test: invalid evw_rec_format value.", 0, 0, 0);
          break;
        }

        /* Start recording */
        cmx_qcp_record_forward(&evw_handle,
                               &cmx_qcp_rec_para_evw,
                               NULL,
                               NULL);
        break;

      /*----------------------------------------------------------*/
      /* PAUSE RECORDING */
      /*----------------------------------------------------------*/
      case 3:
        cmx_qcp_record_pause(NULL, NULL);
        break;

      /*----------------------------------------------------------*/
      /* RESUME RECORDING */
      /*----------------------------------------------------------*/
      case 4:
        cmx_qcp_record_resume(NULL, NULL);
        break;

      /*----------------------------------------------------------*/
      /* STOP RECORDING */
      /*----------------------------------------------------------*/
      case 5:
        cmx_qcp_record_stop(NULL, NULL);
        break;

      default:
        MSG_ERROR("EVW Record Test: invalid evw_rec value.", 0, 0, 0);

    }

    snd_test_block.evw_rec_op = 0xFF;
    snd_test_block.evw_rec_format = 0xFF;
  }
#endif /* FEATURE_AUDFMT_EVW */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

  if(snd_test_block.st_ctl != 0xFF){
    snd_sidetone_ctl(SND_SIDETONE_FORCE_DISABLED, NULL, NULL);
    snd_test_block.st_ctl = 0xFF;
  }

#if defined(FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST) && \
    defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
  if (snd_test_block.amr_wb_test != 0xFF) {
    cmx_handle_type cmx_handle;
    cmx_codec_spec_type   codec_spec;
    cmx_af_ext_speech_codec_type *ext_speech;
    cmx_av_sync_type      av_sync;
#ifdef FEATURE_EFS
    static char test_input_file[FS_FILENAME_MAX_LENGTH_P] = "mmc1/test.dsp";
    fs_rsp_msg_type       rsp_open;
    fs_rsp_msg_type       rsp_read;
    fs_rsp_msg_type       rsp_close;
    fs_access_option_type open_type = FS_OA_READONLY;
#endif /* FEATURE_EFS */

    switch (snd_test_block.amr_wb_test) {
      case 1:
        MSG_ERROR("AMR-WB/WB+ TEST: PLAYING CODEC", 0, 0, 0);

        snd_test_adec_len = 0;

#ifdef FEATURE_EFS
        /* Setup the file to read */
        cmx_handle.source_type = CMX_SOURCE_EFS;
        strcpy(cmx_handle.efs.filename, test_input_file);

        /* Read the file into our test buffer */
        fs_open(cmx_handle.efs.filename, open_type, NULL, NULL, &rsp_open);

        if (rsp_open.open.status == FS_OKAY_S) {
          fs_read(rsp_open.open.handle, &snd_test_adec_buffer,
                  sizeof(snd_test_adec_buffer), NULL, &rsp_read);

          if (rsp_read.read.status == FS_OKAY_S) {
            MSG_ERROR("AMR-WB/WB+ TEST: FILE=%s, SIZE=%d",
                      cmx_handle.efs.filename, rsp_read.read.count, 0);

            snd_test_adec_len = rsp_read.read.count;
          } else {
            MSG_ERROR("AMR-WB/WB+ TEST: FILE=%s, SIZE=%d, read problem?",
                      cmx_handle.efs.filename, rsp_read.read.count, 0);

            snd_test_adec_len = rsp_read.read.count;

            /* Maybe we need to abort if there really is a problem */
          }

          fs_close(rsp_open.open.handle, NULL, &rsp_close);
        } else {
          MSG_ERROR("AMR-WB/WB+ TEST: FILE=%s, couldn't open file",
                    cmx_handle.efs.filename, 0, 0);
          break;
        }
#endif /* FEATURE_EFS */

        /* Setup the memory to use for streaming */
        cmx_handle.source_type = CMX_SOURCE_MEM;
        cmx_handle.mem.buf = snd_test_adec_buffer;
        cmx_handle.mem.len = snd_test_adec_len;

        /* Setup the codec spec */
        ext_speech = &codec_spec.ext_speech_codec;
        ext_speech->codec_type.file_type = CMX_AF_FILE_AMR_WBPLUS;
        ext_speech->sample_rate          = CMX_AF_SAMPLE_RATE_48000;
        ext_speech->channel_mode         = CMX_AF_CHANNEL_DUAL;

        /* Setup the A/V sync */
        av_sync.av_sync_cb_func  = snd_test_av_sync_cb;
        av_sync.av_sync_interval = 2048;
        av_sync.av_sync_mode     = CMX_AV_SYNC_SAMPLES;
        av_sync.client_data      = NULL;

        /* Play codec */
        cmx_audfmt_play_codec_av_sync(&cmx_handle, &codec_spec, &av_sync,
                                      NULL, NULL);
        break;

      case 2:
        MSG_ERROR("AMR-WB/WB+ TEST: STOPPING", 0, 0, 0);

        snd_test_adec_len = 0;

        cmx_audio_playback_control(CMX_AUDIO_PB_CMD_STOP, NULL, NULL);
        break;

      case 3:
        MSG_ERROR("AMR-WB/WB+ TEST: PAUSING", 0, 0, 0);
        cmx_audio_playback_control(CMX_AUDIO_PB_CMD_PAUSE, NULL, NULL);
        break;

      case 4:
        MSG_ERROR("AMR-WB/WB+ TEST: RESUMING", 0, 0, 0);
        cmx_audio_playback_control(CMX_AUDIO_PB_CMD_RESUME, NULL, NULL);
        break;

      case 5:
        MSG_ERROR("AMR-WB/WB+ TEST: FLUSHING", 0, 0, 0);
        cmx_audio_playback_control(CMX_AUDIO_PB_CMD_FLUSH, NULL, NULL);
        break;

      default:
        MSG_ERROR("AMR-WB/WB+ TEST: UNSUPPORTED COMMAND", 0, 0, 0);
        break;
    }

    snd_test_block.amr_wb_test = 0xFF;
  }
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST && FEATURE_AMR_WBPLUS_AUDIO_DEC */

#ifdef SND_TEST_CODEC_OVERRIDE
	if (snd_test_block.codec_override_flag != 0xff)
	{
		/* check if the codec is override flag is set or not? */
		if (snd_test_block.codec_override_flag == TRUE)
		{
			/* override the codec */
			voc_set_codec_override(snd_test_block.codec_to_be_override,
				snd_test_block.codec_override, TRUE);
		}
		else if (snd_test_block.codec_override_flag == FALSE)
		{
			/* disable the override the codec */
			voc_set_codec_override(snd_test_block.codec_to_be_override,
				snd_test_block.codec_override, FALSE);
		}

		snd_test_block.codec_override_flag = 0xff;
	}
#endif // SND_TEST_CODEC_OVERRIDE

  if (snd_test_block.slowtalk_expansion_ratio != 0xff)
  {
    snd_slowtalk_params_type params;

    params.expansion_ratio_level =
        (uint16)snd_test_block.slowtalk_expansion_ratio;
    snd_set_slowtalk_params(&params, NULL, NULL);
    snd_test_block.slowtalk_expansion_ratio = 0xff;
  }

  if (snd_test_block.slowtalk_enable_flag != 0xff)
  {
    if (snd_test_block.slowtalk_enable_flag < 2) {
      snd_slowtalk_ctl(snd_test_block.slowtalk_enable_flag, NULL, NULL);
    }
    snd_test_block.slowtalk_enable_flag = 0xff;
  }

  /* Test code for Aux Line In to USB path */
  if ( snd_test_block.snd_device_usb_flag == TRUE){

#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    /* set the sound device as stereo usb */
    snd_set_device(SND_DEVICE_STEREO_USB,
                 SND_MUTE_MUTED,
                 SND_MUTE_MUTED,
                 NULL, NULL);
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

    snd_test_block.snd_device_usb_flag = FALSE;
  }

  /* configures the AuxPGA line in path */
  if(snd_test_block.aux_line_in_on == TRUE)
  {
    snd_set_aux_line_in(NULL,NULL,SND_AUX_LINE_IN_MODE_ON);
    snd_test_block.aux_line_in_on = FALSE;
  }
  else if (snd_test_block.aux_line_in_off == TRUE) {
    snd_set_aux_line_in(NULL,NULL,SND_AUX_LINE_IN_MODE_OFF);
    snd_test_block.aux_line_in_off = FALSE;
  }
#ifdef FEATURE_BT_AG
      /* FM over BT AG */
      if (snd_test_block.fm_over_bt_ag == 1)
      {
          snd_set_device(SND_DEVICE_BT_HEADSET, SND_MUTE_UNMUTED,
                         SND_MUTE_UNMUTED,
                         NULL, NULL);
/*          voc_acquire(VOC_ACQ_TEST, NULL);*/
          snd_test_block.fm_over_bt_ag = 0xff;

      }
      else if (snd_test_block.fm_over_bt_ag == 2)
      {
/*         voc_release(VOC_ACQ_TEST);*/
         snd_set_device(SND_DEVICE_CURRENT, SND_MUTE_MUTED,
                        SND_MUTE_MUTED,
                        NULL, NULL);
         snd_test_block.fm_over_bt_ag = 0xff;
      }
#endif

}

#ifdef FEATURE_ARM_POST_PROCESSING
void snd_test_pp_cb (const snd_pp_buffer_type snd_test_pp_buffer )
{
  uint16 frame_size = 0;

  if (snd_test_pp_fs_handle != NULL)
  {
    fs_write(snd_test_pp_fs_handle, (uint8*)snd_test_pp_buffer.pp_data,
             snd_test_pp_buffer.pp_buf_length * 2, NULL, &snd_test_pp_snd_fs_rsp);

  }
  /*
  frame_size = snd_test_pp_buffer.pp_buf_length * snd_test_pp_buffer.pp_num_channels;
  */
  mmu_invalidate_data_cache_lines( (uint32*) snd_test_pp_buffer.pp_data ,
                                              (uint32) snd_test_pp_buffer.pp_buf_length );
  frame_size = snd_test_pp_buffer.pp_buf_length;
  while (snd_test_pp_file_shift != 0 && frame_size != 0)
  {
    *(snd_test_pp_buffer.pp_data  + frame_size - 1) <<= (snd_test_pp_file_shift - 1);
    frame_size--;
  }
    snd_pp_done (snd_test_pp_buffer, NULL, NULL);
}
#endif /* FEATURE_ARM_POST_PROCESSING */

#ifdef  FEATURE_AVS_WB_PCM_RX_INTERFACE
void snd_test_rx_pcm_cb(const snd_pp_buffer_type snd_test_pp_buffer)
{
   sndhw_free_wb_pcm_rx_interface_buffer(snd_test_pp_buffer.pp_id);
}
#endif /* FEATURE_AVS_WB_PCM_RX_INTERFACE*/

#endif /* SND_TEST_CODE */

