/* =======================================================================
                          qtv_conc_mgr.cpp

DESCRIPTION
  Implementation of the QTV concurrency manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  To use the services provided, players must register with the concurrency
  manager through init. 

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia/qtv/concurrencymgr/main/latest/src/qtv_conc_mgr.cpp#26 $
$DateTime: 2010/05/04 00:03:26 $
$Change: 1286941 $

========================================================================== */
/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes 
** in this file!
*/
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_conc_mgr.h"
#include "Mpeg4Player.h"
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API 
#include "QtvRealPlayer.h"
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
#include "qtv_diag_main.h"

/* =======================================================================
                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.
========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef PLATFORM_LTK
#error code not present
#endif // PLATFORM_LTK
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
/* Qtv client registered with CM */
cm_client_id_type qtv_cm_client = CM_CLIENT_ID_ANONYMOUS;

qtv_conc_mgr::suspend_type qtv_conc_mgr::suspend_due_to;
qtv_conc_mgr::conc_play_mode_type qtv_conc_mgr::play_mode;
qtv_conc_mgr::in_call_state_type qtv_conc_mgr::in_call_state;
qtv_conc_mgr::suspend_state_type qtv_conc_mgr::suspend_state;
qtv_conc_mgr::call_info_type *qtv_conc_mgr::call_info_table_ptr = NULL;
bool qtv_conc_mgr::is_qtv_suspended_internally = false;

Mpeg4Player *qtv_conc_mgr::av_player_ptr = NULL;
const char *qtv_conc_mgr::current_MIME_type_ptr = NULL;

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
/* Keeps track of whether Real player was in the playing state before 
** it was suspended.
*/
extern boolean real_playingBeforeSuspend;
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
#define IS_REAL_PLAYER \
  (current_MIME_type_ptr && \
   strcmp(current_MIME_type_ptr, REAL_MEDIA_MIME_TYPE) == 0)

#define IS_PLAYER_REGISTERED \
  ((context_ptr == av_player_ptr) && \
   current_MIME_type && \
   current_MIME_type_ptr && \
   (strcmp(current_MIME_type, current_MIME_type_ptr) == 0))

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/* =======================================================================
FUNCTION: 
  qtv_conc_mgr::init

DESCRIPTION:
  If not already initialized, initializes the concurrency manager and 
  registers with CM.

PARAMETERS:
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.
  const char *current_MIME_type
    Used to determine the player is Qtv or Real.
  uint32 num_qtv_instance 
    Number of Qtv player instances. 

RETURN VALUE:
  bool
    True if we were able to successfully register with CM, false otherwise.
========================================================================== */
void qtv_conc_mgr::init(
  Mpeg4Player *context_ptr,
  const char *current_MIME_type,
  uint32 num_qtv_instance)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
               "qtv_conc_mgr::init");

  /* Every instance of Qtv will make a call to init(), but concurrency 
  ** manager will be initialized only when the first Qtv instance is 
  ** created.
  */
  if (num_qtv_instance == 1)
  {
    /* Mpeg4Player handle */
    av_player_ptr = context_ptr;

    /* MIME type */
    current_MIME_type_ptr = current_MIME_type;

    /* Initialize the in_call_state */
    in_call_state = NOT_IN_CALL;

    /* Default play_mode */
    play_mode = PLAY_NORMAL;

    /* Not suspended */
    suspend_due_to = NOT_SUSPENDED_INTERNALLY;

    suspend_state = STATE_NOT_SUSPENDED;
  }
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::register_with_call_mgr

DESCRIPTION:
  Registering the qtv with call manager.

PARAMETERS:
  None.
    
RETURN VALUE:
  None.
========================================================================== */
bool qtv_conc_mgr::register_with_call_mgr()
{
  bool status = false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "qtv_conc_mgr::register_with_call_mgr()");
#ifndef FEATURE_QTV_DISABLE_CONC_MGR_CM_REGISTRATION
    /* Initialize Qtv as a client with CM */
    if (init_qtv_with_cm())
    {
      /* Register the callback with CM and the events to be notified for */
      if (register_qtv_cb_with_cm())
      {
        /* Activate client */
        if (activate_qtv_with_cm())
        {
        status = true;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "activate_qtv_with_cm DONE");
      }
    }
  }
#else
  /* If CM registration is disabled then set status to true and return. */
  status = true;
#endif
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::terminate

DESCRIPTION:
  De-activate and release the Qtv client from CM.

