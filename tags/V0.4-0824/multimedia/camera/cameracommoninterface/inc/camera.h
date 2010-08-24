#ifndef CAMERA_H
#define CAMERA_H

/*============================================================================

                 Camera Service Interface Header File

   DESCRIPTION
     This file contains the definitions needed for the camera service
     interface.

============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameracommoninterface/main/latest/inc/camera.h#9 $ $DateTime: 2009/04/19 21:14:25 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/20/09  chai   Added CAMERA_BESTSHOT_PARTY 
 04/07/09  cg     Re-Enabling Feature FEATURE_CAMERA_BURST_MODE  
 02/12/09  cgera  Added support for MULTISHOT AEC 1.1
 12/18/08  sm     Added support for multi raw preview capture
 11/30/08  sm     Added support for preview rotation
 11/25/08  hap    Adding support for shutter sound play and VFE capture 
                  concurrency
 02/11/08  sanal  Adding Idiag support for frames dropped in a recorded clip
 12/07/07  hap    Modified to signal UI task after camera complete clean up 
                  is done   
 10/1/07   hap    Added changes needed to support YCbCr as a type of encoding.
 07/02/07  pc     Added HTORPC meta comments for ONCRPC remoting.
 07/02/07  arv    Fixed the issue related to compiler warning when LCD_DIRECT_MODE 
                  and MDP is enabled
 03/07/07  zl     Updated FEATURE_CAMERA_MULTI_SENSOR support.
 11/08/06  dhaval Removed the camera_start_record prototype change.
 10/30/06  dhaval Changed prototype of camera_start_record routine to pass
                  space limit value
 10/23/06  dle    Implemented FEATURE_CAMERA_LCD_DIRECT_MODE.
 09/14/06  jv     Redesign camera termination. Added CAMERA_FUNC_TERMINATE to
                  camera_func_type.
 09/01/06 kadhir Removed unwanted Cross Compilation Dependency
                  When qcamcorder Feature Disabled
 08/15/06  jv     Removed Text bestshot mode. Added comments for bestshot.
 08/09/06  dle    Changed use and description for CAMERA_PARM_SENSOR_POSITION,
                  and related CAMERA_PARM_ENCODE_ROTATION and
                  CAMERA_PARM_REFLECT.
 07/27/06  ven    Since HJR and ISO are mutually exclusive, they are merged
                  under a single set of selectable parameters from UI.
                  CAMERA_PARM_HAND_JITTER_REDUCTION will be removed once UI
                  stops referring it.
 07/21/06  wyh    Add CARM_PARM_TRANSITION and associated parameters
 07/18/06  jv     Added CAMERA_BESTSHOT_NIGHT, and Bestshot documentation.
 07/11/06  jch    Put in comment that RAW multishot is not supported.
 07/07/06  ven    Revamped the rotation/reflection functionality.
                  Sensor position is no longer supported
      UI to use encode_rotation and relection to achieve rotation
      for snapshot and preview respectively
 06/22/06  jv     Added CAMERA_BRACKETING_MAX to camera_bracketing_mode_type.
 06/20/06  ven    Cleaned up the unused offline snapshot routines meant to do
                  offline snapshot from raw image in EFS.
 06/20/06  jv     Added maximum element to camera_bestshot_mode_type.
 06/19/06  jv     Added bestshot mode enum. Moved bestshot types to camerai.h.
                  Removed camera_enable_bestshot_mode and
                  camera_disable_bestshot_mode. Now use camera_set_parm to
                  enable or disable bestshot modes.
 06/12/06  jch    Added camera_take_multiple_pictures and related functions.
 06/02/06  dle    Added support for _PARM_FPS_LIST.
 05/26/06  dle    Modified _take_multi_pictures interface description.
 05/09/06  jch    Moved Burstmode Feature from camera.h to camerai.h
 05/04/06  Shahed Change for new A/V sync implementation for camcorder for
                  AAC with CMX MM API sync signal.
 05/02/06  dle    Added FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE and
                  FEATURE_CAMERA_BESTSHOT_MODE.
 04/26/06  nrw    Replaced SIRIUS_BUILD with SIRIUS_CAMERA.
 04/24/06  jch    Implementation of BurstMode feature.
 04/09/06  jn     Removed AF auto mode.
 03/13/06  dle    bAudioPostCard in _start_record() is _V770, not just _V7.
 03/07/06  dl     Added CAMERA_PARM_CHROMA_SUPPRESSION and
                  CAMERA_PARM_HAND_JITTER_REDUCTION.
 03/03/06  vma    Added CAMERA_ERROR_PNG_ABORT to camera_error_type
 02/21/06  dle    Added CAMERA_AF_MODE_MAX as indication of Do Not Change.
 02/17/06  jn     Added CAMERA_FUNC_MOVE_LENS_DONE.
 02/13/06  dle    Added CAMERA_PARM_FLASH_STATE.
 01/29/06  ali    Fixed featurization of new structures and enums
 01/20/06  ali    Added a few paramaters for camera_set_param
                  Added a new parameter in camera_start_record
 1/17/06   babak  Added CAMERA_FUNC_MOVE_LENS as an option for process_func
 11/03/05  anb    Added CAMERA_FUNC_HANDLE_ERROR and CAMERA_FUNC_STOP_STANDALONE
 09/29/05  nrw    Fixed Lint errors.
 09/07/05  nrw    Added Biomorphic JPEG type to camera_format_type.
 08/17/05  nrw    Added enum to uniquely identify attached sensors.
 08/15/05  cl     Added support for setting multiple sets of uuid.
 08/15/05  nrw    Multi-sensor support changes: added three fields to
                  camera_info_type: sensor_available, sensor_selected, and
                  sensor_enbabled.
 08/02/05  nrw    Updated comments.
 06/28/05  nrw    Removed SIRIUS_CAMERA define
 06/27/05  nrw    For SIRIUS_CAMERA only: Moved flash white balance support to
                  camerai.h.
 06/23/05  nrw    For SIRIUS_CAMERA only: Added flash white balance support.
 06/17/05  alin   Set curtain effect transition delay to 2 sec
 06/07/05  dle    Removed _PARM_THUMBNAIL_*. Please use:
                    camera_set_thumbnail_properties() and
                    camera_get_thumbnail_properties().
 05/17/05  mvd    Changes for 6550 BREW 3.1.4.
 05/13/05  wyh    Video encoder transition effects.
 05/05/05  dl     Added BVCM support.
 04/05/05  drl    Revised camera_set_dimensions for consistant use between
                  camera and camcorder apps.
 03/23/05  mvd    Added support for new CAMERA_PARM_PREVIEW_FPS parm
                  and the new camera set_parm function with both P1
                  and P2 arguements.  These are both V7 changes.
 03/22/05  sai    Added support for offline snapshot.
 03/14/05  drl    Added encode_width and encode_height args to
                  camera_set_dimensions for V7 builds.
 03/10/05  anb    Added CAMERA_PARM_EXPOSURE_COMPENSATION to control the
                  exposure compensation setting.
 02/25/05  sch    *_focus_window () returns camera_ret_code_type.
 02/24/05  drl    Changed interface to camera_start_record to add user specified
                  bitrate.  Also added CAMERA_FUNC_INIT_RECORD.
 02/22/05  wh     Go back to versoin 118
 02/22/05  wh/dw  Fading/bitrate control.
 02/15/05  ali    Changes for Fade In/Fade Out
 01/20/05  sch    -Single window focus API uder FEATURE_CAMERA_V7
 01/11/05  kln    Moved prototypes for camera_init(), camera_process_pnge_msg(),
                  and camera_process_qdsp_msg() to camerai.h
 01/09/05  sch    Removed CAMERA_PARM_SNAPSHOT_ENCODING_FORMAT, using
                  FEATURE_CAMERA_ENCODE_PROPERTIES instead.
 01/07/05  kln    Added prototype for function to process messages from png encoder
 01/05/05  sch    Added CAMERA_PARM_SNAPSHOT_ENCODING_FORMAT to let the
                  upperlayer convey the encoding format information to
                  service layer using camera_set_parm().
 12/14/04  dle    Added support for FEATURE_CAMERA_MULTI_SENSOR and
                  FEATURE_CAMERA_ENCODE_PROPERTIES.
 12/07/04  kln    Added enum for camera rotation, removed H1V2 from
                  camera_format_type
 11/16/04  dle    Modified QVPHONE to use arm_kick_backend.
 10/20/04  mvd    Camera V7 changes only!  Added
                  CAMERA_PARM_AUTO_EXPOSURE_MODE.  Backed out changes to
                  camera_exposure_type (this was a mistake from the
                  previous version).  Added auto exposure mode enum type.
 10/14/04  mvd    Removed CAMERA_PARM_AUTO_EXPOSURE_ALGORITHM and associated
                  type (BREW will not support).  Took over
                  CAMERA_PARM_EXPOSURE instead.  All modifications under
                  FEATURE CAMERA V7
 10/11/04  dle    Moved non-essentials to camerai.h.
 09/30/04  ali    Changes for video encode overlay
 09/21/04  dle    Added more LOGGING defines.
 09/15/04  dle    Temporarily back out picture_size in _thumbnail_properties.
                  Changed _NOT_INCALL to _INCALL_NOT_VOICE. No need to treat
                  data call differently.
 09/07/04  dle    Added picture_size to camera_get_thumbnail_properties
                  and camera_get_thumbnail_properties.
 08/18/04  dle    Added camera_get_thumbnail_property() and
                  camera_set_thumbnail_properties().
 07/16/04  dle    Restored camera_enable_qvp().
 07/15/04  dle    Replaced CAMERA_ERROR_FILE_* with CAMERA_ERROR_FS_ and
                  expanded error types.
                  Replaced CAMERA_DEBUG with CAMERA_LOGGING. Expanded and
                  enabled CAMERA_LOGGING as default.
                  Added CAMERA_PARM_INCALL.
 07/14/04  js     Modified camera_enable_qvp() to include VFE category
                  preference.
 06/17/04  dle    Moved CAMERA_DEBUG declarations here.
 06/02/04  dle    Added _PARM_THUMBNAIL_WIDTH, _PARM_THUMBNAIL_HEIGHT,
                  and _PARM_THUMBNAIL_QUALITY.
 04/28/04  mvd    Added exit CB status flag called
                  CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED for condition where
                  File Size Control algo was unable to bring output encoding
                  size under requested file size after 1 or 2 passes.
 04/23/04  dle    Restore support for YCBCR1 via camera_use_ycbcr1. It is
                  set to false by default.
 04/15/04  mvd    Added support for stand_alone image encoder that can be
                  called without requiring sensor activation.  Currently
                  supports JPEG encode in YCbCr, RGB888 & 444 input
                  formats.  camera_encode_picture_stand_alone() has
                  been added to the external interface
 03/22/04  dle    Renamed camera_*_average and camera_*_gain to preview_.
                  Renamed camera_center_weighted_luma to vfe_.
 03/03/04  dle    Added support for antibanding.
 02/04/04  mvd    Removed camera_quality_type - now irrelevant since
                  quality is a percent from 1 to 100.
 01/29/04  mvd    Added API for JPEG File Size Control.  New fn called
                  camera_encode_picture_size_limit() and max file size
                  encoding result specified in arguments of function
                  Units for this new arguement are in Killobytes.
 01/23/04  mvd    Changed declarations of camera_handle mem and efs types
                  for variable JPEG quality (changed to int32 value)
 12/04/03  pdo    Featurized the PNG_ENCODER
 11/07/03  dle    Added the other 3 bayer format.
 10/09/03  dle    Added camera_update_vfe.
 09/24/03  dle    Support of FEATURE_BRIDLE_CAMERA in video modes.
 09/24/03  dle    Added CAMERA_PARM_PREVIEW_MODE, camera_preview_mode_type.
                  Changed camera_mode to preview_mode.
 09/23/03  dle    Added back camera_ok_to_sleep.
 09/16/03  dle    Added _reflect_type and _PARM_REFLECT.
                  Added camera_color_convert().
 09/12/03  dle    Initial support for MIRROR and ICONs.
 08/27/03  dle    Added _STATE_QVP_ENCODING.
 08/20/03  dle    Added items in camera_func_type.
 ===== Below merged from 05.00.15.01
 08/11/03  aaa    Changes to support UUID
 08/01/03  aaa    Added video brand and short header parameters to
                  camera_start_record
 07/30/03  sa     added CAMERA_STATUS_CB to camera_cb_type.
 07/23/03  dle    Added support for FEATURE_QVP.
 ===== Above merged from 05.00.15.01
 08/11/03  dle    Added camera_qvp_reset().
 08/06/03  dle    Added _NIGHTSHOT_MODE. Renamed camera_center_luma to
                  camera_center_weighted_luma.
 07/31/03  dle    Some name changes.
 07/23/03  dle    Added support for FEATURE_QVPHONE.
 07/11/03  dle    _flash_type base is 1 to match aeecamera.h.
 07/10/03  dle    Added _center_luma.
 07/01/03  dle    Added CAMERA_RGB666.
 06/16/03  dle    Removed CAMERA_STATS_COUNT. It is now always 1.
 06/05/03  dle    Changed CAMERA_STATS_COUNT to 1
 05/21/03  dle    Modified camera_flash_type.
 05/16/03  dle    Added camera_blt_ext().
 05/15/03  dle    Modified camera_format_type.
 05/13/03  dle    Added camera_timed_wait().
 05/08/03  dle    Added captured_dx, _dy to _frame_type.
 05/06/03  dle    Added CAMERA_FAILED return code.
 05/02/03  dle    Added running average of stats. Modified _set_fps().
 04/17/03  dle    Added _FUNC_CORRUPT for image corrupt processing.
 04/15/03  dle    Support of RAW encode type.
 04/11/03  dle    Added _set_dimensions(). Removed dimensions in
                  _start_preview(). Encode supports only JPEG. Added support
                  for _PARM_FPS.
 04/01/03  dle    Added items in _parm_type and _format_type.
 03/20/03  dle    Exporting camera_current_frame.
 03/20/03  dle    Enabled sepia support.
 03/19/03  dle    Replaced resize_factor with display_width in
                  camera_start_preview. Added CAMERA_PARM_AUDIO_FMT.
 03/11/03  dle    Added items to camera_error_type. Removed CAMERA_NO_PICTURE
                  return code.
 02/14/03  dle    Added camera_mode_type.
                  Featurized video engine and encode code.
                  take_picture return only raw data in frame_type.
                  Modified camera_frame_type. Captured dimension is now the
                  same as requested picture dimension, no more normalized to
                  multiple of 16.
                  encode is now encode_picture(). It takes frame_type input
                  and output in handle_type.
                  CB_ABORT is for general abort. CB_DSP_ABORT is for DSP abort.
                  Added release_picture().
                  Added set_exif_tag() to replace set_string().
 02/03/03  dle    Replaced camera_set_description() with camera_set_string() for
                  NEWIF.
 01/28/03  dle    Added PARM_EXPOSURE, PARM_WB, PARM_EFFECT.
                  Renamed PARM_REVERSAL to PARM_SENSOR_POSITION,
                  PARM_ORIENTATION to PARM_ENCODE_ROTATION.
 01/21/03  dle    Added timestamp into camera_position_type.
 01/21/03  dle    Removed CAMERA_PARM_ROTATION, added CAMERA_PARM_REVERSAL and
                  CAMERA_PARM_ORIENTATION.
 01/15/03  dle    Added CAMERA_STATE_ENCODE.
                  Added camera_video_handshake().
 01/10/03  dle    Modified camera_orientation_type. Added default_orientation
                  in camera_info_type.
 12/20/02  dle    Removed callback from camera_blt().
 12/18/02  dle    Added camera_encode().
 12/17/02  dle    First modification after 2.0.
 12/03/02  dle    Make NEWIF official by removing it.
 11/12/02  dle    Second writing for NEWIF.
 11/07/02  dle    Added camera_set_overlay and camera_clr_overlay. Removed
                  camera_show_picture().
 10/31/02  dle    Added support for NEWIF. This define will be removed once
                  we have started using new interface.
 10/29/02  dle    Added YCBCR and RBG565 to save picture quality
 10/24/02  dle    Added camera_save_app_picture().
 10/04/02  dle    Moved _FUNC_ extension back to source.
 09/20/02  dle    Moved some _FUNC_ extension from source to here. Added
                  MAX for most enums.
 08/29/02  dle    Added CAMERA_MAX_RET_CODE. Shuffled parm_info_type.
 08/14/02  dle    Initial release.

============================================================================*/
/*============================================================================

   STATES:

     INIT state: after a phone is powered up or after camera_stop is called.
     READY state, after camera_start is called. Also after exiting from
       operational states.
     PREVIEW state, after camera_start_preview is called.
     TAKE_PICTURE state, after camera_take_picture is called.
     RECORDING state, after camera_start_record is called.

   FUNCTION TYPES:

     Synchronous functions: a function that is executed completely in the
     applications context. It does not have callbacks and does not use callback.

     Asynchronous functions: a function that cause state transition. Response
     callback is used to indicate when the function execution is started,
     either successfully or failed. Most asynchronous functions has lasting
     effect. Exit callback is then used to indicate when the function execution
     is completed. The state after transition stays until failure or changed by
     another function call. They include camera_start_preview,
     camera_start_record, camera_pause_record, and camera_resume_record.
     callback is sent.

     Blocked Asynchronous functions: a function that causes parameter change.
     It is executed in the service context but the application is blocked.
     Callback is present but have no real effect.

   FUNCTION CATAGORIES:

     Start stop functions: camera_start and camera_stop. camera_start is the
       only function that is accepted in INIT state. camera_start cannot be
       called in any other states. camera_stop can be called in any except
       INIT state.

ifdef FEATURE_CAMERA_MULTI_SENSOR
     Device select function: This functions allows the application
       to select a sensor.
         camera_select_sensor() can only be executed in INIT state.
endif

     Management functions: camera_get_parm and camera_set_parm. These
       functions can be called at any time except INIT in state.
       camera_set_parm is asynchronous. After the command is executed,
       CAMERA_EXIT_CB_DONE or CAMERA_EXIT_CB_FAILED is sent.

     Operational functions: The rest are opeational functions.

       Primary functions: The functions that change the operational states,
         such as camera_start_preview, camera_start_record.

       Secondary functions: The functions that modify or terminate the
         operational states, such as camera_stop_preview,
         camera_suspend_recrod.

       Utility functions: The functions that deals with the use of
         frame buffers, such as camera_blt, camera_release_frame.

   CALLBACKS:

     Callbacks are used only if the function execution is successful.

     All callbacks have 4 parameters:
       1. Callback code.
       2. client data
       3. Active function (camera_func_type)
       4. Varies based on callback.

     Response callback:
       CAMERA_RSP_CB_SUCCESS
         This is the callback to indicate when function is executed
         successfully and additiona callbacks should be expected.

         The forth parameter is un-used.

     Exit callback:
       The following callbacks indicate exiting the operational state
       to READY state:

       CAMERA_EXIT_CB_DONE, current active function is terminated
       successfully.
         For set_parm, the forth parameter is camera_parm_type.
         For take_picture, the forth parameter is camera_frame_type.
         Otherwise the forth parameter is un-used.

       CAMERA_EXIT_CB_DSP_IDLE, DSP is idle.
         The forth parameter is un-used.

       CAMERA_EXIT_CB_ABORT, fatal DSP error.
         The forth parameter is un-used.

       CAMERA_EXIT_CB_ERROR, other resource error.
         The forth parameter is camera_error_type.

       CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED, The requested file size
         was not met in this encoding, it was exceeded.  The image
         is returned anyway as if encoding was successful.
         The forth parameter is un-used.

     Event callback:
       CAMERA_EVT_CB_FRAME.
       Used for preview or video.
       The forth parameter is camera_frame_type and the returned data
       is raw (YCbCr).

     Event callback:
       CAMERA_EVT_CB_PICTURE.
       Used for returning pictures in camera_take_multiple_pictures().
       The fourth parameter is camera_frame_type and the returned data is
       raw (YCbCr).

   CONTROL FLOW:

     Condition                                                    state

     After power up --------------------------------------------- INIT

     camera_start()
         Get the camera service started. Can only be called in
         INIT state.

         CAMERA_EXIT_CB_DONE callback right away ---------------- READY

     camera_get_parm -------------------------------------------- Any but INIT
         Synchronous function call.
     camera_set_parm -------------------------------------------- Any but INIT
         CAMERA_EXIT_CB_DONE when set is executed --------------- Any but INIT
         CAMERA_EXIT_CB_FAILED if set execution failed ---------- Any but INIT
         (use to change brightness, contrast, etc. before
          entering operational states.)

     camera_start_preview
         Start preview. If DSP code is not loaded, request to
         load.  Once DSP code is loaded, start capture of
         preview frame.

         CAMERA_RSP_CB_SUCCESS callback when function is executed
           successfully ----------------------------------------- PREVIEW

         CAMERA_EVT_FRAME callback. ----------------------------- PREVIEW
           The camera service allocated the preview frame buffer.
           After the application done using the buffer, it must
           call camera_release_frame so the frame buffer can be
           reused.
         CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle ---------- READY
         CAMERA_EXIT_CB_ABORT callback if DSP fatal error ------- READY
         CAMERA_EXIT_CB_ERROR callback if failed ---------------- READY

     camera_blt
         This function is executed completely in the application
         context. It is an utlity function to BIT BLT the preview
         frame into the display buffer.

     camera_release_frame
         This function is executed completely in the application
         context. It is an utility function to release the preview
         frame buffer.

     camera_get_parm -------------------------------------------- PREVIEW
         Synchronous function call.
     camera_set_parm -------------------------------------------- PREVIEW
         CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
         CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW
         (use to change brightness, contrast, etc. while in
          operational states.)

     camera_stop_preview
         Preview is stopped.
         CAMERA_EXIT_CB_DONE callback when preview is stopped --- READY

   In snapshot mode:

     camera_get_parm -------------------------------------------- Any but INIT
         Synchronous function call.
     camera_set_parm -------------------------------------------- Any but INIT
         CAMERA_EXIT_CB_DONE when set is executed --------------- Any but INIT
         CAMERA_EXIT_CB_FAILED if set execution failed ---------- Any but INIT
         (use to change brightness, contrast, etc. while in
          preview state.)

     camera_take_picture ---------------------------------------- TAKE_PICTURE
         Get ready to take a picture. If DSP code is not loaded,
         request to load. Once DSP code is loaded, start capture
         of picture frame. The camera service allocates the needed
         buffer. The captured picture is returned in the forth
         parameter of CAMERA_EXIT_CB_DONE in the form of
         camera_frame_type. The buffer must be released by the
         application via either camera_encode_picture or
         camera_release_picture.

         CAMERA_RSP_CB_SUCCESS callback when function is executed
           successfully ----------------------------------------- TAKE_PICTURE

         CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle ---------- READY
         CAMERA_EXIT_CB_ABORT callback if DSP fatal error ------- READY
         CAMERA_EXIT_CB_ERROR callback if failed ---------------- READY
         CAMERA_EXIT_CB_DONE callback when the picture is taken - READY

     camera_encode_picture -------------------------------------- ENCODE
         The picture provided by the application in frame
         parameter is encoded, and the result is stored as
         specified by the handle. The picture can be either the
         one taken by take_picture, or a picture read from file,
         but has to be in raw (YCbCr) format.
         After this function is executed, the picture buffer
         allocated in take_picture is released.

         CAMERA_RSP_CB_SUCCESS callback when function is executed
           successfully ----------------------------------------- ENCODE

         CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle ---------- READY
         CAMERA_EXIT_CB_ABORT callback if DSP fatal error ------- READY
         CAMERA_EXIT_CB_ERROR callback if failed ---------------- READY
         CAMERA_EXIT_CB_DONE callback when the picture is taken - READY

     camera_encode_picture_size_limit --------------------------- ENCODE
         The picture provided by the application in frame
         parameter is encoded, and the result is stored as
         specified by the handle. The picture can be either the
         one taken by take_picture, or a picture read from file,
         but has to be in raw (YCbCr) format.
         After this function is executed, the picture buffer
         allocated in take_picture is released.
         Also, for JPEG only encoding, a target file size is
         specified in K Bytes that must not be exceeded.

         CAMERA_RSP_CB_SUCCESS callback when function is executed
           successfully ----------------------------------------- ENCODE

         CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle ---------- READY
         CAMERA_EXIT_CB_ABORT callback if DSP fatal error ------- READY
         CAMERA_EXIT_CB_ERROR callback if failed ---------------- READY
         CAMERA_EXIT_CB_DONE callback when the picture is taken - READY
         CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED callback when encoding-READY
            is done, but requested size is not met.

     camera_release_picture
         A utility function to free the picture buffer.

     camera_stop
         Camera function is terminated.
         Called only when camera function is no longer needed.

         CAMERA_EXIT_CB_DONE callback right away ---------------- INIT

   In video mode:

     camera_get_parm -------------------------------------------- Any but INIT
         Synchronous function call.
     camera_set_parm -------------------------------------------- Any but INIT
         CAMERA_EXIT_CB_DONE when set is executed --------------- Any but INIT
         CAMERA_EXIT_CB_FAILED if set execution failed ---------- Any but INIT
         (use to change brightness, contrast, etc. while in
          preview state.)

     camera_start_record ---------------------------------------- RECORDING
         Call video_eng_init().
         When called back by video engine, if DSP code is not
         loaded, request to load, else request DSP to go idle.
         When DSP is loaded or in idle, configure VFE.
         When VFE is configured, enable capture.
         When first image is received, call video_eng_start().
         If frame callback is enabled, video frame is delivered.

         CAMERA_RSP_CB_SUCCESS callback when function is executed
           successfully ----------------------------------------- RECORDING

         CAMERA_EVT_FRAME callback when frame available --------- RECORDING
         CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle ---------- READY
         CAMERA_EXIT_CB_ABORT callback if DSP fatal error ------- READY
         CAMERA_EXIT_CB_ERROR callback if failed ---------------- READY

     camera_get_parm  ------------------------------------------- RECORDING
         Synchronous function call.
     camera_set_parm  ------------------------------------------- RECORDING
         CAMERA_EXIT_CB_DONE when set is executed  -------------- RECORDING
         CAMERA_EXIT_CB_FAILED if set execution failed  --------- RECORDING
         (use to change brightness, contrast, etc. while in
          operational states.)

     camera_pause_record
         Pause recording.

         CAMERA_RSP_CB_SUCCESS callback when function is
           executed --------------------------------------------- RECORDING

     camera_resume_record
         Resume recording.

         CAMERA_RSP_CB_SUCCESS callback when function is executed
           successfully ----------------------------------------- RECORDING

     camera_blt
         This function is executed completely in the application
         context. It is an utlity function to BIT BLT the preview
         frame into the display buffer.

     camera_release_frame
         This function is executed completely in the application
         context. It is an utility function to release the preview
         frame buffer.

     camera_stop_record
         Recording is stopped.

         CAMERA_RSP_CB_SUCCESS callback when function is
           executed successfully -------------------------------- RECORDING

         CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle ---------- READY
         CAMERA_EXIT_CB_ABORT callback if DSP fatal error ------- READY
         CAMERA_EXIT_CB_ERROR callback if failed ---------------- READY
         CAMERA_EXIT_CB_DONE callback when record is stopped ---- READY

     camera_stop
         Camera function is terminated.

         CAMERA_EXIT_CB_DONE callback right away ----------------- INIT

   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/


#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "ipl.h"
#ifdef FEATURE_CAMERA
#include "fs.h"
#ifdef FEATURE_VIDEO_ENCODE
#include "videoeng.h"
#endif /* FEATURE_VIDEO_ENCODE */

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

