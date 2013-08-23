/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Vocoder Task")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P 2   V O C O D E R   D R I V E R
                 A P I   F U N C T I O N S   A N D   T A S K
                     
GENERAL DESCRIPTION
  This task controls the QDSP2 vocoder.

EXTERNALIZED FUNCTIONS
  voc_task
    This is the entrance procedure for the Vocoder Task.  It contains the
    main processing loop for the task.
  voc_acquire
    Initialize the vocoder.
  voc_release
    Release the vocoder.
  voc_config
    Configure the vocoder for a specific service option.
  voc_config_standby
    Configure a specific vocoder into idle state.
  voc_init
    Causes the vocoder to re-acquire frame timing.
  voc_init_analog
    Initialize the vocoder for analog operation.
  voc_shutdown_analog
    Release the vocoder from analog operation.
  voc_get_status
    Get the current vocoder state.
  voc_capability_supported
    Check whether a service option is supported.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  voc_task() must be started up before calling any of the vocoder API functions.

  Copyright(c) 1999 - 2009 by QUALCOMM, Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/voctask.c#10 $ $DateTime: 2009/10/28 22:03:03 $ $Author: kmodak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/09   knm     Fixed compilation error after marimba driver integration.
10/26/09   sud     Removed dependency on feature FEATURE_VOC_AMR_WB for feature
                   .AWB file playback.
                   Updated Copyright.
10/08/09   knm     Integrated new marimba codec driver for SCMM
10/06/09   sj      Added SCMM specific changes
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
04/06/09    av     Adding VP3 Feature.
                   Copy right updated.
02/07/08    kk     Removed redundent code while adding support for Qensemble 
                   phase 2 interface changes.
11/14/07    hs     Added QEnsemble support
10/26/07    hs     Added EVW file format support
09/13/07   anb     Fixed the AUDFMT_QCONCERT task def timer issue.
08/27/07   ymc     Added PCM attribute support.
03/15/07   kan     Added 4GV-WB Code 
02/26/07   kan     Added AMR-WB changes 
12/02/06   suk     Added support for VOIP Combo Vocoder, and Combo vocoder 
                   with G711 and 4GV.
11/10/06   act     Resolved compile warning in voc_clients_requested_cap_dir()
09/11/06    ak     Added functions voc_clients_requested_cap, 
                   voc_clients_requested_cap_dir and voc_check_rec_capability.
                   Made the function voc_check_pb_capability more generic. 
06/16/06    sg     Fixed 13k featurization in voc_capability_supported().
                   Modified voc_check_pb_capability() to handle
                   QDSP_IMAGE_VOC_COMBO.
xx/xx/xx    sm     Added support for standby when others have higher priority.
02/03/06    sm     Added 4GV VOIP support
01/31/06    hz     Added support for in-call mp3 feature.
09/28/05    sp     Fixed a compiler error that occurs on thinUI builds.
09/18/05    sp     Added support for DSP based SBC encoder.
                   Added retry timers at various BT configuration states.
09/18/05    pl     Added support for audio decoder command retry mechanism.
08/16/05    st     Added VOC_QCONCERT_RETRY_TIMER_SIG to rex_wait().
07/05/05    sm     Added G.723.1 support.
05/25/05    aw     Added support for H264D_AMR image.
05/24/05    hz     Added support for external SADC init for MultiMedia 
                   recording.
04/28/05   ymc     Added 4GV NB support.
04/18/05    sm     Added GSM/WCDMA TTY support.
04/11/05    sg     Modified to set the default codec cal. by calling
                   voc_state_set_codec_cal().
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/02/05    aw     Added support for Qconcert; removed unused header file.
02/22/05    sm     Added G.711 vocoder support.
01/25/05    st     Removed FEATURE_BREW_3_0_VOC_SIG.
01/07/05   act     Added support for FEATURE_VOC_NV_CAL. Modified
                   voc_task() to include initialization for voc cal 
                   database.
12/03/04    sm     Added FEATURE_VOC_DTMF_DETECT.  Brew 3.0 support.
12/01/04    aw     Added voc_check_pb_capability to check supported voice
                   playback type.
11/04/04    aw     Added support for DFM SAT on AAC image.
10/27/04    aw     Added support for DFM SAT on QTunes image.
10/14/04    sg     Added a retry timer for ADEC flush command.
09/15/04    sm     In voc_capability_supported(), featurize EVRC vocoder 
                   under MSMAUD_VOC_IS127.
08/19/04    sg     Added voc_config_standby()
06/17/04    aw     Added code to initialize the critical section handle.
06/08/04    aw     Added new signal for Equalizer.
05/04/04    sm     Added FEATURE_PMIC_SPEAKER_ON_OFF_DELAY.
03/04/04    sg     Removed unused API.
01/26/04    sg     Added voc_sleep() API.
12/08/03    sg     Modified to support changes in the vocoder PCM interface.
12/08/03    sg     Modified voc_get_service_option() to return the correct
                   service option, based on the default image being
                   used in voc_state_get_high_image().
05/13/03   ymc     Added GSM half rate support.
           ymc     Fixed default service option setting.
04/29/03    st     Fixed reference to VOC_CAP_GSM_FR.
04/17/03    sm     Corrected featurization problem in voc_get_service_option().
            sg     Added support for vocoder PCM interfaces.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
11/06/02    sm     Include msmaud.h
10/17/02   ymc     Enabled auto sample slipping for VOC_ACQ_MVS.
09/09/02   ymc     Added FEATURE_MVS.
           ymc     Added voc_get_curr_config.
09/06/02    st     Added support for multimode architecture.
08/23/02    sg     Modified code to use FEATURE_EXTERNAL_SDAC.
08/22/02    sg     Added support for TTY activity status indication.
                   (Merged from MSM_AVS.01.05.18.01.02).
08/13/02    st     Added support for MP4 audio decoder.
03/21/02    st     Modified SDAC timer code to reuse for internal codec in
                   support for MSMAUD_CODEC_CLOCK_CTRL feature.
03/12/02    st     Modified code to set config_flag if necessary.
01/15/02   ymc     Added GSM and WCDMA support.
           ymc     Updated copyright.
12/12/01    aw     Use msmaud.h to eliminate targetization.
09/03/01    st     Added code to record a single vocoder frame.
08/14/01   ymc     Removed config_flag support.
07/24/01   ymc     Set config_flag in voc_config_processing.
06/18/01  jkl/ym   Added FEATURE_ACP.
06/05/01    sm     Call voc_tty_cmd() instead of voc_state_set_tty().
05/30/01    sm     Write TTY mode on tty processing call for MSM5100.
10/23/00    st     Modified code in order to have the QSynth config higher
                   priority than the DFM config so that midi ringers can play
                   during incoming analog calls, this is just temporary and
                   will be designed out in the next redesign.
10/13/00   ymc     Featurized support of internal SDAC as MSMHW_INTERNAL_STEREO_DAC.
08/28/00    ro     Uses QDSP Services.
                   Removed voc_offline, voc_online, 
08/24/00    sm     Enable GPIO sdac lines for MSM31 only.
07/15/00    st     Lint updates.
07/14/00   spf     Replaced memory.h with string.h.
05/16/00    st     Added support for codec config retry timer; added support
                   for stereo DAC; modified voc_config_processing() such that
                   VOC_CAP_NONE will unconfig the vocoder for that client,
                   CR13462.
05/12/00   spf     Add TTY support: voc_set_tty_mode() and 
                   voc_set_tty_mode_processing()
05/02/00   spf     Removed call to voc_data_get_dfm_sat_dcc_notify() in
                   voc_process_dfm_dat_dcc().
02/21/00    ro     Check slip mode in acquire/release and not config.
01/25/00    ro     Added support for VR and VM. Added voc_get_service_option,
                   voc_set_vr_mode(). Removed voc_set_msm_mode.
                   Use voc_state_set_codec.
12/07/99   spf     Removed write to DMSS database for DB_VOC_13K_AVAILABLE.
12/01/99   spf     Added TASK_STOP_SIG and TASK_OFFLINE_SIG support.
10/26/99   jkl     Updated comments. Modified voc_task_init.
10/07/99   spf     Backed-out 20ms timer-based state transition retry 
                   mechanism.
10/04/99   jkl     Initialized CODEC selection in voc_task_init().
09/23/99   spf     Implemented 20ms timer-based state transition retry
                   mechanism.
09/16/99   spf     Removed FEATURE_QDSP2 and FEATURE_VOC_SAT_CONTROL.
09/10/99   spf     voc_online, voc_offline support.  voc_init_analog 
                   optimization.