PARAMETERS:
  uint32 num_qtv_instance 
    Number of Qtv player instances. 

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::terminate(uint32 num_qtv_instance)
{
  /* Concurrency manager is terminated only when the last Qtv instance is 
  ** terminated.
  */
  if (num_qtv_instance == 1)
  {
    /* Mpeg4Player handle */
    av_player_ptr = NULL;

    /* MIME type */
    current_MIME_type_ptr = NULL;

    /* Initialize the in_call_state */
    in_call_state = NOT_IN_CALL;

    /* Default play_mode */
    play_mode = PLAY_NORMAL;

    /* Not suspended */
    suspend_due_to = NOT_SUSPENDED_INTERNALLY;

    suspend_state = STATE_NOT_SUSPENDED;

#ifndef FEATURE_QTV_DISABLE_CONC_MGR_CM_REGISTRATION
    /* De-activate and release Qtv client from CM */
    deregister_qtv_from_cm();
#endif /* FEATURE_QTV_DISABLE_CONC_MGR_CM_REGISTRATION */
  }
}

/* =======================================================================
FUNCTION: 
  qtv_conc_mgr::can_process_command

DESCRIPTION:
  Informs the player if a play command can be handled. It looks up the 
  play mode and call state to determine if play can be supported.

PARAMETERS:
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.
  const char *current_MIME_type
    Used to determine the player is Qtv or Real.

RETURN VALUE:
  bool
    True if the command can be processed, false otherwise.
========================================================================== */
bool qtv_conc_mgr::can_process_command(
  Mpeg4Player *context_ptr,
  const char *current_MIME_type)
{
  bool status;

  /* Go through the process of updating information only if the player is 
  ** registered with the concurrency manager.
  */
  if (IS_PLAYER_REGISTERED)
  {
    /* Update the play_mode based on the in_call_state. This will also update
    ** the in_call_state. 
    */
    update_play_mode();

    if (suspend_state == STATE_INT_SUSPENDED ||
        suspend_state == STATE_SUSPENDED)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                   "can_process_command: Can not process command");

      status = false;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                   "can_process_command: Can process command");

      status = true;
    }
  }
  else
  {
    /* If the player is not registered means it has the highest priority,
    ** allow it to process commands.
    */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
                 "can_process_command: High priority instance, process command");

    status = true;
  }
  return status;
}

/* =======================================================================
FUNCTION: 
  qtv_conc_mgr::call_event_notify_cb

DESCRIPTION:
  Call-back function registered with CM. Concurrency manager is notified
  of the call events through this call-back.

PARAMETERS:
  cm_call_event_e_type call_event
    The current call event CM is informing concurrency manager about.
  cm_mm_call_info_s_type *call_info_ptr
    Information associated with the above call event.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::call_event_notify_cb(
  cm_call_event_e_type call_event,
  cm_mm_call_info_s_type *call_info_ptr)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
               "qtv_conc_mgr::call_event_notify_cb:Mpeg4Player ptr %d",
                av_player_ptr);

  if (call_info_ptr)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
           "qtv_conc_mgr::call_event_notify_cb: call type %d,call event %d",
            call_info_ptr->call_type, call_event);
    switch (call_info_ptr->call_type)
    {
      case CM_CALL_TYPE_VOICE:
      case CM_CALL_TYPE_CS_DATA:
      case CM_CALL_TYPE_EMERGENCY:
#ifndef PLATFORM_LTK
      case CM_CALL_TYPE_VT:
      case CM_CALL_TYPE_VT_LOOPBACK:
      case CM_CALL_TYPE_VS:
#endif
      {
        cm_call_id_type call_id = 0;

        if (call_info_ptr->call_id < CM_CALL_ID_MAX)
        {
          call_id = call_info_ptr->call_id;
        }
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                        "call_event_notify_cb: call id out of bounds = %d",
                        call_info_ptr->call_id);
        }

        if(call_info_table_ptr != NULL && call_id < CM_CALL_ID_MAX)
        {
           call_info_table_ptr[call_id].call_state = call_info_ptr->call_state;
           call_info_table_ptr[call_id].call_type  = call_info_ptr->call_type;
           call_info_table_ptr[call_id].mode_info  = call_info_ptr->mode_info;
        }

        process_call_event(call_event, call_id);
        break;
      }

      default:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                      "call_event_notify_cb: Ignoring call_type = %d", 
                      call_info_ptr->call_type);
        break;
      }
    } /* switch (call_info_ptr->call_type) */
  }
} 