#ifdef FEATURE_CAMERA_V7
#define CAMERA_FPS_DENOMINATOR 1000
#endif /* FEATURE_CAMERA_V7 */

/*============================================================================
*                         DATA DECLARATIONS
============================================================================*/
typedef enum
{
  CAMERA_STATE_INIT,
  CAMERA_STATE_READY,
  CAMERA_STATE_PREVIEW,
  CAMERA_STATE_TAKE_PICTURE,
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#ifdef FEATURE_CAMERA_V7
  CAMERA_STATE_OFFLINE_SNAPSHOT,
#endif /* FEATURE_CAMERA_V7 */
  CAMERA_STATE_ENCODE_PICTURE,
  CAMERA_STATE_RECORDING,
  CAMERA_STATE_QVP_ENCODING
} camera_state_type;


/* to select no vignette correction, luma vignette correction */
/* or bayer vignette correction */
#ifdef FEATURE_CAMERA_V7
typedef enum
{
  CAMERA_NO_VIGNETTE_CORRECTION,
  CAMERA_LUMA_VIGNETTE_CORRECTION,
  CAMERA_BAYER_VIGNETTE_CORRECTION
} camera_vc_mode_type;

typedef enum
{
  CAMERA_BVCM_DISABLE,
  CAMERA_BVCM_RAW_CAPTURE,
  CAMERA_BVCM_OFFLINE_CAPTURE
} camera_bvcm_capture_type;
#endif /* FEATURE_CAMERA_V7 */