08/02/99   spf     Featurized voc_init_analog() with FEATURE_VOC_SAT_CONTROL.
07/15/99   spf     DFM support.
06/15/99   ro/sf   First version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer feature specific              */
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "rex.h"                /* definition of REX data types           */
#include "task.h"               /* Task definitions and prototypes        */
#include "msg.h"                /* Message logging/reporting services     */
#include "err.h"                /* Error logging/reporting services       */
#include "dog.h"                /* Watchdog signal and time definitions   */
#include "msmaud.h"             /* Target specific audio features         */
#include "vocdata.h"            /* Vocoder State and Cal Data Definitions */
#include "voci.h"               /* Internal Vocoder Task Definitions      */
#include "vocstate.h"           /* Vocoder State Controller Definitions   */
#include "voc.h"                /* Vocoder interface definition           */
#include "vocsup.h"             /* Vocoder interface definition           */
#include "qdspcmd.h"            /* QDSP2 Driver Command Layer definitions */
#include "mc.h"                 /* Need for MC signals and prototypes     */
#include <string.h>             /* Interface to string library            */

#ifdef FEATURE_EXTERNAL_SDAC
#include "sdac.h"               /* External Stereo DAC driver prototypes  */
#include "bio.h"                /* GPIO definitions                       */
#endif

#ifdef FEATURE_EXTERNAL_SADC
#include "sadc.h"               /* External Stereo ADC driver prototypes  */
#endif

#ifdef FEATURE_MVS
#include "mvsi.h"               /* MVS internal definitions               */
#endif /* FEATURE_MVS */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data and regional data.  There is no global
data.  Since C does not directly support regional data, regional data is
labelled "[Regional Data]".

===========================================================================*/

/*-------------------------------------------------------------------------
** Timers
**-----------------------------------------------------------------------*/
rex_timer_type voc_rpt_timer;         /* Watch Dog Timer                */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

uint16  voc_bt_config_retry_timer_value[VOC_BT_RETRY_TIMER_MAX] = {
   0
  ,5
#ifdef FEATURE_BT_AG
  ,5 
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  ,5 
#endif
#ifdef FEATURE_SBC_DSP_CODEC
  ,10 
#endif
};
rex_timer_type voc_bt_config_retry_timer; /* Command retry Timer            */
#endif

#define VOC_CODEC_RETRY_TIME  5       /* Command retry time             */
rex_timer_type voc_codec_retry_timer; /* Command retry Timer            */

#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
#define VOC_CLK_CTRL_TIME  5                  /* Check clock ctrl time  */
rex_timer_type voc_clk_ctrl_timer;            /* Clock Disable Timer    */
uint32         voc_clk_ctrl_mask_values = 0;  /* Clocks to disable      */
#endif /* FEATURE_EXTERNAL_SDAC || MSMAUD_CODEC_CLOCK_CTRL */

#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
voc_pmic_delay_cb_func_type voc_pmic_delay_cb = NULL;

rex_timer_type voc_pmic_delay_timer;
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */

#ifdef FEATURE_AUDFMT_EQUALIZER
#define VOC_EQ_RETRY_TIME  10                /* Equalizer retry time     */
rex_timer_type voc_equalizer_retry_timer;    /* Equalizer command retry  */
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_QCONCERT
#define VOC_QCONCERT_RETRY_TIME  10          /* QConcert retry time      */
rex_timer_type voc_qconcert_retry_timer;     /* QConcert command retry   */
#endif /* FEATURE_AUDFMT_QCONCERT */

#ifdef FEATURE_QTUNES_COMMON
#define VOC_ADEC_FLUSH_RETRY_TIME  10        /* Flush retry time         */
rex_timer_type voc_adec_flush_retry_timer;   /* ADEC flush retry timer   */
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_VOC_DTMF_DETECT
rex_timer_type voc_dtmf_detect_delay_timer;  /* DTMF detection delay tmr */

voc_dtmf_detect_cb_type voc_rx_detect_cb = NULL;
voc_dtmf_detect_cb_type voc_tx_detect_cb = NULL;
#endif /* FEATURE_VOC_DTMF_DETECT */

#ifdef FEATURE_GASYNTH_INTERFACE
rex_timer_type voc_adec_cmd_delay_timer;     /* ADEC command retry timer */
#endif /* FEATURE_GASYNTH_INTERFACE */

/*-------------------------------------------------------------------------
** Vocoder Task Command Queue, Free Queue and Available Packets.
**-----------------------------------------------------------------------*/
#define VOC_NUM_PACKETS 10              /* # of Free Packets Allocated  */
q_type           voc_cmd_q;             /* Request Packets Queue        */
q_type           voc_free_q;            /* Available Packets Queue      */
voc_packet_type  voc_free_packet[VOC_NUM_PACKETS];



voc_client_config_type voc_client_config[VOC_MAX_ACQUIRE_TYPE];

#ifdef FEATURE_VOC_PCM_INTERFACE
void* voc_pcm_client_fn_tbl[VOC_PCM_INTERFACE_MAX] = {NULL, NULL, NULL, NULL};
#endif

/* <EJECT> */
/*===========================================================================

                 V O C O D E R   A P I   F U N C T I O N S
  
===========================================================================*/
/*===========================================================================

FUNCTION voc_clients_requested_cap(voc_capability_type)

DESCRIPTION
  Determines what all clients have requested a particular capability.
  
DEPENDENCIES
  None

RETURN VALUE
  client mask

SIDE EFFECTS
  None

===========================================================================*/
uint16 voc_clients_requested_cap(voc_capability_type cap)
{

  uint16  client_mask = 0x0000;
  voc_state_control_type *state;
  short                  i;

  state = voc_state_get_state_data();

  for (i=0; i<VOC_MAX_ACQUIRE_TYPE; i++) {
    if (
          ((state->config_mask & (1<<i)) != 0) && 
          (voc_client_config[i].config == cap)
        ) {
      client_mask = client_mask | (1 << i);    
    }
  }

  return client_mask;
}
/* <EJECT> */
/*=============================================================================

FUNCTION voc_clients_requested_cap_dir(voc_capability_type)

DESCRIPTION
  Determines the effective direction of all the clients for a given capability. 
  
DEPENDENCIES
  None

RETURN VALUE
  voc_client_cap_dir_type

SIDE EFFECTS
  None

=============================================================================*/
voc_client_cap_dir_type voc_clients_requested_cap_dir
(voc_capability_type cap)
{
  uint16 client_mask;
  voc_client_cap_dir_type ret_val = VOC_CLIENT_BOTH_DIR;

  /* enc_dir_mask : all clients that need encoder voice path */
  /* dec_dir_mask : all clients that need decoder voice path */
  /* enc_dir_mask & dec_dir_mask : all clients that need both voice paths */
  /* All MM audio apps are not checked, both dirs are preset. */
  /* (client_mask & enc_dir_mask) ^^ (client_mask & dec_dir_mask) 
      : exactly one-directional path is needed by any of the active clients.
  */
  const uint16 enc_dir_mask = (uint16)(~(1<<VOC_ACQ_VOCVS ));
  const uint16 dec_dir_mask = (uint16)(~(1<<VOC_ACQ_VOCREC));
  uint16 client_enc_dir_mask;
  uint16 client_dec_dir_mask;

  client_mask = voc_clients_requested_cap(cap);
  client_enc_dir_mask  = client_mask & enc_dir_mask;
  client_dec_dir_mask  = client_mask & dec_dir_mask;
  /*
  ** This is to ensure that a given capability is not requested for both
  ** rx and tx. It does a Logical Exclusive-OR operation of:
  ** client_enc_dir_mask
  ** and
  ** client_dec_dir_mask
  */
  if(
      ( (!client_enc_dir_mask) &&   client_dec_dir_mask  )|| 
      (   client_enc_dir_mask  && (!client_dec_dir_mask) )
    )
    {
      if(client_dec_dir_mask){ 
        ret_val = VOC_CLIENT_RX_ONLY; 
      }
      else if(client_enc_dir_mask){ 
        ret_val = VOC_CLIENT_TX_ONLY; 
      }
    }    
    return ret_val;
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_curr_config

DESCRIPTION
  Return the current vocoder configuration.  No implications about current
vocoder state which is able to be retrieved by function voc_get_status.

DEPENDENCIES
  None

RETURN VALUE
  voc_capability_type

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_capability_type voc_get_curr_config(void)
{
  return (voc_state_get_state_data()->config);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* <EJECT> */
/*===========================================================================

FUNCTION voc_acquire

DESCRIPTION
  Acquire the vocoder for use.  This takes it out of powerdown.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_acquire
(
  voc_acquire_type  key,         /* What entity wants to acquire the Vocoder */
  voc_state_cb_type cb_ptr
)
{
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->hdr.cmd        = VOC_ACQUIRE;
    packet->acquire.key    = key;
    packet->acquire.cb_ptr = cb_ptr;
    voc_cmd(packet);
  } else {
    MSG_HIGH("voc_acquire failed",0,0,0);
  }

} /* voc_acquire */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_release