/* =======================================================================
FUNCTION: 
  qtv_conc_mgr::process_call_event

DESCRIPTION:
  Processes call events. 
  It suspends the player for an incoming call.
  If the application (Mediaplayer) is not suspended, resumes playback 
  when the call ends. 

PARAMETERS:
  cm_call_event_e_type call_event
    The call event that is to be processed.
  cm_call_id_type call_id
    Id of the call being processed.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::process_call_event(
  cm_call_event_e_type call_event,
  cm_call_id_type call_id)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
               "qtv_conc_mgr::process_call_event");

  switch (call_event)
  {
    case CM_CALL_EVENT_INFO: /* Received if Qtv registers while in a call */
    case CM_CALL_EVENT_INCOM:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                   "process_call_event: Call event info/incom");

      /* If player not suspended in a call set up response then suspend here */
      if(!is_qtv_suspended_internally)
      {
        set_suspend_type(SUSPENDED_INTERNALLY,
                         av_player_ptr,
                         current_MIME_type_ptr);
        suspend(av_player_ptr);
      }
      is_qtv_suspended_internally = false;
      break;
    }

    case CM_CALL_EVENT_SETUP_RES:
    {
      if(call_info_table_ptr != NULL && call_id < CM_CALL_ID_MAX)
      {
        if (call_info_table_ptr[call_id].mode_info.info.gw_cs_call.accept)
        {
          set_suspend_type(SUSPENDED_INTERNALLY,
                           av_player_ptr,
                           current_MIME_type_ptr);
          suspend(av_player_ptr);
          is_qtv_suspended_internally = true;

          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                       "process_call_event: Call set up response: suspending player internally ");
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                       "process_call_event: Setup response with accept false");
        }
      }
      break;
    }

    case  CM_CALL_EVENT_END:
    {
      /* Check if all calls in the call table are idle. In the case where
      ** multiple calls are active, the call end maybe for one of them
      ** and we don't want to change the suspend state till all calls are 
      ** ended.
      */ 
      if (all_calls_are_idle())
      {
        process_call_end();
      }
      else 
      {
        /* One or more calls are still active */
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
                     "process_call_event: Call end, calls still active");
      }
      is_qtv_suspended_internally = false;
      break;
    }

    default:
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                    "process_call_event: Ignoring call_event = %d", 
                    call_event);
      break;
    }
  } /* switch (call_event) */
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::update_play_mode

DESCRIPTION:
  Updates the play mode based on the in-call state and the video codec.

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::update_play_mode()
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                "qtv_conc_mgr::update_play_mode = %d", play_mode);

  /* Get the current in_call_state before determining play_mode */
  update_incall_state();

  if (in_call_state == NOT_IN_CALL)
  {
    /* If we're not in a call implies that we no longer need to be 
    ** suspended. 
    */
    set_suspend_type(NOT_SUSPENDED_INTERNALLY,
                       av_player_ptr,
                       current_MIME_type_ptr);

    play_mode = PLAY_NORMAL;
  }
  else
  {
    /* If we're in a call check if in-call playback is supported */
    if (in_call_audio_is_supported())
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                   "update_play_mode: In-call audio is supported");

      /* If we can support playback then set internally suspended to 
      ** false, so that we can playback while we're in call.
      */
      set_suspend_type(NOT_SUSPENDED_INTERNALLY,
                         av_player_ptr,
                         current_MIME_type_ptr);

      play_mode = PLAY_MIXED_AUDIO;
    } 
    else if ( in_call_video_is_supported() )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                   "update_play_mode: In-call video is supported");

      /* If we can support playback then set internally suspended to 
      ** false, so that we can playback while we're in call.
      */
      set_suspend_type(NOT_SUSPENDED_INTERNALLY,
                       av_player_ptr,
                       current_MIME_type_ptr);

      play_mode = PLAY_CONCURRENT;
    }
    else
    {
      /* In-call behavior is not supported, prevent playback */
      play_mode = PLAY_NOT_SUPPORTED;
    }
  }

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "update_play_mode: play_mode = %d", play_mode);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::get_suspend_state

DESCRIPTION:
  Returns information about what caused the player to suspend.

PARAMETERS:
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.
  const char *current_MIME_type
    Used to determine the player is Qtv or Real.

RETURN VALUE:
  qtv_conc_mgr::suspend_state_type
    Enum of suspend state types.