typedef enum
{
  CAMERA_FUNC_START,
  CAMERA_FUNC_STOP,
  CAMERA_FUNC_SET_DIMENSIONS,
  CAMERA_FUNC_START_PREVIEW,
  CAMERA_FUNC_TAKE_PICTURE,
  CAMERA_FUNC_ENCODE_PICTURE,
  CAMERA_FUNC_COLOR_CONVERT,
  CAMERA_FUNC_START_RECORD,
  CAMERA_FUNC_START_FOCUS,
  CAMERA_FUNC_SET_OVERLAY,
  CAMERA_FUNC_CLR_OVERLAY,
  CAMERA_FUNC_SET_ICON_ARRAY,
  CAMERA_FUNC_CLR_ICON_ARRAY,
  CAMERA_FUNC_SET_POSITION,
  CAMERA_FUNC_SET_EXIF_TAG,
  CAMERA_FUNC_SET_PARM,
#ifdef FEATURE_QVPHONE
  CAMERA_FUNC_ENABLE_QVP,
  CAMERA_FUNC_DISABLE_QVP,
  CAMERA_FUNC_START_QVP_ENCODE,
  CAMERA_FUNC_STOP_QVP_ENCODE,
  CAMERA_FUNC_QVP_RESET,
#endif /* FEATURE_QVPHONE */
  CAMERA_FUNC_CLEANUP,
  CAMERA_FUNC_MAX,

  /*==========================================================================
   * The followings are for internal use only
  ==========================================================================*/
#ifdef FEATURE_CAMERA_MULTI_SENSOR
  CAMERA_FUNC_SELECT_SENSOR,
#endif /* FEATURE_CAMERA_MULTI_SENSOR */
  CAMERA_FUNC_STOP_PREVIEW,
  CAMERA_FUNC_RELEASE_PICTURE,
  CAMERA_FUNC_PAUSE_RECORD,
  CAMERA_FUNC_RESUME_RECORD,
  CAMERA_FUNC_STOP_RECORD,
  CAMERA_FUNC_STOP_FOCUS,
  CAMERA_FUNC_ENABLE_FRAME_CALLBACK,
  CAMERA_FUNC_DISABLE_FRAME_CALLBACK,
  CAMERA_FUNC_RELEASE_FRAME,
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
  CAMERA_FUNC_VIDEO_ENGINE_CB,
  CAMERA_FUNC_AUDIO_READY_CB,
  CAMERA_FUNC_VIDEO_HANDSHAKE,
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE */
  CAMERA_FUNC_BLT,
  CAMERA_FUNC_GET_INFO,
  CAMERA_FUNC_GET_PARM,
  CAMERA_FUNC_SET_REFLECT,
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
  CAMERA_FUNC_SHUTTER_SND,
#endif /* FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND */
#ifdef FEATURE_CAMERA_V7
  CAMERA_FUNC_INIT_RECORD,
  CAMERA_FUNC_HANDLE_ERROR,
  CAMERA_FUNC_STOP_STANDALONE,
  CAMERA_FUNC_MOVE_LENS_DONE,
#endif /* FEATURE_CAMERA_V7 */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

  CAMERA_FUNC_TERMINATE,
  CAMERA_FUNC_MAX1
} camera_func_type;