DESCRIPTION
  Release the vocoder, no longer in use.  This puts it into powerdown.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_release (voc_acquire_type key)
{
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->hdr.cmd        = VOC_RELEASE;
    packet->release.key    = key;
    voc_cmd(packet);
  } else {
    MSG_HIGH("voc_release failed",0,0,0);
  }
} /* voc_release */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_config

DESCRIPTION
  This function configures the Vocoder to a specific service option.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Initialization performed successfully

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_status_type voc_config ( 
  voc_acquire_type    key,
  voc_capability_type option
)
{
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->hdr.cmd        = VOC_CONFIG;
    packet->config.key     = key;
    packet->config.option  = option;
    voc_cmd(packet);
  } else {
    MSG_HIGH("voc_config failed",0,0,0);
    return( VOC_CMD_FAILED_S );
  }

  return( VOC_DONE_S );

} /* end of voc_config */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CONFIG_STANDBY

DESCRIPTION
  This function sets the Vocoder to a specific configuration option, but
  remains in IDLE state, with appropriate firmware loaded onto the DSP.
  voc_config() must be called subsequently to make the system transition
  to vocoder state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_config_standby ( 
  voc_acquire_type    key,
  voc_capability_type option
)
{
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->hdr.cmd        = VOC_CONFIG_STANDBY;
    packet->config.key     = key;
    packet->config.option  = option;
    voc_cmd(packet);
  } else {
    MSG_HIGH("voc_config_standby failed",0,0,0);
    return( VOC_CMD_FAILED_S );
  }

  return( VOC_DONE_S );

}

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION voc_set_vr_mode

DESCRIPTION
  This function sets the Voice Recognition mode

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S       - Command accepted
  VOC_CMD_FAILED_S - Command could not be executed

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_set_vr_mode ( 
  voc_acquire_type  key,
  voc_vr_mode_type  mode
)
{
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->hdr.cmd         = VOC_SET_VR_MODE;
    packet->vr_mode.key     = key;
    packet->vr_mode.vr_mode = mode;
    voc_cmd(packet);
  } else {
    MSG_HIGH("voc_set_vr_mode failed",0,0,0);
    return( VOC_CMD_FAILED_S );
  }

  return( VOC_DONE_S );

} /* end of voc_set_vr_mode */
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOICE_RECORD
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
void voc_rec_frame (
  qdsp_cmd_rec_dir_enum_type dir,
  uint16                     frame_rate,
  uint16                     packet_length,
  uint16                     *frame
) {
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->rec_frame.packet     = vocsup_rec_get_frame_buf();
    if(packet->rec_frame.packet != NULL) {
      packet->hdr.cmd              = VOC_RECORD_FRAME;
      packet->rec_frame.frame_rate = frame_rate;
      if(dir == QDSP_CMD_REC_REVERSE) {
        packet->rec_frame.dir      = VOC_REC_REVERSE;
      } else {
        packet->rec_frame.dir      = VOC_REC_FORWARD;
      }
      memcpy(packet->rec_frame.packet, (uint8 *) frame, packet_length * 2);
      voc_cmd(packet);
    } else {
      voc_cmd_done(packet);
    }
  } else {
    MSG_HIGH("voc_record_packet failed",0,0,0);
  }
}
#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
#ifdef FEATURE_TTY
/*===========================================================================

FUNCTION voc_set_tty_mode

DESCRIPTION
  This function enables or disables TTY capabilities.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S       - Command was successful.
  VOC_CMD_FAILED_S - Command could not be executed.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_status_type voc_set_tty_mode
( 
  voc_tty_mode_type  tty_mode
)
{
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->hdr.cmd           = VOC_SET_TTY_MODE;
    packet->tty_mode.tty_mode = tty_mode;
    voc_cmd(packet);
  } else {
    MSG_HIGH("voc_set_tty_mode failed",0,0,0);
    return( VOC_CMD_FAILED_S );
  }

  return( VOC_DONE_S );

} /* end of voc_set_tty_mode */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

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
void voc_tty_activity_status (
  boolean encoder_status, /* TRUE if encoder is active, FALSE otherwise */
  boolean decoder_status  /* TRUE if decoder is active, FALSE otherwise */
)
{
  voc_packet_type  *packet;

  packet = voc_get_pkt();

  if (packet != NULL) {
    packet->hdr.cmd                   = VOC_TTY_ACTIVITY_STATUS;
    packet->tty_status.encoder_status = encoder_status;
    packet->tty_status.decoder_status = decoder_status;
    voc_cmd(packet);
  } else {
    MSG_HIGH("voc_tty_activity_status failed",0,0,0);
  }
} /* voc_tty_activity_status */
#endif /* FEATURE_TTY_ACTIVITY_STATUS */
#endif /* FEATURE_TTY */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_init

DESCRIPTION
  Signals a vocoder re-init

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_init( void )
{
  /* always return TRUE */
  return(TRUE);
} /* voc_init */

/* <EJECT> */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GET_SERVICE_OPTION

DESCRIPTION
  This function returns the current service option.

DEPENDENCIES
  None.

RETURN VALUE
  Curent service option.

SIDE EFFECTS
  None.

===========================================================================*/
voc_capability_type voc_get_service_option( void )
{
  voc_state_control_type *state;

  state = voc_state_get_state_data();

  if (state->config_mask != 0) {
    return (state->config_pending);
  } else {
#ifdef QDSP_IMAGE_VOC_EVRC_DEFINED
    return (VOC_CAP_IS127);
#elif defined(QDSP_IMAGE_VOC_DEFINED)
    return (VOC_CAP_IS733);
#elif defined(MSMAUD_VOC_4GV)
    return (VOC_CAP_4GV_NB);
#elif defined(MSMAUD_VOC_4GV_WB)
    return (VOC_CAP_4GV_WB);
#elif defined(MSMAUD_VOC_AMR_WB)
    return (VOC_CAP_AMR_WB);
#elif defined(MSMAUD_VOC_AMR)
    return (VOC_CAP_AMR);
#elif defined(MSMAUD_VOC_FR)
#error code not present
#elif defined(MSMAUD_VOC_EFR)
#error code not present
#elif defined(MSMAUD_VOC_HR)
    return (VOC_CAP_GSM_HR);
#elif defined(MSMAUD_VOC_G711)
    return (VOC_CAP_G711);
#elif defined(MSMAUD_VOC_G723)
    return (VOC_CAP_G723);
#else
#error "No Vocoder firmware present!"
#endif  /* QDSP_IMAGE_VOC_EVRC_DEFINED */
  }

} /* voc_get_service_option */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GET_STATUS

DESCRIPTION
  This function returns the current vocoder state.

DEPENDENCIES
  None.

RETURN VALUE
  Curent vocoder state.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_inform_type voc_get_status( void )
{
  
  return (voc_state_get_inform_val());

} /* voc_get_status */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CHECK_PB_CAPABILITY

DESCRIPTION
  This function checks whether the vocoder supports the desired playback type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if the service option is supported
  FALSE - if the service option is not supported

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean voc_check_pb_capability
( 
  voc_capability_type so
)
{
  boolean ret_val = FALSE;
  
  switch (so) {
#if defined(QDSP_MODULE_13K_DEC_DEFINED) || defined(QDSP_MODULE_13K_DEC_E90_DEFINED)
    case VOC_CAP_IS733:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_EVRC_DEC_DEFINED) || defined(QDSP_MODULE_EVRC_DEC_E90_DEFINED)
    case VOC_CAP_IS127:
      ret_val = TRUE;
      break;
#endif 
#if defined(QDSP_MODULE_4GV_DEC_DEFINED) 
    case VOC_CAP_4GV_NB:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_4GV_WB_DEC_DEFINED) 
    case VOC_CAP_4GV_WB:
      ret_val = TRUE;
      break;
#endif

#if defined(QDSP_MODULE_AMPS_DEFINED)       && \
    defined(FEATURE_ACP)                    && \
    (defined(QDSP_IMAGE_VOC_DEFINED)        || \
     defined(QDSP_IMAGE_VOC_EVRC_DEFINED))
#error code not present
#endif 
#if defined(QDSP_MODULE_AMR_WB_DEC_DEFINED)     && \
    defined(QDSP_IMAGE_AMR_WB_DEFINED)
    case VOC_CAP_AMR_WB:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_AMR_DEC_DEFINED)
    case VOC_CAP_AMR:
      ret_val = TRUE;
      break;
#endif 
#if defined(QDSP_MODULE_EFR_DEC_DEFINED)
    case VOC_CAP_GSM_EFR:
      ret_val = TRUE;
      break;
#endif 
#if defined(QDSP_MODULE_FR_DEC_DEFINED)
    case VOC_CAP_GSM_FR:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_HR_DEC_DEFINED)
    case VOC_CAP_GSM_HR:
      ret_val = TRUE;
      break;
#endif
     default: 
      ret_val = FALSE;
      break;
  }
  return (ret_val);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CHECK_REC_CAPABILITY