========================================================================== */
qtv_conc_mgr::suspend_state_type qtv_conc_mgr::get_suspend_state(
  Mpeg4Player *context_ptr,
  const char *current_MIME_type)
{
  qtv_conc_mgr::suspend_state_type status;

  /* Return the suspended state only if the player is registered */
  if (IS_PLAYER_REGISTERED)
  {
    status = suspend_state;
  }
  else
  {
    /* If player is not registered means it has the highest priority,
    ** ignore suspend_state and return not suspended.
    */
    status = STATE_NOT_SUSPENDED;
  }
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::set_suspend_type

DESCRIPTION:
  Used to inform concurrency manager about who suspended the player.

PARAMETERS:
  qtv_conc_mgr::suspend_type
    Enum of suspend types.
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.
  const char *current_MIME_type
    Used to determine the player is Qtv or Real.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::set_suspend_type(
  suspend_type set_suspend_due_to,
  Mpeg4Player *context_ptr,
  const char *current_MIME_type)
{
  /* Set the suspend type only for the registered player */
  if (IS_PLAYER_REGISTERED)
  {
    suspend_due_to = set_suspend_due_to;

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                  "set_suspend_type: Set to = %d", suspend_due_to);

    /* Based on who called the suspend update suspend_state */
    switch(suspend_due_to)
    {
      case SUSPENDED_EXTERNALLY:
      {
        update_state_for_suspended_ext();
        break;
      }

      case NOT_SUSPENDED_EXTERNALLY:
      {
        update_state_for_not_suspended_ext();
        break;
      }

      case SUSPENDED_INTERNALLY:
      {
        update_state_for_suspended_int();
        break;
      }

      case NOT_SUSPENDED_INTERNALLY:
      {
        update_state_for_not_suspended_int();
        break;
      }

      default:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                      "set_suspend_type: Unknown suspend type = %d", 
                      suspend_due_to);
        break;
      }
    } /* switch(suspend_due_to) */
  }
  else
  {
    /* If player is not registered means it has highest priority,
    ** ignore suspend type.
    */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
                  "set_suspend_type: High priority instance, ignoring");
  }
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::update_incall_state

