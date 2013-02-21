/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   M A I N  M O D U L E

GENERAL DESCRIPTION
  This is the minimal "user interface".  It is responsible for ensuring
  the UI task starts and handles the phone events not related to the
  user.  This code will exist to ensure the ui_task runs properly on
  a phone without a UI or with a UI on a different processor.  If the
  phone has a UI on this processor.

EXTERNALIZED FUNCTIONS

  ui_task
    The main user interface task.  Handle events

  ui_kick_dog
    Kicks the watchdog.

  Defines uisleep & uisleepold global variables.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ui_task should be created by MC before anyone calls ui_cmd.
  NV, HS, SND, and DIAG should be started before ui_task.
  All other functions are internal to UI.

  Copyright (c) 1999 through 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/ui_base.c#100 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/09   Krishnac Error fatal nv_task()
08/07/09   Arun    Changed CM subscription event featurization to FEATURE_WLAN from FEATURE_GAN
04/20/09   Gopi    Change for Mango UI Integration
04/02/09   kcp     CR177958 WLAN Subscription is not set for ThinUI GAN builds
03/11/09   Gopi    Change for Mango UI Integration
16/03/09   kcp     resolved Lint errors
09/11/08   sg      Additional condition to look for before doing FDN checks
04/10/08   cvs     Fix race condition with FTM in ThinUI builds
04/10/08   cvs     Add support for AMR WB
03/28/08   cvs     Init RTRE in Full UI builds, also
03/05/08   sg      Add RTRE support in ThinUI builds
03/05/08   cvs     Handle more refresh events from GSDI
01/30/08   cvs     Fix compile warnings
08/28/07   cvs     Handle more refresh events from GSDI
06/29/07   fs/cvs  Moved Sleep client registration before the ACK to tmc
05/21/07   sun     Convert to general card error event instead of handling all
05/16/07   ypmw    Fix the delay of getting the auto answer option from nv
05/11/07   cvs     Support both DRM 1.0 and 2.0 functionality.
03/20/07   cvs     BREW function signature change
03/13/07   cvs     Set up FTM mode in ThinUI builds
02/23/07   jas     Refactoring for HDK builds
02/12/07   mjv     Featurized more legacy UI code with FEATURE_UIONE_HDK
01/25/07   jks     Fixed compile error
01/10/07   jks     Added support for remote ring (ringback) for VoIP calls
11/27/06   cvs     Fix sleep registration to only exist in non-ThinUI builds.
11/09/06   sb      Added support for using KxIpc under FEATURE_IXCONTENTSVC
11/06/06   cvs     gsdi_init + sub fcn should be called on ThinUI for apps proc, too
11/06/06   jas     Reverting ThinUI fix, which doesn't work
09/06/06   jks     Added support for FEATURE_NEW_SLEEP_API
10/02/06   cvs     gsdi_init + sub fcn should be called on ThinUI for apps proc, too
09/23/06   cvs     Fix compile warnings
09/22/06   cvs     Mainlining BREW 4.0 changes
08/18/06    gr     Simplified ipc message handling.
07/18/06   jks     Fixes error in tracking number of active voice calls
07/12/06   cvs     Fix compile warnings
06/23/06   sb      Added IPC message handlers for IxContentMgr/Browser subsystem.
06/21/06   sb/om   Added IPC message handlers for DRM subsystem.
04/28/06   cvs     Fix ThinUI compile errors
04/13/06   cvs     Move call control to its own file
03/23/06   ak      Added Syncronization Function for LTK-STRIDE
03/21/06   cvs     ThinUI: Add unmute for emergency calls, fix dualproc startup
03/08/06   cvs     Fixes related to emergency categories
02/17/06   jks     Added CEPT sounds
01/19/06   cvs     support for emergency call categories
01/12/06   jas     Removed featurization to ensure correct variable value
11/08/05   cvs     Featurize PBM calls
10/19/05   cvs     Fix call control when GSTK modifies num to USS.
08/22/05   AT      Removed call to pbm_init.  Not present in current PBM.
08/19/05   AT      Added UCS2 support to PBM.  On the Apps build,
                   we need to initialize the local copy of the conversion
                   library.
08/09/05   RI      Modified the processing of incoming call.
08/02/05   cvs     look at FTM mode before going online in ThinUI
07/26/05   sun     Added gsdi_init in ThinUI initialization
05/26/05   cvs     support for PBM API changes
03/03/05   cvs     Remove RDM device setup.  RDM defaults this internally.
02/22/05   jas     Updating featurization of CM calls with WLAN
02/17/05   jas     Updated CM calls for WLAN feature
01/27/05   cvs/tm  add init for stride
01/21/05   cvs     Rearchitect uisnd_set_device()
01/20/05   PA      Check if call type is OTASP.
01/20/05   sun     Fixed Compiler error
01/07/04   sun     Fixed Compilation Warnings
12/08/04   cvs     Add AutoAnswer to ThinUI
11/30/04   cvs     Don't go online in ThinUI if PLT (physical layer test) build
11/18/04   PA      Do not call pbm_init if PBM is a separate task.
11/10/04   cvs     Initialize provisioning earlier in powerup
10/28/04   cvs/at  update featurizations to make FEATURE_UI_CORE_REMOVED
                   usable for CDMA phones.  Also first cut at
                   featurizing for dual processor.
09/23/04   SUN     Fix the vocoder settings
08/18/04   AT      Adding in code to only do call control checking
                   for voice and CS data calls.
08/17/04   AT      Fix issue with OTASP where activation code was not being used.
                   Also, restored PBM back to this file.
07/30/04   PA      Initialize sound and call_info for thin UI
07/29/04   sun     Enable voice playback
06/25/04   tml     Remove PBM init if coreapp is defined
06/23/04   sun     Fixed Featurizations
06/14/04   sun     Allow Call Control
06/03/04   sun     Change Operating Mode only for THIN UI
05/26/04   ak      Fixed Compile Error
02/26/04   AT      Initial revision
===========================================================================*/

/* <EJECT> */
/*
 * Secret decoder ring for features used in this file:
 * FEATURE_UI_DUALPROC_APPS   // Dual processor Apps processor
 * FEATURE_UI_DUALPROC_MDM    // Dual processor modem processor
 * FEATURE_CDMA_1900       // CDMA 1900Mhz frequencies
 * FEATURE_CDMA_800        // CDMA 800Mhz frequencies
 * FEATURE_GSM             // GSM
 * FEATURE_GSM_AMR         // Sub GSM Feature
 * FEATURE_GSM_EXT_SPEECH_PREF_LIST// Sub GSM Feature
 * FEATURE_GSM_HALF_RATE   // Sub GSM Feature
 * FEATURE_GSTK            // Supports Generic Sim Tool Kit (more functionality than GSDI)
 * FEATURE_GSTK_DUAL_SLOTS // Supports Generic Sim Tool Kit on two SIMS
 * FEATURE_MMGSDI          // We have GSDI
 * FEATURE_OTASP           // Supports OTASP calls
 * FEATURE_PLT             // Physical layer test--modem bringup
 * FEATURE_SEC_DRM         // Support Digital Rights Management
 * FEATURE_SLEEP           // Supports Sleep.
 * FEATURE_UIM             // Support the SIM/RUIM/USIM
 * FEATURE_UI_CORE_REMOVED // CoreAPP does not exist on this processor
 * FEATURE_UI_OTASP_STAR2281X      // OTASP has *228 for 1X
 * FEATURE_UI_PBM          // Phone book manager
 * FEATURE_WCDMA           // WCDMA
 */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "msg.h"
#include "err.h"

#include <string.h>

#include "uixsnd.h"
#include "ui_base.h"
#ifndef FEATURE_MANGO_UI
#include "ui_callctrl.h"
#endif
#include "uiutils.h"
#include "nv.h"
#include "nv_items.h"
#include "task.h"
#include "dog.h"
#include "tmc.h"

#include "ui.h"
#include "cm.h"

#include "ipc.h"
#include "ipcsvc.h"
#ifdef FEATURE_IXCONTENTSVC
  #include "ipc_dispatch.h"
  #include "ixipc.h"
#endif /* FEATURE_IXCONTENTSVC */

#if defined(FEATURE_SLEEP) || defined(FEATURE_NEW_SLEEP_API)
#include "sleep.h"
#endif

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

// add STRIDE
#ifdef srIMON
#include "OEMInterceptModules.h"
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
#include "touchpad.h"
#include "OEMPointerHelpers.h"
#endif

#ifdef FEATURE_UI_PBM
#ifdef FEATURE_UI_DUALPROC_APPS
#include "pbm_conversion.h"
#endif
#endif

#ifndef FEATURE_UI_CORE_REMOVED
  #include "BREWVersion.h"
  #include "core_ui.h"
#ifdef FEATURE_UI_HS_NEW_INTERFACE2
  #include "hs_public_keys.h"
#else
  #include "hs.h"
#endif
  extern hs_key_type CoreTask_GetKeyDown(void);