DESCRIPTION
  This function checks whether the vocoder supports the desired recording type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if the service option is supported
  FALSE - if the service option is not supported

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean voc_check_rec_capability
( 
  voc_capability_type so
)
{
  boolean ret_val = FALSE;
  
  switch (so) {
#if defined(QDSP_MODULE_13K_ENC_DEFINED) || defined(QDSP_MODULE_13K_ENC_E90_DEFINED)
    case VOC_CAP_IS733:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_EVRC_ENC_DEFINED) || defined(QDSP_MODULE_EVRC_ENC_E90_DEFINED)
    case VOC_CAP_IS127:
      ret_val = TRUE;
      break;
#endif 
#if defined(QDSP_MODULE_4GV_ENC_DEFINED)
    case VOC_CAP_4GV_NB:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_4GV_WB_ENC_DEFINED)
    case VOC_CAP_4GV_WB:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_AMR_ENC_DEFINED) 
    case VOC_CAP_AMR:
      ret_val = TRUE;
      break;
#endif 
#if defined(QDSP_MODULE_EFR_ENC_DEFINED)  
    case VOC_CAP_GSM_EFR:
      ret_val = TRUE;
      break;
#endif 
#if defined(QDSP_MODULE_FR_ENC_DEFINED)  
    case VOC_CAP_GSM_FR:
      ret_val = TRUE;
      break;
#endif
#if defined(QDSP_MODULE_HR_ENC_DEFINED)  
    case VOC_CAP_GSM_HR:
      ret_val = TRUE;
      break;
#endif
     default:
      ret_val = FALSE;
      break;
  }
  return (ret_val);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CAPABILITY_SUPPORTED

DESCRIPTION
  This function checks whether the vocoder supports the
  desired service option.

DEPENDENCIES
  The version information must be initialized by a call to
  voc_test_cmd() prior to calling this routine.

RETURN VALUE
  TRUE - if the service option is supported
  FALSE - if the service option is not supported

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_capability_supported( voc_capability_type so)
{
  boolean ret_val;

  switch (so) {
#if defined(MSMAUD_VOC_IS733)
    case VOC_CAP_IS733:
#endif /* MSMAUD_VOC_IS733 */
#if defined(MSMAUD_VOC_IS127)
    case VOC_CAP_IS127:
#if defined(MSMAUD_VOC_IS127_VOIP)
    case VOC_CAP_IS127_VOIP:
#endif /* MSMAUD_VOC_IS127_VOIP */
#endif /* MSMAUD_VOC_IS127 */
#if defined(MSMAUD_VOC_4GV)
    case VOC_CAP_4GV_NB:
#if defined(MSMAUD_VOC_4GV_VOIP)
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */
#endif  /* MSMAUD_VOC_4GV */
#if defined(MSMAUD_VOC_4GV_WB)
    case VOC_CAP_4GV_WB:
#if defined(MSMAUD_VOC_4GV_WB_VOIP)
    case VOC_CAP_4GV_WB_VOIP:
#endif /* MSMAUD_VOC_4GV_WB_VOIP */
#endif  /* MSMAUD_VOC_4GV_WB */
#if defined(FEATURE_ACP)
#error code not present
#endif  /* FEATURE_ACP */
#if defined(MSMAUD_VOC_AMR_WB)
    case VOC_CAP_AMR_WB:
#endif /* MSMAUD_VOC_AMR_WB */
#if defined(MSMAUD_VOC_AMR)
    case VOC_CAP_AMR:
#endif /* MSMAUD_VOC_AMR */
#if defined(MSMAUD_VOC_EFR)
#error code not present
#endif /* MSMAUD_VOC_EFR */
#if defined(MSMAUD_VOC_FR)
#error code not present
#endif /* MSMAUD_VOC_FR */
#if defined(MSMAUD_VOC_HR)
    case VOC_CAP_GSM_HR:
#endif /* MSMAUD_VOC_HR */
#if defined(MSMAUD_VOC_G711)
    case VOC_CAP_G711:
#endif /* MSMAUD_VOC_G711 */
#if defined(MSMAUD_VOC_G723)
    case VOC_CAP_G723:
#endif /* MSMAUD_VOC_G723 */
      ret_val = TRUE;
      break;

    default:
      ret_val = FALSE;
      break;
  }
  return (ret_val);

} /* voc_capability_supported */

/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

FUNCTION VOC_REGISTER_PCM_INPUT_CLIENT

DESCRIPTION
  Use this function to register a callback to feed PCM data on the RX input
  path to the vocoder. Set the client_func to NULL to de-register a client.

DEPENDENCIES
  The client must acquire the Vocoder before using this interface.