DESCRIPTION:
  Updates the in-call state based on a snapshot of the call manager's
  call table.

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::update_incall_state()
{
  /* Initialize the in_call_state */
  in_call_state = NOT_IN_CALL; 

  /* Update the in_call_state based on the call table */
  for (cm_call_id_type call_id = 0; call_id < CM_CALL_ID_MAX; call_id++)
  {
    if(call_info_table_ptr != NULL && call_id < CM_CALL_ID_MAX)
    {
      if (call_info_table_ptr[call_id].call_state == CM_CALL_STATE_CONV) 
      {
        if (call_info_table_ptr[call_id].call_type == CM_CALL_TYPE_EMERGENCY)
        {
          /* Emergency call, do not allow play */
          in_call_state = IN_CALL_NOT_CONCURRENT;
          break;
        }
        else if (call_info_table_ptr[call_id].call_type == CM_CALL_TYPE_VOICE)
        {
          in_call_state = get_voice_call_state(call_id);
        }
        else
        {
          /* Don't allow playback for VT/VS calls */
          in_call_state = IN_CALL_NOT_CONCURRENT;
        }
      }
      else if (call_info_table_ptr[call_id].call_state == CM_CALL_STATE_INCOM ||
               call_info_table_ptr[call_id].call_state == CM_CALL_STATE_ORIG)
      {
        in_call_state = IN_CALL_NOT_CONCURRENT;
      }
    }
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                "update_incall_state: in_call_state = %d", 
                in_call_state);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::get_voice_call_state

DESCRIPTION:
  Returns the in-call state for a voice call. 

PARAMETERS:
  cm_call_id_type call_id
    Call-id for which the voice call state is requested.

RETURN VALUE:
  qtv_conc_mgr::in_call_state_type
    Enum of in-call states.
========================================================================== */
qtv_conc_mgr::in_call_state_type qtv_conc_mgr::get_voice_call_state(
  cm_call_id_type call_id)
{
  in_call_state_type voice_call_state = IN_CALL_NOT_CONCURRENT;

  Media::CodecType eVoiceCodecType = Media::UNKNOWN_CODEC;
  
  if(call_info_table_ptr != NULL && call_id < CM_CALL_ID_MAX)
  {
    if (call_info_table_ptr[call_id].mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
    {
#ifdef FEATURE_QTV_IN_CALL_1X
#error code not present
#endif /* FEATURE_QTV_IN_CALL_1X */
    }
#ifdef FEATURE_QTV_IN_CALL_UMTS
#error code not present
#endif /* FEATURE_QTV_IN_CALL_UMTS */
    else
    {
      voice_call_state = IN_CALL_NOT_CONCURRENT;
      eVoiceCodecType = Media::UNKNOWN_CODEC;
  
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                    "get_voice_call_state: Unknown voice call type = %d",
                    call_info_table_ptr[call_id].mode_info.info_type);
    }
  }

  if (av_player_ptr)
  {
        av_player_ptr->SetIncallVoiceCodec(eVoiceCodecType);
  }			 
  
  return voice_call_state;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::get_play_mode

DESCRIPTION:
  Returns the current play_mode.

PARAMETERS:
    Mpeg4Player *context_ptr
      This is the Mpeg4Player handle.
    const char *current_MIME_type
      Used to determine the player is Qtv or Real.

RETURN VALUE:
  qtv_conc_mgr::conc_play_mode_type
    Enum of play modes. 
========================================================================== */
qtv_conc_mgr::conc_play_mode_type qtv_conc_mgr::get_play_mode(
  Mpeg4Player *context_ptr,
  const char *current_MIME_type)
{
  qtv_conc_mgr::conc_play_mode_type status;

  /* Return the play_mode based on the in_call_state only if the player is
  ** registered.
  */
  if (IS_PLAYER_REGISTERED)
  {
    status = play_mode;
  }
  else
  {
    /* If the player is not registered means it has the highest priority,
    ** ignore in_call_state and allow playback.
    */
    status = PLAY_NORMAL;
  }
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::suspend

DESCRIPTION:
  Suspends the registered player.

PARAMETERS:
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::suspend(Mpeg4Player *context_ptr)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
               "qtv_conc_mgr::suspend: Suspending internally");

  if (context_ptr) // Mpeg4Player
  {
    /* Suspending internally, Notify SUSPENDING so that key presses are 
    ** disabled in the time window between internal suspend and mediaplayer
    ** being suspended.
    */
    context_ptr->Notify(QtvPlayer::QTV_PLAYER_STATUS_SUSPENDING);

    QTV_SUSPEND_type *event_ptr = QCCreateMessage(QTV_SUSPEND, 
                                                  context_ptr);

    if (event_ptr)
    {
      suspend_state_type suspend_type =
        get_suspend_state(context_ptr,QTV_PLAYER_MIME_TYPE) ;

      if (suspend_type == STATE_INT_SUSPENDED)
      {
        event_ptr->bSuspendIsExternal = false ;
      }
      else 
      {
        event_ptr->bSuspendIsExternal = true ;
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, 
          QTVDIAG_PRIO_LOW, "suspendtype %d",event_ptr->bSuspendIsExternal);

      context_ptr->PostMessage(event_ptr);
    }
  }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if (IS_REAL_PLAYER) // Real player 
  {
    /* Suspending internally, Notify SUSPENDING so that key presses are 
    ** disabled in the time window between internal suspend and mediaplayer
    ** being suspended.
    */
    QtvRealPlayer_Notify(QtvPlayer::QTV_PLAYER_STATUS_SUSPENDING);

    QtvRealPlayer_Suspend();
  }
#endif /* FEATURE_REAL_PLAYER_USES_QTV_API */
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::resume

DESCRIPTION:
  Issues play to the registered player if the player was in the 
  PLAYING/BUFFERING state, else notifies pause to the application.

PARAMETERS:
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::resume(Mpeg4Player *context_ptr)
{
  int32 start_time = -1;
  int32 stop_time = -1;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
               "qtv_conc_mgr::resume: Internal resume");

  /* This is an internal resume */
  set_suspend_type(NOT_SUSPENDED_INTERNALLY, 
                   context_ptr, 
                   current_MIME_type_ptr);

  if (context_ptr) // Mpeg4Player
  {
    QTV_RESUME_type *event_ptr = QCCreateMessage(QTV_RESUME,
                                                 av_player_ptr);
    if (event_ptr)
    {
      context_ptr->PostMessage(event_ptr);
    }
  }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if (IS_REAL_PLAYER) // Real player
  {
    /* Issue a play only if we were playing when we were suspended. */
    if(real_playingBeforeSuspend)
    {
      QtvRealPlayer_PlayClip(start_time, stop_time);
    }
    else if (QtvRealPlayer_IsIdle())
    {
      /* Notify idle, so that app knows Qtv is no longer suspended */
      notify_idle(context_ptr);

      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                   "resume: App not suspended, player in IDLE state");
    }
    else
    {
      /* Notify pause, so that the app knows it is in the enabled state. */ 
      notify_pause(context_ptr);

      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                   "resume: App not suspended, player not in"\
                   "playing state");
    }
  }
#else
  QTV_USE_ARG1(start_time);
  QTV_USE_ARG1(stop_time); 

#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */ 
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::notify_pause