#else
  #ifndef FEATURE_UI_DUALPROC_MDM
    #include "rdevmap.h"
    #include "sd.h"
    #include "sys.h"
    #ifndef FEATURE_PLT
      #include "ftm.h"
    #endif
    #ifdef FEATURE_GSM_HALF_RATE
      #include "ds_cap.h"
    #endif /* FEATURE_GSM_HALF_RATE */
  #endif /* FEATURE_UI_DUALPROC_MDM */
#endif /* FEATURE_UI_CORE_REMOVED */

#ifdef FEATURE_BREW
#include "BREWVersion.h"
#if MIN_BREW_VERSION(4,0)
#include "SigDispatch.h"

/*These should be in a BREW header*/
extern IEnv *gpiRexEnv;
#endif /*MIN_BREW_VERSION 4.0*/
#endif /*FEATURE_BREW*/

#ifndef MIN_BREW_VERSION
#define MIN_BREW_VERSION(x,y) 0
#endif

#if !defined(FEATURE_UI_CORE_REMOVED)
#if defined(FEATURE_SEC_DRM)
extern void OEMDRM_ProcessIPCRsp( union ipc_msg_type**  rsp_msg_ptr );
#endif /* defined(FEATURE_SEC_DRM) */
#endif /* !defined(FEATURE_UI_CORE_REMOVED) */


#if defined (FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
void ui_call_err_cb(void *data_ptr, cm_call_cmd_e_type call_cmd,
                    cm_call_cmd_err_e_type call_cmd_err);
#if defined(FEATURE_MMGSDI) || defined(FEATURE_UIM_RUN_TIME_ENABLE)
typedef struct
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   uint32   rtre_config;                /* RTRE config read from NV */
#endif
#ifdef FEATURE_MMGSDI
   boolean  gw_subs_from_card_avail;    /* GSM, WCDMA subscription from card is available or not */
   boolean  cdma_subs_from_card_avail;  /* CDMA subscription from card is available or not*/
#endif
}SubscriptionInfo;

static SubscriptionInfo ui_subs_info;
#endif // FEATURE_MMGSDI || FEATURE_UIM_RUN_TIME_ENABLE
#endif // FEATURE_UI_CORE_REMOVED && !FEATURE_UI_DUALPROC_MDM

void VoteForSleep (boolean flag);

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
LOCAL rex_timer_type  ui_rpt_timer; /* Timer for kicking the watchdog. */

#ifdef CUST_EDITION
#ifdef FEATURE_UTK2
static  q_type          ui_cmd_q;       // 命令队列：管理待处理命令
extern byte UTK_parse_proactive_command(byte *cmd_data, word cmd_len);
extern void UTK_parse_sms_pp_dl_command(ui_sms_pp_dl_cmd_type *cmd);
#endif /*FEATURE_UTK2*/
#endif

#if !defined(FEATURE_UI_DUALPROC_APPS) || defined(FEATURE_UI_CORE_REMOVED) || defined (FEATURE_UIM_RUN_TIME_ENABLE)
LOCAL cm_client_id_type cm_client_id = (cm_client_id_type) -1;
#endif

#if defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
LOCAL rex_timer_type  ui_autoanswer_timer; /* Timer for autoanswer. */
#define UI_AUTOANSWER_TIME          3000   // millisecond timer for autoanswer
static boolean bAutoAnswer = FALSE;        /* Auto Answer on? */

#define UI_IS_VALID_CALLID(x) ( ((x) < CM_CALL_ID_MAX) ? TRUE : FALSE)

static boolean ui_vocoder_on = FALSE; /*tracks the status of the vocoder*/
static uint8 numberOfVoiceCalls = 0;
static cm_call_id_type ui_origCall_id;
static cm_call_id_type ui_incomCall_id;
static cm_call_type_e_type ui_incomCall_type;
cm_mm_call_info_s_type ui_calls[CM_CALL_ID_MAX];
#endif /* defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM) */

#ifdef FEATURE_NEW_SLEEP_API
  sleep_okts_handle gNewSleepHandle;
#endif /* FEATURE_NEW_SLEEP_API */
#ifdef FEATURE_LCD_TOUCH_ENABLE
static void oemui_touchpadinit(void);
static void oemui_post_pointer_data(void);
#endif

/* <EJECT> */
/*===========================================================================
FUNCTION ui_complete_pwrdown

DESCRIPTION
  This function acknoleges MC that the powerdown has completed.
  It can be called from the CoreApp or from the thin_ui's handling
  of the stop signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ui_complete_pwrdown(void)
{
  task_stop();
}


/* <EJECT> */
/*===========================================================================
FUNCTION HandleStopSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleStopSig(void)
{
  (void) rex_clr_sigs( rex_self(), TASK_STOP_SIG );
#ifdef FEATURE_UI_CORE_REMOVED
  ui_complete_pwrdown();
#endif /* FEATURE_UI_CORE_REMOVED */
  return;
} /* HandleStopSig */

/* <EJECT> */
/*===========================================================================
FUNCTION HandleOfflineSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleOfflineSig(void)
{
  (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
  MSG_MED("UI got offline sig",0,0,0);

  task_offline();
} /* HandleOfflineSig */


/* EJECT */
/*===========================================================================
FUNCTION ui_kick_dog

DESCRIPTION
  This function reports to dog task that UI is alive and kicking!

RETURN VALUE
  None

DEPENDENCIES
  Dog Task is running.
===========================================================================*/
void ui_kick_dog( void )
{

  (void) rex_set_timer( &ui_rpt_timer, DOG_UI_RPT_TIME );
  dog_report( DOG_UI_RPT );

} /* End of ui_kick_dog */

/* EJECT */
/*===========================================================================
FUNCTION UI_process_ipc_msg

DESCRIPTION
  This function handles all incoming IPC messages and dispatches
  them by service ID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ui_process_ipc_msg (void)
{
#ifndef T_WINNT
  union ipc_msg_type*  ipc_msg_ptr = NULL;

  //below 2 lines of code does nothing but is added to remove the lint warning
  //"ipc_msg_ptr declared but not used"
  if(ipc_msg_ptr != NULL)
  {
     ipc_msg_ptr = NULL;
  }

#if !defined(FEATURE_UI_CORE_REMOVED)
#ifdef FEATURE_IXCONTENTSVC
  /* The ipc_dispatch_msg function both receives the ipc message and
  ** deletes it. So there is no need to call ipc_receive or ipc_delete
  ** in this case.
  */
  ipc_dispatch_msg( );
#elif defined(FEATURE_SEC_DRM)
  /* Check the possible ranges of message IDs and call the */
  /* respective processing function.                       */
  ipc_msg_ptr = ipc_receive( IPC_MSG_ANY );
  if (IPCSVC_GET_SVC_ID(*((ipc_msg_id_type*) ipc_msg_ptr)) == IPCSVC_SECDRM)
  {
    /* DRM response to UI client */
    OEMDRM_ProcessIPCRsp( &ipc_msg_ptr );
  }
#else
#ifdef FEATURE_REX_IPC
  if (ipc_msg_ptr == NULL)
    ipc_msg_ptr = ipc_receive( IPC_MSG_ANY );
#endif
#endif /* FEATURE_IXCONTENTSVC */

#else
#ifdef FEATURE_REX_IPC
  ipc_msg_ptr = ipc_receive( IPC_MSG_ANY );
#endif
#endif /* !defined(FEATURE_UI_CORE_REMOVED) */
#ifdef FEATURE_REX_IPC
  if ( ipc_msg_ptr )
  {
    MSG_HIGH( "UI: Unfreed IPC msg 0x%x !",
              *((ipc_msg_id_type*) ipc_msg_ptr), 0, 0 );
    ipc_delete( &ipc_msg_ptr );
  }
#endif
#endif //#ifndef T_WINNT
} /* ui_process_ipc_msg */


/*===========================================================================

FUNCTION ui_wait

DESCRIPTION
  Main Wait routine for UI task. This routine performs the following functions:
    - Waits for a signal in the requested mask to be set.
    - Handles Watchdog report signal
    - Handles Key release to finish the keybeep if Core UI is built in

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type ui_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
   rex_sigs_type always_handle_mask; /* Mask of signals to handle while waiting */
   rex_sigs_type rex_signals_mask;   /* Mask of signals returned by REX     */

#if !defined(FEATURE_UI_CORE_REMOVED)
   ACONTEXT *pLast;