/* Setting of contrast, exposure, white balance, and effect all affect
 * each other. Once one is touched, the current setting of others are no
 * longer valid.
 */
/* Note:
 * When the sensor is rotated to REVERSE position, the application
 * needs to notify the service layer via the followings:
 *   CAMERA_PARM_PREVIEW_ROTATION  = SP_REVERSE
 *     so the overlay frame can be rotated,
 *   CAMERA_PARM_ENCODE_ROTATION = usually 180 0r 270
 *     so the image orientation is not upside down
 *   CAMERA_PARM_REFLECT = WATER_REFLECT so preview looks fine.
 * When the sensor is rotated to NORMAL position, the application
 * needs to notify the service layer via the followings:
 *   CAMERA_PARM_PREVIEW_ROTATION  = SP_NORMAL
 *     so the overlay frame can be rotated back,
 *   CAMERA_PARM_ENCODE_ROTATION = usually 0 or 90
 *     so the image orientation is upside up
 *   CAMERA_PARM_REFLECT = NO_REFLECT so preview looks fine.
 */
typedef enum
{
  /* read only operation states: camera_state_type */
  CAMERA_PARM_STATE,
  /* read only active command in execution: camera_func_type */
  CAMERA_PARM_ACTIVE_CMD,
  /* zoom */
  CAMERA_PARM_ZOOM,
  /* This affects only when encoding. Please see Note above. */
  CAMERA_PARM_ENCODE_ROTATION, /* 0, 90, 180, 270 degrees */
  /* use camera_sp_type. Please see Note above */
  CAMERA_PARM_PREVIEW_ROTATION,
  /* contrast */
  CAMERA_PARM_CONTRAST,
  /* brightness */
  CAMERA_PARM_BRIGHTNESS,
  /* sharpness */
  CAMERA_PARM_SHARPNESS,
  CAMERA_PARM_EXPOSURE,        /* use camera_exposure_type */
  CAMERA_PARM_WB,              /* use camera_wb_type */
  /* use camera_effect_type. Please see Note above. */
  CAMERA_PARM_EFFECT,
#ifdef FEATURE_VIDEO_ENCODE
  CAMERA_PARM_AUDIO_FMT,       /* use video_fmt_stream_audio_type */
#endif
  CAMERA_PARM_FPS,             /* frames per second, unsigned integer number */
  CAMERA_PARM_FLASH,           /* Flash control, see camera_flash_type */
  CAMERA_PARM_RED_EYE_REDUCTION, /* boolean */
  CAMERA_PARM_NIGHTSHOT_MODE,  /* Night shot mode, snapshot at reduced FPS */
  /* Use camera_reflect_type. Please see Note above. */
  CAMERA_PARM_REFLECT,
  CAMERA_PARM_PREVIEW_MODE,    /* Use camera_preview_mode_type */
  CAMERA_PARM_ANTIBANDING,     /* Use camera_anti_banding_type */
  CAMERA_PARM_FOCUS_STEP,
#ifdef FEATURE_CAMERA_V7
  /* Name change to CAMERA_PARM_EXPOSURE_METERING, remove this later */
  CAMERA_PARM_AUTO_EXPOSURE_MODE, /* Use camera_auto_exposure_mode_type */
#endif /* FEATURE_CAMERA_V7 */
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
  CAMERA_PARM_SHUTTER_SOUND,
#endif /* FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND */
#ifdef FEATURE_CAMERA_INCALL
  CAMERA_PARM_INCALL,          /* In call and vocoder type */
#endif /* FEATURE_CAMERA_INCALL */
#ifdef FEATURE_VIDEO_ENCODE_FADING
  CAMERA_PARM_FADING,
#endif /* FEATURE_VIDEO_ENCODE_FADING */
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
  CAMERA_PARM_TRANSITION,
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */
#ifdef FEATURE_CAMERA_V7
  /* Use to control the exposure compensation */
  CAMERA_PARM_EXPOSURE_COMPENSATION,
  CAMERA_PARM_PREVIEW_FPS,
  CAMERA_PARM_EXPOSURE_METERING,
  CAMERA_PARM_ISO,
  CAMERA_PARM_APERTURE,
  CAMERA_PARM_SHUTTER_SPEED,
  CAMERA_PARM_DEBLUR,
  CAMERA_PARM_LUMA_ADAPTATION,
  CAMERA_PARM_STROBE_FLASH,
  CAMERA_PARM_AF_MODE,
  CAMERA_PARM_FRAME_TIMESTAMP,
  CAMERA_PARM_FLASH_STATE,     /* Query only for flash readiness */
  CAMERA_PARM_CHROMA_SUPPRESSION,
  CAMERA_PARM_HAND_JITTER_REDUCTION,
  CAMERA_PARM_FPS_LIST,
#endif /* FEATURE_CAMERA_V7 */
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
#ifdef FEATURE_VIDEO_ENCODE
  CAMERA_PARM_SPACE_LIMIT,
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
  CAMERA_PARM_USER_ATOM,
#endif
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /*FEATURE_CAMERA_BURST_MODE*/
  CAMERA_PARM_MAX
} camera_parm_type;

/* Reflection type is used for CAMERA_PARM_REFLECT.
 * It is only used for preview.
 * For sensor that is mounted facing the phone user: use mirror reflection.
 *
 * For sensor that is mounted facing away from the phone user: use no
 * reflection.
 *
 * For sensor that is mounted normally facing away from the phone user, but can
 * be rotated so that the sensor is facing the phone user:
 *
 *   if the normal preview display is not up side down after roation, then use
 *   no reflection when the sensor is facing away from the user and use mirror
 *   reflection when the sensor is facing the user.
 *
 *   if the normal preview display is up side down after roation, then use
 *   no reflection when the sensor is facing away from the user and use water
 *   reflection when the sensor is facing the user.
 */
typedef enum
{
  CAMERA_NO_REFLECT,
  CAMERA_MIRROR_REFLECT,
  CAMERA_WATER_REFLECT,
  CAMERA_MAX_REFLECT
} camera_reflect_type;

/* Flash state, get only parameter */
typedef enum
{
  CAMERA_FLASH_READY,
  CAMERA_FLASH_NOT_READY,
  CAMERA_NO_FLASH,
  CAMERA_MAX_FLASH_STATE
} camera_flash_state_type;

/* Sensor position type, used for CAMERA_PARM_PREVIEW_ROTATION  */
/* By default, position is set to normal.
 * The UI must make sure the proper value is set to the service layer,
 * which relies on this value to adjust overlay orientation. Please
 * see more info where CAMERA_PARM_PREVIEW_ROTATION  is defined. */
typedef enum
{
  CAMERA_SP_NORMAL = 0, /* 0 degree */
  CAMERA_SP_REVERSE, /* 180 degree */
  CAMERA_SP_CLK_TANGENTIAL, /* 90 degree clock wise */
  CAMERA_SP_ACLK_TANGENTIAL /* 90 degree anti clock wise */
} camera_sp_type;

/* Exposure type, used for CAMERA_PARM_EXPOSURE */
typedef enum
{
  CAMERA_EXPOSURE_MIN_MINUS_1,
  CAMERA_EXPOSURE_AUTO = 1,  /* This list must match aeecamera.h */
  CAMERA_EXPOSURE_DAY,
  CAMERA_EXPOSURE_NIGHT,
  CAMERA_EXPOSURE_LANDSCAPE,
  CAMERA_EXPOSURE_STRONG_LIGHT,
  CAMERA_EXPOSURE_SPOTLIGHT,
  CAMERA_EXPOSURE_PORTRAIT,
  CAMERA_EXPOSURE_MOVING,
  CAMERA_EXPOSURE_MAX_PLUS_1
} camera_exposure_type;

/* White balancing type, used for CAMERA_PARM_WHITE_BALANCING */
typedef enum
{
  CAMERA_WB_MIN_MINUS_1,
  CAMERA_WB_AUTO = 1,  /* This list must match aeecamera.h */
  CAMERA_WB_CUSTOM,
  CAMERA_WB_INCANDESCENT,
  CAMERA_WB_FLUORESCENT,
  CAMERA_WB_DAYLIGHT,
  CAMERA_WB_CLOUDY_DAYLIGHT,
  CAMERA_WB_TWILIGHT,
  CAMERA_WB_SHADE,
  CAMERA_WB_MAX_PLUS_1
} camera_wb_type;

/* Effect type, used for CAMERA_PARM_EFFECT */
typedef enum
{
  CAMERA_EFFECT_MIN_MINUS_1,
  CAMERA_EFFECT_OFF = 1,  /* This list must match aeecamera.h */
  CAMERA_EFFECT_MONO,
  CAMERA_EFFECT_NEGATIVE,
  CAMERA_EFFECT_SOLARIZE,
  CAMERA_EFFECT_PASTEL,
  CAMERA_EFFECT_MOSAIC,
  CAMERA_EFFECT_RESIZE,
  CAMERA_EFFECT_SEPIA,
  CAMERA_EFFECT_MAX_PLUS_1
} camera_effect_type;

typedef enum
{
  CAMERA_FLASH_AUTO = 1,
  CAMERA_FLASH_OFF,
  CAMERA_FLASH_LOW,
  CAMERA_FLASH_MEDIUM,
  CAMERA_FLASH_HIGH,
  CAMERA_FLASH_CUSTOM,
  CAMERA_FLASH_MAX
} camera_flash_type;