DESCRIPTION:
  Notifies pause so that the application knows that it is enabled.

PARAMETERS:
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::notify_pause(Mpeg4Player *context_ptr)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
               "qtv_conc_mgr::notify_pause: Internal pause");

  /* No longer internally suspended */
  set_suspend_type(NOT_SUSPENDED_INTERNALLY,
                   context_ptr,
                   current_MIME_type_ptr);

  /* Notify pause, so that the app knows it is in the enabled state. */ 
  if (context_ptr) // Mpeg4Player
  {
    context_ptr->Notify(QtvPlayer::QTV_PLAYER_STATUS_PAUSED);
  }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if (IS_REAL_PLAYER) // Real player
  {
    QtvRealPlayer_Notify(QtvPlayer::QTV_PLAYER_STATUS_PAUSED);
  }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::notify_idle

DESCRIPTION:
  Notifies idle so that the application knows that Qtv is no longer 
  internally suspended.

PARAMETERS:
  Mpeg4Player *context_ptr
    This is the Mpeg4Player handle.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::notify_idle(Mpeg4Player *context_ptr)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
               "qtv_conc_mgr::notify_idle");

  /* No longer internally suspended */
  set_suspend_type(NOT_SUSPENDED_INTERNALLY,
                   context_ptr,
                   current_MIME_type_ptr);

  /* Notify idle, so that the app knows Qtv is no longer internally suspended */ 
  if (context_ptr) // Mpeg4Player
  {
    context_ptr->Notify(QtvPlayer::QTV_PLAYER_STATUS_IDLE);
  }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API   
  else if (IS_REAL_PLAYER) // Real player
  {
    QtvRealPlayer_Notify(QtvPlayer::QTV_PLAYER_STATUS_IDLE);
  }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::init_qtv_with_cm

DESCRIPTION:
  Initializes Qtv as a client with CM.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True is successfully initialized, false otherwise.
========================================================================== */
bool qtv_conc_mgr::init_qtv_with_cm()
{
  bool status = true;
  if(call_info_table_ptr)
  {
    /* De-allocate the call info table memory and make NULL*/
    QTV_Free(call_info_table_ptr);
    call_info_table_ptr = NULL;
  }

  /* Allocate memory to internal call table information */
  call_info_table_ptr = (call_info_type*)QTV_Malloc((sizeof(call_info_type)) * CM_CALL_ID_MAX);
  if(call_info_table_ptr == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                 "init_qtv_with_cm: call_info_table_ptr memory allocation failed");
    status = false;
    return status;
  }
  memset(call_info_table_ptr, 0, sizeof(call_info_type) * CM_CALL_ID_MAX);

  /* Initialize Qtv as a client with CM */
#ifndef PLATFORM_LTK
  if (cm_client_init(CM_CLIENT_TYPE_BREW_APPS, &qtv_cm_client) != CM_CLIENT_OK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                 "init_qtv_with_cm: CM client initialize failed");

    status = false;
  } 
#endif /* PLATFORM_LTK */
  
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::register_qtv_with_cm

DESCRIPTION:
  Registers Qtv callback with CM.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True if successfully registered, false otherwise.
========================================================================== */
bool qtv_conc_mgr::register_qtv_cb_with_cm()
{
  bool status = true;

  /* Register the callback with CM and the events to be notified for */
  if (cm_mm_client_call_reg(qtv_cm_client,
                            (cm_mm_call_event_f_type *)call_event_notify_cb,
                            CM_CLIENT_EVENT_REG,
                            CM_CALL_EVENT_ORIG,
                            (cm_call_event_e_type) (CM_CALL_EVENT_MAX-1),
                            NULL
                            ) != CM_CLIENT_OK) 
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                 "register_qtv_cb_with_cm: CM client register failed");

    status = false;
  }
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::register_qtv_with_diag

DESCRIPTION:
  Registers Qtv callback with diag. 
    This function is used only in diag context to test in-call Qtv.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True if successfully registered, false otherwise.
========================================================================== */

bool qtv_conc_mgr::register_qtv_cb_with_diag()
{
  bool status = true;

  /* Register the callback with diag.*/
  if(!cm_mm_client_diag_reg((cm_mm_call_event_f_type *)call_event_notify_cb))
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                  "register_qtv_cb_with_diag:CM diag register failed");
     status = false;
  }
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::activate_qtv_with_cm

DESCRIPTION:
  Activates Qtv client with CM and obtains a snapshot of CM's call table.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True if activation was successful, false otherwise.
