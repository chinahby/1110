
/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  This file defines function interfaces and corresponding data
structures for invoking multi-mode voice services.

REFERENCES
  None.

  Copyright(c) 2002 - 2009 by QUALCOMM, Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvs.c#7 $ $DateTime: 2009/10/14 07:34:24 $ $Author: c_ypaida $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/09   yp      CMI changes for SCMM 
10/08/09   rm      Deregistering mvs isrcb chain in IDLE/RESET state
07/13/09   rm      Adding support for 4GV_WB_VoIP.
06/05/09   sud     Added debug message and updated copyright.
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
08/12/08   sj      Fixed klokwork errors
09/28/07   ymc     Removed temporary workaround of channel release.
                   This should be taken care of properly in RRC and NAS now.
08/28/07    sr	   fixed compilation warning
08/20/07    sr     Edited mvs_rx_processing; packet_len was overwritten
03/21/07   ymc     Removed pkt_len in evrc_dl to eliminate compiler warnings.
03/15/07   kan     Added 4GV-WB Code
02/26/07   kan     Added AMR-WB changes
12/02/06   suk     Added support for distinction between EVRC only and
                   EVRC VoIP and corrected the invocation of 4GV time
                   warping function.
10/16/06    ay     Reverted MVS_CLIENT_TEST back to using VOC_ACQ_TEST.
08/29/06    az     Added support for incall mp3 for 65nm platforms.
07/12/06    hz     Added support for GSM/AMR incall mp3 feature.
xx/xx/xx    sm     Added SILENCE generation.
03/18/06    ay     Cleaned up lint errors.
02/03/06    sm     Added 4GV VOIP support
01/16/06    sm     Added FEATURE_QDJ_TIMEWARP
07/28/05    sm     G723 fixes.
07/25/05   ayu     Cleaned up lint errors.
07/05/05    sm     Added G.723.1 support.
04/29/05   ymc     Moved GSM L1 events to MVS.
04/28/05   ymc     Added MVS_CLIENT_VOIP support.
           ymc     Added 4GV vocoder support.
           ymc     Put back AMR mode check in mvs_release temporarily.
                   Need to check AMR mode because mvs_release will always be
                   called before mvs_acquire in NAS.  RRC channels will be
                   set up before mvs_acquire is invoked.
                   Channels release should be moved to RRC in the long run
                   for eliminating occurrence of race condition.
           ymc     Cleaned up lint errors.
02/20/05    sm     Added G.711 vocoder support.
           ymc     Updated Copyright.
           ymc     Updated mvs_to_voc_client[].
           ymc     Fixed support of voice call and PCM packet exchange
                   simultaneously.
01/28/05   ymc     Added support for MVS_MODE_LINEAR_PCM.
11/17/04   ymc     Reduced MVS frame buffer size to 1.
                   Retrieved MVS downlink frames if available before first
                   decoder callback.
                   Removed support for MSMAUD_VOC_WVRC and MSMAUD_VOC_SMV.
11/09/04    sm     Removed ran_seed(), ts.h.  Fixed unaligned memory access.
09/17/04   ymc     Added IS733 and IS127 support.
           ymc     Added support for vocoder task context packet exchange.
09/01/04    sm     Added MSG_MED for uplink frame transfer.
08/18/04   ymc     Added support for MVS_CMD_STANDBY command.
           ymc     Generate random vocoder packets in case of EFR/FR/HR
                   downlink missing frames.
           ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
05/12/04   ymc     Fixed the problem that the most recent vocoder packet
                   would be overwritten as BFI or NO_DATA packets when
                   packets are passed in too fast in the downlink.
            sm     Use calculated frame size for FR/EFR uplink.
01/27/04   ymc     Updated Copyright.
            sm     Removed early exit from mvs_enable_processing() as vocoder
                   will always callback mvs after voc_config().
           ymc     Fixed packet length calculations in AMR UL packet exchange.
                   packet length should be word alligned due to different
                   endian format from ADSP.
           ymc     Added support for MVS_CLIENT_QVP_TEST.
           ymc     Undo the fix of stubbing BFI EFR/FR vocoder packets with
                   all zeros.  BFI packets are now replaced by previous packet
                   instead (in qdspcmd.c).
08/26/03   ymc     De-register all DSM watermarks when mvs_release is invoked.
07/21/03   ymc     Activated MVS DSM buffer management before voc_amr_register.
07/17/03   ymc     Stub BFI vocoder packets with all zeros for fixing
                   screeching noise in GSM FR.
06/29/03   ymc     Added AMR loopback support for non-WCDMA clients.
           ymc     Added support for MVS_CLIENT_QVP.
04/24/03   ymc     Added support of MVS_STATE_CHANGING.
           ymc     Send mode not available event if MVS is pre-empted by other
                   vocoder clients.
           ymc     Fixed problems with consecutive mvs_acquire calls.
           ymc     Break callback chain in mvs_release.
04/17/03    sm     Moved voc->mvs, mvs->voc tables to mvsamr.c.
03/03/03   ymc     Cleaned up all lint errors and warnings.
02/14/03   ymc     Removed next amr mode in AMR uplink packet exchange.
           ymc     Updated mvssup_amr_set_amr_mode interface.
           ymc     Added MVS_AMR_SPEECH_DEGRADED and MVS_AMR_ONSET support.
01/24/03   ymc     Updated copyright.
01/10/03    sm     mvs_acquire() allows same client to acquire, provided the
                   cb_func doesn't change.
12/12/02   ymc     Added GSM half rate support.
                   Fixed GSM FR and EFR loopback functionalities.
10/17/02   ymc     Set mvs_active_client to MVS_CLIENT_NONE in mvs_release.
           ymc     Acquire and release different vocoder acquire type depend
                   on which MVS client.
10/03/02   ymc     Added voice blanking if packet exchange is out of sync.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "ran.h"

#ifdef FEATURE_MVS

#include "msg.h"
#include "err.h"
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#ifdef FEATURE_MVS_GEN_SILENCE
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif
        /* Clock rec/dereg prototypes */
#endif /* FEATURE_MVS_GEN_SILENCE */

#include "mvs.h"
#include "mvsi.h"
#include "mvssup.h"
#include "mvsdsm.h"

#include "task.h"
#include "memory.h"

#include "voc.h"


/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/*---------------------------------------------------------------------------
** MVS Command Queue, Free Queue and Available Packets
**---------------------------------------------------------------------------
*/
LOCAL q_type           mvs_cmd_q;        /* Request Packets Queue           */
LOCAL q_type           mvs_free_q;       /* Available Packets Queue         */
LOCAL mvs_packet_type  mvs_free_packet [MVS_NUM_PACKETS];


/* MVS states for all clients */
LOCAL mvs_client_state_type mvs_client_states[MVS_CLIENT_MAX];

/* MVS clients that have been acquired */
LOCAL uint32 mvs_acquire_mask = 0;

/* Currently active and enabled client using MVS for vocoder packet exchange */
LOCAL mvs_client_type mvs_enabled_client = MVS_CLIENT_NONE;

/* Currently active and enabled client using MVS for PCM packet exchange */
#ifdef FEATURE_VOC_PCM_INTERFACE
LOCAL mvs_client_type mvs_pcm_enabled_client = MVS_CLIENT_NONE;
#endif

/* Current client listening to MVS */
LOCAL mvs_client_type mvs_listen_client = MVS_CLIENT_NONE;

/* Is MVS update done */
LOCAL boolean mvs_update_in_progress = FALSE;

/* MVS loopback setting */
LOCAL mvs_loopback_type mvs_loopback = MVS_LB_NONE;

/* MVS frame queues and buffers */
#define MVS_UL_FRAME_BUF_SIZE 1     /* 1 frame buffer */
#define MVS_DL_FRAME_BUF_SIZE 1     /* 1 frame buffer */
#define MVS_LB_FRAME_BUF_SIZE 1     /* no delay */
#ifdef FEATURE_VOC_PCM_INTERFACE
#define MVS_UL_PCM_FRAME_BUF_SIZE 1
#define MVS_DL_PCM_FRAME_BUF_SIZE 1
#endif

LOCAL mvs_frame_type mvs_ul_frame_buf[MVS_UL_FRAME_BUF_SIZE];
LOCAL mvs_frame_type mvs_dl_frame_buf[MVS_DL_FRAME_BUF_SIZE];
LOCAL mvs_frame_type mvs_lb_frame_buf[MVS_LB_FRAME_BUF_SIZE];
#ifdef FEATURE_VOC_PCM_INTERFACE
LOCAL mvs_frame_type mvs_ul_pcm_frame_buf[MVS_UL_PCM_FRAME_BUF_SIZE];
LOCAL mvs_frame_type mvs_dl_pcm_frame_buf[MVS_DL_PCM_FRAME_BUF_SIZE];
#endif

LOCAL mvs_frame_q_type mvs_ul_frame_q;
LOCAL mvs_frame_q_type mvs_dl_frame_q;
LOCAL mvs_frame_q_type mvs_lb_frame_q;
#ifdef FEATURE_VOC_PCM_INTERFACE
LOCAL mvs_frame_q_type mvs_ul_pcm_frame_q;
LOCAL mvs_frame_q_type mvs_dl_pcm_frame_q;
#endif

LOCAL uint8 pkt_buf[MVS_VOC_PKT_SIZE];

/* ================== C O N V E R S I O N   T A B L E S ================== */

LOCAL voc_acquire_type mvs_to_voc_client[((uint32) MVS_CLIENT_MAX)+1] =
{
  VOC_ACQ_MVS,    /* MVS_CLIENT_CDMA  */
  VOC_ACQ_MVS,    /* MVS_CLIENT_WCDMA */
  VOC_ACQ_MVS,    /* MVS_CLIENT_GSM   */
  VOC_ACQ_VS,     /* MVS_CLIENT_VOIP  */
  VOC_ACQ_VS,     /* MVS_CLIENT_QCHAT */
  VOC_ACQ_VOCVS,  /* MVS_CLIENT_VMEMO */
  VOC_ACQ_MVS,    /* MVS_CLIENT_QVP : auto sample slipping */
  VOC_ACQ_TEST,   /* MVS_CLIENT_QVP_TEST : manual sample slipping */
  VOC_ACQ_TEST,   /* MVS_CLIENT_TEST  */
  VOC_MAX_ACQUIRE_TYPE
};


LOCAL voc_capability_type mvs_to_voc_mode[((uint32) MVS_MODE_MAX)+1] =
{
  VOC_CAP_NONE,         /* MVS_MODE_NONE   */
  VOC_CAP_IS733,        /* MVS_MODE_IS733  */
  VOC_CAP_IS127,        /* MVS_MODE_IS127  */
  VOC_CAP_4GV_NB,       /* MVS_MODE_4GV_NB */
  VOC_CAP_4GV_WB,       /* MVS_MODE_4GV_WB */
#ifdef FEATURE_ACP
#error code not present
#endif
  VOC_CAP_AMR,          /* MVS_MODE_AMR    */
  VOC_CAP_GSM_EFR,      /* MVS_MODE_EFR    */
  VOC_CAP_GSM_FR,       /* MVS_MODE_FR     */
  VOC_CAP_GSM_HR,       /* MVS_MODE_HR     */
  VOC_CAP_IS733,        /* MVS_MODE_LINEAR_PCM, default: 13k */
  VOC_CAP_G711,         /* MVS_MODE_G711   */
  VOC_CAP_G723,         /* MVS_MODE_G723   */
  VOC_CAP_AMR_WB,       /* MVS_MODE_AMR_WB */
  VOC_CAP_MAX
};


void mvs_frame_ul_proc(uint8 *vocoder_packet, mvs_frame_info_type *frame_info,
                       uint16 packet_length, mvs_pkt_status_type *status);

void mvs_frame_dl_proc(uint8 *vocoder_packet, mvs_frame_info_type *frame_info,
                       mvs_pkt_status_type *status);

#ifdef FEATURE_VOC_PCM_INTERFACE
void mvs_pcm_frame_ul_proc(uint8 *vocoder_packet,
                           mvs_frame_info_type *frame_info,
                           uint16 packet_length, mvs_pkt_status_type *status);

void mvs_pcm_frame_dl_proc(uint8 *vocoder_packet,
                           mvs_frame_info_type *frame_info,
                           mvs_pkt_status_type *status);
#endif


/* The following functions define uplink isr callback functions in different
   context requested by client.
*/
LOCAL mvs_packet_ul_fn_type mvs_ul_isr_cb_tbl[MVS_PKT_CONTEXT_MAX] =
{
  NULL,               /* no context -- no packet exchange */
  NULL,               /* ADSP enc isr context -- configurable by client */
  mvs_frame_ul_proc   /* VOC task context -- local isr callback */
};

#ifdef FEATURE_VOC_PCM_INTERFACE
LOCAL mvs_packet_ul_fn_type mvs_pcm_ul_isr_cb_tbl[MVS_PKT_CONTEXT_MAX] =
{
  NULL,               /* no context -- no packet exchange */
  NULL,               /* ADSP pcm enc isr context -- configurable by client */
  mvs_pcm_frame_ul_proc   /* VOC task context -- local isr callback */
};
#endif


/* The following functions define downlink isr callback functions in different
   context requested by client.
*/
LOCAL mvs_packet_dl_fn_type mvs_dl_isr_cb_tbl[MVS_PKT_CONTEXT_MAX] =
{
  NULL,               /* no context -- no packet exchange */
  NULL,               /* ADSP dec isr context -- configurable by client */
  mvs_frame_dl_proc   /* VOC task context -- local isr callback */
};

#ifdef FEATURE_VOC_PCM_INTERFACE
LOCAL mvs_packet_dl_fn_type mvs_pcm_dl_isr_cb_tbl[MVS_PKT_CONTEXT_MAX] =
{
  NULL,               /* no context -- no packet exchange */
  NULL,               /* ADSP pcm dec isr context -- configurable by client */
  mvs_pcm_frame_dl_proc   /* VOC task context -- local isr callback */
};
#endif


#ifdef FEATURE_WCDMA
#error code not present
#endif  /* FEATURE_WCDMA */


#if defined(MSMAUD_VOC_IS733) || defined(MSMAUD_VOC_IS127) \
    || defined(MSMAUD_VOC_4GV) || defined(MSMAUD_VOC_4GV_WB)

LOCAL mvs_voc_rate_type voc_to_mvs_voc_rate[VOC_ERR_RATE] =
{
  MVS_VOC_0_RATE,          /* blank       */
  MVS_VOC_8_RATE,          /* eighth rate */
  MVS_VOC_4_RATE,          /* fourth rate */
  MVS_VOC_2_RATE,          /* half rate   */
  MVS_VOC_1_RATE,          /* full rate   */
  MVS_VOC_ERASURE
};

LOCAL voc_rate_type mvs_to_voc_voc_rate[MVS_VOC_RATE_MAX] =
{
  VOC_0_RATE,              /* blank       */
  VOC_8_RATE,              /* eighth rate */
  VOC_4_RATE,              /* fourth rate */
  VOC_2_RATE,              /* half rate   */
  VOC_1_RATE,              /* full rate   */
  VOC_ERASURE
};

#endif  /* MSMAUD_VOC_IS733 || MSMAUD_VOC_IS127 || MSMAUD_VOC_4GV
|| MSMAUD_VOC_4GV_WB*/


#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif  /* MSMAUD_VOC_EFR || MSMAUD_VOC_FR */

#ifdef MSMAUD_VOC_HR

LOCAL mvs_hr_frame_type voc_to_mvs_hr_frame[VOC_GSM_FRAME_MAX] =
{
  MVS_HR_SID,             /* VOC_GSM_SID */
  MVS_HR_SPEECH_GOOD,     /* VOC_GSM_SPEECH_GOOD */
  MVS_HR_SPEECH_BAD,      /* VOC_GSM_BFI */
  MVS_HR_INVALID_SID,     /* VOC_GSM_INVALID_SID */

  MVS_HR_FRAME_UNDEF,     /* VOC_AMR_SPEECH_GOOD */
  MVS_HR_FRAME_UNDEF,     /* VOC_AMR_SPEECH_DEGRADED */
  MVS_HR_FRAME_UNDEF,     /* VOC_AMR_ONSET */
  MVS_HR_FRAME_UNDEF,     /* VOC_AMR_SPEECH_BAD */
  MVS_HR_FRAME_UNDEF,     /* VOC_AMR_SID_FIRST */
  MVS_HR_FRAME_UNDEF,     /* VOC_AMR_SID_UPDATE */
  MVS_HR_FRAME_UNDEF,     /* VOC_AMR_SID_BAD */
  MVS_HR_FRAME_UNDEF      /* VOC_AMR_NO_DATA */
};

LOCAL voc_gsm_frame_type mvs_to_voc_hr_frame[MVS_HR_FRAME_MAX] =
{
  VOC_GSM_SID,             /* MVS_HR_SID */
  VOC_GSM_SPEECH_GOOD,     /* MVS_HR_SPEECH_GOOD */
  VOC_GSM_BFI,             /* MVS_HR_SPEECH_BAD  */
  VOC_GSM_INVALID_SID      /* MVS_HR_INVALID_SID */
};

#endif  /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
LOCAL mvs_g711_mode_type voc_to_mvs_g711_mode[VOC_G711_MODE_MAX] =
{
  MVS_G711_MODE_MULAW,     /* VOC_G711_MODE_MULAW */
  MVS_G711_MODE_ALAW       /* VOC_G711_MODE_ALAW  */
};

LOCAL voc_g711_mode_type mvs_to_voc_g711_mode[MVS_G711_MODE_MAX] =
{
  VOC_G711_MODE_MULAW,     /* MVS_G711_MODE_MULAW */
  VOC_G711_MODE_ALAW       /* MVS_G711_MODE_ALAW  */
};
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
LOCAL mvs_g723_mode_type voc_to_mvs_g723_mode[VOC_G723_MODE_MAX] =
{
  MVS_G723_MODE_HIGH,     /* VOC_G723_MODE_HIGH  */
  MVS_G723_MODE_LOW,      /* VOC_G723_MODE_LOW   */
  MVS_G723_MODE_SID,      /* VOC_G723_MODE_SID   */
  MVS_G723_MODE_BLANK,    /* VOC_G723_MODE_BLANK */
  MVS_G723_MODE_ERROR     /* VOC_G723_MODE_ERROR */
};

LOCAL voc_g723_mode_type mvs_to_voc_g723_mode[MVS_G723_MODE_MAX] =
{
  VOC_G723_MODE_HIGH,     /* MVS_G723_MODE_HIGH  */
  VOC_G723_MODE_LOW,      /* MVS_G723_MODE_LOW   */
  VOC_G723_MODE_SID,      /* MVS_G723_MODE_SID   */
  VOC_G723_MODE_BLANK,    /* MVS_G723_MODE_BLANK */
  VOC_G723_MODE_ERROR     /* MVS_G723_MODE_ERROR */
};

LOCAL byte mvs_g723_frame_len_table[MVS_G723_MODE_MAX] = {
  24,
  20,
  4,
  1,
  0
};
#endif /* MSMAUD_VOC_G723 */