typedef enum
{
  CAMERA_NIGHTSHOT_MODE_OFF,
  CAMERA_NIGHTSHOT_MODE_ON,
  CAMERA_MAX_NIGHTSHOT_MODE
} camera_nightshot_mode_type;

#ifdef FEATURE_CAMERA_V7
/* Auto focus mode, used for CAMERA_PARM_AF_MODE */
typedef enum
{
  /* _UNCHANGED is a special case. Use negative number to keep
     it out of Qmobicat's selection range. */
  CAMERA_AF_MODE_UNCHANGED = -1,
  CAMERA_AF_MODE_NORMAL    = 0,
  CAMERA_AF_MODE_MACRO,
  CAMERA_AF_MODE_MAX
} camera_af_mode_type;
#endif /* FEATURE_CAMERA_V7 */

#ifdef FEATURE_CAMERA_INCALL
typedef enum
{
  CAMERA_INCALL_NOT_VOICE,
  CAMERA_INCALL_AMR,  /* AMR voice call */
  CAMERA_INCALL_HR,   /* Half rate voice call */
  CAMERA_INCALL_FR,   /* Full rate voice call */
  CAMERA_INCALL_EFR,  /* Extended full rate voice call */
  CAMERA_MAX_INCALL
} camera_incall_type;
#endif /* FEATURE_CAMERA_INCALL */

typedef enum
{
  CAMERA_RSP_CB_SUCCESS,    /* Function is accepted         */
  CAMERA_EXIT_CB_DONE,      /* Function is executed         */
  CAMERA_EXIT_CB_FAILED,    /* Execution failed or rejected */
  CAMERA_EXIT_CB_DSP_IDLE,  /* DSP is in idle state         */
  CAMERA_EXIT_CB_DSP_ABORT, /* Abort due to DSP failure     */
  CAMERA_EXIT_CB_ABORT,     /* Function aborted             */
  CAMERA_EXIT_CB_ERROR,     /* Failed due to resource       */
  CAMERA_EVT_CB_FRAME,      /* Preview or video frame ready */
  CAMERA_EVT_CB_PICTURE,    /* Picture frame ready for multi-shot */
  CAMERA_STATUS_CB,         /* Status updated               */
  CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED, /* Specified file size not achieved,
                                        encoded file written & returned anyway */
  CAMERA_CB_MAX
} camera_cb_type;

typedef enum
{
  CAMERA_ERROR_NO_MEMORY,
  CAMERA_ERROR_EFS_FAIL,                /* Low-level operation failed */
  CAMERA_ERROR_EFS_FILE_OPEN,           /* File already opened */
  CAMERA_ERROR_EFS_FILE_NOT_OPEN,       /* File not opened */
  CAMERA_ERROR_EFS_FILE_ALREADY_EXISTS, /* File already exists */
  CAMERA_ERROR_EFS_NONEXISTENT_DIR,     /* User directory doesn't exist */
  CAMERA_ERROR_EFS_NONEXISTENT_FILE,    /* User directory doesn't exist */
  CAMERA_ERROR_EFS_BAD_FILE_NAME,       /* Client specified invalid file/directory name*/
  CAMERA_ERROR_EFS_BAD_FILE_HANDLE,     /* Client specified invalid file/directory name*/
  CAMERA_ERROR_EFS_SPACE_EXHAUSTED,     /* Out of file system space */
  CAMERA_ERROR_EFS_OPEN_TABLE_FULL,     /* Out of open-file table slots                */
  CAMERA_ERROR_EFS_OTHER_ERROR,         /* Other error                                 */
  CAMERA_ERROR_CONFIG,
  CAMERA_ERROR_EXIF_ENCODE,
  CAMERA_ERROR_VIDEO_ENGINE,
  CAMERA_ERROR_IPL,
  CAMERA_ERROR_INVALID_FORMAT,
  CAMERA_ERROR_PNG_ABORT,
  CAMERA_ERROR_MAX
} camera_error_type;


typedef enum
{
  CAMERA_SUCCESS = 0,
  CAMERA_INVALID_STATE,
  CAMERA_INVALID_PARM,
  CAMERA_INVALID_FORMAT,
  CAMERA_NO_SENSOR,
  CAMERA_NO_MEMORY,
  CAMERA_NOT_SUPPORTED,
  CAMERA_FAILED,
  CAMERA_INVALID_STAND_ALONE_FORMAT,
  CAMERA_MALLOC_FAILED_STAND_ALONE,
  CAMERA_RET_CODE_MAX
} camera_ret_code_type;

typedef enum
{
  CAMERA_RAW,
  CAMERA_JPEG,
  CAMERA_PNG,
  CAMERA_YCBCR_ENCODE,
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
  CAMERA_ENCODE_TYPE_MAX
} camera_encode_type;

#if !defined FEATURE_CAMERA_ENCODE_PROPERTIES && defined FEATURE_CAMERA_V7
typedef enum
{
  CAMERA_SNAPSHOT,
  CAMERA_RAW_SNAPSHOT
} camera_snapshot_type;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES && FEATURE_CAMERA_V7 */

typedef enum
{
#ifdef SIRIUS_CAMERA
#error code not present
#endif /* SIRIUS_CAMERA */
  /* YCbCr, each pixel is two bytes. Two pixels form a unit.
   * MSB is Y, LSB is CB for the first pixel and CR for the second pixel. */
  CAMERA_YCBCR,
#ifdef FEATURE_CAMERA_V7
  /* CAMERA_YCBCR_* is used when capturing video or raw from YCbCr sensor */
  CAMERA_YCBCR_4_2_0,
  CAMERA_YCBCR_4_2_2,
  /* CAMERA_HxVy is used when capturing still image for encoding */
  CAMERA_H1V1,
  CAMERA_H2V1,
  CAMERA_H2V2,
  CAMERA_BAYER_8BIT,
  CAMERA_BAYER_10BIT,
#endif /* FEATURE_CAMERA_V7 */
  /* RGB565, each pixel is two bytes.
   * MS 5-bit is red, the next 6-bit is green. LS 5-bit is blue. */
  CAMERA_RGB565,
  /* RGB666, each pixel is four bytes.
   * MS 14 bits are zeros, the next 6-bit is red, then 6-bit of green.
   * LS 5-bit is blue. */
  CAMERA_RGB666,
  /* RGB444, each pixel is 2 bytes. The MS 4 bits are zeros, the next
   * 4 bits are red, the next 4 bits are green. The LS 4 bits are blue. */
  CAMERA_RGB444,
  /* Bayer, each pixel is 1 bytes. 2x2 pixels form a unit.
   * First line: first byte is blue, second byte is green.
   * Second line: first byte is green, second byte is red. */
  CAMERA_BAYER_BGGR,
  /* Bayer, each pixel is 1 bytes. 2x2 pixels form a unit.
   * First line: first byte is green, second byte is blue.
   * Second line: first byte is red, second byte is green. */
  CAMERA_BAYER_GBRG,
  /* Bayer, each pixel is 1 bytes. 2x2 pixels form a unit.
   * First line: first byte is green, second byte is red.
   * Second line: first byte is blue, second byte is green. */
  CAMERA_BAYER_GRBG,
  /* Bayer, each pixel is 1 bytes. 2x2 pixels form a unit.
   * First line: first byte is red, second byte is green.
   * Second line: first byte is green, second byte is blue. */
  CAMERA_BAYER_RGGB,
  /* RGB888, each pixel is 3 bytes. R is 8 bits, G is 8 bits,
   * B is 8 bits*/
  CAMERA_RGB888
} camera_format_type;

typedef enum
{
  CAMERA_PREVIEW_MODE_SNAPSHOT,
  CAMERA_PREVIEW_MODE_MOVIE,
  CAMERA_MAX_PREVIEW_MODE
} camera_preview_mode_type;

typedef enum
{
  CAMERA_ANTIBANDING_OFF,
  CAMERA_ANTIBANDING_60HZ,
  CAMERA_ANTIBANDING_50HZ,
  CAMERA_MAX_ANTIBANDING,
} camera_antibanding_type;

typedef enum
{
  CAMERA_ORIENTATION_LANDSCAPE,
  CAMERA_ORIENTATION_PORTRAIT
} camera_orientation_type;

typedef enum
{
  CAMERA_DESCRIPTION_STRING,
  CAMERA_USER_COMMENT_STRING,
  CAMERA_GPS_AREA_INFORMATION_STRING
} camera_string_type;


typedef struct
{
  uint32 timestamp;  /* seconds since 1/6/1980          */
  int32  latitude;   /* 180/2^25 degrees, WGS ellipsoid */
  int32  longitude;  /* 360/2^26 degrees                */
  int16  altitude;   /* meters                          */
} camera_position_type;

typedef struct
{
  /* Format of the frame */
  camera_format_type format;
  /* For pre-V7, Width and height of the picture.
   * For V7:
   *   Snapshot:     thumbnail dimension
   *   Raw Snapshot: not applicable
   *   Preview:      not applicable
   */
  uint16 dx;
  uint16 dy;
  /* For pre_V7: For BAYER format, RAW data before scaling.
   * For V7:
   *   Snapshot:     Main image dimension
   *   Raw snapshot: raw image dimension
   *   Preview:      preview image dimension
   */
  uint16 captured_dx;
  uint16 captured_dy;
  /* it indicates the degree of clockwise rotation that should be
   * applied to obtain the exact view of the captured image. */
  uint16 rotation;
#ifdef FEATURE_CAMERA_V7
  /* Preview:      not applicable
   * Raw shapshot: not applicable
   * Snapshot:     thumbnail image buffer
   */
  uint8 *thumbnail_image;
#endif /* FEATURE_CAMERA_V7 */
  /* For pre-V7:
   *   Image buffer ptr
   * For V7:
   *   Preview: preview image buffer ptr
   *   Raw snapshot: Raw image buffer ptr
   *   Shapshot:     Main image buffer ptr
   */
  byte * buffer;
} camera_frame_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_CAMERA_V7
/*~ FIELD camera_frame_type.thumbnail_image POINTER */
#endif /* FEATURE_CAMERA_V7 */
/*~ FIELD camera_frame_type.buffer POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef enum {
  TIFF_DATA_BYTE = 1,
  TIFF_DATA_ASCII = 2,
  TIFF_DATA_SHORT = 3,
  TIFF_DATA_LONG = 4,
  TIFF_DATA_RATIONAL = 5,
  TIFF_DATA_UNDEFINED = 7,
  TIFF_DATA_SLONG = 9,
  TIFF_DATA_SRATIONAL = 10
} tiff_data_type;

typedef enum {
  ZERO_IFD = 0, /* This must match AEECamera.h */
  FIRST_IFD,
  EXIF_IFD,
  GPS_IFD,
  INTEROP_IFD,
  DEFAULT_IFD
} camera_ifd_type;