========================================================================== */
bool qtv_conc_mgr::activate_qtv_with_cm()
{
  bool status = false;

  /* Activate client */
  if (cm_client_act(qtv_cm_client) == CM_CLIENT_OK)
  {
    /* Obtain a snapshot of CM's call table */
    for (cm_call_id_type call_id=0; call_id < CM_CALL_ID_MAX; call_id++)
    {
      cm_mm_call_cmd_get_call_info(NULL, NULL, qtv_cm_client, call_id);
    }
    status = true;
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                 "activate_qtv_with_cm: CM client activate failed");
  }
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::deregister_qtv_from_cm

DESCRIPTION:
  De-registers and releases the Qtv client from CM. 

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::deregister_qtv_from_cm()
{
  /* De-register only if Qtv is registered with CM */
  if (qtv_cm_client != CM_CLIENT_ID_ANONYMOUS)
  {
    /* Deactivate Qtv client */
    cm_client_deact(qtv_cm_client);

    /* Release Qtv client */
    cm_client_release(qtv_cm_client);

    /* Reset the client Id number */
    qtv_cm_client = CM_CLIENT_ID_ANONYMOUS;

    if(call_info_table_ptr)
    {
      /* De-allocate the call info table memory and make NULL*/
      QTV_Free(call_info_table_ptr);
      call_info_table_ptr = NULL;
    }
  }
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::update_state_for_suspended_ext

DESCRIPTION:
  Updates the suspend_state on receiving an external suspend.

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::update_state_for_suspended_ext()
{
  switch(suspend_state)
  {
    case STATE_NOT_SUSPENDED:
    {
      /* If not suspended, an external suspend will move  the state to 
      ** externally suspended.
      */ 
      suspend_state = STATE_EXT_SUSPENDED;
      break;
    }

    case STATE_INT_SUSPENDED:
    {
      /* If already suspended internally, an external suspend will move the 
      ** state to suspended (i.e. suspended both internally and externally).
      */
      suspend_state = STATE_SUSPENDED;
      break;
    }

    default:
    {
      /* If already in suspended (i.e. suspended both internally and 
      ** externally) or externally suspended state, an external
      ** suspend will have no effect on the state.
      */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                    "update_state_for_suspended_ext: No state change");
      break;
    }
  } /* switch(suspend_state) */

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "update_state_for_suspended_ext: suspend_state = %d",
                suspend_state);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::update_state_for_not_suspended_ext

DESCRIPTION:
  Updates the suspend_state on receiving a not suspended externally.

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::update_state_for_not_suspended_ext()
{
  switch(suspend_state)
  {
    case STATE_EXT_SUSPENDED:
    {
      /* If suspended externally, a not suspended externally will move the
      ** state to not suspended.
      */ 
      suspend_state = STATE_NOT_SUSPENDED;
      break;
    }

    case STATE_SUSPENDED:
    {
      /* If suspended (i.e. suspended both internally and externally), a not
      ** suspended externally will move the state to internally suspended.
      */
      suspend_state = STATE_INT_SUSPENDED;
      break;
    }

    default:
    {
      /* If already not suspended or internally suspended, a not suspended
      ** externally will have no effect on the state.
      */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                    "update_state_for_not_suspended_ext: No state change");
      break;
    }
  } /* switch(suspend_state) */

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "update_state_for_not_suspended_ext: suspend_state = %d",
                suspend_state);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::update_state_for_suspended_int

DESCRIPTION:
  Updates the suspend_state on receiving an internal suspend.

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::update_state_for_suspended_int()
{
  switch(suspend_state)
  {
    case STATE_NOT_SUSPENDED:
    {
      /* If not suspended, an internal suspend will move the state to 
      ** internally suspended.
      */
      suspend_state = STATE_INT_SUSPENDED;
      break;
    }

    case STATE_EXT_SUSPENDED:
    {
      /* If already externally suspended, an internal suspend will move the 
      ** state to suspended (i.e. suspended both internally and externally).
      */
      suspend_state = STATE_SUSPENDED;
      break;
    }

    default:
    {
      /* If already suspended (i.e. suspended both internally and externally)
      ** or internally suspended, an internal suspend will have no effect on
      ** the state.
      */  
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                    "update_state_for_suspended_int: No state change");
      break;
    }
  } /* switch(suspend_state) */

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "update_state_for_suspended_int: suspend_state = %d",
                suspend_state);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::update_state_for_not_suspended_int