#ifdef FEATURE_MVS_GEN_SILENCE
/* Clock to fill-in PCM frames when the vocoder is offline (during handover) */

#ifdef FEATURE_CMI
#error code not present
	#else
clk_cb_type mvs_silence_clk_cb;
	#endif
/* Clock to timeout when vocoder is busy with another client, so silence
** can operate irregardless.
*/

#ifdef FEATURE_CMI
#error code not present
	#else
clk_cb_type mvs_voc_clk_cb;
	#endif

/* Structures to aid in silence frame generation */
typedef struct {
  mvs_mode_type  mode;
  union {
    mvs_amr_frame_type amr;
    mvs_gsm_frame_type efr;
    mvs_gsm_frame_type fr;
    mvs_hr_frame_type  hr;
  } type;
  uint8          data[MVS_PCM_PKT_SIZE];
  int            length;
} mvs_silence_frame_type;

/* Storage for one silence frame */
mvs_silence_frame_type  mvs_silence_frame;


#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)
/* Count of consecutive silence frames to conform to AMR vocoder output */
static int mvs_silence_frame_count = 0;
#endif /* MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB */

/* Prototype for silence processing function used through the file */
extern void mvs_process_silence(int4 ms_interval);

#endif /* FEATURE_MVS_GEN_SILENCE */


/* <EJECT> */
/*===========================================================================

       T A S K   Q U E U E   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION mvs_init

DESCRIPTION
  This procedure initializes the MVS command queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_cmd_q
  mvs_free_q
  mvs_free_packet

===========================================================================*/
void mvs_init ( void )
{
  q_link_type *link_item;
  int         i;

  /* Initialize the command queues.
  */
  (void) q_init (&mvs_cmd_q);              /* Initialize the command queue  */
  (void) q_init (&mvs_free_q);             /* Initialize the free queue     */

  /* Add items to mvs_free_q.
  */
  for (i = 0; i < MVS_NUM_PACKETS; i++)
  {
    link_item = q_link( &mvs_free_packet[i], &mvs_free_packet[i].hdr.link );
    q_put( &mvs_free_q, link_item );
  }

  /* Initialize client state table */
  memset(mvs_client_states, 0, sizeof(mvs_client_state_type) *
         ((uint32) MVS_CLIENT_MAX));

  /* Initialize DSM buffers */
#if (defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)) \
    && defined(FEATURE_WCDMA)
#error code not present
#endif

  /* Initialize frame queues */
  mvssup_frame_q_init(&mvs_ul_frame_q, mvs_ul_frame_buf, MVS_UL_FRAME_BUF_SIZE);
  mvssup_frame_q_init(&mvs_dl_frame_q, mvs_dl_frame_buf, MVS_DL_FRAME_BUF_SIZE);
  mvssup_frame_q_init(&mvs_lb_frame_q, mvs_lb_frame_buf, MVS_LB_FRAME_BUF_SIZE);
#ifdef FEATURE_VOC_PCM_INTERFACE
  mvssup_frame_q_init(&mvs_ul_pcm_frame_q, mvs_ul_pcm_frame_buf,
                      MVS_UL_PCM_FRAME_BUF_SIZE);
  mvssup_frame_q_init(&mvs_dl_pcm_frame_q, mvs_dl_pcm_frame_buf,
                      MVS_DL_PCM_FRAME_BUF_SIZE);
#endif

#ifdef FEATURE_MVS_GEN_SILENCE
  /* Initialize Clock Services call-back data structures for silence gen. */
  
  #ifdef FEATURE_CMI
#error code not present
	#else
  clk_def( &mvs_silence_clk_cb );
  clk_def( &mvs_voc_clk_cb );
	#endif
#endif /* FEATURE_MVS_GEN_SILENCE */
} /* end mvs_init */


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_send_cmd_event

DESCRIPTION
  This function sends back a command event through the callback function of
the corresponding client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_send_cmd_event(
  mvs_client_type client,
  mvs_cmd_status_type status
)
{
  mvs_event_type event_data;


  if (client >= MVS_CLIENT_MAX)
  {
    MSG_ERROR("Unknown client %d", client, 0, 0);
    return;
  }

  if (mvs_client_states[client].event_cb != NULL)
  {
    event_data.cmd.hdr.event = MVS_EV_COMMAND;
    event_data.cmd.hdr.client = client;
    event_data.cmd.cmd_status = status;
    mvs_client_states[client].event_cb(&event_data);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_send_mode_event

DESCRIPTION
  This function sends back a mode event through the callback function of the
corresponding client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_send_mode_event(
  mvs_client_type client,
  mvs_mode_type mode,
  mvs_mode_status_type status
)
{
  mvs_event_type event_data;


  if (client >= MVS_CLIENT_MAX)
  {
    MSG_ERROR("Unknown client %d", client, 0, 0);
    return;
  }

  if (mvs_client_states[client].event_cb != NULL)
  {
    event_data.mode.hdr.event = MVS_EV_MODE;
    event_data.mode.hdr.client = client;
    event_data.mode.mode_status = status;
    event_data.mode.mode = mode;
    mvs_client_states[client].event_cb(&event_data);
  }
}


#ifdef FEATURE_MVS_GEN_SILENCE
/* <EJECT> */
/*===========================================================================

FUNCTION
  mvs_check_voc_state

DESCRIPTION
  Checks the current vocoder state in relation to MVS's state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_check_voc_state(
  int4 ms_interval
  /*lint -esym(715,ms_interval) Tell Lint we know ms_interval parameter
  ** is not used */
) {
  mvs_client_state_type *state = &(mvs_client_states[mvs_enabled_client]);

  if(mvs_client_states[mvs_enabled_client].state != MVS_STATE_ENABLE &&
     mvs_client_states[mvs_enabled_client].state != MVS_STATE_SILENCE)
  {
    MSG_ERROR("No response from VOC in %d ms. Silence enabled",ms_interval,0,0);

    /* Set variables so SILENCE mode can generate proper frames */
    state->config_mode = state->desired_config_mode;
    state->context = state->desired_context;

    switch (state->context)
    {
      case MVS_PKT_CONTEXT_ISR :
        state->ul_cb = state->desired_ul_cb;
        state->dl_cb = state->desired_dl_cb;
        break;

      case MVS_PKT_CONTEXT_NONE :
      case MVS_PKT_CONTEXT_MAX :
        state->ul_cb = NULL;
        state->dl_cb = NULL;
        break;

      default :
        if (state->desired_ul_cb == NULL)
        {
          state->ul_cb = NULL;
        }
        else
        {
          state->ul_cb = mvs_ul_isr_cb_tbl[state->context];
        }

        if (state->desired_dl_cb == NULL)
        {
          state->dl_cb = NULL;
        }
        else
        {
          state->dl_cb = mvs_dl_isr_cb_tbl[state->context];
        }
        break;
    }

    /* Tell client we're ready, so silence packets aren't unexpected. */
    mvs_send_mode_event(mvs_enabled_client, state->desired_config_mode,
                        MVS_MODE_READY);

    /* Activate SILENCE mode */
    mvs_client_states[mvs_enabled_client].state = MVS_STATE_SILENCE;
    mvs_process_silence(0);
  }
}

/*===========================================================================

FUNCTION
  mvs_check_voc_state

DESCRIPTION
  Checks the current PCM state in relation to MVS's state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_pcm_check_voc_state(
  int4 ms_interval
  /*lint -esym(715,ms_interval) Tell Lint we know ms_interval parameter
  ** is not used */
) {
  mvs_client_state_type *state = &(mvs_client_states[mvs_enabled_client]);

  if(mvs_client_states[mvs_enabled_client].pcm_state != MVS_STATE_ENABLE &&
     mvs_client_states[mvs_enabled_client].pcm_state != MVS_STATE_SILENCE)
  {
    MSG_ERROR("No response from VOC in %d ms. Silence enabled",ms_interval,0,0);

    /* Set variables so SILENCE mode can generate proper frames */
    state->pcm_context = state->desired_pcm_context;

    switch (state->pcm_context)
    {
      case MVS_PKT_CONTEXT_ISR :
        state->pcm_ul_cb = state->desired_pcm_ul_cb;
        state->pcm_dl_cb = state->desired_pcm_dl_cb;
        break;

      case MVS_PKT_CONTEXT_NONE :
      case MVS_PKT_CONTEXT_MAX :
        state->pcm_ul_cb = NULL;
        state->pcm_dl_cb = NULL;
        break;

      default :
        if (state->desired_pcm_ul_cb == NULL)
        {
          state->pcm_ul_cb = NULL;
        }
        else
        {
          state->pcm_ul_cb = mvs_pcm_ul_isr_cb_tbl[state->context];
        }

        if (state->desired_pcm_dl_cb == NULL)
        {
          state->pcm_dl_cb = NULL;
        }
        else
        {
          state->pcm_dl_cb = mvs_pcm_dl_isr_cb_tbl[state->context];
        }
        break;
    }

    /* Tell client we're ready, so silence packets aren't unexpected. */
    mvs_send_mode_event(mvs_pcm_enabled_client, MVS_MODE_LINEAR_PCM,
                        MVS_MODE_READY);

    /* Activate SILENCE mode */
    mvs_client_states[mvs_enabled_client].pcm_state = MVS_STATE_SILENCE;
    mvs_process_silence(0);
  }
}

/*===========================================================================

FUNCTION
  mvs_put_silence_frame

DESCRIPTION
  Stores a silence frame into local memory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_silence_frame

===========================================================================*/
void mvs_put_silence_frame(
  mvs_mode_type  mode,
  uint8          *frame,
  int            length
) {
  if(frame == NULL) {
    MSG_FATAL("mvs_put_silence_frame() null frame pointer",0,0,0);
  }

  mvs_silence_frame.mode = mode;
  mvs_silence_frame.length = length;
  memcpy(mvs_silence_frame.data, frame, length);
}

/*===========================================================================

FUNCTION
  mvs_reset_silence_frame

DESCRIPTION
  Will reset the current silence frame to a default value for the current mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_reset_silence_frame(mvs_mode_type mode)
{
  /* Silence packet definitions
  **
  */
#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)
  static const uint8 mvs_amr_silence_packet_base[] = {
    0xc7,0x26,0x68,0x83,0x00,0x00
  };
#endif /* MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB */
#ifdef MSMAUD_VOC_EFR
#error code not present
#endif /* MSMAUD_VOC_EFR */
#ifdef MSMAUD_VOC_FR
#error code not present
#endif /* MSMAUD_VOC_FR */
#ifdef MSMAUD_VOC_HR
  static const uint8 mvs_hr_silence_packet_base[] = {
    /* Known good silence frame */
    0x03,0x31,0xbc,0x68,0x7f
  };
#endif /* MSMAUD_VOC_HR */

  MSG_ERROR("SILENCE: Resetting frame mode %d",mode,0,0);
  switch(mode) {
#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)
    case MVS_MODE_AMR:
    case MVS_MODE_AMR_WB:
      mvs_silence_frame_count = 0;
      mvs_put_silence_frame(mode,
                            (uint8*)mvs_amr_silence_packet_base,
                            sizeof(mvs_amr_silence_packet_base));
      break;
#endif /* MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB */
#ifdef MSMAUD_VOC_EFR
#error code not present
#endif /* MSMAUD_VOC_EFR */
#ifdef MSMAUD_VOC_FR
#error code not present
#endif /* MSMAUD_VOC_FR */
#ifdef MSMAUD_VOC_HR
    case MVS_MODE_HR:
      mvs_put_silence_frame(MVS_MODE_HR,
                            (uint8*)mvs_hr_silence_packet_base,
                            sizeof(mvs_hr_silence_packet_base));
      break;
#endif /* MSMAUD_VOC_HR */

    case MVS_MODE_NONE:
    default:
      break;
  }
}

/*===========================================================================

FUNCTION
  mvs_get_silence_frame

DESCRIPTION
  Gets the current silence frame from memory.

DEPENDENCIES
  mvs_silence_frame

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_get_silence_frame(
  mvs_mode_type           mode,
  mvs_silence_frame_type  *silence_frame
) {
  if(mode != mvs_silence_frame.mode) {
    MSG_ERROR("Incompatible mode %d to stored frame %d. Resetting",mode,mvs_silence_frame.mode,0);
    mvs_reset_silence_frame(mode);
  }
  if(silence_frame == NULL) {
    MSG_FATAL("mvs_get_silence_frame() null silence_frame pointer",0,0,0);
  }

  /* add extra information for certain packets */
  switch(mode) {
    case MVS_MODE_EFR:
      silence_frame->type.efr = MVS_GSM_SID;
      break;
    case MVS_MODE_FR:
      silence_frame->type.fr = MVS_GSM_SID;
      break;
    case MVS_MODE_HR:
      silence_frame->type.hr = MVS_HR_SID;
      break;
    case MVS_MODE_AMR:
    case MVS_MODE_AMR_WB:
      silence_frame->type.amr = MVS_AMR_SID_UPDATE;
      break;
    default:
      MSG_ERROR("No silence frames for mode %d",mode,0,0);
      break;
  }
  silence_frame->mode = mvs_silence_frame.mode;
  silence_frame->length = mvs_silence_frame.length;
  memcpy(silence_frame->data, mvs_silence_frame.data, mvs_silence_frame.length);
}

#ifdef MSMAUD_VOC_AMR_WB
/*===========================================================================

FUNCTION mvs_get_awb_silence_frame

DESCRIPTION
  Gets an AMR-WB silence frame, based on AMR-WB vocoder rules.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_get_awb_silence_frame(
  uint8               vocoder_packet[],
  voc_amr_wb_frame_type  *voc_frame_type
)
{
  int pkt_len;
  mvs_silence_frame_type silence_frame;

  if(mvs_silence_frame.mode != MVS_MODE_AMR_WB) {
    mvs_reset_silence_frame(MVS_MODE_AMR_WB);
  }

  /* Generate proper silence frame based on silence counter */
  switch(mvs_silence_frame_count) {
    case 0:
      mvs_silence_frame_count++;
      /* generate sid first */
      pkt_len = 6;
      memset(vocoder_packet, 0, MVS_VOC_AWB_PKT_SIZE);
      *voc_frame_type = VOC_AMR_WB_SID_FIRST;
      break;
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
      mvs_silence_frame_count++;
      /* generate no data */
      pkt_len = 0;
      memset(vocoder_packet, 0, MVS_VOC_AWB_PKT_SIZE);
      *voc_frame_type = VOC_AMR_WB_NO_DATA;
      break;
    case 3:
      mvs_silence_frame_count++;
      /* generate sid update */
      mvs_get_silence_frame(MVS_MODE_AMR_WB, &silence_frame);
      pkt_len = silence_frame.length;
      memset(vocoder_packet+pkt_len, 0, MVS_VOC_AWB_PKT_SIZE-pkt_len);
      memcpy(vocoder_packet, silence_frame.data, pkt_len);
      *voc_frame_type = silence_frame.mode;
      break;
    case 10:
      mvs_silence_frame_count = 3;
      /* generate no data */
      pkt_len = 0;
      memset(vocoder_packet, 0, MVS_VOC_AWB_PKT_SIZE);
      *voc_frame_type = VOC_AMR_WB_NO_DATA;
      break;
  }
}
#endif /* MSMAUD_VOC_AMR_WB */

#ifdef MSMAUD_VOC_AMR
/*===========================================================================

FUNCTION mvs_get_amr_silence_frame

DESCRIPTION
  Gets an AMR silence frame, based on AMR vocoder rules.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_get_amr_silence_frame(
  uint8               vocoder_packet[],
  voc_gsm_frame_type  *voc_frame_type
)
{
  int pkt_len;
  mvs_silence_frame_type silence_frame;

  if(mvs_silence_frame.mode != MVS_MODE_AMR) {
    mvs_reset_silence_frame(MVS_MODE_AMR);
  }

  /* Generate proper silence frame based on silence counter */
  switch(mvs_silence_frame_count) {
    case 0:
      mvs_silence_frame_count++;
      /* generate sid first */
      pkt_len = 6;
      memset(vocoder_packet, 0, MVS_VOC_PKT_SIZE);
      *voc_frame_type = VOC_AMR_SID_FIRST;
      break;
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
      mvs_silence_frame_count++;
      /* generate no data */
      pkt_len = 0;
      memset(vocoder_packet, 0, MVS_VOC_PKT_SIZE);
      *voc_frame_type = VOC_AMR_NO_DATA;
      break;
    case 3:
      mvs_silence_frame_count++;
      /* generate sid update */
      mvs_get_silence_frame(MVS_MODE_AMR, &silence_frame);
      pkt_len = silence_frame.length;
      memset(vocoder_packet+pkt_len, 0, MVS_VOC_PKT_SIZE-pkt_len);
      memcpy(vocoder_packet, silence_frame.data, pkt_len);
      *voc_frame_type = silence_frame.mode;
      break;
    case 10:
      mvs_silence_frame_count = 3;
      /* generate no data */
      pkt_len = 0;
      memset(vocoder_packet, 0, MVS_VOC_PKT_SIZE);
      *voc_frame_type = VOC_AMR_NO_DATA;
      break;
  }
}
#endif /* MSMAUD_VOC_AMR */

#endif /* FEATURE_MVS_GEN_SILENCE */


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_get_pkt

DESCRIPTION
  Get a MVS Command Packet.

DEPENDENCIES
  mvs_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available mvs_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  mvs_free_q (and the packet fetched off voc_free_q)

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
mvs_packet_type *mvs_get_pkt (void)
{
  mvs_packet_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (mvs_packet_type *) q_get (&mvs_free_q);

  if (packet == NULL) {
    ERR ("Ran Out of MVS Packets!", 0, 0, 0);
  } else {
    /* Fill the entire MVS packet with 0's to get
    ** default behavior on un-initialized fields.
    */
    (void) memset (packet, 0, sizeof (mvs_packet_type));
    packet->hdr.cmd = MVS_CMD_INVALID;
  }

  return (packet);

} /* mvs_get_pkt */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_cmd

DESCRIPTION
  Queue a command for processing by the MVS Services.

DEPENDENCIES
  mvs_init() needs to have initialized the queues.

RETURN VALUE
  None

SIDE EFFECTS
  mvs_cmd_q
  vs_tcb

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvs_cmd(
  mvs_packet_type *mvs_cmd_ptr
    /* pointer to VS command */
)
{
  if (mvs_cmd_ptr==NULL)
  {
    return;
  }

  (void) q_link (mvs_cmd_ptr, &mvs_cmd_ptr->hdr.link);    /* Init link field */

  q_put (&mvs_cmd_q, &mvs_cmd_ptr->hdr.link);       /* Put on command queue  */
  (void) rex_set_sigs(&voc_tcb, VOC_MVS_CMD_Q_SIG);  /* Signal a queue event  */

} /* end mvs_cmd */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_cmd_done

DESCRIPTION
  Dispose of a command which is done being processed.

DEPENDENCIES
  mvs_free_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mvs_cmd_done(
  mvs_packet_type *mvs_cmd_ptr
)
{
  if (mvs_cmd_ptr!=NULL)
  {
    q_put (&mvs_free_q, &mvs_cmd_ptr->hdr.link);
  }
} /* mvs_cmd_done */

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION mvs_prand

