#ifndef _CONCURRENCYMGR_H_
#define _CONCURRENCYMGR_H_
/* =========================================================================                    
                          qtv_conc_mgr.h

DESCRIPTION
  Definition of types and classes to support the concurrency manager
  framework.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  To use the services provided, players must register with the concurrency
  manager through init. 

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
============================================================================ */
/* =========================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia/qtv/concurrencymgr/main/latest/inc/qtv_conc_mgr.h#14 $
$DateTime: 2010/07/11 23:39:43 $
$Change: 1363025 $

============================================================================ */
/* =========================================================================
**               Includes and Public Data Declarations
** ========================================================================= */
/* =========================================================================
                     INCLUDE FILES FOR MODULE
============================================================================ */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "media.h"
#include "QtvPlayer.h"
extern "C"
{
  #include "cm.h"
#ifdef FEATURE_QTV_IN_CALL_1X  
#error code not present
#endif  
}

/* =========================================================================
                        DATA DECLARATIONS
============================================================================ */
/* -------------------------------------------------------------------------
** Constant / Define Declarations
** ------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------
** Type Declarations
** ------------------------------------------------------------------------- */
class Mpeg4Player;

/* -------------------------------------------------------------------------
** Global Constant Data Declarations
** ------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------
** Global Data Declarations
** ------------------------------------------------------------------------- */
/* =========================================================================
**                        Macro Definitions
** ========================================================================= */

/* =========================================================================
**                        Class Declarations
** ========================================================================= */
class qtv_conc_mgr
{
public:

  typedef enum 
  {
    PLAY_NORMAL,       /* No voice call in progress, play using QTV's image */
    PLAY_CONCURRENT,   /* Voice call in progress, concurrent video supported */
    PLAY_MIXED_AUDIO,  /* Voice call in progress, mix clip audio with voice call */
    PLAY_NOT_SUPPORTED /* Voice call in progress, concurrent video not supported */
  }conc_play_mode_type;
 
  /* Enum of what caused the player to suspend */
  typedef enum 
  {
    SUSPENDED_EXTERNALLY,     /* Suspend was done externally */
    NOT_SUSPENDED_EXTERNALLY, /* Resume was done externally/not suspended */
    SUSPENDED_INTERNALLY,     /* Suspend was done internally */
    NOT_SUSPENDED_INTERNALLY  /* Resume was done internally/not suspended */
  }suspend_type;

  /* Enum of the suspended state. This is dependent on what caused the player
  ** to suspend
  */
  typedef enum
  {
    STATE_NOT_SUSPENDED, /* Not suspended externally or internally */  
    STATE_EXT_SUSPENDED, /* Suspended externally, not suspended internally */
    STATE_INT_SUSPENDED, /* Suspended internally, not suspended externally */
    STATE_SUSPENDED      /* Suspended internally and externally */
  }suspend_state_type;

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
  static void init(
    Mpeg4Player *context_ptr, 
    const char *current_MIME_type,
    uint32 num_qtv_instance);

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
  static void terminate(uint32 num_qtv_instance);

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
  static bool register_with_call_mgr();

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
  static bool can_process_command(
    Mpeg4Player *context_ptr,
    const char *current_MIME_type);

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
  static qtv_conc_mgr::conc_play_mode_type get_play_mode(
    Mpeg4Player *context_ptr,
    const char *current_MIME_type);

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
  static void set_suspend_type(
    qtv_conc_mgr::suspend_type set_suspend_due_to,
    Mpeg4Player *context_ptr,
    const char *current_MIME_type);

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
      Enum of suspend types.
  ========================================================================== */
  static qtv_conc_mgr::suspend_state_type get_suspend_state(
    Mpeg4Player *context_ptr,
    const char *current_MIME_type);

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
  static void suspend(Mpeg4Player *context_ptr);

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
  static void resume(Mpeg4Player *context_ptr);

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
  static void notify_pause(Mpeg4Player *context_ptr);

  /* =======================================================================
  FUNCTION:
    qtv_conc_mgr::register_qtv_cb_with_diag

  DESCRIPTION:
    Registers callback with diag to test in-call Qtv. 
      This function not to be used except for diag testing!!!

  PARAMETERS:
    None

  RETURN VALUE:
    None.
  ========================================================================== */

  static bool register_qtv_cb_with_diag();

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
  static bool qtv_is_suspended_internally();

private:

  typedef enum 
  {
    NOT_IN_CALL,             /* No active voice call */
    IN_CALL_CONCURRENT_EVRC, /* Active voice call with EVRC vocoder */
    IN_CALL_CONCURRENT_AMR,  /* Active voice call with AMR vocoder */
    IN_CALL_CONCURRENT_13K,  /* Active voice call with 13K vocoder */
    IN_CALL_CONCURRENT_4GV_NB, /* Active voice call with 4GV NB vocoder */
    IN_CALL_CONCURRENT_4GV_WB, /* Active voice call with 4GV WB vocoder */
    IN_CALL_NOT_CONCURRENT   /* Active voice call with unsupported vocoder type or
                                Active emergency call */
  }in_call_state_type;
  static in_call_state_type in_call_state;

  /* CM's call information */
  struct call_info_type 
  {
    cm_call_state_e_type call_state;
    cm_call_type_e_type call_type;
    cm_call_mode_info_s_type mode_info;
  };

  static call_info_type *call_info_table_ptr;

  /* suspend_state is dependent on what caused the player to suspend */
  static suspend_state_type suspend_state;

  /* Keeps track of who made the most recent suspend call */
  static suspend_type suspend_due_to;

  /* play_mode is determined using the in_call_state and videoCodecType */
  static conc_play_mode_type play_mode;

  /* Mpeg4Player handle */
  static Mpeg4Player *av_player_ptr;

  /* MIME type used to determine player */
  static const char *current_MIME_type_ptr;

  /* Determine whether player suspended internally at call set up response event */
  static bool is_qtv_suspended_internally;

  /* =======================================================================
  FUNCTION:
    qtv_conc_mgr::update_incall_state

  DESCRIPTION:
    Updates the in-call state based on a snaphot of the call manager's
    call table.

  PARAMETERS:
    None.

  RETURN VALUE:
    None.
  ========================================================================== */
  static void update_incall_state();

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
  static qtv_conc_mgr::in_call_state_type get_voice_call_state(
  cm_call_id_type call_id);

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
  static void call_event_notify_cb(
    cm_call_event_e_type call_event,
    cm_mm_call_info_s_type *call_info_ptr);

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
  static void process_call_event(
  cm_call_event_e_type call_event,
  cm_call_id_type call_id);

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
  static void update_play_mode();

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
  static bool init_qtv_with_cm();

  /* =======================================================================
  FUNCTION:
    qtv_conc_mgr::register_qtv_with_cm

  DESCRIPTION:
    Registers Qtv callback with CM.

  PARAMETERS:
    None.

  RETURN VALUE:
    bool
      True if successfully reistered, false otherwise.
  ========================================================================== */
  static bool register_qtv_cb_with_cm();

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
  static bool activate_qtv_with_cm();

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
  static void deregister_qtv_from_cm();

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
  static void update_state_for_suspended_ext();

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
  static void update_state_for_not_suspended_ext();

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
  static void update_state_for_suspended_int();

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
  static void update_state_for_not_suspended_int();

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
  static void process_call_end();

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
  static bool all_calls_are_idle();

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
  static void notify_idle(Mpeg4Player *context_ptr);

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
  static bool in_call_video_is_supported();

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
  static bool in_call_audio_is_supported();

};
#endif /* _CONCURRENCYMGR_H_ */