typedef struct
{
  uint16 tag_id;
  camera_ifd_type ifd;
  tiff_data_type type;
  uint16 count;
  void * value;
} camera_exif_tag_type;

typedef enum
{
  CAMERA_ID_0 = 0,
  CAMERA_ID_1,
  CAMERA_ID_MAX
} camera_sensor_id_type;

typedef struct
{
  /* Did this sensor respond as expected during initialization? If not, this
   * sensor is made unavailable for use. */
  boolean sensor_available;
  /* If there are multiple sensors, is this the one that has been allowed
   * control of the bus? */
  boolean sensor_selected;
  /* If there are multiple sensors, is this the currently active one? */
  boolean sensor_enabled;
  /* What is the ID for this sensor */
  uint16 sensor_id;
  /* Sensor model number, null terminated, truncate to 31 characters */
  char sensor_model[32];
  /* Maximum width and height supported by the sensor */
  uint16 sensor_width;
  uint16 sensor_height;
  /* Maximum frames per second supported by this sensor */
  uint16 fps;
  /* Whether the device driver can sense when sensor is rotated */
  boolean sensor_rotation_sensing;
  /* How the sensor are installed */
  uint16 default_rotation;
  camera_orientation_type default_orientation;
} camera_info_type;

typedef struct {
  int32                  quality;
  camera_encode_type     format;
  int32                  file_size;
} camera_encode_properties_type;

typedef enum
{
  CAMERA_DEVICE_MEM,
  CAMERA_DEVICE_EFS,
  CAMERA_DEVICE_MAX
} camera_device_type;

typedef struct {
  camera_device_type     device;
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
  int32                  quality;
  camera_encode_type     format;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */
  uint8                 *buffer;
  uint32                 length;
} camera_handle_mem_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD camera_handle_mem_type.buffer 
    VARRAY LENGTH camera_handle_mem_type.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef struct
{
  camera_device_type     device;
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
  int32                  quality;
  camera_encode_type     format;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */
  char                   filename[FS_FILENAME_MAX_LENGTH_P];
} camera_handle_efs_type;

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD camera_handle_efs_type.filename STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_VIDEO_ENCODE_FADING
typedef enum
{
  CAMERA_PARM_FADE_OFF,
  CAMERA_PARM_FADE_IN,
  CAMERA_PARM_FADE_OUT,
  CAMERA_PARM_FADE_IN_OUT,
  CAMERA_PARM_FADE_MAX
} camera_fading_type;
#endif /* FEATURE_VIDEO_ENCODE_FADING */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
typedef enum
{
  CAMERA_PARM_TRANSITION_ALPHA,
  CAMERA_PARM_TRANSITION_CURTAIN,
  CAMERA_PARM_TRANSITION_OFF
} camera_transition_type;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

typedef union
{
  camera_device_type      device;
  camera_handle_mem_type  mem;
  camera_handle_efs_type  efs;
} camera_handle_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE union camera_handle_type DISC camera_handle_type.device */
/*~ DEFAULT camera_handle_type.device */
/*~ CASE CAMERA_DEVICE_MEM camera_handle_type.mem */
/*~ CASE CAMERA_DEVICE_EFS camera_handle_type.efs */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

typedef struct
{
  int32 minimum_value; /* Minimum allowed value */
  int32 maximum_value; /* Maximum allowed value */
  int32 step_value;    /* step value */
  int32 default_value; /* Default value */
  int32 current_value; /* Current value */
} camera_parm_info_type;

typedef void (*camera_cb_f_type)
  (camera_cb_type cb,
   const void *client_data,
   camera_func_type func,
   int32 parm4
  );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK camera_cb_f_type POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

typedef enum
{
  CAMERA_AUTO_FOCUS,
  CAMERA_MANUAL_FOCUS
} camera_focus_e_type;

#ifdef FEATURE_CAMERA_V7
/* AEC: Frame average weights the whole preview window equally
   AEC: Center Weighted weights the middle X percent of the window
   X percent compared to the rest of the frame
   AEC: Spot metering weights the very center regions 100% and
   discounts other areas                                        */
typedef enum
{
  CAMERA_AEC_FRAME_AVERAGE,
  CAMERA_AEC_CENTER_WEIGHTED,
  CAMERA_AEC_SPOT_METERING,
  CAMERA_AEC_MAX_MODES
} camera_auto_exposure_mode_type;

/* VFE Focus Region:
 *  VFE input image dimension
 * VFE Focus Window:
 *  A rectangle in the VFE Focus Region. VFE Focus Window
 *  area must not exceed one quarter of the area of VFE Focus
 *  Region.
 * Display Focus Region:
 *  Diplay dimensions (not LCD dimensions)
 * Display Focus Window:
 *  A rectangle in Display Focus Region
 *  Focus Window Freedom: Movement of Focus Window in x and y direction.
 */

typedef struct
{
  /* Focus Window dimensions, could be negative. */
  int16 x;
  int16 y;
  int16 dx;
  int16 dy;

  /* Focus Window Freedom granularity in x-direction */
  int16 dx_step_size;

  /* Focus Window Freedom granularity in y-direction */
  int16 dy_step_size;

  /*  Focus Window can only move within this Focus Region and
   *  the maximum Focus Window area must not exceed one quarter of the
   *  Focus Region.
   */
  int16   min_x;
  int16   min_y;
  int16   max_x;
  int16   max_y;
} camera_focus_window_type;

typedef unsigned int Offline_Input_PixelSizeType;
typedef uint16 Offline_Snapshot_PixelSizeType;
typedef uint16 Offline_Thumbnail_PixelSizeType;
typedef uint16 Offline_NumFragments_For_Input;
typedef uint16 Offline_NumFragments_For_Output;

typedef enum Offline_InputFormatType
{
  CAMERA_BAYER_G_B,
  CAMERA_BAYER_B_G,
  CAMERA_BAYER_G_R,
  CAMERA_BAYER_R_G,
  CAMERA_YCbCr_Y_Cb_Y_Cr,
  CAMERA_YCbCr_Y_Cr_Y_Cb,
  CAMERA_YCbCr_Cb_Y_Cr_Y,
  CAMERA_YCbCr_Cr_Y_Cb_Y,
  CAMERA_YCbCr_4_2_2_linepacked,
  CAMERA_YCbCr_4_2_0_linepacked,
  CAMERA_NumberOf_InputFormatType   /* Used for count purposes only */
} Offline_InputFormatType;

typedef enum Offline_YCbCr_InputCositingType
{
  CAMERA_CHROMA_NOT_COSITED,
  CAMERA_CHROMA_COSITED,
  CAMERA_NumberOf_YCbCr_InputCositingType   /* Used for count purposes only */
} Offline_YCbCr_InputCositingType;

typedef enum Offline_Input_PixelDataWidthType
{
  CAMERA_8Bit,
  CAMERA_10Bit,
  CAMERA_NumberOf_PixelDataWidthType /* Used for count purposes only */
} Offline_Input_PixelDataWidthType;

#ifdef FEATURE_VIDEO_MULTI_UUID

#define VIDEO_MAX_UUID       20
#define VIDEO_UUID_ATOM_SIZE 16

typedef  struct {
  uint32 nUuid; // total number of sets of uuid
  uint8 *pAtom[VIDEO_MAX_UUID];
  uint8 *pData[VIDEO_MAX_UUID];
  uint32 dataSize[VIDEO_MAX_UUID];
} video_uuid_info_type;

#endif /* FEATURE_VIDEO_MULTI_UUID */

typedef struct OfflineInputConfigurationType
{
  Offline_YCbCr_InputCositingType  YCbCrCositing    ;
  Offline_InputFormatType          format           ;
  Offline_Input_PixelDataWidthType dataWidth        ;
  Offline_Input_PixelSizeType      height           ;
  Offline_Input_PixelSizeType      width            ;
  Offline_Thumbnail_PixelSizeType  thumbnail_width  ;
  Offline_Thumbnail_PixelSizeType  thumbnail_height ;
  Offline_Snapshot_PixelSizeType   snapshot_width   ;
  Offline_Snapshot_PixelSizeType   snapshot_height  ;
  char*                            file_name        ;
  Offline_NumFragments_For_Input   input_fragments  ;
  Offline_NumFragments_For_Output  output_fragments ;
} OfflineInputConfigurationType;

/* Enum Type for different ISO Mode supported */
typedef enum
{
  CAMERA_ISO_AUTO = 0,
  CAMERA_ISO_DEBLUR,
  CAMERA_ISO_100,
  CAMERA_ISO_200,
  CAMERA_ISO_400,
  CAMERA_ISO_800,
  CAMERA_ISO_MAX
} camera_iso_mode_type;
#endif /* FEATURE_CAMERA_V7 */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
/* Transition effects
**   While processing a pause, if transition effect is on,
**   then the last frame from the pause will transition to the new image\
**   upon resume.
**   E.g. if the transition is alpha fade, then the new clip will start
**   with a still image of the last frame and fade into the clip.
**
** Alpha Fade:
**   Last frame of old image will slowly fade out as new image fades
**   in.
**
** Curtain Fade:
**   New image will open in a curtain from the old
*/

typedef enum {
  VIDENCTRANSITION_NONE,
  VIDENCTRANSITION_ALPHA_FADE,
  VIDENCTRANSITION_CURTAIN_FADE,
  VIDENCTRANSITION_MAX
} VidEncTransitionType;

/* define a 5-second (5000 msec) transition period */
#define VIDENC_TRANSITION_DURATION 2000

#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /*FEATURE_CAMERA_BURST_MODE*/

/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

/*============================================================================
*                         EXTERNAL FUNCTIONS
============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_INIT

DESCRIPTION
              Used to initialize the device at power up. To be called
              from startup routine such as mc_device_init().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_init(void);

#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
/*===========================================================================

FUNCTION      CAMERA_VIDEO_HANDSHAKE

DESCRIPTION
              Used to handle handshake from the video encoder. It will notify
              camera services when a frame is encoded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_video_handshake(uint32 code);
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE*/


/*===========================================================================

FUNCTION      CAMERA_START

DESCRIPTION
              Get the camera services started.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_start
(
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_start */ 
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

/*===========================================================================

FUNCTION      CAMERA_STOP

DESCRIPTION
              The camera service is terminated and the sensor enters power
              down or saving mode.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_stop
(
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_stop */
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