DESCRIPTION
  This function is to generate a pseudo-random vocoder packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_prand(uint8 *vocoder_packet, uint16 size)
{
  uint32 ran;
  uint16 i, j;


  if (size==0)
  {
    return;
  }


  for (i=0,j=0;i<(size>>2);i++)
  {
    ran = ran_next();
    vocoder_packet[j++] = ran & 0xff;
    ran >>= 8;
    vocoder_packet[j++] = ran & 0xff;
    ran >>= 8;
    vocoder_packet[j++] = ran & 0xff;
    ran >>= 8;
    vocoder_packet[j++] = ran & 0xff;
  }


  ran = ran_next();

  for (i=(uint16)(i<<2);i<size;i++)
  {
    vocoder_packet[i] = ran & 0xff;
    ran >>= 8;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_lb_put_pkt

DESCRIPTION
  This function will store a packet into the MVS loopback buffer.  It will
overwrite the current packet in the buffer if it has not been taken.

DEPENDENCIES
  None

RETURN VALUE
  MVS_PKT_STATUS_NORMAL : normal, no overwriting
  MVS_PKT_STATUS_FAST : too fast in trying to put packet,
                        existing packet in the buffer has been overwritten

SIDE EFFECTS
  None

===========================================================================*/
mvs_pkt_status_type mvs_lb_put_pkt(
  uint8 *pkt,
  mvs_frame_info_type *frame_info,
  uint16 pkt_len
)
{
  return mvssup_frame_q_put(&mvs_lb_frame_q, pkt, frame_info, pkt_len);
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_lb_get_pkt

DESCRIPTION
  This function will retrieve a packet from the MVS loopback buffer.

DEPENDENCIES
  None

RETURN VALUE
  MVS_PKT_STATUS_NORMAL : retrieval is successful.
  MVS_PKT_STATUS_SLOW : too fast in trying to get packet,
                        no loopback packet is available.
                        packet produced too slow.

SIDE EFFECTS
  None

===========================================================================*/
mvs_pkt_status_type mvs_lb_get_pkt(
  uint8 *pkt,
  mvs_frame_info_type *frame_info
)
{
  return mvssup_frame_q_get(&mvs_lb_frame_q, pkt, frame_info, NULL);
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_set_loopback

DESCRIPTION
  This function will set up the MVS loopback mode:

  MVS_LB_NONE: no loopback

  MVS_LB_UL2DL_VOC: route uplink voice packets to downlink; bypass DSM queues
  MVS_LB_UL2DL_DSM: route uplink voice packets to downlink through DSM queues
  MVS_LB_UL2DL_PCM: route uplink PCM samples into downlink

  MVS_LB_DL2UL_VOC: route downlink voice packets back to uplink
  MVS_LB_DL2UL_PCM: route downlink PCM samples into uplink

  NOTE: if DSM queues are not utilized, MVS_LB_UL2DL_DSM functions the same as
        MVS_LB_UL2DL_VOC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_set_loopback(
  mvs_loopback_type lb_mode
)
{
  uint32 isave;


  if ((lb_mode < MVS_LB_NONE) || (lb_mode >= MVS_LB_MAX))
  {
    MSG_ERROR("Invalid loopback mode %d", lb_mode, 0, 0);
    return;
  }


  if (lb_mode != mvs_loopback)
  {
    INTLOCK_SAV(isave);
    mvssup_frame_q_empty(&mvs_lb_frame_q);
    mvssup_frame_q_empty(&mvs_ul_frame_q);
    mvssup_frame_q_empty(&mvs_dl_frame_q);
#ifdef FEATURE_VOC_PCM_INTERFACE
    mvssup_frame_q_empty(&mvs_ul_pcm_frame_q);
    mvssup_frame_q_empty(&mvs_dl_pcm_frame_q);
#endif
    INTFREE_SAV(isave);
  }

  mvs_loopback = lb_mode;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_tx_processing

DESCRIPTION
  This function processes TX MVS command and sends all uplink packets
stored in the queue to client.
  This function is driven by vocoder task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_tx_processing(
  mvs_client_type client,
  mvs_frame_q_type *ul_qptr,
  mvs_packet_ul_fn_type ul_func
)
{
  mvs_pkt_status_type status;
  mvs_frame_type frame;
  uint32 isave;


  if ((client >= MVS_CLIENT_MAX) || (ul_qptr == NULL) || (ul_func == NULL))
  {
    return;
  }

  if ((ul_func != mvs_client_states[client].desired_ul_cb)
#ifdef FEATURE_VOC_PCM_INTERFACE
      && (ul_func != mvs_client_states[client].desired_pcm_ul_cb)
#endif
     )
  {
    MSG_ERROR("Unregistered uplink callback function %x", ul_func, 0, 0);
    return;
  }


  do {
      /* prevent to be preempted by enc isr which will queue up packets */
      INTLOCK_SAV(isave);
      if (mvssup_frame_check_q_empty(ul_qptr) == TRUE)
      {
        /* no packets available */
        INTFREE_SAV(isave);
        break;
      }

      status = mvssup_frame_q_get(ul_qptr, frame.frame_data,
                                  &(frame.frame_info), &(frame.frame_len));
      INTFREE_SAV(isave);

      if (status!=MVS_PKT_STATUS_NORMAL)
      {
        MSG_ERROR("Unexpected error %d", status, 0, 0);
        break;
      }


      ul_func(frame.frame_data,
              &(frame.frame_info),
              frame.frame_len,
              &status);
      if (status==MVS_PKT_STATUS_FAST)
      {
        MSG_MED("Warning: lost TX packet status %d", status, 0, 0);
      }
     }
  while (status==MVS_PKT_STATUS_SLOW);    /* to prevent packet loss */
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_rx_processing

DESCRIPTION
  This function processes RX MVS command and retrieves downlink packets
from client to store into the queue.
  This function is driven by vocoder task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_rx_processing(
  mvs_client_type client,
  mvs_frame_q_type *dl_qptr,
  mvs_packet_dl_fn_type dl_func
)
{
  boolean is_q_full;
  mvs_pkt_status_type status;
  mvs_frame_type frame;
  uint16 packet_len;
  uint32 isave;


  if ((client >= MVS_CLIENT_MAX) || (dl_qptr == NULL) || (dl_func == NULL))
  {
    return;
  }

  if (dl_func == mvs_client_states[client].desired_dl_cb)
  {
#ifdef MSMAUD_VOC_G711
    if(mvs_client_states[client].config_mode == MVS_MODE_G711) {
      packet_len = MVS_VOC_G711_PKT_SIZE;
    } else
#endif /* MSMAUD_VOC_G711 */
#ifdef MSMAUD_VOC_G723
    if(mvs_client_states[client].config_mode == MVS_MODE_G723) {
      packet_len = MVS_VOC_G723_PKT_SIZE;
    } else
#endif /* MSMAUD_VOC_G723 */
#ifdef MSMAUD_VOC_AMR_WB
    if(mvs_client_states[client].config_mode == MVS_MODE_AMR_WB) {
      packet_len = MVS_VOC_AWB_PKT_SIZE;
    } else
#endif  /* MSMAUD_VOC_AMR_WB */
    {
      packet_len = MVS_VOC_PKT_SIZE;
    }
  }
#ifdef FEATURE_VOC_PCM_INTERFACE
  else if (dl_func == mvs_client_states[client].desired_pcm_dl_cb)
       {
         packet_len = MVS_PCM_PKT_SIZE;
       }
#endif
       else
       {
         MSG_ERROR("Unregistered downlink callback function %x", dl_func, 0, 0);
         return;
       }


  do {
      /* Prevent to be preempted by dec isr which will dequeue packets */
      INTLOCK_SAV(isave);
      is_q_full = mvssup_frame_check_q_full(dl_qptr);
      INTFREE_SAV(isave);

      if (is_q_full == TRUE)
      {
        /* buffer is full */
        break;
      }

      dl_func(frame.frame_data,
              &(frame.frame_info),
              &status);


       /*
      // limit max. no. of bytes for memcpy

      if (frame.frame_len < packet_len)
      {
        packet_len = frame.frame_len;
      }
      */

      if (status==MVS_PKT_STATUS_SLOW)
      {
        /* no packets available */
        break;
      }

      if (status==MVS_PKT_STATUS_FAST)
      {
        MSG_MED("lost voice frame.", 0, 0, 0);
      }

      /* Prevent to be preempted by dec isr which will dequeue packets */
      INTLOCK_SAV(isave);
      status = mvssup_frame_q_put(dl_qptr, frame.frame_data,
                                  &(frame.frame_info), packet_len);
      INTFREE_SAV(isave);
     }
  while (status==MVS_PKT_STATUS_NORMAL);
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_frame_ul_proc

DESCRIPTION
  Queue up uplink MVS vocoder frame and issue MVS command to deliver all uplink
packets in the buffer.
  This function is called in ADSP enc isr context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_frame_ul_proc(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info,
  uint16 packet_length,
  mvs_pkt_status_type *status
)
{
  mvs_packet_type *packet;


  *status = mvssup_frame_q_put(&mvs_ul_frame_q, vocoder_packet, frame_info,
                               packet_length);

  if (mvs_client_states[mvs_enabled_client].desired_ul_cb != NULL)
  {
    packet = mvs_get_pkt();
    if (packet != NULL)
    {
      packet->hdr.cmd = MVS_CMD_TX;
      packet->tx.client = mvs_enabled_client;
      packet->tx.ul_qptr = &mvs_ul_frame_q;
      packet->tx.ul_func = mvs_client_states[mvs_enabled_client].desired_ul_cb;
      mvs_cmd(packet);
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_frame_dl_proc

DESCRIPTION
  Queue up downlink MVS vocoder frame and issue MVS command to request downlink
packets.
  This function is called in ADSP dec isr context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_frame_dl_proc(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info,
  mvs_pkt_status_type *status
)
{
  mvs_packet_type *packet;
  uint16 dl_frame_len;


  if (mvssup_frame_check_q_empty(&mvs_dl_frame_q) == TRUE)
  {
    *status = MVS_PKT_STATUS_SLOW;
  }
  else
  {
    *status = mvssup_frame_q_get(&mvs_dl_frame_q, vocoder_packet, frame_info,
                                 &dl_frame_len);
  }

  if (mvs_client_states[mvs_enabled_client].desired_dl_cb != NULL)
  {
    packet = mvs_get_pkt();
    if (packet != NULL)
    {
      packet->hdr.cmd = MVS_CMD_RX;
      packet->rx.client = mvs_enabled_client;
      packet->rx.dl_qptr = &mvs_dl_frame_q;
      packet->rx.dl_func = mvs_client_states[mvs_enabled_client].desired_dl_cb;
      mvs_cmd(packet);
    }
  }
}


/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

FUNCTION mvs_pcm_frame_ul_proc

DESCRIPTION
  Queue up uplink MVS PCM frame and issue MVS command to deliver all uplink
packets in the buffer.
  This function is called in ADSP tx pcm read isr context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_pcm_frame_ul_proc(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info,
  uint16 packet_length,
  mvs_pkt_status_type *status
)
{
  mvs_packet_type *packet;


  *status = mvssup_frame_q_put(&mvs_ul_pcm_frame_q, vocoder_packet, frame_info,
                               packet_length);

  if (mvs_client_states[mvs_pcm_enabled_client].desired_pcm_ul_cb != NULL)
  {
    packet = mvs_get_pkt();
    if (packet != NULL)
    {
      packet->hdr.cmd = MVS_CMD_TX;
      packet->tx.client = mvs_pcm_enabled_client;
      packet->tx.ul_qptr = &mvs_ul_pcm_frame_q;
      packet->tx.ul_func
      = mvs_client_states[mvs_pcm_enabled_client].desired_pcm_ul_cb;
      mvs_cmd(packet);
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_pcm_frame_dl_proc

DESCRIPTION
  Queue up downlink MVS PCM frame and issue MVS command to request downlink
packets.
  This function is called in ADSP rx pcm write isr context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_pcm_frame_dl_proc(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info,
  mvs_pkt_status_type *status
)
{
  mvs_packet_type *packet;


  if (mvssup_frame_check_q_empty(&mvs_dl_pcm_frame_q) == TRUE)
  {
    *status = MVS_PKT_STATUS_SLOW;
  }
  else
  {
    *status = mvssup_frame_q_get(&mvs_dl_pcm_frame_q, vocoder_packet,
                                 frame_info, NULL);
  }

  if (mvs_client_states[mvs_pcm_enabled_client].desired_pcm_dl_cb != NULL)
  {
    packet = mvs_get_pkt();
    if (packet != NULL)
    {
      packet->hdr.cmd = MVS_CMD_RX;
      packet->rx.client = mvs_pcm_enabled_client;
      packet->rx.dl_qptr = &mvs_dl_pcm_frame_q;
      packet->rx.dl_func
      = mvs_client_states[mvs_pcm_enabled_client].desired_pcm_dl_cb;
      mvs_cmd(packet);
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_pcm_ul_proc

DESCRIPTION
  Process uplink PCM packets provided by vocoder.  This function is
called in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_pcm_ul_proc(const uint16 *pcm_data)
{
  uint8 pcm_buf[MVS_PCM_PKT_SIZE];
  mvs_frame_info_type dummy_info;
  mvs_pkt_status_type status;


  dummy_info.hdr.mode = MVS_MODE_LINEAR_PCM;

  if ((mvs_pcm_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_pcm_enabled_client].pcm_state
          == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_pcm_enabled_client].pcm_context
          == MVS_PKT_CONTEXT_NONE)
     )
  {
    MSG_ERROR("Invalid MVS client %d for PCM UL processing.",
              mvs_pcm_enabled_client, 0, 0);
    return;
  }


  if ((mvs_client_states[mvs_pcm_enabled_client].pcm_ul_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_PCM))
  {
    return;
  }


  if (mvs_loopback == MVS_LB_DL2UL_PCM)
  {
    if (mvs_lb_get_pkt(pcm_buf, &dummy_info) == MVS_PKT_STATUS_SLOW)
    {
      return;
    }
    pcm_data = (const uint16*)pcm_buf;
  }

  if (mvs_client_states[mvs_pcm_enabled_client].pcm_ul_cb != NULL)
  {
    mvs_client_states[mvs_pcm_enabled_client].pcm_ul_cb((uint8*)pcm_data,
                                                        &dummy_info,
                                                        MVS_PCM_PKT_SIZE,
                                                        &status);
    if (status != MVS_PKT_STATUS_NORMAL)
    {
      MSG_MED("Warning: TX packet status %d", status, 0, 0);
    }
  }

  if (mvs_loopback == MVS_LB_UL2DL_PCM)
  {
    (void)mvs_lb_put_pkt((uint8*)pcm_data, &dummy_info, MVS_PCM_PKT_SIZE);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_pcm_dl_proc

DESCRIPTION
  Retrieve downlink PCM packets and feed them to vocoder.  This
function is called in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_pcm_data_status_type mvs_voc_pcm_dl_proc(uint16 *pcm_data)
{
  mvs_frame_info_type dummy_info;
  mvs_pkt_status_type status = MVS_PKT_STATUS_NORMAL;

  dummy_info.hdr.mode = MVS_MODE_LINEAR_PCM;

  if ((mvs_pcm_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_pcm_enabled_client].pcm_state
          == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_pcm_enabled_client].pcm_context
          == MVS_PKT_CONTEXT_NONE))
  {
    MSG_ERROR("Invalid MVS client %d for PCM DL processing.",
              mvs_pcm_enabled_client, 0, 0);
    return VOC_PCM_DATA_STATUS_UNAVAILABLE;
  }


  if ((mvs_client_states[mvs_pcm_enabled_client].pcm_dl_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_PCM))
  {
    return VOC_PCM_DATA_STATUS_UNAVAILABLE;
  }


  if (mvs_client_states[mvs_pcm_enabled_client].pcm_dl_cb != NULL)
  {
    mvs_client_states[mvs_pcm_enabled_client].pcm_dl_cb((uint8*)pcm_data,
                                                   &dummy_info,
                                                   &status);
    if (status == MVS_PKT_STATUS_SLOW)
    {
      MSG_MED("no PCM DL packets available.", 0, 0, 0);
    }

    if (status == MVS_PKT_STATUS_FAST)
    {
      MSG_MED("lost PCM DL packets.", 0, 0, 0);
    }
  }

  if (mvs_loopback == MVS_LB_UL2DL_PCM)
  {
    status = mvs_lb_get_pkt((uint8*)pcm_data, &dummy_info);
  }

  if (status == MVS_PKT_STATUS_SLOW)
  {
    return VOC_PCM_DATA_STATUS_UNAVAILABLE;
  }


  if (mvs_loopback == MVS_LB_DL2UL_PCM)
  {
    (void)mvs_lb_put_pkt((uint8*)pcm_data, &dummy_info, MVS_PCM_PKT_SIZE);
  }

  return VOC_PCM_DATA_STATUS_AVAILABLE;
}


#endif  /* FEATURE_VOC_PCM_INTERFACE */

/* <EJECT> */
#if defined(MSMAUD_VOC_IS733) || defined(MSMAUD_VOC_IS127) \
    || defined(MSMAUD_VOC_4GV) || defined(MSMAUD_VOC_4GV_WB)
/*===========================================================================

FUNCTION mvs_voc_evrc_ul_proc

DESCRIPTION
  Process uplink 13k or EVRC or 4GV packets provided by vocoder.
  This function is called in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_evrc_ul_proc(
  uint8 *vocoder_packet,
  voc_rate_type frame_rate,
  uint16 pkt_len
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
     )
  {
    MSG_ERROR("Invalid MVS client %d for 13k or EVRC or 4GV UL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  switch (mvs_client_states[mvs_enabled_client].config_mode)
  {
#ifdef MSMAUD_VOC_IS733
    case MVS_MODE_IS733 :
      break;
#endif  /* MSMAUD_VOC_IS733 */

#ifdef MSMAUD_VOC_IS127
    case MVS_MODE_IS127 :
      break;
#endif  /* MSMAUD_VOC_IS127 */

#ifdef MSMAUD_VOC_4GV
    case MVS_MODE_4GV_NB :
      break;
#endif  /* MSMAUD_VOC_4GV */
#ifdef MSMAUD_VOC_4GV_WB
    case MVS_MODE_4GV_WB :
      break;
#endif  /* MSMAUD_VOC_4GV_WB */

    default :
      MSG_ERROR("Invalid MVS client %d for 13k or EVRC or 4GV UL processing.",
                mvs_enabled_client, 0, 0);
      return;
  }

  if ((mvs_client_states[mvs_enabled_client].ul_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    return;
  }


#ifdef FEATURE_VOC_PCM_INTERFACE
  if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
  {
    frame_rate = VOC_0_RATE;
    pkt_len = 0;
  }
#endif  /* FEATURE_VOC_PCM_INTERFACE */


  /* Check for valid frame rate */
  if ((frame_rate < VOC_0_RATE) || (frame_rate >= VOC_ERR_RATE))
  {
    MSG_ERROR("Invalid UL 13k or EVRC or 4GV frame rate %d", frame_rate, 0, 0);
    frame_info.voc_rate.tx_info.rate = MVS_VOC_ERASURE;
  }
  else
  {
    frame_info.voc_rate.tx_info.rate = voc_to_mvs_voc_rate[frame_rate];
  }


  if (mvs_loopback == MVS_LB_DL2UL_VOC)
  {
    if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      return;
    }

    frame_info.voc_rate.tx_info.rate = lb_frame_info.voc_rate.rx_info.rate;
    pkt_len = MVS_VOC_PKT_SIZE;
  }


  if (mvs_client_states[mvs_enabled_client].ul_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].ul_cb(vocoder_packet, &frame_info,
                                               pkt_len, &status);
    if (status != MVS_PKT_STATUS_NORMAL)
    {
      MSG_MED("Warning: TX packet status %d",status,0,0);
    }
  }


  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_PKT_SIZE);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_evrc_dl_proc

DESCRIPTION
  Retrieve downlink 13k or EVRC or 4GV packets and feed them to vocoder.  This
function is called in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_evrc_dl_proc(
  uint8          **vocoder_packet,
  voc_rate_type  *frame_rate
)
{
  mvs_frame_info_type  frame_info;
  mvs_frame_info_type  lb_frame_info;
  mvs_voc_rate_type    rate;
  mvs_pkt_status_type  status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* initialise frame_info to prevent lint errors */
  frame_info.voc_rate.rx_info.rate = MVS_VOC_ERASURE;


  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
     )
  {
    MSG_ERROR("Invalid MVS client %d for 13k or EVRC or 4GV DL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  switch (mvs_client_states[mvs_enabled_client].config_mode)
  {
#ifdef MSMAUD_VOC_IS733
    case MVS_MODE_IS733 :
      break;
#endif  /* MSMAUD_VOC_IS733 */

#ifdef MSMAUD_VOC_IS127
    case MVS_MODE_IS127 :
      break;
#endif  /* MSMAUD_VOC_IS127 */

#ifdef MSMAUD_VOC_4GV
    case MVS_MODE_4GV_NB :
      break;
#endif  /* MSMAUD_VOC_4GV */
#ifdef MSMAUD_VOC_4GV_WB
    case MVS_MODE_4GV_WB :
      break;
#endif  /* MSMAUD_VOC_4GV_WB */

    default :
      MSG_ERROR("Invalid MVS client %d for 13k or EVRC or 4GV DL processing.",
                mvs_enabled_client, 0, 0);
      return;
  }


  *vocoder_packet = pkt_buf;


  if ((mvs_client_states[mvs_enabled_client].dl_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    *frame_rate = VOC_ERASURE;
    return;
  }


  if (mvs_client_states[mvs_enabled_client].dl_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].dl_cb(pkt_buf, &frame_info, &status);

    if (status == MVS_PKT_STATUS_SLOW)
    {
      frame_info.voc_rate.rx_info.rate = MVS_VOC_ERASURE;

      MSG_MED("no 13k or EVRC or 4GV DL pkts available.", 0, 0, 0);
    }

    if (status == MVS_PKT_STATUS_FAST)
    {
      MSG_MED("lost 13k or EVRC or 4GV DL pkts.", 0, 0, 0);
    }
  }

  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    if (mvs_lb_get_pkt(pkt_buf, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      *frame_rate = VOC_ERASURE;
      return;
    }

    frame_info.voc_rate.rx_info.rate = lb_frame_info.voc_rate.tx_info.rate;
  }

  /* Validate rate from info structure */
  rate = frame_info.voc_rate.rx_info.rate;
  if (rate>=MVS_VOC_RATE_MAX)
  {
    *frame_rate = VOC_ERASURE;
  }
  else
  {
    *frame_rate = mvs_to_voc_voc_rate[rate];
  }

  #ifdef FEATURE_QDJ_TIMEWARP
  if (mvs_enabled_client == MVS_CLIENT_VOIP)
  {
    /* Pass timewarp factors for processing */
    mvssup_set_timewarp (frame_info.voc_rate.rx_info.timewarp.enable,
                         (EVRC_PCM_FRAME_LENGTH *
                         frame_info.voc_rate.rx_info.timewarp.factor)/100 );    
    #ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
    #endif /* MSMAUD_VOC_4GV_VOIP */
    #ifdef MSMAUD_VOC_4GV_WB_VOIP
    if ( mvs_client_states[mvs_enabled_client].config_mode == MVS_MODE_4GV_WB )
    {
	  mvssup_set_4gv_wb_timewarp (frame_info.voc_rate.rx_info.timewarp.phase_enable,
	                              frame_info.voc_rate.rx_info.timewarp.run_length,
	                              frame_info.voc_rate.rx_info.timewarp.phase_offset);
    }
    #endif /* MSMAUD_VOC_4GV_WB_VOIP */
    
  }
  #endif /* FEATURE_QDJ_TIMEWARP */

  if (mvs_loopback==MVS_LB_DL2UL_VOC)
  {
    (void)mvs_lb_put_pkt(pkt_buf, &frame_info, MVS_VOC_PKT_SIZE);
  }
}

#endif  /* MSMAUD_VOC_IS733 || MSMAUD_VOC_IS127 || MSMAUD_VOC_4GV
|| MSMAUD_VOC_4GV_WB */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_WB
/*===========================================================================

FUNCTION mvs_voc_awb_ul_proc

DESCRIPTION
  Process uplink AMR-WB packets provided by vocoder.  This function is called
in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_awb_ul_proc(
  uint8                   *vocoder_packet,
  voc_amr_wb_frame_type      voc_frame_type,
  voc_amr_wb_frame_rate_type voc_amr_wb_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_pkt_status_type status;
  uint16 pkt_len;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_AMR_WB)
     )
  {
    MSG_ERROR("Invalid MVS client %d for AMR-WB UL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

#ifdef FEATURE_WCDMA
#error code not present
#endif

  if ((mvs_client_states[mvs_enabled_client].ul_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    return;
  }


#ifdef FEATURE_VOC_PCM_INTERFACE
  if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
  {
    voc_frame_type = VOC_AMR_WB_NO_DATA;
  }
#endif  /* FEATURE_VOC_PCM_INTERFACE */


  if ((voc_frame_type<VOC_AMR_WB_SPEECH_GOOD)
      || (voc_frame_type>=VOC_AMR_WB_FRAME_MAX))
  {
    MSG_ERROR("Invalid UL AMR-WB frame type %d", voc_frame_type, 0, 0);
    frame_info.amr_rate.ul_info.frame = MVS_AMR_NO_DATA;
  }
  else
  {
    frame_info.amr_rate.ul_info.frame = VOC_TO_MVS_AWB_FRAME(voc_frame_type);
  }

  if ((voc_amr_wb_mode<VOC_AMR_WB_RATE_0660)
      || (voc_amr_wb_mode>VOC_AMR_WB_RATE_2385))
  {
    MSG_ERROR("Invalid UL AMR-WB frame mode %d", voc_amr_wb_mode, 0, 0);
    frame_info.amr_rate.ul_info.mode = mvssup_get_ul_awb_mode();
  }
  else
  {
    frame_info.amr_rate.ul_info.mode = VOC_TO_MVS_AWB_MODE(voc_amr_wb_mode);
  }


  /* DL2UL loopback in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if (mvs_loopback == MVS_LB_DL2UL_VOC)
    {
      if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info)
          == MVS_PKT_STATUS_SLOW)
      {
        return;
      }

      frame_info.amr_rate.ul_info.frame = lb_frame_info.amr_rate.dl_info.frame;
      frame_info.amr_rate.ul_info.mode = lb_frame_info.amr_rate.dl_info.mode;
    }
  }


  pkt_len = (uint16)mvs_amr_frame_len(frame_info.amr_rate.ul_info.frame,
                                      frame_info.amr_rate.ul_info.mode);
  /* Keep packet length word-alligned to avoid different endians problems */
  pkt_len = ((pkt_len+1) >> 1) << 1;

#ifdef FEATURE_MVS_GEN_SILENCE
  /* Only store SID UPDATE frames */
  if(mvs_client_states[mvs_enabled_client].state != MVS_STATE_SILENCE &&
     mvs_client_states[mvs_enabled_client].state != MVS_STATE_STANDBY_SILENCE) {
    mvs_process_silence(0);
    if((mvssup_amr_get_scr_mode() == MVS_SCR_ENABLE) &&
       (frame_info.amr_rate.ul_info.mode == MVS_AMR_SID_UPDATE)) {
      mvs_put_silence_frame(MVS_MODE_AMR_WB, vocoder_packet, pkt_len);
    }
  }
#endif /* FEATURE_MVS_GEN_SILENCE */

  /* Send the packet */
  if (mvs_client_states[mvs_enabled_client].ul_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].ul_cb(vocoder_packet, &frame_info,
                                               pkt_len, &status);
    if(status != MVS_PKT_STATUS_NORMAL) {
      MSG_MED("Warning: TX packet status %d",status,0,0);
    }
  }
#ifdef FEATURE_WCDMA
#error code not present
#endif


  /* UL2DL loopbacks in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
    {
      (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_AWB_PKT_SIZE);
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_awb_dl_proc

DESCRIPTION
  Retrieve downlink AMR-WB packets and feed them to vocoder.  This function
is called in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_awb_dl_proc(
  uint8                    *vocoder_packet,
  voc_amr_wb_frame_type    *voc_frame_type,
  voc_amr_wb_frame_rate_type  *voc_amr_wb_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_amr_frame_type  frame;
  mvs_amr_mode_type   mode;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* initialise frame_info to prevent lint error */
  frame_info.amr_rate.dl_info.frame = MVS_AMR_NO_DATA;
  frame_info.amr_rate.dl_info.mode = MVS_AMR_MODE_MAX;


  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_AMR_WB)
     )
  {
    MSG_ERROR("Invalid MVS client %d for AMR-WB DL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  if ((mvs_client_states[mvs_enabled_client].dl_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    *voc_frame_type = VOC_AMR_WB_NO_DATA;
    *voc_amr_wb_mode = VOC_AMR_WB_RATE_MAX;
    return;
  }


  if (mvs_client_states[mvs_enabled_client].dl_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].dl_cb(vocoder_packet, &frame_info,
                                                &status);
    if (status == MVS_PKT_STATUS_SLOW)
    {
      /* MVS AMR DSM has also taken care of the condition when there
         are no packets available in the DSM buffers.
         AMR_NO_DATA and last AMR mode will be used.
      */
      frame_info.amr_rate.dl_info.frame = MVS_AMR_NO_DATA;
      frame_info.amr_rate.dl_info.mode = MVS_AMR_MODE_MAX;

      MSG_MED("no AMR DL packets available.", 0, 0, 0);
    }

    if (status == MVS_PKT_STATUS_FAST)
    {
      MSG_MED("lost AMR DL packets.", 0, 0, 0);
    }
  }
#ifdef FEATURE_WCDMA
#error code not present
#endif


  /* UL2DL loopbacks in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
    {
      if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info)
          == MVS_PKT_STATUS_SLOW)
      {
        *voc_frame_type = VOC_AMR_WB_NO_DATA;
        *voc_amr_wb_mode = VOC_AMR_WB_RATE_MAX;
        return;
      }

      frame_info.amr_rate.dl_info.frame = lb_frame_info.amr_rate.ul_info.frame;
      frame_info.amr_rate.dl_info.mode = lb_frame_info.amr_rate.ul_info.mode;
    }
  }


  frame = frame_info.amr_rate.dl_info.frame;
  mode = frame_info.amr_rate.dl_info.mode;

  if (frame>=MVS_AMR_FRAME_TYPE_MAX)
  {
    *voc_frame_type = VOC_AMR_WB_NO_DATA;
  }
  else
  {
    *voc_frame_type = MVS_TO_VOC_AWB_FRAME(frame);
  }

  if (mode>=MVS_AMR_MODE_MAX)
  {
    *voc_amr_wb_mode = VOC_AMR_WB_RATE_MAX;
  }
  else
  {
    *voc_amr_wb_mode = MVS_TO_VOC_AWB_MODE(mode);
  }


  /* DL2UL loopback in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if (mvs_loopback == MVS_LB_DL2UL_VOC)
    {
      (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_AWB_PKT_SIZE);
    }
  }
}

#endif  /* MSMAUD_VOC_AMR_WB */


/* <EJECT> */
#ifdef MSMAUD_VOC_AMR
/*===========================================================================

FUNCTION mvs_voc_amr_ul_proc

DESCRIPTION
  Process uplink AMR packets provided by vocoder.  This function is called
in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_amr_ul_proc(
  uint8                   *vocoder_packet,
  voc_gsm_frame_type      voc_frame_type,
  voc_gsm_frame_rate_type voc_amr_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_pkt_status_type status;
  uint16 pkt_len;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_AMR)
     )
  {
    MSG_ERROR("Invalid MVS client %d for AMR UL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

#ifdef FEATURE_WCDMA
#error code not present
#endif

  if ((mvs_client_states[mvs_enabled_client].ul_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    return;
  }


#ifdef FEATURE_VOC_PCM_INTERFACE
  if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
  {
    voc_frame_type = VOC_AMR_NO_DATA;
  }
#endif  /* FEATURE_VOC_PCM_INTERFACE */


  if ((voc_frame_type<VOC_AMR_SPEECH_GOOD)
      || (voc_frame_type>=VOC_GSM_FRAME_MAX))
  {
    MSG_ERROR("Invalid UL AMR frame type %d", voc_frame_type, 0, 0);
    frame_info.amr_rate.ul_info.frame = MVS_AMR_NO_DATA;
  }
  else
  {
    frame_info.amr_rate.ul_info.frame = VOC_TO_MVS_AMR_FRAME(voc_frame_type);
  }

  if ((voc_amr_mode<VOC_AMR_RATE_475)
      || (voc_amr_mode>VOC_AMR_RATE_1220))
  {
    MSG_ERROR("Invalid UL AMR frame mode %d", voc_amr_mode, 0, 0);
    frame_info.amr_rate.ul_info.mode = mvssup_get_ul_amr_mode();
  }
  else
  {
    frame_info.amr_rate.ul_info.mode = VOC_TO_MVS_AMR_MODE(voc_amr_mode);
  }


  /* DL2UL loopback in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if (mvs_loopback == MVS_LB_DL2UL_VOC)
    {
      if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info)
          == MVS_PKT_STATUS_SLOW)
      {
        return;
      }

      frame_info.amr_rate.ul_info.frame = lb_frame_info.amr_rate.dl_info.frame;
      frame_info.amr_rate.ul_info.mode = lb_frame_info.amr_rate.dl_info.mode;
    }
  }


  pkt_len = (uint16)mvs_amr_frame_len(frame_info.amr_rate.ul_info.frame,
                                      frame_info.amr_rate.ul_info.mode);
  /* Keep packet length word-alligned to avoid different endians problems */
  pkt_len = ((pkt_len+1) >> 1) << 1;

#ifdef FEATURE_MVS_GEN_SILENCE
  /* Only store SID UPDATE frames */
  if(mvs_client_states[mvs_enabled_client].state != MVS_STATE_SILENCE &&
     mvs_client_states[mvs_enabled_client].state != MVS_STATE_STANDBY_SILENCE) {
    mvs_process_silence(0);
    if((mvssup_amr_get_scr_mode() == MVS_SCR_ENABLE) &&
       (frame_info.amr_rate.ul_info.mode == MVS_AMR_SID_UPDATE)) {
      mvs_put_silence_frame(MVS_MODE_AMR, vocoder_packet, pkt_len);
    }
  }
#endif /* FEATURE_MVS_GEN_SILENCE */

  /* Send the packet */
  if (mvs_client_states[mvs_enabled_client].ul_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].ul_cb(vocoder_packet, &frame_info,
                                               pkt_len, &status);
    if(status != MVS_PKT_STATUS_NORMAL) {
      MSG_MED("Warning: TX packet status %d",status,0,0);
    }
  }
#ifdef FEATURE_WCDMA
#error code not present
#endif


  /* UL2DL loopbacks in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
    {
      (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_PKT_SIZE);
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_amr_dl_proc

DESCRIPTION
  Retrieve downlink AMR packets and feed them to vocoder.  This function
is called in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_amr_dl_proc(
  uint8                    *vocoder_packet,
  voc_gsm_frame_type       *voc_frame_type,
  voc_gsm_frame_rate_type  *voc_amr_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_amr_frame_type  frame;
  mvs_amr_mode_type   mode;
  mvs_pkt_status_type status;

  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* initialise frame_info to prevent lint error */
  frame_info.amr_rate.dl_info.frame = MVS_AMR_NO_DATA;
  frame_info.amr_rate.dl_info.mode = MVS_AMR_MODE_MAX;


  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_AMR)
     )
  {
    MSG_ERROR("Invalid MVS client %d for AMR DL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  if ((mvs_client_states[mvs_enabled_client].dl_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    *voc_frame_type = VOC_AMR_NO_DATA;
    *voc_amr_mode = VOC_GSM_RATE_MAX;
    return;
  }


  if (mvs_client_states[mvs_enabled_client].dl_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].dl_cb(vocoder_packet, &frame_info,
                                               &status);
    if (status == MVS_PKT_STATUS_SLOW)
    {
      /* MVS AMR DSM has also taken care of the condition when there
         are no packets available in the DSM buffers.
         AMR_NO_DATA and last AMR mode will be used.
      */
      frame_info.amr_rate.dl_info.frame = MVS_AMR_NO_DATA;
      frame_info.amr_rate.dl_info.mode = MVS_AMR_MODE_MAX;

      MSG_MED("no AMR DL packets available.", 0, 0, 0);
    }

    if (status == MVS_PKT_STATUS_FAST)
    {
      MSG_MED("lost AMR DL packets.", 0, 0, 0);
    }
  }
#ifdef FEATURE_WCDMA
#error code not present
#endif


  /* UL2DL loopbacks in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
    {
      if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info)
          == MVS_PKT_STATUS_SLOW)
      {
        *voc_frame_type = VOC_AMR_NO_DATA;
        *voc_amr_mode = VOC_GSM_RATE_MAX;
        return;
      }

      frame_info.amr_rate.dl_info.frame = lb_frame_info.amr_rate.ul_info.frame;
      frame_info.amr_rate.dl_info.mode = lb_frame_info.amr_rate.ul_info.mode;
    }
  }


  frame = frame_info.amr_rate.dl_info.frame;
  mode = frame_info.amr_rate.dl_info.mode;

  if (frame>=MVS_AMR_FRAME_TYPE_MAX)
  {
    *voc_frame_type = VOC_AMR_NO_DATA;
  }
  else
  {
    *voc_frame_type = MVS_TO_VOC_AMR_FRAME(frame);
  }

  if (mode>=MVS_AMR_MODE_MAX)
  {
    *voc_amr_mode = VOC_GSM_RATE_MAX;
  }
  else
  {
    *voc_amr_mode = MVS_TO_VOC_AMR_MODE(mode);
  }


  /* DL2UL loopback in WCDMA mode is handled by AMR DSM manager */
#ifdef FEATURE_WCDMA
#error code not present
#endif
  {
    if (mvs_loopback == MVS_LB_DL2UL_VOC)
    {
      (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_PKT_SIZE);
    }
  }
}

#endif  /* MSMAUD_VOC_AMR */


/* <EJECT> */

#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif  /* MSMAUD_VOC_EFR || MSMAUD_VOC_FR */

/* <EJECT> */
#ifdef MSMAUD_VOC_HR
/*===========================================================================

FUNCTION mvs_voc_hr_ul_proc

DESCRIPTION
  Process uplink HR packets provided by vocoder.  This function is called
in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_hr_ul_proc(
  uint8                   *vocoder_packet,
  voc_gsm_frame_type      voc_frame_type,
  boolean                 enc_err_flag
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_HR)
     )
  {
    MSG_ERROR("Invalid MVS client %d for HR UL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }


  if ((mvs_client_states[mvs_enabled_client].ul_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    return;
  }


#ifdef FEATURE_VOC_PCM_INTERFACE
  if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
  {
    voc_frame_type = VOC_GSM_INVALID_SID;
    enc_err_flag = TRUE;
  }
#endif


  if ((voc_frame_type<VOC_GSM_SID) || (voc_frame_type>VOC_GSM_INVALID_SID))
  {
    MSG_ERROR("Invalid UL HR frame type %d", voc_frame_type, 0, 0);
    frame_info.hr_rate.ul_info.frame = MVS_HR_SPEECH_BAD;
    frame_info.hr_rate.ul_info.enc_err_flag = TRUE;
  }
  else
  {
    frame_info.hr_rate.ul_info.frame = voc_to_mvs_hr_frame[voc_frame_type];
    frame_info.hr_rate.ul_info.enc_err_flag = enc_err_flag;
  }

  if (mvs_loopback == MVS_LB_DL2UL_VOC)
  {
    if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      return;
    }

    frame_info.hr_rate.ul_info.frame = lb_frame_info.hr_rate.dl_info.frame;
    frame_info.hr_rate.ul_info.enc_err_flag = lb_frame_info.hr_rate.dl_info.bfi;
  }

#ifdef FEATURE_MVS_GEN_SILENCE
  if(mvs_client_states[mvs_enabled_client].state != MVS_STATE_SILENCE &&
     mvs_client_states[mvs_enabled_client].state != MVS_STATE_STANDBY_SILENCE) {
    mvs_process_silence(0);
    /* Only store SID frames */
    if((mvssup_gsm_get_dtx_mode() == MVS_DTX_ON) &&
       (frame_info.hr_rate.ul_info.frame == MVS_GSM_SID)) {
      mvs_put_silence_frame(MVS_MODE_HR, vocoder_packet, MVS_VOC_PKT_SIZE);
    }
  }
#endif /* FEATURE_MVS_GEN_SILENCE */

  if (mvs_client_states[mvs_enabled_client].ul_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].ul_cb(vocoder_packet, &frame_info,
                                             MVS_VOC_PKT_SIZE, &status);
    if(status != MVS_PKT_STATUS_NORMAL) {
      MSG_MED("Warning: TX packet status %d",status,0,0);
    }
  }


  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_PKT_SIZE);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_hr_dl_proc

DESCRIPTION
  Retrieve downlink HR packets and feed them to vocoder.  This function
is called in ADSP or VDSP interrupt callback sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_hr_dl_proc(
  uint8                    *vocoder_packet,
  voc_gsm_frame_type       *voc_frame_type,
  boolean                  *taf,
  boolean                  *bfi,
  boolean                  *ufi
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_hr_frame_type frame;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* initialize frame_info to prevent lint errors */
  frame_info.hr_rate.dl_info.frame = MVS_HR_FRAME_UNDEF;
  frame_info.hr_rate.dl_info.bfi = FALSE;
  frame_info.hr_rate.dl_info.taf = FALSE;
  frame_info.hr_rate.dl_info.ufi = FALSE;

  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_HR)
     )
  {
    MSG_ERROR("Invalid MVS client %d for HR DL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }


  if ((mvs_client_states[mvs_enabled_client].dl_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    *voc_frame_type = VOC_GSM_BFI;
    *taf = FALSE;
    *bfi = TRUE;
    *ufi = FALSE;
    return;
  }


  if (mvs_client_states[mvs_enabled_client].dl_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].dl_cb(vocoder_packet, &frame_info,
                                               &status);
    if (status == MVS_PKT_STATUS_SLOW)
    {
      mvs_prand(vocoder_packet, MVS_VOC_PKT_SIZE);
      frame_info.hr_rate.dl_info.frame = MVS_HR_SPEECH_BAD;
      frame_info.hr_rate.dl_info.taf = FALSE;
      frame_info.hr_rate.dl_info.bfi = TRUE;
      frame_info.hr_rate.dl_info.ufi = FALSE;

      MSG_MED("no HR DL packets available.", 0, 0, 0);
    }

    if (status == MVS_PKT_STATUS_FAST)
    {
      MSG_MED("lost HR DL packets.", 0, 0, 0);
    }
  }

  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      *voc_frame_type = VOC_GSM_BFI;
      *taf = FALSE;
      *bfi = TRUE;
      *ufi = FALSE;
      return;
    }

    frame_info.hr_rate.dl_info.frame = lb_frame_info.hr_rate.ul_info.frame;
    frame_info.hr_rate.dl_info.taf = FALSE;
    frame_info.hr_rate.dl_info.bfi = lb_frame_info.hr_rate.ul_info.enc_err_flag;
    frame_info.hr_rate.dl_info.ufi = FALSE;
  }

  frame = frame_info.hr_rate.dl_info.frame;
  if (frame>=MVS_HR_FRAME_MAX)
  {
    *voc_frame_type = VOC_GSM_BFI;
  }
  else
  {
    *voc_frame_type = mvs_to_voc_hr_frame[frame];
  }

  *taf = frame_info.hr_rate.dl_info.taf;
  *bfi = frame_info.hr_rate.dl_info.bfi;
  *ufi = frame_info.hr_rate.dl_info.ufi;

  if (mvs_loopback==MVS_LB_DL2UL_VOC)
  {
    (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_PKT_SIZE);
  }
}