#endif /* !defined(FEATURE_UI_CORE_REMOVED)  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Always handle the UI_RPT_TIMER_SIG. This tells UI task to kick the watchdog. */
   always_handle_mask = UI_RPT_TIMER_SIG;

   /* Get current state of rex signals */
   rex_signals_mask = rex_get_sigs( &ui_tcb );

   /* --------------------------------------------------------------------
   ** Loop until the current signals from REX (including the queue signals
   ** faked due to an item on a queue, if any) match with one or more of
   ** the requested signals.
   ** -------------------------------------------------------------------- */
   do
   {
#ifndef USES_MMI
#if !defined(FEATURE_UI_CORE_REMOVED)
#if (!defined(FEATURE_UIONE_HDK) && !defined(FEATURE_MANGO_UI))
      /* Key beep is set to beep while key is down and we haven't
         processed the key release yet.  Wait for the release because
         the wait can take a while and the beep may be too long. */
      if ( !CoreTask_GetBurst() && ( CoreTask_GetKeyDown() != HS_NONE_K ) )
      {
         always_handle_mask |= UI_KEY_SIG;
      }
      else
#endif /* !FEATURE_UIONE_HDK */
      {
         always_handle_mask &= ~UI_KEY_SIG;
      }
#endif /* !defined(FEATURE_UI_CORE_REMOVED) */
#endif // #ifndef USES_MMI
      /* Perform the 'real' wait (if needed) */
      if ((rex_signals_mask & requested_mask) == 0)
      {
         rex_signals_mask = rex_wait( requested_mask | always_handle_mask);
      }

#ifndef FEATURE_UI_CORE_REMOVED
      /* Check for the release.  It can only be the release since
         we don't allow nested keys. */
      if( rex_signals_mask & always_handle_mask & UI_KEY_SIG ) {
        (void) rex_clr_sigs( &ui_tcb, UI_KEY_SIG );

        // need to set context since brew could be in the middle of something
#if MIN_BREW_VERSION(3,0)
        pLast = AEE_EnterAppContext(NULL);
#else
        pLast = AEE_SetAppContext(NULL);
#endif // MIN_BREW_VERSION(3,0)
        handle_keys();

#if MIN_BREW_VERSION(3,0)
        (void) AEE_LeaveAppContext(pLast);
#else
        (void) AEE_SetAppContext(pLast);
#endif // MIN_BREW_VERSION(3,0)
      } /* key signal set */
#endif /* FEATURE_UI_CORE_REMOVED */

      /* Need to kick the watchdog */
      if( rex_signals_mask & UI_RPT_TIMER_SIG ) {
        ui_kick_dog();
      }

   /* do while mask requested does not match current mask */
   } while ((rex_signals_mask & requested_mask) == 0);

   return (rex_signals_mask);

} /* end ui_wait() */

/*===========================================================================

FUNCTION ui_wait_for_nv

DESCRIPTION
  Main Wait routine for UI task. This routine performs the following functions:
    - Waits for a signal in the requested mask to be set.
    - Handles Watchdog report signal
    - Handles Key release to finish the keybeep if Core UI is built in

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type ui_wait_for_nv
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
   rex_sigs_type always_handle_mask; /* Mask of signals to handle while waiting */
   rex_sigs_type rex_signals_mask;   /* Mask of signals returned by REX     */

#if !defined(FEATURE_UI_CORE_REMOVED)
   ACONTEXT *pLast;
#endif /* !defined(FEATURE_UI_CORE_REMOVED)  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Always handle the UI_RPT_TIMER_SIG. This tells UI task to kick the watchdog. */
   always_handle_mask = UI_RPT_TIMER_SIG;

   /* Get current state of rex signals */
   rex_signals_mask = rex_get_sigs( &ui_tcb );

   /* --------------------------------------------------------------------
   ** Loop until the current signals from REX (including the queue signals
   ** faked due to an item on a queue, if any) match with one or more of
   ** the requested signals.
   ** -------------------------------------------------------------------- */
   do
   {
      /* Perform the 'real' wait (if needed) */
      if ((rex_signals_mask & requested_mask) == 0)
      {
         rex_signals_mask = rex_wait( requested_mask | always_handle_mask);
      }

      /* Need to kick the watchdog */
      if( rex_signals_mask & UI_RPT_TIMER_SIG ) {
        ui_kick_dog();
      }

   /* do while mask requested does not match current mask */
   } while ((rex_signals_mask & requested_mask) == 0);

   return (rex_signals_mask);

} /* end ui_wait_for_nv() */


/* <EJECT> */
/*===========================================================================
FUNCTION ui_answer_call

DESCRIPTION
  This function answers an incoming call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined (FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
void ui_answer_call ()
{
  cm_ans_params_s_type    answer_params;
  sys_sys_mode_e_type sys_mode = sd_misc_get_curr_acq_sys_mode();

  memset( &answer_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE, sizeof( cm_ans_params_s_type));

  switch (sys_mode) {
  case SYS_SYS_MODE_CDMA:
  case SYS_SYS_MODE_HDR:
    answer_params.info_type = CM_CALL_MODE_INFO_CDMA;
    answer_params.ans_params.cdma_ans.call_type = ui_incomCall_type;
    break;


  case SYS_SYS_MODE_GSM:
  case SYS_SYS_MODE_WCDMA:
  case SYS_SYS_MODE_GW:
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
    answer_params.info_type = CM_CALL_MODE_INFO_GW_CS;
    answer_params.ans_params.gw_cs_ans.accept = TRUE;
    answer_params.ans_params.gw_cs_ans.call_type = ui_incomCall_type;
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
    break;

  default:
    ERR("Invalid System Mode %d", sys_mode, 0, 0);
    return;
  }

  if (!cm_mm_call_cmd_answer(ui_call_err_cb, NULL, cm_client_id, ui_incomCall_id, &answer_params)) {
    MSG_HIGH("Could not auto answer incoming call", 0, 0, 0);
  }
}
#endif

#ifdef CUST_EDITION
void ui_cmd(ui_cmd_type *cmd_ptr);

#ifdef FEATURE_UTK2
static boolean g_bCanProactiveHandle = FALSE;

void ui_enable_proactive(boolean bEnable)
{
    g_bCanProactiveHandle = bEnable;
}

void set_UTK_session_status(byte st)
{
	ui_cmd_type  *ui_buf_ptr;

#ifdef FEATURE_REX_DYNA_MEM_UI
	ui_buf_ptr = ui_get_cmd();
#else
	if( (ui_buf_ptr = (ui_cmd_type*) q_get( &ui_cmd_free_q)) == NULL )
	{
		ERR("Out of UI cmd buffer", 0,0,0);
		return;
	}
	ui_buf_ptr->proactive_cmd.hdr.done_q_ptr = &ui_cmd_free_q;;
#endif /* FEATURE_REX_DYNA_MEM_UI */

	//wangliang add! 2010-08-13
	if ( ui_buf_ptr == NULL )
	{
		MSG_FATAL("Out of UI cmd buffer", 0,0,0);
		return;
	}
	
	ui_buf_ptr->proactive_cmd.hdr.cmd        = UI_PROACTIVE_UIM_F;
	ui_buf_ptr->proactive_cmd.hdr.task_ptr   = NULL;
	ui_buf_ptr->proactive_cmd.hdr.sigs       = 0;
	//ui_buf_ptr->proactive_cmd.hdr.done_q_ptr = &ui_cmd_free_q;

	ui_buf_ptr->proactive_cmd.num_bytes = 1;
	ui_buf_ptr->proactive_cmd.cmd_data[0] = st; //UIM_TK_END_PROACTIVE_SESSION;

	/* send command to ui */
	ui_cmd( ui_buf_ptr );

	if (st == UIM_TK_END_PROACTIVE_SESSION)
	{
	    uim_power_control( UIM_PROACTIVE_UIM, FALSE);
	    
	    /* Make sure task can process this control */
	    (void) rex_set_sigs( &uim_tcb, UIM_STATE_TOGGLE_SIG);
	}
}
#endif /*FEATURE_UTK2*/

// 这个函数用来让WMS等到AEE初始化完成之后再调用wms_reg_msg_info_cache_cb锁注册的函数
// 否则将导致WMS初始化非法使用BREW Heap导致一系列的问题
void wms_init_ui_cmd(void)
{
	ui_cmd_type  *ui_buf_ptr;

#ifdef FEATURE_REX_DYNA_MEM_UI
	ui_buf_ptr = ui_get_cmd();
#else
	if( (ui_buf_ptr = (ui_cmd_type*) q_get( &ui_cmd_free_q)) == NULL )
	{
		ERR("Out of UI cmd buffer", 0,0,0);
		return;
	}
	ui_buf_ptr->proactive_cmd.hdr.done_q_ptr = &ui_cmd_free_q;;
#endif /* FEATURE_REX_DYNA_MEM_UI */

	//wangliang add! 2010-08-13
	if ( ui_buf_ptr == NULL )
	{
		MSG_FATAL("Out of UI cmd buffer", 0,0,0);
		return;
	}
	
	ui_buf_ptr->hdr.cmd        = UI_WMS_INIT;
	ui_buf_ptr->hdr.task_ptr   = rex_self();
	ui_buf_ptr->hdr.sigs       = UI_OEM_SIG;
    
	/* send command to ui */
	ui_cmd( ui_buf_ptr );
    rex_wait(UI_OEM_SIG);
}