#ifndef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_START_STAND_ALONE

DESCRIPTION
  Perform init on camera services but only for image encoding, not for
  actual picture taking.

DEPENDENCIES
  Must be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_start_stand_alone
(
  camera_cb_f_type callback,
  void *client_data
);
#endif /* nFEATURE_CAMERA_V7 */


#ifndef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_STOP_STAND_ALONE

DESCRIPTION
  Perform mem-deallocation on camera services but only for image encoding, not for
  actual picture taking.  Sensor is not involved here.

  Can be executed in all but INIT state.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_stop_stand_alone
(
  camera_cb_f_type callback,
  void *client_data
);
#endif /* nFEATURE_CAMERA_V7 */

/*===========================================================================

FUNCTION      CAMERA_SET_DIMENSIONS

DESCRIPTION
              Set picture and display sizes.

              This function is synchronous and is executed in the caller's
              context.

DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_dimensions
(
  uint16 picture_width,  /* width of the picture */
  uint16 picture_height, /* height of the picture */
  uint16 display_width,  /* width of display */
#ifdef FEATURE_CAMERA_V7
  uint16 display_height, /*   as compared to the display dimension */
#endif
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_set_dimensions */ 
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
/*===========================================================================

FUNCTION      CAMERA_SET_ENCODE_PROPERTIES

DESCRIPTION
              Set encode properties.

              If encode properties had changed, this function must be called
              before camera_take_picture is called.

              This function is synchronous and is executed in the caller's
              context.

DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_encode_properties
(
  camera_encode_properties_type *encode_properties
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_set_encode_properties */ 
/*~ PARAM IN encode_properties POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */


/*===========================================================================

FUNCTION      CAMERA_START_PREVIEW

DESCRIPTION
              Enters preview state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EVT_CB_FRAME when VFE frame is available.
   3. CAMERA_EXIT_CB_DONE if preview is terminated.
   4. CAMERA_EXIT_CB_ERROR if there is a resource error.
   5. CAMERA_EXIT_CB_ABORT.
   6. CAMERA_EXIT_CB_DSP_IDLE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_start_preview
(
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_start_preview */ 
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_STOP_PREVIEW