#endif  /* MSMAUD_VOC_HR */

/* <EJECT> */
#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION mvs_voc_g711_ul_proc

DESCRIPTION
  Process uplink G.711 packets provided by vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_g711_ul_proc(
  uint8                   *vocoder_packet,
  uint16                  frame_length,
  voc_g711_mode_type      voc_g711_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* Check current state */
  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_G711)
     )
  {
    MSG_ERROR("Invalid MVS client %d for G711 UL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  /* Check for registered callback or loopback enabled */
  if ((mvs_client_states[mvs_enabled_client].ul_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    return;
  }

  /* Validate mode */
  if ((voc_g711_mode < VOC_G711_MODE_MULAW) ||
      (voc_g711_mode > VOC_G711_MODE_ALAW))
  {
    MSG_ERROR("Invalid UL G711 mode %d", voc_g711_mode, 0, 0);
    /* Unknown mode, blank frame */
    frame_info.g711_rate.ul_info.g711_mode = MVS_G711_MODE_MULAW;
    memset(vocoder_packet, 0xFF, frame_length);
  } else {
    frame_info.g711_rate.ul_info.g711_mode = voc_to_mvs_g711_mode[voc_g711_mode];
  }

  if(frame_length != MVS_VOC_G711_PKT_SIZE) {
    MSG_ERROR("Unexpected G711 frame length %d",frame_length,0,0);
    frame_length = MVS_VOC_G711_PKT_SIZE;
  }

  /* Blank if PCM is enabled */
#ifdef FEATURE_VOC_PCM_INTERFACE
  if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
  {
    if(frame_info.g711_rate.ul_info.g711_mode == MVS_G711_MODE_MULAW) {
      memset(vocoder_packet, 0xFF, frame_length);
    } else {
      memset(vocoder_packet, 0xD5, frame_length);
    }
  }
#endif

  if (mvs_loopback == MVS_LB_DL2UL_VOC)
  {
    if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      return;
    }

    frame_info.g711_rate.ul_info.g711_mode =
                                      lb_frame_info.g711_rate.dl_info.g711_mode;
  }

  if (mvs_client_states[mvs_enabled_client].ul_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].ul_cb(vocoder_packet, &frame_info,
                                                frame_length, &status);
    if(status != MVS_PKT_STATUS_NORMAL) {
      MSG_MED("Warning: TX packet status %d",status,0,0);
    }
  }

  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, frame_length);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_g711_dl_proc