DESCRIPTION:
  Updates the suspend_state on receiving a not suspended internally.

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::update_state_for_not_suspended_int()
{
  switch(suspend_state)
  {
    case STATE_INT_SUSPENDED:
    {
      /* If suspended internally, a not suspended internally will move the 
      ** state to not suspended.
      */ 
      suspend_state = STATE_NOT_SUSPENDED;
      break;
    }

    case STATE_SUSPENDED:
    {
      /* If suspended (i.e. suspended both internally and externally), a not
      ** suspended internally will move the state to externally suspended.
      */
      suspend_state = STATE_EXT_SUSPENDED;
      break;
    }

    default:
    {
      /* If already not suspended or externally suspended, a not suspended 
      ** internally will have no effect on the state.
      */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                    "update_state_for_not_suspended_int: No state change");
      break;
    }
  } /* switch(suspend_state) */

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "update_state_for_not_suspended_int: suspend_state = %d",
                suspend_state);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::qtv_is_suspended_internally

DESCRIPTION:
  Used to determine if Qtv is suspended internally.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True if suspended (i.e. suspended both internally and externally) or
    internally suspended, false otherwise.
========================================================================== */
bool qtv_conc_mgr::qtv_is_suspended_internally()
{
  return (suspend_state == STATE_INT_SUSPENDED ||
          suspend_state == STATE_SUSPENDED);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::process_call_end

DESCRIPTION:
  On receiving a call end event based on the suspend_state, notifies pause
  or issues a play clip to the player.

PARAMETERS:
  None.

RETURN VALUE:
  None.
========================================================================== */
void qtv_conc_mgr::process_call_end()
{
  /* Received a call end, determine what to do */
  if (suspend_state == STATE_SUSPENDED)
  {
    /* We were internally suspended and the app was also suspended.
    ** Notify pause so that the app knows that it is enabled.
    */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                 "process_call_end: Call end, notify pause");

    notify_pause(av_player_ptr);
  }
  else if (suspend_state == STATE_INT_SUSPENDED)
  {
    /* We were internally suspended, but the app was not suspended.
    ** Do an internal resume.
    */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                 "process_call_end: Call end, play");

    resume(av_player_ptr);
  }
  else if (suspend_state == STATE_EXT_SUSPENDED)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                 "process_call_end: Call end, nothing to do");
  }
  else 
  {
    /* There are 2 reasons which brings us here:
    ** (i) We are playing in-call and so we're not internally suspended.
    ** (ii) A call was initiated but the setup wasn't completed, which means 
    ** Qtv was never suspended.
    */
    if (play_mode == PLAY_MIXED_AUDIO)
    {
      /* In-call is supported, and we're using the in-call image for playback.
      ** We have to switch back to the regular Qtv image, suspend the player. 
      ** Since this suspend is not related to an incoming call, we will not set
      ** the suspend type. 
      */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                   "process_call_end: In-call supported, switching to Qtv"
                   "image");

      suspend(av_player_ptr);

      /* Resume playback */
      resume(av_player_ptr);
    }
    else
    {
      /* Call was initiated but the setup wasn't completed, which means Qtv was
      ** never suspended. Nothing needs to be done */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                 "process_call_end: Call end, nothing to do");
    }
  }
}
/* =======================================================================
FUNCTION:
  qtv_conc_mgr::all_calls_are_idle

DESCRIPTION:
  Determines from the call table if there are any active calls.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True if all calls are idle, false otherwise.
========================================================================== */
bool qtv_conc_mgr::all_calls_are_idle()
{
  bool status = true;

  /* Check for any active calls */
  for (cm_call_id_type call_id = 0; call_id < CM_CALL_ID_MAX; call_id++)
  {
    if (call_info_table_ptr != NULL && call_info_table_ptr[call_id].call_state != CM_CALL_STATE_IDLE) 
    {
      status = false;
      break;
    }
  }
  return status;
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::in_call_video_is_supported

DESCRIPTION:
  Determines if in-call video is supported.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True if in-call video is supported, false otherwise.
========================================================================== */
bool qtv_conc_mgr::in_call_video_is_supported()
{
  bool videocodec_status = false;
  bool vocoder_status = false;

#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */

  return (videocodec_status && vocoder_status);
}

/* =======================================================================
FUNCTION:
  qtv_conc_mgr::in_call_audio_is_supported

DESCRIPTION:
  Determines if in-call audio (clip audio mixed with voice call) is 
  supported.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    True if in-call audio is supported, false otherwise.
========================================================================== */
bool qtv_conc_mgr::in_call_audio_is_supported()
{
  bool status = false;

#ifdef FEATURE_QTV_IN_CALL_AUDIO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_AUDIO */

  return status;
}