DESCRIPTION
              Exit preview state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PREVIEW state.
  Use start_preview callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE if preview is terminated.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_stop_preview(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_stop_preview */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_TAKE_PICTURE

DESCRIPTION
              The next frame is taken for encoding. Raw data is returned
              in CAMERA_EXIT_CB_DONE camera_frame_type.

              The picture buffer is allocated by the camera service, and must
              be released by the application via either camera_encode_picture
              or camera_release_picture.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is taken
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_take_picture
(
  camera_cb_f_type callback,
  void * client_data
#if !defined FEATURE_CAMERA_ENCODE_PROPERTIES && defined FEATURE_CAMERA_V7
  ,camera_snapshot_type camera_snapshot_mode
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES && FEATURE_CAMERA_V7 */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_take_picture */ 
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 



/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE

DESCRIPTION
              Encode the application picture.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is encoded
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_encode_picture
(
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data
);


#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE_SIZE_LIMIT

DESCRIPTION
              Encode the application picture.  For JPEG, encode to
              be not greater than file size specified (in KBytes)

INPUT
  int32 file_size  - maximum size of resulting encoded image in KBytes.
    So specifying 100 here means target a file no larger than 102400 Bytes.
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is encoded
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_encode_picture_size_limit
(
  int32 file_size,
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data
);
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */

#ifndef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE_STAND_ALONE

DESCRIPTION
  Encode image passed in as a memory pointer.  Do this without
  the VFE or camera layer active.  This will be for stand-alone
  image encoding.
  Support the following: EFS or memory output, RGB modes, YCbCr mode.
  Bayer modes cannot be supported because of camera sensor related input
  that is not present such as digital/analog gains, and white balance etc

  Supported:
  JPEG only
  YCbCr, RGB565, RGB444, RGB666

DEPENDENCIES

INPUT
  int32 file_size  - maximum size of resulting encoded image in KBytes.
    So specifying 100 here means target a file no larger than 102400 Bytes.
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data


RETURN VALUE
  camera_ret_code_type: return from this function
   1. CAMERA_SUCCESS - return success
   2. CAMERA_INVALID_STAND_ALONE_FORMAT - format invalid for this stand-alone
      encoder

  If successful, following are passed when callback is executed:
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is encoded
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_encode_picture_stand_alone
(
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
  int32 file_size,
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */
  camera_frame_type *frame,
  camera_handle_type *handle,
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
  camera_encode_properties_type *encode_properties,
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */
  camera_cb_f_type callback,
  void * client_data
);
#endif /* nFEATURE_CAMERA_V7 */

#ifndef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_COLOR_CONVERT

DESCRIPTION
              Color convert the application picture.

              The input image is color converted to the application
              buffer. Currently only support converting CAMERA_BAYER_BGGR
              to CAMERA_YCBCR.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_color_convert
(
  camera_frame_type *frame, /* Input frame */
  camera_format_type out_format, /* Output format */
  uint32 length, /* Size of the output buffer in bytes */
  byte *out_buffer, /* Output buffer */
  camera_cb_f_type callback,
  void * client_data
);
#endif /* nFEATURE_CAMERA_V7 */


/*===========================================================================

FUNCTION      CAMERA_RELEASE_PICTURE

DESCRIPTION
              The picture buffer in camera_take_picture is released.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_release_picture (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_release_picture */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


#ifdef FEATURE_VIDEO_ENCODE
/*===========================================================================

FUNCTION      CAMERA_START_RECORD

DESCRIPTION
              Video recording is started.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EVT_CB_FRAME when VFE frame is available.
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_ABORT.
   5. CAMERA_EXIT_CB_DSP_IDLE
   6. CAMERA_EXIT_CB_DONE if recording is terminated.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_start_record
(
  video_fmt_type               file_format,      /* i.e. MP4, AVI, ...   */
  video_fmt_stream_video_type  video_format,     /* i.e. MPEG-4, ...     */
  video_fmt_brand_type         video_brand,
  boolean                      short_header,
  video_fmt_stream_audio_type  audio_format,     /* i.e. EVRC, AMR, ...  */
#ifdef FEATURE_CAMERA_V770
  boolean                      bAudioPostCard,
#endif
#ifdef FEATURE_VIDEO_MULTI_UUID
  video_uuid_info_type        *uuid_info,
#else /* FEATURE_VIDEO_MULTI_UUID */
  void *                       uuid_data,
  uint32                       uuid_data_size,
  uint8 *                      uuid_atom,
#endif /* FEATURE_VIDEO_MULTI_UUID */
  video_profile_type           profile,          /* video profile        */
#ifdef FEATURE_CAMERA_V7
  int32                        user_bitrate,     /* bitrate specified by usr */
#endif
  video_handle_type           *destination,      /* where output goes    */
  camera_cb_f_type             callback,
  void                        *client_data
);
#endif /* FEATURE_VIDEO_ENCODE */


#ifdef FEATURE_VIDEO_ENCODE
/*===========================================================================

FUNCTION      CAMERA_PAUSE_RECORD

DESCRIPTION
              Video recording is paused.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in RECORDING state
  Use start_record callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_pause_record (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  camera_pause_record */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 
#endif /* FEATURE_VIDEO_ENCODE */


#ifdef FEATURE_VIDEO_ENCODE
/*===========================================================================

FUNCTION      CAMERA_RESUME_RECORD

DESCRIPTION
              Video recording is resumed.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in RECORDING state.
  Use start_record callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_resume_record (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_resume_record */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 
#endif /* FEATURE_VIDEO_ENCODE */


#ifdef FEATURE_VIDEO_ENCODE
/*===========================================================================

FUNCTION      CAMERA_STOP_RECORD

DESCRIPTION
              Video recording is stopped.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in record state.
  Use start_record callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is taken
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_stop_record (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_stop_record */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 
#endif /* FEATURE_VIDEO_ENCODE */


/*===========================================================================

FUNCTION      CAMERA_ENABLE_FRAME_CALLBACK

DESCRIPTION
              Enable frame callback.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview, start_record, or start_encode callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_enable_frame_callback (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_enable_frame_callback */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_DISABLE_FRAME_CALLBACK

DESCRIPTION
              Disable frame callback.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview, start_record, or start_encode callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_disable_frame_callback (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_disable_frame_callback */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_RELEASE_FRAME

DESCRIPTION
              Release the current frame so buffer could be reused.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview, start_record, or start_encode callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_release_frame (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_release_frame */ 
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

/*===========================================================================

FUNCTION      CAMERA_SET_OVERLAY

DESCRIPTION
              Set the overlay frame.  Overlay frame is copied. The overlay
              frame is in RGB565 and has the size of dx * dy * 2 bytes.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
    CAMERA_EXIT_CB_DONE when set is executed
    CAMERA_EXIT_CB_FAILED if set execution failed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_overlay
(
  uint16 dx,
  uint16 dy,
  uint32 transparent_color,
  void * overlay_bmp,
  camera_cb_f_type callback,
  void *client_data
);


/*===========================================================================

FUNCTION      CAMERA_CLR_OVERLAY

DESCRIPTION
              Clear the overlay frame.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
    CAMERA_EXIT_CB_DONE when set is executed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_clr_overlay
(
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_clr_overlay */
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_SET_ICON_ARRAY

DESCRIPTION
              Set the ICON array. ICON array is copied. The ICON array is a
              array of ICONS in RGB565. dx and dy provides the dimension of a
              frame where the ICONs are located.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
    CAMERA_EXIT_CB_DONE when set is executed
    CAMERA_EXIT_CB_FAILED if set execution failed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_icon_array
(
  /* Display size */
  uint32 dx,
  uint32 dy,
  /* Transparent color, must be the same for all icons and overlay */
  uint32 transparent_color,
  ipl_icon_type ** icon_array
);


/*===========================================================================

FUNCTION      CAMERA_CLR_ICON_ARRAY

DESCRIPTION
              Clear the ICON array.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
    CAMERA_EXIT_CB_DONE when set is executed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_clr_icon_array(void);


/*===========================================================================

FUNCTION      CAMERA_SET_POSITION

DESCRIPTION
              Set position.

              The position is retained for only the next snapshot, then it
              is cleared.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_position
(
  camera_position_type *position,
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_set_position */ 
/*~ PARAM IN position POINTER */
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_SET_EXIF_TAG

DESCRIPTION
              Set EXIF tag.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_exif_tag
(
  camera_exif_tag_type * tag,
  camera_cb_f_type callback,
  void *client_data
);



/*===========================================================================

FUNCTION      CAMERA_BLT

DESCRIPTION
              Color conversion, rotation, or adding frame and block transfer
              the result into destination buffer. Output color is RGB565.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_blt
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16 dst_dx,
  uint16 dst_dy,
  void * dst_ptr,
  /* Rectangle of the destination used for update */
  uint16 x,
  uint16 y,
  uint16 dx,
  uint16 dy
);


/*===========================================================================

FUNCTION      CAMERA_BLT_EXT

DESCRIPTION
              Color conversion, rotation, or adding frame and block transfer
              the result into destination buffer. Output color is as specified.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_blt_ext
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16 dst_dx,
  uint16 dst_dy,
  void * dst_ptr,
  /* Rectangle of the destination used for update */
  uint16 x,
  uint16 y,
  uint16 dx,
  uint16 dy,
  camera_format_type format
);


/*===========================================================================

FUNCTION      CAMERA_SET_PARM

DESCRIPTION
              Set various control on the camera sensor.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_parm
(
  camera_parm_type id,
  int32 parm,
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_set_parm */ 
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


#ifdef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_SET_PARM_2

DESCRIPTION
              Set various control on the camera sensor.
              Set two control parameters with this function.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_parm_2
(
  camera_parm_type id,
  int32 parm1,
  int32 parm2,
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_set_parm_2 */ 
/*~ PARAM IN client_data */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

/*===========================================================================

FUNCTION      CAMERA_GET_PARM_2

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.
   Get two control parameters with this function.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_get_parm_2
(
  camera_parm_type  id,
  int32            *parm1_ptr,
  int32            *parm2_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_get_parm_2 */ 
/*~ PARAM OUT parm1_ptr POINTER */
/*~ PARAM OUT parm2_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

#endif /* FEATURE_CAMERA_V7 */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
/*===========================================================================

FUNCTION      CAMERA_GET_SENSORS

DESCRIPTION
              Retrieve number of camera sensors and a pointer to the info
              table.

              Synchronous function executed in the callers context.

DEPENDENCIES
  None

RETURN VALUE
  Number of camera sensors

SIDE EFFECTS
  None

===========================================================================*/
uint16 camera_get_sensors(camera_info_type **sensor_info);
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
/*===========================================================================

FUNCTION      CAMERA_SELECT_SENSOR

DESCRIPTION
              Select the specified sensor

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_select_sensor(uint16 sensor_id);
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

/*===========================================================================

FUNCTION      CAMERA_GET_INFO

DESCRIPTION
              Retrieve sensor information.

              Synchronous function executed in the callers context.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
void camera_get_info(camera_info_type *info);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_get_info */ 
/*~ PARAM INOUT info POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

/*===========================================================================

FUNCTION      CAMERA_GET_PARM

DESCRIPTION
              Retrieve control values.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_get_parm
(
  camera_parm_type id,
  camera_parm_info_type *parm
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_get_parm */ 
/*~ PARAM OUT parm POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

/*============================================================================
*                         EXPORTED FUNCTIONS
*             These functions are used below the BREW layer
============================================================================*/

/*===========================================================================

FUNCTION      CAMERA_PROCESS_FUNC

DESCRIPTION
              Processes function call in graph task context.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_process_func
(
  uint32 func,
  void *callback,
  void *client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
);


/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_MSG

DESCRIPTION
              Processes QDSP events in graph task context.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_process_qdsp_msg(uint32 msg, void *buffer, uint32 size);

#ifdef FEATURE_QVPHONE
typedef enum
{
  CAMERA_SENSOR_TYPE_NONE,
  CAMERA_SENSOR_TYPE_BAYER,
  CAMERA_SENSOR_TYPE_YCBCR
} camera_sensor_type;


/*===========================================================================

FUNCTION      CAMERA_ENABLE_QVP

DESCRIPTION
              Enable QVP mode. In this mode, camera service does not do DSP
              code download. Camera service also does not support video
              recording and cannot take or encode pictures.


DEPENDENCIES
  Must be in DISABLED, INIT, or READY state.

RETURN VALUE
  camera_ret_code_type
  cb = DSP ISR callback function pointer
  sensor_type = camera sensor type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_enable_qvp
(
 void **cb, /* Output parameter */
 camera_sensor_type *sensor_type, /* Output parameter */
 boolean            use_ycbcr1, /* YCbCr type preference */
 boolean arm_kick_backend   /* Flag indicating if ARM kick backend */
);


/*===========================================================================

FUNCTION      CAMERA_DISABLE_QVP

DESCRIPTION
              Disable QVP mode.


DEPENDENCIES
  Must be in DISABLED, INIT, or READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_disable_qvp(void);


/*===========================================================================

FUNCTION      CAMERA_START_QVP_ENCODE

DESCRIPTION
              QVP encoding is started.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EVT_CB_FRAME callback when a VFE frame is available.
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_ABORT.
   5. CAMERA_EXIT_CB_DSP_IDLE
   6. CAMERA_EXIT_CB_DONE if stopped.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_start_qvp_encode
(
  camera_cb_f_type             callback,
  void                        *client_data
);


/*===========================================================================

FUNCTION      CAMERA_STOP_QVP_ENCODE

DESCRIPTION
              QVP encoding is stopped.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in RECORDING state.
  Use start_encode callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when operation is stopped.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_stop_qvp_encode (void);
#endif /* FEATURE_QVPHONE */


/*===========================================================================

FUNCTION      CAMERA_QVP_RESET

DESCRIPTION
              Notify camera that QVP DSP image is reset.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_qvp_reset(void);

#ifdef FEATURE_MP4_TRANSCODER
/*===========================================================================

FUNCTION      CAMERA_ENABLE_OFFLINE

DESCRIPTION
              Initialize backend for offline encode mode


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_enable_offline(void);
#endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_PNG_ENCODER
/*===========================================================================

FUNCTION      CAMERA_PROCESS_PNGE_MSG

DESCRIPTION
              Process all messages fr PNG encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void camera_process_pnge_msg(uint32 msg);
#endif /* FEATURE_PNG_ENCODER */

/*===========================================================================

FUNCTION      CAMERA_GET_THUMBNAIL_PROPERTIES

DESCRIPTION
              Get thumbnail size and quality

              This function is executed in the caller's context.

DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_get_thumbnail_properties
(
  uint32 *width,   /* width of the thumbnail */
  uint32 *height,  /* height of the thumbnail */
  uint32 *quality  /* encode quality of the thumbnail */
  /* When encoding, picture of this size and up must have thumbnail */
  //uint32 *picture_size
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_get_thumbnail_properties */
/*~ PARAM OUT width POINTER */
/*~ PARAM OUT height POINTER */
/*~ PARAM OUT quality POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_SET_THUMBNAIL_PROPERTIES

DESCRIPTION
              Set thumbnail size and quality

              This function is executed in the caller's context.

DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_thumbnail_properties
(
  uint32 width,   /* width of the thumbnail */
  uint32 height,  /* height of the thumbnail */
  uint32 quality  /* encode quality of the thumbnail */
  /* When encoding, picture of this size and up must have thumbnail */
  //uint32 picture_size
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_set_thumbnail_properties */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

/*===========================================================================

FUNCTION      CAMERA_START_FOCUS

DESCRIPTION
              Start the focus operation.

              For auto mode, once the best focal is achieved, EXIT_CB_DONE
              callback is executed and FOCUS_STEP parameter is updated.

              For manual mode, once the specified value in FOCUS_STEP parameter
              is achieved, EXIT_CB_DONE callback is executed.

              For continuous mode, if supported, EXIT_CB_DONE callback is
              never executed but the FOCUS_STEP parameter is updated
              continuously until camera_stop_focus() is called.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when operation is achieved or stopped.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_start_focus
(
  camera_focus_e_type focus,
  camera_cb_f_type callback,
  void *client_data
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_start_focus */
/*~ PARAM IN client_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 

/*===========================================================================

FUNCTION      CAMERA_STOP_FOCUS

DESCRIPTION
              Stop the continuous focus operation.
              callback and client_data in camera_start_focus is used.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  When continuous focus is stopped, EXIT_CB_DONE callback is executed.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_stop_focus(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_stop_focus */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


#ifdef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_GET_FOCUS_WINDOW

DESCRIPTION
              Gets the focus window. Executes under caller's context

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_get_focus_window (camera_focus_window_type *camera_display_focus_window);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_get_focus_window */
/*~ PARAM OUT camera_display_focus_window POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


/*===========================================================================

FUNCTION      CAMERA_SET_FOCUS_WINDOW

DESCRIPTION
              Sets the focus window. Executes under caller's context

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_focus_window (camera_focus_window_type *camera_display_focus_window);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION camera_set_focus_window */
/*~ PARAM IN camera_display_focus_window POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */ 


#endif /* FEATURE_CAMERA_V7 */


#ifdef FEATURE_PNG_ENCODER
/*===========================================================================

FUNCTION      CAMERA_PROCESS_PNGE_MSG

DESCRIPTION
              Process all messages fr PNG encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void camera_process_pnge_msg(uint32 msg);
#endif /* FEATURE_PNG_ENCODER */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) && defined(FEATURE_MDP)
/*===========================================================================

FUNCTION      CAMERA_SET_LCD_DIRECT_MODE

DESCRIPTION
              Set the LCD direct mode operational parameters.
              Executed synchronously.

DEPENDENCIES
  Must be in ready state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  When direct mode is enabled, camera_blt_ext() cannot be called.
  When direct mode is disabled, camera_blt_direct() cannot be called.

===========================================================================*/
camera_ret_code_type camera_set_lcd_direct_mode
(
 /* mode = 0, direct mode is disabled.
  * mode = 0x10, primary LCD layer 0 (lowest layer)
  * mode = 0x1x, primary LCD layer x
  * mode = 0x20, secondary LCD layer 0 (lowest layer)
  * mode = 0x2x, secondary LCD layer x */
 int32 mode,
 int32 dst_x,  /* destination window x offset */
 int32 dst_y,  /* destination window y offset */
 int32 dst_dx, /* destination window width */
 int32 dst_dy  /* destination window width */
);

/*===========================================================================

FUNCTION      CAMERA_BLT_DIRECT

DESCRIPTION
              Send the video to the display directly.

DEPENDENCIES
  NONE

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  NONE

===========================================================================*/
camera_ret_code_type camera_blt_direct(camera_frame_type *frame);
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE && nFEATURE_MDP */

#endif /* FEATURE_CAMERA */
#endif /* CAMERA_H */