DESCRIPTION
  Retrieve downlink G.711 packets and feed them to vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_g711_dl_proc(
  uint8                    *vocoder_packet,
  uint16                   *frame_length,
  voc_g711_mode_type       *voc_g711_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_g711_mode_type  mvs_g711_mode;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* initialise frame_info to prevent lint error */
  frame_info.g711_rate.dl_info.g711_mode = MVS_G711_MODE_MAX;

  /* Check current state */
  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_G711)
     )
  {
    MSG_ERROR("Invalid MVS client %d for G711 DL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  /* Check for registered callback or loopback enabled */
  if ((mvs_client_states[mvs_enabled_client].dl_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    /* Nothing enabled, blank frame */
    *frame_length = MVS_VOC_G711_PKT_SIZE;
    *voc_g711_mode = VOC_G711_MODE_MULAW;
    memset(vocoder_packet, 0xFF, MVS_VOC_G711_PKT_SIZE);
    return;
  }

  /* Is MVS enabled? */
  if (mvs_client_states[mvs_enabled_client].dl_cb != NULL)
  {
    /* Grab a packet */
    mvs_client_states[mvs_enabled_client].dl_cb(vocoder_packet, &frame_info,
                                                &status);
    if (status == MVS_PKT_STATUS_SLOW) {
      /* Too slow, blank frame */
      *frame_length = MVS_VOC_G711_PKT_SIZE;
      frame_info.g711_rate.dl_info.g711_mode = MVS_G711_MODE_MULAW;
      memset(vocoder_packet, 0xFF, MVS_VOC_G711_PKT_SIZE);

      MSG_MED("no G711 DL packets available.", 0, 0, 0);
    }

    if (status == MVS_PKT_STATUS_FAST) {
      MSG_MED("lost G711 DL packets.", 0, 0, 0);
    }
  }

  /* Are loopbacks enabled? */
  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      /* Too slow, blank frame */
      *frame_length = MVS_VOC_G711_PKT_SIZE;
      *voc_g711_mode = VOC_G711_MODE_MULAW;
      memset(vocoder_packet, 0xFF, MVS_VOC_G711_PKT_SIZE);
      return;
    }

    frame_info.g711_rate.dl_info.g711_mode =
                                      lb_frame_info.g711_rate.ul_info.g711_mode;
  }

  /* Set frame length */
  *frame_length = MVS_VOC_G711_PKT_SIZE;

  /* Check and set G711 mode */
  mvs_g711_mode = frame_info.g711_rate.dl_info.g711_mode;

  /*lint -e568*/
  if ((MVS_G711_MODE_MULAW <= mvs_g711_mode) &&
      (mvs_g711_mode <= MVS_G711_MODE_ALAW))
  /*lint +e568*/
  {
    *voc_g711_mode = mvs_to_voc_g711_mode[mvs_g711_mode];
  }
  else
  {
    MSG_ERROR("Invalid DL G711 mode %d", mvs_g711_mode, 0, 0);
    *voc_g711_mode = VOC_G711_MODE_MULAW;
    /* Blank frame */
    memset(vocoder_packet, 0xFF, MVS_VOC_G711_PKT_SIZE);
  }

  if (mvs_loopback==MVS_LB_DL2UL_VOC)
  {
    (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_G711_PKT_SIZE);
  }
}
#endif  /* MSMAUD_VOC_G711 */

/* <EJECT> */
#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION mvs_voc_g723_ul_proc