/*===========================================================================

FUNCTION uim_send_err_to_ui                         INTERNAL FUNCTION

DESCRIPTION
  Send this uim error command to UI for processing.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  The UI is given the proactive command to process.

===========================================================================*/
void uim_send_err_to_ui
(
  uim_status_type uim_status_param
)
{
  ui_cmd_type  *ui_buf_ptr; /* pointer to buffer for ui cmd */

/* Send a msg status command to the UI task */
#ifdef FEATURE_REX_DYNA_MEM_UI
  ui_buf_ptr = ui_get_cmd();
  if(ui_buf_ptr == NULL)
  {
    MSG_FATAL("Out of UI cmd buffer", 0,0,0);
    return;
  }
#else
  if( (ui_buf_ptr = (ui_cmd_type*) q_get( &ui_cmd_free_q )) == NULL )
  {
    ERR("Out of UI cmd buffer", 0,0,0);
    return;
  }
  ui_buf_ptr->proactive_cmd.hdr.done_q_ptr = &ui_cmd_free_q;;
#endif /* FEATURE_REX_DYNA_MEM_UI */

  ui_buf_ptr->uim_err_cmd.hdr.cmd        = UI_UIM_ERR_F;
  ui_buf_ptr->uim_err_cmd.hdr.task_ptr   = NULL;
  ui_buf_ptr->uim_err_cmd.hdr.sigs       = 0;
  ui_buf_ptr->uim_err_cmd.uim_status     = uim_status_param;
  MSG_FATAL("uim_send_err_to_ui %d", uim_status_param,0,0);
  /* send command to ui */
  ui_cmd( ui_buf_ptr );
} /* uim_send_err_to_ui */

/*==============================================================================
函数: 
    ui_get_cmd
       
说明: 
    函数用来动态分配一块 ui 命令 buffer 返回。
       
参数: 
    none
    
返回值:
    ui_cmd_type 指针。
       
备注:
    REX 、NV 、 DIAG 及 HS 任务必须已运行，TMC heap is initialized.
    
==============================================================================*/
ui_cmd_type* ui_get_cmd(void)
{
    ui_cmd_type* cmd_ptr = NULL;
    
    cmd_ptr = (ui_cmd_type *)mem_malloc(&(tmc_heap), sizeof(ui_cmd_type));
    if (cmd_ptr == NULL)
    {
        ERR("Could not allocate memory for Call object!", 0, 0, 0);
    }
    else
    {
        // Initialize to a default value 
        cmd_ptr->hdr.cmd = UI_NUM_UI_COMMANDS;  
    }
    
    return cmd_ptr;
} 

/*==============================================================================
函数: 
    ui_cmd
       
说明: 
    Allows other tasks to queue up messages to the UI.  Grab a buffer from the
    ui_cmd_free_q, fill it in, and call ui_cmd with it.  All this will
    do is queue it and set a signal for the UI task.  When the command
    is done, the buffer will be queued as directed in the command header,
    and a task you specify may be signaled.
       
参数: 
    cmd_ptr [in]: ui 命令块指针
    
返回值:
    none
       
备注:
    REX 、NV 、 DIAG 及 HS 任务必须已运行，ui_cmd_q already initialized.
    
==============================================================================*/
void ui_cmd(ui_cmd_type *cmd_ptr)
{
    MSG_HIGH( "UI cmd %d", cmd_ptr->hdr.cmd, 0, 0 );
    
    // init link
    (void) q_link(cmd_ptr, &cmd_ptr->hdr.link);
    
    // and queue it
    q_put(&ui_cmd_q, &cmd_ptr->hdr.link);
    
    // signal the UI task
    (void) rex_set_sigs(&ui_tcb, UI_CMD_Q_SIG);
}



/*==============================================================================
函数: 
    oemui_handlecmd
       
说明: 
    函数处理发给 oemui task 的命令。
       
参数: 
    none
    
返回值:
    none
       
备注:
    
==============================================================================*/
static void oemui_handlecmd(ui_cmd_type *cmd_ptr)
{
    IShell *pShell = AEE_GetShell();
    switch(cmd_ptr->hdr.cmd)
    {
#ifdef FEATURE_UIM_TOOLKIT
        case UI_SMS_PP_DL_F:
            UTK_parse_sms_pp_dl_command(&cmd_ptr->sms_pp_dl_cmd);
            break;
            
        case UI_PROACTIVE_UIM_F:
#ifdef FEATURE_UIM_TOOLKIT_UTK
#ifdef FEATURE_UTK2
            if (pShell != NULL)
            {
                byte cmd_type;
                //static boolean first_set_menu = TRUE;
                cmd_type = UTK_parse_proactive_command(cmd_ptr->proactive_cmd.cmd_data, cmd_ptr->proactive_cmd.num_bytes);
                              
                //if(first_set_menu == TRUE && cmd_type == UIM_TK_SETUP_MENU)
                //{
                //    first_set_menu = FALSE;
                //    return;
                //}  
                (void)ISHELL_PostEvent(pShell,
                                       AEECLSID_APP_UTK,
                                       (int)EVT_RUIM_PROACTIVE,
                                       cmd_type,
                                       0);
            }
#endif //FEATURE_UTK2
#endif
            break;
#endif
        case UI_WMS_INIT:
            // Nothing Todo
            break;
            
        case UI_UIM_ERR_F:
            MSG_FATAL("UI_UIM_ERR_F %d",cmd_ptr->uim_err_cmd.uim_status,0,0);
            if(UIM_POLL_ERR_S == cmd_ptr->uim_err_cmd.uim_status)
            {
                extern void CoreTask_HandleUIMErr(void);
                CoreTask_HandleUIMErr();
            }
            break;
            
        default:
            ERR( "ui command 0X%x is ignored!", cmd_ptr->hdr.cmd, 0, 0 );
            break;
    } 
}

/*==============================================================================
函数: 
    process_command_sig
       
说明: 
    函数处理 oemui task 收到的命令信号。
       
参数: 
    none
    
返回值:
    none
       
备注:
    
==============================================================================*/
static void process_command_sig(void)
{
    ui_cmd_type     *cmd_ptr;
    rex_tcb_type    *ctask_ptr;
    rex_sigs_type   csigs;
    
    // 处理命令队列中的命令直到队列为空
    while ((cmd_ptr = (ui_cmd_type *) q_get(&ui_cmd_q)) != NULL)
    {
        // 实际处理命令
        if(g_bCanProactiveHandle)
        {
            oemui_handlecmd(cmd_ptr);
        }
        
        ctask_ptr = cmd_ptr->hdr.task_ptr;
        csigs = cmd_ptr->hdr.sigs;
        
#ifdef FEATURE_REX_DYNA_MEM_UI
        mem_free(&tmc_heap, cmd_ptr);
#else
        if (cmd_ptr->hdr.done_q_ptr)
        {
            q_put( cmd_ptr->hdr.done_q_ptr, &cmd_ptr->hdr.link );
        }
#endif        
        
        if (ctask_ptr)
        {
            (void) rex_set_sigs(ctask_ptr, csigs);
        }
    }
}
#endif

  /*===========================================================================
FUNCTION HandleSignals

DESCRIPTION
  This function handles all signals received by UI task. This function is to
  be invoked for every signal UI recives. It calls the appropriate functions
  to handle each signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleSignals (
    rex_sigs_type sigs
)
{
#ifndef FEATURE_UI_CORE_REMOVED
  CoreAppHandleSignals (sigs);
#endif

  /* Stop task - nothing special to do */
  if ( ( sigs & TASK_STOP_SIG ) != 0 )
  {
    HandleStopSig();
  }

  /* Go offline signal */
  if( ( sigs & TASK_OFFLINE_SIG ) != 0 ) {
    HandleOfflineSig();

  }

  /* Report back to the watchdog.  */
  if( sigs & UI_RPT_TIMER_SIG ) {
    ui_kick_dog();
  }
#ifdef FEATURE_LCD_TOUCH_ENABLE

  if( (sigs & UI_GET_PEN_SIG) != 0 )
  {
		(void) rex_clr_sigs( rex_self(), UI_GET_PEN_SIG );
		oemui_post_pointer_data();
  }
#endif
#ifndef T_WINNT
#if !defined(FEATURE_UI_DUALPROC_MDM)
  /* IPC message signal */
  if ( sigs & UI_IPC_SIG )
  {
    ui_process_ipc_msg( );
  }
#endif /* !defined(FEATURE_UI_DUALPROC_MDM) */
#endif //#ifndef T_WINNT

#if defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
  if ( sigs & UI_AUTOANSWER_SIG )
  {
    (void) rex_clr_sigs( &ui_tcb, UI_AUTOANSWER_SIG );
    //The autoanswer timer expired, so answer the call.
    ui_answer_call();
  }
#endif

#ifdef CUST_EDITION
#ifdef FEATURE_UIM_TOOLKIT
    if ( sigs & UI_CMD_Q_SIG )
    {
        (void) rex_clr_sigs( &ui_tcb, UI_CMD_Q_SIG );
        process_command_sig();
    }
#endif
#endif
}