RETURN VALUE
  VOC_DONE_S       if successful.
  VOC_CMD_FAILED_S if the client's input parameters were incorrect.
  VOC_CMD_BUSY_S   if the vocoder ran out of command buffers to process this
                   command.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_status_type voc_register_pcm_input_client(
  voc_pcm_interface_type       interface,
  voc_pcm_client_input_fn_type client_func
)
{
  voc_status_type  status = VOC_CMD_FAILED_S;
  voc_packet_type  *packet;

  switch (interface) {
    case VOC_PCM_INTERFACE_RX_INPUT:
    case VOC_PCM_INTERFACE_TX_INPUT:
      packet = voc_get_pkt();
      if (packet != NULL) {
        packet->hdr.cmd              = VOC_REGISTER_PCM_CLIENT;
        packet->pcm_client.interface = interface;
        packet->pcm_client.pcm_attrib = NULL;     /* use default for now */
        packet->pcm_client.client_func.input_func = client_func;
        voc_cmd(packet);
        status = VOC_DONE_S;
      }
      else {
        MSG_HIGH("voc_register_pcm_input_client failed",0,0,0);
        status = VOC_CMD_BUSY_S;
      }
      break;

    case VOC_PCM_INTERFACE_RX_OUTPUT:
    case VOC_PCM_INTERFACE_TX_OUTPUT:
      MSG_HIGH("Can only register input function here", 0, 0, 0);
      break;

    default:
      MSG_ERROR("Unknown PCM client type", 0, 0, 0);
      break;
  }
  return status;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REGISTER_PCM_OUTPUT_CLIENT

DESCRIPTION
  Use this function to register a callbacks to get PCM output from the vocoder.

DEPENDENCIES
  The client must acquire the Vocoder before using this interface.

RETURN VALUE
  VOC_DONE_S       if successful.
  VOC_CMD_FAILED_S if the client's input parameters were incorrect.
  VOC_CMD_BUSY_S   if the vocoder ran out of command buffers to process this

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_status_type voc_register_pcm_output_client(
  voc_pcm_interface_type        interface,
  voc_pcm_client_output_fn_type client_func
)
{
  voc_status_type  status = VOC_CMD_FAILED_S;
  voc_packet_type  *packet;

  switch (interface) {
    case VOC_PCM_INTERFACE_RX_OUTPUT:
    case VOC_PCM_INTERFACE_TX_OUTPUT:
      packet = voc_get_pkt();
      if (packet != NULL) {
        packet->hdr.cmd              = VOC_REGISTER_PCM_CLIENT;
        packet->pcm_client.interface = interface;
        packet->pcm_client.pcm_attrib = NULL;   /* use default for now */
        packet->pcm_client.client_func.output_func = client_func;
        voc_cmd(packet);
        status = VOC_DONE_S;
      }
      else {
        MSG_HIGH("voc_register_pcm_output_client failed",0,0,0);
        status = VOC_CMD_BUSY_S;
      }
      break;

    case VOC_PCM_INTERFACE_RX_INPUT:
      MSG_HIGH("Can only register output function here", 0, 0, 0);
      break;

    default:
      MSG_ERROR("Unknown PCM client type", 0, 0, 0);
      break;
  }
  return status;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_VOC_PCM_INTERFACE */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION voc_set_equalizer_retry_timer

DESCRIPTION
  This function sets and/or clears the equalizer retry timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_set_equalizer_retry_timer
(
  boolean enable
)
{
  if (enable) {
    rex_set_timer( &voc_equalizer_retry_timer, VOC_EQ_RETRY_TIME );
  } else {
    rex_clr_timer( &voc_equalizer_retry_timer );
    rex_clr_sigs( &voc_tcb, VOC_EQ_RETRY_TIMER_SIG );
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION voc_set_qconcert_retry_timer

DESCRIPTION
  This function sets and/or clears the equalizer retry timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_set_qconcert_retry_timer
(
  boolean enable
)
{
  if (enable) {
    rex_set_timer( &voc_qconcert_retry_timer, VOC_QCONCERT_RETRY_TIME );
  } else {
    rex_clr_timer( &voc_qconcert_retry_timer );
    rex_clr_sigs( &voc_tcb, VOC_QCONCERT_RETRY_TIMER_SIG );
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#endif /* FEATURE_AUDFMT_QCONCERT */

#ifdef FEATURE_QTUNES_COMMON
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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_set_adec_flush_retry_timer
(
  boolean enable
)
{
  if (enable) {
    rex_set_timer( &voc_adec_flush_retry_timer, VOC_ADEC_FLUSH_RETRY_TIME );
  } else {
    rex_clr_timer( &voc_adec_flush_retry_timer );
    rex_clr_sigs( &voc_tcb, VOC_FLUSH_RETRY_TIMER_SIG );
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_QTUNES_COMMON */

/* <EJECT> */
#ifdef FEATURE_VOC_DTMF_DETECT
/*===========================================================================

FUNCTION VOC_DTMF_DETECTED

DESCRIPTION
  Use this function to respond to a client that is detecting DTMFs.

DEPENDENCIES
  The client must acquire the Vocoder before using this interface.

RETURN VALUE
  VOC_DONE_S       if successful.
  VOC_CMD_FAILED_S if the client's input parameters were incorrect.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_dtmf_detected(
  boolean                        rx_detected,
  voc_dtmf_detected_status_type  dtmf
)
{
  voc_status_type  status = VOC_CMD_FAILED_S;
  voc_packet_type  *packet;

  packet = voc_get_pkt();
  if (packet != NULL) {
    packet->hdr.cmd                   = VOC_DTMF_DETECTED;
    packet->dtmf_detected.rx_detected = rx_detected;
    packet->dtmf_detected.dtmf        = dtmf;
    voc_cmd(packet);
    status = VOC_DONE_S;
  }
  return(status);
}

#endif /* FEATURE_VOC_DTMF_DETECT */

/* <EJECT> */
/*===========================================================================

        I N T E R N A L   V O C O D E R   T A S K   F U N C T I O N S
  
===========================================================================*/
/*===========================================================================

FUNCTION voc_acquire_processing

DESCRIPTION
  Acquire the vocoder for use.  This takes it out of powerdown.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_acquire_processing ( voc_packet_type *pkt )
{
  voc_acquire_type       key;
  voc_state_control_type *state;

  state = voc_state_get_state_data();

  key = pkt->acquire.key;

  /* Check for a legal user key
  */
  if (key >= VOC_MAX_ACQUIRE_TYPE) {
    ERR_FATAL("Invalid key %x", key, 0, 0);
  }

  /* Save the pointer to the function to call when operating status changes */
  state->cb_table[ key ] = pkt->acquire.cb_ptr;

  /* Set the 'in-use' bit for this key  */
  state->acquire_mask |= (1 << key);
  
  /* Determine the sample slip mode.  Use auto sample slipping when in
  ** a call.  This is the only time when the HW vocoder frame reference can
  ** be guaranteed.
  */
  if (state->acquire_mask & ( ((word) 1 << VOC_ACQ_CDMA)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif
#ifdef FEATURE_MVS
                              | ((word) 1 << VOC_ACQ_MVS)
#endif /* FEATURE_MVS */
                            )) {
#ifdef MSMAUD_MSM5200_SLIP_CONTROL
#error code not present
#endif  /* MSMAUD_MSM5200_SLIP_CONTROL */
    state->slip_mode_pending = VOC_STATE_SLIP_AUTO;
  } else {
    state->slip_mode_pending = VOC_STATE_SLIP_MAN;
  }


  /* Take care of any vocoder state changes required by this acquire
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

} /* voc_acquire_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_release_processing

DESCRIPTION
  Release the vocoder, no longer in use.  This puts it into powerdown.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_release_processing (voc_packet_type *pkt)
{
  voc_acquire_type       key;
  voc_state_control_type *state;
  voc_capability_type    config;
  short                  i;

  state = voc_state_get_state_data();

  key = pkt->acquire.key;

  /* Check for a legal user key
  */
  if (key >= VOC_MAX_ACQUIRE_TYPE) {
    ERR_FATAL("Invalid key %x", key, 0, 0);
  }

  /* Clear the callback pointer for this key
  */
  state->cb_table[ key ] = (voc_state_cb_type) NULL;

  /* Clear the 'in-use' bit for this key
  */
  state->acquire_mask &= ~((uint16)(0x0001 << key));

  /* Clear the 'configured' bit for this key
  */
  state->config_mask &= ~((uint16)(0x0001 << key));

  /* Now determine the highest priority configuration
  */
  config = VOC_CAP_NONE;
  for (i=0; i<VOC_MAX_ACQUIRE_TYPE; i++) {
    if ((state->config_mask & (1<<i)) != 0) {
      switch(config) {
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
        default:
          config = voc_client_config[i].config;
          state->config_standby = voc_client_config[i].standby_flag;
          break;
      }
    }
  }
  state->config_pending = config;
  
  /* Determine the sample slip mode.  Use auto sample slipping when in
  ** a call.  This is the only time when the HW vocoder frame reference can
  ** be guaranteed.
  */
  if (state->acquire_mask & ( ((word) 1 << VOC_ACQ_CDMA)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif
#ifdef FEATURE_MVS
                              | ((word) 1 << VOC_ACQ_MVS)
#endif
                            )) {
#ifdef MSMAUD_MSM5200_SLIP_CONTROL
#error code not present
#endif  /* MSMAUD_MSM5200_SLIP_CONTROL */
    state->slip_mode_pending = VOC_STATE_SLIP_AUTO;
  } else {
    state->slip_mode_pending = VOC_STATE_SLIP_MAN;
  }

  /* Take care of any vocoder state changes required by this release
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

} /* voc_release_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_config_processing

DESCRIPTION
  This function configures the Vocoder to a specific service option.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_config_processing (voc_packet_type *pkt)
{
  voc_acquire_type       key;
  voc_state_control_type *state;
  voc_capability_type    config;
  boolean                standby_flag = FALSE;
  short                  i;

  state = voc_state_get_state_data();

  key = pkt->config.key;
  
  /* Check for a legal user key
  */
  if (key >= VOC_MAX_ACQUIRE_TYPE) {
    ERR_FATAL("Invalid key %x", key, 0, 0);
  }

  if(pkt->config.option != VOC_CAP_NONE) {
    /* Set the 'configured' bit for this key
    */
    state->config_mask |= (1 << key);

    /* Save the configuration for this key
    */
    voc_client_config[key].config = pkt->config.option;
    voc_client_config[key].standby_flag = 
                                   (pkt->hdr.cmd == VOC_CONFIG_STANDBY ? 1 : 0);
  } else {
    /* Clear the 'configured' bit for this key
    */
    state->config_mask &= ~((uint16)(0x0001 << key));
  }
  
  /* Now determine the highest priority configuration
  */
  config = VOC_CAP_NONE;
  for (i=0; i<VOC_MAX_ACQUIRE_TYPE; i++) {
    if ((state->config_mask & (1<<i)) != 0) {
      switch (config) {
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
        default:
          config = voc_client_config[i].config;
          standby_flag = voc_client_config[i].standby_flag;
          break;
      }
    }
  }
  state->config_pending = config;
  state->config_standby = standby_flag;

  if(config == pkt->config.option) {
    /* Set config flag if new config is highest config */
    state->config_flag = TRUE;
  }

  /* Take care of any vocoder state changes required by this config
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  
} /* voc_config_processing */

/*===========================================================================

FUNCTION voc_get_desired_config

DESCRIPTION
  Returns the configuration selected by the client specified by the acquire_key

DEPENDENCIES
  Only valid if both the acquire_key and config are set by the selected client.

RETURN VALUE
  capability

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_capability_type voc_get_desired_config(voc_acquire_type acq_key)
{
  if (acq_key < VOC_MAX_ACQUIRE_TYPE) {
    return voc_client_config[acq_key].config;
  }
  return VOC_CAP_NONE;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION voc_set_vr_mode_processing

DESCRIPTION
  This function sets the Voice Recognition mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_vr_mode_processing (voc_packet_type *pkt)
{
  voc_acquire_type       key;
  voc_vr_mode_type       vr_mode;
  voc_state_control_type *state;
  
  state = voc_state_get_state_data();

  key = pkt->vr_mode.key;
  
  /* Check for a legal user key
  */
  if (key >= VOC_MAX_ACQUIRE_TYPE) {
    ERR_FATAL("Invalid key %x", key, 0, 0);
  }
  
  vr_mode = pkt->vr_mode.vr_mode;

  /* Check for a legal vr mode
  */
  if (vr_mode >= VOC_VR_MODE_MAX) {
    ERR_FATAL("Invalid VR mode %x", vr_mode, 0, 0);
  }
  
  /* Save the new vr mode
  */
  state->vr_mode = vr_mode;

  /* Take care of any vocoder state changes required by this config
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  
} /* voc_set_vr_mode_processing */
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_TTY
/*===========================================================================

FUNCTION voc_set_tty_mode_processing

DESCRIPTION
  This function enables or disables TTY capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_tty_mode_processing (voc_packet_type *pkt)
{
  voc_tty_mode_type       tty_mode;
  voc_state_control_type* state;
  
  state = voc_state_get_state_data();

  tty_mode = pkt->tty_mode.tty_mode;

  /* Check for a legal tty mode
  */
  if (tty_mode >= VOC_TTY_MODE_MAX) {
    ERR_FATAL("Invalid TTY mode %x", tty_mode, 0, 0);
  }
  
  /* Save the new tty mode
  */
  state->tty_mode = tty_mode;

  /* Set the tty mode now
  */
  voc_tty_cmd();

  /* Take care of any vocoder state changes required by this config
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  
} /* voc_set_tty_mode_processing */
#endif /* FEATURE_TTY */

/* <EJECT> */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_set_codec_retry_timer( boolean set_timer ) {
  if(set_timer) {
    (void) rex_set_timer( &voc_codec_retry_timer, VOC_CODEC_RETRY_TIME );
  } else {
    (void) rex_clr_timer( &voc_codec_retry_timer );
    (void) rex_clr_sigs( &voc_tcb, VOC_CODEC_RETRY_TIMER_SIG );
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* <EJECT> */
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_set_bt_config_retry_timer( voc_bt_config_retry_timer_type timer ) 
{
  uint16 timer_value = 0;

  if (timer < VOC_BT_RETRY_TIMER_MAX )
  {
    timer_value = voc_bt_config_retry_timer_value[timer];
    if(timer_value) {
      (void) rex_set_timer( &voc_bt_config_retry_timer, timer_value );
    } else {
      (void) rex_clr_timer( &voc_bt_config_retry_timer );
      (void) rex_clr_sigs( &voc_tcb, VOC_BT_CONFIG_RETRY_TIMER_SIG );
    }
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif
/* <EJECT> */
#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_set_clk_ctrl_timer(
  boolean                set_timer,
  voc_clk_ctrl_enum_type mask
) {
  if(set_timer) {
    voc_clk_ctrl_mask_values |= mask;
  } else {
    voc_clk_ctrl_mask_values &= ~mask;
  }

  if(voc_clk_ctrl_mask_values != 0) {
    (void) rex_set_timer( &voc_clk_ctrl_timer, VOC_CLK_CTRL_TIME );
  } else {
    (void) rex_clr_timer( &voc_clk_ctrl_timer );
    (void) rex_clr_sigs( &voc_tcb, VOC_CLK_CTRL_TIMER_SIG );
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_EXTERNAL_SDAC || MSMAUD_CODEC_CLOCK_CTRL */

/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE

/*===========================================================================

FUNCTION voc_pcm_input

DESCRIPTION
  Input type callback from vocoder PCM interface.

DEPENDENCIES
  None

RETURN VALUE
  data status, pcm_data

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_data_status_type voc_pcm_input(
  uint16 *pcm_data,
  const qdsp_cmd_pcm_interface_attrib_type *pcm_attrib
)
{
  qdsp_cmd_pcm_interface_type interface;
  voc_pcm_client_input_fn_type input_fn = NULL;


  if (pcm_attrib==NULL)
  {
    MSG_ERROR("Unspecified PCM attribute.", 0, 0, 0);
    return QDSP_PCM_DATA_STATUS_UNAVAILABLE;
  }

  interface = pcm_attrib->intf_dsc.chan_id;

  switch (interface)
  {
    case QDSP_PCM_INTERFACE_RX_INPUT:
    case QDSP_PCM_INTERFACE_TX_INPUT:
      input_fn 
      = (voc_pcm_client_input_fn_type)(voc_pcm_client_fn_tbl[interface]);

      if (input_fn!=NULL)
      {
        return input_fn(pcm_data);
      }

      break;

    default:
      MSG_ERROR("Unmatched PCM interface type %d", interface, 0, 0);
      break;
  }

 
  return QDSP_PCM_DATA_STATUS_UNAVAILABLE;
}

  
/* <EJECT> */
/*===========================================================================

FUNCTION voc_pcm_output

DESCRIPTION
  Output type callback from vocoder PCM interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_pcm_output(
  const uint16  *pcm_data,
  const qdsp_cmd_pcm_interface_attrib_type *pcm_attrib
)
{
  qdsp_cmd_pcm_interface_type interface;
  voc_pcm_client_output_fn_type output_fn = NULL;


  if (pcm_attrib==NULL)
  {
    MSG_ERROR("Unspecified PCM attribute.", 0, 0, 0);
    return;
  }

  interface = pcm_attrib->intf_dsc.chan_id;

  switch (interface)
  {
    case QDSP_PCM_INTERFACE_RX_OUTPUT:
    case QDSP_PCM_INTERFACE_TX_OUTPUT:
      output_fn 
      = (voc_pcm_client_output_fn_type)(voc_pcm_client_fn_tbl[interface]);

      if (output_fn != NULL)
      {
        output_fn(pcm_data);
      }

      break;

    default:
      MSG_ERROR("Unmatched PCM interface type %d", interface, 0, 0);
      return;
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REGISTER_PCM_CLIENT_PROCESSING

DESCRIPTION
  Processes PCM client's request to register/de-register.
  
DEPENDENCIES
  Client must acquire the Vocoder before using this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_register_pcm_client_processing( voc_packet_type *pkt )
{
  uint32                 *pending_mode;
  voc_state_control_type *state;
  voc_pcm_interface_type interface = pkt->pcm_client.interface;
  qdsp_cmd_pcm_client_input_fn_type input_fn = NULL;
  qdsp_cmd_pcm_client_output_fn_type output_fn = NULL;
  qdsp_cmd_pcm_interface_attrib_type *pcm_attrib
    = (qdsp_cmd_pcm_interface_attrib_type*)(pkt->pcm_client.pcm_attrib);

  
  if (pkt->pcm_client.client_func.func != NULL) 
  {
    input_fn = voc_pcm_input;
    output_fn = voc_pcm_output;
  }

  switch (interface) {
    case VOC_PCM_INTERFACE_RX_INPUT:
      qdsp_cmd_register_pcm_input_client(QDSP_PCM_INTERFACE_RX_INPUT,
                                         pcm_attrib,
                                         input_fn);
      break;

    case VOC_PCM_INTERFACE_TX_INPUT:
      qdsp_cmd_register_pcm_input_client(QDSP_PCM_INTERFACE_TX_INPUT,
                                         pcm_attrib,
                                         input_fn);
      break;

    case VOC_PCM_INTERFACE_RX_OUTPUT:
      qdsp_cmd_register_pcm_output_client(QDSP_PCM_INTERFACE_RX_OUTPUT,
                                          pcm_attrib,
                                          output_fn);
      break;

    case VOC_PCM_INTERFACE_TX_OUTPUT:
      qdsp_cmd_register_pcm_output_client(QDSP_PCM_INTERFACE_TX_OUTPUT,
                                          pcm_attrib,
                                          output_fn);
      break;

    default:
      ERR_FATAL("Unknown PCM interface type", 0, 0, 0);
  }

  voc_pcm_client_fn_tbl[interface] = pkt->pcm_client.client_func.func;


  state = voc_state_get_state_data();

  pending_mode = &state->pcm_int_mode_pending;

  if (pkt->pcm_client.client_func.func == NULL) {
    *pending_mode &= ~(1 << interface);
  }
  else {
    *pending_mode |= (1 << interface);
  }

  if (*pending_mode != state->pcm_int_mode) {
    (void) rex_set_sigs(&voc_tcb, VOC_CHECK_STATE_SIG);
  }
}
#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_VOC_DTMF_DETECT
/*===========================================================================

FUNCTION VOC_DTMF_DETECTED_PROCESSING

DESCRIPTION
  
  
DEPENDENCIES
  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_dtmf_detected_processing( voc_packet_type *pkt )
{
  if(pkt->dtmf_detected.rx_detected) {
    /* Rx DTMF detected.  Callback if necessary. */
    voc_rx_detect_cb(pkt->dtmf_detected.dtmf);
  } else {
    voc_tx_detect_cb(pkt->dtmf_detected.dtmf);
  }
}
#endif /* FEATURE_VOC_DTMF_DETECT */

/* <EJECT> */
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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_cmd ( voc_packet_type *voc_cmd_ptr )
{
  (void) q_link (voc_cmd_ptr, &voc_cmd_ptr->hdr.link);

  q_put (&voc_cmd_q, &voc_cmd_ptr->hdr.link);       /* Put on command queue  */
  (void) rex_set_sigs (&voc_tcb, VOC_CMD_Q_SIG);    /* Signal a queue event  */

} /* end voc_cmd */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_packet_type *voc_get_pkt (void)
{
  voc_packet_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (voc_packet_type *) q_get (&voc_free_q);

  if (packet == NULL) {
    ERR ("Ran Out of VOC Packets!", 0, 0, 0);
  } else {
    /* Fill the entire VOC packet with 0's to get
    ** default behavior on un-initialized fields.
    */
    (void) memset (packet, 0, sizeof (voc_packet_type));
    packet->hdr.cmd = VOC_INVALID;
  }

  return (packet);

} /* voc_get_pkt */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

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
  None.
  
===========================================================================*/
void voc_cmd_done
(
  voc_packet_type *voc_cmd_ptr
)
{
  q_put (&voc_free_q, &voc_cmd_ptr->hdr.link);

} /* voc_cmd_done */

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
void voc_process_cmd( void )
{
  voc_packet_type *cmd_ptr;
  
  /* Clear the 'queued commands' signal prior to emptying the queue.
  ** This is done in order to avoid race conditions where an item is
  ** added to the queue just as we are done servicing the queue.
  */
  (void) rex_clr_sigs( &voc_tcb, VOC_CMD_Q_SIG );

  /* While there are commands to process.
  */
  while ( (cmd_ptr = (voc_packet_type *) q_get(&voc_cmd_q)) != NULL ) {

    /* Process each command type
    */
    switch (cmd_ptr->hdr.cmd ) {

      /* Acquire the vocoder
      */
      case VOC_ACQUIRE:
        voc_acquire_processing (cmd_ptr);
        break;

      /* Configure the vocoder
      */
      case VOC_CONFIG:
        /* Fall through */

      /* Configure the vocoder for standby mode
      */
      case VOC_CONFIG_STANDBY:
        voc_config_processing (cmd_ptr);
        break;

#ifdef FEATURE_PHONE_VR
      /* Set the voice recognition mode
      */
      case VOC_SET_VR_MODE:
        voc_set_vr_mode_processing (cmd_ptr);
        break;
#endif

#ifdef FEATURE_VOICE_RECORD
      /* Recorded frame data command
      */
      case VOC_RECORD_FRAME:
        voc_rec_frame_processing (cmd_ptr->rec_frame.dir,
                                  cmd_ptr->rec_frame.frame_rate,
                                  cmd_ptr->rec_frame.packet);
        break;
#endif

#ifdef FEATURE_TTY
      /* Set the tty mode
      */
      case VOC_SET_TTY_MODE:
        voc_set_tty_mode_processing (cmd_ptr);
        break;

#ifdef FEATURE_TTY_ACTIVITY_STATUS
      case VOC_TTY_ACTIVITY_STATUS:
        voc_tty_status_func(cmd_ptr->tty_status.encoder_status,
                            cmd_ptr->tty_status.decoder_status);
        break;
#endif
#endif /* FEATURE_TTY */
      
      /* Release the vocoder
      */
      case VOC_RELEASE:
        voc_release_processing (cmd_ptr);
        break;

#ifdef FEATURE_VOC_PCM_INTERFACE
      /* Register a PCM client
      */
      case VOC_REGISTER_PCM_CLIENT:
        voc_register_pcm_client_processing(cmd_ptr);
        break;
#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_VOC_DTMF_DETECT
      case VOC_DTMF_DETECTED:
        voc_dtmf_detected_processing(cmd_ptr);
        break;
#endif /* FEATURE_VOC_DTMF_DETECT */

      /* Illegal command */
      case VOC_INVALID:
      default:
        ERR_FATAL("Illegal command %x", cmd_ptr->hdr.cmd, 0, 0);
        /* Does not return */
    }

    voc_cmd_done( cmd_ptr );

  } /* while */
} /* voc_process_cmd */

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
void voc_task_init (void)
{
  int i;

  /* Initialize state controller
  */
  voc_state_control_init();

#ifdef FEATURE_EXTERNAL_SDAC
  /* Initialize the External Stereo DAC */

#ifndef MSMAUD_ON_CHIP_SDAC_SUPPORT
  BIO_TRISTATE(ENC_GPIO_0,0x000f,0x0008);
#endif /* !MSMAUD_ON_CHIP_SDAC_SUPPORT */
  
  (void) sdac_init();
#endif

#ifdef FEATURE_EXTERNAL_SADC
  (void) sadc_init();
#endif /* FEATURE_EXTERNAL_SADC */

  /* Initialize CODEC selection - On chip Codec Channel 0
  */
  voc_data_set_codec(VOC_CODEC_DEFAULT);
  voc_state_set_codec_cal(VOC_CODEC_DEFAULT);
  
  /* Initialize timers
  */
  rex_def_timer( &voc_rpt_timer, &voc_tcb, VOC_RPT_TIMER_SIG );
  rex_def_timer( &voc_codec_retry_timer, &voc_tcb, VOC_CODEC_RETRY_TIMER_SIG );
#ifdef FEATURE_AUDFMT_QCONCERT
  rex_def_timer(&voc_qconcert_retry_timer, &voc_tcb, VOC_QCONCERT_RETRY_TIMER_SIG);
#endif /* FEATURE_AUDFMT_QCONCERT */                   
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
  rex_def_timer( &voc_bt_config_retry_timer, &voc_tcb, 
                 VOC_BT_CONFIG_RETRY_TIMER_SIG );
#endif

#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
  rex_def_timer( &voc_clk_ctrl_timer, &voc_tcb, VOC_CLK_CTRL_TIMER_SIG );
#endif

#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
  rex_def_timer(&voc_pmic_delay_timer, &voc_tcb, VOC_PMIC_SPKR_DELAY_SIG);
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */

#ifdef FEATURE_QTUNES_COMMON
  rex_def_timer(&voc_adec_flush_retry_timer, &voc_tcb,
                VOC_FLUSH_RETRY_TIMER_SIG);
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_AUDFMT_EQUALIZER
  rex_def_timer(&voc_equalizer_retry_timer, &voc_tcb, VOC_EQ_RETRY_TIMER_SIG);
  voc_equalizer_init_crit_sect();
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_VOC_DTMF_DETECT
  rex_def_timer(&voc_dtmf_detect_delay_timer, &voc_tcb, 
                                                     VOC_DTMF_DETECT_DELAY_SIG);
#endif /* FEATURE_VOC_DTMF_DETECT */

#ifdef FEATURE_GASYNTH_INTERFACE
  rex_def_timer(&voc_adec_cmd_delay_timer, &voc_tcb, VOC_ADEC_CMD_RETRY_SIG);
#endif /* FEATURE_GASYNTH_INTERFACE */
 
  /* Initialize the command queues.
  */
  (void) q_init (&voc_cmd_q);              /* Initialize the command queue  */
  (void) q_init (&voc_free_q);             /* Initialize the free queue     */

  /* Add items to voc_free_q.
  */
  for (i = 0; i < VOC_NUM_PACKETS; i++) 
  {
    q_link_type *link_item;
    link_item = q_link( &voc_free_packet[i], &voc_free_packet[i].hdr.link );
    q_put( &voc_free_q, link_item );
  }

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#ifdef FEATURE_MVS
  mvs_init();
#endif /* FEATURE_MVS */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */


  /* Initialize VP3 System
   */
  vocvp3_init(voc_data_get_buf_id_to_buf_size_table(), voc_data_get_buf_id_to_default_data_table());

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

  MSG_LOW("VOC task initialized",0,0,0);

} /* voc_task_init */

#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
/* <EJECT> */
/*===========================================================================

FUNCTION voc_pmic_speaker_delay

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void voc_pmic_speaker_delay(
  rex_timer_cnt_type           time,
  voc_pmic_delay_cb_func_type  cb_func
)
{
  voc_pmic_delay_cb = cb_func;
  rex_set_timer (&voc_pmic_delay_timer, time);
}
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */

#ifdef FEATURE_VOC_NV_CAL
/* <EJECT> */
/*===========================================================================
FUNCTION VOC_NV_WAIT

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
void voc_nv_wait
(
  void
)
{
  rex_sigs_type  sigs;

  /*lint -e716*/
  while( 1 )
  {                     /* until signal back from NV */
    sigs = rex_wait( VOC_NV_SIG | VOC_RPT_TIMER_SIG );

    if( (sigs & VOC_RPT_TIMER_SIG) != 0 )
    {
      dog_report(DOG_VOC_RPT);
      (void) rex_clr_sigs( &voc_tcb, VOC_RPT_TIMER_SIG );
      (void) rex_set_timer( &voc_rpt_timer, DOG_VOC_RPT_TIME );
    }

    if( (sigs & VOC_NV_SIG) != 0 )
    {
      (void)rex_clr_sigs( &voc_tcb, VOC_NV_SIG );
      break;
    }
  }
  /*lint +e716*/
}
#endif /* FEATURE_VOC_NV_CAL */

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
void voc_set_adec_cmd_retry_timer (
  boolean set_timer
) {
  if((set_timer == TRUE) && 
     (rex_get_timer(&voc_adec_cmd_delay_timer) == 0)) {
    (void) rex_set_timer(&voc_adec_cmd_delay_timer, 12);
  } else if (set_timer == FALSE) {
    (void) rex_clr_timer(&voc_adec_cmd_delay_timer);
    (void) rex_clr_sigs(&voc_tcb, VOC_ADEC_CMD_RETRY_SIG);
  }
}
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_task

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  Does not return

SIDE EFFECTS

===========================================================================*/
void voc_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
#ifdef FEATURE_VOC_NV_CAL
  static boolean  cal_init = FALSE;
#endif /* FEATURE_VOC_NV_CAL */
  rex_sigs_type   sigs;

  /* Initialize VOC Task
  */
  voc_task_init();

  /* Wait for start signal from Main Control task
  */
#ifdef FEATURE_MULTIMODE_ARCH
  task_start( VOC_RPT_TIMER_SIG, DOG_VOC_RPT, &voc_rpt_timer );
#else
  mc_task_start( VOC_RPT_TIMER_SIG, DOG_VOC_RPT, &voc_rpt_timer );
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear start signal
  */
  (void) rex_clr_sigs( &voc_tcb, TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs( &voc_tcb, VOC_RPT_TIMER_SIG );

  /* Set the "check state" signal to get vocoder state control started.
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  
  /* Infinite loop -- process events forever
  */

  /* Restore VP3 data from EFS and update PMEM during power up.
   */
   vocvp3_read_from_efs();


  for (;;) {

     /* Wait for something to happen
      */
     sigs = rex_wait( VOC_CHECK_STATE_SIG          | /* Do state control      */
#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
                      VOC_PMIC_SPKR_DELAY_SIG      | /* PMIC delay signal     */
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */
#ifdef FEATURE_VOC_DTMF_DETECT
                      VOC_DTMF_DETECT_DELAY_SIG    | /* DTMF detect signal    */
#endif /* FEATURE_VOC_DTMF_DETECT */
#ifdef FEATURE_MVS
                      VOC_MVS_CMD_Q_SIG            | /* Command queue for MVS */
#endif /* FEATURE_MVS */
                      VOC_CMD_Q_SIG                | /* Command Queue Signal  */
                      VOC_RPT_TIMER_SIG            | /* Watchdog report timer */
                      VOC_CODEC_RETRY_TIMER_SIG    | /* Command retry timer   */
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
                      VOC_BT_CONFIG_RETRY_TIMER_SIG |  /* Command retry timer */
#endif
#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
                      VOC_CLK_CTRL_TIMER_SIG       | /* Clk control timer     */
#endif
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_QTUNES_COMMON
                      VOC_FLUSH_RETRY_TIMER_SIG    | /* ADEC flush retry timer*/
#endif 
#ifdef FEATURE_AUDFMT_EQUALIZER
                      VOC_EQ_RETRY_TIMER_SIG       | /* Equalizer retry timer */
#endif 
#ifdef FEATURE_TTY_UMTS
#error code not present
#endif
#ifdef FEATURE_GASYNTH_INTERFACE
                      VOC_ADEC_CMD_RETRY_SIG       | /* ADEC cmd retry timer  */
#endif /* FEATURE_GASYNTH_INTERFACE */
#ifdef FEATURE_AUDFMT_QCONCERT
                      VOC_QCONCERT_RETRY_TIMER_SIG | /* QConcert retry timer  */
#endif /* FEATURE_AUDFMT_QCONCERT */
                      TASK_STOP_SIG                | /* Task stop from MC     */
                      TASK_OFFLINE_SIG );            /* Task offline from MC  */

#ifdef FEATURE_VOC_NV_CAL
     /* Initialize voc NV cache first time through.
     */
     if ( !cal_init ) {
        voc_cal_read_nv_cal();
        cal_init = TRUE;
     }
#endif /* FEATURE_VOC_NV_CAL */
                                                    
     /* If watchdog report timer expired, report and restart the timer
      */
     if ( (sigs & VOC_RPT_TIMER_SIG) != 0 ) {
        dog_report(DOG_VOC_RPT);
        (void) rex_set_timer( &voc_rpt_timer, DOG_VOC_RPT_TIME );
     }

#if defined(FEATURE_PMIC_SPEAKER_ON_OFF_DELAY)||defined(FEATURE_KAMP_ON_OFF_DELAY)
     /* PMIC cooldown timeout, restore PMIC setting if changed
     */
     if ( (sigs & VOC_PMIC_SPKR_DELAY_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_PMIC_SPKR_DELAY_SIG );
       if(voc_pmic_delay_cb != NULL) {
         voc_pmic_delay_cb();
         voc_pmic_delay_cb = NULL;
       }
     }
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */

#ifdef FEATURE_VOC_DTMF_DETECT
    /* Check to see if another DTMF was detected
    */
    if ( (sigs & VOC_DTMF_DETECT_DELAY_SIG) != 0 ) {
      (void) rex_clr_sigs( &voc_tcb, VOC_DTMF_DETECT_DELAY_SIG );
      voc_dtmf_detect_delay_cb();
    }
#endif /* FEATURE_VOC_DTMF_DETECT */

#ifdef FEATURE_GASYNTH_INTERFACE
     if ( (sigs & VOC_ADEC_CMD_RETRY_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_ADEC_CMD_RETRY_SIG );
       voc_qsynth_adec_cmd_retry();
     }
#endif /* FEATURE_GASYNTH_INTERFACE */

#ifdef FEATURE_MVS
    /* Process the command Q
    */
    if ( (sigs & VOC_MVS_CMD_Q_SIG) != 0 ) {
      (void) rex_clr_sigs( &voc_tcb, VOC_MVS_CMD_Q_SIG );
      mvs_process_cmd();
    }

#endif /* FEATURE_MVS */
     
#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */
     
#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
     /* If clock control disable timer expired, expire clocks or restart the
      * timer
      */
     if ( (sigs & VOC_CLK_CTRL_TIMER_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_CLK_CTRL_TIMER_SIG );
       if(!voc_check_codec_config(voc_clk_ctrl_mask_values)) {
         (void) rex_set_timer( &voc_clk_ctrl_timer, VOC_CLK_CTRL_TIME );
       } else {
         voc_clk_ctrl_mask_values = 0;
       }
     }
#endif /* FEATURE_EXTERNAL_SDAC || MSMAUD_CODEC_CLOCK_CTRL */
     
     /* If retry timer has expired, try to configure the code.
     */
     if ( (sigs & VOC_CODEC_RETRY_TIMER_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_CODEC_RETRY_TIMER_SIG );
       voc_retry_codec_select();
     }

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
     if ( (sigs & VOC_BT_CONFIG_RETRY_TIMER_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_BT_CONFIG_RETRY_TIMER_SIG );
       voc_retry_bt_config();
     }
#endif

     /* Process the command Q
      */
     if ( (sigs & VOC_CMD_Q_SIG) != 0 ) {
       voc_process_cmd();
     }

#ifdef FEATURE_ACP
#error code not present
#endif
     
     /* Do Vocoder State Control
      */
     if ( (sigs & VOC_CHECK_STATE_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
       voc_state_control();
     }

#ifdef FEATURE_QTUNES_COMMON
     /* Retry adec flush command
     */
     if ( (sigs & VOC_FLUSH_RETRY_TIMER_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_FLUSH_RETRY_TIMER_SIG );
       voc_adec_flush(0xFFFF);
     }
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_AUDFMT_EQUALIZER
     /* Setup Equalizer 
     */
     if ( (sigs & VOC_EQ_RETRY_TIMER_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_EQ_RETRY_TIMER_SIG );
       voc_update_equalizer();
     }
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_QCONCERT
     /* Setup Qconcert 
     */
     if ( (sigs & VOC_QCONCERT_RETRY_TIMER_SIG) != 0 ) {
       (void) rex_clr_sigs( &voc_tcb, VOC_QCONCERT_RETRY_TIMER_SIG );
       voc_update_qconcert_config();
     }
#endif /* FEATURE_AUDFMT_QCONCERT */

     /* Handle task offline signal from MC 
      */
     if ( (sigs & TASK_OFFLINE_SIG) != 0 ) {
#ifdef FEATURE_MULTIMODE_ARCH
        task_offline();
#else
        (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /* FEATURE_MULTIMODE_ARCH */

        (void) rex_clr_sigs( &voc_tcb, TASK_OFFLINE_SIG );
     }

     /* Handle task stop signal from MC 
      */
     if ( (sigs & TASK_STOP_SIG) != 0 ) {


       /* Store VP3 data in EFS when powering down.
        */
        vocvp3_write_to_efs();


#ifdef FEATURE_MULTIMODE_ARCH
        task_stop();
#else
        (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /* FEATURE_MULTIMODE_ARCH */
        (void) rex_clr_sigs( &voc_tcb, TASK_STOP_SIG );
     }
  }

} /* voc_task */