DESCRIPTION
  Process uplink G.723.1 packets provided by vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_g723_ul_proc(
  uint8                   *vocoder_packet,
  uint16                  frame_length,
  voc_g723_mode_type      voc_g723_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* Check current state */
  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_G723)
     )
  {
    MSG_ERROR("Invalid MVS client %d for G723 UL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  /* Check for registered callback or loopback enabled */
  if ((mvs_client_states[mvs_enabled_client].ul_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    return;
  }

  /* Validate mode */
  if ((voc_g723_mode < VOC_G723_MODE_HIGH) ||
      (voc_g723_mode >= VOC_G723_MODE_MAX))
  {
    MSG_ERROR("Invalid UL G723 mode %d", voc_g723_mode, 0, 0);
    /* Unknown mode, blank frame */
    frame_info.g723_rate.ul_info.g723_mode = MVS_G723_MODE_BLANK;
    memset(vocoder_packet, 0x00, frame_length);
    vocoder_packet[0] = 0x03;
  } else {
    frame_info.g723_rate.ul_info.g723_mode = voc_to_mvs_g723_mode[voc_g723_mode];
  }

  if(frame_length > MVS_VOC_G723_PKT_SIZE) {
    MSG_ERROR("Unexpected G723 frame length %d",frame_length,0,0);
    frame_length = MVS_VOC_G723_PKT_SIZE;
  }

  /* Blank if PCM is enabled */
#ifdef FEATURE_VOC_PCM_INTERFACE
  if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
  {
    frame_info.g723_rate.ul_info.g723_mode = MVS_G723_MODE_BLANK;
    memset(vocoder_packet, 0x00, frame_length);
    vocoder_packet[0] = 0x03;
  }
#endif

  if (mvs_loopback == MVS_LB_DL2UL_VOC)
  {
    if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      return;
    }

    frame_info.g723_rate.ul_info.g723_mode =
                                      lb_frame_info.g723_rate.dl_info.g723_mode;
  }

  if (mvs_client_states[mvs_enabled_client].ul_cb != NULL)
  {
    mvs_client_states[mvs_enabled_client].ul_cb(vocoder_packet, &frame_info,
                                                frame_length, &status);
    if(status != MVS_PKT_STATUS_NORMAL) {
      MSG_MED("Warning: TX packet status %d",status,0,0);
    }
  }

  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, frame_length);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_g723_dl_proc

DESCRIPTION
  Retrieve downlink G.723.1 packets and feed them to vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_g723_dl_proc(
  uint8                    *vocoder_packet,
  uint16                   *frame_length,
  voc_g723_mode_type       *voc_g723_mode
)
{
  mvs_frame_info_type frame_info;
  mvs_frame_info_type lb_frame_info;
  mvs_g723_mode_type  mvs_g723_mode;
  mvs_pkt_status_type status;


  frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;
  lb_frame_info.hdr.mode = mvs_client_states[mvs_enabled_client].config_mode;

  /* Check current state */
  if ((mvs_enabled_client >= MVS_CLIENT_MAX)
      || (mvs_client_states[mvs_enabled_client].state == MVS_STATE_DISABLE)
      || (mvs_client_states[mvs_enabled_client].config_mode != MVS_MODE_G723)
     )
  {
    MSG_ERROR("Invalid MVS client %d for G723 DL processing.",
              mvs_enabled_client, 0, 0);
    return;
  }

  /* Check for registered callback or loopback enabled */
  if ((mvs_client_states[mvs_enabled_client].dl_cb == NULL)
      && (mvs_loopback != MVS_LB_UL2DL_VOC)
      && (mvs_loopback != MVS_LB_UL2DL_DSM))
  {
    /* Nothing enabled, blank frame */
    *frame_length = 1;
    *voc_g723_mode = VOC_G723_MODE_BLANK;
    memset(vocoder_packet, 0x00, MVS_VOC_G723_PKT_SIZE);
    vocoder_packet[0] = 0x03;
    return;
  }

  /* Is MVS enabled? */
  if (mvs_client_states[mvs_enabled_client].dl_cb != NULL)
  {
    /* Grab a packet */
    mvs_client_states[mvs_enabled_client].dl_cb(vocoder_packet, &frame_info,
                                                &status);
    if (status == MVS_PKT_STATUS_SLOW) {
      /* Too slow, blank frame */
      frame_info.g723_rate.dl_info.g723_mode = MVS_G723_MODE_ERROR;

      MSG_MED("no G723 DL packets available.", 0, 0, 0);
    }

    if (status == MVS_PKT_STATUS_FAST) {
      MSG_MED("lost G723 DL packets.", 0, 0, 0);
    }
  }

  /* Are loopbacks enabled? */
  if ((mvs_loopback==MVS_LB_UL2DL_VOC) || (mvs_loopback==MVS_LB_UL2DL_DSM))
  {
    if (mvs_lb_get_pkt(vocoder_packet, &lb_frame_info) == MVS_PKT_STATUS_SLOW)
    {
      /* Too slow, blank frame */
      *frame_length = 0;
      *voc_g723_mode = VOC_G723_MODE_ERROR;
      return;
    }

    frame_info.g723_rate.dl_info.g723_mode =
                                      lb_frame_info.g723_rate.ul_info.g723_mode;
  }

  /* Check and set G723 mode */
  mvs_g723_mode = frame_info.g723_rate.dl_info.g723_mode;
  if ((mvs_g723_mode < MVS_G723_MODE_HIGH) ||
      (mvs_g723_mode >= MVS_G723_MODE_MAX))
  {
    MSG_ERROR("Invalid DL G723 mode %d", mvs_g723_mode, 0, 0);
    *frame_length = 1;
    *voc_g723_mode = VOC_G723_MODE_BLANK;
    /* Blank frame */
    memset(vocoder_packet, 0x00, MVS_VOC_G723_PKT_SIZE);
    vocoder_packet[0] = 0x03;
  } else {
    /* Convert mode */
    *voc_g723_mode = mvs_to_voc_g723_mode[mvs_g723_mode];
    /* Set frame length */
    *frame_length = mvs_g723_frame_len_table[mvs_g723_mode];
  }

  if (mvs_loopback==MVS_LB_DL2UL_VOC)
  {
    (void)mvs_lb_put_pkt(vocoder_packet, &frame_info, MVS_VOC_G723_PKT_SIZE);
  }
}
#endif  /* MSMAUD_VOC_G723 */

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_register_isrcb_chain

DESCRIPTION
  This function will tie or untie the corresponding callback function, which
is determined based on which configuration mode, with or from the ADSP or
VDSP ISR callback chain.
  Activate or de-activate MVS DSM manager if necessary, based on configuration
mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvs_register_isrcb_chain(
  mvs_mode_type mode,
  boolean enable
)
{
  switch (mode)
  {
#if defined(MSMAUD_VOC_IS733) || defined(MSMAUD_VOC_IS127) \
    || defined(MSMAUD_VOC_4GV)  || defined(MSMAUD_VOC_4GV_WB)
#ifdef MSMAUD_VOC_IS733
    case MVS_MODE_IS733:
#endif
#ifdef MSMAUD_VOC_IS127
    case MVS_MODE_IS127:
#endif
#ifdef MSMAUD_VOC_4GV
    case MVS_MODE_4GV_NB:
#endif
#ifdef MSMAUD_VOC_4GV_WB
    case MVS_MODE_4GV_WB:
#endif
#ifdef FEATURE_VOC_PACKET_INTERFACE
      if (enable==TRUE)
      {
        mvssup_voc_set_frame_rate(mvssup_get_voc_max_rate(),
                                  mvssup_get_voc_min_rate());

        voc_register_packet_clients(mvs_voc_evrc_ul_proc, mvs_voc_evrc_dl_proc);
      }
      else
      {
        voc_register_packet_clients(NULL, NULL);
      }
#else
      MSG_ERROR("Incomplete support for MVS mode %d.", mode, 0, 0);
#endif  /* FEATURE_VOC_PACKET_INTERFACE */
      break;
#endif  /* MSMAUD_VOC_IS733 || MSMAUD_VOC_IS127 || MSMAUD_VOC_4GV
|| MSMAUD_VOC_4GV_WB */

#ifdef MSMAUD_VOC_AMR_WB
    case MVS_MODE_AMR_WB:
      if (enable==TRUE)
      {
        mvssup_amr_set_awb_mode( mvssup_get_ul_awb_mode() );
#ifdef FEATURE_WCDMA
#error code not present
#endif
        voc_amr_wb_register(mvs_voc_awb_ul_proc, mvs_voc_awb_dl_proc);
      }
      else
      {
        voc_amr_wb_register(NULL, NULL);
#ifdef FEATURE_WCDMA
#error code not present
#endif
      }
      break;
#endif  /* MSMAUD_VOC_AMR_WB */

#ifdef MSMAUD_VOC_AMR
    case MVS_MODE_AMR:
      if (enable==TRUE)
      {
        mvssup_amr_set_amr_mode( mvssup_get_ul_amr_mode() );
#ifdef FEATURE_WCDMA
#error code not present
#endif
        voc_amr_register(mvs_voc_amr_ul_proc, mvs_voc_amr_dl_proc);
      }
      else
      {
        voc_amr_register(NULL, NULL);
#ifdef FEATURE_WCDMA
#error code not present
#endif
      }
      break;
#endif  /* MSMAUD_VOC_AMR */

#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif  /* MSMAUD_VOC_EFR || MSMAUD_VOC_FR */

#ifdef MSMAUD_VOC_HR
    case MVS_MODE_HR:
      if (enable==TRUE)
      {
        voc_hr_register(mvs_voc_hr_ul_proc, mvs_voc_hr_dl_proc);
      }
      else
      {
        voc_hr_register(NULL, NULL);
      }
      break;
#endif  /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
    case MVS_MODE_G711:
      if (enable==TRUE) {
        mvssup_set_g711_mode( mvssup_get_g711_mode() );
        voc_g711_register(mvs_voc_g711_ul_proc, mvs_voc_g711_dl_proc);
      } else {
        voc_g711_register(NULL, NULL);
      }
      break;
#endif  /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
    case MVS_MODE_G723:
      if (enable==TRUE) {
        mvssup_set_g723_mode( mvssup_get_g723_mode() );
        voc_g723_register(mvs_voc_g723_ul_proc, mvs_voc_g723_dl_proc);
      } else {
        voc_g723_register(NULL, NULL);
      }
      break;
#endif  /* MSMAUD_VOC_G723 */

#ifdef FEATURE_VOC_PCM_INTERFACE
    case MVS_MODE_LINEAR_PCM:
      if (enable==TRUE)
      {
        (void)voc_register_pcm_output_client(VOC_PCM_INTERFACE_TX_OUTPUT,
                                             mvs_voc_pcm_ul_proc);
        (void)voc_register_pcm_input_client(VOC_PCM_INTERFACE_RX_INPUT,
                                            mvs_voc_pcm_dl_proc);
      }
      else
      {
        (void)voc_register_pcm_output_client(VOC_PCM_INTERFACE_TX_OUTPUT, NULL);
        (void)voc_register_pcm_input_client(VOC_PCM_INTERFACE_RX_INPUT, NULL);
      }
      break;
#endif  /* FEATURE_VOC_PCM_INTERFACE */

    default:
      break;
  }  /* end switch */
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

FUNCTION mvs_pcm_inform_callback

DESCRIPTION
  Processes voc_inform callback and updates MVS accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_pcm_inform_callback(
  voc_inform_type status
)
{
  mvs_client_state_type *state;


  if (mvs_pcm_enabled_client == MVS_CLIENT_NONE)
  {
    return;
  }


  state = &(mvs_client_states[mvs_pcm_enabled_client]);

  MSG_ERROR("PCM: Inform %d, PCM state %d, state %d",status,state->pcm_state,state->state);

  /* Update MVS states for PCM mode */
  switch (status)
  {
    case VOC_INFORM_RESET:
    default:
      mvs_register_isrcb_chain(MVS_MODE_LINEAR_PCM, FALSE);
      state->pcm_context = MVS_PKT_CONTEXT_NONE;

      /* Clean up frame queues */
      mvssup_frame_q_empty(&mvs_ul_pcm_frame_q);
      mvssup_frame_q_empty(&mvs_dl_pcm_frame_q);

      if ((mvs_loopback == MVS_LB_UL2DL_PCM)
          || (mvs_loopback == MVS_LB_DL2UL_PCM))
      {
        mvssup_frame_q_empty(&mvs_lb_frame_q);
      }

      /* other vocoder clients may have pre-empted MVS PCM control */
      if (state->pcm_state == MVS_STATE_ENABLE
#ifdef FEATURE_MVS_GEN_SILENCE
          || state->pcm_state == MVS_STATE_SILENCE
#endif /* FEATURE_MVS_GEN_SILENCE */
          )
      {
#ifdef FEATURE_MVS_GEN_SILENCE
        state->pcm_state = MVS_STATE_SILENCE;
#else
        state->pcm_state = MVS_STATE_DISABLE;

        mvs_send_mode_event(mvs_pcm_enabled_client, MVS_MODE_LINEAR_PCM,
                            MVS_MODE_NOT_AVAIL);
#endif /* FEATURE_MVS_GEN_SILENCE */
      }

      break;

    case VOC_INFORM_IDLE:
    case VOC_INFORM_VOCODER:
#ifdef FEATURE_AVS_INCALL_ADEC_PB
    case VOC_INFORM_VOCODER_ADEC:
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    case VOC_INFORM_MIXER_ACTIVE:
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
      if ((state->desired_pcm_ul_cb != state->pcm_ul_cb)
          || (state->desired_pcm_dl_cb != state->pcm_dl_cb)
          || (state->desired_pcm_context != state->pcm_context)
          || ((state->state != MVS_STATE_ENABLE)
#ifdef FEATURE_MVS_GEN_SILENCE
              && (state->state != MVS_STATE_SILENCE)
#endif /* FEATURE_MVS_GEN_SILENCE */
             ))
      {
        state->pcm_context = state->desired_pcm_context;

        switch (state->pcm_context)
        {
          case MVS_PKT_CONTEXT_ISR :
            state->pcm_ul_cb = state->desired_pcm_ul_cb;
            state->pcm_dl_cb = state->desired_pcm_dl_cb;
            break;

          case MVS_PKT_CONTEXT_NONE :
          case MVS_PKT_CONTEXT_MAX :
            state->pcm_ul_cb = NULL;
            state->pcm_dl_cb = NULL;
            break;

          default :
            if (state->desired_pcm_ul_cb == NULL)
            {
              state->pcm_ul_cb = NULL;
            }
            else
            {
              state->pcm_ul_cb = mvs_pcm_ul_isr_cb_tbl[state->pcm_context];
            }

            if (state->desired_pcm_dl_cb == NULL)
            {
              state->pcm_dl_cb = NULL;
            }
            else
            {
              state->pcm_dl_cb = mvs_pcm_dl_isr_cb_tbl[state->pcm_context];

              /* frames may be available before 1st decoder callback */
              mvs_rx_processing(mvs_pcm_enabled_client, &mvs_dl_pcm_frame_q,
                                state->desired_pcm_dl_cb);
            }
            break;
        }

        state->pcm_state = MVS_STATE_ENABLE;

        mvs_register_isrcb_chain(MVS_MODE_LINEAR_PCM, TRUE);

        mvs_send_mode_event(mvs_pcm_enabled_client, MVS_MODE_LINEAR_PCM,
                            MVS_MODE_READY);
      }

      break;
  }
}

#endif  /* FEATURE_VOC_PCM_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_voc_inform_callback

DESCRIPTION
  Processes voc_inform callback and updates MVS accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_voc_inform_callback(
  voc_inform_type status
)
{
  mvs_client_state_type *state;


  /* Update MVS states for PCM mode */
#ifdef FEATURE_VOC_PCM_INTERFACE
  if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
  {
    mvs_pcm_inform_callback(status);
  }
#endif  /* FEATURE_VOC_PCM_INTERFACE */


  if (mvs_enabled_client == MVS_CLIENT_NONE)
  {
    return;
  }


  state = &(mvs_client_states[mvs_enabled_client]);

  MSG_ERROR("VOC: Inform %d, state %d, config %d",status,state->state,voc_get_curr_config());

  /* Update MVS states for non-PCM modes */
  switch (status) {
    case VOC_INFORM_RESET:
    case VOC_INFORM_IDLE:
    default:

      /* other vocoder clients may have pre-empted MVS control of vocoder */
      if (state->state == MVS_STATE_ENABLE
#ifdef FEATURE_MVS_GEN_SILENCE
          || state->state == MVS_STATE_SILENCE
#endif /* FEATURE_MVS_GEN_SILENCE */
         )
      {
#ifdef FEATURE_MVS_GEN_SILENCE
        state->state = MVS_STATE_SILENCE;
        mvs_register_isrcb_chain(state->config_mode, FALSE);
#else
        mvs_register_isrcb_chain(state->config_mode, FALSE);
             state->state = MVS_STATE_DISABLE;

        mvs_send_mode_event(mvs_enabled_client, state->desired_config_mode,
                            MVS_MODE_NOT_AVAIL);
#endif /* FEATURE_MVS_GEN_SILENCE */

      } else {
        mvs_register_isrcb_chain(state->config_mode, FALSE);
        state->config_mode = MVS_MODE_NONE;

        /* Clean up frame queues */
        mvssup_frame_q_empty(&mvs_ul_frame_q);
        mvssup_frame_q_empty(&mvs_dl_frame_q);

        if ((mvs_loopback != MVS_LB_UL2DL_PCM)
            && (mvs_loopback != MVS_LB_DL2UL_PCM))
        {
          mvssup_frame_q_empty(&mvs_lb_frame_q);
        }
      }

      break;

    case VOC_INFORM_VOCODER:
#ifdef FEATURE_AVS_INCALL_ADEC_PB
    case VOC_INFORM_VOCODER_ADEC:
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    case VOC_INFORM_MIXER_ACTIVE:
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
      if ((voc_get_curr_config() == mvs_to_voc_mode[state->desired_config_mode])
#if defined(MSMAUD_VOC_IS127_VOIP)
          || ( (mvs_enabled_client == MVS_CLIENT_VOIP) &&
               (state->desired_config_mode == MVS_MODE_IS127) &&
               (voc_get_curr_config () == VOC_CAP_IS127_VOIP) )
#endif /* defined(MSMAUD_VOC_IS127_VOIP) */
#if defined(MSMAUD_VOC_4GV_VOIP)
#error code not present
#endif /* defined(MSMAUD_VOC_4GV_VOIP) */
#if defined(MSMAUD_VOC_4GV_WB_VOIP)
          || ( (mvs_enabled_client == MVS_CLIENT_VOIP) &&
               (state->desired_config_mode == MVS_MODE_4GV_WB) &&
               (voc_get_curr_config () == VOC_CAP_4GV_WB_VOIP)  )
#endif /* defined(MSMAUD_VOC_4GV_WB_VOIP) */
#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
          || (voc_get_desired_config (VOC_ACQ_MVS) ==
              mvs_to_voc_mode[state->desired_config_mode])
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB*/
          )
      {
        if ((state->desired_config_mode != state->config_mode)
            || (state->desired_ul_cb != state->ul_cb)
            || (state->desired_dl_cb != state->dl_cb)
            || (state->desired_context != state->context)
            || ((state->state != MVS_STATE_ENABLE)
#ifdef FEATURE_MVS_GEN_SILENCE
                && (state->state != MVS_STATE_SILENCE)
#endif /* FEATURE_MVS_GEN_SILENCE */
               )
           )
        {
#ifdef FEATURE_MVS_GEN_SILENCE
          /* Deregister bailout timer */
          
          #ifdef FEATURE_CMI
#error code not present
	#else
          clk_dereg(&mvs_voc_clk_cb);
	#endif
#endif /* FEATURE_MVS_GEN_SILENCE */

          /* config_mode can only be MVS_MODE_NONE or desired_config_mode */
          state->config_mode = state->desired_config_mode;
          state->context = state->desired_context;

          switch (state->context)
          {
            case MVS_PKT_CONTEXT_ISR :
              state->ul_cb = state->desired_ul_cb;
              state->dl_cb = state->desired_dl_cb;
              break;

            case MVS_PKT_CONTEXT_NONE :
            case MVS_PKT_CONTEXT_MAX :
              state->ul_cb = NULL;
              state->dl_cb = NULL;
              break;

            default :
              if (state->desired_ul_cb == NULL)
              {
                state->ul_cb = NULL;
              }
              else
              {
                state->ul_cb = mvs_ul_isr_cb_tbl[state->context];
              }

              if (state->desired_dl_cb == NULL)
              {
                state->dl_cb = NULL;
              }
              else
              {
                state->dl_cb = mvs_dl_isr_cb_tbl[state->context];

                /* frames may be available before 1st decoder callback */
                mvs_rx_processing(mvs_enabled_client, &mvs_dl_frame_q,
                                  state->desired_dl_cb);
              }
              break;
          }

          state->state = MVS_STATE_ENABLE;

          mvs_register_isrcb_chain(state->desired_config_mode, TRUE);

          mvs_send_mode_event(mvs_enabled_client, state->desired_config_mode,
                              MVS_MODE_READY);

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
        }
#ifdef FEATURE_MVS_GEN_SILENCE
        else {
          /* Recover from silence mode */
          if(state->state == MVS_STATE_SILENCE) {
            state->state = MVS_STATE_ENABLE;
            mvs_register_isrcb_chain(state->desired_config_mode, TRUE);
          }
        }
#endif /* FEATURE_MVS_GEN_SILENCE */
      }
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_release

DESCRIPTION
  This function disables an MVS client, shutting down the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvs_release(
  mvs_client_type client
)
{
  mvs_packet_type  *packet;
  mvs_event_type   event;

  MSG_MED("mvs_release(): client %d", client, 0, 0);

  /* Set up event callback */
  event.cmd.hdr.client = client;
  event.cmd.hdr.event  = MVS_EV_COMMAND;
  event.cmd.cmd_status = MVS_CMD_FAILURE;

  if (client < MVS_CLIENT_MAX) {
      /* Get packet, put on MVS queue */
      packet = mvs_get_pkt();

      if (packet != NULL) {
        packet->hdr.cmd        = MVS_CMD_RELEASE;
        packet->release.client = client;

        event.cmd.cmd_status = MVS_CMD_SUCCESS;
        mvs_cmd(packet);

#ifdef FEATURE_VOC_PCM_INTERFACE
        if (client == mvs_pcm_enabled_client)
        {
          mvs_register_isrcb_chain(MVS_MODE_LINEAR_PCM, FALSE);
        }
#endif

        if (client == mvs_enabled_client)
        {
#ifdef FEATURE_MVS_GEN_SILENCE
          /* Deregister potential interrupt */
          
         
          #ifdef FEATURE_CMI
#error code not present
	#else
          clk_dereg(&mvs_silence_clk_cb);
          clk_dereg(&mvs_voc_clk_cb);
	#endif

#endif /* FEATURE_MVS_GEN_SILENCE */
          mvs_register_isrcb_chain(mvs_client_states[client].config_mode,
                                   FALSE);
        }
      }  /* if packet != NULL */
  } else {
    MSG_ERROR("Invalid client %d", client, 0, 0);
    return;
  }

  if (mvs_client_states[client].event_cb != NULL) {
    mvs_client_states[client].event_cb(&event);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_release_processing

DESCRIPTION
  This function disables an MVS client, shutting down the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_release_processing(
  mvs_client_type client
)
{
  MSG_MED("mvs_release_processing(): client %d", client, 0, 0);

  if(client >= MVS_CLIENT_MAX)
  {
	MSG_ERROR("Invalid MVS Client : %d",client,0,0);
	return;
  }

  /* Release AVS client for PCM mode if enabled */
#ifdef FEATURE_VOC_PCM_INTERFACE
  if (client == mvs_pcm_enabled_client)
  {
    mvs_register_isrcb_chain(MVS_MODE_LINEAR_PCM, FALSE);
    mvs_pcm_enabled_client = MVS_CLIENT_NONE;

    /* Clean frame queues */
    mvssup_frame_q_empty(&mvs_ul_pcm_frame_q);
    mvssup_frame_q_empty(&mvs_dl_pcm_frame_q);
    if ((mvs_loopback == MVS_LB_UL2DL_PCM)
        || (mvs_loopback == MVS_LB_DL2UL_PCM))
    {
      mvssup_frame_q_empty(&mvs_lb_frame_q);
    }
  }
#endif


  /* Release AVS client for vocoder mode if enabled */
  if (client == mvs_enabled_client)
  {
    mvs_register_isrcb_chain(mvs_client_states[client].config_mode, FALSE);
    mvs_enabled_client = MVS_CLIENT_NONE;

    /* Clean up frame queues */
    mvssup_frame_q_empty(&mvs_ul_frame_q);
    mvssup_frame_q_empty(&mvs_dl_frame_q);
    if ((mvs_loopback != MVS_LB_UL2DL_PCM)
        && (mvs_loopback != MVS_LB_DL2UL_PCM))
    {
      mvssup_frame_q_empty(&mvs_lb_frame_q);
    }
  }


  if ((mvs_enabled_client == MVS_CLIENT_NONE)
      || (mvs_to_voc_client[client] != mvs_to_voc_client[mvs_enabled_client]))
  {
#ifdef FEATURE_VOC_PCM_INTERFACE
    if ((mvs_pcm_enabled_client == MVS_CLIENT_NONE)
        || (mvs_to_voc_client[client]
            != mvs_to_voc_client[mvs_pcm_enabled_client]))
#endif
    {
      voc_release(mvs_to_voc_client[client]);
    }
  }

  mvs_send_mode_event(client, MVS_MODE_NONE, MVS_MODE_READY);


  mvs_client_states[client].state = MVS_STATE_DISABLE;
  mvs_client_states[client].event_cb = NULL;

  mvs_client_states[client].desired_config_mode = MVS_MODE_NONE;
  mvs_client_states[client].desired_ul_cb = NULL;
  mvs_client_states[client].desired_dl_cb = NULL;
  mvs_client_states[client].desired_context = MVS_PKT_CONTEXT_NONE;

  mvs_client_states[client].config_mode = MVS_MODE_NONE;
  mvs_client_states[client].ul_cb = NULL;
  mvs_client_states[client].dl_cb = NULL;
  mvs_client_states[client].context = MVS_PKT_CONTEXT_NONE;

#ifdef FEATURE_VOC_PCM_INTERFACE
  mvs_client_states[client].pcm_state = MVS_STATE_DISABLE;

  mvs_client_states[client].desired_pcm_ul_cb = NULL;
  mvs_client_states[client].desired_pcm_dl_cb = NULL;
  mvs_client_states[client].desired_pcm_context = MVS_PKT_CONTEXT_NONE;

  mvs_client_states[client].pcm_ul_cb = NULL;
  mvs_client_states[client].pcm_dl_cb = NULL;
  mvs_client_states[client].pcm_context = MVS_PKT_CONTEXT_NONE;
#endif  /* FEATURE_VOC_PCM_INTERFACE */


  mvs_acquire_mask &= (~(uint32)(1 << ((uint32) client)));
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_acquire

DESCRIPTION
  This function allows a client to acquire MVS.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvs_acquire(
  mvs_client_type    client,
  mvs_event_cb_type  cb_func
)
{
  mvs_packet_type  *packet;
  mvs_event_type   event;

  MSG_MED("mvs_acquire(): client %d", client, 0, 0);

  /* Set up event callback */
  event.cmd.hdr.client = client;
  event.cmd.hdr.event  = MVS_EV_COMMAND;
  event.cmd.cmd_status = MVS_CMD_FAILURE;

  if (client < MVS_CLIENT_MAX) {
      /* Get packet, put on MVS queue */
      packet = mvs_get_pkt();

      if (packet != NULL) {
        packet->hdr.cmd         = MVS_CMD_ACQUIRE;
        packet->acquire.client  = client;
        packet->acquire.cb_func = cb_func;

        event.cmd.cmd_status = MVS_CMD_SUCCESS;
        mvs_cmd(packet);
      }
  }


  if((cb_func != NULL) && (event.cmd.cmd_status != MVS_CMD_SUCCESS)) {
    /* Delay success callback until command completes */
    cb_func(&event);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_acquire_processing

DESCRIPTION
  This function allows a client to acquire MVS.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mvs_acquire_processing(
  mvs_client_type client,
  mvs_event_cb_type cb_func
)
{
  MSG_MED("mvs_acquire_processing(): client %d", client, 0, 0);

  /* If MVS is enabled, multiple mvs_acquire will reset everything,
     requiring mvs_enable to be called again for re-enabling packet exchange.
     WCDMA RLC logical channels setup should not be affected.
  */
  if ((client == mvs_enabled_client)
#ifdef FEATURE_VOC_PCM_INTERFACE
      || (client == mvs_pcm_enabled_client)
#endif
     )
  {
    mvs_release_processing(client);
  }

  mvs_client_states[client].state = MVS_STATE_DISABLE;
#ifdef FEATURE_VOC_PCM_INTERFACE
  mvs_client_states[client].pcm_state = MVS_STATE_DISABLE;
#endif
  mvs_client_states[client].event_cb = cb_func;

  mvs_send_cmd_event(client, MVS_CMD_SUCCESS);

  mvs_acquire_mask |= (1 << ((uint32) client));
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_enable_allowed

DESCRIPTION
  This function determines whether a client is allowed to enable specified
MVS mode or not.

DEPENDENCIES
  None

RETURN VALUE
  MVS_CMD_FAILURE -- The MVS client was not acquired
  MVS_CMD_BUSY    -- MVS is being occupied by other client and there is
                     conflict to activate the requested mode.
  MVS_CMD_SUCCESS -- No problems to enable the client

SIDE EFFECTS
  None

===========================================================================*/
mvs_cmd_status_type mvs_enable_allowed(
  mvs_client_type client,
  mvs_mode_type mode
)
{
  if (client == MVS_CLIENT_NONE)
  {
    MSG_ERROR("Invalid client %d", client, 0, 0);
    return MVS_CMD_FAILURE;
  }

  if ((mvs_acquire_mask & (1 << ((uint32) client))) == 0)
  {
    MSG_ERROR("Client %d not acquired yet", client, 0, 0);
    return MVS_CMD_FAILURE;
  }


  switch (mode)
  {
    case MVS_MODE_MAX :
      return MVS_CMD_FAILURE;

    case MVS_MODE_LINEAR_PCM :
#ifdef FEATURE_VOC_PCM_INTERFACE
      if ((mvs_pcm_enabled_client != MVS_CLIENT_NONE)
          && (client != mvs_pcm_enabled_client))
      {
        return MVS_CMD_BUSY;
      }
      return MVS_CMD_SUCCESS;
#else
      return MVS_CMD_FAILURE;
#endif  /* FEATURE_VOC_PCM_INTERFACE */

    default :
      if ((mvs_enabled_client != MVS_CLIENT_NONE)
          && (client != mvs_enabled_client))
      {
        return MVS_CMD_BUSY;
      }
      return MVS_CMD_SUCCESS;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_enable

DESCRIPTION
  This function enables an MVS client to use the vocoder to process audio
packets of a given mode.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvs_enable(
  mvs_client_type client,
  mvs_mode_type mode,
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
)
{
  mvs_packet_type  *packet;
  mvs_event_type   event;

  MSG_MED("mvs_enable(): client %d, mode %d", client, mode, 0);

  /* Set up event callback */
  event.cmd.hdr.client = client;
  event.cmd.hdr.event  = MVS_EV_COMMAND;
  event.cmd.cmd_status = MVS_CMD_FAILURE;

  if (client < MVS_CLIENT_MAX) {
      /* Get packet, put on MVS queue */
      packet = mvs_get_pkt();

      if (packet != NULL) {
        packet->hdr.cmd        = MVS_CMD_ENABLE;
        packet->enable.client  = client;
        packet->enable.mode    = mode;
        packet->enable.ul_func = ul_func;
        packet->enable.dl_func = dl_func;
        packet->enable.context = context;

        event.cmd.cmd_status = MVS_CMD_SUCCESS;
        mvs_cmd(packet);
      }
  } else {
    MSG_ERROR("Invalid client %d", client, 0, 0);
    return;
  }


  if ((mvs_client_states[client].event_cb != NULL)
      && (event.cmd.cmd_status != MVS_CMD_SUCCESS))
  {
    /* delay success callback for further verification in enable processing */
    mvs_client_states[client].event_cb(&event);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

FUNCTION mvs_enable_pcm_processing

DESCRIPTION
  This function enables or disables an MVS client to use the vocoder to
process PCM samples.  PCM samples can be processed in idle mode.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_enable_pcm_processing(
  mvs_client_type client,
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
)
{
  voc_inform_type voc_status;


  /* Save client data */
  mvs_client_states[client].desired_pcm_ul_cb = ul_func;
  mvs_client_states[client].desired_pcm_dl_cb = dl_func;
  mvs_client_states[client].desired_pcm_context = context;


  voc_acquire(mvs_to_voc_client[client], mvs_voc_inform_callback);

  if (context != MVS_PKT_CONTEXT_NONE)
  {
    if (mvs_enabled_client == MVS_CLIENT_NONE)
    {
      (void)voc_config(mvs_to_voc_client[client],
                       mvs_to_voc_mode[MVS_MODE_LINEAR_PCM]);
    }
    else
    if (mvs_to_voc_client[client] != mvs_to_voc_client[mvs_enabled_client])
    {
      voc_capability_type mvs_voc_cap =  mvs_to_voc_mode[
                                          mvs_client_states[mvs_enabled_client]
                                          .desired_config_mode ];

#if defined(MSMAUD_VOC_4GV_VOIP) || defined(MSMAUD_VOC_IS127_VOIP) || defined(MSMAUD_VOC_4GV_WB_VOIP)      
      if(mvs_enabled_client == MVS_CLIENT_VOIP)
      {
          switch(mvs_client_states[mvs_enabled_client].desired_config_mode)
          {
#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif/*MSMAUD_VOC_4GV_VOIP*/

#ifdef MSMAUD_VOC_IS127_VOIP
          case MVS_MODE_IS127:
            mvs_voc_cap = VOC_CAP_IS127_VOIP;
            break;
#endif/*MSMAUD_VOC_IS127_VOIP*/

#ifdef MSMAUD_VOC_4GV_WB_VOIP
          case MVS_MODE_4GV_WB:
            mvs_voc_cap = VOC_CAP_4GV_WB_VOIP;  
            break;
#endif/*MSMAUD_VOC_4GV_WB_VOIP*/
          }
      }
#endif/* (MSMAUD_VOC_4GV_VOIP) || (MSMAUD_VOC_IS127_VOIP) || (MSMAUD_VOC_4GV_WB_VOIP) */
      
      if (mvs_client_states[mvs_enabled_client].state == MVS_STATE_STANDBY)
      {
         (void)voc_config_standby (mvs_to_voc_client[client],mvs_voc_cap);
      }
      else
      {
         (void)voc_config(mvs_to_voc_client[client],mvs_voc_cap);
      }
    }
#ifdef FEATURE_MVS_GEN_SILENCE
    /* Check in X ms to make sure voc has responded to MVS */
   
    #ifdef FEATURE_CMI
#error code not present
	#else
    clk_reg(&mvs_voc_clk_cb,             /* Call-back structure to register  */
            mvs_pcm_check_voc_state,     /* Routine called by Clock Services */
            100L,                        /* Call routine after X ms          */
            0L,                          /* De-register after first call     */
            FALSE );                     /* Do not call on a periodic basis  */
	#endif
#endif /* FEATURE_MVS_GEN_SILENCE */
  }
  else
  {
    if ((mvs_enabled_client == MVS_CLIENT_NONE)
        || (mvs_to_voc_client[client] != mvs_to_voc_client[mvs_enabled_client]))
    {
      (void)voc_config(mvs_to_voc_client[client], VOC_CAP_NONE);
    }
  }


  /* Multiple PCM mode enable would clean up PCM buffers and reset */
  mvs_register_isrcb_chain(MVS_MODE_LINEAR_PCM, FALSE);
  mvs_client_states[client].pcm_ul_cb = NULL;
  mvs_client_states[client].pcm_dl_cb = NULL;
  mvs_client_states[client].pcm_context = MVS_PKT_CONTEXT_NONE;
  mvs_client_states[client].pcm_state = MVS_STATE_DISABLE;

  /* Clean up frame queues */
  mvssup_frame_q_empty(&mvs_ul_pcm_frame_q);
  mvssup_frame_q_empty(&mvs_dl_pcm_frame_q);

  if ((mvs_loopback == MVS_LB_UL2DL_PCM)
      || (mvs_loopback == MVS_LB_DL2UL_PCM))
  {
    mvssup_frame_q_empty(&mvs_lb_frame_q);
  }


  /* Disable PCM processing */
  if (context == MVS_PKT_CONTEXT_NONE)
  {
    mvs_send_mode_event(client, MVS_MODE_LINEAR_PCM, MVS_MODE_READY);

    mvs_pcm_enabled_client = MVS_CLIENT_NONE;
    return;
  }


  /* may be no AVS state change, no voc inform callback would be triggered */
  voc_status = voc_get_status();
  if ((voc_status == VOC_INFORM_IDLE) || (voc_status == VOC_INFORM_VOCODER))
  {
    mvs_pcm_inform_callback(voc_status);
    return;
  }


#ifdef FEATURE_MVS_GEN_SILENCE
  if(mvs_client_states[client].state == MVS_STATE_SILENCE ||
     mvs_client_states[client].state == MVS_STATE_STANDBY_SILENCE) {

    mvs_client_states[client].pcm_state = MVS_STATE_SILENCE;
    mvs_client_states[client].pcm_ul_cb = ul_func;
    mvs_client_states[client].pcm_dl_cb = dl_func;
    mvs_client_states[client].pcm_context = context;
    mvs_send_mode_event(client, MVS_MODE_LINEAR_PCM, MVS_MODE_READY);

  } else
#endif /* FEATURE_MVS_GEN_SILENCE */
  {
    mvs_client_states[client].pcm_state = MVS_STATE_CHANGING;
    mvs_send_mode_event(client, MVS_MODE_LINEAR_PCM, MVS_MODE_INIT);
  }
}

#endif  /* FEATURE_VOC_PCM_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_enable_processing

DESCRIPTION
  This function enables an MVS client to use the vocoder to process audio
packets of a given mode.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_enable_processing(
  mvs_client_type client,
  mvs_mode_type mode,
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
)
{
  mvs_cmd_status_type status;

  MSG_MED("mvs_enable_processing(): client %d, mode %d", client, mode, 0);

  if(client >= MVS_CLIENT_MAX)
  {
	MSG_ERROR("Invalid MVS Client : %d",client,0,0);
	return;
  }

  /* allow 1 PCM mode and 1 vocoder mode at a time */
  status = mvs_enable_allowed(client, mode);
  mvs_send_cmd_event(client, status);

  if (status != MVS_CMD_SUCCESS)
  {
    return;
  }


  /* lock mvs_enabled_client and mvs_pcm_enabled_client */
  switch (mode)
  {
    case MVS_MODE_NONE :
    case MVS_MODE_MAX :
      break;

    case MVS_MODE_LINEAR_PCM :
#ifdef FEATURE_VOC_PCM_INTERFACE
      if (context != MVS_PKT_CONTEXT_NONE)
      {
        mvs_pcm_enabled_client = client;
      }

      if (mvs_pcm_enabled_client != MVS_CLIENT_NONE)
      {
        mvs_enable_pcm_processing(client, ul_func, dl_func, context);
      }
      else
      {
        mvs_send_mode_event(client, MVS_MODE_LINEAR_PCM, MVS_MODE_READY);
      }
#endif
      return;

    default :
      mvs_enabled_client = client;
      break;
  }


  if (mvs_enabled_client == MVS_CLIENT_NONE)
  {
    mvs_send_mode_event(client, mode, MVS_MODE_READY);
    return;
  }


  /* Save client data */
  mvs_client_states[client].desired_config_mode = mode;
  mvs_client_states[client].desired_ul_cb = ul_func;
  mvs_client_states[client].desired_dl_cb = dl_func;
  mvs_client_states[client].desired_context = context;


  voc_acquire(mvs_to_voc_client[client], mvs_voc_inform_callback);

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

  if (mode != MVS_MODE_NONE)
  {
      
      voc_capability_type mvs_voc_cap = mvs_to_voc_mode[mode];

#if defined(MSMAUD_VOC_4GV_VOIP) || defined(MSMAUD_VOC_IS127_VOIP) || defined(MSMAUD_VOC_4GV_WB_VOIP)      
      if( (client == MVS_CLIENT_VOIP)
#ifdef FEATURE_VOC_PCM_INTERFACE
          ||(mvs_pcm_enabled_client == MVS_CLIENT_VOIP)
#endif
       )
      {
        switch(mode)
        {
#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif/*MSMAUD_VOC_4GV_VOIP*/

#ifdef MSMAUD_VOC_IS127_VOIP
         case MVS_MODE_IS127:
          mvs_voc_cap = VOC_CAP_IS127_VOIP;
          break;
#endif/*MSMAUD_VOC_IS127_VOIP*/

#ifdef MSMAUD_VOC_4GV_WB_VOIP
         case MVS_MODE_4GV_WB:
           mvs_voc_cap = VOC_CAP_4GV_WB_VOIP;  
           break;
#endif/*MSMAUD_VOC_4GV_WB_VOIP*/
          }
      }
#endif/* (MSMAUD_VOC_4GV_VOIP) || (MSMAUD_VOC_IS127_VOIP) || (MSMAUD_VOC_4GV_WB_VOIP) */
      
#ifdef FEATURE_VOC_PCM_INTERFACE
    if ((mvs_pcm_enabled_client != MVS_CLIENT_NONE)
        && (mvs_to_voc_client[client]
            != mvs_to_voc_client[mvs_pcm_enabled_client]))
    {
       if(mvs_pcm_enabled_client == MVS_CLIENT_VOIP)
           (void)voc_config (mvs_to_voc_client[mvs_pcm_enabled_client], mvs_voc_cap);
       else
           (void)voc_config (mvs_to_voc_client[mvs_pcm_enabled_client], mvs_to_voc_mode[mode]);
    }
#endif  /* FEATURE_VOC_PCM_INTERFACE */    
    if(client == MVS_CLIENT_VOIP)
        (void)voc_config (mvs_to_voc_client[client], mvs_voc_cap);
    else
        (void)voc_config (mvs_to_voc_client[client], mvs_to_voc_mode[mode]);

#ifdef FEATURE_MVS_GEN_SILENCE
    /* Check in X ms to make sure voc has responded to MVS */
    
    #ifdef FEATURE_CMI
#error code not present
	#else
    clk_reg(&mvs_voc_clk_cb,             /* Call-back structure to register  */
            mvs_check_voc_state,         /* Routine called by Clock Services */
            100L,                        /* Call routine after 20ms          */
            0L,                          /* De-register after first call     */
            FALSE );                     /* Do not call on a periodic basis  */
	#endif
#endif /* FEATURE_MVS_GEN_SILENCE */
  }
  else
  {
#ifdef FEATURE_VOC_PCM_INTERFACE
    if ((mvs_pcm_enabled_client == MVS_CLIENT_NONE)
        || (mvs_to_voc_client[client]
            != mvs_to_voc_client[mvs_pcm_enabled_client]))
#endif
    {
      (void)voc_config(mvs_to_voc_client[client], VOC_CAP_NONE);
    }
  }


  if (mode == MVS_MODE_NONE
#ifdef FEATURE_MVS_GEN_SILENCE
      || mvs_client_states[client].state == MVS_STATE_SILENCE
      || mvs_client_states[client].state == MVS_STATE_STANDBY_SILENCE
#endif /* FEATURE_MVS_GEN_SILENCE */
     )
  {
    mvs_register_isrcb_chain(mvs_client_states[client].config_mode, FALSE);
    mvs_client_states[client].config_mode = mode;
    mvs_client_states[client].ul_cb = ul_func;
    mvs_client_states[client].dl_cb = dl_func;
    mvs_client_states[client].context = context;

    /* Clean up frame queues */
    mvssup_frame_q_empty(&mvs_ul_frame_q);
    mvssup_frame_q_empty(&mvs_dl_frame_q);

    if ((mvs_loopback != MVS_LB_UL2DL_PCM)
        && (mvs_loopback != MVS_LB_DL2UL_PCM))
    {
      mvssup_frame_q_empty(&mvs_lb_frame_q);
    }

#ifdef FEATURE_MVS_GEN_SILENCE
    if(mvs_client_states[client].state == MVS_STATE_SILENCE ||
       mvs_client_states[client].state == MVS_STATE_STANDBY_SILENCE) {

      mvs_client_states[client].state = MVS_STATE_SILENCE;
      mvs_send_mode_event(client, mode, MVS_MODE_READY);

      mvs_enabled_client = client;
    } else
#endif /* FEATURE_MVS_GEN_SILENCE */
    {
      mvs_client_states[client].state = MVS_STATE_DISABLE;
      mvs_send_mode_event(client, mode, MVS_MODE_READY);

      mvs_enabled_client = MVS_CLIENT_NONE;
    }
    return;
  }

#ifdef FEATURE_MVS_GEN_SILENCE
  if(mvs_client_states[client].state != MVS_STATE_SILENCE &&
     mvs_client_states[client].state != MVS_STATE_STANDBY_SILENCE)
#endif /* FEATURE_MVS_GEN_SILENCE */
  {
    /* CHANGING state is used to isolate if vocoder is exiting VOCODER state
       because of multiple mvs_enable calls. */
    mvs_client_states[client].state = MVS_STATE_CHANGING;
    mvs_send_mode_event(client, mode, MVS_MODE_INIT);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_standby

DESCRIPTION
  This function will cause vocoder to download the requested image but stay
in IDLE mode until mvs_enable is invoked.  This function cannot be used for
PCM mode.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void mvs_standby(
  mvs_client_type client,
  mvs_mode_type mode
)
{
  mvs_packet_type  *packet;
  mvs_event_type   event;

  MSG_MED("mvs_standby(): client %d, mode %d", client, mode, 0);

  /* Set up event callback */
  event.cmd.hdr.client = client;
  event.cmd.hdr.event  = MVS_EV_COMMAND;
  event.cmd.cmd_status = MVS_CMD_FAILURE;

  if (client < MVS_CLIENT_MAX) {
      /* Get packet, put on MVS queue */
      packet = mvs_get_pkt();

      if (packet != NULL) {
        packet->hdr.cmd           = MVS_CMD_STANDBY;
        packet->standby.client    = client;
        packet->standby.mode      = mode;

        event.cmd.cmd_status = MVS_CMD_SUCCESS;
        mvs_cmd(packet);
      }
  } else {
    MSG_ERROR("Invalid client %d", client, 0, 0);
    return;
  }

  if ((mvs_client_states[client].event_cb != NULL)
      && (event.cmd.cmd_status != MVS_CMD_SUCCESS))
  {
    /* delay success callback for further verification in standby processing */
    mvs_client_states[client].event_cb(&event);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_standby_processing

DESCRIPTION
  This function will cause vocoder to download the requested image but stay
in IDLE mode until mvs_enable is invoked.  This function cannot be used for
PCM mode.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_standby_processing(
  mvs_client_type client,
  mvs_mode_type mode
)
{
  mvs_cmd_status_type status;


  if(client >= MVS_CLIENT_MAX)
  {
	MSG_ERROR("Invalid MVS Client : %d",client,0,0);
	return;
  }
  if (mode != MVS_MODE_LINEAR_PCM)
  {
    status = mvs_enable_allowed(client, mode);
  }
  else
  {
    MSG_ERROR("PCM mode not allowed in standby processing.", 0, 0, 0);
    status = MVS_CMD_FAILURE;
  }
  mvs_send_cmd_event(client, status);

  if (status != MVS_CMD_SUCCESS)
  {
    return;
  }


  /* lock mvs_enabled_client */
  switch (mode)
  {
    case MVS_MODE_NONE :
    case MVS_MODE_LINEAR_PCM :
    case MVS_MODE_MAX :
      break;

    default :
      mvs_enabled_client = client;
      break;
  }

  if (mvs_enabled_client == MVS_CLIENT_NONE)
  {
    mvs_send_mode_event(client, mode, MVS_MODE_READY);
    return;
  }


  /* Save client data */
  mvs_client_states[client].desired_config_mode = mode;
  mvs_client_states[client].desired_ul_cb = NULL;
  mvs_client_states[client].desired_dl_cb = NULL;
  mvs_client_states[client].desired_context = MVS_PKT_CONTEXT_NONE;


  voc_acquire(mvs_to_voc_client[client], mvs_voc_inform_callback);

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

  if (mode != MVS_MODE_NONE)
  {
#ifdef FEATURE_VOC_PCM_INTERFACE
    if ((mvs_pcm_enabled_client != MVS_CLIENT_NONE)
        && (mvs_to_voc_client[client]
            != mvs_to_voc_client[mvs_pcm_enabled_client]))
    {
      (void)voc_config_standby(mvs_to_voc_client[mvs_pcm_enabled_client],
                               mvs_to_voc_mode[mode]);
    }
#endif  /* FEATURE_VOC_PCM_INTERFACE */
    (void)voc_config_standby(mvs_to_voc_client[client], mvs_to_voc_mode[mode]);
  }
  else
  {
#ifdef FEATURE_VOC_PCM_INTERFACE
    if ((mvs_pcm_enabled_client == MVS_CLIENT_NONE)
        || (mvs_to_voc_client[client]
            != mvs_to_voc_client[mvs_pcm_enabled_client]))
#endif
    {
      (void)voc_config(mvs_to_voc_client[client], VOC_CAP_NONE);
    }
  }


  if (mode == MVS_MODE_NONE)
  {
    mvs_register_isrcb_chain(mvs_client_states[client].config_mode, FALSE);
    mvs_client_states[client].config_mode = mode;
    mvs_client_states[client].ul_cb = NULL;
    mvs_client_states[client].dl_cb = NULL;
    mvs_client_states[client].context = MVS_PKT_CONTEXT_NONE;

    /* Clean up frame queues */
    mvssup_frame_q_empty(&mvs_ul_frame_q);
    mvssup_frame_q_empty(&mvs_dl_frame_q);

    if ((mvs_loopback != MVS_LB_UL2DL_PCM)
        && (mvs_loopback != MVS_LB_DL2UL_PCM))
    {
      mvssup_frame_q_empty(&mvs_lb_frame_q);
    }

    mvs_client_states[client].state = MVS_STATE_DISABLE;
    mvs_send_mode_event(client, mode, MVS_MODE_READY);

    mvs_enabled_client = MVS_CLIENT_NONE;
    return;
  }


#ifdef FEATURE_MVS_GEN_SILENCE
  if(mvs_client_states[client].state == MVS_STATE_SILENCE ||
     mvs_client_states[client].state == MVS_STATE_STANDBY_SILENCE) {
    /* Standby silence is used to track the combination of standby mode and
    ** silence processing.
    */
    mvs_client_states[client].state = MVS_STATE_STANDBY_SILENCE;
  } else
#endif /* FEATURE_MVS_GEN_SILENCE */
  {
    /* STANDBY state is used to isolate if vocoder is exiting VOCODER state
       because of multiple mvs_enable or mvs_standby calls. */
    mvs_client_states[client].state = MVS_STATE_STANDBY;
  }
  mvs_send_mode_event(client, mode, MVS_MODE_INIT);
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_listen

DESCRIPTION
  This function enables an MVS client to listen to the packets sent to and from
the vocoder.  It is typically used for in-call recording.

DEPENDENCIES
  A client that has called mvs_acquire() cannot also call mvs_listen().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_listen(
  mvs_client_type client,
  mvs_event_cb_type cb_func,
  mvs_listen_fn_type ul_func,
  mvs_listen_fn_type dl_func
)
{
  mvs_packet_type  *packet;
  mvs_event_type   event;

  MSG_MED("mvs_listen(): client %d", client, 0, 0);

  /* Set up event callback */
  event.cmd.hdr.client = client;
  event.cmd.hdr.event  = MVS_EV_COMMAND;
  event.cmd.cmd_status = MVS_CMD_FAILURE;

  if (client < MVS_CLIENT_MAX) {
    if ((mvs_listen_client == MVS_CLIENT_NONE || client == mvs_listen_client)
        && (client != mvs_enabled_client)) {

      /* Get packet, put on MVS queue */
      packet = mvs_get_pkt();

      if (packet != NULL) {
        packet->hdr.cmd        = MVS_CMD_LISTEN;
        packet->listen.client  = client;
        packet->listen.cb_func = cb_func;
        packet->listen.ul_func = ul_func;
        packet->listen.dl_func = dl_func;

        event.cmd.cmd_status = MVS_CMD_SUCCESS;
        mvs_cmd(packet);
      }

    } else {
      event.cmd.cmd_status = MVS_CMD_BUSY;
    }
  } else {
    MSG_ERROR("Invalid client %d", client, 0, 0);
    return;
  }

  if((mvs_client_states[client].event_cb != NULL) &&
     (event.cmd.cmd_status != MVS_CMD_SUCCESS)) {
    /* Delay success callback until command completes */
    mvs_client_states[client].event_cb(&event);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_listen_processing

DESCRIPTION
  This function enables an MVS client to listen to the packets sent to and from
the vocoder.  It is typically used for in-call recording.

DEPENDENCIES
  A client that has called mvs_acquire() cannot also call mvs_listen().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_listen_processing(
  mvs_client_type     client,
  mvs_listen_fn_type  ul_func,
  mvs_listen_fn_type  dl_func,
  mvs_event_cb_type   cb_func
)
{
  MSG_MED("mvs_listen_processing(): client %d", client, 0, 0);

  mvs_client_states[client].listen_event_cb = cb_func;
  mvs_client_states[client].listen_ul_cb = ul_func;
  mvs_client_states[client].listen_dl_cb = dl_func;
  mvs_client_states[client].state = MVS_STATE_LISTEN;

  mvs_listen_client = client;

  mvs_send_cmd_event(client, MVS_CMD_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_update_processing

DESCRIPTION
  This function will re-execute enable and listen command for the current
active and listen client respectively.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_update_processing(void)
{
  mvs_packet_type  *packet;


  /* Re-execute enable command to process MVS updates */
  if (mvs_enabled_client < MVS_CLIENT_MAX) {
    /* Get packet, put on MVS queue */
    packet = mvs_get_pkt();

    if (packet != NULL) {
      packet->hdr.cmd        = MVS_CMD_ENABLE;
      packet->enable.client  = mvs_enabled_client;
      packet->enable.cb_func = mvs_client_states[mvs_enabled_client].event_cb;
      packet->enable.mode    =
                     mvs_client_states[mvs_enabled_client].desired_config_mode;
      packet->enable.ul_func =
                     mvs_client_states[mvs_enabled_client].desired_ul_cb;
      packet->enable.dl_func =
                     mvs_client_states[mvs_enabled_client].desired_dl_cb;
      packet->enable.context =
                     mvs_client_states[mvs_enabled_client].desired_context;
      mvs_cmd(packet);
    }
  }


  /* Re-execute listen command to process MVS updates */
  if (mvs_listen_client < MVS_CLIENT_MAX) {
    /* Get packet, put on MVS queue */
    packet = mvs_get_pkt();

    if (packet != NULL) {
      packet->hdr.cmd        = MVS_CMD_LISTEN;
      packet->listen.client  = mvs_listen_client;
      packet->listen.cb_func =
                           mvs_client_states[mvs_listen_client].listen_event_cb;
      packet->listen.ul_func =
                              mvs_client_states[mvs_listen_client].listen_ul_cb;
      packet->listen.dl_func =
                              mvs_client_states[mvs_listen_client].listen_dl_cb;
      mvs_cmd(packet);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_process_cmd

DESCRIPTION
  This procedure processes requests received by mvs_cmd().  See mvsi.h for
  a description of the actual MVS packets.

DEPENDENCIES
  mvs_init() must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  mvs_cmd_q

===========================================================================*/
void mvs_process_cmd (void)
{
  mvs_packet_type *cmd_ptr;

  /* While there are commands to process, process each command.
  */
  while ((cmd_ptr = (mvs_packet_type *) q_get (&mvs_cmd_q)) != NULL) {

    /* Process each command type.
    */
    switch (cmd_ptr->hdr.cmd)
    {
      case MVS_CMD_ACQUIRE:
        mvs_acquire_processing(cmd_ptr->acquire.client,
                               cmd_ptr->acquire.cb_func);
        break;

      case MVS_CMD_ENABLE:
        mvs_enable_processing(cmd_ptr->enable.client,
                              cmd_ptr->enable.mode,
                              cmd_ptr->enable.ul_func,
                              cmd_ptr->enable.dl_func,
                              cmd_ptr->enable.context);

        break;

      case MVS_CMD_STANDBY:
        mvs_standby_processing(cmd_ptr->standby.client,
                               cmd_ptr->standby.mode);
        break;

      case MVS_CMD_LISTEN:
        mvs_listen_processing(cmd_ptr->listen.client,
                              cmd_ptr->listen.ul_func,
                              cmd_ptr->listen.dl_func,
                              cmd_ptr->listen.cb_func);
        break;

      case MVS_CMD_RELEASE:
        mvs_release_processing(cmd_ptr->release.client);
        break;

      case MVS_CMD_UPDATE:
        mvs_update_processing();
        break;

      case MVS_CMD_TX:
        mvs_tx_processing(cmd_ptr->tx.client,
                          cmd_ptr->tx.ul_qptr,
                          cmd_ptr->tx.ul_func);
        break;

      case MVS_CMD_RX:
        mvs_rx_processing(cmd_ptr->rx.client,
                          cmd_ptr->rx.dl_qptr,
                          cmd_ptr->rx.dl_func);
        break;

      /* Illegal command, does not return.
      */
      case MVS_CMD_INVALID:
      default:
        ERR_FATAL ("Illegal mvs command %x", cmd_ptr->hdr.cmd, 0, 0);
    }

    /* Return VS packet to free q
    */
    mvs_cmd_done(cmd_ptr);

  } /* end of while "commands to process" */
} /* end mvs_process_cmd */


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_mode_supported

DESCRIPTION
  This function determines if a requested MVS mode is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : supported.
  FALSE : not.

SIDE EFFECTS
  None

===========================================================================*/
boolean mvs_mode_supported(
  mvs_mode_type mode
)
{
  if (mode>=MVS_MODE_MAX)
  {
    return FALSE;
  }


  if (mode==MVS_MODE_LINEAR_PCM)
  {
#ifdef FEATURE_VOC_PCM_INTERFACE
    return TRUE;
#else
    return FALSE;
#endif
  }


#ifndef FEATURE_VOC_PACKET_INTERFACE
  /* IS733 and IS127 and 4GV needs FEATURE_VOC_PACKET_INTERFACE support */
  if (mode==MVS_MODE_IS733)
  {
    return FALSE;
  }

  if (mode==MVS_MODE_IS127)
  {
    return FALSE;
  }

  if ((mode==MVS_MODE_4GV_NB) || (mode==MVS_MODE_4GV_WB))
  {
    return FALSE;
  }
#endif  /* !FEATURE_VOC_PACKET_INTERFACE */


  return voc_capability_supported(mvs_to_voc_mode[mode]);
}

#ifdef FEATURE_MVS_GEN_SILENCE
/* <EJECT> */
/*===========================================================================

FUNCTION mvs_process_silence

DESCRIPTION
  This callback function either re-initializes the callback timer for its next
  operation, or it actually processes a silence frame and transmits it to the
  appropriate MVS uplink callback function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_process_silence(
  int4 ms_interval
  /*lint -esym(715,ms_interval) Tell Lint we know ms_interval parameter
  ** is not used */
) {
  uint8                   silence_packet[MVS_PCM_PKT_SIZE];
  voc_gsm_frame_type      frame_type;
#ifdef MSMAUD_VOC_AMR_WB
  voc_amr_wb_frame_type   amr_wb_frame_type;
#endif
  mvs_silence_frame_type  silence_frame;
#ifdef FEATURE_VOC_PCM_INTERFACE
  uint8 pcm_data[320];
#endif /* FEATURE_VOC_PCM_INTERFACE */


  memset(silence_packet, 0, MVS_PCM_PKT_SIZE);

  /* Check if in silence mode */
  if(mvs_client_states[mvs_enabled_client].state == MVS_STATE_SILENCE) {
    /* Transmit silence frame */
    switch(mvs_client_states[mvs_enabled_client].config_mode) {
#ifdef MSMAUD_VOC_AMR_WB
      case MVS_MODE_AMR_WB:
        if(mvssup_amr_get_scr_mode() == MVS_SCR_ENABLE) {
          mvs_get_awb_silence_frame(silence_packet,&amr_wb_frame_type);
          mvs_voc_awb_ul_proc(silence_packet,amr_wb_frame_type,
                              mvssup_get_ul_awb_mode());
          MSG_ERROR("SILENCE: AMR-WB, type:%d, mode:%d",amr_wb_frame_type,mvssup_get_ul_awb_mode(),0);
        }
        break;
#endif /* MSMAUD_VOC_AMR_WB */
#ifdef MSMAUD_VOC_AMR
      case MVS_MODE_AMR:
        if(mvssup_amr_get_scr_mode() == MVS_SCR_ENABLE) {
          mvs_get_amr_silence_frame(silence_packet,&frame_type);
          mvs_voc_amr_ul_proc(silence_packet,frame_type,
                              mvssup_get_ul_amr_mode());
          MSG_ERROR("SILENCE: AMR, type:%d, mode:%d",frame_type,mvssup_get_ul_amr_mode(),0);
        }
        break;
#endif /* MSMAUD_VOC_AMR */
#ifdef MSMAUD_VOC_EFR
#error code not present
#endif /* MSMAUD_VOC_EFR */
#ifdef MSMAUD_VOC_FR
#error code not present
  #endif /* MSMAUD_VOC_FR */

#ifdef MSMAUD_VOC_HR
      case MVS_MODE_HR:
        if(mvssup_gsm_get_dtx_mode() == MVS_DTX_ON) {
          /* generate sid */
          mvs_get_silence_frame(MVS_MODE_HR, &silence_frame);
          memcpy(silence_packet, silence_frame.data, silence_frame.length);
          memset(silence_packet+silence_frame.length, 0xff,
                 MVS_VOC_PKT_SIZE-silence_frame.length);
          mvs_voc_hr_ul_proc(silence_packet,
                             mvs_to_voc_hr_frame[silence_frame.type.hr],
                             FALSE);
          MSG_ERROR("SILENCE: HR, type:%d, mode:%d",mvs_to_voc_gsm_frame[silence_frame.type.fr],VOC_GSM_HALF_RATE,0);
        }
        break;
#endif /* MSMAUD_VOC_HR */

      case MVS_MODE_NONE:
      default:
        break;
    }
#ifdef FEATURE_MVS_GEN_SILENCE
    /* Handle PCM as well */
    if(mvs_client_states[mvs_enabled_client].pcm_state == MVS_STATE_SILENCE) {
      memset(pcm_data, 0, 320);
      mvs_voc_pcm_ul_proc((const uint16*)pcm_data);
      mvs_voc_pcm_dl_proc((uint16 *)pcm_data);
    }
#endif /* FEATURE_MVS_GEN_SILENCE */
#ifdef FEATURE_CMI
#error code not present
#else

    /* Wait 20ms and do it again */
    clk_reg(&mvs_silence_clk_cb,         /* Call-back structure to register  */
            mvs_process_silence,         /* Routine called by Clock Services */
            20L,                         /* Call routine after 20ms          */
            0L,                          /* De-register after first call     */
            FALSE );                     /* Do not call on a periodic basis  */
#endif 
  } else {
    /* Deregister potential interrupt */
    

   #ifdef FEATURE_CMI
#error code not present
	#else
    clk_dereg(&mvs_silence_clk_cb);
    clk_reg(&mvs_silence_clk_cb,         /* Call-back structure to register  */
            mvs_process_silence,         /* Routine called by Clock Services */
            25L,                         /* Call routine after 25ms          */
            0L,                          /* De-register after first call     */
            FALSE );                     /* Do not call on a periodic basis  */
	#endif

#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)
    /* Reset silence frame count for next time */
    mvs_silence_frame_count = 0;
#endif /* MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB */
  }
}
#endif /* FEATURE_MVS_GEN_SILENCE */
#endif  /* FEATURE_MVS */