/* <EJECT> */
/*===========================================================================
FUNCTION ui_subscription_cb

DESCRIPTION
  This function is called at the completion of the subscription available
  and not available commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_subscription_cb(void *data_block, cm_ph_cmd_e_type ph_cmd,
    cm_ph_cmd_err_e_type ph_cmd_err)
{
  if (ph_cmd_err != CM_PH_CMD_ERR_NOERR)
    ERR("Error %d to subscription command %d", ph_cmd_err, ph_cmd, 0);
}

#if defined(FEATURE_UI_CORE_REMOVED)
/* <EJECT> */
/*===========================================================================
FUNCTION   ui_ss_err_cb

DESCRIPTION
  After the SS commands are issued, this callback is called to
  give us the results.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_ss_err_cb(cm_client_id_type client, cm_ss_cmd_e_type cmd,
    cm_ss_cmd_err_e_type cmd_err)
{
  MSG_HIGH("SS Callback %d %d %d", client, cmd, cmd_err);
}

/* <EJECT> */
/*===========================================================================
FUNCTION   ui_call_err_cb

DESCRIPTION
  This command is called after a call command is issued.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_call_err_cb(void *data_ptr, cm_call_cmd_e_type call_cmd,
                    cm_call_cmd_err_e_type call_cmd_err)
{
  MSG_HIGH("Call Command Callback returned %d for command %d",
                  call_cmd_err, call_cmd, 0);
}

/* <EJECT> */
/*===========================================================================
FUNCTION   ui_ss_event_cb

DESCRIPTION
  This function is called when we get an SS event.  It displays the
  results on the QXDM.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_ss_event_cb(cm_ss_event_e_type ss_event,
                    const cm_mm_ss_info_s_type *ii_info_ptr)
{
  MSG_HIGH("Recevied SS EVENT %d", ss_event, 0, 0);
}

#if defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
/* <EJECT> */
/*===========================================================================
FUNCTION   ui_call_event_cb

DESCRIPTION
  When a call event occurs, this callback informs the user that it
  occurred.  In the case of the setup message, this handler will send
  CM the setup response with a default set of bearer capabilities.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Allows the MT call to continue.
===========================================================================*/
void ui_call_event_cb( cm_call_event_e_type call_event, /* call event */
                       const cm_mm_call_info_s_type *call_info_ptr
                                            /* call state information */
)
{
  /* The default bearer capability to use on incoming voice calls. */
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  static cm_bearer_capability_s_type default_bearer_capability =
  {
     TRUE,                  /* validity */

     /* octet 3 */
  #ifdef FEATURE_GSM_HALF_RATE
     DUAL_RATE_FULL_RATE_PREFERRED,
  #else
     FULL_RATE_ONLY,
  #endif /* FEATURE_GSM_HALF_RATE */

     GSM,                   /* coding_standard : GSM */
     CIRCUIT,               /* transfert mode : circuit mode */
     ITC_SPEECH,            /* information transfer capability : speech */
  #if defined (FEATURE_CMI) || defined  (FEATURE_TTY)
     CTM_NOT_SUPPORTED,     /*ctm*/
  #endif /* FEATURE_CMI || FEATURE_TTY */

     /*  support for 3 speech versions: EFR, FR and AMR  */
  #ifdef FEATURE_GSM_AMR
  #if defined (FEATURE_CMI) || defined  (FEATURE_GSM_AMR_WB)
     /* octet 3a */
     GSM_FR_SPEECH_VER_5,
  #endif /* FEATURE_CMI || FEATURE_GSM_AMR_WB */
     /* octet 3a/3b */
     GSM_FR_SPEECH_VER_3,  /* GSM AMR Full Rate */

     /* octet 3b/3c */
     GSM_FR_SPEECH_VER_2,  /* GSM Enhanced Full Rate  */

     /* octet 3c/3d */
     GSM_FR_SPEECH_VER_1,  /* GSM FR */
  #else
     /* octet 3a */
     GSM_FR_SPEECH_VER_2,

     /* octet 3b */
     GSM_FR_SPEECH_VER_1,

     /* octet 3c */
  #ifdef FEATURE_GSM_HALF_RATE
     GSM_HR_SPEECH_VER_1, /* GSM Half Rate Version 0 or 1 */
  #else
     GSM_INVALID_SPEECH_VER,
  #endif /* FEATURE_GSM_HALF_RATE */
  #endif /* FEATURE_GSM_AMR */

  #if defined (FEATURE_CMI) || defined  (FEATURE_GSM_EXT_SPEECH_PREF_LIST)
  #ifdef FEATURE_GSM_AMR
     /* octet 3d */
     GSM_HR_SPEECH_VER_3,

     /* octet 3e */
     GSM_HR_SPEECH_VER_1,
  #else
     /* octet 3d */
     GSM_INVALID_SPEECH_VER,

     /* octet 3e */
     GSM_INVALID_SPEECH_VER,
  #endif /* FEATURE_GSM_AMR */
  #endif /* FEATURE_CMI || FEATURE_GSM_EXT_SPEECH_PREF_LIST */

     /* octet 4 */
     0x00,                  /* compression */
     0x00,                  /* structure */
     0x01,                  /* duplex mode : full duplex */
     0x00,                  /* configuration */
     0x00,                  /* NIRR */
     0x00,                  /* establishment */

     /* octet 5 */
     0x00,                  /* access id */
     0x00,                  /* rate adaption : no rate adaption */
     0x00,                  /* signalling access control */

     /* octet 5a */
     0x00,                  /* other_ITC */
     0x00,                  /* other_rate_adaption */

     /* octet 5b */
     0x00,                  /* rate_adaption_header */
     0x00,                  /* multiple_frame_establishment */
     0x00,                  /* mode_of_operation */
     0x00,                  /* logical_link_identifier_negotiation */
     0x00,                  /* assignor_assignee */
     0x00,                  /* inband_outband_negotiation */

     /* octet 6 */
     0x01,                  /* layer1 id */
     0x00,                  /* user info layer 1 protocol */
     0x00,                  /* sync - async */

     /* octet 6a */
     0x00,                  /* number of stop bits */
     0x00,                  /* negotiation */
     0x00,                  /* number of data bits */
     0x00,                  /* user rate */

     /* octet 6b */
     0x00,                  /* intermediate rate */
     0x00,                  /* NIC on Tx */
     0x00,                  /* NIC on Rx */
     0x00,                  /* parity */

     /* octet 6c */
     0x00,                  /* connection element */
     0x00,                  /* modem type */

     /* These values are for GSM Phase2+/WCDMA */
     /* octet 6d */
     OMT_UNDEFINED,         /* Other Modem type */
     FNUR_UNDEFINED,        /* Fixed Network User Rate */

     /* octet 6e */
     ACC_NONE,                    /* Acceptable Channel Codings */
     MAX_NUMBER_OF_TCH_UNDEFINED, /* Max Number of TCHs */

     /* octet 6f */
     UIMI_UNDEFINED,              /* User Initiated Modifiaction Indication */
     WAIUR_UNDEFINED,             /* Wanted Air User Rate */

     /* octet 6g */
     0x00,                        /* acceptable_channel_coding_extended */
     0x00,                        /* asymmetry_indication - EDGE channel coding */

     /* octet 7 */
     0x02,                        /* layer 2 ID */
     0x00,                        /* user info layer 2 protocol */

     /* For logging purpose */
     0                            /* MT fall-back rate */
  };
#else
  static cm_bearer_capability_s_type default_bearer_capability = {0};
#endif //defined (FEATURE_GSM) || defined (FEATURE_WCDMA)

  /* Inform the user via QXDM. */
  MSG_HIGH("Received a call event %d", call_event, 0, 0);

  if (UI_IS_VALID_CALLID(call_info_ptr->call_id)) {
    memcpy(&ui_calls[call_info_ptr->call_id], (void *) call_info_ptr,
           sizeof(cm_mm_call_info_s_type));
  }

  /* Check if it is the SETUP IND message, this will send the
   * setup response.
   */
  MSG_HIGH("ThinUI Received CM Event %d",call_event,0,0);
  switch (call_event)
  {
    case CM_CALL_EVENT_SETUP_IND:
      {
        cm_setup_res_params_s_type setup_params;

        if (call_info_ptr->call_type != CM_CALL_TYPE_VOICE)
          return;

        memset(&setup_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE,
               sizeof(setup_params));
        setup_params.accept = TRUE;
        memcpy(&setup_params.bearer_capability_1, &default_bearer_capability,
               sizeof(cm_bearer_capability_s_type));
        cm_mm_call_cmd_setup_res(ui_call_err_cb, NULL, cm_client_id,
                                 call_info_ptr->call_id, &setup_params);
      }
      break;

#if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) || defined (CM_API_IPCALL_SS_INFO)
    case CM_CALL_EVENT_PROGRESS_INFO_IND:
      {
        boolean  bRemoteAlert = FALSE;

        MSG_HIGH("Received CM_CALL_EVENT_PROGRESS_INFO_IND ",0,0,0);
#ifdef CM_API_IPCALL_SS_INFO
      if ((call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_IP ) &&
          (call_info_ptr->mode_info.info.ip_call.call_prog_info == CM_CALL_PROG_INFO_REMOTE_ALERT))
      {
         MSG_HIGH("VoIP call prog info REMOTE ALERT ",0,0,0);
         bRemoteAlert = TRUE;
      }
#endif

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
      if (((call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) ||
           (call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_GW_PS)) &&
          (call_info_ptr->mode_info.info.gw_cs_call.call_progress == REMOTE_ALERT))
      {
         bRemoteAlert = TRUE;
      }
#endif

      if (bRemoteAlert)
      {
#ifdef FEATURE_CEPT
        uisnd_snd(UI_CALL_EAR_SND, (uint16)SND_CEPT_RING );
#else
        uisnd_snd(UI_CALL_EAR_SND, (uint16)SND_RING_BACK);
#endif
      }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
      // Turn the vocoder on only if it was not already turned on Thin UI
      else if(call_info_ptr->mode_info.info.gw_cs_call.call_progress == USER_RESUMED &&
              ((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
               (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)) &&
              !ui_vocoder_on)
      {
        uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_UNMUTED, NULL, NULL);
        MSG_HIGH("CM_CALL_EVENT_PROGRESS_INFO_IND unmute",0,0,0);
        ui_vocoder_on = TRUE;
      }
#endif
      }
      break;
#endif //defined (FEATURE_GSM) || defined (FEATURE_WCDMA) || defined (CM_API_IPCALL_SS_INFO)

    case CM_CALL_EVENT_ORIG:
      ui_origCall_id = call_info_ptr->call_id;
      if ((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
          (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
      {
        ++numberOfVoiceCalls;
        MSG_HIGH("CM_CALL_EVENT_ORIG Number voice calls %d",numberOfVoiceCalls,0,0);
      }
      break;

    // Incoming call, start ringing and autoanswer timer
    case CM_CALL_EVENT_INCOM:
      {
        //bAutoAnswer is updated in ui_init.  If the auto answer option is modified in the nv,
        //reset of the phone is required to update the bAutoAnswer value.  Getting the auto answer
        //from nv here directly is not recommending because it will cause unacceptable delay.
        if (bAutoAnswer) {
            //auto answer is enabled.  So, answer it after some time.
            (void) rex_set_timer( &ui_autoanswer_timer, UI_AUTOANSWER_TIME );
        }

        ui_incomCall_id = call_info_ptr->call_id;
        ui_incomCall_type = call_info_ptr->call_type;
        if ((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
            (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
        {
          ++numberOfVoiceCalls;
          MSG_HIGH("CM_CALL_EVENT_INCOM Number voice calls %d",numberOfVoiceCalls,0,0);
        }

        MSG_HIGH("CM_CALL_EVENT_INCOM start ring Number voice calls %d",numberOfVoiceCalls,0,0);
        if (numberOfVoiceCalls > 1)
        {
#ifdef FEATURE_CEPT
          uisnd_snd(UI_MSG_SND, (uint32)SND_CEPT_CALL_WAITING );
#else
          uisnd_snd(UI_MSG_SND, SND_CALL_WAITING);
#endif
        }
        else
        {
          uisnd_snd(UI_ALERT_SND, SND_ALERT);
        }
      }
      break;

    case CM_CALL_EVENT_ANSWER:
      if(call_info_ptr->call_id == ui_incomCall_id)
      {
         // stop any tones that are played
         uisnd_snd_stop();
      }

      // Turn the vocoder on only if it was not already turned on Thin UI
      if (((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
          (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)) &&
          !ui_vocoder_on)
      {
        uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_UNMUTED, NULL, NULL);
        MSG_HIGH("CM_CALL_EVENT_ANSWER unmute",0,0,0);
        ui_vocoder_on = TRUE;
      }
      (void) rex_clr_timer( &ui_autoanswer_timer );
      break;

    case CM_CALL_EVENT_CONNECT:
      // Turn the vocoder on only if it was not already turned on Thin UI
      if (((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
          (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)) &&
          !ui_vocoder_on)
      {
        uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_UNMUTED, NULL, NULL);
        MSG_HIGH("CM_CALL_EVENT_CONNECT unmute",0,0,0);
        ui_vocoder_on = TRUE;
      }

      if(call_info_ptr->call_id == ui_origCall_id)
      {
         // stop playing the ringback for GSM/WCDMA
         uisnd_snd_stop();
      }
      break;

    case CM_CALL_EVENT_END:
    case CM_CALL_EVENT_END_REQ:
      if( (call_info_ptr->call_id == ui_origCall_id) || (call_info_ptr->call_id == ui_incomCall_id))
      {
         // stop playing the ringback and ringing
         uisnd_snd_stop();
      }

      if (call_event == CM_CALL_EVENT_END)
      {
        if ((call_info_ptr->call_type == CM_CALL_TYPE_VOICE) ||
            (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
        {
          if (numberOfVoiceCalls)
            --numberOfVoiceCalls;

          MSG_HIGH("CM_CALL_EVENT_END(_REQ) Number voice calls %d, evt %d",numberOfVoiceCalls,call_event,0);

          // If there are no more voice calls and the vocoder is on,
          // turn it off
          if (!numberOfVoiceCalls && ui_vocoder_on)
          {
            uisnd_set_mute(UISND_MUTE_MUTED, UISND_MUTE_MUTED, NULL, NULL);
            MSG_HIGH("CM_CALL_EVENT_END(_REQ) unmute, evt %d",call_event,0,0);
            ui_vocoder_on = FALSE;
          }
        }
      }

      (void) rex_clr_timer( &ui_autoanswer_timer );
      break;


    default:
      break;
  }
}
#endif /* defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM) */

#if defined(FEATURE_MMGSDI) && defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
/* <EJECT> */
/*===========================================================================
FUNCTION is_subscription_avail

DESCRIPTION This function determines if the subscription should be available
            for the given app (GSM, UMTS, or CDMA).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if subscription is available or FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
static boolean is_subscription_avail(gsdi_card_apps_T *card_apps,
                                     uint32 app_mask, uint32 oper_mask)
{
   boolean subs_avail = FALSE;

   if (card_apps != NULL)
   {
      if ((card_apps->operational_slot & oper_mask) == oper_mask &&
          card_apps->slot1_present &&
          (card_apps->apps_enabled & app_mask) == app_mask)
      {
         subs_avail = TRUE;
      }
   }

   return subs_avail;
}

/* <EJECT> */
/*===========================================================================
FUNCTION send_subscription_avail_from_card_events

DESCRIPTION This function sends the subscription status from card events
  to the CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void send_subscription_avail_from_card_events(SubscriptionInfo *sinfo)
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  uint32 rtre_control = nv_rtre_control();
#endif
  gsdi_card_apps_T  sCardApp;
  cm_subscription_status_e_type gw_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                cdma_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;

  sinfo->cdma_subs_from_card_avail = FALSE;
  sinfo->gw_subs_from_card_avail   = FALSE;

  gsdi_get_apps_available(&sCardApp);

#ifndef FEATURE_MMGSDI_DUAL_SLOT
  sCardApp.operational_slot = GSDI_GSM_SLOT_1_MASK  |
                              GSDI_CDMA_SLOT_1_MASK |
                              GSDI_USIM_SLOT_1_MASK;
#endif

  /* gsm, umts subscription */
  if (is_subscription_avail(&sCardApp, GSDI_GSM_SIM_MASK, GSDI_GSM_SLOT_1_MASK) ||
      is_subscription_avail(&sCardApp, GSDI_USIM_APP_MASK, GSDI_USIM_SLOT_1_MASK))
  {
     gw_status = CM_SUBSCRIPTION_STATUS_CHANGE;
     sinfo->gw_subs_from_card_avail = TRUE;
  }

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* cdma subscription */
  /* check rtre */
  if ((sinfo->rtre_config == NV_RTRE_CONFIG_RUIM_OR_DROP_BACK &&
       rtre_control == NV_RTRE_CONTROL_USE_RUIM) ||
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
      sinfo->rtre_config == NV_RTRE_CONFIG_SIM_ACCESS ||
#endif
      sinfo->rtre_config == NV_RTRE_CONFIG_RUIM_ONLY)
#endif
  {
     if (is_subscription_avail(&sCardApp, GSDI_RUIM_APP_MASK, GSDI_CDMA_SLOT_1_MASK))
     {
        cdma_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        sinfo->cdma_subs_from_card_avail = TRUE;
     }
  }

  cm_ph_cmd_subscription_available(ui_subscription_cb, NULL, cm_client_id,
                                   cdma_status,
                                   gw_status
#ifdef CM_API_WLAN
  #if defined(FEATURE_WLAN)
  // the below event will be sent only for thinUI GAN build
  // rest all will follow the same existing behaviour.
                                   ,CM_SUBSCRIPTION_STATUS_CHANGE
  #else
                                   ,CM_SUBSCRIPTION_STATUS_NO_CHANGE
  #endif
#endif
                                   );
}

/* <EJECT> */
/*===========================================================================
FUNCTION simple_handle_subscription_fn

DESCRIPTION
  This function is called by the GSDI task to tell the UI when the SIM
  has completed initialization.  On the SIM initialization completed
  event, we tell CM that the subscriptions are good to go.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void simple_handle_subscription_fn(gsdi_sim_events_T event)
{
  static boolean sent_not_available = FALSE;

  gsdi_convert_detailed_err_evt_to_gen_err(event,
                                           &event);

  switch (event)
  {
    case GSDI_SIM_INIT_COMPLETED:
      sent_not_available = FALSE;
      send_subscription_avail_from_card_events(&ui_subs_info);

      /* Set operating mode. */
      break;
    case GSDI_SIM_REMOVED:
    case GSDI_DRIVER_ERROR:
    case GSDI_CARD_ERROR:
    case GSDI_SIM_INIT_COMPLETED_NO_PROV:
    case GSDI_NO_SIM_EVENT:
    case GSDI_NO_SIM:
    case GSDI_REFRESH_SIM_RESET:
    case GSDI_REFRESH_SIM_INIT:
    case GSDI_REFRESH_SIM_INIT_FCN:
    case GSDI_REFRESH_APP_RESET:
    case GSDI_REFRESH_3G_SESSION_RESET:
      if (!sent_not_available)
      {
        sent_not_available = TRUE;
        cm_ph_cmd_subscription_not_available(ui_subscription_cb, NULL,
            cm_client_id,
            ui_subs_info.cdma_subs_from_card_avail ? CM_SUBSCRIPTION_STATUS_CHANGE : CM_SUBSCRIPTION_STATUS_NO_CHANGE,
            ui_subs_info.gw_subs_from_card_avail ? CM_SUBSCRIPTION_STATUS_CHANGE : CM_SUBSCRIPTION_STATUS_NO_CHANGE
#ifdef CM_API_WLAN
            , CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
            );
      }
      break;

    default:
      break;
  }
}
#endif // FEATURE_MMGSDI && FEATURE_UI_CORE_REMOVED && !FEATURE_UI_DUALPROC_MDM
#endif // FEATURE_UI_CORE_REMOVED

#if defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
/* <EJECT> */
/*===========================================================================
FUNCTION   ui_is_in_call

DESCRIPTION
  This function checks whether we are in any calls.

DEPENDENCIES
  Currently usable only for Thin UI builds

RETURNS
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ui_is_in_call()
{
  int i;

  for (i = 0; i < CM_CALL_ID_MAX; i++) {
    if (ui_calls[i].call_state != CM_CALL_STATE_IDLE)
      return TRUE;
  }

  return FALSE;
}


/* <EJECT> */
/*===========================================================================
FUNCTION   ui_is_in_sms_call

DESCRIPTION
  This function checks whether we are in an SMS call.

DEPENDENCIES
  Currently usable only for Thin UI builds

RETURNS
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ui_is_in_sms_call()
{
  int i;

  for (i = 0; i < CM_CALL_ID_MAX; i++) {
    if (ui_calls[i].call_state != CM_CALL_STATE_IDLE &&
        ui_calls[i].call_type == CM_CALL_TYPE_SMS)
      return TRUE;
  }

  return FALSE;
}
#endif /* FEATURE_UI_CORE_REMOVED && !FEATURE_UI_DUALPROC_MDM */

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)
/*===========================================================================
FUNCTION   ui_rtre_cb

DESCRIPTION
  Callback for RTRE notification to the CM.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_rtre_cb(void *data_block, cm_ph_cmd_e_type ph_cmd,
                cm_ph_cmd_err_e_type ph_cmd_err)
{
  if (ph_cmd_err != CM_PH_CMD_ERR_NOERR)
    ERR("Error %d to rtre command %d", ph_cmd_err, ph_cmd, 0);
}

/*===========================================================================
FUNCTION   ui_convert_nv_to_cm_rtre

DESCRIPTION
  Convert the RTRE define used by NV to the one used by CM

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
cm_rtre_config_e_type ui_convert_nv_to_cm_rtre(uint32 nv_rtre)
{
   cm_rtre_config_e_type cm_rtre;

   switch (nv_rtre)
   {
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
   case NV_RTRE_CONFIG_SIM_ACCESS:
      cm_rtre = CM_RTRE_CONFIG_SIM_ACCESS;
      break;
#endif

   case NV_RTRE_CONFIG_RUIM_ONLY:
      cm_rtre = CM_RTRE_CONFIG_RUIM_ONLY;
      break;

   case NV_RTRE_CONFIG_NV_ONLY:
      cm_rtre = CM_RTRE_CONFIG_NV_ONLY;
      break;

   case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
   default:
      cm_rtre = CM_RTRE_CONFIG_RUIM_OR_DROP_BACK;
      break;
   }

   return cm_rtre;
}

/*===========================================================================
FUNCTION   ui_init_rtre

DESCRIPTION
  Initialize RTRE in CM and NV.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_init_rtre(void)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  //Get RTRE configuration
  result = ui_get_nv (NV_RTRE_CONFIG_I, &nvi);

  if(result != NV_DONE_S) {  /* write to NV */
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
    nvi.rtre_config = NV_RTRE_CONFIG_NV_ONLY;
#else
	//NV_RTRE_CONFIG_RUIM_ONLY
    nvi.rtre_config = NV_RTRE_CONFIG_RUIM_OR_DROP_BACK;		//Add By zzg 2010_09_08
#endif
    (void) ui_put_nv (NV_RTRE_CONFIG_I, &nvi);
  }

#if defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
  ui_subs_info.rtre_config = nvi.rtre_config;
#endif

  cm_ph_cmd_change_rtre_config(ui_rtre_cb, NULL, cm_client_id,
                               ui_convert_nv_to_cm_rtre(nvi.rtre_config));
}
#endif // FEATURE_UIM_RUN_TIME_ENABLE
#ifdef FEATURE_LCD_TOUCH_ENABLE

boolean oemui_pen_callback(pen_event_type pen_event)
{
	//MSG_FATAL("oemui_pen_callback",0,0,0);
	rex_set_sigs(&ui_tcb,UI_GET_PEN_SIG);
	return TRUE;
}

static void oemui_touchpadinit(void)
{
	//MSG_FATAL("oemui_touchpadinit",0,0,0);
    touchpad_register(oemui_pen_callback);
    touchpad_set_scan_mode( PEN_NORMAL_MODE);
}

static void oemui_post_pointer_data(void)
{
	pen_event_type pen_event;
	uint32 dwParam;
	AEEEvent evtCode;
	
	//IBACKLIGHT_Enable(gpBacklight);
	pointer_keyeablebacklight();
	VoteForSleep(FALSE);
   // MSG_FATAL("oemui_post_pointer_data",0,0,0);
    
	while(touchpad_get_pen_position(&pen_event))
	{
		if(touchpad_get_scan_mode() == PEN_NORMAL_MODE)
		{
			if(pen_event.pen_state == PEN_DOWN)
			{
				evtCode = EVT_POINTER_DOWN;
			}
			else if(pen_event.pen_state == PEN_MOVE)
			{
				evtCode = EVT_POINTER_MOVE;
			}
			else
			{
				evtCode = EVT_POINTER_UP;
			}
            AEE_Event( EVT_KEY,         AVK_LAST, 0 );
            AEE_POINTER_SEND_XY(evtCode,pen_event.pen_x,pen_event.pen_y);
		}
		else
		{
           // ERR("EVENT Pen not in HW Mode",0,0,0);
	    }
	}
}

#endif
/* <EJECT> */
/*===========================================================================
FUNCTION   ui_init

DESCRIPTION
  Initialize the ui task.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_init( void )
{
#if defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
  int i;
  nv_item_type nvi;
#endif
 


#ifdef CUST_EDITION
#ifdef FEATURE_UIM_TOOLKIT
    (void) q_init(&ui_cmd_q);
#endif
#endif

  /* Initialize timers */
  rex_def_timer( &ui_rpt_timer, &ui_tcb, UI_RPT_TIMER_SIG );

#if !defined(FEATURE_UI_CORE_REMOVED) && defined (FEATURE_NEW_SLEEP_API)
  gNewSleepHandle = sleep_register("UI_TASK", FALSE);
#endif /* FEATURE_NEW_SLEEP_API */

  /* Wait for start signal from task controller */
  (void) task_start( UI_RPT_TIMER_SIG, DOG_UI_RPT, &ui_rpt_timer );

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  ** and start our first tick timer
  */
  (void) rex_set_sigs( &ui_tcb, UI_RPT_TIMER_SIG );

#if !defined(FEATURE_UI_DUALPROC_APPS) || defined(FEATURE_UI_CORE_REMOVED) || defined (FEATURE_UIM_RUN_TIME_ENABLE)
  if (cm_client_id == CM_CLIENT_ID_ANONYMOUS)
    cm_client_init(CM_CLIENT_TYPE_UI, &cm_client_id);
#endif //!defined(FEATURE_UI_DUALPROC_APPS) || defined(FEATURE_UI_CORE_REMOVED)

#if defined(FEATURE_MMGSDI) && defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
  gsdi_init(TRUE);
  gsdi_reg_callback(simple_handle_subscription_fn);
#endif

// init RTRE
#if defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)
#ifdef FEATURE_MMGSDI
  ui_subs_info.cdma_subs_from_card_avail = FALSE;
  ui_subs_info.gw_subs_from_card_avail   = FALSE;
#endif
#endif /* FEATURE_UI_CORE_REMOVED && !FEATURE_UI_DUALPROC_MDM */

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  ui_init_rtre();
#endif

#if !defined(FEATURE_UI_DUALPROC_APPS)
#ifndef FEATURE_MANGO_UI
  //init provisioning here
  ui_init_provisioning(cm_client_id);

#if defined(FEATURE_GSTK_FDN_CC_SUPPORT)
  cm_nc_reg_with_call_control_mod(ui_nc_type_allowed_and_callcontrol_cb,
                                  ui_nc_call_control_preferences);
#else
  cm_nc_reg_with_call_control(ui_nc_type_allowed_and_callcontrol_cb,
                              ui_nc_call_control_preferences);
#endif /* FEATURE_GSTK_FDN_CC_SUPPORT */
#endif /* FEATURE_MANGO_UI */
#else  /* !defined(FEATURE_UI_DUALPROC_APPS) */
#ifdef FEATURE_UI_PBM
  pbmconvert_init_conversion();
#endif /* FEATURE_UI_PBM */
#endif /* !defined(FEATURE_UI_DUALPROC_APPS) */

#if !defined(FEATURE_UI_CORE_REMOVED)

  CoreTask_init();
#else
#if !defined(FEATURE_UI_DUALPROC_MDM)
  /* Initialize timers */
  rex_def_timer( &ui_autoanswer_timer, &ui_tcb, UI_AUTOANSWER_SIG );

  for (i = 0; i < CM_CALL_ID_MAX; i++) {
    memset(&ui_calls[i], 0, sizeof(cm_mm_call_info_s_type));
    ui_calls[i].call_state = CM_CALL_STATE_IDLE; /* Just to be sure */
  }

  uisnd_snd_init();

  cm_mm_client_ss_reg(cm_client_id, ui_ss_event_cb, CM_CLIENT_EVENT_REG,
                      CM_SS_EVENT_SRV_CHANGED, CM_SS_EVENT_SRV_NEW,
                      ui_ss_err_cb);

  cm_mm_client_call_reg(cm_client_id, ui_call_event_cb, CM_CLIENT_EVENT_REG,
                        CM_CALL_EVENT_ORIG, CM_CALL_EVENT_EXIT_TC,
                        NULL);

  cm_client_act(cm_client_id);

#if !defined(FEATURE_PLT)
  if((ui_get_nv(NV_FTM_MODE_I, &nvi) != NV_DONE_S) || (nvi.ftm_mode != FTM_MODE))
  {
    if (ui_check_provisioning()) {
      cm_ph_cmd_oprt_mode(ui_subscription_cb, NULL, cm_client_id,
                          SYS_OPRT_MODE_ONLINE );
    } else {
      cm_ph_cmd_oprt_mode(ui_subscription_cb, NULL, cm_client_id,
                          SYS_OPRT_MODE_OFFLINE );
    }

//If RUIM is not enabled and the UE still supports CDMA. Send out the CDMA
// subscription
#if !defined(FEATURE_UIM_RUN_TIME_ENABLE) && defined(FEATURE_CDMA) && defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)

    cm_ph_cmd_subscription_available(ui_subscription_cb, NULL, cm_client_id,
      CM_SUBSCRIPTION_STATUS_CHANGE,
      CM_SUBSCRIPTION_STATUS_NO_CHANGE
#ifdef CM_API_WLAN
      , CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
     );

#endif //!defined(FEATURE_UIM_RUN_TIME_ENABLE) && defined(FEATURE_CDMA) && (defined(FEATURE_UI_CORE_REMOVED) && !defined(FEATURE_UI_DUALPROC_MDM)


#if !defined(FEATURE_MMGSDI) || \
    (defined(FEATURE_UI_CORE_REMOVED) && defined(FEATURE_UIM_RUN_TIME_ENABLE) && !defined(FEATURE_UI_DUALPROC_MDM))

#if defined(FEATURE_UI_CORE_REMOVED) && defined(FEATURE_UIM_RUN_TIME_ENABLE) && !defined(FEATURE_UI_DUALPROC_MDM)
    if (ui_subs_info.rtre_config == NV_RTRE_CONFIG_NV_ONLY)
#endif
    {
    cm_ph_cmd_subscription_available(ui_subscription_cb, NULL, cm_client_id,
      CM_SUBSCRIPTION_STATUS_CHANGE,
      CM_SUBSCRIPTION_STATUS_NO_CHANGE
#ifdef CM_API_WLAN
      , CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
      );
    }
#endif /* !FEATURE_MMGSDI || (FEATURE_UIM_RUN_TIME_ENABLE && FEATURE_UI_CORE_REMOVED && !FEATURE_UI_DUALPROC_MDM)*/
  }
  else /* ftm_get_mode() == FTM_MODE */
  {
    cm_ph_cmd_oprt_mode(ui_subscription_cb, NULL, cm_client_id,
                        SYS_OPRT_MODE_FTM );
  }

#endif // FEATURE_PLT

  //load the auto answer option from nv
  if(ui_get_nv(NV_AUTO_ANSWER_I, &nvi) == NV_DONE_S)
  {
    bAutoAnswer = nvi.auto_answer.enable;
  }
#endif /* !defined(FEATURE_UI_DUALPROC_MDM) */
#endif /* !defined(FEATURE_UI_CORE_REMOVED) */

} /* end of ui_init */

/* <EJECT> */
/*===========================================================================
FUNCTION UI_TASK

DESCRIPTION
  The main user interface function.  This is the big loop outside of the
  state machine which handles signals and all sorts of inputs.  It mostly
  serves to convert inputs into events for the state machine, but it
  also handles keypad incidentals - backlighting and key beeps.

DEPENDENCIES
  REX is running.  NV and DIAG and HS are running.

RETURN VALUE
  Never exits.

SIDE EFFECTS
  Plays with variable ui.

===========================================================================*/
void ui_task (
  dword dummy                     /*lint -esym(715,dummy) */
                                  /*ignore fact that this isn't used */
)
{
  rex_sigs_type waitMask;         /* hold signals */
#if MIN_BREW_VERSION(4,0)
  int           nErr;
  SigDispatch   sigDisp;
#endif

#if !MIN_BREW_VERSION(4,0) && defined(FEATURE_IXCONTENTSVC)
  ixipc_sig_type  recv_signal_type;
  uint32          kx_sigs;
#endif

  ui_init();                      /* initialize task. */
#ifdef FEATURE_LCD_TOUCH_ENABLE

  oemui_touchpadinit();
  #endif
#ifdef srIMON
  OEMInterceptModules_Init();     /* initialize STRIDE */
#ifdef T_WINNT
//Cannot reset UI under LTK, so we need this to make sure registrations are caught from LTK
  IMStartupSync();
#endif
#endif

#if MIN_BREW_VERSION(4,0)
  /* Initialize the signal dispatcher */
#if MIN_BREW_BUILD(4,0,1,26)
  nErr = SigDispatch_Init(gpiRexEnv, &sigDisp);
#else
  nErr = SigDispatch_Init(gpiRexEnv, &sigDisp, AEE_APP_SIG);
#endif

   if (AEE_SUCCESS != nErr) {
     MSG_ERROR("Cannot Initialize Signal Dispatcher", 0, 0, 0);
     SigDispatch_Final(&sigDisp);
     return;
  }
#endif

    /* Initialize WaitMask */
  waitMask = (UI_RPT_TIMER_SIG | TASK_OFFLINE_SIG | TASK_STOP_SIG);

#if !defined (FEATURE_UI_DUALPROC_MDM)
  waitMask |= UI_IPC_SIG;
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE

  waitMask |= UI_GET_PEN_SIG;
#endif
#ifndef FEATURE_UI_CORE_REMOVED
  waitMask |= CoreApp_Getsigs();
#else
#if !defined (FEATURE_UI_DUALPROC_MDM)
  waitMask |= UI_AUTOANSWER_SIG ;
#endif //FEATURE_UI_DUALPROC_MDM
#endif /* FEATURE_UI_CORE_REMOVED. */

#ifdef CUST_EDITION
#ifdef FEATURE_UIM_TOOLKIT
   waitMask |= UI_CMD_Q_SIG;
#endif
#endif

  for( ;; ) {                 /* Wait on REX signals, repeat forever */
     rex_sigs_type sigs;      /* hold signals */

#if MIN_BREW_VERSION(4,0)
     SignalData    sd;
     sigs = waitMask;
     nErr = SigDispatch_Wait(&sigDisp, &sigs, &sd);
#else

#ifdef FEATURE_IXCONTENTSVC
     sigs    = waitMask;    /* Rex signals to wait on */
     kx_sigs = UI_IPC_SIG ; /* Kx signals to wait on */

     recv_signal_type = ixipc_sig_wait( &sigs, &kx_sigs ) ;

     if ( recv_signal_type == IXIPC_KX_SIG_TYPE ) {
       /* Only KxIPC signal is set. As our processing is similar
          for Rex IPC and KxIPC, we only need to make it look like
          Rex IPC signal. */
       sigs = UI_IPC_SIG;
     }
     else if ( recv_signal_type == IXIPC_REX_KX_SIG_TYPE ) {
       /* Rex and Kx handling of IPC is the same. */
       sigs |= UI_IPC_SIG;
     }
#else
     sigs = rex_wait(waitMask);
#endif /* FEATURE_IXCONTENTSVC */
#endif /* MIN_BREW_VERSION(4,0) */

    /* Process the signals */
    HandleSignals( sigs );

#if MIN_BREW_VERSION(4,0)
    if (AEE_SUCCESS == nErr) {
        VoteForSleep(FALSE); /* not okay to sleep */
        SigDispatch_Dispatch(&sigDisp, &sd);
        VoteForSleep(TRUE); /* okay to sleep */
    }
#endif

  }
}
